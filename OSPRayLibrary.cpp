// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "OSPRayDevice.h"
#include "anari/backend/LibraryImpl.h"
#include "anari_library_ospray_export.h"

namespace anari_ospray {

const char **query_extensions();

struct OSPRayLibrary : public anari::LibraryImpl
{
  OSPRayLibrary(
      void *lib, ANARIStatusCallback defaultStatusCB, const void *statusCBPtr);

  ANARIDevice newDevice(const char *subtype) override;
  const char **getDeviceExtensions(const char *deviceType) override;
};

// Definitions ////////////////////////////////////////////////////////////////

OSPRayLibrary::OSPRayLibrary(
    void *lib, ANARIStatusCallback defaultStatusCB, const void *statusCBPtr)
    : anari::LibraryImpl(lib, defaultStatusCB, statusCBPtr)
{}

ANARIDevice OSPRayLibrary::newDevice(const char * /*subtype*/)
{
  return (ANARIDevice) new OSPRayDevice(this_library());
}

const char **OSPRayLibrary::getDeviceExtensions(const char * /*deviceType*/)
{
  return query_extensions();
}

} // namespace anari_ospray

// Define library entrypoint //////////////////////////////////////////////////

extern "C" ANARI_OSPRAY_DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_ENTRYPOINT(
    ospray, handle, scb, scbPtr)
{
  return (ANARILibrary) new anari_ospray::OSPRayLibrary(handle, scb, scbPtr);
}

extern "C" ANARI_OSPRAY_DEVICE_INTERFACE ANARIDevice anariNewOSPRayDevice(
    ANARIStatusCallback defaultCallback, const void *userPtr)
{
  return (ANARIDevice) new anari_ospray::OSPRayDevice(defaultCallback, userPtr);
}
