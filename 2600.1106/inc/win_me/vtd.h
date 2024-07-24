/*****************************************************************************
 *
 *   (C) Copyright MICROSOFT Corp., 1988-1990
 *
 *   Title:	VTD.H -
 *
 *   Version:	1.00
 *
 *   Date:	10-Aug-1988
 *
 *   Author:	RAL
 *
 *------------------------------------------------------------------------------
 *
 *   Change log:
 *
 *      DATE	REV		    DESCRIPTION
 *   ----------- --- -----------------------------------------------------------
 *   10-Aug-1988 RAL Original
 *   12-Oct-1988 RAL Converted to VxD_Service_Table macros
 *   22-Sep-1992 MJM Converted to h file
 *****************************************************************************/

#define _VTD_H

/*XLATOFF*/
#define	VTD_Service	Declare_Service
#define VTD_StdCall_Service Declare_SCService
#pragma	warning (disable:4003)		// turn off

/*XLATON*/

#define VTD_DELAYEX_NEEDRESUMEEXEC	1

/* API Function Codes */

#define VTD_API_GET_VERSION 0x0000
#define VTD_API_GET_TIME    0x0100

/* Sub-Codes for Get Time */

#define VTD_API_GET_TIME_IN_CLOCK_TICKS 0
#define VTD_API_GET_TIME_IN_MS		1
#define VTD_API_GET_VM_EXEC_TIME	2


#ifndef Not_VxD
/*MACROS*/
Begin_Service_Table(VTD, VxD)

VTD_Service	(VTD_Get_Version, LOCAL)
VTD_Service	(VTD_Update_System_Clock, LOCAL)
VTD_Service	(VTD_Get_Interrupt_Period, LOCAL)
VTD_Service	(VTD_Begin_Min_Int_Period, LOCAL)
VTD_Service	(VTD_End_Min_Int_Period, LOCAL)
VTD_Service	(VTD_Disable_Trapping, LOCAL)
VTD_Service	(VTD_Enable_Trapping, LOCAL)
VTD_Service	(VTD_Get_Real_Time, LOCAL)
VTD_Service	(VTD_Get_Date_And_Time, LOCAL)
VTD_Service	(VTD_Adjust_VM_Count, LOCAL)
VTD_Service	(VTD_Delay, LOCAL)


VTD_Service     (VTD_GetTimeZoneBias, LOCAL)
VTD_StdCall_Service	(ObsoleteKeQueryPerformanceCounter, 1, LOCAL)
VTD_StdCall_Service	(ObsoleteKeQuerySystemTime, 1, LOCAL)
/*ENDMACROS*/

#ifdef	NEC_98 //NEC_98

/*MACROS*/
VTD_Service	(VTD_Install_IO_Handle, LOCAL)
VTD_Service	(VTD_Remove_IO_Handle, LOCAL)
/*ENDMACROS*/

#else

/*MACROS*/
// BUGBUG make reserve work
VTD_Service	(VTD_Install_IO_Handle, LOCAL)
VTD_Service	(VTD_Remove_IO_Handle, LOCAL)
/*ENDMACROS*/

#endif //NEC_98

/*MACROS*/
VTD_Service	(_VTD_Delay_Ex, VxD_CODE)

VTD_Service	(VTD_Init_Timer, LOCAL)

End_Service_Table(VTD, VxD)
/*ENDMACROS*/
#endif /*NotVxD*/

//
// The blocking method is the prefered method for any long delay (ie >=1ms).
// It basically blocked the current thread for that amount of time. For delay
// of shorter time, the poll method does not release the current thread for
// any reason (VTD automatically use that one if interrupt are disabled). For
// short time delay with interrupt enabled, use RESUME_EXEC method as we
// allow interrupt to be acknowledged.
//
#define	VTD_DELAY_EX_POLL_METHOD	0x00000000
#define	VTD_DELAY_EX_RESUME_EXEC_METHOD	0x00000001
#define	VTD_DELAY_EX_BLOCKING_METHOD	0x00000002

/*XLATOFF*/
#pragma warning (disable:4035)		// turn off no return code warning

/*
 *  This function is a macro for efficiency.  The parameters passed are
 *  the variables the version (USHORT), cmsFastest and cmsSlowest (ULONG).
 */

#define	VTD_Get_Version(ver, cmsFastest, cmsSlowest) \
{ \
    VxDCall(VTD_Get_Version) \
    __asm xchg [ver],ax \
    __asm xchg [cmsFastest],ebx \
    __asm xchg [cmsSlowest],ecx \
}

void VXDINLINE
VTD_Update_System_Clock(void)
{
    VxDCall(VTD_Update_System_Clock)
}

CMS VXDINLINE
VTD_Get_Interrupt_Period()
{
    Touch_Register(eax)
    VxDCall(VTD_Get_Interrupt_Period)
}

int VXDINLINE
VTD_Begin_Min_Int_Period(CMS cms)
{
    _asm mov eax, cms
    VxDCall(VTD_Begin_Min_Int_Period)
    _asm cmc
    _asm sbb eax,eax
}

int VXDINLINE
VTD_End_Min_Int_Period(CMS cms)
{
    _asm mov eax, cms
    VxDCall(VTD_End_Min_Int_Period)
    _asm cmc
    _asm sbb eax,eax
}

void VXDINLINE
VTD_Enable_Trapping(HVM hvm)
{
    _asm mov ebx, hvm
    VxDCall(VTD_Enable_Trapping)
}

void VXDINLINE
VTD_Disable_Trapping(HVM hvm)
{
    _asm mov ebx, hvm
    VxDCall(VTD_Disable_Trapping)
}

LARGE_INTEGER VXDINLINE
VTD_Get_Real_Time(void)
{
    Touch_Register(eax)
    Touch_Register(edx)
    VxDCall(VTD_Get_Real_Time)
}

ULONG VXDINLINE
VTD_Delay(ULONG cus)
{
    _asm mov ecx, cus
    VxDCall(VTD_Delay)
    _asm mov eax, ecx
}


#pragma warning (default:4035)		// turn on no return code warning

/*XLATON*/
