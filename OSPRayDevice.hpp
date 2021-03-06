// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

// ospray
#include "ospray/ospray.h"
// anari
#include "Object.hpp"
#include "anari/detail/Device.h"
#include "anari/detail/Library.h"

namespace anari {
namespace ospray {

struct OSPRayDevice : public Device
{
  /////////////////////////////////////////////////////////////////////////////
  // Main interface to accepting API calls
  /////////////////////////////////////////////////////////////////////////////

  // Device API ///////////////////////////////////////////////////////////////

  int deviceImplements(const char *extension) override;

  void deviceCommit() override;

  void deviceSetParameter(
      const char *id, ANARIDataType type, const void *mem) override;

  void deviceUnsetParameter(const char *id) override;
  void deviceRelease() override;
  void deviceRetain() override;

  // Data Arrays //////////////////////////////////////////////////////////////

  ANARIArray1D newArray1D(void *appMemory,
      ANARIMemoryDeleter deleter,
      void *userdata,
      ANARIDataType,
      uint64_t numItems1,
      uint64_t byteStride1) override;

  ANARIArray2D newArray2D(void *appMemory,
      ANARIMemoryDeleter deleter,
      void *userdata,
      ANARIDataType,
      uint64_t numItems1,
      uint64_t numItems2,
      uint64_t byteStride1,
      uint64_t byteStride2) override;

  ANARIArray3D newArray3D(void *appMemory,
      ANARIMemoryDeleter deleter,
      void *userdata,
      ANARIDataType,
      uint64_t numItems1,
      uint64_t numItems2,
      uint64_t numItems3,
      uint64_t byteStride1,
      uint64_t byteStride2,
      uint64_t byteStride3) override;

  void *mapArray(ANARIArray) override;
  void unmapArray(ANARIArray) override;

  // Renderable Objects ///////////////////////////////////////////////////////

  ANARILight newLight(const char *type) override;

  ANARICamera newCamera(const char *type) override;

  ANARIGeometry newGeometry(const char *type) override;
  ANARISpatialField newSpatialField(const char *type) override;

  ANARISurface newSurface() override;
  ANARIVolume newVolume(const char *type) override;

  ANARIMaterial newMaterial(const char *material_type) override;

  ANARISampler newSampler(const char *type) override;

  // Instancing ///////////////////////////////////////////////////////////////

  ANARIGroup newGroup() override;

  ANARIInstance newInstance() override;

  // Top-level Worlds /////////////////////////////////////////////////////////

  ANARIWorld newWorld() override;

  int getProperty(ANARIObject object,
      const char *name,
      ANARIDataType type,
      void *mem,
      uint64_t size,
      ANARIWaitMask mask) override;

  // Object + Parameter Lifetime Management ///////////////////////////////////

  void setParameter(ANARIObject object,
      const char *name,
      ANARIDataType type,
      const void *mem) override;

  void unsetParameter(ANARIObject object, const char *name) override;

  void commit(ANARIObject object) override;

  void release(ANARIObject _obj) override;
  void retain(ANARIObject _obj) override;

  // FrameBuffer Manipulation /////////////////////////////////////////////////

  ANARIFrame newFrame() override;

  const void *frameBufferMap(ANARIFrame fb, const char *channel) override;

  void frameBufferUnmap(ANARIFrame fb, const char *channel) override;

  // Frame Rendering //////////////////////////////////////////////////////////

  ANARIRenderer newRenderer(const char *type) override;

  void renderFrame(ANARIFrame) override;
  int frameReady(ANARIFrame, ANARIWaitMask) override;
  void discardFrame(ANARIFrame) override;

  /////////////////////////////////////////////////////////////////////////////
  // Helper/other functions and data members
  /////////////////////////////////////////////////////////////////////////////

  OSPRayDevice();
  ~OSPRayDevice() override;

  bool isModified();
  void flushCommitBuffer();

 private:
  OSPDevice m_device{nullptr};
  void *m_statusUserdata{nullptr};
  ANARIStatusCallback m_statusCallback{nullptr};
  bool m_modified{true};
  std::vector<Object *> m_objectsToCommit;
  bool m_needToSortCommits{false};

  static void errorWrapper(void *userdata, OSPError err, const char *message);
  static void statusWrapper(void *userdata, const char *message);
};

} // namespace ospray
} // namespace anari
