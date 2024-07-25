_VTD_H	EQU	1
VTD_DELAYEX_NEEDRESUMEEXEC	EQU	1
VTD_API_GET_VERSION	EQU	0000H
VTD_API_GET_TIME	EQU	0100H
VTD_API_GET_TIME_IN_CLOCK_TICKS	EQU	0
VTD_API_GET_TIME_IN_MS	EQU	1
VTD_API_GET_VM_EXEC_TIME	EQU	2
ifndef Not_VxD
Begin_Service_Table VTD, VxD
VTD_Service	 VTD_Get_Version, LOCAL
VTD_Service	 VTD_Update_System_Clock, LOCAL
VTD_Service	 VTD_Get_Interrupt_Period, LOCAL
VTD_Service	 VTD_Begin_Min_Int_Period, LOCAL
VTD_Service	 VTD_End_Min_Int_Period, LOCAL
VTD_Service	 VTD_Disable_Trapping, LOCAL
VTD_Service	 VTD_Enable_Trapping, LOCAL
VTD_Service	 VTD_Get_Real_Time, LOCAL
VTD_Service	 VTD_Get_Date_And_Time, LOCAL
VTD_Service	 VTD_Adjust_VM_Count, LOCAL
VTD_Service	 VTD_Delay, LOCAL
VTD_Service      VTD_GetTimeZoneBias, LOCAL
VTD_StdCall_Service	 ObsoleteKeQueryPerformanceCounter, 1, LOCAL
VTD_StdCall_Service	 ObsoleteKeQuerySystemTime, 1, LOCAL
ifdef	NEC_98
VTD_Service	 VTD_Install_IO_Handle, LOCAL
VTD_Service	 VTD_Remove_IO_Handle, LOCAL
else
VTD_Service	 VTD_Install_IO_Handle, LOCAL
VTD_Service	 VTD_Remove_IO_Handle, LOCAL
endif
VTD_Service	 _VTD_Delay_Ex, VxD_CODE
VTD_Service	 VTD_Init_Timer, LOCAL
End_Service_Table VTD, VxD
endif
VTD_DELAY_EX_POLL_METHOD	EQU	00000000H
VTD_DELAY_EX_RESUME_EXEC_METHOD	EQU	00000001H
VTD_DELAY_EX_BLOCKING_METHOD	EQU	00000002H
