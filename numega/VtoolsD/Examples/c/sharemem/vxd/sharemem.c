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

// sharemem.c - main module for VxD sharemem
// Copyright (c) 1998 Compuware Corporation

#define   DEVICE_MAIN
#include  "sharemem.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(SHAREMEM)

DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);
DefineControlHandler(W32_DEVICEIOCONTROL, OnW32Deviceiocontrol);

///////////////////////////////////////////////////////////////////////////////
// Store the mapped address and size of the buffer in application space
//
PCHAR MappedAppBuffer = 0;
DWORD SizeOfAppBuffer = 0;
BOOL bCurrentlyMapped = FALSE;

///////////////////////////////////////////////////////////////////////////////
// Control Message dispatch routine
//
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

///////////////////////////////////////////////////////////////////////////////
// Map the application level buffer to a process independent address
//
// Input:
//		AppPointer		Process specific address of the buffer
//		Size			Size in bytes of the app level buffer
//
// Returns:
//		Returns a process independent address for the buffer

PVOID MapApplicationBuffer(PVOID AppPointer, DWORD Size)
{
	DWORD StartPage, EndPage, PagesSpanned;
	PVOID GlobalAddressOfStartPage;
	PVOID GlobalAddressOfBuffer;

// First calculate the page numbers on which the supplied buffer resides	

	StartPage = (DWORD)AppPointer >> 12;
	EndPage = ( (DWORD)AppPointer + Size - 1 ) >> 12;

// Calculate the number of pages that must be locked/translated

	PagesSpanned = EndPage - StartPage + 1;

// Lock and remap the pages

	GlobalAddressOfStartPage = (PVOID)LinPageLock(StartPage, PagesSpanned, PAGEMAPGLOBAL);

// Verify success

	if ( GlobalAddressOfStartPage == 0 )
		return NULL;

// Calculate the buffer address by adding the page offset

	GlobalAddressOfBuffer = (PVOID) (
		  (DWORD)GlobalAddressOfStartPage + 
		( (DWORD)AppPointer & 0xFFF )
		);

// Return the translated address

	return GlobalAddressOfBuffer;
}

///////////////////////////////////////////////////////////////////////////////
// Unmap a previously mapped buffer
//
// Input:
//		GlobalPointer	Process independent address of the mapped buffer
//		Size			Size in bytes of the buffer
//
VOID UnmapApplicationBuffer(PVOID GlobalAddressOfBuffer, DWORD Size)
{
	DWORD StartPage, EndPage, PagesSpanned;

	StartPage = (DWORD)GlobalAddressOfBuffer >> 12;
	EndPage = ( (DWORD)GlobalAddressOfBuffer + Size - 1 ) >> 12;
	PagesSpanned = StartPage - EndPage + 1;

	LinPageUnLock(StartPage, PagesSpanned, PAGEMAPGLOBAL);
}


///////////////////////////////////////////////////////////////////////////////
// DeviceIoControl Handler
//
//
DWORD OnW32Deviceiocontrol(PIOCTLPARAMS p)
{
	MAP_BUFFER_ARGS* pMapBufferArgs;

	switch ( p->dioc_IOCtlCode )
	{
	case DIOC_OPEN:
		nprintf("The SHAREMEM driver has been opened.\n");
		return DEVIOCTL_NOERROR;

	case DIOC_CLOSEHANDLE:
		if ( bCurrentlyMapped )
		{
			bCurrentlyMapped = FALSE;
			nprintf("Unmapping the buffer.\n");
			UnmapApplicationBuffer(MappedAppBuffer, SizeOfAppBuffer);
		}

		nprintf("The SHAREMEM driver has been closed.\n");
		return DEVIOCTL_NOERROR;
		
	case SHAREMEM_IOCTL_MAP_BUFFER:
		*p->dioc_bytesret = 0;

		if ( bCurrentlyMapped )
		{
			nprintf("Attempt to map when already mapped to another buffer\n");
			return 1;	// error - already mapped
		}

		pMapBufferArgs =  (MAP_BUFFER_ARGS*)p->dioc_InBuf;
	
		ASSERT ( p->dioc_cbInBuf >= sizeof(MAP_BUFFER_ARGS) );
		
		MappedAppBuffer = MapApplicationBuffer(pMapBufferArgs->m_address, pMapBufferArgs->m_size);

		if ( p->dioc_cbOutBuf >= sizeof(PVOID) )
			*(PVOID*)p->dioc_OutBuf = MappedAppBuffer;

		nprintf("App buffer at %08x size %x bytes mapped to global address %08x\n",
			pMapBufferArgs->m_address, pMapBufferArgs->m_size, MappedAppBuffer);

		if ( MappedAppBuffer == NULL )
			return 1;  // error - could not map the buffer

		*p->dioc_bytesret = sizeof(PVOID);

		if ( *MappedAppBuffer ) 
			nprintf("String at global address: |%s|\n", MappedAppBuffer);

		bCurrentlyMapped = TRUE;

		return DEVIOCTL_NOERROR;


	case SHAREMEM_IOCTL_UNMAP_BUFFER:
		if ( ! bCurrentlyMapped )
		{
			nprintf("Attempt to unmap when not mapped.\n");
			return 1;	// error - already mapped
		}

		nprintf("Unmapping the buffer\n");

		UnmapApplicationBuffer(MappedAppBuffer, SizeOfAppBuffer);

		bCurrentlyMapped = FALSE;

		return DEVIOCTL_NOERROR;
	}


	return 1;	// Unsupported function
}
