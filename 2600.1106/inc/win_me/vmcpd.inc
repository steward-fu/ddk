ifndef	_VMCPD_H
_VMCPD_H	EQU	1
ifdef WANT_VMCPD_FLOATING_SAVE

_KFLOATING_SAVE	STRUC
ControlWord	DD	?
StatusWord	DD	?
ErrorOffset	DD	?
ErrorSelector	DD	?
DataOffset	DD	?
DataSelector	DD	?
Cr0NpxState	DD	?
Spare1	DD	?
_KFLOATING_SAVE	ENDS
endif
Begin_Service_Table VMCPD
VMCPD_Service	 VMCPD_Get_Version, LOCAL
VMCPD_Service	 VMCPD_Get_Virt_State, LOCAL
VMCPD_Service	 VMCPD_Set_Virt_State, LOCAL
VMCPD_Service	 VMCPD_Get_CR0_State, LOCAL
VMCPD_Service	 VMCPD_Set_CR0_State, LOCAL
VMCPD_Service	 VMCPD_Get_Thread_State, LOCAL
VMCPD_Service	 VMCPD_Set_Thread_State, LOCAL
VMCPD_Service	 _VMCPD_Get_FP_Instruction_Size, LOCAL
VMCPD_Service	 VMCPD_Set_Thread_Precision, LOCAL
VMCPD_Service    VMCPD_Init_FP, LOCAL
VMCPD_Service    _KeSaveFloatingPointState, LOCAL
VMCPD_Service    _KeRestoreFloatingPointState, LOCAL
VMCPD_Service	 VMCPD_Init_FP_State, LOCAL
End_Service_Table VMCPD
endif
