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

// SLAVEDMA.c - main module for VxD SLAVEDMA
// (c) Copyright 1996, Compuware Corporation

// This VxD is a skeleton for a VxD that uses slaved DMA to transfer
// data from a device to memory. This is not really applicable to
// busmaster devices, including PCI initiators.

// The key to setting up DMA is getting physically contiguous memory.
// Although some devices, support scatter/gather, this is not always
// an option, and makes the programming more complicated.

// In this example, the client calls DeviceIoControl requesting a read 
// from the device. The driver tries to lock the memory and determine
// if it is physically contiguous. If not, it uses a DMA buffer allocated
// by the system to do the transfer.

#define   DEVICE_MAIN
#include  "slavedma.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(SLAVEDMA)

DefineControlHandler(SYS_CRITICAL_INIT, OnSysCriticalInit);
DefineControlHandler(DEVICE_INIT, OnDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT, OnSysDynamicDeviceExit);
DefineControlHandler(W32_DEVICEIOCONTROL, OnW32Deviceiocontrol);

BOOL __cdecl ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_SYS_CRITICAL_INIT(OnSysCriticalInit);
		ON_DEVICE_INIT(OnDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);
		ON_W32_DEVICEIOCONTROL(OnW32Deviceiocontrol);

	END_CONTROL_DISPATCH

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
// Global data

DMAHANDLE	hDMA = 0;				// handle of the "virtualized" DMA channel
BOOL		bDeviceBusy = FALSE;	// TRUE when client is using driver
BOOL		bDynamicLoad = FALSE;	// TRUE if VxD is loaded dynamically

////////////////////////////////////////////////////////////////////////
// Global state information for DMA transfer 

BOOL  TransferInProgress;		// TRUE while DMA active
PVOID ClientBuffer;				// address of client buffer to receive data
ULONG PhysAddr;					// physical address to send to DMA controller
DWORD nBytes;					// number of bytes to read
DWORD nPages;					// number of pages spanned by buffer
PVOID DMABufferLinear;			// linear address of the DMA buffer
BOOL  bUsingDMABuffer = FALSE;	// TRUE if not using client buffer
BOOL  bUsingVDMADBuffer = FALSE;// TRUE if using buffer provided by VDMAD
DMABUFFERID bufID=0;			// buffer ID returned by VDMAD (0 means none)
MEMHANDLE hMem;


////////////////////////////////////////////////////////////////////////
// Handler for control message SYS_CRITICAL_INIT
//
// Called at initialization time (Windows start up)
//
BOOL OnSysCriticalInit(VMHANDLE hVM, PCHAR CommandTail, DWORD refData)
{

// At initialization time, reserve DMA buffer space for our device.
// This call cannot be made after INIT_COMPLETE.

	VDMAD_Reserve_Buffer_Space(MAX_TRANSFER_BYTES >> PAGESHIFT, (PVOID)MAX_PHYS_ADDR);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// Handler for control message DEVICE_INIT
//
// Called at initialization time (Windows start up)
//
BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
	bDeviceBusy	= FALSE;
	TransferInProgress = FALSE;

// Claim the channel for our device, and return TRUE if successful

	hDMA = VDMAD_Virtualize_Channel(DMA_CHANNEL_NUMBER, NULL, NULL);
	if (hDMA == 0)
	{
		dprintf("SLAVEDMA: unable to claim DMA channel %d\n", DMA_CHANNEL_NUMBER);
		return FALSE;
	}
	else
		return TRUE;
}

////////////////////////////////////////////////////////////////////////
// Handler for control message SYS_DYNAMIC_DEVICE_INIT
//
// Called if the driver is dynamically loaded.
//
BOOL OnSysDynamicDeviceInit()
{
	bDeviceBusy	= FALSE;
	bDynamicLoad = TRUE;

// Claim the channel for our device, and return TRUE if successful

	hDMA = VDMAD_Virtualize_Channel(DMA_CHANNEL_NUMBER, NULL, NULL);
	if (hDMA == 0)
	{
		dprintf("SLAVEDMA: unable to claim DMA channel %d\n", DMA_CHANNEL_NUMBER);
		return FALSE;
	}
	else
		return TRUE;
}

////////////////////////////////////////////////////////////////////////
// Handler for control message SYS_DYNAMIC_DEVICE_EXIT
//
// Called when the driver is dynamically unloaded.
//
BOOL OnSysDynamicDeviceExit()
{
// We are done with the DMA handle

	if (hDMA)
		VDMAD_Unvirtualize_Channel(hDMA);

	return TRUE;
}


////////////////////////////////////////////////////////////////////////
// Handler for control message W32_DEVICEIOCONTROL
//
// Handles IOCTL requests from Win32 clients
//
DWORD OnW32Deviceiocontrol(PIOCTLPARAMS p)
{
	BOOL  status;
	DWORD error;
	DWORD MaxLockable;

	VMHANDLE hVM = Get_Cur_VM_Handle();

	switch (p->dioc_IOCtlCode)
	{
	case DIOC_OPEN:					// sent on CreateFile

		if (bDeviceBusy)			// allow only one handle at a time
			return DIOC_FAILURE;			
		else
		{
			dprintf("\nSLAVEDMA: New client\n");
			bDeviceBusy = TRUE;
			TransferInProgress = FALSE;
			return DEVIOCTL_NOERROR;
		}

	case DIOC_CLOSEHANDLE:			// sent on CloseHandle or process exit
		dprintf("SLAVEDMA: closing\n");
		bDeviceBusy = FALSE;
		return DEVIOCTL_NOERROR;

// Handle a DMA read request. The parameters are:
//
// 	p->dioc_InBuf		Linear address of user buffer
//	p->dioc_cbInBuf		Number of bytes to read
	
	case DIOC_DMA_READ_FROM_DEVICE:

// Guard against reentrancy

		if (TransferInProgress)
			return DIOC_FAILURE;

		dprintf("\nSLAVEDMA: New transfer initiated\n");

		TransferInProgress = TRUE;

// Initialize state variables		

		bUsingDMABuffer = FALSE;
		bUsingVDMADBuffer = FALSE;
		bufID=0;

		if (p->dioc_cbInBuf == 0) 
		{
			TransferInProgress = FALSE;
			return DEVIOCTL_NOERROR;
		}

		ClientBuffer = p->dioc_InBuf;	// should check for WORD alignment
		nBytes = p->dioc_cbInBuf;
		nPages = PAGENUM((PCHAR)ClientBuffer+nBytes-1) - PAGENUM(ClientBuffer) + 1;

// The client is requesting a DMA transfer to his buffer. In order to
// do this, we have to lock down the memory and get its physical address.

// (In an alternate design, we could have allocated a buffer in the VxD
// during initialization, and passed its linear address up to the client.
// The advantage to doing that is that it saves the operation of locking
// each time there is a read. However, it puts more constraints on the
// application code. If we had taken that approach, this next call would
// be unnecessary, because we would have stored the buffer physical address.)

		status = VDMAD_Lock_DMA_Region(
					ClientBuffer,
					nBytes,
					0,
					&MaxLockable,
					&PhysAddr,
					&error);

// If VDMAD_Lock_DMA_Region returns 0, it means that the buffer could
// not be locked, or was not physically contiguous. In this case, we have
// to use a buffer provided by VDMAD. However, if the VxD was loaded dynamically,
// we have not reserved buffer space with VDMAD, so we have to try to 
// get it with PageAllocate().

		if (status == 0)
		{			
			dprintf("SLAVEDMA: cannot lock client memory, trying for buffer\n");

// If GetPhysicalAddressOfBuffer returns 0, it means that the buffer could
// not be locked, or was not physically contiguous. In this case, we have
// to use a buffer provided by VDMAD. However, if this is a dynamic load
// we have not reserved buffer space with VDMAD, so we have to try to 
// get it with PageAllocate().

			if (bDynamicLoad)
			{
				nPages = ((nBytes-1) >> PAGESHIFT) + 1;
				dprintf("SLAVEDMA: trying to allocated %x pages\n", nPages);
				status=PageAllocate(
						nPages,
						PG_SYS,
						0,
						0xF, // align on 64K boundary
						0,
						MAX_PHYS_ADDR,
						&PhysAddr,
						PAGECONTIG | PAGEFIXED | PAGEUSEALIGN,
						&hMem,
						&DMABufferLinear);

				if (status == FALSE)
				{
					dprintf("SLAVEDMA: cannot get locked memory\n");
					TransferInProgress = FALSE;
					return DIOC_FAILURE;
				}
				else
				{
					dprintf("SLAVEDMA: allocated a buffer with PageAllocate\n");
					bUsingDMABuffer = TRUE;
					bUsingVDMADBuffer = FALSE;
				}
			}
// If we were statically loaded, get the reserved buffer from VDMAD
			else
			{
				status = VDMAD_Request_Buffer(
							ClientBuffer, 
							nBytes, 
							&bufID, 
							(PVOID*)&PhysAddr, 
							&error);

				if (!status)
				{
					// a surprising failure
					dprintf("SLAVEDMA: failed to get buffer from VDMAD\n");
					TransferInProgress = FALSE;
					return DIOC_FAILURE;
				}
				else
				{
					dprintf("SLAVEDMA: allocated a buffer from VDMAD\n");
					bUsingDMABuffer = TRUE;
					bUsingVDMADBuffer = TRUE;
				}
			}
		}
		else
		{
		// The user buffer was locked by VDMAD_Lock_DMA_Region. However, we 
		// should have a check here to make sure that the physical address
		// of the user buffer does not exceed the maximum physical address
		// that the system DMA controller can handle.
			dprintf("SLAVEDMA: Locked client supplied buffer OK\n");
		}

// At this point we have a locked contiguous region of size nBytes at physical
// address PhysAddr. Now we can set up the DMA transfer.

// First, make sure the DMA controller is NOT responding to our device

		VDMAD_Phys_Mask_Channel(hDMA);

// Tell VDMAD about our buffer

		VDMAD_Set_Region_Info(
			hDMA,
			bufID,
			TRUE,
			bUsingDMABuffer ? DMABufferLinear : ClientBuffer,
			nBytes,
			(PVOID)PhysAddr);

// Tell VDMAD to program the DMA controller for the transfer. The 3rd argument is
// the mode. Refer to hardware documentation (8237) to determine the value appropriate
// for your device.

		VDMAD_Set_Phys_State(hDMA, hVM, DEMAND_MODE|WRITEMEM_MODE|INCREMENT_MODE);

// For debug purposes only, call our diagnostic routine:
		Diagnostic(
			DMA_CHANNEL_NUMBER, 
			bUsingDMABuffer ? DMABufferLinear : ClientBuffer,
			nBytes,
			bUsingVDMADBuffer
			);

// Tell VDMAD to allow the DMA controller to respond to DMA requests from the device.

		VDMAD_UnMask_Channel(hVM, hDMA);

// THIS IS WHERE YOU TELL YOUR DEVICE TO START REQUESTING DMA 

// When the DMA controller transfers the requested number of bytes, it asserts
// the TC signal. This tells the device to assert an interrupt that signals the
// end of the transfer. The device may assert the interrupt sooner if it has
// not enough data to transfer. 

// Note also that this example does not take into account requests that the
// DMA controller cannot perform in a single transfer. To handle this requires
// splitting the request in multiple transfers.

//
// Some drivers poll the DMA counter (VDMAD_Get_Phys_Count) to determine if
// the transfer is complete. Others schedule a timeout. Whatever design is
// chosen (interrupt, poll, timeout), there is clean-up to do when it completes. 
// For a rough outline of what has to be done, see OnTransferComplete below.

// A driver can do overlapped I/O by returning -1 here. If so, it must save
// the overlapped structure pointed to by p->dioc_ovrlp so that it can complete
// the transfer later. If the operation is complete when you return, just return
// DEVIOCTL_NOERROR (and set return byte count in dioc struct).

// Just for testing purposes, call the termination handler now:
		OnTransferComplete();

		return DEVIOCTL_NOERROR;

	default:
		return 1;		// failure

	}
}

///////////////////////////////////////////////////////////////////////
// OnTransferComplete
//
// Upon receiving the terminal count interrupt, the driver should
// acknowledge the interrupt on the device and call this routine.
//

VOID OnTransferComplete(void)
{
	DWORD error;

	VDMAD_Phys_Mask_Channel(hDMA);
			
// If we used a buffer (either one we allocated or one provided by VDMAD,
// we have to copy the data from the buffer to the Client space. Unless
// the caller has scheduled a thread event to ensure that we are in the
// context of the original requestor, the client must not pass a buffer
// address in its private arena, i.e., the memory must be in the shared
// (DLL) region.
	
	if (bUsingDMABuffer)
		if (bUsingVDMADBuffer)
		{
			dprintf("SLAVEDMA: copying data from VDMAD buffer and releasing\n");
			VDMAD_Copy_From_Buffer(bufID, ClientBuffer, 0, nBytes, &error);
			VDMAD_Release_Buffer(bufID);
		}
		else
		{
			dprintf("SLAVEDMA: copying data from PageAllocate buffer and releasing\n");
			memcpy(ClientBuffer, DMABufferLinear, nBytes);
			PageFree(hMem, 0);
		}
	else
	{
		dprintf("SLAVEDMA: unlocking client buffer\n");
		VDMAD_Unlock_DMA_Region(ClientBuffer, nBytes);
	}
// if doing overlapped I/O, the driver can signal the client with 
// VWIN32_DIOCCompletion (not callable at interrupt level).

	TransferInProgress = FALSE;
}

WORD getword(DWORD port)
{
	volatile int i;
	WORD hi=0;
	WORD lo=0;

	lo = (WORD)(BYTE)inp(port);
	for (i=0; i < 10000; i++) ; // delay
	hi = (WORD)(BYTE)inp(port);

	return (hi << 8) | lo;
}

////////////////////////////////////////////////////////////////////////////////////
// Diagnostic routine to verify we are set up OK
//
VOID Diagnostic(DWORD channel, PVOID buffer, DWORD nBytes, BOOL bufferUnknown)
{
	DWORD pte;
	DWORD PhysAddr;
	DWORD Count;

	static struct 
	{
		WORD Base;
		WORD Count;
		WORD Page;

	} *pChan, ChannelInfo[] = {
		{0x00,0x01,0x87},			// channel 0
		{0x02,0x03,0x83},			// channel 1
		{0x04,0x05,0x81},			// channel 2
		{0x06,0x07,0x82},			// channel 3
		{0xc0,0xc2,0x8f},			// channel 4 not available
		{0xc4,0xc6,0x8b},			// channel 5
		{0xc8,0xca,0x89},			// channel 6
		{0xcc,0xce,0x8a}			// channel 7
	};

	pChan = &ChannelInfo[channel];

	// read the current physical address off the controller for specified channel
	
	dprintf("page=%04x\n", (BYTE)_inp(pChan->Page));
	dprintf("base=%04x   %04x\n", getword(pChan->Base), getword(pChan->Base)<<1);

	if (channel > 3)
		PhysAddr= ((DWORD)(BYTE)_inp(pChan->Page) << 16) | (getword(pChan->Base) << 1);
	else
		PhysAddr= ((DWORD)(BYTE)_inp(pChan->Page) << 16) | getword(pChan->Base);


	Count = ((DWORD)getword( pChan->Count)+1);
	if (channel > 3) Count <<= 1;	// channels 4-7 do WORD transfers

	dprintf("SLAVEDMA: Current phys address for DMA channel %d is %08x\n",
		channel, PhysAddr);
		
	dprintf("SLAVEDMA: The current count is %04x bytes\n", Count);

	if (! bufferUnknown )
	{
		// get the page table entry for the buffer.

		CopyPageTable(PAGENUM(buffer), 1, (PVOID*)&pte, 0);
		if (! (pte & 1) )
			dprintf("SLAVEDMA: ERROR - the buffer has not been locked\n");

		else if ( PAGENUM(PhysAddr) != PAGENUM(pte) )
			dprintf("SLAVEDMA: ERROR - buffer physical address is %x\n", 
				(pte & ~0xfff) | (PhysAddr & 0xfff) );
		else
			dprintf("SLAVEDMA: buffer start physical address verified OK\n");
	}
	else
		dprintf("SLAVEDMA: cannot verify address of VDMAD supplied buffer\n");

	if ( Count > nBytes)
		dprintf("SLAVEDMA: ERROR - count is incorrect, expected %x\n", nBytes);
	else if ( Count < nBytes) 
		dprintf("SLAVEDMA: Warning - operation will require multiple transfers\n");
	else
		dprintf("SLAVEDMA: transfer count verified OK\n");	
}






















