// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "OSPRayDevice.hpp"

#include "Array.hpp"
#include "Camera.hpp"
#include "Frame.hpp"
#include "Geometry.hpp"
#include "Group.hpp"
#include "Instance.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Renderer.hpp"
#include "Sampler.hpp"
#include "SpatialField.hpp"
#include "Surface.hpp"
#include "Volume.hpp"
#include "World.hpp"

// ospray
#include "ospray/ospray_cpp.h"
#include "ospray/version.h"

#include <algorithm>
#include <cstdio>

namespace anari {
namespace ospray {

using namespace ::ospray;

///////////////////////////////////////////////////////////////////////////////
// Helpers ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void writeToVoidP(void *_p, T v)
{
  T *p = (T *)_p;
  *p = v;
}

template <typename HANDLE_T, typename OBJECT_T, typename... Args>
static HANDLE_T make_wrapped_handle(Args &&...args)
{
  return (HANDLE_T) new OBJECT_T(std::forward<Args>(args)...);
}

template <typename OBJECT_T = Object>
static OBJECT_T *wrapped_handle_cast(ANARIObject obj)
{
  return (OBJECT_T *)obj;
}

///////////////////////////////////////////////////////////////////////////////
// Default status function ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void statusFunc(const void *userData,
    ANARIDevice device,
    ANARIObject source,
    ANARIDataType sourceType,
    ANARIStatusSeverity severity,
    ANARIStatusCode code,
    const char *message)
{
  if (severity == ANARI_SEVERITY_FATAL_ERROR)
    fprintf(stderr, "[FATAL] %s", message);
  else if (severity == ANARI_SEVERITY_ERROR)
    fprintf(stderr, "[ERROR] %s", message);
  else if (severity == ANARI_SEVERITY_WARNING)
    fprintf(stderr, "[WARN ] %s", message);
  else if (severity == ANARI_SEVERITY_PERFORMANCE_WARNING)
    fprintf(stderr, "[PERF ] %s", message);
  else if (severity == ANARI_SEVERITY_INFO)
    fprintf(stderr, "[INFO ] %s", message);
}

///////////////////////////////////////////////////////////////////////////////
// OSPRayDevice definitions ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
int OSPRayDevice::deviceImplements(const char *_extension)
{
  std::string extension = _extension;

  // supported
  if (extension == "ANARI_KHR_AREA_LIGHTS")
    return 1;
  if (extension == "ANARI_KHR_TRANSFORMATION_MOTION_BLUR")
    return 1;
  if (extension == "ANARI_KHR_AUXILIARY_BUFFERS")
    return 1;
  if (extension == "ANARI_KHR_FRAME_COMPLETION_CALLBACK")
    return 1;
  if (extension == "ANARI_KHR_STOCHASTIC_RENDERING")
    return 1;

  // not supported
  if (extension == "ANARI_KHR_DEVICE_SYNCHRONIZATION")
    return 0;

  return 0;
}
*/

static ANARIStatusCode ospErr2anariCode(OSPError value)
{
  switch (value) {
  case OSP_NO_ERROR:
    return ANARI_STATUS_NO_ERROR;
  case OSP_UNKNOWN_ERROR:
    return ANARI_STATUS_UNKNOWN_ERROR;
  case OSP_INVALID_ARGUMENT:
    return ANARI_STATUS_INVALID_ARGUMENT;
  case OSP_INVALID_OPERATION:
    return ANARI_STATUS_INVALID_OPERATION;
  case OSP_OUT_OF_MEMORY:
    return ANARI_STATUS_OUT_OF_MEMORY;
  case OSP_UNSUPPORTED_CPU:
    return ANARI_STATUS_UNSUPPORTED_DEVICE;
  case OSP_VERSION_MISMATCH:
    return ANARI_STATUS_VERSION_MISMATCH;
  default:
    return ANARI_STATUS_UNKNOWN_ERROR;
  }
}

void OSPRayDevice::errorWrapper(
    void *userdata, OSPError err, const char *message)
{
  OSPRayDevice *device = static_cast<OSPRayDevice *>(userdata);
  device->m_statusCallback(device->m_statusUserdata,
      (ANARIDevice)device,
      nullptr,
      ANARI_UNKNOWN,
      ANARI_SEVERITY_ERROR,
      ospErr2anariCode(err),
      message);
}

void OSPRayDevice::statusWrapper(void *userdata, const char *message)
{
  OSPRayDevice *device = static_cast<OSPRayDevice *>(userdata);
  device->m_statusCallback(device->m_statusUserdata,
      (ANARIDevice)device,
      nullptr,
      ANARI_UNKNOWN,
      ANARI_SEVERITY_INFO,
      ANARI_STATUS_NO_ERROR,
      message);
}

// Data Arrays ////////////////////////////////////////////////////////////////

ANARIArray1D OSPRayDevice::newArray1D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems,
    uint64_t byteStride)
{
  if (byteStride != 0)
    throw std::runtime_error("strided arrays not yet supported");

  return make_wrapped_handle<ANARIArray1D, Array>(
      appMemory, numItems, 1, 1, deleter, userData, type);
}

ANARIArray2D OSPRayDevice::newArray2D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems1,
    uint64_t numItems2,
    uint64_t byteStride1,
    uint64_t byteStride2)
{
  if (byteStride1 != 0 || byteStride2 != 0)
    throw std::runtime_error("strided arrays not yet supported");

  return make_wrapped_handle<ANARIArray2D, Array>(
      appMemory, numItems1, numItems2, 1, deleter, userData, type);
}

ANARIArray3D OSPRayDevice::newArray3D(const void *appMemory,
    ANARIMemoryDeleter deleter,
    const void *userData,
    ANARIDataType type,
    uint64_t numItems1,
    uint64_t numItems2,
    uint64_t numItems3,
    uint64_t byteStride1,
    uint64_t byteStride2,
    uint64_t byteStride3)
{
  if (byteStride1 != 0 || byteStride2 != 0 || byteStride3 != 0)
    throw std::runtime_error("strided arrays not yet supported");

  return make_wrapped_handle<ANARIArray3D, Array>(
      appMemory, numItems1, numItems2, numItems3, deleter, userData, type);
}

void *OSPRayDevice::mapArray(ANARIArray a)
{
  throw std::runtime_error("mapping arrays not yet supported");
}

void OSPRayDevice::unmapArray(ANARIArray a)
{
  throw std::runtime_error("unmapping arrays not yet supported");
}

// Renderable Objects /////////////////////////////////////////////////////////

ANARILight OSPRayDevice::newLight(const char *_type)
{
  std::string type(_type);
  if (type == "directional")
    type = "distant";
  if (type == "point")
    type = "sphere";
  if (type == "ring")
    type = "spot";
  return make_wrapped_handle<ANARILight, Light>(type);
}

ANARICamera OSPRayDevice::newCamera(const char *type)
{
  return make_wrapped_handle<ANARICamera, Camera>(type);
}

ANARIGeometry OSPRayDevice::newGeometry(const char *_type)
{
  std::string type(_type);
  if (type == "triangle" || type == "quad")
    type = "mesh";
  return make_wrapped_handle<ANARIGeometry, Geometry>(type.c_str());
}

ANARISpatialField OSPRayDevice::newSpatialField(const char *type)
{
  return make_wrapped_handle<ANARISpatialField, SpatialField>(type);
}

ANARISurface OSPRayDevice::newSurface()
{
  return make_wrapped_handle<ANARISurface, Surface>();
}

ANARIVolume OSPRayDevice::newVolume(const char *_type)
{
  std::string type(_type);
  if (type == "scivis")
    return make_wrapped_handle<ANARIVolume, Volume>();
  throw std::runtime_error("could not create Volume");
}

// Model Meta-Data ////////////////////////////////////////////////////////////

ANARIMaterial OSPRayDevice::newMaterial(const char *_type)
{
  std::string type(_type);
  if (type == "matte" || type == "transparentMatte")
    type = "obj";
  return make_wrapped_handle<ANARIMaterial, Material>(type);
}

ANARISampler OSPRayDevice::newSampler(const char *_type)
{
  std::string type(_type);
  if (type == "image2D")
    type = "texture2d";
  return make_wrapped_handle<ANARISampler, Sampler>(type);
}

// Instancing /////////////////////////////////////////////////////////////////

ANARIGroup OSPRayDevice::newGroup()
{
  return make_wrapped_handle<ANARIGroup, Group>();
}

ANARIInstance OSPRayDevice::newInstance()
{
  return make_wrapped_handle<ANARIInstance, Instance>();
}

// Top-level Worlds ///////////////////////////////////////////////////////////

ANARIWorld OSPRayDevice::newWorld()
{
  return make_wrapped_handle<ANARIWorld, World>();
}

// TODO: Should move to the JSON/Python code generator later
static const char *osprayDeviceFeatures[] = {"ANARI_OSPRAY_DEVICE",
    "ANARI_CORE_OBJECTS",
    "ANARI_KHR_CAMERA_OMNIDIRECTIONAL",
    "ANARI_KHR_CAMERA_ORTHOGRAPHIC",
    "ANARI_KHR_CAMERA_PERSPECTIVE",
    "ANARI_KHR_GEOMETRY_CURVE",
    "ANARI_KHR_GEOMETRY_CYLINDER",
    "ANARI_KHR_GEOMETRY_QUAD",
    "ANARI_KHR_GEOMETRY_SPHERE",
    "ANARI_KHR_GEOMETRY_TRIANGLE",
    "ANARI_KHR_LIGHT_DIRECTIONAL",
    "ANARI_KHR_LIGHT_POINT",
    "ANARI_KHR_LIGHT_SPOT",
    "ANARI_KHR_AREA_LIGHTS", // TODO: is that the right string name?
    "ANARI_KHR_MATERIAL_MATTE",
    "ANARI_KHR_MATERIAL_TRANSPARENT_MATTE",
    //"ANARI_KHR_SAMPLER_IMAGE1D", supported?
    "ANARI_KHR_SAMPLER_IMAGE2D",
    //"ANARI_KHR_SAMPLER_IMAGE3D", support?
    //"ANARI_KHR_SAMPLER_PRIMITIVE",// TODO: What's sampler primitve?
    "ANARI_KHR_SAMPLER_TRANSFORM",
    "ANARI_KHR_SPATIAL_FIELD_STRUCTURED_REGULAR",
    "ANARI_KHR_VOLUME_SCIVIS",
    "ANARI_CORE_API",
    "ANARI_SPEC_ALL",
    "ANARI_KHR_FRAME_CONTINUATION",
    "ANARI_KHR_TRANSFORMATION_MOTION_BLUR", // TODO: right string name?
    "ANARI_KHR_AUXILIARY_BUFFERS", // TODO: right string name?
    "ANARI_KHR_FRAME_COMPLETION_CALLBACK", // TODO: right string name?
    "ANARI_KHR_STOCHASTIC_RENDERING", // TODO: right string name?
    0};

int OSPRayDevice::getProperty(ANARIObject object,
    const char *name,
    ANARIDataType type,
    void *mem,
    uint64_t size,
    ANARIWaitMask mask)
{
  if (mask == ANARI_WAIT)
    flushCommitBuffer();

  if (handleIsDevice(object)) {
    std::string prop = name;
    if (prop == "version" && type == ANARI_INT32) {
      int version = OSPRAY_VERSION_MAJOR * 10000 + OSPRAY_VERSION_MINOR * 100
          + OSPRAY_VERSION_PATCH;
      writeToVoidP(mem, version);
      return 1;
    } else if (prop == "version.major" && type == ANARI_INT32) {
      writeToVoidP(mem, OSPRAY_VERSION_MAJOR);
      return 1;
    } else if (prop == "version.minor" && type == ANARI_INT32) {
      writeToVoidP(mem, OSPRAY_VERSION_MINOR);
      return 1;
    } else if (prop == "version.patch" && type == ANARI_INT32) {
      writeToVoidP(mem, OSPRAY_VERSION_PATCH);
      return 1;
    } else if (prop == "features" && type == ANARI_VOID_POINTER) {
      writeToVoidP(mem, osprayDeviceFeatures);
      return 1;
    }
  } else
    return wrapped_handle_cast(object)->getProperty(name, type, mem, mask);

  return 0;
}

// Object + Parameter Lifetime Management /////////////////////////////////////

void OSPRayDevice::setParameter(
    ANARIObject object, const char *name, ANARIDataType type, const void *mem)
{
  if (handleIsDevice(object)) {
    deviceSetParameter(name, type, mem);
    return;
  }

  auto *wh = wrapped_handle_cast(object);
  wh->setParam(name, type, mem);
}

void OSPRayDevice::unsetParameter(ANARIObject /*object*/, const char * /*name*/)
{
  // TODO
  // Note: for devices all deviceUnsetParameter, which is also TODO
}

void OSPRayDevice::commitParameters(ANARIObject object)
{
  if (handleIsDevice(object)) {
    deviceCommit();
    return;
  }

  auto *wh = wrapped_handle_cast(object);
  wh->refInc(RefType::INTERNAL);
  if (wh->commitPriority() != CommitPriority::DEFAULT)
    m_needToSortCommits = true;
  m_objectsToCommit.push_back(wh);
}

void OSPRayDevice::release(ANARIObject object)
{
  if (handleIsDevice(object)) {
    deviceRelease();
    return;
  }

  auto *wh = wrapped_handle_cast(object);
  if (wh)
    wh->refDec();
}

void OSPRayDevice::retain(ANARIObject object)
{
  if (handleIsDevice(object)) {
    deviceRetain();
    return;
  }

  auto *wh = wrapped_handle_cast(object);
  if (wh)
    wh->refInc();
}

// Frame Manipulation /////////////////////////////////////////////////////////

ANARIFrame OSPRayDevice::newFrame()
{
  return make_wrapped_handle<ANARIFrame, Frame>();
}

const void *OSPRayDevice::frameBufferMap(ANARIFrame fb_,
    const char *_channel,
    uint32_t *width,
    uint32_t *height,
    ANARIDataType *pixelType)
{
  const std::string channel = _channel;
  auto *fbw = wrapped_handle_cast<Frame>(fb_);
  return fbw->map(channel, width, height, pixelType);
}

void OSPRayDevice::frameBufferUnmap(ANARIFrame fb_, const char *ptr)
{
  auto *fbw = wrapped_handle_cast<Frame>(fb_);
  fbw->unmap(ptr);
}

// Frame Rendering ////////////////////////////////////////////////////////////

ANARIRenderer OSPRayDevice::newRenderer(const char *_type)
{
  std::string type(_type);
  if (type == "default")
    type = "pathtracer";
  return make_wrapped_handle<ANARIRenderer, Renderer>(type);
}

void OSPRayDevice::renderFrame(ANARIFrame f)
{
  flushCommitBuffer();
  wrapped_handle_cast<Frame>(f)->render((ANARIDevice)this);
}

int OSPRayDevice::frameReady(ANARIFrame f_, ANARIWaitMask m)
{
  auto *f = wrapped_handle_cast<Frame>(f_);
  if (m == ANARI_NO_WAIT)
    return ospIsReady(f->future());
  else {
    ospWait(f->future());
    return 1;
  }
}

bool OSPRayDevice::isModified()
{
  bool b = m_modified;
  m_modified = false;
  return b;
}

void OSPRayDevice::discardFrame(ANARIFrame f_)
{
  auto *f = wrapped_handle_cast<Frame>(f_);
  ospCancel(f->future());
}

// Other OSPRayDevice definitions /////////////////////////////////////////////

OSPRayDevice::OSPRayDevice()
{
  initOSPRayDevice();
}

OSPRayDevice::OSPRayDevice(ANARILibrary library) : DeviceImpl(library)
{
  initOSPRayDevice();
}

OSPRayDevice::~OSPRayDevice()
{
  ospShutdown();
}

void OSPRayDevice::flushCommitBuffer()
{
  if (m_needToSortCommits) {
    std::sort(m_objectsToCommit.begin(),
        m_objectsToCommit.end(),
        [](Object *o1, Object *o2) {
          return o1->commitPriority() < o2->commitPriority();
        });
  }

  m_needToSortCommits = false;

  for (auto o : m_objectsToCommit) {
    m_modified = true;
    o->commit();
    o->refDec(RefType::INTERNAL);
  }

  m_objectsToCommit.clear();
}

void OSPRayDevice::deviceCommit()
{
  if (m_statusCallback == nullptr) {
    if (defaultStatusCallback() != nullptr) {
      m_statusCallback = defaultStatusCallback();
      m_statusUserdata = defaultStatusCallbackUserPtr();
    } else
      m_statusCallback = statusFunc;
  }
  ospDeviceSetStatusCallback(m_device, statusWrapper, this);
  ospDeviceSetErrorCallback(m_device, errorWrapper, this);
  ospDeviceCommit(m_device);
}

void OSPRayDevice::deviceSetParameter(
    const char *_id, ANARIDataType type, const void *mem)
{
  std::string id(_id);

  if (id == "statusCallback" && type == ANARI_STATUS_CALLBACK)
    m_statusCallback = *(ANARIStatusCallback *)mem;
  else if (id == "statusCallbackUserData" && type == ANARI_VOID_POINTER)
    m_statusUserdata = (void *)mem;
  else
    ospDeviceSetParam(m_device, _id, enumCast<OSPDataType>(type), mem);
}

void OSPRayDevice::deviceUnsetParameter(const char * /*id*/)
{
  // TODO
}

void OSPRayDevice::deviceRelease()
{
  if (m_device)
    ospDeviceRelease(m_device);
}

void OSPRayDevice::deviceRetain()
{
  if (m_device)
    ospDeviceRetain(m_device);
}

void OSPRayDevice::initOSPRayDevice()
{
  OSPDevice device = ospGetCurrentDevice();

  if (!device) {
    OSPError initError = ospInit();

    if (initError != OSP_NO_ERROR)
      throw std::runtime_error("OSPRay not initialized correctly!");

    device = ospGetCurrentDevice();
    if (!device)
      throw std::runtime_error("OSPRay device could not be fetched!");
  }

  m_device = device;
  ospDeviceRelease(m_device);
}

} // namespace ospray
} // namespace anari

#ifdef _WIN32
#define OSPRAY_DLLEXPORT __declspec(dllexport)
#else
#define OSPRAY_DLLEXPORT
#endif

extern "C" OSPRAY_DLLEXPORT ANARI_DEFINE_LIBRARY_NEW_DEVICE(ospray, library, subtype)
{
  if (subtype == std::string("default") || subtype == std::string("ospray"))
    return (ANARIDevice) new anari::ospray::OSPRayDevice(library);
  return nullptr;
}

extern "C" OSPRAY_DLLEXPORT ANARI_DEFINE_LIBRARY_LOAD_MODULE(ospray, libdata, name)
{
  printf("...loading OSPRay module '%s'\n", name);
  ospLoadModule(name);
}
