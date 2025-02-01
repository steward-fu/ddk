;******************************************************************************
TITLE mmdevldr.inc - Windows/386 MMSYSTEM Device loader DLVxD include file
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1993 - 1995
;
;   Title:  mmdevldr.INC - Windows/386 Config Manager DLVxD DEVLOADER for MMSYSTEM
;
;   Version:    1.00
;
;
;==============================================================================
Multimedia_OEM_ID           equ 0440h            ; MS Reserved OEM # 34
MMDEVLDR_Device_ID           equ Multimedia_OEM_ID + 10 ;MMDEVLDR's device ID

MMDEVLDR_Ver_Major           equ 1                ; version 1.0 of this VxD
MMDEVLDR_Ver_Minor           equ 0


ifdef DDB_Sys_Crit_Init_Done
 
Begin_Service_Table	MMDEVLDR
	MMDEVLDR_Service	MMDEVLDR_Register_Device_Driver, LOCAL
	MMDEVLDR_Service	_MMDEVLDR_SetDevicePresence
        MMDEVLDR_Service        _MMDEVLDR_SetEnvironmentString
        MMDEVLDR_Service        _MMDEVLDR_GetEnvironmentString
        MMDEVLDR_Service        _MMDEVLDR_RemoveEnvironmentString
        MMDEVLDR_Service        _MMDEVLDR_AddEnvironmentString
		MMDEVLDR_Service		_MMDEVLDR_DeviceInitComplete
End_Service_Table MMDEVLDR	

endif   
