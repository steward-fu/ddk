//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  _XchgFreePhysReg
#define _XchgFreePhysReg LOCK__XchgFreePhysReg

#undef  XchgFreePhysReg
#define XchgFreePhysReg LOCK__XchgFreePhysReg

#undef  Wake_Up_VM
#define Wake_Up_VM LOCK_Wake_Up_VM

#undef  Wait_Semaphore
#define Wait_Semaphore LOCK_Wait_Semaphore

#undef  Validate_VM_Handle
#define Validate_VM_Handle LOCK_Validate_VM_Handle

#undef  Validate_Client_Ptr
#define Validate_Client_Ptr LOCK_Validate_Client_Ptr

#undef  Update_System_Clock
#define Update_System_Clock LOCK_Update_System_Clock

#undef  _UnmapFreePhysReg
#define _UnmapFreePhysReg LOCK__UnmapFreePhysReg

#undef  UnmapFreePhysReg
#define UnmapFreePhysReg LOCK__UnmapFreePhysReg

#undef  Unhook_Invalid_Page_Fault
#define Unhook_Invalid_Page_Fault LOCK_Unhook_Invalid_Page_Fault

#undef  Test_Sys_VM_Handle
#define Test_Sys_VM_Handle LOCK_Test_Sys_VM_Handle

#undef  _TestGlobalV86Mem
#define _TestGlobalV86Mem LOCK__TestGlobalV86Mem

#undef  TestGlobalV86Mem
#define TestGlobalV86Mem LOCK__TestGlobalV86Mem

#undef  Test_Debug_Installed
#define Test_Debug_Installed LOCK_Test_Debug_Installed

#undef  Test_DBCS_Lead_Byte
#define Test_DBCS_Lead_Byte LOCK_Test_DBCS_Lead_Byte

#undef  Test_Cur_VM_Handle
#define Test_Cur_VM_Handle LOCK_Test_Cur_VM_Handle

#undef  System_Control
#define System_Control LOCK_System_Control

#undef  Suspend_VM
#define Suspend_VM LOCK_Suspend_VM

#undef  Simulate_Push
#define Simulate_Push LOCK_Simulate_Push

#undef  Simulate_Pop
#define Simulate_Pop LOCK_Simulate_Pop

#undef  Simulate_Int
#define Simulate_Int LOCK_Simulate_Int

#undef  Simulate_Far_Ret_N
#define Simulate_Far_Ret_N LOCK_Simulate_Far_Ret_N

#undef  Simulate_Far_Jmp
#define Simulate_Far_Jmp LOCK_Simulate_Far_Jmp

#undef  Simulate_Far_Call
#define Simulate_Far_Call LOCK_Simulate_Far_Call

#undef  Signal_Semaphore
#define Signal_Semaphore LOCK_Signal_Semaphore

#undef  Set_VM_Time_Out
#define Set_VM_Time_Out LOCK_Set_VM_Time_Out

#undef  Set_V86_Int_Vector
#define Set_V86_Int_Vector LOCK_Set_V86_Int_Vector

#undef  Set_PM_Int_Vector
#define Set_PM_Int_Vector LOCK_Set_PM_Int_Vector

#undef  Set_Time_Slice_Priority
#define Set_Time_Slice_Priority LOCK_Set_Time_Slice_Priority

#undef  Set_Time_Slice_Granularity
#define Set_Time_Slice_Granularity LOCK_Set_Time_Slice_Granularity

#undef  Set_System_Exit_Code
#define Set_System_Exit_Code LOCK_Set_System_Exit_Code

#undef  _SetResetV86Pageable
#define _SetResetV86Pageable LOCK__SetResetV86Pageable

#undef  SetResetV86Pageable
#define SetResetV86Pageable LOCK__SetResetV86Pageable

#undef  Set_PM_Int_Type
#define Set_PM_Int_Type LOCK_Set_PM_Int_Type

#undef  Set_Physical_HMA_Alias
#define Set_Physical_HMA_Alias LOCK_Set_Physical_HMA_Alias

#undef  Set_NMI_Handler_Addr
#define Set_NMI_Handler_Addr LOCK_Set_NMI_Handler_Addr

#undef  Set_Mono_Cur_Pos
#define Set_Mono_Cur_Pos LOCK_Set_Mono_Cur_Pos

#undef  _SetLastV86Page
#define _SetLastV86Page LOCK__SetLastV86Page

#undef  SetLastV86Page
#define SetLastV86Page LOCK__SetLastV86Page

#undef  Set_Global_Time_Out
#define Set_Global_Time_Out LOCK_Set_Global_Time_Out

#undef  _SetFreePhysRegCalBk
#define _SetFreePhysRegCalBk LOCK__SetFreePhysRegCalBk

#undef  SetFreePhysRegCalBk
#define SetFreePhysRegCalBk LOCK__SetFreePhysRegCalBk

#undef  Set_Execution_Focus
#define Set_Execution_Focus LOCK_Set_Execution_Focus

#undef  _SetDescriptor
#define _SetDescriptor LOCK__SetDescriptor

#undef  SetDescriptor
#define SetDescriptor LOCK__SetDescriptor

#undef  Set_Delete_On_Exit_File
#define Set_Delete_On_Exit_File LOCK_Set_Delete_On_Exit_File

#undef  _SelectorMapFlat
#define _SelectorMapFlat LOCK__SelectorMapFlat

#undef  SelectorMapFlat
#define SelectorMapFlat LOCK__SelectorMapFlat

#undef  Schedule_VM_Event
#define Schedule_VM_Event LOCK_Schedule_VM_Event

#undef  Schedule_Global_Event
#define Schedule_Global_Event LOCK_Schedule_Global_Event

#undef  Save_Client_State
#define Save_Client_State LOCK_Save_Client_State

#undef  Resume_VM
#define Resume_VM LOCK_Resume_VM

#undef  Restore_Client_State
#define Restore_Client_State LOCK_Restore_Client_State

#undef  Remove_V86_Break_Point
#define Remove_V86_Break_Point LOCK_Remove_V86_Break_Point

#undef  Remove_Exception_Handler
#define Remove_Exception_Handler LOCK_Remove_Exception_Handler

#undef  Release_Critical_Section
#define Release_Critical_Section LOCK_Release_Critical_Section

#undef  Queue_Debug_String
#define Queue_Debug_String LOCK_Queue_Debug_String

#undef  _PhysIntoV86
#define _PhysIntoV86 LOCK__PhysIntoV86

#undef  PhysIntoV86
#define PhysIntoV86 LOCK__PhysIntoV86

#undef  _PageUnLock
#define _PageUnLock LOCK__PageUnLock

#undef  PageUnLock
#define PageUnLock LOCK__PageUnLock

#undef  _PageResetHandlePAddr
#define _PageResetHandlePAddr LOCK__PageResetHandlePAddr

#undef  PageResetHandlePAddr
#define PageResetHandlePAddr LOCK__PageResetHandlePAddr

#undef  _PageReAllocate
#define _PageReAllocate LOCK__PageReAllocate

#undef  PageReAllocate
#define PageReAllocate LOCK__PageReAllocate

#undef  _PageOutDirtyPages
#define _PageOutDirtyPages LOCK__PageOutDirtyPages

#undef  PageOutDirtyPages
#define PageOutDirtyPages LOCK__PageOutDirtyPages

#undef  _PageLock
#define _PageLock LOCK__PageLock

#undef  PageLock
#define PageLock LOCK__PageLock

#undef  _PageGetSizeAddr
#define _PageGetSizeAddr LOCK__PageGetSizeAddr

#undef  PageGetSizeAddr
#define PageGetSizeAddr LOCK__PageGetSizeAddr

#undef  _PageGetAllocInfo
#define _PageGetAllocInfo LOCK__PageGetAllocInfo

#undef  PageGetAllocInfo
#define PageGetAllocInfo LOCK__PageGetAllocInfo

#undef  _PageFree
#define _PageFree LOCK__PageFree

#undef  PageFree
#define PageFree LOCK__PageFree

#undef  _PageDiscardPages
#define _PageDiscardPages LOCK__PageDiscardPages

#undef  PageDiscardPages
#define PageDiscardPages LOCK__PageDiscardPages

#undef  _PageCheckLinRange
#define _PageCheckLinRange LOCK__PageCheckLinRange

#undef  PageCheckLinRange
#define PageCheckLinRange LOCK__PageCheckLinRange

#undef  _PageAllocate
#define _PageAllocate LOCK__PageAllocate

#undef  PageAllocate
#define PageAllocate LOCK__PageAllocate

#undef  Out_Mono_String
#define Out_Mono_String LOCK_Out_Mono_String

#undef  Out_Mono_Chr_Attrs
#define Out_Mono_Chr_Attrs LOCK_Out_Mono_Chr_Attrs

#undef  Out_Mono_Chr
#define Out_Mono_Chr LOCK_Out_Mono_Chr

#undef  Out_Debug_String
#define Out_Debug_String LOCK_Out_Debug_String

#undef  Out_Debug_Chr
#define Out_Debug_Chr LOCK_Out_Debug_Chr

#undef  OpenFile
#define OpenFile LOCK_OpenFile

#undef  Nuke_VM
#define Nuke_VM LOCK_Nuke_VM

#undef  No_Fail_Resume_VM
#define No_Fail_Resume_VM LOCK_No_Fail_Resume_VM

#undef  _ModifyPageBits
#define _ModifyPageBits LOCK__ModifyPageBits

#undef  ModifyPageBits
#define ModifyPageBits LOCK__ModifyPageBits

#undef  _MMGR_Toggle_HMA
#define _MMGR_Toggle_HMA LOCK__MMGR_Toggle_HMA

#undef  MMGR_Toggle_HMA
#define MMGR_Toggle_HMA LOCK__MMGR_Toggle_HMA

#undef  MMGR_SetNULPageAddr
#define MMGR_SetNULPageAddr LOCK_MMGR_SetNULPageAddr

#undef  _MapPhysToLinear
#define _MapPhysToLinear LOCK__MapPhysToLinear

#undef  MapPhysToLinear
#define MapPhysToLinear LOCK__MapPhysToLinear

#undef  Map_Lin_To_VM_Addr
#define Map_Lin_To_VM_Addr LOCK_Map_Lin_To_VM_Addr

#undef  _MapIntoV86
#define _MapIntoV86 LOCK__MapIntoV86

#undef  MapIntoV86
#define MapIntoV86 LOCK__MapIntoV86

#undef  _MapFreePhysReg
#define _MapFreePhysReg LOCK__MapFreePhysReg

#undef  MapFreePhysReg
#define MapFreePhysReg LOCK__MapFreePhysReg

#undef  Map_Flat
#define Map_Flat LOCK_Map_Flat

#undef  Locate_Byte_In_ROM
#define Locate_Byte_In_ROM LOCK_Locate_Byte_In_ROM

#undef  List_Remove_First
#define List_Remove_First LOCK_List_Remove_First

#undef  List_Remove
#define List_Remove LOCK_List_Remove

#undef  List_Insert
#define List_Insert LOCK_List_Insert

#undef  List_Get_Next
#define List_Get_Next LOCK_List_Get_Next

#undef  List_Get_First
#define List_Get_First LOCK_List_Get_First

#undef  List_Destroy
#define List_Destroy LOCK_List_Destroy

#undef  List_Deallocate
#define List_Deallocate LOCK_List_Deallocate

#undef  List_Create
#define List_Create LOCK_List_Create

#undef  List_Attach_Tail
#define List_Attach_Tail LOCK_List_Attach_Tail

#undef  List_Attach
#define List_Attach LOCK_List_Attach

#undef  List_Allocate
#define List_Allocate LOCK_List_Allocate

#undef  _LinPageUnLock
#define _LinPageUnLock LOCK__LinPageUnLock

#undef  LinPageUnLock
#define LinPageUnLock LOCK__LinPageUnLock

#undef  _LinPageLock
#define _LinPageLock LOCK__LinPageLock

#undef  LinPageLock
#define LinPageLock LOCK__LinPageLock

#undef  _LinMapIntoV86
#define _LinMapIntoV86 LOCK__LinMapIntoV86

#undef  LinMapIntoV86
#define LinMapIntoV86 LOCK__LinMapIntoV86

#undef  Is_Debug_Chr
#define Is_Debug_Chr LOCK_Is_Debug_Chr

#undef  Install_V86_Break_Point
#define Install_V86_Break_Point LOCK_Install_V86_Break_Point

#undef  Install_IO_Handler
#define Install_IO_Handler LOCK_Install_IO_Handler

#undef  In_Debug_Chr
#define In_Debug_Chr LOCK_In_Debug_Chr

#undef  Hook_VMM_Fault
#define Hook_VMM_Fault LOCK_Hook_VMM_Fault

#undef  Hook_V86_Page
#define Hook_V86_Page LOCK_Hook_V86_Page

#undef  Hook_V86_Int_Chain
#define Hook_V86_Int_Chain LOCK_Hook_V86_Int_Chain

#undef  Hook_V86_Fault
#define Hook_V86_Fault LOCK_Hook_V86_Fault

#undef  Hook_PM_Fault
#define Hook_PM_Fault LOCK_Hook_PM_Fault

#undef  Hook_Device_PM_API
#define Hook_Device_PM_API LOCK_Hook_Device_PM_API

#undef  Hook_NMI_Event
#define Hook_NMI_Event LOCK_Hook_NMI_Event

#undef  Hook_Invalid_Page_Fault
#define Hook_Invalid_Page_Fault LOCK_Hook_Invalid_Page_Fault

#undef  Hook_Device_V86_API
#define Hook_Device_V86_API LOCK_Hook_Device_V86_API

#undef  Hook_Device_Service
#define Hook_Device_Service LOCK_Hook_Device_Service

#undef  _HeapReAllocate
#define _HeapReAllocate LOCK__HeapReAllocate

#undef  HeapReAllocate
#define HeapReAllocate LOCK__HeapReAllocate

#undef  _HeapGetSize
#define _HeapGetSize LOCK__HeapGetSize

#undef  HeapGetSize
#define HeapGetSize LOCK__HeapGetSize

#undef  _HeapFree
#define _HeapFree LOCK__HeapFree

#undef  HeapFree
#define HeapFree LOCK__HeapFree

#undef  _HeapAllocate
#define _HeapAllocate LOCK__HeapAllocate

#undef  HeapAllocate
#define HeapAllocate LOCK__HeapAllocate

#undef  _GetVMPgCount
#define _GetVMPgCount LOCK__GetVMPgCount

#undef  GetVMPgCount
#define GetVMPgCount LOCK__GetVMPgCount

#undef  Get_VMM_Version
#define Get_VMM_Version LOCK_Get_VMM_Version

#undef  Get_VMM_Reenter_Count
#define Get_VMM_Reenter_Count LOCK_Get_VMM_Reenter_Count

#undef  Get_VM_Exec_Time
#define Get_VM_Exec_Time LOCK_Get_VM_Exec_Time

#undef  _GetV86PageableArray
#define _GetV86PageableArray LOCK__GetV86PageableArray

#undef  GetV86PageableArray
#define GetV86PageableArray LOCK__GetV86PageableArray

#undef  Get_V86_Int_Vector
#define Get_V86_Int_Vector LOCK_Get_V86_Int_Vector

#undef  Get_Time_Slice_Priority
#define Get_Time_Slice_Priority LOCK_Get_Time_Slice_Priority

#undef  Get_Time_Slice_Info
#define Get_Time_Slice_Info LOCK_Get_Time_Slice_Info

#undef  Get_Time_Slice_Granularity
#define Get_Time_Slice_Granularity LOCK_Get_Time_Slice_Granularity

#undef  Get_Sys_VM_Handle
#define Get_Sys_VM_Handle LOCK_Get_Sys_VM_Handle

#undef  Get_System_Time
#define Get_System_Time LOCK_Get_System_Time

#undef  _GetSetPageOutCount
#define _GetSetPageOutCount LOCK__GetSetPageOutCount

#undef  GetSetPageOutCount
#define GetSetPageOutCount LOCK__GetSetPageOutCount

#undef  _GetSysPageCount
#define _GetSysPageCount LOCK__GetSysPageCount

#undef  GetSysPageCount
#define GetSysPageCount LOCK__GetSysPageCount

#undef  Set_HMA_Info
#define Set_HMA_Info LOCK_Set_HMA_Info

#undef  Get_HMA_Info
#define Get_HMA_Info LOCK_Get_HMA_Info

#undef  GetDetailedVMError
#define GetDetailedVMError LOCK_GetDetailedVMError

#undef  SetDetailedVMError
#define SetDetailedVMError LOCK_SetDetailedVMError

#undef  Get_PSP_Segment
#define Get_PSP_Segment LOCK_Get_PSP_Segment

#undef  Get_Profile_String
#define Get_Profile_String LOCK_Get_Profile_String

#undef  Get_Profile_Hex_Int
#define Get_Profile_Hex_Int LOCK_Get_Profile_Hex_Int

#undef  Get_Profile_Fixed_Point
#define Get_Profile_Fixed_Point LOCK_Get_Profile_Fixed_Point

#undef  Get_Profile_Decimal_Int
#define Get_Profile_Decimal_Int LOCK_Get_Profile_Decimal_Int

#undef  Get_Profile_Boolean
#define Get_Profile_Boolean LOCK_Get_Profile_Boolean

#undef  Get_PM_Int_Vector
#define Get_PM_Int_Vector LOCK_Get_PM_Int_Vector

#undef  Get_PM_Int_Type
#define Get_PM_Int_Type LOCK_Get_PM_Int_Type

#undef  _GetNulPageHandle
#define _GetNulPageHandle LOCK__GetNulPageHandle

#undef  GetNulPageHandle
#define GetNulPageHandle LOCK__GetNulPageHandle

#undef  Get_NMI_Handler_Addr
#define Get_NMI_Handler_Addr LOCK_Get_NMI_Handler_Addr

#undef  Get_Next_VM_Handle
#define Get_Next_VM_Handle LOCK_Get_Next_VM_Handle

#undef  Get_Next_Profile_String
#define Get_Next_Profile_String LOCK_Get_Next_Profile_String

#undef  Get_Name_Of_Ugly_TSR
#define Get_Name_Of_Ugly_TSR LOCK_Get_Name_Of_Ugly_TSR

#undef  Get_Mono_Cur_Pos
#define Get_Mono_Cur_Pos LOCK_Get_Mono_Cur_Pos

#undef  Get_Mono_Chr
#define Get_Mono_Chr LOCK_Get_Mono_Chr

#undef  Get_Machine_Info
#define Get_Machine_Info LOCK_Get_Machine_Info

#undef  Get_Last_Updated_VM_Exec_Time
#define Get_Last_Updated_VM_Exec_Time LOCK_Get_Last_Updated_VM_Exec_Time

#undef  _GetLastV86Page
#define _GetLastV86Page LOCK__GetLastV86Page

#undef  GetLastV86Page
#define GetLastV86Page LOCK__GetLastV86Page

#undef  Get_Last_Updated_System_Time
#define Get_Last_Updated_System_Time LOCK_Get_Last_Updated_System_Time

#undef  _GetGlblRng0V86IntBase
#define _GetGlblRng0V86IntBase LOCK__GetGlblRng0V86IntBase

#undef  GetGlblRng0V86IntBase
#define GetGlblRng0V86IntBase LOCK__GetGlblRng0V86IntBase

#undef  _GetFreePageCount
#define _GetFreePageCount LOCK__GetFreePageCount

#undef  GetFreePageCount
#define GetFreePageCount LOCK__GetFreePageCount

#undef  Get_Fault_Hook_Addrs
#define Get_Fault_Hook_Addrs LOCK_Get_Fault_Hook_Addrs

#undef  Get_Exec_Path
#define Get_Exec_Path LOCK_Get_Exec_Path

#undef  Get_Execution_Focus
#define Get_Execution_Focus LOCK_Get_Execution_Focus

#undef  Get_Environment_String
#define Get_Environment_String LOCK_Get_Environment_String

#undef  _Get_Device_V86_Pages_Array
#define _Get_Device_V86_Pages_Array LOCK__Get_Device_V86_Pages_Array

#undef  Get_Device_V86_Pages_Array
#define Get_Device_V86_Pages_Array LOCK__Get_Device_V86_Pages_Array

#undef  _GetDescriptor
#define _GetDescriptor LOCK__GetDescriptor

#undef  GetDescriptor
#define GetDescriptor LOCK__GetDescriptor

#undef  _GetDemandPageInfo
#define _GetDemandPageInfo LOCK__GetDemandPageInfo

#undef  GetDemandPageInfo
#define GetDemandPageInfo LOCK__GetDemandPageInfo

#undef  Get_Debug_Options
#define Get_Debug_Options LOCK_Get_Debug_Options

#undef  Get_Cur_VM_Handle
#define Get_Cur_VM_Handle LOCK_Get_Cur_VM_Handle

#undef  Get_Crit_Status_No_Block
#define Get_Crit_Status_No_Block LOCK_Get_Crit_Status_No_Block

#undef  Get_Crit_Section_Status
#define Get_Crit_Section_Status LOCK_Get_Crit_Section_Status

#undef  Get_Cur_PM_App_CB
#define Get_Cur_PM_App_CB LOCK_Get_Cur_PM_App_CB

#undef  Get_Config_Directory
#define Get_Config_Directory LOCK_Get_Config_Directory

#undef  _GetFirstV86Page
#define _GetFirstV86Page LOCK__GetFirstV86Page

#undef  GetFirstV86Page
#define GetFirstV86Page LOCK__GetFirstV86Page

#undef  _GetAppFlatDSAlias
#define _GetAppFlatDSAlias LOCK__GetAppFlatDSAlias

#undef  GetAppFlatDSAlias
#define GetAppFlatDSAlias LOCK__GetAppFlatDSAlias

#undef  _Free_Temp_V86_Data_Area
#define _Free_Temp_V86_Data_Area LOCK__Free_Temp_V86_Data_Area

#undef  Free_Temp_V86_Data_Area
#define Free_Temp_V86_Data_Area LOCK__Free_Temp_V86_Data_Area

#undef  _Free_LDT_Selector
#define _Free_LDT_Selector LOCK__Free_LDT_Selector

#undef  Free_LDT_Selector
#define Free_LDT_Selector LOCK__Free_LDT_Selector

#undef  _Free_GDT_Selector
#define _Free_GDT_Selector LOCK__Free_GDT_Selector

#undef  Free_GDT_Selector
#define Free_GDT_Selector LOCK__Free_GDT_Selector

#undef  Fatal_Error_Handler
#define Fatal_Error_Handler LOCK_Fatal_Error_Handler

#undef  Exec_VxD_Int
#define Exec_VxD_Int LOCK_Exec_VxD_Int

#undef  Exec_Int
#define Exec_Int LOCK_Exec_Int

#undef  End_Reentrant_Execution
#define End_Reentrant_Execution LOCK_End_Reentrant_Execution

#undef  End_Crit_And_Suspend
#define End_Crit_And_Suspend LOCK_End_Crit_And_Suspend

#undef  Enable_Local_Trapping
#define Enable_Local_Trapping LOCK_Enable_Local_Trapping

#undef  Enable_Global_Trapping
#define Enable_Global_Trapping LOCK_Enable_Global_Trapping

#undef  Disable_Local_Trapping
#define Disable_Local_Trapping LOCK_Disable_Local_Trapping

#undef  Disable_Global_Trapping
#define Disable_Global_Trapping LOCK_Disable_Global_Trapping

#undef  Destroy_Semaphore
#define Destroy_Semaphore LOCK_Destroy_Semaphore

#undef  _DeAssign_Device_V86_Pages
#define _DeAssign_Device_V86_Pages LOCK__DeAssign_Device_V86_Pages

#undef  DeAssign_Device_V86_Pages
#define DeAssign_Device_V86_Pages LOCK__DeAssign_Device_V86_Pages

#undef  Close_VM
#define Close_VM LOCK_Close_VM

#undef  Convert_Boolean_String
#define Convert_Boolean_String LOCK_Convert_Boolean_String

#undef  Convert_Decimal_String
#define Convert_Decimal_String LOCK_Convert_Decimal_String

#undef  Convert_Fixed_Point_String
#define Convert_Fixed_Point_String LOCK_Convert_Fixed_Point_String

#undef  Convert_Hex_String
#define Convert_Hex_String LOCK_Convert_Hex_String

#undef  _CopyPageTable
#define _CopyPageTable LOCK__CopyPageTable

#undef  CopyPageTable
#define CopyPageTable LOCK__CopyPageTable

#undef  Create_Semaphore
#define Create_Semaphore LOCK_Create_Semaphore

#undef  _AddFreePhysPage
#define _AddFreePhysPage LOCK__AddFreePhysPage

#undef  AddFreePhysPage
#define AddFreePhysPage LOCK__AddFreePhysPage

#undef  _Add_Global_V86_Data_Area
#define _Add_Global_V86_Data_Area LOCK__Add_Global_V86_Data_Area

#undef  Add_Global_V86_Data_Area
#define Add_Global_V86_Data_Area LOCK__Add_Global_V86_Data_Area

#undef  _AddInstanceItem
#define _AddInstanceItem LOCK__AddInstanceItem

#undef  AddInstanceItem
#define AddInstanceItem LOCK__AddInstanceItem

#undef  Adjust_Exec_Priority
#define Adjust_Exec_Priority LOCK_Adjust_Exec_Priority

#undef  Adjust_Execution_Time
#define Adjust_Execution_Time LOCK_Adjust_Execution_Time

#undef  _Allocate_Device_CB_Area
#define _Allocate_Device_CB_Area LOCK__Allocate_Device_CB_Area

#undef  Allocate_Device_CB_Area
#define Allocate_Device_CB_Area LOCK__Allocate_Device_CB_Area

#undef  _Allocate_GDT_Selector
#define _Allocate_GDT_Selector LOCK__Allocate_GDT_Selector

#undef  Allocate_GDT_Selector
#define Allocate_GDT_Selector LOCK__Allocate_GDT_Selector

#undef  _Allocate_Global_V86_Data_Area
#define _Allocate_Global_V86_Data_Area LOCK__Allocate_Global_V86_Data_Area

#undef  Allocate_Global_V86_Data_Area
#define Allocate_Global_V86_Data_Area LOCK__Allocate_Global_V86_Data_Area

#undef  _Allocate_LDT_Selector
#define _Allocate_LDT_Selector LOCK__Allocate_LDT_Selector

#undef  Allocate_LDT_Selector
#define Allocate_LDT_Selector LOCK__Allocate_LDT_Selector

#undef  Allocate_PM_Call_Back
#define Allocate_PM_Call_Back LOCK_Allocate_PM_Call_Back

#undef  _Allocate_Temp_V86_Data_Area
#define _Allocate_Temp_V86_Data_Area LOCK__Allocate_Temp_V86_Data_Area

#undef  Allocate_Temp_V86_Data_Area
#define Allocate_Temp_V86_Data_Area LOCK__Allocate_Temp_V86_Data_Area

#undef  Allocate_V86_Call_Back
#define Allocate_V86_Call_Back LOCK_Allocate_V86_Call_Back

#undef  _Assign_Device_V86_Pages
#define _Assign_Device_V86_Pages LOCK__Assign_Device_V86_Pages

#undef  Assign_Device_V86_Pages
#define Assign_Device_V86_Pages LOCK__Assign_Device_V86_Pages

#undef  Begin_Critical_Section
#define Begin_Critical_Section LOCK_Begin_Critical_Section

#undef  Begin_Reentrant_Execution
#define Begin_Reentrant_Execution LOCK_Begin_Reentrant_Execution

#undef  _BuildDescriptorDWORDs
#define _BuildDescriptorDWORDs LOCK__BuildDescriptorDWORDs

#undef  BuildDescriptorDWORDs
#define BuildDescriptorDWORDs LOCK__BuildDescriptorDWORDs

#undef  Build_Int_Stack_Frame
#define Build_Int_Stack_Frame LOCK_Build_Int_Stack_Frame

#undef  Call_Global_Event
#define Call_Global_Event LOCK_Call_Global_Event

#undef  Call_Priority_VM_Event
#define Call_Priority_VM_Event LOCK_Call_Priority_VM_Event

#undef  Call_VM_Event
#define Call_VM_Event LOCK_Call_VM_Event

#undef  Call_When_Idle
#define Call_When_Idle LOCK_Call_When_Idle

#undef  Call_When_Not_Critical
#define Call_When_Not_Critical LOCK_Call_When_Not_Critical

#undef  Call_When_Task_Switched
#define Call_When_Task_Switched LOCK_Call_When_Task_Switched

#undef  Call_When_VM_Ints_Enabled
#define Call_When_VM_Ints_Enabled LOCK_Call_When_VM_Ints_Enabled

#undef  Call_When_VM_Returns
#define Call_When_VM_Returns LOCK_Call_When_VM_Returns

#undef  Cancel_Global_Event
#define Cancel_Global_Event LOCK_Cancel_Global_Event

#undef  Cancel_Priority_VM_Event
#define Cancel_Priority_VM_Event LOCK_Cancel_Priority_VM_Event

#undef  Cancel_Time_Out
#define Cancel_Time_Out LOCK_Cancel_Time_Out

#undef  Cancel_VM_Event
#define Cancel_VM_Event LOCK_Cancel_VM_Event

#undef  Claim_Critical_Section
#define Claim_Critical_Section LOCK_Claim_Critical_Section

#ifdef WIN40_OR_LATER
#undef  _RegOpenKey
#define _RegOpenKey LOCK__RegOpenKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegOpenKey
#define RegOpenKey LOCK__RegOpenKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCreateKey
#define _RegCreateKey LOCK__RegCreateKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCreateKey
#define RegCreateKey LOCK__RegCreateKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegDeleteKey
#define _RegDeleteKey LOCK__RegDeleteKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegDeleteKey
#define RegDeleteKey LOCK__RegDeleteKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegDeleteValue
#define _RegDeleteValue LOCK__RegDeleteValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegDeleteValue
#define RegDeleteValue LOCK__RegDeleteValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegEnumKey
#define _RegEnumKey LOCK__RegEnumKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegEnumKey
#define RegEnumKey LOCK__RegEnumKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegEnumValue
#define _RegEnumValue LOCK__RegEnumValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegEnumValue
#define RegEnumValue LOCK__RegEnumValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegFlushKey
#define _RegFlushKey LOCK__RegFlushKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegFlushKey
#define RegFlushKey LOCK__RegFlushKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCloseKey
#define _RegCloseKey LOCK__RegCloseKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCloseKey
#define RegCloseKey LOCK__RegCloseKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryValue
#define _RegQueryValue LOCK__RegQueryValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryValue
#define RegQueryValue LOCK__RegQueryValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryValueEx
#define _RegQueryValueEx LOCK__RegQueryValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryValueEx
#define RegQueryValueEx LOCK__RegQueryValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  _RegSetValue
#define _RegSetValue LOCK__RegSetValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegSetValue
#define RegSetValue LOCK__RegSetValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegSetValueEx
#define _RegSetValueEx LOCK__RegSetValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  RegSetValueEx
#define RegSetValueEx LOCK__RegSetValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  _EnterMutex
#define _EnterMutex LOCK__EnterMutex
#endif

#ifdef WIN40_OR_LATER
#undef  EnterMutex
#define EnterMutex LOCK__EnterMutex
#endif

#ifdef WIN40_OR_LATER
#undef  _GetMutexOwner
#define _GetMutexOwner LOCK__GetMutexOwner
#endif

#ifdef WIN40_OR_LATER
#undef  GetMutexOwner
#define GetMutexOwner LOCK__GetMutexOwner
#endif

#ifdef WIN40_OR_LATER
#undef  _LeaveMutex
#define _LeaveMutex LOCK__LeaveMutex
#endif

#ifdef WIN40_OR_LATER
#undef  LeaveMutex
#define LeaveMutex LOCK__LeaveMutex
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Fault
#define Unhook_V86_Fault LOCK_Unhook_V86_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_PM_Fault
#define Unhook_PM_Fault LOCK_Unhook_PM_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_VMM_Fault
#define Unhook_VMM_Fault LOCK_Unhook_VMM_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Int_Chain
#define Unhook_V86_Int_Chain LOCK_Unhook_V86_Int_Chain
#endif

#ifdef WIN40_OR_LATER
#undef  Signal_Semaphore_No_Switch
#define Signal_Semaphore_No_Switch LOCK_Signal_Semaphore_No_Switch
#endif

#ifdef WIN40_OR_LATER
#undef  Remove_IO_Handler
#define Remove_IO_Handler LOCK_Remove_IO_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_NMI_Event
#define Unhook_NMI_Event LOCK_Unhook_NMI_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Not_Critical
#define Cancel_Call_When_Not_Critical LOCK_Cancel_Call_When_Not_Critical
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service
#define Unhook_Device_Service LOCK_Unhook_Device_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Async_Time_Out
#define Set_Async_Time_Out LOCK_Set_Async_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  Get_DDB
#define Get_DDB LOCK_Get_DDB
#endif

#ifdef WIN40_OR_LATER
#undef  _Trace_Out_Service
#define _Trace_Out_Service LOCK__Trace_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Trace_Out_Service
#define Trace_Out_Service LOCK__Trace_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Out_Service
#define _Debug_Out_Service LOCK__Debug_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Out_Service
#define Debug_Out_Service LOCK__Debug_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _AllocateThreadDataSlot
#define _AllocateThreadDataSlot LOCK__AllocateThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  AllocateThreadDataSlot
#define AllocateThreadDataSlot LOCK__AllocateThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  _FreeThreadDataSlot
#define _FreeThreadDataSlot LOCK__FreeThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  FreeThreadDataSlot
#define FreeThreadDataSlot LOCK__FreeThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  _GetLastUpdatedThreadExecTime
#define _GetLastUpdatedThreadExecTime LOCK__GetLastUpdatedThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  GetLastUpdatedThreadExecTime
#define GetLastUpdatedThreadExecTime LOCK__GetLastUpdatedThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  VMMAddImportModuleName
#define VMMAddImportModuleName LOCK_VMMAddImportModuleName
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_Add_DDB
#define VMM_Add_DDB LOCK_VMM_Add_DDB
#endif

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
#undef  VMM_Remove_DDB
#define VMM_Remove_DDB LOCK_VMM_Remove_DDB
#endif

#ifdef WIN40_OR_LATER
#undef  Test_VM_Ints_Enabled
#define Test_VM_Ints_Enabled LOCK_Test_VM_Ints_Enabled
#endif

#ifdef WIN40_OR_LATER
#undef  _BlockOnID
#define _BlockOnID LOCK__BlockOnID
#endif

#ifdef WIN40_OR_LATER
#undef  BlockOnID
#define BlockOnID LOCK__BlockOnID
#endif

#ifdef WIN40_OR_LATER
#undef  Schedule_Thread_Event
#define Schedule_Thread_Event LOCK_Schedule_Thread_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Thread_Event
#define Cancel_Thread_Event LOCK_Cancel_Thread_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Time_Out
#define Set_Thread_Time_Out LOCK_Set_Thread_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  _CreateMutex
#define _CreateMutex LOCK__CreateMutex
#endif

#ifdef WIN40_OR_LATER
#undef  CreateMutex
#define CreateMutex LOCK__CreateMutex
#endif

#ifdef WIN40_OR_LATER
#undef  _DestroyMutex
#define _DestroyMutex LOCK__DestroyMutex
#endif

#ifdef WIN40_OR_LATER
#undef  DestroyMutex
#define DestroyMutex LOCK__DestroyMutex
#endif

#ifdef WIN40_OR_LATER
#undef  Call_When_Thread_Switched
#define Call_When_Thread_Switched LOCK_Call_When_Thread_Switched
#endif

#ifdef WIN40_OR_LATER
#undef  VMMCreateThread
#define VMMCreateThread LOCK_VMMCreateThread
#endif

#ifdef WIN40_OR_LATER
#undef  _GetThreadExecTime
#define _GetThreadExecTime LOCK__GetThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  GetThreadExecTime
#define GetThreadExecTime LOCK__GetThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  VMMTerminateThread
#define VMMTerminateThread LOCK_VMMTerminateThread
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Cur_Thread_Handle
#define Get_Cur_Thread_Handle LOCK_Get_Cur_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Cur_Thread_Handle
#define Test_Cur_Thread_Handle LOCK_Test_Cur_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Sys_Thread_Handle
#define Get_Sys_Thread_Handle LOCK_Get_Sys_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Sys_Thread_Handle
#define Test_Sys_Thread_Handle LOCK_Test_Sys_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Validate_Thread_Handle
#define Validate_Thread_Handle LOCK_Validate_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Initial_Thread_Handle
#define Get_Initial_Thread_Handle LOCK_Get_Initial_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Initial_Thread_Handle
#define Test_Initial_Thread_Handle LOCK_Test_Initial_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Valid_Thread_Handle
#define Debug_Test_Valid_Thread_Handle LOCK_Debug_Test_Valid_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Cur_Thread
#define Debug_Test_Cur_Thread LOCK_Debug_Test_Cur_Thread
#endif

#undef  VMM_GetSystemInitState
#define VMM_GetSystemInitState LOCK_VMM_GetSystemInitState

#undef  Cancel_Call_When_Thread_Switched
#define Cancel_Call_When_Thread_Switched LOCK_Cancel_Call_When_Thread_Switched

#undef  Get_Next_Thread_Handle
#define Get_Next_Thread_Handle LOCK_Get_Next_Thread_Handle

#ifdef WIN40_OR_LATER
#undef  Adjust_Thread_Exec_Priority
#define Adjust_Thread_Exec_Priority LOCK_Adjust_Thread_Exec_Priority
#endif

#ifdef WIN40_OR_LATER
#undef  _Deallocate_Device_CB_Area
#define _Deallocate_Device_CB_Area LOCK__Deallocate_Device_CB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  Deallocate_Device_CB_Area
#define Deallocate_Device_CB_Area LOCK__Deallocate_Device_CB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  Remove_Mult_IO_Handlers
#define Remove_Mult_IO_Handlers LOCK_Remove_Mult_IO_Handlers
#endif

#ifdef WIN40_OR_LATER
#undef  _PageReserve
#define _PageReserve LOCK__PageReserve
#endif

#ifdef WIN40_OR_LATER
#undef  PageReserve
#define PageReserve LOCK__PageReserve
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommit
#define _PageCommit LOCK__PageCommit
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommit
#define PageCommit LOCK__PageCommit
#endif

#ifdef WIN40_OR_LATER
#undef  _PageDecommit
#define _PageDecommit LOCK__PageDecommit
#endif

#ifdef WIN40_OR_LATER
#undef  PageDecommit
#define PageDecommit LOCK__PageDecommit
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerRegister
#define _PagerRegister LOCK__PagerRegister
#endif

#ifdef WIN40_OR_LATER
#undef  PagerRegister
#define PagerRegister LOCK__PagerRegister
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerQuery
#define _PagerQuery LOCK__PagerQuery
#endif

#ifdef WIN40_OR_LATER
#undef  PagerQuery
#define PagerQuery LOCK__PagerQuery
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerDeregister
#define _PagerDeregister LOCK__PagerDeregister
#endif

#ifdef WIN40_OR_LATER
#undef  PagerDeregister
#define PagerDeregister LOCK__PagerDeregister
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextCreate
#define _ContextCreate LOCK__ContextCreate
#endif

#ifdef WIN40_OR_LATER
#undef  ContextCreate
#define ContextCreate LOCK__ContextCreate
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextDestroy
#define _ContextDestroy LOCK__ContextDestroy
#endif

#ifdef WIN40_OR_LATER
#undef  ContextDestroy
#define ContextDestroy LOCK__ContextDestroy
#endif

#ifdef WIN40_OR_LATER
#undef  _PageAttach
#define _PageAttach LOCK__PageAttach
#endif

#ifdef WIN40_OR_LATER
#undef  PageAttach
#define PageAttach LOCK__PageAttach
#endif

#ifdef WIN40_OR_LATER
#undef  _PageFlush
#define _PageFlush LOCK__PageFlush
#endif

#ifdef WIN40_OR_LATER
#undef  PageFlush
#define PageFlush LOCK__PageFlush
#endif

#ifdef WIN40_OR_LATER
#undef  _SignalID
#define _SignalID LOCK__SignalID
#endif

#ifdef WIN40_OR_LATER
#undef  SignalID
#define SignalID LOCK__SignalID
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommitPhys
#define _PageCommitPhys LOCK__PageCommitPhys
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommitPhys
#define PageCommitPhys LOCK__PageCommitPhys
#endif

#ifdef WIN40_OR_LATER
#undef  _Register_Win32_Services
#define _Register_Win32_Services LOCK__Register_Win32_Services
#endif

#ifdef WIN40_OR_LATER
#undef  Register_Win32_Services
#define Register_Win32_Services LOCK__Register_Win32_Services
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Idle
#define Cancel_Call_When_Idle LOCK_Cancel_Call_When_Idle
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Task_Switched
#define Cancel_Call_When_Task_Switched LOCK_Cancel_Call_When_Task_Switched
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Inst_V86_Int_Vec_Base
#define Get_Inst_V86_Int_Vec_Base LOCK_Get_Inst_V86_Int_Vec_Base
#endif

#ifdef WIN40_OR_LATER
#undef  Simulate_VM_IO
#define Simulate_VM_IO LOCK_Simulate_VM_IO
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextSwitch
#define _ContextSwitch LOCK__ContextSwitch
#endif

#ifdef WIN40_OR_LATER
#undef  ContextSwitch
#define ContextSwitch LOCK__ContextSwitch
#endif

#ifdef WIN40_OR_LATER
#undef  _PageModifyPermissions
#define _PageModifyPermissions LOCK__PageModifyPermissions
#endif

#ifdef WIN40_OR_LATER
#undef  PageModifyPermissions
#define PageModifyPermissions LOCK__PageModifyPermissions
#endif

#ifdef WIN40_OR_LATER
#undef  _PageQuery
#define _PageQuery LOCK__PageQuery
#endif

#ifdef WIN40_OR_LATER
#undef  PageQuery
#define PageQuery LOCK__PageQuery
#endif

#ifdef WIN40_OR_LATER
#undef  _GetThreadTerminationStatus
#define _GetThreadTerminationStatus LOCK__GetThreadTerminationStatus
#endif

#ifdef WIN40_OR_LATER
#undef  GetThreadTerminationStatus
#define GetThreadTerminationStatus LOCK__GetThreadTerminationStatus
#endif

#ifdef WIN40_OR_LATER
#undef  _GetInstanceInfo
#define _GetInstanceInfo LOCK__GetInstanceInfo
#endif

#ifdef WIN40_OR_LATER
#undef  GetInstanceInfo
#define GetInstanceInfo LOCK__GetInstanceInfo
#endif

#ifdef WIN40_OR_LATER
#undef  _ExecIntMustComplete
#define _ExecIntMustComplete LOCK__ExecIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ExecIntMustComplete
#define ExecIntMustComplete LOCK__ExecIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _ExecVxDIntMustComplete
#define _ExecVxDIntMustComplete LOCK__ExecVxDIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ExecVxDIntMustComplete
#define ExecVxDIntMustComplete LOCK__ExecVxDIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  Begin_V86_Serialization
#define Begin_V86_Serialization LOCK_Begin_V86_Serialization
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Page
#define Unhook_V86_Page LOCK_Unhook_V86_Page
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_GetVxDLocationList
#define VMM_GetVxDLocationList LOCK_VMM_GetVxDLocationList
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_GetDDBList
#define VMM_GetDDBList LOCK_VMM_GetDDBList
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Instanced_V86_Int_Vector
#define Get_Instanced_V86_Int_Vector LOCK_Get_Instanced_V86_Int_Vector
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Set_Real_DOS_PSP
#define Get_Set_Real_DOS_PSP LOCK_Get_Set_Real_DOS_PSP
#endif

#ifdef WIN40_OR_LATER
#undef  Get_System_Time_Address
#define Get_System_Time_Address LOCK_Get_System_Time_Address
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Crit_Status_Thread
#define Get_Crit_Status_Thread LOCK_Get_Crit_Status_Thread
#endif

#ifdef WIN40_OR_LATER
#undef  Directed_Sys_Control
#define Directed_Sys_Control LOCK_Directed_Sys_Control
#endif

#ifdef WIN40_OR_LATER
#undef  _CallRing3
#define _CallRing3 LOCK__CallRing3
#endif

#ifdef WIN40_OR_LATER
#undef  CallRing3
#define CallRing3 LOCK__CallRing3
#endif

#ifdef WIN40_OR_LATER
#undef  Exec_PM_Int
#define Exec_PM_Int LOCK_Exec_PM_Int
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommitContig
#define _PageCommitContig LOCK__PageCommitContig
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommitContig
#define PageCommitContig LOCK__PageCommitContig
#endif

#ifdef WIN40_OR_LATER
#undef  _GetCurrentContext
#define _GetCurrentContext LOCK__GetCurrentContext
#endif

#ifdef WIN40_OR_LATER
#undef  GetCurrentContext
#define GetCurrentContext LOCK__GetCurrentContext
#endif

#ifdef WIN40_OR_LATER
#undef  Call_Restricted_Event
#define Call_Restricted_Event LOCK_Call_Restricted_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Restricted_Event
#define Cancel_Restricted_Event LOCK_Cancel_Restricted_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Register_PEF_Provider
#define Register_PEF_Provider LOCK_Register_PEF_Provider
#endif

#ifdef WIN40_OR_LATER
#undef  _GetPhysPageInfo
#define _GetPhysPageInfo LOCK__GetPhysPageInfo
#endif

#ifdef WIN40_OR_LATER
#undef  GetPhysPageInfo
#define GetPhysPageInfo LOCK__GetPhysPageInfo
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryInfoKey
#define _RegQueryInfoKey LOCK__RegQueryInfoKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryInfoKey
#define RegQueryInfoKey LOCK__RegQueryInfoKey
#endif

#ifdef WIN40_OR_LATER
#undef  Time_Slice_Sleep
#define Time_Slice_Sleep LOCK_Time_Slice_Sleep
#endif

#ifdef WIN40_OR_LATER
#undef  Boost_With_Decay
#define Boost_With_Decay LOCK_Boost_With_Decay
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Inversion_Pri
#define Set_Inversion_Pri LOCK_Set_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Reset_Inversion_Pri
#define Reset_Inversion_Pri LOCK_Reset_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Release_Inversion_Pri
#define Release_Inversion_Pri LOCK_Release_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Thread_Win32_Pri
#define Get_Thread_Win32_Pri LOCK_Get_Thread_Win32_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Win32_Pri
#define Set_Thread_Win32_Pri LOCK_Set_Thread_Win32_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Static_Boost
#define Set_Thread_Static_Boost LOCK_Set_Thread_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  Set_VM_Static_Boost
#define Set_VM_Static_Boost LOCK_Set_VM_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  Release_Inversion_Pri_ID
#define Release_Inversion_Pri_ID LOCK_Release_Inversion_Pri_ID
#endif

#ifdef WIN40_OR_LATER
#undef  Attach_Thread_To_Group
#define Attach_Thread_To_Group LOCK_Attach_Thread_To_Group
#endif

#ifdef WIN40_OR_LATER
#undef  Detach_Thread_From_Group
#define Detach_Thread_From_Group LOCK_Detach_Thread_From_Group
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Group_Static_Boost
#define Set_Group_Static_Boost LOCK_Set_Group_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  _GetRegistryPath
#define _GetRegistryPath LOCK__GetRegistryPath
#endif

#ifdef WIN40_OR_LATER
#undef  GetRegistryPath
#define GetRegistryPath LOCK__GetRegistryPath
#endif

#ifdef WIN40_OR_LATER
#undef  _RegRemapPreDefKey
#define _RegRemapPreDefKey LOCK__RegRemapPreDefKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegRemapPreDefKey
#define RegRemapPreDefKey LOCK__RegRemapPreDefKey
#endif

#ifdef WIN40_OR_LATER
#undef  End_V86_Serialization
#define End_V86_Serialization LOCK_End_V86_Serialization
#endif

#ifdef WIN40_OR_LATER
#undef  _Assert_Range
#define _Assert_Range LOCK__Assert_Range
#endif

#ifdef WIN40_OR_LATER
#undef  Assert_Range
#define Assert_Range LOCK__Assert_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _PageChangePager
#define _PageChangePager LOCK__PageChangePager
#endif

#ifdef WIN40_OR_LATER
#undef  PageChangePager
#define PageChangePager LOCK__PageChangePager
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCreateDynKey
#define _RegCreateDynKey LOCK__RegCreateDynKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCreateDynKey
#define RegCreateDynKey LOCK__RegCreateDynKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryMultipleValues
#define _RegQueryMultipleValues LOCK__RegQueryMultipleValues
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryMultipleValues
#define RegQueryMultipleValues LOCK__RegQueryMultipleValues
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Boot_Flags
#define Get_Boot_Flags LOCK_Get_Boot_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Boot_Flags
#define Set_Boot_Flags LOCK_Set_Boot_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrcpyn
#define _lstrcpyn LOCK__lstrcpyn
#endif

#ifdef WIN40_OR_LATER
#undef  lstrcpyn
#define lstrcpyn LOCK__lstrcpyn
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrlen
#define _lstrlen LOCK__lstrlen
#endif

#ifdef WIN40_OR_LATER
#undef  lstrlen
#define lstrlen LOCK__lstrlen
#endif

#ifdef WIN40_OR_LATER
#undef  _lmemcpy
#define _lmemcpy LOCK__lmemcpy
#endif

#ifdef WIN40_OR_LATER
#undef  lmemcpy
#define lmemcpy LOCK__lmemcpy
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrcmpi
#define _lstrcmpi LOCK__lstrcmpi
#endif

#ifdef WIN40_OR_LATER
#undef  lstrcmpi
#define lstrcmpi LOCK__lstrcmpi
#endif

#ifdef WIN40_OR_LATER
#undef  _GetVxDName
#define _GetVxDName LOCK__GetVxDName
#endif

#ifdef WIN40_OR_LATER
#undef  GetVxDName
#define GetVxDName LOCK__GetVxDName
#endif

#undef  BlockDev_Command_Complete
#define BlockDev_Command_Complete LOCK_BlockDev_Command_Complete

#undef  BlockDev_Find_Int13_Drive
#define BlockDev_Find_Int13_Drive LOCK_BlockDev_Find_Int13_Drive

#undef  BlockDev_Get_Device_List
#define BlockDev_Get_Device_List LOCK_BlockDev_Get_Device_List

#undef  BlockDev_Get_Version
#define BlockDev_Get_Version LOCK_BlockDev_Get_Version

#undef  BlockDev_Register_Device
#define BlockDev_Register_Device LOCK_BlockDev_Register_Device

#undef  BlockDev_Send_Command
#define BlockDev_Send_Command LOCK_BlockDev_Send_Command

#undef  BlockDev_Synchronous_Command
#define BlockDev_Synchronous_Command LOCK_BlockDev_Synchronous_Command

#undef  BlockDev_Thunk_Sync_Cmd
#define BlockDev_Thunk_Sync_Cmd LOCK_BlockDev_Thunk_Sync_Cmd

#undef  BlockDev_Thunk_Command
#define BlockDev_Thunk_Command LOCK_BlockDev_Thunk_Command

#undef  BlockDev_Thunk_HW_Int
#define BlockDev_Thunk_HW_Int LOCK_BlockDev_Thunk_HW_Int

#undef  DOSMGR_Add_Device
#define DOSMGR_Add_Device LOCK_DOSMGR_Add_Device

#undef  DOSMGR_BackFill_Allowed
#define DOSMGR_BackFill_Allowed LOCK_DOSMGR_BackFill_Allowed

#undef  DOSMGR_Copy_VM_Drive_State
#define DOSMGR_Copy_VM_Drive_State LOCK_DOSMGR_Copy_VM_Drive_State

#undef  DOSMGR_Enable_Indos_Polling
#define DOSMGR_Enable_Indos_Polling LOCK_DOSMGR_Enable_Indos_Polling

#undef  _DOSMGR_Exec_VM
#define _DOSMGR_Exec_VM LOCK__DOSMGR_Exec_VM

#undef  DOSMGR_Exec_VM
#define DOSMGR_Exec_VM LOCK__DOSMGR_Exec_VM

#undef  DOSMGR_Get_DOS_Crit_Status
#define DOSMGR_Get_DOS_Crit_Status LOCK_DOSMGR_Get_DOS_Crit_Status

#undef  DOSMGR_Get_IndosPtr
#define DOSMGR_Get_IndosPtr LOCK_DOSMGR_Get_IndosPtr

#undef  DOSMGR_Get_Version
#define DOSMGR_Get_Version LOCK_DOSMGR_Get_Version

#undef  DOSMGR_Instance_Device
#define DOSMGR_Instance_Device LOCK_DOSMGR_Instance_Device

#undef  DOSMGR_LocalGlobalReg
#define DOSMGR_LocalGlobalReg LOCK_DOSMGR_LocalGlobalReg

#undef  DOSMGR_Remove_Device
#define DOSMGR_Remove_Device LOCK_DOSMGR_Remove_Device

#undef  _DOSMGR_Set_Exec_VM_Data
#define _DOSMGR_Set_Exec_VM_Data LOCK__DOSMGR_Set_Exec_VM_Data

#undef  DOSMGR_Set_Exec_VM_Data
#define DOSMGR_Set_Exec_VM_Data LOCK__DOSMGR_Set_Exec_VM_Data

#undef  DOSNET_Do_PSP_Adjust
#define DOSNET_Do_PSP_Adjust LOCK_DOSNET_Do_PSP_Adjust

#undef  DOSNET_Get_Version
#define DOSNET_Get_Version LOCK_DOSNET_Get_Version

#undef  DOSNET_Send_FILESYSCHANGE
#define DOSNET_Send_FILESYSCHANGE LOCK_DOSNET_Send_FILESYSCHANGE

#undef  EBIOS_Get_Unused_Mem
#define EBIOS_Get_Unused_Mem LOCK_EBIOS_Get_Unused_Mem

#undef  EBIOS_Get_Version
#define EBIOS_Get_Version LOCK_EBIOS_Get_Version

#undef  Int13_Device_Registered
#define Int13_Device_Registered LOCK_Int13_Device_Registered

#undef  Int13_Hooking_BIOS_Int
#define Int13_Hooking_BIOS_Int LOCK_Int13_Hooking_BIOS_Int

#undef  Int13_Translate_VM_Int
#define Int13_Translate_VM_Int LOCK_Int13_Translate_VM_Int

#undef  Int13_Unhooking_BIOS_Int
#define Int13_Unhooking_BIOS_Int LOCK_Int13_Unhooking_BIOS_Int

#undef  VMD_Get_Mouse_Owner
#define VMD_Get_Mouse_Owner LOCK_VMD_Get_Mouse_Owner

#undef  VMD_Get_Version
#define VMD_Get_Version LOCK_VMD_Get_Version

#undef  VMD_Set_Mouse_Type
#define VMD_Set_Mouse_Type LOCK_VMD_Set_Mouse_Type

#undef  SHELL_Event
#define SHELL_Event LOCK_SHELL_Event

#undef  SHELL_Get_Version
#define SHELL_Get_Version LOCK_SHELL_Get_Version

#undef  SHELL_GetVMInfo
#define SHELL_GetVMInfo LOCK_SHELL_GetVMInfo

#undef  SHELL_Message
#define SHELL_Message LOCK_SHELL_Message

#undef  SHELL_Resolve_Contention
#define SHELL_Resolve_Contention LOCK_SHELL_Resolve_Contention

#undef  SHELL_SYSMODAL_Message
#define SHELL_SYSMODAL_Message LOCK_SHELL_SYSMODAL_Message

#ifdef WIN40_OR_LATER
#undef  _SHELL_CallAtAppyTime
#define _SHELL_CallAtAppyTime LOCK__SHELL_CallAtAppyTime
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CallAtAppyTime
#define SHELL_CallAtAppyTime LOCK__SHELL_CallAtAppyTime
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_CancelAppyTimeEvent
#define _SHELL_CancelAppyTimeEvent LOCK__SHELL_CancelAppyTimeEvent
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CancelAppyTimeEvent
#define SHELL_CancelAppyTimeEvent LOCK__SHELL_CancelAppyTimeEvent
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_QueryAppyTimeAvailable
#define _SHELL_QueryAppyTimeAvailable LOCK__SHELL_QueryAppyTimeAvailable
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_QueryAppyTimeAvailable
#define SHELL_QueryAppyTimeAvailable LOCK__SHELL_QueryAppyTimeAvailable
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LocalAllocEx
#define _SHELL_LocalAllocEx LOCK__SHELL_LocalAllocEx
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LocalAllocEx
#define SHELL_LocalAllocEx LOCK__SHELL_LocalAllocEx
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LocalFree
#define _SHELL_LocalFree LOCK__SHELL_LocalFree
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LocalFree
#define SHELL_LocalFree LOCK__SHELL_LocalFree
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_CallDll
#define _SHELL_CallDll LOCK__SHELL_CallDll
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CallDll
#define SHELL_CallDll LOCK__SHELL_CallDll
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_BroadcastSystemMessage
#define _SHELL_BroadcastSystemMessage LOCK__SHELL_BroadcastSystemMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_BroadcastSystemMessage
#define SHELL_BroadcastSystemMessage LOCK__SHELL_BroadcastSystemMessage
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_HookSystemBroadcast
#define _SHELL_HookSystemBroadcast LOCK__SHELL_HookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_HookSystemBroadcast
#define SHELL_HookSystemBroadcast LOCK__SHELL_HookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_UnhookSystemBroadcast
#define _SHELL_UnhookSystemBroadcast LOCK__SHELL_UnhookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_UnhookSystemBroadcast
#define SHELL_UnhookSystemBroadcast LOCK__SHELL_UnhookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_FreeLibrary
#define _SHELL_FreeLibrary LOCK__SHELL_FreeLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_FreeLibrary
#define SHELL_FreeLibrary LOCK__SHELL_FreeLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_GetProcAddress
#define _SHELL_GetProcAddress LOCK__SHELL_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_GetProcAddress
#define SHELL_GetProcAddress LOCK__SHELL_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LoadLibrary
#define _SHELL_LoadLibrary LOCK__SHELL_LoadLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LoadLibrary
#define SHELL_LoadLibrary LOCK__SHELL_LoadLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_PostMessage
#define _SHELL_PostMessage LOCK__SHELL_PostMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_PostMessage
#define SHELL_PostMessage LOCK__SHELL_PostMessage
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_PostShellMessage
#define _SHELL_PostShellMessage LOCK__SHELL_PostShellMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_PostShellMessage
#define SHELL_PostShellMessage LOCK__SHELL_PostShellMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Hook_Properties
#define SHELL_Hook_Properties LOCK_SHELL_Hook_Properties
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Unhook_Properties
#define SHELL_Unhook_Properties LOCK_SHELL_Unhook_Properties
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_ShellExecute
#define _SHELL_ShellExecute LOCK__SHELL_ShellExecute
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_ShellExecute
#define SHELL_ShellExecute LOCK__SHELL_ShellExecute
#endif

#undef  V86MMGR_Allocate_Buffer
#define V86MMGR_Allocate_Buffer LOCK_V86MMGR_Allocate_Buffer

#undef  V86MMGR_Allocate_V86_Pages
#define V86MMGR_Allocate_V86_Pages LOCK_V86MMGR_Allocate_V86_Pages

#undef  V86MMGR_Free_Buffer
#define V86MMGR_Free_Buffer LOCK_V86MMGR_Free_Buffer

#undef  V86MMGR_Free_Page_Map_Region
#define V86MMGR_Free_Page_Map_Region LOCK_V86MMGR_Free_Page_Map_Region

#undef  V86MMGR_Get_EMS_XMS_Limits
#define V86MMGR_Get_EMS_XMS_Limits LOCK_V86MMGR_Get_EMS_XMS_Limits

#undef  V86MMGR_Get_Mapping_Info
#define V86MMGR_Get_Mapping_Info LOCK_V86MMGR_Get_Mapping_Info

#undef  V86MMGR_Get_Version
#define V86MMGR_Get_Version LOCK_V86MMGR_Get_Version

#undef  V86MMGR_Get_VM_Flat_Sel
#define V86MMGR_Get_VM_Flat_Sel LOCK_V86MMGR_Get_VM_Flat_Sel

#undef  V86MMGR_Get_Xlat_Buff_State
#define V86MMGR_Get_Xlat_Buff_State LOCK_V86MMGR_Get_Xlat_Buff_State

#undef  V86MMGR_GetPgStatus
#define V86MMGR_GetPgStatus LOCK_V86MMGR_GetPgStatus

#undef  V86MMGR_Load_Client_Ptr
#define V86MMGR_Load_Client_Ptr LOCK_V86MMGR_Load_Client_Ptr

#undef  V86MMGR_LocalGlobalReg
#define V86MMGR_LocalGlobalReg LOCK_V86MMGR_LocalGlobalReg

#undef  V86MMGR_Map_Pages
#define V86MMGR_Map_Pages LOCK_V86MMGR_Map_Pages

#undef  V86MMGR_ResetBasePages
#define V86MMGR_ResetBasePages LOCK_V86MMGR_ResetBasePages

#undef  V86MMGR_Set_EMS_XMS_Limits
#define V86MMGR_Set_EMS_XMS_Limits LOCK_V86MMGR_Set_EMS_XMS_Limits

#undef  V86MMGR_Set_Mapping_Info
#define V86MMGR_Set_Mapping_Info LOCK_V86MMGR_Set_Mapping_Info

#undef  V86MMGR_Set_Xlat_Buff_State
#define V86MMGR_Set_Xlat_Buff_State LOCK_V86MMGR_Set_Xlat_Buff_State

#undef  V86MMGR_SetAvailMapPgs
#define V86MMGR_SetAvailMapPgs LOCK_V86MMGR_SetAvailMapPgs

#undef  V86MMGR_Xlat_API
#define V86MMGR_Xlat_API LOCK_V86MMGR_Xlat_API

#undef  VCD_Get_Focus
#define VCD_Get_Focus LOCK_VCD_Get_Focus

#undef  VCD_Get_Version
#define VCD_Get_Version LOCK_VCD_Get_Version

#undef  VCD_Set_Port_Global
#define VCD_Set_Port_Global LOCK_VCD_Set_Port_Global

#undef  VCD_Virtualize_Port
#define VCD_Virtualize_Port LOCK_VCD_Virtualize_Port

#ifdef WIN40_OR_LATER
#undef  VMCPD_Set_CR0_State
#define VMCPD_Set_CR0_State LOCK_VMCPD_Set_CR0_State
#endif

#ifdef WIN40_OR_LATER
#undef  VMCPD_Get_CR0_State
#define VMCPD_Get_CR0_State LOCK_VMCPD_Get_CR0_State
#endif

#undef  VMCPD_Get_Thread_State
#define VMCPD_Get_Thread_State LOCK_VMCPD_Get_Thread_State

#ifdef WIN40_OR_LATER
#undef  VMCPD_Set_Thread_State
#define VMCPD_Set_Thread_State LOCK_VMCPD_Set_Thread_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Virtual_EOI
#define VPICD_Virtual_EOI LOCK_VPICD_Virtual_EOI
#endif

#ifdef WIN40_OR_LATER
#undef  VTD_Get_Date_And_Time
#define VTD_Get_Date_And_Time LOCK_VTD_Get_Date_And_Time
#endif

#undef  VDD_Get_ModTime
#define VDD_Get_ModTime LOCK_VDD_Get_ModTime

#undef  VDD_Get_Version
#define VDD_Get_Version LOCK_VDD_Get_Version

#undef  VDD_Msg_BakColor
#define VDD_Msg_BakColor LOCK_VDD_Msg_BakColor

#undef  VDD_Msg_ClrScrn
#define VDD_Msg_ClrScrn LOCK_VDD_Msg_ClrScrn

#undef  VDD_Msg_ForColor
#define VDD_Msg_ForColor LOCK_VDD_Msg_ForColor

#undef  VDD_SetCursPos
#define VDD_SetCursPos LOCK_VDD_SetCursPos

#undef  VDD_Msg_TextOut
#define VDD_Msg_TextOut LOCK_VDD_Msg_TextOut

#undef  VDD_PIF_State
#define VDD_PIF_State LOCK_VDD_PIF_State

#undef  VDD_Query_Access
#define VDD_Query_Access LOCK_VDD_Query_Access

#undef  VDD_Set_HCurTrk
#define VDD_Set_HCurTrk LOCK_VDD_Set_HCurTrk

#undef  VDD_Set_VMType
#define VDD_Set_VMType LOCK_VDD_Set_VMType

#undef  VDMAD_Copy_From_Buffer
#define VDMAD_Copy_From_Buffer LOCK_VDMAD_Copy_From_Buffer

#undef  VDMAD_Copy_To_Buffer
#define VDMAD_Copy_To_Buffer LOCK_VDMAD_Copy_To_Buffer

#undef  VDMAD_Default_Handler
#define VDMAD_Default_Handler LOCK_VDMAD_Default_Handler

#undef  VDMAD_Disable_Translation
#define VDMAD_Disable_Translation LOCK_VDMAD_Disable_Translation

#undef  VDMAD_Enable_Translation
#define VDMAD_Enable_Translation LOCK_VDMAD_Enable_Translation

#undef  VDMAD_Get_EISA_Adr_Mode
#define VDMAD_Get_EISA_Adr_Mode LOCK_VDMAD_Get_EISA_Adr_Mode

#undef  VDMAD_Get_Region_Info
#define VDMAD_Get_Region_Info LOCK_VDMAD_Get_Region_Info

#undef  VDMAD_Get_Version
#define VDMAD_Get_Version LOCK_VDMAD_Get_Version

#undef  VDMAD_Get_Virt_State
#define VDMAD_Get_Virt_State LOCK_VDMAD_Get_Virt_State

#undef  VDMAD_Lock_DMA_Region
#define VDMAD_Lock_DMA_Region LOCK_VDMAD_Lock_DMA_Region

#undef  VDMAD_Mask_Channel
#define VDMAD_Mask_Channel LOCK_VDMAD_Mask_Channel

#undef  VDMAD_Release_Buffer
#define VDMAD_Release_Buffer LOCK_VDMAD_Release_Buffer

#undef  VDMAD_Request_Buffer
#define VDMAD_Request_Buffer LOCK_VDMAD_Request_Buffer

#undef  VDMAD_Reserve_Buffer_Space
#define VDMAD_Reserve_Buffer_Space LOCK_VDMAD_Reserve_Buffer_Space

#undef  VDMAD_Scatter_Lock
#define VDMAD_Scatter_Lock LOCK_VDMAD_Scatter_Lock

#undef  VDMAD_Scatter_Unlock
#define VDMAD_Scatter_Unlock LOCK_VDMAD_Scatter_Unlock

#undef  VDMAD_Set_EISA_Adr_Mode
#define VDMAD_Set_EISA_Adr_Mode LOCK_VDMAD_Set_EISA_Adr_Mode

#undef  VDMAD_Set_Phys_State
#define VDMAD_Set_Phys_State LOCK_VDMAD_Set_Phys_State

#undef  VDMAD_Set_Region_Info
#define VDMAD_Set_Region_Info LOCK_VDMAD_Set_Region_Info

#undef  VDMAD_Set_Virt_State
#define VDMAD_Set_Virt_State LOCK_VDMAD_Set_Virt_State

#undef  VDMAD_Unlock_DMA_Region
#define VDMAD_Unlock_DMA_Region LOCK_VDMAD_Unlock_DMA_Region

#undef  VDMAD_UnMask_Channel
#define VDMAD_UnMask_Channel LOCK_VDMAD_UnMask_Channel

#undef  VDMAD_Virtualize_Channel
#define VDMAD_Virtualize_Channel LOCK_VDMAD_Virtualize_Channel

#undef  VKD_Cancel_Paste
#define VKD_Cancel_Paste LOCK_VKD_Cancel_Paste

#undef  VKD_Define_Hot_Key
#define VKD_Define_Hot_Key LOCK_VKD_Define_Hot_Key

#undef  VKD_Define_Paste_Mode
#define VKD_Define_Paste_Mode LOCK_VKD_Define_Paste_Mode

#undef  VKD_Flush_Msg_Key_Queue
#define VKD_Flush_Msg_Key_Queue LOCK_VKD_Flush_Msg_Key_Queue

#undef  VKD_Force_Keys
#define VKD_Force_Keys LOCK_VKD_Force_Keys

#undef  VKD_Get_Kbd_Owner
#define VKD_Get_Kbd_Owner LOCK_VKD_Get_Kbd_Owner

#undef  VKD_Get_Msg_Key
#define VKD_Get_Msg_Key LOCK_VKD_Get_Msg_Key

#undef  VKD_Get_Version
#define VKD_Get_Version LOCK_VKD_Get_Version

#undef  VKD_Local_Disable_Hot_Key
#define VKD_Local_Disable_Hot_Key LOCK_VKD_Local_Disable_Hot_Key

#undef  VKD_Local_Enable_Hot_Key
#define VKD_Local_Enable_Hot_Key LOCK_VKD_Local_Enable_Hot_Key

#undef  VKD_Peek_Msg_Key
#define VKD_Peek_Msg_Key LOCK_VKD_Peek_Msg_Key

#undef  VKD_Reflect_Hot_Key
#define VKD_Reflect_Hot_Key LOCK_VKD_Reflect_Hot_Key

#undef  VKD_Remove_Hot_Key
#define VKD_Remove_Hot_Key LOCK_VKD_Remove_Hot_Key

#undef  VKD_Start_Paste
#define VKD_Start_Paste LOCK_VKD_Start_Paste

#undef  VMCPD_Get_Version
#define VMCPD_Get_Version LOCK_VMCPD_Get_Version

#undef  VMCPD_Get_Virt_State
#define VMCPD_Get_Virt_State LOCK_VMCPD_Get_Virt_State

#undef  VMCPD_Set_Virt_State
#define VMCPD_Set_Virt_State LOCK_VMCPD_Set_Virt_State

#undef  VMPoll_Enable_Disable
#define VMPoll_Enable_Disable LOCK_VMPoll_Enable_Disable

#undef  VMPoll_Get_Version
#define VMPoll_Get_Version LOCK_VMPoll_Get_Version

#undef  VMPoll_Reset_Detection
#define VMPoll_Reset_Detection LOCK_VMPoll_Reset_Detection

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  VPICD_Call_When_Hw_Int
#define VPICD_Call_When_Hw_Int LOCK_VPICD_Call_When_Hw_Int

#undef  VPICD_Clear_Int_Request
#define VPICD_Clear_Int_Request LOCK_VPICD_Clear_Int_Request

#undef  VPICD_Convert_Handle_To_IRQ
#define VPICD_Convert_Handle_To_IRQ LOCK_VPICD_Convert_Handle_To_IRQ

#undef  VPICD_Convert_Int_To_IRQ
#define VPICD_Convert_Int_To_IRQ LOCK_VPICD_Convert_Int_To_IRQ

#undef  VPICD_Convert_IRQ_To_Int
#define VPICD_Convert_IRQ_To_Int LOCK_VPICD_Convert_IRQ_To_Int

#undef  VPICD_Force_Default_Behavior
#define VPICD_Force_Default_Behavior LOCK_VPICD_Force_Default_Behavior

#undef  VPICD_Force_Default_Owner
#define VPICD_Force_Default_Owner LOCK_VPICD_Force_Default_Owner

#undef  VPICD_Get_Complete_Status
#define VPICD_Get_Complete_Status LOCK_VPICD_Get_Complete_Status

#undef  VPICD_Get_IRQ_Complete_Status
#define VPICD_Get_IRQ_Complete_Status LOCK_VPICD_Get_IRQ_Complete_Status

#undef  VPICD_Get_Status
#define VPICD_Get_Status LOCK_VPICD_Get_Status

#undef  VPICD_Get_Version
#define VPICD_Get_Version LOCK_VPICD_Get_Version

#undef  VPICD_Phys_EOI
#define VPICD_Phys_EOI LOCK_VPICD_Phys_EOI

#undef  VPICD_Physically_Mask
#define VPICD_Physically_Mask LOCK_VPICD_Physically_Mask

#undef  VPICD_Physically_Unmask
#define VPICD_Physically_Unmask LOCK_VPICD_Physically_Unmask

#undef  VPICD_Set_Auto_Masking
#define VPICD_Set_Auto_Masking LOCK_VPICD_Set_Auto_Masking

#undef  VPICD_Set_Int_Request
#define VPICD_Set_Int_Request LOCK_VPICD_Set_Int_Request

#undef  VPICD_Test_Phys_Request
#define VPICD_Test_Phys_Request LOCK_VPICD_Test_Phys_Request

#undef  VPICD_Virtualize_IRQ
#define VPICD_Virtualize_IRQ LOCK_VPICD_Virtualize_IRQ

#ifdef WIN40_OR_LATER
#undef  VPICD_Auto_Mask_At_Inst_Swap
#define VPICD_Auto_Mask_At_Inst_Swap LOCK_VPICD_Auto_Mask_At_Inst_Swap
#endif

#undef  VSD_Get_Version
#define VSD_Get_Version LOCK_VSD_Get_Version

#undef  VTD_Begin_Min_Int_Period
#define VTD_Begin_Min_Int_Period LOCK_VTD_Begin_Min_Int_Period

#undef  VTD_Disable_Trapping
#define VTD_Disable_Trapping LOCK_VTD_Disable_Trapping

#undef  VTD_Enable_Trapping
#define VTD_Enable_Trapping LOCK_VTD_Enable_Trapping

#undef  VTD_End_Min_Int_Period
#define VTD_End_Min_Int_Period LOCK_VTD_End_Min_Int_Period

#undef  VTD_Get_Interrupt_Period
#define VTD_Get_Interrupt_Period LOCK_VTD_Get_Interrupt_Period

#undef  VTD_Get_Real_Time
#define VTD_Get_Real_Time LOCK_VTD_Get_Real_Time

#undef  VTD_Get_Version
#define VTD_Get_Version LOCK_VTD_Get_Version

#undef  VPICD_Thunk_HWInt
#define VPICD_Thunk_HWInt LOCK_VPICD_Thunk_HWInt

#undef  VPICD_Thunk_VirtInt
#define VPICD_Thunk_VirtInt LOCK_VPICD_Thunk_VirtInt

#undef  VPICD_Thunk_EOI
#define VPICD_Thunk_EOI LOCK_VPICD_Thunk_EOI

#undef  VPICD_Thunk_MaskChange
#define VPICD_Thunk_MaskChange LOCK_VPICD_Thunk_MaskChange

#undef  VPICD_Thunk_Iret
#define VPICD_Thunk_Iret LOCK_VPICD_Thunk_Iret

#undef  Install_Exception_Handler
#define Install_Exception_Handler LOCK_Install_Exception_Handler

#ifdef WFW311_OR_LATER
#undef  VXDLDR_GetDeviceList
#define VXDLDR_GetDeviceList LOCK_VXDLDR_GetDeviceList
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_GetVersion
#define VXDLDR_GetVersion LOCK_VXDLDR_GetVersion
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_LoadDevice
#define VXDLDR_LoadDevice LOCK_VXDLDR_LoadDevice
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_UnloadDevice
#define VXDLDR_UnloadDevice LOCK_VXDLDR_UnloadDevice
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_DevInitSucceeded
#define VXDLDR_DevInitSucceeded LOCK_VXDLDR_DevInitSucceeded
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_DevInitFailed
#define VXDLDR_DevInitFailed LOCK_VXDLDR_DevInitFailed
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_APM_BIOS_Version
#define _VPOWERD_Get_APM_BIOS_Version LOCK__VPOWERD_Get_APM_BIOS_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_APM_BIOS_Version
#define VPOWERD_Get_APM_BIOS_Version LOCK__VPOWERD_Get_APM_BIOS_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_State
#define _VPOWERD_Get_Power_State LOCK__VPOWERD_Get_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_State
#define VPOWERD_Get_Power_State LOCK__VPOWERD_Get_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_Management_Level
#define _VPOWERD_Get_Power_Management_Level LOCK__VPOWERD_Get_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_Management_Level
#define VPOWERD_Get_Power_Management_Level LOCK__VPOWERD_Get_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_Status
#define _VPOWERD_Get_Power_Status LOCK__VPOWERD_Get_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_Status
#define VPOWERD_Get_Power_Status LOCK__VPOWERD_Get_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_OEM_APM_Function
#define _VPOWERD_OEM_APM_Function LOCK__VPOWERD_OEM_APM_Function
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_OEM_APM_Function
#define VPOWERD_OEM_APM_Function LOCK__VPOWERD_OEM_APM_Function
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Register_Power_Handler
#define _VPOWERD_Register_Power_Handler LOCK__VPOWERD_Register_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Register_Power_Handler
#define VPOWERD_Register_Power_Handler LOCK__VPOWERD_Register_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Restore_Power_On_Defaults
#define _VPOWERD_Restore_Power_On_Defaults LOCK__VPOWERD_Restore_Power_On_Defaults
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Restore_Power_On_Defaults
#define VPOWERD_Restore_Power_On_Defaults LOCK__VPOWERD_Restore_Power_On_Defaults
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_Device_Power_State
#define _VPOWERD_Set_Device_Power_State LOCK__VPOWERD_Set_Device_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_Device_Power_State
#define VPOWERD_Set_Device_Power_State LOCK__VPOWERD_Set_Device_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_Power_Management_Level
#define _VPOWERD_Set_Power_Management_Level LOCK__VPOWERD_Set_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_Power_Management_Level
#define VPOWERD_Set_Power_Management_Level LOCK__VPOWERD_Set_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_System_Power_State
#define _VPOWERD_Set_System_Power_State LOCK__VPOWERD_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_System_Power_State
#define VPOWERD_Set_System_Power_State LOCK__VPOWERD_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Deregister_Power_Handler
#define _VPOWERD_Deregister_Power_Handler LOCK__VPOWERD_Deregister_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Deregister_Power_Handler
#define VPOWERD_Deregister_Power_Handler LOCK__VPOWERD_Deregister_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Version
#define _VPOWERD_Get_Version LOCK__VPOWERD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Version
#define VPOWERD_Get_Version LOCK__VPOWERD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_W32_Get_System_Power_Status
#define _VPOWERD_W32_Get_System_Power_Status LOCK__VPOWERD_W32_Get_System_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_W32_Get_System_Power_Status
#define VPOWERD_W32_Get_System_Power_Status LOCK__VPOWERD_W32_Get_System_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_W32_Set_System_Power_State
#define _VPOWERD_W32_Set_System_Power_State LOCK__VPOWERD_W32_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_W32_Set_System_Power_State
#define VPOWERD_W32_Set_System_Power_State LOCK__VPOWERD_W32_Set_System_Power_State
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Get_Version
#define VCOMM_Get_Version LOCK_VCOMM_Get_Version
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Acquire_Port
#define _VCOMM_Acquire_Port LOCK__VCOMM_Acquire_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Acquire_Port
#define VCOMM_Acquire_Port LOCK__VCOMM_Acquire_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Add_Port
#define _VCOMM_Add_Port LOCK__VCOMM_Add_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Add_Port
#define VCOMM_Add_Port LOCK__VCOMM_Add_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Register_Port_Driver
#define _VCOMM_Register_Port_Driver LOCK__VCOMM_Register_Port_Driver
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Register_Port_Driver
#define VCOMM_Register_Port_Driver LOCK__VCOMM_Register_Port_Driver
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Release_Port
#define _VCOMM_Release_Port LOCK__VCOMM_Release_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Release_Port
#define VCOMM_Release_Port LOCK__VCOMM_Release_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_ClearCommError
#define _VCOMM_ClearCommError LOCK__VCOMM_ClearCommError
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_ClearCommError
#define VCOMM_ClearCommError LOCK__VCOMM_ClearCommError
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_CloseComm
#define _VCOMM_CloseComm LOCK__VCOMM_CloseComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_CloseComm
#define VCOMM_CloseComm LOCK__VCOMM_CloseComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_EnableCommNotification
#define _VCOMM_EnableCommNotification LOCK__VCOMM_EnableCommNotification
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_EnableCommNotification
#define VCOMM_EnableCommNotification LOCK__VCOMM_EnableCommNotification
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_EscapeCommFunction
#define _VCOMM_EscapeCommFunction LOCK__VCOMM_EscapeCommFunction
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_EscapeCommFunction
#define VCOMM_EscapeCommFunction LOCK__VCOMM_EscapeCommFunction
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommEventMask
#define _VCOMM_GetCommEventMask LOCK__VCOMM_GetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommEventMask
#define VCOMM_GetCommEventMask LOCK__VCOMM_GetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommProperties
#define _VCOMM_GetCommProperties LOCK__VCOMM_GetCommProperties
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommProperties
#define VCOMM_GetCommProperties LOCK__VCOMM_GetCommProperties
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommQueueStatus
#define _VCOMM_GetCommQueueStatus LOCK__VCOMM_GetCommQueueStatus
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommQueueStatus
#define VCOMM_GetCommQueueStatus LOCK__VCOMM_GetCommQueueStatus
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommState
#define _VCOMM_GetCommState LOCK__VCOMM_GetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommState
#define VCOMM_GetCommState LOCK__VCOMM_GetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetLastError
#define _VCOMM_GetLastError LOCK__VCOMM_GetLastError
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetLastError
#define VCOMM_GetLastError LOCK__VCOMM_GetLastError
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetModemStatus
#define _VCOMM_GetModemStatus LOCK__VCOMM_GetModemStatus
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetModemStatus
#define VCOMM_GetModemStatus LOCK__VCOMM_GetModemStatus
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_GetSetCommTimeouts
#define _VCOMM_GetSetCommTimeouts LOCK__VCOMM_GetSetCommTimeouts
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_GetSetCommTimeouts
#define VCOMM_GetSetCommTimeouts LOCK__VCOMM_GetSetCommTimeouts
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_OpenComm
#define _VCOMM_OpenComm LOCK__VCOMM_OpenComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_OpenComm
#define VCOMM_OpenComm LOCK__VCOMM_OpenComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_PurgeComm
#define _VCOMM_PurgeComm LOCK__VCOMM_PurgeComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_PurgeComm
#define VCOMM_PurgeComm LOCK__VCOMM_PurgeComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_ReadComm
#define _VCOMM_ReadComm LOCK__VCOMM_ReadComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_ReadComm
#define VCOMM_ReadComm LOCK__VCOMM_ReadComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetCommEventMask
#define _VCOMM_SetCommEventMask LOCK__VCOMM_SetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetCommEventMask
#define VCOMM_SetCommEventMask LOCK__VCOMM_SetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetCommState
#define _VCOMM_SetCommState LOCK__VCOMM_SetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetCommState
#define VCOMM_SetCommState LOCK__VCOMM_SetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetReadCallback
#define _VCOMM_SetReadCallback LOCK__VCOMM_SetReadCallback
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetReadCallback
#define VCOMM_SetReadCallback LOCK__VCOMM_SetReadCallback
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetupComm
#define _VCOMM_SetupComm LOCK__VCOMM_SetupComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetupComm
#define VCOMM_SetupComm LOCK__VCOMM_SetupComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetWriteCallback
#define _VCOMM_SetWriteCallback LOCK__VCOMM_SetWriteCallback
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetWriteCallback
#define VCOMM_SetWriteCallback LOCK__VCOMM_SetWriteCallback
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_TransmitCommChar
#define _VCOMM_TransmitCommChar LOCK__VCOMM_TransmitCommChar
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_TransmitCommChar
#define VCOMM_TransmitCommChar LOCK__VCOMM_TransmitCommChar
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_WriteComm
#define _VCOMM_WriteComm LOCK__VCOMM_WriteComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_WriteComm
#define VCOMM_WriteComm LOCK__VCOMM_WriteComm
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Win32DCB_To_Ring0
#define VCOMM_Map_Win32DCB_To_Ring0 LOCK_VCOMM_Map_Win32DCB_To_Ring0
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Ring0DCB_To_Win32
#define VCOMM_Map_Ring0DCB_To_Win32 LOCK_VCOMM_Map_Ring0DCB_To_Win32
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Get_Contention_Handler
#define _VCOMM_Get_Contention_Handler LOCK__VCOMM_Get_Contention_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Get_Contention_Handler
#define VCOMM_Get_Contention_Handler LOCK__VCOMM_Get_Contention_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Map_Name_To_Resource
#define _VCOMM_Map_Name_To_Resource LOCK__VCOMM_Map_Name_To_Resource
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Name_To_Resource
#define VCOMM_Map_Name_To_Resource LOCK__VCOMM_Map_Name_To_Resource
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Version
#define _CONFIGMG_Get_Version LOCK__CONFIGMG_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Version
#define CONFIGMG_Get_Version LOCK__CONFIGMG_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Initialize
#define _CONFIGMG_Initialize LOCK__CONFIGMG_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Initialize
#define CONFIGMG_Initialize LOCK__CONFIGMG_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Locate_DevNode
#define _CONFIGMG_Locate_DevNode LOCK__CONFIGMG_Locate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Locate_DevNode
#define CONFIGMG_Locate_DevNode LOCK__CONFIGMG_Locate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Parent
#define _CONFIGMG_Get_Parent LOCK__CONFIGMG_Get_Parent
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Parent
#define CONFIGMG_Get_Parent LOCK__CONFIGMG_Get_Parent
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Child
#define _CONFIGMG_Get_Child LOCK__CONFIGMG_Get_Child
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Child
#define CONFIGMG_Get_Child LOCK__CONFIGMG_Get_Child
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Sibling
#define _CONFIGMG_Get_Sibling LOCK__CONFIGMG_Get_Sibling
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Sibling
#define CONFIGMG_Get_Sibling LOCK__CONFIGMG_Get_Sibling
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_ID_Size
#define _CONFIGMG_Get_Device_ID_Size LOCK__CONFIGMG_Get_Device_ID_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_ID_Size
#define CONFIGMG_Get_Device_ID_Size LOCK__CONFIGMG_Get_Device_ID_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_ID
#define _CONFIGMG_Get_Device_ID LOCK__CONFIGMG_Get_Device_ID
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_ID
#define CONFIGMG_Get_Device_ID LOCK__CONFIGMG_Get_Device_ID
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Depth
#define _CONFIGMG_Get_Depth LOCK__CONFIGMG_Get_Depth
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Depth
#define CONFIGMG_Get_Depth LOCK__CONFIGMG_Get_Depth
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Private_DWord
#define _CONFIGMG_Get_Private_DWord LOCK__CONFIGMG_Get_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Private_DWord
#define CONFIGMG_Get_Private_DWord LOCK__CONFIGMG_Get_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Private_DWord
#define _CONFIGMG_Set_Private_DWord LOCK__CONFIGMG_Set_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Private_DWord
#define CONFIGMG_Set_Private_DWord LOCK__CONFIGMG_Set_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Create_DevNode
#define _CONFIGMG_Create_DevNode LOCK__CONFIGMG_Create_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Create_DevNode
#define CONFIGMG_Create_DevNode LOCK__CONFIGMG_Create_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Remove_SubTree
#define _CONFIGMG_Query_Remove_SubTree LOCK__CONFIGMG_Query_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Remove_SubTree
#define CONFIGMG_Query_Remove_SubTree LOCK__CONFIGMG_Query_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Remove_SubTree
#define _CONFIGMG_Remove_SubTree LOCK__CONFIGMG_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Remove_SubTree
#define CONFIGMG_Remove_SubTree LOCK__CONFIGMG_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Device_Driver
#define _CONFIGMG_Register_Device_Driver LOCK__CONFIGMG_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Device_Driver
#define CONFIGMG_Register_Device_Driver LOCK__CONFIGMG_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Enumerator
#define _CONFIGMG_Register_Enumerator LOCK__CONFIGMG_Register_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Enumerator
#define CONFIGMG_Register_Enumerator LOCK__CONFIGMG_Register_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Arbitrator
#define _CONFIGMG_Register_Arbitrator LOCK__CONFIGMG_Register_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Arbitrator
#define CONFIGMG_Register_Arbitrator LOCK__CONFIGMG_Register_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Deregister_Arbitrator
#define _CONFIGMG_Deregister_Arbitrator LOCK__CONFIGMG_Deregister_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Deregister_Arbitrator
#define CONFIGMG_Deregister_Arbitrator LOCK__CONFIGMG_Deregister_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Res_Des
#define _CONFIGMG_Add_Res_Des LOCK__CONFIGMG_Add_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Res_Des
#define CONFIGMG_Add_Res_Des LOCK__CONFIGMG_Add_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Modify_Res_Des
#define _CONFIGMG_Modify_Res_Des LOCK__CONFIGMG_Modify_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Modify_Res_Des
#define CONFIGMG_Modify_Res_Des LOCK__CONFIGMG_Modify_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Res_Des
#define _CONFIGMG_Free_Res_Des LOCK__CONFIGMG_Free_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Res_Des
#define CONFIGMG_Free_Res_Des LOCK__CONFIGMG_Free_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Next_Res_Des
#define _CONFIGMG_Get_Next_Res_Des LOCK__CONFIGMG_Get_Next_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Next_Res_Des
#define CONFIGMG_Get_Next_Res_Des LOCK__CONFIGMG_Get_Next_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Performance_Info
#define _CONFIGMG_Get_Performance_Info LOCK__CONFIGMG_Get_Performance_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Performance_Info
#define CONFIGMG_Get_Performance_Info LOCK__CONFIGMG_Get_Performance_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Res_Des_Data_Size
#define _CONFIGMG_Get_Res_Des_Data_Size LOCK__CONFIGMG_Get_Res_Des_Data_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Res_Des_Data_Size
#define CONFIGMG_Get_Res_Des_Data_Size LOCK__CONFIGMG_Get_Res_Des_Data_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Res_Des_Data
#define _CONFIGMG_Get_Res_Des_Data LOCK__CONFIGMG_Get_Res_Des_Data
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Res_Des_Data
#define CONFIGMG_Get_Res_Des_Data LOCK__CONFIGMG_Get_Res_Des_Data
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Process_Events_Now
#define _CONFIGMG_Process_Events_Now LOCK__CONFIGMG_Process_Events_Now
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Process_Events_Now
#define CONFIGMG_Process_Events_Now LOCK__CONFIGMG_Process_Events_Now
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Create_Range_List
#define _CONFIGMG_Create_Range_List LOCK__CONFIGMG_Create_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Create_Range_List
#define CONFIGMG_Create_Range_List LOCK__CONFIGMG_Create_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Range
#define _CONFIGMG_Add_Range LOCK__CONFIGMG_Add_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Range
#define CONFIGMG_Add_Range LOCK__CONFIGMG_Add_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Delete_Range
#define _CONFIGMG_Delete_Range LOCK__CONFIGMG_Delete_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Delete_Range
#define CONFIGMG_Delete_Range LOCK__CONFIGMG_Delete_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Test_Range_Available
#define _CONFIGMG_Test_Range_Available LOCK__CONFIGMG_Test_Range_Available
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Test_Range_Available
#define CONFIGMG_Test_Range_Available LOCK__CONFIGMG_Test_Range_Available
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Dup_Range_List
#define _CONFIGMG_Dup_Range_List LOCK__CONFIGMG_Dup_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Dup_Range_List
#define CONFIGMG_Dup_Range_List LOCK__CONFIGMG_Dup_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Range_List
#define _CONFIGMG_Free_Range_List LOCK__CONFIGMG_Free_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Range_List
#define CONFIGMG_Free_Range_List LOCK__CONFIGMG_Free_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Invert_Range_List
#define _CONFIGMG_Invert_Range_List LOCK__CONFIGMG_Invert_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Invert_Range_List
#define CONFIGMG_Invert_Range_List LOCK__CONFIGMG_Invert_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Intersect_Range_List
#define _CONFIGMG_Intersect_Range_List LOCK__CONFIGMG_Intersect_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Intersect_Range_List
#define CONFIGMG_Intersect_Range_List LOCK__CONFIGMG_Intersect_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_First_Range
#define _CONFIGMG_First_Range LOCK__CONFIGMG_First_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_First_Range
#define CONFIGMG_First_Range LOCK__CONFIGMG_First_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Next_Range
#define _CONFIGMG_Next_Range LOCK__CONFIGMG_Next_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Next_Range
#define CONFIGMG_Next_Range LOCK__CONFIGMG_Next_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Dump_Range_List
#define _CONFIGMG_Dump_Range_List LOCK__CONFIGMG_Dump_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Dump_Range_List
#define CONFIGMG_Dump_Range_List LOCK__CONFIGMG_Dump_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Load_DLVxDs
#define _CONFIGMG_Load_DLVxDs LOCK__CONFIGMG_Load_DLVxDs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Load_DLVxDs
#define CONFIGMG_Load_DLVxDs LOCK__CONFIGMG_Load_DLVxDs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DDBs
#define _CONFIGMG_Get_DDBs LOCK__CONFIGMG_Get_DDBs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DDBs
#define CONFIGMG_Get_DDBs LOCK__CONFIGMG_Get_DDBs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_CRC_CheckSum
#define _CONFIGMG_Get_CRC_CheckSum LOCK__CONFIGMG_Get_CRC_CheckSum
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_CRC_CheckSum
#define CONFIGMG_Get_CRC_CheckSum LOCK__CONFIGMG_Get_CRC_CheckSum
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_DevLoader
#define _CONFIGMG_Register_DevLoader LOCK__CONFIGMG_Register_DevLoader
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_DevLoader
#define CONFIGMG_Register_DevLoader LOCK__CONFIGMG_Register_DevLoader
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Reenumerate_DevNode
#define _CONFIGMG_Reenumerate_DevNode LOCK__CONFIGMG_Reenumerate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Reenumerate_DevNode
#define CONFIGMG_Reenumerate_DevNode LOCK__CONFIGMG_Reenumerate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Setup_DevNode
#define _CONFIGMG_Setup_DevNode LOCK__CONFIGMG_Setup_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Setup_DevNode
#define CONFIGMG_Setup_DevNode LOCK__CONFIGMG_Setup_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Reset_Children_Marks
#define _CONFIGMG_Reset_Children_Marks LOCK__CONFIGMG_Reset_Children_Marks
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Reset_Children_Marks
#define CONFIGMG_Reset_Children_Marks LOCK__CONFIGMG_Reset_Children_Marks
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Status
#define _CONFIGMG_Get_DevNode_Status LOCK__CONFIGMG_Get_DevNode_Status
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Status
#define CONFIGMG_Get_DevNode_Status LOCK__CONFIGMG_Get_DevNode_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Remove_Unmarked_Children
#define _CONFIGMG_Remove_Unmarked_Children LOCK__CONFIGMG_Remove_Unmarked_Children
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Remove_Unmarked_Children
#define CONFIGMG_Remove_Unmarked_Children LOCK__CONFIGMG_Remove_Unmarked_Children
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_ISAPNP_To_CM
#define _CONFIGMG_ISAPNP_To_CM LOCK__CONFIGMG_ISAPNP_To_CM
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_ISAPNP_To_CM
#define CONFIGMG_ISAPNP_To_CM LOCK__CONFIGMG_ISAPNP_To_CM
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_CallBack_Device_Driver
#define _CONFIGMG_CallBack_Device_Driver LOCK__CONFIGMG_CallBack_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_CallBack_Device_Driver
#define CONFIGMG_CallBack_Device_Driver LOCK__CONFIGMG_CallBack_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_CallBack_Enumerator
#define _CONFIGMG_CallBack_Enumerator LOCK__CONFIGMG_CallBack_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_CallBack_Enumerator
#define CONFIGMG_CallBack_Enumerator LOCK__CONFIGMG_CallBack_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Alloc_Log_Conf
#define _CONFIGMG_Get_Alloc_Log_Conf LOCK__CONFIGMG_Get_Alloc_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Alloc_Log_Conf
#define CONFIGMG_Get_Alloc_Log_Conf LOCK__CONFIGMG_Get_Alloc_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Key_Size
#define _CONFIGMG_Get_DevNode_Key_Size LOCK__CONFIGMG_Get_DevNode_Key_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Key_Size
#define CONFIGMG_Get_DevNode_Key_Size LOCK__CONFIGMG_Get_DevNode_Key_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Key
#define _CONFIGMG_Get_DevNode_Key LOCK__CONFIGMG_Get_DevNode_Key
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Key
#define CONFIGMG_Get_DevNode_Key LOCK__CONFIGMG_Get_DevNode_Key
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Read_Registry_Value
#define _CONFIGMG_Read_Registry_Value LOCK__CONFIGMG_Read_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Read_Registry_Value
#define CONFIGMG_Read_Registry_Value LOCK__CONFIGMG_Read_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Write_Registry_Value
#define _CONFIGMG_Write_Registry_Value LOCK__CONFIGMG_Write_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Write_Registry_Value
#define CONFIGMG_Write_Registry_Value LOCK__CONFIGMG_Write_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Disable_DevNode
#define _CONFIGMG_Disable_DevNode LOCK__CONFIGMG_Disable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Disable_DevNode
#define CONFIGMG_Disable_DevNode LOCK__CONFIGMG_Disable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Enable_DevNode
#define _CONFIGMG_Enable_DevNode LOCK__CONFIGMG_Enable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Enable_DevNode
#define CONFIGMG_Enable_DevNode LOCK__CONFIGMG_Enable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Move_DevNode
#define _CONFIGMG_Move_DevNode LOCK__CONFIGMG_Move_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Move_DevNode
#define CONFIGMG_Move_DevNode LOCK__CONFIGMG_Move_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Bus_Info
#define _CONFIGMG_Set_Bus_Info LOCK__CONFIGMG_Set_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Bus_Info
#define CONFIGMG_Set_Bus_Info LOCK__CONFIGMG_Set_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Bus_Info
#define _CONFIGMG_Get_Bus_Info LOCK__CONFIGMG_Get_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Bus_Info
#define CONFIGMG_Get_Bus_Info LOCK__CONFIGMG_Get_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_HW_Prof
#define _CONFIGMG_Set_HW_Prof LOCK__CONFIGMG_Set_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_HW_Prof
#define CONFIGMG_Set_HW_Prof LOCK__CONFIGMG_Set_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Recompute_HW_Prof
#define _CONFIGMG_Recompute_HW_Prof LOCK__CONFIGMG_Recompute_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Recompute_HW_Prof
#define CONFIGMG_Recompute_HW_Prof LOCK__CONFIGMG_Recompute_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Change_HW_Prof
#define _CONFIGMG_Query_Change_HW_Prof LOCK__CONFIGMG_Query_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Change_HW_Prof
#define CONFIGMG_Query_Change_HW_Prof LOCK__CONFIGMG_Query_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_Driver_Private_DWord
#define _CONFIGMG_Get_Device_Driver_Private_DWord LOCK__CONFIGMG_Get_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_Driver_Private_DWord
#define CONFIGMG_Get_Device_Driver_Private_DWord LOCK__CONFIGMG_Get_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Device_Driver_Private_DWord
#define _CONFIGMG_Set_Device_Driver_Private_DWord LOCK__CONFIGMG_Set_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Device_Driver_Private_DWord
#define CONFIGMG_Set_Device_Driver_Private_DWord LOCK__CONFIGMG_Set_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_HW_Prof_Flags
#define _CONFIGMG_Get_HW_Prof_Flags LOCK__CONFIGMG_Get_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_HW_Prof_Flags
#define CONFIGMG_Get_HW_Prof_Flags LOCK__CONFIGMG_Get_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_HW_Prof_Flags
#define _CONFIGMG_Set_HW_Prof_Flags LOCK__CONFIGMG_Set_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_HW_Prof_Flags
#define CONFIGMG_Set_HW_Prof_Flags LOCK__CONFIGMG_Set_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Read_Registry_Log_Confs
#define _CONFIGMG_Read_Registry_Log_Confs LOCK__CONFIGMG_Read_Registry_Log_Confs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Read_Registry_Log_Confs
#define CONFIGMG_Read_Registry_Log_Confs LOCK__CONFIGMG_Read_Registry_Log_Confs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Run_Detection
#define _CONFIGMG_Run_Detection LOCK__CONFIGMG_Run_Detection
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Run_Detection
#define CONFIGMG_Run_Detection LOCK__CONFIGMG_Run_Detection
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Call_At_Appy_Time
#define _CONFIGMG_Call_At_Appy_Time LOCK__CONFIGMG_Call_At_Appy_Time
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Call_At_Appy_Time
#define CONFIGMG_Call_At_Appy_Time LOCK__CONFIGMG_Call_At_Appy_Time
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Private_Problem
#define _CONFIGMG_Set_Private_Problem LOCK__CONFIGMG_Set_Private_Problem
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Private_Problem
#define CONFIGMG_Set_Private_Problem LOCK__CONFIGMG_Set_Private_Problem
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Hardware_Profile_Info
#define _CONFIGMG_Get_Hardware_Profile_Info LOCK__CONFIGMG_Get_Hardware_Profile_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Hardware_Profile_Info
#define CONFIGMG_Get_Hardware_Profile_Info LOCK__CONFIGMG_Get_Hardware_Profile_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Arbitrator_Free_Size
#define _CONFIGMG_Query_Arbitrator_Free_Size LOCK__CONFIGMG_Query_Arbitrator_Free_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Arbitrator_Free_Size
#define CONFIGMG_Query_Arbitrator_Free_Size LOCK__CONFIGMG_Query_Arbitrator_Free_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Arbitrator_Free_Data
#define _CONFIGMG_Query_Arbitrator_Free_Data LOCK__CONFIGMG_Query_Arbitrator_Free_Data
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Arbitrator_Free_Data
#define CONFIGMG_Query_Arbitrator_Free_Data LOCK__CONFIGMG_Query_Arbitrator_Free_Data
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Sort_NodeList
#define _CONFIGMG_Sort_NodeList LOCK__CONFIGMG_Sort_NodeList
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Sort_NodeList
#define CONFIGMG_Sort_NodeList LOCK__CONFIGMG_Sort_NodeList
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Yield
#define _CONFIGMG_Yield LOCK__CONFIGMG_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Yield
#define CONFIGMG_Yield LOCK__CONFIGMG_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Lock
#define _CONFIGMG_Lock LOCK__CONFIGMG_Lock
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Lock
#define CONFIGMG_Lock LOCK__CONFIGMG_Lock
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Unlock
#define _CONFIGMG_Unlock LOCK__CONFIGMG_Unlock
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Unlock
#define CONFIGMG_Unlock LOCK__CONFIGMG_Unlock
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Empty_Log_Conf
#define _CONFIGMG_Add_Empty_Log_Conf LOCK__CONFIGMG_Add_Empty_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Empty_Log_Conf
#define CONFIGMG_Add_Empty_Log_Conf LOCK__CONFIGMG_Add_Empty_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Log_Conf
#define _CONFIGMG_Free_Log_Conf LOCK__CONFIGMG_Free_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Log_Conf
#define CONFIGMG_Free_Log_Conf LOCK__CONFIGMG_Free_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_First_Log_Conf
#define _CONFIGMG_Get_First_Log_Conf LOCK__CONFIGMG_Get_First_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_First_Log_Conf
#define CONFIGMG_Get_First_Log_Conf LOCK__CONFIGMG_Get_First_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Next_Log_Conf
#define _CONFIGMG_Get_Next_Log_Conf LOCK__CONFIGMG_Get_Next_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Next_Log_Conf
#define CONFIGMG_Get_Next_Log_Conf LOCK__CONFIGMG_Get_Next_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Fail_Change_HW_Prof
#define _CONFIGMG_Fail_Change_HW_Prof LOCK__CONFIGMG_Fail_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Fail_Change_HW_Prof
#define CONFIGMG_Fail_Change_HW_Prof LOCK__CONFIGMG_Fail_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Call_Enumerator_Function
#define _CONFIGMG_Call_Enumerator_Function LOCK__CONFIGMG_Call_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Call_Enumerator_Function
#define CONFIGMG_Call_Enumerator_Function LOCK__CONFIGMG_Call_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Post_Pointer_Message
#define VMD_Post_Pointer_Message LOCK_VMD_Post_Pointer_Message
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Set_Cursor_Proc
#define VMD_Set_Cursor_Proc LOCK_VMD_Set_Cursor_Proc
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Call_Cursor_Proc
#define VMD_Call_Cursor_Proc LOCK_VMD_Call_Cursor_Proc
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Set_Mouse_Data
#define VMD_Set_Mouse_Data LOCK_VMD_Set_Mouse_Data
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Get_Mouse_Data
#define VMD_Get_Mouse_Data LOCK_VMD_Get_Mouse_Data
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Post_Absolute_Pointer_Message
#define VMD_Post_Absolute_Pointer_Message LOCK_VMD_Post_Absolute_Pointer_Message
#endif

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_Version
#define IFSMgr_Get_Version LOCK_IFSMgr_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterMount
#define IFSMgr_RegisterMount LOCK_IFSMgr_RegisterMount
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterNet
#define IFSMgr_RegisterNet LOCK_IFSMgr_RegisterNet
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterMailSlot
#define IFSMgr_RegisterMailSlot LOCK_IFSMgr_RegisterMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_NetTime
#define IFSMgr_Get_NetTime LOCK_IFSMgr_Get_NetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_DOSTime
#define IFSMgr_Get_DOSTime LOCK_IFSMgr_Get_DOSTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetupConnection
#define IFSMgr_SetupConnection LOCK_IFSMgr_SetupConnection
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DerefConnection
#define IFSMgr_DerefConnection LOCK_IFSMgr_DerefConnection
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ServerDOSCall
#define IFSMgr_ServerDOSCall LOCK_IFSMgr_ServerDOSCall
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CompleteAsync
#define IFSMgr_CompleteAsync LOCK_IFSMgr_CompleteAsync
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterHeap
#define IFSMgr_RegisterHeap LOCK_IFSMgr_RegisterHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetHeap
#define IFSMgr_GetHeap LOCK_IFSMgr_GetHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RetHeap
#define IFSMgr_RetHeap LOCK_IFSMgr_RetHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckHeap
#define IFSMgr_CheckHeap LOCK_IFSMgr_CheckHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckHeapItem
#define IFSMgr_CheckHeapItem LOCK_IFSMgr_CheckHeapItem
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FillHeapSpare
#define IFSMgr_FillHeapSpare LOCK_IFSMgr_FillHeapSpare
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Block
#define IFSMgr_Block LOCK_IFSMgr_Block
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Wakeup
#define IFSMgr_Wakeup LOCK_IFSMgr_Wakeup
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Yield
#define IFSMgr_Yield LOCK_IFSMgr_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SchedEvent
#define IFSMgr_SchedEvent LOCK_IFSMgr_SchedEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_QueueEvent
#define IFSMgr_QueueEvent LOCK_IFSMgr_QueueEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_KillEvent
#define IFSMgr_KillEvent LOCK_IFSMgr_KillEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FreeIOReq
#define IFSMgr_FreeIOReq LOCK_IFSMgr_FreeIOReq
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MakeMailSlot
#define IFSMgr_MakeMailSlot LOCK_IFSMgr_MakeMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DeleteMailSlot
#define IFSMgr_DeleteMailSlot LOCK_IFSMgr_DeleteMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_WriteMailSlot
#define IFSMgr_WriteMailSlot LOCK_IFSMgr_WriteMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_PopUp
#define IFSMgr_PopUp LOCK_IFSMgr_PopUp
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_printf
#define IFSMgr_printf LOCK_IFSMgr_printf
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_AssertFailed
#define IFSMgr_AssertFailed LOCK_IFSMgr_AssertFailed
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_LogEntry
#define IFSMgr_LogEntry LOCK_IFSMgr_LogEntry
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DebugMenu
#define IFSMgr_DebugMenu LOCK_IFSMgr_DebugMenu
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DebugVars
#define IFSMgr_DebugVars LOCK_IFSMgr_DebugVars
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetDebugString
#define IFSMgr_GetDebugString LOCK_IFSMgr_GetDebugString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetDebugHexNum
#define IFSMgr_GetDebugHexNum LOCK_IFSMgr_GetDebugHexNum
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetFunction
#define IFSMgr_NetFunction LOCK_IFSMgr_NetFunction
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DoDelAllUses
#define IFSMgr_DoDelAllUses LOCK_IFSMgr_DoDelAllUses
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetErrString
#define IFSMgr_SetErrString LOCK_IFSMgr_SetErrString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetErrString
#define IFSMgr_GetErrString LOCK_IFSMgr_GetErrString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetReqHook
#define IFSMgr_SetReqHook LOCK_IFSMgr_SetReqHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetPathHook
#define IFSMgr_SetPathHook LOCK_IFSMgr_SetPathHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UseAdd
#define IFSMgr_UseAdd LOCK_IFSMgr_UseAdd
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UseDel
#define IFSMgr_UseDel LOCK_IFSMgr_UseDel
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_InitUseAdd
#define IFSMgr_InitUseAdd LOCK_IFSMgr_InitUseAdd
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ChangeDir
#define IFSMgr_ChangeDir LOCK_IFSMgr_ChangeDir
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DelAllUses
#define IFSMgr_DelAllUses LOCK_IFSMgr_DelAllUses
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CDROM_Attach
#define IFSMgr_CDROM_Attach LOCK_IFSMgr_CDROM_Attach
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CDROM_Detach
#define IFSMgr_CDROM_Detach LOCK_IFSMgr_CDROM_Detach
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32DupHandle
#define IFSMgr_Win32DupHandle LOCK_IFSMgr_Win32DupHandle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Ring0_FileIO
#define IFSMgr_Ring0_FileIO LOCK_IFSMgr_Ring0_FileIO
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32_Get_Ring0_Handle
#define IFSMgr_Win32_Get_Ring0_Handle LOCK_IFSMgr_Win32_Get_Ring0_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_Drive_Info
#define IFSMgr_Get_Drive_Info LOCK_IFSMgr_Get_Drive_Info
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Ring0GetDriveInfo
#define IFSMgr_Ring0GetDriveInfo LOCK_IFSMgr_Ring0GetDriveInfo
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_BlockNoEvents
#define IFSMgr_BlockNoEvents LOCK_IFSMgr_BlockNoEvents
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetToDosTime
#define IFSMgr_NetToDosTime LOCK_IFSMgr_NetToDosTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DosToNetTime
#define IFSMgr_DosToNetTime LOCK_IFSMgr_DosToNetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DosToWin32Time
#define IFSMgr_DosToWin32Time LOCK_IFSMgr_DosToWin32Time
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32ToDosTime
#define IFSMgr_Win32ToDosTime LOCK_IFSMgr_Win32ToDosTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetToWin32Time
#define IFSMgr_NetToWin32Time LOCK_IFSMgr_NetToWin32Time
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32ToNetTime
#define IFSMgr_Win32ToNetTime LOCK_IFSMgr_Win32ToNetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MetaMatch
#define IFSMgr_MetaMatch LOCK_IFSMgr_MetaMatch
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_TransMatch
#define IFSMgr_TransMatch LOCK_IFSMgr_TransMatch
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CallProvider
#define IFSMgr_CallProvider LOCK_IFSMgr_CallProvider
#endif

#ifdef WIN40_OR_LATER
#undef  UniToBCS
#define UniToBCS LOCK_UniToBCS
#endif

#ifdef WIN40_OR_LATER
#undef  UniToBCSPath
#define UniToBCSPath LOCK_UniToBCSPath
#endif

#ifdef WIN40_OR_LATER
#undef  BCSToUni
#define BCSToUni LOCK_BCSToUni
#endif

#ifdef WIN40_OR_LATER
#undef  UniToUpper
#define UniToUpper LOCK_UniToUpper
#endif

#ifdef WIN40_OR_LATER
#undef  UniCharToOEM
#define UniCharToOEM LOCK_UniCharToOEM
#endif

#ifdef WIN40_OR_LATER
#undef  CreateBasis
#define CreateBasis LOCK_CreateBasis
#endif

#ifdef WIN40_OR_LATER
#undef  MatchBasisName
#define MatchBasisName LOCK_MatchBasisName
#endif

#ifdef WIN40_OR_LATER
#undef  AppendBasisTail
#define AppendBasisTail LOCK_AppendBasisTail
#endif

#ifdef WIN40_OR_LATER
#undef  FcbToShort
#define FcbToShort LOCK_FcbToShort
#endif

#ifdef WIN40_OR_LATER
#undef  ShortToFcb
#define ShortToFcb LOCK_ShortToFcb
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ParsePath
#define IFSMgr_ParsePath LOCK_IFSMgr_ParsePath
#endif

#ifdef WIN40_OR_LATER
#undef  Query_PhysLock
#define Query_PhysLock LOCK_Query_PhysLock
#endif

#ifdef WIN40_OR_LATER
#undef  _VolFlush
#define _VolFlush LOCK__VolFlush
#endif

#ifdef WIN40_OR_LATER
#undef  VolFlush
#define VolFlush LOCK__VolFlush
#endif

#ifdef WIN40_OR_LATER
#undef  NotifyVolumeArrival
#define NotifyVolumeArrival LOCK_NotifyVolumeArrival
#endif

#ifdef WIN40_OR_LATER
#undef  NotifyVolumeRemoval
#define NotifyVolumeRemoval LOCK_NotifyVolumeRemoval
#endif

#ifdef WIN40_OR_LATER
#undef  QueryVolumeRemoval
#define QueryVolumeRemoval LOCK_QueryVolumeRemoval
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDUnmountCFSD
#define IFSMgr_FSDUnmountCFSD LOCK_IFSMgr_FSDUnmountCFSD
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetConversionTablePtrs
#define IFSMgr_GetConversionTablePtrs LOCK_IFSMgr_GetConversionTablePtrs
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckAccessConflict
#define IFSMgr_CheckAccessConflict LOCK_IFSMgr_CheckAccessConflict
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_LockFile
#define IFSMgr_LockFile LOCK_IFSMgr_LockFile
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnlockFile
#define IFSMgr_UnlockFile LOCK_IFSMgr_UnlockFile
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RemoveLocks
#define IFSMgr_RemoveLocks LOCK_IFSMgr_RemoveLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckLocks
#define IFSMgr_CheckLocks LOCK_IFSMgr_CheckLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CountLocks
#define IFSMgr_CountLocks LOCK_IFSMgr_CountLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ReassignLockFileInst
#define IFSMgr_ReassignLockFileInst LOCK_IFSMgr_ReassignLockFileInst
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnassignLockList
#define IFSMgr_UnassignLockList LOCK_IFSMgr_UnassignLockList
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MountChildVolume
#define IFSMgr_MountChildVolume LOCK_IFSMgr_MountChildVolume
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnmountChildVolume
#define IFSMgr_UnmountChildVolume LOCK_IFSMgr_UnmountChildVolume
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SwapDrives
#define IFSMgr_SwapDrives LOCK_IFSMgr_SwapDrives
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDMapFHtoIOREQ
#define IFSMgr_FSDMapFHtoIOREQ LOCK_IFSMgr_FSDMapFHtoIOREQ
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDParsePath
#define IFSMgr_FSDParsePath LOCK_IFSMgr_FSDParsePath
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDAttachSFT
#define IFSMgr_FSDAttachSFT LOCK_IFSMgr_FSDAttachSFT
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetTimeZoneBias
#define IFSMgr_GetTimeZoneBias LOCK_IFSMgr_GetTimeZoneBias
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_PNPEvent
#define IFSMgr_PNPEvent LOCK_IFSMgr_PNPEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterCFSD
#define IFSMgr_RegisterCFSD LOCK_IFSMgr_RegisterCFSD
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32MapExtendedHandleToSFT
#define IFSMgr_Win32MapExtendedHandleToSFT LOCK_IFSMgr_Win32MapExtendedHandleToSFT
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DbgSetFileHandleLimit
#define IFSMgr_DbgSetFileHandleLimit LOCK_IFSMgr_DbgSetFileHandleLimit
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32MapSFTToExtendedHandle
#define IFSMgr_Win32MapSFTToExtendedHandle LOCK_IFSMgr_Win32MapSFTToExtendedHandle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDGetCurrentDrive
#define IFSMgr_FSDGetCurrentDrive LOCK_IFSMgr_FSDGetCurrentDrive
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_InstallFileSystemApiHook
#define IFSMgr_InstallFileSystemApiHook LOCK_IFSMgr_InstallFileSystemApiHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RemoveFileSystemApiHook
#define IFSMgr_RemoveFileSystemApiHook LOCK_IFSMgr_RemoveFileSystemApiHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RunScheduledEvents
#define IFSMgr_RunScheduledEvents LOCK_IFSMgr_RunScheduledEvents
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckDelResource
#define IFSMgr_CheckDelResource LOCK_IFSMgr_CheckDelResource
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32GetVMCurdir
#define IFSMgr_Win32GetVMCurdir LOCK_IFSMgr_Win32GetVMCurdir
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetupFailedConnection
#define IFSMgr_SetupFailedConnection LOCK_IFSMgr_SetupFailedConnection
#endif

#ifdef WIN40_OR_LATER
#undef  _GetMappedErr
#define _GetMappedErr LOCK__GetMappedErr
#endif

#ifdef WIN40_OR_LATER
#undef  GetMappedErr
#define GetMappedErr LOCK__GetMappedErr
#endif

#ifdef WIN40_OR_LATER
#undef  ShortToLossyFcb
#define ShortToLossyFcb LOCK_ShortToLossyFcb
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetLockState
#define IFSMgr_GetLockState LOCK_IFSMgr_GetLockState
#endif

#ifdef WIN40_OR_LATER
#undef  BcsToBcs
#define BcsToBcs LOCK_BcsToBcs
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetLoopback
#define IFSMgr_SetLoopback LOCK_IFSMgr_SetLoopback
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ClearLoopback
#define IFSMgr_ClearLoopback LOCK_IFSMgr_ClearLoopback
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ParseOneElement
#define IFSMgr_ParseOneElement LOCK_IFSMgr_ParseOneElement
#endif

#ifdef WIN40_OR_LATER
#undef  BcsToBcsUpper
#define BcsToBcsUpper LOCK_BcsToBcsUpper
#endif

#ifdef WIN40_OR_LATER
#undef  R0_OpenCreateFile
#define R0_OpenCreateFile LOCK_R0_OpenCreateFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_ReadFile
#define R0_ReadFile LOCK_R0_ReadFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_WriteFile
#define R0_WriteFile LOCK_R0_WriteFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_CloseFile
#define R0_CloseFile LOCK_R0_CloseFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetFileSize
#define R0_GetFileSize LOCK_R0_GetFileSize
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindFirstFile
#define R0_FindFirstFile LOCK_R0_FindFirstFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindNextFile
#define R0_FindNextFile LOCK_R0_FindNextFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindCloseFile
#define R0_FindCloseFile LOCK_R0_FindCloseFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetFileAttributes
#define R0_GetFileAttributes LOCK_R0_GetFileAttributes
#endif

#ifdef WIN40_OR_LATER
#undef  R0_SetFileAttributes
#define R0_SetFileAttributes LOCK_R0_SetFileAttributes
#endif

#ifdef WIN40_OR_LATER
#undef  R0_RenameFile
#define R0_RenameFile LOCK_R0_RenameFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_DeleteFile
#define R0_DeleteFile LOCK_R0_DeleteFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_LockFile
#define R0_LockFile LOCK_R0_LockFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_UnlockFile
#define R0_UnlockFile LOCK_R0_UnlockFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetDiskFreeSpace
#define R0_GetDiskFreeSpace LOCK_R0_GetDiskFreeSpace
#endif

#ifdef WIN40_OR_LATER
#undef  R0_ReadAbsoluteDisk
#define R0_ReadAbsoluteDisk LOCK_R0_ReadAbsoluteDisk
#endif

#ifdef WIN40_OR_LATER
#undef  R0_WriteAbsoluteDisk
#define R0_WriteAbsoluteDisk LOCK_R0_WriteAbsoluteDisk
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Get_Version
#define VWIN32_Get_Version LOCK_VWIN32_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_DIOCCompletionRoutine
#define VWIN32_DIOCCompletionRoutine LOCK_VWIN32_DIOCCompletionRoutine
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_QueueUserApc
#define _VWIN32_QueueUserApc LOCK__VWIN32_QueueUserApc
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_QueueUserApc
#define VWIN32_QueueUserApc LOCK__VWIN32_QueueUserApc
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Get_Thread_Context
#define _VWIN32_Get_Thread_Context LOCK__VWIN32_Get_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Get_Thread_Context
#define VWIN32_Get_Thread_Context LOCK__VWIN32_Get_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Emulate_Npx
#define _VWIN32_Emulate_Npx LOCK__VWIN32_Emulate_Npx
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Emulate_Npx
#define VWIN32_Emulate_Npx LOCK__VWIN32_Emulate_Npx
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CheckDelayedNpxTrap
#define _VWIN32_CheckDelayedNpxTrap LOCK__VWIN32_CheckDelayedNpxTrap
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CheckDelayedNpxTrap
#define VWIN32_CheckDelayedNpxTrap LOCK__VWIN32_CheckDelayedNpxTrap
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_LeaveCrstR0
#define VWIN32_LeaveCrstR0 LOCK_VWIN32_LeaveCrstR0
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_FaultPopup
#define _VWIN32_FaultPopup LOCK__VWIN32_FaultPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_FaultPopup
#define VWIN32_FaultPopup LOCK__VWIN32_FaultPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetContextHandle
#define VWIN32_GetContextHandle LOCK_VWIN32_GetContextHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetCurrentProcessHandle
#define VWIN32_GetCurrentProcessHandle LOCK_VWIN32_GetCurrentProcessHandle
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_SetWin32Event
#define _VWIN32_SetWin32Event LOCK__VWIN32_SetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_SetWin32Event
#define VWIN32_SetWin32Event LOCK__VWIN32_SetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_PulseWin32Event
#define _VWIN32_PulseWin32Event LOCK__VWIN32_PulseWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_PulseWin32Event
#define VWIN32_PulseWin32Event LOCK__VWIN32_PulseWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_ResetWin32Event
#define _VWIN32_ResetWin32Event LOCK__VWIN32_ResetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_ResetWin32Event
#define VWIN32_ResetWin32Event LOCK__VWIN32_ResetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_WaitSingleObject
#define _VWIN32_WaitSingleObject LOCK__VWIN32_WaitSingleObject
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_WaitSingleObject
#define VWIN32_WaitSingleObject LOCK__VWIN32_WaitSingleObject
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_WaitMultipleObjects
#define _VWIN32_WaitMultipleObjects LOCK__VWIN32_WaitMultipleObjects
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_WaitMultipleObjects
#define VWIN32_WaitMultipleObjects LOCK__VWIN32_WaitMultipleObjects
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CreateRing0Thread
#define _VWIN32_CreateRing0Thread LOCK__VWIN32_CreateRing0Thread
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CreateRing0Thread
#define VWIN32_CreateRing0Thread LOCK__VWIN32_CreateRing0Thread
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CloseVxDHandle
#define _VWIN32_CloseVxDHandle LOCK__VWIN32_CloseVxDHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CloseVxDHandle
#define VWIN32_CloseVxDHandle LOCK__VWIN32_CloseVxDHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_ActiveTimeBiasSet
#define VWIN32_ActiveTimeBiasSet LOCK_VWIN32_ActiveTimeBiasSet
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_EnterCrstR0
#define VWIN32_EnterCrstR0 LOCK_VWIN32_EnterCrstR0
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_BlueScreenPopup
#define VWIN32_BlueScreenPopup LOCK_VWIN32_BlueScreenPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_TerminateApp
#define VWIN32_TerminateApp LOCK_VWIN32_TerminateApp
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_QueueKernelAPC
#define _VWIN32_QueueKernelAPC LOCK__VWIN32_QueueKernelAPC
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_QueueKernelAPC
#define VWIN32_QueueKernelAPC LOCK__VWIN32_QueueKernelAPC
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_SysErrorBox
#define VWIN32_SysErrorBox LOCK_VWIN32_SysErrorBox
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_IsClientWin32
#define _VWIN32_IsClientWin32 LOCK__VWIN32_IsClientWin32
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_IsClientWin32
#define VWIN32_IsClientWin32 LOCK__VWIN32_IsClientWin32
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Set_Thread_Context
#define _VWIN32_Set_Thread_Context LOCK__VWIN32_Set_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Set_Thread_Context
#define VWIN32_Set_Thread_Context LOCK__VWIN32_Set_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CopyMem
#define _VWIN32_CopyMem LOCK__VWIN32_CopyMem
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CopyMem
#define VWIN32_CopyMem LOCK__VWIN32_CopyMem
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Npx_Exception
#define _VWIN32_Npx_Exception LOCK__VWIN32_Npx_Exception
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Npx_Exception
#define VWIN32_Npx_Exception LOCK__VWIN32_Npx_Exception
#endif

#ifdef WIN40_OR_LATER
#undef  PCCARD_Get_Version
#define PCCARD_Get_Version LOCK_PCCARD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PCCARD_Card_Services
#define PCCARD_Card_Services LOCK_PCCARD_Card_Services
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Get_Version
#define IOS_Get_Version LOCK_IOS_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_BD_Register_Device
#define IOS_BD_Register_Device LOCK_IOS_BD_Register_Device
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Find_Int13_Drive
#define IOS_Find_Int13_Drive LOCK_IOS_Find_Int13_Drive
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Get_Device_List
#define IOS_Get_Device_List LOCK_IOS_Get_Device_List
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_SendCommand
#define IOS_SendCommand LOCK_IOS_SendCommand
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Register
#define IOS_Register LOCK_IOS_Register
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Requestor_Service
#define IOS_Requestor_Service LOCK_IOS_Requestor_Service
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Exclusive_Access
#define IOS_Exclusive_Access LOCK_IOS_Exclusive_Access
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Send_Next_Command
#define IOS_Send_Next_Command LOCK_IOS_Send_Next_Command
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Set_Async_Time_Out
#define IOS_Set_Async_Time_Out LOCK_IOS_Set_Async_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Signal_Semaphore_No_Switch
#define IOS_Signal_Semaphore_No_Switch LOCK_IOS_Signal_Semaphore_No_Switch
#endif

#ifdef WIN40_OR_LATER
#undef  IOSIdleStatus
#define IOSIdleStatus LOCK_IOSIdleStatus
#endif

#ifdef WIN40_OR_LATER
#undef  IOSMapIORSToI24
#define IOSMapIORSToI24 LOCK_IOSMapIORSToI24
#endif

#ifdef WIN40_OR_LATER
#undef  IOSMapIORSToI21
#define IOSMapIORSToI21 LOCK_IOSMapIORSToI21
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_BD_Command_Complete
#define IOS_BD_Command_Complete LOCK_IOS_BD_Command_Complete
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_Register_Device_Driver
#define MMDEVLDR_Register_Device_Driver LOCK_MMDEVLDR_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_SetDevicePresence
#define _MMDEVLDR_SetDevicePresence LOCK__MMDEVLDR_SetDevicePresence
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_SetDevicePresence
#define MMDEVLDR_SetDevicePresence LOCK__MMDEVLDR_SetDevicePresence
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_SetEnvironmentString
#define _MMDEVLDR_SetEnvironmentString LOCK__MMDEVLDR_SetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_SetEnvironmentString
#define MMDEVLDR_SetEnvironmentString LOCK__MMDEVLDR_SetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_GetEnvironmentString
#define _MMDEVLDR_GetEnvironmentString LOCK__MMDEVLDR_GetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_GetEnvironmentString
#define MMDEVLDR_GetEnvironmentString LOCK__MMDEVLDR_GetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_RemoveEnvironmentString
#define _MMDEVLDR_RemoveEnvironmentString LOCK__MMDEVLDR_RemoveEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_RemoveEnvironmentString
#define MMDEVLDR_RemoveEnvironmentString LOCK__MMDEVLDR_RemoveEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_AddEnvironmentString
#define _MMDEVLDR_AddEnvironmentString LOCK__MMDEVLDR_AddEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_AddEnvironmentString
#define MMDEVLDR_AddEnvironmentString LOCK__MMDEVLDR_AddEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Get_Virtualization_Count
#define VPICD_Get_Virtualization_Count LOCK_VPICD_Get_Virtualization_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Phys_Mask_Channel
#define VDMAD_Phys_Mask_Channel LOCK_VDMAD_Phys_Mask_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Phys_Unmask_Channel
#define VDMAD_Phys_Unmask_Channel LOCK_VDMAD_Phys_Unmask_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Unvirtualize_Channel
#define VDMAD_Unvirtualize_Channel LOCK_VDMAD_Unvirtualize_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_IO_Address
#define VDMAD_Set_IO_Address LOCK_VDMAD_Set_IO_Address
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Convert_Hex_Binary
#define Debug_Convert_Hex_Binary LOCK_Debug_Convert_Hex_Binary
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Convert_Hex_Decimal
#define Debug_Convert_Hex_Decimal LOCK_Debug_Convert_Hex_Decimal
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Flags_Service
#define _Debug_Flags_Service LOCK__Debug_Flags_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Flags_Service
#define Debug_Flags_Service LOCK__Debug_Flags_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Cur_VM
#define Debug_Test_Cur_VM LOCK_Debug_Test_Cur_VM
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Valid_Handle
#define Debug_Test_Valid_Handle LOCK_Debug_Test_Valid_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_FindBlock
#define VCache_FindBlock LOCK_VCache_FindBlock
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_FreeBlock
#define VCache_FreeBlock LOCK_VCache_FreeBlock
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_GetSize
#define VCache_GetSize LOCK_VCache_GetSize
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_GetStats
#define VCache_GetStats LOCK_VCache_GetStats
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Get_Version
#define VCache_Get_Version LOCK_VCache_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Hold
#define VCache_Hold LOCK_VCache_Hold
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_MakeMRU
#define VCache_MakeMRU LOCK_VCache_MakeMRU
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Register
#define VCache_Register LOCK_VCache_Register
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_SwapBuffers
#define VCache_SwapBuffers LOCK_VCache_SwapBuffers
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_TestHold
#define VCache_TestHold LOCK_VCache_TestHold
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Unhold
#define VCache_Unhold LOCK_VCache_Unhold
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_CloseLookupCache
#define _VCache_CloseLookupCache LOCK__VCache_CloseLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CloseLookupCache
#define VCache_CloseLookupCache LOCK__VCache_CloseLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_CreateLookupCache
#define _VCache_CreateLookupCache LOCK__VCache_CreateLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CreateLookupCache
#define VCache_CreateLookupCache LOCK__VCache_CreateLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_DeleteLookupCache
#define _VCache_DeleteLookupCache LOCK__VCache_DeleteLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_DeleteLookupCache
#define VCache_DeleteLookupCache LOCK__VCache_DeleteLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_Lookup
#define _VCache_Lookup LOCK__VCache_Lookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Lookup
#define VCache_Lookup LOCK__VCache_Lookup
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_UpdateLookup
#define _VCache_UpdateLookup LOCK__VCache_UpdateLookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_UpdateLookup
#define VCache_UpdateLookup LOCK__VCache_UpdateLookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_RelinquishPage
#define VCache_RelinquishPage LOCK_VCache_RelinquishPage
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_UseThisPage
#define VCache_UseThisPage LOCK_VCache_UseThisPage
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_RecalcSums
#define VCache_RecalcSums LOCK_VCache_RecalcSums
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_TestHandle
#define VCache_TestHandle LOCK_VCache_TestHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_VerifySums
#define VCache_VerifySums LOCK_VCache_VerifySums
#endif

#ifdef WIN40_OR_LATER
#undef  DOSMGR_Alloc_Local_Sys_VM_Mem
#define DOSMGR_Alloc_Local_Sys_VM_Mem LOCK_DOSMGR_Alloc_Local_Sys_VM_Mem
#endif

#ifdef WIN40_OR_LATER
#undef  DOSMGR_Init_UMB_Area
#define DOSMGR_Init_UMB_Area LOCK_DOSMGR_Init_UMB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  _GetRegistryKey
#define _GetRegistryKey LOCK__GetRegistryKey
#endif

#ifdef WIN40_OR_LATER
#undef  GetRegistryKey
#define GetRegistryKey LOCK__GetRegistryKey
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Nest_Exec_Status
#define Get_Nest_Exec_Status LOCK_Get_Nest_Exec_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _AddReclaimableItem
#define _AddReclaimableItem LOCK__AddReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  AddReclaimableItem
#define AddReclaimableItem LOCK__AddReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  _AtEventTime
#define _AtEventTime LOCK__AtEventTime
#endif

#ifdef WIN40_OR_LATER
#undef  AtEventTime
#define AtEventTime LOCK__AtEventTime
#endif

#ifdef WIN40_OR_LATER
#undef  Close_Boot_Log
#define Close_Boot_Log LOCK_Close_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  EnableDisable_Boot_Log
#define EnableDisable_Boot_Log LOCK_EnableDisable_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  _EnumReclaimableItem
#define _EnumReclaimableItem LOCK__EnumReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  EnumReclaimableItem
#define EnumReclaimableItem LOCK__EnumReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  Open_Boot_Log
#define Open_Boot_Log LOCK_Open_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  _SetReclaimableItem
#define _SetReclaimableItem LOCK__SetReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  SetReclaimableItem
#define SetReclaimableItem LOCK__SetReclaimableItem
#endif

#undef  Hook_Device_Service_Ex
#define Hook_Device_Service_Ex LOCK_Hook_Device_Service_Ex

#ifdef WIN40_OR_LATER
#undef  PERF_Get_Version
#define PERF_Get_Version LOCK_PERF_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Register
#define PERF_Server_Register LOCK_PERF_Server_Register
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Deregister
#define PERF_Server_Deregister LOCK_PERF_Server_Deregister
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Add_Stat
#define PERF_Server_Add_Stat LOCK_PERF_Server_Add_Stat
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Remove_Stat
#define PERF_Server_Remove_Stat LOCK_PERF_Server_Remove_Stat
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service_Ex
#define Unhook_Device_Service_Ex LOCK_Unhook_Device_Service_Ex
#endif

#undef  Call_Previous_Hook_Proc
#define Call_Previous_Hook_Proc LOCK_Call_Previous_Hook_Proc

#ifdef WIN40_OR_LATER
#undef  dummy_935
#define dummy_935 LOCK_dummy_935
#endif

#undef  Hook_Device_Service_C
#define Hook_Device_Service_C LOCK_Hook_Device_Service_C

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service_C
#define Unhook_Device_Service_C LOCK_Unhook_Device_Service_C
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_ID
#define _CONFIGMG_Add_ID LOCK__CONFIGMG_Add_ID
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_ID
#define CONFIGMG_Add_ID LOCK__CONFIGMG_Add_ID
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Enumerator_Function
#define _CONFIGMG_Register_Enumerator_Function LOCK__CONFIGMG_Register_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Enumerator_Function
#define CONFIGMG_Register_Enumerator_Function LOCK__CONFIGMG_Register_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Update_User_Activity
#define SHELL_Update_User_Activity LOCK_SHELL_Update_User_Activity
#endif

#undef  VDMAD_Unlock_DMA_Region_No_Dirty
#define VDMAD_Unlock_DMA_Region_No_Dirty LOCK_VDMAD_Unlock_DMA_Region_No_Dirty

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetCurrentDirectory
#define VWIN32_GetCurrentDirectory LOCK_VWIN32_GetCurrentDirectory
#endif

#ifdef WIN40_OR_LATER
#undef  VXDLDR_UnloadMe
#define VXDLDR_UnloadMe LOCK_VXDLDR_UnloadMe
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_InitCompleted
#define _PELDR_InitCompleted LOCK__PELDR_InitCompleted
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_InitCompleted
#define PELDR_InitCompleted LOCK__PELDR_InitCompleted
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_AddExportTable
#define _PELDR_AddExportTable LOCK__PELDR_AddExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_AddExportTable
#define PELDR_AddExportTable LOCK__PELDR_AddExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_FreeModule
#define _PELDR_FreeModule LOCK__PELDR_FreeModule
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_FreeModule
#define PELDR_FreeModule LOCK__PELDR_FreeModule
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetEntryPoint
#define _PELDR_GetEntryPoint LOCK__PELDR_GetEntryPoint
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetEntryPoint
#define PELDR_GetEntryPoint LOCK__PELDR_GetEntryPoint
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetModuleHandle
#define _PELDR_GetModuleHandle LOCK__PELDR_GetModuleHandle
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetModuleHandle
#define PELDR_GetModuleHandle LOCK__PELDR_GetModuleHandle
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetModuleUsage
#define _PELDR_GetModuleUsage LOCK__PELDR_GetModuleUsage
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetModuleUsage
#define PELDR_GetModuleUsage LOCK__PELDR_GetModuleUsage
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetProcAddress
#define _PELDR_GetProcAddress LOCK__PELDR_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetProcAddress
#define PELDR_GetProcAddress LOCK__PELDR_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_LoadModule
#define _PELDR_LoadModule LOCK__PELDR_LoadModule
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_LoadModule
#define PELDR_LoadModule LOCK__PELDR_LoadModule
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_LoadModuleEx
#define _PELDR_LoadModuleEx LOCK__PELDR_LoadModuleEx
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_LoadModuleEx
#define PELDR_LoadModuleEx LOCK__PELDR_LoadModuleEx
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_RemoveExportTable
#define _PELDR_RemoveExportTable LOCK__PELDR_RemoveExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_RemoveExportTable
#define PELDR_RemoveExportTable LOCK__PELDR_RemoveExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_AdjustMinimum
#define VCache_AdjustMinimum LOCK_VCache_AdjustMinimum
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CheckAvail
#define VCache_CheckAvail LOCK_VCache_CheckAvail
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Deregister
#define VCache_Deregister LOCK_VCache_Deregister
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Enum
#define VCache_Enum LOCK_VCache_Enum
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Enable_Keyboard
#define VKD_Enable_Keyboard LOCK_VKD_Enable_Keyboard
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Disable_Keyboard
#define VKD_Disable_Keyboard LOCK_VKD_Disable_Keyboard
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Get_Shift_State
#define VKD_Get_Shift_State LOCK_VKD_Get_Shift_State
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Put_Byte
#define VKD_Put_Byte LOCK_VKD_Put_Byte
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Set_Shift_State
#define VKD_Set_Shift_State LOCK_VKD_Set_Shift_State
#endif

#undef  VCD_InitThunk_Control
#define VCD_InitThunk_Control LOCK_VCD_InitThunk_Control

#undef  VCD_InitThunk_Irq
#define VCD_InitThunk_Irq LOCK_VCD_InitThunk_Irq

#undef  VCD_InitThunk_Io
#define VCD_InitThunk_Io LOCK_VCD_InitThunk_Io

#undef  VCD_Acquire_Port
#define VCD_Acquire_Port LOCK_VCD_Acquire_Port

#undef  VCD_Free_Port_Windows_Style
#define VCD_Free_Port_Windows_Style LOCK_VCD_Free_Port_Windows_Style

#undef  VCD_Steal_Port_Windows_Style
#define VCD_Steal_Port_Windows_Style LOCK_VCD_Steal_Port_Windows_Style

#ifdef WIN40_OR_LATER
#undef  VCD_Find_COM_Index
#define VCD_Find_COM_Index LOCK_VCD_Find_COM_Index
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Set_Port_Global_Special
#define VCD_Set_Port_Global_Special LOCK_VCD_Set_Port_Global_Special
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Virtualize_Port_Dynamic
#define VCD_Virtualize_Port_Dynamic LOCK_VCD_Virtualize_Port_Dynamic
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Unvirtualize_Port_Dynamic
#define VCD_Unvirtualize_Port_Dynamic LOCK_VCD_Unvirtualize_Port_Dynamic
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Acquire_Port_Windows_Style
#define VCD_Acquire_Port_Windows_Style LOCK_VCD_Acquire_Port_Windows_Style
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Free_Port
#define VCD_Free_Port LOCK_VCD_Free_Port
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Check_Update_Soon
#define VDD_Check_Update_Soon LOCK_VDD_Check_Update_Soon
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_GrabRtn
#define VDD_Get_GrabRtn LOCK_VDD_Get_GrabRtn
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Hide_Cursor
#define VDD_Hide_Cursor LOCK_VDD_Hide_Cursor
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Msg_SetCursPos
#define VDD_Msg_SetCursPos LOCK_VDD_Msg_SetCursPos
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Enable_Disable_Mouse_Events
#define VMD_Enable_Disable_Mouse_Events LOCK_VMD_Enable_Disable_Mouse_Events
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Init_File
#define PageFile_Init_File LOCK_PageFile_Init_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Get_Version
#define PageFile_Get_Version LOCK_PageFile_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Clean_Up
#define PageFile_Clean_Up LOCK_PageFile_Clean_Up
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Grow_File
#define PageFile_Grow_File LOCK_PageFile_Grow_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Read_Or_Write
#define PageFile_Read_Or_Write LOCK_PageFile_Read_Or_Write
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Cancel
#define PageFile_Cancel LOCK_PageFile_Cancel
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Test_IO_Valid
#define PageFile_Test_IO_Valid LOCK_PageFile_Test_IO_Valid
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Get_Size_Info
#define PageFile_Get_Size_Info LOCK_PageFile_Get_Size_Info
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Get_Version
#define PageSwap_Get_Version LOCK_PageSwap_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Test_IO_Valid
#define PageSwap_Test_IO_Valid LOCK_PageSwap_Test_IO_Valid
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Read_Or_Write
#define PageSwap_Read_Or_Write LOCK_PageSwap_Read_Or_Write
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Grow_File
#define PageSwap_Grow_File LOCK_PageSwap_Grow_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Init_File
#define PageSwap_Init_File LOCK_PageSwap_Init_File
#endif

#undef  Allocate_PM_App_CB_Area
#define Allocate_PM_App_CB_Area LOCK_Allocate_PM_App_CB_Area

#ifdef WIN40_OR_LATER
#undef  _Call_On_My_Stack
#define _Call_On_My_Stack LOCK__Call_On_My_Stack
#endif

#ifdef WIN40_OR_LATER
#undef  Call_On_My_Stack
#define Call_On_My_Stack LOCK__Call_On_My_Stack
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Printf_Service
#define _Debug_Printf_Service LOCK__Debug_Printf_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Printf_Service
#define Debug_Printf_Service LOCK__Debug_Printf_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _strupr
#define _strupr LOCK__strupr
#endif

#ifdef WIN40_OR_LATER
#undef  strupr
#define strupr LOCK__strupr
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Do_Physical_IO
#define VDD_Do_Physical_IO LOCK_VDD_Do_Physical_IO
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_DISPLAYINFO
#define VDD_Get_DISPLAYINFO LOCK_VDD_Get_DISPLAYINFO
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_Mini_Dispatch_Table
#define VDD_Get_Mini_Dispatch_Table LOCK_VDD_Get_Mini_Dispatch_Table
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_Special_VM_IDs
#define VDD_Get_Special_VM_IDs LOCK_VDD_Get_Special_VM_IDs
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_VM_Info
#define VDD_Get_VM_Info LOCK_VDD_Get_VM_Info
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Register_Extra_Screen_Selector
#define VDD_Register_Extra_Screen_Selector LOCK_VDD_Register_Extra_Screen_Selector
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Register_Virtual_Port
#define VDD_Register_Virtual_Port LOCK_VDD_Register_Virtual_Port
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Set_Sleep_Flag_Addr
#define VDD_Set_Sleep_Flag_Addr LOCK_VDD_Set_Sleep_Flag_Addr
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Takeover_VGA_Port
#define VDD_Takeover_VGA_Port LOCK_VDD_Takeover_VGA_Port
#endif

#undef  Init_Generic_Thunk
#define Init_Generic_Thunk LOCK_Init_Generic_Thunk

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Enumerate_DevNodes
#define _VCOMM_Enumerate_DevNodes LOCK__VCOMM_Enumerate_DevNodes
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Enumerate_DevNodes
#define VCOMM_Enumerate_DevNodes LOCK__VCOMM_Enumerate_DevNodes
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Virt_Count
#define VDMAD_Get_Virt_Count LOCK_VDMAD_Get_Virt_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_Channel_Callbacks
#define VDMAD_Set_Channel_Callbacks LOCK_VDMAD_Set_Channel_Callbacks
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Phys_Status
#define VDMAD_Get_Phys_Status LOCK_VDMAD_Get_Phys_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Phys_Count
#define VDMAD_Get_Phys_Count LOCK_VDMAD_Get_Phys_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Max_Phys_Page
#define VDMAD_Get_Max_Phys_Page LOCK_VDMAD_Get_Max_Phys_Page
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_Virt_Count
#define VDMAD_Set_Virt_Count LOCK_VDMAD_Set_Virt_Count
#endif

#ifdef WIN40_OR_LATER
#undef  _EnterMustComplete
#define _EnterMustComplete LOCK__EnterMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  EnterMustComplete
#define EnterMustComplete LOCK__EnterMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _LeaveMustComplete
#define _LeaveMustComplete LOCK__LeaveMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  LeaveMustComplete
#define LeaveMustComplete LOCK__LeaveMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _ResumeExecMustComplete
#define _ResumeExecMustComplete LOCK__ResumeExecMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ResumeExecMustComplete
#define ResumeExecMustComplete LOCK__ResumeExecMustComplete
#endif

#undef  VTDI_Get_Version
#define VTDI_Get_Version LOCK_VTDI_Get_Version

#undef  VTDI_Get_Info
#define VTDI_Get_Info LOCK_VTDI_Get_Info

#undef  VTDI_Start_Timer
#define VTDI_Start_Timer LOCK_VTDI_Start_Timer

#undef  VTDI_Stop_Timer
#define VTDI_Stop_Timer LOCK_VTDI_Stop_Timer

#undef  dummy_1034
#define dummy_1034 LOCK_dummy_1034

#undef  dummy_1035
#define dummy_1035 LOCK_dummy_1035

#undef  VTDI_Schedule_Event
#define VTDI_Schedule_Event LOCK_VTDI_Schedule_Event

#undef  VTDI_Cancel_Event
#define VTDI_Cancel_Event LOCK_VTDI_Cancel_Event

#undef  VTDI_Block
#define VTDI_Block LOCK_VTDI_Block

#undef  VTDI_Signal
#define VTDI_Signal LOCK_VTDI_Signal

#undef  VTDI_Register
#define VTDI_Register LOCK_VTDI_Register

#ifdef WIN40_OR_LATER
#undef  VTDI_Unload
#define VTDI_Unload LOCK_VTDI_Unload
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Initialize
#define VTDI_Initialize LOCK_VTDI_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Register_UnloadProc
#define VTDI_Register_UnloadProc LOCK_VTDI_Register_UnloadProc
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Register_LoadProc
#define VTDI_Register_LoadProc LOCK_VTDI_Register_LoadProc
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Thunk_HWInt_Ex
#define VPICD_Thunk_HWInt_Ex LOCK_VPICD_Thunk_HWInt_Ex
#endif

#ifdef WIN40_OR_LATER
#undef  Call_Restricted_Event_Ex
#define Call_Restricted_Event_Ex LOCK_Call_Restricted_Event_Ex
#endif

#ifdef WIN41_OR_LATER
#undef  NTKERN_Get_Version
#define NTKERN_Get_Version LOCK_NTKERN_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernCreateFile
#define NtKernCreateFile LOCK_NtKernCreateFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernClose
#define NtKernClose LOCK_NtKernClose
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernReadFile
#define NtKernReadFile LOCK_NtKernReadFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernWriteFile
#define NtKernWriteFile LOCK_NtKernWriteFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernDeviceIoControl
#define NtKernDeviceIoControl LOCK_NtKernDeviceIoControl
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernGetWorkerThread
#define NtKernGetWorkerThread LOCK_NtKernGetWorkerThread
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernLoadDriver
#define NtKernLoadDriver LOCK_NtKernLoadDriver
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernQueueWorkItem
#define NtKernQueueWorkItem LOCK_NtKernQueueWorkItem
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernPhysicalDeviceObjectToDevNode
#define NtKernPhysicalDeviceObjectToDevNode LOCK_NtKernPhysicalDeviceObjectToDevNode
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernSetPhysicalCacheTypeRange
#define NtKernSetPhysicalCacheTypeRange LOCK_NtKernSetPhysicalCacheTypeRange
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernWin9XLoadDriver
#define NtKernWin9XLoadDriver LOCK_NtKernWin9XLoadDriver
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernCancelIoFile
#define NtKernCancelIoFile LOCK_NtKernCancelIoFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernGetVPICDHandleFromInterruptObj
#define NtKernGetVPICDHandleFromInterruptObj LOCK_NtKernGetVPICDHandleFromInterruptObj
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernInternalDeviceIoControl
#define NtKernInternalDeviceIoControl LOCK_NtKernInternalDeviceIoControl
#endif

#ifdef WIN41_OR_LATER
#undef  _KeSaveFloatingPointState
#define _KeSaveFloatingPointState LOCK__KeSaveFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  KeSaveFloatingPointState
#define KeSaveFloatingPointState LOCK__KeSaveFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  _KeRestoreFloatingPointState
#define _KeRestoreFloatingPointState LOCK__KeRestoreFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  KeRestoreFloatingPointState
#define KeRestoreFloatingPointState LOCK__KeRestoreFloatingPointState
#endif

#ifdef WIN40_OR_LATER
#undef  VTD_Delay
#define VTD_Delay LOCK_VTD_Delay
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Get_Version
#define _PCI_Get_Version LOCK__PCI_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Get_Version
#define PCI_Get_Version LOCK__PCI_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Read_Config
#define _PCI_Read_Config LOCK__PCI_Read_Config
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Read_Config
#define PCI_Read_Config LOCK__PCI_Read_Config
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Write_Config
#define _PCI_Write_Config LOCK__PCI_Write_Config
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Write_Config
#define PCI_Write_Config LOCK__PCI_Write_Config
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Lock_Unlock
#define _PCI_Lock_Unlock LOCK__PCI_Lock_Unlock
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Lock_Unlock
#define PCI_Lock_Unlock LOCK__PCI_Lock_Unlock
#endif

#ifdef WIN41_OR_LATER
#undef  _PCCARD_Access_CIS_Memory
#define _PCCARD_Access_CIS_Memory LOCK__PCCARD_Access_CIS_Memory
#endif

#ifdef WIN41_OR_LATER
#undef  PCCARD_Access_CIS_Memory
#define PCCARD_Access_CIS_Memory LOCK__PCCARD_Access_CIS_Memory
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Get_DevNode_PowerCapabilities
#define _CONFIGMG_Get_DevNode_PowerCapabilities LOCK__CONFIGMG_Get_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Get_DevNode_PowerCapabilities
#define CONFIGMG_Get_DevNode_PowerCapabilities LOCK__CONFIGMG_Get_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Set_DevNode_PowerCapabilities
#define _CONFIGMG_Set_DevNode_PowerCapabilities LOCK__CONFIGMG_Set_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Set_DevNode_PowerCapabilities
#define CONFIGMG_Set_DevNode_PowerCapabilities LOCK__CONFIGMG_Set_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Get_DevNode_PowerState
#define _CONFIGMG_Get_DevNode_PowerState LOCK__CONFIGMG_Get_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Get_DevNode_PowerState
#define CONFIGMG_Get_DevNode_PowerState LOCK__CONFIGMG_Get_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Set_DevNode_PowerState
#define _CONFIGMG_Set_DevNode_PowerState LOCK__CONFIGMG_Set_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Set_DevNode_PowerState
#define CONFIGMG_Set_DevNode_PowerState LOCK__CONFIGMG_Set_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Register_DevNode_For_Idle_Detection
#define _CONFIGMG_Register_DevNode_For_Idle_Detection LOCK__CONFIGMG_Register_DevNode_For_Idle_Detection
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Register_DevNode_For_Idle_Detection
#define CONFIGMG_Register_DevNode_For_Idle_Detection LOCK__CONFIGMG_Register_DevNode_For_Idle_Detection
#endif

