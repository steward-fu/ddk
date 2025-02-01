#ifndef __LOGPNP_H__
#define __LOGPNP_H__

BOOL
LogPnP_LoadLogicalDevice(
    IN     PCHAR        OptFileName,
    IN OUT PHID_DEVICE  HidDevice  
);

BOOL
LogPnP_LoadPpdFromFile(
    IN  PCHAR   FileName,
    OUT PHIDP_PREPARSED_DATA   *PpdBuffer,
    OUT PULONG  PpdBufferLength
);

VOID
LogPnP_CloseLogicalHIDDevices(
    IN  PHID_DEVICE LogicalDeviceList,
    IN  ULONG       NumLogicalDevices
);

BOOL
LogPnP_IsLogicalDevice(
    IN  PHID_DEVICE HidDevice
);

#endif
