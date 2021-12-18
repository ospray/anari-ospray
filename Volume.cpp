// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Volume.hpp"

namespace anari {
namespace ospray {

Volume::Volume()
    : Object(ospNewVolumetricModel(), "<none>", CommitPriority::VOLUME),
      m_tf(ospNewTransferFunction("piecewiseLinear"), "piecewiseLinear")
{}

void Volume::commit()
{
  m_tf.commit();
  auto handle = m_tf.handle();
  ospSetParam(m_object, "transferFunction", OSP_TRANSFER_FUNCTION, &handle);

  Object::commit();
}

void Volume::setParam(const char *_id, ANARIDataType type, const void *mem)
{
  std::string id(_id);

  if (id == "field")
    id = "volume";

  if (id == "color" || id == "opacity" || id == "valueRange") {
    if (type == ANARI_FLOAT32_BOX1)
      type = ANARI_FLOAT32_VEC2; // for valueRange
    m_tf.setParam(_id, type, mem);
    return;
  }

  Object::setParam(id.c_str(), type, mem);
}

} // namespace ospray
} // namespace anari
