// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Frame.h"
#include "array/Array.h"
// std
#include <chrono>

namespace anari_ospray {

OSPFrameBufferFormat osprayFormatFromANARI(anari::DataType a)
{
  switch (a) {
  case ANARI_FLOAT32_VEC4:
    return OSP_FB_RGBA32F;
  case ANARI_UFIXED8_VEC4:
    return OSP_FB_RGBA8;
  case ANARI_UFIXED8_RGBA_SRGB:
  default:
    return OSP_FB_SRGBA;
  }
}

Frame::Frame(OSPRayGlobalState *s) : helium::BaseFrame(s) {}

Frame::~Frame()
{
  wait();
  ospRelease(m_osprayFrameBuffer);
  ospRelease(m_osprayDenoiser);
}

bool Frame::isValid() const
{
  return m_renderer && m_renderer->isValid() && m_camera && m_camera->isValid()
      && m_world && m_world->isValid() && m_frameData.size.x > 0
      && m_frameData.size.y > 0;
}

OSPRayGlobalState *Frame::deviceState() const
{
  return (OSPRayGlobalState *)helium::BaseObject::m_state;
}

void Frame::commitParameters()
{
  m_renderer = getParamObject<Renderer>("renderer");
  m_camera = getParamObject<Camera>("camera");
  m_world = getParamObject<World>("world");
  m_colorType = getParam<anari::DataType>("channel.color", ANARI_UNKNOWN);
  m_depthType = getParam<anari::DataType>("channel.depth", ANARI_UNKNOWN);
  m_albedoType = getParam<anari::DataType>("channel.albedo", ANARI_UNKNOWN);
  m_normalType = getParam<anari::DataType>("channel.normal", ANARI_UNKNOWN);
  m_primIdType =
      getParam<anari::DataType>("channel.primitiveId", ANARI_UNKNOWN);
  m_objIdType = getParam<anari::DataType>("channel.objectId", ANARI_UNKNOWN);
  m_instIdType = getParam<anari::DataType>("channel.instanceId", ANARI_UNKNOWN);

  if (m_colorType != ANARI_UNKNOWN && m_colorType != ANARI_UFIXED8_VEC4
      && m_colorType != ANARI_UFIXED8_RGBA_SRGB
      && m_colorType != ANARI_FLOAT32_VEC4)
    m_colorType = ANARI_UFIXED8_RGBA_SRGB;
  if (m_depthType != ANARI_FLOAT32)
    m_depthType = ANARI_UNKNOWN;
  if (m_normalType != ANARI_FIXED16_VEC3 && m_normalType != ANARI_FLOAT32_VEC3)
    m_normalType = ANARI_UNKNOWN;
  if (m_albedoType != ANARI_UFIXED8_VEC3
      && m_albedoType != ANARI_UFIXED8_RGB_SRGB
      && m_albedoType != ANARI_FLOAT32_VEC3)
    m_albedoType = ANARI_UNKNOWN;
  if (m_primIdType != ANARI_UINT32)
    m_primIdType = ANARI_UNKNOWN;
  if (m_objIdType != ANARI_UINT32)
    m_objIdType = ANARI_UNKNOWN;
  if (m_instIdType != ANARI_UINT32)
    m_instIdType = ANARI_UNKNOWN;

  m_accumulation = getParam<bool>("accumulation", false);
  m_frameData.size = getParam<uint2>("size", uint2(0u));
}

void Frame::finalize()
{
  if (!m_renderer) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'renderer' on frame");
  }

  if (!m_camera) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "missing required parameter 'camera' on frame");
  }

  if (!m_world) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "missing required parameter 'world' on frame");
  }

  if (m_frameData.size.x == 0 || m_frameData.size.y == 0) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "need positive parameter 'size' on frame");
  }

  if (isValid())
    initFB(m_renderer->denoise());
}

void Frame::initFB(const bool denoising)
{
  if (m_osprayFrameBuffer) {
    wait();
    ospRelease(m_osprayFrameBuffer);
    m_osprayFrameBuffer = nullptr;
  }

  m_denoising = denoising;
  uint32_t flags = OSP_FB_COLOR;
  if (m_depthType != ANARI_UNKNOWN)
    flags |= OSP_FB_DEPTH;
  if (m_accumulation)
    flags |= OSP_FB_ACCUM;
  if (denoising || m_albedoType != ANARI_UNKNOWN)
    flags |= OSP_FB_ALBEDO;
  if (denoising || m_normalType != ANARI_UNKNOWN)
    flags |= OSP_FB_NORMAL;
  if (m_primIdType != ANARI_UNKNOWN)
    flags |= OSP_FB_ID_PRIMITIVE;
  if (m_objIdType != ANARI_UNKNOWN)
    flags |= OSP_FB_ID_OBJECT;
  if (m_instIdType != ANARI_UNKNOWN)
    flags |= OSP_FB_ID_INSTANCE;

  m_osprayFrameBuffer = ospNewFrameBuffer(m_frameData.size.x,
      m_frameData.size.y,
      osprayFormatFromANARI(m_colorType),
      flags);

  if (m_denoising) {
    if (!m_osprayDenoiser)
      m_osprayDenoiser = ospNewImageOperation("denoiser");

    OSPData data =
        ospNewSharedData1D(&m_osprayDenoiser, OSP_IMAGE_OPERATION, 1);
    ospSetObject(m_osprayFrameBuffer, "imageOperation", data);
    ospCommit(m_osprayFrameBuffer);
    ospRelease(data);
  }
}

bool Frame::getProperty(const std::string_view &name,
    ANARIDataType type,
    void *ptr,
    uint64_t size,
    uint32_t flags)
{
  if (type == ANARI_FLOAT32 && name == "duration") {
    if (flags & ANARI_WAIT)
      wait();
    std::memcpy(ptr, &m_duration, std::min((size_t)size, sizeof(m_duration)));
    return true;
  }

  return false;
}

void Frame::renderFrame()
{
  auto *state = deviceState();
  state->waitOnCurrentFrame();

  state->commitBuffer.flush();

  if (!isValid()) {
    reportMessage(
        ANARI_SEVERITY_ERROR, "skipping render of incomplete frame object");
    return;
  }

  // an unset camera 'aspect' is derived from the frame dimensions
  m_camera->setFrameAspect(m_frameData.size.y > 0
          ? float(m_frameData.size.x) / float(m_frameData.size.y)
          : 1.f);

  if (m_denoising != m_renderer->denoise())
    initFB(m_renderer->denoise());

  if (m_denoising) {
    auto quality = m_renderer->denoiseQuality();
    ospSetParam(m_osprayDenoiser, "quality", OSP_UINT, &quality);
    bool denoiseAlpha = m_renderer->denoiseAlpha();
    ospSetParam(m_osprayDenoiser, "denoiseAlpha", OSP_BOOL, &denoiseAlpha);
    ospCommit(m_osprayDenoiser);
  }

  if (state->commitBuffer.lastObjectFinalization() > m_frameLastRendered) {
    m_world->setAmbientLightValues(
        m_renderer->ambientColor(), m_renderer->ambientRadiance());
    ospResetAccumulation(m_osprayFrameBuffer);
  } else if (!m_accumulation) {
    ospResetAccumulation(m_osprayFrameBuffer);
  }

  m_world->osprayWorldUpdate();

  m_frameLastRendered = helium::newTimeStamp();
  state->currentFrame = this;

  m_osprayFuture = ospRenderFrame(m_osprayFrameBuffer,
      m_renderer->osprayRenderer(),
      m_camera->osprayCamera(),
      m_world->osprayWorld());
}

void *Frame::map(std::string_view channel,
    uint32_t *width,
    uint32_t *height,
    ANARIDataType *pixelType)
{
  wait();

  *width = m_frameData.size.x;
  *height = m_frameData.size.y;

  const size_t numPixels =
      size_t(m_frameData.size.x) * size_t(m_frameData.size.y);

  auto mapOspray = [&](OSPFrameBufferChannel ch, const void *&buf) -> void * {
    buf = ospMapFrameBuffer(m_osprayFrameBuffer, ch);
    return const_cast<void *>(buf);
  };

  if ((channel == "color" || channel == "channel.color")
      && m_colorType != ANARI_UNKNOWN) {
    *pixelType = m_colorType;
    return mapOspray(OSP_FB_COLOR, m_osprayColorBuffer);
  }
  if ((channel == "depth" || channel == "channel.depth")
      && m_depthType != ANARI_UNKNOWN) {
    *pixelType = ANARI_FLOAT32;
    return mapOspray(OSP_FB_DEPTH, m_osprayDepthBuffer);
  }
  if ((channel == "albedo" || channel == "channel.albedo")
      && m_albedoType != ANARI_UNKNOWN) {
    *pixelType = m_albedoType;
    const float *src = static_cast<const float *>(
        mapOspray(OSP_FB_ALBEDO, m_osprayAlbedoBuffer));
    if (!src)
      return nullptr;
    if (m_albedoType == ANARI_FLOAT32_VEC3)
      return const_cast<float *>(src);
    m_albedoStaging = convertFromFloatChannel(src, numPixels, 3, m_albedoType);
    ospUnmapFrameBuffer(m_osprayAlbedoBuffer, m_osprayFrameBuffer);
    m_osprayAlbedoBuffer = nullptr;
    return m_albedoStaging.data();
  }
  if ((channel == "normal" || channel == "channel.normal")
      && m_normalType != ANARI_UNKNOWN) {
    *pixelType = m_normalType;
    const float *src = static_cast<const float *>(
        mapOspray(OSP_FB_NORMAL, m_osprayNormalBuffer));
    if (!src)
      return nullptr;
    if (m_normalType == ANARI_FLOAT32_VEC3)
      return const_cast<float *>(src);
    m_normalStaging = convertFromFloatChannel(src, numPixels, 3, m_normalType);
    ospUnmapFrameBuffer(m_osprayNormalBuffer, m_osprayFrameBuffer);
    m_osprayNormalBuffer = nullptr;
    return m_normalStaging.data();
  }
  if ((channel == "primitiveId" || channel == "channel.primitiveId")
      && m_primIdType != ANARI_UNKNOWN) {
    *pixelType = ANARI_UINT32;
    return mapOspray(OSP_FB_ID_PRIMITIVE, m_osprayPrimIdBuffer);
  }
  if ((channel == "objectId" || channel == "channel.objectId")
      && m_objIdType != ANARI_UNKNOWN) {
    *pixelType = ANARI_UINT32;
    return mapOspray(OSP_FB_ID_OBJECT, m_osprayObjIdBuffer);
  }
  if ((channel == "instanceId" || channel == "channel.instanceId")
      && m_instIdType != ANARI_UNKNOWN) {
    *pixelType = ANARI_UINT32;
    return mapOspray(OSP_FB_ID_INSTANCE, m_osprayInstIdBuffer);
  }

  *width = 0;
  *height = 0;
  *pixelType = ANARI_UNKNOWN;
  return nullptr;
}

void Frame::unmap(std::string_view channel)
{
  if (channel == "color" || channel == "channel.color") {
    if (m_osprayColorBuffer)
      ospUnmapFrameBuffer(m_osprayColorBuffer, m_osprayFrameBuffer);
    m_osprayColorBuffer = nullptr;
  }
  if (channel == "depth" || channel == "channel.depth") {
    if (m_osprayDepthBuffer)
      ospUnmapFrameBuffer(m_osprayDepthBuffer, m_osprayFrameBuffer);
    m_osprayDepthBuffer = nullptr;
  }
  if (channel == "albedo" || channel == "channel.albedo") {
    if (m_osprayAlbedoBuffer)
      ospUnmapFrameBuffer(m_osprayAlbedoBuffer, m_osprayFrameBuffer);
    m_osprayAlbedoBuffer = nullptr;
  }
  if (channel == "normal" || channel == "channel.normal") {
    if (m_osprayNormalBuffer)
      ospUnmapFrameBuffer(m_osprayNormalBuffer, m_osprayFrameBuffer);
    m_osprayNormalBuffer = nullptr;
  }
  if (channel == "primitiveId" || channel == "channel.primitiveId") {
    if (m_osprayPrimIdBuffer)
      ospUnmapFrameBuffer(m_osprayPrimIdBuffer, m_osprayFrameBuffer);
    m_osprayPrimIdBuffer = nullptr;
  }
  if (channel == "objectId" || channel == "channel.objectId") {
    if (m_osprayObjIdBuffer)
      ospUnmapFrameBuffer(m_osprayObjIdBuffer, m_osprayFrameBuffer);
    m_osprayObjIdBuffer = nullptr;
  }
  if (channel == "instanceId" || channel == "channel.instanceId") {
    if (m_osprayInstIdBuffer)
      ospUnmapFrameBuffer(m_osprayInstIdBuffer, m_osprayFrameBuffer);
    m_osprayInstIdBuffer = nullptr;
  }
}

int Frame::frameReady(ANARIWaitMask m)
{
  if (m == ANARI_NO_WAIT)
    return ready();
  else {
    wait();
    return 1;
  }
}

void Frame::discard()
{
  if (m_osprayFuture)
    ospCancel(m_osprayFuture);
}

bool Frame::ready() const
{
  return m_osprayFuture ? ospIsReady(m_osprayFuture) : true;
}

void Frame::wait() const
{
  if (m_osprayFuture) {
    ospWait(m_osprayFuture);
    m_duration = ospGetTaskDuration(m_osprayFuture);
    ospRelease(m_osprayFuture);
    m_osprayFuture = nullptr;
    deviceState()->currentFrame = nullptr;
  }
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Frame *);
