// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Frame.h"
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
  return m_valid;
}

OSPRayGlobalState *Frame::deviceState() const
{
  return (OSPRayGlobalState *)helium::BaseObject::m_state;
}

void Frame::commit()
{
  m_renderer = getParamObject<Renderer>("renderer");
  if (!m_renderer) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'renderer' on frame");
  }

  m_camera = getParamObject<Camera>("camera");
  if (!m_camera) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "missing required parameter 'camera' on frame");
  }

  m_world = getParamObject<World>("world");
  if (!m_world) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "missing required parameter 'world' on frame");
  }

  m_valid = m_renderer && m_renderer->isValid() && m_camera
      && m_camera->isValid() && m_world && m_world->isValid();

  m_colorType = getParam<anari::DataType>("channel.color", ANARI_UNKNOWN);

  m_frameData.size = getParam<uint2>("size", uint2(10));

  initFB(m_renderer->denoise());
}

void Frame::initFB(const bool denoising)
{
  m_denoising = denoising;
  auto flags = OSP_FB_COLOR | OSP_FB_DEPTH | OSP_FB_ACCUM;
  if (m_denoising)
    flags |= OSP_FB_ALBEDO | OSP_FB_NORMAL;

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

bool Frame::getProperty(
    const std::string_view &name, ANARIDataType type, void *ptr, uint32_t flags)
{
  if (type == ANARI_FLOAT32 && name == "duration") {
    if (flags & ANARI_WAIT)
      wait();
    helium::writeToVoidP(ptr, m_duration);
    return true;
  }

  return 0;
}

void Frame::renderFrame()
{
  auto *state = deviceState();
  state->waitOnCurrentFrame();

  auto start = std::chrono::steady_clock::now();

  state->commitBufferFlush();

  if (m_denoising != m_renderer->denoise())
    initFB(!m_denoising); // toggle denoiser

  if (m_denoising) {
    auto quality = m_renderer->denoiseQuality();
    ospSetParam(m_osprayDenoiser, "quality", OSP_UINT, &quality);
    bool denoiseAlpha = m_renderer->denoiseAlpha();
    ospSetParam(m_osprayDenoiser, "denoiseAlpha", OSP_BOOL, &denoiseAlpha);
    ospCommit(m_osprayDenoiser);
  }

  if (!isValid()) {
    reportMessage(
        ANARI_SEVERITY_ERROR, "skipping render of incomplete frame object");
    return;
  }

  if (state->commitBufferLastFlush() > m_frameLastRendered) {
    m_world->setAmbientLightValues(
        m_renderer->ambientColor(), m_renderer->ambientRadiance());
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

  if (channel == "color" || channel == "channel.color") {
    *pixelType = m_colorType;
    return (void *)(m_osprayColorBuffer =
                        ospMapFrameBuffer(m_osprayFrameBuffer, OSP_FB_COLOR));
  } else if (channel == "depth" || channel == "channel.depth") {
    *pixelType = ANARI_FLOAT32;
    return (void *)(m_osprayDepthBuffer =
                        ospMapFrameBuffer(m_osprayFrameBuffer, OSP_FB_DEPTH));
  } else {
    *width = 0;
    *height = 0;
    *pixelType = ANARI_UNKNOWN;
    return nullptr;
  }
}

void Frame::unmap(std::string_view channel)
{
  if (channel == "color" || channel == "channel.color") {
    if (m_osprayColorBuffer)
      ospUnmapFrameBuffer(m_osprayColorBuffer, m_osprayFrameBuffer);
  } else if (channel == "depth" || channel == "channel.depth") {
    if (m_osprayDepthBuffer)
      ospUnmapFrameBuffer(m_osprayDepthBuffer, m_osprayFrameBuffer);
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
