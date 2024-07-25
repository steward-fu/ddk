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

// SCHOOL.c - main module for VxD SCHOOL
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

// This VxD implements an enumerator and device driver for an imaginary bus, SCHOOL.
// The basic idea is that there is a registry entry for the SCHOOL bus under 
// \HKEY_LOCAL_MACHINE\Enum\Root. The registry entry, known as the hardware key,
// has a value named Driver. The data for value Driver is a string that the CONFIGMG
// uses to locate the software key under \HKLM\System\Current\ControlSet\Services\Class.
// This second key is known as the software key. The software key typically has a
// value named DevLoader, the data for which is the name of a VxD.
//
// CONFIGMG loads the VxD specified to be the device loader by the software key. When
// this driver receives control message PNP_NEW_DEVNODE, the parameter to the handler
// indicates it is being called as a device loader. However, like most bus enumerators,
// this example does not load any devices. Instead, it registers as the bus enumerator
// and the device driver for the devnode.
//
// Registering as the enumerator causes CONFIGMG to call back into this VxD's OnEnumerate
// function. When function CM_ENUMERATE is passed, this VxD creates a new devnode for
// the imaginary input device, TELEPATH. From the devnode, the CONFIGMG learns the 
// hardware key for TELEPATH, and from that the software key. The software key identfies
// TELEPATH.VXD as the device loader, enabling it to receive the PNP_NEW_DEVNODE message.
// Like this VxD, TELEPATH defies the instruction of the CONFIGMG, and simply registers
// as the device driver for the devnode instead of actually loading anything. 
//
// When TELEPATH registers as the device driver, it passes the address of its configuation
// handler, OnConfigure. CONFIGMG calls this entry point in TELEPATH, and TELEPATH calls
// services of CONFIGMG to set up its configuration.

#define   DEVICE_MAIN
#include  "school.h"
#undef    DEVICE_MAIN

// Declare the Device Data Block

Declare_Virtual_Device(SCHOOL)

// Prototypes

DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);
DefineControlHandler(PNP_NEW_DEVNODE, OnPnpNewDevnode);
DefineControlHandler(W32_DEVICEIOCONTROL, OnDeviceIoControl);

CONFIGRET CM_HANDLER OnEnumerate(CONFIGFUNC cf, SUBCONFIGFUNC scf, DEVNODE tonode, DEVNODE aboutnode, ULONG flags);
CONFIGRET CM_HANDLER OnArbitrateTelepath(ARBFUNC af, ULONG refdata, DEVNODE devnode,
	NODELIST_HEADER h);

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Global data

REGISTERID arbid;

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

		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);
		ON_PNP_NEW_DEVNODE(OnPnpNewDevnode);
		ON_W32_DEVICEIOCONTROL(OnDeviceIoControl);

	END_CONTROL_DISPATCH

	return TRUE;
}


BOOL OnSysDynamicDeviceInit()
{
	CONFIGMG_Register_Arbitrator(&arbid, ResType_Telepath,
		OnArbitrateTelepath, 0, (DEVNODE)NULL, ARB_GLOBAL);

	return TRUE;
}

BOOL OnSysDynamicDeviceExit()
{
	if (arbid)
		CONFIGMG_Deregister_Arbitrator(arbid, 0);

	return TRUE;
}

DWORD OnDeviceIoControl(PIOCTLPARAMS p)
{
	switch (p->dioc_IOCtlCode)
	{
	
	case SCHOOL_CALLBACK_ENUM:
		dprintf("Issuing CM_Callback_Enumerator call\r\n");
		CONFIGMG_CallBack_Enumerator(OnEnumerate, 0);
		break;
	}
	
	return 0;
}


// ---------------------------------------------------------------------------
// Handler for Control Message PNP_NEW_DEVNODE
//
// The CONFIGMG calls this because of the registry entry under
// HKEY_LOCAL_MACHINE\ENUM\... which identifies this VxD as the
// device loader. Consequently, paramter LoadType is going to be
// DLVXD_LOAD_DEVLOADER. However, instead of loading VxDs (as the
// DDK docs suggest), this driver follows the example of practically
// all other PnP drivers. It does not load any drivers, but instead
// registers as both the enumerator and device driver for the given
// devnode.

CONFIGRET OnPnpNewDevnode(DEVNODE Node, DWORD LoadType)
{
	CONFIGRET code;

	switch (LoadType)
	{

	case DLVXD_LOAD_DEVLOADER /* 1 */:
		code = CONFIGMG_Register_Enumerator(Node, OnEnumerate,
			CM_REGISTER_ENUMERATOR_HARDWARE);

		if (code != CR_SUCCESS)
			return code;

		return CONFIGMG_Register_Device_Driver(Node, NULL, 0,
			CM_REGISTER_DEVICE_DRIVER_REMOVABLE);

	}

	return CR_DEFAULT;
}

// ---------------------------------------------------------------------------
// Enumerator callback
//
// This function is called by the CONFIGMG because it was registered in the
// PNP_NEW_DEVNODE handler.
//
CONFIGRET CM_HANDLER OnEnumerate(
	CONFIGFUNC cf, 
	SUBCONFIGFUNC scf,
	DEVNODE tonode, 
	DEVNODE aboutnode, 
	ULONG flags)
{							// OnEnumerate
	CONFIGRET code;
	ULONG length;			// length of class name
	char class[64];			// device class

	DEVNODE device;			// DEVNODE for device
	LOG_CONF logconf;		// logical configuration
	RES_DES resource;		// resource descriptor handle

	static IRQ_RESOURCE irq = {{0, 0, 0xFFFF, 0}};
	static TELEPATH_RESOURCE telepath = {-1, 0xFF};

#ifdef DEBUG
	char toid[MAX_DEVICE_ID_LEN], aboutid[MAX_DEVICE_ID_LEN];

	CONFIGMG_Get_Device_ID(tonode, toid, sizeof(toid), 0);
	CONFIGMG_Get_Device_ID(aboutnode, aboutid, sizeof(aboutid), 0);

	switch (cf)
	{		   
	case CONFIG_START:
		subfunc = substart[scf];
		break;
	case CONFIG_STOP:
		subfunc = substop[scf];
		break;
	case CONFIG_REMOVE:
		subfunc = subremove[scf];
		break;
	case CONFIG_TEST:
		subfunc = subtest[scf];
		break;
	case CONFIG_APM:
		subfunc = subapm[scf];
		break;
	}		   

	if (cf < NUM_CONFIG_COMMANDS)
		dprintf("SCHOOL ENUMERATOR: %s(%s), to %s, about %s\r\n", lpszConfigName[cf], subfunc, toid, aboutid);
	else
		dprintf("SCHOOL ENUMERATOR: %X(%X), to %s, about %s\r\n", cf, toid, aboutid);
#endif

	switch (cf)
	{						// select on function code

	case CONFIG_SETUP:

		// The very first time we add the device to the hardware
		// tree, CONFIGMG will launch the device installer to load
		// a driver. To avoid having the initial dialog say the
		// device class is "Unknown", force the class in the
		// registry key to be "System" instead. Similarly force
		// the hardware ID so the installer knows which driver to
		// install. This knowledge is similar to what you'd discover
		// from a real Plug and Play device.
		
		length = sizeof(class);

		code = CONFIGMG_Read_Registry_Value(aboutnode, NULL, "Class",
			REG_SZ, class, &length, CM_REGISTRY_HARDWARE);

		if (code == CR_NO_SUCH_VALUE)
		{	
			CONFIGMG_Write_Registry_Value(aboutnode, NULL, "Class",
				REG_SZ, "System", 6, CM_REGISTRY_HARDWARE);

			CONFIGMG_Write_Registry_Value(aboutnode, NULL, "HardwareID",
				REG_SZ, "WCO1234", 6, CM_REGISTRY_HARDWARE);
		}	
		return CR_SUCCESS;


	case CONFIG_ENUMERATE /*5*/:
		
		// Create a DEVNODE for one device. In real life, we'd
		// either use a hardware detection scheme to enumerate
		// devices attached to the bus, or else we'd enumerate
		// a subkey of the registry (e.g., ENUM\SCHOOL) to locate
		// devices.
		
		code=CONFIGMG_Create_DevNode(&device,"SCHOOL\\WCO1234\\0000",tonode, 0);

		if (code == CR_ALREADY_SUCH_DEVNODE)
			return CR_SUCCESS;	// nothing to do on reenumeration

		// Create a logical configuration for our one device.
		// Pretend the device needs an IRQ and one telepathic channel

		CONFIGMG_Add_Empty_Log_Conf(&logconf, device, LCPRI_NORMAL,
			BASIC_LOG_CONF | PRIORITY_EQUAL_LAST);

		CONFIGMG_Add_Res_Des(&resource, logconf, ResType_IRQ,
			&irq, sizeof(irq), 0);

		CONFIGMG_Add_Res_Des(&resource, logconf, ResType_Telepath,
			&telepath, sizeof(telepath), 0);

		return CR_SUCCESS;

	default:
		return CR_DEFAULT;
	}
}	 

///////////////////////////////////////////////////////////////////////
// Telepathic I/O channel resource arbitrator

// SORTNODES sorts the node list into increasing order by flexibility of
// allocation demands.

int bitcount(ULONG mask)
{
	int nbits = 0;
	while (mask)
 	{						// count bits
		if (mask & 1)
			++nbits;
		mask >>= 1;
	}						// count bits

	return nbits;
}

void sortnodes(NODELIST_HEADER h)
{
	PNODELISTITEM node = h->nlh_Head;
	while (node)
	{						// for each node
		RES_DES hres = (RES_DES) node->nl_Test_Req;
		#define pres ((TELEPATH_RESOURCE *) hres)

		node->nl_ulSortDWord = 0;
		while (CONFIGMG_Get_Next_Res_Des(&hres, hres, ResType_Telepath, NULL, 0) == CR_SUCCESS)
			node->nl_ulSortDWord += bitcount(pres->requested);
		node = node->nl_Next;

		#undef pres
	}

	CONFIGMG_Sort_NodeList((NODELIST_HEADER) h, 0);
}


// RELEASE walks through the node list to release assigned resources

void release(NODELIST_HEADER h, PULONG pmap)
{
	PNODELISTITEM node = h->nlh_Head;
	while (node)
  	{						// for each node
		LOG_CONF logconf;

		// Find the allocated configuration (if any) for this devnode. Then loop over
		// its resource descriptors to find the ones for our resource and release
		// the assignment(s)

		if (CONFIGMG_Get_First_Log_Conf(&logconf, (DEVNODE) node->nl_ItsDevNode, ALLOC_LOG_CONF) == CR_SUCCESS)
		{					
			// release this device's channel(s)
			RES_DES hres = (RES_DES) logconf;
			#define pres ((TELEPATH_RESOURCE *) hres)
			while (CONFIGMG_Get_Next_Res_Des(&hres, hres, ResType_Telepath, NULL, 0) == CR_SUCCESS)
				if (pres->allocated >= 0)
					*pmap |= 1 << pres->allocated; // release the one assigned to this log conf
			#undef pres
		}	
 		node = node->nl_Next;
	}		
 }

// ALLOCATE attempts to allocate resources to devices using the requested possibilities
// recorded in the logical configurations.

BOOL nextres(PALLOCPLACE p)
{
	ASSERT(p->node && p->node->nl_Test_Req);

	if (!p->pres)
		p->pres = (TELEPATH_RESOURCE *) p->node->nl_Test_Req;

	while (CONFIGMG_Get_Next_Res_Des((RES_DES*) &p->pres, (RES_DES) p->pres, ResType_Telepath, NULL, 0) != CR_SUCCESS)
	//  while no more descriptors of our resource
	{
		if ((p->node = p->node->nl_Next) == 0)
			return FALSE;		// no more nodes in the list
		p->pres = (TELEPATH_RESOURCE *) p->node->nl_Test_Req;
	}	
	return TRUE;
}

BOOL allocate(PALLOCPLACE p, PULONG pmap)
{
	int channel;				// next channel to try allocating

	if (!nextres(p))
		return TRUE;			// no more of our resource needed, so we've succeeded

	for (p->pres->allocated = -1, channel = 0; channel < 32; ++channel)
 	{
 		// try to allocate a channel
		ALLOCPLACE place = *p;
		ULONG mask = 1 << channel;
		ULONG tempmap = *pmap;

		if (!(tempmap & mask) || !(place.pres->requested & mask))
			continue;			// try next channel because this one isn't free or usable
		tempmap &= ~mask;		// do trial allocation

		if (allocate(&place, &tempmap)) // successful allocation
		{	
			*pmap = tempmap;
			p->pres->allocated = channel;
			return TRUE;
		}					// successful allocation
	}						// try to allocate a channel

	return FALSE;
}							// allocate

// REALLOCATE attempts to reallocate the same resources without regard to which
// alternative possibilities exist

BOOL reallocate(NODELIST_HEADER h, PULONG pmap, BOOL forced)
{							// reallocate
	PNODELISTITEM node = h->nlh_Head;
	
	while (node)
	{						// for each node
		RES_DES hres = (RES_DES) node->nl_Test_Req;
		#define pres ((TELEPATH_RESOURCE *) hres)

		while (CONFIGMG_Get_Next_Res_Des(&hres, hres, ResType_Telepath, NULL, 0) == CR_SUCCESS)
		{					// requires our resource
			ULONG mask;

			ASSERT(pres->allocated >= 0);
			mask = 1 << pres->allocated;
			if ((*pmap & mask) && !forced)
				return FALSE;	// one or more still in use
			*pmap &= ~mask;
		}					// requires our resource
		node = node->nl_Next;

		#undef pres
	}						// for each node
	return TRUE;
}							// reallocate

// OnArbitrateTelepath is the resource arbitrator for telepathic I/O channels

CONFIGRET CM_HANDLER OnArbitrateTelepath(ARBFUNC af, ULONG refdata, DEVNODE devnode,
	NODELIST_HEADER h)
{							// OnArbitrateTelepath
	static ULONG free_map = 0xFF; // real allocation map
	static ULONG free_copy = 0xDEADBEEF; // copy of allocation map

	switch (af)
	{						// select on arbitration function

	// TEST function: Examine each of our resource descriptors in the node list and
	// try to allocated one of the possible choices

	case ARB_TEST_ALLOC:		// af == 0
		{						// ARB_TEST_ALLOC
		ALLOCPLACE place;

		place.node = ((NODELIST_HEADER)h)->nlh_Head;
		place.pres = NULL;

		sortnodes((NODELIST_HEADER) h);
		free_copy = free_map;
		release((NODELIST_HEADER) h, &free_copy);

		if (allocate(&place, &free_copy))
			return CR_SUCCESS;
		else
			return CR_FAILURE;
		}						// ARB_TEST_ALLOC

	// RETEST and FORCE functions: Examine each of our resource descriptors in the node
	// list and allocate the one which is marked allocated in the test config. This
	// differs from TEST by (a) not sorting the node list because it makes no difference,
	// and (b) not even looking at the possible alternative allocations
	
	case ARB_RETEST_ALLOC:		// af == 1
	case ARB_FORCE_ALLOC:		// af == 6
		free_copy = free_map;
		release((NODELIST_HEADER) h, &free_copy);

		if (reallocate((NODELIST_HEADER) h, &free_copy, af == ARB_FORCE_ALLOC))
			return CR_SUCCESS;
		else
			return CR_FAILURE;

	// SET commits the previous trial allocation	
	
	case ARB_SET_ALLOC:			// af == 2
		ASSERT(free_copy != 0xDEADBEEF);
		free_map = free_copy;
		free_copy = 0xDEADBEEF;
		return CR_SUCCESS;

	// RELEASE discards the previous trial allocation
	
	case ARB_RELEASE_ALLOC:		// af == 3
		free_copy = 0xDEADBEEF;
		return CR_SUCCESS;

	// QUERY_FREE returns free resource information in our internal format. The
	// caller has to know what to expect.
	
	case ARB_QUERY_FREE:		// af == 4
		{						// ARB_QUERY_FREE
		struct arbitfree_s *p = (struct arbitfree_s *) h; // ugh!
		p->af_SizeOfInfo = sizeof(ULONG);
		p->af_PointerToInfo = (PVOID*) &free_map;
		return CR_SUCCESS;
		}						// ARB_QUERY_FREE

	case ARB_REMOVE:			// af == 5
		return CR_SUCCESS;

	default:
		return CR_DEFAULT;
	}						// select on arbitration function
}							// OnArbitrateTelepath
