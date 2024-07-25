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

// PCICFG.c - main module for VxD PCICFG
// Copyright (c) 1995-1998 Compuware Corporation

// This is a simple vxd that retrieves the PCI config data for a specified device.
// Its DeviceIoControl handler accepts a vendor and device ID, and it returns the
// the config info for that device, if it is found.

// The driver searches the hardware tree for PCI devices. For each one that it finds,
// it reads the config space to check for a matching vendor/device id pair. If found,
// it returns the information.

// Note that this is limited to searching for one instance of a given device.

// Normal VxD preliminaries
#define   DEVICE_MAIN
#include  "pcicfg.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(PCICFG)

DefineControlHandler(W32_DEVICEIOCONTROL, OnW32Deviceiocontrol);


// Global data for tree walk / search
WORD SearchVendor;		// vendor id to search for
WORD SearchDevice;		// device id to search for
CHAR HwId[128];			// scratch buf for hardware id
DEVNODE theDevnode;		// holds devnode of the device if found


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

 		ON_W32_DEVICEIOCONTROL(OnW32Deviceiocontrol);

	END_CONTROL_DISPATCH

	return TRUE;
}

// Device IoControl handler
DWORD OnW32Deviceiocontrol(PIOCTLPARAMS p)
{
	WORD DeviceId;
	WORD VendorId;
	DEVNODE dn;
	PCI_CONFIG_HEADER_0* pcicfg;
	CONFIGRET status;

	switch (p->dioc_IOCtlCode)
	{
	case DIOC_OPEN:
	case DIOC_CLOSEHANDLE:
		return DEVIOCTL_NOERROR;

	case IOCTL_PCICFG_GETCFG:
		VendorId = ((WORD*)(p->dioc_InBuf))[0];
		DeviceId = ((WORD*)(p->dioc_InBuf))[1];

		pcicfg = (PCI_CONFIG_HEADER_0*)p->dioc_OutBuf;

		dn = LocatePCIDevice(VendorId, DeviceId);
		if ( dn  == 0 )
			*p->dioc_bytesret = 0;

		else
		{
		 	status = GetPCIConfigData(dn, 0, (ULONG*)pcicfg, sizeof(PCI_CONFIG_HEADER_0));
			if (status != CR_SUCCESS)
				*p->dioc_bytesret = 0;

			else
				*p->dioc_bytesret = sizeof(PCI_CONFIG_HEADER_0);
		}
		return DEVIOCTL_NOERROR;

	default:
		return 1; // error
	}
}


// typedef for hw tree walk callback
typedef VOID (*TREEWALKCALLBACK)(DEVNODE DevNode);


// this routine is called for each devnode in the hardware tree when we walk the tree
VOID CheckNode(DEVNODE Node)
{
	CONFIGRET cr;
	WORD thisVendor;
	WORD thisDevice;

	// get the hardware id for this node
	cr = CONFIGMG_Get_Device_ID(Node, HwId, sizeof(HwId), 0);
	if ( cr != CR_SUCCESS )
		return;

	// see if it's a PCI device
	if (strncmp(HwId, "PCI", 3) == 0)
	{
		// if so, see if it's a match
		if (  (GetPCIConfigData(Node, 0, (PDWORD)&thisVendor, 2) == CR_SUCCESS) &&
			  (thisVendor == SearchVendor) &&
			  (GetPCIConfigData(Node, 2, (PDWORD)&thisDevice, 2) == CR_SUCCESS) &&
			  (thisDevice == SearchDevice) 
		   )
			theDevnode = Node;
	}
}


// This function drives the hardware tree walk
VOID WalkHardwareTree(DEVNODE Node, TREEWALKCALLBACK NodeCallback)
{
	DEVNODE Child, Sibling;

	// if paramter Node is NULL, the caller is initializing the
	// walk. To initialize the walk, we get the root devnode by
	// calling CONFIGMG_Locate_DevNode with NULL as the hardware ID.	

	if (Node == 0)
	{
		CONFIGMG_Locate_DevNode(&Node, NULL, 0);		
		// If the root is not found, something is very wrong!
		// Terminate the attempted walk by returning.		
		if (Node == 0)
			return;	
	}

	// At this point we have a devnode, so let the callback Know about it.
	NodeCallback(Node);		
	
	// Now see if it has any children. If not, just return.

	if ( CONFIGMG_Get_Child(&Child, Node, 0) != CR_SUCCESS)		
		return;	
	else	
	{
	// The node passed in has at least one child. Walk this subtree.
		WalkHardwareTree(Child, NodeCallback);	// Now walk subtree of any siblings.
		while ( CONFIGMG_Get_Sibling(&Sibling, Child, 0) == CR_SUCCESS )
		{
			WalkHardwareTree(Sibling, NodeCallback);
			Child = Sibling;
		}	
	}
} 


// This function is called to find a devnode with the given ids
DEVNODE LocatePCIDevice(WORD VendorId, WORD DeviceId)
{
	theDevnode = 0;
	SearchVendor = VendorId;
	SearchDevice = DeviceId;

	WalkHardwareTree(0, CheckNode);	
	return theDevnode;
}


// This function calls the PCI driver to read the config space
CONFIGRET GetPCIConfigData(
	DEVNODE Devnode, 
	DWORD 	StartByteOffset, 
	DWORD* 	ConfigDataBuffer, 
	ULONG 	nBytes)
{
	return 
		CONFIGMG_Call_Enumerator_Function(
			Devnode, 
			PCI_ENUM_FUNC_GET_DEVICE_INFO,
			StartByteOffset,
			ConfigDataBuffer,
			nBytes,
			0);
}

// This function calls the PCI driver to write the config space
CONFIGRET SetPCIConfigData(
	DEVNODE Devnode, 
	DWORD 	StartByteOffset, 
	DWORD* 	ConfigDataBuffer, 
	ULONG 	nBytes)
{
	return 
		CONFIGMG_Call_Enumerator_Function(
			Devnode, 
			PCI_ENUM_FUNC_SET_DEVICE_INFO,
			StartByteOffset,
			ConfigDataBuffer,
			nBytes,
			0);
}

