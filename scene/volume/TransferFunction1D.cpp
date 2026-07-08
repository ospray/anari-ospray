// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "TransferFunction1D.h"

namespace anari_ospray {

TransferFunction1DVolume::TransferFunction1DVolume(OSPRayGlobalState *d)
    : Volume(d), m_colorData(this), m_opacityData(this)
{
  m_osprayTF = ospNewTransferFunction("piecewiseLinear");
}

TransferFunction1DVolume::~TransferFunction1DVolume()
{
  ospRelease(m_osprayTF);
}

void TransferFunction1DVolume::commitParameters()
{
  Volume::commitParameters();
  m_field = getParamObject<SpatialField>("value");
  if (!m_field) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "no spatial field provided to transfer function");
    return;
  }

  m_colorData = getParamObject<Array1D>("color");
  m_color4 = getParam<float4>(
      "color", float4(getParam<float3>("color", float3(1.f)), 1.0f));
  m_color4.w *= getParam<float>("opacity", 1.0f);

  m_opacityData = getParamObject<Array1D>("opacity");
  if (m_opacityData && m_opacityData->elementType() != ANARI_FLOAT32) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "opacity array must have elements of type ANARI_FLOAT32");
    m_opacityData = nullptr;
  }

  if (!getParam("valueRange", ANARI_FLOAT32_BOX1, &m_valueRange))
    m_valueRange = float2(0.f, 1.f);
  m_densityScale = getParam<float>("densityScale", 1.f);
}

void TransferFunction1DVolume::finalize()
{
  if (!isValid())
    return;

  auto &tf = m_osprayTF;
  ospSetParam(tf, "value", OSP_BOX1F, &m_valueRange);

  if (m_colorData) {
    m_color_opacity = convertToColorArray(*m_colorData);
    for (auto &c : m_color_opacity)
      c.w *= m_color4.w;
  } else
    m_color_opacity = std::vector<float4>(
        m_opacityData ? m_opacityData->size() : 1, m_color4);

  auto cd = ospNewSharedData1DStride(m_color_opacity.data(),
      OSP_VEC3F,
      m_color_opacity.size(),
      sizeof(float4));
  ospSetParam(tf, "color", OSP_DATA, &cd);
  ospRelease(cd);

  OSPData od;
  if (m_opacityData) {
    // Combine both opacity sources (color alpha and native array) by linearly
    // resampling each across the value range onto the finer of the two.
    const size_t nc = m_color_opacity.size();
    const size_t no = m_opacityData->size();
    const size_t n = std::max(nc, no);
    const float *na = m_opacityData->beginAs<float>();
    auto sample = [](auto get, size_t count, float t) {
      const float x = t * (count - 1);
      const size_t i = size_t(x);
      const size_t j = std::min(i + 1, count - 1);
      return linalg::lerp(get(i), get(j), x - i);
    };
    m_opacity.resize(n);
    for (size_t i = 0; i < n; ++i) {
      const float t = n > 1 ? float(i) / (n - 1) : 0.f;
      m_opacity[i] =
          sample([&](size_t k) { return m_color_opacity[k].w; }, nc, t)
          * sample([&](size_t k) { return na[k]; }, no, t);
    }
    od = ospNewSharedData1D(m_opacity.data(), OSP_FLOAT, n);
  } else
    od = ospNewSharedData1DStride(&m_color_opacity.data()->w,
        OSP_FLOAT,
        m_color_opacity.size(),
        sizeof(float4));

  ospSetParam(tf, "opacity", OSP_DATA, &od);
  ospRelease(od);
  ospCommit(tf);

  auto om = osprayModel();
  auto ov = m_field->osprayVolume();
  ospSetParam(om, "volume", OSP_VOLUME, &ov);
  ospSetParam(om, "densityScale", OSP_FLOAT, &m_densityScale);
  ospSetParam(om, "transferFunction", OSP_TRANSFER_FUNCTION, &tf);
  Volume::finalize();
}

bool TransferFunction1DVolume::isValid() const
{
  return m_field && m_field->isValid();
}

} // namespace anari_ospray
