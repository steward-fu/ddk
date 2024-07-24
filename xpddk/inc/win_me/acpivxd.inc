ifndef _ACPIVXD_H
_ACPIVXD_H	EQU	1
Begin_Service_Table ACPI, VxD
ACPI_Service	 ACPI_GetVersion, VxD_CODE
ACPI_Service	 _ACPI_Obsolete1, VxD_CODE
ACPI_Service     _ACPI_SetTimingMode, VxD_CODE
ACPI_Service     _ACPI_RegisterOpRegionCookedHandler, VxD_CODE
ACPI_Service	 _ACPI_Obsolete2, VxD_CODE
ACPI_Service	 _ACPI_GetTimingMode, VxD_CODE
ACPI_Service	 _ACPI_GetTaskFile, VxD_CODE
ACPI_Service	 _ACPI_WalkNameSpace, VxD_CODE
ACPI_Service	 _ACPI_GetObject, VxD_CODE
ACPI_Service	 _ACPI_NameSpaceToDevNode, VxD_CODE
ACPI_Service	 _ACPI_DevNodeToNameSpace, VxD_CODE
ACPI_Service	 _ACPI_RunControlMethod, VxD_CODE
ACPI_Service	 _ACPI_Obsolete3, VxD_CODE
ACPI_Service	 _ACPI_Obsolete4, VxD_CODE
ACPI_Service     _ACPI_Obsolete5, VxD_CODE
ACPI_Service	 _ACPI_EvalPackageElement, VxD_CODE
ACPI_Service	 _ACPI_EvalPkgDataElement, VxD_CODE
ACPI_Service	 _ACPI_FreeDataBuffs, VxD_CODE
ACPI_Service	 _ACPI_GetRootSystemDescriptorTable, VxD_CODE
ACPI_Service     _ACPI_Obsolete6, VxD_CODE
ACPI_Service	 _ACPI_GetNameSpaceObject, VxD_CODE
ACPI_Service     _ACPI_IdentifyDebuggerCommInfo, VxD_CODE
ACPI_Service     _ACPI_SetTimingModeEx, VxD_CODE
ACPI_Service     _ACPI_GetTimingModeEx, VxD_CODE
ACPI_Service     _ACPI_GetIDEPMComplianceLevel, VxD_CODE
ACPI_Service     _ACPI_FreeTaskFileBuffer, VxD_CODE
ACPI_Service     _ACPI_Obsolete7, VxD_CODE
ACPI_Service     _ACPI_Enter_Leave_Locked_Phase, VxD_CODE
End_Service_Table ACPI, VxD
ACPI_WALK_GET_PARENT	EQU	0
ACPI_WALK_GET_CHILD	EQU	1
ACPI_WALK_GET_SIBLING	EQU	2

TIMINGMODE	STRUC
tm_PIOSpeed	DD	?
tm_DMASpeed	DD	?
tm_Flags	DD	?
TIMINGMODE	ENDS

TIMINGMODEEX	STRUC
tm_PIOSpeed0	DD	?
tm_DMASpeed0	DD	?
tm_PIOSpeed1	DD	?
tm_DMASpeed1	DD	?
tm_Flags	DD	?
TIMINGMODEEX	ENDS
IDEPM_UNSUPPORTED	EQU	0
IDEPM_OLDSPEC	EQU	1
IDEPM_UPDATEDSPEC	EQU	2
endif
