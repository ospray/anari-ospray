// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

// ANARI-SDK
#ifdef __cplusplus
#include <anari/anari_cpp.hpp>
#else
#include <anari/anari.h>
#endif
// ospray
#include "anari_library_ospray_export.h"

#ifdef __cplusplus
extern "C" {
#endif

ANARI_OSPRAY_DEVICE_INTERFACE ANARIDevice anariNewOSPRayDevice(
    ANARIStatusCallback defaultCallback ANARI_DEFAULT_VAL(0),
    const void *userPtr ANARI_DEFAULT_VAL(0));

#ifdef __cplusplus
} // extern "C"
#endif
