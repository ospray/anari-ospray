// Copyright 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

// ospray
#include "ospray/ospray.h"
// anari
#include "anari/anari_cpp.hpp"
// std
#include <stdexcept>

namespace anari_ospray {

template <typename ENUM_T>
inline ENUM_T enumCast(int value)
{
  throw std::runtime_error("No defined cast for given OSPRay ENUM_T.");
}

template <>
inline OSPLogLevel enumCast(int value)
{
  switch (value) {
  case ANARI_SEVERITY_DEBUG:
    return OSP_LOG_DEBUG;
  case ANARI_SEVERITY_INFO:
    return OSP_LOG_INFO;
  case ANARI_SEVERITY_WARNING:
  case ANARI_SEVERITY_PERFORMANCE_WARNING:
    return OSP_LOG_WARNING;
  case ANARI_SEVERITY_ERROR:
  case ANARI_SEVERITY_FATAL_ERROR:
    return OSP_LOG_ERROR;
  default:
    return OSP_LOG_NONE;
  }
}

template <>
inline OSPDataType enumCast(int value)
{
  switch (value) {
  case ANARI_DEVICE:
    return OSP_DEVICE;
  case ANARI_VOID_POINTER:
  case ANARI_FUNCTION_POINTER:
  case ANARI_MEMORY_DELETER:
  case ANARI_STATUS_CALLBACK:
  case ANARI_FRAME_COMPLETION_CALLBACK:
    return OSP_VOID_PTR;
  case ANARI_BOOL:
    return OSP_BOOL;
  case ANARI_OBJECT:
    return OSP_OBJECT;
  case ANARI_ARRAY:
  case ANARI_ARRAY1D:
  case ANARI_ARRAY2D:
  case ANARI_ARRAY3D:
    return OSP_DATA;
  case ANARI_CAMERA:
    return OSP_CAMERA;
  case ANARI_FRAME:
    return OSP_FRAMEBUFFER;
  case ANARI_SURFACE:
    return OSP_GEOMETRIC_MODEL;
  case ANARI_GEOMETRY:
    return OSP_GEOMETRY;
  case ANARI_INSTANCE:
    return OSP_INSTANCE;
  case ANARI_LIGHT:
    return OSP_LIGHT;
  case ANARI_MATERIAL:
    return OSP_MATERIAL;
  case ANARI_RENDERER:
    return OSP_RENDERER;
  case ANARI_SAMPLER:
    return OSP_TEXTURE;
  case ANARI_SPATIAL_FIELD:
    return OSP_VOLUME;
  case ANARI_VOLUME:
    return OSP_VOLUMETRIC_MODEL;
  case ANARI_GROUP:
    return OSP_GROUP;
  case ANARI_WORLD:
    return OSP_WORLD;
  case ANARI_STRING:
    return OSP_STRING;
  case ANARI_INT8:
    return OSP_CHAR;
  case ANARI_UINT8:
  case ANARI_UFIXED8:
    return OSP_UCHAR;
  case ANARI_UINT8_VEC2:
  case ANARI_UFIXED8_VEC2:
    return OSP_VEC2UC;
  case ANARI_UINT8_VEC3:
  case ANARI_UFIXED8_VEC3:
    return OSP_VEC3UC;
  case ANARI_UINT8_VEC4:
  case ANARI_UFIXED8_VEC4:
    return OSP_VEC4UC;
  case ANARI_INT16:
    return OSP_SHORT;
  case ANARI_UINT16:
    return OSP_USHORT;
  case ANARI_UINT16_VEC2:
    return OSP_VEC2US;
  case ANARI_UINT16_VEC3:
    return OSP_VEC3US;
  case ANARI_UINT16_VEC4:
    return OSP_VEC4US;
  case ANARI_INT32:
    return OSP_INT;
  case ANARI_INT32_VEC2:
    return OSP_VEC2I;
  case ANARI_INT32_VEC3:
    return OSP_VEC3I;
  case ANARI_INT32_VEC4:
    return OSP_VEC4I;
  case ANARI_UINT32:
  case ANARI_DATA_TYPE:
    return OSP_UINT;
  case ANARI_UINT32_VEC2:
    return OSP_VEC2UI;
  case ANARI_UINT32_VEC3:
    return OSP_VEC3UI;
  case ANARI_UINT32_VEC4:
    return OSP_VEC4UI;
  case ANARI_INT64:
    return OSP_LONG;
  case ANARI_INT64_VEC2:
    return OSP_VEC2L;
  case ANARI_INT64_VEC3:
    return OSP_VEC3L;
  case ANARI_INT64_VEC4:
    return OSP_VEC4L;
  case ANARI_UINT64:
    return OSP_ULONG;
  case ANARI_UINT64_VEC2:
    return OSP_VEC2UL;
  case ANARI_UINT64_VEC3:
    return OSP_VEC3UL;
  case ANARI_UINT64_VEC4:
    return OSP_VEC4UL;
  case ANARI_FLOAT32:
    return OSP_FLOAT;
  case ANARI_FLOAT32_VEC2:
    return OSP_VEC2F;
  case ANARI_FLOAT32_VEC3:
    return OSP_VEC3F;
  case ANARI_FLOAT32_VEC4:
    return OSP_VEC4F;
  case ANARI_FLOAT64:
    return OSP_DOUBLE;
  case ANARI_INT32_BOX1:
    return OSP_BOX1I;
  case ANARI_INT32_BOX2:
    return OSP_BOX2I;
  case ANARI_INT32_BOX3:
    return OSP_BOX3I;
  case ANARI_INT32_BOX4:
    return OSP_BOX4I;
  case ANARI_FLOAT32_BOX1:
    return OSP_BOX1F;
  case ANARI_FLOAT32_BOX2:
    return OSP_BOX2F;
  case ANARI_FLOAT32_BOX3:
    return OSP_BOX3F;
  case ANARI_FLOAT32_BOX4:
    return OSP_BOX4F;
  case ANARI_FLOAT32_MAT2:
    return OSP_LINEAR2F;
  case ANARI_FLOAT32_MAT3:
    return OSP_LINEAR3F;
  case ANARI_FLOAT32_MAT2x3:
    return OSP_AFFINE2F;
  case ANARI_FLOAT32_MAT3x4:
    return OSP_AFFINE3F;
  case ANARI_UNKNOWN:
    return OSP_UNKNOWN;
  default:
    throw std::runtime_error("Unhandled OSPDataType enum value '"
        + std::string(anari::toString(ANARIDataType(value))) + "'.");
  }
}

template <>
inline OSPTextureFormat enumCast(int value)
{
  switch (value) {
  case ANARI_UFIXED8_VEC4:
    return OSP_TEXTURE_RGBA8;
  case ANARI_UFIXED8_RGBA_SRGB:
    return OSP_TEXTURE_SRGBA;
  case ANARI_FLOAT32_VEC4:
    return OSP_TEXTURE_RGBA32F;
  case ANARI_UFIXED8_VEC3:
    return OSP_TEXTURE_RGB8;
  case ANARI_UFIXED8_RGB_SRGB:
    return OSP_TEXTURE_SRGB;
  case ANARI_FLOAT32_VEC3:
    return OSP_TEXTURE_RGB32F;
  case ANARI_UFIXED8:
    return OSP_TEXTURE_R8;
  case ANARI_FLOAT32:
    return OSP_TEXTURE_R32F;
  case ANARI_UFIXED8_VEC2:
    return OSP_TEXTURE_RA8;
  case ANARI_UFIXED16_VEC4:
    return OSP_TEXTURE_RGBA16;
  case ANARI_UFIXED16_VEC3:
    return OSP_TEXTURE_RGB16;
  case ANARI_UFIXED16_VEC2:
    return OSP_TEXTURE_RA16;
  case ANARI_UFIXED16:
    return OSP_TEXTURE_R16;
  default:
    throw std::runtime_error("Unhandled OSPTextureFormat enum value '"
        + std::string(anari::toString(ANARIDataType(value))) + "'.");
  }
}

template <>
inline OSPError enumCast(int value)
{
  switch (value) {
  case ANARI_STATUS_NO_ERROR:
    return OSP_NO_ERROR;
  case ANARI_STATUS_UNKNOWN_ERROR:
    return OSP_UNKNOWN_ERROR;
  case ANARI_STATUS_INVALID_ARGUMENT:
    return OSP_INVALID_ARGUMENT;
  case ANARI_STATUS_INVALID_OPERATION:
    return OSP_INVALID_OPERATION;
  case ANARI_STATUS_OUT_OF_MEMORY:
    return OSP_OUT_OF_MEMORY;
  case ANARI_STATUS_UNSUPPORTED_DEVICE:
    return OSP_UNSUPPORTED_CPU;
  case ANARI_STATUS_VERSION_MISMATCH:
    return OSP_VERSION_MISMATCH;
  default:
    throw std::runtime_error("Unhandled OSPError enum value.");
  }
}

template <>
inline OSPFrameBufferFormat enumCast(int value)
{
  switch (value) {
  case ANARI_UFIXED8_VEC4:
    return OSP_FB_RGBA8;
  case ANARI_UFIXED8_RGBA_SRGB:
    return OSP_FB_SRGBA;
  case ANARI_FLOAT32_VEC4:
    return OSP_FB_RGBA32F;
  default:
    throw std::runtime_error("Unhandled OSPFrameBufferFormat enum value.");
  }
}

} // namespace anari_ospray
