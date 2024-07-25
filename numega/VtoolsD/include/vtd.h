/*
 *
 *   (C) Copyright MICROSOFT Corp., 1988-1990
 *   (C) Copyright Compuware Corporation, 1994
 *
 */

#ifndef	__vtd_h_
#define	__vtd_h_


/* API Function Codes */

#define VTD_API_GET_VERSION 0x0000
#define VTD_API_GET_TIME    0x0100

/* Sub-Codes for Get Time */

#define VTD_API_GET_TIME_IN_CLOCK_TICKS 0
#define VTD_API_GET_TIME_IN_MS		1
#define VTD_API_GET_VM_EXEC_TIME	2



#define	VTD_Service	Declare_Service
#define VTD_StdCall_Service Declare_SCService

Begin_Service_Table(VTD)

VTD_Service	(VTD_Get_Version)
VTD_Service	(VTD_Update_System_Clock)
VTD_Service	(VTD_Get_Interrupt_Period)
VTD_Service	(VTD_Begin_Min_Int_Period)
VTD_Service	(VTD_End_Min_Int_Period)
VTD_Service	(VTD_Disable_Trapping)
VTD_Service	(VTD_Enable_Trapping)
VTD_Service	(VTD_Get_Real_Time)
#ifdef WIN40
VTD_Service	(VTD_Get_Date_And_Time)
VTD_Service	(VTD_Adjust_VM_Count)
VTD_Service	(VTD_Delay)
VTD_Service (VTD_GetTimeZoneBias)
#endif
#ifdef WIN41_OR_LATER
VTD_StdCall_Service	(ObsoleteKeQueryPerformanceCounter, 1)
VTD_StdCall_Service	(ObsoleteKeQuerySystemTime, 1)
VTD_Service	(VTD_Install_IO_Handle)
VTD_Service	(VTD_Remove_IO_Handle)
VTD_Service	(_VTD_Delay_Ex)
VTD_Service	(VTD_Init_Timer)
#endif
End_Service_Table(VTD)


#endif		// __vtd_h_
