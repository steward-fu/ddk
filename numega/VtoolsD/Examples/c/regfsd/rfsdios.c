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

// RFSDIOS.C - I/O Subsystem interface for Registry File System
// Copyright (c) 1996, Compuware Corporation

// This module contains functions that are called by the IOS.

#include  "regfsd.h"
#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

DCB* pDcbOfThisDevice;
BYTE Drive;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	REGFSD_Aer
//
// Purpose
//	Async Event Routine for RFSD	
//
// Parameters
//	pAep	pointer to the AEP structure
//
// Return Value
//	Depends on AEP subfunction.
//
// Remarks
//	The function name is required by usage of the macro 
//	Define_Port_Driver (see regfsd.c).
//
//	We handle the following AEPs:
//
//	  AEP_INITIALIZE:	create a DDB for our virtual drive
//	  AEP_INQUIRY:		set up a fake DCB
//	  AEP_CONFIG_DCB:	associate drive letter and insert into
//				calldown list
//	  AEP_BOOT_COMPLETE:	signal OK
//
VOID __cdecl REGFSD_Aer(AEP* pAep)
{
	DCB_COMMON* pDcb;
	ISP_ddb_create CreateDDBPacket;
	IOSDDB* pNewDDB;
	ISP_calldown_insert InsertISP;

//	dprintf("REGFSD: %s\n", GetAEFunctionName(pAep->AEP_func));

	switch (pAep->AEP_func)
	{
	case AEP_INITIALIZE:

// Create a DDB for our "controller". This will cause IOS to send
// AEP_INQUIRY packets for each unit on the controller.

		CreateDDBPacket.ISP_ddb_hdr.ISP_func = ISP_CREATE_DDB;
		CreateDDBPacket.ISP_ddb_size = sizeof(IOSDDB);
		REGFSD_Ilb.ILB_service_rtn((PISP)&CreateDDBPacket);
		pNewDDB = (IOSDDB*)CreateDDBPacket.ISP_ddb_ptr;
		pNewDDB->DDB_number_buses = 1;

		pAep->AEP_result = AEP_SUCCESS;
		break;

	case AEP_DEVICE_INQUIRY:

// The IOS sends this in response to the creation of the DDB. On the
// first call (DCB_unit_on_ctl == 0) we set up the supplied fake DCB
// with the parametes for our virtual drive. On subsequent calls, we
// anwer "no more devices". This terminates the inquiry.

		pDcb = ((AEP_inquiry_device*)pAep)->AEP_i_d_dcb;
		if (((DCB*)pDcb)->DCB_unit_on_ctl == 0)
		{
			SetupDCB((DCB*)pDcb);
			pAep->AEP_result = AEP_SUCCESS;
		}
		else
			pAep->AEP_result = AEP_NO_MORE_DEVICES;	
		break;

	case AEP_CONFIG_DCB:

// The IOS sends this AEP after each successful inquiry call. We
// associate the DCB with a drive letter and insert the request
// handler into the calldown chain.

		pDcb = ((AEP_dcb_config*)pAep)->AEP_d_c_dcb;
		pDcbOfThisDevice = (DCB*)pDcb;
		Drive = AssociateDriveLetter(pDcb);

		memset(&InsertISP, 0, sizeof(InsertISP));
		InsertISP.ISP_i_cd_hdr.ISP_func = ISP_INSERT_CALLDOWN;	
		InsertISP.ISP_i_cd_dcb = pDcb;	
		InsertISP.ISP_i_cd_req = REGFSD_RequestHandler;	
		InsertISP.ISP_i_cd_lgn = DRP_MISC_PD;	
		InsertISP.ISP_i_cd_flags = pDcb->DCB_dmd_flags; 
		InsertISP.ISP_i_cd_ddb = pDcb->DCB_ptr_cd->DCB_cd_ddb;
		InsertISP.ISP_i_cd_expan_len = 0;
		REGFSD_Ilb.ILB_service_rtn((PISP)&InsertISP);

		pAep->AEP_result = AEP_SUCCESS;
		break;

	case AEP_BOOT_COMPLETE:

// Signal OK so that IOS will not unload the driver

		pAep->AEP_result = AEP_SUCCESS;
		break;

	default:
		break;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	REGFSD_RequestHandler
//
// Purpose
//	Handle port driver level I/O requests
//
// Parameters
//	pIop	pointer to the IOP
//
// Return Value
//	none (values returned via IOR, depends on function)
//
// Remarks
//	RFSD supports the following requests:
//	  IOR_READ:		generic read
//	  IOR_GEN_IOCTL:	generic I/O control
//


// The following is a work around for a bug in IOS, which
// assumes that stack parameters are not modified. In fact,
// MSVC++ 5.0 does reuse parameter storage for locals.

VOID __cdecl RequestHandler(IOP * pIop);

VOID __cdecl REGFSD_RequestHandler(IOP * pIop)
{
	RequestHandler(pIop);
}

VOID __cdecl RequestHandler(IOP * pIop)
{
	PDEVICEPARAMS pDP;
	IOR* pIor = &pIop->IOP_ior;
	HKEY h;
	PCHAR pValue, p;
	DWORD pos;
	DWORD ValueSize = READBUFSIZE;
	CHAR readbuf[READBUFSIZE];
	PDEVICEPARAMS pDp;

//	dprintf("REGFSD: %s initiated\tIOP=%x\n", 
//		GetIOFunctionName(pIor->IOR_func), pIop);

	pIor->IOR_status = IORS_INVALID_COMMAND;	// assume error

	switch (pIop->IOP_ior.IOR_func)
	{
	case IOR_READ:

// The IOP was sent by our HandleRead function. The private convention
// is:
//	IOR_start_addr[0] = the Registry key to whose value is to be read
//	IOR_start_addr[1] = name of value to read
//	IOR_private_port  = file position (byte offset from start of value)

		h = (HKEY)pIor->IOR_start_addr[0];
		pValue = (PCHAR)pIor->IOR_start_addr[1];
		pos = pIor->IOR_private_port;
		p = readbuf;

// Read the value

		if (RegQueryValueEx(h, pValue, NULL,NULL, p, &ValueSize) == 
				ERROR_SUCCESS)
		{

// If the value is read, then transfer the data to the client buffer

			if (pos < READBUFSIZE)
			{
				pIor->IOR_status = IORS_SUCCESS;
				pIor->IOR_xfer_count = 
					min(pIor->IOR_xfer_count, 
					    READBUFSIZE-pos);
				memcpy((PVOID)pIor->IOR_buffer_ptr, p+pos, 
					pIor->IOR_xfer_count);
			}
		}

		break;

	case IOR_GEN_IOCTL:
// dprintf("\tdrive=%c function=%x\n", pIor->IOR_ioctl_drive+'A',
//			pIor->IOR_ioctl_function);

		switch (pIor->IOR_ioctl_function)
		{
		case 0x440D:
			// Get Device Parameters

			pDp = (PDEVICEPARAMS)pIor->IOR_ioctl_buffer_ptr;

			if ( (pIor->IOR_ioctl_control_param & 0xff) == 0x60) 
			{
				IoctlGetDeviceParams(pDp);
				pIor->IOR_status = IORS_SUCCESS;
			}

			break;
		
		 }	

		break;
		
	default:
		pIor->IOR_status = IORS_INVALID_COMMAND;
		break;
	}

// Invoke the completion handler. First pop off the empty entry at the
// top of the stack. Note: IOP_callback_ptr is type as IOP_callback_entry*,
// not ULONG when using VtoolsD.

	--pIop->IOP_callback_ptr;	
	pIop->IOP_callback_ptr->IOP_CB_address(pIop);
}

