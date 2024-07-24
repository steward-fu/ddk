ifndef	_IOS_H
_IOS_H	EQU	1
Begin_Service_Table IOS
IOS_Service	 IOS_Get_Version, LOCAL
IOS_Service	 IOS_BD_Register_Device
IOS_Service	 IOS_Find_Int13_Drive
IOS_Service	 IOS_Get_Device_List
IOS_Service	 IOS_SendCommand
IOS_Service	 IOS_BD_Command_Complete
IOS_Service	 IOS_Synchronous_Command
IOS_Service	 IOS_Register
IOS_Service	 IOS_Requestor_Service
IOS_Service	 IOS_Exclusive_Access
IOS_Service	 IOS_Send_Next_Command
IOS_Service	 IOS_Set_Async_Time_Out
IOS_Service	 IOS_Signal_Semaphore_No_Switch
IOS_Service	 IOSIdleStatus
IOS_Service	 IOSMapIORSToI24
IOS_Service	 IOSMapIORSToI21
IOS_Service	 PrintLog
IOS_Service	 IOS_deregister
IOS_Service	 IOS_wait
IOS_Service	 _IOS_SpinDownDrives
IOS_Service	 _IOS_query_udf_mount
IOS_Service	 _IOS_LockDrive
IOS_Service	 _IOS_UnlockDrive
IOS_Service	 _IOS_IsDriveLocked
IOS_Service	 _IOS_SpinUpDrives
End_Service_Table IOS
IOSEA_LOCK_THREAD	EQU	(1 SHL 8)
endif
