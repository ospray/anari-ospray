// Copyright 2021-2022 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Camera.hpp"
#include "util/Math.hpp"

namespace anari {
namespace ospray {

Camera::Camera(std::string subtype)
    : Object(ospNewCamera(subtype.c_str()), subtype)
{}

void Camera::setParam(const char *_id, ANARIDataType type, const void *mem)
{
  std::string id(_id);

  if (id == "imageRegion") {
    Object::setParam("imageStart", ANARI_FLOAT32_VEC2, (const float *)mem);
    Object::setParam("imageEnd", ANARI_FLOAT32_VEC2, ((const float *)mem) + 2);
    return;
  }
  if (id == "fovy") {
    float fovy = rad2deg(*(const float *)mem);
    Object::setParam(_id, type, &fovy);
    return;
  }

  Object::setParam(id.c_str(), type, mem);
}

} // namespace ospray
} // namespace anari
