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

// HOTPCI.C - main module for VxD HOTPCI
//   Copyright (c) 1998 Compuware Corporation
//   Copyright (c) 1998 Virtual Computer Corporation
//   Copyright (c) 1998 Xilinx


// This is a driver for the Hardware Object Technology (HOT II) board from
// Virtual Computer Corporation, which is based on the Xilinx XC4000 series
// FPGA for PCI interfacing.

// This is a Plug and Play (PnP) driver. The driver must be installed via
// an appropriate INF file. Once associated with the boards Vendor/Device
// IDs, the driver is loaded automatically by the system when the board
// is detected. The system invokes the OnPnpNewDevnode entry point, which
// initiates the PnP initialization of the board.

// The driver creates a new DDB for each detected instance of the board. This 
// enables application code to open separate handles for each unit, simply
// by providing a different device name, e.g. "\\\\.\\HOTII_0", "\\\\.\\HOTII_1",
// etc. This also provides compatibility with Windows NT and WDM. The instancing 
// functionality is based on module device.c, which is independent of the
// XC4000/PCI architecure.

// The driver supports the same set of IOCTL (DeviceIoControl) functions as
// does the corresponding NT/WDM driver.

#define   DEVICE_MAIN
#include  "hotpci.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(HOTPCI)

// Globals

DWORD		Unit=0;		// tracks how many units exist
VMHANDLE	hSystemVM;	// handle of the system virtual machine

///////////////////////////////////////////////////////////////////////////
// This is the control message dispatcher for the main driver (DDB).
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

		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);
		ON_SYS_DYNAMIC_DEVICE_EXIT(OnSysDynamicDeviceExit);
		ON_PNP_NEW_DEVNODE(OnPnpNewDevnode);

	END_CONTROL_DISPATCH

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
// This is the control message dispatcher that is used for each instance
// of the HOT II board that is present in the system. The driver creates
// a new DDB (see CreateDeviceUnit) for each board detected and reported
// to OnPnpNewDevnode. Only DeviceIoControl is handled. This routine is
// always invoked by the DEVICE_UNIT thunk, which passes a pointer to 
// itself as parameter pUnit.
//
ULONG __stdcall UnitCtrlMsgDispatcher(
	ULONG Edi,
	ULONG Esi,
	ULONG Ebp,
	ULONG Esp,
	ULONG Ebx,
	ULONG Edx,
	ULONG Ecx,
	ULONG ControlMessage,
	PDEVICE_UNIT pUnit,
	ULONG* pCarryBitReturn
	)
{
	switch (ControlMessage)
	{
	case W32_DEVICEIOCONTROL:
		return DeviceControlHandler((HOTINFO*)pUnit, (PIOCTLPARAMS)Esi);

	default:
		*pCarryBitReturn = 0;
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
// OnSysDynamicDeviceInit
//
// The system calls this when the VxD is first loaded. It just stores the
// system VM handle. 
//
BOOL OnSysDynamicDeviceInit(void)
{
	DPRINTF( ("HotPCI driver now initializing\n") );
	hSystemVM = Get_Sys_VM_Handle();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// OnSysDynamicDeviceExit
//
// The system calls this when the VxD is unloaded.
//
BOOL OnSysDynamicDeviceExit(void)
{
	DPRINTF( ("HotPCI driver exiting\n") );
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// OnPnpNewDevnode
// 
// The system calls this when the PCI bus driver enumerates the function. 
//
CONFIGRET OnPnpNewDevnode(DEVNODE Node, DWORD LoadType)
{  
	HOTINFO* pUnit;  
	CHAR UnitName[9];
	BOOL Result;

	DPRINTF( ("OnPnpNewDevnode called, devnode=%x\n", Node) );
  
	switch(LoadType)
	{

	case DLVXD_LOAD_DEVLOADER : DPRINTF( ("DLVXD_LOAD_DEVLOADER\n") );

		// create a data structure for this unit
		memset(UnitName, 0, sizeof(UnitName));
		sprintf(UnitName,"HOTII_%d", Unit);
		Unit++;
		pUnit = (HOTINFO*)CreateDeviceUnit(
					UnitName, 
					sizeof(HOTINFO),
					UnitCtrlMsgDispatcher,
					Node, 
					&Result
					);

		DPRINTF( ("Created Device Unit %s at %x\n", UnitName, pUnit) );

		
		if ( (pUnit != NULL) && Result ) 
		{
			SaveOriginalConfiguration(pUnit);

			return CONFIGMG_Register_Device_Driver(
				Node, 
				OnConfigure, 
				(DWORD)pUnit, 
				CM_REGISTER_DEVICE_DRIVER_REMOVABLE | CM_REGISTER_DEVICE_DRIVER_DISABLEABLE
				);
		}

		else
			return CR_OUT_OF_MEMORY;
	}

	return CR_DEFAULT;
}


////////////////////////////////////////////////////////////////////////////////
// OnConfigure
//
// Plug and Play callback. The system notifies the driver of plug and play events
// by calling this function.
//
CONFIGRET OnConfigure(
		CONFIGFUNC cf, 
		SUBCONFIGFUNC scf, 
		DEVNODE devnode, 
		DWORD refdata, 
		ULONG flags
		)
{
	CMCONFIG config;
	BOOL result;
	
	// The DEVICE_UNIT was set up as the RefData in the call to CONFIGMG_RegisterDeviceDriver.

	HOTINFO* pUnit = (HOTINFO*)refdata;

	switch (cf)
	{
	case CONFIG_START:
		DPRINTF( ("CONFIG_START, unit=%x\n", pUnit) );
	
    // get information about the Win95 P&P configuration assigned to 
    // the board

		CONFIGMG_Get_Alloc_Log_Conf(&config, devnode, CM_GET_ALLOC_LOG_CONF_ALLOC);
    
		pUnit->birq = config.bIRQRegisters[0];
		pUnit->wiobase = config.wIOPortBase[0];
		pUnit->wiolength = config.wIOPortLength[0];
		pUnit->dmembase = config.dMemBase[0];
		pUnit->dmemlength = config.dMemLength[0];

	// Map the board's memory

		if ( MapMemory(pUnit) == NULL )
			return CR_FAILURE;

	// Hook the interrupt

		if ( HookInterrupt(pUnit, &config) != CR_SUCCESS )
		{
			UnmapMemory(pUnit);
			return CR_FAILURE;
		}		

		return CR_SUCCESS;

	case CONFIG_STOP :
		DPRINTF( ("CONFIG_STOP, unit=%x\n", pUnit) );
		UnhookInterrupt(pUnit);
		UnmapMemory(pUnit);
		CloseEventHandle(pUnit);
		break;

	case CONFIG_REMOVE :
		DPRINTF( ("CONFIG_REMOVE, unit=%x\n", pUnit) );
		UnhookInterrupt(pUnit);
		UnmapMemory(pUnit);
		CloseEventHandle(pUnit);

		DestroyDeviceUnit(&pUnit->Unit);
	    return CR_SUCCESS;

	case CONFIG_FILTER:
	case CONFIG_TEST:
	case CONFIG_ENUMERATE:
	case CONFIG_SETUP:
	case CONFIG_CALLBACK:
	case CONFIG_APM:
	case CONFIG_TEST_FAILED:
	case CONFIG_TEST_SUCCEEDED:
	case CONFIG_VERIFY_DEVICE:
	case CONFIG_PREREMOVE:
	case CONFIG_SHUTDOWN:
	case CONFIG_PREREMOVE2:
	case CONFIG_READY:
	case CONFIG_PROP_CHANGE:
	case CONFIG_PRIVATE:
	case CONFIG_PRESHUTDOWN:
#ifdef WIN98
	case CONFIG_BEGIN_PNP_MODE:
	case CONFIG_LOCK:
	case CONFIG_UNLOCK:
	case CONFIG_IRP:
	case CONFIG_WAKEUP:
	case CONFIG_WAKEUP_CALLBACK:
#endif
    	return CR_DEFAULT;

	default:
		DPRINTF( ("OnConfigure, unit=%x unhandled function=%d\n", pUnit, cf) );
    	return CR_FAILURE;
	}
}

////////////////////////////////////////////////////////////////////////////////
// DeviceControlHandler
//
// This is the DeviceIoControl handler for a DEVICE_UNIT. This is called from
// the unit ctrl msg dispatcher.
//
DWORD DeviceControlHandler(HOTINFO* pUnit, PIOCTLPARAMS p)
{
	CONFIGRET cr;

    p->dioc_bytesret = 0;

	switch (p->dioc_IOCtlCode)
    {
	case DIOC_OPEN :		// createfile
		pUnit->OpenCount++;
		return DEVIOCTL_NOERROR;

	case DIOC_CLOSEHANDLE : // file closed
		pUnit->OpenCount--;
		return DEVIOCTL_NOERROR;

	case DIOC_BASE_ADDRESSES:
		return GetBaseAddresses(pUnit, (struct HOTII_GET_BASE_ARGS*)p->dioc_OutBuf);

	case DIOC_WRITE:      // Burst Memory Write
		return WriteMemory( pUnit, (struct HOTII_READ_WRITE_MEMORY_ARGS*)p->dioc_InBuf, p->dioc_OutBuf);

	case DIOC_READ:       // Burst Memory Read
		return ReadMemory( pUnit, (struct HOTII_READ_WRITE_MEMORY_ARGS*)p->dioc_InBuf, p->dioc_OutBuf);

	case DIOC_IO_WRITE:	// I/O Write
		return WriteIo( pUnit, (struct HOTII_READ_WRITE_IO_ARGS*)p->dioc_InBuf, p->dioc_OutBuf);

	case DIOC_IO_READ:	// I/O Read
		return ReadIo( pUnit, (struct HOTII_READ_WRITE_IO_ARGS*)p->dioc_InBuf, p->dioc_OutBuf);

	case DIOC_REWRITE_BARS: // Rewrite original bars and interrupt line
		return ReloadOriginalConfiguration(pUnit);

	case DIOC_INT_EN:     // Enable notification of interrupt
		return EnableIntrNotify(pUnit, (struct HOTII_INTERRUPT_ENABLE_ARGS*)p->dioc_InBuf);

	case DIOC_INT_DI:     // Disable notification of interrupt
		return DisableIntrNotify(pUnit);

	case DIOC_GETDEVINFO: // Get PCI Configuration Information
		cr = ReadPciConfig(pUnit, 0, sizeof(PCICONFIG), p->dioc_OutBuf);
		*p->dioc_bytesret = sizeof(PCICONFIG);
	
		if ( cr == CR_SUCCESS )
			return DEVIOCTL_NOERROR;
		else
			return cr;
		break;

	default:
		return IOCTL_ERROR;
		break;
	};

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// HookInterrupt
//
// Hook the unit's interrupt to the ISR
//
CONFIGRET HookInterrupt(HOTINFO* pUnit,  CMCONFIG* pConfig)
{
	struct VPICD_IRQ_Descriptor irqdes;

	memset(&irqdes, 0, sizeof(irqdes));

	// initialize the structure containing the hook information
	irqdes.VID_IRQ_Number = pUnit->birq;
	irqdes.VID_Options = VPICD_OPT_SHARE_PMODE_ONLY | VPICD_OPT_REF_DATA;
	irqdes.VID_Hw_Int_Proc = (DWORD)VPICD_Thunk_HWInt_Ex(TheIsr, &pUnit->IsrThunk);

	// set the reference data field so that when the interrupt occurs, we
	// know which device is interupting.
	irqdes.VID_Hw_Int_Ref = pUnit;

	// Try to hook it
	pUnit->hIrq = VPICD_Virtualize_IRQ(&irqdes);
	if (pUnit->hIrq == 0)
	  return CR_CANT_SHARE_IRQ;

	// make sure it is unmasked at the PIC
	VPICD_Physically_Unmask(pUnit->hIrq);

	return CR_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// UnhookInterrupt
//
// Unhook the unit's interrupt.
//
VOID UnhookInterrupt(HOTINFO* pUnit)
{
	if ( pUnit->hIrq != 0 )
	{
		VPICD_Physically_Mask(pUnit->hIrq);
		VPICD_Force_Default_Behavior(pUnit->hIrq);
		pUnit->birq = 0;
		pUnit->hIrq = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////
// MapMemory
//
// Map the physical memory of the unit into the system and user address space.
// Returns the mapped virtual address.
//
PVOID MapMemory(HOTINFO* pUnit)
{
	DWORD PageNumber0;
	DWORD PageNumberN;
	DWORD nPages;
	BOOL bCommitOk;

	// Page number of the first byte of the region	
	PageNumber0 = pUnit->dmembase >> 12;

	// Page number of the last byte of the region	
	PageNumberN = (pUnit->dmembase + pUnit->dmemlength - 1) >> 12;

	// Total pages spanned by the board
	nPages = PageNumberN - PageNumber0 + 1;

	// allocate nPages of virtual address space
	pUnit->pSysLinearAddress = PageReserve(PR_SHARED, nPages, 0);

	// bail out if the address space was not allocated
	if ( pUnit->pSysLinearAddress == (PVOID)(-1) )
		return NULL;

	// add page offset to base virtual address (usually zero)
	pUnit->pSysLinearAddress = (PVOID)
		((DWORD)pUnit->pSysLinearAddress + ((DWORD)pUnit->pSysLinearAddress & (PAGESIZE-1)));


	//  commit the board's physical memory to back the allocated virtual space
	bCommitOk = PageCommitPhys(
		(DWORD)pUnit->pSysLinearAddress >> 12,
		nPages,
		pUnit->dmembase >> 12,
		PC_INCR|PC_WRITEABLE|PC_USER
		);

	// return the mapped address if successful. The address is also usable by
	// applications.
	if ( bCommitOk )
		return pUnit->pSysLinearAddress;
	else
	{
		pUnit->pSysLinearAddress = 0;
		return NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
// UnmapMemory
//
// Unmap the boards memory
//
VOID UnmapMemory(HOTINFO* pUnit)
{
	DWORD PageNumber0;
	DWORD PageNumberN;
	DWORD nPages;

	// if the board's memory has been mapped, decommit and free the virtual space
	
	if ( pUnit->pSysLinearAddress != 0 )
	{

		PageNumber0 = pUnit->dmembase >> 12;
		PageNumberN = (pUnit->dmembase + pUnit->dmemlength - 1) >> 12;
		nPages = PageNumberN - PageNumber0 + 1;

		PageDecommit((DWORD)pUnit->pSysLinearAddress >> 12,	nPages,	0);

		PageFree( (MEMHANDLE) (DWORD)pUnit->pSysLinearAddress & ~(PAGESIZE-1), 0);
		pUnit->pSysLinearAddress = 0;
	}
}


////////////////////////////////////////////////////////////////////////////////
// GetBaseAddresses
//
// Return the i/o base address and the memory base address of the unit
//
DWORD GetBaseAddresses(HOTINFO* pUnit, struct HOTII_GET_BASE_ARGS* pBases)
{
	pBases->IoAddress = (PVOID)pUnit->wiobase;
	pBases->MemAddress = (PVOID)pUnit->dmembase;

	return DEVIOCTL_NOERROR;
}


////////////////////////////////////////////////////////////////////////////////
// ReadMemory
//
// Read from the board's memory space
// 
// Input:
//	pUnit			device unit from which to read
//  pRwArgs			pointer to count/offset structure
//  ClientBUffer	destination of read
//
DWORD ReadMemory( HOTINFO* pUnit, struct HOTII_READ_WRITE_MEMORY_ARGS* pRwArgs, PVOID ClientBuffer)
{
	DWORD ByteCount = pRwArgs->DwordCount*sizeof(DWORD);
	PVOID Buffer = (PVOID) ((PBYTE)pUnit->pSysLinearAddress + pRwArgs->ByteOffset);

	// Verify that the read is within the bounds of the region

	if ( (pUnit->pSysLinearAddress == NULL) ||
		 ((pRwArgs->ByteOffset + ByteCount) > pUnit->dmemlength) )
		 return IOCTL_ERROR;

	memcpy(ClientBuffer, Buffer, ByteCount);

	return DEVIOCTL_NOERROR;
}


////////////////////////////////////////////////////////////////////////////////
// WriteMemory
//
// Write to the board's memory space
// 
// Input:
//	pUnit			device unit to which to write
//  pRwArgs			pointer to count/offset structure
//  ClientBUffer	source for data to be written
//
DWORD WriteMemory( HOTINFO* pUnit, struct HOTII_READ_WRITE_MEMORY_ARGS* pRwArgs, PVOID ClientBuffer)
{
	DWORD ByteCount = pRwArgs->DwordCount*sizeof(DWORD);
	PVOID Buffer = (PVOID) ((PBYTE)pUnit->pSysLinearAddress + pRwArgs->ByteOffset);

	// Verify that the write is within the bounds of the region

	if ( (pUnit->pSysLinearAddress == NULL) ||
		 ((pRwArgs->ByteOffset + ByteCount) > pUnit->dmemlength) )
		 return IOCTL_ERROR;

	memcpy(Buffer, ClientBuffer, ByteCount);

	return DEVIOCTL_NOERROR;
}

////////////////////////////////////////////////////////////////////////////////
// ReadIo
//
// Read from the board's I/O space
//
DWORD ReadIo( HOTINFO* pUnit, struct HOTII_READ_WRITE_IO_ARGS* pRwArgs, PVOID ClientBuffer)
{
	pRwArgs->Data = _inpd((WORD)(pUnit->wiobase+pRwArgs->PortOffset));
	return DEVIOCTL_NOERROR;
}

////////////////////////////////////////////////////////////////////////////////
// WriteIo
//
// Write to the board's I/O space
//
DWORD WriteIo( HOTINFO* pUnit, struct HOTII_READ_WRITE_IO_ARGS* pRwArgs, PVOID ClientBuffer)
{
	outpd((WORD)pUnit->wiobase+pRwArgs->PortOffset, &pRwArgs->Data);
	return DEVIOCTL_NOERROR;
}

////////////////////////////////////////////////////////////////////////////////
// SaveOriginalConfiguration
//
// Save registers in the PCI Config space of a unit, which are cleared when the
// board is reset or reconfigured.
//
VOID SaveOriginalConfiguration(HOTINFO* pUnit)
{
	ReadPciConfig(pUnit, FIELDOFFSET(PCICONFIG, BaseAddr0), sizeof(DWORD), &pUnit->OriginalBar0);
	ReadPciConfig(pUnit, FIELDOFFSET(PCICONFIG, BaseAddr1), sizeof(DWORD), &pUnit->OriginalBar1);
	ReadPciConfig(pUnit, FIELDOFFSET(PCICONFIG, IntLine), sizeof(BYTE), &pUnit->OriginalIntLine);
}

////////////////////////////////////////////////////////////////////////////////
// ReloadOriginalConfiguration
//
// Reloads saved PCI Config registers after a reset or reconfiguration.
//
DWORD ReloadOriginalConfiguration(HOTINFO* pUnit)
{
	WORD Three = 3;

	WritePciConfig(pUnit, FIELDOFFSET(PCICONFIG, BaseAddr0), sizeof(DWORD), &pUnit->OriginalBar0);
	WritePciConfig(pUnit, FIELDOFFSET(PCICONFIG, BaseAddr1), sizeof(DWORD), &pUnit->OriginalBar1);
	WritePciConfig(pUnit, FIELDOFFSET(PCICONFIG, IntLine), sizeof(BYTE), &pUnit->OriginalIntLine);
	WritePciConfig(pUnit, FIELDOFFSET(PCICONFIG, Command), sizeof(WORD), &Three);

	return DEVIOCTL_NOERROR;
}


////////////////////////////////////////////////////////////////////////////////
// ReadPciConfig
//
// Read from the PCI Config space for a given unit
//
// Input:
//	pUnit		unit to read from
//	offset		byte offset in config space where read starts
//	size		number of bytes to read
//	buffer		buffer to receive data read
//
CONFIGRET ReadPciConfig(HOTINFO* pUnit, DWORD offset, DWORD size, PVOID buffer)
{
	return CONFIGMG_Call_Enumerator_Function(
		pUnit->Unit.m_DevNode, 
		PCI_ENUM_FUNC_GET_DEVICE_INFO,
		offset,
		buffer,
		size,
		0
		);
}

////////////////////////////////////////////////////////////////////////////////
// WritePciConfig
//
// Write to the PCI Config space for a given unit
//
// Input:
//	pUnit		unit to write to
//	offset		byte offset in config space where write starts
//	size		number of bytes to write
//	buffer		buffer containing data to be written
//
CONFIGRET WritePciConfig(HOTINFO* pUnit, DWORD offset, DWORD size, PVOID buffer)
{
	return CONFIGMG_Call_Enumerator_Function(
		pUnit->Unit.m_DevNode, 
		PCI_ENUM_FUNC_SET_DEVICE_INFO,
		offset,
		buffer,
		size,
		0
		);
}

////////////////////////////////////////////////////////////////////////////////
// TheIsr
//
// Interrupt service routine. In its default configuration the HOT II board
// interrupts after being reset or reconfigured, at which point the BARs,
// command register, and IntLine values in the config space have been cleared.
//
BOOL __stdcall TheIsr(VMHANDLE hVM, IRQHANDLE hIRQ, DWORD RefData)
{
    DWORD status;
    WORD IOport;
    DWORD data;
    DWORD IMaskPort;
	HOTINFO* pUnit = (HOTINFO*)RefData;
	DWORD CcmZero;

    DPRINTF( ("HotPCI ISR, unit=%x\n", pUnit) );

// The board causes an interrupt when it is reset, and after a reset, the
// BARs and interrupt line fields of the config space memory are zero. Therefore,
// this ISR has to reset them to the original values assigned by the operating
// system. 

// Note: Since PCI interrutps are usually shared, we are reloading the config
// on all occurrences of the interrupt, even those caused by other devices. 
// We have to reload the config in order to know if the given unit caused the
// interrupt, because a unit that has been reset does not decode at its 
// known address until it has been reloaded. It would be preferable to have
// a flag indicating that a reset/reconfig had been set, thereby enabling the
// ISR to conditionally perform the config reload.

	ReloadOriginalConfiguration(pUnit);

// Now test and see if this unit really interrupted.

	CcmZero = CCM_DWORD(pUnit,0);
	if ( (CcmZero & 1) == 0 ) // bit 0 == 0 means interrupt is active
	{
		// stop the board from interrupting
		CCM_DWORD(pUnit, CLR_INT0_OFFSET) = 0;

		// schedule a callback to set the Win32 event shared with the app
	    Call_VM_Event(hSystemVM, IntrEventHandler, pUnit, &pUnit->EventThunk);

    	// signal the end of interrupt processing
	    VPICD_Phys_EOI(hIRQ);

		return TRUE;	// the interrupt was handled
	}
	else // not this unit
		return FALSE;	// the interrupt was not handled
}

////////////////////////////////////////////////////////////////////////////////
// EnableIntrNotify
//
// Called as a DeviceControl services. Stores the handle of a Win32 Event object
// to be signaled when a hardware interrupt occurs.
//
DWORD EnableIntrNotify(HOTINFO* pUnit, struct HOTII_INTERRUPT_ENABLE_ARGS* pArgs)
{
	if (pArgs->EventHandle == NULL)
		return IOCTL_ERROR;		

	pUnit->hClientEvent = pArgs->EventHandle;

	if (pUnit->hClientEvent)
		VWIN32_CloseVxDHandle(pUnit->hClientEvent);		

	return DEVIOCTL_NOERROR;
}

////////////////////////////////////////////////////////////////////////////////
// DisableIntrNotify
//
DWORD DisableIntrNotify(HOTINFO* pUnit)
{
	return CloseEventHandle(pUnit);
}

////////////////////////////////////////////////////////////////////////////////
// CloseEventHandle
//
DWORD CloseEventHandle(HOTINFO* pUnit)
{
	if ( pUnit->hClientEvent)
	{
		VWIN32_CloseVxDHandle(pUnit->hClientEvent);
		pUnit->hClientEvent = NULL;
		return DEVIOCTL_NOERROR;	
	}
	else
		return IOCTL_ERROR;
}

////////////////////////////////////////////////////////////////////////////////
// IntrEventHandler
//
// This is the callback routine, scheduled by the ISR, that sets the Win32 
// Event object whose handle was passed to the driver by an application.
//
VOID __stdcall IntrEventHandler(VMHANDLE hVM, PVOID RefData, PCLIENT_STRUCT pRegs)
{
	HOTINFO* pUnit = (HOTINFO*)RefData;;

	DPRINTF( ("In the VM event handler after ISR, unit=%x\n", pUnit) );

	if ( pUnit->hClientEvent != 0 )
		VWIN32_SetWin32Event(pUnit->hClientEvent);
}
