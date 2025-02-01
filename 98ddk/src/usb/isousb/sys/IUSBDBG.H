/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    IusbDbg.h

Abstract:

	Debug macros 

Environment:

    Kernel mode

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.

Revision History:

    11/18/97 : created

--*/

#ifndef BUSBDBG_INCD
#define BUSBDBG_INCD

#if  DBG

// Defines for debug verbosity level,
//   i.e., test ISOUSB_KdPrintLevel( n, format....) against these values
//  Example:
//
// ISOUSB_KdPrint( DBGLVL_MEDIUM ,("Entering DriverEntry(), RegistryPath=\n    %ws\n", RegistryPath->Buffer ));
// 
//  The above will print iff gDebugLevel >= DBGLVL_MEDIUM
//
#define DBGLVL_OFF				0		// if gDebugLevel set to this, there is NO debug output	
#define DBGLVL_MINIMUM			1		// minimum verbosity	
#define DBGLVL_DEFAULT			2		// default verbosity level if no registry override
#define DBGLVL_MEDIUM			3		// medium verbosity
#define DBGLVL_HIGH				4		// highest 'safe' level (without severely affecting timing )
#define DBGLVL_MAXIMUM			5		// maximum level, may be dangerous



#ifndef DBGSTR_PREFIX
#define DBGSTR_PREFIX "IsoUsb: "  
#endif


// registry path used for parameters global to all instances of the driver
#define ISOUSB_REGISTRY_PARAMETERS_PATH  \
	L"\\REGISTRY\\Machine\\System\\CurrentControlSet\\SERVICES\\ISOUSB\\Parameters"



#define DPRINT DbgPrint

#define TRAP() DbgBreakPoint();



#define ISOUSB_DBGOUTSIZE		512


typedef struct _ISOUSB_DBGDATA {

	// mirrors device extension pending io count
	ULONG PendingIoCount;

	// count of pipe errors detected during the life of this device instance
	ULONG PipeErrorCount;

	// count of pipe resets performed during the life of this device instance
	ULONG ResetPipeCount;

	// count of pipe resets performed during the life of this device instance
	ULONG AbortPipeCount;

} ISOUSB_DBGDATA, *PISOUSB_DBGDATA;

//these declared in debug 'c' file
extern int gDebugLevel; 
extern PISOUSB_DBGDATA gpDbg; 


static const PCHAR szIrpMajFuncDesc[] =
{  // note this depends on corresponding values to the indexes in wdm.h
   "IRP_MJ_CREATE",
   "IRP_MJ_CREATE_NAMED_PIPE",
   "IRP_MJ_CLOSE",
   "IRP_MJ_READ",
   "IRP_MJ_WRITE",
   "IRP_MJ_QUERY_INFORMATION",
   "IRP_MJ_SET_INFORMATION",
   "IRP_MJ_QUERY_EA",
   "IRP_MJ_SET_EA",
   "IRP_MJ_FLUSH_BUFFERS",
   "IRP_MJ_QUERY_VOLUME_INFORMATION",
   "IRP_MJ_SET_VOLUME_INFORMATION",
   "IRP_MJ_DIRECTORY_CONTROL",
   "IRP_MJ_FILE_SYSTEM_CONTROL",
   "IRP_MJ_DEVICE_CONTROL",
   "IRP_MJ_INTERNAL_DEVICE_CONTROL",
   "IRP_MJ_SHUTDOWN",
   "IRP_MJ_LOCK_CONTROL",
   "IRP_MJ_CLEANUP",
   "IRP_MJ_CREATE_MAILSLOT",
   "IRP_MJ_QUERY_SECURITY",
   "IRP_MJ_SET_SECURITY",
   "IRP_MJ_POWER",          
   "IRP_MJ_SYSTEM_CONTROL", 
   "IRP_MJ_DEVICE_CHANGE",  
   "IRP_MJ_QUERY_QUOTA",    
   "IRP_MJ_SET_QUOTA",      
   "IRP_MJ_PNP"            
};
//IRP_MJ_MAXIMUM_FUNCTION defined in wdm.h


static const PCHAR szPnpMnFuncDesc[] =
{	// note this depends on corresponding values to the indexes in wdm.h 

    "IRP_MN_START_DEVICE",
    "IRP_MN_QUERY_REMOVE_DEVICE",
    "IRP_MN_REMOVE_DEVICE",
    "IRP_MN_CANCEL_REMOVE_DEVICE",
    "IRP_MN_STOP_DEVICE",
    "IRP_MN_QUERY_STOP_DEVICE",
    "IRP_MN_CANCEL_STOP_DEVICE",
    "IRP_MN_QUERY_DEVICE_RELATIONS",
    "IRP_MN_QUERY_INTERFACE",
    "IRP_MN_QUERY_CAPABILITIES",
    "IRP_MN_QUERY_RESOURCES",
    "IRP_MN_QUERY_RESOURCE_REQUIREMENTS",
    "IRP_MN_QUERY_DEVICE_TEXT",
    "IRP_MN_FILTER_RESOURCE_REQUIREMENTS",
    "IRP_MN_READ_CONFIG",
    "IRP_MN_WRITE_CONFIG",
    "IRP_MN_EJECT",
    "IRP_MN_SET_LOCK",
    "IRP_MN_QUERY_ID",
    "IRP_MN_QUERY_PNP_DEVICE_STATE",
    "IRP_MN_QUERY_BUS_INFORMATION",
    "IRP_MN_PAGING_NOTIFICATION"
};

#define IRP_PNP_MN_FUNCMAX	IRP_MN_PAGING_NOTIFICATION



static const PCHAR szSystemPowerState[] = 
{
    "PowerSystemUnspecified",
    "PowerSystemWorking",
    "PowerSystemSleeping1",
    "PowerSystemSleeping2",
    "PowerSystemSleeping3",
    "PowerSystemHibernate",
    "PowerSystemShutdown",
    "PowerSystemMaximum"
};

static const PCHAR szDevicePowerState[] = 
{
    "PowerDeviceUnspecified",
    "PowerDeviceD0",
    "PowerDeviceD1",
    "PowerDeviceD2",
    "PowerDeviceD3",
    "PowerDeviceMaximum"
};




void 
ISOUSB_KdPrintL( 
	char * szFormat,
	...
);


BOOLEAN
IsoUsb_GetRegistryDword(
    IN      PWCHAR    RegPath,
    IN      PWCHAR    ValueName,
    IN OUT  PULONG    Value
    );

#define ISOUSB_KdPrintCond( ilev, cond, _x_) \
	if( gDebugLevel && ( ilev <= gDebugLevel ) && ( cond )) { \
			DPRINT( DBGSTR_PREFIX ); \
			DPRINT _x_ ; \
	}




#define ISOUSB_KdPrint( ilev, _x_)  ISOUSB_KdPrintCond( ilev, TRUE, _x_ )


#define ISOUSB_TrapCond( ilev, cond ) if ( gDebugLevel && ( ilev <= gDebugLevel ) && (cond) ) TRAP()
#define ISOUSB_Trap( ilev )	  ISOUSB_TrapCond( ilev, TRUE )


#define ISOUSB_ASSERT( cond ) ASSERT( cond )

#define ISOUSB_StringForDevState( devState )  szDevicePowerState[ devState ] 

#define ISOUSB_StringForSysState( sysState )  szSystemPowerState[ sysState ] 

#define ISOUSB_StringForPnpMnFunc( mnfunc ) szPnpMnFuncDesc[ mnfunc ]

#define ISOUSB_StringForIrpMjFunc(  mjfunc ) szIrpMajFuncDesc[ mjfunc ]


#else // if not DBG

// dummy definitions that go away in the retail build

#define ISOUSB_KdPrintCond( ilev, cond, _x_) 
#define ISOUSB_KdPrint( ilev, _x_)  
#define ISOUSB_TrapCond( ilev, cond ) 
#define ISOUSB_Trap( ilev )
#define ISOUSB_ASSERT( cond )
#define ISOUSB_StringForDevState( devState )
#define ISOUSB_StringForSysState( sysState ) 
#define ISOUSB_StringForPnpMnFunc( mnfunc )
#define ISOUSB_StringForIrpMjFunc(  mjfunc ) 


#endif //DBG

#endif // included



