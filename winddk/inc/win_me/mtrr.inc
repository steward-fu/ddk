ifndef	_MTRR_H_
_MTRR_H_	EQU	1
MTRR_DEVICE_ID	EQU	<DDS_DEVICE_ID>
ifndef	_NTDDK_
MmNonCached	EQU	0
MmCached	EQU	1
MmFrameBufferCached	EQU	2
MmHardwareCoherentCached	EQU	3
MmMaximumCacheType	EQU	4
endif
Begin_Service_Table MTRR, VxD
MTRR_Service		 _MTRR_Get_Version, LOCAL
MTRR_StdCall_Service     MTRRSetPhysicalCacheTypeRange, 4
MTRR_Service		 MTRRIsPatSupported, LOCAL
MTRR_Service		 MTRR_PowerState_Change, LOCAL
End_Service_Table MTRR, VxD
endif
