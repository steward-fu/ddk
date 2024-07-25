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

// MAPDEV.c - main module for VxD MAPDEV
// Copyright (c) 1996 Compuware Corporation

// Various kinds of hardware devices use a memory mapped
// interface, i.e., registers and/or buffers on the device 
// are accessible by reading and writing the processor's
// memory space.

// This example shows how to set up a mapping to a known
// physical address. The VxD makes the device memory available
// to applications, both 16 and 32 bit.

// The input to the VxD, via DeviceIoControl or the Protected
// Mode API entry point, is a physical address and a size (in bytes)
// of the device region to be mapped.

// The output of the VxD is a linear address, and, for 16-bit apps,
// a descriptor that can be used to address region (up to 64kb).
																
#define   DEVICE_MAIN
#include  "mapdev.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(MAPDEV)

DefineControlHandler(W32_DEVICEIOCONTROL, OnW32Deviceiocontrol);

WORD CreateLocalDescriptor(VMHANDLE hVM, PVOID base, DWORD size);

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

////////////////////////////////////////////////////////////
// Device Mapping Function
//
#define PAGENUM(p)  (((ULONG)(p)) >> 12)
#define PAGEOFF(p)  (((ULONG)(p)) & 0xFFF)
#define PAGEBASE(p) (((ULONG)(p)) & ~0xFFF)
#define _NPAGES_(p, k)  ((PAGENUM((char*)p+(k-1)) - PAGENUM(p)) + 1)
//
PVOID MapDevice(PVOID PhysAddress, DWORD SizeInBytes)
{
// There are two ways to map the device memory:
//
// (1) The old 3.1 way, using MapPhysToLinear. This is fine as long
//     the driver is never going to be unloaded:

#ifdef USE_MAP_PHYS_TO_LINEAR

	return MapPhysToLinear(PhysAddress, SizeInBytes, 0);

#else

	PVOID Linear;
	ULONG nPages = _NPAGES_(PhysAddress, SizeInBytes);

//	(2) The Win95 way, which enables the memory to be unmapped	
//	    when the driver unloads, or when the mapping is no longer
//      needed.

	Linear = PageReserve(
		PR_SYSTEM,
		nPages,
		PR_FIXED
		);

	PageCommitPhys(
		PAGENUM(Linear),
		nPages,
		PAGENUM(PhysAddress),
		PC_INCR | PC_WRITEABLE | PC_USER
		);		

	LinPageLock(PAGENUM(Linear), nPages, 0);
	
	return (PVOID) ((ULONG)Linear+PAGEOFF(PhysAddress));

#endif
}

////////////////////////////////////////////////////////////
// Device Unmapping Function
//
//
VOID UnmapDevice(PVOID LinearAddress, DWORD SizeInBytes)
{

#ifdef USE_MAP_PHYS_TO_LINEAR

	// cannot unmap

#else
	LinPageUnLock(
		PAGENUM(LinearAddress),
		_NPAGES_(LinearAddress, SizeInBytes),
		0
		);

	PageDecommit(
		PAGENUM(LinearAddress),
		_NPAGES_(LinearAddress, SizeInBytes),
		0
		);

#endif
}

//--------------------------------------------------------------
// W32_DEVICEIOCONTROL Handler
//
// This function is inovked by 32-bit applications using the 
// Win32 API, DeviceIoControl.
//
// Input:
//	The dioc_InBuf field of the parameter structure contains a
//	pointer to the request structure
//
// Output:
//	Fields of the request structure are updated according to 
//  the request.
//
DWORD OnW32Deviceiocontrol(PIOCTLPARAMS p)
{
	PMAPDEVREQUEST pReq;

	switch (p->dioc_IOCtlCode)
	{
	case DIOC_OPEN:
	case DIOC_CLOSEHANDLE:
		break;

	case MDR_SERVICE_UNMAP:
		pReq = *(PMAPDEVREQUEST*)p->dioc_InBuf;	

		UnmapDevice(
			pReq->mdr_LinearAddress,
			pReq->mdr_SizeInBytes
			);
		pReq->mdr_Status = MDR_STATUS_SUCCESS;
		break;		

	case MDR_SERVICE_MAP:
		pReq = *(PMAPDEVREQUEST*)p->dioc_InBuf;	

		pReq->mdr_LinearAddress = MapDevice(
			pReq->mdr_PhysicalAddress,			// the physical address to map
			pReq->mdr_SizeInBytes				// size of the region
			);									// flags

		if	(pReq->mdr_LinearAddress == NULL) 
			pReq->mdr_Status = MDR_STATUS_ERROR;
		else
			pReq->mdr_Status =  MDR_STATUS_SUCCESS;
		break;

	default:
		return ERROR_INVALID_FUNCTION;
	}

	return DEVIOCTL_NOERROR;
}

//--------------------------------------------------------------
// PM_Api_Handler
//
// This service is inovked by 16-bit applications using the 
// conventional protected mode API mechanism.
//
// Input:
//	client ES:DI	Points to request structure (see mapdev.h)
//
// Output:
//	Fields of the request structure are updated according to 
//  the request.
//
VOID __cdecl PM_Api_Handler(VMHANDLE hVM, PCLIENT_STRUCT pcrs)
{
	PMAPDEVREQUEST  pReq;
	PVOID LinearBase;

	// first get a pointer to the request structure

	pReq = Map_Flat(CLIENT_ES, CLIENT_DI);
	if (pReq == NULL)	// validate
		return;

	// branch on requested service

	switch (pReq->mdr_ServiceID)
	{
	case MDR_SERVICE_MAP:

	// map a device region

		pReq->mdr_LinearAddress = MapDevice(
			pReq->mdr_PhysicalAddress,			// the physical address to map
			pReq->mdr_SizeInBytes				// size of the region
			);


	// verify that the mapping was successful

		if (pReq->mdr_LinearAddress == NULL)
		{
			pReq->mdr_Status = MDR_STATUS_ERROR;
			return;
		}

	// now get a selector

		pReq->mdr_Selector = CreateLocalDescriptor(
								hVM, 
								pReq->mdr_LinearAddress,
								pReq->mdr_SizeInBytes
								);
		if (pReq->mdr_Selector != 0)
			pReq->mdr_Status = MDR_STATUS_SUCCESS;
		else
			pReq->mdr_Status = MDR_STATUS_ERROR;

		break;

	case MDR_SERVICE_UNMAP:
		LinearBase = SelectorMapFlat(
			Get_Cur_VM_Handle(), 		
			(DWORD)pReq->mdr_Selector,
			0
			);

		if (Free_LDT_Selector(hVM, (DWORD)pReq->mdr_Selector,  0))
		{
			UnmapDevice(
				LinearBase,
				pReq->mdr_SizeInBytes
				);
			pReq->mdr_Status = MDR_STATUS_SUCCESS;
		}

		else
			pReq->mdr_Status = MDR_STATUS_ERROR;

		break;

	default:
			pReq->mdr_Status = MDR_STATUS_ERROR;
	}
}

//--------------------------------------------------------------
// CreateDescriptor
//
// This utility routine allocates one PL3 local data descriptor
// for the given base and size. 
//
// Input:
//	hVM		VMHANDLE of calling virtual machine
//	base	base address of descriptor
//	size	size in bytes of region to map with descriptor
//
// Output:
//	Returns the selector for the allocated descriptor, or zero.
//  
//	Note: sizes greater than 64kb are truncated 
//
WORD CreateLocalDescriptor(VMHANDLE hVM, PVOID base, DWORD size)
{
	DWORD DescLo, DescHi, LDTinfo;

	if (size > 0x10000)
		size = 0x10000;

	BuildDescriptorDWORDs(&DescLo, &DescHi, (DWORD)base, size-1, 0xf2, 0, 0);
	return (WORD)Allocate_LDT_Selector(hVM, DescHi, DescLo, 1, 0, &LDTinfo);
}
