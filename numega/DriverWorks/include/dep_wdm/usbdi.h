// DUMMY usbdi.h
// This file is intentionally empty.
//
#ifdef NTVERSION
#if NTVERSION == 'WDM'
#error Wrong INCLUDE path! usbdi.H is from DDK in inc\ddk\wdm\wxp; 98 DDK in src\usb\inc - install usb samples
#endif
