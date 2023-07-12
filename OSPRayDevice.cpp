// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "OSPRayDevice.h"

#include "anari/ext/debug/DebugObject.h"

#include "array/Array1D.h"
#include "array/Array2D.h"
#include "array/Array3D.h"
#include "array/ObjectArray.h"
#include "frame/Frame.h"
#include "scene/volume/spatial_field/SpatialField.h"

namespace anari_ospray {

///////////////////////////////////////////////////////////////////////////////
// Generated function declarations ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const char **query_object_types(ANARIDataType type);

const void *query_object_info(ANARIDataType type,
    const char *subtype,
    const char *infoName,
    ANARIDataType infoType);

const void *query_param_info(ANARIDataType type,
    const char *subtype,
    const char *paramName,
    ANARIDataType paramType,
    const char *infoName,
    ANARIDataType infoType);

anari::debug_device::ObjectFactory *getDebugFactory();

const char **query_extensions();

///////////////////////////////////////////////////////////////////////////////
// Helper functions ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename HANDLE_T, typename OBJECT_T>
inline HANDLE_T getHandleForAPI(OBJECT_T *object)
{
  return (HANDLE_T)object;
}

template <typename OBJECT_T, typename HANDLE_T, typename... Args>
inline HANDLE_T createObjectForAPI(OSPRayGlobalState *s, Args &&...args)
{
  return getHandleForAPI<HANDLE_T>(
      new OBJECT_T(s, std::forward<Args>(args)...));
}

///////////////////////////////////////////////////////////////////////////////
// OSPRayDevice definitions ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Data Arrays ////////////////////////////////////////////////////////////////

ANARIArray1D OSPRayDevice::newArray1D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems)
{
  OSPRayDeviceScope ds(this);

  Array1DMemoryDescriptor md;
  md.appMemory = appMemory;
  md.deleter = deleter;
  md.deleterPtr = userData;
  md.elementType = type;
  md.numItems = numItems;

  if (anari::isObject(type))
    return createObjectForAPI<ObjectArray, ANARIArray1D>(deviceState(), md);
  else
    return createObjectForAPI<Array1D, ANARIArray1D>(deviceState(), md);
}

ANARIArray2D OSPRayDevice::newArray2D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems1,
    uint64_t numItems2)
{
  OSPRayDeviceScope ds(this);

  Array2DMemoryDescriptor md;
  md.appMemory = appMemory;
  md.deleter = deleter;
  md.deleterPtr = userData;
  md.elementType = type;
  md.numItems1 = numItems1;
  md.numItems2 = numItems2;

  return createObjectForAPI<Array2D, ANARIArray2D>(deviceState(), md);
}

ANARIArray3D OSPRayDevice::newArray3D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems1,
    uint64_t numItems2,
    uint64_t numItems3)
{
  OSPRayDeviceScope ds(this);

  Array3DMemoryDescriptor md;
  md.appMemory = appMemory;
  md.deleter = deleter;
  md.deleterPtr = userData;
  md.elementType = type;
  md.numItems1 = numItems1;
  md.numItems2 = numItems2;
  md.numItems3 = numItems3;

  return createObjectForAPI<Array3D, ANARIArray3D>(deviceState(), md);
}

void *OSPRayDevice::mapArray(ANARIArray a)
{
  OSPRayDeviceScope ds(this);
  return helium::BaseDevice::mapArray(a);
}

void OSPRayDevice::unmapArray(ANARIArray a)
{
  OSPRayDeviceScope ds(this);
  helium::BaseDevice::unmapArray(a);
}

// Renderable Objects /////////////////////////////////////////////////////////

ANARILight OSPRayDevice::newLight(const char *subtype)
{
  OSPRayDeviceScope ds(this);
  return getHandleForAPI<ANARILight>(
      Light::createInstance(subtype, deviceState()));
}

ANARICamera OSPRayDevice::newCamera(const char *subtype)
{
  OSPRayDeviceScope ds(this);
  return getHandleForAPI<ANARICamera>(
      Camera::createInstance(subtype, deviceState()));
}

ANARIGeometry OSPRayDevice::newGeometry(const char *subtype)
{
  OSPRayDeviceScope ds(this);
  return getHandleForAPI<ANARIGeometry>(
      Geometry::createInstance(subtype, deviceState()));
}

ANARISpatialField OSPRayDevice::newSpatialField(const char *subtype)
{
  OSPRayDeviceScope ds(this);
  return getHandleForAPI<ANARISpatialField>(
      SpatialField::createInstance(subtype, deviceState()));
}

ANARISurface OSPRayDevice::newSurface()
{
  OSPRayDeviceScope ds(this);
  return createObjectForAPI<Surface, ANARISurface>(deviceState());
}

ANARIVolume OSPRayDevice::newVolume(const char *subtype)
{
  OSPRayDeviceScope ds(this);
  return getHandleForAPI<ANARIVolume>(
      Volume::createInstance(subtype, deviceState()));
}

// Surface Meta-Data //////////////////////////////////////////////////////////

ANARIMaterial OSPRayDevice::newMaterial(const char *subtype)
{
  OSPRayDeviceScope ds(this);
  return getHandleForAPI<ANARIMaterial>(
      Material::createInstance(subtype, deviceState()));
}

ANARISampler OSPRayDevice::newSampler(const char *subtype)
{
  OSPRayDeviceScope ds(this);
  return getHandleForAPI<ANARISampler>(
      Sampler::createInstance(subtype, deviceState()));
}

// Instancing /////////////////////////////////////////////////////////////////

ANARIGroup OSPRayDevice::newGroup()
{
  OSPRayDeviceScope ds(this);
  return createObjectForAPI<Group, ANARIGroup>(deviceState());
}

ANARIInstance OSPRayDevice::newInstance()
{
  OSPRayDeviceScope ds(this);
  return createObjectForAPI<Instance, ANARIInstance>(deviceState());
}

// Top-level Worlds ///////////////////////////////////////////////////////////

ANARIWorld OSPRayDevice::newWorld()
{
  OSPRayDeviceScope ds(this);
  return createObjectForAPI<World, ANARIWorld>(deviceState());
}

// Query functions ////////////////////////////////////////////////////////////

const char **OSPRayDevice::getObjectSubtypes(ANARIDataType objectType)
{
  return query_object_types(objectType);
}

const void *OSPRayDevice::getObjectInfo(ANARIDataType objectType,
    const char *objectSubtype,
    const char *infoName,
    ANARIDataType infoType)
{
  return query_object_info(objectType, objectSubtype, infoName, infoType);
}

const void *OSPRayDevice::getParameterInfo(ANARIDataType objectType,
    const char *objectSubtype,
    const char *parameterName,
    ANARIDataType parameterType,
    const char *infoName,
    ANARIDataType infoType)
{
  return query_param_info(objectType,
      objectSubtype,
      parameterName,
      parameterType,
      infoName,
      infoType);
}

// Object + Parameter Lifetime Management /////////////////////////////////////

int OSPRayDevice::getProperty(ANARIObject object,
    const char *name,
    ANARIDataType type,
    void *mem,
    uint64_t size,
    uint32_t mask)
{
  OSPRayDeviceScope ds(this);

  if (handleIsDevice(object)) {
    std::string_view prop = name;
    if (prop == "debugObjects" && type == ANARI_FUNCTION_POINTER) {
      helium::writeToVoidP(mem, getDebugFactory);
      return 1;
    } else if (prop == "feature" && type == ANARI_STRING_LIST) {
      helium::writeToVoidP(mem, query_extensions());
      return 1;
    } else if (prop == "ospray" && type == ANARI_BOOL) {
      helium::writeToVoidP(mem, true);
      return 1;
    }
  } else {
    if (mask == ANARI_WAIT) {
      deviceState()->waitOnCurrentFrame();
      flushCommitBuffer();
    }
    return helium::referenceFromHandle(object).getProperty(
        name, type, mem, mask);
  }

  return 0;
}

void OSPRayDevice::setParameter(
    ANARIObject o, const char *name, ANARIDataType type, const void *mem)
{
  OSPRayDeviceScope ds(this);
  helium::BaseDevice::setParameter(o, name, type, mem);
}

void OSPRayDevice::unsetParameter(ANARIObject o, const char *name)
{
  OSPRayDeviceScope ds(this);
  helium::BaseDevice::unsetParameter(o, name);
}

void *OSPRayDevice::mapParameterArray1D(ANARIObject object,
    const char *name,
    ANARIDataType dataType,
    uint64_t numElements1,
    uint64_t *elementStride)
{
  OSPRayDeviceScope ds(this);
  return helium::BaseDevice::mapParameterArray1D(
      object, name, dataType, numElements1, elementStride);
}

void *OSPRayDevice::mapParameterArray2D(ANARIObject object,
    const char *name,
    ANARIDataType dataType,
    uint64_t numElements1,
    uint64_t numElements2,
    uint64_t *elementStride)
{
  OSPRayDeviceScope ds(this);
  return helium::BaseDevice::mapParameterArray2D(
      object, name, dataType, numElements1, numElements2, elementStride);
}

void *OSPRayDevice::mapParameterArray3D(ANARIObject object,
    const char *name,
    ANARIDataType dataType,
    uint64_t numElements1,
    uint64_t numElements2,
    uint64_t numElements3,
    uint64_t *elementStride)
{
  OSPRayDeviceScope ds(this);
  return helium::BaseDevice::mapParameterArray3D(object,
      name,
      dataType,
      numElements1,
      numElements2,
      numElements3,
      elementStride);
}

void OSPRayDevice::unmapParameterArray(ANARIObject object, const char *name)
{
  OSPRayDeviceScope ds(this);
  helium::BaseDevice::unmapParameterArray(object, name);
}

void OSPRayDevice::commitParameters(ANARIObject o)
{
  OSPRayDeviceScope ds(this);
  helium::BaseDevice::commitParameters(o);
}

void OSPRayDevice::release(ANARIObject o)
{
  OSPRayDeviceScope ds(this);
  helium::BaseDevice::release(o);
}

void OSPRayDevice::retain(ANARIObject o)
{
  OSPRayDeviceScope ds(this);
  helium::BaseDevice::retain(o);
}

// Frame Manipulation /////////////////////////////////////////////////////////

ANARIFrame OSPRayDevice::newFrame()
{
  OSPRayDeviceScope ds(this);
  return createObjectForAPI<Frame, ANARIFrame>(deviceState());
}

const void *OSPRayDevice::frameBufferMap(ANARIFrame f,
    const char *channel,
    uint32_t *width,
    uint32_t *height,
    ANARIDataType *pixelType)
{
  OSPRayDeviceScope ds(this);
  return helium::BaseDevice::frameBufferMap(
      f, channel, width, height, pixelType);
}

void OSPRayDevice::frameBufferUnmap(ANARIFrame f, const char *channel)
{
  OSPRayDeviceScope ds(this);
  return helium::BaseDevice::frameBufferUnmap(f, channel);
}

// Frame Rendering ////////////////////////////////////////////////////////////

ANARIRenderer OSPRayDevice::newRenderer(const char *subtype)
{
  OSPRayDeviceScope ds(this);
  return getHandleForAPI<ANARIRenderer>(
      Renderer::createInstance(subtype, deviceState()));
}

void OSPRayDevice::renderFrame(ANARIFrame f)
{
  OSPRayDeviceScope ds(this);
  helium::BaseDevice::renderFrame(f);
}

int OSPRayDevice::frameReady(ANARIFrame f, ANARIWaitMask m)
{
  OSPRayDeviceScope ds(this);
  return helium::BaseDevice::frameReady(f, m);
}

void OSPRayDevice::discardFrame(ANARIFrame f)
{
  OSPRayDeviceScope ds(this);
  return helium::BaseDevice::discardFrame(f);
}

// Other OSPRayDevice definitions /////////////////////////////////////////////

OSPRayDevice::OSPRayDevice(ANARIStatusCallback cb, const void *ptr)
    : helium::BaseDevice(cb, ptr)
{
  m_state = std::make_unique<OSPRayGlobalState>(this_device());
  deviceCommitParameters();
}

OSPRayDevice::OSPRayDevice(ANARILibrary l) : helium::BaseDevice(l)
{
  m_state = std::make_unique<OSPRayGlobalState>(this_device());
  deviceCommitParameters();
}

OSPRayDevice::~OSPRayDevice()
{
  setOSPRayDevice();

  auto &state = *deviceState();

  state.commitBuffer.clear();

  reportMessage(ANARI_SEVERITY_DEBUG, "destroying ospray device (%p)", this);

  ospShutdown();

  // NOTE: These object leak warnings are not required to be done by
  //       implementations as the debug layer in the SDK is far more
  //       comprehensive and designed for detecting bugs like this. However
  //       these simple checks are very straightforward to implement and do not
  //       really add substantial code complexity, so they are provided out of
  //       convenience.

  auto reportLeaks = [&](size_t &count, const char *handleType) {
    if (count != 0) {
      reportMessage(ANARI_SEVERITY_WARNING,
          "detected %zu leaked %s objects",
          count,
          handleType);
    }
  };

  reportLeaks(state.objectCounts.frames, "ANARIFrame");
  reportLeaks(state.objectCounts.cameras, "ANARICamera");
  reportLeaks(state.objectCounts.renderers, "ANARIRenderer");
  reportLeaks(state.objectCounts.worlds, "ANARIWorld");
  reportLeaks(state.objectCounts.instances, "ANARIInstance");
  reportLeaks(state.objectCounts.groups, "ANARIGroup");
  reportLeaks(state.objectCounts.lights, "ANARILight");
  reportLeaks(state.objectCounts.surfaces, "ANARISurface");
  reportLeaks(state.objectCounts.geometries, "ANARIGeometry");
  reportLeaks(state.objectCounts.materials, "ANARIMaterial");
  reportLeaks(state.objectCounts.samplers, "ANARISampler");
  reportLeaks(state.objectCounts.volumes, "ANARIVolume");
  reportLeaks(state.objectCounts.spatialFields, "ANARISpatialField");
  reportLeaks(state.objectCounts.arrays, "ANARIArray");

  if (state.objectCounts.unknown != 0) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "detected %zu leaked ANARIObject objects created by unknown subtypes",
        state.objectCounts.unknown);
  }
}

void OSPRayDevice::initDevice()
{
  if (m_initialized)
    return;

  reportMessage(ANARI_SEVERITY_DEBUG, "initializing ospray device (%p)", this);

  auto &state = *deviceState();

  ospLoadModule("cpu");
  state.osprayDevice = ospNewDevice("cpu");

  ospDeviceSetErrorCallback(
      state.osprayDevice,
      [](void *userData, OSPError, const char *errorDetails) {
        auto *ospd = (OSPRayDevice *)userData;
        ospd->reportMessage(
            ANARI_SEVERITY_ERROR, "OSPRay ERROR: %s", errorDetails);
      },
      this);

  ospDeviceSetStatusCallback(
      state.osprayDevice,
      [](void *userData, const char *message) {
        auto *ospd = (OSPRayDevice *)userData;
        ospd->reportMessage(ANARI_SEVERITY_INFO, "OSPRay INFO: %s", message);
      },
      this);

  auto logLevel = OSP_LOG_INFO;
  ospDeviceSetParam(state.osprayDevice, "logLevel", OSP_INT, &logLevel);
  ospDeviceCommit(state.osprayDevice);

  m_initialized = true;
}

void OSPRayDevice::deviceCommitParameters()
{
  auto &state = *deviceState();

  bool allowInvalidSurfaceMaterials = state.allowInvalidSurfaceMaterials;

  state.allowInvalidSurfaceMaterials =
      getParam<bool>("allowInvalidMaterials", true);
  state.invalidMaterialColor =
      getParam<float4>("invalidMaterialColor", float4(1.f, 0.f, 1.f, 1.f));

  if (allowInvalidSurfaceMaterials != state.allowInvalidSurfaceMaterials)
    state.objectUpdates.lastBLSReconstructSceneRequest = helium::newTimeStamp();

  helium::BaseDevice::deviceCommitParameters();
}

void OSPRayDevice::setOSPRayDevice()
{
  m_appDevice = ospGetCurrentDevice();
  ospSetCurrentDevice(deviceState()->osprayDevice);
}

void OSPRayDevice::revertOSPRayDevice()
{
  ospSetCurrentDevice(m_appDevice);
}

OSPRayGlobalState *OSPRayDevice::deviceState() const
{
  return (OSPRayGlobalState *)helium::BaseDevice::m_state.get();
}

OSPRayDevice::OSPRayDeviceScope::OSPRayDeviceScope(OSPRayDevice *d)
    : m_device(d)
{
  d->initDevice();
  m_device->setOSPRayDevice();
}

OSPRayDevice::OSPRayDeviceScope::~OSPRayDeviceScope()
{
  m_device->revertOSPRayDevice();
}

} // namespace anari_ospray
