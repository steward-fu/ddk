//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  _XchgFreePhysReg
#define _XchgFreePhysReg STATIC__XchgFreePhysReg

#undef  XchgFreePhysReg
#define XchgFreePhysReg STATIC__XchgFreePhysReg

#undef  Wake_Up_VM
#define Wake_Up_VM STATIC_Wake_Up_VM

#undef  Wait_Semaphore
#define Wait_Semaphore STATIC_Wait_Semaphore

#undef  Validate_VM_Handle
#define Validate_VM_Handle STATIC_Validate_VM_Handle

#undef  Validate_Client_Ptr
#define Validate_Client_Ptr STATIC_Validate_Client_Ptr

#undef  Update_System_Clock
#define Update_System_Clock STATIC_Update_System_Clock

#undef  _UnmapFreePhysReg
#define _UnmapFreePhysReg STATIC__UnmapFreePhysReg

#undef  UnmapFreePhysReg
#define UnmapFreePhysReg STATIC__UnmapFreePhysReg

#undef  Unhook_Invalid_Page_Fault
#define Unhook_Invalid_Page_Fault STATIC_Unhook_Invalid_Page_Fault

#undef  Test_Sys_VM_Handle
#define Test_Sys_VM_Handle STATIC_Test_Sys_VM_Handle

#undef  _TestGlobalV86Mem
#define _TestGlobalV86Mem STATIC__TestGlobalV86Mem

#undef  TestGlobalV86Mem
#define TestGlobalV86Mem STATIC__TestGlobalV86Mem

#undef  Test_Debug_Installed
#define Test_Debug_Installed STATIC_Test_Debug_Installed

#undef  Test_DBCS_Lead_Byte
#define Test_DBCS_Lead_Byte STATIC_Test_DBCS_Lead_Byte

#undef  Test_Cur_VM_Handle
#define Test_Cur_VM_Handle STATIC_Test_Cur_VM_Handle

#undef  System_Control
#define System_Control STATIC_System_Control

#undef  Suspend_VM
#define Suspend_VM STATIC_Suspend_VM

#undef  Simulate_Push
#define Simulate_Push STATIC_Simulate_Push

#undef  Simulate_Pop
#define Simulate_Pop STATIC_Simulate_Pop

#undef  Simulate_Int
#define Simulate_Int STATIC_Simulate_Int

#undef  Simulate_Far_Ret_N
#define Simulate_Far_Ret_N STATIC_Simulate_Far_Ret_N

#undef  Simulate_Far_Jmp
#define Simulate_Far_Jmp STATIC_Simulate_Far_Jmp

#undef  Simulate_Far_Call
#define Simulate_Far_Call STATIC_Simulate_Far_Call

#undef  Signal_Semaphore
#define Signal_Semaphore STATIC_Signal_Semaphore

#undef  Set_VM_Time_Out
#define Set_VM_Time_Out STATIC_Set_VM_Time_Out

#undef  Set_V86_Int_Vector
#define Set_V86_Int_Vector STATIC_Set_V86_Int_Vector

#undef  Set_PM_Int_Vector
#define Set_PM_Int_Vector STATIC_Set_PM_Int_Vector

#undef  Set_Time_Slice_Priority
#define Set_Time_Slice_Priority STATIC_Set_Time_Slice_Priority

#undef  Set_Time_Slice_Granularity
#define Set_Time_Slice_Granularity STATIC_Set_Time_Slice_Granularity

#undef  Set_System_Exit_Code
#define Set_System_Exit_Code STATIC_Set_System_Exit_Code

#undef  _SetResetV86Pageable
#define _SetResetV86Pageable STATIC__SetResetV86Pageable

#undef  SetResetV86Pageable
#define SetResetV86Pageable STATIC__SetResetV86Pageable

#undef  Set_PM_Int_Type
#define Set_PM_Int_Type STATIC_Set_PM_Int_Type

#undef  Set_Physical_HMA_Alias
#define Set_Physical_HMA_Alias STATIC_Set_Physical_HMA_Alias

#undef  Set_NMI_Handler_Addr
#define Set_NMI_Handler_Addr STATIC_Set_NMI_Handler_Addr

#undef  Set_Mono_Cur_Pos
#define Set_Mono_Cur_Pos STATIC_Set_Mono_Cur_Pos

#undef  _SetLastV86Page
#define _SetLastV86Page STATIC__SetLastV86Page

#undef  SetLastV86Page
#define SetLastV86Page STATIC__SetLastV86Page

#undef  Set_Global_Time_Out
#define Set_Global_Time_Out STATIC_Set_Global_Time_Out

#undef  _SetFreePhysRegCalBk
#define _SetFreePhysRegCalBk STATIC__SetFreePhysRegCalBk

#undef  SetFreePhysRegCalBk
#define SetFreePhysRegCalBk STATIC__SetFreePhysRegCalBk

#undef  Set_Execution_Focus
#define Set_Execution_Focus STATIC_Set_Execution_Focus

#undef  _SetDescriptor
#define _SetDescriptor STATIC__SetDescriptor

#undef  SetDescriptor
#define SetDescriptor STATIC__SetDescriptor

#undef  Set_Delete_On_Exit_File
#define Set_Delete_On_Exit_File STATIC_Set_Delete_On_Exit_File

#undef  _SelectorMapFlat
#define _SelectorMapFlat STATIC__SelectorMapFlat

#undef  SelectorMapFlat
#define SelectorMapFlat STATIC__SelectorMapFlat

#undef  Schedule_VM_Event
#define Schedule_VM_Event STATIC_Schedule_VM_Event

#undef  Schedule_Global_Event
#define Schedule_Global_Event STATIC_Schedule_Global_Event

#undef  Save_Client_State
#define Save_Client_State STATIC_Save_Client_State

#undef  Resume_VM
#define Resume_VM STATIC_Resume_VM

#undef  Restore_Client_State
#define Restore_Client_State STATIC_Restore_Client_State

#undef  Remove_V86_Break_Point
#define Remove_V86_Break_Point STATIC_Remove_V86_Break_Point

#undef  Remove_Exception_Handler
#define Remove_Exception_Handler STATIC_Remove_Exception_Handler

#undef  Release_Critical_Section
#define Release_Critical_Section STATIC_Release_Critical_Section

#undef  Queue_Debug_String
#define Queue_Debug_String STATIC_Queue_Debug_String

#undef  _PhysIntoV86
#define _PhysIntoV86 STATIC__PhysIntoV86

#undef  PhysIntoV86
#define PhysIntoV86 STATIC__PhysIntoV86

#undef  _PageUnLock
#define _PageUnLock STATIC__PageUnLock

#undef  PageUnLock
#define PageUnLock STATIC__PageUnLock

#undef  _PageResetHandlePAddr
#define _PageResetHandlePAddr STATIC__PageResetHandlePAddr

#undef  PageResetHandlePAddr
#define PageResetHandlePAddr STATIC__PageResetHandlePAddr

#undef  _PageReAllocate
#define _PageReAllocate STATIC__PageReAllocate

#undef  PageReAllocate
#define PageReAllocate STATIC__PageReAllocate

#undef  _PageOutDirtyPages
#define _PageOutDirtyPages STATIC__PageOutDirtyPages

#undef  PageOutDirtyPages
#define PageOutDirtyPages STATIC__PageOutDirtyPages

#undef  _PageLock
#define _PageLock STATIC__PageLock

#undef  PageLock
#define PageLock STATIC__PageLock

#undef  _PageGetSizeAddr
#define _PageGetSizeAddr STATIC__PageGetSizeAddr

#undef  PageGetSizeAddr
#define PageGetSizeAddr STATIC__PageGetSizeAddr

#undef  _PageGetAllocInfo
#define _PageGetAllocInfo STATIC__PageGetAllocInfo

#undef  PageGetAllocInfo
#define PageGetAllocInfo STATIC__PageGetAllocInfo

#undef  _PageFree
#define _PageFree STATIC__PageFree

#undef  PageFree
#define PageFree STATIC__PageFree

#undef  _PageDiscardPages
#define _PageDiscardPages STATIC__PageDiscardPages

#undef  PageDiscardPages
#define PageDiscardPages STATIC__PageDiscardPages

#undef  _PageCheckLinRange
#define _PageCheckLinRange STATIC__PageCheckLinRange

#undef  PageCheckLinRange
#define PageCheckLinRange STATIC__PageCheckLinRange

#undef  _PageAllocate
#define _PageAllocate STATIC__PageAllocate

#undef  PageAllocate
#define PageAllocate STATIC__PageAllocate

#undef  Out_Mono_String
#define Out_Mono_String STATIC_Out_Mono_String

#undef  Out_Mono_Chr_Attrs
#define Out_Mono_Chr_Attrs STATIC_Out_Mono_Chr_Attrs

#undef  Out_Mono_Chr
#define Out_Mono_Chr STATIC_Out_Mono_Chr

#undef  Out_Debug_String
#define Out_Debug_String STATIC_Out_Debug_String

#undef  Out_Debug_Chr
#define Out_Debug_Chr STATIC_Out_Debug_Chr

#undef  OpenFile
#define OpenFile STATIC_OpenFile

#undef  Nuke_VM
#define Nuke_VM STATIC_Nuke_VM

#undef  No_Fail_Resume_VM
#define No_Fail_Resume_VM STATIC_No_Fail_Resume_VM

#undef  _ModifyPageBits
#define _ModifyPageBits STATIC__ModifyPageBits

#undef  ModifyPageBits
#define ModifyPageBits STATIC__ModifyPageBits

#undef  _MMGR_Toggle_HMA
#define _MMGR_Toggle_HMA STATIC__MMGR_Toggle_HMA

#undef  MMGR_Toggle_HMA
#define MMGR_Toggle_HMA STATIC__MMGR_Toggle_HMA

#undef  MMGR_SetNULPageAddr
#define MMGR_SetNULPageAddr STATIC_MMGR_SetNULPageAddr

#undef  _MapPhysToLinear
#define _MapPhysToLinear STATIC__MapPhysToLinear

#undef  MapPhysToLinear
#define MapPhysToLinear STATIC__MapPhysToLinear

#undef  Map_Lin_To_VM_Addr
#define Map_Lin_To_VM_Addr STATIC_Map_Lin_To_VM_Addr

#undef  _MapIntoV86
#define _MapIntoV86 STATIC__MapIntoV86

#undef  MapIntoV86
#define MapIntoV86 STATIC__MapIntoV86

#undef  _MapFreePhysReg
#define _MapFreePhysReg STATIC__MapFreePhysReg

#undef  MapFreePhysReg
#define MapFreePhysReg STATIC__MapFreePhysReg

#undef  Map_Flat
#define Map_Flat STATIC_Map_Flat

#undef  Locate_Byte_In_ROM
#define Locate_Byte_In_ROM STATIC_Locate_Byte_In_ROM

#undef  List_Remove_First
#define List_Remove_First STATIC_List_Remove_First

#undef  List_Remove
#define List_Remove STATIC_List_Remove

#undef  List_Insert
#define List_Insert STATIC_List_Insert

#undef  List_Get_Next
#define List_Get_Next STATIC_List_Get_Next

#undef  List_Get_First
#define List_Get_First STATIC_List_Get_First

#undef  List_Destroy
#define List_Destroy STATIC_List_Destroy

#undef  List_Deallocate
#define List_Deallocate STATIC_List_Deallocate

#undef  List_Create
#define List_Create STATIC_List_Create

#undef  List_Attach_Tail
#define List_Attach_Tail STATIC_List_Attach_Tail

#undef  List_Attach
#define List_Attach STATIC_List_Attach

#undef  List_Allocate
#define List_Allocate STATIC_List_Allocate

#undef  _LinPageUnLock
#define _LinPageUnLock STATIC__LinPageUnLock

#undef  LinPageUnLock
#define LinPageUnLock STATIC__LinPageUnLock

#undef  _LinPageLock
#define _LinPageLock STATIC__LinPageLock

#undef  LinPageLock
#define LinPageLock STATIC__LinPageLock

#undef  _LinMapIntoV86
#define _LinMapIntoV86 STATIC__LinMapIntoV86

#undef  LinMapIntoV86
#define LinMapIntoV86 STATIC__LinMapIntoV86

#undef  Is_Debug_Chr
#define Is_Debug_Chr STATIC_Is_Debug_Chr

#undef  Install_V86_Break_Point
#define Install_V86_Break_Point STATIC_Install_V86_Break_Point

#undef  Install_IO_Handler
#define Install_IO_Handler STATIC_Install_IO_Handler

#undef  In_Debug_Chr
#define In_Debug_Chr STATIC_In_Debug_Chr

#undef  Hook_VMM_Fault
#define Hook_VMM_Fault STATIC_Hook_VMM_Fault

#undef  Hook_V86_Page
#define Hook_V86_Page STATIC_Hook_V86_Page

#undef  Hook_V86_Int_Chain
#define Hook_V86_Int_Chain STATIC_Hook_V86_Int_Chain

#undef  Hook_V86_Fault
#define Hook_V86_Fault STATIC_Hook_V86_Fault

#undef  Hook_PM_Fault
#define Hook_PM_Fault STATIC_Hook_PM_Fault

#undef  Hook_Device_PM_API
#define Hook_Device_PM_API STATIC_Hook_Device_PM_API

#undef  Hook_NMI_Event
#define Hook_NMI_Event STATIC_Hook_NMI_Event

#undef  Hook_Invalid_Page_Fault
#define Hook_Invalid_Page_Fault STATIC_Hook_Invalid_Page_Fault

#undef  Hook_Device_V86_API
#define Hook_Device_V86_API STATIC_Hook_Device_V86_API

#undef  Hook_Device_Service
#define Hook_Device_Service STATIC_Hook_Device_Service

#undef  _HeapReAllocate
#define _HeapReAllocate STATIC__HeapReAllocate

#undef  HeapReAllocate
#define HeapReAllocate STATIC__HeapReAllocate

#undef  _HeapGetSize
#define _HeapGetSize STATIC__HeapGetSize

#undef  HeapGetSize
#define HeapGetSize STATIC__HeapGetSize

#undef  _HeapFree
#define _HeapFree STATIC__HeapFree

#undef  HeapFree
#define HeapFree STATIC__HeapFree

#undef  _HeapAllocate
#define _HeapAllocate STATIC__HeapAllocate

#undef  HeapAllocate
#define HeapAllocate STATIC__HeapAllocate

#undef  _GetVMPgCount
#define _GetVMPgCount STATIC__GetVMPgCount

#undef  GetVMPgCount
#define GetVMPgCount STATIC__GetVMPgCount

#undef  Get_VMM_Version
#define Get_VMM_Version STATIC_Get_VMM_Version

#undef  Get_VMM_Reenter_Count
#define Get_VMM_Reenter_Count STATIC_Get_VMM_Reenter_Count

#undef  Get_VM_Exec_Time
#define Get_VM_Exec_Time STATIC_Get_VM_Exec_Time

#undef  _GetV86PageableArray
#define _GetV86PageableArray STATIC__GetV86PageableArray

#undef  GetV86PageableArray
#define GetV86PageableArray STATIC__GetV86PageableArray

#undef  Get_V86_Int_Vector
#define Get_V86_Int_Vector STATIC_Get_V86_Int_Vector

#undef  Get_Time_Slice_Priority
#define Get_Time_Slice_Priority STATIC_Get_Time_Slice_Priority

#undef  Get_Time_Slice_Info
#define Get_Time_Slice_Info STATIC_Get_Time_Slice_Info

#undef  Get_Time_Slice_Granularity
#define Get_Time_Slice_Granularity STATIC_Get_Time_Slice_Granularity

#undef  Get_Sys_VM_Handle
#define Get_Sys_VM_Handle STATIC_Get_Sys_VM_Handle

#undef  Get_System_Time
#define Get_System_Time STATIC_Get_System_Time

#undef  _GetSetPageOutCount
#define _GetSetPageOutCount STATIC__GetSetPageOutCount

#undef  GetSetPageOutCount
#define GetSetPageOutCount STATIC__GetSetPageOutCount

#undef  _GetSysPageCount
#define _GetSysPageCount STATIC__GetSysPageCount

#undef  GetSysPageCount
#define GetSysPageCount STATIC__GetSysPageCount

#undef  Set_HMA_Info
#define Set_HMA_Info STATIC_Set_HMA_Info

#undef  Get_HMA_Info
#define Get_HMA_Info STATIC_Get_HMA_Info

#undef  GetDetailedVMError
#define GetDetailedVMError STATIC_GetDetailedVMError

#undef  SetDetailedVMError
#define SetDetailedVMError STATIC_SetDetailedVMError

#undef  Get_PSP_Segment
#define Get_PSP_Segment STATIC_Get_PSP_Segment

#undef  Get_Profile_String
#define Get_Profile_String STATIC_Get_Profile_String

#undef  Get_Profile_Hex_Int
#define Get_Profile_Hex_Int STATIC_Get_Profile_Hex_Int

#undef  Get_Profile_Fixed_Point
#define Get_Profile_Fixed_Point STATIC_Get_Profile_Fixed_Point

#undef  Get_Profile_Decimal_Int
#define Get_Profile_Decimal_Int STATIC_Get_Profile_Decimal_Int

#undef  Get_Profile_Boolean
#define Get_Profile_Boolean STATIC_Get_Profile_Boolean

#undef  Get_PM_Int_Vector
#define Get_PM_Int_Vector STATIC_Get_PM_Int_Vector

#undef  Get_PM_Int_Type
#define Get_PM_Int_Type STATIC_Get_PM_Int_Type

#undef  _GetNulPageHandle
#define _GetNulPageHandle STATIC__GetNulPageHandle

#undef  GetNulPageHandle
#define GetNulPageHandle STATIC__GetNulPageHandle

#undef  Get_NMI_Handler_Addr
#define Get_NMI_Handler_Addr STATIC_Get_NMI_Handler_Addr

#undef  Get_Next_VM_Handle
#define Get_Next_VM_Handle STATIC_Get_Next_VM_Handle

#undef  Get_Next_Profile_String
#define Get_Next_Profile_String STATIC_Get_Next_Profile_String

#undef  Get_Name_Of_Ugly_TSR
#define Get_Name_Of_Ugly_TSR STATIC_Get_Name_Of_Ugly_TSR

#undef  Get_Mono_Cur_Pos
#define Get_Mono_Cur_Pos STATIC_Get_Mono_Cur_Pos

#undef  Get_Mono_Chr
#define Get_Mono_Chr STATIC_Get_Mono_Chr

#undef  Get_Machine_Info
#define Get_Machine_Info STATIC_Get_Machine_Info

#undef  Get_Last_Updated_VM_Exec_Time
#define Get_Last_Updated_VM_Exec_Time STATIC_Get_Last_Updated_VM_Exec_Time

#undef  _GetLastV86Page
#define _GetLastV86Page STATIC__GetLastV86Page

#undef  GetLastV86Page
#define GetLastV86Page STATIC__GetLastV86Page

#undef  Get_Last_Updated_System_Time
#define Get_Last_Updated_System_Time STATIC_Get_Last_Updated_System_Time

#undef  _GetGlblRng0V86IntBase
#define _GetGlblRng0V86IntBase STATIC__GetGlblRng0V86IntBase

#undef  GetGlblRng0V86IntBase
#define GetGlblRng0V86IntBase STATIC__GetGlblRng0V86IntBase

#undef  _GetFreePageCount
#define _GetFreePageCount STATIC__GetFreePageCount

#undef  GetFreePageCount
#define GetFreePageCount STATIC__GetFreePageCount

#undef  Get_Fault_Hook_Addrs
#define Get_Fault_Hook_Addrs STATIC_Get_Fault_Hook_Addrs

#undef  Get_Exec_Path
#define Get_Exec_Path STATIC_Get_Exec_Path

#undef  Get_Execution_Focus
#define Get_Execution_Focus STATIC_Get_Execution_Focus

#undef  Get_Environment_String
#define Get_Environment_String STATIC_Get_Environment_String

#undef  _Get_Device_V86_Pages_Array
#define _Get_Device_V86_Pages_Array STATIC__Get_Device_V86_Pages_Array

#undef  Get_Device_V86_Pages_Array
#define Get_Device_V86_Pages_Array STATIC__Get_Device_V86_Pages_Array

#undef  _GetDescriptor
#define _GetDescriptor STATIC__GetDescriptor

#undef  GetDescriptor
#define GetDescriptor STATIC__GetDescriptor

#undef  _GetDemandPageInfo
#define _GetDemandPageInfo STATIC__GetDemandPageInfo

#undef  GetDemandPageInfo
#define GetDemandPageInfo STATIC__GetDemandPageInfo

#undef  Get_Debug_Options
#define Get_Debug_Options STATIC_Get_Debug_Options

#undef  Get_Cur_VM_Handle
#define Get_Cur_VM_Handle STATIC_Get_Cur_VM_Handle

#undef  Get_Crit_Status_No_Block
#define Get_Crit_Status_No_Block STATIC_Get_Crit_Status_No_Block

#undef  Get_Crit_Section_Status
#define Get_Crit_Section_Status STATIC_Get_Crit_Section_Status

#undef  Get_Cur_PM_App_CB
#define Get_Cur_PM_App_CB STATIC_Get_Cur_PM_App_CB

#undef  Get_Config_Directory
#define Get_Config_Directory STATIC_Get_Config_Directory

#undef  _GetFirstV86Page
#define _GetFirstV86Page STATIC__GetFirstV86Page

#undef  GetFirstV86Page
#define GetFirstV86Page STATIC__GetFirstV86Page

#undef  _GetAppFlatDSAlias
#define _GetAppFlatDSAlias STATIC__GetAppFlatDSAlias

#undef  GetAppFlatDSAlias
#define GetAppFlatDSAlias STATIC__GetAppFlatDSAlias

#undef  _Free_Temp_V86_Data_Area
#define _Free_Temp_V86_Data_Area STATIC__Free_Temp_V86_Data_Area

#undef  Free_Temp_V86_Data_Area
#define Free_Temp_V86_Data_Area STATIC__Free_Temp_V86_Data_Area

#undef  _Free_LDT_Selector
#define _Free_LDT_Selector STATIC__Free_LDT_Selector

#undef  Free_LDT_Selector
#define Free_LDT_Selector STATIC__Free_LDT_Selector

#undef  _Free_GDT_Selector
#define _Free_GDT_Selector STATIC__Free_GDT_Selector

#undef  Free_GDT_Selector
#define Free_GDT_Selector STATIC__Free_GDT_Selector

#undef  Fatal_Error_Handler
#define Fatal_Error_Handler STATIC_Fatal_Error_Handler

#undef  Exec_VxD_Int
#define Exec_VxD_Int STATIC_Exec_VxD_Int

#undef  Exec_Int
#define Exec_Int STATIC_Exec_Int

#undef  End_Reentrant_Execution
#define End_Reentrant_Execution STATIC_End_Reentrant_Execution

#undef  End_Crit_And_Suspend
#define End_Crit_And_Suspend STATIC_End_Crit_And_Suspend

#undef  Enable_Local_Trapping
#define Enable_Local_Trapping STATIC_Enable_Local_Trapping

#undef  Enable_Global_Trapping
#define Enable_Global_Trapping STATIC_Enable_Global_Trapping

#undef  Disable_Local_Trapping
#define Disable_Local_Trapping STATIC_Disable_Local_Trapping

#undef  Disable_Global_Trapping
#define Disable_Global_Trapping STATIC_Disable_Global_Trapping

#undef  Destroy_Semaphore
#define Destroy_Semaphore STATIC_Destroy_Semaphore

#undef  _DeAssign_Device_V86_Pages
#define _DeAssign_Device_V86_Pages STATIC__DeAssign_Device_V86_Pages

#undef  DeAssign_Device_V86_Pages
#define DeAssign_Device_V86_Pages STATIC__DeAssign_Device_V86_Pages

#undef  Close_VM
#define Close_VM STATIC_Close_VM

#undef  Convert_Boolean_String
#define Convert_Boolean_String STATIC_Convert_Boolean_String

#undef  Convert_Decimal_String
#define Convert_Decimal_String STATIC_Convert_Decimal_String

#undef  Convert_Fixed_Point_String
#define Convert_Fixed_Point_String STATIC_Convert_Fixed_Point_String

#undef  Convert_Hex_String
#define Convert_Hex_String STATIC_Convert_Hex_String

#undef  _CopyPageTable
#define _CopyPageTable STATIC__CopyPageTable

#undef  CopyPageTable
#define CopyPageTable STATIC__CopyPageTable

#undef  Create_Semaphore
#define Create_Semaphore STATIC_Create_Semaphore

#undef  _AddFreePhysPage
#define _AddFreePhysPage STATIC__AddFreePhysPage

#undef  AddFreePhysPage
#define AddFreePhysPage STATIC__AddFreePhysPage

#undef  _Add_Global_V86_Data_Area
#define _Add_Global_V86_Data_Area STATIC__Add_Global_V86_Data_Area

#undef  Add_Global_V86_Data_Area
#define Add_Global_V86_Data_Area STATIC__Add_Global_V86_Data_Area

#undef  _AddInstanceItem
#define _AddInstanceItem STATIC__AddInstanceItem

#undef  AddInstanceItem
#define AddInstanceItem STATIC__AddInstanceItem

#undef  Adjust_Exec_Priority
#define Adjust_Exec_Priority STATIC_Adjust_Exec_Priority

#undef  Adjust_Execution_Time
#define Adjust_Execution_Time STATIC_Adjust_Execution_Time

#undef  _Allocate_Device_CB_Area
#define _Allocate_Device_CB_Area STATIC__Allocate_Device_CB_Area

#undef  Allocate_Device_CB_Area
#define Allocate_Device_CB_Area STATIC__Allocate_Device_CB_Area

#undef  _Allocate_GDT_Selector
#define _Allocate_GDT_Selector STATIC__Allocate_GDT_Selector

#undef  Allocate_GDT_Selector
#define Allocate_GDT_Selector STATIC__Allocate_GDT_Selector

#undef  _Allocate_Global_V86_Data_Area
#define _Allocate_Global_V86_Data_Area STATIC__Allocate_Global_V86_Data_Area

#undef  Allocate_Global_V86_Data_Area
#define Allocate_Global_V86_Data_Area STATIC__Allocate_Global_V86_Data_Area

#undef  _Allocate_LDT_Selector
#define _Allocate_LDT_Selector STATIC__Allocate_LDT_Selector

#undef  Allocate_LDT_Selector
#define Allocate_LDT_Selector STATIC__Allocate_LDT_Selector

#undef  Allocate_PM_Call_Back
#define Allocate_PM_Call_Back STATIC_Allocate_PM_Call_Back

#undef  _Allocate_Temp_V86_Data_Area
#define _Allocate_Temp_V86_Data_Area STATIC__Allocate_Temp_V86_Data_Area

#undef  Allocate_Temp_V86_Data_Area
#define Allocate_Temp_V86_Data_Area STATIC__Allocate_Temp_V86_Data_Area

#undef  Allocate_V86_Call_Back
#define Allocate_V86_Call_Back STATIC_Allocate_V86_Call_Back

#undef  _Assign_Device_V86_Pages
#define _Assign_Device_V86_Pages STATIC__Assign_Device_V86_Pages

#undef  Assign_Device_V86_Pages
#define Assign_Device_V86_Pages STATIC__Assign_Device_V86_Pages

#undef  Begin_Critical_Section
#define Begin_Critical_Section STATIC_Begin_Critical_Section

#undef  Begin_Reentrant_Execution
#define Begin_Reentrant_Execution STATIC_Begin_Reentrant_Execution

#undef  _BuildDescriptorDWORDs
#define _BuildDescriptorDWORDs STATIC__BuildDescriptorDWORDs

#undef  BuildDescriptorDWORDs
#define BuildDescriptorDWORDs STATIC__BuildDescriptorDWORDs

#undef  Build_Int_Stack_Frame
#define Build_Int_Stack_Frame STATIC_Build_Int_Stack_Frame

#undef  Call_Global_Event
#define Call_Global_Event STATIC_Call_Global_Event

#undef  Call_Priority_VM_Event
#define Call_Priority_VM_Event STATIC_Call_Priority_VM_Event

#undef  Call_VM_Event
#define Call_VM_Event STATIC_Call_VM_Event

#undef  Call_When_Idle
#define Call_When_Idle STATIC_Call_When_Idle

#undef  Call_When_Not_Critical
#define Call_When_Not_Critical STATIC_Call_When_Not_Critical

#undef  Call_When_Task_Switched
#define Call_When_Task_Switched STATIC_Call_When_Task_Switched

#undef  Call_When_VM_Ints_Enabled
#define Call_When_VM_Ints_Enabled STATIC_Call_When_VM_Ints_Enabled

#undef  Call_When_VM_Returns
#define Call_When_VM_Returns STATIC_Call_When_VM_Returns

#undef  Cancel_Global_Event
#define Cancel_Global_Event STATIC_Cancel_Global_Event

#undef  Cancel_Priority_VM_Event
#define Cancel_Priority_VM_Event STATIC_Cancel_Priority_VM_Event

#undef  Cancel_Time_Out
#define Cancel_Time_Out STATIC_Cancel_Time_Out

#undef  Cancel_VM_Event
#define Cancel_VM_Event STATIC_Cancel_VM_Event

#undef  Claim_Critical_Section
#define Claim_Critical_Section STATIC_Claim_Critical_Section

#ifdef WIN40_OR_LATER
#undef  _RegOpenKey
#define _RegOpenKey STATIC__RegOpenKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegOpenKey
#define RegOpenKey STATIC__RegOpenKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCreateKey
#define _RegCreateKey STATIC__RegCreateKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCreateKey
#define RegCreateKey STATIC__RegCreateKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegDeleteKey
#define _RegDeleteKey STATIC__RegDeleteKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegDeleteKey
#define RegDeleteKey STATIC__RegDeleteKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegDeleteValue
#define _RegDeleteValue STATIC__RegDeleteValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegDeleteValue
#define RegDeleteValue STATIC__RegDeleteValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegEnumKey
#define _RegEnumKey STATIC__RegEnumKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegEnumKey
#define RegEnumKey STATIC__RegEnumKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegEnumValue
#define _RegEnumValue STATIC__RegEnumValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegEnumValue
#define RegEnumValue STATIC__RegEnumValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegFlushKey
#define _RegFlushKey STATIC__RegFlushKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegFlushKey
#define RegFlushKey STATIC__RegFlushKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCloseKey
#define _RegCloseKey STATIC__RegCloseKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCloseKey
#define RegCloseKey STATIC__RegCloseKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryValue
#define _RegQueryValue STATIC__RegQueryValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryValue
#define RegQueryValue STATIC__RegQueryValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryValueEx
#define _RegQueryValueEx STATIC__RegQueryValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryValueEx
#define RegQueryValueEx STATIC__RegQueryValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  _RegSetValue
#define _RegSetValue STATIC__RegSetValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegSetValue
#define RegSetValue STATIC__RegSetValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegSetValueEx
#define _RegSetValueEx STATIC__RegSetValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  RegSetValueEx
#define RegSetValueEx STATIC__RegSetValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  _EnterMutex
#define _EnterMutex STATIC__EnterMutex
#endif

#ifdef WIN40_OR_LATER
#undef  EnterMutex
#define EnterMutex STATIC__EnterMutex
#endif

#ifdef WIN40_OR_LATER
#undef  _GetMutexOwner
#define _GetMutexOwner STATIC__GetMutexOwner
#endif

#ifdef WIN40_OR_LATER
#undef  GetMutexOwner
#define GetMutexOwner STATIC__GetMutexOwner
#endif

#ifdef WIN40_OR_LATER
#undef  _LeaveMutex
#define _LeaveMutex STATIC__LeaveMutex
#endif

#ifdef WIN40_OR_LATER
#undef  LeaveMutex
#define LeaveMutex STATIC__LeaveMutex
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Fault
#define Unhook_V86_Fault STATIC_Unhook_V86_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_PM_Fault
#define Unhook_PM_Fault STATIC_Unhook_PM_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_VMM_Fault
#define Unhook_VMM_Fault STATIC_Unhook_VMM_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Int_Chain
#define Unhook_V86_Int_Chain STATIC_Unhook_V86_Int_Chain
#endif

#ifdef WIN40_OR_LATER
#undef  Signal_Semaphore_No_Switch
#define Signal_Semaphore_No_Switch STATIC_Signal_Semaphore_No_Switch
#endif

#ifdef WIN40_OR_LATER
#undef  Remove_IO_Handler
#define Remove_IO_Handler STATIC_Remove_IO_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_NMI_Event
#define Unhook_NMI_Event STATIC_Unhook_NMI_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Not_Critical
#define Cancel_Call_When_Not_Critical STATIC_Cancel_Call_When_Not_Critical
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service
#define Unhook_Device_Service STATIC_Unhook_Device_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Async_Time_Out
#define Set_Async_Time_Out STATIC_Set_Async_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  Get_DDB
#define Get_DDB STATIC_Get_DDB
#endif

#ifdef WIN40_OR_LATER
#undef  _Trace_Out_Service
#define _Trace_Out_Service STATIC__Trace_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Trace_Out_Service
#define Trace_Out_Service STATIC__Trace_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Out_Service
#define _Debug_Out_Service STATIC__Debug_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Out_Service
#define Debug_Out_Service STATIC__Debug_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _AllocateThreadDataSlot
#define _AllocateThreadDataSlot STATIC__AllocateThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  AllocateThreadDataSlot
#define AllocateThreadDataSlot STATIC__AllocateThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  _FreeThreadDataSlot
#define _FreeThreadDataSlot STATIC__FreeThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  FreeThreadDataSlot
#define FreeThreadDataSlot STATIC__FreeThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  _GetLastUpdatedThreadExecTime
#define _GetLastUpdatedThreadExecTime STATIC__GetLastUpdatedThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  GetLastUpdatedThreadExecTime
#define GetLastUpdatedThreadExecTime STATIC__GetLastUpdatedThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  VMMAddImportModuleName
#define VMMAddImportModuleName STATIC_VMMAddImportModuleName
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_Add_DDB
#define VMM_Add_DDB STATIC_VMM_Add_DDB
#endif

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
#undef  VMM_Remove_DDB
#define VMM_Remove_DDB STATIC_VMM_Remove_DDB
#endif

#ifdef WIN40_OR_LATER
#undef  Test_VM_Ints_Enabled
#define Test_VM_Ints_Enabled STATIC_Test_VM_Ints_Enabled
#endif

#ifdef WIN40_OR_LATER
#undef  _BlockOnID
#define _BlockOnID STATIC__BlockOnID
#endif

#ifdef WIN40_OR_LATER
#undef  BlockOnID
#define BlockOnID STATIC__BlockOnID
#endif

#ifdef WIN40_OR_LATER
#undef  Schedule_Thread_Event
#define Schedule_Thread_Event STATIC_Schedule_Thread_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Thread_Event
#define Cancel_Thread_Event STATIC_Cancel_Thread_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Time_Out
#define Set_Thread_Time_Out STATIC_Set_Thread_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  _CreateMutex
#define _CreateMutex STATIC__CreateMutex
#endif

#ifdef WIN40_OR_LATER
#undef  CreateMutex
#define CreateMutex STATIC__CreateMutex
#endif

#ifdef WIN40_OR_LATER
#undef  _DestroyMutex
#define _DestroyMutex STATIC__DestroyMutex
#endif

#ifdef WIN40_OR_LATER
#undef  DestroyMutex
#define DestroyMutex STATIC__DestroyMutex
#endif

#ifdef WIN40_OR_LATER
#undef  Call_When_Thread_Switched
#define Call_When_Thread_Switched STATIC_Call_When_Thread_Switched
#endif

#ifdef WIN40_OR_LATER
#undef  VMMCreateThread
#define VMMCreateThread STATIC_VMMCreateThread
#endif

#ifdef WIN40_OR_LATER
#undef  _GetThreadExecTime
#define _GetThreadExecTime STATIC__GetThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  GetThreadExecTime
#define GetThreadExecTime STATIC__GetThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  VMMTerminateThread
#define VMMTerminateThread STATIC_VMMTerminateThread
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Cur_Thread_Handle
#define Get_Cur_Thread_Handle STATIC_Get_Cur_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Cur_Thread_Handle
#define Test_Cur_Thread_Handle STATIC_Test_Cur_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Sys_Thread_Handle
#define Get_Sys_Thread_Handle STATIC_Get_Sys_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Sys_Thread_Handle
#define Test_Sys_Thread_Handle STATIC_Test_Sys_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Validate_Thread_Handle
#define Validate_Thread_Handle STATIC_Validate_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Initial_Thread_Handle
#define Get_Initial_Thread_Handle STATIC_Get_Initial_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Initial_Thread_Handle
#define Test_Initial_Thread_Handle STATIC_Test_Initial_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Valid_Thread_Handle
#define Debug_Test_Valid_Thread_Handle STATIC_Debug_Test_Valid_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Cur_Thread
#define Debug_Test_Cur_Thread STATIC_Debug_Test_Cur_Thread
#endif

#undef  VMM_GetSystemInitState
#define VMM_GetSystemInitState STATIC_VMM_GetSystemInitState

#undef  Cancel_Call_When_Thread_Switched
#define Cancel_Call_When_Thread_Switched STATIC_Cancel_Call_When_Thread_Switched

#undef  Get_Next_Thread_Handle
#define Get_Next_Thread_Handle STATIC_Get_Next_Thread_Handle

#ifdef WIN40_OR_LATER
#undef  Adjust_Thread_Exec_Priority
#define Adjust_Thread_Exec_Priority STATIC_Adjust_Thread_Exec_Priority
#endif

#ifdef WIN40_OR_LATER
#undef  _Deallocate_Device_CB_Area
#define _Deallocate_Device_CB_Area STATIC__Deallocate_Device_CB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  Deallocate_Device_CB_Area
#define Deallocate_Device_CB_Area STATIC__Deallocate_Device_CB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  Remove_Mult_IO_Handlers
#define Remove_Mult_IO_Handlers STATIC_Remove_Mult_IO_Handlers
#endif

#ifdef WIN40_OR_LATER
#undef  _PageReserve
#define _PageReserve STATIC__PageReserve
#endif

#ifdef WIN40_OR_LATER
#undef  PageReserve
#define PageReserve STATIC__PageReserve
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommit
#define _PageCommit STATIC__PageCommit
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommit
#define PageCommit STATIC__PageCommit
#endif

#ifdef WIN40_OR_LATER
#undef  _PageDecommit
#define _PageDecommit STATIC__PageDecommit
#endif

#ifdef WIN40_OR_LATER
#undef  PageDecommit
#define PageDecommit STATIC__PageDecommit
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerRegister
#define _PagerRegister STATIC__PagerRegister
#endif

#ifdef WIN40_OR_LATER
#undef  PagerRegister
#define PagerRegister STATIC__PagerRegister
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerQuery
#define _PagerQuery STATIC__PagerQuery
#endif

#ifdef WIN40_OR_LATER
#undef  PagerQuery
#define PagerQuery STATIC__PagerQuery
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerDeregister
#define _PagerDeregister STATIC__PagerDeregister
#endif

#ifdef WIN40_OR_LATER
#undef  PagerDeregister
#define PagerDeregister STATIC__PagerDeregister
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextCreate
#define _ContextCreate STATIC__ContextCreate
#endif

#ifdef WIN40_OR_LATER
#undef  ContextCreate
#define ContextCreate STATIC__ContextCreate
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextDestroy
#define _ContextDestroy STATIC__ContextDestroy
#endif

#ifdef WIN40_OR_LATER
#undef  ContextDestroy
#define ContextDestroy STATIC__ContextDestroy
#endif

#ifdef WIN40_OR_LATER
#undef  _PageAttach
#define _PageAttach STATIC__PageAttach
#endif

#ifdef WIN40_OR_LATER
#undef  PageAttach
#define PageAttach STATIC__PageAttach
#endif

#ifdef WIN40_OR_LATER
#undef  _PageFlush
#define _PageFlush STATIC__PageFlush
#endif

#ifdef WIN40_OR_LATER
#undef  PageFlush
#define PageFlush STATIC__PageFlush
#endif

#ifdef WIN40_OR_LATER
#undef  _SignalID
#define _SignalID STATIC__SignalID
#endif

#ifdef WIN40_OR_LATER
#undef  SignalID
#define SignalID STATIC__SignalID
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommitPhys
#define _PageCommitPhys STATIC__PageCommitPhys
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommitPhys
#define PageCommitPhys STATIC__PageCommitPhys
#endif

#ifdef WIN40_OR_LATER
#undef  _Register_Win32_Services
#define _Register_Win32_Services STATIC__Register_Win32_Services
#endif

#ifdef WIN40_OR_LATER
#undef  Register_Win32_Services
#define Register_Win32_Services STATIC__Register_Win32_Services
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Idle
#define Cancel_Call_When_Idle STATIC_Cancel_Call_When_Idle
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Task_Switched
#define Cancel_Call_When_Task_Switched STATIC_Cancel_Call_When_Task_Switched
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Inst_V86_Int_Vec_Base
#define Get_Inst_V86_Int_Vec_Base STATIC_Get_Inst_V86_Int_Vec_Base
#endif

#ifdef WIN40_OR_LATER
#undef  Simulate_VM_IO
#define Simulate_VM_IO STATIC_Simulate_VM_IO
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextSwitch
#define _ContextSwitch STATIC__ContextSwitch
#endif

#ifdef WIN40_OR_LATER
#undef  ContextSwitch
#define ContextSwitch STATIC__ContextSwitch
#endif

#ifdef WIN40_OR_LATER
#undef  _PageModifyPermissions
#define _PageModifyPermissions STATIC__PageModifyPermissions
#endif

#ifdef WIN40_OR_LATER
#undef  PageModifyPermissions
#define PageModifyPermissions STATIC__PageModifyPermissions
#endif

#ifdef WIN40_OR_LATER
#undef  _PageQuery
#define _PageQuery STATIC__PageQuery
#endif

#ifdef WIN40_OR_LATER
#undef  PageQuery
#define PageQuery STATIC__PageQuery
#endif

#ifdef WIN40_OR_LATER
#undef  _GetThreadTerminationStatus
#define _GetThreadTerminationStatus STATIC__GetThreadTerminationStatus
#endif

#ifdef WIN40_OR_LATER
#undef  GetThreadTerminationStatus
#define GetThreadTerminationStatus STATIC__GetThreadTerminationStatus
#endif

#ifdef WIN40_OR_LATER
#undef  _GetInstanceInfo
#define _GetInstanceInfo STATIC__GetInstanceInfo
#endif

#ifdef WIN40_OR_LATER
#undef  GetInstanceInfo
#define GetInstanceInfo STATIC__GetInstanceInfo
#endif

#ifdef WIN40_OR_LATER
#undef  _ExecIntMustComplete
#define _ExecIntMustComplete STATIC__ExecIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ExecIntMustComplete
#define ExecIntMustComplete STATIC__ExecIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _ExecVxDIntMustComplete
#define _ExecVxDIntMustComplete STATIC__ExecVxDIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ExecVxDIntMustComplete
#define ExecVxDIntMustComplete STATIC__ExecVxDIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  Begin_V86_Serialization
#define Begin_V86_Serialization STATIC_Begin_V86_Serialization
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Page
#define Unhook_V86_Page STATIC_Unhook_V86_Page
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_GetVxDLocationList
#define VMM_GetVxDLocationList STATIC_VMM_GetVxDLocationList
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_GetDDBList
#define VMM_GetDDBList STATIC_VMM_GetDDBList
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Instanced_V86_Int_Vector
#define Get_Instanced_V86_Int_Vector STATIC_Get_Instanced_V86_Int_Vector
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Set_Real_DOS_PSP
#define Get_Set_Real_DOS_PSP STATIC_Get_Set_Real_DOS_PSP
#endif

#ifdef WIN40_OR_LATER
#undef  Get_System_Time_Address
#define Get_System_Time_Address STATIC_Get_System_Time_Address
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Crit_Status_Thread
#define Get_Crit_Status_Thread STATIC_Get_Crit_Status_Thread
#endif

#ifdef WIN40_OR_LATER
#undef  Directed_Sys_Control
#define Directed_Sys_Control STATIC_Directed_Sys_Control
#endif

#ifdef WIN40_OR_LATER
#undef  _CallRing3
#define _CallRing3 STATIC__CallRing3
#endif

#ifdef WIN40_OR_LATER
#undef  CallRing3
#define CallRing3 STATIC__CallRing3
#endif

#ifdef WIN40_OR_LATER
#undef  Exec_PM_Int
#define Exec_PM_Int STATIC_Exec_PM_Int
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommitContig
#define _PageCommitContig STATIC__PageCommitContig
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommitContig
#define PageCommitContig STATIC__PageCommitContig
#endif

#ifdef WIN40_OR_LATER
#undef  _GetCurrentContext
#define _GetCurrentContext STATIC__GetCurrentContext
#endif

#ifdef WIN40_OR_LATER
#undef  GetCurrentContext
#define GetCurrentContext STATIC__GetCurrentContext
#endif

#ifdef WIN40_OR_LATER
#undef  Call_Restricted_Event
#define Call_Restricted_Event STATIC_Call_Restricted_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Restricted_Event
#define Cancel_Restricted_Event STATIC_Cancel_Restricted_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Register_PEF_Provider
#define Register_PEF_Provider STATIC_Register_PEF_Provider
#endif

#ifdef WIN40_OR_LATER
#undef  _GetPhysPageInfo
#define _GetPhysPageInfo STATIC__GetPhysPageInfo
#endif

#ifdef WIN40_OR_LATER
#undef  GetPhysPageInfo
#define GetPhysPageInfo STATIC__GetPhysPageInfo
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryInfoKey
#define _RegQueryInfoKey STATIC__RegQueryInfoKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryInfoKey
#define RegQueryInfoKey STATIC__RegQueryInfoKey
#endif

#ifdef WIN40_OR_LATER
#undef  Time_Slice_Sleep
#define Time_Slice_Sleep STATIC_Time_Slice_Sleep
#endif

#ifdef WIN40_OR_LATER
#undef  Boost_With_Decay
#define Boost_With_Decay STATIC_Boost_With_Decay
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Inversion_Pri
#define Set_Inversion_Pri STATIC_Set_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Reset_Inversion_Pri
#define Reset_Inversion_Pri STATIC_Reset_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Release_Inversion_Pri
#define Release_Inversion_Pri STATIC_Release_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Thread_Win32_Pri
#define Get_Thread_Win32_Pri STATIC_Get_Thread_Win32_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Win32_Pri
#define Set_Thread_Win32_Pri STATIC_Set_Thread_Win32_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Static_Boost
#define Set_Thread_Static_Boost STATIC_Set_Thread_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  Set_VM_Static_Boost
#define Set_VM_Static_Boost STATIC_Set_VM_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  Release_Inversion_Pri_ID
#define Release_Inversion_Pri_ID STATIC_Release_Inversion_Pri_ID
#endif

#ifdef WIN40_OR_LATER
#undef  Attach_Thread_To_Group
#define Attach_Thread_To_Group STATIC_Attach_Thread_To_Group
#endif

#ifdef WIN40_OR_LATER
#undef  Detach_Thread_From_Group
#define Detach_Thread_From_Group STATIC_Detach_Thread_From_Group
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Group_Static_Boost
#define Set_Group_Static_Boost STATIC_Set_Group_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  _GetRegistryPath
#define _GetRegistryPath STATIC__GetRegistryPath
#endif

#ifdef WIN40_OR_LATER
#undef  GetRegistryPath
#define GetRegistryPath STATIC__GetRegistryPath
#endif

#ifdef WIN40_OR_LATER
#undef  _RegRemapPreDefKey
#define _RegRemapPreDefKey STATIC__RegRemapPreDefKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegRemapPreDefKey
#define RegRemapPreDefKey STATIC__RegRemapPreDefKey
#endif

#ifdef WIN40_OR_LATER
#undef  End_V86_Serialization
#define End_V86_Serialization STATIC_End_V86_Serialization
#endif

#ifdef WIN40_OR_LATER
#undef  _Assert_Range
#define _Assert_Range STATIC__Assert_Range
#endif

#ifdef WIN40_OR_LATER
#undef  Assert_Range
#define Assert_Range STATIC__Assert_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _PageChangePager
#define _PageChangePager STATIC__PageChangePager
#endif

#ifdef WIN40_OR_LATER
#undef  PageChangePager
#define PageChangePager STATIC__PageChangePager
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCreateDynKey
#define _RegCreateDynKey STATIC__RegCreateDynKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCreateDynKey
#define RegCreateDynKey STATIC__RegCreateDynKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryMultipleValues
#define _RegQueryMultipleValues STATIC__RegQueryMultipleValues
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryMultipleValues
#define RegQueryMultipleValues STATIC__RegQueryMultipleValues
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Boot_Flags
#define Get_Boot_Flags STATIC_Get_Boot_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Boot_Flags
#define Set_Boot_Flags STATIC_Set_Boot_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrcpyn
#define _lstrcpyn STATIC__lstrcpyn
#endif

#ifdef WIN40_OR_LATER
#undef  lstrcpyn
#define lstrcpyn STATIC__lstrcpyn
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrlen
#define _lstrlen STATIC__lstrlen
#endif

#ifdef WIN40_OR_LATER
#undef  lstrlen
#define lstrlen STATIC__lstrlen
#endif

#ifdef WIN40_OR_LATER
#undef  _lmemcpy
#define _lmemcpy STATIC__lmemcpy
#endif

#ifdef WIN40_OR_LATER
#undef  lmemcpy
#define lmemcpy STATIC__lmemcpy
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrcmpi
#define _lstrcmpi STATIC__lstrcmpi
#endif

#ifdef WIN40_OR_LATER
#undef  lstrcmpi
#define lstrcmpi STATIC__lstrcmpi
#endif

#ifdef WIN40_OR_LATER
#undef  _GetVxDName
#define _GetVxDName STATIC__GetVxDName
#endif

#ifdef WIN40_OR_LATER
#undef  GetVxDName
#define GetVxDName STATIC__GetVxDName
#endif

#undef  BlockDev_Command_Complete
#define BlockDev_Command_Complete STATIC_BlockDev_Command_Complete

#undef  BlockDev_Find_Int13_Drive
#define BlockDev_Find_Int13_Drive STATIC_BlockDev_Find_Int13_Drive

#undef  BlockDev_Get_Device_List
#define BlockDev_Get_Device_List STATIC_BlockDev_Get_Device_List

#undef  BlockDev_Get_Version
#define BlockDev_Get_Version STATIC_BlockDev_Get_Version

#undef  BlockDev_Register_Device
#define BlockDev_Register_Device STATIC_BlockDev_Register_Device

#undef  BlockDev_Send_Command
#define BlockDev_Send_Command STATIC_BlockDev_Send_Command

#undef  BlockDev_Synchronous_Command
#define BlockDev_Synchronous_Command STATIC_BlockDev_Synchronous_Command

#undef  BlockDev_Thunk_Sync_Cmd
#define BlockDev_Thunk_Sync_Cmd STATIC_BlockDev_Thunk_Sync_Cmd

#undef  BlockDev_Thunk_Command
#define BlockDev_Thunk_Command STATIC_BlockDev_Thunk_Command

#undef  BlockDev_Thunk_HW_Int
#define BlockDev_Thunk_HW_Int STATIC_BlockDev_Thunk_HW_Int

#undef  DOSMGR_Add_Device
#define DOSMGR_Add_Device STATIC_DOSMGR_Add_Device

#undef  DOSMGR_BackFill_Allowed
#define DOSMGR_BackFill_Allowed STATIC_DOSMGR_BackFill_Allowed

#undef  DOSMGR_Copy_VM_Drive_State
#define DOSMGR_Copy_VM_Drive_State STATIC_DOSMGR_Copy_VM_Drive_State

#undef  DOSMGR_Enable_Indos_Polling
#define DOSMGR_Enable_Indos_Polling STATIC_DOSMGR_Enable_Indos_Polling

#undef  _DOSMGR_Exec_VM
#define _DOSMGR_Exec_VM STATIC__DOSMGR_Exec_VM

#undef  DOSMGR_Exec_VM
#define DOSMGR_Exec_VM STATIC__DOSMGR_Exec_VM

#undef  DOSMGR_Get_DOS_Crit_Status
#define DOSMGR_Get_DOS_Crit_Status STATIC_DOSMGR_Get_DOS_Crit_Status

#undef  DOSMGR_Get_IndosPtr
#define DOSMGR_Get_IndosPtr STATIC_DOSMGR_Get_IndosPtr

#undef  DOSMGR_Get_Version
#define DOSMGR_Get_Version STATIC_DOSMGR_Get_Version

#undef  DOSMGR_Instance_Device
#define DOSMGR_Instance_Device STATIC_DOSMGR_Instance_Device

#undef  DOSMGR_LocalGlobalReg
#define DOSMGR_LocalGlobalReg STATIC_DOSMGR_LocalGlobalReg

#undef  DOSMGR_Remove_Device
#define DOSMGR_Remove_Device STATIC_DOSMGR_Remove_Device

#undef  _DOSMGR_Set_Exec_VM_Data
#define _DOSMGR_Set_Exec_VM_Data STATIC__DOSMGR_Set_Exec_VM_Data

#undef  DOSMGR_Set_Exec_VM_Data
#define DOSMGR_Set_Exec_VM_Data STATIC__DOSMGR_Set_Exec_VM_Data

#undef  DOSNET_Do_PSP_Adjust
#define DOSNET_Do_PSP_Adjust STATIC_DOSNET_Do_PSP_Adjust

#undef  DOSNET_Get_Version
#define DOSNET_Get_Version STATIC_DOSNET_Get_Version

#undef  DOSNET_Send_FILESYSCHANGE
#define DOSNET_Send_FILESYSCHANGE STATIC_DOSNET_Send_FILESYSCHANGE

#undef  EBIOS_Get_Unused_Mem
#define EBIOS_Get_Unused_Mem STATIC_EBIOS_Get_Unused_Mem

#undef  EBIOS_Get_Version
#define EBIOS_Get_Version STATIC_EBIOS_Get_Version

#undef  Int13_Device_Registered
#define Int13_Device_Registered STATIC_Int13_Device_Registered

#undef  Int13_Hooking_BIOS_Int
#define Int13_Hooking_BIOS_Int STATIC_Int13_Hooking_BIOS_Int

#undef  Int13_Translate_VM_Int
#define Int13_Translate_VM_Int STATIC_Int13_Translate_VM_Int

#undef  Int13_Unhooking_BIOS_Int
#define Int13_Unhooking_BIOS_Int STATIC_Int13_Unhooking_BIOS_Int

#undef  VMD_Get_Mouse_Owner
#define VMD_Get_Mouse_Owner STATIC_VMD_Get_Mouse_Owner

#undef  VMD_Get_Version
#define VMD_Get_Version STATIC_VMD_Get_Version

#undef  VMD_Set_Mouse_Type
#define VMD_Set_Mouse_Type STATIC_VMD_Set_Mouse_Type

#undef  SHELL_Event
#define SHELL_Event STATIC_SHELL_Event

#undef  SHELL_Get_Version
#define SHELL_Get_Version STATIC_SHELL_Get_Version

#undef  SHELL_GetVMInfo
#define SHELL_GetVMInfo STATIC_SHELL_GetVMInfo

#undef  SHELL_Message
#define SHELL_Message STATIC_SHELL_Message

#undef  SHELL_Resolve_Contention
#define SHELL_Resolve_Contention STATIC_SHELL_Resolve_Contention

#undef  SHELL_SYSMODAL_Message
#define SHELL_SYSMODAL_Message STATIC_SHELL_SYSMODAL_Message

#ifdef WIN40_OR_LATER
#undef  _SHELL_CallAtAppyTime
#define _SHELL_CallAtAppyTime STATIC__SHELL_CallAtAppyTime
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CallAtAppyTime
#define SHELL_CallAtAppyTime STATIC__SHELL_CallAtAppyTime
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_CancelAppyTimeEvent
#define _SHELL_CancelAppyTimeEvent STATIC__SHELL_CancelAppyTimeEvent
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CancelAppyTimeEvent
#define SHELL_CancelAppyTimeEvent STATIC__SHELL_CancelAppyTimeEvent
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_QueryAppyTimeAvailable
#define _SHELL_QueryAppyTimeAvailable STATIC__SHELL_QueryAppyTimeAvailable
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_QueryAppyTimeAvailable
#define SHELL_QueryAppyTimeAvailable STATIC__SHELL_QueryAppyTimeAvailable
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LocalAllocEx
#define _SHELL_LocalAllocEx STATIC__SHELL_LocalAllocEx
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LocalAllocEx
#define SHELL_LocalAllocEx STATIC__SHELL_LocalAllocEx
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LocalFree
#define _SHELL_LocalFree STATIC__SHELL_LocalFree
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LocalFree
#define SHELL_LocalFree STATIC__SHELL_LocalFree
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_CallDll
#define _SHELL_CallDll STATIC__SHELL_CallDll
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CallDll
#define SHELL_CallDll STATIC__SHELL_CallDll
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_BroadcastSystemMessage
#define _SHELL_BroadcastSystemMessage STATIC__SHELL_BroadcastSystemMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_BroadcastSystemMessage
#define SHELL_BroadcastSystemMessage STATIC__SHELL_BroadcastSystemMessage
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_HookSystemBroadcast
#define _SHELL_HookSystemBroadcast STATIC__SHELL_HookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_HookSystemBroadcast
#define SHELL_HookSystemBroadcast STATIC__SHELL_HookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_UnhookSystemBroadcast
#define _SHELL_UnhookSystemBroadcast STATIC__SHELL_UnhookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_UnhookSystemBroadcast
#define SHELL_UnhookSystemBroadcast STATIC__SHELL_UnhookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_FreeLibrary
#define _SHELL_FreeLibrary STATIC__SHELL_FreeLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_FreeLibrary
#define SHELL_FreeLibrary STATIC__SHELL_FreeLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_GetProcAddress
#define _SHELL_GetProcAddress STATIC__SHELL_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_GetProcAddress
#define SHELL_GetProcAddress STATIC__SHELL_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LoadLibrary
#define _SHELL_LoadLibrary STATIC__SHELL_LoadLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LoadLibrary
#define SHELL_LoadLibrary STATIC__SHELL_LoadLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_PostMessage
#define _SHELL_PostMessage STATIC__SHELL_PostMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_PostMessage
#define SHELL_PostMessage STATIC__SHELL_PostMessage
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_PostShellMessage
#define _SHELL_PostShellMessage STATIC__SHELL_PostShellMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_PostShellMessage
#define SHELL_PostShellMessage STATIC__SHELL_PostShellMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Hook_Properties
#define SHELL_Hook_Properties STATIC_SHELL_Hook_Properties
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Unhook_Properties
#define SHELL_Unhook_Properties STATIC_SHELL_Unhook_Properties
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_ShellExecute
#define _SHELL_ShellExecute STATIC__SHELL_ShellExecute
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_ShellExecute
#define SHELL_ShellExecute STATIC__SHELL_ShellExecute
#endif

#undef  V86MMGR_Allocate_Buffer
#define V86MMGR_Allocate_Buffer STATIC_V86MMGR_Allocate_Buffer

#undef  V86MMGR_Allocate_V86_Pages
#define V86MMGR_Allocate_V86_Pages STATIC_V86MMGR_Allocate_V86_Pages

#undef  V86MMGR_Free_Buffer
#define V86MMGR_Free_Buffer STATIC_V86MMGR_Free_Buffer

#undef  V86MMGR_Free_Page_Map_Region
#define V86MMGR_Free_Page_Map_Region STATIC_V86MMGR_Free_Page_Map_Region

#undef  V86MMGR_Get_EMS_XMS_Limits
#define V86MMGR_Get_EMS_XMS_Limits STATIC_V86MMGR_Get_EMS_XMS_Limits

#undef  V86MMGR_Get_Mapping_Info
#define V86MMGR_Get_Mapping_Info STATIC_V86MMGR_Get_Mapping_Info

#undef  V86MMGR_Get_Version
#define V86MMGR_Get_Version STATIC_V86MMGR_Get_Version

#undef  V86MMGR_Get_VM_Flat_Sel
#define V86MMGR_Get_VM_Flat_Sel STATIC_V86MMGR_Get_VM_Flat_Sel

#undef  V86MMGR_Get_Xlat_Buff_State
#define V86MMGR_Get_Xlat_Buff_State STATIC_V86MMGR_Get_Xlat_Buff_State

#undef  V86MMGR_GetPgStatus
#define V86MMGR_GetPgStatus STATIC_V86MMGR_GetPgStatus

#undef  V86MMGR_Load_Client_Ptr
#define V86MMGR_Load_Client_Ptr STATIC_V86MMGR_Load_Client_Ptr

#undef  V86MMGR_LocalGlobalReg
#define V86MMGR_LocalGlobalReg STATIC_V86MMGR_LocalGlobalReg

#undef  V86MMGR_Map_Pages
#define V86MMGR_Map_Pages STATIC_V86MMGR_Map_Pages

#undef  V86MMGR_ResetBasePages
#define V86MMGR_ResetBasePages STATIC_V86MMGR_ResetBasePages

#undef  V86MMGR_Set_EMS_XMS_Limits
#define V86MMGR_Set_EMS_XMS_Limits STATIC_V86MMGR_Set_EMS_XMS_Limits

#undef  V86MMGR_Set_Mapping_Info
#define V86MMGR_Set_Mapping_Info STATIC_V86MMGR_Set_Mapping_Info

#undef  V86MMGR_Set_Xlat_Buff_State
#define V86MMGR_Set_Xlat_Buff_State STATIC_V86MMGR_Set_Xlat_Buff_State

#undef  V86MMGR_SetAvailMapPgs
#define V86MMGR_SetAvailMapPgs STATIC_V86MMGR_SetAvailMapPgs

#undef  V86MMGR_Xlat_API
#define V86MMGR_Xlat_API STATIC_V86MMGR_Xlat_API

#undef  VCD_Get_Focus
#define VCD_Get_Focus STATIC_VCD_Get_Focus

#undef  VCD_Get_Version
#define VCD_Get_Version STATIC_VCD_Get_Version

#undef  VCD_Set_Port_Global
#define VCD_Set_Port_Global STATIC_VCD_Set_Port_Global

#undef  VCD_Virtualize_Port
#define VCD_Virtualize_Port STATIC_VCD_Virtualize_Port

#ifdef WIN40_OR_LATER
#undef  VMCPD_Set_CR0_State
#define VMCPD_Set_CR0_State STATIC_VMCPD_Set_CR0_State
#endif

#ifdef WIN40_OR_LATER
#undef  VMCPD_Get_CR0_State
#define VMCPD_Get_CR0_State STATIC_VMCPD_Get_CR0_State
#endif

#undef  VMCPD_Get_Thread_State
#define VMCPD_Get_Thread_State STATIC_VMCPD_Get_Thread_State

#ifdef WIN40_OR_LATER
#undef  VMCPD_Set_Thread_State
#define VMCPD_Set_Thread_State STATIC_VMCPD_Set_Thread_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Virtual_EOI
#define VPICD_Virtual_EOI STATIC_VPICD_Virtual_EOI
#endif

#ifdef WIN40_OR_LATER
#undef  VTD_Get_Date_And_Time
#define VTD_Get_Date_And_Time STATIC_VTD_Get_Date_And_Time
#endif

#undef  VDD_Get_ModTime
#define VDD_Get_ModTime STATIC_VDD_Get_ModTime

#undef  VDD_Get_Version
#define VDD_Get_Version STATIC_VDD_Get_Version

#undef  VDD_Msg_BakColor
#define VDD_Msg_BakColor STATIC_VDD_Msg_BakColor

#undef  VDD_Msg_ClrScrn
#define VDD_Msg_ClrScrn STATIC_VDD_Msg_ClrScrn

#undef  VDD_Msg_ForColor
#define VDD_Msg_ForColor STATIC_VDD_Msg_ForColor

#undef  VDD_SetCursPos
#define VDD_SetCursPos STATIC_VDD_SetCursPos

#undef  VDD_Msg_TextOut
#define VDD_Msg_TextOut STATIC_VDD_Msg_TextOut

#undef  VDD_PIF_State
#define VDD_PIF_State STATIC_VDD_PIF_State

#undef  VDD_Query_Access
#define VDD_Query_Access STATIC_VDD_Query_Access

#undef  VDD_Set_HCurTrk
#define VDD_Set_HCurTrk STATIC_VDD_Set_HCurTrk

#undef  VDD_Set_VMType
#define VDD_Set_VMType STATIC_VDD_Set_VMType

#undef  VDMAD_Copy_From_Buffer
#define VDMAD_Copy_From_Buffer STATIC_VDMAD_Copy_From_Buffer

#undef  VDMAD_Copy_To_Buffer
#define VDMAD_Copy_To_Buffer STATIC_VDMAD_Copy_To_Buffer

#undef  VDMAD_Default_Handler
#define VDMAD_Default_Handler STATIC_VDMAD_Default_Handler

#undef  VDMAD_Disable_Translation
#define VDMAD_Disable_Translation STATIC_VDMAD_Disable_Translation

#undef  VDMAD_Enable_Translation
#define VDMAD_Enable_Translation STATIC_VDMAD_Enable_Translation

#undef  VDMAD_Get_EISA_Adr_Mode
#define VDMAD_Get_EISA_Adr_Mode STATIC_VDMAD_Get_EISA_Adr_Mode

#undef  VDMAD_Get_Region_Info
#define VDMAD_Get_Region_Info STATIC_VDMAD_Get_Region_Info

#undef  VDMAD_Get_Version
#define VDMAD_Get_Version STATIC_VDMAD_Get_Version

#undef  VDMAD_Get_Virt_State
#define VDMAD_Get_Virt_State STATIC_VDMAD_Get_Virt_State

#undef  VDMAD_Lock_DMA_Region
#define VDMAD_Lock_DMA_Region STATIC_VDMAD_Lock_DMA_Region

#undef  VDMAD_Mask_Channel
#define VDMAD_Mask_Channel STATIC_VDMAD_Mask_Channel

#undef  VDMAD_Release_Buffer
#define VDMAD_Release_Buffer STATIC_VDMAD_Release_Buffer

#undef  VDMAD_Request_Buffer
#define VDMAD_Request_Buffer STATIC_VDMAD_Request_Buffer

#undef  VDMAD_Reserve_Buffer_Space
#define VDMAD_Reserve_Buffer_Space STATIC_VDMAD_Reserve_Buffer_Space

#undef  VDMAD_Scatter_Lock
#define VDMAD_Scatter_Lock STATIC_VDMAD_Scatter_Lock

#undef  VDMAD_Scatter_Unlock
#define VDMAD_Scatter_Unlock STATIC_VDMAD_Scatter_Unlock

#undef  VDMAD_Set_EISA_Adr_Mode
#define VDMAD_Set_EISA_Adr_Mode STATIC_VDMAD_Set_EISA_Adr_Mode

#undef  VDMAD_Set_Phys_State
#define VDMAD_Set_Phys_State STATIC_VDMAD_Set_Phys_State

#undef  VDMAD_Set_Region_Info
#define VDMAD_Set_Region_Info STATIC_VDMAD_Set_Region_Info

#undef  VDMAD_Set_Virt_State
#define VDMAD_Set_Virt_State STATIC_VDMAD_Set_Virt_State

#undef  VDMAD_Unlock_DMA_Region
#define VDMAD_Unlock_DMA_Region STATIC_VDMAD_Unlock_DMA_Region

#undef  VDMAD_UnMask_Channel
#define VDMAD_UnMask_Channel STATIC_VDMAD_UnMask_Channel

#undef  VDMAD_Virtualize_Channel
#define VDMAD_Virtualize_Channel STATIC_VDMAD_Virtualize_Channel

#undef  VKD_Cancel_Paste
#define VKD_Cancel_Paste STATIC_VKD_Cancel_Paste

#undef  VKD_Define_Hot_Key
#define VKD_Define_Hot_Key STATIC_VKD_Define_Hot_Key

#undef  VKD_Define_Paste_Mode
#define VKD_Define_Paste_Mode STATIC_VKD_Define_Paste_Mode

#undef  VKD_Flush_Msg_Key_Queue
#define VKD_Flush_Msg_Key_Queue STATIC_VKD_Flush_Msg_Key_Queue

#undef  VKD_Force_Keys
#define VKD_Force_Keys STATIC_VKD_Force_Keys

#undef  VKD_Get_Kbd_Owner
#define VKD_Get_Kbd_Owner STATIC_VKD_Get_Kbd_Owner

#undef  VKD_Get_Msg_Key
#define VKD_Get_Msg_Key STATIC_VKD_Get_Msg_Key

#undef  VKD_Get_Version
#define VKD_Get_Version STATIC_VKD_Get_Version

#undef  VKD_Local_Disable_Hot_Key
#define VKD_Local_Disable_Hot_Key STATIC_VKD_Local_Disable_Hot_Key

#undef  VKD_Local_Enable_Hot_Key
#define VKD_Local_Enable_Hot_Key STATIC_VKD_Local_Enable_Hot_Key

#undef  VKD_Peek_Msg_Key
#define VKD_Peek_Msg_Key STATIC_VKD_Peek_Msg_Key

#undef  VKD_Reflect_Hot_Key
#define VKD_Reflect_Hot_Key STATIC_VKD_Reflect_Hot_Key

#undef  VKD_Remove_Hot_Key
#define VKD_Remove_Hot_Key STATIC_VKD_Remove_Hot_Key

#undef  VKD_Start_Paste
#define VKD_Start_Paste STATIC_VKD_Start_Paste

#undef  VMCPD_Get_Version
#define VMCPD_Get_Version STATIC_VMCPD_Get_Version

#undef  VMCPD_Get_Virt_State
#define VMCPD_Get_Virt_State STATIC_VMCPD_Get_Virt_State

#undef  VMCPD_Set_Virt_State
#define VMCPD_Set_Virt_State STATIC_VMCPD_Set_Virt_State

#undef  VMPoll_Enable_Disable
#define VMPoll_Enable_Disable STATIC_VMPoll_Enable_Disable

#undef  VMPoll_Get_Version
#define VMPoll_Get_Version STATIC_VMPoll_Get_Version

#undef  VMPoll_Reset_Detection
#define VMPoll_Reset_Detection STATIC_VMPoll_Reset_Detection

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  VPICD_Call_When_Hw_Int
#define VPICD_Call_When_Hw_Int STATIC_VPICD_Call_When_Hw_Int

#undef  VPICD_Clear_Int_Request
#define VPICD_Clear_Int_Request STATIC_VPICD_Clear_Int_Request

#undef  VPICD_Convert_Handle_To_IRQ
#define VPICD_Convert_Handle_To_IRQ STATIC_VPICD_Convert_Handle_To_IRQ

#undef  VPICD_Convert_Int_To_IRQ
#define VPICD_Convert_Int_To_IRQ STATIC_VPICD_Convert_Int_To_IRQ

#undef  VPICD_Convert_IRQ_To_Int
#define VPICD_Convert_IRQ_To_Int STATIC_VPICD_Convert_IRQ_To_Int

#undef  VPICD_Force_Default_Behavior
#define VPICD_Force_Default_Behavior STATIC_VPICD_Force_Default_Behavior

#undef  VPICD_Force_Default_Owner
#define VPICD_Force_Default_Owner STATIC_VPICD_Force_Default_Owner

#undef  VPICD_Get_Complete_Status
#define VPICD_Get_Complete_Status STATIC_VPICD_Get_Complete_Status

#undef  VPICD_Get_IRQ_Complete_Status
#define VPICD_Get_IRQ_Complete_Status STATIC_VPICD_Get_IRQ_Complete_Status

#undef  VPICD_Get_Status
#define VPICD_Get_Status STATIC_VPICD_Get_Status

#undef  VPICD_Get_Version
#define VPICD_Get_Version STATIC_VPICD_Get_Version

#undef  VPICD_Phys_EOI
#define VPICD_Phys_EOI STATIC_VPICD_Phys_EOI

#undef  VPICD_Physically_Mask
#define VPICD_Physically_Mask STATIC_VPICD_Physically_Mask

#undef  VPICD_Physically_Unmask
#define VPICD_Physically_Unmask STATIC_VPICD_Physically_Unmask

#undef  VPICD_Set_Auto_Masking
#define VPICD_Set_Auto_Masking STATIC_VPICD_Set_Auto_Masking

#undef  VPICD_Set_Int_Request
#define VPICD_Set_Int_Request STATIC_VPICD_Set_Int_Request

#undef  VPICD_Test_Phys_Request
#define VPICD_Test_Phys_Request STATIC_VPICD_Test_Phys_Request

#undef  VPICD_Virtualize_IRQ
#define VPICD_Virtualize_IRQ STATIC_VPICD_Virtualize_IRQ

#ifdef WIN40_OR_LATER
#undef  VPICD_Auto_Mask_At_Inst_Swap
#define VPICD_Auto_Mask_At_Inst_Swap STATIC_VPICD_Auto_Mask_At_Inst_Swap
#endif

#undef  VSD_Get_Version
#define VSD_Get_Version STATIC_VSD_Get_Version

#undef  VTD_Begin_Min_Int_Period
#define VTD_Begin_Min_Int_Period STATIC_VTD_Begin_Min_Int_Period

#undef  VTD_Disable_Trapping
#define VTD_Disable_Trapping STATIC_VTD_Disable_Trapping

#undef  VTD_Enable_Trapping
#define VTD_Enable_Trapping STATIC_VTD_Enable_Trapping

#undef  VTD_End_Min_Int_Period
#define VTD_End_Min_Int_Period STATIC_VTD_End_Min_Int_Period

#undef  VTD_Get_Interrupt_Period
#define VTD_Get_Interrupt_Period STATIC_VTD_Get_Interrupt_Period

#undef  VTD_Get_Real_Time
#define VTD_Get_Real_Time STATIC_VTD_Get_Real_Time

#undef  VTD_Get_Version
#define VTD_Get_Version STATIC_VTD_Get_Version

#undef  VPICD_Thunk_HWInt
#define VPICD_Thunk_HWInt STATIC_VPICD_Thunk_HWInt

#undef  VPICD_Thunk_VirtInt
#define VPICD_Thunk_VirtInt STATIC_VPICD_Thunk_VirtInt

#undef  VPICD_Thunk_EOI
#define VPICD_Thunk_EOI STATIC_VPICD_Thunk_EOI

#undef  VPICD_Thunk_MaskChange
#define VPICD_Thunk_MaskChange STATIC_VPICD_Thunk_MaskChange

#undef  VPICD_Thunk_Iret
#define VPICD_Thunk_Iret STATIC_VPICD_Thunk_Iret

#undef  Install_Exception_Handler
#define Install_Exception_Handler STATIC_Install_Exception_Handler

#ifdef WFW311_OR_LATER
#undef  VXDLDR_GetDeviceList
#define VXDLDR_GetDeviceList STATIC_VXDLDR_GetDeviceList
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_GetVersion
#define VXDLDR_GetVersion STATIC_VXDLDR_GetVersion
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_LoadDevice
#define VXDLDR_LoadDevice STATIC_VXDLDR_LoadDevice
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_UnloadDevice
#define VXDLDR_UnloadDevice STATIC_VXDLDR_UnloadDevice
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_DevInitSucceeded
#define VXDLDR_DevInitSucceeded STATIC_VXDLDR_DevInitSucceeded
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_DevInitFailed
#define VXDLDR_DevInitFailed STATIC_VXDLDR_DevInitFailed
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_APM_BIOS_Version
#define _VPOWERD_Get_APM_BIOS_Version STATIC__VPOWERD_Get_APM_BIOS_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_APM_BIOS_Version
#define VPOWERD_Get_APM_BIOS_Version STATIC__VPOWERD_Get_APM_BIOS_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_State
#define _VPOWERD_Get_Power_State STATIC__VPOWERD_Get_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_State
#define VPOWERD_Get_Power_State STATIC__VPOWERD_Get_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_Management_Level
#define _VPOWERD_Get_Power_Management_Level STATIC__VPOWERD_Get_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_Management_Level
#define VPOWERD_Get_Power_Management_Level STATIC__VPOWERD_Get_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_Status
#define _VPOWERD_Get_Power_Status STATIC__VPOWERD_Get_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_Status
#define VPOWERD_Get_Power_Status STATIC__VPOWERD_Get_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_OEM_APM_Function
#define _VPOWERD_OEM_APM_Function STATIC__VPOWERD_OEM_APM_Function
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_OEM_APM_Function
#define VPOWERD_OEM_APM_Function STATIC__VPOWERD_OEM_APM_Function
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Register_Power_Handler
#define _VPOWERD_Register_Power_Handler STATIC__VPOWERD_Register_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Register_Power_Handler
#define VPOWERD_Register_Power_Handler STATIC__VPOWERD_Register_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Restore_Power_On_Defaults
#define _VPOWERD_Restore_Power_On_Defaults STATIC__VPOWERD_Restore_Power_On_Defaults
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Restore_Power_On_Defaults
#define VPOWERD_Restore_Power_On_Defaults STATIC__VPOWERD_Restore_Power_On_Defaults
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_Device_Power_State
#define _VPOWERD_Set_Device_Power_State STATIC__VPOWERD_Set_Device_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_Device_Power_State
#define VPOWERD_Set_Device_Power_State STATIC__VPOWERD_Set_Device_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_Power_Management_Level
#define _VPOWERD_Set_Power_Management_Level STATIC__VPOWERD_Set_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_Power_Management_Level
#define VPOWERD_Set_Power_Management_Level STATIC__VPOWERD_Set_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_System_Power_State
#define _VPOWERD_Set_System_Power_State STATIC__VPOWERD_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_System_Power_State
#define VPOWERD_Set_System_Power_State STATIC__VPOWERD_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Deregister_Power_Handler
#define _VPOWERD_Deregister_Power_Handler STATIC__VPOWERD_Deregister_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Deregister_Power_Handler
#define VPOWERD_Deregister_Power_Handler STATIC__VPOWERD_Deregister_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Version
#define _VPOWERD_Get_Version STATIC__VPOWERD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Version
#define VPOWERD_Get_Version STATIC__VPOWERD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_W32_Get_System_Power_Status
#define _VPOWERD_W32_Get_System_Power_Status STATIC__VPOWERD_W32_Get_System_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_W32_Get_System_Power_Status
#define VPOWERD_W32_Get_System_Power_Status STATIC__VPOWERD_W32_Get_System_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_W32_Set_System_Power_State
#define _VPOWERD_W32_Set_System_Power_State STATIC__VPOWERD_W32_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_W32_Set_System_Power_State
#define VPOWERD_W32_Set_System_Power_State STATIC__VPOWERD_W32_Set_System_Power_State
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Get_Version
#define VCOMM_Get_Version STATIC_VCOMM_Get_Version
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Acquire_Port
#define _VCOMM_Acquire_Port STATIC__VCOMM_Acquire_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Acquire_Port
#define VCOMM_Acquire_Port STATIC__VCOMM_Acquire_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Add_Port
#define _VCOMM_Add_Port STATIC__VCOMM_Add_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Add_Port
#define VCOMM_Add_Port STATIC__VCOMM_Add_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Register_Port_Driver
#define _VCOMM_Register_Port_Driver STATIC__VCOMM_Register_Port_Driver
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Register_Port_Driver
#define VCOMM_Register_Port_Driver STATIC__VCOMM_Register_Port_Driver
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Release_Port
#define _VCOMM_Release_Port STATIC__VCOMM_Release_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Release_Port
#define VCOMM_Release_Port STATIC__VCOMM_Release_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_ClearCommError
#define _VCOMM_ClearCommError STATIC__VCOMM_ClearCommError
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_ClearCommError
#define VCOMM_ClearCommError STATIC__VCOMM_ClearCommError
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_CloseComm
#define _VCOMM_CloseComm STATIC__VCOMM_CloseComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_CloseComm
#define VCOMM_CloseComm STATIC__VCOMM_CloseComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_EnableCommNotification
#define _VCOMM_EnableCommNotification STATIC__VCOMM_EnableCommNotification
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_EnableCommNotification
#define VCOMM_EnableCommNotification STATIC__VCOMM_EnableCommNotification
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_EscapeCommFunction
#define _VCOMM_EscapeCommFunction STATIC__VCOMM_EscapeCommFunction
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_EscapeCommFunction
#define VCOMM_EscapeCommFunction STATIC__VCOMM_EscapeCommFunction
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommEventMask
#define _VCOMM_GetCommEventMask STATIC__VCOMM_GetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommEventMask
#define VCOMM_GetCommEventMask STATIC__VCOMM_GetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommProperties
#define _VCOMM_GetCommProperties STATIC__VCOMM_GetCommProperties
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommProperties
#define VCOMM_GetCommProperties STATIC__VCOMM_GetCommProperties
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommQueueStatus
#define _VCOMM_GetCommQueueStatus STATIC__VCOMM_GetCommQueueStatus
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommQueueStatus
#define VCOMM_GetCommQueueStatus STATIC__VCOMM_GetCommQueueStatus
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommState
#define _VCOMM_GetCommState STATIC__VCOMM_GetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommState
#define VCOMM_GetCommState STATIC__VCOMM_GetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetLastError
#define _VCOMM_GetLastError STATIC__VCOMM_GetLastError
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetLastError
#define VCOMM_GetLastError STATIC__VCOMM_GetLastError
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetModemStatus
#define _VCOMM_GetModemStatus STATIC__VCOMM_GetModemStatus
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetModemStatus
#define VCOMM_GetModemStatus STATIC__VCOMM_GetModemStatus
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_GetSetCommTimeouts
#define _VCOMM_GetSetCommTimeouts STATIC__VCOMM_GetSetCommTimeouts
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_GetSetCommTimeouts
#define VCOMM_GetSetCommTimeouts STATIC__VCOMM_GetSetCommTimeouts
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_OpenComm
#define _VCOMM_OpenComm STATIC__VCOMM_OpenComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_OpenComm
#define VCOMM_OpenComm STATIC__VCOMM_OpenComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_PurgeComm
#define _VCOMM_PurgeComm STATIC__VCOMM_PurgeComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_PurgeComm
#define VCOMM_PurgeComm STATIC__VCOMM_PurgeComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_ReadComm
#define _VCOMM_ReadComm STATIC__VCOMM_ReadComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_ReadComm
#define VCOMM_ReadComm STATIC__VCOMM_ReadComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetCommEventMask
#define _VCOMM_SetCommEventMask STATIC__VCOMM_SetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetCommEventMask
#define VCOMM_SetCommEventMask STATIC__VCOMM_SetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetCommState
#define _VCOMM_SetCommState STATIC__VCOMM_SetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetCommState
#define VCOMM_SetCommState STATIC__VCOMM_SetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetReadCallback
#define _VCOMM_SetReadCallback STATIC__VCOMM_SetReadCallback
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetReadCallback
#define VCOMM_SetReadCallback STATIC__VCOMM_SetReadCallback
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetupComm
#define _VCOMM_SetupComm STATIC__VCOMM_SetupComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetupComm
#define VCOMM_SetupComm STATIC__VCOMM_SetupComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetWriteCallback
#define _VCOMM_SetWriteCallback STATIC__VCOMM_SetWriteCallback
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetWriteCallback
#define VCOMM_SetWriteCallback STATIC__VCOMM_SetWriteCallback
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_TransmitCommChar
#define _VCOMM_TransmitCommChar STATIC__VCOMM_TransmitCommChar
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_TransmitCommChar
#define VCOMM_TransmitCommChar STATIC__VCOMM_TransmitCommChar
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_WriteComm
#define _VCOMM_WriteComm STATIC__VCOMM_WriteComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_WriteComm
#define VCOMM_WriteComm STATIC__VCOMM_WriteComm
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Win32DCB_To_Ring0
#define VCOMM_Map_Win32DCB_To_Ring0 STATIC_VCOMM_Map_Win32DCB_To_Ring0
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Ring0DCB_To_Win32
#define VCOMM_Map_Ring0DCB_To_Win32 STATIC_VCOMM_Map_Ring0DCB_To_Win32
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Get_Contention_Handler
#define _VCOMM_Get_Contention_Handler STATIC__VCOMM_Get_Contention_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Get_Contention_Handler
#define VCOMM_Get_Contention_Handler STATIC__VCOMM_Get_Contention_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Map_Name_To_Resource
#define _VCOMM_Map_Name_To_Resource STATIC__VCOMM_Map_Name_To_Resource
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Name_To_Resource
#define VCOMM_Map_Name_To_Resource STATIC__VCOMM_Map_Name_To_Resource
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Version
#define _CONFIGMG_Get_Version STATIC__CONFIGMG_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Version
#define CONFIGMG_Get_Version STATIC__CONFIGMG_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Initialize
#define _CONFIGMG_Initialize STATIC__CONFIGMG_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Initialize
#define CONFIGMG_Initialize STATIC__CONFIGMG_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Locate_DevNode
#define _CONFIGMG_Locate_DevNode STATIC__CONFIGMG_Locate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Locate_DevNode
#define CONFIGMG_Locate_DevNode STATIC__CONFIGMG_Locate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Parent
#define _CONFIGMG_Get_Parent STATIC__CONFIGMG_Get_Parent
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Parent
#define CONFIGMG_Get_Parent STATIC__CONFIGMG_Get_Parent
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Child
#define _CONFIGMG_Get_Child STATIC__CONFIGMG_Get_Child
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Child
#define CONFIGMG_Get_Child STATIC__CONFIGMG_Get_Child
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Sibling
#define _CONFIGMG_Get_Sibling STATIC__CONFIGMG_Get_Sibling
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Sibling
#define CONFIGMG_Get_Sibling STATIC__CONFIGMG_Get_Sibling
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_ID_Size
#define _CONFIGMG_Get_Device_ID_Size STATIC__CONFIGMG_Get_Device_ID_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_ID_Size
#define CONFIGMG_Get_Device_ID_Size STATIC__CONFIGMG_Get_Device_ID_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_ID
#define _CONFIGMG_Get_Device_ID STATIC__CONFIGMG_Get_Device_ID
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_ID
#define CONFIGMG_Get_Device_ID STATIC__CONFIGMG_Get_Device_ID
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Depth
#define _CONFIGMG_Get_Depth STATIC__CONFIGMG_Get_Depth
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Depth
#define CONFIGMG_Get_Depth STATIC__CONFIGMG_Get_Depth
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Private_DWord
#define _CONFIGMG_Get_Private_DWord STATIC__CONFIGMG_Get_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Private_DWord
#define CONFIGMG_Get_Private_DWord STATIC__CONFIGMG_Get_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Private_DWord
#define _CONFIGMG_Set_Private_DWord STATIC__CONFIGMG_Set_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Private_DWord
#define CONFIGMG_Set_Private_DWord STATIC__CONFIGMG_Set_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Create_DevNode
#define _CONFIGMG_Create_DevNode STATIC__CONFIGMG_Create_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Create_DevNode
#define CONFIGMG_Create_DevNode STATIC__CONFIGMG_Create_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Remove_SubTree
#define _CONFIGMG_Query_Remove_SubTree STATIC__CONFIGMG_Query_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Remove_SubTree
#define CONFIGMG_Query_Remove_SubTree STATIC__CONFIGMG_Query_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Remove_SubTree
#define _CONFIGMG_Remove_SubTree STATIC__CONFIGMG_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Remove_SubTree
#define CONFIGMG_Remove_SubTree STATIC__CONFIGMG_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Device_Driver
#define _CONFIGMG_Register_Device_Driver STATIC__CONFIGMG_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Device_Driver
#define CONFIGMG_Register_Device_Driver STATIC__CONFIGMG_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Enumerator
#define _CONFIGMG_Register_Enumerator STATIC__CONFIGMG_Register_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Enumerator
#define CONFIGMG_Register_Enumerator STATIC__CONFIGMG_Register_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Arbitrator
#define _CONFIGMG_Register_Arbitrator STATIC__CONFIGMG_Register_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Arbitrator
#define CONFIGMG_Register_Arbitrator STATIC__CONFIGMG_Register_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Deregister_Arbitrator
#define _CONFIGMG_Deregister_Arbitrator STATIC__CONFIGMG_Deregister_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Deregister_Arbitrator
#define CONFIGMG_Deregister_Arbitrator STATIC__CONFIGMG_Deregister_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Res_Des
#define _CONFIGMG_Add_Res_Des STATIC__CONFIGMG_Add_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Res_Des
#define CONFIGMG_Add_Res_Des STATIC__CONFIGMG_Add_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Modify_Res_Des
#define _CONFIGMG_Modify_Res_Des STATIC__CONFIGMG_Modify_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Modify_Res_Des
#define CONFIGMG_Modify_Res_Des STATIC__CONFIGMG_Modify_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Res_Des
#define _CONFIGMG_Free_Res_Des STATIC__CONFIGMG_Free_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Res_Des
#define CONFIGMG_Free_Res_Des STATIC__CONFIGMG_Free_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Next_Res_Des
#define _CONFIGMG_Get_Next_Res_Des STATIC__CONFIGMG_Get_Next_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Next_Res_Des
#define CONFIGMG_Get_Next_Res_Des STATIC__CONFIGMG_Get_Next_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Performance_Info
#define _CONFIGMG_Get_Performance_Info STATIC__CONFIGMG_Get_Performance_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Performance_Info
#define CONFIGMG_Get_Performance_Info STATIC__CONFIGMG_Get_Performance_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Res_Des_Data_Size
#define _CONFIGMG_Get_Res_Des_Data_Size STATIC__CONFIGMG_Get_Res_Des_Data_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Res_Des_Data_Size
#define CONFIGMG_Get_Res_Des_Data_Size STATIC__CONFIGMG_Get_Res_Des_Data_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Res_Des_Data
#define _CONFIGMG_Get_Res_Des_Data STATIC__CONFIGMG_Get_Res_Des_Data
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Res_Des_Data
#define CONFIGMG_Get_Res_Des_Data STATIC__CONFIGMG_Get_Res_Des_Data
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Process_Events_Now
#define _CONFIGMG_Process_Events_Now STATIC__CONFIGMG_Process_Events_Now
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Process_Events_Now
#define CONFIGMG_Process_Events_Now STATIC__CONFIGMG_Process_Events_Now
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Create_Range_List
#define _CONFIGMG_Create_Range_List STATIC__CONFIGMG_Create_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Create_Range_List
#define CONFIGMG_Create_Range_List STATIC__CONFIGMG_Create_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Range
#define _CONFIGMG_Add_Range STATIC__CONFIGMG_Add_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Range
#define CONFIGMG_Add_Range STATIC__CONFIGMG_Add_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Delete_Range
#define _CONFIGMG_Delete_Range STATIC__CONFIGMG_Delete_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Delete_Range
#define CONFIGMG_Delete_Range STATIC__CONFIGMG_Delete_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Test_Range_Available
#define _CONFIGMG_Test_Range_Available STATIC__CONFIGMG_Test_Range_Available
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Test_Range_Available
#define CONFIGMG_Test_Range_Available STATIC__CONFIGMG_Test_Range_Available
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Dup_Range_List
#define _CONFIGMG_Dup_Range_List STATIC__CONFIGMG_Dup_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Dup_Range_List
#define CONFIGMG_Dup_Range_List STATIC__CONFIGMG_Dup_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Range_List
#define _CONFIGMG_Free_Range_List STATIC__CONFIGMG_Free_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Range_List
#define CONFIGMG_Free_Range_List STATIC__CONFIGMG_Free_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Invert_Range_List
#define _CONFIGMG_Invert_Range_List STATIC__CONFIGMG_Invert_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Invert_Range_List
#define CONFIGMG_Invert_Range_List STATIC__CONFIGMG_Invert_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Intersect_Range_List
#define _CONFIGMG_Intersect_Range_List STATIC__CONFIGMG_Intersect_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Intersect_Range_List
#define CONFIGMG_Intersect_Range_List STATIC__CONFIGMG_Intersect_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_First_Range
#define _CONFIGMG_First_Range STATIC__CONFIGMG_First_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_First_Range
#define CONFIGMG_First_Range STATIC__CONFIGMG_First_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Next_Range
#define _CONFIGMG_Next_Range STATIC__CONFIGMG_Next_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Next_Range
#define CONFIGMG_Next_Range STATIC__CONFIGMG_Next_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Dump_Range_List
#define _CONFIGMG_Dump_Range_List STATIC__CONFIGMG_Dump_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Dump_Range_List
#define CONFIGMG_Dump_Range_List STATIC__CONFIGMG_Dump_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Load_DLVxDs
#define _CONFIGMG_Load_DLVxDs STATIC__CONFIGMG_Load_DLVxDs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Load_DLVxDs
#define CONFIGMG_Load_DLVxDs STATIC__CONFIGMG_Load_DLVxDs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DDBs
#define _CONFIGMG_Get_DDBs STATIC__CONFIGMG_Get_DDBs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DDBs
#define CONFIGMG_Get_DDBs STATIC__CONFIGMG_Get_DDBs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_CRC_CheckSum
#define _CONFIGMG_Get_CRC_CheckSum STATIC__CONFIGMG_Get_CRC_CheckSum
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_CRC_CheckSum
#define CONFIGMG_Get_CRC_CheckSum STATIC__CONFIGMG_Get_CRC_CheckSum
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_DevLoader
#define _CONFIGMG_Register_DevLoader STATIC__CONFIGMG_Register_DevLoader
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_DevLoader
#define CONFIGMG_Register_DevLoader STATIC__CONFIGMG_Register_DevLoader
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Reenumerate_DevNode
#define _CONFIGMG_Reenumerate_DevNode STATIC__CONFIGMG_Reenumerate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Reenumerate_DevNode
#define CONFIGMG_Reenumerate_DevNode STATIC__CONFIGMG_Reenumerate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Setup_DevNode
#define _CONFIGMG_Setup_DevNode STATIC__CONFIGMG_Setup_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Setup_DevNode
#define CONFIGMG_Setup_DevNode STATIC__CONFIGMG_Setup_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Reset_Children_Marks
#define _CONFIGMG_Reset_Children_Marks STATIC__CONFIGMG_Reset_Children_Marks
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Reset_Children_Marks
#define CONFIGMG_Reset_Children_Marks STATIC__CONFIGMG_Reset_Children_Marks
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Status
#define _CONFIGMG_Get_DevNode_Status STATIC__CONFIGMG_Get_DevNode_Status
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Status
#define CONFIGMG_Get_DevNode_Status STATIC__CONFIGMG_Get_DevNode_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Remove_Unmarked_Children
#define _CONFIGMG_Remove_Unmarked_Children STATIC__CONFIGMG_Remove_Unmarked_Children
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Remove_Unmarked_Children
#define CONFIGMG_Remove_Unmarked_Children STATIC__CONFIGMG_Remove_Unmarked_Children
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_ISAPNP_To_CM
#define _CONFIGMG_ISAPNP_To_CM STATIC__CONFIGMG_ISAPNP_To_CM
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_ISAPNP_To_CM
#define CONFIGMG_ISAPNP_To_CM STATIC__CONFIGMG_ISAPNP_To_CM
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_CallBack_Device_Driver
#define _CONFIGMG_CallBack_Device_Driver STATIC__CONFIGMG_CallBack_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_CallBack_Device_Driver
#define CONFIGMG_CallBack_Device_Driver STATIC__CONFIGMG_CallBack_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_CallBack_Enumerator
#define _CONFIGMG_CallBack_Enumerator STATIC__CONFIGMG_CallBack_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_CallBack_Enumerator
#define CONFIGMG_CallBack_Enumerator STATIC__CONFIGMG_CallBack_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Alloc_Log_Conf
#define _CONFIGMG_Get_Alloc_Log_Conf STATIC__CONFIGMG_Get_Alloc_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Alloc_Log_Conf
#define CONFIGMG_Get_Alloc_Log_Conf STATIC__CONFIGMG_Get_Alloc_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Key_Size
#define _CONFIGMG_Get_DevNode_Key_Size STATIC__CONFIGMG_Get_DevNode_Key_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Key_Size
#define CONFIGMG_Get_DevNode_Key_Size STATIC__CONFIGMG_Get_DevNode_Key_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Key
#define _CONFIGMG_Get_DevNode_Key STATIC__CONFIGMG_Get_DevNode_Key
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Key
#define CONFIGMG_Get_DevNode_Key STATIC__CONFIGMG_Get_DevNode_Key
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Read_Registry_Value
#define _CONFIGMG_Read_Registry_Value STATIC__CONFIGMG_Read_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Read_Registry_Value
#define CONFIGMG_Read_Registry_Value STATIC__CONFIGMG_Read_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Write_Registry_Value
#define _CONFIGMG_Write_Registry_Value STATIC__CONFIGMG_Write_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Write_Registry_Value
#define CONFIGMG_Write_Registry_Value STATIC__CONFIGMG_Write_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Disable_DevNode
#define _CONFIGMG_Disable_DevNode STATIC__CONFIGMG_Disable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Disable_DevNode
#define CONFIGMG_Disable_DevNode STATIC__CONFIGMG_Disable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Enable_DevNode
#define _CONFIGMG_Enable_DevNode STATIC__CONFIGMG_Enable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Enable_DevNode
#define CONFIGMG_Enable_DevNode STATIC__CONFIGMG_Enable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Move_DevNode
#define _CONFIGMG_Move_DevNode STATIC__CONFIGMG_Move_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Move_DevNode
#define CONFIGMG_Move_DevNode STATIC__CONFIGMG_Move_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Bus_Info
#define _CONFIGMG_Set_Bus_Info STATIC__CONFIGMG_Set_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Bus_Info
#define CONFIGMG_Set_Bus_Info STATIC__CONFIGMG_Set_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Bus_Info
#define _CONFIGMG_Get_Bus_Info STATIC__CONFIGMG_Get_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Bus_Info
#define CONFIGMG_Get_Bus_Info STATIC__CONFIGMG_Get_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_HW_Prof
#define _CONFIGMG_Set_HW_Prof STATIC__CONFIGMG_Set_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_HW_Prof
#define CONFIGMG_Set_HW_Prof STATIC__CONFIGMG_Set_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Recompute_HW_Prof
#define _CONFIGMG_Recompute_HW_Prof STATIC__CONFIGMG_Recompute_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Recompute_HW_Prof
#define CONFIGMG_Recompute_HW_Prof STATIC__CONFIGMG_Recompute_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Change_HW_Prof
#define _CONFIGMG_Query_Change_HW_Prof STATIC__CONFIGMG_Query_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Change_HW_Prof
#define CONFIGMG_Query_Change_HW_Prof STATIC__CONFIGMG_Query_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_Driver_Private_DWord
#define _CONFIGMG_Get_Device_Driver_Private_DWord STATIC__CONFIGMG_Get_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_Driver_Private_DWord
#define CONFIGMG_Get_Device_Driver_Private_DWord STATIC__CONFIGMG_Get_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Device_Driver_Private_DWord
#define _CONFIGMG_Set_Device_Driver_Private_DWord STATIC__CONFIGMG_Set_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Device_Driver_Private_DWord
#define CONFIGMG_Set_Device_Driver_Private_DWord STATIC__CONFIGMG_Set_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_HW_Prof_Flags
#define _CONFIGMG_Get_HW_Prof_Flags STATIC__CONFIGMG_Get_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_HW_Prof_Flags
#define CONFIGMG_Get_HW_Prof_Flags STATIC__CONFIGMG_Get_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_HW_Prof_Flags
#define _CONFIGMG_Set_HW_Prof_Flags STATIC__CONFIGMG_Set_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_HW_Prof_Flags
#define CONFIGMG_Set_HW_Prof_Flags STATIC__CONFIGMG_Set_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Read_Registry_Log_Confs
#define _CONFIGMG_Read_Registry_Log_Confs STATIC__CONFIGMG_Read_Registry_Log_Confs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Read_Registry_Log_Confs
#define CONFIGMG_Read_Registry_Log_Confs STATIC__CONFIGMG_Read_Registry_Log_Confs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Run_Detection
#define _CONFIGMG_Run_Detection STATIC__CONFIGMG_Run_Detection
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Run_Detection
#define CONFIGMG_Run_Detection STATIC__CONFIGMG_Run_Detection
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Call_At_Appy_Time
#define _CONFIGMG_Call_At_Appy_Time STATIC__CONFIGMG_Call_At_Appy_Time
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Call_At_Appy_Time
#define CONFIGMG_Call_At_Appy_Time STATIC__CONFIGMG_Call_At_Appy_Time
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Private_Problem
#define _CONFIGMG_Set_Private_Problem STATIC__CONFIGMG_Set_Private_Problem
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Private_Problem
#define CONFIGMG_Set_Private_Problem STATIC__CONFIGMG_Set_Private_Problem
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Hardware_Profile_Info
#define _CONFIGMG_Get_Hardware_Profile_Info STATIC__CONFIGMG_Get_Hardware_Profile_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Hardware_Profile_Info
#define CONFIGMG_Get_Hardware_Profile_Info STATIC__CONFIGMG_Get_Hardware_Profile_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Arbitrator_Free_Size
#define _CONFIGMG_Query_Arbitrator_Free_Size STATIC__CONFIGMG_Query_Arbitrator_Free_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Arbitrator_Free_Size
#define CONFIGMG_Query_Arbitrator_Free_Size STATIC__CONFIGMG_Query_Arbitrator_Free_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Arbitrator_Free_Data
#define _CONFIGMG_Query_Arbitrator_Free_Data STATIC__CONFIGMG_Query_Arbitrator_Free_Data
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Arbitrator_Free_Data
#define CONFIGMG_Query_Arbitrator_Free_Data STATIC__CONFIGMG_Query_Arbitrator_Free_Data
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Sort_NodeList
#define _CONFIGMG_Sort_NodeList STATIC__CONFIGMG_Sort_NodeList
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Sort_NodeList
#define CONFIGMG_Sort_NodeList STATIC__CONFIGMG_Sort_NodeList
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Yield
#define _CONFIGMG_Yield STATIC__CONFIGMG_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Yield
#define CONFIGMG_Yield STATIC__CONFIGMG_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Lock
#define _CONFIGMG_Lock STATIC__CONFIGMG_Lock
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Lock
#define CONFIGMG_Lock STATIC__CONFIGMG_Lock
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Unlock
#define _CONFIGMG_Unlock STATIC__CONFIGMG_Unlock
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Unlock
#define CONFIGMG_Unlock STATIC__CONFIGMG_Unlock
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Empty_Log_Conf
#define _CONFIGMG_Add_Empty_Log_Conf STATIC__CONFIGMG_Add_Empty_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Empty_Log_Conf
#define CONFIGMG_Add_Empty_Log_Conf STATIC__CONFIGMG_Add_Empty_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Log_Conf
#define _CONFIGMG_Free_Log_Conf STATIC__CONFIGMG_Free_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Log_Conf
#define CONFIGMG_Free_Log_Conf STATIC__CONFIGMG_Free_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_First_Log_Conf
#define _CONFIGMG_Get_First_Log_Conf STATIC__CONFIGMG_Get_First_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_First_Log_Conf
#define CONFIGMG_Get_First_Log_Conf STATIC__CONFIGMG_Get_First_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Next_Log_Conf
#define _CONFIGMG_Get_Next_Log_Conf STATIC__CONFIGMG_Get_Next_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Next_Log_Conf
#define CONFIGMG_Get_Next_Log_Conf STATIC__CONFIGMG_Get_Next_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Fail_Change_HW_Prof
#define _CONFIGMG_Fail_Change_HW_Prof STATIC__CONFIGMG_Fail_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Fail_Change_HW_Prof
#define CONFIGMG_Fail_Change_HW_Prof STATIC__CONFIGMG_Fail_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Call_Enumerator_Function
#define _CONFIGMG_Call_Enumerator_Function STATIC__CONFIGMG_Call_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Call_Enumerator_Function
#define CONFIGMG_Call_Enumerator_Function STATIC__CONFIGMG_Call_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Post_Pointer_Message
#define VMD_Post_Pointer_Message STATIC_VMD_Post_Pointer_Message
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Set_Cursor_Proc
#define VMD_Set_Cursor_Proc STATIC_VMD_Set_Cursor_Proc
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Call_Cursor_Proc
#define VMD_Call_Cursor_Proc STATIC_VMD_Call_Cursor_Proc
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Set_Mouse_Data
#define VMD_Set_Mouse_Data STATIC_VMD_Set_Mouse_Data
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Get_Mouse_Data
#define VMD_Get_Mouse_Data STATIC_VMD_Get_Mouse_Data
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Post_Absolute_Pointer_Message
#define VMD_Post_Absolute_Pointer_Message STATIC_VMD_Post_Absolute_Pointer_Message
#endif

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_Version
#define IFSMgr_Get_Version STATIC_IFSMgr_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterMount
#define IFSMgr_RegisterMount STATIC_IFSMgr_RegisterMount
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterNet
#define IFSMgr_RegisterNet STATIC_IFSMgr_RegisterNet
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterMailSlot
#define IFSMgr_RegisterMailSlot STATIC_IFSMgr_RegisterMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_NetTime
#define IFSMgr_Get_NetTime STATIC_IFSMgr_Get_NetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_DOSTime
#define IFSMgr_Get_DOSTime STATIC_IFSMgr_Get_DOSTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetupConnection
#define IFSMgr_SetupConnection STATIC_IFSMgr_SetupConnection
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DerefConnection
#define IFSMgr_DerefConnection STATIC_IFSMgr_DerefConnection
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ServerDOSCall
#define IFSMgr_ServerDOSCall STATIC_IFSMgr_ServerDOSCall
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CompleteAsync
#define IFSMgr_CompleteAsync STATIC_IFSMgr_CompleteAsync
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterHeap
#define IFSMgr_RegisterHeap STATIC_IFSMgr_RegisterHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetHeap
#define IFSMgr_GetHeap STATIC_IFSMgr_GetHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RetHeap
#define IFSMgr_RetHeap STATIC_IFSMgr_RetHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckHeap
#define IFSMgr_CheckHeap STATIC_IFSMgr_CheckHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckHeapItem
#define IFSMgr_CheckHeapItem STATIC_IFSMgr_CheckHeapItem
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FillHeapSpare
#define IFSMgr_FillHeapSpare STATIC_IFSMgr_FillHeapSpare
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Block
#define IFSMgr_Block STATIC_IFSMgr_Block
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Wakeup
#define IFSMgr_Wakeup STATIC_IFSMgr_Wakeup
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Yield
#define IFSMgr_Yield STATIC_IFSMgr_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SchedEvent
#define IFSMgr_SchedEvent STATIC_IFSMgr_SchedEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_QueueEvent
#define IFSMgr_QueueEvent STATIC_IFSMgr_QueueEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_KillEvent
#define IFSMgr_KillEvent STATIC_IFSMgr_KillEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FreeIOReq
#define IFSMgr_FreeIOReq STATIC_IFSMgr_FreeIOReq
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MakeMailSlot
#define IFSMgr_MakeMailSlot STATIC_IFSMgr_MakeMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DeleteMailSlot
#define IFSMgr_DeleteMailSlot STATIC_IFSMgr_DeleteMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_WriteMailSlot
#define IFSMgr_WriteMailSlot STATIC_IFSMgr_WriteMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_PopUp
#define IFSMgr_PopUp STATIC_IFSMgr_PopUp
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_printf
#define IFSMgr_printf STATIC_IFSMgr_printf
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_AssertFailed
#define IFSMgr_AssertFailed STATIC_IFSMgr_AssertFailed
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_LogEntry
#define IFSMgr_LogEntry STATIC_IFSMgr_LogEntry
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DebugMenu
#define IFSMgr_DebugMenu STATIC_IFSMgr_DebugMenu
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DebugVars
#define IFSMgr_DebugVars STATIC_IFSMgr_DebugVars
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetDebugString
#define IFSMgr_GetDebugString STATIC_IFSMgr_GetDebugString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetDebugHexNum
#define IFSMgr_GetDebugHexNum STATIC_IFSMgr_GetDebugHexNum
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetFunction
#define IFSMgr_NetFunction STATIC_IFSMgr_NetFunction
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DoDelAllUses
#define IFSMgr_DoDelAllUses STATIC_IFSMgr_DoDelAllUses
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetErrString
#define IFSMgr_SetErrString STATIC_IFSMgr_SetErrString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetErrString
#define IFSMgr_GetErrString STATIC_IFSMgr_GetErrString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetReqHook
#define IFSMgr_SetReqHook STATIC_IFSMgr_SetReqHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetPathHook
#define IFSMgr_SetPathHook STATIC_IFSMgr_SetPathHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UseAdd
#define IFSMgr_UseAdd STATIC_IFSMgr_UseAdd
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UseDel
#define IFSMgr_UseDel STATIC_IFSMgr_UseDel
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_InitUseAdd
#define IFSMgr_InitUseAdd STATIC_IFSMgr_InitUseAdd
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ChangeDir
#define IFSMgr_ChangeDir STATIC_IFSMgr_ChangeDir
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DelAllUses
#define IFSMgr_DelAllUses STATIC_IFSMgr_DelAllUses
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CDROM_Attach
#define IFSMgr_CDROM_Attach STATIC_IFSMgr_CDROM_Attach
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CDROM_Detach
#define IFSMgr_CDROM_Detach STATIC_IFSMgr_CDROM_Detach
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32DupHandle
#define IFSMgr_Win32DupHandle STATIC_IFSMgr_Win32DupHandle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Ring0_FileIO
#define IFSMgr_Ring0_FileIO STATIC_IFSMgr_Ring0_FileIO
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32_Get_Ring0_Handle
#define IFSMgr_Win32_Get_Ring0_Handle STATIC_IFSMgr_Win32_Get_Ring0_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_Drive_Info
#define IFSMgr_Get_Drive_Info STATIC_IFSMgr_Get_Drive_Info
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Ring0GetDriveInfo
#define IFSMgr_Ring0GetDriveInfo STATIC_IFSMgr_Ring0GetDriveInfo
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_BlockNoEvents
#define IFSMgr_BlockNoEvents STATIC_IFSMgr_BlockNoEvents
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetToDosTime
#define IFSMgr_NetToDosTime STATIC_IFSMgr_NetToDosTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DosToNetTime
#define IFSMgr_DosToNetTime STATIC_IFSMgr_DosToNetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DosToWin32Time
#define IFSMgr_DosToWin32Time STATIC_IFSMgr_DosToWin32Time
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32ToDosTime
#define IFSMgr_Win32ToDosTime STATIC_IFSMgr_Win32ToDosTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetToWin32Time
#define IFSMgr_NetToWin32Time STATIC_IFSMgr_NetToWin32Time
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32ToNetTime
#define IFSMgr_Win32ToNetTime STATIC_IFSMgr_Win32ToNetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MetaMatch
#define IFSMgr_MetaMatch STATIC_IFSMgr_MetaMatch
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_TransMatch
#define IFSMgr_TransMatch STATIC_IFSMgr_TransMatch
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CallProvider
#define IFSMgr_CallProvider STATIC_IFSMgr_CallProvider
#endif

#ifdef WIN40_OR_LATER
#undef  UniToBCS
#define UniToBCS STATIC_UniToBCS
#endif

#ifdef WIN40_OR_LATER
#undef  UniToBCSPath
#define UniToBCSPath STATIC_UniToBCSPath
#endif

#ifdef WIN40_OR_LATER
#undef  BCSToUni
#define BCSToUni STATIC_BCSToUni
#endif

#ifdef WIN40_OR_LATER
#undef  UniToUpper
#define UniToUpper STATIC_UniToUpper
#endif

#ifdef WIN40_OR_LATER
#undef  UniCharToOEM
#define UniCharToOEM STATIC_UniCharToOEM
#endif

#ifdef WIN40_OR_LATER
#undef  CreateBasis
#define CreateBasis STATIC_CreateBasis
#endif

#ifdef WIN40_OR_LATER
#undef  MatchBasisName
#define MatchBasisName STATIC_MatchBasisName
#endif

#ifdef WIN40_OR_LATER
#undef  AppendBasisTail
#define AppendBasisTail STATIC_AppendBasisTail
#endif

#ifdef WIN40_OR_LATER
#undef  FcbToShort
#define FcbToShort STATIC_FcbToShort
#endif

#ifdef WIN40_OR_LATER
#undef  ShortToFcb
#define ShortToFcb STATIC_ShortToFcb
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ParsePath
#define IFSMgr_ParsePath STATIC_IFSMgr_ParsePath
#endif

#ifdef WIN40_OR_LATER
#undef  Query_PhysLock
#define Query_PhysLock STATIC_Query_PhysLock
#endif

#ifdef WIN40_OR_LATER
#undef  _VolFlush
#define _VolFlush STATIC__VolFlush
#endif

#ifdef WIN40_OR_LATER
#undef  VolFlush
#define VolFlush STATIC__VolFlush
#endif

#ifdef WIN40_OR_LATER
#undef  NotifyVolumeArrival
#define NotifyVolumeArrival STATIC_NotifyVolumeArrival
#endif

#ifdef WIN40_OR_LATER
#undef  NotifyVolumeRemoval
#define NotifyVolumeRemoval STATIC_NotifyVolumeRemoval
#endif

#ifdef WIN40_OR_LATER
#undef  QueryVolumeRemoval
#define QueryVolumeRemoval STATIC_QueryVolumeRemoval
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDUnmountCFSD
#define IFSMgr_FSDUnmountCFSD STATIC_IFSMgr_FSDUnmountCFSD
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetConversionTablePtrs
#define IFSMgr_GetConversionTablePtrs STATIC_IFSMgr_GetConversionTablePtrs
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckAccessConflict
#define IFSMgr_CheckAccessConflict STATIC_IFSMgr_CheckAccessConflict
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_LockFile
#define IFSMgr_LockFile STATIC_IFSMgr_LockFile
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnlockFile
#define IFSMgr_UnlockFile STATIC_IFSMgr_UnlockFile
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RemoveLocks
#define IFSMgr_RemoveLocks STATIC_IFSMgr_RemoveLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckLocks
#define IFSMgr_CheckLocks STATIC_IFSMgr_CheckLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CountLocks
#define IFSMgr_CountLocks STATIC_IFSMgr_CountLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ReassignLockFileInst
#define IFSMgr_ReassignLockFileInst STATIC_IFSMgr_ReassignLockFileInst
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnassignLockList
#define IFSMgr_UnassignLockList STATIC_IFSMgr_UnassignLockList
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MountChildVolume
#define IFSMgr_MountChildVolume STATIC_IFSMgr_MountChildVolume
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnmountChildVolume
#define IFSMgr_UnmountChildVolume STATIC_IFSMgr_UnmountChildVolume
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SwapDrives
#define IFSMgr_SwapDrives STATIC_IFSMgr_SwapDrives
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDMapFHtoIOREQ
#define IFSMgr_FSDMapFHtoIOREQ STATIC_IFSMgr_FSDMapFHtoIOREQ
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDParsePath
#define IFSMgr_FSDParsePath STATIC_IFSMgr_FSDParsePath
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDAttachSFT
#define IFSMgr_FSDAttachSFT STATIC_IFSMgr_FSDAttachSFT
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetTimeZoneBias
#define IFSMgr_GetTimeZoneBias STATIC_IFSMgr_GetTimeZoneBias
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_PNPEvent
#define IFSMgr_PNPEvent STATIC_IFSMgr_PNPEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterCFSD
#define IFSMgr_RegisterCFSD STATIC_IFSMgr_RegisterCFSD
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32MapExtendedHandleToSFT
#define IFSMgr_Win32MapExtendedHandleToSFT STATIC_IFSMgr_Win32MapExtendedHandleToSFT
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DbgSetFileHandleLimit
#define IFSMgr_DbgSetFileHandleLimit STATIC_IFSMgr_DbgSetFileHandleLimit
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32MapSFTToExtendedHandle
#define IFSMgr_Win32MapSFTToExtendedHandle STATIC_IFSMgr_Win32MapSFTToExtendedHandle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDGetCurrentDrive
#define IFSMgr_FSDGetCurrentDrive STATIC_IFSMgr_FSDGetCurrentDrive
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_InstallFileSystemApiHook
#define IFSMgr_InstallFileSystemApiHook STATIC_IFSMgr_InstallFileSystemApiHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RemoveFileSystemApiHook
#define IFSMgr_RemoveFileSystemApiHook STATIC_IFSMgr_RemoveFileSystemApiHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RunScheduledEvents
#define IFSMgr_RunScheduledEvents STATIC_IFSMgr_RunScheduledEvents
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckDelResource
#define IFSMgr_CheckDelResource STATIC_IFSMgr_CheckDelResource
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32GetVMCurdir
#define IFSMgr_Win32GetVMCurdir STATIC_IFSMgr_Win32GetVMCurdir
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetupFailedConnection
#define IFSMgr_SetupFailedConnection STATIC_IFSMgr_SetupFailedConnection
#endif

#ifdef WIN40_OR_LATER
#undef  _GetMappedErr
#define _GetMappedErr STATIC__GetMappedErr
#endif

#ifdef WIN40_OR_LATER
#undef  GetMappedErr
#define GetMappedErr STATIC__GetMappedErr
#endif

#ifdef WIN40_OR_LATER
#undef  ShortToLossyFcb
#define ShortToLossyFcb STATIC_ShortToLossyFcb
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetLockState
#define IFSMgr_GetLockState STATIC_IFSMgr_GetLockState
#endif

#ifdef WIN40_OR_LATER
#undef  BcsToBcs
#define BcsToBcs STATIC_BcsToBcs
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetLoopback
#define IFSMgr_SetLoopback STATIC_IFSMgr_SetLoopback
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ClearLoopback
#define IFSMgr_ClearLoopback STATIC_IFSMgr_ClearLoopback
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ParseOneElement
#define IFSMgr_ParseOneElement STATIC_IFSMgr_ParseOneElement
#endif

#ifdef WIN40_OR_LATER
#undef  BcsToBcsUpper
#define BcsToBcsUpper STATIC_BcsToBcsUpper
#endif

#ifdef WIN40_OR_LATER
#undef  R0_OpenCreateFile
#define R0_OpenCreateFile STATIC_R0_OpenCreateFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_ReadFile
#define R0_ReadFile STATIC_R0_ReadFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_WriteFile
#define R0_WriteFile STATIC_R0_WriteFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_CloseFile
#define R0_CloseFile STATIC_R0_CloseFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetFileSize
#define R0_GetFileSize STATIC_R0_GetFileSize
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindFirstFile
#define R0_FindFirstFile STATIC_R0_FindFirstFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindNextFile
#define R0_FindNextFile STATIC_R0_FindNextFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindCloseFile
#define R0_FindCloseFile STATIC_R0_FindCloseFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetFileAttributes
#define R0_GetFileAttributes STATIC_R0_GetFileAttributes
#endif

#ifdef WIN40_OR_LATER
#undef  R0_SetFileAttributes
#define R0_SetFileAttributes STATIC_R0_SetFileAttributes
#endif

#ifdef WIN40_OR_LATER
#undef  R0_RenameFile
#define R0_RenameFile STATIC_R0_RenameFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_DeleteFile
#define R0_DeleteFile STATIC_R0_DeleteFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_LockFile
#define R0_LockFile STATIC_R0_LockFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_UnlockFile
#define R0_UnlockFile STATIC_R0_UnlockFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetDiskFreeSpace
#define R0_GetDiskFreeSpace STATIC_R0_GetDiskFreeSpace
#endif

#ifdef WIN40_OR_LATER
#undef  R0_ReadAbsoluteDisk
#define R0_ReadAbsoluteDisk STATIC_R0_ReadAbsoluteDisk
#endif

#ifdef WIN40_OR_LATER
#undef  R0_WriteAbsoluteDisk
#define R0_WriteAbsoluteDisk STATIC_R0_WriteAbsoluteDisk
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Get_Version
#define VWIN32_Get_Version STATIC_VWIN32_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_DIOCCompletionRoutine
#define VWIN32_DIOCCompletionRoutine STATIC_VWIN32_DIOCCompletionRoutine
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_QueueUserApc
#define _VWIN32_QueueUserApc STATIC__VWIN32_QueueUserApc
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_QueueUserApc
#define VWIN32_QueueUserApc STATIC__VWIN32_QueueUserApc
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Get_Thread_Context
#define _VWIN32_Get_Thread_Context STATIC__VWIN32_Get_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Get_Thread_Context
#define VWIN32_Get_Thread_Context STATIC__VWIN32_Get_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Emulate_Npx
#define _VWIN32_Emulate_Npx STATIC__VWIN32_Emulate_Npx
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Emulate_Npx
#define VWIN32_Emulate_Npx STATIC__VWIN32_Emulate_Npx
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CheckDelayedNpxTrap
#define _VWIN32_CheckDelayedNpxTrap STATIC__VWIN32_CheckDelayedNpxTrap
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CheckDelayedNpxTrap
#define VWIN32_CheckDelayedNpxTrap STATIC__VWIN32_CheckDelayedNpxTrap
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_LeaveCrstR0
#define VWIN32_LeaveCrstR0 STATIC_VWIN32_LeaveCrstR0
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_FaultPopup
#define _VWIN32_FaultPopup STATIC__VWIN32_FaultPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_FaultPopup
#define VWIN32_FaultPopup STATIC__VWIN32_FaultPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetContextHandle
#define VWIN32_GetContextHandle STATIC_VWIN32_GetContextHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetCurrentProcessHandle
#define VWIN32_GetCurrentProcessHandle STATIC_VWIN32_GetCurrentProcessHandle
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_SetWin32Event
#define _VWIN32_SetWin32Event STATIC__VWIN32_SetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_SetWin32Event
#define VWIN32_SetWin32Event STATIC__VWIN32_SetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_PulseWin32Event
#define _VWIN32_PulseWin32Event STATIC__VWIN32_PulseWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_PulseWin32Event
#define VWIN32_PulseWin32Event STATIC__VWIN32_PulseWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_ResetWin32Event
#define _VWIN32_ResetWin32Event STATIC__VWIN32_ResetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_ResetWin32Event
#define VWIN32_ResetWin32Event STATIC__VWIN32_ResetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_WaitSingleObject
#define _VWIN32_WaitSingleObject STATIC__VWIN32_WaitSingleObject
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_WaitSingleObject
#define VWIN32_WaitSingleObject STATIC__VWIN32_WaitSingleObject
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_WaitMultipleObjects
#define _VWIN32_WaitMultipleObjects STATIC__VWIN32_WaitMultipleObjects
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_WaitMultipleObjects
#define VWIN32_WaitMultipleObjects STATIC__VWIN32_WaitMultipleObjects
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CreateRing0Thread
#define _VWIN32_CreateRing0Thread STATIC__VWIN32_CreateRing0Thread
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CreateRing0Thread
#define VWIN32_CreateRing0Thread STATIC__VWIN32_CreateRing0Thread
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CloseVxDHandle
#define _VWIN32_CloseVxDHandle STATIC__VWIN32_CloseVxDHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CloseVxDHandle
#define VWIN32_CloseVxDHandle STATIC__VWIN32_CloseVxDHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_ActiveTimeBiasSet
#define VWIN32_ActiveTimeBiasSet STATIC_VWIN32_ActiveTimeBiasSet
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_EnterCrstR0
#define VWIN32_EnterCrstR0 STATIC_VWIN32_EnterCrstR0
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_BlueScreenPopup
#define VWIN32_BlueScreenPopup STATIC_VWIN32_BlueScreenPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_TerminateApp
#define VWIN32_TerminateApp STATIC_VWIN32_TerminateApp
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_QueueKernelAPC
#define _VWIN32_QueueKernelAPC STATIC__VWIN32_QueueKernelAPC
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_QueueKernelAPC
#define VWIN32_QueueKernelAPC STATIC__VWIN32_QueueKernelAPC
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_SysErrorBox
#define VWIN32_SysErrorBox STATIC_VWIN32_SysErrorBox
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_IsClientWin32
#define _VWIN32_IsClientWin32 STATIC__VWIN32_IsClientWin32
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_IsClientWin32
#define VWIN32_IsClientWin32 STATIC__VWIN32_IsClientWin32
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Set_Thread_Context
#define _VWIN32_Set_Thread_Context STATIC__VWIN32_Set_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Set_Thread_Context
#define VWIN32_Set_Thread_Context STATIC__VWIN32_Set_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CopyMem
#define _VWIN32_CopyMem STATIC__VWIN32_CopyMem
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CopyMem
#define VWIN32_CopyMem STATIC__VWIN32_CopyMem
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Npx_Exception
#define _VWIN32_Npx_Exception STATIC__VWIN32_Npx_Exception
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Npx_Exception
#define VWIN32_Npx_Exception STATIC__VWIN32_Npx_Exception
#endif

#ifdef WIN40_OR_LATER
#undef  PCCARD_Get_Version
#define PCCARD_Get_Version STATIC_PCCARD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PCCARD_Card_Services
#define PCCARD_Card_Services STATIC_PCCARD_Card_Services
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Get_Version
#define IOS_Get_Version STATIC_IOS_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_BD_Register_Device
#define IOS_BD_Register_Device STATIC_IOS_BD_Register_Device
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Find_Int13_Drive
#define IOS_Find_Int13_Drive STATIC_IOS_Find_Int13_Drive
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Get_Device_List
#define IOS_Get_Device_List STATIC_IOS_Get_Device_List
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_SendCommand
#define IOS_SendCommand STATIC_IOS_SendCommand
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Register
#define IOS_Register STATIC_IOS_Register
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Requestor_Service
#define IOS_Requestor_Service STATIC_IOS_Requestor_Service
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Exclusive_Access
#define IOS_Exclusive_Access STATIC_IOS_Exclusive_Access
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Send_Next_Command
#define IOS_Send_Next_Command STATIC_IOS_Send_Next_Command
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Set_Async_Time_Out
#define IOS_Set_Async_Time_Out STATIC_IOS_Set_Async_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Signal_Semaphore_No_Switch
#define IOS_Signal_Semaphore_No_Switch STATIC_IOS_Signal_Semaphore_No_Switch
#endif

#ifdef WIN40_OR_LATER
#undef  IOSIdleStatus
#define IOSIdleStatus STATIC_IOSIdleStatus
#endif

#ifdef WIN40_OR_LATER
#undef  IOSMapIORSToI24
#define IOSMapIORSToI24 STATIC_IOSMapIORSToI24
#endif

#ifdef WIN40_OR_LATER
#undef  IOSMapIORSToI21
#define IOSMapIORSToI21 STATIC_IOSMapIORSToI21
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_BD_Command_Complete
#define IOS_BD_Command_Complete STATIC_IOS_BD_Command_Complete
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_Register_Device_Driver
#define MMDEVLDR_Register_Device_Driver STATIC_MMDEVLDR_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_SetDevicePresence
#define _MMDEVLDR_SetDevicePresence STATIC__MMDEVLDR_SetDevicePresence
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_SetDevicePresence
#define MMDEVLDR_SetDevicePresence STATIC__MMDEVLDR_SetDevicePresence
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_SetEnvironmentString
#define _MMDEVLDR_SetEnvironmentString STATIC__MMDEVLDR_SetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_SetEnvironmentString
#define MMDEVLDR_SetEnvironmentString STATIC__MMDEVLDR_SetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_GetEnvironmentString
#define _MMDEVLDR_GetEnvironmentString STATIC__MMDEVLDR_GetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_GetEnvironmentString
#define MMDEVLDR_GetEnvironmentString STATIC__MMDEVLDR_GetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_RemoveEnvironmentString
#define _MMDEVLDR_RemoveEnvironmentString STATIC__MMDEVLDR_RemoveEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_RemoveEnvironmentString
#define MMDEVLDR_RemoveEnvironmentString STATIC__MMDEVLDR_RemoveEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_AddEnvironmentString
#define _MMDEVLDR_AddEnvironmentString STATIC__MMDEVLDR_AddEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_AddEnvironmentString
#define MMDEVLDR_AddEnvironmentString STATIC__MMDEVLDR_AddEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Get_Virtualization_Count
#define VPICD_Get_Virtualization_Count STATIC_VPICD_Get_Virtualization_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Phys_Mask_Channel
#define VDMAD_Phys_Mask_Channel STATIC_VDMAD_Phys_Mask_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Phys_Unmask_Channel
#define VDMAD_Phys_Unmask_Channel STATIC_VDMAD_Phys_Unmask_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Unvirtualize_Channel
#define VDMAD_Unvirtualize_Channel STATIC_VDMAD_Unvirtualize_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_IO_Address
#define VDMAD_Set_IO_Address STATIC_VDMAD_Set_IO_Address
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Convert_Hex_Binary
#define Debug_Convert_Hex_Binary STATIC_Debug_Convert_Hex_Binary
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Convert_Hex_Decimal
#define Debug_Convert_Hex_Decimal STATIC_Debug_Convert_Hex_Decimal
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Flags_Service
#define _Debug_Flags_Service STATIC__Debug_Flags_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Flags_Service
#define Debug_Flags_Service STATIC__Debug_Flags_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Cur_VM
#define Debug_Test_Cur_VM STATIC_Debug_Test_Cur_VM
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Valid_Handle
#define Debug_Test_Valid_Handle STATIC_Debug_Test_Valid_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_FindBlock
#define VCache_FindBlock STATIC_VCache_FindBlock
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_FreeBlock
#define VCache_FreeBlock STATIC_VCache_FreeBlock
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_GetSize
#define VCache_GetSize STATIC_VCache_GetSize
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_GetStats
#define VCache_GetStats STATIC_VCache_GetStats
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Get_Version
#define VCache_Get_Version STATIC_VCache_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Hold
#define VCache_Hold STATIC_VCache_Hold
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_MakeMRU
#define VCache_MakeMRU STATIC_VCache_MakeMRU
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Register
#define VCache_Register STATIC_VCache_Register
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_SwapBuffers
#define VCache_SwapBuffers STATIC_VCache_SwapBuffers
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_TestHold
#define VCache_TestHold STATIC_VCache_TestHold
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Unhold
#define VCache_Unhold STATIC_VCache_Unhold
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_CloseLookupCache
#define _VCache_CloseLookupCache STATIC__VCache_CloseLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CloseLookupCache
#define VCache_CloseLookupCache STATIC__VCache_CloseLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_CreateLookupCache
#define _VCache_CreateLookupCache STATIC__VCache_CreateLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CreateLookupCache
#define VCache_CreateLookupCache STATIC__VCache_CreateLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_DeleteLookupCache
#define _VCache_DeleteLookupCache STATIC__VCache_DeleteLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_DeleteLookupCache
#define VCache_DeleteLookupCache STATIC__VCache_DeleteLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_Lookup
#define _VCache_Lookup STATIC__VCache_Lookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Lookup
#define VCache_Lookup STATIC__VCache_Lookup
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_UpdateLookup
#define _VCache_UpdateLookup STATIC__VCache_UpdateLookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_UpdateLookup
#define VCache_UpdateLookup STATIC__VCache_UpdateLookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_RelinquishPage
#define VCache_RelinquishPage STATIC_VCache_RelinquishPage
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_UseThisPage
#define VCache_UseThisPage STATIC_VCache_UseThisPage
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_RecalcSums
#define VCache_RecalcSums STATIC_VCache_RecalcSums
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_TestHandle
#define VCache_TestHandle STATIC_VCache_TestHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_VerifySums
#define VCache_VerifySums STATIC_VCache_VerifySums
#endif

#ifdef WIN40_OR_LATER
#undef  DOSMGR_Alloc_Local_Sys_VM_Mem
#define DOSMGR_Alloc_Local_Sys_VM_Mem STATIC_DOSMGR_Alloc_Local_Sys_VM_Mem
#endif

#ifdef WIN40_OR_LATER
#undef  DOSMGR_Init_UMB_Area
#define DOSMGR_Init_UMB_Area STATIC_DOSMGR_Init_UMB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  _GetRegistryKey
#define _GetRegistryKey STATIC__GetRegistryKey
#endif

#ifdef WIN40_OR_LATER
#undef  GetRegistryKey
#define GetRegistryKey STATIC__GetRegistryKey
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Nest_Exec_Status
#define Get_Nest_Exec_Status STATIC_Get_Nest_Exec_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _AddReclaimableItem
#define _AddReclaimableItem STATIC__AddReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  AddReclaimableItem
#define AddReclaimableItem STATIC__AddReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  _AtEventTime
#define _AtEventTime STATIC__AtEventTime
#endif

#ifdef WIN40_OR_LATER
#undef  AtEventTime
#define AtEventTime STATIC__AtEventTime
#endif

#ifdef WIN40_OR_LATER
#undef  Close_Boot_Log
#define Close_Boot_Log STATIC_Close_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  EnableDisable_Boot_Log
#define EnableDisable_Boot_Log STATIC_EnableDisable_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  _EnumReclaimableItem
#define _EnumReclaimableItem STATIC__EnumReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  EnumReclaimableItem
#define EnumReclaimableItem STATIC__EnumReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  Open_Boot_Log
#define Open_Boot_Log STATIC_Open_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  _SetReclaimableItem
#define _SetReclaimableItem STATIC__SetReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  SetReclaimableItem
#define SetReclaimableItem STATIC__SetReclaimableItem
#endif

#undef  Hook_Device_Service_Ex
#define Hook_Device_Service_Ex STATIC_Hook_Device_Service_Ex

#ifdef WIN40_OR_LATER
#undef  PERF_Get_Version
#define PERF_Get_Version STATIC_PERF_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Register
#define PERF_Server_Register STATIC_PERF_Server_Register
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Deregister
#define PERF_Server_Deregister STATIC_PERF_Server_Deregister
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Add_Stat
#define PERF_Server_Add_Stat STATIC_PERF_Server_Add_Stat
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Remove_Stat
#define PERF_Server_Remove_Stat STATIC_PERF_Server_Remove_Stat
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service_Ex
#define Unhook_Device_Service_Ex STATIC_Unhook_Device_Service_Ex
#endif

#undef  Call_Previous_Hook_Proc
#define Call_Previous_Hook_Proc STATIC_Call_Previous_Hook_Proc

#ifdef WIN40_OR_LATER
#undef  dummy_935
#define dummy_935 STATIC_dummy_935
#endif

#undef  Hook_Device_Service_C
#define Hook_Device_Service_C STATIC_Hook_Device_Service_C

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service_C
#define Unhook_Device_Service_C STATIC_Unhook_Device_Service_C
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_ID
#define _CONFIGMG_Add_ID STATIC__CONFIGMG_Add_ID
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_ID
#define CONFIGMG_Add_ID STATIC__CONFIGMG_Add_ID
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Enumerator_Function
#define _CONFIGMG_Register_Enumerator_Function STATIC__CONFIGMG_Register_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Enumerator_Function
#define CONFIGMG_Register_Enumerator_Function STATIC__CONFIGMG_Register_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Update_User_Activity
#define SHELL_Update_User_Activity STATIC_SHELL_Update_User_Activity
#endif

#undef  VDMAD_Unlock_DMA_Region_No_Dirty
#define VDMAD_Unlock_DMA_Region_No_Dirty STATIC_VDMAD_Unlock_DMA_Region_No_Dirty

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetCurrentDirectory
#define VWIN32_GetCurrentDirectory STATIC_VWIN32_GetCurrentDirectory
#endif

#ifdef WIN40_OR_LATER
#undef  VXDLDR_UnloadMe
#define VXDLDR_UnloadMe STATIC_VXDLDR_UnloadMe
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_InitCompleted
#define _PELDR_InitCompleted STATIC__PELDR_InitCompleted
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_InitCompleted
#define PELDR_InitCompleted STATIC__PELDR_InitCompleted
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_AddExportTable
#define _PELDR_AddExportTable STATIC__PELDR_AddExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_AddExportTable
#define PELDR_AddExportTable STATIC__PELDR_AddExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_FreeModule
#define _PELDR_FreeModule STATIC__PELDR_FreeModule
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_FreeModule
#define PELDR_FreeModule STATIC__PELDR_FreeModule
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetEntryPoint
#define _PELDR_GetEntryPoint STATIC__PELDR_GetEntryPoint
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetEntryPoint
#define PELDR_GetEntryPoint STATIC__PELDR_GetEntryPoint
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetModuleHandle
#define _PELDR_GetModuleHandle STATIC__PELDR_GetModuleHandle
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetModuleHandle
#define PELDR_GetModuleHandle STATIC__PELDR_GetModuleHandle
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetModuleUsage
#define _PELDR_GetModuleUsage STATIC__PELDR_GetModuleUsage
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetModuleUsage
#define PELDR_GetModuleUsage STATIC__PELDR_GetModuleUsage
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetProcAddress
#define _PELDR_GetProcAddress STATIC__PELDR_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetProcAddress
#define PELDR_GetProcAddress STATIC__PELDR_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_LoadModule
#define _PELDR_LoadModule STATIC__PELDR_LoadModule
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_LoadModule
#define PELDR_LoadModule STATIC__PELDR_LoadModule
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_LoadModuleEx
#define _PELDR_LoadModuleEx STATIC__PELDR_LoadModuleEx
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_LoadModuleEx
#define PELDR_LoadModuleEx STATIC__PELDR_LoadModuleEx
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_RemoveExportTable
#define _PELDR_RemoveExportTable STATIC__PELDR_RemoveExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_RemoveExportTable
#define PELDR_RemoveExportTable STATIC__PELDR_RemoveExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_AdjustMinimum
#define VCache_AdjustMinimum STATIC_VCache_AdjustMinimum
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CheckAvail
#define VCache_CheckAvail STATIC_VCache_CheckAvail
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Deregister
#define VCache_Deregister STATIC_VCache_Deregister
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Enum
#define VCache_Enum STATIC_VCache_Enum
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Enable_Keyboard
#define VKD_Enable_Keyboard STATIC_VKD_Enable_Keyboard
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Disable_Keyboard
#define VKD_Disable_Keyboard STATIC_VKD_Disable_Keyboard
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Get_Shift_State
#define VKD_Get_Shift_State STATIC_VKD_Get_Shift_State
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Put_Byte
#define VKD_Put_Byte STATIC_VKD_Put_Byte
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Set_Shift_State
#define VKD_Set_Shift_State STATIC_VKD_Set_Shift_State
#endif

#undef  VCD_InitThunk_Control
#define VCD_InitThunk_Control STATIC_VCD_InitThunk_Control

#undef  VCD_InitThunk_Irq
#define VCD_InitThunk_Irq STATIC_VCD_InitThunk_Irq

#undef  VCD_InitThunk_Io
#define VCD_InitThunk_Io STATIC_VCD_InitThunk_Io

#undef  VCD_Acquire_Port
#define VCD_Acquire_Port STATIC_VCD_Acquire_Port

#undef  VCD_Free_Port_Windows_Style
#define VCD_Free_Port_Windows_Style STATIC_VCD_Free_Port_Windows_Style

#undef  VCD_Steal_Port_Windows_Style
#define VCD_Steal_Port_Windows_Style STATIC_VCD_Steal_Port_Windows_Style

#ifdef WIN40_OR_LATER
#undef  VCD_Find_COM_Index
#define VCD_Find_COM_Index STATIC_VCD_Find_COM_Index
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Set_Port_Global_Special
#define VCD_Set_Port_Global_Special STATIC_VCD_Set_Port_Global_Special
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Virtualize_Port_Dynamic
#define VCD_Virtualize_Port_Dynamic STATIC_VCD_Virtualize_Port_Dynamic
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Unvirtualize_Port_Dynamic
#define VCD_Unvirtualize_Port_Dynamic STATIC_VCD_Unvirtualize_Port_Dynamic
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Acquire_Port_Windows_Style
#define VCD_Acquire_Port_Windows_Style STATIC_VCD_Acquire_Port_Windows_Style
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Free_Port
#define VCD_Free_Port STATIC_VCD_Free_Port
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Check_Update_Soon
#define VDD_Check_Update_Soon STATIC_VDD_Check_Update_Soon
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_GrabRtn
#define VDD_Get_GrabRtn STATIC_VDD_Get_GrabRtn
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Hide_Cursor
#define VDD_Hide_Cursor STATIC_VDD_Hide_Cursor
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Msg_SetCursPos
#define VDD_Msg_SetCursPos STATIC_VDD_Msg_SetCursPos
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Enable_Disable_Mouse_Events
#define VMD_Enable_Disable_Mouse_Events STATIC_VMD_Enable_Disable_Mouse_Events
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Init_File
#define PageFile_Init_File STATIC_PageFile_Init_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Get_Version
#define PageFile_Get_Version STATIC_PageFile_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Clean_Up
#define PageFile_Clean_Up STATIC_PageFile_Clean_Up
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Grow_File
#define PageFile_Grow_File STATIC_PageFile_Grow_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Read_Or_Write
#define PageFile_Read_Or_Write STATIC_PageFile_Read_Or_Write
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Cancel
#define PageFile_Cancel STATIC_PageFile_Cancel
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Test_IO_Valid
#define PageFile_Test_IO_Valid STATIC_PageFile_Test_IO_Valid
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Get_Size_Info
#define PageFile_Get_Size_Info STATIC_PageFile_Get_Size_Info
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Get_Version
#define PageSwap_Get_Version STATIC_PageSwap_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Test_IO_Valid
#define PageSwap_Test_IO_Valid STATIC_PageSwap_Test_IO_Valid
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Read_Or_Write
#define PageSwap_Read_Or_Write STATIC_PageSwap_Read_Or_Write
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Grow_File
#define PageSwap_Grow_File STATIC_PageSwap_Grow_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Init_File
#define PageSwap_Init_File STATIC_PageSwap_Init_File
#endif

#undef  Allocate_PM_App_CB_Area
#define Allocate_PM_App_CB_Area STATIC_Allocate_PM_App_CB_Area

#ifdef WIN40_OR_LATER
#undef  _Call_On_My_Stack
#define _Call_On_My_Stack STATIC__Call_On_My_Stack
#endif

#ifdef WIN40_OR_LATER
#undef  Call_On_My_Stack
#define Call_On_My_Stack STATIC__Call_On_My_Stack
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Printf_Service
#define _Debug_Printf_Service STATIC__Debug_Printf_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Printf_Service
#define Debug_Printf_Service STATIC__Debug_Printf_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _strupr
#define _strupr STATIC__strupr
#endif

#ifdef WIN40_OR_LATER
#undef  strupr
#define strupr STATIC__strupr
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Do_Physical_IO
#define VDD_Do_Physical_IO STATIC_VDD_Do_Physical_IO
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_DISPLAYINFO
#define VDD_Get_DISPLAYINFO STATIC_VDD_Get_DISPLAYINFO
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_Mini_Dispatch_Table
#define VDD_Get_Mini_Dispatch_Table STATIC_VDD_Get_Mini_Dispatch_Table
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_Special_VM_IDs
#define VDD_Get_Special_VM_IDs STATIC_VDD_Get_Special_VM_IDs
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_VM_Info
#define VDD_Get_VM_Info STATIC_VDD_Get_VM_Info
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Register_Extra_Screen_Selector
#define VDD_Register_Extra_Screen_Selector STATIC_VDD_Register_Extra_Screen_Selector
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Register_Virtual_Port
#define VDD_Register_Virtual_Port STATIC_VDD_Register_Virtual_Port
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Set_Sleep_Flag_Addr
#define VDD_Set_Sleep_Flag_Addr STATIC_VDD_Set_Sleep_Flag_Addr
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Takeover_VGA_Port
#define VDD_Takeover_VGA_Port STATIC_VDD_Takeover_VGA_Port
#endif

#undef  Init_Generic_Thunk
#define Init_Generic_Thunk STATIC_Init_Generic_Thunk

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Enumerate_DevNodes
#define _VCOMM_Enumerate_DevNodes STATIC__VCOMM_Enumerate_DevNodes
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Enumerate_DevNodes
#define VCOMM_Enumerate_DevNodes STATIC__VCOMM_Enumerate_DevNodes
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Virt_Count
#define VDMAD_Get_Virt_Count STATIC_VDMAD_Get_Virt_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_Channel_Callbacks
#define VDMAD_Set_Channel_Callbacks STATIC_VDMAD_Set_Channel_Callbacks
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Phys_Status
#define VDMAD_Get_Phys_Status STATIC_VDMAD_Get_Phys_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Phys_Count
#define VDMAD_Get_Phys_Count STATIC_VDMAD_Get_Phys_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Max_Phys_Page
#define VDMAD_Get_Max_Phys_Page STATIC_VDMAD_Get_Max_Phys_Page
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_Virt_Count
#define VDMAD_Set_Virt_Count STATIC_VDMAD_Set_Virt_Count
#endif

#ifdef WIN40_OR_LATER
#undef  _EnterMustComplete
#define _EnterMustComplete STATIC__EnterMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  EnterMustComplete
#define EnterMustComplete STATIC__EnterMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _LeaveMustComplete
#define _LeaveMustComplete STATIC__LeaveMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  LeaveMustComplete
#define LeaveMustComplete STATIC__LeaveMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _ResumeExecMustComplete
#define _ResumeExecMustComplete STATIC__ResumeExecMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ResumeExecMustComplete
#define ResumeExecMustComplete STATIC__ResumeExecMustComplete
#endif

#undef  VTDI_Get_Version
#define VTDI_Get_Version STATIC_VTDI_Get_Version

#undef  VTDI_Get_Info
#define VTDI_Get_Info STATIC_VTDI_Get_Info

#undef  VTDI_Start_Timer
#define VTDI_Start_Timer STATIC_VTDI_Start_Timer

#undef  VTDI_Stop_Timer
#define VTDI_Stop_Timer STATIC_VTDI_Stop_Timer

#undef  dummy_1034
#define dummy_1034 STATIC_dummy_1034

#undef  dummy_1035
#define dummy_1035 STATIC_dummy_1035

#undef  VTDI_Schedule_Event
#define VTDI_Schedule_Event STATIC_VTDI_Schedule_Event

#undef  VTDI_Cancel_Event
#define VTDI_Cancel_Event STATIC_VTDI_Cancel_Event

#undef  VTDI_Block
#define VTDI_Block STATIC_VTDI_Block

#undef  VTDI_Signal
#define VTDI_Signal STATIC_VTDI_Signal

#undef  VTDI_Register
#define VTDI_Register STATIC_VTDI_Register

#ifdef WIN40_OR_LATER
#undef  VTDI_Unload
#define VTDI_Unload STATIC_VTDI_Unload
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Initialize
#define VTDI_Initialize STATIC_VTDI_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Register_UnloadProc
#define VTDI_Register_UnloadProc STATIC_VTDI_Register_UnloadProc
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Register_LoadProc
#define VTDI_Register_LoadProc STATIC_VTDI_Register_LoadProc
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Thunk_HWInt_Ex
#define VPICD_Thunk_HWInt_Ex STATIC_VPICD_Thunk_HWInt_Ex
#endif

#ifdef WIN40_OR_LATER
#undef  Call_Restricted_Event_Ex
#define Call_Restricted_Event_Ex STATIC_Call_Restricted_Event_Ex
#endif

#ifdef WIN41_OR_LATER
#undef  NTKERN_Get_Version
#define NTKERN_Get_Version STATIC_NTKERN_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernCreateFile
#define NtKernCreateFile STATIC_NtKernCreateFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernClose
#define NtKernClose STATIC_NtKernClose
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernReadFile
#define NtKernReadFile STATIC_NtKernReadFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernWriteFile
#define NtKernWriteFile STATIC_NtKernWriteFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernDeviceIoControl
#define NtKernDeviceIoControl STATIC_NtKernDeviceIoControl
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernGetWorkerThread
#define NtKernGetWorkerThread STATIC_NtKernGetWorkerThread
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernLoadDriver
#define NtKernLoadDriver STATIC_NtKernLoadDriver
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernQueueWorkItem
#define NtKernQueueWorkItem STATIC_NtKernQueueWorkItem
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernPhysicalDeviceObjectToDevNode
#define NtKernPhysicalDeviceObjectToDevNode STATIC_NtKernPhysicalDeviceObjectToDevNode
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernSetPhysicalCacheTypeRange
#define NtKernSetPhysicalCacheTypeRange STATIC_NtKernSetPhysicalCacheTypeRange
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernWin9XLoadDriver
#define NtKernWin9XLoadDriver STATIC_NtKernWin9XLoadDriver
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernCancelIoFile
#define NtKernCancelIoFile STATIC_NtKernCancelIoFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernGetVPICDHandleFromInterruptObj
#define NtKernGetVPICDHandleFromInterruptObj STATIC_NtKernGetVPICDHandleFromInterruptObj
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernInternalDeviceIoControl
#define NtKernInternalDeviceIoControl STATIC_NtKernInternalDeviceIoControl
#endif

#ifdef WIN41_OR_LATER
#undef  _KeSaveFloatingPointState
#define _KeSaveFloatingPointState STATIC__KeSaveFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  KeSaveFloatingPointState
#define KeSaveFloatingPointState STATIC__KeSaveFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  _KeRestoreFloatingPointState
#define _KeRestoreFloatingPointState STATIC__KeRestoreFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  KeRestoreFloatingPointState
#define KeRestoreFloatingPointState STATIC__KeRestoreFloatingPointState
#endif

#ifdef WIN40_OR_LATER
#undef  VTD_Delay
#define VTD_Delay STATIC_VTD_Delay
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Get_Version
#define _PCI_Get_Version STATIC__PCI_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Get_Version
#define PCI_Get_Version STATIC__PCI_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Read_Config
#define _PCI_Read_Config STATIC__PCI_Read_Config
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Read_Config
#define PCI_Read_Config STATIC__PCI_Read_Config
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Write_Config
#define _PCI_Write_Config STATIC__PCI_Write_Config
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Write_Config
#define PCI_Write_Config STATIC__PCI_Write_Config
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Lock_Unlock
#define _PCI_Lock_Unlock STATIC__PCI_Lock_Unlock
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Lock_Unlock
#define PCI_Lock_Unlock STATIC__PCI_Lock_Unlock
#endif

#ifdef WIN41_OR_LATER
#undef  _PCCARD_Access_CIS_Memory
#define _PCCARD_Access_CIS_Memory STATIC__PCCARD_Access_CIS_Memory
#endif

#ifdef WIN41_OR_LATER
#undef  PCCARD_Access_CIS_Memory
#define PCCARD_Access_CIS_Memory STATIC__PCCARD_Access_CIS_Memory
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Get_DevNode_PowerCapabilities
#define _CONFIGMG_Get_DevNode_PowerCapabilities STATIC__CONFIGMG_Get_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Get_DevNode_PowerCapabilities
#define CONFIGMG_Get_DevNode_PowerCapabilities STATIC__CONFIGMG_Get_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Set_DevNode_PowerCapabilities
#define _CONFIGMG_Set_DevNode_PowerCapabilities STATIC__CONFIGMG_Set_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Set_DevNode_PowerCapabilities
#define CONFIGMG_Set_DevNode_PowerCapabilities STATIC__CONFIGMG_Set_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Get_DevNode_PowerState
#define _CONFIGMG_Get_DevNode_PowerState STATIC__CONFIGMG_Get_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Get_DevNode_PowerState
#define CONFIGMG_Get_DevNode_PowerState STATIC__CONFIGMG_Get_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Set_DevNode_PowerState
#define _CONFIGMG_Set_DevNode_PowerState STATIC__CONFIGMG_Set_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Set_DevNode_PowerState
#define CONFIGMG_Set_DevNode_PowerState STATIC__CONFIGMG_Set_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Register_DevNode_For_Idle_Detection
#define _CONFIGMG_Register_DevNode_For_Idle_Detection STATIC__CONFIGMG_Register_DevNode_For_Idle_Detection
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Register_DevNode_For_Idle_Detection
#define CONFIGMG_Register_DevNode_For_Idle_Detection STATIC__CONFIGMG_Register_DevNode_For_Idle_Detection
#endif

