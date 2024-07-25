// DUMMY hidport.h
// This file is intentionally empty.
//
#ifdef NTVERSION
#if NTVERSION == 'WDM'
#error The file is from the DDK at src\wdm\hid\inc. Install DDK HID samples to install hidport.h.  Or update INCLUDE path for hidport.h in the DDK! 
#endif
