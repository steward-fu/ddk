// IPFilterHookioctl.h
//
// Define control codes for IPFilterHook driver
//

#ifndef __IPFilterHookioctl__h_
#define __IPFilterHookioctl__h_

#define IPFILTERHOOK_IOCTL_SetHook CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IPFILTERHOOK_IOCTL_ClearHook CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif
