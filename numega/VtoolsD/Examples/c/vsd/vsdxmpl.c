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

// VSDXMPL.C - main module for VxD VSDXMPL

// This simple VSD reports to the debug console all I/O requests for 
// CDROM devices. It illustrates the basic requirements of writing a
// VSD for Windows 95.

// Summary of Important IOS Data Structures
// ----------------------------------------
// 
// DRP - Driver Registration Packet 
//		This structure contains information about a VSD. It is passed to
//		IOS when the VSD registers. Information in the DRP specifies
//		various parameters (e.g., the layer or group number to which the
//		VSD belongs), and provides the Async Event Routine address.
//
// ILB - IOS Linkage Block
//		Each VSD must allocate (static or on the heap) an ILB for 
//	   	communicating with the IOS. A pointer to the ILB is in the DRP.
//		When the VSD registers with IOS, IOS initializes the ILB. The
//		ILB contains addresses of entry points in the IOS that the VSD
//		can call.
//
// ISP - I/O Service Packet
//		A VSD uses this structure is used to request services from the 
//		IOS. The fields vary depending on which service is being requested.
//
// IOR - I/O Request 
//		This structure contains the parameters for a particular I/O request,
//		(e.g. how many bytes to transfer, start address, etc.). IORs are sent
//		to VSDs inside IOPs.
//
// IOP - I/O Packet
//		This structure contains an IOR and additional routing information for
//		a particular I/O request. 
//
// DCB - Device Control? Block
//		DCBs correspond to devices, either physical or logical. The DCB 
//		contains information about the type of device and its parameters.
//		When DCBs are created, each VSD is given the opportunity (via its
//		Async Event Routine) to insert itself into the calldown chain for
//		that DCB.
//
// AEP - Async Event Packet
//		IOS notifies each VSD of various device related events. The AEP
//		describes the type of event.
//
// DCB_cd_entry - DCB Calldown list Entry
//		For each DCB, the IOS maintains a list of VSDs that will be called
//		when there is an I/O request for that DCB. The list is in the form
//		of linked DCB_cd_entry structures. The first field of this structure
//		is the address of a VSD's I/O request handler.
//

// Sequence of Events in this Example
// ----------------------------------
// - The file must first be placed in windows\system\iosubsys. The IOS
//   will automatically load any file with the .vxd extension if it is
//   a valid dynamically loadable VxD.

// - If loaded ok, the VSD receives control message SYS_DYNAMIC_DEVICE_INIT.
//   This is the only control message that a VSD needs to process.

// - The handler for SYS_DYNAMIC_DEVICE_INIT calls IOS_Register, passing the
//   address of its DRP. Note that the DRP contains the address of the 
//   VSD's Async Event Routine (AER), which is the entry point that IOS will
//   call when there is an event to process. During registration, the IOS
//   will initialize the ILB, so that the VSD can call for services from
//   the IOS.

// - Eventually, the configuration process results in creation of various
//   DCBs, each corresponding to a logical or physical device in the system.
//   When a DCB is created, the IOS sends an AEP to each VSD's AER. 
//   The AEP type for a new DCB is AEP_CONFIG_DCB. This VSD looks for DCBs
//   that have device type of CDROM. When the AER finds one, it builds an
//   ISP to request insertion into the calldown list, and then calls IOS
//   thru the ILB. The ISP includes the address of the VSD's I/O request 
//   handler.

// - IOS sends the AEP for Boot Completion. At this point, the VSD decides
//   whether or not is should stay loaded. If it has inserted itself into at least 
//   one calldown chain, it sets the AEP_result field to AEP_SUCCESS. Otherwise,
//   it sets AEP_result to AEP_FAILURE. The latter tells IOS to unload the VSD.

// - When an I/O request for a CDROM occurs, IOS calls down the calldown
//   list associated with the DCB corresponding to the CDROM device. The
//   IOP is passed on the stack to each request handler.

// - In this example, the request handler doesn't really do anything. 
//   In a real VSD, it might examine the IOR in the IOP and actually process 
//   the request. In this case, the VSD only wants to pass the request down 
//   the calldown chain. 

// - Before passing the request down the chain, this VSD sets up a callback
//   so that it will be called when the I/O request is complete. Each IOP
//   contains a callback stack. If a VSD wants to be called back, it writes
//   the address of its completion handler to the current top of the callback
//   stack, and then bumps up the callback stack. 

// - In order to perform the calldown, the VSD gets from the IOP the current 
//   DCB_cd_entry (which points to the currently executing request handler),
//   and uses that to find the next DCB_cd_entry structure. The next 
//   DCB_cd_entry contains the address of the next request handler. Before
//   calling it, this VSD sets the pointer in the IOP to the next 
//   DCB_cd_entry (IOP_calldown_ptr) to point to the DCB_cd_entry that
//   corresponds to the VSD it is calling.

// - When the request is completed, the VSD's completion handler is called.
//   The completion handler removes itself from the callback stack by
//   decrementing the callback stack pointer in the IOP (IOP_callback_ptr).
//   The completion handler *must* then call the completion handler at
//   the top of the callback stack.

#define   DEVICE_MAIN
#include  "vsdxmpl.h"
#undef    DEVICE_MAIN

Declare_Layered_Driver(VSDXMPL, VSDXMPL_LGN_MASK, "123456789abcdef", 1, 0, 0)

DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT, OnSysDynamicDeviceInit);

VOID __cdecl VSDXMPL_CompletionHandler(IOP* pIop);
VOID __cdecl VSDXMPL_RequestHandler(IOP* pIop);
const CHAR* GetIOStatus(ULONG status);
const CHAR* GetIOFunctionName(ULONG func);
const CHAR* GetAEFunctionName(ULONG func);
BOOL bFoundAtLeastOneDevice;

// Control Message Dispatcher
//
// The system calls this routine to send the VSD a control message. The only 
// control message of significance for VSDs is SYS_DYNAMIC_DEVICE_INIT.
//
BOOL ControlDispatcher(
	DWORD dwControlMessage,
	DWORD EBX,
	DWORD EDX,
	DWORD ESI,
	DWORD EDI,
	DWORD ECX)
{
	START_CONTROL_DISPATCH

		ON_SYS_DYNAMIC_DEVICE_INIT(OnSysDynamicDeviceInit);

	END_CONTROL_DISPATCH

	return TRUE;
}

// SYS_DYNAMIC_DEVICE_INIT Handler
//
// This routine handles the control message SYS_DYNAMIC_DEVICE_INIT. Its main function
// is to register the VSD with IOS.
//
BOOL OnSysDynamicDeviceInit(void)
{
	dprintf("VSDXMPL: entering dynamic device init\n");

	IOS_Register(&VSDXMPL_Drp);

// IOS initializes the ILB during registration. Upon successful registration,
// the VSD may call through VSDXMPL_Ilb.ILB_service_rtn to request various
// services from IOS.

	dprintf("VSDXMPL: registration result=%04x\n", VSDXMPL_Drp.DRP_reg_result);

// Init a flag indicating that we have not yet seen a DCB that we care about
	bFoundAtLeastOneDevice = FALSE;

// The DRP_reg_result field contains the status of the registration. In this
// case, we consider either DRP_REMAIN_RESIDENT or DRP_MINIMIZE to be a 
// successful registration.

	return (VSDXMPL_Drp.DRP_reg_result == DRP_REMAIN_RESIDENT) ||
		   (VSDXMPL_Drp.DRP_reg_result == DRP_MINIMIZE);
}


// Async Event Routine
//
// IOS calls this routine to notify the VSD of various events related to devices.
// This is how a VSD learns of the creation of new DCBs.
//
VOID __cdecl VSDXMPL_Aer(AEP* pAep)
{
	DCB_COMMON* pDcb;
	ISP_calldown_insert InsertISP;

	dprintf("VSDXMPL: %s\n", GetAEFunctionName(pAep->AEP_func));
	switch (pAep->AEP_func)
	{
	case AEP_INITIALIZE:

// Perform initialization here.
		pAep->AEP_result = AEP_SUCCESS;
		break;

	case AEP_CONFIG_DCB:

// IOS sends this message when a new DCB is introduced to the system. The VSD 
// may at this point insert itself into the calldown list for the DCB. The 
// async event packet for this message is described by struct AEP_dcb_config.

		pDcb = ((AEP_dcb_config*)pAep)->AEP_d_c_dcb;

// In the case of this example VSD, we are only interested in physical CD-ROM devices.		

		if ( (pDcb->DCB_device_flags & DCB_DEV_PHYSICAL) &&
			 (pDcb->DCB_device_type == DCB_type_cdrom) )
		{
			dprintf("VSDXMPL: Inserting into calldown chain for DCB %x\n", pDcb);

// Initialize to zero the structure that will be passed back to IOS
			memset(&InsertISP, 0, sizeof(InsertISP));

// Set the required fields to arrange for insertion into the calldown list
			InsertISP.ISP_i_cd_hdr.ISP_func = ISP_INSERT_CALLDOWN;	// set function
			InsertISP.ISP_i_cd_dcb = pDcb;	// set device
			InsertISP.ISP_i_cd_req = VSDXMPL_RequestHandler;	// specify our request handler
			InsertISP.ISP_i_cd_lgn = VSDXMPL_LGN_ORD;	// specify IOS layer
			InsertISP.ISP_i_cd_flags = pDcb->DCB_dmd_flags; // copy current demand flags
			InsertISP.ISP_i_cd_ddb = pDcb->DCB_ptr_cd->DCB_cd_ddb;

// Call IOS on the entry supplied in ILB at registration time
			VSDXMPL_Ilb.ILB_service_rtn((PISP)&InsertISP);

// Set the flag indicating that we have inserted into at least one calldown list.
			bFoundAtLeastOneDevice = TRUE;
		}
		pAep->AEP_result = AEP_SUCCESS;
		break;

	case AEP_BOOT_COMPLETE:
// Tell IOS whether or not to unload this VxD, based on whether or not we found a 
// device that we care about.
		pAep->AEP_result = bFoundAtLeastOneDevice ? AEP_SUCCESS : AEP_FAILURE;
		break;

	default:
		break;
	}
}

// I/O Request Handler
//
// This is the request handler for I/O requests. This VSD installs this handler for all
// physical DCBs whose device type is CD-ROM. 

VOID __cdecl VSDXMPL_RequestHandler(IOP* pIop)
{
	dprintf("VSDXMPL: %s initiated\tIOP=%x\n", 
		GetIOFunctionName(pIop->IOP_ior.IOR_func), pIop);

// Add the completion handler for this VSD to the callback stack
	pIop->IOP_callback_ptr->IOP_CB_address = VSDXMPL_CompletionHandler;
	pIop->IOP_callback_ptr++;

// Set the IOP's pointer to the next calldown entry to the calldown entry that follows the calldown entry
// for this VSD
	pIop->IOP_calldown_ptr = pIop->IOP_calldown_ptr->DCB_cd_next;

// Invoke the next calldown function
	((PFNIOP)pIop->IOP_calldown_ptr->DCB_cd_io_address)(pIop);
}


// Completion handler
//
// The request handler sets up a call to this routine to notify the VSD that an I/O request
// has completed. It is responsible for removing itself from the callback stack and calling
// the next completion handler.
//
VOID __cdecl VSDXMPL_CompletionHandler(IOP* pIop)
{
	dprintf("VSDXMPL: %s completed\tIOP=%x status=%s\n", 
		GetIOFunctionName(pIop->IOP_ior.IOR_func), pIop, 
		GetIOStatus(pIop->IOP_ior.IOR_status));

	if (pIop->IOP_ior.IOR_status >= IORS_ERROR_DESIGNTR) // if error occurred
		; // process error

// remove top of callback stack
	--pIop->IOP_callback_ptr;	

// call previous notification 
	pIop->IOP_callback_ptr->IOP_CB_address(pIop);
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// The remaining functions look up names for manifest constants displayed to the debug 
// console.
//

typedef struct _name_assoc {
	BYTE _func;
	const char* _name;
} NAMETABLE;


const char* GetName(NAMETABLE nameTable[], int n, ULONG func)
{

	int i;

	for (i = 0; i < n; i++)
		if (func == nameTable[i]._func)
			return nameTable[i]._name;

	return "<unknown>";
}

const CHAR* GetIOFunctionName(ULONG func)
{
	
	static NAMETABLE nameTable[] = {
		{IOR_READ, "IOR_READ"},
		{IOR_WRITE, "IOR_WRITE"},
		{IOR_VERIFY, "IOR_VERIFY"},
		{IOR_CANCEL, "IOR_CANCEL"},
		{IOR_WRITEV, "IOR_WRITEV"},
		{IOR_MEDIA_CHECK, "IOR_MEDIA_CHECK"},
		{IOR_MEDIA_CHECK_RESET, "IOR_MEDIA_CHECK_RESET"},
		{IOR_LOAD_MEDIA, "{IOR_IOR_LOAD_MEDIA"},
		{IOR_EJECT_MEDIA, "IOR_EJECT_MEDIA"},
		{IOR_LOCK_MEDIA, "IOR_LOCK_MEDIA"},
		{IOR_UNLOCK_MEDIA, "IOR_UNLOCK_MEDIA"},
		{IOR_REQUEST_SENSE, "IOR_REQUEST_SENSE"},
		{IOR_COMPUTE_GEOM, "IOR_COMPUTE_GEOM"},
		{IOR_GEN_IOCTL, "IOR_GEN_IOCTL"},
		{IOR_FORMAT, "IOR_FORMAT"},
		{IOR_SCSI_PASS_THROUGH, "IOR_SCSI_PASS_THROUGH"},
		{IOR_CLEAR_QUEUE, "IOR_CLEAR_QUEUE"},
		{IOR_DOS_RESET, "IOR_DOS_RESET"},
		{IOR_SCSI_REQUEST, "IOR_SCSI_REQUEST"},
		{IOR_SET_WRITE_STATUS, "IOR_SET_WRITE_STATUS"},
		{IOR_RESTART_QUEUE, "IOR_RESTART_QUEUE"},
		{IOR_ABORT_QUEUE, "IOR_ABORT_QUEUE"},
		{IOR_SPIN_DOWN, "IOR_SPIN_DOWN"},
		{IOR_SPIN_UP, "IOR_SPIN_UP"},
		{IOR_FLUSH_DRIVE, "IOR_FLUSH_DRIVE"},
		{IOR_FLUSH_DRIVE_AND_DISCARD, "IOR_FLUSH_DRIVE_AND_DISCARD"},
		{IOR_FSD_EXT, "IOR_FSD_EXT"}
	};

#define NIOCOMMANDS (sizeof(nameTable)/sizeof(NAMETABLE))

	return GetName(nameTable, NIOCOMMANDS, func);
}

const CHAR* GetAEFunctionName(ULONG func)
{
	static NAMETABLE nameTable[] = {
		{AEP_INITIALIZE, "AEP_INITIALIZE"},
		{AEP_SYSTEM_CRIT_SHUTDOWN, "AEP_SYSTEM_CRIT_SHUTDOWN"},
		{AEP_BOOT_COMPLETE, "AEP_BOOT_COMPLETE"},
		{AEP_CONFIG_DCB, "AEP_CONFIG_DCB"},
		{AEP_UNCONFIG_DCB, "AEP_UNCONFIG_DCB"},
		{AEP_IOP_TIMEOUT, "AEP_IOP_TIMEOUT"},
		{AEP_DEVICE_INQUIRY, "AEP_DEVICE_INQUIRY"},
		{AEP_HALF_SEC, "AEP_HALF_SEC"},
		{AEP_1_SEC, "AEP_1_SEC"},
		{AEP_2_SECS, "AEP_2_SECS"},
		{AEP_4_SECS, "AEP_4_SECS"},
		{AEP_DBG_DOT_CMD, "AEP_DBG_DOT_CMD"},
		{AEP_ASSOCIATE_DCB, "AEP_ASSOCIATE_DCB"},
		{AEP_REAL_MODE_HANDOFF, "AEP_REAL_MODE_HANDOFF"},
		{AEP_SYSTEM_SHUTDOWN, "AEP_SYSTEM_SHUTDOWN"},
		{AEP_UNINITIALIZE, "AEP_UNINITIALIZE"},
		{AEP_DCB_LOCK, "AEP_DCB_LOCK"},
		{AEP_MOUNT_NOTIFY, "AEP_MOUNT_NOTIFY"},
		{AEP_CREATE_VRP, "AEP_CREATE_VRP"},
		{AEP_DESTROY_VRP, "AEP_DESTROY_VRP"},
		{AEP_REFRESH_DRIVE, "AEP_REFRESH_DRIVE"},
		{AEP_PEND_UNCONFIG_DCB, "AEP_PEND_UNCONFIG_DCB"},
		{AEP_1E_VEC_UPDATE, "AEP_1E_VEC_UPDATE"},
		{AEP_CHANGE_RPM, "AEP_CHANGE_RPM"}
	};

#define NAEFUNCTIONS (sizeof(nameTable)/sizeof(NAMETABLE))

		return GetName(nameTable, NAEFUNCTIONS, func);
}

const CHAR* GetIOStatus(ULONG status)
{
	static NAMETABLE nameTable[] = {
		{IORS_SUCCESS, "IORS_SUCCESS"},
		{IORS_SUCCESS_WITH_RETRY, "IORS_SUCCESS_WITH_RETRY"},
		{IORS_SUCCESS_WITH_ECC, "IORS_SUCCESS_WITH_ECC"},
		{IORS_INVALID_SECTOR, "IORS_INVALID_SECTOR"},
		{IORS_CANCELED, "IORS_CANCELED"},
		{IORS_CMD_IN_PROGRESS, "IORS_CMD_IN_PROGRESS"},
		{IORS_INVALID_CMD_PTR, "IORS_INVALID_CMD_PTR"},
		{IORS_MEDIA_ERROR, "IORS_MEDIA_ERROR"},
		{IORS_DEVICE_ERROR, "IORS_DEVICE_ERROR"},
		{IORS_INVALID_COMMAND, "IORS_INVALID_COMMAND"},
		{IORS_NO_DEVICE, "IORS_NO_DEVICE"},
		{IORS_NO_MEDIA, "IORS_NO_MEDIA"},
		{IORS_UNCERTAIN_MEDIA, "IORS_UNCERTAIN_MEDIA"},
		{IORS_UNREC_ERROR, "IORS_UNREC_ERROR"},
		{IORS_HW_FAILURE, "IORS_HW_FAILURE"},
		{IORS_UNFORMATTED_MEDIA, "IORS_UNFORMATTED_MEDIA"},
		{IORS_MEMORY_ERROR, "IORS_MEMORY_ERROR"},
		{IORS_TIME_OUT, "IORS_TIME_OUT"},
		{IORS_WRITE_PROTECT, "IORS_WRITE_PROTECT"},
		{IORS_NOT_READY, "IORS_NOT_READY"},
		{IORS_BUSY, "IORS_BUSY"},
		{IORS_VOL_NOT_LOCKED, "IORS_VOL_NOT_LOCKED"},
		{IORS_VOL_LOCKED, "IORS_VOL_LOCKED"},
		{IORS_VOL_NOT_REMOVABLE, "IORS_VOL_NOT_REMOVABLE"},
		{IORS_VOL_IN_USE, "IORS_VOL_IN_USE"},
		{IORS_LOCK_COUNT_EXCEEDED, "IORS_LOCK_COUNT_EXCEEDED"},
		{IORS_VALID_EJECT_FAILED, "IORS_VALID_EJECT_FAILED"},
		{IORS_ILLEGAL_ACCESS_MODE, "IORS_ILLEGAL_ACCESS_MODE"},
		{IORS_LOCK_VIOLATION, "IORS_LOCK_VIOLATION"},
		{IORS_WRONG_MEDIA, "IORS_WRONG_MEDIA"},
		{IORS_OUT_OF_SPACE, "IORS_OUT_OF_SPACE"},
		{IORS_BIG_IO_BREAKUP_FAILED, "IORS_BIG_IO_BREAKUP_FAILED"},
		{IORS_ERROR_DESIGNTR, "IORS_ERROR_DESIGNTR"},
		{IORS_INVALID_PARM, "IORS_INVALID_PARM"}
	};


#define NSTATUS (sizeof(nameTable)/sizeof(NAMETABLE))

		return GetName(nameTable, NSTATUS, status);
}