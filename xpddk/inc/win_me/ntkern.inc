ifndef _NTKERN_H
_NTKERN_H	EQU	1
Begin_Service_Table NTKERN, VxD
NTKERN_Service       _NTKERN_Get_Version, LOCAL
NTKERN_StdCall_Service   _NtKernCreateFile, 11, VxD_CODE
NTKERN_StdCall_Service   _NtKernClose, 1, VxD_CODE
NTKERN_StdCall_Service   _NtKernReadFile, 9, VxD_CODE
NTKERN_StdCall_Service   _NtKernWriteFile, 9, VxD_CODE
NTKERN_StdCall_Service   _NtKernDeviceIoControl, 10, VxD_CODE
NTKERN_Service       _NtKernGetWorkerThread, VxD_CODE
NTKERN_StdCall_Service   _NtKernLoadDriver, 1, VxD_CODE
NTKERN_StdCall_Service   _NtKernQueueWorkItem, 2, VxD_CODE
NTKERN_Service       _NtKernPhysicalDeviceObjectToDevNode, VxD_CODE
NTKERN_StdCall_Service   _NtKernSetPhysicalCacheTypeRange, 4, VxD_CODE
NTKERN_Service       _NtKernWin9XLoadDriver, VxD_CODE
NTKERN_StdCall_Service   _NtKernCancelIoFile, 2, VxD_CODE
NTKERN_Service       _NtKernGetVPICDHandleFromInterruptObj, VXD_CODE
NTKERN_StdCall_Service   _NtKernInternalDeviceIoControl, 10, VxD_CODE
NTKERN_Service       _NtKernPreLoadDriver, VxD_CODE
NTKERN_Service       _NtKernCheckUnPreLoadDriver, VxD_CODE
NTKERN_Service       _NtKernRtInfo, VxD_CODE
End_Service_Table NTKERN, VxD
endif
