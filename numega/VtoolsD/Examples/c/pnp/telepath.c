//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

// TELEPATH.c - main module for VxD TELEPATH
// Copyright (c) 1996, Compuware Corporation 
// Adapted from code written by Walter Oney for
// Microsoft Systems Journal January 1996.
// Reprinted with permission.

// This VxD was developed by Walter Oney Software in conjunction with the article
// "Building Device Drivers That Conform to Windows 95 Plug and Play Standards",
// which appeared in Microsoft Systems Journal, December 1995. The second part of
// the series appeared in MSJ, January 1996. With permission of the author, Compuware
// Corporation has adapted the code to VtoolsD.

// Walter Oney offers regular classes in VxD programming.
// He may be contacted at:  WaltOney@tiac.net

// See SCHOOL.C for an overview of how this VxD works.

#define   DEVICE_MAIN
#include  "telepath.h"
#undef    DEVICE_MAIN

// Declare the Device Data Block

Declare_Virtual_Device(TELEPATH)

// Prototypes 

DefineControlHandler(PNP_NEW_DEVNODE, OnPnpNewDevnode);
CONFIGRET OnConfigure(CONFIGFUNC cf, SUBCONFIGFUNC scf,
	DEVNODE devnode, DWORD refdata, ULONG flags);

// Global data

BYTE irq = 0xff;			// allocated IRQ

// The following declarations define the strings used to identify calls to the driver
// when building for debug

#ifdef DEBUG
DEBUG_CONFIG_NAMES	// macro defines lpszConfigName array
char *subfunc = "";
char *substart[] = {
	"DYNAMIC_START", 
	"FIRST_START"
	};
char *substop[] = {
	"DYNAMIC_STOP", 
	"HAS_PROBLEM"
	};
char *subremove[] = {
	"DYNAMIC", 
	"SHUTDOWN", 
	"REBOOT"
	};
char *subtest[] = {
	"CAN_STOP", 
	"CAN_REMOVE"
	};
char *subapm[] = {
	"TEST_STANDBY", 
	"TEST_SUSPEND",
	"TEST_STANDBY_FAILED", 
	"TEST_SUSPEND_FAILED",
	"TEST_STANDBY_SUCCEEDED", 
	"TEST_SUSPEND_SUCCEEDED",
	"RESUME_STANDBY", 
	"RESUME_SUSPEND", 
	"RESUME_CRITICAL",
	"UI_ALLOWED"
	};
#endif  // DEBUG

// -----------------------------------------------------------------------
// Control message dispatcher

BOOL __cdecl ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_PNP_NEW_DEVNODE(OnPnpNewDevnode);

	END_CONTROL_DISPATCH

	return TRUE;
}

// -----------------------------------------------------------------------
// Handler for control message PNP_NEW_DEVNODE
//
// Input:
//		Node		DEVNODE of the device
//		LoadType	identifies the operation being performed
//
// The registry identifies this VxD as the device loader for the device. When the
// CONFIGMG sends the PNP_NEW_DEVNODE message, this VxD responds by registering
// as the *driver* for the specified devnode.
//
CONFIGRET OnPnpNewDevnode(DEVNODE Node, DWORD LoadType)
{
	switch (LoadType)		// select function to perform
	{						

	case DLVXD_LOAD_DEVLOADER /*2*/:
		return CONFIGMG_Register_Device_Driver(
				Node, 
				OnConfigure, 
				0,
				CM_REGISTER_DEVICE_DRIVER_REMOVABLE | 
					CM_REGISTER_DEVICE_DRIVER_DISABLEABLE
				);
	};		

	return CR_DEFAULT;
}

// -----------------------------------------------------------------------
// Configuration handler
//
// This is the routine that this VxD supplies to CONFIGMG when registering as
// the device driver. It handles various configuration messages sent by CONFIGMG.

CONFIGRET OnConfigure(
	CONFIGFUNC cf, 		// function id
	SUBCONFIGFUNC scf,	// subfunction id
	DEVNODE devnode, 	// device node being configured
	DWORD refdata, 		// context information (function specific)
	ULONG flags			// function specific flags
)
{
	CMCONFIG config;
	LOG_CONF logconf;
	RES_DES hres;

// If this is the debug build, emit messages to debug console
#ifdef DEBUG
	char id[MAX_DEVICE_ID_LEN];

	switch (cf)
	{						// get subfunction name
	case CONFIG_START:
		subfunc = substart[scf]; 	break;
	case CONFIG_STOP:
		subfunc = substop[scf];		break;
	case CONFIG_REMOVE:
		subfunc = subremove[scf];	break;
	case CONFIG_TEST:
		subfunc = subtest[scf];		break;
	case CONFIG_APM:
		subfunc = subapm[scf];		break;
	}						

	CONFIGMG_Get_Device_ID(devnode, id, sizeof(id), 0);
	if (cf < NUM_CONFIG_COMMANDS)
		dprintf("TELEPATH DRIVER: %s(%s), %s\r\n", lpszConfigName[cf], subfunc, id);
	else
		dprintf("TELEPATH DRIVER: %X(%X), %s\r\n", cf, scf, id);
#endif

	switch (cf)					// branch on function code
	{		
	case CONFIG_START:

		// This is the normal way to obtain a configuration that
		// only includes the standard resources:
		
		CONFIGMG_Get_Alloc_Log_Conf(&config, devnode, CM_GET_ALLOC_LOG_CONF_ALLOC);
		irq = config.bIRQRegisters[0];

		// This is how to get a configuration that includes a
		// private resource:

		CONFIGMG_Get_First_Log_Conf(&logconf, devnode, ALLOC_LOG_CONF);
		if (CONFIGMG_Get_Next_Res_Des(&hres, (RES_DES) logconf,
			ResType_Telepath, NULL, 0) == CR_SUCCESS)
		{						
			TELEPATH_RESOURCE* pres = (TELEPATH_RESOURCE*) hres;
			// do something with pres->allocated
		}						

		// Here we would initialize the device, doing things
		// like VPICD_Virtualize_IRQ, etc.

		return CR_SUCCESS;


	case CONFIG_REMOVE /* 4 */:
	case CONFIG_STOP   /* 2 */:
		
		// Here we would disable the device and do things
		// like unvirtualizing our IRQ
		
		irq = 0xff;
		return CR_SUCCESS;

	default:
		return CR_DEFAULT;
	}			  
}				  
