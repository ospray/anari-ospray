// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Instance.hpp"

namespace anari {
namespace ospray {

Instance::Instance()
    : Object(ospNewInstance(), "<none>", CommitPriority::INSTANCE)
{}

} // namespace ospray
} // namespace anari
