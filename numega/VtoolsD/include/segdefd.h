//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  _XchgFreePhysReg
#define _XchgFreePhysReg DEBUG__XchgFreePhysReg

#undef  XchgFreePhysReg
#define XchgFreePhysReg DEBUG__XchgFreePhysReg

#undef  Wake_Up_VM
#define Wake_Up_VM DEBUG_Wake_Up_VM

#undef  Wait_Semaphore
#define Wait_Semaphore DEBUG_Wait_Semaphore

#undef  Validate_VM_Handle
#define Validate_VM_Handle DEBUG_Validate_VM_Handle

#undef  Validate_Client_Ptr
#define Validate_Client_Ptr DEBUG_Validate_Client_Ptr

#undef  Update_System_Clock
#define Update_System_Clock DEBUG_Update_System_Clock

#undef  _UnmapFreePhysReg
#define _UnmapFreePhysReg DEBUG__UnmapFreePhysReg

#undef  UnmapFreePhysReg
#define UnmapFreePhysReg DEBUG__UnmapFreePhysReg

#undef  Unhook_Invalid_Page_Fault
#define Unhook_Invalid_Page_Fault DEBUG_Unhook_Invalid_Page_Fault

#undef  Test_Sys_VM_Handle
#define Test_Sys_VM_Handle DEBUG_Test_Sys_VM_Handle

#undef  _TestGlobalV86Mem
#define _TestGlobalV86Mem DEBUG__TestGlobalV86Mem

#undef  TestGlobalV86Mem
#define TestGlobalV86Mem DEBUG__TestGlobalV86Mem

#undef  Test_Debug_Installed
#define Test_Debug_Installed DEBUG_Test_Debug_Installed

#undef  Test_DBCS_Lead_Byte
#define Test_DBCS_Lead_Byte DEBUG_Test_DBCS_Lead_Byte

#undef  Test_Cur_VM_Handle
#define Test_Cur_VM_Handle DEBUG_Test_Cur_VM_Handle

#undef  System_Control
#define System_Control DEBUG_System_Control

#undef  Suspend_VM
#define Suspend_VM DEBUG_Suspend_VM

#undef  Simulate_Push
#define Simulate_Push DEBUG_Simulate_Push

#undef  Simulate_Pop
#define Simulate_Pop DEBUG_Simulate_Pop

#undef  Simulate_Int
#define Simulate_Int DEBUG_Simulate_Int

#undef  Simulate_Far_Ret_N
#define Simulate_Far_Ret_N DEBUG_Simulate_Far_Ret_N

#undef  Simulate_Far_Jmp
#define Simulate_Far_Jmp DEBUG_Simulate_Far_Jmp

#undef  Simulate_Far_Call
#define Simulate_Far_Call DEBUG_Simulate_Far_Call

#undef  Signal_Semaphore
#define Signal_Semaphore DEBUG_Signal_Semaphore

#undef  Set_VM_Time_Out
#define Set_VM_Time_Out DEBUG_Set_VM_Time_Out

#undef  Set_V86_Int_Vector
#define Set_V86_Int_Vector DEBUG_Set_V86_Int_Vector

#undef  Set_PM_Int_Vector
#define Set_PM_Int_Vector DEBUG_Set_PM_Int_Vector

#undef  Set_Time_Slice_Priority
#define Set_Time_Slice_Priority DEBUG_Set_Time_Slice_Priority

#undef  Set_Time_Slice_Granularity
#define Set_Time_Slice_Granularity DEBUG_Set_Time_Slice_Granularity

#undef  Set_System_Exit_Code
#define Set_System_Exit_Code DEBUG_Set_System_Exit_Code

#undef  _SetResetV86Pageable
#define _SetResetV86Pageable DEBUG__SetResetV86Pageable

#undef  SetResetV86Pageable
#define SetResetV86Pageable DEBUG__SetResetV86Pageable

#undef  Set_PM_Int_Type
#define Set_PM_Int_Type DEBUG_Set_PM_Int_Type

#undef  Set_Physical_HMA_Alias
#define Set_Physical_HMA_Alias DEBUG_Set_Physical_HMA_Alias

#undef  Set_NMI_Handler_Addr
#define Set_NMI_Handler_Addr DEBUG_Set_NMI_Handler_Addr

#undef  Set_Mono_Cur_Pos
#define Set_Mono_Cur_Pos DEBUG_Set_Mono_Cur_Pos

#undef  _SetLastV86Page
#define _SetLastV86Page DEBUG__SetLastV86Page

#undef  SetLastV86Page
#define SetLastV86Page DEBUG__SetLastV86Page

#undef  Set_Global_Time_Out
#define Set_Global_Time_Out DEBUG_Set_Global_Time_Out

#undef  _SetFreePhysRegCalBk
#define _SetFreePhysRegCalBk DEBUG__SetFreePhysRegCalBk

#undef  SetFreePhysRegCalBk
#define SetFreePhysRegCalBk DEBUG__SetFreePhysRegCalBk

#undef  Set_Execution_Focus
#define Set_Execution_Focus DEBUG_Set_Execution_Focus

#undef  _SetDescriptor
#define _SetDescriptor DEBUG__SetDescriptor

#undef  SetDescriptor
#define SetDescriptor DEBUG__SetDescriptor

#undef  Set_Delete_On_Exit_File
#define Set_Delete_On_Exit_File DEBUG_Set_Delete_On_Exit_File

#undef  _SelectorMapFlat
#define _SelectorMapFlat DEBUG__SelectorMapFlat

#undef  SelectorMapFlat
#define SelectorMapFlat DEBUG__SelectorMapFlat

#undef  Schedule_VM_Event
#define Schedule_VM_Event DEBUG_Schedule_VM_Event

#undef  Schedule_Global_Event
#define Schedule_Global_Event DEBUG_Schedule_Global_Event

#undef  Save_Client_State
#define Save_Client_State DEBUG_Save_Client_State

#undef  Resume_VM
#define Resume_VM DEBUG_Resume_VM

#undef  Restore_Client_State
#define Restore_Client_State DEBUG_Restore_Client_State

#undef  Remove_V86_Break_Point
#define Remove_V86_Break_Point DEBUG_Remove_V86_Break_Point

#undef  Remove_Exception_Handler
#define Remove_Exception_Handler DEBUG_Remove_Exception_Handler

#undef  Release_Critical_Section
#define Release_Critical_Section DEBUG_Release_Critical_Section

#undef  Queue_Debug_String
#define Queue_Debug_String DEBUG_Queue_Debug_String

#undef  _PhysIntoV86
#define _PhysIntoV86 DEBUG__PhysIntoV86

#undef  PhysIntoV86
#define PhysIntoV86 DEBUG__PhysIntoV86

#undef  _PageUnLock
#define _PageUnLock DEBUG__PageUnLock

#undef  PageUnLock
#define PageUnLock DEBUG__PageUnLock

#undef  _PageResetHandlePAddr
#define _PageResetHandlePAddr DEBUG__PageResetHandlePAddr

#undef  PageResetHandlePAddr
#define PageResetHandlePAddr DEBUG__PageResetHandlePAddr

#undef  _PageReAllocate
#define _PageReAllocate DEBUG__PageReAllocate

#undef  PageReAllocate
#define PageReAllocate DEBUG__PageReAllocate

#undef  _PageOutDirtyPages
#define _PageOutDirtyPages DEBUG__PageOutDirtyPages

#undef  PageOutDirtyPages
#define PageOutDirtyPages DEBUG__PageOutDirtyPages

#undef  _PageLock
#define _PageLock DEBUG__PageLock

#undef  PageLock
#define PageLock DEBUG__PageLock

#undef  _PageGetSizeAddr
#define _PageGetSizeAddr DEBUG__PageGetSizeAddr

#undef  PageGetSizeAddr
#define PageGetSizeAddr DEBUG__PageGetSizeAddr

#undef  _PageGetAllocInfo
#define _PageGetAllocInfo DEBUG__PageGetAllocInfo

#undef  PageGetAllocInfo
#define PageGetAllocInfo DEBUG__PageGetAllocInfo

#undef  _PageFree
#define _PageFree DEBUG__PageFree

#undef  PageFree
#define PageFree DEBUG__PageFree

#undef  _PageDiscardPages
#define _PageDiscardPages DEBUG__PageDiscardPages

#undef  PageDiscardPages
#define PageDiscardPages DEBUG__PageDiscardPages

#undef  _PageCheckLinRange
#define _PageCheckLinRange DEBUG__PageCheckLinRange

#undef  PageCheckLinRange
#define PageCheckLinRange DEBUG__PageCheckLinRange

#undef  _PageAllocate
#define _PageAllocate DEBUG__PageAllocate

#undef  PageAllocate
#define PageAllocate DEBUG__PageAllocate

#undef  Out_Mono_String
#define Out_Mono_String DEBUG_Out_Mono_String

#undef  Out_Mono_Chr_Attrs
#define Out_Mono_Chr_Attrs DEBUG_Out_Mono_Chr_Attrs

#undef  Out_Mono_Chr
#define Out_Mono_Chr DEBUG_Out_Mono_Chr

#undef  Out_Debug_String
#define Out_Debug_String DEBUG_Out_Debug_String

#undef  Out_Debug_Chr
#define Out_Debug_Chr DEBUG_Out_Debug_Chr

#undef  OpenFile
#define OpenFile DEBUG_OpenFile

#undef  Nuke_VM
#define Nuke_VM DEBUG_Nuke_VM

#undef  No_Fail_Resume_VM
#define No_Fail_Resume_VM DEBUG_No_Fail_Resume_VM

#undef  _ModifyPageBits
#define _ModifyPageBits DEBUG__ModifyPageBits

#undef  ModifyPageBits
#define ModifyPageBits DEBUG__ModifyPageBits

#undef  _MMGR_Toggle_HMA
#define _MMGR_Toggle_HMA DEBUG__MMGR_Toggle_HMA

#undef  MMGR_Toggle_HMA
#define MMGR_Toggle_HMA DEBUG__MMGR_Toggle_HMA

#undef  MMGR_SetNULPageAddr
#define MMGR_SetNULPageAddr DEBUG_MMGR_SetNULPageAddr

#undef  _MapPhysToLinear
#define _MapPhysToLinear DEBUG__MapPhysToLinear

#undef  MapPhysToLinear
#define MapPhysToLinear DEBUG__MapPhysToLinear

#undef  Map_Lin_To_VM_Addr
#define Map_Lin_To_VM_Addr DEBUG_Map_Lin_To_VM_Addr

#undef  _MapIntoV86
#define _MapIntoV86 DEBUG__MapIntoV86

#undef  MapIntoV86
#define MapIntoV86 DEBUG__MapIntoV86

#undef  _MapFreePhysReg
#define _MapFreePhysReg DEBUG__MapFreePhysReg

#undef  MapFreePhysReg
#define MapFreePhysReg DEBUG__MapFreePhysReg

#undef  Map_Flat
#define Map_Flat DEBUG_Map_Flat

#undef  Locate_Byte_In_ROM
#define Locate_Byte_In_ROM DEBUG_Locate_Byte_In_ROM

#undef  List_Remove_First
#define List_Remove_First DEBUG_List_Remove_First

#undef  List_Remove
#define List_Remove DEBUG_List_Remove

#undef  List_Insert
#define List_Insert DEBUG_List_Insert

#undef  List_Get_Next
#define List_Get_Next DEBUG_List_Get_Next

#undef  List_Get_First
#define List_Get_First DEBUG_List_Get_First

#undef  List_Destroy
#define List_Destroy DEBUG_List_Destroy

#undef  List_Deallocate
#define List_Deallocate DEBUG_List_Deallocate

#undef  List_Create
#define List_Create DEBUG_List_Create

#undef  List_Attach_Tail
#define List_Attach_Tail DEBUG_List_Attach_Tail

#undef  List_Attach
#define List_Attach DEBUG_List_Attach

#undef  List_Allocate
#define List_Allocate DEBUG_List_Allocate

#undef  _LinPageUnLock
#define _LinPageUnLock DEBUG__LinPageUnLock

#undef  LinPageUnLock
#define LinPageUnLock DEBUG__LinPageUnLock

#undef  _LinPageLock
#define _LinPageLock DEBUG__LinPageLock

#undef  LinPageLock
#define LinPageLock DEBUG__LinPageLock

#undef  _LinMapIntoV86
#define _LinMapIntoV86 DEBUG__LinMapIntoV86

#undef  LinMapIntoV86
#define LinMapIntoV86 DEBUG__LinMapIntoV86

#undef  Is_Debug_Chr
#define Is_Debug_Chr DEBUG_Is_Debug_Chr

#undef  Install_V86_Break_Point
#define Install_V86_Break_Point DEBUG_Install_V86_Break_Point

#undef  Install_IO_Handler
#define Install_IO_Handler DEBUG_Install_IO_Handler

#undef  In_Debug_Chr
#define In_Debug_Chr DEBUG_In_Debug_Chr

#undef  Hook_VMM_Fault
#define Hook_VMM_Fault DEBUG_Hook_VMM_Fault

#undef  Hook_V86_Page
#define Hook_V86_Page DEBUG_Hook_V86_Page

#undef  Hook_V86_Int_Chain
#define Hook_V86_Int_Chain DEBUG_Hook_V86_Int_Chain

#undef  Hook_V86_Fault
#define Hook_V86_Fault DEBUG_Hook_V86_Fault

#undef  Hook_PM_Fault
#define Hook_PM_Fault DEBUG_Hook_PM_Fault

#undef  Hook_Device_PM_API
#define Hook_Device_PM_API DEBUG_Hook_Device_PM_API

#undef  Hook_NMI_Event
#define Hook_NMI_Event DEBUG_Hook_NMI_Event

#undef  Hook_Invalid_Page_Fault
#define Hook_Invalid_Page_Fault DEBUG_Hook_Invalid_Page_Fault

#undef  Hook_Device_V86_API
#define Hook_Device_V86_API DEBUG_Hook_Device_V86_API

#undef  Hook_Device_Service
#define Hook_Device_Service DEBUG_Hook_Device_Service

#undef  _HeapReAllocate
#define _HeapReAllocate DEBUG__HeapReAllocate

#undef  HeapReAllocate
#define HeapReAllocate DEBUG__HeapReAllocate

#undef  _HeapGetSize
#define _HeapGetSize DEBUG__HeapGetSize

#undef  HeapGetSize
#define HeapGetSize DEBUG__HeapGetSize

#undef  _HeapFree
#define _HeapFree DEBUG__HeapFree

#undef  HeapFree
#define HeapFree DEBUG__HeapFree

#undef  _HeapAllocate
#define _HeapAllocate DEBUG__HeapAllocate

#undef  HeapAllocate
#define HeapAllocate DEBUG__HeapAllocate

#undef  _GetVMPgCount
#define _GetVMPgCount DEBUG__GetVMPgCount

#undef  GetVMPgCount
#define GetVMPgCount DEBUG__GetVMPgCount

#undef  Get_VMM_Version
#define Get_VMM_Version DEBUG_Get_VMM_Version

#undef  Get_VMM_Reenter_Count
#define Get_VMM_Reenter_Count DEBUG_Get_VMM_Reenter_Count

#undef  Get_VM_Exec_Time
#define Get_VM_Exec_Time DEBUG_Get_VM_Exec_Time

#undef  _GetV86PageableArray
#define _GetV86PageableArray DEBUG__GetV86PageableArray

#undef  GetV86PageableArray
#define GetV86PageableArray DEBUG__GetV86PageableArray

#undef  Get_V86_Int_Vector
#define Get_V86_Int_Vector DEBUG_Get_V86_Int_Vector

#undef  Get_Time_Slice_Priority
#define Get_Time_Slice_Priority DEBUG_Get_Time_Slice_Priority

#undef  Get_Time_Slice_Info
#define Get_Time_Slice_Info DEBUG_Get_Time_Slice_Info

#undef  Get_Time_Slice_Granularity
#define Get_Time_Slice_Granularity DEBUG_Get_Time_Slice_Granularity

#undef  Get_Sys_VM_Handle
#define Get_Sys_VM_Handle DEBUG_Get_Sys_VM_Handle

#undef  Get_System_Time
#define Get_System_Time DEBUG_Get_System_Time

#undef  _GetSetPageOutCount
#define _GetSetPageOutCount DEBUG__GetSetPageOutCount

#undef  GetSetPageOutCount
#define GetSetPageOutCount DEBUG__GetSetPageOutCount

#undef  _GetSysPageCount
#define _GetSysPageCount DEBUG__GetSysPageCount

#undef  GetSysPageCount
#define GetSysPageCount DEBUG__GetSysPageCount

#undef  Set_HMA_Info
#define Set_HMA_Info DEBUG_Set_HMA_Info

#undef  Get_HMA_Info
#define Get_HMA_Info DEBUG_Get_HMA_Info

#undef  GetDetailedVMError
#define GetDetailedVMError DEBUG_GetDetailedVMError

#undef  SetDetailedVMError
#define SetDetailedVMError DEBUG_SetDetailedVMError

#undef  Get_PSP_Segment
#define Get_PSP_Segment DEBUG_Get_PSP_Segment

#undef  Get_Profile_String
#define Get_Profile_String DEBUG_Get_Profile_String

#undef  Get_Profile_Hex_Int
#define Get_Profile_Hex_Int DEBUG_Get_Profile_Hex_Int

#undef  Get_Profile_Fixed_Point
#define Get_Profile_Fixed_Point DEBUG_Get_Profile_Fixed_Point

#undef  Get_Profile_Decimal_Int
#define Get_Profile_Decimal_Int DEBUG_Get_Profile_Decimal_Int

#undef  Get_Profile_Boolean
#define Get_Profile_Boolean DEBUG_Get_Profile_Boolean

#undef  Get_PM_Int_Vector
#define Get_PM_Int_Vector DEBUG_Get_PM_Int_Vector

#undef  Get_PM_Int_Type
#define Get_PM_Int_Type DEBUG_Get_PM_Int_Type

#undef  _GetNulPageHandle
#define _GetNulPageHandle DEBUG__GetNulPageHandle

#undef  GetNulPageHandle
#define GetNulPageHandle DEBUG__GetNulPageHandle

#undef  Get_NMI_Handler_Addr
#define Get_NMI_Handler_Addr DEBUG_Get_NMI_Handler_Addr

#undef  Get_Next_VM_Handle
#define Get_Next_VM_Handle DEBUG_Get_Next_VM_Handle

#undef  Get_Next_Profile_String
#define Get_Next_Profile_String DEBUG_Get_Next_Profile_String

#undef  Get_Name_Of_Ugly_TSR
#define Get_Name_Of_Ugly_TSR DEBUG_Get_Name_Of_Ugly_TSR

#undef  Get_Mono_Cur_Pos
#define Get_Mono_Cur_Pos DEBUG_Get_Mono_Cur_Pos

#undef  Get_Mono_Chr
#define Get_Mono_Chr DEBUG_Get_Mono_Chr

#undef  Get_Machine_Info
#define Get_Machine_Info DEBUG_Get_Machine_Info

#undef  Get_Last_Updated_VM_Exec_Time
#define Get_Last_Updated_VM_Exec_Time DEBUG_Get_Last_Updated_VM_Exec_Time

#undef  _GetLastV86Page
#define _GetLastV86Page DEBUG__GetLastV86Page

#undef  GetLastV86Page
#define GetLastV86Page DEBUG__GetLastV86Page

#undef  Get_Last_Updated_System_Time
#define Get_Last_Updated_System_Time DEBUG_Get_Last_Updated_System_Time

#undef  _GetGlblRng0V86IntBase
#define _GetGlblRng0V86IntBase DEBUG__GetGlblRng0V86IntBase

#undef  GetGlblRng0V86IntBase
#define GetGlblRng0V86IntBase DEBUG__GetGlblRng0V86IntBase

#undef  _GetFreePageCount
#define _GetFreePageCount DEBUG__GetFreePageCount

#undef  GetFreePageCount
#define GetFreePageCount DEBUG__GetFreePageCount

#undef  Get_Fault_Hook_Addrs
#define Get_Fault_Hook_Addrs DEBUG_Get_Fault_Hook_Addrs

#undef  Get_Exec_Path
#define Get_Exec_Path DEBUG_Get_Exec_Path

#undef  Get_Execution_Focus
#define Get_Execution_Focus DEBUG_Get_Execution_Focus

#undef  Get_Environment_String
#define Get_Environment_String DEBUG_Get_Environment_String

#undef  _Get_Device_V86_Pages_Array
#define _Get_Device_V86_Pages_Array DEBUG__Get_Device_V86_Pages_Array

#undef  Get_Device_V86_Pages_Array
#define Get_Device_V86_Pages_Array DEBUG__Get_Device_V86_Pages_Array

#undef  _GetDescriptor
#define _GetDescriptor DEBUG__GetDescriptor

#undef  GetDescriptor
#define GetDescriptor DEBUG__GetDescriptor

#undef  _GetDemandPageInfo
#define _GetDemandPageInfo DEBUG__GetDemandPageInfo

#undef  GetDemandPageInfo
#define GetDemandPageInfo DEBUG__GetDemandPageInfo

#undef  Get_Debug_Options
#define Get_Debug_Options DEBUG_Get_Debug_Options

#undef  Get_Cur_VM_Handle
#define Get_Cur_VM_Handle DEBUG_Get_Cur_VM_Handle

#undef  Get_Crit_Status_No_Block
#define Get_Crit_Status_No_Block DEBUG_Get_Crit_Status_No_Block

#undef  Get_Crit_Section_Status
#define Get_Crit_Section_Status DEBUG_Get_Crit_Section_Status

#undef  Get_Cur_PM_App_CB
#define Get_Cur_PM_App_CB DEBUG_Get_Cur_PM_App_CB

#undef  Get_Config_Directory
#define Get_Config_Directory DEBUG_Get_Config_Directory

#undef  _GetFirstV86Page
#define _GetFirstV86Page DEBUG__GetFirstV86Page

#undef  GetFirstV86Page
#define GetFirstV86Page DEBUG__GetFirstV86Page

#undef  _GetAppFlatDSAlias
#define _GetAppFlatDSAlias DEBUG__GetAppFlatDSAlias

#undef  GetAppFlatDSAlias
#define GetAppFlatDSAlias DEBUG__GetAppFlatDSAlias

#undef  _Free_Temp_V86_Data_Area
#define _Free_Temp_V86_Data_Area DEBUG__Free_Temp_V86_Data_Area

#undef  Free_Temp_V86_Data_Area
#define Free_Temp_V86_Data_Area DEBUG__Free_Temp_V86_Data_Area

#undef  _Free_LDT_Selector
#define _Free_LDT_Selector DEBUG__Free_LDT_Selector

#undef  Free_LDT_Selector
#define Free_LDT_Selector DEBUG__Free_LDT_Selector

#undef  _Free_GDT_Selector
#define _Free_GDT_Selector DEBUG__Free_GDT_Selector

#undef  Free_GDT_Selector
#define Free_GDT_Selector DEBUG__Free_GDT_Selector

#undef  Fatal_Error_Handler
#define Fatal_Error_Handler DEBUG_Fatal_Error_Handler

#undef  Exec_VxD_Int
#define Exec_VxD_Int DEBUG_Exec_VxD_Int

#undef  Exec_Int
#define Exec_Int DEBUG_Exec_Int

#undef  End_Reentrant_Execution
#define End_Reentrant_Execution DEBUG_End_Reentrant_Execution

#undef  End_Crit_And_Suspend
#define End_Crit_And_Suspend DEBUG_End_Crit_And_Suspend

#undef  Enable_Local_Trapping
#define Enable_Local_Trapping DEBUG_Enable_Local_Trapping

#undef  Enable_Global_Trapping
#define Enable_Global_Trapping DEBUG_Enable_Global_Trapping

#undef  Disable_Local_Trapping
#define Disable_Local_Trapping DEBUG_Disable_Local_Trapping

#undef  Disable_Global_Trapping
#define Disable_Global_Trapping DEBUG_Disable_Global_Trapping

#undef  Destroy_Semaphore
#define Destroy_Semaphore DEBUG_Destroy_Semaphore

#undef  _DeAssign_Device_V86_Pages
#define _DeAssign_Device_V86_Pages DEBUG__DeAssign_Device_V86_Pages

#undef  DeAssign_Device_V86_Pages
#define DeAssign_Device_V86_Pages DEBUG__DeAssign_Device_V86_Pages

#undef  Close_VM
#define Close_VM DEBUG_Close_VM

#undef  Convert_Boolean_String
#define Convert_Boolean_String DEBUG_Convert_Boolean_String

#undef  Convert_Decimal_String
#define Convert_Decimal_String DEBUG_Convert_Decimal_String

#undef  Convert_Fixed_Point_String
#define Convert_Fixed_Point_String DEBUG_Convert_Fixed_Point_String

#undef  Convert_Hex_String
#define Convert_Hex_String DEBUG_Convert_Hex_String

#undef  _CopyPageTable
#define _CopyPageTable DEBUG__CopyPageTable

#undef  CopyPageTable
#define CopyPageTable DEBUG__CopyPageTable

#undef  Create_Semaphore
#define Create_Semaphore DEBUG_Create_Semaphore

#undef  _AddFreePhysPage
#define _AddFreePhysPage DEBUG__AddFreePhysPage

#undef  AddFreePhysPage
#define AddFreePhysPage DEBUG__AddFreePhysPage

#undef  _Add_Global_V86_Data_Area
#define _Add_Global_V86_Data_Area DEBUG__Add_Global_V86_Data_Area

#undef  Add_Global_V86_Data_Area
#define Add_Global_V86_Data_Area DEBUG__Add_Global_V86_Data_Area

#undef  _AddInstanceItem
#define _AddInstanceItem DEBUG__AddInstanceItem

#undef  AddInstanceItem
#define AddInstanceItem DEBUG__AddInstanceItem

#undef  Adjust_Exec_Priority
#define Adjust_Exec_Priority DEBUG_Adjust_Exec_Priority

#undef  Adjust_Execution_Time
#define Adjust_Execution_Time DEBUG_Adjust_Execution_Time

#undef  _Allocate_Device_CB_Area
#define _Allocate_Device_CB_Area DEBUG__Allocate_Device_CB_Area

#undef  Allocate_Device_CB_Area
#define Allocate_Device_CB_Area DEBUG__Allocate_Device_CB_Area

#undef  _Allocate_GDT_Selector
#define _Allocate_GDT_Selector DEBUG__Allocate_GDT_Selector

#undef  Allocate_GDT_Selector
#define Allocate_GDT_Selector DEBUG__Allocate_GDT_Selector

#undef  _Allocate_Global_V86_Data_Area
#define _Allocate_Global_V86_Data_Area DEBUG__Allocate_Global_V86_Data_Area

#undef  Allocate_Global_V86_Data_Area
#define Allocate_Global_V86_Data_Area DEBUG__Allocate_Global_V86_Data_Area

#undef  _Allocate_LDT_Selector
#define _Allocate_LDT_Selector DEBUG__Allocate_LDT_Selector

#undef  Allocate_LDT_Selector
#define Allocate_LDT_Selector DEBUG__Allocate_LDT_Selector

#undef  Allocate_PM_Call_Back
#define Allocate_PM_Call_Back DEBUG_Allocate_PM_Call_Back

#undef  _Allocate_Temp_V86_Data_Area
#define _Allocate_Temp_V86_Data_Area DEBUG__Allocate_Temp_V86_Data_Area

#undef  Allocate_Temp_V86_Data_Area
#define Allocate_Temp_V86_Data_Area DEBUG__Allocate_Temp_V86_Data_Area

#undef  Allocate_V86_Call_Back
#define Allocate_V86_Call_Back DEBUG_Allocate_V86_Call_Back

#undef  _Assign_Device_V86_Pages
#define _Assign_Device_V86_Pages DEBUG__Assign_Device_V86_Pages

#undef  Assign_Device_V86_Pages
#define Assign_Device_V86_Pages DEBUG__Assign_Device_V86_Pages

#undef  Begin_Critical_Section
#define Begin_Critical_Section DEBUG_Begin_Critical_Section

#undef  Begin_Reentrant_Execution
#define Begin_Reentrant_Execution DEBUG_Begin_Reentrant_Execution

#undef  _BuildDescriptorDWORDs
#define _BuildDescriptorDWORDs DEBUG__BuildDescriptorDWORDs

#undef  BuildDescriptorDWORDs
#define BuildDescriptorDWORDs DEBUG__BuildDescriptorDWORDs

#undef  Build_Int_Stack_Frame
#define Build_Int_Stack_Frame DEBUG_Build_Int_Stack_Frame

#undef  Call_Global_Event
#define Call_Global_Event DEBUG_Call_Global_Event

#undef  Call_Priority_VM_Event
#define Call_Priority_VM_Event DEBUG_Call_Priority_VM_Event

#undef  Call_VM_Event
#define Call_VM_Event DEBUG_Call_VM_Event

#undef  Call_When_Idle
#define Call_When_Idle DEBUG_Call_When_Idle

#undef  Call_When_Not_Critical
#define Call_When_Not_Critical DEBUG_Call_When_Not_Critical

#undef  Call_When_Task_Switched
#define Call_When_Task_Switched DEBUG_Call_When_Task_Switched

#undef  Call_When_VM_Ints_Enabled
#define Call_When_VM_Ints_Enabled DEBUG_Call_When_VM_Ints_Enabled

#undef  Call_When_VM_Returns
#define Call_When_VM_Returns DEBUG_Call_When_VM_Returns

#undef  Cancel_Global_Event
#define Cancel_Global_Event DEBUG_Cancel_Global_Event

#undef  Cancel_Priority_VM_Event
#define Cancel_Priority_VM_Event DEBUG_Cancel_Priority_VM_Event

#undef  Cancel_Time_Out
#define Cancel_Time_Out DEBUG_Cancel_Time_Out

#undef  Cancel_VM_Event
#define Cancel_VM_Event DEBUG_Cancel_VM_Event

#undef  Claim_Critical_Section
#define Claim_Critical_Section DEBUG_Claim_Critical_Section

#ifdef WIN40_OR_LATER
#undef  _RegOpenKey
#define _RegOpenKey DEBUG__RegOpenKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegOpenKey
#define RegOpenKey DEBUG__RegOpenKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCreateKey
#define _RegCreateKey DEBUG__RegCreateKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCreateKey
#define RegCreateKey DEBUG__RegCreateKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegDeleteKey
#define _RegDeleteKey DEBUG__RegDeleteKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegDeleteKey
#define RegDeleteKey DEBUG__RegDeleteKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegDeleteValue
#define _RegDeleteValue DEBUG__RegDeleteValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegDeleteValue
#define RegDeleteValue DEBUG__RegDeleteValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegEnumKey
#define _RegEnumKey DEBUG__RegEnumKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegEnumKey
#define RegEnumKey DEBUG__RegEnumKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegEnumValue
#define _RegEnumValue DEBUG__RegEnumValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegEnumValue
#define RegEnumValue DEBUG__RegEnumValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegFlushKey
#define _RegFlushKey DEBUG__RegFlushKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegFlushKey
#define RegFlushKey DEBUG__RegFlushKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCloseKey
#define _RegCloseKey DEBUG__RegCloseKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCloseKey
#define RegCloseKey DEBUG__RegCloseKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryValue
#define _RegQueryValue DEBUG__RegQueryValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryValue
#define RegQueryValue DEBUG__RegQueryValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryValueEx
#define _RegQueryValueEx DEBUG__RegQueryValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryValueEx
#define RegQueryValueEx DEBUG__RegQueryValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  _RegSetValue
#define _RegSetValue DEBUG__RegSetValue
#endif

#ifdef WIN40_OR_LATER
#undef  RegSetValue
#define RegSetValue DEBUG__RegSetValue
#endif

#ifdef WIN40_OR_LATER
#undef  _RegSetValueEx
#define _RegSetValueEx DEBUG__RegSetValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  RegSetValueEx
#define RegSetValueEx DEBUG__RegSetValueEx
#endif

#ifdef WIN40_OR_LATER
#undef  _EnterMutex
#define _EnterMutex DEBUG__EnterMutex
#endif

#ifdef WIN40_OR_LATER
#undef  EnterMutex
#define EnterMutex DEBUG__EnterMutex
#endif

#ifdef WIN40_OR_LATER
#undef  _GetMutexOwner
#define _GetMutexOwner DEBUG__GetMutexOwner
#endif

#ifdef WIN40_OR_LATER
#undef  GetMutexOwner
#define GetMutexOwner DEBUG__GetMutexOwner
#endif

#ifdef WIN40_OR_LATER
#undef  _LeaveMutex
#define _LeaveMutex DEBUG__LeaveMutex
#endif

#ifdef WIN40_OR_LATER
#undef  LeaveMutex
#define LeaveMutex DEBUG__LeaveMutex
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Fault
#define Unhook_V86_Fault DEBUG_Unhook_V86_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_PM_Fault
#define Unhook_PM_Fault DEBUG_Unhook_PM_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_VMM_Fault
#define Unhook_VMM_Fault DEBUG_Unhook_VMM_Fault
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Int_Chain
#define Unhook_V86_Int_Chain DEBUG_Unhook_V86_Int_Chain
#endif

#ifdef WIN40_OR_LATER
#undef  Signal_Semaphore_No_Switch
#define Signal_Semaphore_No_Switch DEBUG_Signal_Semaphore_No_Switch
#endif

#ifdef WIN40_OR_LATER
#undef  Remove_IO_Handler
#define Remove_IO_Handler DEBUG_Remove_IO_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_NMI_Event
#define Unhook_NMI_Event DEBUG_Unhook_NMI_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Not_Critical
#define Cancel_Call_When_Not_Critical DEBUG_Cancel_Call_When_Not_Critical
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service
#define Unhook_Device_Service DEBUG_Unhook_Device_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Async_Time_Out
#define Set_Async_Time_Out DEBUG_Set_Async_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  Get_DDB
#define Get_DDB DEBUG_Get_DDB
#endif

#ifdef WIN40_OR_LATER
#undef  _Trace_Out_Service
#define _Trace_Out_Service DEBUG__Trace_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Trace_Out_Service
#define Trace_Out_Service DEBUG__Trace_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Out_Service
#define _Debug_Out_Service DEBUG__Debug_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Out_Service
#define Debug_Out_Service DEBUG__Debug_Out_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _AllocateThreadDataSlot
#define _AllocateThreadDataSlot DEBUG__AllocateThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  AllocateThreadDataSlot
#define AllocateThreadDataSlot DEBUG__AllocateThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  _FreeThreadDataSlot
#define _FreeThreadDataSlot DEBUG__FreeThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  FreeThreadDataSlot
#define FreeThreadDataSlot DEBUG__FreeThreadDataSlot
#endif

#ifdef WIN40_OR_LATER
#undef  _GetLastUpdatedThreadExecTime
#define _GetLastUpdatedThreadExecTime DEBUG__GetLastUpdatedThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  GetLastUpdatedThreadExecTime
#define GetLastUpdatedThreadExecTime DEBUG__GetLastUpdatedThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  VMMAddImportModuleName
#define VMMAddImportModuleName DEBUG_VMMAddImportModuleName
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_Add_DDB
#define VMM_Add_DDB DEBUG_VMM_Add_DDB
#endif

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
#undef  VMM_Remove_DDB
#define VMM_Remove_DDB DEBUG_VMM_Remove_DDB
#endif

#ifdef WIN40_OR_LATER
#undef  Test_VM_Ints_Enabled
#define Test_VM_Ints_Enabled DEBUG_Test_VM_Ints_Enabled
#endif

#ifdef WIN40_OR_LATER
#undef  _BlockOnID
#define _BlockOnID DEBUG__BlockOnID
#endif

#ifdef WIN40_OR_LATER
#undef  BlockOnID
#define BlockOnID DEBUG__BlockOnID
#endif

#ifdef WIN40_OR_LATER
#undef  Schedule_Thread_Event
#define Schedule_Thread_Event DEBUG_Schedule_Thread_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Thread_Event
#define Cancel_Thread_Event DEBUG_Cancel_Thread_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Time_Out
#define Set_Thread_Time_Out DEBUG_Set_Thread_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  _CreateMutex
#define _CreateMutex DEBUG__CreateMutex
#endif

#ifdef WIN40_OR_LATER
#undef  CreateMutex
#define CreateMutex DEBUG__CreateMutex
#endif

#ifdef WIN40_OR_LATER
#undef  _DestroyMutex
#define _DestroyMutex DEBUG__DestroyMutex
#endif

#ifdef WIN40_OR_LATER
#undef  DestroyMutex
#define DestroyMutex DEBUG__DestroyMutex
#endif

#ifdef WIN40_OR_LATER
#undef  Call_When_Thread_Switched
#define Call_When_Thread_Switched DEBUG_Call_When_Thread_Switched
#endif

#ifdef WIN40_OR_LATER
#undef  VMMCreateThread
#define VMMCreateThread DEBUG_VMMCreateThread
#endif

#ifdef WIN40_OR_LATER
#undef  _GetThreadExecTime
#define _GetThreadExecTime DEBUG__GetThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  GetThreadExecTime
#define GetThreadExecTime DEBUG__GetThreadExecTime
#endif

#ifdef WIN40_OR_LATER
#undef  VMMTerminateThread
#define VMMTerminateThread DEBUG_VMMTerminateThread
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Cur_Thread_Handle
#define Get_Cur_Thread_Handle DEBUG_Get_Cur_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Cur_Thread_Handle
#define Test_Cur_Thread_Handle DEBUG_Test_Cur_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Sys_Thread_Handle
#define Get_Sys_Thread_Handle DEBUG_Get_Sys_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Sys_Thread_Handle
#define Test_Sys_Thread_Handle DEBUG_Test_Sys_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Validate_Thread_Handle
#define Validate_Thread_Handle DEBUG_Validate_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Initial_Thread_Handle
#define Get_Initial_Thread_Handle DEBUG_Get_Initial_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Test_Initial_Thread_Handle
#define Test_Initial_Thread_Handle DEBUG_Test_Initial_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Valid_Thread_Handle
#define Debug_Test_Valid_Thread_Handle DEBUG_Debug_Test_Valid_Thread_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Cur_Thread
#define Debug_Test_Cur_Thread DEBUG_Debug_Test_Cur_Thread
#endif

#undef  VMM_GetSystemInitState
#define VMM_GetSystemInitState DEBUG_VMM_GetSystemInitState

#undef  Cancel_Call_When_Thread_Switched
#define Cancel_Call_When_Thread_Switched DEBUG_Cancel_Call_When_Thread_Switched

#undef  Get_Next_Thread_Handle
#define Get_Next_Thread_Handle DEBUG_Get_Next_Thread_Handle

#ifdef WIN40_OR_LATER
#undef  Adjust_Thread_Exec_Priority
#define Adjust_Thread_Exec_Priority DEBUG_Adjust_Thread_Exec_Priority
#endif

#ifdef WIN40_OR_LATER
#undef  _Deallocate_Device_CB_Area
#define _Deallocate_Device_CB_Area DEBUG__Deallocate_Device_CB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  Deallocate_Device_CB_Area
#define Deallocate_Device_CB_Area DEBUG__Deallocate_Device_CB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  Remove_Mult_IO_Handlers
#define Remove_Mult_IO_Handlers DEBUG_Remove_Mult_IO_Handlers
#endif

#ifdef WIN40_OR_LATER
#undef  _PageReserve
#define _PageReserve DEBUG__PageReserve
#endif

#ifdef WIN40_OR_LATER
#undef  PageReserve
#define PageReserve DEBUG__PageReserve
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommit
#define _PageCommit DEBUG__PageCommit
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommit
#define PageCommit DEBUG__PageCommit
#endif

#ifdef WIN40_OR_LATER
#undef  _PageDecommit
#define _PageDecommit DEBUG__PageDecommit
#endif

#ifdef WIN40_OR_LATER
#undef  PageDecommit
#define PageDecommit DEBUG__PageDecommit
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerRegister
#define _PagerRegister DEBUG__PagerRegister
#endif

#ifdef WIN40_OR_LATER
#undef  PagerRegister
#define PagerRegister DEBUG__PagerRegister
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerQuery
#define _PagerQuery DEBUG__PagerQuery
#endif

#ifdef WIN40_OR_LATER
#undef  PagerQuery
#define PagerQuery DEBUG__PagerQuery
#endif

#ifdef WIN40_OR_LATER
#undef  _PagerDeregister
#define _PagerDeregister DEBUG__PagerDeregister
#endif

#ifdef WIN40_OR_LATER
#undef  PagerDeregister
#define PagerDeregister DEBUG__PagerDeregister
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextCreate
#define _ContextCreate DEBUG__ContextCreate
#endif

#ifdef WIN40_OR_LATER
#undef  ContextCreate
#define ContextCreate DEBUG__ContextCreate
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextDestroy
#define _ContextDestroy DEBUG__ContextDestroy
#endif

#ifdef WIN40_OR_LATER
#undef  ContextDestroy
#define ContextDestroy DEBUG__ContextDestroy
#endif

#ifdef WIN40_OR_LATER
#undef  _PageAttach
#define _PageAttach DEBUG__PageAttach
#endif

#ifdef WIN40_OR_LATER
#undef  PageAttach
#define PageAttach DEBUG__PageAttach
#endif

#ifdef WIN40_OR_LATER
#undef  _PageFlush
#define _PageFlush DEBUG__PageFlush
#endif

#ifdef WIN40_OR_LATER
#undef  PageFlush
#define PageFlush DEBUG__PageFlush
#endif

#ifdef WIN40_OR_LATER
#undef  _SignalID
#define _SignalID DEBUG__SignalID
#endif

#ifdef WIN40_OR_LATER
#undef  SignalID
#define SignalID DEBUG__SignalID
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommitPhys
#define _PageCommitPhys DEBUG__PageCommitPhys
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommitPhys
#define PageCommitPhys DEBUG__PageCommitPhys
#endif

#ifdef WIN40_OR_LATER
#undef  _Register_Win32_Services
#define _Register_Win32_Services DEBUG__Register_Win32_Services
#endif

#ifdef WIN40_OR_LATER
#undef  Register_Win32_Services
#define Register_Win32_Services DEBUG__Register_Win32_Services
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Idle
#define Cancel_Call_When_Idle DEBUG_Cancel_Call_When_Idle
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Call_When_Task_Switched
#define Cancel_Call_When_Task_Switched DEBUG_Cancel_Call_When_Task_Switched
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Inst_V86_Int_Vec_Base
#define Get_Inst_V86_Int_Vec_Base DEBUG_Get_Inst_V86_Int_Vec_Base
#endif

#ifdef WIN40_OR_LATER
#undef  Simulate_VM_IO
#define Simulate_VM_IO DEBUG_Simulate_VM_IO
#endif

#ifdef WIN40_OR_LATER
#undef  _ContextSwitch
#define _ContextSwitch DEBUG__ContextSwitch
#endif

#ifdef WIN40_OR_LATER
#undef  ContextSwitch
#define ContextSwitch DEBUG__ContextSwitch
#endif

#ifdef WIN40_OR_LATER
#undef  _PageModifyPermissions
#define _PageModifyPermissions DEBUG__PageModifyPermissions
#endif

#ifdef WIN40_OR_LATER
#undef  PageModifyPermissions
#define PageModifyPermissions DEBUG__PageModifyPermissions
#endif

#ifdef WIN40_OR_LATER
#undef  _PageQuery
#define _PageQuery DEBUG__PageQuery
#endif

#ifdef WIN40_OR_LATER
#undef  PageQuery
#define PageQuery DEBUG__PageQuery
#endif

#ifdef WIN40_OR_LATER
#undef  _GetThreadTerminationStatus
#define _GetThreadTerminationStatus DEBUG__GetThreadTerminationStatus
#endif

#ifdef WIN40_OR_LATER
#undef  GetThreadTerminationStatus
#define GetThreadTerminationStatus DEBUG__GetThreadTerminationStatus
#endif

#ifdef WIN40_OR_LATER
#undef  _GetInstanceInfo
#define _GetInstanceInfo DEBUG__GetInstanceInfo
#endif

#ifdef WIN40_OR_LATER
#undef  GetInstanceInfo
#define GetInstanceInfo DEBUG__GetInstanceInfo
#endif

#ifdef WIN40_OR_LATER
#undef  _ExecIntMustComplete
#define _ExecIntMustComplete DEBUG__ExecIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ExecIntMustComplete
#define ExecIntMustComplete DEBUG__ExecIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _ExecVxDIntMustComplete
#define _ExecVxDIntMustComplete DEBUG__ExecVxDIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ExecVxDIntMustComplete
#define ExecVxDIntMustComplete DEBUG__ExecVxDIntMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  Begin_V86_Serialization
#define Begin_V86_Serialization DEBUG_Begin_V86_Serialization
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_V86_Page
#define Unhook_V86_Page DEBUG_Unhook_V86_Page
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_GetVxDLocationList
#define VMM_GetVxDLocationList DEBUG_VMM_GetVxDLocationList
#endif

#ifdef WIN40_OR_LATER
#undef  VMM_GetDDBList
#define VMM_GetDDBList DEBUG_VMM_GetDDBList
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Instanced_V86_Int_Vector
#define Get_Instanced_V86_Int_Vector DEBUG_Get_Instanced_V86_Int_Vector
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Set_Real_DOS_PSP
#define Get_Set_Real_DOS_PSP DEBUG_Get_Set_Real_DOS_PSP
#endif

#ifdef WIN40_OR_LATER
#undef  Get_System_Time_Address
#define Get_System_Time_Address DEBUG_Get_System_Time_Address
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Crit_Status_Thread
#define Get_Crit_Status_Thread DEBUG_Get_Crit_Status_Thread
#endif

#ifdef WIN40_OR_LATER
#undef  Directed_Sys_Control
#define Directed_Sys_Control DEBUG_Directed_Sys_Control
#endif

#ifdef WIN40_OR_LATER
#undef  _CallRing3
#define _CallRing3 DEBUG__CallRing3
#endif

#ifdef WIN40_OR_LATER
#undef  CallRing3
#define CallRing3 DEBUG__CallRing3
#endif

#ifdef WIN40_OR_LATER
#undef  Exec_PM_Int
#define Exec_PM_Int DEBUG_Exec_PM_Int
#endif

#ifdef WIN40_OR_LATER
#undef  _PageCommitContig
#define _PageCommitContig DEBUG__PageCommitContig
#endif

#ifdef WIN40_OR_LATER
#undef  PageCommitContig
#define PageCommitContig DEBUG__PageCommitContig
#endif

#ifdef WIN40_OR_LATER
#undef  _GetCurrentContext
#define _GetCurrentContext DEBUG__GetCurrentContext
#endif

#ifdef WIN40_OR_LATER
#undef  GetCurrentContext
#define GetCurrentContext DEBUG__GetCurrentContext
#endif

#ifdef WIN40_OR_LATER
#undef  Call_Restricted_Event
#define Call_Restricted_Event DEBUG_Call_Restricted_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Cancel_Restricted_Event
#define Cancel_Restricted_Event DEBUG_Cancel_Restricted_Event
#endif

#ifdef WIN40_OR_LATER
#undef  Register_PEF_Provider
#define Register_PEF_Provider DEBUG_Register_PEF_Provider
#endif

#ifdef WIN40_OR_LATER
#undef  _GetPhysPageInfo
#define _GetPhysPageInfo DEBUG__GetPhysPageInfo
#endif

#ifdef WIN40_OR_LATER
#undef  GetPhysPageInfo
#define GetPhysPageInfo DEBUG__GetPhysPageInfo
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryInfoKey
#define _RegQueryInfoKey DEBUG__RegQueryInfoKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryInfoKey
#define RegQueryInfoKey DEBUG__RegQueryInfoKey
#endif

#ifdef WIN40_OR_LATER
#undef  Time_Slice_Sleep
#define Time_Slice_Sleep DEBUG_Time_Slice_Sleep
#endif

#ifdef WIN40_OR_LATER
#undef  Boost_With_Decay
#define Boost_With_Decay DEBUG_Boost_With_Decay
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Inversion_Pri
#define Set_Inversion_Pri DEBUG_Set_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Reset_Inversion_Pri
#define Reset_Inversion_Pri DEBUG_Reset_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Release_Inversion_Pri
#define Release_Inversion_Pri DEBUG_Release_Inversion_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Thread_Win32_Pri
#define Get_Thread_Win32_Pri DEBUG_Get_Thread_Win32_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Win32_Pri
#define Set_Thread_Win32_Pri DEBUG_Set_Thread_Win32_Pri
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Thread_Static_Boost
#define Set_Thread_Static_Boost DEBUG_Set_Thread_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  Set_VM_Static_Boost
#define Set_VM_Static_Boost DEBUG_Set_VM_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  Release_Inversion_Pri_ID
#define Release_Inversion_Pri_ID DEBUG_Release_Inversion_Pri_ID
#endif

#ifdef WIN40_OR_LATER
#undef  Attach_Thread_To_Group
#define Attach_Thread_To_Group DEBUG_Attach_Thread_To_Group
#endif

#ifdef WIN40_OR_LATER
#undef  Detach_Thread_From_Group
#define Detach_Thread_From_Group DEBUG_Detach_Thread_From_Group
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Group_Static_Boost
#define Set_Group_Static_Boost DEBUG_Set_Group_Static_Boost
#endif

#ifdef WIN40_OR_LATER
#undef  _GetRegistryPath
#define _GetRegistryPath DEBUG__GetRegistryPath
#endif

#ifdef WIN40_OR_LATER
#undef  GetRegistryPath
#define GetRegistryPath DEBUG__GetRegistryPath
#endif

#ifdef WIN40_OR_LATER
#undef  _RegRemapPreDefKey
#define _RegRemapPreDefKey DEBUG__RegRemapPreDefKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegRemapPreDefKey
#define RegRemapPreDefKey DEBUG__RegRemapPreDefKey
#endif

#ifdef WIN40_OR_LATER
#undef  End_V86_Serialization
#define End_V86_Serialization DEBUG_End_V86_Serialization
#endif

#ifdef WIN40_OR_LATER
#undef  _Assert_Range
#define _Assert_Range DEBUG__Assert_Range
#endif

#ifdef WIN40_OR_LATER
#undef  Assert_Range
#define Assert_Range DEBUG__Assert_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _PageChangePager
#define _PageChangePager DEBUG__PageChangePager
#endif

#ifdef WIN40_OR_LATER
#undef  PageChangePager
#define PageChangePager DEBUG__PageChangePager
#endif

#ifdef WIN40_OR_LATER
#undef  _RegCreateDynKey
#define _RegCreateDynKey DEBUG__RegCreateDynKey
#endif

#ifdef WIN40_OR_LATER
#undef  RegCreateDynKey
#define RegCreateDynKey DEBUG__RegCreateDynKey
#endif

#ifdef WIN40_OR_LATER
#undef  _RegQueryMultipleValues
#define _RegQueryMultipleValues DEBUG__RegQueryMultipleValues
#endif

#ifdef WIN40_OR_LATER
#undef  RegQueryMultipleValues
#define RegQueryMultipleValues DEBUG__RegQueryMultipleValues
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Boot_Flags
#define Get_Boot_Flags DEBUG_Get_Boot_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  Set_Boot_Flags
#define Set_Boot_Flags DEBUG_Set_Boot_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrcpyn
#define _lstrcpyn DEBUG__lstrcpyn
#endif

#ifdef WIN40_OR_LATER
#undef  lstrcpyn
#define lstrcpyn DEBUG__lstrcpyn
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrlen
#define _lstrlen DEBUG__lstrlen
#endif

#ifdef WIN40_OR_LATER
#undef  lstrlen
#define lstrlen DEBUG__lstrlen
#endif

#ifdef WIN40_OR_LATER
#undef  _lmemcpy
#define _lmemcpy DEBUG__lmemcpy
#endif

#ifdef WIN40_OR_LATER
#undef  lmemcpy
#define lmemcpy DEBUG__lmemcpy
#endif

#ifdef WIN40_OR_LATER
#undef  _lstrcmpi
#define _lstrcmpi DEBUG__lstrcmpi
#endif

#ifdef WIN40_OR_LATER
#undef  lstrcmpi
#define lstrcmpi DEBUG__lstrcmpi
#endif

#ifdef WIN40_OR_LATER
#undef  _GetVxDName
#define _GetVxDName DEBUG__GetVxDName
#endif

#ifdef WIN40_OR_LATER
#undef  GetVxDName
#define GetVxDName DEBUG__GetVxDName
#endif

#undef  BlockDev_Command_Complete
#define BlockDev_Command_Complete DEBUG_BlockDev_Command_Complete

#undef  BlockDev_Find_Int13_Drive
#define BlockDev_Find_Int13_Drive DEBUG_BlockDev_Find_Int13_Drive

#undef  BlockDev_Get_Device_List
#define BlockDev_Get_Device_List DEBUG_BlockDev_Get_Device_List

#undef  BlockDev_Get_Version
#define BlockDev_Get_Version DEBUG_BlockDev_Get_Version

#undef  BlockDev_Register_Device
#define BlockDev_Register_Device DEBUG_BlockDev_Register_Device

#undef  BlockDev_Send_Command
#define BlockDev_Send_Command DEBUG_BlockDev_Send_Command

#undef  BlockDev_Synchronous_Command
#define BlockDev_Synchronous_Command DEBUG_BlockDev_Synchronous_Command

#undef  BlockDev_Thunk_Sync_Cmd
#define BlockDev_Thunk_Sync_Cmd DEBUG_BlockDev_Thunk_Sync_Cmd

#undef  BlockDev_Thunk_Command
#define BlockDev_Thunk_Command DEBUG_BlockDev_Thunk_Command

#undef  BlockDev_Thunk_HW_Int
#define BlockDev_Thunk_HW_Int DEBUG_BlockDev_Thunk_HW_Int

#undef  DOSMGR_Add_Device
#define DOSMGR_Add_Device DEBUG_DOSMGR_Add_Device

#undef  DOSMGR_BackFill_Allowed
#define DOSMGR_BackFill_Allowed DEBUG_DOSMGR_BackFill_Allowed

#undef  DOSMGR_Copy_VM_Drive_State
#define DOSMGR_Copy_VM_Drive_State DEBUG_DOSMGR_Copy_VM_Drive_State

#undef  DOSMGR_Enable_Indos_Polling
#define DOSMGR_Enable_Indos_Polling DEBUG_DOSMGR_Enable_Indos_Polling

#undef  _DOSMGR_Exec_VM
#define _DOSMGR_Exec_VM DEBUG__DOSMGR_Exec_VM

#undef  DOSMGR_Exec_VM
#define DOSMGR_Exec_VM DEBUG__DOSMGR_Exec_VM

#undef  DOSMGR_Get_DOS_Crit_Status
#define DOSMGR_Get_DOS_Crit_Status DEBUG_DOSMGR_Get_DOS_Crit_Status

#undef  DOSMGR_Get_IndosPtr
#define DOSMGR_Get_IndosPtr DEBUG_DOSMGR_Get_IndosPtr

#undef  DOSMGR_Get_Version
#define DOSMGR_Get_Version DEBUG_DOSMGR_Get_Version

#undef  DOSMGR_Instance_Device
#define DOSMGR_Instance_Device DEBUG_DOSMGR_Instance_Device

#undef  DOSMGR_LocalGlobalReg
#define DOSMGR_LocalGlobalReg DEBUG_DOSMGR_LocalGlobalReg

#undef  DOSMGR_Remove_Device
#define DOSMGR_Remove_Device DEBUG_DOSMGR_Remove_Device

#undef  _DOSMGR_Set_Exec_VM_Data
#define _DOSMGR_Set_Exec_VM_Data DEBUG__DOSMGR_Set_Exec_VM_Data

#undef  DOSMGR_Set_Exec_VM_Data
#define DOSMGR_Set_Exec_VM_Data DEBUG__DOSMGR_Set_Exec_VM_Data

#undef  DOSNET_Do_PSP_Adjust
#define DOSNET_Do_PSP_Adjust DEBUG_DOSNET_Do_PSP_Adjust

#undef  DOSNET_Get_Version
#define DOSNET_Get_Version DEBUG_DOSNET_Get_Version

#undef  DOSNET_Send_FILESYSCHANGE
#define DOSNET_Send_FILESYSCHANGE DEBUG_DOSNET_Send_FILESYSCHANGE

#undef  EBIOS_Get_Unused_Mem
#define EBIOS_Get_Unused_Mem DEBUG_EBIOS_Get_Unused_Mem

#undef  EBIOS_Get_Version
#define EBIOS_Get_Version DEBUG_EBIOS_Get_Version

#undef  Int13_Device_Registered
#define Int13_Device_Registered DEBUG_Int13_Device_Registered

#undef  Int13_Hooking_BIOS_Int
#define Int13_Hooking_BIOS_Int DEBUG_Int13_Hooking_BIOS_Int

#undef  Int13_Translate_VM_Int
#define Int13_Translate_VM_Int DEBUG_Int13_Translate_VM_Int

#undef  Int13_Unhooking_BIOS_Int
#define Int13_Unhooking_BIOS_Int DEBUG_Int13_Unhooking_BIOS_Int

#undef  VMD_Get_Mouse_Owner
#define VMD_Get_Mouse_Owner DEBUG_VMD_Get_Mouse_Owner

#undef  VMD_Get_Version
#define VMD_Get_Version DEBUG_VMD_Get_Version

#undef  VMD_Set_Mouse_Type
#define VMD_Set_Mouse_Type DEBUG_VMD_Set_Mouse_Type

#undef  SHELL_Event
#define SHELL_Event DEBUG_SHELL_Event

#undef  SHELL_Get_Version
#define SHELL_Get_Version DEBUG_SHELL_Get_Version

#undef  SHELL_GetVMInfo
#define SHELL_GetVMInfo DEBUG_SHELL_GetVMInfo

#undef  SHELL_Message
#define SHELL_Message DEBUG_SHELL_Message

#undef  SHELL_Resolve_Contention
#define SHELL_Resolve_Contention DEBUG_SHELL_Resolve_Contention

#undef  SHELL_SYSMODAL_Message
#define SHELL_SYSMODAL_Message DEBUG_SHELL_SYSMODAL_Message

#ifdef WIN40_OR_LATER
#undef  _SHELL_CallAtAppyTime
#define _SHELL_CallAtAppyTime DEBUG__SHELL_CallAtAppyTime
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CallAtAppyTime
#define SHELL_CallAtAppyTime DEBUG__SHELL_CallAtAppyTime
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_CancelAppyTimeEvent
#define _SHELL_CancelAppyTimeEvent DEBUG__SHELL_CancelAppyTimeEvent
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CancelAppyTimeEvent
#define SHELL_CancelAppyTimeEvent DEBUG__SHELL_CancelAppyTimeEvent
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_QueryAppyTimeAvailable
#define _SHELL_QueryAppyTimeAvailable DEBUG__SHELL_QueryAppyTimeAvailable
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_QueryAppyTimeAvailable
#define SHELL_QueryAppyTimeAvailable DEBUG__SHELL_QueryAppyTimeAvailable
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LocalAllocEx
#define _SHELL_LocalAllocEx DEBUG__SHELL_LocalAllocEx
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LocalAllocEx
#define SHELL_LocalAllocEx DEBUG__SHELL_LocalAllocEx
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LocalFree
#define _SHELL_LocalFree DEBUG__SHELL_LocalFree
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LocalFree
#define SHELL_LocalFree DEBUG__SHELL_LocalFree
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_CallDll
#define _SHELL_CallDll DEBUG__SHELL_CallDll
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_CallDll
#define SHELL_CallDll DEBUG__SHELL_CallDll
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_BroadcastSystemMessage
#define _SHELL_BroadcastSystemMessage DEBUG__SHELL_BroadcastSystemMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_BroadcastSystemMessage
#define SHELL_BroadcastSystemMessage DEBUG__SHELL_BroadcastSystemMessage
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_HookSystemBroadcast
#define _SHELL_HookSystemBroadcast DEBUG__SHELL_HookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_HookSystemBroadcast
#define SHELL_HookSystemBroadcast DEBUG__SHELL_HookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_UnhookSystemBroadcast
#define _SHELL_UnhookSystemBroadcast DEBUG__SHELL_UnhookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_UnhookSystemBroadcast
#define SHELL_UnhookSystemBroadcast DEBUG__SHELL_UnhookSystemBroadcast
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_FreeLibrary
#define _SHELL_FreeLibrary DEBUG__SHELL_FreeLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_FreeLibrary
#define SHELL_FreeLibrary DEBUG__SHELL_FreeLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_GetProcAddress
#define _SHELL_GetProcAddress DEBUG__SHELL_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_GetProcAddress
#define SHELL_GetProcAddress DEBUG__SHELL_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_LoadLibrary
#define _SHELL_LoadLibrary DEBUG__SHELL_LoadLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_LoadLibrary
#define SHELL_LoadLibrary DEBUG__SHELL_LoadLibrary
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_PostMessage
#define _SHELL_PostMessage DEBUG__SHELL_PostMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_PostMessage
#define SHELL_PostMessage DEBUG__SHELL_PostMessage
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_PostShellMessage
#define _SHELL_PostShellMessage DEBUG__SHELL_PostShellMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_PostShellMessage
#define SHELL_PostShellMessage DEBUG__SHELL_PostShellMessage
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Hook_Properties
#define SHELL_Hook_Properties DEBUG_SHELL_Hook_Properties
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Unhook_Properties
#define SHELL_Unhook_Properties DEBUG_SHELL_Unhook_Properties
#endif

#ifdef WIN40_OR_LATER
#undef  _SHELL_ShellExecute
#define _SHELL_ShellExecute DEBUG__SHELL_ShellExecute
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_ShellExecute
#define SHELL_ShellExecute DEBUG__SHELL_ShellExecute
#endif

#undef  V86MMGR_Allocate_Buffer
#define V86MMGR_Allocate_Buffer DEBUG_V86MMGR_Allocate_Buffer

#undef  V86MMGR_Allocate_V86_Pages
#define V86MMGR_Allocate_V86_Pages DEBUG_V86MMGR_Allocate_V86_Pages

#undef  V86MMGR_Free_Buffer
#define V86MMGR_Free_Buffer DEBUG_V86MMGR_Free_Buffer

#undef  V86MMGR_Free_Page_Map_Region
#define V86MMGR_Free_Page_Map_Region DEBUG_V86MMGR_Free_Page_Map_Region

#undef  V86MMGR_Get_EMS_XMS_Limits
#define V86MMGR_Get_EMS_XMS_Limits DEBUG_V86MMGR_Get_EMS_XMS_Limits

#undef  V86MMGR_Get_Mapping_Info
#define V86MMGR_Get_Mapping_Info DEBUG_V86MMGR_Get_Mapping_Info

#undef  V86MMGR_Get_Version
#define V86MMGR_Get_Version DEBUG_V86MMGR_Get_Version

#undef  V86MMGR_Get_VM_Flat_Sel
#define V86MMGR_Get_VM_Flat_Sel DEBUG_V86MMGR_Get_VM_Flat_Sel

#undef  V86MMGR_Get_Xlat_Buff_State
#define V86MMGR_Get_Xlat_Buff_State DEBUG_V86MMGR_Get_Xlat_Buff_State

#undef  V86MMGR_GetPgStatus
#define V86MMGR_GetPgStatus DEBUG_V86MMGR_GetPgStatus

#undef  V86MMGR_Load_Client_Ptr
#define V86MMGR_Load_Client_Ptr DEBUG_V86MMGR_Load_Client_Ptr

#undef  V86MMGR_LocalGlobalReg
#define V86MMGR_LocalGlobalReg DEBUG_V86MMGR_LocalGlobalReg

#undef  V86MMGR_Map_Pages
#define V86MMGR_Map_Pages DEBUG_V86MMGR_Map_Pages

#undef  V86MMGR_ResetBasePages
#define V86MMGR_ResetBasePages DEBUG_V86MMGR_ResetBasePages

#undef  V86MMGR_Set_EMS_XMS_Limits
#define V86MMGR_Set_EMS_XMS_Limits DEBUG_V86MMGR_Set_EMS_XMS_Limits

#undef  V86MMGR_Set_Mapping_Info
#define V86MMGR_Set_Mapping_Info DEBUG_V86MMGR_Set_Mapping_Info

#undef  V86MMGR_Set_Xlat_Buff_State
#define V86MMGR_Set_Xlat_Buff_State DEBUG_V86MMGR_Set_Xlat_Buff_State

#undef  V86MMGR_SetAvailMapPgs
#define V86MMGR_SetAvailMapPgs DEBUG_V86MMGR_SetAvailMapPgs

#undef  V86MMGR_Xlat_API
#define V86MMGR_Xlat_API DEBUG_V86MMGR_Xlat_API

#undef  VCD_Get_Focus
#define VCD_Get_Focus DEBUG_VCD_Get_Focus

#undef  VCD_Get_Version
#define VCD_Get_Version DEBUG_VCD_Get_Version

#undef  VCD_Set_Port_Global
#define VCD_Set_Port_Global DEBUG_VCD_Set_Port_Global

#undef  VCD_Virtualize_Port
#define VCD_Virtualize_Port DEBUG_VCD_Virtualize_Port

#ifdef WIN40_OR_LATER
#undef  VMCPD_Set_CR0_State
#define VMCPD_Set_CR0_State DEBUG_VMCPD_Set_CR0_State
#endif

#ifdef WIN40_OR_LATER
#undef  VMCPD_Get_CR0_State
#define VMCPD_Get_CR0_State DEBUG_VMCPD_Get_CR0_State
#endif

#undef  VMCPD_Get_Thread_State
#define VMCPD_Get_Thread_State DEBUG_VMCPD_Get_Thread_State

#ifdef WIN40_OR_LATER
#undef  VMCPD_Set_Thread_State
#define VMCPD_Set_Thread_State DEBUG_VMCPD_Set_Thread_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Virtual_EOI
#define VPICD_Virtual_EOI DEBUG_VPICD_Virtual_EOI
#endif

#ifdef WIN40_OR_LATER
#undef  VTD_Get_Date_And_Time
#define VTD_Get_Date_And_Time DEBUG_VTD_Get_Date_And_Time
#endif

#undef  VDD_Get_ModTime
#define VDD_Get_ModTime DEBUG_VDD_Get_ModTime

#undef  VDD_Get_Version
#define VDD_Get_Version DEBUG_VDD_Get_Version

#undef  VDD_Msg_BakColor
#define VDD_Msg_BakColor DEBUG_VDD_Msg_BakColor

#undef  VDD_Msg_ClrScrn
#define VDD_Msg_ClrScrn DEBUG_VDD_Msg_ClrScrn

#undef  VDD_Msg_ForColor
#define VDD_Msg_ForColor DEBUG_VDD_Msg_ForColor

#undef  VDD_SetCursPos
#define VDD_SetCursPos DEBUG_VDD_SetCursPos

#undef  VDD_Msg_TextOut
#define VDD_Msg_TextOut DEBUG_VDD_Msg_TextOut

#undef  VDD_PIF_State
#define VDD_PIF_State DEBUG_VDD_PIF_State

#undef  VDD_Query_Access
#define VDD_Query_Access DEBUG_VDD_Query_Access

#undef  VDD_Set_HCurTrk
#define VDD_Set_HCurTrk DEBUG_VDD_Set_HCurTrk

#undef  VDD_Set_VMType
#define VDD_Set_VMType DEBUG_VDD_Set_VMType

#undef  VDMAD_Copy_From_Buffer
#define VDMAD_Copy_From_Buffer DEBUG_VDMAD_Copy_From_Buffer

#undef  VDMAD_Copy_To_Buffer
#define VDMAD_Copy_To_Buffer DEBUG_VDMAD_Copy_To_Buffer

#undef  VDMAD_Default_Handler
#define VDMAD_Default_Handler DEBUG_VDMAD_Default_Handler

#undef  VDMAD_Disable_Translation
#define VDMAD_Disable_Translation DEBUG_VDMAD_Disable_Translation

#undef  VDMAD_Enable_Translation
#define VDMAD_Enable_Translation DEBUG_VDMAD_Enable_Translation

#undef  VDMAD_Get_EISA_Adr_Mode
#define VDMAD_Get_EISA_Adr_Mode DEBUG_VDMAD_Get_EISA_Adr_Mode

#undef  VDMAD_Get_Region_Info
#define VDMAD_Get_Region_Info DEBUG_VDMAD_Get_Region_Info

#undef  VDMAD_Get_Version
#define VDMAD_Get_Version DEBUG_VDMAD_Get_Version

#undef  VDMAD_Get_Virt_State
#define VDMAD_Get_Virt_State DEBUG_VDMAD_Get_Virt_State

#undef  VDMAD_Lock_DMA_Region
#define VDMAD_Lock_DMA_Region DEBUG_VDMAD_Lock_DMA_Region

#undef  VDMAD_Mask_Channel
#define VDMAD_Mask_Channel DEBUG_VDMAD_Mask_Channel

#undef  VDMAD_Release_Buffer
#define VDMAD_Release_Buffer DEBUG_VDMAD_Release_Buffer

#undef  VDMAD_Request_Buffer
#define VDMAD_Request_Buffer DEBUG_VDMAD_Request_Buffer

#undef  VDMAD_Reserve_Buffer_Space
#define VDMAD_Reserve_Buffer_Space DEBUG_VDMAD_Reserve_Buffer_Space

#undef  VDMAD_Scatter_Lock
#define VDMAD_Scatter_Lock DEBUG_VDMAD_Scatter_Lock

#undef  VDMAD_Scatter_Unlock
#define VDMAD_Scatter_Unlock DEBUG_VDMAD_Scatter_Unlock

#undef  VDMAD_Set_EISA_Adr_Mode
#define VDMAD_Set_EISA_Adr_Mode DEBUG_VDMAD_Set_EISA_Adr_Mode

#undef  VDMAD_Set_Phys_State
#define VDMAD_Set_Phys_State DEBUG_VDMAD_Set_Phys_State

#undef  VDMAD_Set_Region_Info
#define VDMAD_Set_Region_Info DEBUG_VDMAD_Set_Region_Info

#undef  VDMAD_Set_Virt_State
#define VDMAD_Set_Virt_State DEBUG_VDMAD_Set_Virt_State

#undef  VDMAD_Unlock_DMA_Region
#define VDMAD_Unlock_DMA_Region DEBUG_VDMAD_Unlock_DMA_Region

#undef  VDMAD_UnMask_Channel
#define VDMAD_UnMask_Channel DEBUG_VDMAD_UnMask_Channel

#undef  VDMAD_Virtualize_Channel
#define VDMAD_Virtualize_Channel DEBUG_VDMAD_Virtualize_Channel

#undef  VKD_Cancel_Paste
#define VKD_Cancel_Paste DEBUG_VKD_Cancel_Paste

#undef  VKD_Define_Hot_Key
#define VKD_Define_Hot_Key DEBUG_VKD_Define_Hot_Key

#undef  VKD_Define_Paste_Mode
#define VKD_Define_Paste_Mode DEBUG_VKD_Define_Paste_Mode

#undef  VKD_Flush_Msg_Key_Queue
#define VKD_Flush_Msg_Key_Queue DEBUG_VKD_Flush_Msg_Key_Queue

#undef  VKD_Force_Keys
#define VKD_Force_Keys DEBUG_VKD_Force_Keys

#undef  VKD_Get_Kbd_Owner
#define VKD_Get_Kbd_Owner DEBUG_VKD_Get_Kbd_Owner

#undef  VKD_Get_Msg_Key
#define VKD_Get_Msg_Key DEBUG_VKD_Get_Msg_Key

#undef  VKD_Get_Version
#define VKD_Get_Version DEBUG_VKD_Get_Version

#undef  VKD_Local_Disable_Hot_Key
#define VKD_Local_Disable_Hot_Key DEBUG_VKD_Local_Disable_Hot_Key

#undef  VKD_Local_Enable_Hot_Key
#define VKD_Local_Enable_Hot_Key DEBUG_VKD_Local_Enable_Hot_Key

#undef  VKD_Peek_Msg_Key
#define VKD_Peek_Msg_Key DEBUG_VKD_Peek_Msg_Key

#undef  VKD_Reflect_Hot_Key
#define VKD_Reflect_Hot_Key DEBUG_VKD_Reflect_Hot_Key

#undef  VKD_Remove_Hot_Key
#define VKD_Remove_Hot_Key DEBUG_VKD_Remove_Hot_Key

#undef  VKD_Start_Paste
#define VKD_Start_Paste DEBUG_VKD_Start_Paste

#undef  VMCPD_Get_Version
#define VMCPD_Get_Version DEBUG_VMCPD_Get_Version

#undef  VMCPD_Get_Virt_State
#define VMCPD_Get_Virt_State DEBUG_VMCPD_Get_Virt_State

#undef  VMCPD_Set_Virt_State
#define VMCPD_Set_Virt_State DEBUG_VMCPD_Set_Virt_State

#undef  VMPoll_Enable_Disable
#define VMPoll_Enable_Disable DEBUG_VMPoll_Enable_Disable

#undef  VMPoll_Get_Version
#define VMPoll_Get_Version DEBUG_VMPoll_Get_Version

#undef  VMPoll_Reset_Detection
#define VMPoll_Reset_Detection DEBUG_VMPoll_Reset_Detection

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  VPICD_Call_When_Hw_Int
#define VPICD_Call_When_Hw_Int DEBUG_VPICD_Call_When_Hw_Int

#undef  VPICD_Clear_Int_Request
#define VPICD_Clear_Int_Request DEBUG_VPICD_Clear_Int_Request

#undef  VPICD_Convert_Handle_To_IRQ
#define VPICD_Convert_Handle_To_IRQ DEBUG_VPICD_Convert_Handle_To_IRQ

#undef  VPICD_Convert_Int_To_IRQ
#define VPICD_Convert_Int_To_IRQ DEBUG_VPICD_Convert_Int_To_IRQ

#undef  VPICD_Convert_IRQ_To_Int
#define VPICD_Convert_IRQ_To_Int DEBUG_VPICD_Convert_IRQ_To_Int

#undef  VPICD_Force_Default_Behavior
#define VPICD_Force_Default_Behavior DEBUG_VPICD_Force_Default_Behavior

#undef  VPICD_Force_Default_Owner
#define VPICD_Force_Default_Owner DEBUG_VPICD_Force_Default_Owner

#undef  VPICD_Get_Complete_Status
#define VPICD_Get_Complete_Status DEBUG_VPICD_Get_Complete_Status

#undef  VPICD_Get_IRQ_Complete_Status
#define VPICD_Get_IRQ_Complete_Status DEBUG_VPICD_Get_IRQ_Complete_Status

#undef  VPICD_Get_Status
#define VPICD_Get_Status DEBUG_VPICD_Get_Status

#undef  VPICD_Get_Version
#define VPICD_Get_Version DEBUG_VPICD_Get_Version

#undef  VPICD_Phys_EOI
#define VPICD_Phys_EOI DEBUG_VPICD_Phys_EOI

#undef  VPICD_Physically_Mask
#define VPICD_Physically_Mask DEBUG_VPICD_Physically_Mask

#undef  VPICD_Physically_Unmask
#define VPICD_Physically_Unmask DEBUG_VPICD_Physically_Unmask

#undef  VPICD_Set_Auto_Masking
#define VPICD_Set_Auto_Masking DEBUG_VPICD_Set_Auto_Masking

#undef  VPICD_Set_Int_Request
#define VPICD_Set_Int_Request DEBUG_VPICD_Set_Int_Request

#undef  VPICD_Test_Phys_Request
#define VPICD_Test_Phys_Request DEBUG_VPICD_Test_Phys_Request

#undef  VPICD_Virtualize_IRQ
#define VPICD_Virtualize_IRQ DEBUG_VPICD_Virtualize_IRQ

#ifdef WIN40_OR_LATER
#undef  VPICD_Auto_Mask_At_Inst_Swap
#define VPICD_Auto_Mask_At_Inst_Swap DEBUG_VPICD_Auto_Mask_At_Inst_Swap
#endif

#undef  VSD_Get_Version
#define VSD_Get_Version DEBUG_VSD_Get_Version

#undef  VTD_Begin_Min_Int_Period
#define VTD_Begin_Min_Int_Period DEBUG_VTD_Begin_Min_Int_Period

#undef  VTD_Disable_Trapping
#define VTD_Disable_Trapping DEBUG_VTD_Disable_Trapping

#undef  VTD_Enable_Trapping
#define VTD_Enable_Trapping DEBUG_VTD_Enable_Trapping

#undef  VTD_End_Min_Int_Period
#define VTD_End_Min_Int_Period DEBUG_VTD_End_Min_Int_Period

#undef  VTD_Get_Interrupt_Period
#define VTD_Get_Interrupt_Period DEBUG_VTD_Get_Interrupt_Period

#undef  VTD_Get_Real_Time
#define VTD_Get_Real_Time DEBUG_VTD_Get_Real_Time

#undef  VTD_Get_Version
#define VTD_Get_Version DEBUG_VTD_Get_Version

#undef  VPICD_Thunk_HWInt
#define VPICD_Thunk_HWInt DEBUG_VPICD_Thunk_HWInt

#undef  VPICD_Thunk_VirtInt
#define VPICD_Thunk_VirtInt DEBUG_VPICD_Thunk_VirtInt

#undef  VPICD_Thunk_EOI
#define VPICD_Thunk_EOI DEBUG_VPICD_Thunk_EOI

#undef  VPICD_Thunk_MaskChange
#define VPICD_Thunk_MaskChange DEBUG_VPICD_Thunk_MaskChange

#undef  VPICD_Thunk_Iret
#define VPICD_Thunk_Iret DEBUG_VPICD_Thunk_Iret

#undef  Install_Exception_Handler
#define Install_Exception_Handler DEBUG_Install_Exception_Handler

#ifdef WFW311_OR_LATER
#undef  VXDLDR_GetDeviceList
#define VXDLDR_GetDeviceList DEBUG_VXDLDR_GetDeviceList
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_GetVersion
#define VXDLDR_GetVersion DEBUG_VXDLDR_GetVersion
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_LoadDevice
#define VXDLDR_LoadDevice DEBUG_VXDLDR_LoadDevice
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_UnloadDevice
#define VXDLDR_UnloadDevice DEBUG_VXDLDR_UnloadDevice
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_DevInitSucceeded
#define VXDLDR_DevInitSucceeded DEBUG_VXDLDR_DevInitSucceeded
#endif

#ifdef WFW311_OR_LATER
#undef  VXDLDR_DevInitFailed
#define VXDLDR_DevInitFailed DEBUG_VXDLDR_DevInitFailed
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_APM_BIOS_Version
#define _VPOWERD_Get_APM_BIOS_Version DEBUG__VPOWERD_Get_APM_BIOS_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_APM_BIOS_Version
#define VPOWERD_Get_APM_BIOS_Version DEBUG__VPOWERD_Get_APM_BIOS_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_State
#define _VPOWERD_Get_Power_State DEBUG__VPOWERD_Get_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_State
#define VPOWERD_Get_Power_State DEBUG__VPOWERD_Get_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_Management_Level
#define _VPOWERD_Get_Power_Management_Level DEBUG__VPOWERD_Get_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_Management_Level
#define VPOWERD_Get_Power_Management_Level DEBUG__VPOWERD_Get_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Power_Status
#define _VPOWERD_Get_Power_Status DEBUG__VPOWERD_Get_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Power_Status
#define VPOWERD_Get_Power_Status DEBUG__VPOWERD_Get_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_OEM_APM_Function
#define _VPOWERD_OEM_APM_Function DEBUG__VPOWERD_OEM_APM_Function
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_OEM_APM_Function
#define VPOWERD_OEM_APM_Function DEBUG__VPOWERD_OEM_APM_Function
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Register_Power_Handler
#define _VPOWERD_Register_Power_Handler DEBUG__VPOWERD_Register_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Register_Power_Handler
#define VPOWERD_Register_Power_Handler DEBUG__VPOWERD_Register_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Restore_Power_On_Defaults
#define _VPOWERD_Restore_Power_On_Defaults DEBUG__VPOWERD_Restore_Power_On_Defaults
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Restore_Power_On_Defaults
#define VPOWERD_Restore_Power_On_Defaults DEBUG__VPOWERD_Restore_Power_On_Defaults
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_Device_Power_State
#define _VPOWERD_Set_Device_Power_State DEBUG__VPOWERD_Set_Device_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_Device_Power_State
#define VPOWERD_Set_Device_Power_State DEBUG__VPOWERD_Set_Device_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_Power_Management_Level
#define _VPOWERD_Set_Power_Management_Level DEBUG__VPOWERD_Set_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_Power_Management_Level
#define VPOWERD_Set_Power_Management_Level DEBUG__VPOWERD_Set_Power_Management_Level
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Set_System_Power_State
#define _VPOWERD_Set_System_Power_State DEBUG__VPOWERD_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Set_System_Power_State
#define VPOWERD_Set_System_Power_State DEBUG__VPOWERD_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Deregister_Power_Handler
#define _VPOWERD_Deregister_Power_Handler DEBUG__VPOWERD_Deregister_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Deregister_Power_Handler
#define VPOWERD_Deregister_Power_Handler DEBUG__VPOWERD_Deregister_Power_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_Get_Version
#define _VPOWERD_Get_Version DEBUG__VPOWERD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_Get_Version
#define VPOWERD_Get_Version DEBUG__VPOWERD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_W32_Get_System_Power_Status
#define _VPOWERD_W32_Get_System_Power_Status DEBUG__VPOWERD_W32_Get_System_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_W32_Get_System_Power_Status
#define VPOWERD_W32_Get_System_Power_Status DEBUG__VPOWERD_W32_Get_System_Power_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _VPOWERD_W32_Set_System_Power_State
#define _VPOWERD_W32_Set_System_Power_State DEBUG__VPOWERD_W32_Set_System_Power_State
#endif

#ifdef WIN40_OR_LATER
#undef  VPOWERD_W32_Set_System_Power_State
#define VPOWERD_W32_Set_System_Power_State DEBUG__VPOWERD_W32_Set_System_Power_State
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Get_Version
#define VCOMM_Get_Version DEBUG_VCOMM_Get_Version
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Acquire_Port
#define _VCOMM_Acquire_Port DEBUG__VCOMM_Acquire_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Acquire_Port
#define VCOMM_Acquire_Port DEBUG__VCOMM_Acquire_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Add_Port
#define _VCOMM_Add_Port DEBUG__VCOMM_Add_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Add_Port
#define VCOMM_Add_Port DEBUG__VCOMM_Add_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Register_Port_Driver
#define _VCOMM_Register_Port_Driver DEBUG__VCOMM_Register_Port_Driver
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Register_Port_Driver
#define VCOMM_Register_Port_Driver DEBUG__VCOMM_Register_Port_Driver
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_Release_Port
#define _VCOMM_Release_Port DEBUG__VCOMM_Release_Port
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_Release_Port
#define VCOMM_Release_Port DEBUG__VCOMM_Release_Port
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_ClearCommError
#define _VCOMM_ClearCommError DEBUG__VCOMM_ClearCommError
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_ClearCommError
#define VCOMM_ClearCommError DEBUG__VCOMM_ClearCommError
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_CloseComm
#define _VCOMM_CloseComm DEBUG__VCOMM_CloseComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_CloseComm
#define VCOMM_CloseComm DEBUG__VCOMM_CloseComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_EnableCommNotification
#define _VCOMM_EnableCommNotification DEBUG__VCOMM_EnableCommNotification
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_EnableCommNotification
#define VCOMM_EnableCommNotification DEBUG__VCOMM_EnableCommNotification
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_EscapeCommFunction
#define _VCOMM_EscapeCommFunction DEBUG__VCOMM_EscapeCommFunction
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_EscapeCommFunction
#define VCOMM_EscapeCommFunction DEBUG__VCOMM_EscapeCommFunction
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommEventMask
#define _VCOMM_GetCommEventMask DEBUG__VCOMM_GetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommEventMask
#define VCOMM_GetCommEventMask DEBUG__VCOMM_GetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommProperties
#define _VCOMM_GetCommProperties DEBUG__VCOMM_GetCommProperties
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommProperties
#define VCOMM_GetCommProperties DEBUG__VCOMM_GetCommProperties
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommQueueStatus
#define _VCOMM_GetCommQueueStatus DEBUG__VCOMM_GetCommQueueStatus
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommQueueStatus
#define VCOMM_GetCommQueueStatus DEBUG__VCOMM_GetCommQueueStatus
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetCommState
#define _VCOMM_GetCommState DEBUG__VCOMM_GetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetCommState
#define VCOMM_GetCommState DEBUG__VCOMM_GetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetLastError
#define _VCOMM_GetLastError DEBUG__VCOMM_GetLastError
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetLastError
#define VCOMM_GetLastError DEBUG__VCOMM_GetLastError
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_GetModemStatus
#define _VCOMM_GetModemStatus DEBUG__VCOMM_GetModemStatus
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_GetModemStatus
#define VCOMM_GetModemStatus DEBUG__VCOMM_GetModemStatus
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_GetSetCommTimeouts
#define _VCOMM_GetSetCommTimeouts DEBUG__VCOMM_GetSetCommTimeouts
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_GetSetCommTimeouts
#define VCOMM_GetSetCommTimeouts DEBUG__VCOMM_GetSetCommTimeouts
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_OpenComm
#define _VCOMM_OpenComm DEBUG__VCOMM_OpenComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_OpenComm
#define VCOMM_OpenComm DEBUG__VCOMM_OpenComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_PurgeComm
#define _VCOMM_PurgeComm DEBUG__VCOMM_PurgeComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_PurgeComm
#define VCOMM_PurgeComm DEBUG__VCOMM_PurgeComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_ReadComm
#define _VCOMM_ReadComm DEBUG__VCOMM_ReadComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_ReadComm
#define VCOMM_ReadComm DEBUG__VCOMM_ReadComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetCommEventMask
#define _VCOMM_SetCommEventMask DEBUG__VCOMM_SetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetCommEventMask
#define VCOMM_SetCommEventMask DEBUG__VCOMM_SetCommEventMask
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetCommState
#define _VCOMM_SetCommState DEBUG__VCOMM_SetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetCommState
#define VCOMM_SetCommState DEBUG__VCOMM_SetCommState
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetReadCallback
#define _VCOMM_SetReadCallback DEBUG__VCOMM_SetReadCallback
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetReadCallback
#define VCOMM_SetReadCallback DEBUG__VCOMM_SetReadCallback
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetupComm
#define _VCOMM_SetupComm DEBUG__VCOMM_SetupComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetupComm
#define VCOMM_SetupComm DEBUG__VCOMM_SetupComm
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_SetWriteCallback
#define _VCOMM_SetWriteCallback DEBUG__VCOMM_SetWriteCallback
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_SetWriteCallback
#define VCOMM_SetWriteCallback DEBUG__VCOMM_SetWriteCallback
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_TransmitCommChar
#define _VCOMM_TransmitCommChar DEBUG__VCOMM_TransmitCommChar
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_TransmitCommChar
#define VCOMM_TransmitCommChar DEBUG__VCOMM_TransmitCommChar
#endif

#ifdef WFW311_OR_LATER
#undef  _VCOMM_WriteComm
#define _VCOMM_WriteComm DEBUG__VCOMM_WriteComm
#endif

#ifdef WFW311_OR_LATER
#undef  VCOMM_WriteComm
#define VCOMM_WriteComm DEBUG__VCOMM_WriteComm
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Win32DCB_To_Ring0
#define VCOMM_Map_Win32DCB_To_Ring0 DEBUG_VCOMM_Map_Win32DCB_To_Ring0
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Ring0DCB_To_Win32
#define VCOMM_Map_Ring0DCB_To_Win32 DEBUG_VCOMM_Map_Ring0DCB_To_Win32
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Get_Contention_Handler
#define _VCOMM_Get_Contention_Handler DEBUG__VCOMM_Get_Contention_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Get_Contention_Handler
#define VCOMM_Get_Contention_Handler DEBUG__VCOMM_Get_Contention_Handler
#endif

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Map_Name_To_Resource
#define _VCOMM_Map_Name_To_Resource DEBUG__VCOMM_Map_Name_To_Resource
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Map_Name_To_Resource
#define VCOMM_Map_Name_To_Resource DEBUG__VCOMM_Map_Name_To_Resource
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Version
#define _CONFIGMG_Get_Version DEBUG__CONFIGMG_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Version
#define CONFIGMG_Get_Version DEBUG__CONFIGMG_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Initialize
#define _CONFIGMG_Initialize DEBUG__CONFIGMG_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Initialize
#define CONFIGMG_Initialize DEBUG__CONFIGMG_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Locate_DevNode
#define _CONFIGMG_Locate_DevNode DEBUG__CONFIGMG_Locate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Locate_DevNode
#define CONFIGMG_Locate_DevNode DEBUG__CONFIGMG_Locate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Parent
#define _CONFIGMG_Get_Parent DEBUG__CONFIGMG_Get_Parent
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Parent
#define CONFIGMG_Get_Parent DEBUG__CONFIGMG_Get_Parent
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Child
#define _CONFIGMG_Get_Child DEBUG__CONFIGMG_Get_Child
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Child
#define CONFIGMG_Get_Child DEBUG__CONFIGMG_Get_Child
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Sibling
#define _CONFIGMG_Get_Sibling DEBUG__CONFIGMG_Get_Sibling
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Sibling
#define CONFIGMG_Get_Sibling DEBUG__CONFIGMG_Get_Sibling
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_ID_Size
#define _CONFIGMG_Get_Device_ID_Size DEBUG__CONFIGMG_Get_Device_ID_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_ID_Size
#define CONFIGMG_Get_Device_ID_Size DEBUG__CONFIGMG_Get_Device_ID_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_ID
#define _CONFIGMG_Get_Device_ID DEBUG__CONFIGMG_Get_Device_ID
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_ID
#define CONFIGMG_Get_Device_ID DEBUG__CONFIGMG_Get_Device_ID
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Depth
#define _CONFIGMG_Get_Depth DEBUG__CONFIGMG_Get_Depth
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Depth
#define CONFIGMG_Get_Depth DEBUG__CONFIGMG_Get_Depth
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Private_DWord
#define _CONFIGMG_Get_Private_DWord DEBUG__CONFIGMG_Get_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Private_DWord
#define CONFIGMG_Get_Private_DWord DEBUG__CONFIGMG_Get_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Private_DWord
#define _CONFIGMG_Set_Private_DWord DEBUG__CONFIGMG_Set_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Private_DWord
#define CONFIGMG_Set_Private_DWord DEBUG__CONFIGMG_Set_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Create_DevNode
#define _CONFIGMG_Create_DevNode DEBUG__CONFIGMG_Create_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Create_DevNode
#define CONFIGMG_Create_DevNode DEBUG__CONFIGMG_Create_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Remove_SubTree
#define _CONFIGMG_Query_Remove_SubTree DEBUG__CONFIGMG_Query_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Remove_SubTree
#define CONFIGMG_Query_Remove_SubTree DEBUG__CONFIGMG_Query_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Remove_SubTree
#define _CONFIGMG_Remove_SubTree DEBUG__CONFIGMG_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Remove_SubTree
#define CONFIGMG_Remove_SubTree DEBUG__CONFIGMG_Remove_SubTree
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Device_Driver
#define _CONFIGMG_Register_Device_Driver DEBUG__CONFIGMG_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Device_Driver
#define CONFIGMG_Register_Device_Driver DEBUG__CONFIGMG_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Enumerator
#define _CONFIGMG_Register_Enumerator DEBUG__CONFIGMG_Register_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Enumerator
#define CONFIGMG_Register_Enumerator DEBUG__CONFIGMG_Register_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Arbitrator
#define _CONFIGMG_Register_Arbitrator DEBUG__CONFIGMG_Register_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Arbitrator
#define CONFIGMG_Register_Arbitrator DEBUG__CONFIGMG_Register_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Deregister_Arbitrator
#define _CONFIGMG_Deregister_Arbitrator DEBUG__CONFIGMG_Deregister_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Deregister_Arbitrator
#define CONFIGMG_Deregister_Arbitrator DEBUG__CONFIGMG_Deregister_Arbitrator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Res_Des
#define _CONFIGMG_Add_Res_Des DEBUG__CONFIGMG_Add_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Res_Des
#define CONFIGMG_Add_Res_Des DEBUG__CONFIGMG_Add_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Modify_Res_Des
#define _CONFIGMG_Modify_Res_Des DEBUG__CONFIGMG_Modify_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Modify_Res_Des
#define CONFIGMG_Modify_Res_Des DEBUG__CONFIGMG_Modify_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Res_Des
#define _CONFIGMG_Free_Res_Des DEBUG__CONFIGMG_Free_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Res_Des
#define CONFIGMG_Free_Res_Des DEBUG__CONFIGMG_Free_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Next_Res_Des
#define _CONFIGMG_Get_Next_Res_Des DEBUG__CONFIGMG_Get_Next_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Next_Res_Des
#define CONFIGMG_Get_Next_Res_Des DEBUG__CONFIGMG_Get_Next_Res_Des
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Performance_Info
#define _CONFIGMG_Get_Performance_Info DEBUG__CONFIGMG_Get_Performance_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Performance_Info
#define CONFIGMG_Get_Performance_Info DEBUG__CONFIGMG_Get_Performance_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Res_Des_Data_Size
#define _CONFIGMG_Get_Res_Des_Data_Size DEBUG__CONFIGMG_Get_Res_Des_Data_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Res_Des_Data_Size
#define CONFIGMG_Get_Res_Des_Data_Size DEBUG__CONFIGMG_Get_Res_Des_Data_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Res_Des_Data
#define _CONFIGMG_Get_Res_Des_Data DEBUG__CONFIGMG_Get_Res_Des_Data
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Res_Des_Data
#define CONFIGMG_Get_Res_Des_Data DEBUG__CONFIGMG_Get_Res_Des_Data
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Process_Events_Now
#define _CONFIGMG_Process_Events_Now DEBUG__CONFIGMG_Process_Events_Now
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Process_Events_Now
#define CONFIGMG_Process_Events_Now DEBUG__CONFIGMG_Process_Events_Now
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Create_Range_List
#define _CONFIGMG_Create_Range_List DEBUG__CONFIGMG_Create_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Create_Range_List
#define CONFIGMG_Create_Range_List DEBUG__CONFIGMG_Create_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Range
#define _CONFIGMG_Add_Range DEBUG__CONFIGMG_Add_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Range
#define CONFIGMG_Add_Range DEBUG__CONFIGMG_Add_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Delete_Range
#define _CONFIGMG_Delete_Range DEBUG__CONFIGMG_Delete_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Delete_Range
#define CONFIGMG_Delete_Range DEBUG__CONFIGMG_Delete_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Test_Range_Available
#define _CONFIGMG_Test_Range_Available DEBUG__CONFIGMG_Test_Range_Available
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Test_Range_Available
#define CONFIGMG_Test_Range_Available DEBUG__CONFIGMG_Test_Range_Available
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Dup_Range_List
#define _CONFIGMG_Dup_Range_List DEBUG__CONFIGMG_Dup_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Dup_Range_List
#define CONFIGMG_Dup_Range_List DEBUG__CONFIGMG_Dup_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Range_List
#define _CONFIGMG_Free_Range_List DEBUG__CONFIGMG_Free_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Range_List
#define CONFIGMG_Free_Range_List DEBUG__CONFIGMG_Free_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Invert_Range_List
#define _CONFIGMG_Invert_Range_List DEBUG__CONFIGMG_Invert_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Invert_Range_List
#define CONFIGMG_Invert_Range_List DEBUG__CONFIGMG_Invert_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Intersect_Range_List
#define _CONFIGMG_Intersect_Range_List DEBUG__CONFIGMG_Intersect_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Intersect_Range_List
#define CONFIGMG_Intersect_Range_List DEBUG__CONFIGMG_Intersect_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_First_Range
#define _CONFIGMG_First_Range DEBUG__CONFIGMG_First_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_First_Range
#define CONFIGMG_First_Range DEBUG__CONFIGMG_First_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Next_Range
#define _CONFIGMG_Next_Range DEBUG__CONFIGMG_Next_Range
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Next_Range
#define CONFIGMG_Next_Range DEBUG__CONFIGMG_Next_Range
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Dump_Range_List
#define _CONFIGMG_Dump_Range_List DEBUG__CONFIGMG_Dump_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Dump_Range_List
#define CONFIGMG_Dump_Range_List DEBUG__CONFIGMG_Dump_Range_List
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Load_DLVxDs
#define _CONFIGMG_Load_DLVxDs DEBUG__CONFIGMG_Load_DLVxDs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Load_DLVxDs
#define CONFIGMG_Load_DLVxDs DEBUG__CONFIGMG_Load_DLVxDs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DDBs
#define _CONFIGMG_Get_DDBs DEBUG__CONFIGMG_Get_DDBs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DDBs
#define CONFIGMG_Get_DDBs DEBUG__CONFIGMG_Get_DDBs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_CRC_CheckSum
#define _CONFIGMG_Get_CRC_CheckSum DEBUG__CONFIGMG_Get_CRC_CheckSum
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_CRC_CheckSum
#define CONFIGMG_Get_CRC_CheckSum DEBUG__CONFIGMG_Get_CRC_CheckSum
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_DevLoader
#define _CONFIGMG_Register_DevLoader DEBUG__CONFIGMG_Register_DevLoader
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_DevLoader
#define CONFIGMG_Register_DevLoader DEBUG__CONFIGMG_Register_DevLoader
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Reenumerate_DevNode
#define _CONFIGMG_Reenumerate_DevNode DEBUG__CONFIGMG_Reenumerate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Reenumerate_DevNode
#define CONFIGMG_Reenumerate_DevNode DEBUG__CONFIGMG_Reenumerate_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Setup_DevNode
#define _CONFIGMG_Setup_DevNode DEBUG__CONFIGMG_Setup_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Setup_DevNode
#define CONFIGMG_Setup_DevNode DEBUG__CONFIGMG_Setup_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Reset_Children_Marks
#define _CONFIGMG_Reset_Children_Marks DEBUG__CONFIGMG_Reset_Children_Marks
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Reset_Children_Marks
#define CONFIGMG_Reset_Children_Marks DEBUG__CONFIGMG_Reset_Children_Marks
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Status
#define _CONFIGMG_Get_DevNode_Status DEBUG__CONFIGMG_Get_DevNode_Status
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Status
#define CONFIGMG_Get_DevNode_Status DEBUG__CONFIGMG_Get_DevNode_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Remove_Unmarked_Children
#define _CONFIGMG_Remove_Unmarked_Children DEBUG__CONFIGMG_Remove_Unmarked_Children
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Remove_Unmarked_Children
#define CONFIGMG_Remove_Unmarked_Children DEBUG__CONFIGMG_Remove_Unmarked_Children
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_ISAPNP_To_CM
#define _CONFIGMG_ISAPNP_To_CM DEBUG__CONFIGMG_ISAPNP_To_CM
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_ISAPNP_To_CM
#define CONFIGMG_ISAPNP_To_CM DEBUG__CONFIGMG_ISAPNP_To_CM
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_CallBack_Device_Driver
#define _CONFIGMG_CallBack_Device_Driver DEBUG__CONFIGMG_CallBack_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_CallBack_Device_Driver
#define CONFIGMG_CallBack_Device_Driver DEBUG__CONFIGMG_CallBack_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_CallBack_Enumerator
#define _CONFIGMG_CallBack_Enumerator DEBUG__CONFIGMG_CallBack_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_CallBack_Enumerator
#define CONFIGMG_CallBack_Enumerator DEBUG__CONFIGMG_CallBack_Enumerator
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Alloc_Log_Conf
#define _CONFIGMG_Get_Alloc_Log_Conf DEBUG__CONFIGMG_Get_Alloc_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Alloc_Log_Conf
#define CONFIGMG_Get_Alloc_Log_Conf DEBUG__CONFIGMG_Get_Alloc_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Key_Size
#define _CONFIGMG_Get_DevNode_Key_Size DEBUG__CONFIGMG_Get_DevNode_Key_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Key_Size
#define CONFIGMG_Get_DevNode_Key_Size DEBUG__CONFIGMG_Get_DevNode_Key_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_DevNode_Key
#define _CONFIGMG_Get_DevNode_Key DEBUG__CONFIGMG_Get_DevNode_Key
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_DevNode_Key
#define CONFIGMG_Get_DevNode_Key DEBUG__CONFIGMG_Get_DevNode_Key
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Read_Registry_Value
#define _CONFIGMG_Read_Registry_Value DEBUG__CONFIGMG_Read_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Read_Registry_Value
#define CONFIGMG_Read_Registry_Value DEBUG__CONFIGMG_Read_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Write_Registry_Value
#define _CONFIGMG_Write_Registry_Value DEBUG__CONFIGMG_Write_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Write_Registry_Value
#define CONFIGMG_Write_Registry_Value DEBUG__CONFIGMG_Write_Registry_Value
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Disable_DevNode
#define _CONFIGMG_Disable_DevNode DEBUG__CONFIGMG_Disable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Disable_DevNode
#define CONFIGMG_Disable_DevNode DEBUG__CONFIGMG_Disable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Enable_DevNode
#define _CONFIGMG_Enable_DevNode DEBUG__CONFIGMG_Enable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Enable_DevNode
#define CONFIGMG_Enable_DevNode DEBUG__CONFIGMG_Enable_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Move_DevNode
#define _CONFIGMG_Move_DevNode DEBUG__CONFIGMG_Move_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Move_DevNode
#define CONFIGMG_Move_DevNode DEBUG__CONFIGMG_Move_DevNode
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Bus_Info
#define _CONFIGMG_Set_Bus_Info DEBUG__CONFIGMG_Set_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Bus_Info
#define CONFIGMG_Set_Bus_Info DEBUG__CONFIGMG_Set_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Bus_Info
#define _CONFIGMG_Get_Bus_Info DEBUG__CONFIGMG_Get_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Bus_Info
#define CONFIGMG_Get_Bus_Info DEBUG__CONFIGMG_Get_Bus_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_HW_Prof
#define _CONFIGMG_Set_HW_Prof DEBUG__CONFIGMG_Set_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_HW_Prof
#define CONFIGMG_Set_HW_Prof DEBUG__CONFIGMG_Set_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Recompute_HW_Prof
#define _CONFIGMG_Recompute_HW_Prof DEBUG__CONFIGMG_Recompute_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Recompute_HW_Prof
#define CONFIGMG_Recompute_HW_Prof DEBUG__CONFIGMG_Recompute_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Change_HW_Prof
#define _CONFIGMG_Query_Change_HW_Prof DEBUG__CONFIGMG_Query_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Change_HW_Prof
#define CONFIGMG_Query_Change_HW_Prof DEBUG__CONFIGMG_Query_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Device_Driver_Private_DWord
#define _CONFIGMG_Get_Device_Driver_Private_DWord DEBUG__CONFIGMG_Get_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Device_Driver_Private_DWord
#define CONFIGMG_Get_Device_Driver_Private_DWord DEBUG__CONFIGMG_Get_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Device_Driver_Private_DWord
#define _CONFIGMG_Set_Device_Driver_Private_DWord DEBUG__CONFIGMG_Set_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Device_Driver_Private_DWord
#define CONFIGMG_Set_Device_Driver_Private_DWord DEBUG__CONFIGMG_Set_Device_Driver_Private_DWord
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_HW_Prof_Flags
#define _CONFIGMG_Get_HW_Prof_Flags DEBUG__CONFIGMG_Get_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_HW_Prof_Flags
#define CONFIGMG_Get_HW_Prof_Flags DEBUG__CONFIGMG_Get_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_HW_Prof_Flags
#define _CONFIGMG_Set_HW_Prof_Flags DEBUG__CONFIGMG_Set_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_HW_Prof_Flags
#define CONFIGMG_Set_HW_Prof_Flags DEBUG__CONFIGMG_Set_HW_Prof_Flags
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Read_Registry_Log_Confs
#define _CONFIGMG_Read_Registry_Log_Confs DEBUG__CONFIGMG_Read_Registry_Log_Confs
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Read_Registry_Log_Confs
#define CONFIGMG_Read_Registry_Log_Confs DEBUG__CONFIGMG_Read_Registry_Log_Confs
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Run_Detection
#define _CONFIGMG_Run_Detection DEBUG__CONFIGMG_Run_Detection
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Run_Detection
#define CONFIGMG_Run_Detection DEBUG__CONFIGMG_Run_Detection
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Call_At_Appy_Time
#define _CONFIGMG_Call_At_Appy_Time DEBUG__CONFIGMG_Call_At_Appy_Time
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Call_At_Appy_Time
#define CONFIGMG_Call_At_Appy_Time DEBUG__CONFIGMG_Call_At_Appy_Time
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Set_Private_Problem
#define _CONFIGMG_Set_Private_Problem DEBUG__CONFIGMG_Set_Private_Problem
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Set_Private_Problem
#define CONFIGMG_Set_Private_Problem DEBUG__CONFIGMG_Set_Private_Problem
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Hardware_Profile_Info
#define _CONFIGMG_Get_Hardware_Profile_Info DEBUG__CONFIGMG_Get_Hardware_Profile_Info
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Hardware_Profile_Info
#define CONFIGMG_Get_Hardware_Profile_Info DEBUG__CONFIGMG_Get_Hardware_Profile_Info
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Arbitrator_Free_Size
#define _CONFIGMG_Query_Arbitrator_Free_Size DEBUG__CONFIGMG_Query_Arbitrator_Free_Size
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Arbitrator_Free_Size
#define CONFIGMG_Query_Arbitrator_Free_Size DEBUG__CONFIGMG_Query_Arbitrator_Free_Size
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Query_Arbitrator_Free_Data
#define _CONFIGMG_Query_Arbitrator_Free_Data DEBUG__CONFIGMG_Query_Arbitrator_Free_Data
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Query_Arbitrator_Free_Data
#define CONFIGMG_Query_Arbitrator_Free_Data DEBUG__CONFIGMG_Query_Arbitrator_Free_Data
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Sort_NodeList
#define _CONFIGMG_Sort_NodeList DEBUG__CONFIGMG_Sort_NodeList
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Sort_NodeList
#define CONFIGMG_Sort_NodeList DEBUG__CONFIGMG_Sort_NodeList
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Yield
#define _CONFIGMG_Yield DEBUG__CONFIGMG_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Yield
#define CONFIGMG_Yield DEBUG__CONFIGMG_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Lock
#define _CONFIGMG_Lock DEBUG__CONFIGMG_Lock
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Lock
#define CONFIGMG_Lock DEBUG__CONFIGMG_Lock
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Unlock
#define _CONFIGMG_Unlock DEBUG__CONFIGMG_Unlock
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Unlock
#define CONFIGMG_Unlock DEBUG__CONFIGMG_Unlock
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_Empty_Log_Conf
#define _CONFIGMG_Add_Empty_Log_Conf DEBUG__CONFIGMG_Add_Empty_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_Empty_Log_Conf
#define CONFIGMG_Add_Empty_Log_Conf DEBUG__CONFIGMG_Add_Empty_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Free_Log_Conf
#define _CONFIGMG_Free_Log_Conf DEBUG__CONFIGMG_Free_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Free_Log_Conf
#define CONFIGMG_Free_Log_Conf DEBUG__CONFIGMG_Free_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_First_Log_Conf
#define _CONFIGMG_Get_First_Log_Conf DEBUG__CONFIGMG_Get_First_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_First_Log_Conf
#define CONFIGMG_Get_First_Log_Conf DEBUG__CONFIGMG_Get_First_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Get_Next_Log_Conf
#define _CONFIGMG_Get_Next_Log_Conf DEBUG__CONFIGMG_Get_Next_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Get_Next_Log_Conf
#define CONFIGMG_Get_Next_Log_Conf DEBUG__CONFIGMG_Get_Next_Log_Conf
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Fail_Change_HW_Prof
#define _CONFIGMG_Fail_Change_HW_Prof DEBUG__CONFIGMG_Fail_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Fail_Change_HW_Prof
#define CONFIGMG_Fail_Change_HW_Prof DEBUG__CONFIGMG_Fail_Change_HW_Prof
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Call_Enumerator_Function
#define _CONFIGMG_Call_Enumerator_Function DEBUG__CONFIGMG_Call_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Call_Enumerator_Function
#define CONFIGMG_Call_Enumerator_Function DEBUG__CONFIGMG_Call_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Post_Pointer_Message
#define VMD_Post_Pointer_Message DEBUG_VMD_Post_Pointer_Message
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Set_Cursor_Proc
#define VMD_Set_Cursor_Proc DEBUG_VMD_Set_Cursor_Proc
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Call_Cursor_Proc
#define VMD_Call_Cursor_Proc DEBUG_VMD_Call_Cursor_Proc
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Set_Mouse_Data
#define VMD_Set_Mouse_Data DEBUG_VMD_Set_Mouse_Data
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Get_Mouse_Data
#define VMD_Get_Mouse_Data DEBUG_VMD_Get_Mouse_Data
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Post_Absolute_Pointer_Message
#define VMD_Post_Absolute_Pointer_Message DEBUG_VMD_Post_Absolute_Pointer_Message
#endif

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_Version
#define IFSMgr_Get_Version DEBUG_IFSMgr_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterMount
#define IFSMgr_RegisterMount DEBUG_IFSMgr_RegisterMount
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterNet
#define IFSMgr_RegisterNet DEBUG_IFSMgr_RegisterNet
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterMailSlot
#define IFSMgr_RegisterMailSlot DEBUG_IFSMgr_RegisterMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_NetTime
#define IFSMgr_Get_NetTime DEBUG_IFSMgr_Get_NetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_DOSTime
#define IFSMgr_Get_DOSTime DEBUG_IFSMgr_Get_DOSTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetupConnection
#define IFSMgr_SetupConnection DEBUG_IFSMgr_SetupConnection
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DerefConnection
#define IFSMgr_DerefConnection DEBUG_IFSMgr_DerefConnection
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ServerDOSCall
#define IFSMgr_ServerDOSCall DEBUG_IFSMgr_ServerDOSCall
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CompleteAsync
#define IFSMgr_CompleteAsync DEBUG_IFSMgr_CompleteAsync
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterHeap
#define IFSMgr_RegisterHeap DEBUG_IFSMgr_RegisterHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetHeap
#define IFSMgr_GetHeap DEBUG_IFSMgr_GetHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RetHeap
#define IFSMgr_RetHeap DEBUG_IFSMgr_RetHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckHeap
#define IFSMgr_CheckHeap DEBUG_IFSMgr_CheckHeap
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckHeapItem
#define IFSMgr_CheckHeapItem DEBUG_IFSMgr_CheckHeapItem
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FillHeapSpare
#define IFSMgr_FillHeapSpare DEBUG_IFSMgr_FillHeapSpare
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Block
#define IFSMgr_Block DEBUG_IFSMgr_Block
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Wakeup
#define IFSMgr_Wakeup DEBUG_IFSMgr_Wakeup
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Yield
#define IFSMgr_Yield DEBUG_IFSMgr_Yield
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SchedEvent
#define IFSMgr_SchedEvent DEBUG_IFSMgr_SchedEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_QueueEvent
#define IFSMgr_QueueEvent DEBUG_IFSMgr_QueueEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_KillEvent
#define IFSMgr_KillEvent DEBUG_IFSMgr_KillEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FreeIOReq
#define IFSMgr_FreeIOReq DEBUG_IFSMgr_FreeIOReq
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MakeMailSlot
#define IFSMgr_MakeMailSlot DEBUG_IFSMgr_MakeMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DeleteMailSlot
#define IFSMgr_DeleteMailSlot DEBUG_IFSMgr_DeleteMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_WriteMailSlot
#define IFSMgr_WriteMailSlot DEBUG_IFSMgr_WriteMailSlot
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_PopUp
#define IFSMgr_PopUp DEBUG_IFSMgr_PopUp
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_printf
#define IFSMgr_printf DEBUG_IFSMgr_printf
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_AssertFailed
#define IFSMgr_AssertFailed DEBUG_IFSMgr_AssertFailed
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_LogEntry
#define IFSMgr_LogEntry DEBUG_IFSMgr_LogEntry
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DebugMenu
#define IFSMgr_DebugMenu DEBUG_IFSMgr_DebugMenu
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DebugVars
#define IFSMgr_DebugVars DEBUG_IFSMgr_DebugVars
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetDebugString
#define IFSMgr_GetDebugString DEBUG_IFSMgr_GetDebugString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetDebugHexNum
#define IFSMgr_GetDebugHexNum DEBUG_IFSMgr_GetDebugHexNum
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetFunction
#define IFSMgr_NetFunction DEBUG_IFSMgr_NetFunction
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DoDelAllUses
#define IFSMgr_DoDelAllUses DEBUG_IFSMgr_DoDelAllUses
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetErrString
#define IFSMgr_SetErrString DEBUG_IFSMgr_SetErrString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetErrString
#define IFSMgr_GetErrString DEBUG_IFSMgr_GetErrString
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetReqHook
#define IFSMgr_SetReqHook DEBUG_IFSMgr_SetReqHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetPathHook
#define IFSMgr_SetPathHook DEBUG_IFSMgr_SetPathHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UseAdd
#define IFSMgr_UseAdd DEBUG_IFSMgr_UseAdd
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UseDel
#define IFSMgr_UseDel DEBUG_IFSMgr_UseDel
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_InitUseAdd
#define IFSMgr_InitUseAdd DEBUG_IFSMgr_InitUseAdd
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ChangeDir
#define IFSMgr_ChangeDir DEBUG_IFSMgr_ChangeDir
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DelAllUses
#define IFSMgr_DelAllUses DEBUG_IFSMgr_DelAllUses
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CDROM_Attach
#define IFSMgr_CDROM_Attach DEBUG_IFSMgr_CDROM_Attach
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CDROM_Detach
#define IFSMgr_CDROM_Detach DEBUG_IFSMgr_CDROM_Detach
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32DupHandle
#define IFSMgr_Win32DupHandle DEBUG_IFSMgr_Win32DupHandle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Ring0_FileIO
#define IFSMgr_Ring0_FileIO DEBUG_IFSMgr_Ring0_FileIO
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32_Get_Ring0_Handle
#define IFSMgr_Win32_Get_Ring0_Handle DEBUG_IFSMgr_Win32_Get_Ring0_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Get_Drive_Info
#define IFSMgr_Get_Drive_Info DEBUG_IFSMgr_Get_Drive_Info
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Ring0GetDriveInfo
#define IFSMgr_Ring0GetDriveInfo DEBUG_IFSMgr_Ring0GetDriveInfo
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_BlockNoEvents
#define IFSMgr_BlockNoEvents DEBUG_IFSMgr_BlockNoEvents
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetToDosTime
#define IFSMgr_NetToDosTime DEBUG_IFSMgr_NetToDosTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DosToNetTime
#define IFSMgr_DosToNetTime DEBUG_IFSMgr_DosToNetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DosToWin32Time
#define IFSMgr_DosToWin32Time DEBUG_IFSMgr_DosToWin32Time
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32ToDosTime
#define IFSMgr_Win32ToDosTime DEBUG_IFSMgr_Win32ToDosTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_NetToWin32Time
#define IFSMgr_NetToWin32Time DEBUG_IFSMgr_NetToWin32Time
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32ToNetTime
#define IFSMgr_Win32ToNetTime DEBUG_IFSMgr_Win32ToNetTime
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MetaMatch
#define IFSMgr_MetaMatch DEBUG_IFSMgr_MetaMatch
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_TransMatch
#define IFSMgr_TransMatch DEBUG_IFSMgr_TransMatch
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CallProvider
#define IFSMgr_CallProvider DEBUG_IFSMgr_CallProvider
#endif

#ifdef WIN40_OR_LATER
#undef  UniToBCS
#define UniToBCS DEBUG_UniToBCS
#endif

#ifdef WIN40_OR_LATER
#undef  UniToBCSPath
#define UniToBCSPath DEBUG_UniToBCSPath
#endif

#ifdef WIN40_OR_LATER
#undef  BCSToUni
#define BCSToUni DEBUG_BCSToUni
#endif

#ifdef WIN40_OR_LATER
#undef  UniToUpper
#define UniToUpper DEBUG_UniToUpper
#endif

#ifdef WIN40_OR_LATER
#undef  UniCharToOEM
#define UniCharToOEM DEBUG_UniCharToOEM
#endif

#ifdef WIN40_OR_LATER
#undef  CreateBasis
#define CreateBasis DEBUG_CreateBasis
#endif

#ifdef WIN40_OR_LATER
#undef  MatchBasisName
#define MatchBasisName DEBUG_MatchBasisName
#endif

#ifdef WIN40_OR_LATER
#undef  AppendBasisTail
#define AppendBasisTail DEBUG_AppendBasisTail
#endif

#ifdef WIN40_OR_LATER
#undef  FcbToShort
#define FcbToShort DEBUG_FcbToShort
#endif

#ifdef WIN40_OR_LATER
#undef  ShortToFcb
#define ShortToFcb DEBUG_ShortToFcb
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ParsePath
#define IFSMgr_ParsePath DEBUG_IFSMgr_ParsePath
#endif

#ifdef WIN40_OR_LATER
#undef  Query_PhysLock
#define Query_PhysLock DEBUG_Query_PhysLock
#endif

#ifdef WIN40_OR_LATER
#undef  _VolFlush
#define _VolFlush DEBUG__VolFlush
#endif

#ifdef WIN40_OR_LATER
#undef  VolFlush
#define VolFlush DEBUG__VolFlush
#endif

#ifdef WIN40_OR_LATER
#undef  NotifyVolumeArrival
#define NotifyVolumeArrival DEBUG_NotifyVolumeArrival
#endif

#ifdef WIN40_OR_LATER
#undef  NotifyVolumeRemoval
#define NotifyVolumeRemoval DEBUG_NotifyVolumeRemoval
#endif

#ifdef WIN40_OR_LATER
#undef  QueryVolumeRemoval
#define QueryVolumeRemoval DEBUG_QueryVolumeRemoval
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDUnmountCFSD
#define IFSMgr_FSDUnmountCFSD DEBUG_IFSMgr_FSDUnmountCFSD
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetConversionTablePtrs
#define IFSMgr_GetConversionTablePtrs DEBUG_IFSMgr_GetConversionTablePtrs
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckAccessConflict
#define IFSMgr_CheckAccessConflict DEBUG_IFSMgr_CheckAccessConflict
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_LockFile
#define IFSMgr_LockFile DEBUG_IFSMgr_LockFile
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnlockFile
#define IFSMgr_UnlockFile DEBUG_IFSMgr_UnlockFile
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RemoveLocks
#define IFSMgr_RemoveLocks DEBUG_IFSMgr_RemoveLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckLocks
#define IFSMgr_CheckLocks DEBUG_IFSMgr_CheckLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CountLocks
#define IFSMgr_CountLocks DEBUG_IFSMgr_CountLocks
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ReassignLockFileInst
#define IFSMgr_ReassignLockFileInst DEBUG_IFSMgr_ReassignLockFileInst
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnassignLockList
#define IFSMgr_UnassignLockList DEBUG_IFSMgr_UnassignLockList
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_MountChildVolume
#define IFSMgr_MountChildVolume DEBUG_IFSMgr_MountChildVolume
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_UnmountChildVolume
#define IFSMgr_UnmountChildVolume DEBUG_IFSMgr_UnmountChildVolume
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SwapDrives
#define IFSMgr_SwapDrives DEBUG_IFSMgr_SwapDrives
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDMapFHtoIOREQ
#define IFSMgr_FSDMapFHtoIOREQ DEBUG_IFSMgr_FSDMapFHtoIOREQ
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDParsePath
#define IFSMgr_FSDParsePath DEBUG_IFSMgr_FSDParsePath
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDAttachSFT
#define IFSMgr_FSDAttachSFT DEBUG_IFSMgr_FSDAttachSFT
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetTimeZoneBias
#define IFSMgr_GetTimeZoneBias DEBUG_IFSMgr_GetTimeZoneBias
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_PNPEvent
#define IFSMgr_PNPEvent DEBUG_IFSMgr_PNPEvent
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RegisterCFSD
#define IFSMgr_RegisterCFSD DEBUG_IFSMgr_RegisterCFSD
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32MapExtendedHandleToSFT
#define IFSMgr_Win32MapExtendedHandleToSFT DEBUG_IFSMgr_Win32MapExtendedHandleToSFT
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_DbgSetFileHandleLimit
#define IFSMgr_DbgSetFileHandleLimit DEBUG_IFSMgr_DbgSetFileHandleLimit
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32MapSFTToExtendedHandle
#define IFSMgr_Win32MapSFTToExtendedHandle DEBUG_IFSMgr_Win32MapSFTToExtendedHandle
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_FSDGetCurrentDrive
#define IFSMgr_FSDGetCurrentDrive DEBUG_IFSMgr_FSDGetCurrentDrive
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_InstallFileSystemApiHook
#define IFSMgr_InstallFileSystemApiHook DEBUG_IFSMgr_InstallFileSystemApiHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RemoveFileSystemApiHook
#define IFSMgr_RemoveFileSystemApiHook DEBUG_IFSMgr_RemoveFileSystemApiHook
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_RunScheduledEvents
#define IFSMgr_RunScheduledEvents DEBUG_IFSMgr_RunScheduledEvents
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_CheckDelResource
#define IFSMgr_CheckDelResource DEBUG_IFSMgr_CheckDelResource
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_Win32GetVMCurdir
#define IFSMgr_Win32GetVMCurdir DEBUG_IFSMgr_Win32GetVMCurdir
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetupFailedConnection
#define IFSMgr_SetupFailedConnection DEBUG_IFSMgr_SetupFailedConnection
#endif

#ifdef WIN40_OR_LATER
#undef  _GetMappedErr
#define _GetMappedErr DEBUG__GetMappedErr
#endif

#ifdef WIN40_OR_LATER
#undef  GetMappedErr
#define GetMappedErr DEBUG__GetMappedErr
#endif

#ifdef WIN40_OR_LATER
#undef  ShortToLossyFcb
#define ShortToLossyFcb DEBUG_ShortToLossyFcb
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_GetLockState
#define IFSMgr_GetLockState DEBUG_IFSMgr_GetLockState
#endif

#ifdef WIN40_OR_LATER
#undef  BcsToBcs
#define BcsToBcs DEBUG_BcsToBcs
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_SetLoopback
#define IFSMgr_SetLoopback DEBUG_IFSMgr_SetLoopback
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ClearLoopback
#define IFSMgr_ClearLoopback DEBUG_IFSMgr_ClearLoopback
#endif

#ifdef WIN40_OR_LATER
#undef  IFSMgr_ParseOneElement
#define IFSMgr_ParseOneElement DEBUG_IFSMgr_ParseOneElement
#endif

#ifdef WIN40_OR_LATER
#undef  BcsToBcsUpper
#define BcsToBcsUpper DEBUG_BcsToBcsUpper
#endif

#ifdef WIN40_OR_LATER
#undef  R0_OpenCreateFile
#define R0_OpenCreateFile DEBUG_R0_OpenCreateFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_ReadFile
#define R0_ReadFile DEBUG_R0_ReadFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_WriteFile
#define R0_WriteFile DEBUG_R0_WriteFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_CloseFile
#define R0_CloseFile DEBUG_R0_CloseFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetFileSize
#define R0_GetFileSize DEBUG_R0_GetFileSize
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindFirstFile
#define R0_FindFirstFile DEBUG_R0_FindFirstFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindNextFile
#define R0_FindNextFile DEBUG_R0_FindNextFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_FindCloseFile
#define R0_FindCloseFile DEBUG_R0_FindCloseFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetFileAttributes
#define R0_GetFileAttributes DEBUG_R0_GetFileAttributes
#endif

#ifdef WIN40_OR_LATER
#undef  R0_SetFileAttributes
#define R0_SetFileAttributes DEBUG_R0_SetFileAttributes
#endif

#ifdef WIN40_OR_LATER
#undef  R0_RenameFile
#define R0_RenameFile DEBUG_R0_RenameFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_DeleteFile
#define R0_DeleteFile DEBUG_R0_DeleteFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_LockFile
#define R0_LockFile DEBUG_R0_LockFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_UnlockFile
#define R0_UnlockFile DEBUG_R0_UnlockFile
#endif

#ifdef WIN40_OR_LATER
#undef  R0_GetDiskFreeSpace
#define R0_GetDiskFreeSpace DEBUG_R0_GetDiskFreeSpace
#endif

#ifdef WIN40_OR_LATER
#undef  R0_ReadAbsoluteDisk
#define R0_ReadAbsoluteDisk DEBUG_R0_ReadAbsoluteDisk
#endif

#ifdef WIN40_OR_LATER
#undef  R0_WriteAbsoluteDisk
#define R0_WriteAbsoluteDisk DEBUG_R0_WriteAbsoluteDisk
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Get_Version
#define VWIN32_Get_Version DEBUG_VWIN32_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_DIOCCompletionRoutine
#define VWIN32_DIOCCompletionRoutine DEBUG_VWIN32_DIOCCompletionRoutine
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_QueueUserApc
#define _VWIN32_QueueUserApc DEBUG__VWIN32_QueueUserApc
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_QueueUserApc
#define VWIN32_QueueUserApc DEBUG__VWIN32_QueueUserApc
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Get_Thread_Context
#define _VWIN32_Get_Thread_Context DEBUG__VWIN32_Get_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Get_Thread_Context
#define VWIN32_Get_Thread_Context DEBUG__VWIN32_Get_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Emulate_Npx
#define _VWIN32_Emulate_Npx DEBUG__VWIN32_Emulate_Npx
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Emulate_Npx
#define VWIN32_Emulate_Npx DEBUG__VWIN32_Emulate_Npx
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CheckDelayedNpxTrap
#define _VWIN32_CheckDelayedNpxTrap DEBUG__VWIN32_CheckDelayedNpxTrap
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CheckDelayedNpxTrap
#define VWIN32_CheckDelayedNpxTrap DEBUG__VWIN32_CheckDelayedNpxTrap
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_LeaveCrstR0
#define VWIN32_LeaveCrstR0 DEBUG_VWIN32_LeaveCrstR0
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_FaultPopup
#define _VWIN32_FaultPopup DEBUG__VWIN32_FaultPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_FaultPopup
#define VWIN32_FaultPopup DEBUG__VWIN32_FaultPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetContextHandle
#define VWIN32_GetContextHandle DEBUG_VWIN32_GetContextHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetCurrentProcessHandle
#define VWIN32_GetCurrentProcessHandle DEBUG_VWIN32_GetCurrentProcessHandle
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_SetWin32Event
#define _VWIN32_SetWin32Event DEBUG__VWIN32_SetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_SetWin32Event
#define VWIN32_SetWin32Event DEBUG__VWIN32_SetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_PulseWin32Event
#define _VWIN32_PulseWin32Event DEBUG__VWIN32_PulseWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_PulseWin32Event
#define VWIN32_PulseWin32Event DEBUG__VWIN32_PulseWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_ResetWin32Event
#define _VWIN32_ResetWin32Event DEBUG__VWIN32_ResetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_ResetWin32Event
#define VWIN32_ResetWin32Event DEBUG__VWIN32_ResetWin32Event
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_WaitSingleObject
#define _VWIN32_WaitSingleObject DEBUG__VWIN32_WaitSingleObject
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_WaitSingleObject
#define VWIN32_WaitSingleObject DEBUG__VWIN32_WaitSingleObject
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_WaitMultipleObjects
#define _VWIN32_WaitMultipleObjects DEBUG__VWIN32_WaitMultipleObjects
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_WaitMultipleObjects
#define VWIN32_WaitMultipleObjects DEBUG__VWIN32_WaitMultipleObjects
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CreateRing0Thread
#define _VWIN32_CreateRing0Thread DEBUG__VWIN32_CreateRing0Thread
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CreateRing0Thread
#define VWIN32_CreateRing0Thread DEBUG__VWIN32_CreateRing0Thread
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CloseVxDHandle
#define _VWIN32_CloseVxDHandle DEBUG__VWIN32_CloseVxDHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CloseVxDHandle
#define VWIN32_CloseVxDHandle DEBUG__VWIN32_CloseVxDHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_ActiveTimeBiasSet
#define VWIN32_ActiveTimeBiasSet DEBUG_VWIN32_ActiveTimeBiasSet
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_EnterCrstR0
#define VWIN32_EnterCrstR0 DEBUG_VWIN32_EnterCrstR0
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_BlueScreenPopup
#define VWIN32_BlueScreenPopup DEBUG_VWIN32_BlueScreenPopup
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_TerminateApp
#define VWIN32_TerminateApp DEBUG_VWIN32_TerminateApp
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_QueueKernelAPC
#define _VWIN32_QueueKernelAPC DEBUG__VWIN32_QueueKernelAPC
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_QueueKernelAPC
#define VWIN32_QueueKernelAPC DEBUG__VWIN32_QueueKernelAPC
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_SysErrorBox
#define VWIN32_SysErrorBox DEBUG_VWIN32_SysErrorBox
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_IsClientWin32
#define _VWIN32_IsClientWin32 DEBUG__VWIN32_IsClientWin32
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_IsClientWin32
#define VWIN32_IsClientWin32 DEBUG__VWIN32_IsClientWin32
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Set_Thread_Context
#define _VWIN32_Set_Thread_Context DEBUG__VWIN32_Set_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Set_Thread_Context
#define VWIN32_Set_Thread_Context DEBUG__VWIN32_Set_Thread_Context
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_CopyMem
#define _VWIN32_CopyMem DEBUG__VWIN32_CopyMem
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_CopyMem
#define VWIN32_CopyMem DEBUG__VWIN32_CopyMem
#endif

#ifdef WIN40_OR_LATER
#undef  _VWIN32_Npx_Exception
#define _VWIN32_Npx_Exception DEBUG__VWIN32_Npx_Exception
#endif

#ifdef WIN40_OR_LATER
#undef  VWIN32_Npx_Exception
#define VWIN32_Npx_Exception DEBUG__VWIN32_Npx_Exception
#endif

#ifdef WIN40_OR_LATER
#undef  PCCARD_Get_Version
#define PCCARD_Get_Version DEBUG_PCCARD_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PCCARD_Card_Services
#define PCCARD_Card_Services DEBUG_PCCARD_Card_Services
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Get_Version
#define IOS_Get_Version DEBUG_IOS_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_BD_Register_Device
#define IOS_BD_Register_Device DEBUG_IOS_BD_Register_Device
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Find_Int13_Drive
#define IOS_Find_Int13_Drive DEBUG_IOS_Find_Int13_Drive
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Get_Device_List
#define IOS_Get_Device_List DEBUG_IOS_Get_Device_List
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_SendCommand
#define IOS_SendCommand DEBUG_IOS_SendCommand
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Register
#define IOS_Register DEBUG_IOS_Register
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Requestor_Service
#define IOS_Requestor_Service DEBUG_IOS_Requestor_Service
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Exclusive_Access
#define IOS_Exclusive_Access DEBUG_IOS_Exclusive_Access
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Send_Next_Command
#define IOS_Send_Next_Command DEBUG_IOS_Send_Next_Command
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Set_Async_Time_Out
#define IOS_Set_Async_Time_Out DEBUG_IOS_Set_Async_Time_Out
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_Signal_Semaphore_No_Switch
#define IOS_Signal_Semaphore_No_Switch DEBUG_IOS_Signal_Semaphore_No_Switch
#endif

#ifdef WIN40_OR_LATER
#undef  IOSIdleStatus
#define IOSIdleStatus DEBUG_IOSIdleStatus
#endif

#ifdef WIN40_OR_LATER
#undef  IOSMapIORSToI24
#define IOSMapIORSToI24 DEBUG_IOSMapIORSToI24
#endif

#ifdef WIN40_OR_LATER
#undef  IOSMapIORSToI21
#define IOSMapIORSToI21 DEBUG_IOSMapIORSToI21
#endif

#ifdef WIN40_OR_LATER
#undef  IOS_BD_Command_Complete
#define IOS_BD_Command_Complete DEBUG_IOS_BD_Command_Complete
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_Register_Device_Driver
#define MMDEVLDR_Register_Device_Driver DEBUG_MMDEVLDR_Register_Device_Driver
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_SetDevicePresence
#define _MMDEVLDR_SetDevicePresence DEBUG__MMDEVLDR_SetDevicePresence
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_SetDevicePresence
#define MMDEVLDR_SetDevicePresence DEBUG__MMDEVLDR_SetDevicePresence
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_SetEnvironmentString
#define _MMDEVLDR_SetEnvironmentString DEBUG__MMDEVLDR_SetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_SetEnvironmentString
#define MMDEVLDR_SetEnvironmentString DEBUG__MMDEVLDR_SetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_GetEnvironmentString
#define _MMDEVLDR_GetEnvironmentString DEBUG__MMDEVLDR_GetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_GetEnvironmentString
#define MMDEVLDR_GetEnvironmentString DEBUG__MMDEVLDR_GetEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_RemoveEnvironmentString
#define _MMDEVLDR_RemoveEnvironmentString DEBUG__MMDEVLDR_RemoveEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_RemoveEnvironmentString
#define MMDEVLDR_RemoveEnvironmentString DEBUG__MMDEVLDR_RemoveEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  _MMDEVLDR_AddEnvironmentString
#define _MMDEVLDR_AddEnvironmentString DEBUG__MMDEVLDR_AddEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  MMDEVLDR_AddEnvironmentString
#define MMDEVLDR_AddEnvironmentString DEBUG__MMDEVLDR_AddEnvironmentString
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Get_Virtualization_Count
#define VPICD_Get_Virtualization_Count DEBUG_VPICD_Get_Virtualization_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Phys_Mask_Channel
#define VDMAD_Phys_Mask_Channel DEBUG_VDMAD_Phys_Mask_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Phys_Unmask_Channel
#define VDMAD_Phys_Unmask_Channel DEBUG_VDMAD_Phys_Unmask_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Unvirtualize_Channel
#define VDMAD_Unvirtualize_Channel DEBUG_VDMAD_Unvirtualize_Channel
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_IO_Address
#define VDMAD_Set_IO_Address DEBUG_VDMAD_Set_IO_Address
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Convert_Hex_Binary
#define Debug_Convert_Hex_Binary DEBUG_Debug_Convert_Hex_Binary
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Convert_Hex_Decimal
#define Debug_Convert_Hex_Decimal DEBUG_Debug_Convert_Hex_Decimal
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Flags_Service
#define _Debug_Flags_Service DEBUG__Debug_Flags_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Flags_Service
#define Debug_Flags_Service DEBUG__Debug_Flags_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Cur_VM
#define Debug_Test_Cur_VM DEBUG_Debug_Test_Cur_VM
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Test_Valid_Handle
#define Debug_Test_Valid_Handle DEBUG_Debug_Test_Valid_Handle
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_FindBlock
#define VCache_FindBlock DEBUG_VCache_FindBlock
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_FreeBlock
#define VCache_FreeBlock DEBUG_VCache_FreeBlock
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_GetSize
#define VCache_GetSize DEBUG_VCache_GetSize
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_GetStats
#define VCache_GetStats DEBUG_VCache_GetStats
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Get_Version
#define VCache_Get_Version DEBUG_VCache_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Hold
#define VCache_Hold DEBUG_VCache_Hold
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_MakeMRU
#define VCache_MakeMRU DEBUG_VCache_MakeMRU
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Register
#define VCache_Register DEBUG_VCache_Register
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_SwapBuffers
#define VCache_SwapBuffers DEBUG_VCache_SwapBuffers
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_TestHold
#define VCache_TestHold DEBUG_VCache_TestHold
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Unhold
#define VCache_Unhold DEBUG_VCache_Unhold
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_CloseLookupCache
#define _VCache_CloseLookupCache DEBUG__VCache_CloseLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CloseLookupCache
#define VCache_CloseLookupCache DEBUG__VCache_CloseLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_CreateLookupCache
#define _VCache_CreateLookupCache DEBUG__VCache_CreateLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CreateLookupCache
#define VCache_CreateLookupCache DEBUG__VCache_CreateLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_DeleteLookupCache
#define _VCache_DeleteLookupCache DEBUG__VCache_DeleteLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_DeleteLookupCache
#define VCache_DeleteLookupCache DEBUG__VCache_DeleteLookupCache
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_Lookup
#define _VCache_Lookup DEBUG__VCache_Lookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Lookup
#define VCache_Lookup DEBUG__VCache_Lookup
#endif

#ifdef WIN40_OR_LATER
#undef  _VCache_UpdateLookup
#define _VCache_UpdateLookup DEBUG__VCache_UpdateLookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_UpdateLookup
#define VCache_UpdateLookup DEBUG__VCache_UpdateLookup
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_RelinquishPage
#define VCache_RelinquishPage DEBUG_VCache_RelinquishPage
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_UseThisPage
#define VCache_UseThisPage DEBUG_VCache_UseThisPage
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_RecalcSums
#define VCache_RecalcSums DEBUG_VCache_RecalcSums
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_TestHandle
#define VCache_TestHandle DEBUG_VCache_TestHandle
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_VerifySums
#define VCache_VerifySums DEBUG_VCache_VerifySums
#endif

#ifdef WIN40_OR_LATER
#undef  DOSMGR_Alloc_Local_Sys_VM_Mem
#define DOSMGR_Alloc_Local_Sys_VM_Mem DEBUG_DOSMGR_Alloc_Local_Sys_VM_Mem
#endif

#ifdef WIN40_OR_LATER
#undef  DOSMGR_Init_UMB_Area
#define DOSMGR_Init_UMB_Area DEBUG_DOSMGR_Init_UMB_Area
#endif

#ifdef WIN40_OR_LATER
#undef  _GetRegistryKey
#define _GetRegistryKey DEBUG__GetRegistryKey
#endif

#ifdef WIN40_OR_LATER
#undef  GetRegistryKey
#define GetRegistryKey DEBUG__GetRegistryKey
#endif

#ifdef WIN40_OR_LATER
#undef  Get_Nest_Exec_Status
#define Get_Nest_Exec_Status DEBUG_Get_Nest_Exec_Status
#endif

#ifdef WIN40_OR_LATER
#undef  _AddReclaimableItem
#define _AddReclaimableItem DEBUG__AddReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  AddReclaimableItem
#define AddReclaimableItem DEBUG__AddReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  _AtEventTime
#define _AtEventTime DEBUG__AtEventTime
#endif

#ifdef WIN40_OR_LATER
#undef  AtEventTime
#define AtEventTime DEBUG__AtEventTime
#endif

#ifdef WIN40_OR_LATER
#undef  Close_Boot_Log
#define Close_Boot_Log DEBUG_Close_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  EnableDisable_Boot_Log
#define EnableDisable_Boot_Log DEBUG_EnableDisable_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  _EnumReclaimableItem
#define _EnumReclaimableItem DEBUG__EnumReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  EnumReclaimableItem
#define EnumReclaimableItem DEBUG__EnumReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  Open_Boot_Log
#define Open_Boot_Log DEBUG_Open_Boot_Log
#endif

#ifdef WIN40_OR_LATER
#undef  _SetReclaimableItem
#define _SetReclaimableItem DEBUG__SetReclaimableItem
#endif

#ifdef WIN40_OR_LATER
#undef  SetReclaimableItem
#define SetReclaimableItem DEBUG__SetReclaimableItem
#endif

#undef  Hook_Device_Service_Ex
#define Hook_Device_Service_Ex DEBUG_Hook_Device_Service_Ex

#ifdef WIN40_OR_LATER
#undef  PERF_Get_Version
#define PERF_Get_Version DEBUG_PERF_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Register
#define PERF_Server_Register DEBUG_PERF_Server_Register
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Deregister
#define PERF_Server_Deregister DEBUG_PERF_Server_Deregister
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Add_Stat
#define PERF_Server_Add_Stat DEBUG_PERF_Server_Add_Stat
#endif

#ifdef WIN40_OR_LATER
#undef  PERF_Server_Remove_Stat
#define PERF_Server_Remove_Stat DEBUG_PERF_Server_Remove_Stat
#endif

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service_Ex
#define Unhook_Device_Service_Ex DEBUG_Unhook_Device_Service_Ex
#endif

#undef  Call_Previous_Hook_Proc
#define Call_Previous_Hook_Proc DEBUG_Call_Previous_Hook_Proc

#ifdef WIN40_OR_LATER
#undef  dummy_935
#define dummy_935 DEBUG_dummy_935
#endif

#undef  Hook_Device_Service_C
#define Hook_Device_Service_C DEBUG_Hook_Device_Service_C

#ifdef WIN40_OR_LATER
#undef  Unhook_Device_Service_C
#define Unhook_Device_Service_C DEBUG_Unhook_Device_Service_C
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Add_ID
#define _CONFIGMG_Add_ID DEBUG__CONFIGMG_Add_ID
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Add_ID
#define CONFIGMG_Add_ID DEBUG__CONFIGMG_Add_ID
#endif

#ifdef WIN40_OR_LATER
#undef  _CONFIGMG_Register_Enumerator_Function
#define _CONFIGMG_Register_Enumerator_Function DEBUG__CONFIGMG_Register_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  CONFIGMG_Register_Enumerator_Function
#define CONFIGMG_Register_Enumerator_Function DEBUG__CONFIGMG_Register_Enumerator_Function
#endif

#ifdef WIN40_OR_LATER
#undef  SHELL_Update_User_Activity
#define SHELL_Update_User_Activity DEBUG_SHELL_Update_User_Activity
#endif

#undef  VDMAD_Unlock_DMA_Region_No_Dirty
#define VDMAD_Unlock_DMA_Region_No_Dirty DEBUG_VDMAD_Unlock_DMA_Region_No_Dirty

#ifdef WIN40_OR_LATER
#undef  VWIN32_GetCurrentDirectory
#define VWIN32_GetCurrentDirectory DEBUG_VWIN32_GetCurrentDirectory
#endif

#ifdef WIN40_OR_LATER
#undef  VXDLDR_UnloadMe
#define VXDLDR_UnloadMe DEBUG_VXDLDR_UnloadMe
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_InitCompleted
#define _PELDR_InitCompleted DEBUG__PELDR_InitCompleted
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_InitCompleted
#define PELDR_InitCompleted DEBUG__PELDR_InitCompleted
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_AddExportTable
#define _PELDR_AddExportTable DEBUG__PELDR_AddExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_AddExportTable
#define PELDR_AddExportTable DEBUG__PELDR_AddExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_FreeModule
#define _PELDR_FreeModule DEBUG__PELDR_FreeModule
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_FreeModule
#define PELDR_FreeModule DEBUG__PELDR_FreeModule
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetEntryPoint
#define _PELDR_GetEntryPoint DEBUG__PELDR_GetEntryPoint
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetEntryPoint
#define PELDR_GetEntryPoint DEBUG__PELDR_GetEntryPoint
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetModuleHandle
#define _PELDR_GetModuleHandle DEBUG__PELDR_GetModuleHandle
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetModuleHandle
#define PELDR_GetModuleHandle DEBUG__PELDR_GetModuleHandle
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetModuleUsage
#define _PELDR_GetModuleUsage DEBUG__PELDR_GetModuleUsage
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetModuleUsage
#define PELDR_GetModuleUsage DEBUG__PELDR_GetModuleUsage
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_GetProcAddress
#define _PELDR_GetProcAddress DEBUG__PELDR_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_GetProcAddress
#define PELDR_GetProcAddress DEBUG__PELDR_GetProcAddress
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_LoadModule
#define _PELDR_LoadModule DEBUG__PELDR_LoadModule
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_LoadModule
#define PELDR_LoadModule DEBUG__PELDR_LoadModule
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_LoadModuleEx
#define _PELDR_LoadModuleEx DEBUG__PELDR_LoadModuleEx
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_LoadModuleEx
#define PELDR_LoadModuleEx DEBUG__PELDR_LoadModuleEx
#endif

#ifdef WIN40_OR_LATER
#undef  _PELDR_RemoveExportTable
#define _PELDR_RemoveExportTable DEBUG__PELDR_RemoveExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  PELDR_RemoveExportTable
#define PELDR_RemoveExportTable DEBUG__PELDR_RemoveExportTable
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_AdjustMinimum
#define VCache_AdjustMinimum DEBUG_VCache_AdjustMinimum
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_CheckAvail
#define VCache_CheckAvail DEBUG_VCache_CheckAvail
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Deregister
#define VCache_Deregister DEBUG_VCache_Deregister
#endif

#ifdef WIN40_OR_LATER
#undef  VCache_Enum
#define VCache_Enum DEBUG_VCache_Enum
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Enable_Keyboard
#define VKD_Enable_Keyboard DEBUG_VKD_Enable_Keyboard
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Disable_Keyboard
#define VKD_Disable_Keyboard DEBUG_VKD_Disable_Keyboard
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Get_Shift_State
#define VKD_Get_Shift_State DEBUG_VKD_Get_Shift_State
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Put_Byte
#define VKD_Put_Byte DEBUG_VKD_Put_Byte
#endif

#ifdef WIN40_OR_LATER
#undef  VKD_Set_Shift_State
#define VKD_Set_Shift_State DEBUG_VKD_Set_Shift_State
#endif

#undef  VCD_InitThunk_Control
#define VCD_InitThunk_Control DEBUG_VCD_InitThunk_Control

#undef  VCD_InitThunk_Irq
#define VCD_InitThunk_Irq DEBUG_VCD_InitThunk_Irq

#undef  VCD_InitThunk_Io
#define VCD_InitThunk_Io DEBUG_VCD_InitThunk_Io

#undef  VCD_Acquire_Port
#define VCD_Acquire_Port DEBUG_VCD_Acquire_Port

#undef  VCD_Free_Port_Windows_Style
#define VCD_Free_Port_Windows_Style DEBUG_VCD_Free_Port_Windows_Style

#undef  VCD_Steal_Port_Windows_Style
#define VCD_Steal_Port_Windows_Style DEBUG_VCD_Steal_Port_Windows_Style

#ifdef WIN40_OR_LATER
#undef  VCD_Find_COM_Index
#define VCD_Find_COM_Index DEBUG_VCD_Find_COM_Index
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Set_Port_Global_Special
#define VCD_Set_Port_Global_Special DEBUG_VCD_Set_Port_Global_Special
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Virtualize_Port_Dynamic
#define VCD_Virtualize_Port_Dynamic DEBUG_VCD_Virtualize_Port_Dynamic
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Unvirtualize_Port_Dynamic
#define VCD_Unvirtualize_Port_Dynamic DEBUG_VCD_Unvirtualize_Port_Dynamic
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Acquire_Port_Windows_Style
#define VCD_Acquire_Port_Windows_Style DEBUG_VCD_Acquire_Port_Windows_Style
#endif

#ifdef WIN40_OR_LATER
#undef  VCD_Free_Port
#define VCD_Free_Port DEBUG_VCD_Free_Port
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Check_Update_Soon
#define VDD_Check_Update_Soon DEBUG_VDD_Check_Update_Soon
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_GrabRtn
#define VDD_Get_GrabRtn DEBUG_VDD_Get_GrabRtn
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Hide_Cursor
#define VDD_Hide_Cursor DEBUG_VDD_Hide_Cursor
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Msg_SetCursPos
#define VDD_Msg_SetCursPos DEBUG_VDD_Msg_SetCursPos
#endif

#ifdef WIN40_OR_LATER
#undef  VMD_Enable_Disable_Mouse_Events
#define VMD_Enable_Disable_Mouse_Events DEBUG_VMD_Enable_Disable_Mouse_Events
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Init_File
#define PageFile_Init_File DEBUG_PageFile_Init_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Get_Version
#define PageFile_Get_Version DEBUG_PageFile_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Clean_Up
#define PageFile_Clean_Up DEBUG_PageFile_Clean_Up
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Grow_File
#define PageFile_Grow_File DEBUG_PageFile_Grow_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Read_Or_Write
#define PageFile_Read_Or_Write DEBUG_PageFile_Read_Or_Write
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Cancel
#define PageFile_Cancel DEBUG_PageFile_Cancel
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Test_IO_Valid
#define PageFile_Test_IO_Valid DEBUG_PageFile_Test_IO_Valid
#endif

#ifdef WIN40_OR_LATER
#undef  PageFile_Get_Size_Info
#define PageFile_Get_Size_Info DEBUG_PageFile_Get_Size_Info
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Get_Version
#define PageSwap_Get_Version DEBUG_PageSwap_Get_Version
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Test_IO_Valid
#define PageSwap_Test_IO_Valid DEBUG_PageSwap_Test_IO_Valid
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Read_Or_Write
#define PageSwap_Read_Or_Write DEBUG_PageSwap_Read_Or_Write
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Grow_File
#define PageSwap_Grow_File DEBUG_PageSwap_Grow_File
#endif

#ifdef WIN40_OR_LATER
#undef  PageSwap_Init_File
#define PageSwap_Init_File DEBUG_PageSwap_Init_File
#endif

#undef  Allocate_PM_App_CB_Area
#define Allocate_PM_App_CB_Area DEBUG_Allocate_PM_App_CB_Area

#ifdef WIN40_OR_LATER
#undef  _Call_On_My_Stack
#define _Call_On_My_Stack DEBUG__Call_On_My_Stack
#endif

#ifdef WIN40_OR_LATER
#undef  Call_On_My_Stack
#define Call_On_My_Stack DEBUG__Call_On_My_Stack
#endif

#ifdef WIN40_OR_LATER
#undef  _Debug_Printf_Service
#define _Debug_Printf_Service DEBUG__Debug_Printf_Service
#endif

#ifdef WIN40_OR_LATER
#undef  Debug_Printf_Service
#define Debug_Printf_Service DEBUG__Debug_Printf_Service
#endif

#ifdef WIN40_OR_LATER
#undef  _strupr
#define _strupr DEBUG__strupr
#endif

#ifdef WIN40_OR_LATER
#undef  strupr
#define strupr DEBUG__strupr
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Do_Physical_IO
#define VDD_Do_Physical_IO DEBUG_VDD_Do_Physical_IO
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_DISPLAYINFO
#define VDD_Get_DISPLAYINFO DEBUG_VDD_Get_DISPLAYINFO
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_Mini_Dispatch_Table
#define VDD_Get_Mini_Dispatch_Table DEBUG_VDD_Get_Mini_Dispatch_Table
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_Special_VM_IDs
#define VDD_Get_Special_VM_IDs DEBUG_VDD_Get_Special_VM_IDs
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Get_VM_Info
#define VDD_Get_VM_Info DEBUG_VDD_Get_VM_Info
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Register_Extra_Screen_Selector
#define VDD_Register_Extra_Screen_Selector DEBUG_VDD_Register_Extra_Screen_Selector
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Register_Virtual_Port
#define VDD_Register_Virtual_Port DEBUG_VDD_Register_Virtual_Port
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Set_Sleep_Flag_Addr
#define VDD_Set_Sleep_Flag_Addr DEBUG_VDD_Set_Sleep_Flag_Addr
#endif

#ifdef WIN40_OR_LATER
#undef  VDD_Takeover_VGA_Port
#define VDD_Takeover_VGA_Port DEBUG_VDD_Takeover_VGA_Port
#endif

#undef  Init_Generic_Thunk
#define Init_Generic_Thunk DEBUG_Init_Generic_Thunk

#ifdef WIN40_OR_LATER
#undef  _VCOMM_Enumerate_DevNodes
#define _VCOMM_Enumerate_DevNodes DEBUG__VCOMM_Enumerate_DevNodes
#endif

#ifdef WIN40_OR_LATER
#undef  VCOMM_Enumerate_DevNodes
#define VCOMM_Enumerate_DevNodes DEBUG__VCOMM_Enumerate_DevNodes
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Virt_Count
#define VDMAD_Get_Virt_Count DEBUG_VDMAD_Get_Virt_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_Channel_Callbacks
#define VDMAD_Set_Channel_Callbacks DEBUG_VDMAD_Set_Channel_Callbacks
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Phys_Status
#define VDMAD_Get_Phys_Status DEBUG_VDMAD_Get_Phys_Status
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Phys_Count
#define VDMAD_Get_Phys_Count DEBUG_VDMAD_Get_Phys_Count
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Get_Max_Phys_Page
#define VDMAD_Get_Max_Phys_Page DEBUG_VDMAD_Get_Max_Phys_Page
#endif

#ifdef WIN40_OR_LATER
#undef  VDMAD_Set_Virt_Count
#define VDMAD_Set_Virt_Count DEBUG_VDMAD_Set_Virt_Count
#endif

#ifdef WIN40_OR_LATER
#undef  _EnterMustComplete
#define _EnterMustComplete DEBUG__EnterMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  EnterMustComplete
#define EnterMustComplete DEBUG__EnterMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _LeaveMustComplete
#define _LeaveMustComplete DEBUG__LeaveMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  LeaveMustComplete
#define LeaveMustComplete DEBUG__LeaveMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  _ResumeExecMustComplete
#define _ResumeExecMustComplete DEBUG__ResumeExecMustComplete
#endif

#ifdef WIN40_OR_LATER
#undef  ResumeExecMustComplete
#define ResumeExecMustComplete DEBUG__ResumeExecMustComplete
#endif

#undef  VTDI_Get_Version
#define VTDI_Get_Version DEBUG_VTDI_Get_Version

#undef  VTDI_Get_Info
#define VTDI_Get_Info DEBUG_VTDI_Get_Info

#undef  VTDI_Start_Timer
#define VTDI_Start_Timer DEBUG_VTDI_Start_Timer

#undef  VTDI_Stop_Timer
#define VTDI_Stop_Timer DEBUG_VTDI_Stop_Timer

#undef  dummy_1034
#define dummy_1034 DEBUG_dummy_1034

#undef  dummy_1035
#define dummy_1035 DEBUG_dummy_1035

#undef  VTDI_Schedule_Event
#define VTDI_Schedule_Event DEBUG_VTDI_Schedule_Event

#undef  VTDI_Cancel_Event
#define VTDI_Cancel_Event DEBUG_VTDI_Cancel_Event

#undef  VTDI_Block
#define VTDI_Block DEBUG_VTDI_Block

#undef  VTDI_Signal
#define VTDI_Signal DEBUG_VTDI_Signal

#undef  VTDI_Register
#define VTDI_Register DEBUG_VTDI_Register

#ifdef WIN40_OR_LATER
#undef  VTDI_Unload
#define VTDI_Unload DEBUG_VTDI_Unload
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Initialize
#define VTDI_Initialize DEBUG_VTDI_Initialize
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Register_UnloadProc
#define VTDI_Register_UnloadProc DEBUG_VTDI_Register_UnloadProc
#endif

#ifdef WIN40_OR_LATER
#undef  VTDI_Register_LoadProc
#define VTDI_Register_LoadProc DEBUG_VTDI_Register_LoadProc
#endif

#ifdef WIN40_OR_LATER
#undef  VPICD_Thunk_HWInt_Ex
#define VPICD_Thunk_HWInt_Ex DEBUG_VPICD_Thunk_HWInt_Ex
#endif

#ifdef WIN40_OR_LATER
#undef  Call_Restricted_Event_Ex
#define Call_Restricted_Event_Ex DEBUG_Call_Restricted_Event_Ex
#endif

#ifdef WIN41_OR_LATER
#undef  NTKERN_Get_Version
#define NTKERN_Get_Version DEBUG_NTKERN_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernCreateFile
#define NtKernCreateFile DEBUG_NtKernCreateFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernClose
#define NtKernClose DEBUG_NtKernClose
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernReadFile
#define NtKernReadFile DEBUG_NtKernReadFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernWriteFile
#define NtKernWriteFile DEBUG_NtKernWriteFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernDeviceIoControl
#define NtKernDeviceIoControl DEBUG_NtKernDeviceIoControl
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernGetWorkerThread
#define NtKernGetWorkerThread DEBUG_NtKernGetWorkerThread
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernLoadDriver
#define NtKernLoadDriver DEBUG_NtKernLoadDriver
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernQueueWorkItem
#define NtKernQueueWorkItem DEBUG_NtKernQueueWorkItem
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernPhysicalDeviceObjectToDevNode
#define NtKernPhysicalDeviceObjectToDevNode DEBUG_NtKernPhysicalDeviceObjectToDevNode
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernSetPhysicalCacheTypeRange
#define NtKernSetPhysicalCacheTypeRange DEBUG_NtKernSetPhysicalCacheTypeRange
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernWin9XLoadDriver
#define NtKernWin9XLoadDriver DEBUG_NtKernWin9XLoadDriver
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernCancelIoFile
#define NtKernCancelIoFile DEBUG_NtKernCancelIoFile
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernGetVPICDHandleFromInterruptObj
#define NtKernGetVPICDHandleFromInterruptObj DEBUG_NtKernGetVPICDHandleFromInterruptObj
#endif

#ifdef WIN41_OR_LATER
#undef  NtKernInternalDeviceIoControl
#define NtKernInternalDeviceIoControl DEBUG_NtKernInternalDeviceIoControl
#endif

#ifdef WIN41_OR_LATER
#undef  _KeSaveFloatingPointState
#define _KeSaveFloatingPointState DEBUG__KeSaveFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  KeSaveFloatingPointState
#define KeSaveFloatingPointState DEBUG__KeSaveFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  _KeRestoreFloatingPointState
#define _KeRestoreFloatingPointState DEBUG__KeRestoreFloatingPointState
#endif

#ifdef WIN41_OR_LATER
#undef  KeRestoreFloatingPointState
#define KeRestoreFloatingPointState DEBUG__KeRestoreFloatingPointState
#endif

#ifdef WIN40_OR_LATER
#undef  VTD_Delay
#define VTD_Delay DEBUG_VTD_Delay
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Get_Version
#define _PCI_Get_Version DEBUG__PCI_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Get_Version
#define PCI_Get_Version DEBUG__PCI_Get_Version
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Read_Config
#define _PCI_Read_Config DEBUG__PCI_Read_Config
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Read_Config
#define PCI_Read_Config DEBUG__PCI_Read_Config
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Write_Config
#define _PCI_Write_Config DEBUG__PCI_Write_Config
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Write_Config
#define PCI_Write_Config DEBUG__PCI_Write_Config
#endif

#ifdef WIN41_OR_LATER
#undef  _PCI_Lock_Unlock
#define _PCI_Lock_Unlock DEBUG__PCI_Lock_Unlock
#endif

#ifdef WIN41_OR_LATER
#undef  PCI_Lock_Unlock
#define PCI_Lock_Unlock DEBUG__PCI_Lock_Unlock
#endif

#ifdef WIN41_OR_LATER
#undef  _PCCARD_Access_CIS_Memory
#define _PCCARD_Access_CIS_Memory DEBUG__PCCARD_Access_CIS_Memory
#endif

#ifdef WIN41_OR_LATER
#undef  PCCARD_Access_CIS_Memory
#define PCCARD_Access_CIS_Memory DEBUG__PCCARD_Access_CIS_Memory
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Get_DevNode_PowerCapabilities
#define _CONFIGMG_Get_DevNode_PowerCapabilities DEBUG__CONFIGMG_Get_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Get_DevNode_PowerCapabilities
#define CONFIGMG_Get_DevNode_PowerCapabilities DEBUG__CONFIGMG_Get_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Set_DevNode_PowerCapabilities
#define _CONFIGMG_Set_DevNode_PowerCapabilities DEBUG__CONFIGMG_Set_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Set_DevNode_PowerCapabilities
#define CONFIGMG_Set_DevNode_PowerCapabilities DEBUG__CONFIGMG_Set_DevNode_PowerCapabilities
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Get_DevNode_PowerState
#define _CONFIGMG_Get_DevNode_PowerState DEBUG__CONFIGMG_Get_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Get_DevNode_PowerState
#define CONFIGMG_Get_DevNode_PowerState DEBUG__CONFIGMG_Get_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Set_DevNode_PowerState
#define _CONFIGMG_Set_DevNode_PowerState DEBUG__CONFIGMG_Set_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Set_DevNode_PowerState
#define CONFIGMG_Set_DevNode_PowerState DEBUG__CONFIGMG_Set_DevNode_PowerState
#endif

#ifdef WIN41_OR_LATER
#undef  _CONFIGMG_Register_DevNode_For_Idle_Detection
#define _CONFIGMG_Register_DevNode_For_Idle_Detection DEBUG__CONFIGMG_Register_DevNode_For_Idle_Detection
#endif

#ifdef WIN41_OR_LATER
#undef  CONFIGMG_Register_DevNode_For_Idle_Detection
#define CONFIGMG_Register_DevNode_For_Idle_Detection DEBUG__CONFIGMG_Register_DevNode_For_Idle_Detection
#endif

