//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  _XchgFreePhysReg
#define _XchgFreePhysReg PAGEABLE__XchgFreePhysReg

#undef  XchgFreePhysReg
#define XchgFreePhysReg PAGEABLE__XchgFreePhysReg

#undef  Wake_Up_VM
#define Wake_Up_VM PAGEABLE_Wake_Up_VM

#undef  Wait_Semaphore
#define Wait_Semaphore PAGEABLE_Wait_Semaphore

#undef  Validate_VM_Handle
#define Validate_VM_Handle PAGEABLE_Validate_VM_Handle

#undef  Validate_Client_Ptr
#define Validate_Client_Ptr PAGEABLE_Validate_Client_Ptr

#undef  Update_System_Clock
#define Update_System_Clock PAGEABLE_Update_System_Clock

#undef  _UnmapFreePhysReg
#define _UnmapFreePhysReg PAGEABLE__UnmapFreePhysReg

#undef  UnmapFreePhysReg
#define UnmapFreePhysReg PAGEABLE__UnmapFreePhysReg

#undef  Unhook_Invalid_Page_Fault
#define Unhook_Invalid_Page_Fault PAGEABLE_Unhook_Invalid_Page_Fault

#undef  Test_Sys_VM_Handle
#define Test_Sys_VM_Handle PAGEABLE_Test_Sys_VM_Handle

#undef  _TestGlobalV86Mem
#define _TestGlobalV86Mem PAGEABLE__TestGlobalV86Mem

#undef  TestGlobalV86Mem
#define TestGlobalV86Mem PAGEABLE__TestGlobalV86Mem

#undef  Test_Debug_Installed
#define Test_Debug_Installed PAGEABLE_Test_Debug_Installed

#undef  Test_DBCS_Lead_Byte
#define Test_DBCS_Lead_Byte PAGEABLE_Test_DBCS_Lead_Byte

#undef  Test_Cur_VM_Handle
#define Test_Cur_VM_Handle PAGEABLE_Test_Cur_VM_Handle

#undef  System_Control
#define System_Control PAGEABLE_System_Control

#undef  Suspend_VM
#define Suspend_VM PAGEABLE_Suspend_VM

#undef  Simulate_Push
#define Simulate_Push PAGEABLE_Simulate_Push

#undef  Simulate_Pop
#define Simulate_Pop PAGEABLE_Simulate_Pop

#undef  Simulate_Int
#define Simulate_Int PAGEABLE_Simulate_Int

#undef  Simulate_Far_Ret_N
#define Simulate_Far_Ret_N PAGEABLE_Simulate_Far_Ret_N

#undef  Simulate_Far_Jmp
#define Simulate_Far_Jmp PAGEABLE_Simulate_Far_Jmp

#undef  Simulate_Far_Call
#define Simulate_Far_Call PAGEABLE_Simulate_Far_Call

#undef  Signal_Semaphore
#define Signal_Semaphore PAGEABLE_Signal_Semaphore

#undef  Set_VM_Time_Out
#define Set_VM_Time_Out PAGEABLE_Set_VM_Time_Out

#undef  Set_V86_Int_Vector
#define Set_V86_Int_Vector PAGEABLE_Set_V86_Int_Vector

#undef  Set_PM_Int_Vector
#define Set_PM_Int_Vector PAGEABLE_Set_PM_Int_Vector

#undef  Set_Time_Slice_Priority
#define Set_Time_Slice_Priority PAGEABLE_Set_Time_Slice_Priority

#undef  Set_Time_Slice_Granularity
#define Set_Time_Slice_Granularity PAGEABLE_Set_Time_Slice_Granularity

#undef  Set_System_Exit_Code
#define Set_System_Exit_Code PAGEABLE_Set_System_Exit_Code

#undef  _SetResetV86Pageable
#define _SetResetV86Pageable PAGEABLE__SetResetV86Pageable

#undef  SetResetV86Pageable
#define SetResetV86Pageable PAGEABLE__SetResetV86Pageable

#undef  Set_PM_Int_Type
#define Set_PM_Int_Type PAGEABLE_Set_PM_Int_Type

#undef  Set_Physical_HMA_Alias
#define Set_Physical_HMA_Alias PAGEABLE_Set_Physical_HMA_Alias

#undef  Set_NMI_Handler_Addr
#define Set_NMI_Handler_Addr PAGEABLE_Set_NMI_Handler_Addr

#undef  Set_Mono_Cur_Pos
#define Set_Mono_Cur_Pos PAGEABLE_Set_Mono_Cur_Pos

#undef  _SetLastV86Page
#define _SetLastV86Page PAGEABLE__SetLastV86Page

#undef  SetLastV86Page
#define SetLastV86Page PAGEABLE__SetLastV86Page

#undef  Set_Global_Time_Out
#define Set_Global_Time_Out PAGEABLE_Set_Global_Time_Out

#undef  _SetFreePhysRegCalBk
#define _SetFreePhysRegCalBk PAGEABLE__SetFreePhysRegCalBk

#undef  SetFreePhysRegCalBk
#define SetFreePhysRegCalBk PAGEABLE__SetFreePhysRegCalBk

#undef  Set_Execution_Focus
#define Set_Execution_Focus PAGEABLE_Set_Execution_Focus

#undef  _SetDescriptor
#define _SetDescriptor PAGEABLE__SetDescriptor

#undef  SetDescriptor
#define SetDescriptor PAGEABLE__SetDescriptor

#undef  Set_Delete_On_Exit_File
#define Set_Delete_On_Exit_File PAGEABLE_Set_Delete_On_Exit_File

#undef  _SelectorMapFlat
#define _SelectorMapFlat PAGEABLE__SelectorMapFlat

#undef  SelectorMapFlat
#define SelectorMapFlat PAGEABLE__SelectorMapFlat

#undef  Schedule_VM_Event
#define Schedule_VM_Event PAGEABLE_Schedule_VM_Event

#undef  Schedule_Global_Event
#define Schedule_Global_Event PAGEABLE_Schedule_Global_Event

#undef  Save_Client_State
#define Save_Client_State PAGEABLE_Save_Client_State

#undef  Resume_VM
#define Resume_VM PAGEABLE_Resume_VM

#undef  Restore_Client_State
#define Restore_Client_State PAGEABLE_Restore_Client_State

#undef  Remove_V86_Break_Point
#define Remove_V86_Break_Point PAGEABLE_Remove_V86_Break_Point

#undef  Remove_Exception_Handler
#define Remove_Exception_Handler PAGEABLE_Remove_Exception_Handler

#undef  Release_Critical_Section
#define Release_Critical_Section PAGEABLE_Release_Critical_Section

#undef  Queue_Debug_String
#define Queue_Debug_String PAGEABLE_Queue_Debug_String

#undef  _PhysIntoV86
#define _PhysIntoV86 PAGEABLE__PhysIntoV86

#undef  PhysIntoV86
#define PhysIntoV86 PAGEABLE__PhysIntoV86

#undef  _PageUnLock
#define _PageUnLock PAGEABLE__PageUnLock

#undef  PageUnLock
#define PageUnLock PAGEABLE__PageUnLock

#undef  _PageResetHandlePAddr
#define _PageResetHandlePAddr PAGEABLE__PageResetHandlePAddr

#undef  PageResetHandlePAddr
#define PageResetHandlePAddr PAGEABLE__PageResetHandlePAddr

#undef  _PageReAllocate
#define _PageReAllocate PAGEABLE__PageReAllocate

#undef  PageReAllocate
#define PageReAllocate PAGEABLE__PageReAllocate

#undef  _PageOutDirtyPages
#define _PageOutDirtyPages PAGEABLE__PageOutDirtyPages

#undef  PageOutDirtyPages
#define PageOutDirtyPages PAGEABLE__PageOutDirtyPages

#undef  _PageLock
#define _PageLock PAGEABLE__PageLock

#undef  PageLock
#define PageLock PAGEABLE__PageLock

#undef  _PageGetSizeAddr
#define _PageGetSizeAddr PAGEABLE__PageGetSizeAddr

#undef  PageGetSizeAddr
#define PageGetSizeAddr PAGEABLE__PageGetSizeAddr

#undef  _PageGetAllocInfo
#define _PageGetAllocInfo PAGEABLE__PageGetAllocInfo

#undef  PageGetAllocInfo
#define PageGetAllocInfo PAGEABLE__PageGetAllocInfo

#undef  _PageFree
#define _PageFree PAGEABLE__PageFree

#undef  PageFree
#define PageFree PAGEABLE__PageFree

#undef  _PageDiscardPages
#define _PageDiscardPages PAGEABLE__PageDiscardPages

#undef  PageDiscardPages
#define PageDiscardPages PAGEABLE__PageDiscardPages

#undef  _PageCheckLinRange
#define _PageCheckLinRange PAGEABLE__PageCheckLinRange

#undef  PageCheckLinRange
#define PageCheckLinRange PAGEABLE__PageCheckLinRange

#undef  _PageAllocate
#define _PageAllocate PAGEABLE__PageAllocate

#undef  PageAllocate
#define PageAllocate PAGEABLE__PageAllocate

#undef  Out_Mono_String
#define Out_Mono_String PAGEABLE_Out_Mono_String

#undef  Out_Mono_Chr_Attrs
#define Out_Mono_Chr_Attrs PAGEABLE_Out_Mono_Chr_Attrs

#undef  Out_Mono_Chr
#define Out_Mono_Chr PAGEABLE_Out_Mono_Chr

#undef  Out_Debug_String
#define Out_Debug_String PAGEABLE_Out_Debug_String

#undef  Out_Debug_Chr
#define Out_Debug_Chr PAGEABLE_Out_Debug_Chr

#undef  OpenFile
#define OpenFile PAGEABLE_OpenFile

#undef  Nuke_VM
#define Nuke_VM PAGEABLE_Nuke_VM

#undef  No_Fail_Resume_VM
#define No_Fail_Resume_VM PAGEABLE_No_Fail_Resume_VM

#undef  _ModifyPageBits
#define _ModifyPageBits PAGEABLE__ModifyPageBits

#undef  ModifyPageBits
#define ModifyPageBits PAGEABLE__ModifyPageBits

#undef  _MMGR_Toggle_HMA
#define _MMGR_Toggle_HMA PAGEABLE__MMGR_Toggle_HMA

#undef  MMGR_Toggle_HMA
#define MMGR_Toggle_HMA PAGEABLE__MMGR_Toggle_HMA

#undef  MMGR_SetNULPageAddr
#define MMGR_SetNULPageAddr PAGEABLE_MMGR_SetNULPageAddr

#undef  _MapPhysToLinear
#define _MapPhysToLinear PAGEABLE__MapPhysToLinear

#undef  MapPhysToLinear
#define MapPhysToLinear PAGEABLE__MapPhysToLinear

#undef  Map_Lin_To_VM_Addr
#define Map_Lin_To_VM_Addr PAGEABLE_Map_Lin_To_VM_Addr

#undef  _MapIntoV86
#define _MapIntoV86 PAGEABLE__MapIntoV86

#undef  MapIntoV86
#define MapIntoV86 PAGEABLE__MapIntoV86

#undef  _MapFreePhysReg
#define _MapFreePhysReg PAGEABLE__MapFreePhysReg

#undef  MapFreePhysReg
#define MapFreePhysReg PAGEABLE__MapFreePhysReg

#undef  Map_Flat
#define Map_Flat PAGEABLE_Map_Flat

#undef  Locate_Byte_In_ROM
#define Locate_Byte_In_ROM PAGEABLE_Locate_Byte_In_ROM

#undef  List_Remove_First
#define List_Remove_First PAGEABLE_List_Remove_First

#undef  List_Remove
#define List_Remove PAGEABLE_List_Remove

#undef  List_Insert
#define List_Insert PAGEABLE_List_Insert

#undef  List_Get_Next
#define List_Get_Next PAGEABLE_List_Get_Next

#undef  List_Get_First
#define List_Get_First PAGEABLE_List_Get_First

#undef  List_Destroy
#define List_Destroy PAGEABLE_List_Destroy

#undef  List_Deallocate
#define List_Deallocate PAGEABLE_List_Deallocate

#undef  List_Create
#define List_Create PAGEABLE_List_Create

#undef  List_Attach_Tail
#define List_Attach_Tail PAGEABLE_List_Attach_Tail

#undef  List_Attach
#define List_Attach PAGEABLE_List_Attach

#undef  List_Allocate
#define List_Allocate PAGEABLE_List_Allocate

#undef  _LinPageUnLock
#define _LinPageUnLock PAGEABLE__LinPageUnLock

#undef  LinPageUnLock
#define LinPageUnLock PAGEABLE__LinPageUnLock

#undef  _LinPageLock
#define _LinPageLock PAGEABLE__LinPageLock

#undef  LinPageLock
#define LinPageLock PAGEABLE__LinPageLock

#undef  _LinMapIntoV86
#define _LinMapIntoV86 PAGEABLE__LinMapIntoV86

#undef  LinMapIntoV86
#define LinMapIntoV86 PAGEABLE__LinMapIntoV86

#undef  Is_Debug_Chr
#define Is_Debug_Chr PAGEABLE_Is_Debug_Chr

#undef  Install_V86_Break_Point
#define Install_V86_Break_Point PAGEABLE_Install_V86_Break_Point

#undef  Install_IO_Handler
#define Install_IO_Handler PAGEABLE_Install_IO_Handler

#undef  In_Debug_Chr
#define In_Debug_Chr PAGEABLE_In_Debug_Chr

#undef  Hook_VMM_Fault
#define Hook_VMM_Fault PAGEABLE_Hook_VMM_Fault

#undef  Hook_V86_Page
#define Hook_V86_Page PAGEABLE_Hook_V86_Page

#undef  Hook_V86_Int_Chain
#define Hook_V86_Int_Chain PAGEABLE_Hook_V86_Int_Chain

#undef  Hook_V86_Fault
#define Hook_V86_Fault PAGEABLE_Hook_V86_Fault

#undef  Hook_PM_Fault
#define Hook_PM_Fault PAGEABLE_Hook_PM_Fault

#undef  Hook_Device_PM_API
#define Hook_Device_PM_API PAGEABLE_Hook_Device_PM_API

#undef  Hook_NMI_Event
#define Hook_NMI_Event PAGEABLE_Hook_NMI_Event

#undef  Hook_Invalid_Page_Fault
#define Hook_Invalid_Page_Fault PAGEABLE_Hook_Invalid_Page_Fault

#undef  Hook_Device_V86_API
#define Hook_Device_V86_API PAGEABLE_Hook_Device_V86_API

#undef  Hook_Device_Service
#define Hook_Device_Service PAGEABLE_Hook_Device_Service

#undef  _HeapReAllocate
#define _HeapReAllocate PAGEABLE__HeapReAllocate

#undef  HeapReAllocate
#define HeapReAllocate PAGEABLE__HeapReAllocate

#undef  _HeapGetSize
#define _HeapGetSize PAGEABLE__HeapGetSize

#undef  HeapGetSize
#define HeapGetSize PAGEABLE__HeapGetSize

#undef  _HeapFree
#define _HeapFree PAGEABLE__HeapFree

#undef  HeapFree
#define HeapFree PAGEABLE__HeapFree

#undef  _HeapAllocate
#define _HeapAllocate PAGEABLE__HeapAllocate

#undef  HeapAllocate
#define HeapAllocate PAGEABLE__HeapAllocate

#undef  _GetVMPgCount
#define _GetVMPgCount PAGEABLE__GetVMPgCount

#undef  GetVMPgCount
#define GetVMPgCount PAGEABLE__GetVMPgCount

#undef  Get_VMM_Version
#define Get_VMM_Version PAGEABLE_Get_VMM_Version

#undef  Get_VMM_Reenter_Count
#define Get_VMM_Reenter_Count PAGEABLE_Get_VMM_Reenter_Count

#undef  Get_VM_Exec_Time
#define Get_VM_Exec_Time PAGEABLE_Get_VM_Exec_Time

#undef  _GetV86PageableArray
#define _GetV86PageableArray PAGEABLE__GetV86PageableArray

#undef  GetV86PageableArray
#define GetV86PageableArray PAGEABLE__GetV86PageableArray

#undef  Get_V86_Int_Vector
#define Get_V86_Int_Vector PAGEABLE_Get_V86_Int_Vector

#undef  Get_Time_Slice_Priority
#define Get_Time_Slice_Priority PAGEABLE_Get_Time_Slice_Priority

#undef  Get_Time_Slice_Info
#define Get_Time_Slice_Info PAGEABLE_Get_Time_Slice_Info

#undef  Get_Time_Slice_Granularity
#define Get_Time_Slice_Granularity PAGEABLE_Get_Time_Slice_Granularity

#undef  Get_Sys_VM_Handle
#define Get_Sys_VM_Handle PAGEABLE_Get_Sys_VM_Handle

#undef  Get_System_Time
#define Get_System_Time PAGEABLE_Get_System_Time

#undef  _GetSetPageOutCount
#define _GetSetPageOutCount PAGEABLE__GetSetPageOutCount

#undef  GetSetPageOutCount
#define GetSetPageOutCount PAGEABLE__GetSetPageOutCount

#undef  _GetSysPageCount
#define _GetSysPageCount PAGEABLE__GetSysPageCount

#undef  GetSysPageCount
#define GetSysPageCount PAGEABLE__GetSysPageCount

#undef  Set_HMA_Info
#define Set_HMA_Info PAGEABLE_Set_HMA_Info

#undef  Get_HMA_Info
#define Get_HMA_Info PAGEABLE_Get_HMA_Info

#undef  GetDetailedVMError
#define GetDetailedVMError PAGEABLE_GetDetailedVMError

#undef  SetDetailedVMError
#define SetDetailedVMError PAGEABLE_SetDetailedVMError

#undef  Get_PSP_Segment
#define Get_PSP_Segment PAGEABLE_Get_PSP_Segment

#undef  Get_Profile_String
#define Get_Profile_String PAGEABLE_Get_Profile_String

#undef  Get_Profile_Hex_Int
#define Get_Profile_Hex_Int PAGEABLE_Get_Profile_Hex_Int

#undef  Get_Profile_Fixed_Point
#define Get_Profile_Fixed_Point PAGEABLE_Get_Profile_Fixed_Point

#undef  Get_Profile_Decimal_Int
#define Get_Profile_Decimal_Int PAGEABLE_Get_Profile_Decimal_Int

#undef  Get_Profile_Boolean
#define Get_Profile_Boolean PAGEABLE_Get_Profile_Boolean

#undef  Get_PM_Int_Vector
#define Get_PM_Int_Vector PAGEABLE_Get_PM_Int_Vector

#undef  Get_PM_Int_Type
#define Get_PM_Int_Type PAGEABLE_Get_PM_Int_Type

#undef  _GetNulPageHandle
#define _GetNulPageHandle PAGEABLE__GetNulPageHandle

#undef  GetNulPageHandle
#define GetNulPageHandle PAGEABLE__GetNulPageHandle

#undef  Get_NMI_Handler_Addr
#define Get_NMI_Handler_Addr PAGEABLE_Get_NMI_Handler_Addr

#undef  Get_Next_VM_Handle
#define Get_Next_VM_Handle PAGEABLE_Get_Next_VM_Handle

#undef  Get_Next_Profile_String
#define Get_Next_Profile_String PAGEABLE_Get_Next_Profile_String

#undef  Get_Name_Of_Ugly_TSR
#define Get_Name_Of_Ugly_TSR PAGEABLE_Get_Name_Of_Ugly_TSR

#undef  Get_Mono_Cur_Pos
#define Get_Mono_Cur_Pos PAGEABLE_Get_Mono_Cur_Pos

#undef  Get_Mono_Chr
#define Get_Mono_Chr PAGEABLE_Get_Mono_Chr

#undef  Get_Machine_Info
#define Get_Machine_Info PAGEABLE_Get_Machine_Info

#undef  Get_Last_Updated_VM_Exec_Time
#define Get_Last_Updated_VM_Exec_Time PAGEABLE_Get_Last_Updated_VM_Exec_Time

#undef  _GetLastV86Page
#define _GetLastV86Page PAGEABLE__GetLastV86Page

#undef  GetLastV86Page
#define GetLastV86Page PAGEABLE__GetLastV86Page

#undef  Get_Last_Updated_System_Time
#define Get_Last_Updated_System_Time PAGEABLE_Get_Last_Updated_System_Time

#undef  _GetGlblRng0V86IntBase
#define _GetGlblRng0V86IntBase PAGEABLE__GetGlblRng0V86IntBase

#undef  GetGlblRng0V86IntBase
#define GetGlblRng0V86IntBase PAGEABLE__GetGlblRng0V86IntBase

#undef  _GetFreePageCount
#define _GetFreePageCount PAGEABLE__GetFreePageCount

#undef  GetFreePageCount
#define GetFreePageCount PAGEABLE__GetFreePageCount

#undef  Get_Fault_Hook_Addrs
#define Get_Fault_Hook_Addrs PAGEABLE_Get_Fault_Hook_Addrs

#undef  Get_Exec_Path
#define Get_Exec_Path PAGEABLE_Get_Exec_Path

#undef  Get_Execution_Focus
#define Get_Execution_Focus PAGEABLE_Get_Execution_Focus

#undef  Get_Environment_String
#define Get_Environment_String PAGEABLE_Get_Environment_String

#undef  _Get_Device_V86_Pages_Array
#define _Get_Device_V86_Pages_Array PAGEABLE__Get_Device_V86_Pages_Array

#undef  Get_Device_V86_Pages_Array
#define Get_Device_V86_Pages_Array PAGEABLE__Get_Device_V86_Pages_Array

#undef  _GetDescriptor
#define _GetDescriptor PAGEABLE__GetDescriptor

#undef  GetDescriptor
#define GetDescriptor PAGEABLE__GetDescriptor

#undef  _GetDemandPageInfo
#define _GetDemandPageInfo PAGEABLE__GetDemandPageInfo

#undef  GetDemandPageInfo
#define GetDemandPageInfo PAGEABLE__GetDemandPageInfo

#undef  Get_Debug_Options
#define Get_Debug_Options PAGEABLE_Get_Debug_Options

#undef  Get_Cur_VM_Handle
#define Get_Cur_VM_Handle PAGEABLE_Get_Cur_VM_Handle

#undef  Get_Crit_Status_No_Block
#define Get_Crit_Status_No_Block PAGEABLE_Get_Crit_Status_No_Block

#undef  Get_Crit_Section_Status
#define Get_Crit_Section_Status PAGEABLE_Get_Crit_Section_Status

#undef  Get_Cur_PM_App_CB
#define Get_Cur_PM_App_CB PAGEABLE_Get_Cur_PM_App_CB

#undef  Get_Config_Directory
#define Get_Config_Directory PAGEABLE_Get_Config_Directory

#undef  _GetFirstV86Page
#define _GetFirstV86Page PAGEABLE__GetFirstV86Page

#undef  GetFirstV86Page
#define GetFirstV86Page PAGEABLE__GetFirstV86Page

#undef  _GetAppFlatDSAlias
#define _GetAppFlatDSAlias PAGEABLE__GetAppFlatDSAlias

#undef  GetAppFlatDSAlias
#define GetAppFlatDSAlias PAGEABLE__GetAppFlatDSAlias

#undef  _Free_Temp_V86_Data_Area
#define _Free_Temp_V86_Data_Area PAGEABLE__Free_Temp_V86_Data_Area

#undef  Free_Temp_V86_Data_Area
#define Free_Temp_V86_Data_Area PAGEABLE__Free_Temp_V86_Data_Area

#undef  _Free_LDT_Selector
#define _Free_LDT_Selector PAGEABLE__Free_LDT_Selector

#undef  Free_LDT_Selector
#define Free_LDT_Selector PAGEABLE__Free_LDT_Selector

#undef  _Free_GDT_Selector
#define _Free_GDT_Selector PAGEABLE__Free_GDT_Selector

#undef  Free_GDT_Selector
#define Free_GDT_Selector PAGEABLE__Free_GDT_Selector

#undef  Fatal_Error_Handler
#define Fatal_Error_Handler PAGEABLE_Fatal_Error_Handler

#undef  Exec_VxD_Int
#define Exec_VxD_Int PAGEABLE_Exec_VxD_Int

#undef  Exec_Int
#define Exec_Int PAGEABLE_Exec_Int

#undef  End_Reentrant_Execution
#define End_Reentrant_Execution PAGEABLE_End_Reentrant_Execution

#undef  End_Crit_And_Suspend
#define End_Crit_And_Suspend PAGEABLE_End_Crit_And_Suspend

#undef  Enable_Local_Trapping
#define Enable_Local_Trapping PAGEABLE_Enable_Local_Trapping

#undef  Enable_Global_Trapping
#define Enable_Global_Trapping PAGEABLE_Enable_Global_Trapping

#undef  Disable_Local_Trapping
#define Disable_Local_Trapping PAGEABLE_Disable_Local_Trapping

#undef  Disable_Global_Trapping
#define Disable_Global_Trapping PAGEABLE_Disable_Global_Trapping

#undef  Destroy_Semaphore
#define Destroy_Semaphore PAGEABLE_Destroy_Semaphore

#undef  _DeAssign_Device_V86_Pages
#define _DeAssign_Device_V86_Pages PAGEABLE__DeAssign_Device_V86_Pages

#undef  DeAssign_Device_V86_Pages
#define DeAssign_Device_V86_Pages PAGEABLE__DeAssign_Device_V86_Pages

#undef  Close_VM
#define Close_VM PAGEABLE_Close_VM

#undef  Convert_Boolean_String
#define Convert_Boolean_String PAGEABLE_Convert_Boolean_String

#undef  Convert_Decimal_String
#define Convert_Decimal_String PAGEABLE_Convert_Decimal_String

#undef  Convert_Fixed_Point_String
#define Convert_Fixed_Point_String PAGEABLE_Convert_Fixed_Point_String

#undef  Convert_Hex_String
#define Convert_Hex_String PAGEABLE_Convert_Hex_String

#undef  _CopyPageTable
#define _CopyPageTable PAGEABLE__CopyPageTable

#undef  CopyPageTable
#define CopyPageTable PAGEABLE__CopyPageTable

#undef  Create_Semaphore
#define Create_Semaphore PAGEABLE_Create_Semaphore

#undef  _AddFreePhysPage
#define _AddFreePhysPage PAGEABLE__AddFreePhysPage

#undef  AddFreePhysPage
#define AddFreePhysPage PAGEABLE__AddFreePhysPage

#undef  _Add_Global_V86_Data_Area
#define _Add_Global_V86_Data_Area PAGEABLE__Add_Global_V86_Data_Area

#undef  Add_Global_V86_Data_Area
#define Add_Global_V86_Data_Area PAGEABLE__Add_Global_V86_Data_Area

#undef  _AddInstanceItem
#define _AddInstanceItem PAGEABLE__AddInstanceItem

#undef  AddInstanceItem
#define AddInstanceItem PAGEABLE__AddInstanceItem

#undef  Adjust_Exec_Priority
#define Adjust_Exec_Priority PAGEABLE_Adjust_Exec_Priority

#undef  Adjust_Execution_Time
#define Adjust_Execution_Time PAGEABLE_Adjust_Execution_Time

#undef  _Allocate_Device_CB_Area
#define _Allocate_Device_CB_Area PAGEABLE__Allocate_Device_CB_Area

#undef  Allocate_Device_CB_Area
#define Allocate_Device_CB_Area PAGEABLE__Allocate_Device_CB_Area

#undef  _Allocate_GDT_Selector
#define _Allocate_GDT_Selector PAGEABLE__Allocate_GDT_Selector

#undef  Allocate_GDT_Selector
#define Allocate_GDT_Selector PAGEABLE__Allocate_GDT_Selector

#undef  _Allocate_Global_V86_Data_Area
#define _Allocate_Global_V86_Data_Area PAGEABLE__Allocate_Global_V86_Data_Area

#undef  Allocate_Global_V86_Data_Area
#define Allocate_Global_V86_Data_Area PAGEABLE__Allocate_Global_V86_Data_Area

#undef  _Allocate_LDT_Selector
#define _Allocate_LDT_Selector PAGEABLE__Allocate_LDT_Selector

#undef  Allocate_LDT_Selector
#define Allocate_LDT_Selector PAGEABLE__Allocate_LDT_Selector

#undef  Allocate_PM_Call_Back
#define Allocate_PM_Call_Back PAGEABLE_Allocate_PM_Call_Back

#undef  _Allocate_Temp_V86_Data_Area
#define _Allocate_Temp_V86_Data_Area PAGEABLE__Allocate_Temp_V86_Data_Area

#undef  Allocate_Temp_V86_Data_Area
#define Allocate_Temp_V86_Data_Area PAGEABLE__Allocate_Temp_V86_Data_Area

#undef  Allocate_V86_Call_Back
#define Allocate_V86_Call_Back PAGEABLE_Allocate_V86_Call_Back

#undef  _Assign_Device_V86_Pages
#define _Assign_Device_V86_Pages PAGEABLE__Assign_Device_V86_Pages

#undef  Assign_Device_V86_Pages
#define Assign_Device_V86_Pages PAGEABLE__Assign_Device_V86_Pages

#undef  Begin_Critical_Section
#define Begin_Critical_Section PAGEABLE_Begin_Critical_Section

#undef  Begin_Reentrant_Execution
#define Begin_Reentrant_Execution PAGEABLE_Begin_Reentrant_Execution

#undef  _BuildDescriptorDWORDs
#define _BuildDescriptorDWORDs PAGEABLE__BuildDescriptorDWORDs

#undef  BuildDescriptorDWORDs
#define BuildDescriptorDWORDs PAGEABLE__BuildDescriptorDWORDs

#undef  Build_Int_Stack_Frame
#define Build_Int_Stack_Frame PAGEABLE_Build_Int_Stack_Frame

#undef  Call_Global_Event
#define Call_Global_Event PAGEABLE_Call_Global_Event

#undef  Call_Priority_VM_Event
#define Call_Priority_VM_Event PAGEABLE_Call_Priority_VM_Event

#undef  Call_VM_Event
#define Call_VM_Event PAGEABLE_Call_VM_Event

#undef  Call_When_Idle
#define Call_When_Idle PAGEABLE_Call_When_Idle

#undef  Call_When_Not_Critical
#define Call_When_Not_Critical PAGEABLE_Call_When_Not_Critical

#undef  Call_When_Task_Switched
#define Call_When_Task_Switched PAGEABLE_Call_When_Task_Switched

#undef  Call_When_VM_Ints_Enabled
#define Call_When_VM_Ints_Enabled PAGEABLE_Call_When_VM_Ints_Enabled

#undef  Call_When_VM_Returns
#define Call_When_VM_Returns PAGEABLE_Call_When_VM_Returns

#undef  Cancel_Global_Event
#define Cancel_Global_Event PAGEABLE_Cancel_Global_Event

#undef  Cancel_Priority_VM_Event
#define Cancel_Priority_VM_Event PAGEABLE_Cancel_Priority_VM_Event

#undef  Cancel_Time_Out
#define Cancel_Time_Out PAGEABLE_Cancel_Time_Out

#undef  Cancel_VM_Event
#define Cancel_VM_Event PAGEABLE_Cancel_VM_Event

#undef  Claim_Critical_Section
#define Claim_Critical_Section PAGEABLE_Claim_Critical_Section

#ifdef WIN40_OR_LATER
#undef  _RegOpenKey
#define _RegOpenKey PAGEABLE__RegOpenKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegOpenKey
#define RegOpenKey PAGEABLE__RegOpenKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCreateKey
#define _RegCreateKey PAGEABLE__RegCreateKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCreateKey
#define RegCreateKey PAGEABLE__RegCreateKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegDeleteKey
#define _RegDeleteKey PAGEABLE__RegDeleteKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegDeleteKey
#define RegDeleteKey PAGEABLE__RegDeleteKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegDeleteValue
#define _RegDeleteValue PAGEABLE__RegDeleteValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegDeleteValue
#define RegDeleteValue PAGEABLE__RegDeleteValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegEnumKey
#define _RegEnumKey PAGEABLE__RegEnumKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegEnumKey
#define RegEnumKey PAGEABLE__RegEnumKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegEnumValue
#define _RegEnumValue PAGEABLE__RegEnumValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegEnumValue
#define RegEnumValue PAGEABLE__RegEnumValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegFlushKey
#define _RegFlushKey PAGEABLE__RegFlushKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegFlushKey
#define RegFlushKey PAGEABLE__RegFlushKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCloseKey
#define _RegCloseKey PAGEABLE__RegCloseKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCloseKey
#define RegCloseKey PAGEABLE__RegCloseKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryValue
#define _RegQueryValue PAGEABLE__RegQueryValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryValue
#define RegQueryValue PAGEABLE__RegQueryValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryValueEx
#define _RegQueryValueEx PAGEABLE__RegQueryValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryValueEx
#define RegQueryValueEx PAGEABLE__RegQueryValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  _RegSetValue
#define _RegSetValue PAGEABLE__RegSetValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegSetValue
#define RegSetValue PAGEABLE__RegSetValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegSetValueEx
#define _RegSetValueEx PAGEABLE__RegSetValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  RegSetValueEx
#define RegSetValueEx PAGEABLE__RegSetValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  _EnterMutex
#define _EnterMutex PAGEABLE__EnterMutex
#endif

#ifdef WIN40_OR_LATER
#undef  EnterMutex
#define EnterMutex PAGEABLE__EnterMutex
#endif

#ifdef WIN40_OR_LATER
#undef  _GetMutexOwner
#define _GetMutexOwner PAGEABLE__GetMutexOwner
#endif

#ifdef WIN40_OR_LATER
#undef  GetMutexOwner
#define GetMutexOwner PAGEABLE__GetMutexOwner
#endif

#ifdef WIN40_OR_LATER
#undef  _LeaveMutex
#define _LeaveMutex PAGEABLE__LeaveMutex
#endif

#ifdef WIN40_OR_LATER
#undef  LeaveMutex
#define LeaveMutex PAGEABLE__LeaveMutex
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Fault
#define Unhook_V86_Fault PAGEABLE_Unhook_V86_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_PM_Fault
#define Unhook_PM_Fault PAGEABLE_Unhook_PM_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_VMM_Fault
#define Unhook_VMM_Fault PAGEABLE_Unhook_VMM_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Int_Chain
#define Unhook_V86_Int_Chain PAGEABLE_Unhook_V86_Int_Chain
#endif

#ifdef WIN40_OR_LATER
#undef  Signal_Semaphore_No_Switch
#define Signal_Semaphore_No_Switch PAGEABLE_Signal_Semaphore_No_Switch
#endif

#ifdef WIN40_OR_LATER
#undef  Remove_IO_Handler
#define Remove_IO_Handler PAGEABLE_Remove_IO_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_NMI_Event
#define Unhook_NMI_Event PAGEABLE_Unhook_NMI_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Not_Critical
#define Cancel_Call_When_Not_Critical PAGEABLE_Cancel_Call_When_Not_Critical
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service
#define Unhook_Device_Service PAGEABLE_Unhook_Device_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Async_Time_Out
#define Set_Async_Time_Out PAGEABLE_Set_Async_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  Get_DDB
#define Get_DDB PAGEABLE_Get_DDB
#endif

#ifdef WIN40_OR_LATER
#undef  _Trace_Out_Service
#define _Trace_Out_Service PAGEABLE__Trace_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Trace_Out_Service
#define Trace_Out_Service PAGEABLE__Trace_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Out_Service
#define _Debug_Out_Service PAGEABLE__Debug_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Out_Service
#define Debug_Out_Service PAGEABLE__Debug_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _AllocateThreadDataSlot
#define _AllocateThreadDataSlot PAGEABLE__AllocateThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  AllocateThreadDataSlot
#define AllocateThreadDataSlot PAGEABLE__AllocateThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  _FreeThreadDataSlot
#define _FreeThreadDataSlot PAGEABLE__FreeThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  FreeThreadDataSlot
#define FreeThreadDataSlot PAGEABLE__FreeThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  _GetLastUpdatedThreadExecTime
#define _GetLastUpdatedThreadExecTime PAGEABLE__GetLastUpdatedThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  GetLastUpdatedThreadExecTime
#define GetLastUpdatedThreadExecTime PAGEABLE__GetLastUpdatedThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  VMMAddImportModuleName
#define VMMAddImportModuleName PAGEABLE_VMMAddImportModuleName
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_Add_DDB
#define VMM_Add_DDB PAGEABLE_VMM_Add_DDB
#endif

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
#undef  VMM_Remove_DDB
#define VMM_Remove_DDB PAGEABLE_VMM_Remove_DDB
#endif

#ifdef WIN40_OR_LATER
#undef  Test_VM_Ints_Enabled
#define Test_VM_Ints_Enabled PAGEABLE_Test_VM_Ints_Enabled
#endif

#ifdef WIN40_OR_LATER
#undef  _BlockOnID
#define _BlockOnID PAGEABLE__BlockOnID
#endif

#ifdef WIN40_OR_LATER
#undef  BlockOnID
#define BlockOnID PAGEABLE__BlockOnID
#endif

#ifdef WIN40_OR_LATER
#undef  Schedule_Thread_Event
#define Schedule_Thread_Event PAGEABLE_Schedule_Thread_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Thread_Event
#define Cancel_Thread_Event PAGEABLE_Cancel_Thread_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Time_Out
#define Set_Thread_Time_Out PAGEABLE_Set_Thread_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  _CreateMutex
#define _CreateMutex PAGEABLE__CreateMutex
#endif

#ifdef WIN40_OR_LATER
#undef  CreateMutex
#define CreateMutex PAGEABLE__CreateMutex
#endif

#ifdef WIN40_OR_LATER
#undef  _DestroyMutex
#define _DestroyMutex PAGEABLE__DestroyMutex
#endif

#ifdef WIN40_OR_LATER
#undef  DestroyMutex
#define DestroyMutex PAGEABLE__DestroyMutex
#endif

#ifdef WIN40_OR_LATER
#undef  Call_When_Thread_Switched
#define Call_When_Thread_Switched PAGEABLE_Call_When_Thread_Switched
#endif

#ifdef WIN40_OR_LATER
#undef  VMMCreateThread
#define VMMCreateThread PAGEABLE_VMMCreateThread
#endif

#ifdef WIN40_OR_LATER
#undef  _GetThreadExecTime
#define _GetThreadExecTime PAGEABLE__GetThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  GetThreadExecTime
#define GetThreadExecTime PAGEABLE__GetThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  VMMTerminateThread
#define VMMTerminateThread PAGEABLE_VMMTerminateThread
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Cur_Thread_Handle
#define Get_Cur_Thread_Handle PAGEABLE_Get_Cur_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Cur_Thread_Handle
#define Test_Cur_Thread_Handle PAGEABLE_Test_Cur_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Sys_Thread_Handle
#define Get_Sys_Thread_Handle PAGEABLE_Get_Sys_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Sys_Thread_Handle
#define Test_Sys_Thread_Handle PAGEABLE_Test_Sys_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Validate_Thread_Handle
#define Validate_Thread_Handle PAGEABLE_Validate_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Initial_Thread_Handle
#define Get_Initial_Thread_Handle PAGEABLE_Get_Initial_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Initial_Thread_Handle
#define Test_Initial_Thread_Handle PAGEABLE_Test_Initial_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Valid_Thread_Handle
#define Debug_Test_Valid_Thread_Handle PAGEABLE_Debug_Test_Valid_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Cur_Thread
#define Debug_Test_Cur_Thread PAGEABLE_Debug_Test_Cur_Thread
#endif

#undef  VMM_GetSystemInitState
#define VMM_GetSystemInitState PAGEABLE_VMM_GetSystemInitState

#undef  Cancel_Call_When_Thread_Switched
#define Cancel_Call_When_Thread_Switched PAGEABLE_Cancel_Call_When_Thread_Switched

#undef  Get_Next_Thread_Handle
#define Get_Next_Thread_Handle PAGEABLE_Get_Next_Thread_Handle

#ifdef WIN40_OR_LATER
#undef  Adjust_Thread_Exec_Priority
#define Adjust_Thread_Exec_Priority PAGEABLE_Adjust_Thread_Exec_Priority
#endif

#ifdef WIN40_OR_LATER
#undef  _Deallocate_Device_CB_Area
#define _Deallocate_Device_CB_Area PAGEABLE__Deallocate_Device_CB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  Deallocate_Device_CB_Area
#define Deallocate_Device_CB_Area PAGEABLE__Deallocate_Device_CB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  Remove_Mult_IO_Handlers
#define Remove_Mult_IO_Handlers PAGEABLE_Remove_Mult_IO_Handlers
#endif

#ifdef WIN40_OR_LATER
#undef  _PageReserve
#define _PageReserve PAGEABLE__PageReserve
#endif

#ifdef WIN40_OR_LATER
#undef  PageReserve
#define PageReserve PAGEABLE__PageReserve
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommit
#define _PageCommit PAGEABLE__PageCommit
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommit
#define PageCommit PAGEABLE__PageCommit
#endif

#ifdef WIN40_OR_LATER
#undef  _PageDecommit
#define _PageDecommit PAGEABLE__PageDecommit
#endif

#ifdef WIN40_OR_LATER
#undef  PageDecommit
#define PageDecommit PAGEABLE__PageDecommit
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerRegister
#define _PagerRegister PAGEABLE__PagerRegister
#endif

#ifdef WIN40_OR_LATER
#undef  PagerRegister
#define PagerRegister PAGEABLE__PagerRegister
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerQuery
#define _PagerQuery PAGEABLE__PagerQuery
#endif

#ifdef WIN40_OR_LATER
#undef  PagerQuery
#define PagerQuery PAGEABLE__PagerQuery
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerDeregister
#define _PagerDeregister PAGEABLE__PagerDeregister
#endif

#ifdef WIN40_OR_LATER
#undef  PagerDeregister
#define PagerDeregister PAGEABLE__PagerDeregister
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextCreate
#define _ContextCreate PAGEABLE__ContextCreate
#endif

#ifdef WIN40_OR_LATER
#undef  ContextCreate
#define ContextCreate PAGEABLE__ContextCreate
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextDestroy
#define _ContextDestroy PAGEABLE__ContextDestroy
#endif

#ifdef WIN40_OR_LATER
#undef  ContextDestroy
#define ContextDestroy PAGEABLE__ContextDestroy
#endif

#ifdef WIN40_OR_LATER
#undef  _PageAttach
#define _PageAttach PAGEABLE__PageAttach
#endif

#ifdef WIN40_OR_LATER
#undef  PageAttach
#define PageAttach PAGEABLE__PageAttach
#endif

#ifdef WIN40_OR_LATER
#undef  _PageFlush
#define _PageFlush PAGEABLE__PageFlush
#endif

#ifdef WIN40_OR_LATER
#undef  PageFlush
#define PageFlush PAGEABLE__PageFlush
#endif

#ifdef WIN40_OR_LATER
#undef  _SignalID
#define _SignalID PAGEABLE__SignalID
#endif

#ifdef WIN40_OR_LATER
#undef  SignalID
#define SignalID PAGEABLE__SignalID
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommitPhys
#define _PageCommitPhys PAGEABLE__PageCommitPhys
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommitPhys
#define PageCommitPhys PAGEABLE__PageCommitPhys
#endif

#ifdef WIN40_OR_LATER
#undef  _Register_Win32_Services
#define _Register_Win32_Services PAGEABLE__Register_Win32_Services
#endif

#ifdef WIN40_OR_LATER
#undef  Register_Win32_Services
#define Register_Win32_Services PAGEABLE__Register_Win32_Services
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Idle
#define Cancel_Call_When_Idle PAGEABLE_Cancel_Call_When_Idle
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Task_Switched
#define Cancel_Call_When_Task_Switched PAGEABLE_Cancel_Call_When_Task_Switched
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Inst_V86_Int_Vec_Base
#define Get_Inst_V86_Int_Vec_Base PAGEABLE_Get_Inst_V86_Int_Vec_Base
#endif

#ifdef WIN40_OR_LATER
#undef  Simulate_VM_IO
#define Simulate_VM_IO PAGEABLE_Simulate_VM_IO
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextSwitch
#define _ContextSwitch PAGEABLE__ContextSwitch
#endif

#ifdef WIN40_OR_LATER
#undef  ContextSwitch
#define ContextSwitch PAGEABLE__ContextSwitch
#endif

#ifdef WIN40_OR_LATER
#undef  _PageModifyPermissions
#define _PageModifyPermissions PAGEABLE__PageModifyPermissions
#endif

#ifdef WIN40_OR_LATER
#undef  PageModifyPermissions
#define PageModifyPermissions PAGEABLE__PageModifyPermissions
#endif

#ifdef WIN40_OR_LATER
#undef  _PageQuery
#define _PageQuery PAGEABLE__PageQuery
#endif

#ifdef WIN40_OR_LATER
#undef  PageQuery
#define PageQuery PAGEABLE__PageQuery
#endif

#ifdef WIN40_OR_LATER
#undef  _GetThreadTerminationStatus
#define _GetThreadTerminationStatus PAGEABLE__GetThreadTerminationStatus
#endif

#ifdef WIN40_OR_LATER
#undef  GetThreadTerminationStatus
#define GetThreadTerminationStatus PAGEABLE__GetThreadTerminationStatus
#endif

#ifdef WIN40_OR_LATER
#undef  _GetInstanceInfo
#define _GetInstanceInfo PAGEABLE__GetInstanceInfo
#endif

#ifdef WIN40_OR_LATER
#undef  GetInstanceInfo
#define GetInstanceInfo PAGEABLE__GetInstanceInfo
#endif

#ifdef WIN40_OR_LATER
#undef  _ExecIntMustComplete
#define _ExecIntMustComplete PAGEABLE__ExecIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ExecIntMustComplete
#define ExecIntMustComplete PAGEABLE__ExecIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _ExecVxDIntMustComplete
#define _ExecVxDIntMustComplete PAGEABLE__ExecVxDIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ExecVxDIntMustComplete
#define ExecVxDIntMustComplete PAGEABLE__ExecVxDIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  Begin_V86_Serialization
#define Begin_V86_Serialization PAGEABLE_Begin_V86_Serialization
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Page
#define Unhook_V86_Page PAGEABLE_Unhook_V86_Page
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_GetVxDLocationList
#define VMM_GetVxDLocationList PAGEABLE_VMM_GetVxDLocationList
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_GetDDBList
#define VMM_GetDDBList PAGEABLE_VMM_GetDDBList
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Instanced_V86_Int_Vector
#define Get_Instanced_V86_Int_Vector PAGEABLE_Get_Instanced_V86_Int_Vector
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Set_Real_DOS_PSP
#define Get_Set_Real_DOS_PSP PAGEABLE_Get_Set_Real_DOS_PSP
#endif

#ifdef WIN40_OR_LATER
#undef  Get_System_Time_Address
#define Get_System_Time_Address PAGEABLE_Get_System_Time_Address
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Crit_Status_Thread
#define Get_Crit_Status_Thread PAGEABLE_Get_Crit_Status_Thread
#endif

#ifdef WIN40_OR_LATER
#undef  Directed_Sys_Control
#define Directed_Sys_Control PAGEABLE_Directed_Sys_Control
#endif

#ifdef WIN40_OR_LATER
#undef  _CallRing3
#define _CallRing3 PAGEABLE__CallRing3
#endif

#ifdef WIN40_OR_LATER
#undef  CallRing3
#define CallRing3 PAGEABLE__CallRing3
#endif

#ifdef WIN40_OR_LATER
#undef  Exec_PM_Int
#define Exec_PM_Int PAGEABLE_Exec_PM_Int
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommitContig
#define _PageCommitContig PAGEABLE__PageCommitContig
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommitContig
#define PageCommitContig PAGEABLE__PageCommitContig
#endif

#ifdef WIN40_OR_LATER
#undef  _GetCurrentContext
#define _GetCurrentContext PAGEABLE__GetCurrentContext
#endif

#ifdef WIN40_OR_LATER
#undef  GetCurrentContext
#define GetCurrentContext PAGEABLE__GetCurrentContext
#endif

#ifdef WIN40_OR_LATER
#undef  Call_Restricted_Event
#define Call_Restricted_Event PAGEABLE_Call_Restricted_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Restricted_Event
#define Cancel_Restricted_Event PAGEABLE_Cancel_Restricted_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Register_PEF_Provider
#define Register_PEF_Provider PAGEABLE_Register_PEF_Provider
#endif

#ifdef WIN40_OR_LATER
#undef  _GetPhysPageInfo
#define _GetPhysPageInfo PAGEABLE__GetPhysPageInfo
#endif

#ifdef WIN40_OR_LATER
#undef  GetPhysPageInfo
#define GetPhysPageInfo PAGEABLE__GetPhysPageInfo
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryInfoKey
#define _RegQueryInfoKey PAGEABLE__RegQueryInfoKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryInfoKey
#define RegQueryInfoKey PAGEABLE__RegQueryInfoKey
#endif

#ifdef WIN40_OR_LATER
#undef  Time_Slice_Sleep
#define Time_Slice_Sleep PAGEABLE_Time_Slice_Sleep
#endif

#ifdef WIN40_OR_LATER
#undef  Boost_With_Decay
#define Boost_With_Decay PAGEABLE_Boost_With_Decay
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Inversion_Pri
#define Set_Inversion_Pri PAGEABLE_Set_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Reset_Inversion_Pri
#define Reset_Inversion_Pri PAGEABLE_Reset_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Release_Inversion_Pri
#define Release_Inversion_Pri PAGEABLE_Release_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Thread_Win32_Pri
#define Get_Thread_Win32_Pri PAGEABLE_Get_Thread_Win32_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Win32_Pri
#define Set_Thread_Win32_Pri PAGEABLE_Set_Thread_Win32_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Static_Boost
#define Set_Thread_Static_Boost PAGEABLE_Set_Thread_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  Set_VM_Static_Boost
#define Set_VM_Static_Boost PAGEABLE_Set_VM_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  Release_Inversion_Pri_ID
#define Release_Inversion_Pri_ID PAGEABLE_Release_Inversion_Pri_ID
#endif

#ifdef WIN40_OR_LATER
#undef  Attach_Thread_To_Group
#define Attach_Thread_To_Group PAGEABLE_Attach_Thread_To_Group
#endif

#ifdef WIN40_OR_LATER
#undef  Detach_Thread_From_Group
#define Detach_Thread_From_Group PAGEABLE_Detach_Thread_From_Group
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Group_Static_Boost
#define Set_Group_Static_Boost PAGEABLE_Set_Group_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  _GetRegistryPath
#define _GetRegistryPath PAGEABLE__GetRegistryPath
#endif

#ifdef WIN40_OR_LATER
#undef  GetRegistryPath
#define GetRegistryPath PAGEABLE__GetRegistryPath
#endif

#ifdef WIN40_OR_LATER
#undef  _RegRemapPreDefKey
#define _RegRemapPreDefKey PAGEABLE__RegRemapPreDefKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegRemapPreDefKey
#define RegRemapPreDefKey PAGEABLE__RegRemapPreDefKey
#endif

#ifdef WIN40_OR_LATER
#undef  End_V86_Serialization
#define End_V86_Serialization PAGEABLE_End_V86_Serialization
#endif

#ifdef WIN40_OR_LATER
#undef  _Assert_Range
#define _Assert_Range PAGEABLE__Assert_Range
#endif

#ifdef WIN40_OR_LATER
#undef  Assert_Range
#define Assert_Range PAGEABLE__Assert_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _PageChangePager
#define _PageChangePager PAGEABLE__PageChangePager
#endif

#ifdef WIN40_OR_LATER
#undef  PageChangePager
#define PageChangePager PAGEABLE__PageChangePager
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCreateDynKey
#define _RegCreateDynKey PAGEABLE__RegCreateDynKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCreateDynKey
#define RegCreateDynKey PAGEABLE__RegCreateDynKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryMultipleValues
#define _RegQueryMultipleValues PAGEABLE__RegQueryMultipleValues
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryMultipleValues
#define RegQueryMultipleValues PAGEABLE__RegQueryMultipleValues
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Boot_Flags
#define Get_Boot_Flags PAGEABLE_Get_Boot_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Boot_Flags
#define Set_Boot_Flags PAGEABLE_Set_Boot_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrcpyn
#define _lstrcpyn PAGEABLE__lstrcpyn
#endif

#ifdef WIN40_OR_LATER
#undef  lstrcpyn
#define lstrcpyn PAGEABLE__lstrcpyn
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrlen
#define _lstrlen PAGEABLE__lstrlen
#endif

#ifdef WIN40_OR_LATER
#undef  lstrlen
#define lstrlen PAGEABLE__lstrlen
#endif

#ifdef WIN40_OR_LATER
#undef  _lmemcpy
#define _lmemcpy PAGEABLE__lmemcpy
#endif

#ifdef WIN40_OR_LATER
#undef  lmemcpy
#define lmemcpy PAGEABLE__lmemcpy
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrcmpi
#define _lstrcmpi PAGEABLE__lstrcmpi
#endif

#ifdef WIN40_OR_LATER
#undef  lstrcmpi
#define lstrcmpi PAGEABLE__lstrcmpi
#endif

#ifdef WIN40_OR_LATER
#undef  _GetVxDName
#define _GetVxDName PAGEABLE__GetVxDName
#endif

#ifdef WIN40_OR_LATER
#undef  GetVxDName
#define GetVxDName PAGEABLE__GetVxDName
#endif

#undef  BlockDev_Command_Complete
#define BlockDev_Command_Complete PAGEABLE_BlockDev_Command_Complete

#undef  BlockDev_Find_Int13_Drive
#define BlockDev_Find_Int13_Drive PAGEABLE_BlockDev_Find_Int13_Drive

#undef  BlockDev_Get_Device_List
#define BlockDev_Get_Device_List PAGEABLE_BlockDev_Get_Device_List

#undef  BlockDev_Get_Version
#define BlockDev_Get_Version PAGEABLE_BlockDev_Get_Version

#undef  BlockDev_Register_Device
#define BlockDev_Register_Device PAGEABLE_BlockDev_Register_Device

#undef  BlockDev_Send_Command
#define BlockDev_Send_Command PAGEABLE_BlockDev_Send_Command

#undef  BlockDev_Synchronous_Command
#define BlockDev_Synchronous_Command PAGEABLE_BlockDev_Synchronous_Command

#undef  BlockDev_Thunk_Sync_Cmd
#define BlockDev_Thunk_Sync_Cmd PAGEABLE_BlockDev_Thunk_Sync_Cmd

#undef  BlockDev_Thunk_Command
#define BlockDev_Thunk_Command PAGEABLE_BlockDev_Thunk_Command

#undef  BlockDev_Thunk_HW_Int
#define BlockDev_Thunk_HW_Int PAGEABLE_BlockDev_Thunk_HW_Int

#undef  DOSMGR_Add_Device
#define DOSMGR_Add_Device PAGEABLE_DOSMGR_Add_Device

#undef  DOSMGR_BackFill_Allowed
#define DOSMGR_BackFill_Allowed PAGEABLE_DOSMGR_BackFill_Allowed

#undef  DOSMGR_Copy_VM_Drive_State
#define DOSMGR_Copy_VM_Drive_State PAGEABLE_DOSMGR_Copy_VM_Drive_State

#undef  DOSMGR_Enable_Indos_Polling
#define DOSMGR_Enable_Indos_Polling PAGEABLE_DOSMGR_Enable_Indos_Polling

#undef  _DOSMGR_Exec_VM
#define _DOSMGR_Exec_VM PAGEABLE__DOSMGR_Exec_VM

#undef  DOSMGR_Exec_VM
#define DOSMGR_Exec_VM PAGEABLE__DOSMGR_Exec_VM

#undef  DOSMGR_Get_DOS_Crit_Status
#define DOSMGR_Get_DOS_Crit_Status PAGEABLE_DOSMGR_Get_DOS_Crit_Status

#undef  DOSMGR_Get_IndosPtr
#define DOSMGR_Get_IndosPtr PAGEABLE_DOSMGR_Get_IndosPtr

#undef  DOSMGR_Get_Version
#define DOSMGR_Get_Version PAGEABLE_DOSMGR_Get_Version

#undef  DOSMGR_Instance_Device
#define DOSMGR_Instance_Device PAGEABLE_DOSMGR_Instance_Device

#undef  DOSMGR_LocalGlobalReg
#define DOSMGR_LocalGlobalReg PAGEABLE_DOSMGR_LocalGlobalReg

#undef  DOSMGR_Remove_Device
#define DOSMGR_Remove_Device PAGEABLE_DOSMGR_Remove_Device

#undef  _DOSMGR_Set_Exec_VM_Data
#define _DOSMGR_Set_Exec_VM_Data PAGEABLE__DOSMGR_Set_Exec_VM_Data

#undef  DOSMGR_Set_Exec_VM_Data
#define DOSMGR_Set_Exec_VM_Data PAGEABLE__DOSMGR_Set_Exec_VM_Data

#undef  DOSNET_Do_PSP_Adjust
#define DOSNET_Do_PSP_Adjust PAGEABLE_DOSNET_Do_PSP_Adjust

#undef  DOSNET_Get_Version
#define DOSNET_Get_Version PAGEABLE_DOSNET_Get_Version

#undef  DOSNET_Send_FILESYSCHANGE
#define DOSNET_Send_FILESYSCHANGE PAGEABLE_DOSNET_Send_FILESYSCHANGE

#undef  EBIOS_Get_Unused_Mem
#define EBIOS_Get_Unused_Mem PAGEABLE_EBIOS_Get_Unused_Mem

#undef  EBIOS_Get_Version
#define EBIOS_Get_Version PAGEABLE_EBIOS_Get_Version

#undef  Int13_Device_Registered
#define Int13_Device_Registered PAGEABLE_Int13_Device_Registered

#undef  Int13_Hooking_BIOS_Int
#define Int13_Hooking_BIOS_Int PAGEABLE_Int13_Hooking_BIOS_Int

#undef  Int13_Translate_VM_Int
#define Int13_Translate_VM_Int PAGEABLE_Int13_Translate_VM_Int

#undef  Int13_Unhooking_BIOS_Int
#define Int13_Unhooking_BIOS_Int PAGEABLE_Int13_Unhooking_BIOS_Int

#undef  VMD_Get_Mouse_Owner
#define VMD_Get_Mouse_Owner PAGEABLE_VMD_Get_Mouse_Owner

#undef  VMD_Get_Version
#define VMD_Get_Version PAGEABLE_VMD_Get_Version

#undef  VMD_Set_Mouse_Type
#define VMD_Set_Mouse_Type PAGEABLE_VMD_Set_Mouse_Type

#undef  SHELL_Event
#define SHELL_Event PAGEABLE_SHELL_Event

#undef  SHELL_Get_Version
#define SHELL_Get_Version PAGEABLE_SHELL_Get_Version

#undef  SHELL_GetVMInfo
#define SHELL_GetVMInfo PAGEABLE_SHELL_GetVMInfo

#undef  SHELL_Message
#define SHELL_Message PAGEABLE_SHELL_Message

#undef  SHELL_Resolve_Contention
#define SHELL_Resolve_Contention PAGEABLE_SHELL_Resolve_Contention

#undef  SHELL_SYSMODAL_Message
#define SHELL_SYSMODAL_Message PAGEABLE_SHELL_SYSMODAL_Message

#ifdef WIN40_OR_LATER
#undef  _SHELL_CallAtAppyTime
#define _SHELL_CallAtAppyTime PAGEABLE__SHELL_CallAtAppyTime
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CallAtAppyTime
#define SHELL_CallAtAppyTime PAGEABLE__SHELL_CallAtAppyTime
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_CancelAppyTimeEvent
#define _SHELL_CancelAppyTimeEvent PAGEABLE__SHELL_CancelAppyTimeEvent
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CancelAppyTimeEvent
#define SHELL_CancelAppyTimeEvent PAGEABLE__SHELL_CancelAppyTimeEvent
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_QueryAppyTimeAvailable
#define _SHELL_QueryAppyTimeAvailable PAGEABLE__SHELL_QueryAppyTimeAvailable
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_QueryAppyTimeAvailable
#define SHELL_QueryAppyTimeAvailable PAGEABLE__SHELL_QueryAppyTimeAvailable
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LocalAllocEx
#define _SHELL_LocalAllocEx PAGEABLE__SHELL_LocalAllocEx
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LocalAllocEx
#define SHELL_LocalAllocEx PAGEABLE__SHELL_LocalAllocEx
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LocalFree
#define _SHELL_LocalFree PAGEABLE__SHELL_LocalFree
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LocalFree
#define SHELL_LocalFree PAGEABLE__SHELL_LocalFree
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_CallDll
#define _SHELL_CallDll PAGEABLE__SHELL_CallDll
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CallDll
#define SHELL_CallDll PAGEABLE__SHELL_CallDll
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_BroadcastSystemMessage
#define _SHELL_BroadcastSystemMessage PAGEABLE__SHELL_BroadcastSystemMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_BroadcastSystemMessage
#define SHELL_BroadcastSystemMessage PAGEABLE__SHELL_BroadcastSystemMessage
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_HookSystemBroadcast
#define _SHELL_HookSystemBroadcast PAGEABLE__SHELL_HookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_HookSystemBroadcast
#define SHELL_HookSystemBroadcast PAGEABLE__SHELL_HookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_UnhookSystemBroadcast
#define _SHELL_UnhookSystemBroadcast PAGEABLE__SHELL_UnhookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_UnhookSystemBroadcast
#define SHELL_UnhookSystemBroadcast PAGEABLE__SHELL_UnhookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_FreeLibrary
#define _SHELL_FreeLibrary PAGEABLE__SHELL_FreeLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_FreeLibrary
#define SHELL_FreeLibrary PAGEABLE__SHELL_FreeLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_GetProcAddress
#define _SHELL_GetProcAddress PAGEABLE__SHELL_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_GetProcAddress
#define SHELL_GetProcAddress PAGEABLE__SHELL_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LoadLibrary
#define _SHELL_LoadLibrary PAGEABLE__SHELL_LoadLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LoadLibrary
#define SHELL_LoadLibrary PAGEABLE__SHELL_LoadLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_PostMessage
#define _SHELL_PostMessage PAGEABLE__SHELL_PostMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_PostMessage
#define SHELL_PostMessage PAGEABLE__SHELL_PostMessage
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_PostShellMessage
#define _SHELL_PostShellMessage PAGEABLE__SHELL_PostShellMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_PostShellMessage
#define SHELL_PostShellMessage PAGEABLE__SHELL_PostShellMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Hook_Properties
#define SHELL_Hook_Properties PAGEABLE_SHELL_Hook_Properties
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Unhook_Properties
#define SHELL_Unhook_Properties PAGEABLE_SHELL_Unhook_Properties
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_ShellExecute
#define _SHELL_ShellExecute PAGEABLE__SHELL_ShellExecute
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_ShellExecute
#define SHELL_ShellExecute PAGEABLE__SHELL_ShellExecute
#endif

#undef  V86MMGR_Allocate_Buffer
#define V86MMGR_Allocate_Buffer PAGEABLE_V86MMGR_Allocate_Buffer

#undef  V86MMGR_Allocate_V86_Pages
#define V86MMGR_Allocate_V86_Pages PAGEABLE_V86MMGR_Allocate_V86_Pages

#undef  V86MMGR_Free_Buffer
#define V86MMGR_Free_Buffer PAGEABLE_V86MMGR_Free_Buffer

#undef  V86MMGR_Free_Page_Map_Region
#define V86MMGR_Free_Page_Map_Region PAGEABLE_V86MMGR_Free_Page_Map_Region

#undef  V86MMGR_Get_EMS_XMS_Limits
#define V86MMGR_Get_EMS_XMS_Limits PAGEABLE_V86MMGR_Get_EMS_XMS_Limits

#undef  V86MMGR_Get_Mapping_Info
#define V86MMGR_Get_Mapping_Info PAGEABLE_V86MMGR_Get_Mapping_Info

#undef  V86MMGR_Get_Version
#define V86MMGR_Get_Version PAGEABLE_V86MMGR_Get_Version

#undef  V86MMGR_Get_VM_Flat_Sel
#define V86MMGR_Get_VM_Flat_Sel PAGEABLE_V86MMGR_Get_VM_Flat_Sel

#undef  V86MMGR_Get_Xlat_Buff_State
#define V86MMGR_Get_Xlat_Buff_State PAGEABLE_V86MMGR_Get_Xlat_Buff_State

#undef  V86MMGR_GetPgStatus
#define V86MMGR_GetPgStatus PAGEABLE_V86MMGR_GetPgStatus

#undef  V86MMGR_Load_Client_Ptr
#define V86MMGR_Load_Client_Ptr PAGEABLE_V86MMGR_Load_Client_Ptr

#undef  V86MMGR_LocalGlobalReg
#define V86MMGR_LocalGlobalReg PAGEABLE_V86MMGR_LocalGlobalReg

#undef  V86MMGR_Map_Pages
#define V86MMGR_Map_Pages PAGEABLE_V86MMGR_Map_Pages

#undef  V86MMGR_ResetBasePages
#define V86MMGR_ResetBasePages PAGEABLE_V86MMGR_ResetBasePages

#undef  V86MMGR_Set_EMS_XMS_Limits
#define V86MMGR_Set_EMS_XMS_Limits PAGEABLE_V86MMGR_Set_EMS_XMS_Limits

#undef  V86MMGR_Set_Mapping_Info
#define V86MMGR_Set_Mapping_Info PAGEABLE_V86MMGR_Set_Mapping_Info

#undef  V86MMGR_Set_Xlat_Buff_State
#define V86MMGR_Set_Xlat_Buff_State PAGEABLE_V86MMGR_Set_Xlat_Buff_State

#undef  V86MMGR_SetAvailMapPgs
#define V86MMGR_SetAvailMapPgs PAGEABLE_V86MMGR_SetAvailMapPgs

#undef  V86MMGR_Xlat_API
#define V86MMGR_Xlat_API PAGEABLE_V86MMGR_Xlat_API

#undef  VCD_Get_Focus
#define VCD_Get_Focus PAGEABLE_VCD_Get_Focus

#undef  VCD_Get_Version
#define VCD_Get_Version PAGEABLE_VCD_Get_Version

#undef  VCD_Set_Port_Global
#define VCD_Set_Port_Global PAGEABLE_VCD_Set_Port_Global

#undef  VCD_Virtualize_Port
#define VCD_Virtualize_Port PAGEABLE_VCD_Virtualize_Port

#ifdef WIN40_OR_LATER
#undef  VMCPD_Set_CR0_State
#define VMCPD_Set_CR0_State PAGEABLE_VMCPD_Set_CR0_State
#endif

#ifdef WIN40_OR_LATER
#undef  VMCPD_Get_CR0_State
#define VMCPD_Get_CR0_State PAGEABLE_VMCPD_Get_CR0_State
#endif

#undef  VMCPD_Get_Thread_State
#define VMCPD_Get_Thread_State PAGEABLE_VMCPD_Get_Thread_State

#ifdef WIN40_OR_LATER
#undef  VMCPD_Set_Thread_State
#define VMCPD_Set_Thread_State PAGEABLE_VMCPD_Set_Thread_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Virtual_EOI
#define VPICD_Virtual_EOI PAGEABLE_VPICD_Virtual_EOI
#endif

#ifdef WIN40_OR_LATER
#undef  VTD_Get_Date_And_Time
#define VTD_Get_Date_And_Time PAGEABLE_VTD_Get_Date_And_Time
#endif

#undef  VDD_Get_ModTime
#define VDD_Get_ModTime PAGEABLE_VDD_Get_ModTime

#undef  VDD_Get_Version
#define VDD_Get_Version PAGEABLE_VDD_Get_Version

#undef  VDD_Msg_BakColor
#define VDD_Msg_BakColor PAGEABLE_VDD_Msg_BakColor

#undef  VDD_Msg_ClrScrn
#define VDD_Msg_ClrScrn PAGEABLE_VDD_Msg_ClrScrn

#undef  VDD_Msg_ForColor
#define VDD_Msg_ForColor PAGEABLE_VDD_Msg_ForColor

#undef  VDD_SetCursPos
#define VDD_SetCursPos PAGEABLE_VDD_SetCursPos

#undef  VDD_Msg_TextOut
#define VDD_Msg_TextOut PAGEABLE_VDD_Msg_TextOut

#undef  VDD_PIF_State
#define VDD_PIF_State PAGEABLE_VDD_PIF_State

#undef  VDD_Query_Access
#define VDD_Query_Access PAGEABLE_VDD_Query_Access

#undef  VDD_Set_HCurTrk
#define VDD_Set_HCurTrk PAGEABLE_VDD_Set_HCurTrk

#undef  VDD_Set_VMType
#define VDD_Set_VMType PAGEABLE_VDD_Set_VMType

#undef  VDMAD_Copy_From_Buffer
#define VDMAD_Copy_From_Buffer PAGEABLE_VDMAD_Copy_From_Buffer

#undef  VDMAD_Copy_To_Buffer
#define VDMAD_Copy_To_Buffer PAGEABLE_VDMAD_Copy_To_Buffer

#undef  VDMAD_Default_Handler
#define VDMAD_Default_Handler PAGEABLE_VDMAD_Default_Handler

#undef  VDMAD_Disable_Translation
#define VDMAD_Disable_Translation PAGEABLE_VDMAD_Disable_Translation

#undef  VDMAD_Enable_Translation
#define VDMAD_Enable_Translation PAGEABLE_VDMAD_Enable_Translation

#undef  VDMAD_Get_EISA_Adr_Mode
#define VDMAD_Get_EISA_Adr_Mode PAGEABLE_VDMAD_Get_EISA_Adr_Mode

#undef  VDMAD_Get_Region_Info
#define VDMAD_Get_Region_Info PAGEABLE_VDMAD_Get_Region_Info

#undef  VDMAD_Get_Version
#define VDMAD_Get_Version PAGEABLE_VDMAD_Get_Version

#undef  VDMAD_Get_Virt_State
#define VDMAD_Get_Virt_State PAGEABLE_VDMAD_Get_Virt_State

#undef  VDMAD_Lock_DMA_Region
#define VDMAD_Lock_DMA_Region PAGEABLE_VDMAD_Lock_DMA_Region

#undef  VDMAD_Mask_Channel
#define VDMAD_Mask_Channel PAGEABLE_VDMAD_Mask_Channel

#undef  VDMAD_Release_Buffer
#define VDMAD_Release_Buffer PAGEABLE_VDMAD_Release_Buffer

#undef  VDMAD_Request_Buffer
#define VDMAD_Request_Buffer PAGEABLE_VDMAD_Request_Buffer

#undef  VDMAD_Reserve_Buffer_Space
#define VDMAD_Reserve_Buffer_Space PAGEABLE_VDMAD_Reserve_Buffer_Space

#undef  VDMAD_Scatter_Lock
#define VDMAD_Scatter_Lock PAGEABLE_VDMAD_Scatter_Lock

#undef  VDMAD_Scatter_Unlock
#define VDMAD_Scatter_Unlock PAGEABLE_VDMAD_Scatter_Unlock

#undef  VDMAD_Set_EISA_Adr_Mode
#define VDMAD_Set_EISA_Adr_Mode PAGEABLE_VDMAD_Set_EISA_Adr_Mode

#undef  VDMAD_Set_Phys_State
#define VDMAD_Set_Phys_State PAGEABLE_VDMAD_Set_Phys_State

#undef  VDMAD_Set_Region_Info
#define VDMAD_Set_Region_Info PAGEABLE_VDMAD_Set_Region_Info

#undef  VDMAD_Set_Virt_State
#define VDMAD_Set_Virt_State PAGEABLE_VDMAD_Set_Virt_State

#undef  VDMAD_Unlock_DMA_Region
#define VDMAD_Unlock_DMA_Region PAGEABLE_VDMAD_Unlock_DMA_Region

#undef  VDMAD_UnMask_Channel
#define VDMAD_UnMask_Channel PAGEABLE_VDMAD_UnMask_Channel

#undef  VDMAD_Virtualize_Channel
#define VDMAD_Virtualize_Channel PAGEABLE_VDMAD_Virtualize_Channel

#undef  VKD_Cancel_Paste
#define VKD_Cancel_Paste PAGEABLE_VKD_Cancel_Paste

#undef  VKD_Define_Hot_Key
#define VKD_Define_Hot_Key PAGEABLE_VKD_Define_Hot_Key

#undef  VKD_Define_Paste_Mode
#define VKD_Define_Paste_Mode PAGEABLE_VKD_Define_Paste_Mode

#undef  VKD_Flush_Msg_Key_Queue
#define VKD_Flush_Msg_Key_Queue PAGEABLE_VKD_Flush_Msg_Key_Queue

#undef  VKD_Force_Keys
#define VKD_Force_Keys PAGEABLE_VKD_Force_Keys

#undef  VKD_Get_Kbd_Owner
#define VKD_Get_Kbd_Owner PAGEABLE_VKD_Get_Kbd_Owner

#undef  VKD_Get_Msg_Key
#define VKD_Get_Msg_Key PAGEABLE_VKD_Get_Msg_Key

#undef  VKD_Get_Version
#define VKD_Get_Version PAGEABLE_VKD_Get_Version

#undef  VKD_Local_Disable_Hot_Key
#define VKD_Local_Disable_Hot_Key PAGEABLE_VKD_Local_Disable_Hot_Key

#undef  VKD_Local_Enable_Hot_Key
#define VKD_Local_Enable_Hot_Key PAGEABLE_VKD_Local_Enable_Hot_Key

#undef  VKD_Peek_Msg_Key
#define VKD_Peek_Msg_Key PAGEABLE_VKD_Peek_Msg_Key

#undef  VKD_Reflect_Hot_Key
#define VKD_Reflect_Hot_Key PAGEABLE_VKD_Reflect_Hot_Key

#undef  VKD_Remove_Hot_Key
#define VKD_Remove_Hot_Key PAGEABLE_VKD_Remove_Hot_Key

#undef  VKD_Start_Paste
#define VKD_Start_Paste PAGEABLE_VKD_Start_Paste

#undef  VMCPD_Get_Version
#define VMCPD_Get_Version PAGEABLE_VMCPD_Get_Version

#undef  VMCPD_Get_Virt_State
#define VMCPD_Get_Virt_State PAGEABLE_VMCPD_Get_Virt_State

#undef  VMCPD_Set_Virt_State
#define VMCPD_Set_Virt_State PAGEABLE_VMCPD_Set_Virt_State

#undef  VMPoll_Enable_Disable
#define VMPoll_Enable_Disable PAGEABLE_VMPoll_Enable_Disable

#undef  VMPoll_Get_Version
#define VMPoll_Get_Version PAGEABLE_VMPoll_Get_Version

#undef  VMPoll_Reset_Detection
#define VMPoll_Reset_Detection PAGEABLE_VMPoll_Reset_Detection

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  VPICD_Call_When_Hw_Int
#define VPICD_Call_When_Hw_Int PAGEABLE_VPICD_Call_When_Hw_Int

#undef  VPICD_Clear_Int_Request
#define VPICD_Clear_Int_Request PAGEABLE_VPICD_Clear_Int_Request

#undef  VPICD_Convert_Handle_To_IRQ
#define VPICD_Convert_Handle_To_IRQ PAGEABLE_VPICD_Convert_Handle_To_IRQ

#undef  VPICD_Convert_Int_To_IRQ
#define VPICD_Convert_Int_To_IRQ PAGEABLE_VPICD_Convert_Int_To_IRQ

#undef  VPICD_Convert_IRQ_To_Int
#define VPICD_Convert_IRQ_To_Int PAGEABLE_VPICD_Convert_IRQ_To_Int

#undef  VPICD_Force_Default_Behavior
#define VPICD_Force_Default_Behavior PAGEABLE_VPICD_Force_Default_Behavior

#undef  VPICD_Force_Default_Owner
#define VPICD_Force_Default_Owner PAGEABLE_VPICD_Force_Default_Owner

#undef  VPICD_Get_Complete_Status
#define VPICD_Get_Complete_Status PAGEABLE_VPICD_Get_Complete_Status

#undef  VPICD_Get_IRQ_Complete_Status
#define VPICD_Get_IRQ_Complete_Status PAGEABLE_VPICD_Get_IRQ_Complete_Status

#undef  VPICD_Get_Status
#define VPICD_Get_Status PAGEABLE_VPICD_Get_Status

#undef  VPICD_Get_Version
#define VPICD_Get_Version PAGEABLE_VPICD_Get_Version

#undef  VPICD_Phys_EOI
#define VPICD_Phys_EOI PAGEABLE_VPICD_Phys_EOI

#undef  VPICD_Physically_Mask
#define VPICD_Physically_Mask PAGEABLE_VPICD_Physically_Mask

#undef  VPICD_Physically_Unmask
#define VPICD_Physically_Unmask PAGEABLE_VPICD_Physically_Unmask

#undef  VPICD_Set_Auto_Masking
#define VPICD_Set_Auto_Masking PAGEABLE_VPICD_Set_Auto_Masking

#undef  VPICD_Set_Int_Request
#define VPICD_Set_Int_Request PAGEABLE_VPICD_Set_Int_Request

#undef  VPICD_Test_Phys_Request
#define VPICD_Test_Phys_Request PAGEABLE_VPICD_Test_Phys_Request

#undef  VPICD_Virtualize_IRQ
#define VPICD_Virtualize_IRQ PAGEABLE_VPICD_Virtualize_IRQ

#ifdef WIN40_OR_LATER
#undef  VPICD_Auto_Mask_At_Inst_Swap
#define VPICD_Auto_Mask_At_Inst_Swap PAGEABLE_VPICD_Auto_Mask_At_Inst_Swap
#endif

#undef  VSD_Get_Version
#define VSD_Get_Version PAGEABLE_VSD_Get_Version

#undef  VTD_Begin_Min_Int_Period
#define VTD_Begin_Min_Int_Period PAGEABLE_VTD_Begin_Min_Int_Period

#undef  VTD_Disable_Trapping
#define VTD_Disable_Trapping PAGEABLE_VTD_Disable_Trapping

#undef  VTD_Enable_Trapping
#define VTD_Enable_Trapping PAGEABLE_VTD_Enable_Trapping

#undef  VTD_End_Min_Int_Period
#define VTD_End_Min_Int_Period PAGEABLE_VTD_End_Min_Int_Period

#undef  VTD_Get_Interrupt_Period
#define VTD_Get_Interrupt_Period PAGEABLE_VTD_Get_Interrupt_Period

#undef  VTD_Get_Real_Time
#define VTD_Get_Real_Time PAGEABLE_VTD_Get_Real_Time

#undef  VTD_Get_Version
#define VTD_Get_Version PAGEABLE_VTD_Get_Version

#undef  VPICD_Thunk_HWInt
#define VPICD_Thunk_HWInt PAGEABLE_VPICD_Thunk_HWInt

#undef  VPICD_Thunk_VirtInt
#define VPICD_Thunk_VirtInt PAGEABLE_VPICD_Thunk_VirtInt

#undef  VPICD_Thunk_EOI
#define VPICD_Thunk_EOI PAGEABLE_VPICD_Thunk_EOI

#undef  VPICD_Thunk_MaskChange
#define VPICD_Thunk_MaskChange PAGEABLE_VPICD_Thunk_MaskChange

#undef  VPICD_Thunk_Iret
#define VPICD_Thunk_Iret PAGEABLE_VPICD_Thunk_Iret

#undef  Install_Exception_Handler
#define Install_Exception_Handler PAGEABLE_Install_Exception_Handler

#ifdef WFW311_OR_LATER
#undef  VXDLDR_GetDeviceList
#define VXDLDR_GetDeviceList PAGEABLE_VXDLDR_GetDeviceList
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_GetVersion
#define VXDLDR_GetVersion PAGEABLE_VXDLDR_GetVersion
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_LoadDevice
#define VXDLDR_LoadDevice PAGEABLE_VXDLDR_LoadDevice
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_UnloadDevice
#define VXDLDR_UnloadDevice PAGEABLE_VXDLDR_UnloadDevice
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_DevInitSucceeded
#define VXDLDR_DevInitSucceeded PAGEABLE_VXDLDR_DevInitSucceeded
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_DevInitFailed
#define VXDLDR_DevInitFailed PAGEABLE_VXDLDR_DevInitFailed
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_APM_BIOS_Version
#define _VPOWERD_Get_APM_BIOS_Version PAGEABLE__VPOWERD_Get_APM_BIOS_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_APM_BIOS_Version
#define VPOWERD_Get_APM_BIOS_Version PAGEABLE__VPOWERD_Get_APM_BIOS_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_State
#define _VPOWERD_Get_Power_State PAGEABLE__VPOWERD_Get_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_State
#define VPOWERD_Get_Power_State PAGEABLE__VPOWERD_Get_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_Management_Level
#define _VPOWERD_Get_Power_Management_Level PAGEABLE__VPOWERD_Get_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_Management_Level
#define VPOWERD_Get_Power_Management_Level PAGEABLE__VPOWERD_Get_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_Status
#define _VPOWERD_Get_Power_Status PAGEABLE__VPOWERD_Get_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_Status
#define VPOWERD_Get_Power_Status PAGEABLE__VPOWERD_Get_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_OEM_APM_Function
#define _VPOWERD_OEM_APM_Function PAGEABLE__VPOWERD_OEM_APM_Function
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_OEM_APM_Function
#define VPOWERD_OEM_APM_Function PAGEABLE__VPOWERD_OEM_APM_Function
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Register_Power_Handler
#define _VPOWERD_Register_Power_Handler PAGEABLE__VPOWERD_Register_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Register_Power_Handler
#define VPOWERD_Register_Power_Handler PAGEABLE__VPOWERD_Register_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Restore_Power_On_Defaults
#define _VPOWERD_Restore_Power_On_Defaults PAGEABLE__VPOWERD_Restore_Power_On_Defaults
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Restore_Power_On_Defaults
#define VPOWERD_Restore_Power_On_Defaults PAGEABLE__VPOWERD_Restore_Power_On_Defaults
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_Device_Power_State
#define _VPOWERD_Set_Device_Power_State PAGEABLE__VPOWERD_Set_Device_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_Device_Power_State
#define VPOWERD_Set_Device_Power_State PAGEABLE__VPOWERD_Set_Device_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_Power_Management_Level
#define _VPOWERD_Set_Power_Management_Level PAGEABLE__VPOWERD_Set_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_Power_Management_Level
#define VPOWERD_Set_Power_Management_Level PAGEABLE__VPOWERD_Set_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_System_Power_State
#define _VPOWERD_Set_System_Power_State PAGEABLE__VPOWERD_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_System_Power_State
#define VPOWERD_Set_System_Power_State PAGEABLE__VPOWERD_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Deregister_Power_Handler
#define _VPOWERD_Deregister_Power_Handler PAGEABLE__VPOWERD_Deregister_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Deregister_Power_Handler
#define VPOWERD_Deregister_Power_Handler PAGEABLE__VPOWERD_Deregister_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Version
#define _VPOWERD_Get_Version PAGEABLE__VPOWERD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Version
#define VPOWERD_Get_Version PAGEABLE__VPOWERD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_W32_Get_System_Power_Status
#define _VPOWERD_W32_Get_System_Power_Status PAGEABLE__VPOWERD_W32_Get_System_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_W32_Get_System_Power_Status
#define VPOWERD_W32_Get_System_Power_Status PAGEABLE__VPOWERD_W32_Get_System_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_W32_Set_System_Power_State
#define _VPOWERD_W32_Set_System_Power_State PAGEABLE__VPOWERD_W32_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_W32_Set_System_Power_State
#define VPOWERD_W32_Set_System_Power_State PAGEABLE__VPOWERD_W32_Set_System_Power_State
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Get_Version
#define VCOMM_Get_Version PAGEABLE_VCOMM_Get_Version
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Acquire_Port
#define _VCOMM_Acquire_Port PAGEABLE__VCOMM_Acquire_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Acquire_Port
#define VCOMM_Acquire_Port PAGEABLE__VCOMM_Acquire_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Add_Port
#define _VCOMM_Add_Port PAGEABLE__VCOMM_Add_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Add_Port
#define VCOMM_Add_Port PAGEABLE__VCOMM_Add_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Register_Port_Driver
#define _VCOMM_Register_Port_Driver PAGEABLE__VCOMM_Register_Port_Driver
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Register_Port_Driver
#define VCOMM_Register_Port_Driver PAGEABLE__VCOMM_Register_Port_Driver
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Release_Port
#define _VCOMM_Release_Port PAGEABLE__VCOMM_Release_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Release_Port
#define VCOMM_Release_Port PAGEABLE__VCOMM_Release_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_ClearCommError
#define _VCOMM_ClearCommError PAGEABLE__VCOMM_ClearCommError
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_ClearCommError
#define VCOMM_ClearCommError PAGEABLE__VCOMM_ClearCommError
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_CloseComm
#define _VCOMM_CloseComm PAGEABLE__VCOMM_CloseComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_CloseComm
#define VCOMM_CloseComm PAGEABLE__VCOMM_CloseComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_EnableCommNotification
#define _VCOMM_EnableCommNotification PAGEABLE__VCOMM_EnableCommNotification
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_EnableCommNotification
#define VCOMM_EnableCommNotification PAGEABLE__VCOMM_EnableCommNotification
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_EscapeCommFunction
#define _VCOMM_EscapeCommFunction PAGEABLE__VCOMM_EscapeCommFunction
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_EscapeCommFunction
#define VCOMM_EscapeCommFunction PAGEABLE__VCOMM_EscapeCommFunction
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommEventMask
#define _VCOMM_GetCommEventMask PAGEABLE__VCOMM_GetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommEventMask
#define VCOMM_GetCommEventMask PAGEABLE__VCOMM_GetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommProperties
#define _VCOMM_GetCommProperties PAGEABLE__VCOMM_GetCommProperties
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommProperties
#define VCOMM_GetCommProperties PAGEABLE__VCOMM_GetCommProperties
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommQueueStatus
#define _VCOMM_GetCommQueueStatus PAGEABLE__VCOMM_GetCommQueueStatus
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommQueueStatus
#define VCOMM_GetCommQueueStatus PAGEABLE__VCOMM_GetCommQueueStatus
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommState
#define _VCOMM_GetCommState PAGEABLE__VCOMM_GetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommState
#define VCOMM_GetCommState PAGEABLE__VCOMM_GetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetLastError
#define _VCOMM_GetLastError PAGEABLE__VCOMM_GetLastError
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetLastError
#define VCOMM_GetLastError PAGEABLE__VCOMM_GetLastError
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetModemStatus
#define _VCOMM_GetModemStatus PAGEABLE__VCOMM_GetModemStatus
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetModemStatus
#define VCOMM_GetModemStatus PAGEABLE__VCOMM_GetModemStatus
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_GetSetCommTimeouts
#define _VCOMM_GetSetCommTimeouts PAGEABLE__VCOMM_GetSetCommTimeouts
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_GetSetCommTimeouts
#define VCOMM_GetSetCommTimeouts PAGEABLE__VCOMM_GetSetCommTimeouts
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_OpenComm
#define _VCOMM_OpenComm PAGEABLE__VCOMM_OpenComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_OpenComm
#define VCOMM_OpenComm PAGEABLE__VCOMM_OpenComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_PurgeComm
#define _VCOMM_PurgeComm PAGEABLE__VCOMM_PurgeComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_PurgeComm
#define VCOMM_PurgeComm PAGEABLE__VCOMM_PurgeComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_ReadComm
#define _VCOMM_ReadComm PAGEABLE__VCOMM_ReadComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_ReadComm
#define VCOMM_ReadComm PAGEABLE__VCOMM_ReadComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetCommEventMask
#define _VCOMM_SetCommEventMask PAGEABLE__VCOMM_SetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetCommEventMask
#define VCOMM_SetCommEventMask PAGEABLE__VCOMM_SetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetCommState
#define _VCOMM_SetCommState PAGEABLE__VCOMM_SetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetCommState
#define VCOMM_SetCommState PAGEABLE__VCOMM_SetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetReadCallback
#define _VCOMM_SetReadCallback PAGEABLE__VCOMM_SetReadCallback
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetReadCallback
#define VCOMM_SetReadCallback PAGEABLE__VCOMM_SetReadCallback
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetupComm
#define _VCOMM_SetupComm PAGEABLE__VCOMM_SetupComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetupComm
#define VCOMM_SetupComm PAGEABLE__VCOMM_SetupComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetWriteCallback
#define _VCOMM_SetWriteCallback PAGEABLE__VCOMM_SetWriteCallback
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetWriteCallback
#define VCOMM_SetWriteCallback PAGEABLE__VCOMM_SetWriteCallback
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_TransmitCommChar
#define _VCOMM_TransmitCommChar PAGEABLE__VCOMM_TransmitCommChar
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_TransmitCommChar
#define VCOMM_TransmitCommChar PAGEABLE__VCOMM_TransmitCommChar
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_WriteComm
#define _VCOMM_WriteComm PAGEABLE__VCOMM_WriteComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_WriteComm
#define VCOMM_WriteComm PAGEABLE__VCOMM_WriteComm
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Win32DCB_To_Ring0
#define VCOMM_Map_Win32DCB_To_Ring0 PAGEABLE_VCOMM_Map_Win32DCB_To_Ring0
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Ring0DCB_To_Win32
#define VCOMM_Map_Ring0DCB_To_Win32 PAGEABLE_VCOMM_Map_Ring0DCB_To_Win32
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Get_Contention_Handler
#define _VCOMM_Get_Contention_Handler PAGEABLE__VCOMM_Get_Contention_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Get_Contention_Handler
#define VCOMM_Get_Contention_Handler PAGEABLE__VCOMM_Get_Contention_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Map_Name_To_Resource
#define _VCOMM_Map_Name_To_Resource PAGEABLE__VCOMM_Map_Name_To_Resource
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Name_To_Resource
#define VCOMM_Map_Name_To_Resource PAGEABLE__VCOMM_Map_Name_To_Resource
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Version
#define _CONFIGMG_Get_Version PAGEABLE__CONFIGMG_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Version
#define CONFIGMG_Get_Version PAGEABLE__CONFIGMG_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Initialize
#define _CONFIGMG_Initialize PAGEABLE__CONFIGMG_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Initialize
#define CONFIGMG_Initialize PAGEABLE__CONFIGMG_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Locate_DevNode
#define _CONFIGMG_Locate_DevNode PAGEABLE__CONFIGMG_Locate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Locate_DevNode
#define CONFIGMG_Locate_DevNode PAGEABLE__CONFIGMG_Locate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Parent
#define _CONFIGMG_Get_Parent PAGEABLE__CONFIGMG_Get_Parent
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Parent
#define CONFIGMG_Get_Parent PAGEABLE__CONFIGMG_Get_Parent
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Child
#define _CONFIGMG_Get_Child PAGEABLE__CONFIGMG_Get_Child
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Child
#define CONFIGMG_Get_Child PAGEABLE__CONFIGMG_Get_Child
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Sibling
#define _CONFIGMG_Get_Sibling PAGEABLE__CONFIGMG_Get_Sibling
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Sibling
#define CONFIGMG_Get_Sibling PAGEABLE__CONFIGMG_Get_Sibling
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_ID_Size
#define _CONFIGMG_Get_Device_ID_Size PAGEABLE__CONFIGMG_Get_Device_ID_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_ID_Size
#define CONFIGMG_Get_Device_ID_Size PAGEABLE__CONFIGMG_Get_Device_ID_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_ID
#define _CONFIGMG_Get_Device_ID PAGEABLE__CONFIGMG_Get_Device_ID
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_ID
#define CONFIGMG_Get_Device_ID PAGEABLE__CONFIGMG_Get_Device_ID
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Depth
#define _CONFIGMG_Get_Depth PAGEABLE__CONFIGMG_Get_Depth
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Depth
#define CONFIGMG_Get_Depth PAGEABLE__CONFIGMG_Get_Depth
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Private_DWord
#define _CONFIGMG_Get_Private_DWord PAGEABLE__CONFIGMG_Get_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Private_DWord
#define CONFIGMG_Get_Private_DWord PAGEABLE__CONFIGMG_Get_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Private_DWord
#define _CONFIGMG_Set_Private_DWord PAGEABLE__CONFIGMG_Set_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Private_DWord
#define CONFIGMG_Set_Private_DWord PAGEABLE__CONFIGMG_Set_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Create_DevNode
#define _CONFIGMG_Create_DevNode PAGEABLE__CONFIGMG_Create_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Create_DevNode
#define CONFIGMG_Create_DevNode PAGEABLE__CONFIGMG_Create_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Remove_SubTree
#define _CONFIGMG_Query_Remove_SubTree PAGEABLE__CONFIGMG_Query_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Remove_SubTree
#define CONFIGMG_Query_Remove_SubTree PAGEABLE__CONFIGMG_Query_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Remove_SubTree
#define _CONFIGMG_Remove_SubTree PAGEABLE__CONFIGMG_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Remove_SubTree
#define CONFIGMG_Remove_SubTree PAGEABLE__CONFIGMG_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Device_Driver
#define _CONFIGMG_Register_Device_Driver PAGEABLE__CONFIGMG_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Device_Driver
#define CONFIGMG_Register_Device_Driver PAGEABLE__CONFIGMG_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Enumerator
#define _CONFIGMG_Register_Enumerator PAGEABLE__CONFIGMG_Register_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Enumerator
#define CONFIGMG_Register_Enumerator PAGEABLE__CONFIGMG_Register_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Arbitrator
#define _CONFIGMG_Register_Arbitrator PAGEABLE__CONFIGMG_Register_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Arbitrator
#define CONFIGMG_Register_Arbitrator PAGEABLE__CONFIGMG_Register_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Deregister_Arbitrator
#define _CONFIGMG_Deregister_Arbitrator PAGEABLE__CONFIGMG_Deregister_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Deregister_Arbitrator
#define CONFIGMG_Deregister_Arbitrator PAGEABLE__CONFIGMG_Deregister_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Res_Des
#define _CONFIGMG_Add_Res_Des PAGEABLE__CONFIGMG_Add_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Res_Des
#define CONFIGMG_Add_Res_Des PAGEABLE__CONFIGMG_Add_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Modify_Res_Des
#define _CONFIGMG_Modify_Res_Des PAGEABLE__CONFIGMG_Modify_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Modify_Res_Des
#define CONFIGMG_Modify_Res_Des PAGEABLE__CONFIGMG_Modify_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Res_Des
#define _CONFIGMG_Free_Res_Des PAGEABLE__CONFIGMG_Free_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Res_Des
#define CONFIGMG_Free_Res_Des PAGEABLE__CONFIGMG_Free_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Next_Res_Des
#define _CONFIGMG_Get_Next_Res_Des PAGEABLE__CONFIGMG_Get_Next_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Next_Res_Des
#define CONFIGMG_Get_Next_Res_Des PAGEABLE__CONFIGMG_Get_Next_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Performance_Info
#define _CONFIGMG_Get_Performance_Info PAGEABLE__CONFIGMG_Get_Performance_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Performance_Info
#define CONFIGMG_Get_Performance_Info PAGEABLE__CONFIGMG_Get_Performance_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Res_Des_Data_Size
#define _CONFIGMG_Get_Res_Des_Data_Size PAGEABLE__CONFIGMG_Get_Res_Des_Data_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Res_Des_Data_Size
#define CONFIGMG_Get_Res_Des_Data_Size PAGEABLE__CONFIGMG_Get_Res_Des_Data_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Res_Des_Data
#define _CONFIGMG_Get_Res_Des_Data PAGEABLE__CONFIGMG_Get_Res_Des_Data
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Res_Des_Data
#define CONFIGMG_Get_Res_Des_Data PAGEABLE__CONFIGMG_Get_Res_Des_Data
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Process_Events_Now
#define _CONFIGMG_Process_Events_Now PAGEABLE__CONFIGMG_Process_Events_Now
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Process_Events_Now
#define CONFIGMG_Process_Events_Now PAGEABLE__CONFIGMG_Process_Events_Now
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Create_Range_List
#define _CONFIGMG_Create_Range_List PAGEABLE__CONFIGMG_Create_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Create_Range_List
#define CONFIGMG_Create_Range_List PAGEABLE__CONFIGMG_Create_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Range
#define _CONFIGMG_Add_Range PAGEABLE__CONFIGMG_Add_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Range
#define CONFIGMG_Add_Range PAGEABLE__CONFIGMG_Add_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Delete_Range
#define _CONFIGMG_Delete_Range PAGEABLE__CONFIGMG_Delete_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Delete_Range
#define CONFIGMG_Delete_Range PAGEABLE__CONFIGMG_Delete_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Test_Range_Available
#define _CONFIGMG_Test_Range_Available PAGEABLE__CONFIGMG_Test_Range_Available
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Test_Range_Available
#define CONFIGMG_Test_Range_Available PAGEABLE__CONFIGMG_Test_Range_Available
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Dup_Range_List
#define _CONFIGMG_Dup_Range_List PAGEABLE__CONFIGMG_Dup_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Dup_Range_List
#define CONFIGMG_Dup_Range_List PAGEABLE__CONFIGMG_Dup_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Range_List
#define _CONFIGMG_Free_Range_List PAGEABLE__CONFIGMG_Free_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Range_List
#define CONFIGMG_Free_Range_List PAGEABLE__CONFIGMG_Free_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Invert_Range_List
#define _CONFIGMG_Invert_Range_List PAGEABLE__CONFIGMG_Invert_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Invert_Range_List
#define CONFIGMG_Invert_Range_List PAGEABLE__CONFIGMG_Invert_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Intersect_Range_List
#define _CONFIGMG_Intersect_Range_List PAGEABLE__CONFIGMG_Intersect_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Intersect_Range_List
#define CONFIGMG_Intersect_Range_List PAGEABLE__CONFIGMG_Intersect_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_First_Range
#define _CONFIGMG_First_Range PAGEABLE__CONFIGMG_First_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_First_Range
#define CONFIGMG_First_Range PAGEABLE__CONFIGMG_First_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Next_Range
#define _CONFIGMG_Next_Range PAGEABLE__CONFIGMG_Next_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Next_Range
#define CONFIGMG_Next_Range PAGEABLE__CONFIGMG_Next_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Dump_Range_List
#define _CONFIGMG_Dump_Range_List PAGEABLE__CONFIGMG_Dump_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Dump_Range_List
#define CONFIGMG_Dump_Range_List PAGEABLE__CONFIGMG_Dump_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Load_DLVxDs
#define _CONFIGMG_Load_DLVxDs PAGEABLE__CONFIGMG_Load_DLVxDs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Load_DLVxDs
#define CONFIGMG_Load_DLVxDs PAGEABLE__CONFIGMG_Load_DLVxDs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DDBs
#define _CONFIGMG_Get_DDBs PAGEABLE__CONFIGMG_Get_DDBs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DDBs
#define CONFIGMG_Get_DDBs PAGEABLE__CONFIGMG_Get_DDBs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_CRC_CheckSum
#define _CONFIGMG_Get_CRC_CheckSum PAGEABLE__CONFIGMG_Get_CRC_CheckSum
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_CRC_CheckSum
#define CONFIGMG_Get_CRC_CheckSum PAGEABLE__CONFIGMG_Get_CRC_CheckSum
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_DevLoader
#define _CONFIGMG_Register_DevLoader PAGEABLE__CONFIGMG_Register_DevLoader
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_DevLoader
#define CONFIGMG_Register_DevLoader PAGEABLE__CONFIGMG_Register_DevLoader
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Reenumerate_DevNode
#define _CONFIGMG_Reenumerate_DevNode PAGEABLE__CONFIGMG_Reenumerate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Reenumerate_DevNode
#define CONFIGMG_Reenumerate_DevNode PAGEABLE__CONFIGMG_Reenumerate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Setup_DevNode
#define _CONFIGMG_Setup_DevNode PAGEABLE__CONFIGMG_Setup_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Setup_DevNode
#define CONFIGMG_Setup_DevNode PAGEABLE__CONFIGMG_Setup_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Reset_Children_Marks
#define _CONFIGMG_Reset_Children_Marks PAGEABLE__CONFIGMG_Reset_Children_Marks
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Reset_Children_Marks
#define CONFIGMG_Reset_Children_Marks PAGEABLE__CONFIGMG_Reset_Children_Marks
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Status
#define _CONFIGMG_Get_DevNode_Status PAGEABLE__CONFIGMG_Get_DevNode_Status
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Status
#define CONFIGMG_Get_DevNode_Status PAGEABLE__CONFIGMG_Get_DevNode_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Remove_Unmarked_Children
#define _CONFIGMG_Remove_Unmarked_Children PAGEABLE__CONFIGMG_Remove_Unmarked_Children
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Remove_Unmarked_Children
#define CONFIGMG_Remove_Unmarked_Children PAGEABLE__CONFIGMG_Remove_Unmarked_Children
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_ISAPNP_To_CM
#define _CONFIGMG_ISAPNP_To_CM PAGEABLE__CONFIGMG_ISAPNP_To_CM
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_ISAPNP_To_CM
#define CONFIGMG_ISAPNP_To_CM PAGEABLE__CONFIGMG_ISAPNP_To_CM
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_CallBack_Device_Driver
#define _CONFIGMG_CallBack_Device_Driver PAGEABLE__CONFIGMG_CallBack_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_CallBack_Device_Driver
#define CONFIGMG_CallBack_Device_Driver PAGEABLE__CONFIGMG_CallBack_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_CallBack_Enumerator
#define _CONFIGMG_CallBack_Enumerator PAGEABLE__CONFIGMG_CallBack_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_CallBack_Enumerator
#define CONFIGMG_CallBack_Enumerator PAGEABLE__CONFIGMG_CallBack_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Alloc_Log_Conf
#define _CONFIGMG_Get_Alloc_Log_Conf PAGEABLE__CONFIGMG_Get_Alloc_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Alloc_Log_Conf
#define CONFIGMG_Get_Alloc_Log_Conf PAGEABLE__CONFIGMG_Get_Alloc_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Key_Size
#define _CONFIGMG_Get_DevNode_Key_Size PAGEABLE__CONFIGMG_Get_DevNode_Key_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Key_Size
#define CONFIGMG_Get_DevNode_Key_Size PAGEABLE__CONFIGMG_Get_DevNode_Key_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Key
#define _CONFIGMG_Get_DevNode_Key PAGEABLE__CONFIGMG_Get_DevNode_Key
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Key
#define CONFIGMG_Get_DevNode_Key PAGEABLE__CONFIGMG_Get_DevNode_Key
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Read_Registry_Value
#define _CONFIGMG_Read_Registry_Value PAGEABLE__CONFIGMG_Read_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Read_Registry_Value
#define CONFIGMG_Read_Registry_Value PAGEABLE__CONFIGMG_Read_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Write_Registry_Value
#define _CONFIGMG_Write_Registry_Value PAGEABLE__CONFIGMG_Write_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Write_Registry_Value
#define CONFIGMG_Write_Registry_Value PAGEABLE__CONFIGMG_Write_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Disable_DevNode
#define _CONFIGMG_Disable_DevNode PAGEABLE__CONFIGMG_Disable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Disable_DevNode
#define CONFIGMG_Disable_DevNode PAGEABLE__CONFIGMG_Disable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Enable_DevNode
#define _CONFIGMG_Enable_DevNode PAGEABLE__CONFIGMG_Enable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Enable_DevNode
#define CONFIGMG_Enable_DevNode PAGEABLE__CONFIGMG_Enable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Move_DevNode
#define _CONFIGMG_Move_DevNode PAGEABLE__CONFIGMG_Move_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Move_DevNode
#define CONFIGMG_Move_DevNode PAGEABLE__CONFIGMG_Move_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Bus_Info
#define _CONFIGMG_Set_Bus_Info PAGEABLE__CONFIGMG_Set_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Bus_Info
#define CONFIGMG_Set_Bus_Info PAGEABLE__CONFIGMG_Set_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Bus_Info
#define _CONFIGMG_Get_Bus_Info PAGEABLE__CONFIGMG_Get_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Bus_Info
#define CONFIGMG_Get_Bus_Info PAGEABLE__CONFIGMG_Get_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_HW_Prof
#define _CONFIGMG_Set_HW_Prof PAGEABLE__CONFIGMG_Set_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_HW_Prof
#define CONFIGMG_Set_HW_Prof PAGEABLE__CONFIGMG_Set_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Recompute_HW_Prof
#define _CONFIGMG_Recompute_HW_Prof PAGEABLE__CONFIGMG_Recompute_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Recompute_HW_Prof
#define CONFIGMG_Recompute_HW_Prof PAGEABLE__CONFIGMG_Recompute_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Change_HW_Prof
#define _CONFIGMG_Query_Change_HW_Prof PAGEABLE__CONFIGMG_Query_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Change_HW_Prof
#define CONFIGMG_Query_Change_HW_Prof PAGEABLE__CONFIGMG_Query_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_Driver_Private_DWord
#define _CONFIGMG_Get_Device_Driver_Private_DWord PAGEABLE__CONFIGMG_Get_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_Driver_Private_DWord
#define CONFIGMG_Get_Device_Driver_Private_DWord PAGEABLE__CONFIGMG_Get_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Device_Driver_Private_DWord
#define _CONFIGMG_Set_Device_Driver_Private_DWord PAGEABLE__CONFIGMG_Set_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Device_Driver_Private_DWord
#define CONFIGMG_Set_Device_Driver_Private_DWord PAGEABLE__CONFIGMG_Set_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_HW_Prof_Flags
#define _CONFIGMG_Get_HW_Prof_Flags PAGEABLE__CONFIGMG_Get_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_HW_Prof_Flags
#define CONFIGMG_Get_HW_Prof_Flags PAGEABLE__CONFIGMG_Get_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_HW_Prof_Flags
#define _CONFIGMG_Set_HW_Prof_Flags PAGEABLE__CONFIGMG_Set_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_HW_Prof_Flags
#define CONFIGMG_Set_HW_Prof_Flags PAGEABLE__CONFIGMG_Set_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Read_Registry_Log_Confs
#define _CONFIGMG_Read_Registry_Log_Confs PAGEABLE__CONFIGMG_Read_Registry_Log_Confs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Read_Registry_Log_Confs
#define CONFIGMG_Read_Registry_Log_Confs PAGEABLE__CONFIGMG_Read_Registry_Log_Confs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Run_Detection
#define _CONFIGMG_Run_Detection PAGEABLE__CONFIGMG_Run_Detection
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Run_Detection
#define CONFIGMG_Run_Detection PAGEABLE__CONFIGMG_Run_Detection
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Call_At_Appy_Time
#define _CONFIGMG_Call_At_Appy_Time PAGEABLE__CONFIGMG_Call_At_Appy_Time
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Call_At_Appy_Time
#define CONFIGMG_Call_At_Appy_Time PAGEABLE__CONFIGMG_Call_At_Appy_Time
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Private_Problem
#define _CONFIGMG_Set_Private_Problem PAGEABLE__CONFIGMG_Set_Private_Problem
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Private_Problem
#define CONFIGMG_Set_Private_Problem PAGEABLE__CONFIGMG_Set_Private_Problem
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Hardware_Profile_Info
#define _CONFIGMG_Get_Hardware_Profile_Info PAGEABLE__CONFIGMG_Get_Hardware_Profile_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Hardware_Profile_Info
#define CONFIGMG_Get_Hardware_Profile_Info PAGEABLE__CONFIGMG_Get_Hardware_Profile_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Arbitrator_Free_Size
#define _CONFIGMG_Query_Arbitrator_Free_Size PAGEABLE__CONFIGMG_Query_Arbitrator_Free_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Arbitrator_Free_Size
#define CONFIGMG_Query_Arbitrator_Free_Size PAGEABLE__CONFIGMG_Query_Arbitrator_Free_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Arbitrator_Free_Data
#define _CONFIGMG_Query_Arbitrator_Free_Data PAGEABLE__CONFIGMG_Query_Arbitrator_Free_Data
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Arbitrator_Free_Data
#define CONFIGMG_Query_Arbitrator_Free_Data PAGEABLE__CONFIGMG_Query_Arbitrator_Free_Data
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Sort_NodeList
#define _CONFIGMG_Sort_NodeList PAGEABLE__CONFIGMG_Sort_NodeList
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Sort_NodeList
#define CONFIGMG_Sort_NodeList PAGEABLE__CONFIGMG_Sort_NodeList
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Yield
#define _CONFIGMG_Yield PAGEABLE__CONFIGMG_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Yield
#define CONFIGMG_Yield PAGEABLE__CONFIGMG_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Lock
#define _CONFIGMG_Lock PAGEABLE__CONFIGMG_Lock
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Lock
#define CONFIGMG_Lock PAGEABLE__CONFIGMG_Lock
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Unlock
#define _CONFIGMG_Unlock PAGEABLE__CONFIGMG_Unlock
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Unlock
#define CONFIGMG_Unlock PAGEABLE__CONFIGMG_Unlock
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Empty_Log_Conf
#define _CONFIGMG_Add_Empty_Log_Conf PAGEABLE__CONFIGMG_Add_Empty_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Empty_Log_Conf
#define CONFIGMG_Add_Empty_Log_Conf PAGEABLE__CONFIGMG_Add_Empty_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Log_Conf
#define _CONFIGMG_Free_Log_Conf PAGEABLE__CONFIGMG_Free_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Log_Conf
#define CONFIGMG_Free_Log_Conf PAGEABLE__CONFIGMG_Free_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_First_Log_Conf
#define _CONFIGMG_Get_First_Log_Conf PAGEABLE__CONFIGMG_Get_First_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_First_Log_Conf
#define CONFIGMG_Get_First_Log_Conf PAGEABLE__CONFIGMG_Get_First_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Next_Log_Conf
#define _CONFIGMG_Get_Next_Log_Conf PAGEABLE__CONFIGMG_Get_Next_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Next_Log_Conf
#define CONFIGMG_Get_Next_Log_Conf PAGEABLE__CONFIGMG_Get_Next_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Fail_Change_HW_Prof
#define _CONFIGMG_Fail_Change_HW_Prof PAGEABLE__CONFIGMG_Fail_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Fail_Change_HW_Prof
#define CONFIGMG_Fail_Change_HW_Prof PAGEABLE__CONFIGMG_Fail_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Call_Enumerator_Function
#define _CONFIGMG_Call_Enumerator_Function PAGEABLE__CONFIGMG_Call_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Call_Enumerator_Function
#define CONFIGMG_Call_Enumerator_Function PAGEABLE__CONFIGMG_Call_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Post_Pointer_Message
#define VMD_Post_Pointer_Message PAGEABLE_VMD_Post_Pointer_Message
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Set_Cursor_Proc
#define VMD_Set_Cursor_Proc PAGEABLE_VMD_Set_Cursor_Proc
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Call_Cursor_Proc
#define VMD_Call_Cursor_Proc PAGEABLE_VMD_Call_Cursor_Proc
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Set_Mouse_Data
#define VMD_Set_Mouse_Data PAGEABLE_VMD_Set_Mouse_Data
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Get_Mouse_Data
#define VMD_Get_Mouse_Data PAGEABLE_VMD_Get_Mouse_Data
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Post_Absolute_Pointer_Message
#define VMD_Post_Absolute_Pointer_Message PAGEABLE_VMD_Post_Absolute_Pointer_Message
#endif

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_Version
#define IFSMgr_Get_Version PAGEABLE_IFSMgr_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterMount
#define IFSMgr_RegisterMount PAGEABLE_IFSMgr_RegisterMount
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterNet
#define IFSMgr_RegisterNet PAGEABLE_IFSMgr_RegisterNet
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterMailSlot
#define IFSMgr_RegisterMailSlot PAGEABLE_IFSMgr_RegisterMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_NetTime
#define IFSMgr_Get_NetTime PAGEABLE_IFSMgr_Get_NetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_DOSTime
#define IFSMgr_Get_DOSTime PAGEABLE_IFSMgr_Get_DOSTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetupConnection
#define IFSMgr_SetupConnection PAGEABLE_IFSMgr_SetupConnection
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DerefConnection
#define IFSMgr_DerefConnection PAGEABLE_IFSMgr_DerefConnection
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ServerDOSCall
#define IFSMgr_ServerDOSCall PAGEABLE_IFSMgr_ServerDOSCall
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CompleteAsync
#define IFSMgr_CompleteAsync PAGEABLE_IFSMgr_CompleteAsync
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterHeap
#define IFSMgr_RegisterHeap PAGEABLE_IFSMgr_RegisterHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetHeap
#define IFSMgr_GetHeap PAGEABLE_IFSMgr_GetHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RetHeap
#define IFSMgr_RetHeap PAGEABLE_IFSMgr_RetHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckHeap
#define IFSMgr_CheckHeap PAGEABLE_IFSMgr_CheckHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckHeapItem
#define IFSMgr_CheckHeapItem PAGEABLE_IFSMgr_CheckHeapItem
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FillHeapSpare
#define IFSMgr_FillHeapSpare PAGEABLE_IFSMgr_FillHeapSpare
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Block
#define IFSMgr_Block PAGEABLE_IFSMgr_Block
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Wakeup
#define IFSMgr_Wakeup PAGEABLE_IFSMgr_Wakeup
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Yield
#define IFSMgr_Yield PAGEABLE_IFSMgr_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SchedEvent
#define IFSMgr_SchedEvent PAGEABLE_IFSMgr_SchedEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_QueueEvent
#define IFSMgr_QueueEvent PAGEABLE_IFSMgr_QueueEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_KillEvent
#define IFSMgr_KillEvent PAGEABLE_IFSMgr_KillEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FreeIOReq
#define IFSMgr_FreeIOReq PAGEABLE_IFSMgr_FreeIOReq
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MakeMailSlot
#define IFSMgr_MakeMailSlot PAGEABLE_IFSMgr_MakeMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DeleteMailSlot
#define IFSMgr_DeleteMailSlot PAGEABLE_IFSMgr_DeleteMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_WriteMailSlot
#define IFSMgr_WriteMailSlot PAGEABLE_IFSMgr_WriteMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_PopUp
#define IFSMgr_PopUp PAGEABLE_IFSMgr_PopUp
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_printf
#define IFSMgr_printf PAGEABLE_IFSMgr_printf
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_AssertFailed
#define IFSMgr_AssertFailed PAGEABLE_IFSMgr_AssertFailed
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_LogEntry
#define IFSMgr_LogEntry PAGEABLE_IFSMgr_LogEntry
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DebugMenu
#define IFSMgr_DebugMenu PAGEABLE_IFSMgr_DebugMenu
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DebugVars
#define IFSMgr_DebugVars PAGEABLE_IFSMgr_DebugVars
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetDebugString
#define IFSMgr_GetDebugString PAGEABLE_IFSMgr_GetDebugString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetDebugHexNum
#define IFSMgr_GetDebugHexNum PAGEABLE_IFSMgr_GetDebugHexNum
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetFunction
#define IFSMgr_NetFunction PAGEABLE_IFSMgr_NetFunction
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DoDelAllUses
#define IFSMgr_DoDelAllUses PAGEABLE_IFSMgr_DoDelAllUses
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetErrString
#define IFSMgr_SetErrString PAGEABLE_IFSMgr_SetErrString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetErrString
#define IFSMgr_GetErrString PAGEABLE_IFSMgr_GetErrString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetReqHook
#define IFSMgr_SetReqHook PAGEABLE_IFSMgr_SetReqHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetPathHook
#define IFSMgr_SetPathHook PAGEABLE_IFSMgr_SetPathHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UseAdd
#define IFSMgr_UseAdd PAGEABLE_IFSMgr_UseAdd
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UseDel
#define IFSMgr_UseDel PAGEABLE_IFSMgr_UseDel
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_InitUseAdd
#define IFSMgr_InitUseAdd PAGEABLE_IFSMgr_InitUseAdd
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ChangeDir
#define IFSMgr_ChangeDir PAGEABLE_IFSMgr_ChangeDir
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DelAllUses
#define IFSMgr_DelAllUses PAGEABLE_IFSMgr_DelAllUses
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CDROM_Attach
#define IFSMgr_CDROM_Attach PAGEABLE_IFSMgr_CDROM_Attach
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CDROM_Detach
#define IFSMgr_CDROM_Detach PAGEABLE_IFSMgr_CDROM_Detach
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32DupHandle
#define IFSMgr_Win32DupHandle PAGEABLE_IFSMgr_Win32DupHandle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Ring0_FileIO
#define IFSMgr_Ring0_FileIO PAGEABLE_IFSMgr_Ring0_FileIO
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32_Get_Ring0_Handle
#define IFSMgr_Win32_Get_Ring0_Handle PAGEABLE_IFSMgr_Win32_Get_Ring0_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_Drive_Info
#define IFSMgr_Get_Drive_Info PAGEABLE_IFSMgr_Get_Drive_Info
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Ring0GetDriveInfo
#define IFSMgr_Ring0GetDriveInfo PAGEABLE_IFSMgr_Ring0GetDriveInfo
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_BlockNoEvents
#define IFSMgr_BlockNoEvents PAGEABLE_IFSMgr_BlockNoEvents
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetToDosTime
#define IFSMgr_NetToDosTime PAGEABLE_IFSMgr_NetToDosTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DosToNetTime
#define IFSMgr_DosToNetTime PAGEABLE_IFSMgr_DosToNetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DosToWin32Time
#define IFSMgr_DosToWin32Time PAGEABLE_IFSMgr_DosToWin32Time
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32ToDosTime
#define IFSMgr_Win32ToDosTime PAGEABLE_IFSMgr_Win32ToDosTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetToWin32Time
#define IFSMgr_NetToWin32Time PAGEABLE_IFSMgr_NetToWin32Time
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32ToNetTime
#define IFSMgr_Win32ToNetTime PAGEABLE_IFSMgr_Win32ToNetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MetaMatch
#define IFSMgr_MetaMatch PAGEABLE_IFSMgr_MetaMatch
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_TransMatch
#define IFSMgr_TransMatch PAGEABLE_IFSMgr_TransMatch
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CallProvider
#define IFSMgr_CallProvider PAGEABLE_IFSMgr_CallProvider
#endif

#ifdef WIN40_OR_LATER
#undef  UniToBCS
#define UniToBCS PAGEABLE_UniToBCS
#endif

#ifdef WIN40_OR_LATER
#undef  UniToBCSPath
#define UniToBCSPath PAGEABLE_UniToBCSPath
#endif

#ifdef WIN40_OR_LATER
#undef  BCSToUni
#define BCSToUni PAGEABLE_BCSToUni
#endif

#ifdef WIN40_OR_LATER
#undef  UniToUpper
#define UniToUpper PAGEABLE_UniToUpper
#endif

#ifdef WIN40_OR_LATER
#undef  UniCharToOEM
#define UniCharToOEM PAGEABLE_UniCharToOEM
#endif

#ifdef WIN40_OR_LATER
#undef  CreateBasis
#define CreateBasis PAGEABLE_CreateBasis
#endif

#ifdef WIN40_OR_LATER
#undef  MatchBasisName
#define MatchBasisName PAGEABLE_MatchBasisName
#endif

#ifdef WIN40_OR_LATER
#undef  AppendBasisTail
#define AppendBasisTail PAGEABLE_AppendBasisTail
#endif

#ifdef WIN40_OR_LATER
#undef  FcbToShort
#define FcbToShort PAGEABLE_FcbToShort
#endif

#ifdef WIN40_OR_LATER
#undef  ShortToFcb
#define ShortToFcb PAGEABLE_ShortToFcb
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ParsePath
#define IFSMgr_ParsePath PAGEABLE_IFSMgr_ParsePath
#endif

#ifdef WIN40_OR_LATER
#undef  Query_PhysLock
#define Query_PhysLock PAGEABLE_Query_PhysLock
#endif

#ifdef WIN40_OR_LATER
#undef  _VolFlush
#define _VolFlush PAGEABLE__VolFlush
#endif

#ifdef WIN40_OR_LATER
#undef  VolFlush
#define VolFlush PAGEABLE__VolFlush
#endif

#ifdef WIN40_OR_LATER
#undef  NotifyVolumeArrival
#define NotifyVolumeArrival PAGEABLE_NotifyVolumeArrival
#endif

#ifdef WIN40_OR_LATER
#undef  NotifyVolumeRemoval
#define NotifyVolumeRemoval PAGEABLE_NotifyVolumeRemoval
#endif

#ifdef WIN40_OR_LATER
#undef  QueryVolumeRemoval
#define QueryVolumeRemoval PAGEABLE_QueryVolumeRemoval
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDUnmountCFSD
#define IFSMgr_FSDUnmountCFSD PAGEABLE_IFSMgr_FSDUnmountCFSD
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetConversionTablePtrs
#define IFSMgr_GetConversionTablePtrs PAGEABLE_IFSMgr_GetConversionTablePtrs
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckAccessConflict
#define IFSMgr_CheckAccessConflict PAGEABLE_IFSMgr_CheckAccessConflict
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_LockFile
#define IFSMgr_LockFile PAGEABLE_IFSMgr_LockFile
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnlockFile
#define IFSMgr_UnlockFile PAGEABLE_IFSMgr_UnlockFile
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RemoveLocks
#define IFSMgr_RemoveLocks PAGEABLE_IFSMgr_RemoveLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckLocks
#define IFSMgr_CheckLocks PAGEABLE_IFSMgr_CheckLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CountLocks
#define IFSMgr_CountLocks PAGEABLE_IFSMgr_CountLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ReassignLockFileInst
#define IFSMgr_ReassignLockFileInst PAGEABLE_IFSMgr_ReassignLockFileInst
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnassignLockList
#define IFSMgr_UnassignLockList PAGEABLE_IFSMgr_UnassignLockList
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MountChildVolume
#define IFSMgr_MountChildVolume PAGEABLE_IFSMgr_MountChildVolume
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnmountChildVolume
#define IFSMgr_UnmountChildVolume PAGEABLE_IFSMgr_UnmountChildVolume
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SwapDrives
#define IFSMgr_SwapDrives PAGEABLE_IFSMgr_SwapDrives
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDMapFHtoIOREQ
#define IFSMgr_FSDMapFHtoIOREQ PAGEABLE_IFSMgr_FSDMapFHtoIOREQ
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDParsePath
#define IFSMgr_FSDParsePath PAGEABLE_IFSMgr_FSDParsePath
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDAttachSFT
#define IFSMgr_FSDAttachSFT PAGEABLE_IFSMgr_FSDAttachSFT
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetTimeZoneBias
#define IFSMgr_GetTimeZoneBias PAGEABLE_IFSMgr_GetTimeZoneBias
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_PNPEvent
#define IFSMgr_PNPEvent PAGEABLE_IFSMgr_PNPEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterCFSD
#define IFSMgr_RegisterCFSD PAGEABLE_IFSMgr_RegisterCFSD
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32MapExtendedHandleToSFT
#define IFSMgr_Win32MapExtendedHandleToSFT PAGEABLE_IFSMgr_Win32MapExtendedHandleToSFT
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DbgSetFileHandleLimit
#define IFSMgr_DbgSetFileHandleLimit PAGEABLE_IFSMgr_DbgSetFileHandleLimit
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32MapSFTToExtendedHandle
#define IFSMgr_Win32MapSFTToExtendedHandle PAGEABLE_IFSMgr_Win32MapSFTToExtendedHandle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDGetCurrentDrive
#define IFSMgr_FSDGetCurrentDrive PAGEABLE_IFSMgr_FSDGetCurrentDrive
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_InstallFileSystemApiHook
#define IFSMgr_InstallFileSystemApiHook PAGEABLE_IFSMgr_InstallFileSystemApiHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RemoveFileSystemApiHook
#define IFSMgr_RemoveFileSystemApiHook PAGEABLE_IFSMgr_RemoveFileSystemApiHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RunScheduledEvents
#define IFSMgr_RunScheduledEvents PAGEABLE_IFSMgr_RunScheduledEvents
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckDelResource
#define IFSMgr_CheckDelResource PAGEABLE_IFSMgr_CheckDelResource
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32GetVMCurdir
#define IFSMgr_Win32GetVMCurdir PAGEABLE_IFSMgr_Win32GetVMCurdir
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetupFailedConnection
#define IFSMgr_SetupFailedConnection PAGEABLE_IFSMgr_SetupFailedConnection
#endif

#ifdef WIN40_OR_LATER
#undef  _GetMappedErr
#define _GetMappedErr PAGEABLE__GetMappedErr
#endif

#ifdef WIN40_OR_LATER
#undef  GetMappedErr
#define GetMappedErr PAGEABLE__GetMappedErr
#endif

#ifdef WIN40_OR_LATER
#undef  ShortToLossyFcb
#define ShortToLossyFcb PAGEABLE_ShortToLossyFcb
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetLockState
#define IFSMgr_GetLockState PAGEABLE_IFSMgr_GetLockState
#endif

#ifdef WIN40_OR_LATER
#undef  BcsToBcs
#define BcsToBcs PAGEABLE_BcsToBcs
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetLoopback
#define IFSMgr_SetLoopback PAGEABLE_IFSMgr_SetLoopback
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ClearLoopback
#define IFSMgr_ClearLoopback PAGEABLE_IFSMgr_ClearLoopback
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ParseOneElement
#define IFSMgr_ParseOneElement PAGEABLE_IFSMgr_ParseOneElement
#endif

#ifdef WIN40_OR_LATER
#undef  BcsToBcsUpper
#define BcsToBcsUpper PAGEABLE_BcsToBcsUpper
#endif

#ifdef WIN40_OR_LATER
#undef  R0_OpenCreateFile
#define R0_OpenCreateFile PAGEABLE_R0_OpenCreateFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_ReadFile
#define R0_ReadFile PAGEABLE_R0_ReadFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_WriteFile
#define R0_WriteFile PAGEABLE_R0_WriteFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_CloseFile
#define R0_CloseFile PAGEABLE_R0_CloseFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetFileSize
#define R0_GetFileSize PAGEABLE_R0_GetFileSize
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindFirstFile
#define R0_FindFirstFile PAGEABLE_R0_FindFirstFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindNextFile
#define R0_FindNextFile PAGEABLE_R0_FindNextFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindCloseFile
#define R0_FindCloseFile PAGEABLE_R0_FindCloseFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetFileAttributes
#define R0_GetFileAttributes PAGEABLE_R0_GetFileAttributes
#endif

#ifdef WIN40_OR_LATER
#undef  R0_SetFileAttributes
#define R0_SetFileAttributes PAGEABLE_R0_SetFileAttributes
#endif

#ifdef WIN40_OR_LATER
#undef  R0_RenameFile
#define R0_RenameFile PAGEABLE_R0_RenameFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_DeleteFile
#define R0_DeleteFile PAGEABLE_R0_DeleteFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_LockFile
#define R0_LockFile PAGEABLE_R0_LockFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_UnlockFile
#define R0_UnlockFile PAGEABLE_R0_UnlockFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetDiskFreeSpace
#define R0_GetDiskFreeSpace PAGEABLE_R0_GetDiskFreeSpace
#endif

#ifdef WIN40_OR_LATER
#undef  R0_ReadAbsoluteDisk
#define R0_ReadAbsoluteDisk PAGEABLE_R0_ReadAbsoluteDisk
#endif

#ifdef WIN40_OR_LATER
#undef  R0_WriteAbsoluteDisk
#define R0_WriteAbsoluteDisk PAGEABLE_R0_WriteAbsoluteDisk
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Get_Version
#define VWIN32_Get_Version PAGEABLE_VWIN32_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_DIOCCompletionRoutine
#define VWIN32_DIOCCompletionRoutine PAGEABLE_VWIN32_DIOCCompletionRoutine
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_QueueUserApc
#define _VWIN32_QueueUserApc PAGEABLE__VWIN32_QueueUserApc
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_QueueUserApc
#define VWIN32_QueueUserApc PAGEABLE__VWIN32_QueueUserApc
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Get_Thread_Context
#define _VWIN32_Get_Thread_Context PAGEABLE__VWIN32_Get_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Get_Thread_Context
#define VWIN32_Get_Thread_Context PAGEABLE__VWIN32_Get_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Emulate_Npx
#define _VWIN32_Emulate_Npx PAGEABLE__VWIN32_Emulate_Npx
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Emulate_Npx
#define VWIN32_Emulate_Npx PAGEABLE__VWIN32_Emulate_Npx
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CheckDelayedNpxTrap
#define _VWIN32_CheckDelayedNpxTrap PAGEABLE__VWIN32_CheckDelayedNpxTrap
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CheckDelayedNpxTrap
#define VWIN32_CheckDelayedNpxTrap PAGEABLE__VWIN32_CheckDelayedNpxTrap
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_LeaveCrstR0
#define VWIN32_LeaveCrstR0 PAGEABLE_VWIN32_LeaveCrstR0
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_FaultPopup
#define _VWIN32_FaultPopup PAGEABLE__VWIN32_FaultPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_FaultPopup
#define VWIN32_FaultPopup PAGEABLE__VWIN32_FaultPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetContextHandle
#define VWIN32_GetContextHandle PAGEABLE_VWIN32_GetContextHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetCurrentProcessHandle
#define VWIN32_GetCurrentProcessHandle PAGEABLE_VWIN32_GetCurrentProcessHandle
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_SetWin32Event
#define _VWIN32_SetWin32Event PAGEABLE__VWIN32_SetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_SetWin32Event
#define VWIN32_SetWin32Event PAGEABLE__VWIN32_SetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_PulseWin32Event
#define _VWIN32_PulseWin32Event PAGEABLE__VWIN32_PulseWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_PulseWin32Event
#define VWIN32_PulseWin32Event PAGEABLE__VWIN32_PulseWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_ResetWin32Event
#define _VWIN32_ResetWin32Event PAGEABLE__VWIN32_ResetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_ResetWin32Event
#define VWIN32_ResetWin32Event PAGEABLE__VWIN32_ResetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_WaitSingleObject
#define _VWIN32_WaitSingleObject PAGEABLE__VWIN32_WaitSingleObject
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_WaitSingleObject
#define VWIN32_WaitSingleObject PAGEABLE__VWIN32_WaitSingleObject
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_WaitMultipleObjects
#define _VWIN32_WaitMultipleObjects PAGEABLE__VWIN32_WaitMultipleObjects
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_WaitMultipleObjects
#define VWIN32_WaitMultipleObjects PAGEABLE__VWIN32_WaitMultipleObjects
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CreateRing0Thread
#define _VWIN32_CreateRing0Thread PAGEABLE__VWIN32_CreateRing0Thread
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CreateRing0Thread
#define VWIN32_CreateRing0Thread PAGEABLE__VWIN32_CreateRing0Thread
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CloseVxDHandle
#define _VWIN32_CloseVxDHandle PAGEABLE__VWIN32_CloseVxDHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CloseVxDHandle
#define VWIN32_CloseVxDHandle PAGEABLE__VWIN32_CloseVxDHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_ActiveTimeBiasSet
#define VWIN32_ActiveTimeBiasSet PAGEABLE_VWIN32_ActiveTimeBiasSet
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_EnterCrstR0
#define VWIN32_EnterCrstR0 PAGEABLE_VWIN32_EnterCrstR0
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_BlueScreenPopup
#define VWIN32_BlueScreenPopup PAGEABLE_VWIN32_BlueScreenPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_TerminateApp
#define VWIN32_TerminateApp PAGEABLE_VWIN32_TerminateApp
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_QueueKernelAPC
#define _VWIN32_QueueKernelAPC PAGEABLE__VWIN32_QueueKernelAPC
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_QueueKernelAPC
#define VWIN32_QueueKernelAPC PAGEABLE__VWIN32_QueueKernelAPC
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_SysErrorBox
#define VWIN32_SysErrorBox PAGEABLE_VWIN32_SysErrorBox
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_IsClientWin32
#define _VWIN32_IsClientWin32 PAGEABLE__VWIN32_IsClientWin32
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_IsClientWin32
#define VWIN32_IsClientWin32 PAGEABLE__VWIN32_IsClientWin32
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Set_Thread_Context
#define _VWIN32_Set_Thread_Context PAGEABLE__VWIN32_Set_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Set_Thread_Context
#define VWIN32_Set_Thread_Context PAGEABLE__VWIN32_Set_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CopyMem
#define _VWIN32_CopyMem PAGEABLE__VWIN32_CopyMem
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CopyMem
#define VWIN32_CopyMem PAGEABLE__VWIN32_CopyMem
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Npx_Exception
#define _VWIN32_Npx_Exception PAGEABLE__VWIN32_Npx_Exception
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Npx_Exception
#define VWIN32_Npx_Exception PAGEABLE__VWIN32_Npx_Exception
#endif

#ifdef WIN40_OR_LATER
#undef  PCCARD_Get_Version
#define PCCARD_Get_Version PAGEABLE_PCCARD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PCCARD_Card_Services
#define PCCARD_Card_Services PAGEABLE_PCCARD_Card_Services
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Get_Version
#define IOS_Get_Version PAGEABLE_IOS_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_BD_Register_Device
#define IOS_BD_Register_Device PAGEABLE_IOS_BD_Register_Device
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Find_Int13_Drive
#define IOS_Find_Int13_Drive PAGEABLE_IOS_Find_Int13_Drive
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Get_Device_List
#define IOS_Get_Device_List PAGEABLE_IOS_Get_Device_List
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_SendCommand
#define IOS_SendCommand PAGEABLE_IOS_SendCommand
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Register
#define IOS_Register PAGEABLE_IOS_Register
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Requestor_Service
#define IOS_Requestor_Service PAGEABLE_IOS_Requestor_Service
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Exclusive_Access
#define IOS_Exclusive_Access PAGEABLE_IOS_Exclusive_Access
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Send_Next_Command
#define IOS_Send_Next_Command PAGEABLE_IOS_Send_Next_Command
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Set_Async_Time_Out
#define IOS_Set_Async_Time_Out PAGEABLE_IOS_Set_Async_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Signal_Semaphore_No_Switch
#define IOS_Signal_Semaphore_No_Switch PAGEABLE_IOS_Signal_Semaphore_No_Switch
#endif

#ifdef WIN40_OR_LATER
#undef  IOSIdleStatus
#define IOSIdleStatus PAGEABLE_IOSIdleStatus
#endif

#ifdef WIN40_OR_LATER
#undef  IOSMapIORSToI24
#define IOSMapIORSToI24 PAGEABLE_IOSMapIORSToI24
#endif

#ifdef WIN40_OR_LATER
#undef  IOSMapIORSToI21
#define IOSMapIORSToI21 PAGEABLE_IOSMapIORSToI21
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_BD_Command_Complete
#define IOS_BD_Command_Complete PAGEABLE_IOS_BD_Command_Complete
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_Register_Device_Driver
#define MMDEVLDR_Register_Device_Driver PAGEABLE_MMDEVLDR_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_SetDevicePresence
#define _MMDEVLDR_SetDevicePresence PAGEABLE__MMDEVLDR_SetDevicePresence
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_SetDevicePresence
#define MMDEVLDR_SetDevicePresence PAGEABLE__MMDEVLDR_SetDevicePresence
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_SetEnvironmentString
#define _MMDEVLDR_SetEnvironmentString PAGEABLE__MMDEVLDR_SetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_SetEnvironmentString
#define MMDEVLDR_SetEnvironmentString PAGEABLE__MMDEVLDR_SetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_GetEnvironmentString
#define _MMDEVLDR_GetEnvironmentString PAGEABLE__MMDEVLDR_GetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_GetEnvironmentString
#define MMDEVLDR_GetEnvironmentString PAGEABLE__MMDEVLDR_GetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_RemoveEnvironmentString
#define _MMDEVLDR_RemoveEnvironmentString PAGEABLE__MMDEVLDR_RemoveEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_RemoveEnvironmentString
#define MMDEVLDR_RemoveEnvironmentString PAGEABLE__MMDEVLDR_RemoveEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_AddEnvironmentString
#define _MMDEVLDR_AddEnvironmentString PAGEABLE__MMDEVLDR_AddEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_AddEnvironmentString
#define MMDEVLDR_AddEnvironmentString PAGEABLE__MMDEVLDR_AddEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Get_Virtualization_Count
#define VPICD_Get_Virtualization_Count PAGEABLE_VPICD_Get_Virtualization_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Phys_Mask_Channel
#define VDMAD_Phys_Mask_Channel PAGEABLE_VDMAD_Phys_Mask_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Phys_Unmask_Channel
#define VDMAD_Phys_Unmask_Channel PAGEABLE_VDMAD_Phys_Unmask_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Unvirtualize_Channel
#define VDMAD_Unvirtualize_Channel PAGEABLE_VDMAD_Unvirtualize_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_IO_Address
#define VDMAD_Set_IO_Address PAGEABLE_VDMAD_Set_IO_Address
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Convert_Hex_Binary
#define Debug_Convert_Hex_Binary PAGEABLE_Debug_Convert_Hex_Binary
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Convert_Hex_Decimal
#define Debug_Convert_Hex_Decimal PAGEABLE_Debug_Convert_Hex_Decimal
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Flags_Service
#define _Debug_Flags_Service PAGEABLE__Debug_Flags_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Flags_Service
#define Debug_Flags_Service PAGEABLE__Debug_Flags_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Cur_VM
#define Debug_Test_Cur_VM PAGEABLE_Debug_Test_Cur_VM
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Valid_Handle
#define Debug_Test_Valid_Handle PAGEABLE_Debug_Test_Valid_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_FindBlock
#define VCache_FindBlock PAGEABLE_VCache_FindBlock
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_FreeBlock
#define VCache_FreeBlock PAGEABLE_VCache_FreeBlock
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_GetSize
#define VCache_GetSize PAGEABLE_VCache_GetSize
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_GetStats
#define VCache_GetStats PAGEABLE_VCache_GetStats
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Get_Version
#define VCache_Get_Version PAGEABLE_VCache_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Hold
#define VCache_Hold PAGEABLE_VCache_Hold
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_MakeMRU
#define VCache_MakeMRU PAGEABLE_VCache_MakeMRU
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Register
#define VCache_Register PAGEABLE_VCache_Register
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_SwapBuffers
#define VCache_SwapBuffers PAGEABLE_VCache_SwapBuffers
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_TestHold
#define VCache_TestHold PAGEABLE_VCache_TestHold
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Unhold
#define VCache_Unhold PAGEABLE_VCache_Unhold
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_CloseLookupCache
#define _VCache_CloseLookupCache PAGEABLE__VCache_CloseLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CloseLookupCache
#define VCache_CloseLookupCache PAGEABLE__VCache_CloseLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_CreateLookupCache
#define _VCache_CreateLookupCache PAGEABLE__VCache_CreateLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CreateLookupCache
#define VCache_CreateLookupCache PAGEABLE__VCache_CreateLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_DeleteLookupCache
#define _VCache_DeleteLookupCache PAGEABLE__VCache_DeleteLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_DeleteLookupCache
#define VCache_DeleteLookupCache PAGEABLE__VCache_DeleteLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_Lookup
#define _VCache_Lookup PAGEABLE__VCache_Lookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Lookup
#define VCache_Lookup PAGEABLE__VCache_Lookup
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_UpdateLookup
#define _VCache_UpdateLookup PAGEABLE__VCache_UpdateLookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_UpdateLookup
#define VCache_UpdateLookup PAGEABLE__VCache_UpdateLookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_RelinquishPage
#define VCache_RelinquishPage PAGEABLE_VCache_RelinquishPage
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_UseThisPage
#define VCache_UseThisPage PAGEABLE_VCache_UseThisPage
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_RecalcSums
#define VCache_RecalcSums PAGEABLE_VCache_RecalcSums
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_TestHandle
#define VCache_TestHandle PAGEABLE_VCache_TestHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_VerifySums
#define VCache_VerifySums PAGEABLE_VCache_VerifySums
#endif

#ifdef WIN40_OR_LATER
#undef  DOSMGR_Alloc_Local_Sys_VM_Mem
#define DOSMGR_Alloc_Local_Sys_VM_Mem PAGEABLE_DOSMGR_Alloc_Local_Sys_VM_Mem
#endif

#ifdef WIN40_OR_LATER
#undef  DOSMGR_Init_UMB_Area
#define DOSMGR_Init_UMB_Area PAGEABLE_DOSMGR_Init_UMB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  _GetRegistryKey
#define _GetRegistryKey PAGEABLE__GetRegistryKey
#endif

#ifdef WIN40_OR_LATER
#undef  GetRegistryKey
#define GetRegistryKey PAGEABLE__GetRegistryKey
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Nest_Exec_Status
#define Get_Nest_Exec_Status PAGEABLE_Get_Nest_Exec_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _AddReclaimableItem
#define _AddReclaimableItem PAGEABLE__AddReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  AddReclaimableItem
#define AddReclaimableItem PAGEABLE__AddReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  _AtEventTime
#define _AtEventTime PAGEABLE__AtEventTime
#endif

#ifdef WIN40_OR_LATER
#undef  AtEventTime
#define AtEventTime PAGEABLE__AtEventTime
#endif

#ifdef WIN40_OR_LATER
#undef  Close_Boot_Log
#define Close_Boot_Log PAGEABLE_Close_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  EnableDisable_Boot_Log
#define EnableDisable_Boot_Log PAGEABLE_EnableDisable_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  _EnumReclaimableItem
#define _EnumReclaimableItem PAGEABLE__EnumReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  EnumReclaimableItem
#define EnumReclaimableItem PAGEABLE__EnumReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  Open_Boot_Log
#define Open_Boot_Log PAGEABLE_Open_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  _SetReclaimableItem
#define _SetReclaimableItem PAGEABLE__SetReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  SetReclaimableItem
#define SetReclaimableItem PAGEABLE__SetReclaimableItem
#endif

#undef  Hook_Device_Service_Ex
#define Hook_Device_Service_Ex PAGEABLE_Hook_Device_Service_Ex

#ifdef WIN40_OR_LATER
#undef  PERF_Get_Version
#define PERF_Get_Version PAGEABLE_PERF_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Register
#define PERF_Server_Register PAGEABLE_PERF_Server_Register
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Deregister
#define PERF_Server_Deregister PAGEABLE_PERF_Server_Deregister
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Add_Stat
#define PERF_Server_Add_Stat PAGEABLE_PERF_Server_Add_Stat
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Remove_Stat
#define PERF_Server_Remove_Stat PAGEABLE_PERF_Server_Remove_Stat
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service_Ex
#define Unhook_Device_Service_Ex PAGEABLE_Unhook_Device_Service_Ex
#endif

#undef  Call_Previous_Hook_Proc
#define Call_Previous_Hook_Proc PAGEABLE_Call_Previous_Hook_Proc

#ifdef WIN40_OR_LATER
#undef  dummy_935
#define dummy_935 PAGEABLE_dummy_935
#endif

#undef  Hook_Device_Service_C
#define Hook_Device_Service_C PAGEABLE_Hook_Device_Service_C

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service_C
#define Unhook_Device_Service_C PAGEABLE_Unhook_Device_Service_C
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_ID
#define _CONFIGMG_Add_ID PAGEABLE__CONFIGMG_Add_ID
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_ID
#define CONFIGMG_Add_ID PAGEABLE__CONFIGMG_Add_ID
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Enumerator_Function
#define _CONFIGMG_Register_Enumerator_Function PAGEABLE__CONFIGMG_Register_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Enumerator_Function
#define CONFIGMG_Register_Enumerator_Function PAGEABLE__CONFIGMG_Register_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Update_User_Activity
#define SHELL_Update_User_Activity PAGEABLE_SHELL_Update_User_Activity
#endif

#undef  VDMAD_Unlock_DMA_Region_No_Dirty
#define VDMAD_Unlock_DMA_Region_No_Dirty PAGEABLE_VDMAD_Unlock_DMA_Region_No_Dirty

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetCurrentDirectory
#define VWIN32_GetCurrentDirectory PAGEABLE_VWIN32_GetCurrentDirectory
#endif

#ifdef WIN40_OR_LATER
#undef  VXDLDR_UnloadMe
#define VXDLDR_UnloadMe PAGEABLE_VXDLDR_UnloadMe
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_InitCompleted
#define _PELDR_InitCompleted PAGEABLE__PELDR_InitCompleted
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_InitCompleted
#define PELDR_InitCompleted PAGEABLE__PELDR_InitCompleted
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_AddExportTable
#define _PELDR_AddExportTable PAGEABLE__PELDR_AddExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_AddExportTable
#define PELDR_AddExportTable PAGEABLE__PELDR_AddExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_FreeModule
#define _PELDR_FreeModule PAGEABLE__PELDR_FreeModule
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_FreeModule
#define PELDR_FreeModule PAGEABLE__PELDR_FreeModule
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetEntryPoint
#define _PELDR_GetEntryPoint PAGEABLE__PELDR_GetEntryPoint
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetEntryPoint
#define PELDR_GetEntryPoint PAGEABLE__PELDR_GetEntryPoint
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetModuleHandle
#define _PELDR_GetModuleHandle PAGEABLE__PELDR_GetModuleHandle
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetModuleHandle
#define PELDR_GetModuleHandle PAGEABLE__PELDR_GetModuleHandle
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetModuleUsage
#define _PELDR_GetModuleUsage PAGEABLE__PELDR_GetModuleUsage
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetModuleUsage
#define PELDR_GetModuleUsage PAGEABLE__PELDR_GetModuleUsage
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetProcAddress
#define _PELDR_GetProcAddress PAGEABLE__PELDR_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetProcAddress
#define PELDR_GetProcAddress PAGEABLE__PELDR_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_LoadModule
#define _PELDR_LoadModule PAGEABLE__PELDR_LoadModule
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_LoadModule
#define PELDR_LoadModule PAGEABLE__PELDR_LoadModule
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_LoadModuleEx
#define _PELDR_LoadModuleEx PAGEABLE__PELDR_LoadModuleEx
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_LoadModuleEx
#define PELDR_LoadModuleEx PAGEABLE__PELDR_LoadModuleEx
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_RemoveExportTable
#define _PELDR_RemoveExportTable PAGEABLE__PELDR_RemoveExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_RemoveExportTable
#define PELDR_RemoveExportTable PAGEABLE__PELDR_RemoveExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_AdjustMinimum
#define VCache_AdjustMinimum PAGEABLE_VCache_AdjustMinimum
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CheckAvail
#define VCache_CheckAvail PAGEABLE_VCache_CheckAvail
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Deregister
#define VCache_Deregister PAGEABLE_VCache_Deregister
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Enum
#define VCache_Enum PAGEABLE_VCache_Enum
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Enable_Keyboard
#define VKD_Enable_Keyboard PAGEABLE_VKD_Enable_Keyboard
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Disable_Keyboard
#define VKD_Disable_Keyboard PAGEABLE_VKD_Disable_Keyboard
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Get_Shift_State
#define VKD_Get_Shift_State PAGEABLE_VKD_Get_Shift_State
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Put_Byte
#define VKD_Put_Byte PAGEABLE_VKD_Put_Byte
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Set_Shift_State
#define VKD_Set_Shift_State PAGEABLE_VKD_Set_Shift_State
#endif

#undef  VCD_InitThunk_Control
#define VCD_InitThunk_Control PAGEABLE_VCD_InitThunk_Control

#undef  VCD_InitThunk_Irq
#define VCD_InitThunk_Irq PAGEABLE_VCD_InitThunk_Irq

#undef  VCD_InitThunk_Io
#define VCD_InitThunk_Io PAGEABLE_VCD_InitThunk_Io

#undef  VCD_Acquire_Port
#define VCD_Acquire_Port PAGEABLE_VCD_Acquire_Port

#undef  VCD_Free_Port_Windows_Style
#define VCD_Free_Port_Windows_Style PAGEABLE_VCD_Free_Port_Windows_Style

#undef  VCD_Steal_Port_Windows_Style
#define VCD_Steal_Port_Windows_Style PAGEABLE_VCD_Steal_Port_Windows_Style

#ifdef WIN40_OR_LATER
#undef  VCD_Find_COM_Index
#define VCD_Find_COM_Index PAGEABLE_VCD_Find_COM_Index
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Set_Port_Global_Special
#define VCD_Set_Port_Global_Special PAGEABLE_VCD_Set_Port_Global_Special
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Virtualize_Port_Dynamic
#define VCD_Virtualize_Port_Dynamic PAGEABLE_VCD_Virtualize_Port_Dynamic
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Unvirtualize_Port_Dynamic
#define VCD_Unvirtualize_Port_Dynamic PAGEABLE_VCD_Unvirtualize_Port_Dynamic
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Acquire_Port_Windows_Style
#define VCD_Acquire_Port_Windows_Style PAGEABLE_VCD_Acquire_Port_Windows_Style
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Free_Port
#define VCD_Free_Port PAGEABLE_VCD_Free_Port
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Check_Update_Soon
#define VDD_Check_Update_Soon PAGEABLE_VDD_Check_Update_Soon
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_GrabRtn
#define VDD_Get_GrabRtn PAGEABLE_VDD_Get_GrabRtn
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Hide_Cursor
#define VDD_Hide_Cursor PAGEABLE_VDD_Hide_Cursor
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Msg_SetCursPos
#define VDD_Msg_SetCursPos PAGEABLE_VDD_Msg_SetCursPos
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Enable_Disable_Mouse_Events
#define VMD_Enable_Disable_Mouse_Events PAGEABLE_VMD_Enable_Disable_Mouse_Events
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Init_File
#define PageFile_Init_File PAGEABLE_PageFile_Init_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Get_Version
#define PageFile_Get_Version PAGEABLE_PageFile_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Clean_Up
#define PageFile_Clean_Up PAGEABLE_PageFile_Clean_Up
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Grow_File
#define PageFile_Grow_File PAGEABLE_PageFile_Grow_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Read_Or_Write
#define PageFile_Read_Or_Write PAGEABLE_PageFile_Read_Or_Write
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Cancel
#define PageFile_Cancel PAGEABLE_PageFile_Cancel
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Test_IO_Valid
#define PageFile_Test_IO_Valid PAGEABLE_PageFile_Test_IO_Valid
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Get_Size_Info
#define PageFile_Get_Size_Info PAGEABLE_PageFile_Get_Size_Info
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Get_Version
#define PageSwap_Get_Version PAGEABLE_PageSwap_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Test_IO_Valid
#define PageSwap_Test_IO_Valid PAGEABLE_PageSwap_Test_IO_Valid
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Read_Or_Write
#define PageSwap_Read_Or_Write PAGEABLE_PageSwap_Read_Or_Write
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Grow_File
#define PageSwap_Grow_File PAGEABLE_PageSwap_Grow_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Init_File
#define PageSwap_Init_File PAGEABLE_PageSwap_Init_File
#endif

#undef  Allocate_PM_App_CB_Area
#define Allocate_PM_App_CB_Area PAGEABLE_Allocate_PM_App_CB_Area

#ifdef WIN40_OR_LATER
#undef  _Call_On_My_Stack
#define _Call_On_My_Stack PAGEABLE__Call_On_My_Stack
#endif

#ifdef WIN40_OR_LATER
#undef  Call_On_My_Stack
#define Call_On_My_Stack PAGEABLE__Call_On_My_Stack
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Printf_Service
#define _Debug_Printf_Service PAGEABLE__Debug_Printf_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Printf_Service
#define Debug_Printf_Service PAGEABLE__Debug_Printf_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _strupr
#define _strupr PAGEABLE__strupr
#endif

#ifdef WIN40_OR_LATER
#undef  strupr
#define strupr PAGEABLE__strupr
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Do_Physical_IO
#define VDD_Do_Physical_IO PAGEABLE_VDD_Do_Physical_IO
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_DISPLAYINFO
#define VDD_Get_DISPLAYINFO PAGEABLE_VDD_Get_DISPLAYINFO
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_Mini_Dispatch_Table
#define VDD_Get_Mini_Dispatch_Table PAGEABLE_VDD_Get_Mini_Dispatch_Table
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_Special_VM_IDs
#define VDD_Get_Special_VM_IDs PAGEABLE_VDD_Get_Special_VM_IDs
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_VM_Info
#define VDD_Get_VM_Info PAGEABLE_VDD_Get_VM_Info
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Register_Extra_Screen_Selector
#define VDD_Register_Extra_Screen_Selector PAGEABLE_VDD_Register_Extra_Screen_Selector
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Register_Virtual_Port
#define VDD_Register_Virtual_Port PAGEABLE_VDD_Register_Virtual_Port
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Set_Sleep_Flag_Addr
#define VDD_Set_Sleep_Flag_Addr PAGEABLE_VDD_Set_Sleep_Flag_Addr
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Takeover_VGA_Port
#define VDD_Takeover_VGA_Port PAGEABLE_VDD_Takeover_VGA_Port
#endif

#undef  Init_Generic_Thunk
#define Init_Generic_Thunk PAGEABLE_Init_Generic_Thunk

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Enumerate_DevNodes
#define _VCOMM_Enumerate_DevNodes PAGEABLE__VCOMM_Enumerate_DevNodes
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Enumerate_DevNodes
#define VCOMM_Enumerate_DevNodes PAGEABLE__VCOMM_Enumerate_DevNodes
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Virt_Count
#define VDMAD_Get_Virt_Count PAGEABLE_VDMAD_Get_Virt_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_Channel_Callbacks
#define VDMAD_Set_Channel_Callbacks PAGEABLE_VDMAD_Set_Channel_Callbacks
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Phys_Status
#define VDMAD_Get_Phys_Status PAGEABLE_VDMAD_Get_Phys_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Phys_Count
#define VDMAD_Get_Phys_Count PAGEABLE_VDMAD_Get_Phys_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Max_Phys_Page
#define VDMAD_Get_Max_Phys_Page PAGEABLE_VDMAD_Get_Max_Phys_Page
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_Virt_Count
#define VDMAD_Set_Virt_Count PAGEABLE_VDMAD_Set_Virt_Count
#endif

#ifdef WIN40_OR_LATER
#undef  _EnterMustComplete
#define _EnterMustComplete PAGEABLE__EnterMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  EnterMustComplete
#define EnterMustComplete PAGEABLE__EnterMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _LeaveMustComplete
#define _LeaveMustComplete PAGEABLE__LeaveMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  LeaveMustComplete
#define LeaveMustComplete PAGEABLE__LeaveMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _ResumeExecMustComplete
#define _ResumeExecMustComplete PAGEABLE__ResumeExecMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ResumeExecMustComplete
#define ResumeExecMustComplete PAGEABLE__ResumeExecMustComplete
#endif

#undef  VTDI_Get_Version
#define VTDI_Get_Version PAGEABLE_VTDI_Get_Version

#undef  VTDI_Get_Info
#define VTDI_Get_Info PAGEABLE_VTDI_Get_Info

#undef  VTDI_Start_Timer
#define VTDI_Start_Timer PAGEABLE_VTDI_Start_Timer

#undef  VTDI_Stop_Timer
#define VTDI_Stop_Timer PAGEABLE_VTDI_Stop_Timer

#undef  dummy_1034
#define dummy_1034 PAGEABLE_dummy_1034

#undef  dummy_1035
#define dummy_1035 PAGEABLE_dummy_1035

#undef  VTDI_Schedule_Event
#define VTDI_Schedule_Event PAGEABLE_VTDI_Schedule_Event

#undef  VTDI_Cancel_Event
#define VTDI_Cancel_Event PAGEABLE_VTDI_Cancel_Event

#undef  VTDI_Block
#define VTDI_Block PAGEABLE_VTDI_Block

#undef  VTDI_Signal
#define VTDI_Signal PAGEABLE_VTDI_Signal

#undef  VTDI_Register
#define VTDI_Register PAGEABLE_VTDI_Register

#ifdef WIN40_OR_LATER
#undef  VTDI_Unload
#define VTDI_Unload PAGEABLE_VTDI_Unload
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Initialize
#define VTDI_Initialize PAGEABLE_VTDI_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Register_UnloadProc
#define VTDI_Register_UnloadProc PAGEABLE_VTDI_Register_UnloadProc
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Register_LoadProc
#define VTDI_Register_LoadProc PAGEABLE_VTDI_Register_LoadProc
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Thunk_HWInt_Ex
#define VPICD_Thunk_HWInt_Ex PAGEABLE_VPICD_Thunk_HWInt_Ex
#endif

#ifdef WIN40_OR_LATER
#undef  Call_Restricted_Event_Ex
#define Call_Restricted_Event_Ex PAGEABLE_Call_Restricted_Event_Ex
#endif

#ifdef WIN41_OR_LATER
#undef  NTKERN_Get_Version
#define NTKERN_Get_Version PAGEABLE_NTKERN_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernCreateFile
#define NtKernCreateFile PAGEABLE_NtKernCreateFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernClose
#define NtKernClose PAGEABLE_NtKernClose
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernReadFile
#define NtKernReadFile PAGEABLE_NtKernReadFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernWriteFile
#define NtKernWriteFile PAGEABLE_NtKernWriteFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernDeviceIoControl
#define NtKernDeviceIoControl PAGEABLE_NtKernDeviceIoControl
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernGetWorkerThread
#define NtKernGetWorkerThread PAGEABLE_NtKernGetWorkerThread
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernLoadDriver
#define NtKernLoadDriver PAGEABLE_NtKernLoadDriver
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernQueueWorkItem
#define NtKernQueueWorkItem PAGEABLE_NtKernQueueWorkItem
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernPhysicalDeviceObjectToDevNode
#define NtKernPhysicalDeviceObjectToDevNode PAGEABLE_NtKernPhysicalDeviceObjectToDevNode
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernSetPhysicalCacheTypeRange
#define NtKernSetPhysicalCacheTypeRange PAGEABLE_NtKernSetPhysicalCacheTypeRange
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernWin9XLoadDriver
#define NtKernWin9XLoadDriver PAGEABLE_NtKernWin9XLoadDriver
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernCancelIoFile
#define NtKernCancelIoFile PAGEABLE_NtKernCancelIoFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernGetVPICDHandleFromInterruptObj
#define NtKernGetVPICDHandleFromInterruptObj PAGEABLE_NtKernGetVPICDHandleFromInterruptObj
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernInternalDeviceIoControl
#define NtKernInternalDeviceIoControl PAGEABLE_NtKernInternalDeviceIoControl
#endif

#ifdef WIN41_OR_LATER
#undef  _KeSaveFloatingPointState
#define _KeSaveFloatingPointState PAGEABLE__KeSaveFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  KeSaveFloatingPointState
#define KeSaveFloatingPointState PAGEABLE__KeSaveFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  _KeRestoreFloatingPointState
#define _KeRestoreFloatingPointState PAGEABLE__KeRestoreFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  KeRestoreFloatingPointState
#define KeRestoreFloatingPointState PAGEABLE__KeRestoreFloatingPointState
#endif

#ifdef WIN40_OR_LATER
#undef  VTD_Delay
#define VTD_Delay PAGEABLE_VTD_Delay
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Get_Version
#define _PCI_Get_Version PAGEABLE__PCI_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Get_Version
#define PCI_Get_Version PAGEABLE__PCI_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Read_Config
#define _PCI_Read_Config PAGEABLE__PCI_Read_Config
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Read_Config
#define PCI_Read_Config PAGEABLE__PCI_Read_Config
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Write_Config
#define _PCI_Write_Config PAGEABLE__PCI_Write_Config
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Write_Config
#define PCI_Write_Config PAGEABLE__PCI_Write_Config
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Lock_Unlock
#define _PCI_Lock_Unlock PAGEABLE__PCI_Lock_Unlock
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Lock_Unlock
#define PCI_Lock_Unlock PAGEABLE__PCI_Lock_Unlock
#endif

#ifdef WIN41_OR_LATER
#undef  _PCCARD_Access_CIS_Memory
#define _PCCARD_Access_CIS_Memory PAGEABLE__PCCARD_Access_CIS_Memory
#endif

#ifdef WIN41_OR_LATER
#undef  PCCARD_Access_CIS_Memory
#define PCCARD_Access_CIS_Memory PAGEABLE__PCCARD_Access_CIS_Memory
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Get_DevNode_PowerCapabilities
#define _CONFIGMG_Get_DevNode_PowerCapabilities PAGEABLE__CONFIGMG_Get_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Get_DevNode_PowerCapabilities
#define CONFIGMG_Get_DevNode_PowerCapabilities PAGEABLE__CONFIGMG_Get_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Set_DevNode_PowerCapabilities
#define _CONFIGMG_Set_DevNode_PowerCapabilities PAGEABLE__CONFIGMG_Set_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Set_DevNode_PowerCapabilities
#define CONFIGMG_Set_DevNode_PowerCapabilities PAGEABLE__CONFIGMG_Set_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Get_DevNode_PowerState
#define _CONFIGMG_Get_DevNode_PowerState PAGEABLE__CONFIGMG_Get_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Get_DevNode_PowerState
#define CONFIGMG_Get_DevNode_PowerState PAGEABLE__CONFIGMG_Get_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Set_DevNode_PowerState
#define _CONFIGMG_Set_DevNode_PowerState PAGEABLE__CONFIGMG_Set_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Set_DevNode_PowerState
#define CONFIGMG_Set_DevNode_PowerState PAGEABLE__CONFIGMG_Set_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Register_DevNode_For_Idle_Detection
#define _CONFIGMG_Register_DevNode_For_Idle_Detection PAGEABLE__CONFIGMG_Register_DevNode_For_Idle_Detection
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Register_DevNode_For_Idle_Detection
#define CONFIGMG_Register_DevNode_For_Idle_Detection PAGEABLE__CONFIGMG_Register_DevNode_For_Idle_Detection
#endif

