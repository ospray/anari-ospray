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
  const bool use =
      m_opacityData && (m_opacityData->size() != m_color_opacity.size());
  if (use)
    od = m_opacityData->osprayData();
  else {
    if (m_opacityData)
      for (size_t i = 0; i < m_color_opacity.size(); ++i)
        m_color_opacity[i].w *= m_opacityData->beginAs<float>()[i];
    od = ospNewSharedData1DStride(&m_color_opacity.data()->w,
        OSP_FLOAT,
        m_color_opacity.size(),
        sizeof(float4));
  }

  ospSetParam(tf, "opacity", OSP_DATA, &od);
  if (!use)
    ospRelease(od);
  ospCommit(tf);

  auto om = osprayModel();
  auto ov = m_field->osprayVolume();
  ospSetParam(om, "volume", OSP_VOLUME, &ov);
  ospSetParam(om, "densityScale", OSP_FLOAT, &m_densityScale);
  ospSetParam(om, "transferFunction", OSP_TRANSFER_FUNCTION, &tf);
  ospCommit(om);
}

bool TransferFunction1DVolume::isValid() const
{
  return m_field && m_field->isValid();
}

} // namespace anari_ospray
