// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Camera.hpp"

namespace anari {
namespace ospray {

Camera::Camera(std::string subtype)
    : Object(ospNewCamera(subtype.c_str()), subtype)
{}

} // namespace ospray
} // namespace anari
