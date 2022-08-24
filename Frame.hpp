// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.hpp"

namespace anari {
namespace ospray {

struct Frame : public Object
{
  Frame();
  ~Frame() override;

  bool getProperty(const std::string &name,
      ANARIDataType type,
      void *ptr,
      uint32_t flags) override;

  void commit() override;
  void setParam(const char *id, ANARIDataType type, const void *mem) override;

  void render(ANARIDevice d);

  OSPFuture future();

  const void *map(const std::string &channel,
      uint32_t *width,
      uint32_t *height,
      ANARIDataType *pixelType);

  void unmap(const char *ptr);

 private:
  void constructHandle();

  bool m_reconstructOnCommit{true};

  int m_size_x{0};
  int m_size_y{0};
  int m_format{ANARI_FLOAT32_VEC4};
  bool m_depthBuffer{false};
  bool m_normalBuffer{false};
  bool m_albedoBuffer{false};

  IntrusivePtr<Object> m_world;
  IntrusivePtr<Object> m_camera;
  IntrusivePtr<Object> m_renderer;

  OSPFuture m_future{nullptr};

  ANARIFrameCompletionCallback m_continuation{nullptr};
  void *m_continuationPtr{nullptr};
};

} // namespace ospray
} // namespace anari
