
/*****************************************************************************
 *
 *   (C) Copyright MICROSOFT Corp., 1988-1990
 *
 *   Title:     IOS.H - Include file for Dragon I/O Supervisor
 *
 *   Version:   1.00
 *
 *   Date:      15-Mar-1992
 *
 *   Author:
 *
 *-----------------------------------------------------------------------------
 *
 *   Change log:
 *
 *   DATE        REV DESCRIPTION
 *   ----------- --- -----------------------------------------------------------
 */

#ifndef	_IOS_H
#define	_IOS_H

/**/
/* Define the IOS VxD device ID bugbug - this must be finalized */
/**/

/**/
/* Define the IOS service table.  The first 7 services are supersets of */
/* BlockDev services																		*/
/**/

/*XLATOFF*/
#define	IOS_Service	Declare_Service
#pragma warning (disable:4003)          // turn off not enough params warning
/*XLATON*/

/*MACROS*/
Begin_Service_Table(IOS)

//   PROCEDURE: IOS_Get_Version
//
//   DESCRIPTION:
//       ios service to return version
//
//   ENTRY:
//       none
//
//   EXIT:
//      eax = version
//      carry clear

IOS_Service	(IOS_Get_Version, LOCAL)

//   PROCEDURE: IOS_BD_Register_Device
//
//   DESCRIPTION:
//      ios service to register a BlockDev device
//
//   ENTRY:
//      edi => BDD (BlockDev Device Descriptor)
//
//   EXIT:
//      CLC is successful, STC if not
//      

IOS_Service	(IOS_BD_Register_Device)

//   PROCEDURE: IOS_Find_Int13_Drive
//
//   DESCRIPTION:
//      ios service to return BDD for int 13 drive
//
//   ENTRY:
//      al = int 13 unit #
//
//   EXIT:
//      edi => BDD if successful
//      CLC is successful, STC if not
//      

IOS_Service	(IOS_Find_Int13_Drive)

//   PROCEDURE: IOS_Get_Device_List
//
//   DESCRIPTION:
//      returns pointer to first BDD
//
//   ENTRY:
//      none
//
//   EXIT:
//      edi => BDD or zero if none
//      

IOS_Service	(IOS_Get_Device_List)

//   PROCEDURE: IOS_SendCommand
//
//   DESCRIPTION:
//      service to accept BlockDev Command Blocks, or I/O Requests (version 2)
//
//   ENTRY:
//      if IORF_VERSION_002 is set:
//           esi points to IOR
//	 			 edi points to DCB_bdd
//
//      otherwise: (for backward BlockDev client compatibility)
//           esi points to BlockDev_Command_Block
//           edi points to BlockDev_Device_Descriptor
//
//       EXIT:
//			    none
//      

IOS_Service	(IOS_SendCommand)

//   PROCEDURE: IOS_BD_Command_Complete
//
//   DESCRIPTION:
//      service to indicate a command has completed
//
//   ENTRY:
//		  esi => IOR
//
//   EXIT:
//      none
//    

IOS_Service	(IOS_BD_Command_Complete)

//   PROCEDURE: IOS_Synchronous_Command
//
//   DESCRIPTION:
//      not currently supported under Chicago
//
//   ENTRY:
//
//   EXIT:
//    

IOS_Service	(IOS_Synchronous_Command)

//   PROCEDURE: IOS_Register
//
//   DESCRIPTION:
//   		service to register a driver with IOS
//
//   ENTRY:
//		   [esp] => Driver Registration Packet (DRP)
//
//   EXIT:
//    	DRP_result set appropriately
//

IOS_Service	(IOS_Register)

//   PROCEDURE: IOS_Requestor_Service
//
//   DESCRIPTION:
//   		service for access to some IOS services
//
//   ENTRY:
//		   [esp] => IOS Requestor Service (IRS) packet
//
//   EXIT:
//    	IRS_result set appropriately
//

IOS_Service	(IOS_Requestor_Service)

//   PROCEDURE: IOS_Exclusive_Access
//
//   DESCRIPTION:
//			locks a given drive for exclusive access from one VM
//
//   ENTRY: 
//				eax - drive to lock
//	       	    - ah - set to IOSEA_LOCK_THREAD or clear, indicating a thread
//		             or VM based lock (lock only)
//	         ebx - VM handle to lock on / release
//	         ecx - nz - obtain exclusive access
//	             - 0 - release exclusive access
//
//   EXIT: 
//				carry set if unsuccessful
//

IOS_Service	(IOS_Exclusive_Access)

//   PROCEDURE: IOS_Send_Next_Command
//
//   DESCRIPTION:
//   		service to send the next command down to a device
//
//   ENTRY:
//			edi => Device Descriptor Block (DCB) for device
//
//   EXIT:
//    	none
//

IOS_Service	(IOS_Send_Next_Command)

//   PROCEDURE: IOS_Set_Async_Timeout
//
//   DESCRIPTION:
//   		identical to VMMCall Set_Async_Timeoout
//
//   ENTRY:
//       EAX = Number of milliseconds to wait until time-out
//       EDX = Reference data to return to procedure
//       ESI = Address of procedure to call when time-out occurs
//
//   EXIT:
//       If time-out was NOT scheduled then
//           ESI = 0 (This is useful since 0 = NO TIME-OUT SCHEDULED)
//       else
//           ESI = Time-out handle (used to cancel time-out)
//    	none
//

IOS_Service	(IOS_Set_Async_Time_Out)

//   PROCEDURE: IOS_Signal_Semaphore_No_Switch
//
//   DESCRIPTION:
//   		identical to VMMCall Signal_Semaphore_No_Switch
//
//   ENTRY:
//			eax = semaphore handle
//
//   EXIT:
//    	none
//

IOS_Service	(IOS_Signal_Semaphore_No_Switch)

//   PROCEDURE: IOSIdleStatus
//
//   DESCRIPTION:
//			This routine determines whether IOS is idle by
//			walking the IOS heap looking for allocated IOPs.
// 		Therefore this should not be called after
//			allocating an IOP.
//   		
//
//   ENTRY:
//			none
//
//   EXIT:
//    	eax = zero if idle, non-zero if not
//

IOS_Service	(IOSIdleStatus)

//   PROCEDURE: IOSMapIORSToI24
//
//   DESCRIPTION:
//			service maps an IOR error code to its int 24 equivalent.
//   		
//
//   ENTRY:
//			eax = IOR error code
//			ecx = IOR function
//
//   EXIT:
//			eax = int 23 error code

IOS_Service	(IOSMapIORSToI24)

//   PROCEDURE: IOSMapIORSToI24
//
//   DESCRIPTION:
//			service maps an IOR error code to its int 21 equivalent.
//   		
//
//   ENTRY:
//			ecx = IOR error code
//
//   EXIT:
//			eax = int 21 error code

IOS_Service	(IOSMapIORSToI21)

//   PROCEDURE: PrintLog
//
//   DESCRIPTION:
//   		
//   ENTRY:
//
//   EXIT:
//			

IOS_Service	(PrintLog)

//   PROCEDURE: IOS_Deregister
//
//   DESCRIPTION:
//   		service to register a driver with IOS
//
//   ENTRY:
//		   esi => Driver Vector Table [DVT]
//
//   EXIT:
//			none
//

IOS_Service	(IOS_deregister)

IOS_Service	(IOS_wait)

//   PROCEDURE: IOS_wait
//
//   DESCRIPTION:
//   		service to stall the processor for the approximate amount of specified
//           microseconds
//
//   ENTRY:
//		   [esp] => approximate number of microseconds to stall. 
//
//   EXIT:
//

IOS_Service	(_IOS_SpinDownDrives)

//   PROCEDURE: IOS_SpinDownDrives
//
//   DESCRIPTION:
//	service to spindown all IOS managed drives
//
//   ENTRY: none
//
//   EXIT: none

IOS_Service	(_IOS_query_udf_mount)

//	Check to see if UDF should mount on a device
//	
//
//   ENTRY:
//
//	drive number to check on stack (ccall)
//
//   EXIT: eax = 0 Do not mount UDF
//	      eax !=0 ok to mount UDF
//	    
//	
//
//   USES: ccall (eax, ecx, edx)
//

IOS_Service	(_IOS_LockDrive)

//   PROCEDURE: IOS_LockDrive
//
//   DESCRIPTION:
//	Lock a drive for exclusive access.  The drive may not
//	contain the swap file.
//
//   ENTRY:
//	drive number to check on stack (ccall)
//
//   EXIT:
//	(EAX) = NULL, lock NOT granted
//	(EAX) = IOS entry point, lock granted
//	
//
//   USES: ccall (eax, ecx, edx)

IOS_Service	(_IOS_UnlockDrive)

//   PROCEDURE: IOS_UnlockDrive
//
//   DESCRIPTION:
//	Unlock a drive locked for exclusive access.
//
//   ENTRY:
//	entry point returned by IOS_LockDrive (ccall)
//
//   EXIT:
//	(EAX) = 0, Success
//	(EAX) != 0, Unlock failed
//	
//   USES: ccall (eax, ecx, edx)

IOS_Service	(_IOS_IsDriveLocked)

//   PROCEDURE: IOS_IsDriveLocked
//
//   DESCRIPTION:
//	Return true if drive is locked for exclusive access
//
//   ENTRY:
//	drive number to check on stack (ccall)
//
//   EXIT:
//	(EAX) = 0, drive not locked
//	(EAX) != 0, drive locked
//	
//   USES: ccall (eax, ecx, edx)

IOS_Service	(_IOS_SpinUpDrives)

//   PROCEDURE: IOS_SpinUpDrives
//
//   DESCRIPTION:
//	service to spin up all IOS managed drives. Only VPOWERD should
//	use while preparing to reboot.
//
//   ENTRY: none
//
//   EXIT: none

End_Service_Table(IOS)

/*ENDMACROS*/

/**/
/** Definitions for VFBACKUP exclusive access services */
/**/

#define IOSEA_LOCK_THREAD (1 << 8)	// indicates a DCB lock is thread based

#endif	// _IOS_H
