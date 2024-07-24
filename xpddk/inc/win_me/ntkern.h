/******************************************************************************
 *
 *   (C) Copyright MICROSOFT Corp., 1988-1990
 *
 *   Title:     NTKERN.H - Include file for Windows NT Kernel VxD
 *
 *   Version:   4.00
 *
 *   Date:      23-Oct-1995
 *
 *   Author:    FCF
 *
 *-----------------------------------------------------------------------------
 *
 *   Change log:
 *
 *      DATE    REV                 DESCRIPTION
 *   ----------- --- ----------------------------------------------------------
 *   23-Oct-1995 FCF Started
 *
 ******************************************************************************/

#ifndef _NTKERN_H
#define _NTKERN_H


/*INT32*/

/*XLATOFF*/
#define NTKERN_Service   Declare_Service
#define NTKERN_StdCall_Service Declare_SCService
/*XLATON*/

/*MACROS*/
Begin_Service_Table(NTKERN, VxD)

NTKERN_Service      (_NTKERN_Get_Version, LOCAL)
NTKERN_StdCall_Service  (_NtKernCreateFile, 11, VxD_CODE)
NTKERN_StdCall_Service  (_NtKernClose, 1, VxD_CODE)
NTKERN_StdCall_Service  (_NtKernReadFile, 9, VxD_CODE)
NTKERN_StdCall_Service  (_NtKernWriteFile, 9, VxD_CODE)
NTKERN_StdCall_Service  (_NtKernDeviceIoControl, 10, VxD_CODE)
NTKERN_Service      (_NtKernGetWorkerThread, VxD_CODE)
NTKERN_StdCall_Service  (_NtKernLoadDriver, 1, VxD_CODE)
NTKERN_StdCall_Service  (_NtKernQueueWorkItem, 2, VxD_CODE)
NTKERN_Service      (_NtKernPhysicalDeviceObjectToDevNode, VxD_CODE)
NTKERN_StdCall_Service  (_NtKernSetPhysicalCacheTypeRange, 4, VxD_CODE)
NTKERN_Service      (_NtKernWin9XLoadDriver, VxD_CODE)
NTKERN_StdCall_Service  (_NtKernCancelIoFile, 2, VxD_CODE)
NTKERN_Service      (_NtKernGetVPICDHandleFromInterruptObj, VXD_CODE)
NTKERN_StdCall_Service  (_NtKernInternalDeviceIoControl, 10, VxD_CODE)
NTKERN_Service      (_NtKernPreLoadDriver, VxD_CODE)
NTKERN_Service      (_NtKernCheckUnPreLoadDriver, VxD_CODE)
NTKERN_Service      (_NtKernRtInfo, VxD_CODE)

End_Service_Table(NTKERN, VxD)
/*ENDMACROS*/


#endif  // ifndef _NTKERN_H

