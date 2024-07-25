//		Vsvctab.h
//
//	Helper macros for Vtoolsc.
//
//	Copyright (c) 1994 by Compuware Corporation
//

// NOTE:	Do not guard this file with a sentinel
//		It should be included multiple times.


#undef	Begin_VxD_Service_Table
#undef	VxD_Service
#undef	End_VxD_Service_Table

#ifdef DEVICE_MAIN

// Macro to define and initialize the VxD service table

#define Begin_VxD_Service_Table(deviceName) void (*VXD_SERVICE_TABLE[])()={

#define End_VxD_Service_Table	(void(*)()) 0L}; 	\
 	extern int VXD_SERVICE_TABLE_SIZE = sizeof(VXD_SERVICE_TABLE)/4 - 1;

#define VxD_Service(serviceName) (void(*)())serviceName, 

#else

// Expand to declare services for external modules

#define Begin_VxD_Service_Table(deviceName) enum deviceName##_services \
			{ deviceName##_dummy = (deviceName##_DeviceID << 16)-1,

#define End_VxD_Service_Table	 };

#define VxD_Service(serviceName)  __##serviceName,

#endif
