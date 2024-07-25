//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  _XchgFreePhysReg
#define _XchgFreePhysReg INIT__XchgFreePhysReg

#undef  XchgFreePhysReg
#define XchgFreePhysReg INIT__XchgFreePhysReg

#undef  Wake_Up_VM
#define Wake_Up_VM INIT_Wake_Up_VM

#undef  Wait_Semaphore
#define Wait_Semaphore INIT_Wait_Semaphore

#undef  Validate_VM_Handle
#define Validate_VM_Handle INIT_Validate_VM_Handle

#undef  Validate_Client_Ptr
#define Validate_Client_Ptr INIT_Validate_Client_Ptr

#undef  Update_System_Clock
#define Update_System_Clock INIT_Update_System_Clock

#undef  _UnmapFreePhysReg
#define _UnmapFreePhysReg INIT__UnmapFreePhysReg

#undef  UnmapFreePhysReg
#define UnmapFreePhysReg INIT__UnmapFreePhysReg

#undef  Unhook_Invalid_Page_Fault
#define Unhook_Invalid_Page_Fault INIT_Unhook_Invalid_Page_Fault

#undef  Test_Sys_VM_Handle
#define Test_Sys_VM_Handle INIT_Test_Sys_VM_Handle

#undef  _TestGlobalV86Mem
#define _TestGlobalV86Mem INIT__TestGlobalV86Mem

#undef  TestGlobalV86Mem
#define TestGlobalV86Mem INIT__TestGlobalV86Mem

#undef  Test_Debug_Installed
#define Test_Debug_Installed INIT_Test_Debug_Installed

#undef  Test_DBCS_Lead_Byte
#define Test_DBCS_Lead_Byte INIT_Test_DBCS_Lead_Byte

#undef  Test_Cur_VM_Handle
#define Test_Cur_VM_Handle INIT_Test_Cur_VM_Handle

#undef  System_Control
#define System_Control INIT_System_Control

#undef  Suspend_VM
#define Suspend_VM INIT_Suspend_VM

#undef  Simulate_Push
#define Simulate_Push INIT_Simulate_Push

#undef  Simulate_Pop
#define Simulate_Pop INIT_Simulate_Pop

#undef  Simulate_Int
#define Simulate_Int INIT_Simulate_Int

#undef  Simulate_Far_Ret_N
#define Simulate_Far_Ret_N INIT_Simulate_Far_Ret_N

#undef  Simulate_Far_Jmp
#define Simulate_Far_Jmp INIT_Simulate_Far_Jmp

#undef  Simulate_Far_Call
#define Simulate_Far_Call INIT_Simulate_Far_Call

#undef  Signal_Semaphore
#define Signal_Semaphore INIT_Signal_Semaphore

#undef  Set_VM_Time_Out
#define Set_VM_Time_Out INIT_Set_VM_Time_Out

#undef  Set_V86_Int_Vector
#define Set_V86_Int_Vector INIT_Set_V86_Int_Vector

#undef  Set_PM_Int_Vector
#define Set_PM_Int_Vector INIT_Set_PM_Int_Vector

#undef  Set_Time_Slice_Priority
#define Set_Time_Slice_Priority INIT_Set_Time_Slice_Priority

#undef  Set_Time_Slice_Granularity
#define Set_Time_Slice_Granularity INIT_Set_Time_Slice_Granularity

#undef  Set_System_Exit_Code
#define Set_System_Exit_Code INIT_Set_System_Exit_Code

#undef  _SetResetV86Pageable
#define _SetResetV86Pageable INIT__SetResetV86Pageable

#undef  SetResetV86Pageable
#define SetResetV86Pageable INIT__SetResetV86Pageable

#undef  Set_PM_Int_Type
#define Set_PM_Int_Type INIT_Set_PM_Int_Type

#undef  Set_Physical_HMA_Alias
#define Set_Physical_HMA_Alias INIT_Set_Physical_HMA_Alias

#undef  Set_NMI_Handler_Addr
#define Set_NMI_Handler_Addr INIT_Set_NMI_Handler_Addr

#undef  Set_Mono_Cur_Pos
#define Set_Mono_Cur_Pos INIT_Set_Mono_Cur_Pos

#undef  _SetLastV86Page
#define _SetLastV86Page INIT__SetLastV86Page

#undef  SetLastV86Page
#define SetLastV86Page INIT__SetLastV86Page

#undef  Set_Global_Time_Out
#define Set_Global_Time_Out INIT_Set_Global_Time_Out

#undef  _SetFreePhysRegCalBk
#define _SetFreePhysRegCalBk INIT__SetFreePhysRegCalBk

#undef  SetFreePhysRegCalBk
#define SetFreePhysRegCalBk INIT__SetFreePhysRegCalBk

#undef  Set_Execution_Focus
#define Set_Execution_Focus INIT_Set_Execution_Focus

#undef  _SetDescriptor
#define _SetDescriptor INIT__SetDescriptor

#undef  SetDescriptor
#define SetDescriptor INIT__SetDescriptor

#undef  Set_Delete_On_Exit_File
#define Set_Delete_On_Exit_File INIT_Set_Delete_On_Exit_File

#undef  _SelectorMapFlat
#define _SelectorMapFlat INIT__SelectorMapFlat

#undef  SelectorMapFlat
#define SelectorMapFlat INIT__SelectorMapFlat

#undef  Schedule_VM_Event
#define Schedule_VM_Event INIT_Schedule_VM_Event

#undef  Schedule_Global_Event
#define Schedule_Global_Event INIT_Schedule_Global_Event

#undef  Save_Client_State
#define Save_Client_State INIT_Save_Client_State

#undef  Resume_VM
#define Resume_VM INIT_Resume_VM

#undef  Restore_Client_State
#define Restore_Client_State INIT_Restore_Client_State

#undef  Remove_V86_Break_Point
#define Remove_V86_Break_Point INIT_Remove_V86_Break_Point

#undef  Remove_Exception_Handler
#define Remove_Exception_Handler INIT_Remove_Exception_Handler

#undef  Release_Critical_Section
#define Release_Critical_Section INIT_Release_Critical_Section

#undef  Queue_Debug_String
#define Queue_Debug_String INIT_Queue_Debug_String

#undef  _PhysIntoV86
#define _PhysIntoV86 INIT__PhysIntoV86

#undef  PhysIntoV86
#define PhysIntoV86 INIT__PhysIntoV86

#undef  _PageUnLock
#define _PageUnLock INIT__PageUnLock

#undef  PageUnLock
#define PageUnLock INIT__PageUnLock

#undef  _PageResetHandlePAddr
#define _PageResetHandlePAddr INIT__PageResetHandlePAddr

#undef  PageResetHandlePAddr
#define PageResetHandlePAddr INIT__PageResetHandlePAddr

#undef  _PageReAllocate
#define _PageReAllocate INIT__PageReAllocate

#undef  PageReAllocate
#define PageReAllocate INIT__PageReAllocate

#undef  _PageOutDirtyPages
#define _PageOutDirtyPages INIT__PageOutDirtyPages

#undef  PageOutDirtyPages
#define PageOutDirtyPages INIT__PageOutDirtyPages

#undef  _PageLock
#define _PageLock INIT__PageLock

#undef  PageLock
#define PageLock INIT__PageLock

#undef  _PageGetSizeAddr
#define _PageGetSizeAddr INIT__PageGetSizeAddr

#undef  PageGetSizeAddr
#define PageGetSizeAddr INIT__PageGetSizeAddr

#undef  _PageGetAllocInfo
#define _PageGetAllocInfo INIT__PageGetAllocInfo

#undef  PageGetAllocInfo
#define PageGetAllocInfo INIT__PageGetAllocInfo

#undef  _PageFree
#define _PageFree INIT__PageFree

#undef  PageFree
#define PageFree INIT__PageFree

#undef  _PageDiscardPages
#define _PageDiscardPages INIT__PageDiscardPages

#undef  PageDiscardPages
#define PageDiscardPages INIT__PageDiscardPages

#undef  _PageCheckLinRange
#define _PageCheckLinRange INIT__PageCheckLinRange

#undef  PageCheckLinRange
#define PageCheckLinRange INIT__PageCheckLinRange

#undef  _PageAllocate
#define _PageAllocate INIT__PageAllocate

#undef  PageAllocate
#define PageAllocate INIT__PageAllocate

#undef  Out_Mono_String
#define Out_Mono_String INIT_Out_Mono_String

#undef  Out_Mono_Chr_Attrs
#define Out_Mono_Chr_Attrs INIT_Out_Mono_Chr_Attrs

#undef  Out_Mono_Chr
#define Out_Mono_Chr INIT_Out_Mono_Chr

#undef  Out_Debug_String
#define Out_Debug_String INIT_Out_Debug_String

#undef  Out_Debug_Chr
#define Out_Debug_Chr INIT_Out_Debug_Chr

#undef  OpenFile
#define OpenFile INIT_OpenFile

#undef  Nuke_VM
#define Nuke_VM INIT_Nuke_VM

#undef  No_Fail_Resume_VM
#define No_Fail_Resume_VM INIT_No_Fail_Resume_VM

#undef  _ModifyPageBits
#define _ModifyPageBits INIT__ModifyPageBits

#undef  ModifyPageBits
#define ModifyPageBits INIT__ModifyPageBits

#undef  _MMGR_Toggle_HMA
#define _MMGR_Toggle_HMA INIT__MMGR_Toggle_HMA

#undef  MMGR_Toggle_HMA
#define MMGR_Toggle_HMA INIT__MMGR_Toggle_HMA

#undef  MMGR_SetNULPageAddr
#define MMGR_SetNULPageAddr INIT_MMGR_SetNULPageAddr

#undef  _MapPhysToLinear
#define _MapPhysToLinear INIT__MapPhysToLinear

#undef  MapPhysToLinear
#define MapPhysToLinear INIT__MapPhysToLinear

#undef  Map_Lin_To_VM_Addr
#define Map_Lin_To_VM_Addr INIT_Map_Lin_To_VM_Addr

#undef  _MapIntoV86
#define _MapIntoV86 INIT__MapIntoV86

#undef  MapIntoV86
#define MapIntoV86 INIT__MapIntoV86

#undef  _MapFreePhysReg
#define _MapFreePhysReg INIT__MapFreePhysReg

#undef  MapFreePhysReg
#define MapFreePhysReg INIT__MapFreePhysReg

#undef  Map_Flat
#define Map_Flat INIT_Map_Flat

#undef  Locate_Byte_In_ROM
#define Locate_Byte_In_ROM INIT_Locate_Byte_In_ROM

#undef  List_Remove_First
#define List_Remove_First INIT_List_Remove_First

#undef  List_Remove
#define List_Remove INIT_List_Remove

#undef  List_Insert
#define List_Insert INIT_List_Insert

#undef  List_Get_Next
#define List_Get_Next INIT_List_Get_Next

#undef  List_Get_First
#define List_Get_First INIT_List_Get_First

#undef  List_Destroy
#define List_Destroy INIT_List_Destroy

#undef  List_Deallocate
#define List_Deallocate INIT_List_Deallocate

#undef  List_Create
#define List_Create INIT_List_Create

#undef  List_Attach_Tail
#define List_Attach_Tail INIT_List_Attach_Tail

#undef  List_Attach
#define List_Attach INIT_List_Attach

#undef  List_Allocate
#define List_Allocate INIT_List_Allocate

#undef  _LinPageUnLock
#define _LinPageUnLock INIT__LinPageUnLock

#undef  LinPageUnLock
#define LinPageUnLock INIT__LinPageUnLock

#undef  _LinPageLock
#define _LinPageLock INIT__LinPageLock

#undef  LinPageLock
#define LinPageLock INIT__LinPageLock

#undef  _LinMapIntoV86
#define _LinMapIntoV86 INIT__LinMapIntoV86

#undef  LinMapIntoV86
#define LinMapIntoV86 INIT__LinMapIntoV86

#undef  Is_Debug_Chr
#define Is_Debug_Chr INIT_Is_Debug_Chr

#undef  Install_V86_Break_Point
#define Install_V86_Break_Point INIT_Install_V86_Break_Point

#undef  Install_IO_Handler
#define Install_IO_Handler INIT_Install_IO_Handler

#undef  In_Debug_Chr
#define In_Debug_Chr INIT_In_Debug_Chr

#undef  Hook_VMM_Fault
#define Hook_VMM_Fault INIT_Hook_VMM_Fault

#undef  Hook_V86_Page
#define Hook_V86_Page INIT_Hook_V86_Page

#undef  Hook_V86_Int_Chain
#define Hook_V86_Int_Chain INIT_Hook_V86_Int_Chain

#undef  Hook_V86_Fault
#define Hook_V86_Fault INIT_Hook_V86_Fault

#undef  Hook_PM_Fault
#define Hook_PM_Fault INIT_Hook_PM_Fault

#undef  Hook_Device_PM_API
#define Hook_Device_PM_API INIT_Hook_Device_PM_API

#undef  Hook_NMI_Event
#define Hook_NMI_Event INIT_Hook_NMI_Event

#undef  Hook_Invalid_Page_Fault
#define Hook_Invalid_Page_Fault INIT_Hook_Invalid_Page_Fault

#undef  Hook_Device_V86_API
#define Hook_Device_V86_API INIT_Hook_Device_V86_API

#undef  Hook_Device_Service
#define Hook_Device_Service INIT_Hook_Device_Service

#undef  _HeapReAllocate
#define _HeapReAllocate INIT__HeapReAllocate

#undef  HeapReAllocate
#define HeapReAllocate INIT__HeapReAllocate

#undef  _HeapGetSize
#define _HeapGetSize INIT__HeapGetSize

#undef  HeapGetSize
#define HeapGetSize INIT__HeapGetSize

#undef  _HeapFree
#define _HeapFree INIT__HeapFree

#undef  HeapFree
#define HeapFree INIT__HeapFree

#undef  _HeapAllocate
#define _HeapAllocate INIT__HeapAllocate

#undef  HeapAllocate
#define HeapAllocate INIT__HeapAllocate

#undef  _GetVMPgCount
#define _GetVMPgCount INIT__GetVMPgCount

#undef  GetVMPgCount
#define GetVMPgCount INIT__GetVMPgCount

#undef  Get_VMM_Version
#define Get_VMM_Version INIT_Get_VMM_Version

#undef  Get_VMM_Reenter_Count
#define Get_VMM_Reenter_Count INIT_Get_VMM_Reenter_Count

#undef  Get_VM_Exec_Time
#define Get_VM_Exec_Time INIT_Get_VM_Exec_Time

#undef  _GetV86PageableArray
#define _GetV86PageableArray INIT__GetV86PageableArray

#undef  GetV86PageableArray
#define GetV86PageableArray INIT__GetV86PageableArray

#undef  Get_V86_Int_Vector
#define Get_V86_Int_Vector INIT_Get_V86_Int_Vector

#undef  Get_Time_Slice_Priority
#define Get_Time_Slice_Priority INIT_Get_Time_Slice_Priority

#undef  Get_Time_Slice_Info
#define Get_Time_Slice_Info INIT_Get_Time_Slice_Info

#undef  Get_Time_Slice_Granularity
#define Get_Time_Slice_Granularity INIT_Get_Time_Slice_Granularity

#undef  Get_Sys_VM_Handle
#define Get_Sys_VM_Handle INIT_Get_Sys_VM_Handle

#undef  Get_System_Time
#define Get_System_Time INIT_Get_System_Time

#undef  _GetSetPageOutCount
#define _GetSetPageOutCount INIT__GetSetPageOutCount

#undef  GetSetPageOutCount
#define GetSetPageOutCount INIT__GetSetPageOutCount

#undef  _GetSysPageCount
#define _GetSysPageCount INIT__GetSysPageCount

#undef  GetSysPageCount
#define GetSysPageCount INIT__GetSysPageCount

#undef  Set_HMA_Info
#define Set_HMA_Info INIT_Set_HMA_Info

#undef  Get_HMA_Info
#define Get_HMA_Info INIT_Get_HMA_Info

#undef  GetDetailedVMError
#define GetDetailedVMError INIT_GetDetailedVMError

#undef  SetDetailedVMError
#define SetDetailedVMError INIT_SetDetailedVMError

#undef  Get_PSP_Segment
#define Get_PSP_Segment INIT_Get_PSP_Segment

#undef  Get_Profile_String
#define Get_Profile_String INIT_Get_Profile_String

#undef  Get_Profile_Hex_Int
#define Get_Profile_Hex_Int INIT_Get_Profile_Hex_Int

#undef  Get_Profile_Fixed_Point
#define Get_Profile_Fixed_Point INIT_Get_Profile_Fixed_Point

#undef  Get_Profile_Decimal_Int
#define Get_Profile_Decimal_Int INIT_Get_Profile_Decimal_Int

#undef  Get_Profile_Boolean
#define Get_Profile_Boolean INIT_Get_Profile_Boolean

#undef  Get_PM_Int_Vector
#define Get_PM_Int_Vector INIT_Get_PM_Int_Vector

#undef  Get_PM_Int_Type
#define Get_PM_Int_Type INIT_Get_PM_Int_Type

#undef  _GetNulPageHandle
#define _GetNulPageHandle INIT__GetNulPageHandle

#undef  GetNulPageHandle
#define GetNulPageHandle INIT__GetNulPageHandle

#undef  Get_NMI_Handler_Addr
#define Get_NMI_Handler_Addr INIT_Get_NMI_Handler_Addr

#undef  Get_Next_VM_Handle
#define Get_Next_VM_Handle INIT_Get_Next_VM_Handle

#undef  Get_Next_Profile_String
#define Get_Next_Profile_String INIT_Get_Next_Profile_String

#undef  Get_Name_Of_Ugly_TSR
#define Get_Name_Of_Ugly_TSR INIT_Get_Name_Of_Ugly_TSR

#undef  Get_Mono_Cur_Pos
#define Get_Mono_Cur_Pos INIT_Get_Mono_Cur_Pos

#undef  Get_Mono_Chr
#define Get_Mono_Chr INIT_Get_Mono_Chr

#undef  Get_Machine_Info
#define Get_Machine_Info INIT_Get_Machine_Info

#undef  Get_Last_Updated_VM_Exec_Time
#define Get_Last_Updated_VM_Exec_Time INIT_Get_Last_Updated_VM_Exec_Time

#undef  _GetLastV86Page
#define _GetLastV86Page INIT__GetLastV86Page

#undef  GetLastV86Page
#define GetLastV86Page INIT__GetLastV86Page

#undef  Get_Last_Updated_System_Time
#define Get_Last_Updated_System_Time INIT_Get_Last_Updated_System_Time

#undef  _GetGlblRng0V86IntBase
#define _GetGlblRng0V86IntBase INIT__GetGlblRng0V86IntBase

#undef  GetGlblRng0V86IntBase
#define GetGlblRng0V86IntBase INIT__GetGlblRng0V86IntBase

#undef  _GetFreePageCount
#define _GetFreePageCount INIT__GetFreePageCount

#undef  GetFreePageCount
#define GetFreePageCount INIT__GetFreePageCount

#undef  Get_Fault_Hook_Addrs
#define Get_Fault_Hook_Addrs INIT_Get_Fault_Hook_Addrs

#undef  Get_Exec_Path
#define Get_Exec_Path INIT_Get_Exec_Path

#undef  Get_Execution_Focus
#define Get_Execution_Focus INIT_Get_Execution_Focus

#undef  Get_Environment_String
#define Get_Environment_String INIT_Get_Environment_String

#undef  _Get_Device_V86_Pages_Array
#define _Get_Device_V86_Pages_Array INIT__Get_Device_V86_Pages_Array

#undef  Get_Device_V86_Pages_Array
#define Get_Device_V86_Pages_Array INIT__Get_Device_V86_Pages_Array

#undef  _GetDescriptor
#define _GetDescriptor INIT__GetDescriptor

#undef  GetDescriptor
#define GetDescriptor INIT__GetDescriptor

#undef  _GetDemandPageInfo
#define _GetDemandPageInfo INIT__GetDemandPageInfo

#undef  GetDemandPageInfo
#define GetDemandPageInfo INIT__GetDemandPageInfo

#undef  Get_Debug_Options
#define Get_Debug_Options INIT_Get_Debug_Options

#undef  Get_Cur_VM_Handle
#define Get_Cur_VM_Handle INIT_Get_Cur_VM_Handle

#undef  Get_Crit_Status_No_Block
#define Get_Crit_Status_No_Block INIT_Get_Crit_Status_No_Block

#undef  Get_Crit_Section_Status
#define Get_Crit_Section_Status INIT_Get_Crit_Section_Status

#undef  Get_Cur_PM_App_CB
#define Get_Cur_PM_App_CB INIT_Get_Cur_PM_App_CB

#undef  Get_Config_Directory
#define Get_Config_Directory INIT_Get_Config_Directory

#undef  _GetFirstV86Page
#define _GetFirstV86Page INIT__GetFirstV86Page

#undef  GetFirstV86Page
#define GetFirstV86Page INIT__GetFirstV86Page

#undef  _GetAppFlatDSAlias
#define _GetAppFlatDSAlias INIT__GetAppFlatDSAlias

#undef  GetAppFlatDSAlias
#define GetAppFlatDSAlias INIT__GetAppFlatDSAlias

#undef  _Free_Temp_V86_Data_Area
#define _Free_Temp_V86_Data_Area INIT__Free_Temp_V86_Data_Area

#undef  Free_Temp_V86_Data_Area
#define Free_Temp_V86_Data_Area INIT__Free_Temp_V86_Data_Area

#undef  _Free_LDT_Selector
#define _Free_LDT_Selector INIT__Free_LDT_Selector

#undef  Free_LDT_Selector
#define Free_LDT_Selector INIT__Free_LDT_Selector

#undef  _Free_GDT_Selector
#define _Free_GDT_Selector INIT__Free_GDT_Selector

#undef  Free_GDT_Selector
#define Free_GDT_Selector INIT__Free_GDT_Selector

#undef  Fatal_Error_Handler
#define Fatal_Error_Handler INIT_Fatal_Error_Handler

#undef  Exec_VxD_Int
#define Exec_VxD_Int INIT_Exec_VxD_Int

#undef  Exec_Int
#define Exec_Int INIT_Exec_Int

#undef  End_Reentrant_Execution
#define End_Reentrant_Execution INIT_End_Reentrant_Execution

#undef  End_Crit_And_Suspend
#define End_Crit_And_Suspend INIT_End_Crit_And_Suspend

#undef  Enable_Local_Trapping
#define Enable_Local_Trapping INIT_Enable_Local_Trapping

#undef  Enable_Global_Trapping
#define Enable_Global_Trapping INIT_Enable_Global_Trapping

#undef  Disable_Local_Trapping
#define Disable_Local_Trapping INIT_Disable_Local_Trapping

#undef  Disable_Global_Trapping
#define Disable_Global_Trapping INIT_Disable_Global_Trapping

#undef  Destroy_Semaphore
#define Destroy_Semaphore INIT_Destroy_Semaphore

#undef  _DeAssign_Device_V86_Pages
#define _DeAssign_Device_V86_Pages INIT__DeAssign_Device_V86_Pages

#undef  DeAssign_Device_V86_Pages
#define DeAssign_Device_V86_Pages INIT__DeAssign_Device_V86_Pages

#undef  Close_VM
#define Close_VM INIT_Close_VM

#undef  Convert_Boolean_String
#define Convert_Boolean_String INIT_Convert_Boolean_String

#undef  Convert_Decimal_String
#define Convert_Decimal_String INIT_Convert_Decimal_String

#undef  Convert_Fixed_Point_String
#define Convert_Fixed_Point_String INIT_Convert_Fixed_Point_String

#undef  Convert_Hex_String
#define Convert_Hex_String INIT_Convert_Hex_String

#undef  _CopyPageTable
#define _CopyPageTable INIT__CopyPageTable

#undef  CopyPageTable
#define CopyPageTable INIT__CopyPageTable

#undef  Create_Semaphore
#define Create_Semaphore INIT_Create_Semaphore

#undef  _AddFreePhysPage
#define _AddFreePhysPage INIT__AddFreePhysPage

#undef  AddFreePhysPage
#define AddFreePhysPage INIT__AddFreePhysPage

#undef  _Add_Global_V86_Data_Area
#define _Add_Global_V86_Data_Area INIT__Add_Global_V86_Data_Area

#undef  Add_Global_V86_Data_Area
#define Add_Global_V86_Data_Area INIT__Add_Global_V86_Data_Area

#undef  _AddInstanceItem
#define _AddInstanceItem INIT__AddInstanceItem

#undef  AddInstanceItem
#define AddInstanceItem INIT__AddInstanceItem

#undef  Adjust_Exec_Priority
#define Adjust_Exec_Priority INIT_Adjust_Exec_Priority

#undef  Adjust_Execution_Time
#define Adjust_Execution_Time INIT_Adjust_Execution_Time

#undef  _Allocate_Device_CB_Area
#define _Allocate_Device_CB_Area INIT__Allocate_Device_CB_Area

#undef  Allocate_Device_CB_Area
#define Allocate_Device_CB_Area INIT__Allocate_Device_CB_Area

#undef  _Allocate_GDT_Selector
#define _Allocate_GDT_Selector INIT__Allocate_GDT_Selector

#undef  Allocate_GDT_Selector
#define Allocate_GDT_Selector INIT__Allocate_GDT_Selector

#undef  _Allocate_Global_V86_Data_Area
#define _Allocate_Global_V86_Data_Area INIT__Allocate_Global_V86_Data_Area

#undef  Allocate_Global_V86_Data_Area
#define Allocate_Global_V86_Data_Area INIT__Allocate_Global_V86_Data_Area

#undef  _Allocate_LDT_Selector
#define _Allocate_LDT_Selector INIT__Allocate_LDT_Selector

#undef  Allocate_LDT_Selector
#define Allocate_LDT_Selector INIT__Allocate_LDT_Selector

#undef  Allocate_PM_Call_Back
#define Allocate_PM_Call_Back INIT_Allocate_PM_Call_Back

#undef  _Allocate_Temp_V86_Data_Area
#define _Allocate_Temp_V86_Data_Area INIT__Allocate_Temp_V86_Data_Area

#undef  Allocate_Temp_V86_Data_Area
#define Allocate_Temp_V86_Data_Area INIT__Allocate_Temp_V86_Data_Area

#undef  Allocate_V86_Call_Back
#define Allocate_V86_Call_Back INIT_Allocate_V86_Call_Back

#undef  _Assign_Device_V86_Pages
#define _Assign_Device_V86_Pages INIT__Assign_Device_V86_Pages

#undef  Assign_Device_V86_Pages
#define Assign_Device_V86_Pages INIT__Assign_Device_V86_Pages

#undef  Begin_Critical_Section
#define Begin_Critical_Section INIT_Begin_Critical_Section

#undef  Begin_Reentrant_Execution
#define Begin_Reentrant_Execution INIT_Begin_Reentrant_Execution

#undef  _BuildDescriptorDWORDs
#define _BuildDescriptorDWORDs INIT__BuildDescriptorDWORDs

#undef  BuildDescriptorDWORDs
#define BuildDescriptorDWORDs INIT__BuildDescriptorDWORDs

#undef  Build_Int_Stack_Frame
#define Build_Int_Stack_Frame INIT_Build_Int_Stack_Frame

#undef  Call_Global_Event
#define Call_Global_Event INIT_Call_Global_Event

#undef  Call_Priority_VM_Event
#define Call_Priority_VM_Event INIT_Call_Priority_VM_Event

#undef  Call_VM_Event
#define Call_VM_Event INIT_Call_VM_Event

#undef  Call_When_Idle
#define Call_When_Idle INIT_Call_When_Idle

#undef  Call_When_Not_Critical
#define Call_When_Not_Critical INIT_Call_When_Not_Critical

#undef  Call_When_Task_Switched
#define Call_When_Task_Switched INIT_Call_When_Task_Switched

#undef  Call_When_VM_Ints_Enabled
#define Call_When_VM_Ints_Enabled INIT_Call_When_VM_Ints_Enabled

#undef  Call_When_VM_Returns
#define Call_When_VM_Returns INIT_Call_When_VM_Returns

#undef  Cancel_Global_Event
#define Cancel_Global_Event INIT_Cancel_Global_Event

#undef  Cancel_Priority_VM_Event
#define Cancel_Priority_VM_Event INIT_Cancel_Priority_VM_Event

#undef  Cancel_Time_Out
#define Cancel_Time_Out INIT_Cancel_Time_Out

#undef  Cancel_VM_Event
#define Cancel_VM_Event INIT_Cancel_VM_Event

#undef  Claim_Critical_Section
#define Claim_Critical_Section INIT_Claim_Critical_Section

#ifdef WIN40_OR_LATER
#undef  _RegOpenKey
#define _RegOpenKey INIT__RegOpenKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegOpenKey
#define RegOpenKey INIT__RegOpenKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCreateKey
#define _RegCreateKey INIT__RegCreateKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCreateKey
#define RegCreateKey INIT__RegCreateKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegDeleteKey
#define _RegDeleteKey INIT__RegDeleteKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegDeleteKey
#define RegDeleteKey INIT__RegDeleteKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegDeleteValue
#define _RegDeleteValue INIT__RegDeleteValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegDeleteValue
#define RegDeleteValue INIT__RegDeleteValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegEnumKey
#define _RegEnumKey INIT__RegEnumKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegEnumKey
#define RegEnumKey INIT__RegEnumKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegEnumValue
#define _RegEnumValue INIT__RegEnumValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegEnumValue
#define RegEnumValue INIT__RegEnumValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegFlushKey
#define _RegFlushKey INIT__RegFlushKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegFlushKey
#define RegFlushKey INIT__RegFlushKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCloseKey
#define _RegCloseKey INIT__RegCloseKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCloseKey
#define RegCloseKey INIT__RegCloseKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryValue
#define _RegQueryValue INIT__RegQueryValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryValue
#define RegQueryValue INIT__RegQueryValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryValueEx
#define _RegQueryValueEx INIT__RegQueryValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryValueEx
#define RegQueryValueEx INIT__RegQueryValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  _RegSetValue
#define _RegSetValue INIT__RegSetValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegSetValue
#define RegSetValue INIT__RegSetValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegSetValueEx
#define _RegSetValueEx INIT__RegSetValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  RegSetValueEx
#define RegSetValueEx INIT__RegSetValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  _EnterMutex
#define _EnterMutex INIT__EnterMutex
#endif

#ifdef WIN40_OR_LATER
#undef  EnterMutex
#define EnterMutex INIT__EnterMutex
#endif

#ifdef WIN40_OR_LATER
#undef  _GetMutexOwner
#define _GetMutexOwner INIT__GetMutexOwner
#endif

#ifdef WIN40_OR_LATER
#undef  GetMutexOwner
#define GetMutexOwner INIT__GetMutexOwner
#endif

#ifdef WIN40_OR_LATER
#undef  _LeaveMutex
#define _LeaveMutex INIT__LeaveMutex
#endif

#ifdef WIN40_OR_LATER
#undef  LeaveMutex
#define LeaveMutex INIT__LeaveMutex
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Fault
#define Unhook_V86_Fault INIT_Unhook_V86_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_PM_Fault
#define Unhook_PM_Fault INIT_Unhook_PM_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_VMM_Fault
#define Unhook_VMM_Fault INIT_Unhook_VMM_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Int_Chain
#define Unhook_V86_Int_Chain INIT_Unhook_V86_Int_Chain
#endif

#ifdef WIN40_OR_LATER
#undef  Signal_Semaphore_No_Switch
#define Signal_Semaphore_No_Switch INIT_Signal_Semaphore_No_Switch
#endif

#ifdef WIN40_OR_LATER
#undef  Remove_IO_Handler
#define Remove_IO_Handler INIT_Remove_IO_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_NMI_Event
#define Unhook_NMI_Event INIT_Unhook_NMI_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Not_Critical
#define Cancel_Call_When_Not_Critical INIT_Cancel_Call_When_Not_Critical
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service
#define Unhook_Device_Service INIT_Unhook_Device_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Async_Time_Out
#define Set_Async_Time_Out INIT_Set_Async_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  Get_DDB
#define Get_DDB INIT_Get_DDB
#endif

#ifdef WIN40_OR_LATER
#undef  _Trace_Out_Service
#define _Trace_Out_Service INIT__Trace_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Trace_Out_Service
#define Trace_Out_Service INIT__Trace_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Out_Service
#define _Debug_Out_Service INIT__Debug_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Out_Service
#define Debug_Out_Service INIT__Debug_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _AllocateThreadDataSlot
#define _AllocateThreadDataSlot INIT__AllocateThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  AllocateThreadDataSlot
#define AllocateThreadDataSlot INIT__AllocateThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  _FreeThreadDataSlot
#define _FreeThreadDataSlot INIT__FreeThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  FreeThreadDataSlot
#define FreeThreadDataSlot INIT__FreeThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  _GetLastUpdatedThreadExecTime
#define _GetLastUpdatedThreadExecTime INIT__GetLastUpdatedThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  GetLastUpdatedThreadExecTime
#define GetLastUpdatedThreadExecTime INIT__GetLastUpdatedThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  VMMAddImportModuleName
#define VMMAddImportModuleName INIT_VMMAddImportModuleName
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_Add_DDB
#define VMM_Add_DDB INIT_VMM_Add_DDB
#endif

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
#undef  VMM_Remove_DDB
#define VMM_Remove_DDB INIT_VMM_Remove_DDB
#endif

#ifdef WIN40_OR_LATER
#undef  Test_VM_Ints_Enabled
#define Test_VM_Ints_Enabled INIT_Test_VM_Ints_Enabled
#endif

#ifdef WIN40_OR_LATER
#undef  _BlockOnID
#define _BlockOnID INIT__BlockOnID
#endif

#ifdef WIN40_OR_LATER
#undef  BlockOnID
#define BlockOnID INIT__BlockOnID
#endif

#ifdef WIN40_OR_LATER
#undef  Schedule_Thread_Event
#define Schedule_Thread_Event INIT_Schedule_Thread_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Thread_Event
#define Cancel_Thread_Event INIT_Cancel_Thread_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Time_Out
#define Set_Thread_Time_Out INIT_Set_Thread_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  _CreateMutex
#define _CreateMutex INIT__CreateMutex
#endif

#ifdef WIN40_OR_LATER
#undef  CreateMutex
#define CreateMutex INIT__CreateMutex
#endif

#ifdef WIN40_OR_LATER
#undef  _DestroyMutex
#define _DestroyMutex INIT__DestroyMutex
#endif

#ifdef WIN40_OR_LATER
#undef  DestroyMutex
#define DestroyMutex INIT__DestroyMutex
#endif

#ifdef WIN40_OR_LATER
#undef  Call_When_Thread_Switched
#define Call_When_Thread_Switched INIT_Call_When_Thread_Switched
#endif

#ifdef WIN40_OR_LATER
#undef  VMMCreateThread
#define VMMCreateThread INIT_VMMCreateThread
#endif

#ifdef WIN40_OR_LATER
#undef  _GetThreadExecTime
#define _GetThreadExecTime INIT__GetThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  GetThreadExecTime
#define GetThreadExecTime INIT__GetThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  VMMTerminateThread
#define VMMTerminateThread INIT_VMMTerminateThread
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Cur_Thread_Handle
#define Get_Cur_Thread_Handle INIT_Get_Cur_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Cur_Thread_Handle
#define Test_Cur_Thread_Handle INIT_Test_Cur_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Sys_Thread_Handle
#define Get_Sys_Thread_Handle INIT_Get_Sys_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Sys_Thread_Handle
#define Test_Sys_Thread_Handle INIT_Test_Sys_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Validate_Thread_Handle
#define Validate_Thread_Handle INIT_Validate_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Initial_Thread_Handle
#define Get_Initial_Thread_Handle INIT_Get_Initial_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Initial_Thread_Handle
#define Test_Initial_Thread_Handle INIT_Test_Initial_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Valid_Thread_Handle
#define Debug_Test_Valid_Thread_Handle INIT_Debug_Test_Valid_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Cur_Thread
#define Debug_Test_Cur_Thread INIT_Debug_Test_Cur_Thread
#endif

#undef  VMM_GetSystemInitState
#define VMM_GetSystemInitState INIT_VMM_GetSystemInitState

#undef  Cancel_Call_When_Thread_Switched
#define Cancel_Call_When_Thread_Switched INIT_Cancel_Call_When_Thread_Switched

#undef  Get_Next_Thread_Handle
#define Get_Next_Thread_Handle INIT_Get_Next_Thread_Handle

#ifdef WIN40_OR_LATER
#undef  Adjust_Thread_Exec_Priority
#define Adjust_Thread_Exec_Priority INIT_Adjust_Thread_Exec_Priority
#endif

#ifdef WIN40_OR_LATER
#undef  _Deallocate_Device_CB_Area
#define _Deallocate_Device_CB_Area INIT__Deallocate_Device_CB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  Deallocate_Device_CB_Area
#define Deallocate_Device_CB_Area INIT__Deallocate_Device_CB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  Remove_Mult_IO_Handlers
#define Remove_Mult_IO_Handlers INIT_Remove_Mult_IO_Handlers
#endif

#ifdef WIN40_OR_LATER
#undef  _PageReserve
#define _PageReserve INIT__PageReserve
#endif

#ifdef WIN40_OR_LATER
#undef  PageReserve
#define PageReserve INIT__PageReserve
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommit
#define _PageCommit INIT__PageCommit
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommit
#define PageCommit INIT__PageCommit
#endif

#ifdef WIN40_OR_LATER
#undef  _PageDecommit
#define _PageDecommit INIT__PageDecommit
#endif

#ifdef WIN40_OR_LATER
#undef  PageDecommit
#define PageDecommit INIT__PageDecommit
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerRegister
#define _PagerRegister INIT__PagerRegister
#endif

#ifdef WIN40_OR_LATER
#undef  PagerRegister
#define PagerRegister INIT__PagerRegister
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerQuery
#define _PagerQuery INIT__PagerQuery
#endif

#ifdef WIN40_OR_LATER
#undef  PagerQuery
#define PagerQuery INIT__PagerQuery
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerDeregister
#define _PagerDeregister INIT__PagerDeregister
#endif

#ifdef WIN40_OR_LATER
#undef  PagerDeregister
#define PagerDeregister INIT__PagerDeregister
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextCreate
#define _ContextCreate INIT__ContextCreate
#endif

#ifdef WIN40_OR_LATER
#undef  ContextCreate
#define ContextCreate INIT__ContextCreate
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextDestroy
#define _ContextDestroy INIT__ContextDestroy
#endif

#ifdef WIN40_OR_LATER
#undef  ContextDestroy
#define ContextDestroy INIT__ContextDestroy
#endif

#ifdef WIN40_OR_LATER
#undef  _PageAttach
#define _PageAttach INIT__PageAttach
#endif

#ifdef WIN40_OR_LATER
#undef  PageAttach
#define PageAttach INIT__PageAttach
#endif

#ifdef WIN40_OR_LATER
#undef  _PageFlush
#define _PageFlush INIT__PageFlush
#endif

#ifdef WIN40_OR_LATER
#undef  PageFlush
#define PageFlush INIT__PageFlush
#endif

#ifdef WIN40_OR_LATER
#undef  _SignalID
#define _SignalID INIT__SignalID
#endif

#ifdef WIN40_OR_LATER
#undef  SignalID
#define SignalID INIT__SignalID
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommitPhys
#define _PageCommitPhys INIT__PageCommitPhys
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommitPhys
#define PageCommitPhys INIT__PageCommitPhys
#endif

#ifdef WIN40_OR_LATER
#undef  _Register_Win32_Services
#define _Register_Win32_Services INIT__Register_Win32_Services
#endif

#ifdef WIN40_OR_LATER
#undef  Register_Win32_Services
#define Register_Win32_Services INIT__Register_Win32_Services
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Idle
#define Cancel_Call_When_Idle INIT_Cancel_Call_When_Idle
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Task_Switched
#define Cancel_Call_When_Task_Switched INIT_Cancel_Call_When_Task_Switched
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Inst_V86_Int_Vec_Base
#define Get_Inst_V86_Int_Vec_Base INIT_Get_Inst_V86_Int_Vec_Base
#endif

#ifdef WIN40_OR_LATER
#undef  Simulate_VM_IO
#define Simulate_VM_IO INIT_Simulate_VM_IO
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextSwitch
#define _ContextSwitch INIT__ContextSwitch
#endif

#ifdef WIN40_OR_LATER
#undef  ContextSwitch
#define ContextSwitch INIT__ContextSwitch
#endif

#ifdef WIN40_OR_LATER
#undef  _PageModifyPermissions
#define _PageModifyPermissions INIT__PageModifyPermissions
#endif

#ifdef WIN40_OR_LATER
#undef  PageModifyPermissions
#define PageModifyPermissions INIT__PageModifyPermissions
#endif

#ifdef WIN40_OR_LATER
#undef  _PageQuery
#define _PageQuery INIT__PageQuery
#endif

#ifdef WIN40_OR_LATER
#undef  PageQuery
#define PageQuery INIT__PageQuery
#endif

#ifdef WIN40_OR_LATER
#undef  _GetThreadTerminationStatus
#define _GetThreadTerminationStatus INIT__GetThreadTerminationStatus
#endif

#ifdef WIN40_OR_LATER
#undef  GetThreadTerminationStatus
#define GetThreadTerminationStatus INIT__GetThreadTerminationStatus
#endif

#ifdef WIN40_OR_LATER
#undef  _GetInstanceInfo
#define _GetInstanceInfo INIT__GetInstanceInfo
#endif

#ifdef WIN40_OR_LATER
#undef  GetInstanceInfo
#define GetInstanceInfo INIT__GetInstanceInfo
#endif

#ifdef WIN40_OR_LATER
#undef  _ExecIntMustComplete
#define _ExecIntMustComplete INIT__ExecIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ExecIntMustComplete
#define ExecIntMustComplete INIT__ExecIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _ExecVxDIntMustComplete
#define _ExecVxDIntMustComplete INIT__ExecVxDIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ExecVxDIntMustComplete
#define ExecVxDIntMustComplete INIT__ExecVxDIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  Begin_V86_Serialization
#define Begin_V86_Serialization INIT_Begin_V86_Serialization
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Page
#define Unhook_V86_Page INIT_Unhook_V86_Page
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_GetVxDLocationList
#define VMM_GetVxDLocationList INIT_VMM_GetVxDLocationList
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_GetDDBList
#define VMM_GetDDBList INIT_VMM_GetDDBList
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Instanced_V86_Int_Vector
#define Get_Instanced_V86_Int_Vector INIT_Get_Instanced_V86_Int_Vector
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Set_Real_DOS_PSP
#define Get_Set_Real_DOS_PSP INIT_Get_Set_Real_DOS_PSP
#endif

#ifdef WIN40_OR_LATER
#undef  Get_System_Time_Address
#define Get_System_Time_Address INIT_Get_System_Time_Address
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Crit_Status_Thread
#define Get_Crit_Status_Thread INIT_Get_Crit_Status_Thread
#endif

#ifdef WIN40_OR_LATER
#undef  Directed_Sys_Control
#define Directed_Sys_Control INIT_Directed_Sys_Control
#endif

#ifdef WIN40_OR_LATER
#undef  _CallRing3
#define _CallRing3 INIT__CallRing3
#endif

#ifdef WIN40_OR_LATER
#undef  CallRing3
#define CallRing3 INIT__CallRing3
#endif

#ifdef WIN40_OR_LATER
#undef  Exec_PM_Int
#define Exec_PM_Int INIT_Exec_PM_Int
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommitContig
#define _PageCommitContig INIT__PageCommitContig
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommitContig
#define PageCommitContig INIT__PageCommitContig
#endif

#ifdef WIN40_OR_LATER
#undef  _GetCurrentContext
#define _GetCurrentContext INIT__GetCurrentContext
#endif

#ifdef WIN40_OR_LATER
#undef  GetCurrentContext
#define GetCurrentContext INIT__GetCurrentContext
#endif

#ifdef WIN40_OR_LATER
#undef  Call_Restricted_Event
#define Call_Restricted_Event INIT_Call_Restricted_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Restricted_Event
#define Cancel_Restricted_Event INIT_Cancel_Restricted_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Register_PEF_Provider
#define Register_PEF_Provider INIT_Register_PEF_Provider
#endif

#ifdef WIN40_OR_LATER
#undef  _GetPhysPageInfo
#define _GetPhysPageInfo INIT__GetPhysPageInfo
#endif

#ifdef WIN40_OR_LATER
#undef  GetPhysPageInfo
#define GetPhysPageInfo INIT__GetPhysPageInfo
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryInfoKey
#define _RegQueryInfoKey INIT__RegQueryInfoKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryInfoKey
#define RegQueryInfoKey INIT__RegQueryInfoKey
#endif

#ifdef WIN40_OR_LATER
#undef  Time_Slice_Sleep
#define Time_Slice_Sleep INIT_Time_Slice_Sleep
#endif

#ifdef WIN40_OR_LATER
#undef  Boost_With_Decay
#define Boost_With_Decay INIT_Boost_With_Decay
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Inversion_Pri
#define Set_Inversion_Pri INIT_Set_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Reset_Inversion_Pri
#define Reset_Inversion_Pri INIT_Reset_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Release_Inversion_Pri
#define Release_Inversion_Pri INIT_Release_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Thread_Win32_Pri
#define Get_Thread_Win32_Pri INIT_Get_Thread_Win32_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Win32_Pri
#define Set_Thread_Win32_Pri INIT_Set_Thread_Win32_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Static_Boost
#define Set_Thread_Static_Boost INIT_Set_Thread_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  Set_VM_Static_Boost
#define Set_VM_Static_Boost INIT_Set_VM_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  Release_Inversion_Pri_ID
#define Release_Inversion_Pri_ID INIT_Release_Inversion_Pri_ID
#endif

#ifdef WIN40_OR_LATER
#undef  Attach_Thread_To_Group
#define Attach_Thread_To_Group INIT_Attach_Thread_To_Group
#endif

#ifdef WIN40_OR_LATER
#undef  Detach_Thread_From_Group
#define Detach_Thread_From_Group INIT_Detach_Thread_From_Group
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Group_Static_Boost
#define Set_Group_Static_Boost INIT_Set_Group_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  _GetRegistryPath
#define _GetRegistryPath INIT__GetRegistryPath
#endif

#ifdef WIN40_OR_LATER
#undef  GetRegistryPath
#define GetRegistryPath INIT__GetRegistryPath
#endif

#ifdef WIN40_OR_LATER
#undef  _RegRemapPreDefKey
#define _RegRemapPreDefKey INIT__RegRemapPreDefKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegRemapPreDefKey
#define RegRemapPreDefKey INIT__RegRemapPreDefKey
#endif

#ifdef WIN40_OR_LATER
#undef  End_V86_Serialization
#define End_V86_Serialization INIT_End_V86_Serialization
#endif

#ifdef WIN40_OR_LATER
#undef  _Assert_Range
#define _Assert_Range INIT__Assert_Range
#endif

#ifdef WIN40_OR_LATER
#undef  Assert_Range
#define Assert_Range INIT__Assert_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _PageChangePager
#define _PageChangePager INIT__PageChangePager
#endif

#ifdef WIN40_OR_LATER
#undef  PageChangePager
#define PageChangePager INIT__PageChangePager
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCreateDynKey
#define _RegCreateDynKey INIT__RegCreateDynKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCreateDynKey
#define RegCreateDynKey INIT__RegCreateDynKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryMultipleValues
#define _RegQueryMultipleValues INIT__RegQueryMultipleValues
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryMultipleValues
#define RegQueryMultipleValues INIT__RegQueryMultipleValues
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Boot_Flags
#define Get_Boot_Flags INIT_Get_Boot_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Boot_Flags
#define Set_Boot_Flags INIT_Set_Boot_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrcpyn
#define _lstrcpyn INIT__lstrcpyn
#endif

#ifdef WIN40_OR_LATER
#undef  lstrcpyn
#define lstrcpyn INIT__lstrcpyn
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrlen
#define _lstrlen INIT__lstrlen
#endif

#ifdef WIN40_OR_LATER
#undef  lstrlen
#define lstrlen INIT__lstrlen
#endif

#ifdef WIN40_OR_LATER
#undef  _lmemcpy
#define _lmemcpy INIT__lmemcpy
#endif

#ifdef WIN40_OR_LATER
#undef  lmemcpy
#define lmemcpy INIT__lmemcpy
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrcmpi
#define _lstrcmpi INIT__lstrcmpi
#endif

#ifdef WIN40_OR_LATER
#undef  lstrcmpi
#define lstrcmpi INIT__lstrcmpi
#endif

#ifdef WIN40_OR_LATER
#undef  _GetVxDName
#define _GetVxDName INIT__GetVxDName
#endif

#ifdef WIN40_OR_LATER
#undef  GetVxDName
#define GetVxDName INIT__GetVxDName
#endif

#undef  BlockDev_Command_Complete
#define BlockDev_Command_Complete INIT_BlockDev_Command_Complete

#undef  BlockDev_Find_Int13_Drive
#define BlockDev_Find_Int13_Drive INIT_BlockDev_Find_Int13_Drive

#undef  BlockDev_Get_Device_List
#define BlockDev_Get_Device_List INIT_BlockDev_Get_Device_List

#undef  BlockDev_Get_Version
#define BlockDev_Get_Version INIT_BlockDev_Get_Version

#undef  BlockDev_Register_Device
#define BlockDev_Register_Device INIT_BlockDev_Register_Device

#undef  BlockDev_Send_Command
#define BlockDev_Send_Command INIT_BlockDev_Send_Command

#undef  BlockDev_Synchronous_Command
#define BlockDev_Synchronous_Command INIT_BlockDev_Synchronous_Command

#undef  BlockDev_Thunk_Sync_Cmd
#define BlockDev_Thunk_Sync_Cmd INIT_BlockDev_Thunk_Sync_Cmd

#undef  BlockDev_Thunk_Command
#define BlockDev_Thunk_Command INIT_BlockDev_Thunk_Command

#undef  BlockDev_Thunk_HW_Int
#define BlockDev_Thunk_HW_Int INIT_BlockDev_Thunk_HW_Int

#undef  DOSMGR_Add_Device
#define DOSMGR_Add_Device INIT_DOSMGR_Add_Device

#undef  DOSMGR_BackFill_Allowed
#define DOSMGR_BackFill_Allowed INIT_DOSMGR_BackFill_Allowed

#undef  DOSMGR_Copy_VM_Drive_State
#define DOSMGR_Copy_VM_Drive_State INIT_DOSMGR_Copy_VM_Drive_State

#undef  DOSMGR_Enable_Indos_Polling
#define DOSMGR_Enable_Indos_Polling INIT_DOSMGR_Enable_Indos_Polling

#undef  _DOSMGR_Exec_VM
#define _DOSMGR_Exec_VM INIT__DOSMGR_Exec_VM

#undef  DOSMGR_Exec_VM
#define DOSMGR_Exec_VM INIT__DOSMGR_Exec_VM

#undef  DOSMGR_Get_DOS_Crit_Status
#define DOSMGR_Get_DOS_Crit_Status INIT_DOSMGR_Get_DOS_Crit_Status

#undef  DOSMGR_Get_IndosPtr
#define DOSMGR_Get_IndosPtr INIT_DOSMGR_Get_IndosPtr

#undef  DOSMGR_Get_Version
#define DOSMGR_Get_Version INIT_DOSMGR_Get_Version

#undef  DOSMGR_Instance_Device
#define DOSMGR_Instance_Device INIT_DOSMGR_Instance_Device

#undef  DOSMGR_LocalGlobalReg
#define DOSMGR_LocalGlobalReg INIT_DOSMGR_LocalGlobalReg

#undef  DOSMGR_Remove_Device
#define DOSMGR_Remove_Device INIT_DOSMGR_Remove_Device

#undef  _DOSMGR_Set_Exec_VM_Data
#define _DOSMGR_Set_Exec_VM_Data INIT__DOSMGR_Set_Exec_VM_Data

#undef  DOSMGR_Set_Exec_VM_Data
#define DOSMGR_Set_Exec_VM_Data INIT__DOSMGR_Set_Exec_VM_Data

#undef  DOSNET_Do_PSP_Adjust
#define DOSNET_Do_PSP_Adjust INIT_DOSNET_Do_PSP_Adjust

#undef  DOSNET_Get_Version
#define DOSNET_Get_Version INIT_DOSNET_Get_Version

#undef  DOSNET_Send_FILESYSCHANGE
#define DOSNET_Send_FILESYSCHANGE INIT_DOSNET_Send_FILESYSCHANGE

#undef  EBIOS_Get_Unused_Mem
#define EBIOS_Get_Unused_Mem INIT_EBIOS_Get_Unused_Mem

#undef  EBIOS_Get_Version
#define EBIOS_Get_Version INIT_EBIOS_Get_Version

#undef  Int13_Device_Registered
#define Int13_Device_Registered INIT_Int13_Device_Registered

#undef  Int13_Hooking_BIOS_Int
#define Int13_Hooking_BIOS_Int INIT_Int13_Hooking_BIOS_Int

#undef  Int13_Translate_VM_Int
#define Int13_Translate_VM_Int INIT_Int13_Translate_VM_Int

#undef  Int13_Unhooking_BIOS_Int
#define Int13_Unhooking_BIOS_Int INIT_Int13_Unhooking_BIOS_Int

#undef  VMD_Get_Mouse_Owner
#define VMD_Get_Mouse_Owner INIT_VMD_Get_Mouse_Owner

#undef  VMD_Get_Version
#define VMD_Get_Version INIT_VMD_Get_Version

#undef  VMD_Set_Mouse_Type
#define VMD_Set_Mouse_Type INIT_VMD_Set_Mouse_Type

#undef  SHELL_Event
#define SHELL_Event INIT_SHELL_Event

#undef  SHELL_Get_Version
#define SHELL_Get_Version INIT_SHELL_Get_Version

#undef  SHELL_GetVMInfo
#define SHELL_GetVMInfo INIT_SHELL_GetVMInfo

#undef  SHELL_Message
#define SHELL_Message INIT_SHELL_Message

#undef  SHELL_Resolve_Contention
#define SHELL_Resolve_Contention INIT_SHELL_Resolve_Contention

#undef  SHELL_SYSMODAL_Message
#define SHELL_SYSMODAL_Message INIT_SHELL_SYSMODAL_Message

#ifdef WIN40_OR_LATER
#undef  _SHELL_CallAtAppyTime
#define _SHELL_CallAtAppyTime INIT__SHELL_CallAtAppyTime
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CallAtAppyTime
#define SHELL_CallAtAppyTime INIT__SHELL_CallAtAppyTime
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_CancelAppyTimeEvent
#define _SHELL_CancelAppyTimeEvent INIT__SHELL_CancelAppyTimeEvent
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CancelAppyTimeEvent
#define SHELL_CancelAppyTimeEvent INIT__SHELL_CancelAppyTimeEvent
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_QueryAppyTimeAvailable
#define _SHELL_QueryAppyTimeAvailable INIT__SHELL_QueryAppyTimeAvailable
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_QueryAppyTimeAvailable
#define SHELL_QueryAppyTimeAvailable INIT__SHELL_QueryAppyTimeAvailable
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LocalAllocEx
#define _SHELL_LocalAllocEx INIT__SHELL_LocalAllocEx
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LocalAllocEx
#define SHELL_LocalAllocEx INIT__SHELL_LocalAllocEx
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LocalFree
#define _SHELL_LocalFree INIT__SHELL_LocalFree
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LocalFree
#define SHELL_LocalFree INIT__SHELL_LocalFree
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_CallDll
#define _SHELL_CallDll INIT__SHELL_CallDll
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CallDll
#define SHELL_CallDll INIT__SHELL_CallDll
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_BroadcastSystemMessage
#define _SHELL_BroadcastSystemMessage INIT__SHELL_BroadcastSystemMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_BroadcastSystemMessage
#define SHELL_BroadcastSystemMessage INIT__SHELL_BroadcastSystemMessage
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_HookSystemBroadcast
#define _SHELL_HookSystemBroadcast INIT__SHELL_HookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_HookSystemBroadcast
#define SHELL_HookSystemBroadcast INIT__SHELL_HookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_UnhookSystemBroadcast
#define _SHELL_UnhookSystemBroadcast INIT__SHELL_UnhookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_UnhookSystemBroadcast
#define SHELL_UnhookSystemBroadcast INIT__SHELL_UnhookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_FreeLibrary
#define _SHELL_FreeLibrary INIT__SHELL_FreeLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_FreeLibrary
#define SHELL_FreeLibrary INIT__SHELL_FreeLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_GetProcAddress
#define _SHELL_GetProcAddress INIT__SHELL_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_GetProcAddress
#define SHELL_GetProcAddress INIT__SHELL_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LoadLibrary
#define _SHELL_LoadLibrary INIT__SHELL_LoadLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LoadLibrary
#define SHELL_LoadLibrary INIT__SHELL_LoadLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_PostMessage
#define _SHELL_PostMessage INIT__SHELL_PostMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_PostMessage
#define SHELL_PostMessage INIT__SHELL_PostMessage
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_PostShellMessage
#define _SHELL_PostShellMessage INIT__SHELL_PostShellMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_PostShellMessage
#define SHELL_PostShellMessage INIT__SHELL_PostShellMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Hook_Properties
#define SHELL_Hook_Properties INIT_SHELL_Hook_Properties
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Unhook_Properties
#define SHELL_Unhook_Properties INIT_SHELL_Unhook_Properties
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_ShellExecute
#define _SHELL_ShellExecute INIT__SHELL_ShellExecute
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_ShellExecute
#define SHELL_ShellExecute INIT__SHELL_ShellExecute
#endif

#undef  V86MMGR_Allocate_Buffer
#define V86MMGR_Allocate_Buffer INIT_V86MMGR_Allocate_Buffer

#undef  V86MMGR_Allocate_V86_Pages
#define V86MMGR_Allocate_V86_Pages INIT_V86MMGR_Allocate_V86_Pages

#undef  V86MMGR_Free_Buffer
#define V86MMGR_Free_Buffer INIT_V86MMGR_Free_Buffer

#undef  V86MMGR_Free_Page_Map_Region
#define V86MMGR_Free_Page_Map_Region INIT_V86MMGR_Free_Page_Map_Region

#undef  V86MMGR_Get_EMS_XMS_Limits
#define V86MMGR_Get_EMS_XMS_Limits INIT_V86MMGR_Get_EMS_XMS_Limits

#undef  V86MMGR_Get_Mapping_Info
#define V86MMGR_Get_Mapping_Info INIT_V86MMGR_Get_Mapping_Info

#undef  V86MMGR_Get_Version
#define V86MMGR_Get_Version INIT_V86MMGR_Get_Version

#undef  V86MMGR_Get_VM_Flat_Sel
#define V86MMGR_Get_VM_Flat_Sel INIT_V86MMGR_Get_VM_Flat_Sel

#undef  V86MMGR_Get_Xlat_Buff_State
#define V86MMGR_Get_Xlat_Buff_State INIT_V86MMGR_Get_Xlat_Buff_State

#undef  V86MMGR_GetPgStatus
#define V86MMGR_GetPgStatus INIT_V86MMGR_GetPgStatus

#undef  V86MMGR_Load_Client_Ptr
#define V86MMGR_Load_Client_Ptr INIT_V86MMGR_Load_Client_Ptr

#undef  V86MMGR_LocalGlobalReg
#define V86MMGR_LocalGlobalReg INIT_V86MMGR_LocalGlobalReg

#undef  V86MMGR_Map_Pages
#define V86MMGR_Map_Pages INIT_V86MMGR_Map_Pages

#undef  V86MMGR_ResetBasePages
#define V86MMGR_ResetBasePages INIT_V86MMGR_ResetBasePages

#undef  V86MMGR_Set_EMS_XMS_Limits
#define V86MMGR_Set_EMS_XMS_Limits INIT_V86MMGR_Set_EMS_XMS_Limits

#undef  V86MMGR_Set_Mapping_Info
#define V86MMGR_Set_Mapping_Info INIT_V86MMGR_Set_Mapping_Info

#undef  V86MMGR_Set_Xlat_Buff_State
#define V86MMGR_Set_Xlat_Buff_State INIT_V86MMGR_Set_Xlat_Buff_State

#undef  V86MMGR_SetAvailMapPgs
#define V86MMGR_SetAvailMapPgs INIT_V86MMGR_SetAvailMapPgs

#undef  V86MMGR_Xlat_API
#define V86MMGR_Xlat_API INIT_V86MMGR_Xlat_API

#undef  VCD_Get_Focus
#define VCD_Get_Focus INIT_VCD_Get_Focus

#undef  VCD_Get_Version
#define VCD_Get_Version INIT_VCD_Get_Version

#undef  VCD_Set_Port_Global
#define VCD_Set_Port_Global INIT_VCD_Set_Port_Global

#undef  VCD_Virtualize_Port
#define VCD_Virtualize_Port INIT_VCD_Virtualize_Port

#ifdef WIN40_OR_LATER
#undef  VMCPD_Set_CR0_State
#define VMCPD_Set_CR0_State INIT_VMCPD_Set_CR0_State
#endif

#ifdef WIN40_OR_LATER
#undef  VMCPD_Get_CR0_State
#define VMCPD_Get_CR0_State INIT_VMCPD_Get_CR0_State
#endif

#undef  VMCPD_Get_Thread_State
#define VMCPD_Get_Thread_State INIT_VMCPD_Get_Thread_State

#ifdef WIN40_OR_LATER
#undef  VMCPD_Set_Thread_State
#define VMCPD_Set_Thread_State INIT_VMCPD_Set_Thread_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Virtual_EOI
#define VPICD_Virtual_EOI INIT_VPICD_Virtual_EOI
#endif

#ifdef WIN40_OR_LATER
#undef  VTD_Get_Date_And_Time
#define VTD_Get_Date_And_Time INIT_VTD_Get_Date_And_Time
#endif

#undef  VDD_Get_ModTime
#define VDD_Get_ModTime INIT_VDD_Get_ModTime

#undef  VDD_Get_Version
#define VDD_Get_Version INIT_VDD_Get_Version

#undef  VDD_Msg_BakColor
#define VDD_Msg_BakColor INIT_VDD_Msg_BakColor

#undef  VDD_Msg_ClrScrn
#define VDD_Msg_ClrScrn INIT_VDD_Msg_ClrScrn

#undef  VDD_Msg_ForColor
#define VDD_Msg_ForColor INIT_VDD_Msg_ForColor

#undef  VDD_SetCursPos
#define VDD_SetCursPos INIT_VDD_SetCursPos

#undef  VDD_Msg_TextOut
#define VDD_Msg_TextOut INIT_VDD_Msg_TextOut

#undef  VDD_PIF_State
#define VDD_PIF_State INIT_VDD_PIF_State

#undef  VDD_Query_Access
#define VDD_Query_Access INIT_VDD_Query_Access

#undef  VDD_Set_HCurTrk
#define VDD_Set_HCurTrk INIT_VDD_Set_HCurTrk

#undef  VDD_Set_VMType
#define VDD_Set_VMType INIT_VDD_Set_VMType

#undef  VDMAD_Copy_From_Buffer
#define VDMAD_Copy_From_Buffer INIT_VDMAD_Copy_From_Buffer

#undef  VDMAD_Copy_To_Buffer
#define VDMAD_Copy_To_Buffer INIT_VDMAD_Copy_To_Buffer

#undef  VDMAD_Default_Handler
#define VDMAD_Default_Handler INIT_VDMAD_Default_Handler

#undef  VDMAD_Disable_Translation
#define VDMAD_Disable_Translation INIT_VDMAD_Disable_Translation

#undef  VDMAD_Enable_Translation
#define VDMAD_Enable_Translation INIT_VDMAD_Enable_Translation

#undef  VDMAD_Get_EISA_Adr_Mode
#define VDMAD_Get_EISA_Adr_Mode INIT_VDMAD_Get_EISA_Adr_Mode

#undef  VDMAD_Get_Region_Info
#define VDMAD_Get_Region_Info INIT_VDMAD_Get_Region_Info

#undef  VDMAD_Get_Version
#define VDMAD_Get_Version INIT_VDMAD_Get_Version

#undef  VDMAD_Get_Virt_State
#define VDMAD_Get_Virt_State INIT_VDMAD_Get_Virt_State

#undef  VDMAD_Lock_DMA_Region
#define VDMAD_Lock_DMA_Region INIT_VDMAD_Lock_DMA_Region

#undef  VDMAD_Mask_Channel
#define VDMAD_Mask_Channel INIT_VDMAD_Mask_Channel

#undef  VDMAD_Release_Buffer
#define VDMAD_Release_Buffer INIT_VDMAD_Release_Buffer

#undef  VDMAD_Request_Buffer
#define VDMAD_Request_Buffer INIT_VDMAD_Request_Buffer

#undef  VDMAD_Reserve_Buffer_Space
#define VDMAD_Reserve_Buffer_Space INIT_VDMAD_Reserve_Buffer_Space

#undef  VDMAD_Scatter_Lock
#define VDMAD_Scatter_Lock INIT_VDMAD_Scatter_Lock

#undef  VDMAD_Scatter_Unlock
#define VDMAD_Scatter_Unlock INIT_VDMAD_Scatter_Unlock

#undef  VDMAD_Set_EISA_Adr_Mode
#define VDMAD_Set_EISA_Adr_Mode INIT_VDMAD_Set_EISA_Adr_Mode

#undef  VDMAD_Set_Phys_State
#define VDMAD_Set_Phys_State INIT_VDMAD_Set_Phys_State

#undef  VDMAD_Set_Region_Info
#define VDMAD_Set_Region_Info INIT_VDMAD_Set_Region_Info

#undef  VDMAD_Set_Virt_State
#define VDMAD_Set_Virt_State INIT_VDMAD_Set_Virt_State

#undef  VDMAD_Unlock_DMA_Region
#define VDMAD_Unlock_DMA_Region INIT_VDMAD_Unlock_DMA_Region

#undef  VDMAD_UnMask_Channel
#define VDMAD_UnMask_Channel INIT_VDMAD_UnMask_Channel

#undef  VDMAD_Virtualize_Channel
#define VDMAD_Virtualize_Channel INIT_VDMAD_Virtualize_Channel

#undef  VKD_Cancel_Paste
#define VKD_Cancel_Paste INIT_VKD_Cancel_Paste

#undef  VKD_Define_Hot_Key
#define VKD_Define_Hot_Key INIT_VKD_Define_Hot_Key

#undef  VKD_Define_Paste_Mode
#define VKD_Define_Paste_Mode INIT_VKD_Define_Paste_Mode

#undef  VKD_Flush_Msg_Key_Queue
#define VKD_Flush_Msg_Key_Queue INIT_VKD_Flush_Msg_Key_Queue

#undef  VKD_Force_Keys
#define VKD_Force_Keys INIT_VKD_Force_Keys

#undef  VKD_Get_Kbd_Owner
#define VKD_Get_Kbd_Owner INIT_VKD_Get_Kbd_Owner

#undef  VKD_Get_Msg_Key
#define VKD_Get_Msg_Key INIT_VKD_Get_Msg_Key

#undef  VKD_Get_Version
#define VKD_Get_Version INIT_VKD_Get_Version

#undef  VKD_Local_Disable_Hot_Key
#define VKD_Local_Disable_Hot_Key INIT_VKD_Local_Disable_Hot_Key

#undef  VKD_Local_Enable_Hot_Key
#define VKD_Local_Enable_Hot_Key INIT_VKD_Local_Enable_Hot_Key

#undef  VKD_Peek_Msg_Key
#define VKD_Peek_Msg_Key INIT_VKD_Peek_Msg_Key

#undef  VKD_Reflect_Hot_Key
#define VKD_Reflect_Hot_Key INIT_VKD_Reflect_Hot_Key

#undef  VKD_Remove_Hot_Key
#define VKD_Remove_Hot_Key INIT_VKD_Remove_Hot_Key

#undef  VKD_Start_Paste
#define VKD_Start_Paste INIT_VKD_Start_Paste

#undef  VMCPD_Get_Version
#define VMCPD_Get_Version INIT_VMCPD_Get_Version

#undef  VMCPD_Get_Virt_State
#define VMCPD_Get_Virt_State INIT_VMCPD_Get_Virt_State

#undef  VMCPD_Set_Virt_State
#define VMCPD_Set_Virt_State INIT_VMCPD_Set_Virt_State

#undef  VMPoll_Enable_Disable
#define VMPoll_Enable_Disable INIT_VMPoll_Enable_Disable

#undef  VMPoll_Get_Version
#define VMPoll_Get_Version INIT_VMPoll_Get_Version

#undef  VMPoll_Reset_Detection
#define VMPoll_Reset_Detection INIT_VMPoll_Reset_Detection

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  VPICD_Call_When_Hw_Int
#define VPICD_Call_When_Hw_Int INIT_VPICD_Call_When_Hw_Int

#undef  VPICD_Clear_Int_Request
#define VPICD_Clear_Int_Request INIT_VPICD_Clear_Int_Request

#undef  VPICD_Convert_Handle_To_IRQ
#define VPICD_Convert_Handle_To_IRQ INIT_VPICD_Convert_Handle_To_IRQ

#undef  VPICD_Convert_Int_To_IRQ
#define VPICD_Convert_Int_To_IRQ INIT_VPICD_Convert_Int_To_IRQ

#undef  VPICD_Convert_IRQ_To_Int
#define VPICD_Convert_IRQ_To_Int INIT_VPICD_Convert_IRQ_To_Int

#undef  VPICD_Force_Default_Behavior
#define VPICD_Force_Default_Behavior INIT_VPICD_Force_Default_Behavior

#undef  VPICD_Force_Default_Owner
#define VPICD_Force_Default_Owner INIT_VPICD_Force_Default_Owner

#undef  VPICD_Get_Complete_Status
#define VPICD_Get_Complete_Status INIT_VPICD_Get_Complete_Status

#undef  VPICD_Get_IRQ_Complete_Status
#define VPICD_Get_IRQ_Complete_Status INIT_VPICD_Get_IRQ_Complete_Status

#undef  VPICD_Get_Status
#define VPICD_Get_Status INIT_VPICD_Get_Status

#undef  VPICD_Get_Version
#define VPICD_Get_Version INIT_VPICD_Get_Version

#undef  VPICD_Phys_EOI
#define VPICD_Phys_EOI INIT_VPICD_Phys_EOI

#undef  VPICD_Physically_Mask
#define VPICD_Physically_Mask INIT_VPICD_Physically_Mask

#undef  VPICD_Physically_Unmask
#define VPICD_Physically_Unmask INIT_VPICD_Physically_Unmask

#undef  VPICD_Set_Auto_Masking
#define VPICD_Set_Auto_Masking INIT_VPICD_Set_Auto_Masking

#undef  VPICD_Set_Int_Request
#define VPICD_Set_Int_Request INIT_VPICD_Set_Int_Request

#undef  VPICD_Test_Phys_Request
#define VPICD_Test_Phys_Request INIT_VPICD_Test_Phys_Request

#undef  VPICD_Virtualize_IRQ
#define VPICD_Virtualize_IRQ INIT_VPICD_Virtualize_IRQ

#ifdef WIN40_OR_LATER
#undef  VPICD_Auto_Mask_At_Inst_Swap
#define VPICD_Auto_Mask_At_Inst_Swap INIT_VPICD_Auto_Mask_At_Inst_Swap
#endif

#undef  VSD_Get_Version
#define VSD_Get_Version INIT_VSD_Get_Version

#undef  VTD_Begin_Min_Int_Period
#define VTD_Begin_Min_Int_Period INIT_VTD_Begin_Min_Int_Period

#undef  VTD_Disable_Trapping
#define VTD_Disable_Trapping INIT_VTD_Disable_Trapping

#undef  VTD_Enable_Trapping
#define VTD_Enable_Trapping INIT_VTD_Enable_Trapping

#undef  VTD_End_Min_Int_Period
#define VTD_End_Min_Int_Period INIT_VTD_End_Min_Int_Period

#undef  VTD_Get_Interrupt_Period
#define VTD_Get_Interrupt_Period INIT_VTD_Get_Interrupt_Period

#undef  VTD_Get_Real_Time
#define VTD_Get_Real_Time INIT_VTD_Get_Real_Time

#undef  VTD_Get_Version
#define VTD_Get_Version INIT_VTD_Get_Version

#undef  VPICD_Thunk_HWInt
#define VPICD_Thunk_HWInt INIT_VPICD_Thunk_HWInt

#undef  VPICD_Thunk_VirtInt
#define VPICD_Thunk_VirtInt INIT_VPICD_Thunk_VirtInt

#undef  VPICD_Thunk_EOI
#define VPICD_Thunk_EOI INIT_VPICD_Thunk_EOI

#undef  VPICD_Thunk_MaskChange
#define VPICD_Thunk_MaskChange INIT_VPICD_Thunk_MaskChange

#undef  VPICD_Thunk_Iret
#define VPICD_Thunk_Iret INIT_VPICD_Thunk_Iret

#undef  Install_Exception_Handler
#define Install_Exception_Handler INIT_Install_Exception_Handler

#ifdef WFW311_OR_LATER
#undef  VXDLDR_GetDeviceList
#define VXDLDR_GetDeviceList INIT_VXDLDR_GetDeviceList
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_GetVersion
#define VXDLDR_GetVersion INIT_VXDLDR_GetVersion
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_LoadDevice
#define VXDLDR_LoadDevice INIT_VXDLDR_LoadDevice
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_UnloadDevice
#define VXDLDR_UnloadDevice INIT_VXDLDR_UnloadDevice
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_DevInitSucceeded
#define VXDLDR_DevInitSucceeded INIT_VXDLDR_DevInitSucceeded
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_DevInitFailed
#define VXDLDR_DevInitFailed INIT_VXDLDR_DevInitFailed
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_APM_BIOS_Version
#define _VPOWERD_Get_APM_BIOS_Version INIT__VPOWERD_Get_APM_BIOS_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_APM_BIOS_Version
#define VPOWERD_Get_APM_BIOS_Version INIT__VPOWERD_Get_APM_BIOS_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_State
#define _VPOWERD_Get_Power_State INIT__VPOWERD_Get_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_State
#define VPOWERD_Get_Power_State INIT__VPOWERD_Get_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_Management_Level
#define _VPOWERD_Get_Power_Management_Level INIT__VPOWERD_Get_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_Management_Level
#define VPOWERD_Get_Power_Management_Level INIT__VPOWERD_Get_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_Status
#define _VPOWERD_Get_Power_Status INIT__VPOWERD_Get_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_Status
#define VPOWERD_Get_Power_Status INIT__VPOWERD_Get_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_OEM_APM_Function
#define _VPOWERD_OEM_APM_Function INIT__VPOWERD_OEM_APM_Function
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_OEM_APM_Function
#define VPOWERD_OEM_APM_Function INIT__VPOWERD_OEM_APM_Function
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Register_Power_Handler
#define _VPOWERD_Register_Power_Handler INIT__VPOWERD_Register_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Register_Power_Handler
#define VPOWERD_Register_Power_Handler INIT__VPOWERD_Register_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Restore_Power_On_Defaults
#define _VPOWERD_Restore_Power_On_Defaults INIT__VPOWERD_Restore_Power_On_Defaults
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Restore_Power_On_Defaults
#define VPOWERD_Restore_Power_On_Defaults INIT__VPOWERD_Restore_Power_On_Defaults
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_Device_Power_State
#define _VPOWERD_Set_Device_Power_State INIT__VPOWERD_Set_Device_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_Device_Power_State
#define VPOWERD_Set_Device_Power_State INIT__VPOWERD_Set_Device_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_Power_Management_Level
#define _VPOWERD_Set_Power_Management_Level INIT__VPOWERD_Set_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_Power_Management_Level
#define VPOWERD_Set_Power_Management_Level INIT__VPOWERD_Set_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_System_Power_State
#define _VPOWERD_Set_System_Power_State INIT__VPOWERD_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_System_Power_State
#define VPOWERD_Set_System_Power_State INIT__VPOWERD_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Deregister_Power_Handler
#define _VPOWERD_Deregister_Power_Handler INIT__VPOWERD_Deregister_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Deregister_Power_Handler
#define VPOWERD_Deregister_Power_Handler INIT__VPOWERD_Deregister_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Version
#define _VPOWERD_Get_Version INIT__VPOWERD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Version
#define VPOWERD_Get_Version INIT__VPOWERD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_W32_Get_System_Power_Status
#define _VPOWERD_W32_Get_System_Power_Status INIT__VPOWERD_W32_Get_System_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_W32_Get_System_Power_Status
#define VPOWERD_W32_Get_System_Power_Status INIT__VPOWERD_W32_Get_System_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_W32_Set_System_Power_State
#define _VPOWERD_W32_Set_System_Power_State INIT__VPOWERD_W32_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_W32_Set_System_Power_State
#define VPOWERD_W32_Set_System_Power_State INIT__VPOWERD_W32_Set_System_Power_State
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Get_Version
#define VCOMM_Get_Version INIT_VCOMM_Get_Version
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Acquire_Port
#define _VCOMM_Acquire_Port INIT__VCOMM_Acquire_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Acquire_Port
#define VCOMM_Acquire_Port INIT__VCOMM_Acquire_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Add_Port
#define _VCOMM_Add_Port INIT__VCOMM_Add_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Add_Port
#define VCOMM_Add_Port INIT__VCOMM_Add_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Register_Port_Driver
#define _VCOMM_Register_Port_Driver INIT__VCOMM_Register_Port_Driver
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Register_Port_Driver
#define VCOMM_Register_Port_Driver INIT__VCOMM_Register_Port_Driver
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Release_Port
#define _VCOMM_Release_Port INIT__VCOMM_Release_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Release_Port
#define VCOMM_Release_Port INIT__VCOMM_Release_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_ClearCommError
#define _VCOMM_ClearCommError INIT__VCOMM_ClearCommError
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_ClearCommError
#define VCOMM_ClearCommError INIT__VCOMM_ClearCommError
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_CloseComm
#define _VCOMM_CloseComm INIT__VCOMM_CloseComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_CloseComm
#define VCOMM_CloseComm INIT__VCOMM_CloseComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_EnableCommNotification
#define _VCOMM_EnableCommNotification INIT__VCOMM_EnableCommNotification
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_EnableCommNotification
#define VCOMM_EnableCommNotification INIT__VCOMM_EnableCommNotification
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_EscapeCommFunction
#define _VCOMM_EscapeCommFunction INIT__VCOMM_EscapeCommFunction
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_EscapeCommFunction
#define VCOMM_EscapeCommFunction INIT__VCOMM_EscapeCommFunction
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommEventMask
#define _VCOMM_GetCommEventMask INIT__VCOMM_GetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommEventMask
#define VCOMM_GetCommEventMask INIT__VCOMM_GetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommProperties
#define _VCOMM_GetCommProperties INIT__VCOMM_GetCommProperties
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommProperties
#define VCOMM_GetCommProperties INIT__VCOMM_GetCommProperties
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommQueueStatus
#define _VCOMM_GetCommQueueStatus INIT__VCOMM_GetCommQueueStatus
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommQueueStatus
#define VCOMM_GetCommQueueStatus INIT__VCOMM_GetCommQueueStatus
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommState
#define _VCOMM_GetCommState INIT__VCOMM_GetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommState
#define VCOMM_GetCommState INIT__VCOMM_GetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetLastError
#define _VCOMM_GetLastError INIT__VCOMM_GetLastError
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetLastError
#define VCOMM_GetLastError INIT__VCOMM_GetLastError
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetModemStatus
#define _VCOMM_GetModemStatus INIT__VCOMM_GetModemStatus
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetModemStatus
#define VCOMM_GetModemStatus INIT__VCOMM_GetModemStatus
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_GetSetCommTimeouts
#define _VCOMM_GetSetCommTimeouts INIT__VCOMM_GetSetCommTimeouts
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_GetSetCommTimeouts
#define VCOMM_GetSetCommTimeouts INIT__VCOMM_GetSetCommTimeouts
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_OpenComm
#define _VCOMM_OpenComm INIT__VCOMM_OpenComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_OpenComm
#define VCOMM_OpenComm INIT__VCOMM_OpenComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_PurgeComm
#define _VCOMM_PurgeComm INIT__VCOMM_PurgeComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_PurgeComm
#define VCOMM_PurgeComm INIT__VCOMM_PurgeComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_ReadComm
#define _VCOMM_ReadComm INIT__VCOMM_ReadComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_ReadComm
#define VCOMM_ReadComm INIT__VCOMM_ReadComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetCommEventMask
#define _VCOMM_SetCommEventMask INIT__VCOMM_SetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetCommEventMask
#define VCOMM_SetCommEventMask INIT__VCOMM_SetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetCommState
#define _VCOMM_SetCommState INIT__VCOMM_SetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetCommState
#define VCOMM_SetCommState INIT__VCOMM_SetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetReadCallback
#define _VCOMM_SetReadCallback INIT__VCOMM_SetReadCallback
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetReadCallback
#define VCOMM_SetReadCallback INIT__VCOMM_SetReadCallback
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetupComm
#define _VCOMM_SetupComm INIT__VCOMM_SetupComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetupComm
#define VCOMM_SetupComm INIT__VCOMM_SetupComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetWriteCallback
#define _VCOMM_SetWriteCallback INIT__VCOMM_SetWriteCallback
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetWriteCallback
#define VCOMM_SetWriteCallback INIT__VCOMM_SetWriteCallback
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_TransmitCommChar
#define _VCOMM_TransmitCommChar INIT__VCOMM_TransmitCommChar
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_TransmitCommChar
#define VCOMM_TransmitCommChar INIT__VCOMM_TransmitCommChar
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_WriteComm
#define _VCOMM_WriteComm INIT__VCOMM_WriteComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_WriteComm
#define VCOMM_WriteComm INIT__VCOMM_WriteComm
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Win32DCB_To_Ring0
#define VCOMM_Map_Win32DCB_To_Ring0 INIT_VCOMM_Map_Win32DCB_To_Ring0
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Ring0DCB_To_Win32
#define VCOMM_Map_Ring0DCB_To_Win32 INIT_VCOMM_Map_Ring0DCB_To_Win32
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Get_Contention_Handler
#define _VCOMM_Get_Contention_Handler INIT__VCOMM_Get_Contention_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Get_Contention_Handler
#define VCOMM_Get_Contention_Handler INIT__VCOMM_Get_Contention_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Map_Name_To_Resource
#define _VCOMM_Map_Name_To_Resource INIT__VCOMM_Map_Name_To_Resource
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Name_To_Resource
#define VCOMM_Map_Name_To_Resource INIT__VCOMM_Map_Name_To_Resource
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Version
#define _CONFIGMG_Get_Version INIT__CONFIGMG_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Version
#define CONFIGMG_Get_Version INIT__CONFIGMG_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Initialize
#define _CONFIGMG_Initialize INIT__CONFIGMG_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Initialize
#define CONFIGMG_Initialize INIT__CONFIGMG_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Locate_DevNode
#define _CONFIGMG_Locate_DevNode INIT__CONFIGMG_Locate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Locate_DevNode
#define CONFIGMG_Locate_DevNode INIT__CONFIGMG_Locate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Parent
#define _CONFIGMG_Get_Parent INIT__CONFIGMG_Get_Parent
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Parent
#define CONFIGMG_Get_Parent INIT__CONFIGMG_Get_Parent
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Child
#define _CONFIGMG_Get_Child INIT__CONFIGMG_Get_Child
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Child
#define CONFIGMG_Get_Child INIT__CONFIGMG_Get_Child
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Sibling
#define _CONFIGMG_Get_Sibling INIT__CONFIGMG_Get_Sibling
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Sibling
#define CONFIGMG_Get_Sibling INIT__CONFIGMG_Get_Sibling
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_ID_Size
#define _CONFIGMG_Get_Device_ID_Size INIT__CONFIGMG_Get_Device_ID_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_ID_Size
#define CONFIGMG_Get_Device_ID_Size INIT__CONFIGMG_Get_Device_ID_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_ID
#define _CONFIGMG_Get_Device_ID INIT__CONFIGMG_Get_Device_ID
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_ID
#define CONFIGMG_Get_Device_ID INIT__CONFIGMG_Get_Device_ID
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Depth
#define _CONFIGMG_Get_Depth INIT__CONFIGMG_Get_Depth
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Depth
#define CONFIGMG_Get_Depth INIT__CONFIGMG_Get_Depth
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Private_DWord
#define _CONFIGMG_Get_Private_DWord INIT__CONFIGMG_Get_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Private_DWord
#define CONFIGMG_Get_Private_DWord INIT__CONFIGMG_Get_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Private_DWord
#define _CONFIGMG_Set_Private_DWord INIT__CONFIGMG_Set_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Private_DWord
#define CONFIGMG_Set_Private_DWord INIT__CONFIGMG_Set_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Create_DevNode
#define _CONFIGMG_Create_DevNode INIT__CONFIGMG_Create_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Create_DevNode
#define CONFIGMG_Create_DevNode INIT__CONFIGMG_Create_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Remove_SubTree
#define _CONFIGMG_Query_Remove_SubTree INIT__CONFIGMG_Query_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Remove_SubTree
#define CONFIGMG_Query_Remove_SubTree INIT__CONFIGMG_Query_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Remove_SubTree
#define _CONFIGMG_Remove_SubTree INIT__CONFIGMG_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Remove_SubTree
#define CONFIGMG_Remove_SubTree INIT__CONFIGMG_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Device_Driver
#define _CONFIGMG_Register_Device_Driver INIT__CONFIGMG_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Device_Driver
#define CONFIGMG_Register_Device_Driver INIT__CONFIGMG_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Enumerator
#define _CONFIGMG_Register_Enumerator INIT__CONFIGMG_Register_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Enumerator
#define CONFIGMG_Register_Enumerator INIT__CONFIGMG_Register_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Arbitrator
#define _CONFIGMG_Register_Arbitrator INIT__CONFIGMG_Register_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Arbitrator
#define CONFIGMG_Register_Arbitrator INIT__CONFIGMG_Register_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Deregister_Arbitrator
#define _CONFIGMG_Deregister_Arbitrator INIT__CONFIGMG_Deregister_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Deregister_Arbitrator
#define CONFIGMG_Deregister_Arbitrator INIT__CONFIGMG_Deregister_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Res_Des
#define _CONFIGMG_Add_Res_Des INIT__CONFIGMG_Add_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Res_Des
#define CONFIGMG_Add_Res_Des INIT__CONFIGMG_Add_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Modify_Res_Des
#define _CONFIGMG_Modify_Res_Des INIT__CONFIGMG_Modify_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Modify_Res_Des
#define CONFIGMG_Modify_Res_Des INIT__CONFIGMG_Modify_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Res_Des
#define _CONFIGMG_Free_Res_Des INIT__CONFIGMG_Free_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Res_Des
#define CONFIGMG_Free_Res_Des INIT__CONFIGMG_Free_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Next_Res_Des
#define _CONFIGMG_Get_Next_Res_Des INIT__CONFIGMG_Get_Next_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Next_Res_Des
#define CONFIGMG_Get_Next_Res_Des INIT__CONFIGMG_Get_Next_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Performance_Info
#define _CONFIGMG_Get_Performance_Info INIT__CONFIGMG_Get_Performance_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Performance_Info
#define CONFIGMG_Get_Performance_Info INIT__CONFIGMG_Get_Performance_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Res_Des_Data_Size
#define _CONFIGMG_Get_Res_Des_Data_Size INIT__CONFIGMG_Get_Res_Des_Data_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Res_Des_Data_Size
#define CONFIGMG_Get_Res_Des_Data_Size INIT__CONFIGMG_Get_Res_Des_Data_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Res_Des_Data
#define _CONFIGMG_Get_Res_Des_Data INIT__CONFIGMG_Get_Res_Des_Data
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Res_Des_Data
#define CONFIGMG_Get_Res_Des_Data INIT__CONFIGMG_Get_Res_Des_Data
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Process_Events_Now
#define _CONFIGMG_Process_Events_Now INIT__CONFIGMG_Process_Events_Now
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Process_Events_Now
#define CONFIGMG_Process_Events_Now INIT__CONFIGMG_Process_Events_Now
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Create_Range_List
#define _CONFIGMG_Create_Range_List INIT__CONFIGMG_Create_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Create_Range_List
#define CONFIGMG_Create_Range_List INIT__CONFIGMG_Create_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Range
#define _CONFIGMG_Add_Range INIT__CONFIGMG_Add_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Range
#define CONFIGMG_Add_Range INIT__CONFIGMG_Add_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Delete_Range
#define _CONFIGMG_Delete_Range INIT__CONFIGMG_Delete_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Delete_Range
#define CONFIGMG_Delete_Range INIT__CONFIGMG_Delete_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Test_Range_Available
#define _CONFIGMG_Test_Range_Available INIT__CONFIGMG_Test_Range_Available
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Test_Range_Available
#define CONFIGMG_Test_Range_Available INIT__CONFIGMG_Test_Range_Available
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Dup_Range_List
#define _CONFIGMG_Dup_Range_List INIT__CONFIGMG_Dup_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Dup_Range_List
#define CONFIGMG_Dup_Range_List INIT__CONFIGMG_Dup_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Range_List
#define _CONFIGMG_Free_Range_List INIT__CONFIGMG_Free_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Range_List
#define CONFIGMG_Free_Range_List INIT__CONFIGMG_Free_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Invert_Range_List
#define _CONFIGMG_Invert_Range_List INIT__CONFIGMG_Invert_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Invert_Range_List
#define CONFIGMG_Invert_Range_List INIT__CONFIGMG_Invert_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Intersect_Range_List
#define _CONFIGMG_Intersect_Range_List INIT__CONFIGMG_Intersect_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Intersect_Range_List
#define CONFIGMG_Intersect_Range_List INIT__CONFIGMG_Intersect_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_First_Range
#define _CONFIGMG_First_Range INIT__CONFIGMG_First_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_First_Range
#define CONFIGMG_First_Range INIT__CONFIGMG_First_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Next_Range
#define _CONFIGMG_Next_Range INIT__CONFIGMG_Next_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Next_Range
#define CONFIGMG_Next_Range INIT__CONFIGMG_Next_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Dump_Range_List
#define _CONFIGMG_Dump_Range_List INIT__CONFIGMG_Dump_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Dump_Range_List
#define CONFIGMG_Dump_Range_List INIT__CONFIGMG_Dump_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Load_DLVxDs
#define _CONFIGMG_Load_DLVxDs INIT__CONFIGMG_Load_DLVxDs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Load_DLVxDs
#define CONFIGMG_Load_DLVxDs INIT__CONFIGMG_Load_DLVxDs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DDBs
#define _CONFIGMG_Get_DDBs INIT__CONFIGMG_Get_DDBs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DDBs
#define CONFIGMG_Get_DDBs INIT__CONFIGMG_Get_DDBs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_CRC_CheckSum
#define _CONFIGMG_Get_CRC_CheckSum INIT__CONFIGMG_Get_CRC_CheckSum
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_CRC_CheckSum
#define CONFIGMG_Get_CRC_CheckSum INIT__CONFIGMG_Get_CRC_CheckSum
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_DevLoader
#define _CONFIGMG_Register_DevLoader INIT__CONFIGMG_Register_DevLoader
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_DevLoader
#define CONFIGMG_Register_DevLoader INIT__CONFIGMG_Register_DevLoader
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Reenumerate_DevNode
#define _CONFIGMG_Reenumerate_DevNode INIT__CONFIGMG_Reenumerate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Reenumerate_DevNode
#define CONFIGMG_Reenumerate_DevNode INIT__CONFIGMG_Reenumerate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Setup_DevNode
#define _CONFIGMG_Setup_DevNode INIT__CONFIGMG_Setup_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Setup_DevNode
#define CONFIGMG_Setup_DevNode INIT__CONFIGMG_Setup_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Reset_Children_Marks
#define _CONFIGMG_Reset_Children_Marks INIT__CONFIGMG_Reset_Children_Marks
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Reset_Children_Marks
#define CONFIGMG_Reset_Children_Marks INIT__CONFIGMG_Reset_Children_Marks
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Status
#define _CONFIGMG_Get_DevNode_Status INIT__CONFIGMG_Get_DevNode_Status
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Status
#define CONFIGMG_Get_DevNode_Status INIT__CONFIGMG_Get_DevNode_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Remove_Unmarked_Children
#define _CONFIGMG_Remove_Unmarked_Children INIT__CONFIGMG_Remove_Unmarked_Children
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Remove_Unmarked_Children
#define CONFIGMG_Remove_Unmarked_Children INIT__CONFIGMG_Remove_Unmarked_Children
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_ISAPNP_To_CM
#define _CONFIGMG_ISAPNP_To_CM INIT__CONFIGMG_ISAPNP_To_CM
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_ISAPNP_To_CM
#define CONFIGMG_ISAPNP_To_CM INIT__CONFIGMG_ISAPNP_To_CM
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_CallBack_Device_Driver
#define _CONFIGMG_CallBack_Device_Driver INIT__CONFIGMG_CallBack_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_CallBack_Device_Driver
#define CONFIGMG_CallBack_Device_Driver INIT__CONFIGMG_CallBack_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_CallBack_Enumerator
#define _CONFIGMG_CallBack_Enumerator INIT__CONFIGMG_CallBack_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_CallBack_Enumerator
#define CONFIGMG_CallBack_Enumerator INIT__CONFIGMG_CallBack_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Alloc_Log_Conf
#define _CONFIGMG_Get_Alloc_Log_Conf INIT__CONFIGMG_Get_Alloc_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Alloc_Log_Conf
#define CONFIGMG_Get_Alloc_Log_Conf INIT__CONFIGMG_Get_Alloc_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Key_Size
#define _CONFIGMG_Get_DevNode_Key_Size INIT__CONFIGMG_Get_DevNode_Key_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Key_Size
#define CONFIGMG_Get_DevNode_Key_Size INIT__CONFIGMG_Get_DevNode_Key_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Key
#define _CONFIGMG_Get_DevNode_Key INIT__CONFIGMG_Get_DevNode_Key
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Key
#define CONFIGMG_Get_DevNode_Key INIT__CONFIGMG_Get_DevNode_Key
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Read_Registry_Value
#define _CONFIGMG_Read_Registry_Value INIT__CONFIGMG_Read_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Read_Registry_Value
#define CONFIGMG_Read_Registry_Value INIT__CONFIGMG_Read_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Write_Registry_Value
#define _CONFIGMG_Write_Registry_Value INIT__CONFIGMG_Write_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Write_Registry_Value
#define CONFIGMG_Write_Registry_Value INIT__CONFIGMG_Write_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Disable_DevNode
#define _CONFIGMG_Disable_DevNode INIT__CONFIGMG_Disable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Disable_DevNode
#define CONFIGMG_Disable_DevNode INIT__CONFIGMG_Disable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Enable_DevNode
#define _CONFIGMG_Enable_DevNode INIT__CONFIGMG_Enable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Enable_DevNode
#define CONFIGMG_Enable_DevNode INIT__CONFIGMG_Enable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Move_DevNode
#define _CONFIGMG_Move_DevNode INIT__CONFIGMG_Move_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Move_DevNode
#define CONFIGMG_Move_DevNode INIT__CONFIGMG_Move_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Bus_Info
#define _CONFIGMG_Set_Bus_Info INIT__CONFIGMG_Set_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Bus_Info
#define CONFIGMG_Set_Bus_Info INIT__CONFIGMG_Set_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Bus_Info
#define _CONFIGMG_Get_Bus_Info INIT__CONFIGMG_Get_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Bus_Info
#define CONFIGMG_Get_Bus_Info INIT__CONFIGMG_Get_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_HW_Prof
#define _CONFIGMG_Set_HW_Prof INIT__CONFIGMG_Set_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_HW_Prof
#define CONFIGMG_Set_HW_Prof INIT__CONFIGMG_Set_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Recompute_HW_Prof
#define _CONFIGMG_Recompute_HW_Prof INIT__CONFIGMG_Recompute_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Recompute_HW_Prof
#define CONFIGMG_Recompute_HW_Prof INIT__CONFIGMG_Recompute_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Change_HW_Prof
#define _CONFIGMG_Query_Change_HW_Prof INIT__CONFIGMG_Query_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Change_HW_Prof
#define CONFIGMG_Query_Change_HW_Prof INIT__CONFIGMG_Query_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_Driver_Private_DWord
#define _CONFIGMG_Get_Device_Driver_Private_DWord INIT__CONFIGMG_Get_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_Driver_Private_DWord
#define CONFIGMG_Get_Device_Driver_Private_DWord INIT__CONFIGMG_Get_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Device_Driver_Private_DWord
#define _CONFIGMG_Set_Device_Driver_Private_DWord INIT__CONFIGMG_Set_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Device_Driver_Private_DWord
#define CONFIGMG_Set_Device_Driver_Private_DWord INIT__CONFIGMG_Set_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_HW_Prof_Flags
#define _CONFIGMG_Get_HW_Prof_Flags INIT__CONFIGMG_Get_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_HW_Prof_Flags
#define CONFIGMG_Get_HW_Prof_Flags INIT__CONFIGMG_Get_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_HW_Prof_Flags
#define _CONFIGMG_Set_HW_Prof_Flags INIT__CONFIGMG_Set_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_HW_Prof_Flags
#define CONFIGMG_Set_HW_Prof_Flags INIT__CONFIGMG_Set_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Read_Registry_Log_Confs
#define _CONFIGMG_Read_Registry_Log_Confs INIT__CONFIGMG_Read_Registry_Log_Confs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Read_Registry_Log_Confs
#define CONFIGMG_Read_Registry_Log_Confs INIT__CONFIGMG_Read_Registry_Log_Confs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Run_Detection
#define _CONFIGMG_Run_Detection INIT__CONFIGMG_Run_Detection
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Run_Detection
#define CONFIGMG_Run_Detection INIT__CONFIGMG_Run_Detection
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Call_At_Appy_Time
#define _CONFIGMG_Call_At_Appy_Time INIT__CONFIGMG_Call_At_Appy_Time
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Call_At_Appy_Time
#define CONFIGMG_Call_At_Appy_Time INIT__CONFIGMG_Call_At_Appy_Time
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Private_Problem
#define _CONFIGMG_Set_Private_Problem INIT__CONFIGMG_Set_Private_Problem
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Private_Problem
#define CONFIGMG_Set_Private_Problem INIT__CONFIGMG_Set_Private_Problem
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Hardware_Profile_Info
#define _CONFIGMG_Get_Hardware_Profile_Info INIT__CONFIGMG_Get_Hardware_Profile_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Hardware_Profile_Info
#define CONFIGMG_Get_Hardware_Profile_Info INIT__CONFIGMG_Get_Hardware_Profile_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Arbitrator_Free_Size
#define _CONFIGMG_Query_Arbitrator_Free_Size INIT__CONFIGMG_Query_Arbitrator_Free_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Arbitrator_Free_Size
#define CONFIGMG_Query_Arbitrator_Free_Size INIT__CONFIGMG_Query_Arbitrator_Free_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Arbitrator_Free_Data
#define _CONFIGMG_Query_Arbitrator_Free_Data INIT__CONFIGMG_Query_Arbitrator_Free_Data
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Arbitrator_Free_Data
#define CONFIGMG_Query_Arbitrator_Free_Data INIT__CONFIGMG_Query_Arbitrator_Free_Data
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Sort_NodeList
#define _CONFIGMG_Sort_NodeList INIT__CONFIGMG_Sort_NodeList
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Sort_NodeList
#define CONFIGMG_Sort_NodeList INIT__CONFIGMG_Sort_NodeList
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Yield
#define _CONFIGMG_Yield INIT__CONFIGMG_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Yield
#define CONFIGMG_Yield INIT__CONFIGMG_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Lock
#define _CONFIGMG_Lock INIT__CONFIGMG_Lock
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Lock
#define CONFIGMG_Lock INIT__CONFIGMG_Lock
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Unlock
#define _CONFIGMG_Unlock INIT__CONFIGMG_Unlock
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Unlock
#define CONFIGMG_Unlock INIT__CONFIGMG_Unlock
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Empty_Log_Conf
#define _CONFIGMG_Add_Empty_Log_Conf INIT__CONFIGMG_Add_Empty_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Empty_Log_Conf
#define CONFIGMG_Add_Empty_Log_Conf INIT__CONFIGMG_Add_Empty_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Log_Conf
#define _CONFIGMG_Free_Log_Conf INIT__CONFIGMG_Free_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Log_Conf
#define CONFIGMG_Free_Log_Conf INIT__CONFIGMG_Free_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_First_Log_Conf
#define _CONFIGMG_Get_First_Log_Conf INIT__CONFIGMG_Get_First_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_First_Log_Conf
#define CONFIGMG_Get_First_Log_Conf INIT__CONFIGMG_Get_First_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Next_Log_Conf
#define _CONFIGMG_Get_Next_Log_Conf INIT__CONFIGMG_Get_Next_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Next_Log_Conf
#define CONFIGMG_Get_Next_Log_Conf INIT__CONFIGMG_Get_Next_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Fail_Change_HW_Prof
#define _CONFIGMG_Fail_Change_HW_Prof INIT__CONFIGMG_Fail_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Fail_Change_HW_Prof
#define CONFIGMG_Fail_Change_HW_Prof INIT__CONFIGMG_Fail_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Call_Enumerator_Function
#define _CONFIGMG_Call_Enumerator_Function INIT__CONFIGMG_Call_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Call_Enumerator_Function
#define CONFIGMG_Call_Enumerator_Function INIT__CONFIGMG_Call_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Post_Pointer_Message
#define VMD_Post_Pointer_Message INIT_VMD_Post_Pointer_Message
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Set_Cursor_Proc
#define VMD_Set_Cursor_Proc INIT_VMD_Set_Cursor_Proc
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Call_Cursor_Proc
#define VMD_Call_Cursor_Proc INIT_VMD_Call_Cursor_Proc
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Set_Mouse_Data
#define VMD_Set_Mouse_Data INIT_VMD_Set_Mouse_Data
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Get_Mouse_Data
#define VMD_Get_Mouse_Data INIT_VMD_Get_Mouse_Data
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Post_Absolute_Pointer_Message
#define VMD_Post_Absolute_Pointer_Message INIT_VMD_Post_Absolute_Pointer_Message
#endif

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_Version
#define IFSMgr_Get_Version INIT_IFSMgr_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterMount
#define IFSMgr_RegisterMount INIT_IFSMgr_RegisterMount
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterNet
#define IFSMgr_RegisterNet INIT_IFSMgr_RegisterNet
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterMailSlot
#define IFSMgr_RegisterMailSlot INIT_IFSMgr_RegisterMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_NetTime
#define IFSMgr_Get_NetTime INIT_IFSMgr_Get_NetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_DOSTime
#define IFSMgr_Get_DOSTime INIT_IFSMgr_Get_DOSTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetupConnection
#define IFSMgr_SetupConnection INIT_IFSMgr_SetupConnection
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DerefConnection
#define IFSMgr_DerefConnection INIT_IFSMgr_DerefConnection
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ServerDOSCall
#define IFSMgr_ServerDOSCall INIT_IFSMgr_ServerDOSCall
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CompleteAsync
#define IFSMgr_CompleteAsync INIT_IFSMgr_CompleteAsync
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterHeap
#define IFSMgr_RegisterHeap INIT_IFSMgr_RegisterHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetHeap
#define IFSMgr_GetHeap INIT_IFSMgr_GetHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RetHeap
#define IFSMgr_RetHeap INIT_IFSMgr_RetHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckHeap
#define IFSMgr_CheckHeap INIT_IFSMgr_CheckHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckHeapItem
#define IFSMgr_CheckHeapItem INIT_IFSMgr_CheckHeapItem
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FillHeapSpare
#define IFSMgr_FillHeapSpare INIT_IFSMgr_FillHeapSpare
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Block
#define IFSMgr_Block INIT_IFSMgr_Block
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Wakeup
#define IFSMgr_Wakeup INIT_IFSMgr_Wakeup
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Yield
#define IFSMgr_Yield INIT_IFSMgr_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SchedEvent
#define IFSMgr_SchedEvent INIT_IFSMgr_SchedEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_QueueEvent
#define IFSMgr_QueueEvent INIT_IFSMgr_QueueEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_KillEvent
#define IFSMgr_KillEvent INIT_IFSMgr_KillEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FreeIOReq
#define IFSMgr_FreeIOReq INIT_IFSMgr_FreeIOReq
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MakeMailSlot
#define IFSMgr_MakeMailSlot INIT_IFSMgr_MakeMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DeleteMailSlot
#define IFSMgr_DeleteMailSlot INIT_IFSMgr_DeleteMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_WriteMailSlot
#define IFSMgr_WriteMailSlot INIT_IFSMgr_WriteMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_PopUp
#define IFSMgr_PopUp INIT_IFSMgr_PopUp
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_printf
#define IFSMgr_printf INIT_IFSMgr_printf
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_AssertFailed
#define IFSMgr_AssertFailed INIT_IFSMgr_AssertFailed
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_LogEntry
#define IFSMgr_LogEntry INIT_IFSMgr_LogEntry
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DebugMenu
#define IFSMgr_DebugMenu INIT_IFSMgr_DebugMenu
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DebugVars
#define IFSMgr_DebugVars INIT_IFSMgr_DebugVars
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetDebugString
#define IFSMgr_GetDebugString INIT_IFSMgr_GetDebugString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetDebugHexNum
#define IFSMgr_GetDebugHexNum INIT_IFSMgr_GetDebugHexNum
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetFunction
#define IFSMgr_NetFunction INIT_IFSMgr_NetFunction
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DoDelAllUses
#define IFSMgr_DoDelAllUses INIT_IFSMgr_DoDelAllUses
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetErrString
#define IFSMgr_SetErrString INIT_IFSMgr_SetErrString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetErrString
#define IFSMgr_GetErrString INIT_IFSMgr_GetErrString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetReqHook
#define IFSMgr_SetReqHook INIT_IFSMgr_SetReqHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetPathHook
#define IFSMgr_SetPathHook INIT_IFSMgr_SetPathHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UseAdd
#define IFSMgr_UseAdd INIT_IFSMgr_UseAdd
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UseDel
#define IFSMgr_UseDel INIT_IFSMgr_UseDel
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_InitUseAdd
#define IFSMgr_InitUseAdd INIT_IFSMgr_InitUseAdd
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ChangeDir
#define IFSMgr_ChangeDir INIT_IFSMgr_ChangeDir
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DelAllUses
#define IFSMgr_DelAllUses INIT_IFSMgr_DelAllUses
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CDROM_Attach
#define IFSMgr_CDROM_Attach INIT_IFSMgr_CDROM_Attach
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CDROM_Detach
#define IFSMgr_CDROM_Detach INIT_IFSMgr_CDROM_Detach
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32DupHandle
#define IFSMgr_Win32DupHandle INIT_IFSMgr_Win32DupHandle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Ring0_FileIO
#define IFSMgr_Ring0_FileIO INIT_IFSMgr_Ring0_FileIO
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32_Get_Ring0_Handle
#define IFSMgr_Win32_Get_Ring0_Handle INIT_IFSMgr_Win32_Get_Ring0_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_Drive_Info
#define IFSMgr_Get_Drive_Info INIT_IFSMgr_Get_Drive_Info
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Ring0GetDriveInfo
#define IFSMgr_Ring0GetDriveInfo INIT_IFSMgr_Ring0GetDriveInfo
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_BlockNoEvents
#define IFSMgr_BlockNoEvents INIT_IFSMgr_BlockNoEvents
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetToDosTime
#define IFSMgr_NetToDosTime INIT_IFSMgr_NetToDosTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DosToNetTime
#define IFSMgr_DosToNetTime INIT_IFSMgr_DosToNetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DosToWin32Time
#define IFSMgr_DosToWin32Time INIT_IFSMgr_DosToWin32Time
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32ToDosTime
#define IFSMgr_Win32ToDosTime INIT_IFSMgr_Win32ToDosTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetToWin32Time
#define IFSMgr_NetToWin32Time INIT_IFSMgr_NetToWin32Time
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32ToNetTime
#define IFSMgr_Win32ToNetTime INIT_IFSMgr_Win32ToNetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MetaMatch
#define IFSMgr_MetaMatch INIT_IFSMgr_MetaMatch
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_TransMatch
#define IFSMgr_TransMatch INIT_IFSMgr_TransMatch
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CallProvider
#define IFSMgr_CallProvider INIT_IFSMgr_CallProvider
#endif

#ifdef WIN40_OR_LATER
#undef  UniToBCS
#define UniToBCS INIT_UniToBCS
#endif

#ifdef WIN40_OR_LATER
#undef  UniToBCSPath
#define UniToBCSPath INIT_UniToBCSPath
#endif

#ifdef WIN40_OR_LATER
#undef  BCSToUni
#define BCSToUni INIT_BCSToUni
#endif

#ifdef WIN40_OR_LATER
#undef  UniToUpper
#define UniToUpper INIT_UniToUpper
#endif

#ifdef WIN40_OR_LATER
#undef  UniCharToOEM
#define UniCharToOEM INIT_UniCharToOEM
#endif

#ifdef WIN40_OR_LATER
#undef  CreateBasis
#define CreateBasis INIT_CreateBasis
#endif

#ifdef WIN40_OR_LATER
#undef  MatchBasisName
#define MatchBasisName INIT_MatchBasisName
#endif

#ifdef WIN40_OR_LATER
#undef  AppendBasisTail
#define AppendBasisTail INIT_AppendBasisTail
#endif

#ifdef WIN40_OR_LATER
#undef  FcbToShort
#define FcbToShort INIT_FcbToShort
#endif

#ifdef WIN40_OR_LATER
#undef  ShortToFcb
#define ShortToFcb INIT_ShortToFcb
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ParsePath
#define IFSMgr_ParsePath INIT_IFSMgr_ParsePath
#endif

#ifdef WIN40_OR_LATER
#undef  Query_PhysLock
#define Query_PhysLock INIT_Query_PhysLock
#endif

#ifdef WIN40_OR_LATER
#undef  _VolFlush
#define _VolFlush INIT__VolFlush
#endif

#ifdef WIN40_OR_LATER
#undef  VolFlush
#define VolFlush INIT__VolFlush
#endif

#ifdef WIN40_OR_LATER
#undef  NotifyVolumeArrival
#define NotifyVolumeArrival INIT_NotifyVolumeArrival
#endif

#ifdef WIN40_OR_LATER
#undef  NotifyVolumeRemoval
#define NotifyVolumeRemoval INIT_NotifyVolumeRemoval
#endif

#ifdef WIN40_OR_LATER
#undef  QueryVolumeRemoval
#define QueryVolumeRemoval INIT_QueryVolumeRemoval
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDUnmountCFSD
#define IFSMgr_FSDUnmountCFSD INIT_IFSMgr_FSDUnmountCFSD
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetConversionTablePtrs
#define IFSMgr_GetConversionTablePtrs INIT_IFSMgr_GetConversionTablePtrs
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckAccessConflict
#define IFSMgr_CheckAccessConflict INIT_IFSMgr_CheckAccessConflict
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_LockFile
#define IFSMgr_LockFile INIT_IFSMgr_LockFile
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnlockFile
#define IFSMgr_UnlockFile INIT_IFSMgr_UnlockFile
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RemoveLocks
#define IFSMgr_RemoveLocks INIT_IFSMgr_RemoveLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckLocks
#define IFSMgr_CheckLocks INIT_IFSMgr_CheckLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CountLocks
#define IFSMgr_CountLocks INIT_IFSMgr_CountLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ReassignLockFileInst
#define IFSMgr_ReassignLockFileInst INIT_IFSMgr_ReassignLockFileInst
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnassignLockList
#define IFSMgr_UnassignLockList INIT_IFSMgr_UnassignLockList
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MountChildVolume
#define IFSMgr_MountChildVolume INIT_IFSMgr_MountChildVolume
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnmountChildVolume
#define IFSMgr_UnmountChildVolume INIT_IFSMgr_UnmountChildVolume
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SwapDrives
#define IFSMgr_SwapDrives INIT_IFSMgr_SwapDrives
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDMapFHtoIOREQ
#define IFSMgr_FSDMapFHtoIOREQ INIT_IFSMgr_FSDMapFHtoIOREQ
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDParsePath
#define IFSMgr_FSDParsePath INIT_IFSMgr_FSDParsePath
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDAttachSFT
#define IFSMgr_FSDAttachSFT INIT_IFSMgr_FSDAttachSFT
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetTimeZoneBias
#define IFSMgr_GetTimeZoneBias INIT_IFSMgr_GetTimeZoneBias
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_PNPEvent
#define IFSMgr_PNPEvent INIT_IFSMgr_PNPEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterCFSD
#define IFSMgr_RegisterCFSD INIT_IFSMgr_RegisterCFSD
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32MapExtendedHandleToSFT
#define IFSMgr_Win32MapExtendedHandleToSFT INIT_IFSMgr_Win32MapExtendedHandleToSFT
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DbgSetFileHandleLimit
#define IFSMgr_DbgSetFileHandleLimit INIT_IFSMgr_DbgSetFileHandleLimit
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32MapSFTToExtendedHandle
#define IFSMgr_Win32MapSFTToExtendedHandle INIT_IFSMgr_Win32MapSFTToExtendedHandle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDGetCurrentDrive
#define IFSMgr_FSDGetCurrentDrive INIT_IFSMgr_FSDGetCurrentDrive
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_InstallFileSystemApiHook
#define IFSMgr_InstallFileSystemApiHook INIT_IFSMgr_InstallFileSystemApiHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RemoveFileSystemApiHook
#define IFSMgr_RemoveFileSystemApiHook INIT_IFSMgr_RemoveFileSystemApiHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RunScheduledEvents
#define IFSMgr_RunScheduledEvents INIT_IFSMgr_RunScheduledEvents
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckDelResource
#define IFSMgr_CheckDelResource INIT_IFSMgr_CheckDelResource
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32GetVMCurdir
#define IFSMgr_Win32GetVMCurdir INIT_IFSMgr_Win32GetVMCurdir
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetupFailedConnection
#define IFSMgr_SetupFailedConnection INIT_IFSMgr_SetupFailedConnection
#endif

#ifdef WIN40_OR_LATER
#undef  _GetMappedErr
#define _GetMappedErr INIT__GetMappedErr
#endif

#ifdef WIN40_OR_LATER
#undef  GetMappedErr
#define GetMappedErr INIT__GetMappedErr
#endif

#ifdef WIN40_OR_LATER
#undef  ShortToLossyFcb
#define ShortToLossyFcb INIT_ShortToLossyFcb
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetLockState
#define IFSMgr_GetLockState INIT_IFSMgr_GetLockState
#endif

#ifdef WIN40_OR_LATER
#undef  BcsToBcs
#define BcsToBcs INIT_BcsToBcs
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetLoopback
#define IFSMgr_SetLoopback INIT_IFSMgr_SetLoopback
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ClearLoopback
#define IFSMgr_ClearLoopback INIT_IFSMgr_ClearLoopback
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ParseOneElement
#define IFSMgr_ParseOneElement INIT_IFSMgr_ParseOneElement
#endif

#ifdef WIN40_OR_LATER
#undef  BcsToBcsUpper
#define BcsToBcsUpper INIT_BcsToBcsUpper
#endif

#ifdef WIN40_OR_LATER
#undef  R0_OpenCreateFile
#define R0_OpenCreateFile INIT_R0_OpenCreateFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_ReadFile
#define R0_ReadFile INIT_R0_ReadFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_WriteFile
#define R0_WriteFile INIT_R0_WriteFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_CloseFile
#define R0_CloseFile INIT_R0_CloseFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetFileSize
#define R0_GetFileSize INIT_R0_GetFileSize
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindFirstFile
#define R0_FindFirstFile INIT_R0_FindFirstFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindNextFile
#define R0_FindNextFile INIT_R0_FindNextFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindCloseFile
#define R0_FindCloseFile INIT_R0_FindCloseFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetFileAttributes
#define R0_GetFileAttributes INIT_R0_GetFileAttributes
#endif

#ifdef WIN40_OR_LATER
#undef  R0_SetFileAttributes
#define R0_SetFileAttributes INIT_R0_SetFileAttributes
#endif

#ifdef WIN40_OR_LATER
#undef  R0_RenameFile
#define R0_RenameFile INIT_R0_RenameFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_DeleteFile
#define R0_DeleteFile INIT_R0_DeleteFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_LockFile
#define R0_LockFile INIT_R0_LockFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_UnlockFile
#define R0_UnlockFile INIT_R0_UnlockFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetDiskFreeSpace
#define R0_GetDiskFreeSpace INIT_R0_GetDiskFreeSpace
#endif

#ifdef WIN40_OR_LATER
#undef  R0_ReadAbsoluteDisk
#define R0_ReadAbsoluteDisk INIT_R0_ReadAbsoluteDisk
#endif

#ifdef WIN40_OR_LATER
#undef  R0_WriteAbsoluteDisk
#define R0_WriteAbsoluteDisk INIT_R0_WriteAbsoluteDisk
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Get_Version
#define VWIN32_Get_Version INIT_VWIN32_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_DIOCCompletionRoutine
#define VWIN32_DIOCCompletionRoutine INIT_VWIN32_DIOCCompletionRoutine
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_QueueUserApc
#define _VWIN32_QueueUserApc INIT__VWIN32_QueueUserApc
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_QueueUserApc
#define VWIN32_QueueUserApc INIT__VWIN32_QueueUserApc
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Get_Thread_Context
#define _VWIN32_Get_Thread_Context INIT__VWIN32_Get_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Get_Thread_Context
#define VWIN32_Get_Thread_Context INIT__VWIN32_Get_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Emulate_Npx
#define _VWIN32_Emulate_Npx INIT__VWIN32_Emulate_Npx
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Emulate_Npx
#define VWIN32_Emulate_Npx INIT__VWIN32_Emulate_Npx
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CheckDelayedNpxTrap
#define _VWIN32_CheckDelayedNpxTrap INIT__VWIN32_CheckDelayedNpxTrap
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CheckDelayedNpxTrap
#define VWIN32_CheckDelayedNpxTrap INIT__VWIN32_CheckDelayedNpxTrap
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_LeaveCrstR0
#define VWIN32_LeaveCrstR0 INIT_VWIN32_LeaveCrstR0
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_FaultPopup
#define _VWIN32_FaultPopup INIT__VWIN32_FaultPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_FaultPopup
#define VWIN32_FaultPopup INIT__VWIN32_FaultPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetContextHandle
#define VWIN32_GetContextHandle INIT_VWIN32_GetContextHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetCurrentProcessHandle
#define VWIN32_GetCurrentProcessHandle INIT_VWIN32_GetCurrentProcessHandle
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_SetWin32Event
#define _VWIN32_SetWin32Event INIT__VWIN32_SetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_SetWin32Event
#define VWIN32_SetWin32Event INIT__VWIN32_SetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_PulseWin32Event
#define _VWIN32_PulseWin32Event INIT__VWIN32_PulseWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_PulseWin32Event
#define VWIN32_PulseWin32Event INIT__VWIN32_PulseWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_ResetWin32Event
#define _VWIN32_ResetWin32Event INIT__VWIN32_ResetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_ResetWin32Event
#define VWIN32_ResetWin32Event INIT__VWIN32_ResetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_WaitSingleObject
#define _VWIN32_WaitSingleObject INIT__VWIN32_WaitSingleObject
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_WaitSingleObject
#define VWIN32_WaitSingleObject INIT__VWIN32_WaitSingleObject
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_WaitMultipleObjects
#define _VWIN32_WaitMultipleObjects INIT__VWIN32_WaitMultipleObjects
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_WaitMultipleObjects
#define VWIN32_WaitMultipleObjects INIT__VWIN32_WaitMultipleObjects
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CreateRing0Thread
#define _VWIN32_CreateRing0Thread INIT__VWIN32_CreateRing0Thread
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CreateRing0Thread
#define VWIN32_CreateRing0Thread INIT__VWIN32_CreateRing0Thread
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CloseVxDHandle
#define _VWIN32_CloseVxDHandle INIT__VWIN32_CloseVxDHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CloseVxDHandle
#define VWIN32_CloseVxDHandle INIT__VWIN32_CloseVxDHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_ActiveTimeBiasSet
#define VWIN32_ActiveTimeBiasSet INIT_VWIN32_ActiveTimeBiasSet
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_EnterCrstR0
#define VWIN32_EnterCrstR0 INIT_VWIN32_EnterCrstR0
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_BlueScreenPopup
#define VWIN32_BlueScreenPopup INIT_VWIN32_BlueScreenPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_TerminateApp
#define VWIN32_TerminateApp INIT_VWIN32_TerminateApp
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_QueueKernelAPC
#define _VWIN32_QueueKernelAPC INIT__VWIN32_QueueKernelAPC
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_QueueKernelAPC
#define VWIN32_QueueKernelAPC INIT__VWIN32_QueueKernelAPC
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_SysErrorBox
#define VWIN32_SysErrorBox INIT_VWIN32_SysErrorBox
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_IsClientWin32
#define _VWIN32_IsClientWin32 INIT__VWIN32_IsClientWin32
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_IsClientWin32
#define VWIN32_IsClientWin32 INIT__VWIN32_IsClientWin32
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Set_Thread_Context
#define _VWIN32_Set_Thread_Context INIT__VWIN32_Set_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Set_Thread_Context
#define VWIN32_Set_Thread_Context INIT__VWIN32_Set_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CopyMem
#define _VWIN32_CopyMem INIT__VWIN32_CopyMem
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CopyMem
#define VWIN32_CopyMem INIT__VWIN32_CopyMem
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Npx_Exception
#define _VWIN32_Npx_Exception INIT__VWIN32_Npx_Exception
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Npx_Exception
#define VWIN32_Npx_Exception INIT__VWIN32_Npx_Exception
#endif

#ifdef WIN40_OR_LATER
#undef  PCCARD_Get_Version
#define PCCARD_Get_Version INIT_PCCARD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PCCARD_Card_Services
#define PCCARD_Card_Services INIT_PCCARD_Card_Services
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Get_Version
#define IOS_Get_Version INIT_IOS_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_BD_Register_Device
#define IOS_BD_Register_Device INIT_IOS_BD_Register_Device
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Find_Int13_Drive
#define IOS_Find_Int13_Drive INIT_IOS_Find_Int13_Drive
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Get_Device_List
#define IOS_Get_Device_List INIT_IOS_Get_Device_List
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_SendCommand
#define IOS_SendCommand INIT_IOS_SendCommand
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Register
#define IOS_Register INIT_IOS_Register
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Requestor_Service
#define IOS_Requestor_Service INIT_IOS_Requestor_Service
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Exclusive_Access
#define IOS_Exclusive_Access INIT_IOS_Exclusive_Access
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Send_Next_Command
#define IOS_Send_Next_Command INIT_IOS_Send_Next_Command
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Set_Async_Time_Out
#define IOS_Set_Async_Time_Out INIT_IOS_Set_Async_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Signal_Semaphore_No_Switch
#define IOS_Signal_Semaphore_No_Switch INIT_IOS_Signal_Semaphore_No_Switch
#endif

#ifdef WIN40_OR_LATER
#undef  IOSIdleStatus
#define IOSIdleStatus INIT_IOSIdleStatus
#endif

#ifdef WIN40_OR_LATER
#undef  IOSMapIORSToI24
#define IOSMapIORSToI24 INIT_IOSMapIORSToI24
#endif

#ifdef WIN40_OR_LATER
#undef  IOSMapIORSToI21
#define IOSMapIORSToI21 INIT_IOSMapIORSToI21
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_BD_Command_Complete
#define IOS_BD_Command_Complete INIT_IOS_BD_Command_Complete
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_Register_Device_Driver
#define MMDEVLDR_Register_Device_Driver INIT_MMDEVLDR_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_SetDevicePresence
#define _MMDEVLDR_SetDevicePresence INIT__MMDEVLDR_SetDevicePresence
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_SetDevicePresence
#define MMDEVLDR_SetDevicePresence INIT__MMDEVLDR_SetDevicePresence
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_SetEnvironmentString
#define _MMDEVLDR_SetEnvironmentString INIT__MMDEVLDR_SetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_SetEnvironmentString
#define MMDEVLDR_SetEnvironmentString INIT__MMDEVLDR_SetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_GetEnvironmentString
#define _MMDEVLDR_GetEnvironmentString INIT__MMDEVLDR_GetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_GetEnvironmentString
#define MMDEVLDR_GetEnvironmentString INIT__MMDEVLDR_GetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_RemoveEnvironmentString
#define _MMDEVLDR_RemoveEnvironmentString INIT__MMDEVLDR_RemoveEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_RemoveEnvironmentString
#define MMDEVLDR_RemoveEnvironmentString INIT__MMDEVLDR_RemoveEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_AddEnvironmentString
#define _MMDEVLDR_AddEnvironmentString INIT__MMDEVLDR_AddEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_AddEnvironmentString
#define MMDEVLDR_AddEnvironmentString INIT__MMDEVLDR_AddEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Get_Virtualization_Count
#define VPICD_Get_Virtualization_Count INIT_VPICD_Get_Virtualization_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Phys_Mask_Channel
#define VDMAD_Phys_Mask_Channel INIT_VDMAD_Phys_Mask_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Phys_Unmask_Channel
#define VDMAD_Phys_Unmask_Channel INIT_VDMAD_Phys_Unmask_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Unvirtualize_Channel
#define VDMAD_Unvirtualize_Channel INIT_VDMAD_Unvirtualize_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_IO_Address
#define VDMAD_Set_IO_Address INIT_VDMAD_Set_IO_Address
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Convert_Hex_Binary
#define Debug_Convert_Hex_Binary INIT_Debug_Convert_Hex_Binary
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Convert_Hex_Decimal
#define Debug_Convert_Hex_Decimal INIT_Debug_Convert_Hex_Decimal
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Flags_Service
#define _Debug_Flags_Service INIT__Debug_Flags_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Flags_Service
#define Debug_Flags_Service INIT__Debug_Flags_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Cur_VM
#define Debug_Test_Cur_VM INIT_Debug_Test_Cur_VM
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Valid_Handle
#define Debug_Test_Valid_Handle INIT_Debug_Test_Valid_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_FindBlock
#define VCache_FindBlock INIT_VCache_FindBlock
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_FreeBlock
#define VCache_FreeBlock INIT_VCache_FreeBlock
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_GetSize
#define VCache_GetSize INIT_VCache_GetSize
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_GetStats
#define VCache_GetStats INIT_VCache_GetStats
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Get_Version
#define VCache_Get_Version INIT_VCache_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Hold
#define VCache_Hold INIT_VCache_Hold
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_MakeMRU
#define VCache_MakeMRU INIT_VCache_MakeMRU
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Register
#define VCache_Register INIT_VCache_Register
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_SwapBuffers
#define VCache_SwapBuffers INIT_VCache_SwapBuffers
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_TestHold
#define VCache_TestHold INIT_VCache_TestHold
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Unhold
#define VCache_Unhold INIT_VCache_Unhold
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_CloseLookupCache
#define _VCache_CloseLookupCache INIT__VCache_CloseLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CloseLookupCache
#define VCache_CloseLookupCache INIT__VCache_CloseLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_CreateLookupCache
#define _VCache_CreateLookupCache INIT__VCache_CreateLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CreateLookupCache
#define VCache_CreateLookupCache INIT__VCache_CreateLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_DeleteLookupCache
#define _VCache_DeleteLookupCache INIT__VCache_DeleteLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_DeleteLookupCache
#define VCache_DeleteLookupCache INIT__VCache_DeleteLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_Lookup
#define _VCache_Lookup INIT__VCache_Lookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Lookup
#define VCache_Lookup INIT__VCache_Lookup
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_UpdateLookup
#define _VCache_UpdateLookup INIT__VCache_UpdateLookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_UpdateLookup
#define VCache_UpdateLookup INIT__VCache_UpdateLookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_RelinquishPage
#define VCache_RelinquishPage INIT_VCache_RelinquishPage
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_UseThisPage
#define VCache_UseThisPage INIT_VCache_UseThisPage
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_RecalcSums
#define VCache_RecalcSums INIT_VCache_RecalcSums
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_TestHandle
#define VCache_TestHandle INIT_VCache_TestHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_VerifySums
#define VCache_VerifySums INIT_VCache_VerifySums
#endif

#ifdef WIN40_OR_LATER
#undef  DOSMGR_Alloc_Local_Sys_VM_Mem
#define DOSMGR_Alloc_Local_Sys_VM_Mem INIT_DOSMGR_Alloc_Local_Sys_VM_Mem
#endif

#ifdef WIN40_OR_LATER
#undef  DOSMGR_Init_UMB_Area
#define DOSMGR_Init_UMB_Area INIT_DOSMGR_Init_UMB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  _GetRegistryKey
#define _GetRegistryKey INIT__GetRegistryKey
#endif

#ifdef WIN40_OR_LATER
#undef  GetRegistryKey
#define GetRegistryKey INIT__GetRegistryKey
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Nest_Exec_Status
#define Get_Nest_Exec_Status INIT_Get_Nest_Exec_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _AddReclaimableItem
#define _AddReclaimableItem INIT__AddReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  AddReclaimableItem
#define AddReclaimableItem INIT__AddReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  _AtEventTime
#define _AtEventTime INIT__AtEventTime
#endif

#ifdef WIN40_OR_LATER
#undef  AtEventTime
#define AtEventTime INIT__AtEventTime
#endif

#ifdef WIN40_OR_LATER
#undef  Close_Boot_Log
#define Close_Boot_Log INIT_Close_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  EnableDisable_Boot_Log
#define EnableDisable_Boot_Log INIT_EnableDisable_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  _EnumReclaimableItem
#define _EnumReclaimableItem INIT__EnumReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  EnumReclaimableItem
#define EnumReclaimableItem INIT__EnumReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  Open_Boot_Log
#define Open_Boot_Log INIT_Open_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  _SetReclaimableItem
#define _SetReclaimableItem INIT__SetReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  SetReclaimableItem
#define SetReclaimableItem INIT__SetReclaimableItem
#endif

#undef  Hook_Device_Service_Ex
#define Hook_Device_Service_Ex INIT_Hook_Device_Service_Ex

#ifdef WIN40_OR_LATER
#undef  PERF_Get_Version
#define PERF_Get_Version INIT_PERF_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Register
#define PERF_Server_Register INIT_PERF_Server_Register
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Deregister
#define PERF_Server_Deregister INIT_PERF_Server_Deregister
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Add_Stat
#define PERF_Server_Add_Stat INIT_PERF_Server_Add_Stat
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Remove_Stat
#define PERF_Server_Remove_Stat INIT_PERF_Server_Remove_Stat
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service_Ex
#define Unhook_Device_Service_Ex INIT_Unhook_Device_Service_Ex
#endif

#undef  Call_Previous_Hook_Proc
#define Call_Previous_Hook_Proc INIT_Call_Previous_Hook_Proc

#ifdef WIN40_OR_LATER
#undef  dummy_935
#define dummy_935 INIT_dummy_935
#endif

#undef  Hook_Device_Service_C
#define Hook_Device_Service_C INIT_Hook_Device_Service_C

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service_C
#define Unhook_Device_Service_C INIT_Unhook_Device_Service_C
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_ID
#define _CONFIGMG_Add_ID INIT__CONFIGMG_Add_ID
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_ID
#define CONFIGMG_Add_ID INIT__CONFIGMG_Add_ID
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Enumerator_Function
#define _CONFIGMG_Register_Enumerator_Function INIT__CONFIGMG_Register_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Enumerator_Function
#define CONFIGMG_Register_Enumerator_Function INIT__CONFIGMG_Register_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Update_User_Activity
#define SHELL_Update_User_Activity INIT_SHELL_Update_User_Activity
#endif

#undef  VDMAD_Unlock_DMA_Region_No_Dirty
#define VDMAD_Unlock_DMA_Region_No_Dirty INIT_VDMAD_Unlock_DMA_Region_No_Dirty

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetCurrentDirectory
#define VWIN32_GetCurrentDirectory INIT_VWIN32_GetCurrentDirectory
#endif

#ifdef WIN40_OR_LATER
#undef  VXDLDR_UnloadMe
#define VXDLDR_UnloadMe INIT_VXDLDR_UnloadMe
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_InitCompleted
#define _PELDR_InitCompleted INIT__PELDR_InitCompleted
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_InitCompleted
#define PELDR_InitCompleted INIT__PELDR_InitCompleted
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_AddExportTable
#define _PELDR_AddExportTable INIT__PELDR_AddExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_AddExportTable
#define PELDR_AddExportTable INIT__PELDR_AddExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_FreeModule
#define _PELDR_FreeModule INIT__PELDR_FreeModule
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_FreeModule
#define PELDR_FreeModule INIT__PELDR_FreeModule
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetEntryPoint
#define _PELDR_GetEntryPoint INIT__PELDR_GetEntryPoint
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetEntryPoint
#define PELDR_GetEntryPoint INIT__PELDR_GetEntryPoint
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetModuleHandle
#define _PELDR_GetModuleHandle INIT__PELDR_GetModuleHandle
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetModuleHandle
#define PELDR_GetModuleHandle INIT__PELDR_GetModuleHandle
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetModuleUsage
#define _PELDR_GetModuleUsage INIT__PELDR_GetModuleUsage
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetModuleUsage
#define PELDR_GetModuleUsage INIT__PELDR_GetModuleUsage
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetProcAddress
#define _PELDR_GetProcAddress INIT__PELDR_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetProcAddress
#define PELDR_GetProcAddress INIT__PELDR_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_LoadModule
#define _PELDR_LoadModule INIT__PELDR_LoadModule
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_LoadModule
#define PELDR_LoadModule INIT__PELDR_LoadModule
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_LoadModuleEx
#define _PELDR_LoadModuleEx INIT__PELDR_LoadModuleEx
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_LoadModuleEx
#define PELDR_LoadModuleEx INIT__PELDR_LoadModuleEx
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_RemoveExportTable
#define _PELDR_RemoveExportTable INIT__PELDR_RemoveExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_RemoveExportTable
#define PELDR_RemoveExportTable INIT__PELDR_RemoveExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_AdjustMinimum
#define VCache_AdjustMinimum INIT_VCache_AdjustMinimum
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CheckAvail
#define VCache_CheckAvail INIT_VCache_CheckAvail
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Deregister
#define VCache_Deregister INIT_VCache_Deregister
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Enum
#define VCache_Enum INIT_VCache_Enum
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Enable_Keyboard
#define VKD_Enable_Keyboard INIT_VKD_Enable_Keyboard
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Disable_Keyboard
#define VKD_Disable_Keyboard INIT_VKD_Disable_Keyboard
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Get_Shift_State
#define VKD_Get_Shift_State INIT_VKD_Get_Shift_State
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Put_Byte
#define VKD_Put_Byte INIT_VKD_Put_Byte
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Set_Shift_State
#define VKD_Set_Shift_State INIT_VKD_Set_Shift_State
#endif

#undef  VCD_InitThunk_Control
#define VCD_InitThunk_Control INIT_VCD_InitThunk_Control

#undef  VCD_InitThunk_Irq
#define VCD_InitThunk_Irq INIT_VCD_InitThunk_Irq

#undef  VCD_InitThunk_Io
#define VCD_InitThunk_Io INIT_VCD_InitThunk_Io

#undef  VCD_Acquire_Port
#define VCD_Acquire_Port INIT_VCD_Acquire_Port

#undef  VCD_Free_Port_Windows_Style
#define VCD_Free_Port_Windows_Style INIT_VCD_Free_Port_Windows_Style

#undef  VCD_Steal_Port_Windows_Style
#define VCD_Steal_Port_Windows_Style INIT_VCD_Steal_Port_Windows_Style

#ifdef WIN40_OR_LATER
#undef  VCD_Find_COM_Index
#define VCD_Find_COM_Index INIT_VCD_Find_COM_Index
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Set_Port_Global_Special
#define VCD_Set_Port_Global_Special INIT_VCD_Set_Port_Global_Special
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Virtualize_Port_Dynamic
#define VCD_Virtualize_Port_Dynamic INIT_VCD_Virtualize_Port_Dynamic
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Unvirtualize_Port_Dynamic
#define VCD_Unvirtualize_Port_Dynamic INIT_VCD_Unvirtualize_Port_Dynamic
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Acquire_Port_Windows_Style
#define VCD_Acquire_Port_Windows_Style INIT_VCD_Acquire_Port_Windows_Style
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Free_Port
#define VCD_Free_Port INIT_VCD_Free_Port
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Check_Update_Soon
#define VDD_Check_Update_Soon INIT_VDD_Check_Update_Soon
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_GrabRtn
#define VDD_Get_GrabRtn INIT_VDD_Get_GrabRtn
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Hide_Cursor
#define VDD_Hide_Cursor INIT_VDD_Hide_Cursor
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Msg_SetCursPos
#define VDD_Msg_SetCursPos INIT_VDD_Msg_SetCursPos
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Enable_Disable_Mouse_Events
#define VMD_Enable_Disable_Mouse_Events INIT_VMD_Enable_Disable_Mouse_Events
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Init_File
#define PageFile_Init_File INIT_PageFile_Init_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Get_Version
#define PageFile_Get_Version INIT_PageFile_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Clean_Up
#define PageFile_Clean_Up INIT_PageFile_Clean_Up
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Grow_File
#define PageFile_Grow_File INIT_PageFile_Grow_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Read_Or_Write
#define PageFile_Read_Or_Write INIT_PageFile_Read_Or_Write
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Cancel
#define PageFile_Cancel INIT_PageFile_Cancel
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Test_IO_Valid
#define PageFile_Test_IO_Valid INIT_PageFile_Test_IO_Valid
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Get_Size_Info
#define PageFile_Get_Size_Info INIT_PageFile_Get_Size_Info
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Get_Version
#define PageSwap_Get_Version INIT_PageSwap_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Test_IO_Valid
#define PageSwap_Test_IO_Valid INIT_PageSwap_Test_IO_Valid
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Read_Or_Write
#define PageSwap_Read_Or_Write INIT_PageSwap_Read_Or_Write
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Grow_File
#define PageSwap_Grow_File INIT_PageSwap_Grow_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Init_File
#define PageSwap_Init_File INIT_PageSwap_Init_File
#endif

#undef  Allocate_PM_App_CB_Area
#define Allocate_PM_App_CB_Area INIT_Allocate_PM_App_CB_Area

#ifdef WIN40_OR_LATER
#undef  _Call_On_My_Stack
#define _Call_On_My_Stack INIT__Call_On_My_Stack
#endif

#ifdef WIN40_OR_LATER
#undef  Call_On_My_Stack
#define Call_On_My_Stack INIT__Call_On_My_Stack
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Printf_Service
#define _Debug_Printf_Service INIT__Debug_Printf_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Printf_Service
#define Debug_Printf_Service INIT__Debug_Printf_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _strupr
#define _strupr INIT__strupr
#endif

#ifdef WIN40_OR_LATER
#undef  strupr
#define strupr INIT__strupr
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Do_Physical_IO
#define VDD_Do_Physical_IO INIT_VDD_Do_Physical_IO
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_DISPLAYINFO
#define VDD_Get_DISPLAYINFO INIT_VDD_Get_DISPLAYINFO
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_Mini_Dispatch_Table
#define VDD_Get_Mini_Dispatch_Table INIT_VDD_Get_Mini_Dispatch_Table
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_Special_VM_IDs
#define VDD_Get_Special_VM_IDs INIT_VDD_Get_Special_VM_IDs
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_VM_Info
#define VDD_Get_VM_Info INIT_VDD_Get_VM_Info
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Register_Extra_Screen_Selector
#define VDD_Register_Extra_Screen_Selector INIT_VDD_Register_Extra_Screen_Selector
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Register_Virtual_Port
#define VDD_Register_Virtual_Port INIT_VDD_Register_Virtual_Port
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Set_Sleep_Flag_Addr
#define VDD_Set_Sleep_Flag_Addr INIT_VDD_Set_Sleep_Flag_Addr
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Takeover_VGA_Port
#define VDD_Takeover_VGA_Port INIT_VDD_Takeover_VGA_Port
#endif

#undef  Init_Generic_Thunk
#define Init_Generic_Thunk INIT_Init_Generic_Thunk

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Enumerate_DevNodes
#define _VCOMM_Enumerate_DevNodes INIT__VCOMM_Enumerate_DevNodes
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Enumerate_DevNodes
#define VCOMM_Enumerate_DevNodes INIT__VCOMM_Enumerate_DevNodes
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Virt_Count
#define VDMAD_Get_Virt_Count INIT_VDMAD_Get_Virt_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_Channel_Callbacks
#define VDMAD_Set_Channel_Callbacks INIT_VDMAD_Set_Channel_Callbacks
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Phys_Status
#define VDMAD_Get_Phys_Status INIT_VDMAD_Get_Phys_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Phys_Count
#define VDMAD_Get_Phys_Count INIT_VDMAD_Get_Phys_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Max_Phys_Page
#define VDMAD_Get_Max_Phys_Page INIT_VDMAD_Get_Max_Phys_Page
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_Virt_Count
#define VDMAD_Set_Virt_Count INIT_VDMAD_Set_Virt_Count
#endif

#ifdef WIN40_OR_LATER
#undef  _EnterMustComplete
#define _EnterMustComplete INIT__EnterMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  EnterMustComplete
#define EnterMustComplete INIT__EnterMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _LeaveMustComplete
#define _LeaveMustComplete INIT__LeaveMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  LeaveMustComplete
#define LeaveMustComplete INIT__LeaveMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _ResumeExecMustComplete
#define _ResumeExecMustComplete INIT__ResumeExecMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ResumeExecMustComplete
#define ResumeExecMustComplete INIT__ResumeExecMustComplete
#endif

#undef  VTDI_Get_Version
#define VTDI_Get_Version INIT_VTDI_Get_Version

#undef  VTDI_Get_Info
#define VTDI_Get_Info INIT_VTDI_Get_Info

#undef  VTDI_Start_Timer
#define VTDI_Start_Timer INIT_VTDI_Start_Timer

#undef  VTDI_Stop_Timer
#define VTDI_Stop_Timer INIT_VTDI_Stop_Timer

#undef  dummy_1034
#define dummy_1034 INIT_dummy_1034

#undef  dummy_1035
#define dummy_1035 INIT_dummy_1035

#undef  VTDI_Schedule_Event
#define VTDI_Schedule_Event INIT_VTDI_Schedule_Event

#undef  VTDI_Cancel_Event
#define VTDI_Cancel_Event INIT_VTDI_Cancel_Event

#undef  VTDI_Block
#define VTDI_Block INIT_VTDI_Block

#undef  VTDI_Signal
#define VTDI_Signal INIT_VTDI_Signal

#undef  VTDI_Register
#define VTDI_Register INIT_VTDI_Register

#ifdef WIN40_OR_LATER
#undef  VTDI_Unload
#define VTDI_Unload INIT_VTDI_Unload
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Initialize
#define VTDI_Initialize INIT_VTDI_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Register_UnloadProc
#define VTDI_Register_UnloadProc INIT_VTDI_Register_UnloadProc
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Register_LoadProc
#define VTDI_Register_LoadProc INIT_VTDI_Register_LoadProc
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Thunk_HWInt_Ex
#define VPICD_Thunk_HWInt_Ex INIT_VPICD_Thunk_HWInt_Ex
#endif

#ifdef WIN40_OR_LATER
#undef  Call_Restricted_Event_Ex
#define Call_Restricted_Event_Ex INIT_Call_Restricted_Event_Ex
#endif

#ifdef WIN41_OR_LATER
#undef  NTKERN_Get_Version
#define NTKERN_Get_Version INIT_NTKERN_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernCreateFile
#define NtKernCreateFile INIT_NtKernCreateFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernClose
#define NtKernClose INIT_NtKernClose
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernReadFile
#define NtKernReadFile INIT_NtKernReadFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernWriteFile
#define NtKernWriteFile INIT_NtKernWriteFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernDeviceIoControl
#define NtKernDeviceIoControl INIT_NtKernDeviceIoControl
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernGetWorkerThread
#define NtKernGetWorkerThread INIT_NtKernGetWorkerThread
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernLoadDriver
#define NtKernLoadDriver INIT_NtKernLoadDriver
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernQueueWorkItem
#define NtKernQueueWorkItem INIT_NtKernQueueWorkItem
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernPhysicalDeviceObjectToDevNode
#define NtKernPhysicalDeviceObjectToDevNode INIT_NtKernPhysicalDeviceObjectToDevNode
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernSetPhysicalCacheTypeRange
#define NtKernSetPhysicalCacheTypeRange INIT_NtKernSetPhysicalCacheTypeRange
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernWin9XLoadDriver
#define NtKernWin9XLoadDriver INIT_NtKernWin9XLoadDriver
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernCancelIoFile
#define NtKernCancelIoFile INIT_NtKernCancelIoFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernGetVPICDHandleFromInterruptObj
#define NtKernGetVPICDHandleFromInterruptObj INIT_NtKernGetVPICDHandleFromInterruptObj
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernInternalDeviceIoControl
#define NtKernInternalDeviceIoControl INIT_NtKernInternalDeviceIoControl
#endif

#ifdef WIN41_OR_LATER
#undef  _KeSaveFloatingPointState
#define _KeSaveFloatingPointState INIT__KeSaveFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  KeSaveFloatingPointState
#define KeSaveFloatingPointState INIT__KeSaveFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  _KeRestoreFloatingPointState
#define _KeRestoreFloatingPointState INIT__KeRestoreFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  KeRestoreFloatingPointState
#define KeRestoreFloatingPointState INIT__KeRestoreFloatingPointState
#endif

#ifdef WIN40_OR_LATER
#undef  VTD_Delay
#define VTD_Delay INIT_VTD_Delay
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Get_Version
#define _PCI_Get_Version INIT__PCI_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Get_Version
#define PCI_Get_Version INIT__PCI_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Read_Config
#define _PCI_Read_Config INIT__PCI_Read_Config
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Read_Config
#define PCI_Read_Config INIT__PCI_Read_Config
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Write_Config
#define _PCI_Write_Config INIT__PCI_Write_Config
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Write_Config
#define PCI_Write_Config INIT__PCI_Write_Config
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Lock_Unlock
#define _PCI_Lock_Unlock INIT__PCI_Lock_Unlock
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Lock_Unlock
#define PCI_Lock_Unlock INIT__PCI_Lock_Unlock
#endif

#ifdef WIN41_OR_LATER
#undef  _PCCARD_Access_CIS_Memory
#define _PCCARD_Access_CIS_Memory INIT__PCCARD_Access_CIS_Memory
#endif

#ifdef WIN41_OR_LATER
#undef  PCCARD_Access_CIS_Memory
#define PCCARD_Access_CIS_Memory INIT__PCCARD_Access_CIS_Memory
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Get_DevNode_PowerCapabilities
#define _CONFIGMG_Get_DevNode_PowerCapabilities INIT__CONFIGMG_Get_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Get_DevNode_PowerCapabilities
#define CONFIGMG_Get_DevNode_PowerCapabilities INIT__CONFIGMG_Get_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Set_DevNode_PowerCapabilities
#define _CONFIGMG_Set_DevNode_PowerCapabilities INIT__CONFIGMG_Set_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Set_DevNode_PowerCapabilities
#define CONFIGMG_Set_DevNode_PowerCapabilities INIT__CONFIGMG_Set_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Get_DevNode_PowerState
#define _CONFIGMG_Get_DevNode_PowerState INIT__CONFIGMG_Get_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Get_DevNode_PowerState
#define CONFIGMG_Get_DevNode_PowerState INIT__CONFIGMG_Get_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Set_DevNode_PowerState
#define _CONFIGMG_Set_DevNode_PowerState INIT__CONFIGMG_Set_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Set_DevNode_PowerState
#define CONFIGMG_Set_DevNode_PowerState INIT__CONFIGMG_Set_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Register_DevNode_For_Idle_Detection
#define _CONFIGMG_Register_DevNode_For_Idle_Detection INIT__CONFIGMG_Register_DevNode_For_Idle_Detection
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Register_DevNode_For_Idle_Detection
#define CONFIGMG_Register_DevNode_For_Idle_Detection INIT__CONFIGMG_Register_DevNode_For_Idle_Detection
#endif

