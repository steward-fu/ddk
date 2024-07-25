/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/*****************************************************************************
 *
 *   Title:     IOS.H - Include file for Dragon I/O Supervisor
 *
 *   Version:   1.00
 *
 */

#define IOS_Device_ID	0x00010	// same as BLOCKDEV
#define IOS_DEVICE_ID	0x00010	// same as BLOCKDEV


/* Define the IOS VxD version # */

#define IOS_Major_Ver 0x3
#define IOS_Minor_Ver 0xA

/* Define the IOS service table.  The first 7 services are supersets of */
/* BlockDev services																		*/

#define	IOS_Service	Declare_Service

Begin_Service_Table(IOS)
	IOS_Service (IOS_Get_Version)
	IOS_Service (IOS_BD_Register_Device)
	IOS_Service (IOS_Find_Int13_Drive)
	IOS_Service (IOS_Get_Device_List)
	IOS_Service (IOS_SendCommand)
	IOS_Service (IOS_BD_Command_Complete)
	IOS_Service (IOS_Synchronous_Command)
	IOS_Service (IOS_Register)
	IOS_Service (IOS_Requestor_Service)
	IOS_Service (IOS_Exclusive_Access)
	IOS_Service (IOS_Send_Next_Command)
	IOS_Service (IOS_Set_Async_Time_Out)
	IOS_Service (IOS_Signal_Semaphore_No_Switch)
	IOS_Service (IOSIdleStatus)
	IOS_Service (IOSMapIORSToI24)
	IOS_Service (IOSMapIORSToI21)
	IOS_Service (PrintLog)
End_Service_Table(IOS)

/** Definitions for VFBACKUP exclusive access services */

#define IOSEA_LOCK_THREAD (1 << 8)	// indicates a DCB lock is thread based
