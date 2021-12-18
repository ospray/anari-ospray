// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Renderer.hpp"

namespace anari {
namespace ospray {

Renderer::Renderer(std::string subtype)
    : Object(ospNewRenderer(subtype.c_str()), subtype)
{}

} // namespace ospray
} // namespace anari