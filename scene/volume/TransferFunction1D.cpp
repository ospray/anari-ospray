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
  m_colorData = getParamObject<Array1D>("color");
  m_opacityData = getParamObject<Array1D>("opacity");
  if (!getParam("valueRange", ANARI_FLOAT32_BOX1, &m_valueRange))
    m_valueRange = float2(0.f, 1.f);
  m_densityScale = getParam<float>("densityScale", 1.f);
}

void TransferFunction1DVolume::finalize()
{
  if (!m_field) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "no spatial field provided to transfer function");
    return;
  }

  if (!m_colorData) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "no color data provided to transfer function");
    return;
  }

  if (!m_opacityData) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "no opacity data provided to transfer function");
    return;
  }

  auto tf = m_osprayTF;
  ospSetParam(tf, "value", OSP_BOX1F, &m_valueRange);
  auto cd = m_colorData->osprayData();
  ospSetParam(tf, "color", OSP_DATA, &cd);
  auto od = m_opacityData->osprayData();
  ospSetParam(tf, "opacity", OSP_DATA, &od);
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
  return m_field && m_field->isValid() && m_colorData && m_opacityData;
}

} // namespace anari_ospray
