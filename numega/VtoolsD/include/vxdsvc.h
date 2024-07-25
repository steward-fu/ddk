//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

DWORD __cdecl LOCK__XchgFreePhysReg (DWORD LinPgNum, CONST VOID * PhysAddr, DWORD  Flags) ;
DWORD __cdecl INIT__XchgFreePhysReg (DWORD LinPgNum, CONST VOID * PhysAddr, DWORD  Flags) ;
DWORD __cdecl DEBUG__XchgFreePhysReg (DWORD LinPgNum, CONST VOID * PhysAddr, DWORD  Flags) ;
DWORD __cdecl STATIC__XchgFreePhysReg (DWORD LinPgNum, CONST VOID * PhysAddr, DWORD  Flags) ;
DWORD __cdecl PAGEABLE__XchgFreePhysReg (DWORD LinPgNum, CONST VOID * PhysAddr, DWORD  Flags) ;

VOID __stdcall LOCK_Wake_Up_VM (VMHANDLE hVM) ;
VOID __stdcall INIT_Wake_Up_VM (VMHANDLE hVM) ;
VOID __stdcall DEBUG_Wake_Up_VM (VMHANDLE hVM) ;
VOID __stdcall STATIC_Wake_Up_VM (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_Wake_Up_VM (VMHANDLE hVM) ;

VOID __stdcall LOCK_Wait_Semaphore (SEMHANDLE hSem, DWORD Flags) ;
VOID __stdcall INIT_Wait_Semaphore (SEMHANDLE hSem, DWORD Flags) ;
VOID __stdcall DEBUG_Wait_Semaphore (SEMHANDLE hSem, DWORD Flags) ;
VOID __stdcall STATIC_Wait_Semaphore (SEMHANDLE hSem, DWORD Flags) ;
VOID __stdcall PAGEABLE_Wait_Semaphore (SEMHANDLE hSem, DWORD Flags) ;

BOOL __stdcall LOCK_Validate_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall INIT_Validate_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall DEBUG_Validate_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall STATIC_Validate_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall PAGEABLE_Validate_VM_Handle (VMHANDLE hVM) ;

VOID __stdcall LOCK_Validate_Client_Ptr (CONST CLIENT_STRUCT * pRegs) ;
VOID __stdcall INIT_Validate_Client_Ptr (CONST CLIENT_STRUCT * pRegs) ;
VOID __stdcall DEBUG_Validate_Client_Ptr (CONST CLIENT_STRUCT * pRegs) ;
VOID __stdcall STATIC_Validate_Client_Ptr (CONST CLIENT_STRUCT * pRegs) ;
VOID __stdcall PAGEABLE_Validate_Client_Ptr (CONST CLIENT_STRUCT * pRegs) ;

VOID __stdcall LOCK_Update_System_Clock (DWORD Time) ;
VOID __stdcall INIT_Update_System_Clock (DWORD Time) ;
VOID __stdcall DEBUG_Update_System_Clock (DWORD Time) ;
VOID __stdcall STATIC_Update_System_Clock (DWORD Time) ;
VOID __stdcall PAGEABLE_Update_System_Clock (DWORD Time) ;

BOOL __cdecl LOCK__UnmapFreePhysReg (DWORD LinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl INIT__UnmapFreePhysReg (DWORD LinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl DEBUG__UnmapFreePhysReg (DWORD LinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl STATIC__UnmapFreePhysReg (DWORD LinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl PAGEABLE__UnmapFreePhysReg (DWORD LinPgNum, DWORD nPages, DWORD Flags) ;

BOOL __stdcall LOCK_Unhook_Invalid_Page_Fault (PIPF_HANDLER Hookproc, CONST IPF_THUNK *  pThunk) ;
BOOL __stdcall INIT_Unhook_Invalid_Page_Fault (PIPF_HANDLER Hookproc, CONST IPF_THUNK *  pThunk) ;
BOOL __stdcall DEBUG_Unhook_Invalid_Page_Fault (PIPF_HANDLER Hookproc, CONST IPF_THUNK *  pThunk) ;
BOOL __stdcall STATIC_Unhook_Invalid_Page_Fault (PIPF_HANDLER Hookproc, CONST IPF_THUNK *  pThunk) ;
BOOL __stdcall PAGEABLE_Unhook_Invalid_Page_Fault (PIPF_HANDLER Hookproc, CONST IPF_THUNK *  pThunk) ;

BOOL __stdcall LOCK_Test_Sys_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall INIT_Test_Sys_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall DEBUG_Test_Sys_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall STATIC_Test_Sys_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall PAGEABLE_Test_Sys_VM_Handle (VMHANDLE hVM) ;

#define Test_Reenter() VMMcall(Test_Reenter)
MEMSTATUS __cdecl LOCK__TestGlobalV86Mem (CONST VOID * VMLinAddr, DWORD nBytes, DWORD Flags) ;
MEMSTATUS __cdecl INIT__TestGlobalV86Mem (CONST VOID * VMLinAddr, DWORD nBytes, DWORD Flags) ;
MEMSTATUS __cdecl DEBUG__TestGlobalV86Mem (CONST VOID * VMLinAddr, DWORD nBytes, DWORD Flags) ;
MEMSTATUS __cdecl STATIC__TestGlobalV86Mem (CONST VOID * VMLinAddr, DWORD nBytes, DWORD Flags) ;
MEMSTATUS __cdecl PAGEABLE__TestGlobalV86Mem (CONST VOID * VMLinAddr, DWORD nBytes, DWORD Flags) ;

BOOL __stdcall LOCK_Test_Debug_Installed (void) ;
BOOL __stdcall INIT_Test_Debug_Installed (void) ;
BOOL __stdcall DEBUG_Test_Debug_Installed (void) ;
BOOL __stdcall STATIC_Test_Debug_Installed (void) ;
BOOL __stdcall PAGEABLE_Test_Debug_Installed (void) ;

BOOL __stdcall LOCK_Test_DBCS_Lead_Byte (DWORD Value) ;
BOOL __stdcall INIT_Test_DBCS_Lead_Byte (DWORD Value) ;
BOOL __stdcall DEBUG_Test_DBCS_Lead_Byte (DWORD Value) ;
BOOL __stdcall STATIC_Test_DBCS_Lead_Byte (DWORD Value) ;
BOOL __stdcall PAGEABLE_Test_DBCS_Lead_Byte (DWORD Value) ;

BOOL __stdcall LOCK_Test_Cur_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall INIT_Test_Cur_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall DEBUG_Test_Cur_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall STATIC_Test_Cur_VM_Handle (VMHANDLE hVM) ;
BOOL __stdcall PAGEABLE_Test_Cur_VM_Handle (VMHANDLE hVM) ;

BOOL __stdcall LOCK_System_Control (DWORD Message, VMHANDLE hVM, DWORD Param1, DWORD Param2, DWORD Param3) ;
BOOL __stdcall INIT_System_Control (DWORD Message, VMHANDLE hVM, DWORD Param1, DWORD Param2, DWORD Param3) ;
BOOL __stdcall DEBUG_System_Control (DWORD Message, VMHANDLE hVM, DWORD Param1, DWORD Param2, DWORD Param3) ;
BOOL __stdcall STATIC_System_Control (DWORD Message, VMHANDLE hVM, DWORD Param1, DWORD Param2, DWORD Param3) ;
BOOL __stdcall PAGEABLE_System_Control (DWORD Message, VMHANDLE hVM, DWORD Param1, DWORD Param2, DWORD Param3) ;

BOOL __stdcall LOCK_Suspend_VM (VMHANDLE hVM) ;
BOOL __stdcall INIT_Suspend_VM (VMHANDLE hVM) ;
BOOL __stdcall DEBUG_Suspend_VM (VMHANDLE hVM) ;
BOOL __stdcall STATIC_Suspend_VM (VMHANDLE hVM) ;
BOOL __stdcall PAGEABLE_Suspend_VM (VMHANDLE hVM) ;

VOID __stdcall LOCK_Simulate_Push (DWORD Value) ;
VOID __stdcall INIT_Simulate_Push (DWORD Value) ;
VOID __stdcall DEBUG_Simulate_Push (DWORD Value) ;
VOID __stdcall STATIC_Simulate_Push (DWORD Value) ;
VOID __stdcall PAGEABLE_Simulate_Push (DWORD Value) ;

DWORD __cdecl LOCK_Simulate_Pop (void) ;
DWORD __cdecl INIT_Simulate_Pop (void) ;
DWORD __cdecl DEBUG_Simulate_Pop (void) ;
DWORD __cdecl STATIC_Simulate_Pop (void) ;
DWORD __cdecl PAGEABLE_Simulate_Pop (void) ;

#define Simulate_Iret() VMMcall(Simulate_Iret)
VOID __stdcall LOCK_Simulate_Int (DWORD IntNum) ;
VOID __stdcall INIT_Simulate_Int (DWORD IntNum) ;
VOID __stdcall DEBUG_Simulate_Int (DWORD IntNum) ;
VOID __stdcall STATIC_Simulate_Int (DWORD IntNum) ;
VOID __stdcall PAGEABLE_Simulate_Int (DWORD IntNum) ;

VOID __stdcall LOCK_Simulate_Far_Ret_N (DWORD nBytes) ;
VOID __stdcall INIT_Simulate_Far_Ret_N (DWORD nBytes) ;
VOID __stdcall DEBUG_Simulate_Far_Ret_N (DWORD nBytes) ;
VOID __stdcall STATIC_Simulate_Far_Ret_N (DWORD nBytes) ;
VOID __stdcall PAGEABLE_Simulate_Far_Ret_N (DWORD nBytes) ;

#define Simulate_Far_Ret() VMMcall(Simulate_Far_Ret)
VOID __stdcall LOCK_Simulate_Far_Jmp (WORD theSegment, DWORD theOffset) ;
VOID __stdcall INIT_Simulate_Far_Jmp (WORD theSegment, DWORD theOffset) ;
VOID __stdcall DEBUG_Simulate_Far_Jmp (WORD theSegment, DWORD theOffset) ;
VOID __stdcall STATIC_Simulate_Far_Jmp (WORD theSegment, DWORD theOffset) ;
VOID __stdcall PAGEABLE_Simulate_Far_Jmp (WORD theSegment, DWORD theOffset) ;

VOID __stdcall LOCK_Simulate_Far_Call (WORD theSegment, DWORD theOffset) ;
VOID __stdcall INIT_Simulate_Far_Call (WORD theSegment, DWORD theOffset) ;
VOID __stdcall DEBUG_Simulate_Far_Call (WORD theSegment, DWORD theOffset) ;
VOID __stdcall STATIC_Simulate_Far_Call (WORD theSegment, DWORD theOffset) ;
VOID __stdcall PAGEABLE_Simulate_Far_Call (WORD theSegment, DWORD theOffset) ;

VOID __stdcall LOCK_Signal_Semaphore (SEMHANDLE hSem) ;
VOID __stdcall INIT_Signal_Semaphore (SEMHANDLE hSem) ;
VOID __stdcall DEBUG_Signal_Semaphore (SEMHANDLE hSem) ;
VOID __stdcall STATIC_Signal_Semaphore (SEMHANDLE hSem) ;
VOID __stdcall PAGEABLE_Signal_Semaphore (SEMHANDLE hSem) ;

TIMEOUTHANDLE __stdcall LOCK_Set_VM_Time_Out (DWORD Time, VMHANDLE hVM, CONST VOID * Refdata, PTIMEOUT_HANDLER TimeoutCallback, PTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall INIT_Set_VM_Time_Out (DWORD Time, VMHANDLE hVM, CONST VOID * Refdata, PTIMEOUT_HANDLER TimeoutCallback, PTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall DEBUG_Set_VM_Time_Out (DWORD Time, VMHANDLE hVM, CONST VOID * Refdata, PTIMEOUT_HANDLER TimeoutCallback, PTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall STATIC_Set_VM_Time_Out (DWORD Time, VMHANDLE hVM, CONST VOID * Refdata, PTIMEOUT_HANDLER TimeoutCallback, PTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall PAGEABLE_Set_VM_Time_Out (DWORD Time, VMHANDLE hVM, CONST VOID * Refdata, PTIMEOUT_HANDLER TimeoutCallback, PTIMEOUT_THUNK pThunk) ;

VOID __stdcall LOCK_Set_V86_Int_Vector (DWORD Intnum, WORD theSeg, DWORD theOffset) ;
VOID __stdcall INIT_Set_V86_Int_Vector (DWORD Intnum, WORD theSeg, DWORD theOffset) ;
VOID __stdcall DEBUG_Set_V86_Int_Vector (DWORD Intnum, WORD theSeg, DWORD theOffset) ;
VOID __stdcall STATIC_Set_V86_Int_Vector (DWORD Intnum, WORD theSeg, DWORD theOffset) ;
VOID __stdcall PAGEABLE_Set_V86_Int_Vector (DWORD Intnum, WORD theSeg, DWORD theOffset) ;

VOID __stdcall LOCK_Set_PM_Int_Vector (DWORD Intnum, WORD theSeg, DWORD theOffset) ;
VOID __stdcall INIT_Set_PM_Int_Vector (DWORD Intnum, WORD theSeg, DWORD theOffset) ;
VOID __stdcall DEBUG_Set_PM_Int_Vector (DWORD Intnum, WORD theSeg, DWORD theOffset) ;
VOID __stdcall STATIC_Set_PM_Int_Vector (DWORD Intnum, WORD theSeg, DWORD theOffset) ;
VOID __stdcall PAGEABLE_Set_PM_Int_Vector (DWORD Intnum, WORD theSeg, DWORD theOffset) ;

#define Set_V86_Exec_Mode() VMMcall(Set_V86_Exec_Mode)
BOOL __stdcall LOCK_Set_Time_Slice_Priority (DWORD Flags, VMHANDLE hVM, DWORD Foreground, DWORD Background) ;
BOOL __stdcall INIT_Set_Time_Slice_Priority (DWORD Flags, VMHANDLE hVM, DWORD Foreground, DWORD Background) ;
BOOL __stdcall DEBUG_Set_Time_Slice_Priority (DWORD Flags, VMHANDLE hVM, DWORD Foreground, DWORD Background) ;
BOOL __stdcall STATIC_Set_Time_Slice_Priority (DWORD Flags, VMHANDLE hVM, DWORD Foreground, DWORD Background) ;
BOOL __stdcall PAGEABLE_Set_Time_Slice_Priority (DWORD Flags, VMHANDLE hVM, DWORD Foreground, DWORD Background) ;

VOID __stdcall LOCK_Set_Time_Slice_Granularity (DWORD Time) ;
VOID __stdcall INIT_Set_Time_Slice_Granularity (DWORD Time) ;
VOID __stdcall DEBUG_Set_Time_Slice_Granularity (DWORD Time) ;
VOID __stdcall STATIC_Set_Time_Slice_Granularity (DWORD Time) ;
VOID __stdcall PAGEABLE_Set_Time_Slice_Granularity (DWORD Time) ;

VOID __stdcall LOCK_Set_System_Exit_Code (BYTE ExitCode) ;
VOID __stdcall INIT_Set_System_Exit_Code (BYTE ExitCode) ;
VOID __stdcall DEBUG_Set_System_Exit_Code (BYTE ExitCode) ;
VOID __stdcall STATIC_Set_System_Exit_Code (BYTE ExitCode) ;
VOID __stdcall PAGEABLE_Set_System_Exit_Code (BYTE ExitCode) ;

BOOL __cdecl LOCK__SetResetV86Pageable (VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl INIT__SetResetV86Pageable (VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl DEBUG__SetResetV86Pageable (VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl STATIC__SetResetV86Pageable (VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl PAGEABLE__SetResetV86Pageable (VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages, DWORD Flags) ;

VOID __stdcall LOCK_Set_PM_Int_Type (DWORD IntNum, DWORD IntType) ;
VOID __stdcall INIT_Set_PM_Int_Type (DWORD IntNum, DWORD IntType) ;
VOID __stdcall DEBUG_Set_PM_Int_Type (DWORD IntNum, DWORD IntType) ;
VOID __stdcall STATIC_Set_PM_Int_Type (DWORD IntNum, DWORD IntType) ;
VOID __stdcall PAGEABLE_Set_PM_Int_Type (DWORD IntNum, DWORD IntType) ;

#define Set_PM_Exec_Mode() VMMcall(Set_PM_Exec_Mode)
VOID __stdcall LOCK_Set_Physical_HMA_Alias (CONST VOID * Entries) ;
VOID __stdcall INIT_Set_Physical_HMA_Alias (CONST VOID * Entries) ;
VOID __stdcall DEBUG_Set_Physical_HMA_Alias (CONST VOID * Entries) ;
VOID __stdcall STATIC_Set_Physical_HMA_Alias (CONST VOID * Entries) ;
VOID __stdcall PAGEABLE_Set_Physical_HMA_Alias (CONST VOID * Entries) ;

VOID __stdcall LOCK_Set_NMI_Handler_Addr (PNMI_HANDLER NmiHandler, PNMI_THUNK pThunk) ;
VOID __stdcall INIT_Set_NMI_Handler_Addr (PNMI_HANDLER NmiHandler, PNMI_THUNK pThunk) ;
VOID __stdcall DEBUG_Set_NMI_Handler_Addr (PNMI_HANDLER NmiHandler, PNMI_THUNK pThunk) ;
VOID __stdcall STATIC_Set_NMI_Handler_Addr (PNMI_HANDLER NmiHandler, PNMI_THUNK pThunk) ;
VOID __stdcall PAGEABLE_Set_NMI_Handler_Addr (PNMI_HANDLER NmiHandler, PNMI_THUNK pThunk) ;

VOID __stdcall LOCK_Set_Mono_Cur_Pos (DWORD Row, DWORD Col) ;
VOID __stdcall INIT_Set_Mono_Cur_Pos (DWORD Row, DWORD Col) ;
VOID __stdcall DEBUG_Set_Mono_Cur_Pos (DWORD Row, DWORD Col) ;
VOID __stdcall STATIC_Set_Mono_Cur_Pos (DWORD Row, DWORD Col) ;
VOID __stdcall PAGEABLE_Set_Mono_Cur_Pos (DWORD Row, DWORD Col) ;

BOOL __cdecl LOCK__SetLastV86Page (DWORD Pgnum, DWORD Flags) ;
BOOL __cdecl INIT__SetLastV86Page (DWORD Pgnum, DWORD Flags) ;
BOOL __cdecl DEBUG__SetLastV86Page (DWORD Pgnum, DWORD Flags) ;
BOOL __cdecl STATIC__SetLastV86Page (DWORD Pgnum, DWORD Flags) ;
BOOL __cdecl PAGEABLE__SetLastV86Page (DWORD Pgnum, DWORD Flags) ;

TIMEOUTHANDLE __stdcall LOCK_Set_Global_Time_Out (DWORD Time, CONST VOID * Refdata, PTIMEOUT_HANDLER TimeoutCallback, PTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall INIT_Set_Global_Time_Out (DWORD Time, CONST VOID * Refdata, PTIMEOUT_HANDLER TimeoutCallback, PTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall DEBUG_Set_Global_Time_Out (DWORD Time, CONST VOID * Refdata, PTIMEOUT_HANDLER TimeoutCallback, PTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall STATIC_Set_Global_Time_Out (DWORD Time, CONST VOID * Refdata, PTIMEOUT_HANDLER TimeoutCallback, PTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall PAGEABLE_Set_Global_Time_Out (DWORD Time, CONST VOID * Refdata, PTIMEOUT_HANDLER TimeoutCallback, PTIMEOUT_THUNK pThunk) ;

BOOL __stdcall LOCK__SetFreePhysRegCalBk (CONST VOID * Callback, DWORD Flags, PFREEPHYS_THUNK pThunk) ;
BOOL __stdcall INIT__SetFreePhysRegCalBk (CONST VOID * Callback, DWORD Flags, PFREEPHYS_THUNK pThunk) ;
BOOL __stdcall DEBUG__SetFreePhysRegCalBk (CONST VOID * Callback, DWORD Flags, PFREEPHYS_THUNK pThunk) ;
BOOL __stdcall STATIC__SetFreePhysRegCalBk (CONST VOID * Callback, DWORD Flags, PFREEPHYS_THUNK pThunk) ;
BOOL __stdcall PAGEABLE__SetFreePhysRegCalBk (CONST VOID * Callback, DWORD Flags, PFREEPHYS_THUNK pThunk) ;

BOOL __stdcall LOCK_Set_Execution_Focus (VMHANDLE hVM) ;
BOOL __stdcall INIT_Set_Execution_Focus (VMHANDLE hVM) ;
BOOL __stdcall DEBUG_Set_Execution_Focus (VMHANDLE hVM) ;
BOOL __stdcall STATIC_Set_Execution_Focus (VMHANDLE hVM) ;
BOOL __stdcall PAGEABLE_Set_Execution_Focus (VMHANDLE hVM) ;

BOOL __cdecl LOCK__SetDescriptor (DWORD Selector, VMHANDLE hVM, DWORD DescDW1, DWORD DescDW2, DWORD Flags) ;
BOOL __cdecl INIT__SetDescriptor (DWORD Selector, VMHANDLE hVM, DWORD DescDW1, DWORD DescDW2, DWORD Flags) ;
BOOL __cdecl DEBUG__SetDescriptor (DWORD Selector, VMHANDLE hVM, DWORD DescDW1, DWORD DescDW2, DWORD Flags) ;
BOOL __cdecl STATIC__SetDescriptor (DWORD Selector, VMHANDLE hVM, DWORD DescDW1, DWORD DescDW2, DWORD Flags) ;
BOOL __cdecl PAGEABLE__SetDescriptor (DWORD Selector, VMHANDLE hVM, DWORD DescDW1, DWORD DescDW2, DWORD Flags) ;

BOOL __stdcall LOCK_Set_Delete_On_Exit_File (CONST CHAR * pFilename) ;
BOOL __stdcall INIT_Set_Delete_On_Exit_File (CONST CHAR * pFilename) ;
BOOL __stdcall DEBUG_Set_Delete_On_Exit_File (CONST CHAR * pFilename) ;
BOOL __stdcall STATIC_Set_Delete_On_Exit_File (CONST CHAR * pFilename) ;
BOOL __stdcall PAGEABLE_Set_Delete_On_Exit_File (CONST CHAR * pFilename) ;

PVOID __cdecl LOCK__SelectorMapFlat (VMHANDLE hVM, DWORD Selector, DWORD Flags) ;
PVOID __cdecl INIT__SelectorMapFlat (VMHANDLE hVM, DWORD Selector, DWORD Flags) ;
PVOID __cdecl DEBUG__SelectorMapFlat (VMHANDLE hVM, DWORD Selector, DWORD Flags) ;
PVOID __cdecl STATIC__SelectorMapFlat (VMHANDLE hVM, DWORD Selector, DWORD Flags) ;
PVOID __cdecl PAGEABLE__SelectorMapFlat (VMHANDLE hVM, DWORD Selector, DWORD Flags) ;

EVENTHANDLE __stdcall LOCK_Schedule_VM_Event (VMHANDLE hVM, PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall INIT_Schedule_VM_Event (VMHANDLE hVM, PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall DEBUG_Schedule_VM_Event (VMHANDLE hVM, PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall STATIC_Schedule_VM_Event (VMHANDLE hVM, PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall PAGEABLE_Schedule_VM_Event (VMHANDLE hVM, PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;

EVENTHANDLE __stdcall LOCK_Schedule_Global_Event (PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall INIT_Schedule_Global_Event (PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall DEBUG_Schedule_Global_Event (PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall STATIC_Schedule_Global_Event (PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall PAGEABLE_Schedule_Global_Event (PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;

VOID __stdcall LOCK_Save_Client_State (PCLIENT_STRUCT pClientState) ;
VOID __stdcall INIT_Save_Client_State (PCLIENT_STRUCT pClientState) ;
VOID __stdcall DEBUG_Save_Client_State (PCLIENT_STRUCT pClientState) ;
VOID __stdcall STATIC_Save_Client_State (PCLIENT_STRUCT pClientState) ;
VOID __stdcall PAGEABLE_Save_Client_State (PCLIENT_STRUCT pClientState) ;

BOOL __stdcall LOCK_Resume_VM (VMHANDLE hVM) ;
BOOL __stdcall INIT_Resume_VM (VMHANDLE hVM) ;
BOOL __stdcall DEBUG_Resume_VM (VMHANDLE hVM) ;
BOOL __stdcall STATIC_Resume_VM (VMHANDLE hVM) ;
BOOL __stdcall PAGEABLE_Resume_VM (VMHANDLE hVM) ;

#define Resume_Exec() VMMcall(Resume_Exec)
VOID __stdcall LOCK_Restore_Client_State (PCLIENT_STRUCT pClientState) ;
VOID __stdcall INIT_Restore_Client_State (PCLIENT_STRUCT pClientState) ;
VOID __stdcall DEBUG_Restore_Client_State (PCLIENT_STRUCT pClientState) ;
VOID __stdcall STATIC_Restore_Client_State (PCLIENT_STRUCT pClientState) ;
VOID __stdcall PAGEABLE_Restore_Client_State (PCLIENT_STRUCT pClientState) ;

BOOL __stdcall LOCK_Remove_V86_Break_Point (SEGOFFSET BreakAddr) ;
BOOL __stdcall INIT_Remove_V86_Break_Point (SEGOFFSET BreakAddr) ;
BOOL __stdcall DEBUG_Remove_V86_Break_Point (SEGOFFSET BreakAddr) ;
BOOL __stdcall STATIC_Remove_V86_Break_Point (SEGOFFSET BreakAddr) ;
BOOL __stdcall PAGEABLE_Remove_V86_Break_Point (SEGOFFSET BreakAddr) ;

BOOL __stdcall LOCK_Remove_Exception_Handler (PEH_STRUC ExceptStruc) ;
BOOL __stdcall INIT_Remove_Exception_Handler (PEH_STRUC ExceptStruc) ;
BOOL __stdcall DEBUG_Remove_Exception_Handler (PEH_STRUC ExceptStruc) ;
BOOL __stdcall STATIC_Remove_Exception_Handler (PEH_STRUC ExceptStruc) ;
BOOL __stdcall PAGEABLE_Remove_Exception_Handler (PEH_STRUC ExceptStruc) ;

#define Release_Time_Slice() VMMcall(Release_Time_Slice)
VOID __stdcall LOCK_Release_Critical_Section (DWORD Claims) ;
VOID __stdcall INIT_Release_Critical_Section (DWORD Claims) ;
VOID __stdcall DEBUG_Release_Critical_Section (DWORD Claims) ;
VOID __stdcall STATIC_Release_Critical_Section (DWORD Claims) ;
VOID __stdcall PAGEABLE_Release_Critical_Section (DWORD Claims) ;

VOID __stdcall LOCK_Queue_Debug_String (DWORD val1, DWORD val2, CONST CHAR * pString) ;
VOID __stdcall INIT_Queue_Debug_String (DWORD val1, DWORD val2, CONST CHAR * pString) ;
VOID __stdcall DEBUG_Queue_Debug_String (DWORD val1, DWORD val2, CONST CHAR * pString) ;
VOID __stdcall STATIC_Queue_Debug_String (DWORD val1, DWORD val2, CONST CHAR * pString) ;
VOID __stdcall PAGEABLE_Queue_Debug_String (DWORD val1, DWORD val2, CONST CHAR * pString) ;

BOOL __cdecl LOCK__PhysIntoV86 (DWORD PhysPage, VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl INIT__PhysIntoV86 (DWORD PhysPage, VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl DEBUG__PhysIntoV86 (DWORD PhysPage, VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl STATIC__PhysIntoV86 (DWORD PhysPage, VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl PAGEABLE__PhysIntoV86 (DWORD PhysPage, VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages, DWORD Flags) ;

BOOL __cdecl LOCK__PageUnLock (MEMHANDLE hMem, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl INIT__PageUnLock (MEMHANDLE hMem, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl DEBUG__PageUnLock (MEMHANDLE hMem, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl STATIC__PageUnLock (MEMHANDLE hMem, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl PAGEABLE__PageUnLock (MEMHANDLE hMem, DWORD nPages, DWORD PageOff, DWORD Flags) ;

BOOL __cdecl LOCK__PageResetHandlePAddr (MEMHANDLE hMem, DWORD PgOff, DWORD nPages, DWORD PhysPgNum, DWORD Flags) ;
BOOL __cdecl INIT__PageResetHandlePAddr (MEMHANDLE hMem, DWORD PgOff, DWORD nPages, DWORD PhysPgNum, DWORD Flags) ;
BOOL __cdecl DEBUG__PageResetHandlePAddr (MEMHANDLE hMem, DWORD PgOff, DWORD nPages, DWORD PhysPgNum, DWORD Flags) ;
BOOL __cdecl STATIC__PageResetHandlePAddr (MEMHANDLE hMem, DWORD PgOff, DWORD nPages, DWORD PhysPgNum, DWORD Flags) ;
BOOL __cdecl PAGEABLE__PageResetHandlePAddr (MEMHANDLE hMem, DWORD PgOff, DWORD nPages, DWORD PhysPgNum, DWORD Flags) ;

MEMHANDLE __stdcall LOCK__PageReAllocate (MEMHANDLE hMem, DWORD nPages, DWORD Flags, PPVOID pAddr) ;
MEMHANDLE __stdcall INIT__PageReAllocate (MEMHANDLE hMem, DWORD nPages, DWORD Flags, PPVOID pAddr) ;
MEMHANDLE __stdcall DEBUG__PageReAllocate (MEMHANDLE hMem, DWORD nPages, DWORD Flags, PPVOID pAddr) ;
MEMHANDLE __stdcall STATIC__PageReAllocate (MEMHANDLE hMem, DWORD nPages, DWORD Flags, PPVOID pAddr) ;
MEMHANDLE __stdcall PAGEABLE__PageReAllocate (MEMHANDLE hMem, DWORD nPages, DWORD Flags, PPVOID pAddr) ;

DWORD __cdecl LOCK__PageOutDirtyPages (DWORD nPages, DWORD Flags) ;
DWORD __cdecl INIT__PageOutDirtyPages (DWORD nPages, DWORD Flags) ;
DWORD __cdecl DEBUG__PageOutDirtyPages (DWORD nPages, DWORD Flags) ;
DWORD __cdecl STATIC__PageOutDirtyPages (DWORD nPages, DWORD Flags) ;
DWORD __cdecl PAGEABLE__PageOutDirtyPages (DWORD nPages, DWORD Flags) ;

BOOL __cdecl LOCK__PageLock (MEMHANDLE hMem, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl INIT__PageLock (MEMHANDLE hMem, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl DEBUG__PageLock (MEMHANDLE hMem, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl STATIC__PageLock (MEMHANDLE hMem, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl PAGEABLE__PageLock (MEMHANDLE hMem, DWORD nPages, DWORD PageOff, DWORD Flags) ;

DWORD __stdcall LOCK__PageGetSizeAddr (MEMHANDLE hMem, DWORD Flags, PVOID pAddr) ;
DWORD __stdcall INIT__PageGetSizeAddr (MEMHANDLE hMem, DWORD Flags, PVOID pAddr) ;
DWORD __stdcall DEBUG__PageGetSizeAddr (MEMHANDLE hMem, DWORD Flags, PVOID pAddr) ;
DWORD __stdcall STATIC__PageGetSizeAddr (MEMHANDLE hMem, DWORD Flags, PVOID pAddr) ;
DWORD __stdcall PAGEABLE__PageGetSizeAddr (MEMHANDLE hMem, DWORD Flags, PVOID pAddr) ;

DWORD __stdcall LOCK__PageGetAllocInfo (DWORD Flags, PDWORD pLockable) ;
DWORD __stdcall INIT__PageGetAllocInfo (DWORD Flags, PDWORD pLockable) ;
DWORD __stdcall DEBUG__PageGetAllocInfo (DWORD Flags, PDWORD pLockable) ;
DWORD __stdcall STATIC__PageGetAllocInfo (DWORD Flags, PDWORD pLockable) ;
DWORD __stdcall PAGEABLE__PageGetAllocInfo (DWORD Flags, PDWORD pLockable) ;

BOOL __cdecl LOCK__PageFree (MEMHANDLE hMem, DWORD Flags) ;
BOOL __cdecl INIT__PageFree (MEMHANDLE hMem, DWORD Flags) ;
BOOL __cdecl DEBUG__PageFree (MEMHANDLE hMem, DWORD Flags) ;
BOOL __cdecl STATIC__PageFree (MEMHANDLE hMem, DWORD Flags) ;
BOOL __cdecl PAGEABLE__PageFree (MEMHANDLE hMem, DWORD Flags) ;

BOOL __cdecl LOCK__PageDiscardPages (DWORD LinPgNum, VMHANDLE hVM, DWORD nPages, DWORD Flags) ;
BOOL __cdecl INIT__PageDiscardPages (DWORD LinPgNum, VMHANDLE hVM, DWORD nPages, DWORD Flags) ;
BOOL __cdecl DEBUG__PageDiscardPages (DWORD LinPgNum, VMHANDLE hVM, DWORD nPages, DWORD Flags) ;
BOOL __cdecl STATIC__PageDiscardPages (DWORD LinPgNum, VMHANDLE hVM, DWORD nPages, DWORD Flags) ;
BOOL __cdecl PAGEABLE__PageDiscardPages (DWORD LinPgNum, VMHANDLE hVM, DWORD nPages, DWORD Flags) ;

DWORD __cdecl LOCK__PageCheckLinRange (DWORD hLinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl INIT__PageCheckLinRange (DWORD hLinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl DEBUG__PageCheckLinRange (DWORD hLinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl STATIC__PageCheckLinRange (DWORD hLinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl PAGEABLE__PageCheckLinRange (DWORD hLinPgNum, DWORD nPages, DWORD Flags) ;

BOOL __stdcall LOCK__PageAllocate (DWORD nPages, DWORD pType, VMHANDLE hVM, DWORD AlignMask, DWORD minPhys, DWORD maxPhys, PVOID PhysAddr, DWORD Flags, PMEMHANDLE  pHandle, PPVOID pAddress) ;
BOOL __stdcall INIT__PageAllocate (DWORD nPages, DWORD pType, VMHANDLE hVM, DWORD AlignMask, DWORD minPhys, DWORD maxPhys, PVOID PhysAddr, DWORD Flags, PMEMHANDLE  pHandle, PPVOID pAddress) ;
BOOL __stdcall DEBUG__PageAllocate (DWORD nPages, DWORD pType, VMHANDLE hVM, DWORD AlignMask, DWORD minPhys, DWORD maxPhys, PVOID PhysAddr, DWORD Flags, PMEMHANDLE  pHandle, PPVOID pAddress) ;
BOOL __stdcall STATIC__PageAllocate (DWORD nPages, DWORD pType, VMHANDLE hVM, DWORD AlignMask, DWORD minPhys, DWORD maxPhys, PVOID PhysAddr, DWORD Flags, PMEMHANDLE  pHandle, PPVOID pAddress) ;
BOOL __stdcall PAGEABLE__PageAllocate (DWORD nPages, DWORD pType, VMHANDLE hVM, DWORD AlignMask, DWORD minPhys, DWORD maxPhys, PVOID PhysAddr, DWORD Flags, PMEMHANDLE  pHandle, PPVOID pAddress) ;

VOID __stdcall LOCK_Out_Mono_String (CONST CHAR * pString) ;
VOID __stdcall INIT_Out_Mono_String (CONST CHAR * pString) ;
VOID __stdcall DEBUG_Out_Mono_String (CONST CHAR * pString) ;
VOID __stdcall STATIC_Out_Mono_String (CONST CHAR * pString) ;
VOID __stdcall PAGEABLE_Out_Mono_String (CONST CHAR * pString) ;

VOID __stdcall LOCK_Out_Mono_Chr_Attrs (DWORD Char, DWORD Attributes) ;
VOID __stdcall INIT_Out_Mono_Chr_Attrs (DWORD Char, DWORD Attributes) ;
VOID __stdcall DEBUG_Out_Mono_Chr_Attrs (DWORD Char, DWORD Attributes) ;
VOID __stdcall STATIC_Out_Mono_Chr_Attrs (DWORD Char, DWORD Attributes) ;
VOID __stdcall PAGEABLE_Out_Mono_Chr_Attrs (DWORD Char, DWORD Attributes) ;

VOID __stdcall LOCK_Out_Mono_Chr (BYTE Char) ;
VOID __stdcall INIT_Out_Mono_Chr (BYTE Char) ;
VOID __stdcall DEBUG_Out_Mono_Chr (BYTE Char) ;
VOID __stdcall STATIC_Out_Mono_Chr (BYTE Char) ;
VOID __stdcall PAGEABLE_Out_Mono_Chr (BYTE Char) ;

VOID __stdcall LOCK_Out_Debug_String (CONST CHAR * String) ;
VOID __stdcall INIT_Out_Debug_String (CONST CHAR * String) ;
VOID __stdcall DEBUG_Out_Debug_String (CONST CHAR * String) ;
VOID __stdcall STATIC_Out_Debug_String (CONST CHAR * String) ;
VOID __stdcall PAGEABLE_Out_Debug_String (CONST CHAR * String) ;

VOID __stdcall LOCK_Out_Debug_Chr (BYTE Char) ;
VOID __stdcall INIT_Out_Debug_Chr (BYTE Char) ;
VOID __stdcall DEBUG_Out_Debug_Chr (BYTE Char) ;
VOID __stdcall STATIC_Out_Debug_Chr (BYTE Char) ;
VOID __stdcall PAGEABLE_Out_Debug_Chr (BYTE Char) ;

DWORD __stdcall LOCK_OpenFile (CONST CHAR * Filename, PVOID Buffer) ;
DWORD __stdcall INIT_OpenFile (CONST CHAR * Filename, PVOID Buffer) ;
DWORD __stdcall DEBUG_OpenFile (CONST CHAR * Filename, PVOID Buffer) ;
DWORD __stdcall STATIC_OpenFile (CONST CHAR * Filename, PVOID Buffer) ;
DWORD __stdcall PAGEABLE_OpenFile (CONST CHAR * Filename, PVOID Buffer) ;

VOID __stdcall LOCK_Nuke_VM (VMHANDLE hVM) ;
VOID __stdcall INIT_Nuke_VM (VMHANDLE hVM) ;
VOID __stdcall DEBUG_Nuke_VM (VMHANDLE hVM) ;
VOID __stdcall STATIC_Nuke_VM (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_Nuke_VM (VMHANDLE hVM) ;

VOID __stdcall LOCK_No_Fail_Resume_VM (VMHANDLE hVM) ;
VOID __stdcall INIT_No_Fail_Resume_VM (VMHANDLE hVM) ;
VOID __stdcall DEBUG_No_Fail_Resume_VM (VMHANDLE hVM) ;
VOID __stdcall STATIC_No_Fail_Resume_VM (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_No_Fail_Resume_VM (VMHANDLE hVM) ;

BOOL __cdecl LOCK__ModifyPageBits (VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages,  DWORD bitAnd, DWORD bitOR, DWORD pType, DWORD Flags) ;
BOOL __cdecl INIT__ModifyPageBits (VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages,  DWORD bitAnd, DWORD bitOR, DWORD pType, DWORD Flags) ;
BOOL __cdecl DEBUG__ModifyPageBits (VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages,  DWORD bitAnd, DWORD bitOR, DWORD pType, DWORD Flags) ;
BOOL __cdecl STATIC__ModifyPageBits (VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages,  DWORD bitAnd, DWORD bitOR, DWORD pType, DWORD Flags) ;
BOOL __cdecl PAGEABLE__ModifyPageBits (VMHANDLE hVM, DWORD VMLinPgNum, DWORD nPages,  DWORD bitAnd, DWORD bitOR, DWORD pType, DWORD Flags) ;

BOOL __cdecl LOCK__MMGR_Toggle_HMA (VMHANDLE hVM, DWORD Flags) ;
BOOL __cdecl INIT__MMGR_Toggle_HMA (VMHANDLE hVM, DWORD Flags) ;
BOOL __cdecl DEBUG__MMGR_Toggle_HMA (VMHANDLE hVM, DWORD Flags) ;
BOOL __cdecl STATIC__MMGR_Toggle_HMA (VMHANDLE hVM, DWORD Flags) ;
BOOL __cdecl PAGEABLE__MMGR_Toggle_HMA (VMHANDLE hVM, DWORD Flags) ;

VOID __stdcall LOCK_MMGR_SetNULPageAddr (CONST VOID * PhysAddr) ;
VOID __stdcall INIT_MMGR_SetNULPageAddr (CONST VOID * PhysAddr) ;
VOID __stdcall DEBUG_MMGR_SetNULPageAddr (CONST VOID * PhysAddr) ;
VOID __stdcall STATIC_MMGR_SetNULPageAddr (CONST VOID * PhysAddr) ;
VOID __stdcall PAGEABLE_MMGR_SetNULPageAddr (CONST VOID * PhysAddr) ;

PVOID __cdecl LOCK__MapPhysToLinear (CONST VOID * PhysAddr, DWORD nBytes, DWORD Flags) ;
PVOID __cdecl INIT__MapPhysToLinear (CONST VOID * PhysAddr, DWORD nBytes, DWORD Flags) ;
PVOID __cdecl DEBUG__MapPhysToLinear (CONST VOID * PhysAddr, DWORD nBytes, DWORD Flags) ;
PVOID __cdecl STATIC__MapPhysToLinear (CONST VOID * PhysAddr, DWORD nBytes, DWORD Flags) ;
PVOID __cdecl PAGEABLE__MapPhysToLinear (CONST VOID * PhysAddr, DWORD nBytes, DWORD Flags) ;

BOOL __stdcall LOCK_Map_Lin_To_VM_Addr (CONST VOID * LinAddr, DWORD Limit, PDWORD pSeg, PDWORD pOffset) ;
BOOL __stdcall INIT_Map_Lin_To_VM_Addr (CONST VOID * LinAddr, DWORD Limit, PDWORD pSeg, PDWORD pOffset) ;
BOOL __stdcall DEBUG_Map_Lin_To_VM_Addr (CONST VOID * LinAddr, DWORD Limit, PDWORD pSeg, PDWORD pOffset) ;
BOOL __stdcall STATIC_Map_Lin_To_VM_Addr (CONST VOID * LinAddr, DWORD Limit, PDWORD pSeg, PDWORD pOffset) ;
BOOL __stdcall PAGEABLE_Map_Lin_To_VM_Addr (CONST VOID * LinAddr, DWORD Limit, PDWORD pSeg, PDWORD pOffset) ;

BOOL __cdecl LOCK__MapIntoV86 (MEMHANDLE hMem, VMHANDLE hVM, DWORD VMLinPageNumber, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl INIT__MapIntoV86 (MEMHANDLE hMem, VMHANDLE hVM, DWORD VMLinPageNumber, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl DEBUG__MapIntoV86 (MEMHANDLE hMem, VMHANDLE hVM, DWORD VMLinPageNumber, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl STATIC__MapIntoV86 (MEMHANDLE hMem, VMHANDLE hVM, DWORD VMLinPageNumber, DWORD nPages, DWORD PageOff, DWORD Flags) ;
BOOL __cdecl PAGEABLE__MapIntoV86 (MEMHANDLE hMem, VMHANDLE hVM, DWORD VMLinPageNumber, DWORD nPages, DWORD PageOff, DWORD Flags) ;

DWORD __cdecl LOCK__MapFreePhysReg (DWORD LinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl INIT__MapFreePhysReg (DWORD LinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl DEBUG__MapFreePhysReg (DWORD LinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl STATIC__MapFreePhysReg (DWORD LinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl PAGEABLE__MapFreePhysReg (DWORD LinPgNum, DWORD nPages, DWORD Flags) ;

PVOID __stdcall LOCK_Map_Flat (BYTE RefSegment, BYTE RefOffset) ;
PVOID __stdcall INIT_Map_Flat (BYTE RefSegment, BYTE RefOffset) ;
PVOID __stdcall DEBUG_Map_Flat (BYTE RefSegment, BYTE RefOffset) ;
PVOID __stdcall STATIC_Map_Flat (BYTE RefSegment, BYTE RefOffset) ;
PVOID __stdcall PAGEABLE_Map_Flat (BYTE RefSegment, BYTE RefOffset) ;

#define Log_Proc_Call() VMMcall(Log_Proc_Call)
PVOID __stdcall LOCK_Locate_Byte_In_ROM (BYTE theByte) ;
PVOID __stdcall INIT_Locate_Byte_In_ROM (BYTE theByte) ;
PVOID __stdcall DEBUG_Locate_Byte_In_ROM (BYTE theByte) ;
PVOID __stdcall STATIC_Locate_Byte_In_ROM (BYTE theByte) ;
PVOID __stdcall PAGEABLE_Locate_Byte_In_ROM (BYTE theByte) ;

PNODE __stdcall LOCK_List_Remove_First (LISTHANDLE hList) ;
PNODE __stdcall INIT_List_Remove_First (LISTHANDLE hList) ;
PNODE __stdcall DEBUG_List_Remove_First (LISTHANDLE hList) ;
PNODE __stdcall STATIC_List_Remove_First (LISTHANDLE hList) ;
PNODE __stdcall PAGEABLE_List_Remove_First (LISTHANDLE hList) ;

BOOL __stdcall LOCK_List_Remove (LISTHANDLE hList, CONST NODE * pNode) ;
BOOL __stdcall INIT_List_Remove (LISTHANDLE hList, CONST NODE * pNode) ;
BOOL __stdcall DEBUG_List_Remove (LISTHANDLE hList, CONST NODE * pNode) ;
BOOL __stdcall STATIC_List_Remove (LISTHANDLE hList, CONST NODE * pNode) ;
BOOL __stdcall PAGEABLE_List_Remove (LISTHANDLE hList, CONST NODE * pNode) ;

VOID __stdcall LOCK_List_Insert (LISTHANDLE hList, CONST NODE * pNewnode, CONST NODE *  pOldnode) ;
VOID __stdcall INIT_List_Insert (LISTHANDLE hList, CONST NODE * pNewnode, CONST NODE *  pOldnode) ;
VOID __stdcall DEBUG_List_Insert (LISTHANDLE hList, CONST NODE * pNewnode, CONST NODE *  pOldnode) ;
VOID __stdcall STATIC_List_Insert (LISTHANDLE hList, CONST NODE * pNewnode, CONST NODE *  pOldnode) ;
VOID __stdcall PAGEABLE_List_Insert (LISTHANDLE hList, CONST NODE * pNewnode, CONST NODE *  pOldnode) ;

PNODE __stdcall LOCK_List_Get_Next (LISTHANDLE hList, CONST NODE * pNode) ;
PNODE __stdcall INIT_List_Get_Next (LISTHANDLE hList, CONST NODE * pNode) ;
PNODE __stdcall DEBUG_List_Get_Next (LISTHANDLE hList, CONST NODE * pNode) ;
PNODE __stdcall STATIC_List_Get_Next (LISTHANDLE hList, CONST NODE * pNode) ;
PNODE __stdcall PAGEABLE_List_Get_Next (LISTHANDLE hList, CONST NODE * pNode) ;

PNODE __stdcall LOCK_List_Get_First (LISTHANDLE hList) ;
PNODE __stdcall INIT_List_Get_First (LISTHANDLE hList) ;
PNODE __stdcall DEBUG_List_Get_First (LISTHANDLE hList) ;
PNODE __stdcall STATIC_List_Get_First (LISTHANDLE hList) ;
PNODE __stdcall PAGEABLE_List_Get_First (LISTHANDLE hList) ;

VOID __stdcall LOCK_List_Destroy (LISTHANDLE hList) ;
VOID __stdcall INIT_List_Destroy (LISTHANDLE hList) ;
VOID __stdcall DEBUG_List_Destroy (LISTHANDLE hList) ;
VOID __stdcall STATIC_List_Destroy (LISTHANDLE hList) ;
VOID __stdcall PAGEABLE_List_Destroy (LISTHANDLE hList) ;

VOID __stdcall LOCK_List_Deallocate (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall INIT_List_Deallocate (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall DEBUG_List_Deallocate (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall STATIC_List_Deallocate (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall PAGEABLE_List_Deallocate (LISTHANDLE hList, CONST NODE * pNode) ;

LISTHANDLE __stdcall LOCK_List_Create (DWORD Flags, DWORD NodeSize) ;
LISTHANDLE __stdcall INIT_List_Create (DWORD Flags, DWORD NodeSize) ;
LISTHANDLE __stdcall DEBUG_List_Create (DWORD Flags, DWORD NodeSize) ;
LISTHANDLE __stdcall STATIC_List_Create (DWORD Flags, DWORD NodeSize) ;
LISTHANDLE __stdcall PAGEABLE_List_Create (DWORD Flags, DWORD NodeSize) ;

VOID __stdcall LOCK_List_Attach_Tail (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall INIT_List_Attach_Tail (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall DEBUG_List_Attach_Tail (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall STATIC_List_Attach_Tail (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall PAGEABLE_List_Attach_Tail (LISTHANDLE hList, CONST NODE * pNode) ;

VOID __stdcall LOCK_List_Attach (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall INIT_List_Attach (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall DEBUG_List_Attach (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall STATIC_List_Attach (LISTHANDLE hList, CONST NODE * pNode) ;
VOID __stdcall PAGEABLE_List_Attach (LISTHANDLE hList, CONST NODE * pNode) ;

PNODE __stdcall LOCK_List_Allocate (LISTHANDLE hList) ;
PNODE __stdcall INIT_List_Allocate (LISTHANDLE hList) ;
PNODE __stdcall DEBUG_List_Allocate (LISTHANDLE hList) ;
PNODE __stdcall STATIC_List_Allocate (LISTHANDLE hList) ;
PNODE __stdcall PAGEABLE_List_Allocate (LISTHANDLE hList) ;

BOOL __cdecl LOCK__LinPageUnLock (DWORD HLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl INIT__LinPageUnLock (DWORD HLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl DEBUG__LinPageUnLock (DWORD HLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl STATIC__LinPageUnLock (DWORD HLinPgNum, DWORD nPages, DWORD Flags) ;
BOOL __cdecl PAGEABLE__LinPageUnLock (DWORD HLinPgNum, DWORD nPages, DWORD Flags) ;

DWORD __cdecl LOCK__LinPageLock (DWORD HLinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl INIT__LinPageLock (DWORD HLinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl DEBUG__LinPageLock (DWORD HLinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl STATIC__LinPageLock (DWORD HLinPgNum, DWORD nPages, DWORD Flags) ;
DWORD __cdecl PAGEABLE__LinPageLock (DWORD HLinPgNum, DWORD nPages, DWORD Flags) ;

BOOL __stdcall LOCK__LinMapIntoV86 (DWORD HLinPgNum, VMHANDLE hVM, DWORD  VMLinPgNum, DWORD nPages, DWORD Flags, PVOID pV86Page) ;
BOOL __stdcall INIT__LinMapIntoV86 (DWORD HLinPgNum, VMHANDLE hVM, DWORD  VMLinPgNum, DWORD nPages, DWORD Flags, PVOID pV86Page) ;
BOOL __stdcall DEBUG__LinMapIntoV86 (DWORD HLinPgNum, VMHANDLE hVM, DWORD  VMLinPgNum, DWORD nPages, DWORD Flags, PVOID pV86Page) ;
BOOL __stdcall STATIC__LinMapIntoV86 (DWORD HLinPgNum, VMHANDLE hVM, DWORD  VMLinPgNum, DWORD nPages, DWORD Flags, PVOID pV86Page) ;
BOOL __stdcall PAGEABLE__LinMapIntoV86 (DWORD HLinPgNum, VMHANDLE hVM, DWORD  VMLinPgNum, DWORD nPages, DWORD Flags, PVOID pV86Page) ;

DWORD __stdcall LOCK_Is_Debug_Chr (void) ;
DWORD __stdcall INIT_Is_Debug_Chr (void) ;
DWORD __stdcall DEBUG_Is_Debug_Chr (void) ;
DWORD __stdcall STATIC_Is_Debug_Chr (void) ;
DWORD __stdcall PAGEABLE_Is_Debug_Chr (void) ;

BOOL __stdcall LOCK_Install_V86_Break_Point (SEGOFFSET BreakAddr, CONST VOID * Refdata, PV86Break_HANDLER Callback, PV86Break_THUNK pThunk) ;
BOOL __stdcall INIT_Install_V86_Break_Point (SEGOFFSET BreakAddr, CONST VOID * Refdata, PV86Break_HANDLER Callback, PV86Break_THUNK pThunk) ;
BOOL __stdcall DEBUG_Install_V86_Break_Point (SEGOFFSET BreakAddr, CONST VOID * Refdata, PV86Break_HANDLER Callback, PV86Break_THUNK pThunk) ;
BOOL __stdcall STATIC_Install_V86_Break_Point (SEGOFFSET BreakAddr, CONST VOID * Refdata, PV86Break_HANDLER Callback, PV86Break_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Install_V86_Break_Point (SEGOFFSET BreakAddr, CONST VOID * Refdata, PV86Break_HANDLER Callback, PV86Break_THUNK pThunk) ;

BOOL __stdcall LOCK_Install_IO_Handler (DWORD PortNum, PIO_HANDLER IOCallback, PIO_THUNK pThunk) ;
BOOL __stdcall INIT_Install_IO_Handler (DWORD PortNum, PIO_HANDLER IOCallback, PIO_THUNK pThunk) ;
BOOL __stdcall DEBUG_Install_IO_Handler (DWORD PortNum, PIO_HANDLER IOCallback, PIO_THUNK pThunk) ;
BOOL __stdcall STATIC_Install_IO_Handler (DWORD PortNum, PIO_HANDLER IOCallback, PIO_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Install_IO_Handler (DWORD PortNum, PIO_HANDLER IOCallback, PIO_THUNK pThunk) ;

WORD __stdcall LOCK_In_Debug_Chr (void) ;
WORD __stdcall INIT_In_Debug_Chr (void) ;
WORD __stdcall DEBUG_In_Debug_Chr (void) ;
WORD __stdcall STATIC_In_Debug_Chr (void) ;
WORD __stdcall PAGEABLE_In_Debug_Chr (void) ;

PVOID __stdcall LOCK_Hook_VMM_Fault (DWORD IntNum, PVMMFault_HANDLER FaultProc, PVMMFault_THUNK pThunk) ;
PVOID __stdcall INIT_Hook_VMM_Fault (DWORD IntNum, PVMMFault_HANDLER FaultProc, PVMMFault_THUNK pThunk) ;
PVOID __stdcall DEBUG_Hook_VMM_Fault (DWORD IntNum, PVMMFault_HANDLER FaultProc, PVMMFault_THUNK pThunk) ;
PVOID __stdcall STATIC_Hook_VMM_Fault (DWORD IntNum, PVMMFault_HANDLER FaultProc, PVMMFault_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_Hook_VMM_Fault (DWORD IntNum, PVMMFault_HANDLER FaultProc, PVMMFault_THUNK pThunk) ;

BOOL __stdcall LOCK_Hook_V86_Page (DWORD PageNum, PV86Page_HANDLER Callback, PV86Page_THUNK pThunk) ;
BOOL __stdcall INIT_Hook_V86_Page (DWORD PageNum, PV86Page_HANDLER Callback, PV86Page_THUNK pThunk) ;
BOOL __stdcall DEBUG_Hook_V86_Page (DWORD PageNum, PV86Page_HANDLER Callback, PV86Page_THUNK pThunk) ;
BOOL __stdcall STATIC_Hook_V86_Page (DWORD PageNum, PV86Page_HANDLER Callback, PV86Page_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Hook_V86_Page (DWORD PageNum, PV86Page_HANDLER Callback, PV86Page_THUNK pThunk) ;

BOOL __stdcall LOCK_Hook_V86_Int_Chain (DWORD IntNum, PV86Int_HANDLER FaultProc, PV86Int_THUNK pThunk) ;
BOOL __stdcall INIT_Hook_V86_Int_Chain (DWORD IntNum, PV86Int_HANDLER FaultProc, PV86Int_THUNK pThunk) ;
BOOL __stdcall DEBUG_Hook_V86_Int_Chain (DWORD IntNum, PV86Int_HANDLER FaultProc, PV86Int_THUNK pThunk) ;
BOOL __stdcall STATIC_Hook_V86_Int_Chain (DWORD IntNum, PV86Int_HANDLER FaultProc, PV86Int_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Hook_V86_Int_Chain (DWORD IntNum, PV86Int_HANDLER FaultProc, PV86Int_THUNK pThunk) ;

PVOID __stdcall LOCK_Hook_V86_Fault (DWORD IntNum, PV86Fault_HANDLER FaultProc, PV86Fault_THUNK pThunk) ;
PVOID __stdcall INIT_Hook_V86_Fault (DWORD IntNum, PV86Fault_HANDLER FaultProc, PV86Fault_THUNK pThunk) ;
PVOID __stdcall DEBUG_Hook_V86_Fault (DWORD IntNum, PV86Fault_HANDLER FaultProc, PV86Fault_THUNK pThunk) ;
PVOID __stdcall STATIC_Hook_V86_Fault (DWORD IntNum, PV86Fault_HANDLER FaultProc, PV86Fault_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_Hook_V86_Fault (DWORD IntNum, PV86Fault_HANDLER FaultProc, PV86Fault_THUNK pThunk) ;

PVOID __stdcall LOCK_Hook_PM_Fault (DWORD IntNum, PPMFault_HANDLER FaultProc, PPMFault_THUNK pThunk) ;
PVOID __stdcall INIT_Hook_PM_Fault (DWORD IntNum, PPMFault_HANDLER FaultProc, PPMFault_THUNK pThunk) ;
PVOID __stdcall DEBUG_Hook_PM_Fault (DWORD IntNum, PPMFault_HANDLER FaultProc, PPMFault_THUNK pThunk) ;
PVOID __stdcall STATIC_Hook_PM_Fault (DWORD IntNum, PPMFault_HANDLER FaultProc, PPMFault_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_Hook_PM_Fault (DWORD IntNum, PPMFault_HANDLER FaultProc, PPMFault_THUNK pThunk) ;

PVOID __stdcall LOCK_Hook_Device_PM_API (DWORD devID, PPMApi_HANDLER Callback, PPMApi_THUNK pThunk) ;
PVOID __stdcall INIT_Hook_Device_PM_API (DWORD devID, PPMApi_HANDLER Callback, PPMApi_THUNK pThunk) ;
PVOID __stdcall DEBUG_Hook_Device_PM_API (DWORD devID, PPMApi_HANDLER Callback, PPMApi_THUNK pThunk) ;
PVOID __stdcall STATIC_Hook_Device_PM_API (DWORD devID, PPMApi_HANDLER Callback, PPMApi_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_Hook_Device_PM_API (DWORD devID, PPMApi_HANDLER Callback, PPMApi_THUNK pThunk) ;

VOID __stdcall LOCK_Hook_NMI_Event (PNMIEvent_HANDLER NmiProc, PNMIEvent_THUNK pThunk) ;
VOID __stdcall INIT_Hook_NMI_Event (PNMIEvent_HANDLER NmiProc, PNMIEvent_THUNK pThunk) ;
VOID __stdcall DEBUG_Hook_NMI_Event (PNMIEvent_HANDLER NmiProc, PNMIEvent_THUNK pThunk) ;
VOID __stdcall STATIC_Hook_NMI_Event (PNMIEvent_HANDLER NmiProc, PNMIEvent_THUNK pThunk) ;
VOID __stdcall PAGEABLE_Hook_NMI_Event (PNMIEvent_HANDLER NmiProc, PNMIEvent_THUNK pThunk) ;

BOOL __stdcall LOCK_Hook_Invalid_Page_Fault (PIPF_HANDLER HookProc, PIPF_THUNK pThunk) ;
BOOL __stdcall INIT_Hook_Invalid_Page_Fault (PIPF_HANDLER HookProc, PIPF_THUNK pThunk) ;
BOOL __stdcall DEBUG_Hook_Invalid_Page_Fault (PIPF_HANDLER HookProc, PIPF_THUNK pThunk) ;
BOOL __stdcall STATIC_Hook_Invalid_Page_Fault (PIPF_HANDLER HookProc, PIPF_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Hook_Invalid_Page_Fault (PIPF_HANDLER HookProc, PIPF_THUNK pThunk) ;

PVOID __stdcall LOCK_Hook_Device_V86_API (DWORD devID, PV86Api_HANDLER Callback, PV86Api_THUNK pThunk) ;
PVOID __stdcall INIT_Hook_Device_V86_API (DWORD devID, PV86Api_HANDLER Callback, PV86Api_THUNK pThunk) ;
PVOID __stdcall DEBUG_Hook_Device_V86_API (DWORD devID, PV86Api_HANDLER Callback, PV86Api_THUNK pThunk) ;
PVOID __stdcall STATIC_Hook_Device_V86_API (DWORD devID, PV86Api_HANDLER Callback, PV86Api_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_Hook_Device_V86_API (DWORD devID, PV86Api_HANDLER Callback, PV86Api_THUNK pThunk) ;

PVOID __stdcall LOCK_Hook_Device_Service (DWORD Service, PDeviceService_HANDLER HookProc, PDeviceService_THUNK pThunk) ;
PVOID __stdcall INIT_Hook_Device_Service (DWORD Service, PDeviceService_HANDLER HookProc, PDeviceService_THUNK pThunk) ;
PVOID __stdcall DEBUG_Hook_Device_Service (DWORD Service, PDeviceService_HANDLER HookProc, PDeviceService_THUNK pThunk) ;
PVOID __stdcall STATIC_Hook_Device_Service (DWORD Service, PDeviceService_HANDLER HookProc, PDeviceService_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_Hook_Device_Service (DWORD Service, PDeviceService_HANDLER HookProc, PDeviceService_THUNK pThunk) ;

PVOID __cdecl LOCK__HeapReAllocate (CONST VOID * hAddress, DWORD nBytes, DWORD Flags) ;
PVOID __cdecl INIT__HeapReAllocate (CONST VOID * hAddress, DWORD nBytes, DWORD Flags) ;
PVOID __cdecl DEBUG__HeapReAllocate (CONST VOID * hAddress, DWORD nBytes, DWORD Flags) ;
PVOID __cdecl STATIC__HeapReAllocate (CONST VOID * hAddress, DWORD nBytes, DWORD Flags) ;
PVOID __cdecl PAGEABLE__HeapReAllocate (CONST VOID * hAddress, DWORD nBytes, DWORD Flags) ;

DWORD __cdecl LOCK__HeapGetSize (CONST VOID * hAddress, DWORD Flags) ;
DWORD __cdecl INIT__HeapGetSize (CONST VOID * hAddress, DWORD Flags) ;
DWORD __cdecl DEBUG__HeapGetSize (CONST VOID * hAddress, DWORD Flags) ;
DWORD __cdecl STATIC__HeapGetSize (CONST VOID * hAddress, DWORD Flags) ;
DWORD __cdecl PAGEABLE__HeapGetSize (CONST VOID * hAddress, DWORD Flags) ;

BOOL __cdecl LOCK__HeapFree (CONST VOID * hAddress, DWORD Flags) ;
BOOL __cdecl INIT__HeapFree (CONST VOID * hAddress, DWORD Flags) ;
BOOL __cdecl DEBUG__HeapFree (CONST VOID * hAddress, DWORD Flags) ;
BOOL __cdecl STATIC__HeapFree (CONST VOID * hAddress, DWORD Flags) ;
BOOL __cdecl PAGEABLE__HeapFree (CONST VOID * hAddress, DWORD Flags) ;

PVOID __cdecl LOCK__HeapAllocate (DWORD nBytes, DWORD Flags) ;
PVOID __cdecl INIT__HeapAllocate (DWORD nBytes, DWORD Flags) ;
PVOID __cdecl DEBUG__HeapAllocate (DWORD nBytes, DWORD Flags) ;
PVOID __cdecl STATIC__HeapAllocate (DWORD nBytes, DWORD Flags) ;
PVOID __cdecl PAGEABLE__HeapAllocate (DWORD nBytes, DWORD Flags) ;

DWORD __stdcall LOCK__GetVMPgCount (VMHANDLE hVM, DWORD Flags, PVOID pUnmapped) ;
DWORD __stdcall INIT__GetVMPgCount (VMHANDLE hVM, DWORD Flags, PVOID pUnmapped) ;
DWORD __stdcall DEBUG__GetVMPgCount (VMHANDLE hVM, DWORD Flags, PVOID pUnmapped) ;
DWORD __stdcall STATIC__GetVMPgCount (VMHANDLE hVM, DWORD Flags, PVOID pUnmapped) ;
DWORD __stdcall PAGEABLE__GetVMPgCount (VMHANDLE hVM, DWORD Flags, PVOID pUnmapped) ;

DWORD __stdcall LOCK_Get_VMM_Version (PVOID pDebugRev) ;
DWORD __stdcall INIT_Get_VMM_Version (PVOID pDebugRev) ;
DWORD __stdcall DEBUG_Get_VMM_Version (PVOID pDebugRev) ;
DWORD __stdcall STATIC_Get_VMM_Version (PVOID pDebugRev) ;
DWORD __stdcall PAGEABLE_Get_VMM_Version (PVOID pDebugRev) ;

DWORD __stdcall LOCK_Get_VMM_Reenter_Count (void) ;
DWORD __stdcall INIT_Get_VMM_Reenter_Count (void) ;
DWORD __stdcall DEBUG_Get_VMM_Reenter_Count (void) ;
DWORD __stdcall STATIC_Get_VMM_Reenter_Count (void) ;
DWORD __stdcall PAGEABLE_Get_VMM_Reenter_Count (void) ;

DWORD __stdcall LOCK_Get_VM_Exec_Time (VMHANDLE hVM) ;
DWORD __stdcall INIT_Get_VM_Exec_Time (VMHANDLE hVM) ;
DWORD __stdcall DEBUG_Get_VM_Exec_Time (VMHANDLE hVM) ;
DWORD __stdcall STATIC_Get_VM_Exec_Time (VMHANDLE hVM) ;
DWORD __stdcall PAGEABLE_Get_VM_Exec_Time (VMHANDLE hVM) ;

BOOL __cdecl LOCK__GetV86PageableArray (VMHANDLE hVM, PVOID pArrayBuf, DWORD Flags) ;
BOOL __cdecl INIT__GetV86PageableArray (VMHANDLE hVM, PVOID pArrayBuf, DWORD Flags) ;
BOOL __cdecl DEBUG__GetV86PageableArray (VMHANDLE hVM, PVOID pArrayBuf, DWORD Flags) ;
BOOL __cdecl STATIC__GetV86PageableArray (VMHANDLE hVM, PVOID pArrayBuf, DWORD Flags) ;
BOOL __cdecl PAGEABLE__GetV86PageableArray (VMHANDLE hVM, PVOID pArrayBuf, DWORD Flags) ;

VOID __stdcall LOCK_Get_V86_Int_Vector (DWORD Intnum, PWORD pSegment, PDWORD pOffset) ;
VOID __stdcall INIT_Get_V86_Int_Vector (DWORD Intnum, PWORD pSegment, PDWORD pOffset) ;
VOID __stdcall DEBUG_Get_V86_Int_Vector (DWORD Intnum, PWORD pSegment, PDWORD pOffset) ;
VOID __stdcall STATIC_Get_V86_Int_Vector (DWORD Intnum, PWORD pSegment, PDWORD pOffset) ;
VOID __stdcall PAGEABLE_Get_V86_Int_Vector (DWORD Intnum, PWORD pSegment, PDWORD pOffset) ;

VOID __stdcall LOCK_Get_Time_Slice_Priority (VMHANDLE hVM, PVOID pStatus, PVOID pFG, PVOID pBG, PVOID pTotalUsed) ;
VOID __stdcall INIT_Get_Time_Slice_Priority (VMHANDLE hVM, PVOID pStatus, PVOID pFG, PVOID pBG, PVOID pTotalUsed) ;
VOID __stdcall DEBUG_Get_Time_Slice_Priority (VMHANDLE hVM, PVOID pStatus, PVOID pFG, PVOID pBG, PVOID pTotalUsed) ;
VOID __stdcall STATIC_Get_Time_Slice_Priority (VMHANDLE hVM, PVOID pStatus, PVOID pFG, PVOID pBG, PVOID pTotalUsed) ;
VOID __stdcall PAGEABLE_Get_Time_Slice_Priority (VMHANDLE hVM, PVOID pStatus, PVOID pFG, PVOID pBG, PVOID pTotalUsed) ;

VOID __stdcall LOCK_Get_Time_Slice_Info (PVOID pNumber, PVOID pCurrent, PVOID pIdleCount) ;
VOID __stdcall INIT_Get_Time_Slice_Info (PVOID pNumber, PVOID pCurrent, PVOID pIdleCount) ;
VOID __stdcall DEBUG_Get_Time_Slice_Info (PVOID pNumber, PVOID pCurrent, PVOID pIdleCount) ;
VOID __stdcall STATIC_Get_Time_Slice_Info (PVOID pNumber, PVOID pCurrent, PVOID pIdleCount) ;
VOID __stdcall PAGEABLE_Get_Time_Slice_Info (PVOID pNumber, PVOID pCurrent, PVOID pIdleCount) ;

DWORD __cdecl LOCK_Get_Time_Slice_Granularity (void) ;
DWORD __cdecl INIT_Get_Time_Slice_Granularity (void) ;
DWORD __cdecl DEBUG_Get_Time_Slice_Granularity (void) ;
DWORD __cdecl STATIC_Get_Time_Slice_Granularity (void) ;
DWORD __cdecl PAGEABLE_Get_Time_Slice_Granularity (void) ;

VMHANDLE __stdcall LOCK_Get_Sys_VM_Handle (void) ;
VMHANDLE __stdcall INIT_Get_Sys_VM_Handle (void) ;
VMHANDLE __stdcall DEBUG_Get_Sys_VM_Handle (void) ;
VMHANDLE __stdcall STATIC_Get_Sys_VM_Handle (void) ;
VMHANDLE __stdcall PAGEABLE_Get_Sys_VM_Handle (void) ;

DWORD __cdecl LOCK_Get_System_Time (void) ;
DWORD __cdecl INIT_Get_System_Time (void) ;
DWORD __cdecl DEBUG_Get_System_Time (void) ;
DWORD __cdecl STATIC_Get_System_Time (void) ;
DWORD __cdecl PAGEABLE_Get_System_Time (void) ;

DWORD __cdecl LOCK__GetSetPageOutCount (DWORD NewCount, DWORD Flags) ;
DWORD __cdecl INIT__GetSetPageOutCount (DWORD NewCount, DWORD Flags) ;
DWORD __cdecl DEBUG__GetSetPageOutCount (DWORD NewCount, DWORD Flags) ;
DWORD __cdecl STATIC__GetSetPageOutCount (DWORD NewCount, DWORD Flags) ;
DWORD __cdecl PAGEABLE__GetSetPageOutCount (DWORD NewCount, DWORD Flags) ;

DWORD __cdecl LOCK__GetSysPageCount (DWORD Flags) ;
DWORD __cdecl INIT__GetSysPageCount (DWORD Flags) ;
DWORD __cdecl DEBUG__GetSysPageCount (DWORD Flags) ;
DWORD __cdecl STATIC__GetSysPageCount (DWORD Flags) ;
DWORD __cdecl PAGEABLE__GetSysPageCount (DWORD Flags) ;

VOID __stdcall LOCK_Set_HMA_Info (DWORD Count) ;
VOID __stdcall INIT_Set_HMA_Info (DWORD Count) ;
VOID __stdcall DEBUG_Set_HMA_Info (DWORD Count) ;
VOID __stdcall STATIC_Set_HMA_Info (DWORD Count) ;
VOID __stdcall PAGEABLE_Set_HMA_Info (DWORD Count) ;

VOID __stdcall LOCK_Get_HMA_Info (PBOOL pGlobal, PDWORD pV86Address, PDWORD pCount) ;
VOID __stdcall INIT_Get_HMA_Info (PBOOL pGlobal, PDWORD pV86Address, PDWORD pCount) ;
VOID __stdcall DEBUG_Get_HMA_Info (PBOOL pGlobal, PDWORD pV86Address, PDWORD pCount) ;
VOID __stdcall STATIC_Get_HMA_Info (PBOOL pGlobal, PDWORD pV86Address, PDWORD pCount) ;
VOID __stdcall PAGEABLE_Get_HMA_Info (PBOOL pGlobal, PDWORD pV86Address, PDWORD pCount) ;

BOOL __stdcall LOCK_GetDetailedVMError (VMHANDLE hVM, PVOID pError, PVOID pRefdata) ;
BOOL __stdcall INIT_GetDetailedVMError (VMHANDLE hVM, PVOID pError, PVOID pRefdata) ;
BOOL __stdcall DEBUG_GetDetailedVMError (VMHANDLE hVM, PVOID pError, PVOID pRefdata) ;
BOOL __stdcall STATIC_GetDetailedVMError (VMHANDLE hVM, PVOID pError, PVOID pRefdata) ;
BOOL __stdcall PAGEABLE_GetDetailedVMError (VMHANDLE hVM, PVOID pError, PVOID pRefdata) ;

VOID __stdcall LOCK_SetDetailedVMError (VMHANDLE hVM, DWORD Errcode, CONST VOID * pRefdata) ;
VOID __stdcall INIT_SetDetailedVMError (VMHANDLE hVM, DWORD Errcode, CONST VOID * pRefdata) ;
VOID __stdcall DEBUG_SetDetailedVMError (VMHANDLE hVM, DWORD Errcode, CONST VOID * pRefdata) ;
VOID __stdcall STATIC_SetDetailedVMError (VMHANDLE hVM, DWORD Errcode, CONST VOID * pRefdata) ;
VOID __stdcall PAGEABLE_SetDetailedVMError (VMHANDLE hVM, DWORD Errcode, CONST VOID * pRefdata) ;

WORD __cdecl LOCK_Get_PSP_Segment (void) ;
WORD __cdecl INIT_Get_PSP_Segment (void) ;
WORD __cdecl DEBUG_Get_PSP_Segment (void) ;
WORD __cdecl STATIC_Get_PSP_Segment (void) ;
WORD __cdecl PAGEABLE_Get_PSP_Segment (void) ;

PCHAR __stdcall LOCK_Get_Profile_String (CONST CHAR * Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
PCHAR __stdcall INIT_Get_Profile_String (CONST CHAR * Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
PCHAR __stdcall DEBUG_Get_Profile_String (CONST CHAR * Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
PCHAR __stdcall STATIC_Get_Profile_String (CONST CHAR * Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
PCHAR __stdcall PAGEABLE_Get_Profile_String (CONST CHAR * Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;

DWORD __stdcall LOCK_Get_Profile_Hex_Int (DWORD Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall INIT_Get_Profile_Hex_Int (DWORD Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall DEBUG_Get_Profile_Hex_Int (DWORD Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall STATIC_Get_Profile_Hex_Int (DWORD Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall PAGEABLE_Get_Profile_Hex_Int (DWORD Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;

DWORD __stdcall LOCK_Get_Profile_Fixed_Point (DWORD Default, DWORD Places, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall INIT_Get_Profile_Fixed_Point (DWORD Default, DWORD Places, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall DEBUG_Get_Profile_Fixed_Point (DWORD Default, DWORD Places, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall STATIC_Get_Profile_Fixed_Point (DWORD Default, DWORD Places, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall PAGEABLE_Get_Profile_Fixed_Point (DWORD Default, DWORD Places, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;

DWORD __stdcall LOCK_Get_Profile_Decimal_Int (DWORD Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall INIT_Get_Profile_Decimal_Int (DWORD Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall DEBUG_Get_Profile_Decimal_Int (DWORD Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall STATIC_Get_Profile_Decimal_Int (DWORD Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
DWORD __stdcall PAGEABLE_Get_Profile_Decimal_Int (DWORD Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;

BOOL __stdcall LOCK_Get_Profile_Boolean (BOOL Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
BOOL __stdcall INIT_Get_Profile_Boolean (BOOL Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
BOOL __stdcall DEBUG_Get_Profile_Boolean (BOOL Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
BOOL __stdcall STATIC_Get_Profile_Boolean (BOOL Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;
BOOL __stdcall PAGEABLE_Get_Profile_Boolean (BOOL Default, CONST CHAR * pSection, CONST CHAR * pKeyname, DWORD * pStatus) ;

BOOL __stdcall LOCK_Get_PM_Int_Vector (DWORD Intnum, PWORD pSegment, PDWORD pOffset) ;
BOOL __stdcall INIT_Get_PM_Int_Vector (DWORD Intnum, PWORD pSegment, PDWORD pOffset) ;
BOOL __stdcall DEBUG_Get_PM_Int_Vector (DWORD Intnum, PWORD pSegment, PDWORD pOffset) ;
BOOL __stdcall STATIC_Get_PM_Int_Vector (DWORD Intnum, PWORD pSegment, PDWORD pOffset) ;
BOOL __stdcall PAGEABLE_Get_PM_Int_Vector (DWORD Intnum, PWORD pSegment, PDWORD pOffset) ;

BOOL __stdcall LOCK_Get_PM_Int_Type (DWORD Intnum) ;
BOOL __stdcall INIT_Get_PM_Int_Type (DWORD Intnum) ;
BOOL __stdcall DEBUG_Get_PM_Int_Type (DWORD Intnum) ;
BOOL __stdcall STATIC_Get_PM_Int_Type (DWORD Intnum) ;
BOOL __stdcall PAGEABLE_Get_PM_Int_Type (DWORD Intnum) ;

MEMHANDLE __cdecl LOCK__GetNulPageHandle (void) ;
MEMHANDLE __cdecl INIT__GetNulPageHandle (void) ;
MEMHANDLE __cdecl DEBUG__GetNulPageHandle (void) ;
MEMHANDLE __cdecl STATIC__GetNulPageHandle (void) ;
MEMHANDLE __cdecl PAGEABLE__GetNulPageHandle (void) ;

PVOID __stdcall LOCK_Get_NMI_Handler_Addr (void) ;
PVOID __stdcall INIT_Get_NMI_Handler_Addr (void) ;
PVOID __stdcall DEBUG_Get_NMI_Handler_Addr (void) ;
PVOID __stdcall STATIC_Get_NMI_Handler_Addr (void) ;
PVOID __stdcall PAGEABLE_Get_NMI_Handler_Addr (void) ;

VMHANDLE __stdcall LOCK_Get_Next_VM_Handle (VMHANDLE hVM) ;
VMHANDLE __stdcall INIT_Get_Next_VM_Handle (VMHANDLE hVM) ;
VMHANDLE __stdcall DEBUG_Get_Next_VM_Handle (VMHANDLE hVM) ;
VMHANDLE __stdcall STATIC_Get_Next_VM_Handle (VMHANDLE hVM) ;
VMHANDLE __stdcall PAGEABLE_Get_Next_VM_Handle (VMHANDLE hVM) ;

BOOL __stdcall LOCK_Get_Next_Profile_String (CONST VOID * pProfile, CONST CHAR * pKeyname, PVOID pNext) ;
BOOL __stdcall INIT_Get_Next_Profile_String (CONST VOID * pProfile, CONST CHAR * pKeyname, PVOID pNext) ;
BOOL __stdcall DEBUG_Get_Next_Profile_String (CONST VOID * pProfile, CONST CHAR * pKeyname, PVOID pNext) ;
BOOL __stdcall STATIC_Get_Next_Profile_String (CONST VOID * pProfile, CONST CHAR * pKeyname, PVOID pNext) ;
BOOL __stdcall PAGEABLE_Get_Next_Profile_String (CONST VOID * pProfile, CONST CHAR * pKeyname, PVOID pNext) ;

BOOL __stdcall LOCK_Get_Name_Of_Ugly_TSR (PCHAR pName) ;
BOOL __stdcall INIT_Get_Name_Of_Ugly_TSR (PCHAR pName) ;
BOOL __stdcall DEBUG_Get_Name_Of_Ugly_TSR (PCHAR pName) ;
BOOL __stdcall STATIC_Get_Name_Of_Ugly_TSR (PCHAR pName) ;
BOOL __stdcall PAGEABLE_Get_Name_Of_Ugly_TSR (PCHAR pName) ;

WORD __stdcall LOCK_Get_Mono_Cur_Pos (void) ;
WORD __stdcall INIT_Get_Mono_Cur_Pos (void) ;
WORD __stdcall DEBUG_Get_Mono_Cur_Pos (void) ;
WORD __stdcall STATIC_Get_Mono_Cur_Pos (void) ;
WORD __stdcall PAGEABLE_Get_Mono_Cur_Pos (void) ;

WORD __cdecl LOCK_Get_Mono_Chr (void) ;
WORD __cdecl INIT_Get_Mono_Chr (void) ;
WORD __cdecl DEBUG_Get_Mono_Chr (void) ;
WORD __cdecl STATIC_Get_Mono_Chr (void) ;
WORD __cdecl PAGEABLE_Get_Mono_Chr (void) ;

VOID __stdcall LOCK_Get_Machine_Info (PMACHINE_INFO pInfo) ;
VOID __stdcall INIT_Get_Machine_Info (PMACHINE_INFO pInfo) ;
VOID __stdcall DEBUG_Get_Machine_Info (PMACHINE_INFO pInfo) ;
VOID __stdcall STATIC_Get_Machine_Info (PMACHINE_INFO pInfo) ;
VOID __stdcall PAGEABLE_Get_Machine_Info (PMACHINE_INFO pInfo) ;

DWORD __stdcall LOCK_Get_Last_Updated_VM_Exec_Time (VMHANDLE hVM) ;
DWORD __stdcall INIT_Get_Last_Updated_VM_Exec_Time (VMHANDLE hVM) ;
DWORD __stdcall DEBUG_Get_Last_Updated_VM_Exec_Time (VMHANDLE hVM) ;
DWORD __stdcall STATIC_Get_Last_Updated_VM_Exec_Time (VMHANDLE hVM) ;
DWORD __stdcall PAGEABLE_Get_Last_Updated_VM_Exec_Time (VMHANDLE hVM) ;

DWORD __cdecl LOCK__GetLastV86Page (void) ;
DWORD __cdecl INIT__GetLastV86Page (void) ;
DWORD __cdecl DEBUG__GetLastV86Page (void) ;
DWORD __cdecl STATIC__GetLastV86Page (void) ;
DWORD __cdecl PAGEABLE__GetLastV86Page (void) ;

DWORD __cdecl LOCK_Get_Last_Updated_System_Time (void) ;
DWORD __cdecl INIT_Get_Last_Updated_System_Time (void) ;
DWORD __cdecl DEBUG_Get_Last_Updated_System_Time (void) ;
DWORD __cdecl STATIC_Get_Last_Updated_System_Time (void) ;
DWORD __cdecl PAGEABLE_Get_Last_Updated_System_Time (void) ;

DWORD __cdecl LOCK__GetGlblRng0V86IntBase (void) ;
DWORD __cdecl INIT__GetGlblRng0V86IntBase (void) ;
DWORD __cdecl DEBUG__GetGlblRng0V86IntBase (void) ;
DWORD __cdecl STATIC__GetGlblRng0V86IntBase (void) ;
DWORD __cdecl PAGEABLE__GetGlblRng0V86IntBase (void) ;

DWORD __stdcall LOCK__GetFreePageCount (DWORD Flags, PDWORD pLockablePageCount) ;
DWORD __stdcall INIT__GetFreePageCount (DWORD Flags, PDWORD pLockablePageCount) ;
DWORD __stdcall DEBUG__GetFreePageCount (DWORD Flags, PDWORD pLockablePageCount) ;
DWORD __stdcall STATIC__GetFreePageCount (DWORD Flags, PDWORD pLockablePageCount) ;
DWORD __stdcall PAGEABLE__GetFreePageCount (DWORD Flags, PDWORD pLockablePageCount) ;

BOOL __stdcall LOCK_Get_Fault_Hook_Addrs (DWORD Intnum, PVOID pV86handler, PVOID pPMhandler, PVOID pVMMhandler) ;
BOOL __stdcall INIT_Get_Fault_Hook_Addrs (DWORD Intnum, PVOID pV86handler, PVOID pPMhandler, PVOID pVMMhandler) ;
BOOL __stdcall DEBUG_Get_Fault_Hook_Addrs (DWORD Intnum, PVOID pV86handler, PVOID pPMhandler, PVOID pVMMhandler) ;
BOOL __stdcall STATIC_Get_Fault_Hook_Addrs (DWORD Intnum, PVOID pV86handler, PVOID pPMhandler, PVOID pVMMhandler) ;
BOOL __stdcall PAGEABLE_Get_Fault_Hook_Addrs (DWORD Intnum, PVOID pV86handler, PVOID pPMhandler, PVOID pVMMhandler) ;

PCHAR __stdcall LOCK_Get_Exec_Path (PDWORD pCount) ;
PCHAR __stdcall INIT_Get_Exec_Path (PDWORD pCount) ;
PCHAR __stdcall DEBUG_Get_Exec_Path (PDWORD pCount) ;
PCHAR __stdcall STATIC_Get_Exec_Path (PDWORD pCount) ;
PCHAR __stdcall PAGEABLE_Get_Exec_Path (PDWORD pCount) ;

VMHANDLE __stdcall LOCK_Get_Execution_Focus (void) ;
VMHANDLE __stdcall INIT_Get_Execution_Focus (void) ;
VMHANDLE __stdcall DEBUG_Get_Execution_Focus (void) ;
VMHANDLE __stdcall STATIC_Get_Execution_Focus (void) ;
VMHANDLE __stdcall PAGEABLE_Get_Execution_Focus (void) ;

BOOL __stdcall LOCK_Get_Environment_String (CONST CHAR * Variable, PPCHAR pResult) ;
BOOL __stdcall INIT_Get_Environment_String (CONST CHAR * Variable, PPCHAR pResult) ;
BOOL __stdcall DEBUG_Get_Environment_String (CONST CHAR * Variable, PPCHAR pResult) ;
BOOL __stdcall STATIC_Get_Environment_String (CONST CHAR * Variable, PPCHAR pResult) ;
BOOL __stdcall PAGEABLE_Get_Environment_String (CONST CHAR * Variable, PPCHAR pResult) ;

BOOL __cdecl LOCK__Get_Device_V86_Pages_Array (VMHANDLE hVM, PVOID ArrayBuf, DWORD Flags) ;
BOOL __cdecl INIT__Get_Device_V86_Pages_Array (VMHANDLE hVM, PVOID ArrayBuf, DWORD Flags) ;
BOOL __cdecl DEBUG__Get_Device_V86_Pages_Array (VMHANDLE hVM, PVOID ArrayBuf, DWORD Flags) ;
BOOL __cdecl STATIC__Get_Device_V86_Pages_Array (VMHANDLE hVM, PVOID ArrayBuf, DWORD Flags) ;
BOOL __cdecl PAGEABLE__Get_Device_V86_Pages_Array (VMHANDLE hVM, PVOID ArrayBuf, DWORD Flags) ;

BOOL __stdcall LOCK__GetDescriptor (PVOID pHiword, PVOID pLoword, DWORD Selector, VMHANDLE hVM, DWORD Flags) ;
BOOL __stdcall INIT__GetDescriptor (PVOID pHiword, PVOID pLoword, DWORD Selector, VMHANDLE hVM, DWORD Flags) ;
BOOL __stdcall DEBUG__GetDescriptor (PVOID pHiword, PVOID pLoword, DWORD Selector, VMHANDLE hVM, DWORD Flags) ;
BOOL __stdcall STATIC__GetDescriptor (PVOID pHiword, PVOID pLoword, DWORD Selector, VMHANDLE hVM, DWORD Flags) ;
BOOL __stdcall PAGEABLE__GetDescriptor (PVOID pHiword, PVOID pLoword, DWORD Selector, VMHANDLE hVM, DWORD Flags) ;

VOID __cdecl LOCK__GetDemandPageInfo (PDEMANDINFO DemandInfo, DWORD Flags) ;
VOID __cdecl INIT__GetDemandPageInfo (PDEMANDINFO DemandInfo, DWORD Flags) ;
VOID __cdecl DEBUG__GetDemandPageInfo (PDEMANDINFO DemandInfo, DWORD Flags) ;
VOID __cdecl STATIC__GetDemandPageInfo (PDEMANDINFO DemandInfo, DWORD Flags) ;
VOID __cdecl PAGEABLE__GetDemandPageInfo (PDEMANDINFO DemandInfo, DWORD Flags) ;

BOOL __stdcall LOCK_Get_Debug_Options (DWORD TestChar) ;
BOOL __stdcall INIT_Get_Debug_Options (DWORD TestChar) ;
BOOL __stdcall DEBUG_Get_Debug_Options (DWORD TestChar) ;
BOOL __stdcall STATIC_Get_Debug_Options (DWORD TestChar) ;
BOOL __stdcall PAGEABLE_Get_Debug_Options (DWORD TestChar) ;

VMHANDLE __stdcall LOCK_Get_Cur_VM_Handle (void) ;
VMHANDLE __stdcall INIT_Get_Cur_VM_Handle (void) ;
VMHANDLE __stdcall DEBUG_Get_Cur_VM_Handle (void) ;
VMHANDLE __stdcall STATIC_Get_Cur_VM_Handle (void) ;
VMHANDLE __stdcall PAGEABLE_Get_Cur_VM_Handle (void) ;

BOOL __stdcall LOCK_Get_Crit_Status_No_Block (PDWORD pCount, PVMHANDLE phVM) ;
BOOL __stdcall INIT_Get_Crit_Status_No_Block (PDWORD pCount, PVMHANDLE phVM) ;
BOOL __stdcall DEBUG_Get_Crit_Status_No_Block (PDWORD pCount, PVMHANDLE phVM) ;
BOOL __stdcall STATIC_Get_Crit_Status_No_Block (PDWORD pCount, PVMHANDLE phVM) ;
BOOL __stdcall PAGEABLE_Get_Crit_Status_No_Block (PDWORD pCount, PVMHANDLE phVM) ;

BOOL __stdcall LOCK_Get_Crit_Section_Status (PDWORD pCount, PVMHANDLE phVM) ;
BOOL __stdcall INIT_Get_Crit_Section_Status (PDWORD pCount, PVMHANDLE phVM) ;
BOOL __stdcall DEBUG_Get_Crit_Section_Status (PDWORD pCount, PVMHANDLE phVM) ;
BOOL __stdcall STATIC_Get_Crit_Section_Status (PDWORD pCount, PVMHANDLE phVM) ;
BOOL __stdcall PAGEABLE_Get_Crit_Section_Status (PDWORD pCount, PVMHANDLE phVM) ;

PPMCB __stdcall LOCK_Get_Cur_PM_App_CB (VMHANDLE hVM) ;
PPMCB __stdcall INIT_Get_Cur_PM_App_CB (VMHANDLE hVM) ;
PPMCB __stdcall DEBUG_Get_Cur_PM_App_CB (VMHANDLE hVM) ;
PPMCB __stdcall STATIC_Get_Cur_PM_App_CB (VMHANDLE hVM) ;
PPMCB __stdcall PAGEABLE_Get_Cur_PM_App_CB (VMHANDLE hVM) ;

PCHAR __stdcall LOCK_Get_Config_Directory (void) ;
PCHAR __stdcall INIT_Get_Config_Directory (void) ;
PCHAR __stdcall DEBUG_Get_Config_Directory (void) ;
PCHAR __stdcall STATIC_Get_Config_Directory (void) ;
PCHAR __stdcall PAGEABLE_Get_Config_Directory (void) ;

DWORD __cdecl LOCK__GetFirstV86Page (void) ;
DWORD __cdecl INIT__GetFirstV86Page (void) ;
DWORD __cdecl DEBUG__GetFirstV86Page (void) ;
DWORD __cdecl STATIC__GetFirstV86Page (void) ;
DWORD __cdecl PAGEABLE__GetFirstV86Page (void) ;

WORD __cdecl LOCK__GetAppFlatDSAlias (void) ;
WORD __cdecl INIT__GetAppFlatDSAlias (void) ;
WORD __cdecl DEBUG__GetAppFlatDSAlias (void) ;
WORD __cdecl STATIC__GetAppFlatDSAlias (void) ;
WORD __cdecl PAGEABLE__GetAppFlatDSAlias (void) ;

BOOL __cdecl LOCK__Free_Temp_V86_Data_Area (void) ;
BOOL __cdecl INIT__Free_Temp_V86_Data_Area (void) ;
BOOL __cdecl DEBUG__Free_Temp_V86_Data_Area (void) ;
BOOL __cdecl STATIC__Free_Temp_V86_Data_Area (void) ;
BOOL __cdecl PAGEABLE__Free_Temp_V86_Data_Area (void) ;

BOOL __cdecl LOCK__Free_LDT_Selector (VMHANDLE hVM, DWORD Selector, DWORD Flags) ;
BOOL __cdecl INIT__Free_LDT_Selector (VMHANDLE hVM, DWORD Selector, DWORD Flags) ;
BOOL __cdecl DEBUG__Free_LDT_Selector (VMHANDLE hVM, DWORD Selector, DWORD Flags) ;
BOOL __cdecl STATIC__Free_LDT_Selector (VMHANDLE hVM, DWORD Selector, DWORD Flags) ;
BOOL __cdecl PAGEABLE__Free_LDT_Selector (VMHANDLE hVM, DWORD Selector, DWORD Flags) ;

BOOL __cdecl LOCK__Free_GDT_Selector (DWORD Selector, DWORD Flags) ;
BOOL __cdecl INIT__Free_GDT_Selector (DWORD Selector, DWORD Flags) ;
BOOL __cdecl DEBUG__Free_GDT_Selector (DWORD Selector, DWORD Flags) ;
BOOL __cdecl STATIC__Free_GDT_Selector (DWORD Selector, DWORD Flags) ;
BOOL __cdecl PAGEABLE__Free_GDT_Selector (DWORD Selector, DWORD Flags) ;

#define Fatal_Memory_Error() VMMcall(Fatal_Memory_Error)
VOID __stdcall LOCK_Fatal_Error_Handler (CONST CHAR * MsgPtr, DWORD ErrFlags) ;
VOID __stdcall INIT_Fatal_Error_Handler (CONST CHAR * MsgPtr, DWORD ErrFlags) ;
VOID __stdcall DEBUG_Fatal_Error_Handler (CONST CHAR * MsgPtr, DWORD ErrFlags) ;
VOID __stdcall STATIC_Fatal_Error_Handler (CONST CHAR * MsgPtr, DWORD ErrFlags) ;
VOID __stdcall PAGEABLE_Fatal_Error_Handler (CONST CHAR * MsgPtr, DWORD ErrFlags) ;

VOID __stdcall LOCK_Exec_VxD_Int (DWORD Intnum, PALLREGS pRegisters) ;
VOID __stdcall INIT_Exec_VxD_Int (DWORD Intnum, PALLREGS pRegisters) ;
VOID __stdcall DEBUG_Exec_VxD_Int (DWORD Intnum, PALLREGS pRegisters) ;
VOID __stdcall STATIC_Exec_VxD_Int (DWORD Intnum, PALLREGS pRegisters) ;
VOID __stdcall PAGEABLE_Exec_VxD_Int (DWORD Intnum, PALLREGS pRegisters) ;

VOID __stdcall LOCK_Exec_Int (DWORD Intnum) ;
VOID __stdcall INIT_Exec_Int (DWORD Intnum) ;
VOID __stdcall DEBUG_Exec_Int (DWORD Intnum) ;
VOID __stdcall STATIC_Exec_Int (DWORD Intnum) ;
VOID __stdcall PAGEABLE_Exec_Int (DWORD Intnum) ;

#define End_Use_Locked_PM_Stack() VMMcall(End_Use_Locked_PM_Stack)
VOID __stdcall LOCK_End_Reentrant_Execution (DWORD Count) ;
VOID __stdcall INIT_End_Reentrant_Execution (DWORD Count) ;
VOID __stdcall DEBUG_End_Reentrant_Execution (DWORD Count) ;
VOID __stdcall STATIC_End_Reentrant_Execution (DWORD Count) ;
VOID __stdcall PAGEABLE_End_Reentrant_Execution (DWORD Count) ;

#define End_Nest_Exec() VMMcall(End_Nest_Exec)
#define End_Critical_Section() VMMcall(End_Critical_Section)
BOOL __stdcall LOCK_End_Crit_And_Suspend (void) ;
BOOL __stdcall INIT_End_Crit_And_Suspend (void) ;
BOOL __stdcall DEBUG_End_Crit_And_Suspend (void) ;
BOOL __stdcall STATIC_End_Crit_And_Suspend (void) ;
BOOL __stdcall PAGEABLE_End_Crit_And_Suspend (void) ;

#define Enable_VM_Ints() VMMcall(Enable_VM_Ints)
VOID __stdcall LOCK_Enable_Local_Trapping (VMHANDLE hVM, DWORD Port) ;
VOID __stdcall INIT_Enable_Local_Trapping (VMHANDLE hVM, DWORD Port) ;
VOID __stdcall DEBUG_Enable_Local_Trapping (VMHANDLE hVM, DWORD Port) ;
VOID __stdcall STATIC_Enable_Local_Trapping (VMHANDLE hVM, DWORD Port) ;
VOID __stdcall PAGEABLE_Enable_Local_Trapping (VMHANDLE hVM, DWORD Port) ;

VOID __stdcall LOCK_Enable_Global_Trapping (DWORD Port) ;
VOID __stdcall INIT_Enable_Global_Trapping (DWORD Port) ;
VOID __stdcall DEBUG_Enable_Global_Trapping (DWORD Port) ;
VOID __stdcall STATIC_Enable_Global_Trapping (DWORD Port) ;
VOID __stdcall PAGEABLE_Enable_Global_Trapping (DWORD Port) ;

#define Disable_VM_Ints() VMMcall(Disable_VM_Ints)
VOID __stdcall LOCK_Disable_Local_Trapping (VMHANDLE hVM, DWORD Port) ;
VOID __stdcall INIT_Disable_Local_Trapping (VMHANDLE hVM, DWORD Port) ;
VOID __stdcall DEBUG_Disable_Local_Trapping (VMHANDLE hVM, DWORD Port) ;
VOID __stdcall STATIC_Disable_Local_Trapping (VMHANDLE hVM, DWORD Port) ;
VOID __stdcall PAGEABLE_Disable_Local_Trapping (VMHANDLE hVM, DWORD Port) ;

VOID __stdcall LOCK_Disable_Global_Trapping (DWORD Port) ;
VOID __stdcall INIT_Disable_Global_Trapping (DWORD Port) ;
VOID __stdcall DEBUG_Disable_Global_Trapping (DWORD Port) ;
VOID __stdcall STATIC_Disable_Global_Trapping (DWORD Port) ;
VOID __stdcall PAGEABLE_Disable_Global_Trapping (DWORD Port) ;

VOID __stdcall LOCK_Destroy_Semaphore (SEMHANDLE hSem) ;
VOID __stdcall INIT_Destroy_Semaphore (SEMHANDLE hSem) ;
VOID __stdcall DEBUG_Destroy_Semaphore (SEMHANDLE hSem) ;
VOID __stdcall STATIC_Destroy_Semaphore (SEMHANDLE hSem) ;
VOID __stdcall PAGEABLE_Destroy_Semaphore (SEMHANDLE hSem) ;

BOOL __cdecl LOCK__DeAssign_Device_V86_Pages (DWORD VMLinrPage, DWORD nPages, VMHANDLE hVM, DWORD Flags) ;
BOOL __cdecl INIT__DeAssign_Device_V86_Pages (DWORD VMLinrPage, DWORD nPages, VMHANDLE hVM, DWORD Flags) ;
BOOL __cdecl DEBUG__DeAssign_Device_V86_Pages (DWORD VMLinrPage, DWORD nPages, VMHANDLE hVM, DWORD Flags) ;
BOOL __cdecl STATIC__DeAssign_Device_V86_Pages (DWORD VMLinrPage, DWORD nPages, VMHANDLE hVM, DWORD Flags) ;
BOOL __cdecl PAGEABLE__DeAssign_Device_V86_Pages (DWORD VMLinrPage, DWORD nPages, VMHANDLE hVM, DWORD Flags) ;

#define Clear_Mono_Screen() VMMcall(Clear_Mono_Screen)
VOID __stdcall LOCK_Close_VM (DWORD TimeOut, VMHANDLE hVM, DWORD Flags) ;
VOID __stdcall INIT_Close_VM (DWORD TimeOut, VMHANDLE hVM, DWORD Flags) ;
VOID __stdcall DEBUG_Close_VM (DWORD TimeOut, VMHANDLE hVM, DWORD Flags) ;
VOID __stdcall STATIC_Close_VM (DWORD TimeOut, VMHANDLE hVM, DWORD Flags) ;
VOID __stdcall PAGEABLE_Close_VM (DWORD TimeOut, VMHANDLE hVM, DWORD Flags) ;

BOOL __stdcall LOCK_Convert_Boolean_String (CONST CHAR * String, PBOOL pStatus) ;
BOOL __stdcall INIT_Convert_Boolean_String (CONST CHAR * String, PBOOL pStatus) ;
BOOL __stdcall DEBUG_Convert_Boolean_String (CONST CHAR * String, PBOOL pStatus) ;
BOOL __stdcall STATIC_Convert_Boolean_String (CONST CHAR * String, PBOOL pStatus) ;
BOOL __stdcall PAGEABLE_Convert_Boolean_String (CONST CHAR * String, PBOOL pStatus) ;

DWORD __stdcall LOCK_Convert_Decimal_String (CONST CHAR * String, PPCHAR pTermPtr) ;
DWORD __stdcall INIT_Convert_Decimal_String (CONST CHAR * String, PPCHAR pTermPtr) ;
DWORD __stdcall DEBUG_Convert_Decimal_String (CONST CHAR * String, PPCHAR pTermPtr) ;
DWORD __stdcall STATIC_Convert_Decimal_String (CONST CHAR * String, PPCHAR pTermPtr) ;
DWORD __stdcall PAGEABLE_Convert_Decimal_String (CONST CHAR * String, PPCHAR pTermPtr) ;

DWORD __stdcall LOCK_Convert_Fixed_Point_String (CONST CHAR * String, DWORD Places, PPCHAR pTermPtr) ;
DWORD __stdcall INIT_Convert_Fixed_Point_String (CONST CHAR * String, DWORD Places, PPCHAR pTermPtr) ;
DWORD __stdcall DEBUG_Convert_Fixed_Point_String (CONST CHAR * String, DWORD Places, PPCHAR pTermPtr) ;
DWORD __stdcall STATIC_Convert_Fixed_Point_String (CONST CHAR * String, DWORD Places, PPCHAR pTermPtr) ;
DWORD __stdcall PAGEABLE_Convert_Fixed_Point_String (CONST CHAR * String, DWORD Places, PPCHAR pTermPtr) ;

DWORD __stdcall LOCK_Convert_Hex_String (CONST CHAR * String, PPCHAR pTermPtr) ;
DWORD __stdcall INIT_Convert_Hex_String (CONST CHAR * String, PPCHAR pTermPtr) ;
DWORD __stdcall DEBUG_Convert_Hex_String (CONST CHAR * String, PPCHAR pTermPtr) ;
DWORD __stdcall STATIC_Convert_Hex_String (CONST CHAR * String, PPCHAR pTermPtr) ;
DWORD __stdcall PAGEABLE_Convert_Hex_String (CONST CHAR * String, PPCHAR pTermPtr) ;

BOOL __cdecl LOCK__CopyPageTable (DWORD LinPgNum, DWORD nPages, PPVOID PageBuf, DWORD Flags) ;
BOOL __cdecl INIT__CopyPageTable (DWORD LinPgNum, DWORD nPages, PPVOID PageBuf, DWORD Flags) ;
BOOL __cdecl DEBUG__CopyPageTable (DWORD LinPgNum, DWORD nPages, PPVOID PageBuf, DWORD Flags) ;
BOOL __cdecl STATIC__CopyPageTable (DWORD LinPgNum, DWORD nPages, PPVOID PageBuf, DWORD Flags) ;
BOOL __cdecl PAGEABLE__CopyPageTable (DWORD LinPgNum, DWORD nPages, PPVOID PageBuf, DWORD Flags) ;

#define Crash_Cur_VM() VMMcall(Crash_Cur_VM)
SEMHANDLE __stdcall LOCK_Create_Semaphore (DWORD TokenCount) ;
SEMHANDLE __stdcall INIT_Create_Semaphore (DWORD TokenCount) ;
SEMHANDLE __stdcall DEBUG_Create_Semaphore (DWORD TokenCount) ;
SEMHANDLE __stdcall STATIC_Create_Semaphore (DWORD TokenCount) ;
SEMHANDLE __stdcall PAGEABLE_Create_Semaphore (DWORD TokenCount) ;

DWORD __cdecl LOCK__AddFreePhysPage (DWORD PhysPageNum, DWORD nPages, DWORD flags) ;
DWORD __cdecl INIT__AddFreePhysPage (DWORD PhysPageNum, DWORD nPages, DWORD flags) ;
DWORD __cdecl DEBUG__AddFreePhysPage (DWORD PhysPageNum, DWORD nPages, DWORD flags) ;
DWORD __cdecl STATIC__AddFreePhysPage (DWORD PhysPageNum, DWORD nPages, DWORD flags) ;
DWORD __cdecl PAGEABLE__AddFreePhysPage (DWORD PhysPageNum, DWORD nPages, DWORD flags) ;

BOOL __cdecl LOCK__Add_Global_V86_Data_Area (DWORD LinAddr, DWORD nBytes, DWORD flags) ;
BOOL __cdecl INIT__Add_Global_V86_Data_Area (DWORD LinAddr, DWORD nBytes, DWORD flags) ;
BOOL __cdecl DEBUG__Add_Global_V86_Data_Area (DWORD LinAddr, DWORD nBytes, DWORD flags) ;
BOOL __cdecl STATIC__Add_Global_V86_Data_Area (DWORD LinAddr, DWORD nBytes, DWORD flags) ;
BOOL __cdecl PAGEABLE__Add_Global_V86_Data_Area (DWORD LinAddr, DWORD nBytes, DWORD flags) ;

BOOL __cdecl LOCK__AddInstanceItem (CONST PINSTDATASTRUC InstStruc, DWORD flags) ;
BOOL __cdecl INIT__AddInstanceItem (CONST PINSTDATASTRUC InstStruc, DWORD flags) ;
BOOL __cdecl DEBUG__AddInstanceItem (CONST PINSTDATASTRUC InstStruc, DWORD flags) ;
BOOL __cdecl STATIC__AddInstanceItem (CONST PINSTDATASTRUC InstStruc, DWORD flags) ;
BOOL __cdecl PAGEABLE__AddInstanceItem (CONST PINSTDATASTRUC InstStruc, DWORD flags) ;

VOID __stdcall LOCK_Adjust_Exec_Priority (INT PriorityBoost, VMHANDLE hVM) ;
VOID __stdcall INIT_Adjust_Exec_Priority (INT PriorityBoost, VMHANDLE hVM) ;
VOID __stdcall DEBUG_Adjust_Exec_Priority (INT PriorityBoost, VMHANDLE hVM) ;
VOID __stdcall STATIC_Adjust_Exec_Priority (INT PriorityBoost, VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_Adjust_Exec_Priority (INT PriorityBoost, VMHANDLE hVM) ;

VOID __stdcall LOCK_Adjust_Execution_Time (DWORD Time, VMHANDLE hVM) ;
VOID __stdcall INIT_Adjust_Execution_Time (DWORD Time, VMHANDLE hVM) ;
VOID __stdcall DEBUG_Adjust_Execution_Time (DWORD Time, VMHANDLE hVM) ;
VOID __stdcall STATIC_Adjust_Execution_Time (DWORD Time, VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_Adjust_Execution_Time (DWORD Time, VMHANDLE hVM) ;

DWORD __cdecl LOCK__Allocate_Device_CB_Area (DWORD nBytes, DWORD flags) ;
DWORD __cdecl INIT__Allocate_Device_CB_Area (DWORD nBytes, DWORD flags) ;
DWORD __cdecl DEBUG__Allocate_Device_CB_Area (DWORD nBytes, DWORD flags) ;
DWORD __cdecl STATIC__Allocate_Device_CB_Area (DWORD nBytes, DWORD flags) ;
DWORD __cdecl PAGEABLE__Allocate_Device_CB_Area (DWORD nBytes, DWORD flags) ;

DWORD __cdecl LOCK__Allocate_GDT_Selector (DWORD Desc1, DWORD Desc2, DWORD flags) ;
DWORD __cdecl INIT__Allocate_GDT_Selector (DWORD Desc1, DWORD Desc2, DWORD flags) ;
DWORD __cdecl DEBUG__Allocate_GDT_Selector (DWORD Desc1, DWORD Desc2, DWORD flags) ;
DWORD __cdecl STATIC__Allocate_GDT_Selector (DWORD Desc1, DWORD Desc2, DWORD flags) ;
DWORD __cdecl PAGEABLE__Allocate_GDT_Selector (DWORD Desc1, DWORD Desc2, DWORD flags) ;

PVOID __cdecl LOCK__Allocate_Global_V86_Data_Area (DWORD nBytes, DWORD flags) ;
PVOID __cdecl INIT__Allocate_Global_V86_Data_Area (DWORD nBytes, DWORD flags) ;
PVOID __cdecl DEBUG__Allocate_Global_V86_Data_Area (DWORD nBytes, DWORD flags) ;
PVOID __cdecl STATIC__Allocate_Global_V86_Data_Area (DWORD nBytes, DWORD flags) ;
PVOID __cdecl PAGEABLE__Allocate_Global_V86_Data_Area (DWORD nBytes, DWORD flags) ;

DWORD __stdcall LOCK__Allocate_LDT_Selector (VMHANDLE hVM, DWORD DescDWORD1, DWORD DescDWORD2, DWORD Count, DWORD flags, PDWORD pLDTInfo) ;
DWORD __stdcall INIT__Allocate_LDT_Selector (VMHANDLE hVM, DWORD DescDWORD1, DWORD DescDWORD2, DWORD Count, DWORD flags, PDWORD pLDTInfo) ;
DWORD __stdcall DEBUG__Allocate_LDT_Selector (VMHANDLE hVM, DWORD DescDWORD1, DWORD DescDWORD2, DWORD Count, DWORD flags, PDWORD pLDTInfo) ;
DWORD __stdcall STATIC__Allocate_LDT_Selector (VMHANDLE hVM, DWORD DescDWORD1, DWORD DescDWORD2, DWORD Count, DWORD flags, PDWORD pLDTInfo) ;
DWORD __stdcall PAGEABLE__Allocate_LDT_Selector (VMHANDLE hVM, DWORD DescDWORD1, DWORD DescDWORD2, DWORD Count, DWORD flags, PDWORD pLDTInfo) ;

SEGOFFSET __stdcall LOCK_Allocate_PM_Call_Back (CONST VOID * RefData, PPMCall_HANDLER Callback, PPMCall_THUNK pThunk) ;
SEGOFFSET __stdcall INIT_Allocate_PM_Call_Back (CONST VOID * RefData, PPMCall_HANDLER Callback, PPMCall_THUNK pThunk) ;
SEGOFFSET __stdcall DEBUG_Allocate_PM_Call_Back (CONST VOID * RefData, PPMCall_HANDLER Callback, PPMCall_THUNK pThunk) ;
SEGOFFSET __stdcall STATIC_Allocate_PM_Call_Back (CONST VOID * RefData, PPMCall_HANDLER Callback, PPMCall_THUNK pThunk) ;
SEGOFFSET __stdcall PAGEABLE_Allocate_PM_Call_Back (CONST VOID * RefData, PPMCall_HANDLER Callback, PPMCall_THUNK pThunk) ;

PVOID __cdecl LOCK__Allocate_Temp_V86_Data_Area (DWORD nBytes, DWORD flags) ;
PVOID __cdecl INIT__Allocate_Temp_V86_Data_Area (DWORD nBytes, DWORD flags) ;
PVOID __cdecl DEBUG__Allocate_Temp_V86_Data_Area (DWORD nBytes, DWORD flags) ;
PVOID __cdecl STATIC__Allocate_Temp_V86_Data_Area (DWORD nBytes, DWORD flags) ;
PVOID __cdecl PAGEABLE__Allocate_Temp_V86_Data_Area (DWORD nBytes, DWORD flags) ;

SEGOFFSET __stdcall LOCK_Allocate_V86_Call_Back (CONST VOID * RefData, PV86Call_HANDLER Callback, PV86Call_THUNK pThunk) ;
SEGOFFSET __stdcall INIT_Allocate_V86_Call_Back (CONST VOID * RefData, PV86Call_HANDLER Callback, PV86Call_THUNK pThunk) ;
SEGOFFSET __stdcall DEBUG_Allocate_V86_Call_Back (CONST VOID * RefData, PV86Call_HANDLER Callback, PV86Call_THUNK pThunk) ;
SEGOFFSET __stdcall STATIC_Allocate_V86_Call_Back (CONST VOID * RefData, PV86Call_HANDLER Callback, PV86Call_THUNK pThunk) ;
SEGOFFSET __stdcall PAGEABLE_Allocate_V86_Call_Back (CONST VOID * RefData, PV86Call_HANDLER Callback, PV86Call_THUNK pThunk) ;

BOOL __cdecl LOCK__Assign_Device_V86_Pages (DWORD VMLinrPage, DWORD nPages, VMHANDLE VM, DWORD flags) ;
BOOL __cdecl INIT__Assign_Device_V86_Pages (DWORD VMLinrPage, DWORD nPages, VMHANDLE VM, DWORD flags) ;
BOOL __cdecl DEBUG__Assign_Device_V86_Pages (DWORD VMLinrPage, DWORD nPages, VMHANDLE VM, DWORD flags) ;
BOOL __cdecl STATIC__Assign_Device_V86_Pages (DWORD VMLinrPage, DWORD nPages, VMHANDLE VM, DWORD flags) ;
BOOL __cdecl PAGEABLE__Assign_Device_V86_Pages (DWORD VMLinrPage, DWORD nPages, VMHANDLE VM, DWORD flags) ;

VOID __stdcall LOCK_Begin_Critical_Section (DWORD Flags) ;
VOID __stdcall INIT_Begin_Critical_Section (DWORD Flags) ;
VOID __stdcall DEBUG_Begin_Critical_Section (DWORD Flags) ;
VOID __stdcall STATIC_Begin_Critical_Section (DWORD Flags) ;
VOID __stdcall PAGEABLE_Begin_Critical_Section (DWORD Flags) ;

#define Begin_Nest_Exec() VMMcall(Begin_Nest_Exec)
#define Begin_Nest_V86_Exec() VMMcall(Begin_Nest_V86_Exec)
DWORD __stdcall LOCK_Begin_Reentrant_Execution (void) ;
DWORD __stdcall INIT_Begin_Reentrant_Execution (void) ;
DWORD __stdcall DEBUG_Begin_Reentrant_Execution (void) ;
DWORD __stdcall STATIC_Begin_Reentrant_Execution (void) ;
DWORD __stdcall PAGEABLE_Begin_Reentrant_Execution (void) ;

#define Begin_Use_Locked_PM_Stack() VMMcall(Begin_Use_Locked_PM_Stack)
VOID __stdcall LOCK__BuildDescriptorDWORDs (PVOID pDWlow, PVOID pDWhigh, DWORD descBase, DWORD descLimit, DWORD descType, DWORD descSize, DWORD Flags) ;
VOID __stdcall INIT__BuildDescriptorDWORDs (PVOID pDWlow, PVOID pDWhigh, DWORD descBase, DWORD descLimit, DWORD descType, DWORD descSize, DWORD Flags) ;
VOID __stdcall DEBUG__BuildDescriptorDWORDs (PVOID pDWlow, PVOID pDWhigh, DWORD descBase, DWORD descLimit, DWORD descType, DWORD descSize, DWORD Flags) ;
VOID __stdcall STATIC__BuildDescriptorDWORDs (PVOID pDWlow, PVOID pDWhigh, DWORD descBase, DWORD descLimit, DWORD descType, DWORD descSize, DWORD Flags) ;
VOID __stdcall PAGEABLE__BuildDescriptorDWORDs (PVOID pDWlow, PVOID pDWhigh, DWORD descBase, DWORD descLimit, DWORD descType, DWORD descSize, DWORD Flags) ;

VOID __stdcall LOCK_Build_Int_Stack_Frame (WORD theSegment, DWORD theOffset) ;
VOID __stdcall INIT_Build_Int_Stack_Frame (WORD theSegment, DWORD theOffset) ;
VOID __stdcall DEBUG_Build_Int_Stack_Frame (WORD theSegment, DWORD theOffset) ;
VOID __stdcall STATIC_Build_Int_Stack_Frame (WORD theSegment, DWORD theOffset) ;
VOID __stdcall PAGEABLE_Build_Int_Stack_Frame (WORD theSegment, DWORD theOffset) ;

EVENTHANDLE __stdcall LOCK_Call_Global_Event (PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall INIT_Call_Global_Event (PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall DEBUG_Call_Global_Event (PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall STATIC_Call_Global_Event (PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall PAGEABLE_Call_Global_Event (PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;

EVENTHANDLE __stdcall LOCK_Call_Priority_VM_Event (DWORD PriorityBoost, VMHANDLE hVM, DWORD Flags, CONST VOID * Refdata, PPriorityVMEvent_HANDLER EventCallback, DWORD Timeout, PPriorityVMEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall INIT_Call_Priority_VM_Event (DWORD PriorityBoost, VMHANDLE hVM, DWORD Flags, CONST VOID * Refdata, PPriorityVMEvent_HANDLER EventCallback, DWORD Timeout, PPriorityVMEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall DEBUG_Call_Priority_VM_Event (DWORD PriorityBoost, VMHANDLE hVM, DWORD Flags, CONST VOID * Refdata, PPriorityVMEvent_HANDLER EventCallback, DWORD Timeout, PPriorityVMEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall STATIC_Call_Priority_VM_Event (DWORD PriorityBoost, VMHANDLE hVM, DWORD Flags, CONST VOID * Refdata, PPriorityVMEvent_HANDLER EventCallback, DWORD Timeout, PPriorityVMEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall PAGEABLE_Call_Priority_VM_Event (DWORD PriorityBoost, VMHANDLE hVM, DWORD Flags, CONST VOID * Refdata, PPriorityVMEvent_HANDLER EventCallback, DWORD Timeout, PPriorityVMEvent_THUNK pThunk) ;

EVENTHANDLE __stdcall LOCK_Call_VM_Event (VMHANDLE hVM, PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall INIT_Call_VM_Event (VMHANDLE hVM, PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall DEBUG_Call_VM_Event (VMHANDLE hVM, PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall STATIC_Call_VM_Event (VMHANDLE hVM, PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall PAGEABLE_Call_VM_Event (VMHANDLE hVM, PEvent_HANDLER EventCallback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;

BOOL __stdcall LOCK_Call_When_Idle (PIdle_HANDLER IdleCallback, PIdle_THUNK pThunk) ;
BOOL __stdcall INIT_Call_When_Idle (PIdle_HANDLER IdleCallback, PIdle_THUNK pThunk) ;
BOOL __stdcall DEBUG_Call_When_Idle (PIdle_HANDLER IdleCallback, PIdle_THUNK pThunk) ;
BOOL __stdcall STATIC_Call_When_Idle (PIdle_HANDLER IdleCallback, PIdle_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Call_When_Idle (PIdle_HANDLER IdleCallback, PIdle_THUNK pThunk) ;

VOID __stdcall LOCK_Call_When_Not_Critical (CONST VOID * Refdata, PEvent_HANDLER Callback, PEvent_THUNK pThunk) ;
VOID __stdcall INIT_Call_When_Not_Critical (CONST VOID * Refdata, PEvent_HANDLER Callback, PEvent_THUNK pThunk) ;
VOID __stdcall DEBUG_Call_When_Not_Critical (CONST VOID * Refdata, PEvent_HANDLER Callback, PEvent_THUNK pThunk) ;
VOID __stdcall STATIC_Call_When_Not_Critical (CONST VOID * Refdata, PEvent_HANDLER Callback, PEvent_THUNK pThunk) ;
VOID __stdcall PAGEABLE_Call_When_Not_Critical (CONST VOID * Refdata, PEvent_HANDLER Callback, PEvent_THUNK pThunk) ;

VOID __stdcall LOCK_Call_When_Task_Switched (PTaskSwitched_HANDLER TaskSwitchCallback, PTaskSwitched_THUNK pThunk) ;
VOID __stdcall INIT_Call_When_Task_Switched (PTaskSwitched_HANDLER TaskSwitchCallback, PTaskSwitched_THUNK pThunk) ;
VOID __stdcall DEBUG_Call_When_Task_Switched (PTaskSwitched_HANDLER TaskSwitchCallback, PTaskSwitched_THUNK pThunk) ;
VOID __stdcall STATIC_Call_When_Task_Switched (PTaskSwitched_HANDLER TaskSwitchCallback, PTaskSwitched_THUNK pThunk) ;
VOID __stdcall PAGEABLE_Call_When_Task_Switched (PTaskSwitched_HANDLER TaskSwitchCallback, PTaskSwitched_THUNK pThunk) ;

VOID __stdcall LOCK_Call_When_VM_Ints_Enabled (PEvent_HANDLER Callback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
VOID __stdcall INIT_Call_When_VM_Ints_Enabled (PEvent_HANDLER Callback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
VOID __stdcall DEBUG_Call_When_VM_Ints_Enabled (PEvent_HANDLER Callback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
VOID __stdcall STATIC_Call_When_VM_Ints_Enabled (PEvent_HANDLER Callback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;
VOID __stdcall PAGEABLE_Call_When_VM_Ints_Enabled (PEvent_HANDLER Callback, CONST VOID * Refdata, PEvent_THUNK pThunk) ;

VOID __stdcall LOCK_Call_When_VM_Returns (PVMReturns_HANDLER Callback, CONST VOID * Refdata, DWORD Timeout, PVMReturns_THUNK pThunk) ;
VOID __stdcall INIT_Call_When_VM_Returns (PVMReturns_HANDLER Callback, CONST VOID * Refdata, DWORD Timeout, PVMReturns_THUNK pThunk) ;
VOID __stdcall DEBUG_Call_When_VM_Returns (PVMReturns_HANDLER Callback, CONST VOID * Refdata, DWORD Timeout, PVMReturns_THUNK pThunk) ;
VOID __stdcall STATIC_Call_When_VM_Returns (PVMReturns_HANDLER Callback, CONST VOID * Refdata, DWORD Timeout, PVMReturns_THUNK pThunk) ;
VOID __stdcall PAGEABLE_Call_When_VM_Returns (PVMReturns_HANDLER Callback, CONST VOID * Refdata, DWORD Timeout, PVMReturns_THUNK pThunk) ;

VOID __stdcall LOCK_Cancel_Global_Event (EVENTHANDLE Event) ;
VOID __stdcall INIT_Cancel_Global_Event (EVENTHANDLE Event) ;
VOID __stdcall DEBUG_Cancel_Global_Event (EVENTHANDLE Event) ;
VOID __stdcall STATIC_Cancel_Global_Event (EVENTHANDLE Event) ;
VOID __stdcall PAGEABLE_Cancel_Global_Event (EVENTHANDLE Event) ;

VOID __stdcall LOCK_Cancel_Priority_VM_Event (EVENTHANDLE Event) ;
VOID __stdcall INIT_Cancel_Priority_VM_Event (EVENTHANDLE Event) ;
VOID __stdcall DEBUG_Cancel_Priority_VM_Event (EVENTHANDLE Event) ;
VOID __stdcall STATIC_Cancel_Priority_VM_Event (EVENTHANDLE Event) ;
VOID __stdcall PAGEABLE_Cancel_Priority_VM_Event (EVENTHANDLE Event) ;

VOID __stdcall LOCK_Cancel_Time_Out (TIMEOUTHANDLE TimeOut) ;
VOID __stdcall INIT_Cancel_Time_Out (TIMEOUTHANDLE TimeOut) ;
VOID __stdcall DEBUG_Cancel_Time_Out (TIMEOUTHANDLE TimeOut) ;
VOID __stdcall STATIC_Cancel_Time_Out (TIMEOUTHANDLE TimeOut) ;
VOID __stdcall PAGEABLE_Cancel_Time_Out (TIMEOUTHANDLE TimeOut) ;

VOID __stdcall LOCK_Cancel_VM_Event (VMHANDLE hVM, EVENTHANDLE Event) ;
VOID __stdcall INIT_Cancel_VM_Event (VMHANDLE hVM, EVENTHANDLE Event) ;
VOID __stdcall DEBUG_Cancel_VM_Event (VMHANDLE hVM, EVENTHANDLE Event) ;
VOID __stdcall STATIC_Cancel_VM_Event (VMHANDLE hVM, EVENTHANDLE Event) ;
VOID __stdcall PAGEABLE_Cancel_VM_Event (VMHANDLE hVM, EVENTHANDLE Event) ;

VOID __stdcall LOCK_Claim_Critical_Section (DWORD Claims, DWORD Flags) ;
VOID __stdcall INIT_Claim_Critical_Section (DWORD Claims, DWORD Flags) ;
VOID __stdcall DEBUG_Claim_Critical_Section (DWORD Claims, DWORD Flags) ;
VOID __stdcall STATIC_Claim_Critical_Section (DWORD Claims, DWORD Flags) ;
VOID __stdcall PAGEABLE_Claim_Critical_Section (DWORD Claims, DWORD Flags) ;

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegOpenKey (HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult) ;
LONG __cdecl INIT__RegOpenKey (HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult) ;
LONG __cdecl DEBUG__RegOpenKey (HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult) ;
LONG __cdecl STATIC__RegOpenKey (HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult) ;
LONG __cdecl PAGEABLE__RegOpenKey (HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult) ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegCreateKey (HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult) ;
LONG __cdecl INIT__RegCreateKey (HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult) ;
LONG __cdecl DEBUG__RegCreateKey (HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult) ;
LONG __cdecl STATIC__RegCreateKey (HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult) ;
LONG __cdecl PAGEABLE__RegCreateKey (HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult) ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegDeleteKey (HKEY hkey, PCHAR lpszSubKey) ;
LONG __cdecl INIT__RegDeleteKey (HKEY hkey, PCHAR lpszSubKey) ;
LONG __cdecl DEBUG__RegDeleteKey (HKEY hkey, PCHAR lpszSubKey) ;
LONG __cdecl STATIC__RegDeleteKey (HKEY hkey, PCHAR lpszSubKey) ;
LONG __cdecl PAGEABLE__RegDeleteKey (HKEY hkey, PCHAR lpszSubKey) ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegDeleteValue (HKEY hkey, PCHAR lpszValue) ;
LONG __cdecl INIT__RegDeleteValue (HKEY hkey, PCHAR lpszValue) ;
LONG __cdecl DEBUG__RegDeleteValue (HKEY hkey, PCHAR lpszValue) ;
LONG __cdecl STATIC__RegDeleteValue (HKEY hkey, PCHAR lpszValue) ;
LONG __cdecl PAGEABLE__RegDeleteValue (HKEY hkey, PCHAR lpszValue) ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegEnumKey (HKEY hkey, DWORD iSubKey, PCHAR lpszName, DWORD cchName) ;
LONG __cdecl INIT__RegEnumKey (HKEY hkey, DWORD iSubKey, PCHAR lpszName, DWORD cchName) ;
LONG __cdecl DEBUG__RegEnumKey (HKEY hkey, DWORD iSubKey, PCHAR lpszName, DWORD cchName) ;
LONG __cdecl STATIC__RegEnumKey (HKEY hkey, DWORD iSubKey, PCHAR lpszName, DWORD cchName) ;
LONG __cdecl PAGEABLE__RegEnumKey (HKEY hkey, DWORD iSubKey, PCHAR lpszName, DWORD cchName) ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegEnumValue (HKEY hkey, DWORD iValue, PCHAR lpszValue,PDWORD lpcchValue, PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData, PDWORD lpcbData) ;
LONG __cdecl INIT__RegEnumValue (HKEY hkey, DWORD iValue, PCHAR lpszValue,PDWORD lpcchValue, PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData, PDWORD lpcbData) ;
LONG __cdecl DEBUG__RegEnumValue (HKEY hkey, DWORD iValue, PCHAR lpszValue,PDWORD lpcchValue, PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData, PDWORD lpcbData) ;
LONG __cdecl STATIC__RegEnumValue (HKEY hkey, DWORD iValue, PCHAR lpszValue,PDWORD lpcchValue, PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData, PDWORD lpcbData) ;
LONG __cdecl PAGEABLE__RegEnumValue (HKEY hkey, DWORD iValue, PCHAR lpszValue,PDWORD lpcchValue, PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData, PDWORD lpcbData) ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegFlushKey (HKEY hkey) ;
LONG __cdecl INIT__RegFlushKey (HKEY hkey) ;
LONG __cdecl DEBUG__RegFlushKey (HKEY hkey) ;
LONG __cdecl STATIC__RegFlushKey (HKEY hkey) ;
LONG __cdecl PAGEABLE__RegFlushKey (HKEY hkey) ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegCloseKey (HKEY hkey) ;
LONG __cdecl INIT__RegCloseKey (HKEY hkey) ;
LONG __cdecl DEBUG__RegCloseKey (HKEY hkey) ;
LONG __cdecl STATIC__RegCloseKey (HKEY hkey) ;
LONG __cdecl PAGEABLE__RegCloseKey (HKEY hkey) ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegQueryValue (HKEY hkey, PCHAR lpszSubKey, PCHAR lpszValue,PLONG cbValue) ;
LONG __cdecl INIT__RegQueryValue (HKEY hkey, PCHAR lpszSubKey, PCHAR lpszValue,PLONG cbValue) ;
LONG __cdecl DEBUG__RegQueryValue (HKEY hkey, PCHAR lpszSubKey, PCHAR lpszValue,PLONG cbValue) ;
LONG __cdecl STATIC__RegQueryValue (HKEY hkey, PCHAR lpszSubKey, PCHAR lpszValue,PLONG cbValue) ;
LONG __cdecl PAGEABLE__RegQueryValue (HKEY hkey, PCHAR lpszSubKey, PCHAR lpszValue,PLONG cbValue) ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegQueryValueEx (HKEY hkey, PCHAR lpszValueName,PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData,PDWORD lpcbData) ;
LONG __cdecl INIT__RegQueryValueEx (HKEY hkey, PCHAR lpszValueName,PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData,PDWORD lpcbData) ;
LONG __cdecl DEBUG__RegQueryValueEx (HKEY hkey, PCHAR lpszValueName,PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData,PDWORD lpcbData) ;
LONG __cdecl STATIC__RegQueryValueEx (HKEY hkey, PCHAR lpszValueName,PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData,PDWORD lpcbData) ;
LONG __cdecl PAGEABLE__RegQueryValueEx (HKEY hkey, PCHAR lpszValueName,PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData,PDWORD lpcbData) ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegSetValue (HKEY hkey, PCHAR lpszSubKey, DWORD fdwType,PCHAR lpszData, DWORD cbData) ;
LONG __cdecl INIT__RegSetValue (HKEY hkey, PCHAR lpszSubKey, DWORD fdwType,PCHAR lpszData, DWORD cbData) ;
LONG __cdecl DEBUG__RegSetValue (HKEY hkey, PCHAR lpszSubKey, DWORD fdwType,PCHAR lpszData, DWORD cbData) ;
LONG __cdecl STATIC__RegSetValue (HKEY hkey, PCHAR lpszSubKey, DWORD fdwType,PCHAR lpszData, DWORD cbData) ;
LONG __cdecl PAGEABLE__RegSetValue (HKEY hkey, PCHAR lpszSubKey, DWORD fdwType,PCHAR lpszData, DWORD cbData) ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK__RegSetValueEx (HKEY hkey, PCHAR lpszValueName,DWORD dwReserved, DWORD fdwType, PBYTE lpbDta, DWORD cbData) ;
LONG __cdecl INIT__RegSetValueEx (HKEY hkey, PCHAR lpszValueName,DWORD dwReserved, DWORD fdwType, PBYTE lpbDta, DWORD cbData) ;
LONG __cdecl DEBUG__RegSetValueEx (HKEY hkey, PCHAR lpszValueName,DWORD dwReserved, DWORD fdwType, PBYTE lpbDta, DWORD cbData) ;
LONG __cdecl STATIC__RegSetValueEx (HKEY hkey, PCHAR lpszValueName,DWORD dwReserved, DWORD fdwType, PBYTE lpbDta, DWORD cbData) ;
LONG __cdecl PAGEABLE__RegSetValueEx (HKEY hkey, PCHAR lpszValueName,DWORD dwReserved, DWORD fdwType, PBYTE lpbDta, DWORD cbData) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__EnterMutex (MUTEXHANDLE hMutex, DWORD flags) ;
VOID __cdecl INIT__EnterMutex (MUTEXHANDLE hMutex, DWORD flags) ;
VOID __cdecl DEBUG__EnterMutex (MUTEXHANDLE hMutex, DWORD flags) ;
VOID __cdecl STATIC__EnterMutex (MUTEXHANDLE hMutex, DWORD flags) ;
VOID __cdecl PAGEABLE__EnterMutex (MUTEXHANDLE hMutex, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
THREADHANDLE __cdecl LOCK__GetMutexOwner (MUTEXHANDLE hMutex) ;
THREADHANDLE __cdecl INIT__GetMutexOwner (MUTEXHANDLE hMutex) ;
THREADHANDLE __cdecl DEBUG__GetMutexOwner (MUTEXHANDLE hMutex) ;
THREADHANDLE __cdecl STATIC__GetMutexOwner (MUTEXHANDLE hMutex) ;
THREADHANDLE __cdecl PAGEABLE__GetMutexOwner (MUTEXHANDLE hMutex) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__LeaveMutex (MUTEXHANDLE hMutex) ;
VOID __cdecl INIT__LeaveMutex (MUTEXHANDLE hMutex) ;
VOID __cdecl DEBUG__LeaveMutex (MUTEXHANDLE hMutex) ;
VOID __cdecl STATIC__LeaveMutex (MUTEXHANDLE hMutex) ;
VOID __cdecl PAGEABLE__LeaveMutex (MUTEXHANDLE hMutex) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Unhook_V86_Fault (DWORD IntNum, PV86Fault_HANDLER FaultProc,PV86Fault_THUNK pThunk) ;
BOOL __stdcall INIT_Unhook_V86_Fault (DWORD IntNum, PV86Fault_HANDLER FaultProc,PV86Fault_THUNK pThunk) ;
BOOL __stdcall DEBUG_Unhook_V86_Fault (DWORD IntNum, PV86Fault_HANDLER FaultProc,PV86Fault_THUNK pThunk) ;
BOOL __stdcall STATIC_Unhook_V86_Fault (DWORD IntNum, PV86Fault_HANDLER FaultProc,PV86Fault_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Unhook_V86_Fault (DWORD IntNum, PV86Fault_HANDLER FaultProc,PV86Fault_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Unhook_PM_Fault (DWORD IntNum, PPMFault_HANDLER FaultProc, PPMFault_THUNK pThunk) ;
BOOL __stdcall INIT_Unhook_PM_Fault (DWORD IntNum, PPMFault_HANDLER FaultProc, PPMFault_THUNK pThunk) ;
BOOL __stdcall DEBUG_Unhook_PM_Fault (DWORD IntNum, PPMFault_HANDLER FaultProc, PPMFault_THUNK pThunk) ;
BOOL __stdcall STATIC_Unhook_PM_Fault (DWORD IntNum, PPMFault_HANDLER FaultProc, PPMFault_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Unhook_PM_Fault (DWORD IntNum, PPMFault_HANDLER FaultProc, PPMFault_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Unhook_VMM_Fault (DWORD IntNum, PVMMFault_HANDLER FaultProc, PVMMFault_THUNK pThunk) ;
BOOL __stdcall INIT_Unhook_VMM_Fault (DWORD IntNum, PVMMFault_HANDLER FaultProc, PVMMFault_THUNK pThunk) ;
BOOL __stdcall DEBUG_Unhook_VMM_Fault (DWORD IntNum, PVMMFault_HANDLER FaultProc, PVMMFault_THUNK pThunk) ;
BOOL __stdcall STATIC_Unhook_VMM_Fault (DWORD IntNum, PVMMFault_HANDLER FaultProc, PVMMFault_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Unhook_VMM_Fault (DWORD IntNum, PVMMFault_HANDLER FaultProc, PVMMFault_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Unhook_V86_Int_Chain (DWORD i, PV86Int_HANDLER pFunc, PV86Int_THUNK pThunk) ;
BOOL __stdcall INIT_Unhook_V86_Int_Chain (DWORD i, PV86Int_HANDLER pFunc, PV86Int_THUNK pThunk) ;
BOOL __stdcall DEBUG_Unhook_V86_Int_Chain (DWORD i, PV86Int_HANDLER pFunc, PV86Int_THUNK pThunk) ;
BOOL __stdcall STATIC_Unhook_V86_Int_Chain (DWORD i, PV86Int_HANDLER pFunc, PV86Int_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Unhook_V86_Int_Chain (DWORD i, PV86Int_HANDLER pFunc, PV86Int_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Signal_Semaphore_No_Switch (SEMHANDLE sem) ;
VOID __stdcall INIT_Signal_Semaphore_No_Switch (SEMHANDLE sem) ;
VOID __stdcall DEBUG_Signal_Semaphore_No_Switch (SEMHANDLE sem) ;
VOID __stdcall STATIC_Signal_Semaphore_No_Switch (SEMHANDLE sem) ;
VOID __stdcall PAGEABLE_Signal_Semaphore_No_Switch (SEMHANDLE sem) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Remove_IO_Handler (DWORD iPort) ;
BOOL __stdcall INIT_Remove_IO_Handler (DWORD iPort) ;
BOOL __stdcall DEBUG_Remove_IO_Handler (DWORD iPort) ;
BOOL __stdcall STATIC_Remove_IO_Handler (DWORD iPort) ;
BOOL __stdcall PAGEABLE_Remove_IO_Handler (DWORD iPort) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Unhook_NMI_Event (PNMIEvent_HANDLER NmiProc, PNMIEvent_THUNK pThunk) ;
VOID __stdcall INIT_Unhook_NMI_Event (PNMIEvent_HANDLER NmiProc, PNMIEvent_THUNK pThunk) ;
VOID __stdcall DEBUG_Unhook_NMI_Event (PNMIEvent_HANDLER NmiProc, PNMIEvent_THUNK pThunk) ;
VOID __stdcall STATIC_Unhook_NMI_Event (PNMIEvent_HANDLER NmiProc, PNMIEvent_THUNK pThunk) ;
VOID __stdcall PAGEABLE_Unhook_NMI_Event (PNMIEvent_HANDLER NmiProc, PNMIEvent_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Cancel_Call_When_Not_Critical (PEvent_HANDLER Callback, PEvent_THUNK pThunk) ;
BOOL __stdcall INIT_Cancel_Call_When_Not_Critical (PEvent_HANDLER Callback, PEvent_THUNK pThunk) ;
BOOL __stdcall DEBUG_Cancel_Call_When_Not_Critical (PEvent_HANDLER Callback, PEvent_THUNK pThunk) ;
BOOL __stdcall STATIC_Cancel_Call_When_Not_Critical (PEvent_HANDLER Callback, PEvent_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Cancel_Call_When_Not_Critical (PEvent_HANDLER Callback, PEvent_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Unhook_Device_Service (DWORD Service, PDeviceService_HANDLER HookProc, PDeviceService_THUNK pThunk) ;
BOOL __stdcall INIT_Unhook_Device_Service (DWORD Service, PDeviceService_HANDLER HookProc, PDeviceService_THUNK pThunk) ;
BOOL __stdcall DEBUG_Unhook_Device_Service (DWORD Service, PDeviceService_HANDLER HookProc, PDeviceService_THUNK pThunk) ;
BOOL __stdcall STATIC_Unhook_Device_Service (DWORD Service, PDeviceService_HANDLER HookProc, PDeviceService_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Unhook_Device_Service (DWORD Service, PDeviceService_HANDLER HookProc, PDeviceService_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
TIMEOUTHANDLE __stdcall LOCK_Set_Async_Time_Out (DWORD Time, CONST VOID * Refdata, PASYNCTIMEOUT_HANDLER TimeoutCallback, PASYNCTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall INIT_Set_Async_Time_Out (DWORD Time, CONST VOID * Refdata, PASYNCTIMEOUT_HANDLER TimeoutCallback, PASYNCTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall DEBUG_Set_Async_Time_Out (DWORD Time, CONST VOID * Refdata, PASYNCTIMEOUT_HANDLER TimeoutCallback, PASYNCTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall STATIC_Set_Async_Time_Out (DWORD Time, CONST VOID * Refdata, PASYNCTIMEOUT_HANDLER TimeoutCallback, PASYNCTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall PAGEABLE_Set_Async_Time_Out (DWORD Time, CONST VOID * Refdata, PASYNCTIMEOUT_HANDLER TimeoutCallback, PASYNCTIMEOUT_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
PDDB __stdcall LOCK_Get_DDB (WORD DeviceID, PCHAR pName) ;
PDDB __stdcall INIT_Get_DDB (WORD DeviceID, PCHAR pName) ;
PDDB __stdcall DEBUG_Get_DDB (WORD DeviceID, PCHAR pName) ;
PDDB __stdcall STATIC_Get_DDB (WORD DeviceID, PCHAR pName) ;
PDDB __stdcall PAGEABLE_Get_DDB (WORD DeviceID, PCHAR pName) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK__Trace_Out_Service (PCHAR string) ;
VOID __stdcall INIT__Trace_Out_Service (PCHAR string) ;
VOID __stdcall DEBUG__Trace_Out_Service (PCHAR string) ;
VOID __stdcall STATIC__Trace_Out_Service (PCHAR string) ;
VOID __stdcall PAGEABLE__Trace_Out_Service (PCHAR string) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK__Debug_Out_Service (PCHAR string) ;
VOID __stdcall INIT__Debug_Out_Service (PCHAR string) ;
VOID __stdcall DEBUG__Debug_Out_Service (PCHAR string) ;
VOID __stdcall STATIC__Debug_Out_Service (PCHAR string) ;
VOID __stdcall PAGEABLE__Debug_Out_Service (PCHAR string) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__AllocateThreadDataSlot (void) ;
DWORD __cdecl INIT__AllocateThreadDataSlot (void) ;
DWORD __cdecl DEBUG__AllocateThreadDataSlot (void) ;
DWORD __cdecl STATIC__AllocateThreadDataSlot (void) ;
DWORD __cdecl PAGEABLE__AllocateThreadDataSlot (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__FreeThreadDataSlot (DWORD slotOffset) ;
VOID __cdecl INIT__FreeThreadDataSlot (DWORD slotOffset) ;
VOID __cdecl DEBUG__FreeThreadDataSlot (DWORD slotOffset) ;
VOID __cdecl STATIC__FreeThreadDataSlot (DWORD slotOffset) ;
VOID __cdecl PAGEABLE__FreeThreadDataSlot (DWORD slotOffset) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__GetLastUpdatedThreadExecTime (THREADHANDLE hThread) ;
DWORD __cdecl INIT__GetLastUpdatedThreadExecTime (THREADHANDLE hThread) ;
DWORD __cdecl DEBUG__GetLastUpdatedThreadExecTime (THREADHANDLE hThread) ;
DWORD __cdecl STATIC__GetLastUpdatedThreadExecTime (THREADHANDLE hThread) ;
DWORD __cdecl PAGEABLE__GetLastUpdatedThreadExecTime (THREADHANDLE hThread) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VMMAddImportModuleName (PCHAR countedString, PDWORD pIndex) ;
BOOL __stdcall INIT_VMMAddImportModuleName (PCHAR countedString, PDWORD pIndex) ;
BOOL __stdcall DEBUG_VMMAddImportModuleName (PCHAR countedString, PDWORD pIndex) ;
BOOL __stdcall STATIC_VMMAddImportModuleName (PCHAR countedString, PDWORD pIndex) ;
BOOL __stdcall PAGEABLE_VMMAddImportModuleName (PCHAR countedString, PDWORD pIndex) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VMM_Add_DDB (PDDB pDdb) ;
BOOL __stdcall INIT_VMM_Add_DDB (PDDB pDdb) ;
BOOL __stdcall DEBUG_VMM_Add_DDB (PDDB pDdb) ;
BOOL __stdcall STATIC_VMM_Add_DDB (PDDB pDdb) ;
BOOL __stdcall PAGEABLE_VMM_Add_DDB (PDDB pDdb) ;
#endif

#define Time_Slice_Wake_Sys_VM() VMMcall(Time_Slice_Wake_Sys_VM)
//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VMM_Remove_DDB (PDDB pDdb) ;
BOOL __stdcall INIT_VMM_Remove_DDB (PDDB pDdb) ;
BOOL __stdcall DEBUG_VMM_Remove_DDB (PDDB pDdb) ;
BOOL __stdcall STATIC_VMM_Remove_DDB (PDDB pDdb) ;
BOOL __stdcall PAGEABLE_VMM_Remove_DDB (PDDB pDdb) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Test_VM_Ints_Enabled (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs) ;
BOOL __stdcall INIT_Test_VM_Ints_Enabled (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs) ;
BOOL __stdcall DEBUG_Test_VM_Ints_Enabled (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs) ;
BOOL __stdcall STATIC_Test_VM_Ints_Enabled (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs) ;
BOOL __stdcall PAGEABLE_Test_VM_Ints_Enabled (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__BlockOnID (DWORD id, DWORD flags) ;
VOID __cdecl INIT__BlockOnID (DWORD id, DWORD flags) ;
VOID __cdecl DEBUG__BlockOnID (DWORD id, DWORD flags) ;
VOID __cdecl STATIC__BlockOnID (DWORD id, DWORD flags) ;
VOID __cdecl PAGEABLE__BlockOnID (DWORD id, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
EVENTHANDLE __stdcall LOCK_Schedule_Thread_Event (THREADHANDLE hThread, PThreadEvent_HANDLER EventCallback, CONST VOID * Refdata, PThreadEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall INIT_Schedule_Thread_Event (THREADHANDLE hThread, PThreadEvent_HANDLER EventCallback, CONST VOID * Refdata, PThreadEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall DEBUG_Schedule_Thread_Event (THREADHANDLE hThread, PThreadEvent_HANDLER EventCallback, CONST VOID * Refdata, PThreadEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall STATIC_Schedule_Thread_Event (THREADHANDLE hThread, PThreadEvent_HANDLER EventCallback, CONST VOID * Refdata, PThreadEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall PAGEABLE_Schedule_Thread_Event (THREADHANDLE hThread, PThreadEvent_HANDLER EventCallback, CONST VOID * Refdata, PThreadEvent_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Cancel_Thread_Event (THREADHANDLE hThread, EVENTHANDLE hEvent) ;
VOID __stdcall INIT_Cancel_Thread_Event (THREADHANDLE hThread, EVENTHANDLE hEvent) ;
VOID __stdcall DEBUG_Cancel_Thread_Event (THREADHANDLE hThread, EVENTHANDLE hEvent) ;
VOID __stdcall STATIC_Cancel_Thread_Event (THREADHANDLE hThread, EVENTHANDLE hEvent) ;
VOID __stdcall PAGEABLE_Cancel_Thread_Event (THREADHANDLE hThread, EVENTHANDLE hEvent) ;
#endif

#ifdef WIN40_OR_LATER
TIMEOUTHANDLE __stdcall LOCK_Set_Thread_Time_Out (DWORD Time, THREADHANDLE hThread, CONST VOID * Refdata, PTHREADTIMEOUT_HANDLER TimeoutCallback, PTHREADTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall INIT_Set_Thread_Time_Out (DWORD Time, THREADHANDLE hThread, CONST VOID * Refdata, PTHREADTIMEOUT_HANDLER TimeoutCallback, PTHREADTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall DEBUG_Set_Thread_Time_Out (DWORD Time, THREADHANDLE hThread, CONST VOID * Refdata, PTHREADTIMEOUT_HANDLER TimeoutCallback, PTHREADTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall STATIC_Set_Thread_Time_Out (DWORD Time, THREADHANDLE hThread, CONST VOID * Refdata, PTHREADTIMEOUT_HANDLER TimeoutCallback, PTHREADTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall PAGEABLE_Set_Thread_Time_Out (DWORD Time, THREADHANDLE hThread, CONST VOID * Refdata, PTHREADTIMEOUT_HANDLER TimeoutCallback, PTHREADTIMEOUT_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
MUTEXHANDLE __cdecl LOCK__CreateMutex (LONG Boost, ULONG flags) ;
MUTEXHANDLE __cdecl INIT__CreateMutex (LONG Boost, ULONG flags) ;
MUTEXHANDLE __cdecl DEBUG__CreateMutex (LONG Boost, ULONG flags) ;
MUTEXHANDLE __cdecl STATIC__CreateMutex (LONG Boost, ULONG flags) ;
MUTEXHANDLE __cdecl PAGEABLE__CreateMutex (LONG Boost, ULONG flags) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__DestroyMutex (MUTEXHANDLE hmtx) ;
BOOL __cdecl INIT__DestroyMutex (MUTEXHANDLE hmtx) ;
BOOL __cdecl DEBUG__DestroyMutex (MUTEXHANDLE hmtx) ;
BOOL __cdecl STATIC__DestroyMutex (MUTEXHANDLE hmtx) ;
BOOL __cdecl PAGEABLE__DestroyMutex (MUTEXHANDLE hmtx) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Call_When_Thread_Switched (PTHREADSWITCH_HANDLER callback, PTHREADSWITCH_THUNK pThunk) ;
VOID __stdcall INIT_Call_When_Thread_Switched (PTHREADSWITCH_HANDLER callback, PTHREADSWITCH_THUNK pThunk) ;
VOID __stdcall DEBUG_Call_When_Thread_Switched (PTHREADSWITCH_HANDLER callback, PTHREADSWITCH_THUNK pThunk) ;
VOID __stdcall STATIC_Call_When_Thread_Switched (PTHREADSWITCH_HANDLER callback, PTHREADSWITCH_THUNK pThunk) ;
VOID __stdcall PAGEABLE_Call_When_Thread_Switched (PTHREADSWITCH_HANDLER callback, PTHREADSWITCH_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
THREADHANDLE __cdecl LOCK_VMMCreateThread (DWORD _ss, DWORD _esp, DWORD _cs, DWORD _eip, DWORD _ds, DWORD _es, DWORD ThreadType, PVOID InitCallback, PVOID refData) ;
THREADHANDLE __cdecl INIT_VMMCreateThread (DWORD _ss, DWORD _esp, DWORD _cs, DWORD _eip, DWORD _ds, DWORD _es, DWORD ThreadType, PVOID InitCallback, PVOID refData) ;
THREADHANDLE __cdecl DEBUG_VMMCreateThread (DWORD _ss, DWORD _esp, DWORD _cs, DWORD _eip, DWORD _ds, DWORD _es, DWORD ThreadType, PVOID InitCallback, PVOID refData) ;
THREADHANDLE __cdecl STATIC_VMMCreateThread (DWORD _ss, DWORD _esp, DWORD _cs, DWORD _eip, DWORD _ds, DWORD _es, DWORD ThreadType, PVOID InitCallback, PVOID refData) ;
THREADHANDLE __cdecl PAGEABLE_VMMCreateThread (DWORD _ss, DWORD _esp, DWORD _cs, DWORD _eip, DWORD _ds, DWORD _es, DWORD ThreadType, PVOID InitCallback, PVOID refData) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__GetThreadExecTime (THREADHANDLE hThread) ;
DWORD __cdecl INIT__GetThreadExecTime (THREADHANDLE hThread) ;
DWORD __cdecl DEBUG__GetThreadExecTime (THREADHANDLE hThread) ;
DWORD __cdecl STATIC__GetThreadExecTime (THREADHANDLE hThread) ;
DWORD __cdecl PAGEABLE__GetThreadExecTime (THREADHANDLE hThread) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VMMTerminateThread (THREADHANDLE hThread) ;
VOID __stdcall INIT_VMMTerminateThread (THREADHANDLE hThread) ;
VOID __stdcall DEBUG_VMMTerminateThread (THREADHANDLE hThread) ;
VOID __stdcall STATIC_VMMTerminateThread (THREADHANDLE hThread) ;
VOID __stdcall PAGEABLE_VMMTerminateThread (THREADHANDLE hThread) ;
#endif

#ifdef WIN40_OR_LATER
THREADHANDLE __stdcall LOCK_Get_Cur_Thread_Handle (void) ;
THREADHANDLE __stdcall INIT_Get_Cur_Thread_Handle (void) ;
THREADHANDLE __stdcall DEBUG_Get_Cur_Thread_Handle (void) ;
THREADHANDLE __stdcall STATIC_Get_Cur_Thread_Handle (void) ;
THREADHANDLE __stdcall PAGEABLE_Get_Cur_Thread_Handle (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Test_Cur_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall INIT_Test_Cur_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall DEBUG_Test_Cur_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall STATIC_Test_Cur_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall PAGEABLE_Test_Cur_Thread_Handle (THREADHANDLE hThread) ;
#endif

#ifdef WIN40_OR_LATER
THREADHANDLE __stdcall LOCK_Get_Sys_Thread_Handle (void) ;
THREADHANDLE __stdcall INIT_Get_Sys_Thread_Handle (void) ;
THREADHANDLE __stdcall DEBUG_Get_Sys_Thread_Handle (void) ;
THREADHANDLE __stdcall STATIC_Get_Sys_Thread_Handle (void) ;
THREADHANDLE __stdcall PAGEABLE_Get_Sys_Thread_Handle (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Test_Sys_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall INIT_Test_Sys_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall DEBUG_Test_Sys_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall STATIC_Test_Sys_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall PAGEABLE_Test_Sys_Thread_Handle (THREADHANDLE hThread) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Validate_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall INIT_Validate_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall DEBUG_Validate_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall STATIC_Validate_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall PAGEABLE_Validate_Thread_Handle (THREADHANDLE hThread) ;
#endif

#ifdef WIN40_OR_LATER
THREADHANDLE __stdcall LOCK_Get_Initial_Thread_Handle (VMHANDLE hVM) ;
THREADHANDLE __stdcall INIT_Get_Initial_Thread_Handle (VMHANDLE hVM) ;
THREADHANDLE __stdcall DEBUG_Get_Initial_Thread_Handle (VMHANDLE hVM) ;
THREADHANDLE __stdcall STATIC_Get_Initial_Thread_Handle (VMHANDLE hVM) ;
THREADHANDLE __stdcall PAGEABLE_Get_Initial_Thread_Handle (VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Test_Initial_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall INIT_Test_Initial_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall DEBUG_Test_Initial_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall STATIC_Test_Initial_Thread_Handle (THREADHANDLE hThread) ;
BOOL __stdcall PAGEABLE_Test_Initial_Thread_Handle (THREADHANDLE hThread) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Debug_Test_Valid_Thread_Handle (THREADHANDLE hThread) ;
VOID __stdcall INIT_Debug_Test_Valid_Thread_Handle (THREADHANDLE hThread) ;
VOID __stdcall DEBUG_Debug_Test_Valid_Thread_Handle (THREADHANDLE hThread) ;
VOID __stdcall STATIC_Debug_Test_Valid_Thread_Handle (THREADHANDLE hThread) ;
VOID __stdcall PAGEABLE_Debug_Test_Valid_Thread_Handle (THREADHANDLE hThread) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Debug_Test_Cur_Thread (THREADHANDLE hThread) ;
VOID __stdcall INIT_Debug_Test_Cur_Thread (THREADHANDLE hThread) ;
VOID __stdcall DEBUG_Debug_Test_Cur_Thread (THREADHANDLE hThread) ;
VOID __stdcall STATIC_Debug_Test_Cur_Thread (THREADHANDLE hThread) ;
VOID __stdcall PAGEABLE_Debug_Test_Cur_Thread (THREADHANDLE hThread) ;
#endif

DWORD __cdecl LOCK_VMM_GetSystemInitState () ;
DWORD __cdecl INIT_VMM_GetSystemInitState () ;
DWORD __cdecl DEBUG_VMM_GetSystemInitState () ;
DWORD __cdecl STATIC_VMM_GetSystemInitState () ;
DWORD __cdecl PAGEABLE_VMM_GetSystemInitState () ;

BOOL __stdcall LOCK_Cancel_Call_When_Thread_Switched (PTHREADSWITCH_HANDLER pCallback, PTHREADSWITCH_THUNK pThunk) ;
BOOL __stdcall INIT_Cancel_Call_When_Thread_Switched (PTHREADSWITCH_HANDLER pCallback, PTHREADSWITCH_THUNK pThunk) ;
BOOL __stdcall DEBUG_Cancel_Call_When_Thread_Switched (PTHREADSWITCH_HANDLER pCallback, PTHREADSWITCH_THUNK pThunk) ;
BOOL __stdcall STATIC_Cancel_Call_When_Thread_Switched (PTHREADSWITCH_HANDLER pCallback, PTHREADSWITCH_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Cancel_Call_When_Thread_Switched (PTHREADSWITCH_HANDLER pCallback, PTHREADSWITCH_THUNK pThunk) ;

THREADHANDLE __stdcall LOCK_Get_Next_Thread_Handle (THREADHANDLE hThread) ;
THREADHANDLE __stdcall INIT_Get_Next_Thread_Handle (THREADHANDLE hThread) ;
THREADHANDLE __stdcall DEBUG_Get_Next_Thread_Handle (THREADHANDLE hThread) ;
THREADHANDLE __stdcall STATIC_Get_Next_Thread_Handle (THREADHANDLE hThread) ;
THREADHANDLE __stdcall PAGEABLE_Get_Next_Thread_Handle (THREADHANDLE hThread) ;

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Adjust_Thread_Exec_Priority (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall INIT_Adjust_Thread_Exec_Priority (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall DEBUG_Adjust_Thread_Exec_Priority (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall STATIC_Adjust_Thread_Exec_Priority (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall PAGEABLE_Adjust_Thread_Exec_Priority (THREADHANDLE hThread, LONG boost) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__Deallocate_Device_CB_Area (DWORD cbOffset, DWORD flags) ;
VOID __cdecl INIT__Deallocate_Device_CB_Area (DWORD cbOffset, DWORD flags) ;
VOID __cdecl DEBUG__Deallocate_Device_CB_Area (DWORD cbOffset, DWORD flags) ;
VOID __cdecl STATIC__Deallocate_Device_CB_Area (DWORD cbOffset, DWORD flags) ;
VOID __cdecl PAGEABLE__Deallocate_Device_CB_Area (DWORD cbOffset, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Remove_Mult_IO_Handlers (PVOID IOTable) ;
VOID __stdcall INIT_Remove_Mult_IO_Handlers (PVOID IOTable) ;
VOID __stdcall DEBUG_Remove_Mult_IO_Handlers (PVOID IOTable) ;
VOID __stdcall STATIC_Remove_Mult_IO_Handlers (PVOID IOTable) ;
VOID __stdcall PAGEABLE_Remove_Mult_IO_Handlers (PVOID IOTable) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK__PageReserve (DWORD ipage, DWORD npages, DWORD flags) ;
PVOID __cdecl INIT__PageReserve (DWORD ipage, DWORD npages, DWORD flags) ;
PVOID __cdecl DEBUG__PageReserve (DWORD ipage, DWORD npages, DWORD flags) ;
PVOID __cdecl STATIC__PageReserve (DWORD ipage, DWORD npages, DWORD flags) ;
PVOID __cdecl PAGEABLE__PageReserve (DWORD ipage, DWORD npages, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__PageCommit (DWORD ipage, DWORD npages, PAGERHANDLE hpd, DWORD pagerdata, DWORD flags) ;
BOOL __cdecl INIT__PageCommit (DWORD ipage, DWORD npages, PAGERHANDLE hpd, DWORD pagerdata, DWORD flags) ;
BOOL __cdecl DEBUG__PageCommit (DWORD ipage, DWORD npages, PAGERHANDLE hpd, DWORD pagerdata, DWORD flags) ;
BOOL __cdecl STATIC__PageCommit (DWORD ipage, DWORD npages, PAGERHANDLE hpd, DWORD pagerdata, DWORD flags) ;
BOOL __cdecl PAGEABLE__PageCommit (DWORD ipage, DWORD npages, PAGERHANDLE hpd, DWORD pagerdata, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__PageDecommit (DWORD ipage, DWORD npages, DWORD flags) ;
BOOL __cdecl INIT__PageDecommit (DWORD ipage, DWORD npages, DWORD flags) ;
BOOL __cdecl DEBUG__PageDecommit (DWORD ipage, DWORD npages, DWORD flags) ;
BOOL __cdecl STATIC__PageDecommit (DWORD ipage, DWORD npages, DWORD flags) ;
BOOL __cdecl PAGEABLE__PageDecommit (DWORD ipage, DWORD npages, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
PAGERHANDLE __cdecl LOCK__PagerRegister (PPD ppd) ;
PAGERHANDLE __cdecl INIT__PagerRegister (PPD ppd) ;
PAGERHANDLE __cdecl DEBUG__PagerRegister (PPD ppd) ;
PAGERHANDLE __cdecl STATIC__PagerRegister (PPD ppd) ;
PAGERHANDLE __cdecl PAGEABLE__PagerRegister (PPD ppd) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__PagerQuery (PAGERHANDLE hpd, PPD ppd) ;
BOOL __cdecl INIT__PagerQuery (PAGERHANDLE hpd, PPD ppd) ;
BOOL __cdecl DEBUG__PagerQuery (PAGERHANDLE hpd, PPD ppd) ;
BOOL __cdecl STATIC__PagerQuery (PAGERHANDLE hpd, PPD ppd) ;
BOOL __cdecl PAGEABLE__PagerQuery (PAGERHANDLE hpd, PPD ppd) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__PagerDeregister (PAGERHANDLE hpd) ;
BOOL __cdecl INIT__PagerDeregister (PAGERHANDLE hpd) ;
BOOL __cdecl DEBUG__PagerDeregister (PAGERHANDLE hpd) ;
BOOL __cdecl STATIC__PagerDeregister (PAGERHANDLE hpd) ;
BOOL __cdecl PAGEABLE__PagerDeregister (PAGERHANDLE hpd) ;
#endif

#ifdef WIN40_OR_LATER
CONTEXTHANDLE __cdecl LOCK__ContextCreate (void) ;
CONTEXTHANDLE __cdecl INIT__ContextCreate (void) ;
CONTEXTHANDLE __cdecl DEBUG__ContextCreate (void) ;
CONTEXTHANDLE __cdecl STATIC__ContextCreate (void) ;
CONTEXTHANDLE __cdecl PAGEABLE__ContextCreate (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__ContextDestroy (CONTEXTHANDLE hcd) ;
BOOL __cdecl INIT__ContextDestroy (CONTEXTHANDLE hcd) ;
BOOL __cdecl DEBUG__ContextDestroy (CONTEXTHANDLE hcd) ;
BOOL __cdecl STATIC__ContextDestroy (CONTEXTHANDLE hcd) ;
BOOL __cdecl PAGEABLE__ContextDestroy (CONTEXTHANDLE hcd) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__PageAttach (DWORD ipage, CONTEXTHANDLE hcontextsrc, DWORD npages) ;
BOOL __cdecl INIT__PageAttach (DWORD ipage, CONTEXTHANDLE hcontextsrc, DWORD npages) ;
BOOL __cdecl DEBUG__PageAttach (DWORD ipage, CONTEXTHANDLE hcontextsrc, DWORD npages) ;
BOOL __cdecl STATIC__PageAttach (DWORD ipage, CONTEXTHANDLE hcontextsrc, DWORD npages) ;
BOOL __cdecl PAGEABLE__PageAttach (DWORD ipage, CONTEXTHANDLE hcontextsrc, DWORD npages) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__PageFlush (DWORD ipage, DWORD npages) ;
BOOL __cdecl INIT__PageFlush (DWORD ipage, DWORD npages) ;
BOOL __cdecl DEBUG__PageFlush (DWORD ipage, DWORD npages) ;
BOOL __cdecl STATIC__PageFlush (DWORD ipage, DWORD npages) ;
BOOL __cdecl PAGEABLE__PageFlush (DWORD ipage, DWORD npages) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__SignalID (DWORD id) ;
VOID __cdecl INIT__SignalID (DWORD id) ;
VOID __cdecl DEBUG__SignalID (DWORD id) ;
VOID __cdecl STATIC__SignalID (DWORD id) ;
VOID __cdecl PAGEABLE__SignalID (DWORD id) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__PageCommitPhys (DWORD ipage, DWORD npages, DWORD physpage, DWORD flags) ;
BOOL __cdecl INIT__PageCommitPhys (DWORD ipage, DWORD npages, DWORD physpage, DWORD flags) ;
BOOL __cdecl DEBUG__PageCommitPhys (DWORD ipage, DWORD npages, DWORD physpage, DWORD flags) ;
BOOL __cdecl STATIC__PageCommitPhys (DWORD ipage, DWORD npages, DWORD physpage, DWORD flags) ;
BOOL __cdecl PAGEABLE__PageCommitPhys (DWORD ipage, DWORD npages, DWORD physpage, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__Register_Win32_Services (PVOID pServiceTable, PDDB pDDB) ;
VOID __cdecl INIT__Register_Win32_Services (PVOID pServiceTable, PDDB pDDB) ;
VOID __cdecl DEBUG__Register_Win32_Services (PVOID pServiceTable, PDDB pDDB) ;
VOID __cdecl STATIC__Register_Win32_Services (PVOID pServiceTable, PDDB pDDB) ;
VOID __cdecl PAGEABLE__Register_Win32_Services (PVOID pServiceTable, PDDB pDDB) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Cancel_Call_When_Idle (PIdle_HANDLER pCallback, PIdle_THUNK pThunk) ;
BOOL __stdcall INIT_Cancel_Call_When_Idle (PIdle_HANDLER pCallback, PIdle_THUNK pThunk) ;
BOOL __stdcall DEBUG_Cancel_Call_When_Idle (PIdle_HANDLER pCallback, PIdle_THUNK pThunk) ;
BOOL __stdcall STATIC_Cancel_Call_When_Idle (PIdle_HANDLER pCallback, PIdle_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Cancel_Call_When_Idle (PIdle_HANDLER pCallback, PIdle_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Cancel_Call_When_Task_Switched (PTaskSwitched_HANDLER pCallback, PTaskSwitched_THUNK pThunk) ;
BOOL __stdcall INIT_Cancel_Call_When_Task_Switched (PTaskSwitched_HANDLER pCallback, PTaskSwitched_THUNK pThunk) ;
BOOL __stdcall DEBUG_Cancel_Call_When_Task_Switched (PTaskSwitched_HANDLER pCallback, PTaskSwitched_THUNK pThunk) ;
BOOL __stdcall STATIC_Cancel_Call_When_Task_Switched (PTaskSwitched_HANDLER pCallback, PTaskSwitched_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Cancel_Call_When_Task_Switched (PTaskSwitched_HANDLER pCallback, PTaskSwitched_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
PSEGOFFSET __stdcall LOCK_Get_Inst_V86_Int_Vec_Base (VMHANDLE hVM) ;
PSEGOFFSET __stdcall INIT_Get_Inst_V86_Int_Vec_Base (VMHANDLE hVM) ;
PSEGOFFSET __stdcall DEBUG_Get_Inst_V86_Int_Vec_Base (VMHANDLE hVM) ;
PSEGOFFSET __stdcall STATIC_Get_Inst_V86_Int_Vec_Base (VMHANDLE hVM) ;
PSEGOFFSET __stdcall PAGEABLE_Get_Inst_V86_Int_Vec_Base (VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_Simulate_VM_IO (DWORD PortID, DWORD IOType, DWORD OutputData) ;
DWORD __stdcall INIT_Simulate_VM_IO (DWORD PortID, DWORD IOType, DWORD OutputData) ;
DWORD __stdcall DEBUG_Simulate_VM_IO (DWORD PortID, DWORD IOType, DWORD OutputData) ;
DWORD __stdcall STATIC_Simulate_VM_IO (DWORD PortID, DWORD IOType, DWORD OutputData) ;
DWORD __stdcall PAGEABLE_Simulate_VM_IO (DWORD PortID, DWORD IOType, DWORD OutputData) ;
#endif

#ifdef WIN40_OR_LATER
CONTEXTHANDLE __cdecl LOCK__ContextSwitch (CONTEXTHANDLE hcd) ;
CONTEXTHANDLE __cdecl INIT__ContextSwitch (CONTEXTHANDLE hcd) ;
CONTEXTHANDLE __cdecl DEBUG__ContextSwitch (CONTEXTHANDLE hcd) ;
CONTEXTHANDLE __cdecl STATIC__ContextSwitch (CONTEXTHANDLE hcd) ;
CONTEXTHANDLE __cdecl PAGEABLE__ContextSwitch (CONTEXTHANDLE hcd) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__PageModifyPermissions (DWORD ipage, DWORD npages, DWORD permand, DWORD permor) ;
DWORD __cdecl INIT__PageModifyPermissions (DWORD ipage, DWORD npages, DWORD permand, DWORD permor) ;
DWORD __cdecl DEBUG__PageModifyPermissions (DWORD ipage, DWORD npages, DWORD permand, DWORD permor) ;
DWORD __cdecl STATIC__PageModifyPermissions (DWORD ipage, DWORD npages, DWORD permand, DWORD permor) ;
DWORD __cdecl PAGEABLE__PageModifyPermissions (DWORD ipage, DWORD npages, DWORD permand, DWORD permor) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__PageQuery (DWORD pbase, PMEMORY_BASIC_INFORMATION pmbi, DWORD cbmbi) ;
DWORD __cdecl INIT__PageQuery (DWORD pbase, PMEMORY_BASIC_INFORMATION pmbi, DWORD cbmbi) ;
DWORD __cdecl DEBUG__PageQuery (DWORD pbase, PMEMORY_BASIC_INFORMATION pmbi, DWORD cbmbi) ;
DWORD __cdecl STATIC__PageQuery (DWORD pbase, PMEMORY_BASIC_INFORMATION pmbi, DWORD cbmbi) ;
DWORD __cdecl PAGEABLE__PageQuery (DWORD pbase, PMEMORY_BASIC_INFORMATION pmbi, DWORD cbmbi) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__GetThreadTerminationStatus (THREADHANDLE hThread) ;
DWORD __cdecl INIT__GetThreadTerminationStatus (THREADHANDLE hThread) ;
DWORD __cdecl DEBUG__GetThreadTerminationStatus (THREADHANDLE hThread) ;
DWORD __cdecl STATIC__GetThreadTerminationStatus (THREADHANDLE hThread) ;
DWORD __cdecl PAGEABLE__GetThreadTerminationStatus (THREADHANDLE hThread) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__GetInstanceInfo (PVOID pRegion, DWORD cbSize) ;
DWORD __cdecl INIT__GetInstanceInfo (PVOID pRegion, DWORD cbSize) ;
DWORD __cdecl DEBUG__GetInstanceInfo (PVOID pRegion, DWORD cbSize) ;
DWORD __cdecl STATIC__GetInstanceInfo (PVOID pRegion, DWORD cbSize) ;
DWORD __cdecl PAGEABLE__GetInstanceInfo (PVOID pRegion, DWORD cbSize) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK__ExecIntMustComplete (DWORD interrupt) ;
VOID __stdcall INIT__ExecIntMustComplete (DWORD interrupt) ;
VOID __stdcall DEBUG__ExecIntMustComplete (DWORD interrupt) ;
VOID __stdcall STATIC__ExecIntMustComplete (DWORD interrupt) ;
VOID __stdcall PAGEABLE__ExecIntMustComplete (DWORD interrupt) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK__ExecVxDIntMustComplete (DWORD Intnum, PALLREGS pRegisters) ;
VOID __stdcall INIT__ExecVxDIntMustComplete (DWORD Intnum, PALLREGS pRegisters) ;
VOID __stdcall DEBUG__ExecVxDIntMustComplete (DWORD Intnum, PALLREGS pRegisters) ;
VOID __stdcall STATIC__ExecVxDIntMustComplete (DWORD Intnum, PALLREGS pRegisters) ;
VOID __stdcall PAGEABLE__ExecVxDIntMustComplete (DWORD Intnum, PALLREGS pRegisters) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Begin_V86_Serialization (DWORD dwFlags) ;
VOID __stdcall INIT_Begin_V86_Serialization (DWORD dwFlags) ;
VOID __stdcall DEBUG_Begin_V86_Serialization (DWORD dwFlags) ;
VOID __stdcall STATIC_Begin_V86_Serialization (DWORD dwFlags) ;
VOID __stdcall PAGEABLE_Begin_V86_Serialization (DWORD dwFlags) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Unhook_V86_Page (DWORD PageNum, PV86Page_HANDLER HookProc, PV86Page_THUNK pThunk) ;
BOOL __stdcall INIT_Unhook_V86_Page (DWORD PageNum, PV86Page_HANDLER HookProc, PV86Page_THUNK pThunk) ;
BOOL __stdcall DEBUG_Unhook_V86_Page (DWORD PageNum, PV86Page_HANDLER HookProc, PV86Page_THUNK pThunk) ;
BOOL __stdcall STATIC_Unhook_V86_Page (DWORD PageNum, PV86Page_HANDLER HookProc, PV86Page_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Unhook_V86_Page (DWORD PageNum, PV86Page_HANDLER HookProc, PV86Page_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __stdcall LOCK_VMM_GetVxDLocationList (PDWORD pVxDCount, PDWORD pTableSize) ;
PVOID __stdcall INIT_VMM_GetVxDLocationList (PDWORD pVxDCount, PDWORD pTableSize) ;
PVOID __stdcall DEBUG_VMM_GetVxDLocationList (PDWORD pVxDCount, PDWORD pTableSize) ;
PVOID __stdcall STATIC_VMM_GetVxDLocationList (PDWORD pVxDCount, PDWORD pTableSize) ;
PVOID __stdcall PAGEABLE_VMM_GetVxDLocationList (PDWORD pVxDCount, PDWORD pTableSize) ;
#endif

#ifdef WIN40_OR_LATER
PDDB __cdecl LOCK_VMM_GetDDBList (void) ;
PDDB __cdecl INIT_VMM_GetDDBList (void) ;
PDDB __cdecl DEBUG_VMM_GetDDBList (void) ;
PDDB __cdecl STATIC_VMM_GetDDBList (void) ;
PDDB __cdecl PAGEABLE_VMM_GetDDBList (void) ;
#endif

#ifdef WIN40_OR_LATER
SEGOFFSET __stdcall LOCK_Get_Instanced_V86_Int_Vector (BYTE intNum, VMHANDLE hVM) ;
SEGOFFSET __stdcall INIT_Get_Instanced_V86_Int_Vector (BYTE intNum, VMHANDLE hVM) ;
SEGOFFSET __stdcall DEBUG_Get_Instanced_V86_Int_Vector (BYTE intNum, VMHANDLE hVM) ;
SEGOFFSET __stdcall STATIC_Get_Instanced_V86_Int_Vector (BYTE intNum, VMHANDLE hVM) ;
SEGOFFSET __stdcall PAGEABLE_Get_Instanced_V86_Int_Vector (BYTE intNum, VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
WORD __stdcall LOCK_Get_Set_Real_DOS_PSP (VMHANDLE hVM, WORD newPSP, DWORD dwFlags) ;
WORD __stdcall INIT_Get_Set_Real_DOS_PSP (VMHANDLE hVM, WORD newPSP, DWORD dwFlags) ;
WORD __stdcall DEBUG_Get_Set_Real_DOS_PSP (VMHANDLE hVM, WORD newPSP, DWORD dwFlags) ;
WORD __stdcall STATIC_Get_Set_Real_DOS_PSP (VMHANDLE hVM, WORD newPSP, DWORD dwFlags) ;
WORD __stdcall PAGEABLE_Get_Set_Real_DOS_PSP (VMHANDLE hVM, WORD newPSP, DWORD dwFlags) ;
#endif

#ifdef WIN40_OR_LATER
PDWORD __cdecl LOCK_Get_System_Time_Address (void) ;
PDWORD __cdecl INIT_Get_System_Time_Address (void) ;
PDWORD __cdecl DEBUG_Get_System_Time_Address (void) ;
PDWORD __cdecl STATIC_Get_System_Time_Address (void) ;
PDWORD __cdecl PAGEABLE_Get_System_Time_Address (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Get_Crit_Status_Thread (PTHREADHANDLE phThread, PDWORD pClaimCount) ;
BOOL __stdcall INIT_Get_Crit_Status_Thread (PTHREADHANDLE phThread, PDWORD pClaimCount) ;
BOOL __stdcall DEBUG_Get_Crit_Status_Thread (PTHREADHANDLE phThread, PDWORD pClaimCount) ;
BOOL __stdcall STATIC_Get_Crit_Status_Thread (PTHREADHANDLE phThread, PDWORD pClaimCount) ;
BOOL __stdcall PAGEABLE_Get_Crit_Status_Thread (PTHREADHANDLE phThread, PDWORD pClaimCount) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Directed_Sys_Control (PDDB pDDB, DWORD iCtrlMsg, PALLREGS pRegisters) ;
VOID __stdcall INIT_Directed_Sys_Control (PDDB pDDB, DWORD iCtrlMsg, PALLREGS pRegisters) ;
VOID __stdcall DEBUG_Directed_Sys_Control (PDDB pDDB, DWORD iCtrlMsg, PALLREGS pRegisters) ;
VOID __stdcall STATIC_Directed_Sys_Control (PDDB pDDB, DWORD iCtrlMsg, PALLREGS pRegisters) ;
VOID __stdcall PAGEABLE_Directed_Sys_Control (PDDB pDDB, DWORD iCtrlMsg, PALLREGS pRegisters) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK__CallRing3 (DWORD _cs, DWORD _ip, DWORD cbArgs, PVOID lpvArgs) ;
BOOL __stdcall INIT__CallRing3 (DWORD _cs, DWORD _ip, DWORD cbArgs, PVOID lpvArgs) ;
BOOL __stdcall DEBUG__CallRing3 (DWORD _cs, DWORD _ip, DWORD cbArgs, PVOID lpvArgs) ;
BOOL __stdcall STATIC__CallRing3 (DWORD _cs, DWORD _ip, DWORD cbArgs, PVOID lpvArgs) ;
BOOL __stdcall PAGEABLE__CallRing3 (DWORD _cs, DWORD _ip, DWORD cbArgs, PVOID lpvArgs) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Exec_PM_Int (DWORD interrupt) ;
VOID __stdcall INIT_Exec_PM_Int (DWORD interrupt) ;
VOID __stdcall DEBUG_Exec_PM_Int (DWORD interrupt) ;
VOID __stdcall STATIC_Exec_PM_Int (DWORD interrupt) ;
VOID __stdcall PAGEABLE_Exec_PM_Int (DWORD interrupt) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK__PageCommitContig (DWORD ipage, DWORD nPages, DWORD flags, DWORD alignmask, DWORD minphys, DWORD maxphys) ;
PVOID __cdecl INIT__PageCommitContig (DWORD ipage, DWORD nPages, DWORD flags, DWORD alignmask, DWORD minphys, DWORD maxphys) ;
PVOID __cdecl DEBUG__PageCommitContig (DWORD ipage, DWORD nPages, DWORD flags, DWORD alignmask, DWORD minphys, DWORD maxphys) ;
PVOID __cdecl STATIC__PageCommitContig (DWORD ipage, DWORD nPages, DWORD flags, DWORD alignmask, DWORD minphys, DWORD maxphys) ;
PVOID __cdecl PAGEABLE__PageCommitContig (DWORD ipage, DWORD nPages, DWORD flags, DWORD alignmask, DWORD minphys, DWORD maxphys) ;
#endif

#ifdef WIN40_OR_LATER
CONTEXTHANDLE __cdecl LOCK__GetCurrentContext (void) ;
CONTEXTHANDLE __cdecl INIT__GetCurrentContext (void) ;
CONTEXTHANDLE __cdecl DEBUG__GetCurrentContext (void) ;
CONTEXTHANDLE __cdecl STATIC__GetCurrentContext (void) ;
CONTEXTHANDLE __cdecl PAGEABLE__GetCurrentContext (void) ;
#endif

#ifdef WIN40_OR_LATER
EVENTHANDLE __stdcall LOCK_Call_Restricted_Event (LONG boost, HANDLE hVMorThread, DWORD flags, PVOID refData, PRestrictedEvent_HANDLER pCallback, DWORD timeout, PRestrictedEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall INIT_Call_Restricted_Event (LONG boost, HANDLE hVMorThread, DWORD flags, PVOID refData, PRestrictedEvent_HANDLER pCallback, DWORD timeout, PRestrictedEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall DEBUG_Call_Restricted_Event (LONG boost, HANDLE hVMorThread, DWORD flags, PVOID refData, PRestrictedEvent_HANDLER pCallback, DWORD timeout, PRestrictedEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall STATIC_Call_Restricted_Event (LONG boost, HANDLE hVMorThread, DWORD flags, PVOID refData, PRestrictedEvent_HANDLER pCallback, DWORD timeout, PRestrictedEvent_THUNK pThunk) ;
EVENTHANDLE __stdcall PAGEABLE_Call_Restricted_Event (LONG boost, HANDLE hVMorThread, DWORD flags, PVOID refData, PRestrictedEvent_HANDLER pCallback, DWORD timeout, PRestrictedEvent_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Cancel_Restricted_Event (EVENTHANDLE hEvent) ;
VOID __stdcall INIT_Cancel_Restricted_Event (EVENTHANDLE hEvent) ;
VOID __stdcall DEBUG_Cancel_Restricted_Event (EVENTHANDLE hEvent) ;
VOID __stdcall STATIC_Cancel_Restricted_Event (EVENTHANDLE hEvent) ;
VOID __stdcall PAGEABLE_Cancel_Restricted_Event (EVENTHANDLE hEvent) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_Register_PEF_Provider (DWORD PEFFlag) ;
DWORD __stdcall INIT_Register_PEF_Provider (DWORD PEFFlag) ;
DWORD __stdcall DEBUG_Register_PEF_Provider (DWORD PEFFlag) ;
DWORD __stdcall STATIC_Register_PEF_Provider (DWORD PEFFlag) ;
DWORD __stdcall PAGEABLE_Register_PEF_Provider (DWORD PEFFlag) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__GetPhysPageInfo (DWORD iPage, DWORD nPages, DWORD flags) ;
DWORD __cdecl INIT__GetPhysPageInfo (DWORD iPage, DWORD nPages, DWORD flags) ;
DWORD __cdecl DEBUG__GetPhysPageInfo (DWORD iPage, DWORD nPages, DWORD flags) ;
DWORD __cdecl STATIC__GetPhysPageInfo (DWORD iPage, DWORD nPages, DWORD flags) ;
DWORD __cdecl PAGEABLE__GetPhysPageInfo (DWORD iPage, DWORD nPages, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__RegQueryInfoKey (HKEY hKey,PCHAR szClass,PDWORD pcchClass,PDWORD pdwRes,PDWORD pcSubKeys,PDWORD pcchMaxSubKey,PDWORD pcchMaxClass,PDWORD pcVals,PDWORD pcchMaxValName,PDWORD pcbMaxValueData,PDWORD pcbSecDesc,PFILETIME pWriteTime) ;
DWORD __cdecl INIT__RegQueryInfoKey (HKEY hKey,PCHAR szClass,PDWORD pcchClass,PDWORD pdwRes,PDWORD pcSubKeys,PDWORD pcchMaxSubKey,PDWORD pcchMaxClass,PDWORD pcVals,PDWORD pcchMaxValName,PDWORD pcbMaxValueData,PDWORD pcbSecDesc,PFILETIME pWriteTime) ;
DWORD __cdecl DEBUG__RegQueryInfoKey (HKEY hKey,PCHAR szClass,PDWORD pcchClass,PDWORD pdwRes,PDWORD pcSubKeys,PDWORD pcchMaxSubKey,PDWORD pcchMaxClass,PDWORD pcVals,PDWORD pcchMaxValName,PDWORD pcbMaxValueData,PDWORD pcbSecDesc,PFILETIME pWriteTime) ;
DWORD __cdecl STATIC__RegQueryInfoKey (HKEY hKey,PCHAR szClass,PDWORD pcchClass,PDWORD pdwRes,PDWORD pcSubKeys,PDWORD pcchMaxSubKey,PDWORD pcchMaxClass,PDWORD pcVals,PDWORD pcchMaxValName,PDWORD pcbMaxValueData,PDWORD pcbSecDesc,PFILETIME pWriteTime) ;
DWORD __cdecl PAGEABLE__RegQueryInfoKey (HKEY hKey,PCHAR szClass,PDWORD pcchClass,PDWORD pdwRes,PDWORD pcSubKeys,PDWORD pcchMaxSubKey,PDWORD pcchMaxClass,PDWORD pcVals,PDWORD pcchMaxValName,PDWORD pcbMaxValueData,PDWORD pcbSecDesc,PFILETIME pWriteTime) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Time_Slice_Sleep (LONG msec) ;
BOOL __stdcall INIT_Time_Slice_Sleep (LONG msec) ;
BOOL __stdcall DEBUG_Time_Slice_Sleep (LONG msec) ;
BOOL __stdcall STATIC_Time_Slice_Sleep (LONG msec) ;
BOOL __stdcall PAGEABLE_Time_Slice_Sleep (LONG msec) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Boost_With_Decay (THREADHANDLE hThread, LONG boost, LONG decay) ;
VOID __stdcall INIT_Boost_With_Decay (THREADHANDLE hThread, LONG boost, LONG decay) ;
VOID __stdcall DEBUG_Boost_With_Decay (THREADHANDLE hThread, LONG boost, LONG decay) ;
VOID __stdcall STATIC_Boost_With_Decay (THREADHANDLE hThread, LONG boost, LONG decay) ;
VOID __stdcall PAGEABLE_Boost_With_Decay (THREADHANDLE hThread, LONG boost, LONG decay) ;
#endif

#ifdef WIN40_OR_LATER
HANDLE __stdcall LOCK_Set_Inversion_Pri (THREADHANDLE hTarget, THREADHANDLE hBooster, DWORD priority, DWORD BoostID) ;
HANDLE __stdcall INIT_Set_Inversion_Pri (THREADHANDLE hTarget, THREADHANDLE hBooster, DWORD priority, DWORD BoostID) ;
HANDLE __stdcall DEBUG_Set_Inversion_Pri (THREADHANDLE hTarget, THREADHANDLE hBooster, DWORD priority, DWORD BoostID) ;
HANDLE __stdcall STATIC_Set_Inversion_Pri (THREADHANDLE hTarget, THREADHANDLE hBooster, DWORD priority, DWORD BoostID) ;
HANDLE __stdcall PAGEABLE_Set_Inversion_Pri (THREADHANDLE hTarget, THREADHANDLE hBooster, DWORD priority, DWORD BoostID) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Reset_Inversion_Pri (THREADHANDLE hBoost, DWORD priority) ;
VOID __stdcall INIT_Reset_Inversion_Pri (THREADHANDLE hBoost, DWORD priority) ;
VOID __stdcall DEBUG_Reset_Inversion_Pri (THREADHANDLE hBoost, DWORD priority) ;
VOID __stdcall STATIC_Reset_Inversion_Pri (THREADHANDLE hBoost, DWORD priority) ;
VOID __stdcall PAGEABLE_Reset_Inversion_Pri (THREADHANDLE hBoost, DWORD priority) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Release_Inversion_Pri (THREADHANDLE hBoost) ;
BOOL __stdcall INIT_Release_Inversion_Pri (THREADHANDLE hBoost) ;
BOOL __stdcall DEBUG_Release_Inversion_Pri (THREADHANDLE hBoost) ;
BOOL __stdcall STATIC_Release_Inversion_Pri (THREADHANDLE hBoost) ;
BOOL __stdcall PAGEABLE_Release_Inversion_Pri (THREADHANDLE hBoost) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_Get_Thread_Win32_Pri (THREADHANDLE hThread, PDWORD pCurrent, PDWORD pDecay, PDWORD pInversion) ;
DWORD __stdcall INIT_Get_Thread_Win32_Pri (THREADHANDLE hThread, PDWORD pCurrent, PDWORD pDecay, PDWORD pInversion) ;
DWORD __stdcall DEBUG_Get_Thread_Win32_Pri (THREADHANDLE hThread, PDWORD pCurrent, PDWORD pDecay, PDWORD pInversion) ;
DWORD __stdcall STATIC_Get_Thread_Win32_Pri (THREADHANDLE hThread, PDWORD pCurrent, PDWORD pDecay, PDWORD pInversion) ;
DWORD __stdcall PAGEABLE_Get_Thread_Win32_Pri (THREADHANDLE hThread, PDWORD pCurrent, PDWORD pDecay, PDWORD pInversion) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Set_Thread_Win32_Pri (THREADHANDLE hThread, DWORD priority) ;
BOOL __stdcall INIT_Set_Thread_Win32_Pri (THREADHANDLE hThread, DWORD priority) ;
BOOL __stdcall DEBUG_Set_Thread_Win32_Pri (THREADHANDLE hThread, DWORD priority) ;
BOOL __stdcall STATIC_Set_Thread_Win32_Pri (THREADHANDLE hThread, DWORD priority) ;
BOOL __stdcall PAGEABLE_Set_Thread_Win32_Pri (THREADHANDLE hThread, DWORD priority) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Set_Thread_Static_Boost (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall INIT_Set_Thread_Static_Boost (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall DEBUG_Set_Thread_Static_Boost (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall STATIC_Set_Thread_Static_Boost (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall PAGEABLE_Set_Thread_Static_Boost (THREADHANDLE hThread, LONG boost) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Set_VM_Static_Boost (VMHANDLE hVM, LONG boost) ;
VOID __stdcall INIT_Set_VM_Static_Boost (VMHANDLE hVM, LONG boost) ;
VOID __stdcall DEBUG_Set_VM_Static_Boost (VMHANDLE hVM, LONG boost) ;
VOID __stdcall STATIC_Set_VM_Static_Boost (VMHANDLE hVM, LONG boost) ;
VOID __stdcall PAGEABLE_Set_VM_Static_Boost (VMHANDLE hVM, LONG boost) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_Release_Inversion_Pri_ID (THREADHANDLE hThread, DWORD ID) ;
DWORD __stdcall INIT_Release_Inversion_Pri_ID (THREADHANDLE hThread, DWORD ID) ;
DWORD __stdcall DEBUG_Release_Inversion_Pri_ID (THREADHANDLE hThread, DWORD ID) ;
DWORD __stdcall STATIC_Release_Inversion_Pri_ID (THREADHANDLE hThread, DWORD ID) ;
DWORD __stdcall PAGEABLE_Release_Inversion_Pri_ID (THREADHANDLE hThread, DWORD ID) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Attach_Thread_To_Group (THREADHANDLE hGroupThread, THREADHANDLE hThreadToAdd) ;
VOID __stdcall INIT_Attach_Thread_To_Group (THREADHANDLE hGroupThread, THREADHANDLE hThreadToAdd) ;
VOID __stdcall DEBUG_Attach_Thread_To_Group (THREADHANDLE hGroupThread, THREADHANDLE hThreadToAdd) ;
VOID __stdcall STATIC_Attach_Thread_To_Group (THREADHANDLE hGroupThread, THREADHANDLE hThreadToAdd) ;
VOID __stdcall PAGEABLE_Attach_Thread_To_Group (THREADHANDLE hGroupThread, THREADHANDLE hThreadToAdd) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Detach_Thread_From_Group (THREADHANDLE hThread) ;
VOID __stdcall INIT_Detach_Thread_From_Group (THREADHANDLE hThread) ;
VOID __stdcall DEBUG_Detach_Thread_From_Group (THREADHANDLE hThread) ;
VOID __stdcall STATIC_Detach_Thread_From_Group (THREADHANDLE hThread) ;
VOID __stdcall PAGEABLE_Detach_Thread_From_Group (THREADHANDLE hThread) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Set_Group_Static_Boost (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall INIT_Set_Group_Static_Boost (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall DEBUG_Set_Group_Static_Boost (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall STATIC_Set_Group_Static_Boost (THREADHANDLE hThread, LONG boost) ;
VOID __stdcall PAGEABLE_Set_Group_Static_Boost (THREADHANDLE hThread, LONG boost) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__GetRegistryPath (PDDB ThisDDB, PCHAR UserBuff, DWORD UserBuffSize) ;
DWORD __cdecl INIT__GetRegistryPath (PDDB ThisDDB, PCHAR UserBuff, DWORD UserBuffSize) ;
DWORD __cdecl DEBUG__GetRegistryPath (PDDB ThisDDB, PCHAR UserBuff, DWORD UserBuffSize) ;
DWORD __cdecl STATIC__GetRegistryPath (PDDB ThisDDB, PCHAR UserBuff, DWORD UserBuffSize) ;
DWORD __cdecl PAGEABLE__GetRegistryPath (PDDB ThisDDB, PCHAR UserBuff, DWORD UserBuffSize) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__RegRemapPreDefKey (HKEY hKey, HKEY hRootKey) ;
DWORD __cdecl INIT__RegRemapPreDefKey (HKEY hKey, HKEY hRootKey) ;
DWORD __cdecl DEBUG__RegRemapPreDefKey (HKEY hKey, HKEY hRootKey) ;
DWORD __cdecl STATIC__RegRemapPreDefKey (HKEY hKey, HKEY hRootKey) ;
DWORD __cdecl PAGEABLE__RegRemapPreDefKey (HKEY hKey, HKEY hRootKey) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_End_V86_Serialization (void) ;
VOID __cdecl INIT_End_V86_Serialization (void) ;
VOID __cdecl DEBUG_End_V86_Serialization (void) ;
VOID __cdecl STATIC_End_V86_Serialization (void) ;
VOID __cdecl PAGEABLE_End_V86_Serialization (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__Assert_Range (DWORD pStruc, DWORD ulSize, DWORD signature, DWORD lSignatureOffset, DWORD ulFlags) ;
BOOL __cdecl INIT__Assert_Range (DWORD pStruc, DWORD ulSize, DWORD signature, DWORD lSignatureOffset, DWORD ulFlags) ;
BOOL __cdecl DEBUG__Assert_Range (DWORD pStruc, DWORD ulSize, DWORD signature, DWORD lSignatureOffset, DWORD ulFlags) ;
BOOL __cdecl STATIC__Assert_Range (DWORD pStruc, DWORD ulSize, DWORD signature, DWORD lSignatureOffset, DWORD ulFlags) ;
BOOL __cdecl PAGEABLE__Assert_Range (DWORD pStruc, DWORD ulSize, DWORD signature, DWORD lSignatureOffset, DWORD ulFlags) ;
#endif

#ifdef WIN40_OR_LATER
ULONG __cdecl LOCK__PageChangePager (ULONG thePage, ULONG nPages, HANDLE hpd, ULONG pagerdata, ULONG flags) ;
ULONG __cdecl INIT__PageChangePager (ULONG thePage, ULONG nPages, HANDLE hpd, ULONG pagerdata, ULONG flags) ;
ULONG __cdecl DEBUG__PageChangePager (ULONG thePage, ULONG nPages, HANDLE hpd, ULONG pagerdata, ULONG flags) ;
ULONG __cdecl STATIC__PageChangePager (ULONG thePage, ULONG nPages, HANDLE hpd, ULONG pagerdata, ULONG flags) ;
ULONG __cdecl PAGEABLE__PageChangePager (ULONG thePage, ULONG nPages, HANDLE hpd, ULONG pagerdata, ULONG flags) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__RegCreateDynKey (PCHAR szName, PVOID KeyContext, PPROVIDER pInfo, PPVALUE pValList, DWORD dwNumVals, HKEY* pKeyHandle) ;
DWORD __cdecl INIT__RegCreateDynKey (PCHAR szName, PVOID KeyContext, PPROVIDER pInfo, PPVALUE pValList, DWORD dwNumVals, HKEY* pKeyHandle) ;
DWORD __cdecl DEBUG__RegCreateDynKey (PCHAR szName, PVOID KeyContext, PPROVIDER pInfo, PPVALUE pValList, DWORD dwNumVals, HKEY* pKeyHandle) ;
DWORD __cdecl STATIC__RegCreateDynKey (PCHAR szName, PVOID KeyContext, PPROVIDER pInfo, PPVALUE pValList, DWORD dwNumVals, HKEY* pKeyHandle) ;
DWORD __cdecl PAGEABLE__RegCreateDynKey (PCHAR szName, PVOID KeyContext, PPROVIDER pInfo, PPVALUE pValList, DWORD dwNumVals, HKEY* pKeyHandle) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__RegQueryMultipleValues (HKEY hKey, PVALENT pValent, DWORD dwNumVals, PCHAR pValueBuf, PDWORD pTotSize) ;
DWORD __cdecl INIT__RegQueryMultipleValues (HKEY hKey, PVALENT pValent, DWORD dwNumVals, PCHAR pValueBuf, PDWORD pTotSize) ;
DWORD __cdecl DEBUG__RegQueryMultipleValues (HKEY hKey, PVALENT pValent, DWORD dwNumVals, PCHAR pValueBuf, PDWORD pTotSize) ;
DWORD __cdecl STATIC__RegQueryMultipleValues (HKEY hKey, PVALENT pValent, DWORD dwNumVals, PCHAR pValueBuf, PDWORD pTotSize) ;
DWORD __cdecl PAGEABLE__RegQueryMultipleValues (HKEY hKey, PVALENT pValent, DWORD dwNumVals, PCHAR pValueBuf, PDWORD pTotSize) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_Get_Boot_Flags (void) ;
DWORD __cdecl INIT_Get_Boot_Flags (void) ;
DWORD __cdecl DEBUG_Get_Boot_Flags (void) ;
DWORD __cdecl STATIC_Get_Boot_Flags (void) ;
DWORD __cdecl PAGEABLE_Get_Boot_Flags (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Set_Boot_Flags (DWORD flags) ;
VOID __stdcall INIT_Set_Boot_Flags (DWORD flags) ;
VOID __stdcall DEBUG_Set_Boot_Flags (DWORD flags) ;
VOID __stdcall STATIC_Set_Boot_Flags (DWORD flags) ;
VOID __stdcall PAGEABLE_Set_Boot_Flags (DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
PBYTE __cdecl LOCK__lstrcpyn (PBYTE dest, PBYTE src, DWORD maxDestBytes, PDWORD pcUnused) ;
PBYTE __cdecl INIT__lstrcpyn (PBYTE dest, PBYTE src, DWORD maxDestBytes, PDWORD pcUnused) ;
PBYTE __cdecl DEBUG__lstrcpyn (PBYTE dest, PBYTE src, DWORD maxDestBytes, PDWORD pcUnused) ;
PBYTE __cdecl STATIC__lstrcpyn (PBYTE dest, PBYTE src, DWORD maxDestBytes, PDWORD pcUnused) ;
PBYTE __cdecl PAGEABLE__lstrcpyn (PBYTE dest, PBYTE src, DWORD maxDestBytes, PDWORD pcUnused) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__lstrlen (PCHAR sz) ;
DWORD __cdecl INIT__lstrlen (PCHAR sz) ;
DWORD __cdecl DEBUG__lstrlen (PCHAR sz) ;
DWORD __cdecl STATIC__lstrlen (PCHAR sz) ;
DWORD __cdecl PAGEABLE__lstrlen (PCHAR sz) ;
#endif

#ifdef WIN40_OR_LATER
PBYTE __cdecl LOCK__lmemcpy (PBYTE dest, PBYTE src, DWORD nBytes, PBYTE* pEndDest) ;
PBYTE __cdecl INIT__lmemcpy (PBYTE dest, PBYTE src, DWORD nBytes, PBYTE* pEndDest) ;
PBYTE __cdecl DEBUG__lmemcpy (PBYTE dest, PBYTE src, DWORD nBytes, PBYTE* pEndDest) ;
PBYTE __cdecl STATIC__lmemcpy (PBYTE dest, PBYTE src, DWORD nBytes, PBYTE* pEndDest) ;
PBYTE __cdecl PAGEABLE__lmemcpy (PBYTE dest, PBYTE src, DWORD nBytes, PBYTE* pEndDest) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK__lstrcmpi (PCHAR string1, PCHAR string2) ;
INT __cdecl INIT__lstrcmpi (PCHAR string1, PCHAR string2) ;
INT __cdecl DEBUG__lstrcmpi (PCHAR string1, PCHAR string2) ;
INT __cdecl STATIC__lstrcmpi (PCHAR string1, PCHAR string2) ;
INT __cdecl PAGEABLE__lstrcmpi (PCHAR string1, PCHAR string2) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK__GetVxDName (DWORD laddr, PCHAR pObjectName) ;
PVOID __cdecl INIT__GetVxDName (DWORD laddr, PCHAR pObjectName) ;
PVOID __cdecl DEBUG__GetVxDName (DWORD laddr, PCHAR pObjectName) ;
PVOID __cdecl STATIC__GetVxDName (DWORD laddr, PCHAR pObjectName) ;
PVOID __cdecl PAGEABLE__GetVxDName (DWORD laddr, PCHAR pObjectName) ;
#endif

VOID __stdcall LOCK_BlockDev_Command_Complete (CONST BDD * pBDD, CONST BCB * pBCB) ;
VOID __stdcall INIT_BlockDev_Command_Complete (CONST BDD * pBDD, CONST BCB * pBCB) ;
VOID __stdcall DEBUG_BlockDev_Command_Complete (CONST BDD * pBDD, CONST BCB * pBCB) ;
VOID __stdcall STATIC_BlockDev_Command_Complete (CONST BDD * pBDD, CONST BCB * pBCB) ;
VOID __stdcall PAGEABLE_BlockDev_Command_Complete (CONST BDD * pBDD, CONST BCB * pBCB) ;

PBDD __stdcall LOCK_BlockDev_Find_Int13_Drive (CHAR DriveNum) ;
PBDD __stdcall INIT_BlockDev_Find_Int13_Drive (CHAR DriveNum) ;
PBDD __stdcall DEBUG_BlockDev_Find_Int13_Drive (CHAR DriveNum) ;
PBDD __stdcall STATIC_BlockDev_Find_Int13_Drive (CHAR DriveNum) ;
PBDD __stdcall PAGEABLE_BlockDev_Find_Int13_Drive (CHAR DriveNum) ;

PBDD __stdcall LOCK_BlockDev_Get_Device_List (void) ;
PBDD __stdcall INIT_BlockDev_Get_Device_List (void) ;
PBDD __stdcall DEBUG_BlockDev_Get_Device_List (void) ;
PBDD __stdcall STATIC_BlockDev_Get_Device_List (void) ;
PBDD __stdcall PAGEABLE_BlockDev_Get_Device_List (void) ;

DWORD __stdcall LOCK_BlockDev_Get_Version (PDWORD pFlags) ;
DWORD __stdcall INIT_BlockDev_Get_Version (PDWORD pFlags) ;
DWORD __stdcall DEBUG_BlockDev_Get_Version (PDWORD pFlags) ;
DWORD __stdcall STATIC_BlockDev_Get_Version (PDWORD pFlags) ;
DWORD __stdcall PAGEABLE_BlockDev_Get_Version (PDWORD pFlags) ;

BOOL __stdcall LOCK_BlockDev_Register_Device (PBDD pBDD) ;
BOOL __stdcall INIT_BlockDev_Register_Device (PBDD pBDD) ;
BOOL __stdcall DEBUG_BlockDev_Register_Device (PBDD pBDD) ;
BOOL __stdcall STATIC_BlockDev_Register_Device (PBDD pBDD) ;
BOOL __stdcall PAGEABLE_BlockDev_Register_Device (PBDD pBDD) ;

VOID __stdcall LOCK_BlockDev_Send_Command (CONST BDD * pBDD, CONST BCB * pBCB) ;
VOID __stdcall INIT_BlockDev_Send_Command (CONST BDD * pBDD, CONST BCB * pBCB) ;
VOID __stdcall DEBUG_BlockDev_Send_Command (CONST BDD * pBDD, CONST BCB * pBCB) ;
VOID __stdcall STATIC_BlockDev_Send_Command (CONST BDD * pBDD, CONST BCB * pBCB) ;
VOID __stdcall PAGEABLE_BlockDev_Send_Command (CONST BDD * pBDD, CONST BCB * pBCB) ;

BOOL __stdcall LOCK_BlockDev_Synchronous_Command (PALLREGS pRegisters, PBDD pbdd) ;
BOOL __stdcall INIT_BlockDev_Synchronous_Command (PALLREGS pRegisters, PBDD pbdd) ;
BOOL __stdcall DEBUG_BlockDev_Synchronous_Command (PALLREGS pRegisters, PBDD pbdd) ;
BOOL __stdcall STATIC_BlockDev_Synchronous_Command (PALLREGS pRegisters, PBDD pbdd) ;
BOOL __stdcall PAGEABLE_BlockDev_Synchronous_Command (PALLREGS pRegisters, PBDD pbdd) ;

PVOID __stdcall LOCK_BlockDev_Thunk_Sync_Cmd (PBLOCKDEV_Sync_Cmd_HANDLER Callback, PBLOCKDEV_Sync_Cmd_THUNK pThunk) ;
PVOID __stdcall INIT_BlockDev_Thunk_Sync_Cmd (PBLOCKDEV_Sync_Cmd_HANDLER Callback, PBLOCKDEV_Sync_Cmd_THUNK pThunk) ;
PVOID __stdcall DEBUG_BlockDev_Thunk_Sync_Cmd (PBLOCKDEV_Sync_Cmd_HANDLER Callback, PBLOCKDEV_Sync_Cmd_THUNK pThunk) ;
PVOID __stdcall STATIC_BlockDev_Thunk_Sync_Cmd (PBLOCKDEV_Sync_Cmd_HANDLER Callback, PBLOCKDEV_Sync_Cmd_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_BlockDev_Thunk_Sync_Cmd (PBLOCKDEV_Sync_Cmd_HANDLER Callback, PBLOCKDEV_Sync_Cmd_THUNK pThunk) ;

PVOID __stdcall LOCK_BlockDev_Thunk_Command (PBLOCKDEV_Command_HANDLER Callback, PBLOCKDEV_Command_THUNK pThunk) ;
PVOID __stdcall INIT_BlockDev_Thunk_Command (PBLOCKDEV_Command_HANDLER Callback, PBLOCKDEV_Command_THUNK pThunk) ;
PVOID __stdcall DEBUG_BlockDev_Thunk_Command (PBLOCKDEV_Command_HANDLER Callback, PBLOCKDEV_Command_THUNK pThunk) ;
PVOID __stdcall STATIC_BlockDev_Thunk_Command (PBLOCKDEV_Command_HANDLER Callback, PBLOCKDEV_Command_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_BlockDev_Thunk_Command (PBLOCKDEV_Command_HANDLER Callback, PBLOCKDEV_Command_THUNK pThunk) ;

PVOID __stdcall LOCK_BlockDev_Thunk_HW_Int (PBLOCKDEV_HW_Int_HANDLER Callback, PBLOCKDEV_HW_Int_THUNK pThunk) ;
PVOID __stdcall INIT_BlockDev_Thunk_HW_Int (PBLOCKDEV_HW_Int_HANDLER Callback, PBLOCKDEV_HW_Int_THUNK pThunk) ;
PVOID __stdcall DEBUG_BlockDev_Thunk_HW_Int (PBLOCKDEV_HW_Int_HANDLER Callback, PBLOCKDEV_HW_Int_THUNK pThunk) ;
PVOID __stdcall STATIC_BlockDev_Thunk_HW_Int (PBLOCKDEV_HW_Int_HANDLER Callback, PBLOCKDEV_HW_Int_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_BlockDev_Thunk_HW_Int (PBLOCKDEV_HW_Int_HANDLER Callback, PBLOCKDEV_HW_Int_THUNK pThunk) ;

BOOL __stdcall LOCK_DOSMGR_Add_Device (VMHANDLE hVM, PVOID DevAddr) ;
BOOL __stdcall INIT_DOSMGR_Add_Device (VMHANDLE hVM, PVOID DevAddr) ;
BOOL __stdcall DEBUG_DOSMGR_Add_Device (VMHANDLE hVM, PVOID DevAddr) ;
BOOL __stdcall STATIC_DOSMGR_Add_Device (VMHANDLE hVM, PVOID DevAddr) ;
BOOL __stdcall PAGEABLE_DOSMGR_Add_Device (VMHANDLE hVM, PVOID DevAddr) ;

BOOL __stdcall LOCK_DOSMGR_BackFill_Allowed (void) ;
BOOL __stdcall INIT_DOSMGR_BackFill_Allowed (void) ;
BOOL __stdcall DEBUG_DOSMGR_BackFill_Allowed (void) ;
BOOL __stdcall STATIC_DOSMGR_BackFill_Allowed (void) ;
BOOL __stdcall PAGEABLE_DOSMGR_BackFill_Allowed (void) ;

VOID __stdcall LOCK_DOSMGR_Copy_VM_Drive_State (VMHANDLE hVMDest, VMHANDLE hVMSrc) ;
VOID __stdcall INIT_DOSMGR_Copy_VM_Drive_State (VMHANDLE hVMDest, VMHANDLE hVMSrc) ;
VOID __stdcall DEBUG_DOSMGR_Copy_VM_Drive_State (VMHANDLE hVMDest, VMHANDLE hVMSrc) ;
VOID __stdcall STATIC_DOSMGR_Copy_VM_Drive_State (VMHANDLE hVMDest, VMHANDLE hVMSrc) ;
VOID __stdcall PAGEABLE_DOSMGR_Copy_VM_Drive_State (VMHANDLE hVMDest, VMHANDLE hVMSrc) ;

BOOL __stdcall LOCK_DOSMGR_Enable_Indos_Polling (void) ;
BOOL __stdcall INIT_DOSMGR_Enable_Indos_Polling (void) ;
BOOL __stdcall DEBUG_DOSMGR_Enable_Indos_Polling (void) ;
BOOL __stdcall STATIC_DOSMGR_Enable_Indos_Polling (void) ;
BOOL __stdcall PAGEABLE_DOSMGR_Enable_Indos_Polling (void) ;

VOID __stdcall LOCK__DOSMGR_Exec_VM (VMHANDLE hVM, DWORD V86Size, PExecVM_HANDLER CallBack, CONST VOID * RefData, PExecVM_THUNK pThunk) ;
VOID __stdcall INIT__DOSMGR_Exec_VM (VMHANDLE hVM, DWORD V86Size, PExecVM_HANDLER CallBack, CONST VOID * RefData, PExecVM_THUNK pThunk) ;
VOID __stdcall DEBUG__DOSMGR_Exec_VM (VMHANDLE hVM, DWORD V86Size, PExecVM_HANDLER CallBack, CONST VOID * RefData, PExecVM_THUNK pThunk) ;
VOID __stdcall STATIC__DOSMGR_Exec_VM (VMHANDLE hVM, DWORD V86Size, PExecVM_HANDLER CallBack, CONST VOID * RefData, PExecVM_THUNK pThunk) ;
VOID __stdcall PAGEABLE__DOSMGR_Exec_VM (VMHANDLE hVM, DWORD V86Size, PExecVM_HANDLER CallBack, CONST VOID * RefData, PExecVM_THUNK pThunk) ;

BOOL __stdcall LOCK_DOSMGR_Get_DOS_Crit_Status (void) ;
BOOL __stdcall INIT_DOSMGR_Get_DOS_Crit_Status (void) ;
BOOL __stdcall DEBUG_DOSMGR_Get_DOS_Crit_Status (void) ;
BOOL __stdcall STATIC_DOSMGR_Get_DOS_Crit_Status (void) ;
BOOL __stdcall PAGEABLE_DOSMGR_Get_DOS_Crit_Status (void) ;

PVOID __stdcall LOCK_DOSMGR_Get_IndosPtr (void) ;
PVOID __stdcall INIT_DOSMGR_Get_IndosPtr (void) ;
PVOID __stdcall DEBUG_DOSMGR_Get_IndosPtr (void) ;
PVOID __stdcall STATIC_DOSMGR_Get_IndosPtr (void) ;
PVOID __stdcall PAGEABLE_DOSMGR_Get_IndosPtr (void) ;

DWORD __stdcall LOCK_DOSMGR_Get_Version (void) ;
DWORD __stdcall INIT_DOSMGR_Get_Version (void) ;
DWORD __stdcall DEBUG_DOSMGR_Get_Version (void) ;
DWORD __stdcall STATIC_DOSMGR_Get_Version (void) ;
DWORD __stdcall PAGEABLE_DOSMGR_Get_Version (void) ;

BOOL __stdcall LOCK_DOSMGR_Instance_Device (CONST CHAR * DevName) ;
BOOL __stdcall INIT_DOSMGR_Instance_Device (CONST CHAR * DevName) ;
BOOL __stdcall DEBUG_DOSMGR_Instance_Device (CONST CHAR * DevName) ;
BOOL __stdcall STATIC_DOSMGR_Instance_Device (CONST CHAR * DevName) ;
BOOL __stdcall PAGEABLE_DOSMGR_Instance_Device (CONST CHAR * DevName) ;

DWORD __stdcall LOCK_DOSMGR_LocalGlobalReg (DWORD pgStart, DWORD pgEnd) ;
DWORD __stdcall INIT_DOSMGR_LocalGlobalReg (DWORD pgStart, DWORD pgEnd) ;
DWORD __stdcall DEBUG_DOSMGR_LocalGlobalReg (DWORD pgStart, DWORD pgEnd) ;
DWORD __stdcall STATIC_DOSMGR_LocalGlobalReg (DWORD pgStart, DWORD pgEnd) ;
DWORD __stdcall PAGEABLE_DOSMGR_LocalGlobalReg (DWORD pgStart, DWORD pgEnd) ;

BOOL __stdcall LOCK_DOSMGR_Remove_Device (VMHANDLE hVM, CONST VOID * DevAddr) ;
BOOL __stdcall INIT_DOSMGR_Remove_Device (VMHANDLE hVM, CONST VOID * DevAddr) ;
BOOL __stdcall DEBUG_DOSMGR_Remove_Device (VMHANDLE hVM, CONST VOID * DevAddr) ;
BOOL __stdcall STATIC_DOSMGR_Remove_Device (VMHANDLE hVM, CONST VOID * DevAddr) ;
BOOL __stdcall PAGEABLE_DOSMGR_Remove_Device (VMHANDLE hVM, CONST VOID * DevAddr) ;

VOID __cdecl LOCK__DOSMGR_Set_Exec_VM_Data (VMHANDLE hVM, CONST CHAR * CommTail, CONST CHAR * PRGName, CONST CHAR * InitDrvDir) ;
VOID __cdecl INIT__DOSMGR_Set_Exec_VM_Data (VMHANDLE hVM, CONST CHAR * CommTail, CONST CHAR * PRGName, CONST CHAR * InitDrvDir) ;
VOID __cdecl DEBUG__DOSMGR_Set_Exec_VM_Data (VMHANDLE hVM, CONST CHAR * CommTail, CONST CHAR * PRGName, CONST CHAR * InitDrvDir) ;
VOID __cdecl STATIC__DOSMGR_Set_Exec_VM_Data (VMHANDLE hVM, CONST CHAR * CommTail, CONST CHAR * PRGName, CONST CHAR * InitDrvDir) ;
VOID __cdecl PAGEABLE__DOSMGR_Set_Exec_VM_Data (VMHANDLE hVM, CONST CHAR * CommTail, CONST CHAR * PRGName, CONST CHAR * InitDrvDir) ;

BOOL __stdcall LOCK_DOSNET_Do_PSP_Adjust (PBOOL pCheckini) ;
BOOL __stdcall INIT_DOSNET_Do_PSP_Adjust (PBOOL pCheckini) ;
BOOL __stdcall DEBUG_DOSNET_Do_PSP_Adjust (PBOOL pCheckini) ;
BOOL __stdcall STATIC_DOSNET_Do_PSP_Adjust (PBOOL pCheckini) ;
BOOL __stdcall PAGEABLE_DOSNET_Do_PSP_Adjust (PBOOL pCheckini) ;

DWORD __stdcall LOCK_DOSNET_Get_Version (void) ;
DWORD __stdcall INIT_DOSNET_Get_Version (void) ;
DWORD __stdcall DEBUG_DOSNET_Get_Version (void) ;
DWORD __stdcall STATIC_DOSNET_Get_Version (void) ;
DWORD __stdcall PAGEABLE_DOSNET_Get_Version (void) ;

BOOL __stdcall LOCK_DOSNET_Send_FILESYSCHANGE (CHAR DriveNum, VMHANDLE hVM) ;
BOOL __stdcall INIT_DOSNET_Send_FILESYSCHANGE (CHAR DriveNum, VMHANDLE hVM) ;
BOOL __stdcall DEBUG_DOSNET_Send_FILESYSCHANGE (CHAR DriveNum, VMHANDLE hVM) ;
BOOL __stdcall STATIC_DOSNET_Send_FILESYSCHANGE (CHAR DriveNum, VMHANDLE hVM) ;
BOOL __stdcall PAGEABLE_DOSNET_Send_FILESYSCHANGE (CHAR DriveNum, VMHANDLE hVM) ;

BOOL __stdcall LOCK_EBIOS_Get_Unused_Mem (PDWORD pSeg, PDWORD pSize, PBOOL pGlobal) ;
BOOL __stdcall INIT_EBIOS_Get_Unused_Mem (PDWORD pSeg, PDWORD pSize, PBOOL pGlobal) ;
BOOL __stdcall DEBUG_EBIOS_Get_Unused_Mem (PDWORD pSeg, PDWORD pSize, PBOOL pGlobal) ;
BOOL __stdcall STATIC_EBIOS_Get_Unused_Mem (PDWORD pSeg, PDWORD pSize, PBOOL pGlobal) ;
BOOL __stdcall PAGEABLE_EBIOS_Get_Unused_Mem (PDWORD pSeg, PDWORD pSize, PBOOL pGlobal) ;

BOOL __stdcall LOCK_EBIOS_Get_Version (PDWORD pVersion, PDWORD pStart, PDWORD pNpages) ;
BOOL __stdcall INIT_EBIOS_Get_Version (PDWORD pVersion, PDWORD pStart, PDWORD pNpages) ;
BOOL __stdcall DEBUG_EBIOS_Get_Version (PDWORD pVersion, PDWORD pStart, PDWORD pNpages) ;
BOOL __stdcall STATIC_EBIOS_Get_Version (PDWORD pVersion, PDWORD pStart, PDWORD pNpages) ;
BOOL __stdcall PAGEABLE_EBIOS_Get_Version (PDWORD pVersion, PDWORD pStart, PDWORD pNpages) ;

PBDD __stdcall LOCK_Int13_Device_Registered (CONST BDD * pbdd) ;
PBDD __stdcall INIT_Int13_Device_Registered (CONST BDD * pbdd) ;
PBDD __stdcall DEBUG_Int13_Device_Registered (CONST BDD * pbdd) ;
PBDD __stdcall STATIC_Int13_Device_Registered (CONST BDD * pbdd) ;
PBDD __stdcall PAGEABLE_Int13_Device_Registered (CONST BDD * pbdd) ;

VOID __stdcall LOCK_Int13_Hooking_BIOS_Int (SEGOFFSET org13, SEGOFFSET orgROM13) ;
VOID __stdcall INIT_Int13_Hooking_BIOS_Int (SEGOFFSET org13, SEGOFFSET orgROM13) ;
VOID __stdcall DEBUG_Int13_Hooking_BIOS_Int (SEGOFFSET org13, SEGOFFSET orgROM13) ;
VOID __stdcall STATIC_Int13_Hooking_BIOS_Int (SEGOFFSET org13, SEGOFFSET orgROM13) ;
VOID __stdcall PAGEABLE_Int13_Hooking_BIOS_Int (SEGOFFSET org13, SEGOFFSET orgROM13) ;

VOID __stdcall LOCK_Int13_Translate_VM_Int (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs) ;
VOID __stdcall INIT_Int13_Translate_VM_Int (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs) ;
VOID __stdcall DEBUG_Int13_Translate_VM_Int (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs) ;
VOID __stdcall STATIC_Int13_Translate_VM_Int (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs) ;
VOID __stdcall PAGEABLE_Int13_Translate_VM_Int (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs) ;

VOID __stdcall LOCK_Int13_Unhooking_BIOS_Int (SEGOFFSET org13, SEGOFFSET orgROM13) ;
VOID __stdcall INIT_Int13_Unhooking_BIOS_Int (SEGOFFSET org13, SEGOFFSET orgROM13) ;
VOID __stdcall DEBUG_Int13_Unhooking_BIOS_Int (SEGOFFSET org13, SEGOFFSET orgROM13) ;
VOID __stdcall STATIC_Int13_Unhooking_BIOS_Int (SEGOFFSET org13, SEGOFFSET orgROM13) ;
VOID __stdcall PAGEABLE_Int13_Unhooking_BIOS_Int (SEGOFFSET org13, SEGOFFSET orgROM13) ;

VMHANDLE __stdcall LOCK_VMD_Get_Mouse_Owner (void) ;
VMHANDLE __stdcall INIT_VMD_Get_Mouse_Owner (void) ;
VMHANDLE __stdcall DEBUG_VMD_Get_Mouse_Owner (void) ;
VMHANDLE __stdcall STATIC_VMD_Get_Mouse_Owner (void) ;
VMHANDLE __stdcall PAGEABLE_VMD_Get_Mouse_Owner (void) ;

DWORD __stdcall LOCK_VMD_Get_Version (void) ;
DWORD __stdcall INIT_VMD_Get_Version (void) ;
DWORD __stdcall DEBUG_VMD_Get_Version (void) ;
DWORD __stdcall STATIC_VMD_Get_Version (void) ;
DWORD __stdcall PAGEABLE_VMD_Get_Version (void) ;

DWORD __stdcall LOCK_VMD_Set_Mouse_Type (DWORD irq, DWORD MouseType, CHAR Port) ;
DWORD __stdcall INIT_VMD_Set_Mouse_Type (DWORD irq, DWORD MouseType, CHAR Port) ;
DWORD __stdcall DEBUG_VMD_Set_Mouse_Type (DWORD irq, DWORD MouseType, CHAR Port) ;
DWORD __stdcall STATIC_VMD_Set_Mouse_Type (DWORD irq, DWORD MouseType, CHAR Port) ;
DWORD __stdcall PAGEABLE_VMD_Set_Mouse_Type (DWORD irq, DWORD MouseType, CHAR Port) ;

BOOL __stdcall LOCK_SHELL_Event (VMHANDLE hVM, DWORD EventNum, WORD wParam, PSHELLEvent_HANDLER Callback, CONST VOID * Refdata, PEVENTHANDLE pEvent, PSHELLEvent_THUNK pThunk) ;
BOOL __stdcall INIT_SHELL_Event (VMHANDLE hVM, DWORD EventNum, WORD wParam, PSHELLEvent_HANDLER Callback, CONST VOID * Refdata, PEVENTHANDLE pEvent, PSHELLEvent_THUNK pThunk) ;
BOOL __stdcall DEBUG_SHELL_Event (VMHANDLE hVM, DWORD EventNum, WORD wParam, PSHELLEvent_HANDLER Callback, CONST VOID * Refdata, PEVENTHANDLE pEvent, PSHELLEvent_THUNK pThunk) ;
BOOL __stdcall STATIC_SHELL_Event (VMHANDLE hVM, DWORD EventNum, WORD wParam, PSHELLEvent_HANDLER Callback, CONST VOID * Refdata, PEVENTHANDLE pEvent, PSHELLEvent_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_SHELL_Event (VMHANDLE hVM, DWORD EventNum, WORD wParam, PSHELLEvent_HANDLER Callback, CONST VOID * Refdata, PEVENTHANDLE pEvent, PSHELLEvent_THUNK pThunk) ;

DWORD __stdcall LOCK_SHELL_Get_Version (void) ;
DWORD __stdcall INIT_SHELL_Get_Version (void) ;
DWORD __stdcall DEBUG_SHELL_Get_Version (void) ;
DWORD __stdcall STATIC_SHELL_Get_Version (void) ;
DWORD __stdcall PAGEABLE_SHELL_Get_Version (void) ;

VOID __stdcall LOCK_SHELL_GetVMInfo (VMHANDLE hVM, PALLREGS pRegisters) ;
VOID __stdcall INIT_SHELL_GetVMInfo (VMHANDLE hVM, PALLREGS pRegisters) ;
VOID __stdcall DEBUG_SHELL_GetVMInfo (VMHANDLE hVM, PALLREGS pRegisters) ;
VOID __stdcall STATIC_SHELL_GetVMInfo (VMHANDLE hVM, PALLREGS pRegisters) ;
VOID __stdcall PAGEABLE_SHELL_GetVMInfo (VMHANDLE hVM, PALLREGS pRegisters) ;

EVENTHANDLE __stdcall LOCK_SHELL_Message (VMHANDLE hVM, DWORD Flags, CONST CHAR * Message, CONST CHAR * Caption, CONST VOID * Callback, CONST VOID * Refdata, PSHELLMessage_THUNK pThunk) ;
EVENTHANDLE __stdcall INIT_SHELL_Message (VMHANDLE hVM, DWORD Flags, CONST CHAR * Message, CONST CHAR * Caption, CONST VOID * Callback, CONST VOID * Refdata, PSHELLMessage_THUNK pThunk) ;
EVENTHANDLE __stdcall DEBUG_SHELL_Message (VMHANDLE hVM, DWORD Flags, CONST CHAR * Message, CONST CHAR * Caption, CONST VOID * Callback, CONST VOID * Refdata, PSHELLMessage_THUNK pThunk) ;
EVENTHANDLE __stdcall STATIC_SHELL_Message (VMHANDLE hVM, DWORD Flags, CONST CHAR * Message, CONST CHAR * Caption, CONST VOID * Callback, CONST VOID * Refdata, PSHELLMessage_THUNK pThunk) ;
EVENTHANDLE __stdcall PAGEABLE_SHELL_Message (VMHANDLE hVM, DWORD Flags, CONST CHAR * Message, CONST CHAR * Caption, CONST VOID * Callback, CONST VOID * Refdata, PSHELLMessage_THUNK pThunk) ;

VMHANDLE __stdcall LOCK_SHELL_Resolve_Contention (VMHANDLE hVMOwner, VMHANDLE hVMCurrent, CONST CHAR * Device) ;
VMHANDLE __stdcall INIT_SHELL_Resolve_Contention (VMHANDLE hVMOwner, VMHANDLE hVMCurrent, CONST CHAR * Device) ;
VMHANDLE __stdcall DEBUG_SHELL_Resolve_Contention (VMHANDLE hVMOwner, VMHANDLE hVMCurrent, CONST CHAR * Device) ;
VMHANDLE __stdcall STATIC_SHELL_Resolve_Contention (VMHANDLE hVMOwner, VMHANDLE hVMCurrent, CONST CHAR * Device) ;
VMHANDLE __stdcall PAGEABLE_SHELL_Resolve_Contention (VMHANDLE hVMOwner, VMHANDLE hVMCurrent, CONST CHAR * Device) ;

DWORD __stdcall LOCK_SHELL_SYSMODAL_Message (VMHANDLE hVM, DWORD Flags, CONST CHAR * Message, CONST CHAR * Caption) ;
DWORD __stdcall INIT_SHELL_SYSMODAL_Message (VMHANDLE hVM, DWORD Flags, CONST CHAR * Message, CONST CHAR * Caption) ;
DWORD __stdcall DEBUG_SHELL_SYSMODAL_Message (VMHANDLE hVM, DWORD Flags, CONST CHAR * Message, CONST CHAR * Caption) ;
DWORD __stdcall STATIC_SHELL_SYSMODAL_Message (VMHANDLE hVM, DWORD Flags, CONST CHAR * Message, CONST CHAR * Caption) ;
DWORD __stdcall PAGEABLE_SHELL_SYSMODAL_Message (VMHANDLE hVM, DWORD Flags, CONST CHAR * Message, CONST CHAR * Caption) ;

#ifdef WIN40_OR_LATER
APPY_HANDLE __cdecl LOCK__SHELL_CallAtAppyTime (APPY_CALLBACK pfnAppyCallBack, PVOID dwRefData, DWORD flags, DWORD timeout) ;
APPY_HANDLE __cdecl INIT__SHELL_CallAtAppyTime (APPY_CALLBACK pfnAppyCallBack, PVOID dwRefData, DWORD flags, DWORD timeout) ;
APPY_HANDLE __cdecl DEBUG__SHELL_CallAtAppyTime (APPY_CALLBACK pfnAppyCallBack, PVOID dwRefData, DWORD flags, DWORD timeout) ;
APPY_HANDLE __cdecl STATIC__SHELL_CallAtAppyTime (APPY_CALLBACK pfnAppyCallBack, PVOID dwRefData, DWORD flags, DWORD timeout) ;
APPY_HANDLE __cdecl PAGEABLE__SHELL_CallAtAppyTime (APPY_CALLBACK pfnAppyCallBack, PVOID dwRefData, DWORD flags, DWORD timeout) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__SHELL_CancelAppyTimeEvent (APPY_HANDLE hAppy) ;
VOID __cdecl INIT__SHELL_CancelAppyTimeEvent (APPY_HANDLE hAppy) ;
VOID __cdecl DEBUG__SHELL_CancelAppyTimeEvent (APPY_HANDLE hAppy) ;
VOID __cdecl STATIC__SHELL_CancelAppyTimeEvent (APPY_HANDLE hAppy) ;
VOID __cdecl PAGEABLE__SHELL_CancelAppyTimeEvent (APPY_HANDLE hAppy) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__SHELL_QueryAppyTimeAvailable (VOID) ;
BOOL __cdecl INIT__SHELL_QueryAppyTimeAvailable (VOID) ;
BOOL __cdecl DEBUG__SHELL_QueryAppyTimeAvailable (VOID) ;
BOOL __cdecl STATIC__SHELL_QueryAppyTimeAvailable (VOID) ;
BOOL __cdecl PAGEABLE__SHELL_QueryAppyTimeAvailable (VOID) ;
#endif

#ifdef WIN40_OR_LATER
SEGOFFSET __stdcall LOCK__SHELL_LocalAllocEx (DWORD fl, DWORD cb, PVOID lpvBuf, PPVOID linearAddress) ;
SEGOFFSET __stdcall INIT__SHELL_LocalAllocEx (DWORD fl, DWORD cb, PVOID lpvBuf, PPVOID linearAddress) ;
SEGOFFSET __stdcall DEBUG__SHELL_LocalAllocEx (DWORD fl, DWORD cb, PVOID lpvBuf, PPVOID linearAddress) ;
SEGOFFSET __stdcall STATIC__SHELL_LocalAllocEx (DWORD fl, DWORD cb, PVOID lpvBuf, PPVOID linearAddress) ;
SEGOFFSET __stdcall PAGEABLE__SHELL_LocalAllocEx (DWORD fl, DWORD cb, PVOID lpvBuf, PPVOID linearAddress) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__SHELL_LocalFree (SEGOFFSET hdata) ;
VOID __cdecl INIT__SHELL_LocalFree (SEGOFFSET hdata) ;
VOID __cdecl DEBUG__SHELL_LocalFree (SEGOFFSET hdata) ;
VOID __cdecl STATIC__SHELL_LocalFree (SEGOFFSET hdata) ;
VOID __cdecl PAGEABLE__SHELL_LocalFree (SEGOFFSET hdata) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__SHELL_CallDll (PCHAR lpszDll, PCHAR lpszProcName, DWORD cbArgs, PVOID lpvArgs) ;
DWORD __cdecl INIT__SHELL_CallDll (PCHAR lpszDll, PCHAR lpszProcName, DWORD cbArgs, PVOID lpvArgs) ;
DWORD __cdecl DEBUG__SHELL_CallDll (PCHAR lpszDll, PCHAR lpszProcName, DWORD cbArgs, PVOID lpvArgs) ;
DWORD __cdecl STATIC__SHELL_CallDll (PCHAR lpszDll, PCHAR lpszProcName, DWORD cbArgs, PVOID lpvArgs) ;
DWORD __cdecl PAGEABLE__SHELL_CallDll (PCHAR lpszDll, PCHAR lpszProcName, DWORD cbArgs, PVOID lpvArgs) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__SHELL_BroadcastSystemMessage (DWORD dwFlags, PDWORD lpdwRecipients, DWORD uMsg, DWORD wparam, DWORD lparam) ;
DWORD __cdecl INIT__SHELL_BroadcastSystemMessage (DWORD dwFlags, PDWORD lpdwRecipients, DWORD uMsg, DWORD wparam, DWORD lparam) ;
DWORD __cdecl DEBUG__SHELL_BroadcastSystemMessage (DWORD dwFlags, PDWORD lpdwRecipients, DWORD uMsg, DWORD wparam, DWORD lparam) ;
DWORD __cdecl STATIC__SHELL_BroadcastSystemMessage (DWORD dwFlags, PDWORD lpdwRecipients, DWORD uMsg, DWORD wparam, DWORD lparam) ;
DWORD __cdecl PAGEABLE__SHELL_BroadcastSystemMessage (DWORD dwFlags, PDWORD lpdwRecipients, DWORD uMsg, DWORD wparam, DWORD lparam) ;
#endif

#ifdef WIN40_OR_LATER
SYSBHOOK_HANDLE __cdecl LOCK__SHELL_HookSystemBroadcast (SYSBHOOK_CALLBACK pfnSysBHookCallBack, DWORD dwRefData, DWORD dwCallOrder) ;
SYSBHOOK_HANDLE __cdecl INIT__SHELL_HookSystemBroadcast (SYSBHOOK_CALLBACK pfnSysBHookCallBack, DWORD dwRefData, DWORD dwCallOrder) ;
SYSBHOOK_HANDLE __cdecl DEBUG__SHELL_HookSystemBroadcast (SYSBHOOK_CALLBACK pfnSysBHookCallBack, DWORD dwRefData, DWORD dwCallOrder) ;
SYSBHOOK_HANDLE __cdecl STATIC__SHELL_HookSystemBroadcast (SYSBHOOK_CALLBACK pfnSysBHookCallBack, DWORD dwRefData, DWORD dwCallOrder) ;
SYSBHOOK_HANDLE __cdecl PAGEABLE__SHELL_HookSystemBroadcast (SYSBHOOK_CALLBACK pfnSysBHookCallBack, DWORD dwRefData, DWORD dwCallOrder) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__SHELL_UnhookSystemBroadcast (SYSBHOOK_HANDLE SysBHookHandle) ;
VOID __cdecl INIT__SHELL_UnhookSystemBroadcast (SYSBHOOK_HANDLE SysBHookHandle) ;
VOID __cdecl DEBUG__SHELL_UnhookSystemBroadcast (SYSBHOOK_HANDLE SysBHookHandle) ;
VOID __cdecl STATIC__SHELL_UnhookSystemBroadcast (SYSBHOOK_HANDLE SysBHookHandle) ;
VOID __cdecl PAGEABLE__SHELL_UnhookSystemBroadcast (SYSBHOOK_HANDLE SysBHookHandle) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__SHELL_FreeLibrary (LIBHANDLE hLib) ;
VOID __cdecl INIT__SHELL_FreeLibrary (LIBHANDLE hLib) ;
VOID __cdecl DEBUG__SHELL_FreeLibrary (LIBHANDLE hLib) ;
VOID __cdecl STATIC__SHELL_FreeLibrary (LIBHANDLE hLib) ;
VOID __cdecl PAGEABLE__SHELL_FreeLibrary (LIBHANDLE hLib) ;
#endif

#ifdef WIN40_OR_LATER
SEGOFFSET __cdecl LOCK__SHELL_GetProcAddress (LIBHANDLE hLib, PCHAR szProcname) ;
SEGOFFSET __cdecl INIT__SHELL_GetProcAddress (LIBHANDLE hLib, PCHAR szProcname) ;
SEGOFFSET __cdecl DEBUG__SHELL_GetProcAddress (LIBHANDLE hLib, PCHAR szProcname) ;
SEGOFFSET __cdecl STATIC__SHELL_GetProcAddress (LIBHANDLE hLib, PCHAR szProcname) ;
SEGOFFSET __cdecl PAGEABLE__SHELL_GetProcAddress (LIBHANDLE hLib, PCHAR szProcname) ;
#endif

#ifdef WIN40_OR_LATER
LIBHANDLE __cdecl LOCK__SHELL_LoadLibrary (PCHAR szLibname) ;
LIBHANDLE __cdecl INIT__SHELL_LoadLibrary (PCHAR szLibname) ;
LIBHANDLE __cdecl DEBUG__SHELL_LoadLibrary (PCHAR szLibname) ;
LIBHANDLE __cdecl STATIC__SHELL_LoadLibrary (PCHAR szLibname) ;
LIBHANDLE __cdecl PAGEABLE__SHELL_LoadLibrary (PCHAR szLibname) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__SHELL_PostMessage (HANDLE hWnd, DWORD uMsg, WORD wParam, DWORD lParam, PPostMessage_HANDLER pCallback, PVOID dwRefData) ;
BOOL __cdecl INIT__SHELL_PostMessage (HANDLE hWnd, DWORD uMsg, WORD wParam, DWORD lParam, PPostMessage_HANDLER pCallback, PVOID dwRefData) ;
BOOL __cdecl DEBUG__SHELL_PostMessage (HANDLE hWnd, DWORD uMsg, WORD wParam, DWORD lParam, PPostMessage_HANDLER pCallback, PVOID dwRefData) ;
BOOL __cdecl STATIC__SHELL_PostMessage (HANDLE hWnd, DWORD uMsg, WORD wParam, DWORD lParam, PPostMessage_HANDLER pCallback, PVOID dwRefData) ;
BOOL __cdecl PAGEABLE__SHELL_PostMessage (HANDLE hWnd, DWORD uMsg, WORD wParam, DWORD lParam, PPostMessage_HANDLER pCallback, PVOID dwRefData) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__SHELL_PostShellMessage (WORD wParam, DWORD lParam) ;
VOID __cdecl INIT__SHELL_PostShellMessage (WORD wParam, DWORD lParam) ;
VOID __cdecl DEBUG__SHELL_PostShellMessage (WORD wParam, DWORD lParam) ;
VOID __cdecl STATIC__SHELL_PostShellMessage (WORD wParam, DWORD lParam) ;
VOID __cdecl PAGEABLE__SHELL_PostShellMessage (WORD wParam, DWORD lParam) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_SHELL_Hook_Properties (DWORD GroupID, PCHAR DLLName, PPropHook_HANDLER PropertyHook, PPropHook_THUNK pThunk) ;
BOOL __stdcall INIT_SHELL_Hook_Properties (DWORD GroupID, PCHAR DLLName, PPropHook_HANDLER PropertyHook, PPropHook_THUNK pThunk) ;
BOOL __stdcall DEBUG_SHELL_Hook_Properties (DWORD GroupID, PCHAR DLLName, PPropHook_HANDLER PropertyHook, PPropHook_THUNK pThunk) ;
BOOL __stdcall STATIC_SHELL_Hook_Properties (DWORD GroupID, PCHAR DLLName, PPropHook_HANDLER PropertyHook, PPropHook_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_SHELL_Hook_Properties (DWORD GroupID, PCHAR DLLName, PPropHook_HANDLER PropertyHook, PPropHook_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_SHELL_Unhook_Properties (DWORD GroupID, PPropHook_THUNK pThunk, PPropHook_HANDLER pCallback) ;
VOID __stdcall INIT_SHELL_Unhook_Properties (DWORD GroupID, PPropHook_THUNK pThunk, PPropHook_HANDLER pCallback) ;
VOID __stdcall DEBUG_SHELL_Unhook_Properties (DWORD GroupID, PPropHook_THUNK pThunk, PPropHook_HANDLER pCallback) ;
VOID __stdcall STATIC_SHELL_Unhook_Properties (DWORD GroupID, PPropHook_THUNK pThunk, PPropHook_HANDLER pCallback) ;
VOID __stdcall PAGEABLE_SHELL_Unhook_Properties (DWORD GroupID, PPropHook_THUNK pThunk, PPropHook_HANDLER pCallback) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__SHELL_ShellExecute (PSHEXPACKET shex) ;
DWORD __cdecl INIT__SHELL_ShellExecute (PSHEXPACKET shex) ;
DWORD __cdecl DEBUG__SHELL_ShellExecute (PSHEXPACKET shex) ;
DWORD __cdecl STATIC__SHELL_ShellExecute (PSHEXPACKET shex) ;
DWORD __cdecl PAGEABLE__SHELL_ShellExecute (PSHEXPACKET shex) ;
#endif

BOOL __stdcall LOCK_V86MMGR_Allocate_Buffer (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD nBytes, WORD theSeg, CONST VOID * theOffset, BOOL bCopyToV86, PDWORD pActual, PSEGOFFSET pBuffer) ;
BOOL __stdcall INIT_V86MMGR_Allocate_Buffer (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD nBytes, WORD theSeg, CONST VOID * theOffset, BOOL bCopyToV86, PDWORD pActual, PSEGOFFSET pBuffer) ;
BOOL __stdcall DEBUG_V86MMGR_Allocate_Buffer (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD nBytes, WORD theSeg, CONST VOID * theOffset, BOOL bCopyToV86, PDWORD pActual, PSEGOFFSET pBuffer) ;
BOOL __stdcall STATIC_V86MMGR_Allocate_Buffer (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD nBytes, WORD theSeg, CONST VOID * theOffset, BOOL bCopyToV86, PDWORD pActual, PSEGOFFSET pBuffer) ;
BOOL __stdcall PAGEABLE_V86MMGR_Allocate_Buffer (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD nBytes, WORD theSeg, CONST VOID * theOffset, BOOL bCopyToV86, PDWORD pActual, PSEGOFFSET pBuffer) ;

DWORD __stdcall LOCK_V86MMGR_Allocate_V86_Pages (VMHANDLE hVM, DWORD nRequested, DWORD nMinimum, DWORD Flags) ;
DWORD __stdcall INIT_V86MMGR_Allocate_V86_Pages (VMHANDLE hVM, DWORD nRequested, DWORD nMinimum, DWORD Flags) ;
DWORD __stdcall DEBUG_V86MMGR_Allocate_V86_Pages (VMHANDLE hVM, DWORD nRequested, DWORD nMinimum, DWORD Flags) ;
DWORD __stdcall STATIC_V86MMGR_Allocate_V86_Pages (VMHANDLE hVM, DWORD nRequested, DWORD nMinimum, DWORD Flags) ;
DWORD __stdcall PAGEABLE_V86MMGR_Allocate_V86_Pages (VMHANDLE hVM, DWORD nRequested, DWORD nMinimum, DWORD Flags) ;

VOID __stdcall LOCK_V86MMGR_Free_Buffer (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD nBytes, WORD theSeg, CONST VOID * theOffset, BOOL bCopyFromV86) ;
VOID __stdcall INIT_V86MMGR_Free_Buffer (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD nBytes, WORD theSeg, CONST VOID * theOffset, BOOL bCopyFromV86) ;
VOID __stdcall DEBUG_V86MMGR_Free_Buffer (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD nBytes, WORD theSeg, CONST VOID * theOffset, BOOL bCopyFromV86) ;
VOID __stdcall STATIC_V86MMGR_Free_Buffer (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD nBytes, WORD theSeg, CONST VOID * theOffset, BOOL bCopyFromV86) ;
VOID __stdcall PAGEABLE_V86MMGR_Free_Buffer (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD nBytes, WORD theSeg, CONST VOID * theOffset, BOOL bCopyFromV86) ;

VOID __stdcall LOCK_V86MMGR_Free_Page_Map_Region (MAPHANDLE hMap) ;
VOID __stdcall INIT_V86MMGR_Free_Page_Map_Region (MAPHANDLE hMap) ;
VOID __stdcall DEBUG_V86MMGR_Free_Page_Map_Region (MAPHANDLE hMap) ;
VOID __stdcall STATIC_V86MMGR_Free_Page_Map_Region (MAPHANDLE hMap) ;
VOID __stdcall PAGEABLE_V86MMGR_Free_Page_Map_Region (MAPHANDLE hMap) ;

VOID __stdcall LOCK_V86MMGR_Get_EMS_XMS_Limits (VMHANDLE hVM, PALLREGS pRegisters) ;
VOID __stdcall INIT_V86MMGR_Get_EMS_XMS_Limits (VMHANDLE hVM, PALLREGS pRegisters) ;
VOID __stdcall DEBUG_V86MMGR_Get_EMS_XMS_Limits (VMHANDLE hVM, PALLREGS pRegisters) ;
VOID __stdcall STATIC_V86MMGR_Get_EMS_XMS_Limits (VMHANDLE hVM, PALLREGS pRegisters) ;
VOID __stdcall PAGEABLE_V86MMGR_Get_EMS_XMS_Limits (VMHANDLE hVM, PALLREGS pRegisters) ;

DWORD __stdcall LOCK_V86MMGR_Get_Mapping_Info (void) ;
DWORD __stdcall INIT_V86MMGR_Get_Mapping_Info (void) ;
DWORD __stdcall DEBUG_V86MMGR_Get_Mapping_Info (void) ;
DWORD __stdcall STATIC_V86MMGR_Get_Mapping_Info (void) ;
DWORD __stdcall PAGEABLE_V86MMGR_Get_Mapping_Info (void) ;

DWORD __stdcall LOCK_V86MMGR_Get_Version (void) ;
DWORD __stdcall INIT_V86MMGR_Get_Version (void) ;
DWORD __stdcall DEBUG_V86MMGR_Get_Version (void) ;
DWORD __stdcall STATIC_V86MMGR_Get_Version (void) ;
DWORD __stdcall PAGEABLE_V86MMGR_Get_Version (void) ;

DWORD __stdcall LOCK_V86MMGR_Get_VM_Flat_Sel (VMHANDLE hVM) ;
DWORD __stdcall INIT_V86MMGR_Get_VM_Flat_Sel (VMHANDLE hVM) ;
DWORD __stdcall DEBUG_V86MMGR_Get_VM_Flat_Sel (VMHANDLE hVM) ;
DWORD __stdcall STATIC_V86MMGR_Get_VM_Flat_Sel (VMHANDLE hVM) ;
DWORD __stdcall PAGEABLE_V86MMGR_Get_VM_Flat_Sel (VMHANDLE hVM) ;

DWORD __stdcall LOCK_V86MMGR_Get_Xlat_Buff_State (VMHANDLE hVM, PDWORD pNotInUse, PDWORD pTotalSize) ;
DWORD __stdcall INIT_V86MMGR_Get_Xlat_Buff_State (VMHANDLE hVM, PDWORD pNotInUse, PDWORD pTotalSize) ;
DWORD __stdcall DEBUG_V86MMGR_Get_Xlat_Buff_State (VMHANDLE hVM, PDWORD pNotInUse, PDWORD pTotalSize) ;
DWORD __stdcall STATIC_V86MMGR_Get_Xlat_Buff_State (VMHANDLE hVM, PDWORD pNotInUse, PDWORD pTotalSize) ;
DWORD __stdcall PAGEABLE_V86MMGR_Get_Xlat_Buff_State (VMHANDLE hVM, PDWORD pNotInUse, PDWORD pTotalSize) ;

DWORD __stdcall LOCK_V86MMGR_GetPgStatus (VMHANDLE hVM, DWORD PageNumber) ;
DWORD __stdcall INIT_V86MMGR_GetPgStatus (VMHANDLE hVM, DWORD PageNumber) ;
DWORD __stdcall DEBUG_V86MMGR_GetPgStatus (VMHANDLE hVM, DWORD PageNumber) ;
DWORD __stdcall STATIC_V86MMGR_GetPgStatus (VMHANDLE hVM, DWORD PageNumber) ;
DWORD __stdcall PAGEABLE_V86MMGR_GetPgStatus (VMHANDLE hVM, DWORD PageNumber) ;

PVOID __stdcall LOCK_V86MMGR_Load_Client_Ptr (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD ClientRegister, PWORD pSeg) ;
PVOID __stdcall INIT_V86MMGR_Load_Client_Ptr (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD ClientRegister, PWORD pSeg) ;
PVOID __stdcall DEBUG_V86MMGR_Load_Client_Ptr (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD ClientRegister, PWORD pSeg) ;
PVOID __stdcall STATIC_V86MMGR_Load_Client_Ptr (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD ClientRegister, PWORD pSeg) ;
PVOID __stdcall PAGEABLE_V86MMGR_Load_Client_Ptr (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, DWORD ClientRegister, PWORD pSeg) ;

DWORD __stdcall LOCK_V86MMGR_LocalGlobalReg (DWORD pgStart, DWORD pgEnd) ;
DWORD __stdcall INIT_V86MMGR_LocalGlobalReg (DWORD pgStart, DWORD pgEnd) ;
DWORD __stdcall DEBUG_V86MMGR_LocalGlobalReg (DWORD pgStart, DWORD pgEnd) ;
DWORD __stdcall STATIC_V86MMGR_LocalGlobalReg (DWORD pgStart, DWORD pgEnd) ;
DWORD __stdcall PAGEABLE_V86MMGR_LocalGlobalReg (DWORD pgStart, DWORD pgEnd) ;

BOOL __stdcall LOCK_V86MMGR_Map_Pages (CONST VOID * LinearAddress, DWORD nBytes, PMAPHANDLE phMap, PPVOID pRetLinAddr) ;
BOOL __stdcall INIT_V86MMGR_Map_Pages (CONST VOID * LinearAddress, DWORD nBytes, PMAPHANDLE phMap, PPVOID pRetLinAddr) ;
BOOL __stdcall DEBUG_V86MMGR_Map_Pages (CONST VOID * LinearAddress, DWORD nBytes, PMAPHANDLE phMap, PPVOID pRetLinAddr) ;
BOOL __stdcall STATIC_V86MMGR_Map_Pages (CONST VOID * LinearAddress, DWORD nBytes, PMAPHANDLE phMap, PPVOID pRetLinAddr) ;
BOOL __stdcall PAGEABLE_V86MMGR_Map_Pages (CONST VOID * LinearAddress, DWORD nBytes, PMAPHANDLE phMap, PPVOID pRetLinAddr) ;

#define V86MMGR_NoUMBInitCalls() VMMcall(V86MMGR_NoUMBInitCalls)
BOOL __stdcall LOCK_V86MMGR_ResetBasePages (VMHANDLE hVM, DWORD Count, DWORD PageNumber) ;
BOOL __stdcall INIT_V86MMGR_ResetBasePages (VMHANDLE hVM, DWORD Count, DWORD PageNumber) ;
BOOL __stdcall DEBUG_V86MMGR_ResetBasePages (VMHANDLE hVM, DWORD Count, DWORD PageNumber) ;
BOOL __stdcall STATIC_V86MMGR_ResetBasePages (VMHANDLE hVM, DWORD Count, DWORD PageNumber) ;
BOOL __stdcall PAGEABLE_V86MMGR_ResetBasePages (VMHANDLE hVM, DWORD Count, DWORD PageNumber) ;

BOOL __stdcall LOCK_V86MMGR_Set_EMS_XMS_Limits (VMHANDLE hVM, CONST ALLREGS * pRegisters) ;
BOOL __stdcall INIT_V86MMGR_Set_EMS_XMS_Limits (VMHANDLE hVM, CONST ALLREGS * pRegisters) ;
BOOL __stdcall DEBUG_V86MMGR_Set_EMS_XMS_Limits (VMHANDLE hVM, CONST ALLREGS * pRegisters) ;
BOOL __stdcall STATIC_V86MMGR_Set_EMS_XMS_Limits (VMHANDLE hVM, CONST ALLREGS * pRegisters) ;
BOOL __stdcall PAGEABLE_V86MMGR_Set_EMS_XMS_Limits (VMHANDLE hVM, CONST ALLREGS * pRegisters) ;

VOID __stdcall LOCK_V86MMGR_Set_Mapping_Info (BYTE minCopy, BYTE maxCopy, BYTE minPrivGlobal, BYTE minSharedGlobal, BYTE maxGlobal) ;
VOID __stdcall INIT_V86MMGR_Set_Mapping_Info (BYTE minCopy, BYTE maxCopy, BYTE minPrivGlobal, BYTE minSharedGlobal, BYTE maxGlobal) ;
VOID __stdcall DEBUG_V86MMGR_Set_Mapping_Info (BYTE minCopy, BYTE maxCopy, BYTE minPrivGlobal, BYTE minSharedGlobal, BYTE maxGlobal) ;
VOID __stdcall STATIC_V86MMGR_Set_Mapping_Info (BYTE minCopy, BYTE maxCopy, BYTE minPrivGlobal, BYTE minSharedGlobal, BYTE maxGlobal) ;
VOID __stdcall PAGEABLE_V86MMGR_Set_Mapping_Info (BYTE minCopy, BYTE maxCopy, BYTE minPrivGlobal, BYTE minSharedGlobal, BYTE maxGlobal) ;

VOID __stdcall LOCK_V86MMGR_Set_Xlat_Buff_State (VMHANDLE hVM, DWORD BufferSegment, DWORD nBytesAvailable, DWORD nBytesTotal) ;
VOID __stdcall INIT_V86MMGR_Set_Xlat_Buff_State (VMHANDLE hVM, DWORD BufferSegment, DWORD nBytesAvailable, DWORD nBytesTotal) ;
VOID __stdcall DEBUG_V86MMGR_Set_Xlat_Buff_State (VMHANDLE hVM, DWORD BufferSegment, DWORD nBytesAvailable, DWORD nBytesTotal) ;
VOID __stdcall STATIC_V86MMGR_Set_Xlat_Buff_State (VMHANDLE hVM, DWORD BufferSegment, DWORD nBytesAvailable, DWORD nBytesTotal) ;
VOID __stdcall PAGEABLE_V86MMGR_Set_Xlat_Buff_State (VMHANDLE hVM, DWORD BufferSegment, DWORD nBytesAvailable, DWORD nBytesTotal) ;

BOOL __stdcall LOCK_V86MMGR_SetAvailMapPgs (DWORD FirstPageNum, DWORD nPages) ;
BOOL __stdcall INIT_V86MMGR_SetAvailMapPgs (DWORD FirstPageNum, DWORD nPages) ;
BOOL __stdcall DEBUG_V86MMGR_SetAvailMapPgs (DWORD FirstPageNum, DWORD nPages) ;
BOOL __stdcall STATIC_V86MMGR_SetAvailMapPgs (DWORD FirstPageNum, DWORD nPages) ;
BOOL __stdcall PAGEABLE_V86MMGR_SetAvailMapPgs (DWORD FirstPageNum, DWORD nPages) ;

#define V86MMGR_SetLocalA20() VMMcall(V86MMGR_SetLocalA20)
BOOL __stdcall LOCK_V86MMGR_Xlat_API (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, CONST VOID * pScript) ;
BOOL __stdcall INIT_V86MMGR_Xlat_API (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, CONST VOID * pScript) ;
BOOL __stdcall DEBUG_V86MMGR_Xlat_API (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, CONST VOID * pScript) ;
BOOL __stdcall STATIC_V86MMGR_Xlat_API (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, CONST VOID * pScript) ;
BOOL __stdcall PAGEABLE_V86MMGR_Xlat_API (VMHANDLE hVM, CONST CLIENT_STRUCT * pRegs, CONST VOID * pScript) ;

VMHANDLE __stdcall LOCK_VCD_Get_Focus (DWORD ComPort) ;
VMHANDLE __stdcall INIT_VCD_Get_Focus (DWORD ComPort) ;
VMHANDLE __stdcall DEBUG_VCD_Get_Focus (DWORD ComPort) ;
VMHANDLE __stdcall STATIC_VCD_Get_Focus (DWORD ComPort) ;
VMHANDLE __stdcall PAGEABLE_VCD_Get_Focus (DWORD ComPort) ;

DWORD __stdcall LOCK_VCD_Get_Version (void) ;
DWORD __stdcall INIT_VCD_Get_Version (void) ;
DWORD __stdcall DEBUG_VCD_Get_Version (void) ;
DWORD __stdcall STATIC_VCD_Get_Version (void) ;
DWORD __stdcall PAGEABLE_VCD_Get_Version (void) ;

BOOL __stdcall LOCK_VCD_Set_Port_Global (DWORD ComPort, BOOL bLocalPort) ;
BOOL __stdcall INIT_VCD_Set_Port_Global (DWORD ComPort, BOOL bLocalPort) ;
BOOL __stdcall DEBUG_VCD_Set_Port_Global (DWORD ComPort, BOOL bLocalPort) ;
BOOL __stdcall STATIC_VCD_Set_Port_Global (DWORD ComPort, BOOL bLocalPort) ;
BOOL __stdcall PAGEABLE_VCD_Set_Port_Global (DWORD ComPort, BOOL bLocalPort) ;

COMHANDLE __stdcall LOCK_VCD_Virtualize_Port (DWORD ComPort, DWORD Flags, DWORD ExtraComBytes, DWORD ExtraCBBytes, VCD_PROCLIST * pProcList) ;
COMHANDLE __stdcall INIT_VCD_Virtualize_Port (DWORD ComPort, DWORD Flags, DWORD ExtraComBytes, DWORD ExtraCBBytes, VCD_PROCLIST * pProcList) ;
COMHANDLE __stdcall DEBUG_VCD_Virtualize_Port (DWORD ComPort, DWORD Flags, DWORD ExtraComBytes, DWORD ExtraCBBytes, VCD_PROCLIST * pProcList) ;
COMHANDLE __stdcall STATIC_VCD_Virtualize_Port (DWORD ComPort, DWORD Flags, DWORD ExtraComBytes, DWORD ExtraCBBytes, VCD_PROCLIST * pProcList) ;
COMHANDLE __stdcall PAGEABLE_VCD_Virtualize_Port (DWORD ComPort, DWORD Flags, DWORD ExtraComBytes, DWORD ExtraCBBytes, VCD_PROCLIST * pProcList) ;

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VMCPD_Set_CR0_State (THREADHANDLE hThread, DWORD CR0_Flags) ;
VOID __stdcall INIT_VMCPD_Set_CR0_State (THREADHANDLE hThread, DWORD CR0_Flags) ;
VOID __stdcall DEBUG_VMCPD_Set_CR0_State (THREADHANDLE hThread, DWORD CR0_Flags) ;
VOID __stdcall STATIC_VMCPD_Set_CR0_State (THREADHANDLE hThread, DWORD CR0_Flags) ;
VOID __stdcall PAGEABLE_VMCPD_Set_CR0_State (THREADHANDLE hThread, DWORD CR0_Flags) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VMCPD_Get_CR0_State (THREADHANDLE hThread) ;
DWORD __stdcall INIT_VMCPD_Get_CR0_State (THREADHANDLE hThread) ;
DWORD __stdcall DEBUG_VMCPD_Get_CR0_State (THREADHANDLE hThread) ;
DWORD __stdcall STATIC_VMCPD_Get_CR0_State (THREADHANDLE hThread) ;
DWORD __stdcall PAGEABLE_VMCPD_Get_CR0_State (THREADHANDLE hThread) ;
#endif

VOID __stdcall LOCK_VMCPD_Get_Thread_State (THREADHANDLE hThread, PDWORD pState) ;
VOID __stdcall INIT_VMCPD_Get_Thread_State (THREADHANDLE hThread, PDWORD pState) ;
VOID __stdcall DEBUG_VMCPD_Get_Thread_State (THREADHANDLE hThread, PDWORD pState) ;
VOID __stdcall STATIC_VMCPD_Get_Thread_State (THREADHANDLE hThread, PDWORD pState) ;
VOID __stdcall PAGEABLE_VMCPD_Get_Thread_State (THREADHANDLE hThread, PDWORD pState) ;

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VMCPD_Set_Thread_State (THREADHANDLE hThread, PDWORD pState) ;
VOID __stdcall INIT_VMCPD_Set_Thread_State (THREADHANDLE hThread, PDWORD pState) ;
VOID __stdcall DEBUG_VMCPD_Set_Thread_State (THREADHANDLE hThread, PDWORD pState) ;
VOID __stdcall STATIC_VMCPD_Set_Thread_State (THREADHANDLE hThread, PDWORD pState) ;
VOID __stdcall PAGEABLE_VMCPD_Set_Thread_State (THREADHANDLE hThread, PDWORD pState) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VPICD_Virtual_EOI (IRQHANDLE hIRQ, VMHANDLE hVM) ;
VOID __stdcall INIT_VPICD_Virtual_EOI (IRQHANDLE hIRQ, VMHANDLE hVM) ;
VOID __stdcall DEBUG_VPICD_Virtual_EOI (IRQHANDLE hIRQ, VMHANDLE hVM) ;
VOID __stdcall STATIC_VPICD_Virtual_EOI (IRQHANDLE hIRQ, VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VPICD_Virtual_EOI (IRQHANDLE hIRQ, VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VTD_Get_Date_And_Time (PDWORD pDate) ;
DWORD __stdcall INIT_VTD_Get_Date_And_Time (PDWORD pDate) ;
DWORD __stdcall DEBUG_VTD_Get_Date_And_Time (PDWORD pDate) ;
DWORD __stdcall STATIC_VTD_Get_Date_And_Time (PDWORD pDate) ;
DWORD __stdcall PAGEABLE_VTD_Get_Date_And_Time (PDWORD pDate) ;
#endif

DWORD __stdcall LOCK_VDD_Get_ModTime (VMHANDLE hVM) ;
DWORD __stdcall INIT_VDD_Get_ModTime (VMHANDLE hVM) ;
DWORD __stdcall DEBUG_VDD_Get_ModTime (VMHANDLE hVM) ;
DWORD __stdcall STATIC_VDD_Get_ModTime (VMHANDLE hVM) ;
DWORD __stdcall PAGEABLE_VDD_Get_ModTime (VMHANDLE hVM) ;

DWORD __stdcall LOCK_VDD_Get_Version (PPCHAR pID) ;
DWORD __stdcall INIT_VDD_Get_Version (PPCHAR pID) ;
DWORD __stdcall DEBUG_VDD_Get_Version (PPCHAR pID) ;
DWORD __stdcall STATIC_VDD_Get_Version (PPCHAR pID) ;
DWORD __stdcall PAGEABLE_VDD_Get_Version (PPCHAR pID) ;

VOID __stdcall LOCK_VDD_Msg_BakColor (VMHANDLE hVM, DWORD Color) ;
VOID __stdcall INIT_VDD_Msg_BakColor (VMHANDLE hVM, DWORD Color) ;
VOID __stdcall DEBUG_VDD_Msg_BakColor (VMHANDLE hVM, DWORD Color) ;
VOID __stdcall STATIC_VDD_Msg_BakColor (VMHANDLE hVM, DWORD Color) ;
VOID __stdcall PAGEABLE_VDD_Msg_BakColor (VMHANDLE hVM, DWORD Color) ;

VOID __stdcall LOCK_VDD_Msg_ClrScrn (VMHANDLE hVM, DWORD Attribute, PDWORD pWidth, PDWORD pHeight) ;
VOID __stdcall INIT_VDD_Msg_ClrScrn (VMHANDLE hVM, DWORD Attribute, PDWORD pWidth, PDWORD pHeight) ;
VOID __stdcall DEBUG_VDD_Msg_ClrScrn (VMHANDLE hVM, DWORD Attribute, PDWORD pWidth, PDWORD pHeight) ;
VOID __stdcall STATIC_VDD_Msg_ClrScrn (VMHANDLE hVM, DWORD Attribute, PDWORD pWidth, PDWORD pHeight) ;
VOID __stdcall PAGEABLE_VDD_Msg_ClrScrn (VMHANDLE hVM, DWORD Attribute, PDWORD pWidth, PDWORD pHeight) ;

VOID __stdcall LOCK_VDD_Msg_ForColor (VMHANDLE hVM, DWORD Color) ;
VOID __stdcall INIT_VDD_Msg_ForColor (VMHANDLE hVM, DWORD Color) ;
VOID __stdcall DEBUG_VDD_Msg_ForColor (VMHANDLE hVM, DWORD Color) ;
VOID __stdcall STATIC_VDD_Msg_ForColor (VMHANDLE hVM, DWORD Color) ;
VOID __stdcall PAGEABLE_VDD_Msg_ForColor (VMHANDLE hVM, DWORD Color) ;

VOID __stdcall LOCK_VDD_SetCursPos (VMHANDLE hVM, DWORD Row, DWORD Col) ;
VOID __stdcall INIT_VDD_SetCursPos (VMHANDLE hVM, DWORD Row, DWORD Col) ;
VOID __stdcall DEBUG_VDD_SetCursPos (VMHANDLE hVM, DWORD Row, DWORD Col) ;
VOID __stdcall STATIC_VDD_SetCursPos (VMHANDLE hVM, DWORD Row, DWORD Col) ;
VOID __stdcall PAGEABLE_VDD_SetCursPos (VMHANDLE hVM, DWORD Row, DWORD Col) ;

VOID __stdcall LOCK_VDD_Msg_TextOut (VMHANDLE hVM, CONST CHAR * pText, DWORD nBytes, DWORD Row, DWORD Col) ;
VOID __stdcall INIT_VDD_Msg_TextOut (VMHANDLE hVM, CONST CHAR * pText, DWORD nBytes, DWORD Row, DWORD Col) ;
VOID __stdcall DEBUG_VDD_Msg_TextOut (VMHANDLE hVM, CONST CHAR * pText, DWORD nBytes, DWORD Row, DWORD Col) ;
VOID __stdcall STATIC_VDD_Msg_TextOut (VMHANDLE hVM, CONST CHAR * pText, DWORD nBytes, DWORD Row, DWORD Col) ;
VOID __stdcall PAGEABLE_VDD_Msg_TextOut (VMHANDLE hVM, CONST CHAR * pText, DWORD nBytes, DWORD Row, DWORD Col) ;

VOID __stdcall LOCK_VDD_PIF_State (VMHANDLE hVM, WORD PifBits) ;
VOID __stdcall INIT_VDD_PIF_State (VMHANDLE hVM, WORD PifBits) ;
VOID __stdcall DEBUG_VDD_PIF_State (VMHANDLE hVM, WORD PifBits) ;
VOID __stdcall STATIC_VDD_PIF_State (VMHANDLE hVM, WORD PifBits) ;
VOID __stdcall PAGEABLE_VDD_PIF_State (VMHANDLE hVM, WORD PifBits) ;

BOOL __stdcall LOCK_VDD_Query_Access (PVOID pVDD) ;
BOOL __stdcall INIT_VDD_Query_Access (PVOID pVDD) ;
BOOL __stdcall DEBUG_VDD_Query_Access (PVOID pVDD) ;
BOOL __stdcall STATIC_VDD_Query_Access (PVOID pVDD) ;
BOOL __stdcall PAGEABLE_VDD_Query_Access (PVOID pVDD) ;

VOID __stdcall LOCK_VDD_Set_HCurTrk (VMHANDLE hVM) ;
VOID __stdcall INIT_VDD_Set_HCurTrk (VMHANDLE hVM) ;
VOID __stdcall DEBUG_VDD_Set_HCurTrk (VMHANDLE hVM) ;
VOID __stdcall STATIC_VDD_Set_HCurTrk (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VDD_Set_HCurTrk (VMHANDLE hVM) ;

VOID __stdcall LOCK_VDD_Set_VMType (VMHANDLE hVM, BOOL bInWindow) ;
VOID __stdcall INIT_VDD_Set_VMType (VMHANDLE hVM, BOOL bInWindow) ;
VOID __stdcall DEBUG_VDD_Set_VMType (VMHANDLE hVM, BOOL bInWindow) ;
VOID __stdcall STATIC_VDD_Set_VMType (VMHANDLE hVM, BOOL bInWindow) ;
VOID __stdcall PAGEABLE_VDD_Set_VMType (VMHANDLE hVM, BOOL bInWindow) ;

BOOL __stdcall LOCK_VDMAD_Copy_From_Buffer (DMABUFFERID BufID, PVOID LinearRegion, DWORD BufferOffset, DWORD BufferSize, PDWORD pResult) ;
BOOL __stdcall INIT_VDMAD_Copy_From_Buffer (DMABUFFERID BufID, PVOID LinearRegion, DWORD BufferOffset, DWORD BufferSize, PDWORD pResult) ;
BOOL __stdcall DEBUG_VDMAD_Copy_From_Buffer (DMABUFFERID BufID, PVOID LinearRegion, DWORD BufferOffset, DWORD BufferSize, PDWORD pResult) ;
BOOL __stdcall STATIC_VDMAD_Copy_From_Buffer (DMABUFFERID BufID, PVOID LinearRegion, DWORD BufferOffset, DWORD BufferSize, PDWORD pResult) ;
BOOL __stdcall PAGEABLE_VDMAD_Copy_From_Buffer (DMABUFFERID BufID, PVOID LinearRegion, DWORD BufferOffset, DWORD BufferSize, PDWORD pResult) ;

BOOL __stdcall LOCK_VDMAD_Copy_To_Buffer (DMABUFFERID BufID, CONST VOID * LinearRegion, DWORD BufferOffset, DWORD BufferSize, PDWORD pResult) ;
BOOL __stdcall INIT_VDMAD_Copy_To_Buffer (DMABUFFERID BufID, CONST VOID * LinearRegion, DWORD BufferOffset, DWORD BufferSize, PDWORD pResult) ;
BOOL __stdcall DEBUG_VDMAD_Copy_To_Buffer (DMABUFFERID BufID, CONST VOID * LinearRegion, DWORD BufferOffset, DWORD BufferSize, PDWORD pResult) ;
BOOL __stdcall STATIC_VDMAD_Copy_To_Buffer (DMABUFFERID BufID, CONST VOID * LinearRegion, DWORD BufferOffset, DWORD BufferSize, PDWORD pResult) ;
BOOL __stdcall PAGEABLE_VDMAD_Copy_To_Buffer (DMABUFFERID BufID, CONST VOID * LinearRegion, DWORD BufferOffset, DWORD BufferSize, PDWORD pResult) ;

VOID __stdcall LOCK_VDMAD_Default_Handler (VMHANDLE hVM, DMAHANDLE hDMA) ;
VOID __stdcall INIT_VDMAD_Default_Handler (VMHANDLE hVM, DMAHANDLE hDMA) ;
VOID __stdcall DEBUG_VDMAD_Default_Handler (VMHANDLE hVM, DMAHANDLE hDMA) ;
VOID __stdcall STATIC_VDMAD_Default_Handler (VMHANDLE hVM, DMAHANDLE hDMA) ;
VOID __stdcall PAGEABLE_VDMAD_Default_Handler (VMHANDLE hVM, DMAHANDLE hDMA) ;

BOOL __stdcall LOCK_VDMAD_Disable_Translation (VMHANDLE hVM, DMAHANDLE hDMA) ;
BOOL __stdcall INIT_VDMAD_Disable_Translation (VMHANDLE hVM, DMAHANDLE hDMA) ;
BOOL __stdcall DEBUG_VDMAD_Disable_Translation (VMHANDLE hVM, DMAHANDLE hDMA) ;
BOOL __stdcall STATIC_VDMAD_Disable_Translation (VMHANDLE hVM, DMAHANDLE hDMA) ;
BOOL __stdcall PAGEABLE_VDMAD_Disable_Translation (VMHANDLE hVM, DMAHANDLE hDMA) ;

BOOL __stdcall LOCK_VDMAD_Enable_Translation (VMHANDLE hVM, DMAHANDLE hDMA, PBOOL pAutoTranslateReEnabled) ;
BOOL __stdcall INIT_VDMAD_Enable_Translation (VMHANDLE hVM, DMAHANDLE hDMA, PBOOL pAutoTranslateReEnabled) ;
BOOL __stdcall DEBUG_VDMAD_Enable_Translation (VMHANDLE hVM, DMAHANDLE hDMA, PBOOL pAutoTranslateReEnabled) ;
BOOL __stdcall STATIC_VDMAD_Enable_Translation (VMHANDLE hVM, DMAHANDLE hDMA, PBOOL pAutoTranslateReEnabled) ;
BOOL __stdcall PAGEABLE_VDMAD_Enable_Translation (VMHANDLE hVM, DMAHANDLE hDMA, PBOOL pAutoTranslateReEnabled) ;

DWORD __stdcall LOCK_VDMAD_Get_EISA_Adr_Mode (DWORD Channel, DMAHANDLE hDMA) ;
DWORD __stdcall INIT_VDMAD_Get_EISA_Adr_Mode (DWORD Channel, DMAHANDLE hDMA) ;
DWORD __stdcall DEBUG_VDMAD_Get_EISA_Adr_Mode (DWORD Channel, DMAHANDLE hDMA) ;
DWORD __stdcall STATIC_VDMAD_Get_EISA_Adr_Mode (DWORD Channel, DMAHANDLE hDMA) ;
DWORD __stdcall PAGEABLE_VDMAD_Get_EISA_Adr_Mode (DWORD Channel, DMAHANDLE hDMA) ;

VOID __stdcall LOCK_VDMAD_Get_Region_Info (DMAHANDLE hDMA, PDMABUFFERID pBuf, PBOOL pbLocked, PPVOID pLinearRegion, PDWORD pSize) ;
VOID __stdcall INIT_VDMAD_Get_Region_Info (DMAHANDLE hDMA, PDMABUFFERID pBuf, PBOOL pbLocked, PPVOID pLinearRegion, PDWORD pSize) ;
VOID __stdcall DEBUG_VDMAD_Get_Region_Info (DMAHANDLE hDMA, PDMABUFFERID pBuf, PBOOL pbLocked, PPVOID pLinearRegion, PDWORD pSize) ;
VOID __stdcall STATIC_VDMAD_Get_Region_Info (DMAHANDLE hDMA, PDMABUFFERID pBuf, PBOOL pbLocked, PPVOID pLinearRegion, PDWORD pSize) ;
VOID __stdcall PAGEABLE_VDMAD_Get_Region_Info (DMAHANDLE hDMA, PDMABUFFERID pBuf, PBOOL pbLocked, PPVOID pLinearRegion, PDWORD pSize) ;

DWORD __stdcall LOCK_VDMAD_Get_Version (void) ;
DWORD __stdcall INIT_VDMAD_Get_Version (void) ;
DWORD __stdcall DEBUG_VDMAD_Get_Version (void) ;
DWORD __stdcall STATIC_VDMAD_Get_Version (void) ;
DWORD __stdcall PAGEABLE_VDMAD_Get_Version (void) ;

PVOID __stdcall LOCK_VDMAD_Get_Virt_State (VMHANDLE hVM, DMAHANDLE hDMA, PDWORD pCount, PDWORD pMode) ;
PVOID __stdcall INIT_VDMAD_Get_Virt_State (VMHANDLE hVM, DMAHANDLE hDMA, PDWORD pCount, PDWORD pMode) ;
PVOID __stdcall DEBUG_VDMAD_Get_Virt_State (VMHANDLE hVM, DMAHANDLE hDMA, PDWORD pCount, PDWORD pMode) ;
PVOID __stdcall STATIC_VDMAD_Get_Virt_State (VMHANDLE hVM, DMAHANDLE hDMA, PDWORD pCount, PDWORD pMode) ;
PVOID __stdcall PAGEABLE_VDMAD_Get_Virt_State (VMHANDLE hVM, DMAHANDLE hDMA, PDWORD pCount, PDWORD pMode) ;

BOOL __stdcall LOCK_VDMAD_Lock_DMA_Region (CONST VOID * LinearAddress, DWORD nBytes, BYTE Alignment, PDWORD pLockCount, PDWORD pPhysAddress, PDWORD pError) ;
BOOL __stdcall INIT_VDMAD_Lock_DMA_Region (CONST VOID * LinearAddress, DWORD nBytes, BYTE Alignment, PDWORD pLockCount, PDWORD pPhysAddress, PDWORD pError) ;
BOOL __stdcall DEBUG_VDMAD_Lock_DMA_Region (CONST VOID * LinearAddress, DWORD nBytes, BYTE Alignment, PDWORD pLockCount, PDWORD pPhysAddress, PDWORD pError) ;
BOOL __stdcall STATIC_VDMAD_Lock_DMA_Region (CONST VOID * LinearAddress, DWORD nBytes, BYTE Alignment, PDWORD pLockCount, PDWORD pPhysAddress, PDWORD pError) ;
BOOL __stdcall PAGEABLE_VDMAD_Lock_DMA_Region (CONST VOID * LinearAddress, DWORD nBytes, BYTE Alignment, PDWORD pLockCount, PDWORD pPhysAddress, PDWORD pError) ;

VOID __stdcall LOCK_VDMAD_Mask_Channel (DMAHANDLE hDMA) ;
VOID __stdcall INIT_VDMAD_Mask_Channel (DMAHANDLE hDMA) ;
VOID __stdcall DEBUG_VDMAD_Mask_Channel (DMAHANDLE hDMA) ;
VOID __stdcall STATIC_VDMAD_Mask_Channel (DMAHANDLE hDMA) ;
VOID __stdcall PAGEABLE_VDMAD_Mask_Channel (DMAHANDLE hDMA) ;

BOOL __stdcall LOCK_VDMAD_Release_Buffer (DMABUFFERID BufID) ;
BOOL __stdcall INIT_VDMAD_Release_Buffer (DMABUFFERID BufID) ;
BOOL __stdcall DEBUG_VDMAD_Release_Buffer (DMABUFFERID BufID) ;
BOOL __stdcall STATIC_VDMAD_Release_Buffer (DMABUFFERID BufID) ;
BOOL __stdcall PAGEABLE_VDMAD_Release_Buffer (DMABUFFERID BufID) ;

BOOL __stdcall LOCK_VDMAD_Request_Buffer (CONST VOID * LinearAddress, DWORD nBytes, PDMABUFFERID pBufID, PPVOID pPhysicalAddress, PDWORD pError) ;
BOOL __stdcall INIT_VDMAD_Request_Buffer (CONST VOID * LinearAddress, DWORD nBytes, PDMABUFFERID pBufID, PPVOID pPhysicalAddress, PDWORD pError) ;
BOOL __stdcall DEBUG_VDMAD_Request_Buffer (CONST VOID * LinearAddress, DWORD nBytes, PDMABUFFERID pBufID, PPVOID pPhysicalAddress, PDWORD pError) ;
BOOL __stdcall STATIC_VDMAD_Request_Buffer (CONST VOID * LinearAddress, DWORD nBytes, PDMABUFFERID pBufID, PPVOID pPhysicalAddress, PDWORD pError) ;
BOOL __stdcall PAGEABLE_VDMAD_Request_Buffer (CONST VOID * LinearAddress, DWORD nBytes, PDMABUFFERID pBufID, PPVOID pPhysicalAddress, PDWORD pError) ;

VOID __stdcall LOCK_VDMAD_Reserve_Buffer_Space (DWORD nPages, CONST VOID * MaxPhysicalAddress) ;
VOID __stdcall INIT_VDMAD_Reserve_Buffer_Space (DWORD nPages, CONST VOID * MaxPhysicalAddress) ;
VOID __stdcall DEBUG_VDMAD_Reserve_Buffer_Space (DWORD nPages, CONST VOID * MaxPhysicalAddress) ;
VOID __stdcall STATIC_VDMAD_Reserve_Buffer_Space (DWORD nPages, CONST VOID * MaxPhysicalAddress) ;
VOID __stdcall PAGEABLE_VDMAD_Reserve_Buffer_Space (DWORD nPages, CONST VOID * MaxPhysicalAddress) ;

DWORD __stdcall LOCK_VDMAD_Scatter_Lock (VMHANDLE hVM, DWORD Flags, PVOID pDDS, PDWORD pTableEntriesNeeded, PDWORD pOffset) ;
DWORD __stdcall INIT_VDMAD_Scatter_Lock (VMHANDLE hVM, DWORD Flags, PVOID pDDS, PDWORD pTableEntriesNeeded, PDWORD pOffset) ;
DWORD __stdcall DEBUG_VDMAD_Scatter_Lock (VMHANDLE hVM, DWORD Flags, PVOID pDDS, PDWORD pTableEntriesNeeded, PDWORD pOffset) ;
DWORD __stdcall STATIC_VDMAD_Scatter_Lock (VMHANDLE hVM, DWORD Flags, PVOID pDDS, PDWORD pTableEntriesNeeded, PDWORD pOffset) ;
DWORD __stdcall PAGEABLE_VDMAD_Scatter_Lock (VMHANDLE hVM, DWORD Flags, PVOID pDDS, PDWORD pTableEntriesNeeded, PDWORD pOffset) ;

BOOL __stdcall LOCK_VDMAD_Scatter_Unlock (VMHANDLE hVM, DWORD Flags, PVOID pDDS) ;
BOOL __stdcall INIT_VDMAD_Scatter_Unlock (VMHANDLE hVM, DWORD Flags, PVOID pDDS) ;
BOOL __stdcall DEBUG_VDMAD_Scatter_Unlock (VMHANDLE hVM, DWORD Flags, PVOID pDDS) ;
BOOL __stdcall STATIC_VDMAD_Scatter_Unlock (VMHANDLE hVM, DWORD Flags, PVOID pDDS) ;
BOOL __stdcall PAGEABLE_VDMAD_Scatter_Unlock (VMHANDLE hVM, DWORD Flags, PVOID pDDS) ;

VOID __stdcall LOCK_VDMAD_Set_EISA_Adr_Mode (DWORD Channel, DMAHANDLE hDMA, DWORD Mode) ;
VOID __stdcall INIT_VDMAD_Set_EISA_Adr_Mode (DWORD Channel, DMAHANDLE hDMA, DWORD Mode) ;
VOID __stdcall DEBUG_VDMAD_Set_EISA_Adr_Mode (DWORD Channel, DMAHANDLE hDMA, DWORD Mode) ;
VOID __stdcall STATIC_VDMAD_Set_EISA_Adr_Mode (DWORD Channel, DMAHANDLE hDMA, DWORD Mode) ;
VOID __stdcall PAGEABLE_VDMAD_Set_EISA_Adr_Mode (DWORD Channel, DMAHANDLE hDMA, DWORD Mode) ;

VOID __stdcall LOCK_VDMAD_Set_Phys_State (DMAHANDLE hDMA, VMHANDLE hVM, DWORD Mode) ;
VOID __stdcall INIT_VDMAD_Set_Phys_State (DMAHANDLE hDMA, VMHANDLE hVM, DWORD Mode) ;
VOID __stdcall DEBUG_VDMAD_Set_Phys_State (DMAHANDLE hDMA, VMHANDLE hVM, DWORD Mode) ;
VOID __stdcall STATIC_VDMAD_Set_Phys_State (DMAHANDLE hDMA, VMHANDLE hVM, DWORD Mode) ;
VOID __stdcall PAGEABLE_VDMAD_Set_Phys_State (DMAHANDLE hDMA, VMHANDLE hVM, DWORD Mode) ;

VOID __stdcall LOCK_VDMAD_Set_Region_Info (DMAHANDLE hDMA, DMABUFFERID BufID, BOOL bLock, CONST VOID * LinearRegion, DWORD nBytes, PVOID PhysicalAddress) ;
VOID __stdcall INIT_VDMAD_Set_Region_Info (DMAHANDLE hDMA, DMABUFFERID BufID, BOOL bLock, CONST VOID * LinearRegion, DWORD nBytes, PVOID PhysicalAddress) ;
VOID __stdcall DEBUG_VDMAD_Set_Region_Info (DMAHANDLE hDMA, DMABUFFERID BufID, BOOL bLock, CONST VOID * LinearRegion, DWORD nBytes, PVOID PhysicalAddress) ;
VOID __stdcall STATIC_VDMAD_Set_Region_Info (DMAHANDLE hDMA, DMABUFFERID BufID, BOOL bLock, CONST VOID * LinearRegion, DWORD nBytes, PVOID PhysicalAddress) ;
VOID __stdcall PAGEABLE_VDMAD_Set_Region_Info (DMAHANDLE hDMA, DMABUFFERID BufID, BOOL bLock, CONST VOID * LinearRegion, DWORD nBytes, PVOID PhysicalAddress) ;

VOID __stdcall LOCK_VDMAD_Set_Virt_State (VMHANDLE hVM, DMAHANDLE hDMA, PVOID Address, DWORD nBytes, DWORD Mode) ;
VOID __stdcall INIT_VDMAD_Set_Virt_State (VMHANDLE hVM, DMAHANDLE hDMA, PVOID Address, DWORD nBytes, DWORD Mode) ;
VOID __stdcall DEBUG_VDMAD_Set_Virt_State (VMHANDLE hVM, DMAHANDLE hDMA, PVOID Address, DWORD nBytes, DWORD Mode) ;
VOID __stdcall STATIC_VDMAD_Set_Virt_State (VMHANDLE hVM, DMAHANDLE hDMA, PVOID Address, DWORD nBytes, DWORD Mode) ;
VOID __stdcall PAGEABLE_VDMAD_Set_Virt_State (VMHANDLE hVM, DMAHANDLE hDMA, PVOID Address, DWORD nBytes, DWORD Mode) ;

BOOL __stdcall LOCK_VDMAD_Unlock_DMA_Region (CONST VOID * LinearAddress, DWORD nBytes) ;
BOOL __stdcall INIT_VDMAD_Unlock_DMA_Region (CONST VOID * LinearAddress, DWORD nBytes) ;
BOOL __stdcall DEBUG_VDMAD_Unlock_DMA_Region (CONST VOID * LinearAddress, DWORD nBytes) ;
BOOL __stdcall STATIC_VDMAD_Unlock_DMA_Region (CONST VOID * LinearAddress, DWORD nBytes) ;
BOOL __stdcall PAGEABLE_VDMAD_Unlock_DMA_Region (CONST VOID * LinearAddress, DWORD nBytes) ;

VOID __stdcall LOCK_VDMAD_UnMask_Channel (VMHANDLE hVM, DMAHANDLE hDMA) ;
VOID __stdcall INIT_VDMAD_UnMask_Channel (VMHANDLE hVM, DMAHANDLE hDMA) ;
VOID __stdcall DEBUG_VDMAD_UnMask_Channel (VMHANDLE hVM, DMAHANDLE hDMA) ;
VOID __stdcall STATIC_VDMAD_UnMask_Channel (VMHANDLE hVM, DMAHANDLE hDMA) ;
VOID __stdcall PAGEABLE_VDMAD_UnMask_Channel (VMHANDLE hVM, DMAHANDLE hDMA) ;

DMAHANDLE __stdcall LOCK_VDMAD_Virtualize_Channel (DWORD Channel, PVDMADVirtualize_HANDLER Callback, PVDMADVirtualize_THUNK pThunk) ;
DMAHANDLE __stdcall INIT_VDMAD_Virtualize_Channel (DWORD Channel, PVDMADVirtualize_HANDLER Callback, PVDMADVirtualize_THUNK pThunk) ;
DMAHANDLE __stdcall DEBUG_VDMAD_Virtualize_Channel (DWORD Channel, PVDMADVirtualize_HANDLER Callback, PVDMADVirtualize_THUNK pThunk) ;
DMAHANDLE __stdcall STATIC_VDMAD_Virtualize_Channel (DWORD Channel, PVDMADVirtualize_HANDLER Callback, PVDMADVirtualize_THUNK pThunk) ;
DMAHANDLE __stdcall PAGEABLE_VDMAD_Virtualize_Channel (DWORD Channel, PVDMADVirtualize_HANDLER Callback, PVDMADVirtualize_THUNK pThunk) ;

#define VKD_Cancel_Hot_Key_State() VMMcall(VKD_Cancel_Hot_Key_State)
VOID __stdcall LOCK_VKD_Cancel_Paste (VMHANDLE hVM) ;
VOID __stdcall INIT_VKD_Cancel_Paste (VMHANDLE hVM) ;
VOID __stdcall DEBUG_VKD_Cancel_Paste (VMHANDLE hVM) ;
VOID __stdcall STATIC_VKD_Cancel_Paste (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VKD_Cancel_Paste (VMHANDLE hVM) ;

HOTKEYHANDLE __stdcall LOCK_VKD_Define_Hot_Key (BYTE ScanCode, BYTE ScanType, DWORD theShiftState, DWORD Flags, PVKDHotkey_HANDLER Callback, CONST VOID * Refdata, DWORD MaxDelay, PVKDHotkey_THUNK pThunk) ;
HOTKEYHANDLE __stdcall INIT_VKD_Define_Hot_Key (BYTE ScanCode, BYTE ScanType, DWORD theShiftState, DWORD Flags, PVKDHotkey_HANDLER Callback, CONST VOID * Refdata, DWORD MaxDelay, PVKDHotkey_THUNK pThunk) ;
HOTKEYHANDLE __stdcall DEBUG_VKD_Define_Hot_Key (BYTE ScanCode, BYTE ScanType, DWORD theShiftState, DWORD Flags, PVKDHotkey_HANDLER Callback, CONST VOID * Refdata, DWORD MaxDelay, PVKDHotkey_THUNK pThunk) ;
HOTKEYHANDLE __stdcall STATIC_VKD_Define_Hot_Key (BYTE ScanCode, BYTE ScanType, DWORD theShiftState, DWORD Flags, PVKDHotkey_HANDLER Callback, CONST VOID * Refdata, DWORD MaxDelay, PVKDHotkey_THUNK pThunk) ;
HOTKEYHANDLE __stdcall PAGEABLE_VKD_Define_Hot_Key (BYTE ScanCode, BYTE ScanType, DWORD theShiftState, DWORD Flags, PVKDHotkey_HANDLER Callback, CONST VOID * Refdata, DWORD MaxDelay, PVKDHotkey_THUNK pThunk) ;

VOID __stdcall LOCK_VKD_Define_Paste_Mode (BOOL bForceInt9, VMHANDLE hVM) ;
VOID __stdcall INIT_VKD_Define_Paste_Mode (BOOL bForceInt9, VMHANDLE hVM) ;
VOID __stdcall DEBUG_VKD_Define_Paste_Mode (BOOL bForceInt9, VMHANDLE hVM) ;
VOID __stdcall STATIC_VKD_Define_Paste_Mode (BOOL bForceInt9, VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VKD_Define_Paste_Mode (BOOL bForceInt9, VMHANDLE hVM) ;

VOID __stdcall LOCK_VKD_Flush_Msg_Key_Queue (VMHANDLE hVM) ;
VOID __stdcall INIT_VKD_Flush_Msg_Key_Queue (VMHANDLE hVM) ;
VOID __stdcall DEBUG_VKD_Flush_Msg_Key_Queue (VMHANDLE hVM) ;
VOID __stdcall STATIC_VKD_Flush_Msg_Key_Queue (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VKD_Flush_Msg_Key_Queue (VMHANDLE hVM) ;

DWORD __stdcall LOCK_VKD_Force_Keys (CONST VOID * pScanCodeBuffer, DWORD nCodes) ;
DWORD __stdcall INIT_VKD_Force_Keys (CONST VOID * pScanCodeBuffer, DWORD nCodes) ;
DWORD __stdcall DEBUG_VKD_Force_Keys (CONST VOID * pScanCodeBuffer, DWORD nCodes) ;
DWORD __stdcall STATIC_VKD_Force_Keys (CONST VOID * pScanCodeBuffer, DWORD nCodes) ;
DWORD __stdcall PAGEABLE_VKD_Force_Keys (CONST VOID * pScanCodeBuffer, DWORD nCodes) ;

VMHANDLE __stdcall LOCK_VKD_Get_Kbd_Owner (VOID) ;
VMHANDLE __stdcall INIT_VKD_Get_Kbd_Owner (VOID) ;
VMHANDLE __stdcall DEBUG_VKD_Get_Kbd_Owner (VOID) ;
VMHANDLE __stdcall STATIC_VKD_Get_Kbd_Owner (VOID) ;
VMHANDLE __stdcall PAGEABLE_VKD_Get_Kbd_Owner (VOID) ;

BOOL __stdcall LOCK_VKD_Get_Msg_Key (VMHANDLE hVM, PBYTE pScanCode, PBYTE pState) ;
BOOL __stdcall INIT_VKD_Get_Msg_Key (VMHANDLE hVM, PBYTE pScanCode, PBYTE pState) ;
BOOL __stdcall DEBUG_VKD_Get_Msg_Key (VMHANDLE hVM, PBYTE pScanCode, PBYTE pState) ;
BOOL __stdcall STATIC_VKD_Get_Msg_Key (VMHANDLE hVM, PBYTE pScanCode, PBYTE pState) ;
BOOL __stdcall PAGEABLE_VKD_Get_Msg_Key (VMHANDLE hVM, PBYTE pScanCode, PBYTE pState) ;

DWORD __stdcall LOCK_VKD_Get_Version (void) ;
DWORD __stdcall INIT_VKD_Get_Version (void) ;
DWORD __stdcall DEBUG_VKD_Get_Version (void) ;
DWORD __stdcall STATIC_VKD_Get_Version (void) ;
DWORD __stdcall PAGEABLE_VKD_Get_Version (void) ;

VOID __stdcall LOCK_VKD_Local_Disable_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey) ;
VOID __stdcall INIT_VKD_Local_Disable_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey) ;
VOID __stdcall DEBUG_VKD_Local_Disable_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey) ;
VOID __stdcall STATIC_VKD_Local_Disable_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey) ;
VOID __stdcall PAGEABLE_VKD_Local_Disable_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey) ;

VOID __stdcall LOCK_VKD_Local_Enable_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey) ;
VOID __stdcall INIT_VKD_Local_Enable_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey) ;
VOID __stdcall DEBUG_VKD_Local_Enable_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey) ;
VOID __stdcall STATIC_VKD_Local_Enable_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey) ;
VOID __stdcall PAGEABLE_VKD_Local_Enable_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey) ;

BOOL __stdcall LOCK_VKD_Peek_Msg_Key (VMHANDLE hVM, PBYTE pScanCode, PBYTE pState) ;
BOOL __stdcall INIT_VKD_Peek_Msg_Key (VMHANDLE hVM, PBYTE pScanCode, PBYTE pState) ;
BOOL __stdcall DEBUG_VKD_Peek_Msg_Key (VMHANDLE hVM, PBYTE pScanCode, PBYTE pState) ;
BOOL __stdcall STATIC_VKD_Peek_Msg_Key (VMHANDLE hVM, PBYTE pScanCode, PBYTE pState) ;
BOOL __stdcall PAGEABLE_VKD_Peek_Msg_Key (VMHANDLE hVM, PBYTE pScanCode, PBYTE pState) ;

VOID __stdcall LOCK_VKD_Reflect_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey, WORD State) ;
VOID __stdcall INIT_VKD_Reflect_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey, WORD State) ;
VOID __stdcall DEBUG_VKD_Reflect_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey, WORD State) ;
VOID __stdcall STATIC_VKD_Reflect_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey, WORD State) ;
VOID __stdcall PAGEABLE_VKD_Reflect_Hot_Key (VMHANDLE hVM, HOTKEYHANDLE hHotkey, WORD State) ;

VOID __stdcall LOCK_VKD_Remove_Hot_Key (HOTKEYHANDLE hHotkey) ;
VOID __stdcall INIT_VKD_Remove_Hot_Key (HOTKEYHANDLE hHotkey) ;
VOID __stdcall DEBUG_VKD_Remove_Hot_Key (HOTKEYHANDLE hHotkey) ;
VOID __stdcall STATIC_VKD_Remove_Hot_Key (HOTKEYHANDLE hHotkey) ;
VOID __stdcall PAGEABLE_VKD_Remove_Hot_Key (HOTKEYHANDLE hHotkey) ;

BOOL __stdcall LOCK_VKD_Start_Paste (VMHANDLE hVM, PVOID pPasteBuffer, DWORD nEntries, PVOID Refdata, PVKDStartPaste_HANDLER Callback, PVKDStartPaste_THUNK pThunk) ;
BOOL __stdcall INIT_VKD_Start_Paste (VMHANDLE hVM, PVOID pPasteBuffer, DWORD nEntries, PVOID Refdata, PVKDStartPaste_HANDLER Callback, PVKDStartPaste_THUNK pThunk) ;
BOOL __stdcall DEBUG_VKD_Start_Paste (VMHANDLE hVM, PVOID pPasteBuffer, DWORD nEntries, PVOID Refdata, PVKDStartPaste_HANDLER Callback, PVKDStartPaste_THUNK pThunk) ;
BOOL __stdcall STATIC_VKD_Start_Paste (VMHANDLE hVM, PVOID pPasteBuffer, DWORD nEntries, PVOID Refdata, PVKDStartPaste_HANDLER Callback, PVKDStartPaste_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_VKD_Start_Paste (VMHANDLE hVM, PVOID pPasteBuffer, DWORD nEntries, PVOID Refdata, PVKDStartPaste_HANDLER Callback, PVKDStartPaste_THUNK pThunk) ;

DWORD __stdcall LOCK_VMCPD_Get_Version (PDWORD mcpType) ;
DWORD __stdcall INIT_VMCPD_Get_Version (PDWORD mcpType) ;
DWORD __stdcall DEBUG_VMCPD_Get_Version (PDWORD mcpType) ;
DWORD __stdcall STATIC_VMCPD_Get_Version (PDWORD mcpType) ;
DWORD __stdcall PAGEABLE_VMCPD_Get_Version (PDWORD mcpType) ;

DWORD __stdcall LOCK_VMCPD_Get_Virt_State (VMHANDLE hVM) ;
DWORD __stdcall INIT_VMCPD_Get_Virt_State (VMHANDLE hVM) ;
DWORD __stdcall DEBUG_VMCPD_Get_Virt_State (VMHANDLE hVM) ;
DWORD __stdcall STATIC_VMCPD_Get_Virt_State (VMHANDLE hVM) ;
DWORD __stdcall PAGEABLE_VMCPD_Get_Virt_State (VMHANDLE hVM) ;

VOID __stdcall LOCK_VMCPD_Set_Virt_State (VMHANDLE hVM, DWORD State) ;
VOID __stdcall INIT_VMCPD_Set_Virt_State (VMHANDLE hVM, DWORD State) ;
VOID __stdcall DEBUG_VMCPD_Set_Virt_State (VMHANDLE hVM, DWORD State) ;
VOID __stdcall STATIC_VMCPD_Set_Virt_State (VMHANDLE hVM, DWORD State) ;
VOID __stdcall PAGEABLE_VMCPD_Set_Virt_State (VMHANDLE hVM, DWORD State) ;

VOID __stdcall LOCK_VMPoll_Enable_Disable (BOOL bEnable, VMHANDLE hVM) ;
VOID __stdcall INIT_VMPoll_Enable_Disable (BOOL bEnable, VMHANDLE hVM) ;
VOID __stdcall DEBUG_VMPoll_Enable_Disable (BOOL bEnable, VMHANDLE hVM) ;
VOID __stdcall STATIC_VMPoll_Enable_Disable (BOOL bEnable, VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VMPoll_Enable_Disable (BOOL bEnable, VMHANDLE hVM) ;

DWORD __stdcall LOCK_VMPoll_Get_Version (void) ;
DWORD __stdcall INIT_VMPoll_Get_Version (void) ;
DWORD __stdcall DEBUG_VMPoll_Get_Version (void) ;
DWORD __stdcall STATIC_VMPoll_Get_Version (void) ;
DWORD __stdcall PAGEABLE_VMPoll_Get_Version (void) ;

VOID __stdcall LOCK_VMPoll_Reset_Detection (VMHANDLE hVM) ;
VOID __stdcall INIT_VMPoll_Reset_Detection (VMHANDLE hVM) ;
VOID __stdcall DEBUG_VMPoll_Reset_Detection (VMHANDLE hVM) ;
VOID __stdcall STATIC_VMPoll_Reset_Detection (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VMPoll_Reset_Detection (VMHANDLE hVM) ;

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

PVOID __stdcall LOCK_VPICD_Call_When_Hw_Int (PHWInt_HANDLER Callback, PHWInt_THUNK pThunk) ;
PVOID __stdcall INIT_VPICD_Call_When_Hw_Int (PHWInt_HANDLER Callback, PHWInt_THUNK pThunk) ;
PVOID __stdcall DEBUG_VPICD_Call_When_Hw_Int (PHWInt_HANDLER Callback, PHWInt_THUNK pThunk) ;
PVOID __stdcall STATIC_VPICD_Call_When_Hw_Int (PHWInt_HANDLER Callback, PHWInt_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_VPICD_Call_When_Hw_Int (PHWInt_HANDLER Callback, PHWInt_THUNK pThunk) ;

VOID __stdcall LOCK_VPICD_Clear_Int_Request (VMHANDLE hVM, IRQHANDLE hIRQ) ;
VOID __stdcall INIT_VPICD_Clear_Int_Request (VMHANDLE hVM, IRQHANDLE hIRQ) ;
VOID __stdcall DEBUG_VPICD_Clear_Int_Request (VMHANDLE hVM, IRQHANDLE hIRQ) ;
VOID __stdcall STATIC_VPICD_Clear_Int_Request (VMHANDLE hVM, IRQHANDLE hIRQ) ;
VOID __stdcall PAGEABLE_VPICD_Clear_Int_Request (VMHANDLE hVM, IRQHANDLE hIRQ) ;

DWORD __stdcall LOCK_VPICD_Convert_Handle_To_IRQ (IRQHANDLE hIRQ) ;
DWORD __stdcall INIT_VPICD_Convert_Handle_To_IRQ (IRQHANDLE hIRQ) ;
DWORD __stdcall DEBUG_VPICD_Convert_Handle_To_IRQ (IRQHANDLE hIRQ) ;
DWORD __stdcall STATIC_VPICD_Convert_Handle_To_IRQ (IRQHANDLE hIRQ) ;
DWORD __stdcall PAGEABLE_VPICD_Convert_Handle_To_IRQ (IRQHANDLE hIRQ) ;

DWORD __stdcall LOCK_VPICD_Convert_Int_To_IRQ (VMHANDLE hVM, DWORD VecNum) ;
DWORD __stdcall INIT_VPICD_Convert_Int_To_IRQ (VMHANDLE hVM, DWORD VecNum) ;
DWORD __stdcall DEBUG_VPICD_Convert_Int_To_IRQ (VMHANDLE hVM, DWORD VecNum) ;
DWORD __stdcall STATIC_VPICD_Convert_Int_To_IRQ (VMHANDLE hVM, DWORD VecNum) ;
DWORD __stdcall PAGEABLE_VPICD_Convert_Int_To_IRQ (VMHANDLE hVM, DWORD VecNum) ;

DWORD __stdcall LOCK_VPICD_Convert_IRQ_To_Int (VMHANDLE hVM, DWORD IRQNum) ;
DWORD __stdcall INIT_VPICD_Convert_IRQ_To_Int (VMHANDLE hVM, DWORD IRQNum) ;
DWORD __stdcall DEBUG_VPICD_Convert_IRQ_To_Int (VMHANDLE hVM, DWORD IRQNum) ;
DWORD __stdcall STATIC_VPICD_Convert_IRQ_To_Int (VMHANDLE hVM, DWORD IRQNum) ;
DWORD __stdcall PAGEABLE_VPICD_Convert_IRQ_To_Int (VMHANDLE hVM, DWORD IRQNum) ;

VOID __stdcall LOCK_VPICD_Force_Default_Behavior (IRQHANDLE hIRQ) ;
VOID __stdcall INIT_VPICD_Force_Default_Behavior (IRQHANDLE hIRQ) ;
VOID __stdcall DEBUG_VPICD_Force_Default_Behavior (IRQHANDLE hIRQ) ;
VOID __stdcall STATIC_VPICD_Force_Default_Behavior (IRQHANDLE hIRQ) ;
VOID __stdcall PAGEABLE_VPICD_Force_Default_Behavior (IRQHANDLE hIRQ) ;

BOOL __stdcall LOCK_VPICD_Force_Default_Owner (VMHANDLE hVM, IRQHANDLE hIRQ) ;
BOOL __stdcall INIT_VPICD_Force_Default_Owner (VMHANDLE hVM, IRQHANDLE hIRQ) ;
BOOL __stdcall DEBUG_VPICD_Force_Default_Owner (VMHANDLE hVM, IRQHANDLE hIRQ) ;
BOOL __stdcall STATIC_VPICD_Force_Default_Owner (VMHANDLE hVM, IRQHANDLE hIRQ) ;
BOOL __stdcall PAGEABLE_VPICD_Force_Default_Owner (VMHANDLE hVM, IRQHANDLE hIRQ) ;

DWORD __stdcall LOCK_VPICD_Get_Complete_Status (VMHANDLE hVM, IRQHANDLE hIRQ) ;
DWORD __stdcall INIT_VPICD_Get_Complete_Status (VMHANDLE hVM, IRQHANDLE hIRQ) ;
DWORD __stdcall DEBUG_VPICD_Get_Complete_Status (VMHANDLE hVM, IRQHANDLE hIRQ) ;
DWORD __stdcall STATIC_VPICD_Get_Complete_Status (VMHANDLE hVM, IRQHANDLE hIRQ) ;
DWORD __stdcall PAGEABLE_VPICD_Get_Complete_Status (VMHANDLE hVM, IRQHANDLE hIRQ) ;

BOOL __stdcall LOCK_VPICD_Get_IRQ_Complete_Status (DWORD IRQNum, PDWORD pStatus) ;
BOOL __stdcall INIT_VPICD_Get_IRQ_Complete_Status (DWORD IRQNum, PDWORD pStatus) ;
BOOL __stdcall DEBUG_VPICD_Get_IRQ_Complete_Status (DWORD IRQNum, PDWORD pStatus) ;
BOOL __stdcall STATIC_VPICD_Get_IRQ_Complete_Status (DWORD IRQNum, PDWORD pStatus) ;
BOOL __stdcall PAGEABLE_VPICD_Get_IRQ_Complete_Status (DWORD IRQNum, PDWORD pStatus) ;

DWORD __stdcall LOCK_VPICD_Get_Status (VMHANDLE hVM, IRQHANDLE hIRQ) ;
DWORD __stdcall INIT_VPICD_Get_Status (VMHANDLE hVM, IRQHANDLE hIRQ) ;
DWORD __stdcall DEBUG_VPICD_Get_Status (VMHANDLE hVM, IRQHANDLE hIRQ) ;
DWORD __stdcall STATIC_VPICD_Get_Status (VMHANDLE hVM, IRQHANDLE hIRQ) ;
DWORD __stdcall PAGEABLE_VPICD_Get_Status (VMHANDLE hVM, IRQHANDLE hIRQ) ;

DWORD __stdcall LOCK_VPICD_Get_Version (PDWORD pFlags, PDWORD pMaxIRQ) ;
DWORD __stdcall INIT_VPICD_Get_Version (PDWORD pFlags, PDWORD pMaxIRQ) ;
DWORD __stdcall DEBUG_VPICD_Get_Version (PDWORD pFlags, PDWORD pMaxIRQ) ;
DWORD __stdcall STATIC_VPICD_Get_Version (PDWORD pFlags, PDWORD pMaxIRQ) ;
DWORD __stdcall PAGEABLE_VPICD_Get_Version (PDWORD pFlags, PDWORD pMaxIRQ) ;

VOID __stdcall LOCK_VPICD_Phys_EOI (IRQHANDLE hIRQ) ;
VOID __stdcall INIT_VPICD_Phys_EOI (IRQHANDLE hIRQ) ;
VOID __stdcall DEBUG_VPICD_Phys_EOI (IRQHANDLE hIRQ) ;
VOID __stdcall STATIC_VPICD_Phys_EOI (IRQHANDLE hIRQ) ;
VOID __stdcall PAGEABLE_VPICD_Phys_EOI (IRQHANDLE hIRQ) ;

VOID __stdcall LOCK_VPICD_Physically_Mask (IRQHANDLE hIRQ) ;
VOID __stdcall INIT_VPICD_Physically_Mask (IRQHANDLE hIRQ) ;
VOID __stdcall DEBUG_VPICD_Physically_Mask (IRQHANDLE hIRQ) ;
VOID __stdcall STATIC_VPICD_Physically_Mask (IRQHANDLE hIRQ) ;
VOID __stdcall PAGEABLE_VPICD_Physically_Mask (IRQHANDLE hIRQ) ;

VOID __stdcall LOCK_VPICD_Physically_Unmask (IRQHANDLE hIRQ) ;
VOID __stdcall INIT_VPICD_Physically_Unmask (IRQHANDLE hIRQ) ;
VOID __stdcall DEBUG_VPICD_Physically_Unmask (IRQHANDLE hIRQ) ;
VOID __stdcall STATIC_VPICD_Physically_Unmask (IRQHANDLE hIRQ) ;
VOID __stdcall PAGEABLE_VPICD_Physically_Unmask (IRQHANDLE hIRQ) ;

VOID __stdcall LOCK_VPICD_Set_Auto_Masking (IRQHANDLE hIRQ) ;
VOID __stdcall INIT_VPICD_Set_Auto_Masking (IRQHANDLE hIRQ) ;
VOID __stdcall DEBUG_VPICD_Set_Auto_Masking (IRQHANDLE hIRQ) ;
VOID __stdcall STATIC_VPICD_Set_Auto_Masking (IRQHANDLE hIRQ) ;
VOID __stdcall PAGEABLE_VPICD_Set_Auto_Masking (IRQHANDLE hIRQ) ;

VOID __stdcall LOCK_VPICD_Set_Int_Request (VMHANDLE hVM, IRQHANDLE hIRQ) ;
VOID __stdcall INIT_VPICD_Set_Int_Request (VMHANDLE hVM, IRQHANDLE hIRQ) ;
VOID __stdcall DEBUG_VPICD_Set_Int_Request (VMHANDLE hVM, IRQHANDLE hIRQ) ;
VOID __stdcall STATIC_VPICD_Set_Int_Request (VMHANDLE hVM, IRQHANDLE hIRQ) ;
VOID __stdcall PAGEABLE_VPICD_Set_Int_Request (VMHANDLE hVM, IRQHANDLE hIRQ) ;

BOOL __stdcall LOCK_VPICD_Test_Phys_Request (IRQHANDLE hIRQ) ;
BOOL __stdcall INIT_VPICD_Test_Phys_Request (IRQHANDLE hIRQ) ;
BOOL __stdcall DEBUG_VPICD_Test_Phys_Request (IRQHANDLE hIRQ) ;
BOOL __stdcall STATIC_VPICD_Test_Phys_Request (IRQHANDLE hIRQ) ;
BOOL __stdcall PAGEABLE_VPICD_Test_Phys_Request (IRQHANDLE hIRQ) ;

IRQHANDLE __stdcall LOCK_VPICD_Virtualize_IRQ (CONST VID * pVID) ;
IRQHANDLE __stdcall INIT_VPICD_Virtualize_IRQ (CONST VID * pVID) ;
IRQHANDLE __stdcall DEBUG_VPICD_Virtualize_IRQ (CONST VID * pVID) ;
IRQHANDLE __stdcall STATIC_VPICD_Virtualize_IRQ (CONST VID * pVID) ;
IRQHANDLE __stdcall PAGEABLE_VPICD_Virtualize_IRQ (CONST VID * pVID) ;

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VPICD_Auto_Mask_At_Inst_Swap (IRQHANDLE hirq) ;
BOOL __stdcall INIT_VPICD_Auto_Mask_At_Inst_Swap (IRQHANDLE hirq) ;
BOOL __stdcall DEBUG_VPICD_Auto_Mask_At_Inst_Swap (IRQHANDLE hirq) ;
BOOL __stdcall STATIC_VPICD_Auto_Mask_At_Inst_Swap (IRQHANDLE hirq) ;
BOOL __stdcall PAGEABLE_VPICD_Auto_Mask_At_Inst_Swap (IRQHANDLE hirq) ;
#endif

#define VPICD_Begin_Inst_Page_Swap() VMMcall(VPICD_Begin_Inst_Page_Swap)
#define VPICD_End_Inst_Page_Swap() VMMcall(VPICD_End_Inst_Page_Swap)
#define VSD_Bell() VMMcall(VSD_Bell)
DWORD __cdecl LOCK_VSD_Get_Version (void) ;
DWORD __cdecl INIT_VSD_Get_Version (void) ;
DWORD __cdecl DEBUG_VSD_Get_Version (void) ;
DWORD __cdecl STATIC_VSD_Get_Version (void) ;
DWORD __cdecl PAGEABLE_VSD_Get_Version (void) ;

BOOL __stdcall LOCK_VTD_Begin_Min_Int_Period (DWORD Period) ;
BOOL __stdcall INIT_VTD_Begin_Min_Int_Period (DWORD Period) ;
BOOL __stdcall DEBUG_VTD_Begin_Min_Int_Period (DWORD Period) ;
BOOL __stdcall STATIC_VTD_Begin_Min_Int_Period (DWORD Period) ;
BOOL __stdcall PAGEABLE_VTD_Begin_Min_Int_Period (DWORD Period) ;

VOID __stdcall LOCK_VTD_Disable_Trapping (VMHANDLE hVM) ;
VOID __stdcall INIT_VTD_Disable_Trapping (VMHANDLE hVM) ;
VOID __stdcall DEBUG_VTD_Disable_Trapping (VMHANDLE hVM) ;
VOID __stdcall STATIC_VTD_Disable_Trapping (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VTD_Disable_Trapping (VMHANDLE hVM) ;

VOID __stdcall LOCK_VTD_Enable_Trapping (VMHANDLE hVM) ;
VOID __stdcall INIT_VTD_Enable_Trapping (VMHANDLE hVM) ;
VOID __stdcall DEBUG_VTD_Enable_Trapping (VMHANDLE hVM) ;
VOID __stdcall STATIC_VTD_Enable_Trapping (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VTD_Enable_Trapping (VMHANDLE hVM) ;

BOOL __stdcall LOCK_VTD_End_Min_Int_Period (DWORD Period) ;
BOOL __stdcall INIT_VTD_End_Min_Int_Period (DWORD Period) ;
BOOL __stdcall DEBUG_VTD_End_Min_Int_Period (DWORD Period) ;
BOOL __stdcall STATIC_VTD_End_Min_Int_Period (DWORD Period) ;
BOOL __stdcall PAGEABLE_VTD_End_Min_Int_Period (DWORD Period) ;

DWORD __stdcall LOCK_VTD_Get_Interrupt_Period (void) ;
DWORD __stdcall INIT_VTD_Get_Interrupt_Period (void) ;
DWORD __stdcall DEBUG_VTD_Get_Interrupt_Period (void) ;
DWORD __stdcall STATIC_VTD_Get_Interrupt_Period (void) ;
DWORD __stdcall PAGEABLE_VTD_Get_Interrupt_Period (void) ;

VOID __stdcall LOCK_VTD_Get_Real_Time (PDWORD pHiWord, PDWORD pLoWord) ;
VOID __stdcall INIT_VTD_Get_Real_Time (PDWORD pHiWord, PDWORD pLoWord) ;
VOID __stdcall DEBUG_VTD_Get_Real_Time (PDWORD pHiWord, PDWORD pLoWord) ;
VOID __stdcall STATIC_VTD_Get_Real_Time (PDWORD pHiWord, PDWORD pLoWord) ;
VOID __stdcall PAGEABLE_VTD_Get_Real_Time (PDWORD pHiWord, PDWORD pLoWord) ;

DWORD __stdcall LOCK_VTD_Get_Version (PDWORD pFastest, PDWORD pSlowest) ;
DWORD __stdcall INIT_VTD_Get_Version (PDWORD pFastest, PDWORD pSlowest) ;
DWORD __stdcall DEBUG_VTD_Get_Version (PDWORD pFastest, PDWORD pSlowest) ;
DWORD __stdcall STATIC_VTD_Get_Version (PDWORD pFastest, PDWORD pSlowest) ;
DWORD __stdcall PAGEABLE_VTD_Get_Version (PDWORD pFastest, PDWORD pSlowest) ;

#define VTD_Update_System_Clock() VMMcall(VTD_Update_System_Clock)
PVOID __stdcall LOCK_VPICD_Thunk_HWInt (PVPICD_HWInt_HANDLER Callback, PVPICD_HWInt_THUNK pThunk) ;
PVOID __stdcall INIT_VPICD_Thunk_HWInt (PVPICD_HWInt_HANDLER Callback, PVPICD_HWInt_THUNK pThunk) ;
PVOID __stdcall DEBUG_VPICD_Thunk_HWInt (PVPICD_HWInt_HANDLER Callback, PVPICD_HWInt_THUNK pThunk) ;
PVOID __stdcall STATIC_VPICD_Thunk_HWInt (PVPICD_HWInt_HANDLER Callback, PVPICD_HWInt_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_VPICD_Thunk_HWInt (PVPICD_HWInt_HANDLER Callback, PVPICD_HWInt_THUNK pThunk) ;

PVOID __stdcall LOCK_VPICD_Thunk_VirtInt (PVPICD_VirtInt_HANDLER Callback, PVPICD_VirtInt_THUNK pThunk) ;
PVOID __stdcall INIT_VPICD_Thunk_VirtInt (PVPICD_VirtInt_HANDLER Callback, PVPICD_VirtInt_THUNK pThunk) ;
PVOID __stdcall DEBUG_VPICD_Thunk_VirtInt (PVPICD_VirtInt_HANDLER Callback, PVPICD_VirtInt_THUNK pThunk) ;
PVOID __stdcall STATIC_VPICD_Thunk_VirtInt (PVPICD_VirtInt_HANDLER Callback, PVPICD_VirtInt_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_VPICD_Thunk_VirtInt (PVPICD_VirtInt_HANDLER Callback, PVPICD_VirtInt_THUNK pThunk) ;

PVOID __stdcall LOCK_VPICD_Thunk_EOI (PVPICD_EOI_HANDLER Callback, PVPICD_EOI_THUNK pThunk) ;
PVOID __stdcall INIT_VPICD_Thunk_EOI (PVPICD_EOI_HANDLER Callback, PVPICD_EOI_THUNK pThunk) ;
PVOID __stdcall DEBUG_VPICD_Thunk_EOI (PVPICD_EOI_HANDLER Callback, PVPICD_EOI_THUNK pThunk) ;
PVOID __stdcall STATIC_VPICD_Thunk_EOI (PVPICD_EOI_HANDLER Callback, PVPICD_EOI_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_VPICD_Thunk_EOI (PVPICD_EOI_HANDLER Callback, PVPICD_EOI_THUNK pThunk) ;

PVOID __stdcall LOCK_VPICD_Thunk_MaskChange (PVPICD_MaskChange_HANDLER Callback, PVPICD_MaskChange_THUNK pThunk) ;
PVOID __stdcall INIT_VPICD_Thunk_MaskChange (PVPICD_MaskChange_HANDLER Callback, PVPICD_MaskChange_THUNK pThunk) ;
PVOID __stdcall DEBUG_VPICD_Thunk_MaskChange (PVPICD_MaskChange_HANDLER Callback, PVPICD_MaskChange_THUNK pThunk) ;
PVOID __stdcall STATIC_VPICD_Thunk_MaskChange (PVPICD_MaskChange_HANDLER Callback, PVPICD_MaskChange_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_VPICD_Thunk_MaskChange (PVPICD_MaskChange_HANDLER Callback, PVPICD_MaskChange_THUNK pThunk) ;

PVOID __stdcall LOCK_VPICD_Thunk_Iret (PVPICD_Iret_HANDLER Callback, PVPICD_Iret_THUNK pThunk) ;
PVOID __stdcall INIT_VPICD_Thunk_Iret (PVPICD_Iret_HANDLER Callback, PVPICD_Iret_THUNK pThunk) ;
PVOID __stdcall DEBUG_VPICD_Thunk_Iret (PVPICD_Iret_HANDLER Callback, PVPICD_Iret_THUNK pThunk) ;
PVOID __stdcall STATIC_VPICD_Thunk_Iret (PVPICD_Iret_HANDLER Callback, PVPICD_Iret_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_VPICD_Thunk_Iret (PVPICD_Iret_HANDLER Callback, PVPICD_Iret_THUNK pThunk) ;

BOOL __stdcall LOCK_Install_Exception_Handler (PEH_STRUC pEHStruct, PEXCEPTION_THUNK pThunk) ;
BOOL __stdcall INIT_Install_Exception_Handler (PEH_STRUC pEHStruct, PEXCEPTION_THUNK pThunk) ;
BOOL __stdcall DEBUG_Install_Exception_Handler (PEH_STRUC pEHStruct, PEXCEPTION_THUNK pThunk) ;
BOOL __stdcall STATIC_Install_Exception_Handler (PEH_STRUC pEHStruct, PEXCEPTION_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Install_Exception_Handler (PEH_STRUC pEHStruct, PEXCEPTION_THUNK pThunk) ;

#ifdef WFW311_OR_LATER
PDEVICEINFO __cdecl LOCK_VXDLDR_GetDeviceList (void) ;
PDEVICEINFO __cdecl INIT_VXDLDR_GetDeviceList (void) ;
PDEVICEINFO __cdecl DEBUG_VXDLDR_GetDeviceList (void) ;
PDEVICEINFO __cdecl STATIC_VXDLDR_GetDeviceList (void) ;
PDEVICEINFO __cdecl PAGEABLE_VXDLDR_GetDeviceList (void) ;
#endif

#ifdef WFW311_OR_LATER
WORD __cdecl LOCK_VXDLDR_GetVersion (void) ;
WORD __cdecl INIT_VXDLDR_GetVersion (void) ;
WORD __cdecl DEBUG_VXDLDR_GetVersion (void) ;
WORD __cdecl STATIC_VXDLDR_GetVersion (void) ;
WORD __cdecl PAGEABLE_VXDLDR_GetVersion (void) ;
#endif

#ifdef WFW311_OR_LATER
DWORD __stdcall LOCK_VXDLDR_LoadDevice (PCHAR szDeviceName, DWORD dwFlags, PDEVICEINFO * pHandle, PDDB * ppDDB) ;
DWORD __stdcall INIT_VXDLDR_LoadDevice (PCHAR szDeviceName, DWORD dwFlags, PDEVICEINFO * pHandle, PDDB * ppDDB) ;
DWORD __stdcall DEBUG_VXDLDR_LoadDevice (PCHAR szDeviceName, DWORD dwFlags, PDEVICEINFO * pHandle, PDDB * ppDDB) ;
DWORD __stdcall STATIC_VXDLDR_LoadDevice (PCHAR szDeviceName, DWORD dwFlags, PDEVICEINFO * pHandle, PDDB * ppDDB) ;
DWORD __stdcall PAGEABLE_VXDLDR_LoadDevice (PCHAR szDeviceName, DWORD dwFlags, PDEVICEINFO * pHandle, PDDB * ppDDB) ;
#endif

#ifdef WFW311_OR_LATER
DWORD __stdcall LOCK_VXDLDR_UnloadDevice (WORD deviceID, PCHAR szDevname) ;
DWORD __stdcall INIT_VXDLDR_UnloadDevice (WORD deviceID, PCHAR szDevname) ;
DWORD __stdcall DEBUG_VXDLDR_UnloadDevice (WORD deviceID, PCHAR szDevname) ;
DWORD __stdcall STATIC_VXDLDR_UnloadDevice (WORD deviceID, PCHAR szDevname) ;
DWORD __stdcall PAGEABLE_VXDLDR_UnloadDevice (WORD deviceID, PCHAR szDevname) ;
#endif

#ifdef WFW311_OR_LATER
DWORD __stdcall LOCK_VXDLDR_DevInitSucceeded (PDEVICEINFO devHandle) ;
DWORD __stdcall INIT_VXDLDR_DevInitSucceeded (PDEVICEINFO devHandle) ;
DWORD __stdcall DEBUG_VXDLDR_DevInitSucceeded (PDEVICEINFO devHandle) ;
DWORD __stdcall STATIC_VXDLDR_DevInitSucceeded (PDEVICEINFO devHandle) ;
DWORD __stdcall PAGEABLE_VXDLDR_DevInitSucceeded (PDEVICEINFO devHandle) ;
#endif

#ifdef WFW311_OR_LATER
DWORD __stdcall LOCK_VXDLDR_DevInitFailed (PDEVICEINFO devHandle) ;
DWORD __stdcall INIT_VXDLDR_DevInitFailed (PDEVICEINFO devHandle) ;
DWORD __stdcall DEBUG_VXDLDR_DevInitFailed (PDEVICEINFO devHandle) ;
DWORD __stdcall STATIC_VXDLDR_DevInitFailed (PDEVICEINFO devHandle) ;
DWORD __stdcall PAGEABLE_VXDLDR_DevInitFailed (PDEVICEINFO devHandle) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_Get_APM_BIOS_Version (void) ;
DWORD __cdecl INIT__VPOWERD_Get_APM_BIOS_Version (void) ;
DWORD __cdecl DEBUG__VPOWERD_Get_APM_BIOS_Version (void) ;
DWORD __cdecl STATIC__VPOWERD_Get_APM_BIOS_Version (void) ;
DWORD __cdecl PAGEABLE__VPOWERD_Get_APM_BIOS_Version (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_Get_Power_State (POWER_DEVICE_ID Power_Device_ID,LPPOWER_STATE lpPower_State) ;
DWORD __cdecl INIT__VPOWERD_Get_Power_State (POWER_DEVICE_ID Power_Device_ID,LPPOWER_STATE lpPower_State) ;
DWORD __cdecl DEBUG__VPOWERD_Get_Power_State (POWER_DEVICE_ID Power_Device_ID,LPPOWER_STATE lpPower_State) ;
DWORD __cdecl STATIC__VPOWERD_Get_Power_State (POWER_DEVICE_ID Power_Device_ID,LPPOWER_STATE lpPower_State) ;
DWORD __cdecl PAGEABLE__VPOWERD_Get_Power_State (POWER_DEVICE_ID Power_Device_ID,LPPOWER_STATE lpPower_State) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_Get_Power_Management_Level (void) ;
DWORD __cdecl INIT__VPOWERD_Get_Power_Management_Level (void) ;
DWORD __cdecl DEBUG__VPOWERD_Get_Power_Management_Level (void) ;
DWORD __cdecl STATIC__VPOWERD_Get_Power_Management_Level (void) ;
DWORD __cdecl PAGEABLE__VPOWERD_Get_Power_Management_Level (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_Get_Power_Status (POWER_DEVICE_ID Power_Device_ID, POWER_STATUS * pPowerStatus) ;
DWORD __cdecl INIT__VPOWERD_Get_Power_Status (POWER_DEVICE_ID Power_Device_ID, POWER_STATUS * pPowerStatus) ;
DWORD __cdecl DEBUG__VPOWERD_Get_Power_Status (POWER_DEVICE_ID Power_Device_ID, POWER_STATUS * pPowerStatus) ;
DWORD __cdecl STATIC__VPOWERD_Get_Power_Status (POWER_DEVICE_ID Power_Device_ID, POWER_STATUS * pPowerStatus) ;
DWORD __cdecl PAGEABLE__VPOWERD_Get_Power_Status (POWER_DEVICE_ID Power_Device_ID, POWER_STATUS * pPowerStatus) ;
#endif

#ifdef WIN40_OR_LATER
WORD __cdecl LOCK__VPOWERD_OEM_APM_Function (OEM_APM_REGS * pRegisters) ;
WORD __cdecl INIT__VPOWERD_OEM_APM_Function (OEM_APM_REGS * pRegisters) ;
WORD __cdecl DEBUG__VPOWERD_OEM_APM_Function (OEM_APM_REGS * pRegisters) ;
WORD __cdecl STATIC__VPOWERD_OEM_APM_Function (OEM_APM_REGS * pRegisters) ;
WORD __cdecl PAGEABLE__VPOWERD_OEM_APM_Function (OEM_APM_REGS * pRegisters) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_Register_Power_Handler (POWER_HANDLER pCallback, DWORD InitOrder, DWORD flags) ;
DWORD __cdecl INIT__VPOWERD_Register_Power_Handler (POWER_HANDLER pCallback, DWORD InitOrder, DWORD flags) ;
DWORD __cdecl DEBUG__VPOWERD_Register_Power_Handler (POWER_HANDLER pCallback, DWORD InitOrder, DWORD flags) ;
DWORD __cdecl STATIC__VPOWERD_Register_Power_Handler (POWER_HANDLER pCallback, DWORD InitOrder, DWORD flags) ;
DWORD __cdecl PAGEABLE__VPOWERD_Register_Power_Handler (POWER_HANDLER pCallback, DWORD InitOrder, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_Restore_Power_On_Defaults (void) ;
DWORD __cdecl INIT__VPOWERD_Restore_Power_On_Defaults (void) ;
DWORD __cdecl DEBUG__VPOWERD_Restore_Power_On_Defaults (void) ;
DWORD __cdecl STATIC__VPOWERD_Restore_Power_On_Defaults (void) ;
DWORD __cdecl PAGEABLE__VPOWERD_Restore_Power_On_Defaults (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_Set_Device_Power_State (DWORD apmID, DWORD PowerState) ;
DWORD __cdecl INIT__VPOWERD_Set_Device_Power_State (DWORD apmID, DWORD PowerState) ;
DWORD __cdecl DEBUG__VPOWERD_Set_Device_Power_State (DWORD apmID, DWORD PowerState) ;
DWORD __cdecl STATIC__VPOWERD_Set_Device_Power_State (DWORD apmID, DWORD PowerState) ;
DWORD __cdecl PAGEABLE__VPOWERD_Set_Device_Power_State (DWORD apmID, DWORD PowerState) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_Set_Power_Management_Level (DWORD level) ;
DWORD __cdecl INIT__VPOWERD_Set_Power_Management_Level (DWORD level) ;
DWORD __cdecl DEBUG__VPOWERD_Set_Power_Management_Level (DWORD level) ;
DWORD __cdecl STATIC__VPOWERD_Set_Power_Management_Level (DWORD level) ;
DWORD __cdecl PAGEABLE__VPOWERD_Set_Power_Management_Level (DWORD level) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_Set_System_Power_State (DWORD PowerState, DWORD RequestType) ;
DWORD __cdecl INIT__VPOWERD_Set_System_Power_State (DWORD PowerState, DWORD RequestType) ;
DWORD __cdecl DEBUG__VPOWERD_Set_System_Power_State (DWORD PowerState, DWORD RequestType) ;
DWORD __cdecl STATIC__VPOWERD_Set_System_Power_State (DWORD PowerState, DWORD RequestType) ;
DWORD __cdecl PAGEABLE__VPOWERD_Set_System_Power_State (DWORD PowerState, DWORD RequestType) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_Deregister_Power_Handler (POWER_HANDLER pCallback, DWORD flags) ;
DWORD __cdecl INIT__VPOWERD_Deregister_Power_Handler (POWER_HANDLER pCallback, DWORD flags) ;
DWORD __cdecl DEBUG__VPOWERD_Deregister_Power_Handler (POWER_HANDLER pCallback, DWORD flags) ;
DWORD __cdecl STATIC__VPOWERD_Deregister_Power_Handler (POWER_HANDLER pCallback, DWORD flags) ;
DWORD __cdecl PAGEABLE__VPOWERD_Deregister_Power_Handler (POWER_HANDLER pCallback, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
WORD __cdecl LOCK__VPOWERD_Get_Version (void) ;
WORD __cdecl INIT__VPOWERD_Get_Version (void) ;
WORD __cdecl DEBUG__VPOWERD_Get_Version (void) ;
WORD __cdecl STATIC__VPOWERD_Get_Version (void) ;
WORD __cdecl PAGEABLE__VPOWERD_Get_Version (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_W32_Get_System_Power_Status (WIN32_SYSTEM_POWER_STATUS * pStatus) ;
DWORD __cdecl INIT__VPOWERD_W32_Get_System_Power_Status (WIN32_SYSTEM_POWER_STATUS * pStatus) ;
DWORD __cdecl DEBUG__VPOWERD_W32_Get_System_Power_Status (WIN32_SYSTEM_POWER_STATUS * pStatus) ;
DWORD __cdecl STATIC__VPOWERD_W32_Get_System_Power_Status (WIN32_SYSTEM_POWER_STATUS * pStatus) ;
DWORD __cdecl PAGEABLE__VPOWERD_W32_Get_System_Power_Status (WIN32_SYSTEM_POWER_STATUS * pStatus) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VPOWERD_W32_Set_System_Power_State (BOOL Suspend_Flag, BOOL Force_Flag) ;
DWORD __cdecl INIT__VPOWERD_W32_Set_System_Power_State (BOOL Suspend_Flag, BOOL Force_Flag) ;
DWORD __cdecl DEBUG__VPOWERD_W32_Set_System_Power_State (BOOL Suspend_Flag, BOOL Force_Flag) ;
DWORD __cdecl STATIC__VPOWERD_W32_Set_System_Power_State (BOOL Suspend_Flag, BOOL Force_Flag) ;
DWORD __cdecl PAGEABLE__VPOWERD_W32_Set_System_Power_State (BOOL Suspend_Flag, BOOL Force_Flag) ;
#endif

#ifdef WFW311_OR_LATER
DWORD __cdecl LOCK_VCOMM_Get_Version (void) ;
DWORD __cdecl INIT_VCOMM_Get_Version (void) ;
DWORD __cdecl DEBUG_VCOMM_Get_Version (void) ;
DWORD __cdecl STATIC_VCOMM_Get_Version (void) ;
DWORD __cdecl PAGEABLE_VCOMM_Get_Version (void) ;
#endif

#ifdef WFW311_OR_LATER
HANDLE __cdecl LOCK__VCOMM_Acquire_Port (PPORTDATA handle, DWORD portNumber, VMHANDLE hVM, DWORD flags, PCHAR szVxDName) ;
HANDLE __cdecl INIT__VCOMM_Acquire_Port (PPORTDATA handle, DWORD portNumber, VMHANDLE hVM, DWORD flags, PCHAR szVxDName) ;
HANDLE __cdecl DEBUG__VCOMM_Acquire_Port (PPORTDATA handle, DWORD portNumber, VMHANDLE hVM, DWORD flags, PCHAR szVxDName) ;
HANDLE __cdecl STATIC__VCOMM_Acquire_Port (PPORTDATA handle, DWORD portNumber, VMHANDLE hVM, DWORD flags, PCHAR szVxDName) ;
HANDLE __cdecl PAGEABLE__VCOMM_Acquire_Port (PPORTDATA handle, DWORD portNumber, VMHANDLE hVM, DWORD flags, PCHAR szVxDName) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_Add_Port (PVOID RefData, PCOMMPORTOPENFUNC pPortOpen, PCHAR szPortName) ;
BOOL __cdecl INIT__VCOMM_Add_Port (PVOID RefData, PCOMMPORTOPENFUNC pPortOpen, PCHAR szPortName) ;
BOOL __cdecl DEBUG__VCOMM_Add_Port (PVOID RefData, PCOMMPORTOPENFUNC pPortOpen, PCHAR szPortName) ;
BOOL __cdecl STATIC__VCOMM_Add_Port (PVOID RefData, PCOMMPORTOPENFUNC pPortOpen, PCHAR szPortName) ;
BOOL __cdecl PAGEABLE__VCOMM_Add_Port (PVOID RefData, PCOMMPORTOPENFUNC pPortOpen, PCHAR szPortName) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_Register_Port_Driver (PDRIVERCTRLFUNC pDC) ;
BOOL __cdecl INIT__VCOMM_Register_Port_Driver (PDRIVERCTRLFUNC pDC) ;
BOOL __cdecl DEBUG__VCOMM_Register_Port_Driver (PDRIVERCTRLFUNC pDC) ;
BOOL __cdecl STATIC__VCOMM_Register_Port_Driver (PDRIVERCTRLFUNC pDC) ;
BOOL __cdecl PAGEABLE__VCOMM_Register_Port_Driver (PDRIVERCTRLFUNC pDC) ;
#endif

#ifdef WFW311_OR_LATER
VOID __cdecl LOCK__VCOMM_Release_Port (COMMPORTHANDLE hPort, VMHANDLE hOwnerVM) ;
VOID __cdecl INIT__VCOMM_Release_Port (COMMPORTHANDLE hPort, VMHANDLE hOwnerVM) ;
VOID __cdecl DEBUG__VCOMM_Release_Port (COMMPORTHANDLE hPort, VMHANDLE hOwnerVM) ;
VOID __cdecl STATIC__VCOMM_Release_Port (COMMPORTHANDLE hPort, VMHANDLE hOwnerVM) ;
VOID __cdecl PAGEABLE__VCOMM_Release_Port (COMMPORTHANDLE hPort, VMHANDLE hOwnerVM) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_ClearCommError (COMMPORTHANDLE hPort, _COMSTAT * pcmst, PDWORD pErrors) ;
BOOL __cdecl INIT__VCOMM_ClearCommError (COMMPORTHANDLE hPort, _COMSTAT * pcmst, PDWORD pErrors) ;
BOOL __cdecl DEBUG__VCOMM_ClearCommError (COMMPORTHANDLE hPort, _COMSTAT * pcmst, PDWORD pErrors) ;
BOOL __cdecl STATIC__VCOMM_ClearCommError (COMMPORTHANDLE hPort, _COMSTAT * pcmst, PDWORD pErrors) ;
BOOL __cdecl PAGEABLE__VCOMM_ClearCommError (COMMPORTHANDLE hPort, _COMSTAT * pcmst, PDWORD pErrors) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_CloseComm (COMMPORTHANDLE hPort) ;
BOOL __cdecl INIT__VCOMM_CloseComm (COMMPORTHANDLE hPort) ;
BOOL __cdecl DEBUG__VCOMM_CloseComm (COMMPORTHANDLE hPort) ;
BOOL __cdecl STATIC__VCOMM_CloseComm (COMMPORTHANDLE hPort) ;
BOOL __cdecl PAGEABLE__VCOMM_CloseComm (COMMPORTHANDLE hPort) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_EnableCommNotification (COMMPORTHANDLE hPort,PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl INIT__VCOMM_EnableCommNotification (COMMPORTHANDLE hPort,PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl DEBUG__VCOMM_EnableCommNotification (COMMPORTHANDLE hPort,PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl STATIC__VCOMM_EnableCommNotification (COMMPORTHANDLE hPort,PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl PAGEABLE__VCOMM_EnableCommNotification (COMMPORTHANDLE hPort,PCOMMNOTIFYPROC pProc, PVOID refData) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_EscapeCommFunction (COMMPORTHANDLE hPort, DWORD func, DWORD inData, PVOID outData) ;
BOOL __cdecl INIT__VCOMM_EscapeCommFunction (COMMPORTHANDLE hPort, DWORD func, DWORD inData, PVOID outData) ;
BOOL __cdecl DEBUG__VCOMM_EscapeCommFunction (COMMPORTHANDLE hPort, DWORD func, DWORD inData, PVOID outData) ;
BOOL __cdecl STATIC__VCOMM_EscapeCommFunction (COMMPORTHANDLE hPort, DWORD func, DWORD inData, PVOID outData) ;
BOOL __cdecl PAGEABLE__VCOMM_EscapeCommFunction (COMMPORTHANDLE hPort, DWORD func, DWORD inData, PVOID outData) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_GetCommEventMask (COMMPORTHANDLE hPort, DWORD dwMask, PDWORD events) ;
BOOL __cdecl INIT__VCOMM_GetCommEventMask (COMMPORTHANDLE hPort, DWORD dwMask, PDWORD events) ;
BOOL __cdecl DEBUG__VCOMM_GetCommEventMask (COMMPORTHANDLE hPort, DWORD dwMask, PDWORD events) ;
BOOL __cdecl STATIC__VCOMM_GetCommEventMask (COMMPORTHANDLE hPort, DWORD dwMask, PDWORD events) ;
BOOL __cdecl PAGEABLE__VCOMM_GetCommEventMask (COMMPORTHANDLE hPort, DWORD dwMask, PDWORD events) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_GetCommProperties (COMMPORTHANDLE hPort, _COMMPROP * pcmmprop) ;
BOOL __cdecl INIT__VCOMM_GetCommProperties (COMMPORTHANDLE hPort, _COMMPROP * pcmmprop) ;
BOOL __cdecl DEBUG__VCOMM_GetCommProperties (COMMPORTHANDLE hPort, _COMMPROP * pcmmprop) ;
BOOL __cdecl STATIC__VCOMM_GetCommProperties (COMMPORTHANDLE hPort, _COMMPROP * pcmmprop) ;
BOOL __cdecl PAGEABLE__VCOMM_GetCommProperties (COMMPORTHANDLE hPort, _COMMPROP * pcmmprop) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_GetCommQueueStatus (COMMPORTHANDLE hPort, _COMSTAT * pcmst) ;
BOOL __cdecl INIT__VCOMM_GetCommQueueStatus (COMMPORTHANDLE hPort, _COMSTAT * pcmst) ;
BOOL __cdecl DEBUG__VCOMM_GetCommQueueStatus (COMMPORTHANDLE hPort, _COMSTAT * pcmst) ;
BOOL __cdecl STATIC__VCOMM_GetCommQueueStatus (COMMPORTHANDLE hPort, _COMSTAT * pcmst) ;
BOOL __cdecl PAGEABLE__VCOMM_GetCommQueueStatus (COMMPORTHANDLE hPort, _COMSTAT * pcmst) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_GetCommState (COMMPORTHANDLE hPort, _DCB * pdcb) ;
BOOL __cdecl INIT__VCOMM_GetCommState (COMMPORTHANDLE hPort, _DCB * pdcb) ;
BOOL __cdecl DEBUG__VCOMM_GetCommState (COMMPORTHANDLE hPort, _DCB * pdcb) ;
BOOL __cdecl STATIC__VCOMM_GetCommState (COMMPORTHANDLE hPort, _DCB * pdcb) ;
BOOL __cdecl PAGEABLE__VCOMM_GetCommState (COMMPORTHANDLE hPort, _DCB * pdcb) ;
#endif

#ifdef WFW311_OR_LATER
DWORD __cdecl LOCK__VCOMM_GetLastError (COMMPORTHANDLE hPort) ;
DWORD __cdecl INIT__VCOMM_GetLastError (COMMPORTHANDLE hPort) ;
DWORD __cdecl DEBUG__VCOMM_GetLastError (COMMPORTHANDLE hPort) ;
DWORD __cdecl STATIC__VCOMM_GetLastError (COMMPORTHANDLE hPort) ;
DWORD __cdecl PAGEABLE__VCOMM_GetLastError (COMMPORTHANDLE hPort) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_GetModemStatus (COMMPORTHANDLE hPort, PDWORD pModemStatus) ;
BOOL __cdecl INIT__VCOMM_GetModemStatus (COMMPORTHANDLE hPort, PDWORD pModemStatus) ;
BOOL __cdecl DEBUG__VCOMM_GetModemStatus (COMMPORTHANDLE hPort, PDWORD pModemStatus) ;
BOOL __cdecl STATIC__VCOMM_GetModemStatus (COMMPORTHANDLE hPort, PDWORD pModemStatus) ;
BOOL __cdecl PAGEABLE__VCOMM_GetModemStatus (COMMPORTHANDLE hPort, PDWORD pModemStatus) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__VCOMM_GetSetCommTimeouts (COMMPORTHANDLE hPort, COMMTIMEOUTS * pcmtos, DWORD action) ;
BOOL __cdecl INIT__VCOMM_GetSetCommTimeouts (COMMPORTHANDLE hPort, COMMTIMEOUTS * pcmtos, DWORD action) ;
BOOL __cdecl DEBUG__VCOMM_GetSetCommTimeouts (COMMPORTHANDLE hPort, COMMTIMEOUTS * pcmtos, DWORD action) ;
BOOL __cdecl STATIC__VCOMM_GetSetCommTimeouts (COMMPORTHANDLE hPort, COMMTIMEOUTS * pcmtos, DWORD action) ;
BOOL __cdecl PAGEABLE__VCOMM_GetSetCommTimeouts (COMMPORTHANDLE hPort, COMMTIMEOUTS * pcmtos, DWORD action) ;
#endif

#ifdef WFW311_OR_LATER
COMMPORTHANDLE __cdecl LOCK__VCOMM_OpenComm (PCHAR szPortName, VMHANDLE VMid) ;
COMMPORTHANDLE __cdecl INIT__VCOMM_OpenComm (PCHAR szPortName, VMHANDLE VMid) ;
COMMPORTHANDLE __cdecl DEBUG__VCOMM_OpenComm (PCHAR szPortName, VMHANDLE VMid) ;
COMMPORTHANDLE __cdecl STATIC__VCOMM_OpenComm (PCHAR szPortName, VMHANDLE VMid) ;
COMMPORTHANDLE __cdecl PAGEABLE__VCOMM_OpenComm (PCHAR szPortName, VMHANDLE VMid) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_PurgeComm (COMMPORTHANDLE hPort, DWORD queueType) ;
BOOL __cdecl INIT__VCOMM_PurgeComm (COMMPORTHANDLE hPort, DWORD queueType) ;
BOOL __cdecl DEBUG__VCOMM_PurgeComm (COMMPORTHANDLE hPort, DWORD queueType) ;
BOOL __cdecl STATIC__VCOMM_PurgeComm (COMMPORTHANDLE hPort, DWORD queueType) ;
BOOL __cdecl PAGEABLE__VCOMM_PurgeComm (COMMPORTHANDLE hPort, DWORD queueType) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_ReadComm (COMMPORTHANDLE hPort, PVOID buf, DWORD nRequest, PDWORD pNRead) ;
BOOL __cdecl INIT__VCOMM_ReadComm (COMMPORTHANDLE hPort, PVOID buf, DWORD nRequest, PDWORD pNRead) ;
BOOL __cdecl DEBUG__VCOMM_ReadComm (COMMPORTHANDLE hPort, PVOID buf, DWORD nRequest, PDWORD pNRead) ;
BOOL __cdecl STATIC__VCOMM_ReadComm (COMMPORTHANDLE hPort, PVOID buf, DWORD nRequest, PDWORD pNRead) ;
BOOL __cdecl PAGEABLE__VCOMM_ReadComm (COMMPORTHANDLE hPort, PVOID buf, DWORD nRequest, PDWORD pNRead) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_SetCommEventMask (COMMPORTHANDLE hPort, DWORD dwMask, PDWORD events) ;
BOOL __cdecl INIT__VCOMM_SetCommEventMask (COMMPORTHANDLE hPort, DWORD dwMask, PDWORD events) ;
BOOL __cdecl DEBUG__VCOMM_SetCommEventMask (COMMPORTHANDLE hPort, DWORD dwMask, PDWORD events) ;
BOOL __cdecl STATIC__VCOMM_SetCommEventMask (COMMPORTHANDLE hPort, DWORD dwMask, PDWORD events) ;
BOOL __cdecl PAGEABLE__VCOMM_SetCommEventMask (COMMPORTHANDLE hPort, DWORD dwMask, PDWORD events) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_SetCommState (COMMPORTHANDLE hPort, _DCB * pdcb, DWORD actionMask) ;
BOOL __cdecl INIT__VCOMM_SetCommState (COMMPORTHANDLE hPort, _DCB * pdcb, DWORD actionMask) ;
BOOL __cdecl DEBUG__VCOMM_SetCommState (COMMPORTHANDLE hPort, _DCB * pdcb, DWORD actionMask) ;
BOOL __cdecl STATIC__VCOMM_SetCommState (COMMPORTHANDLE hPort, _DCB * pdcb, DWORD actionMask) ;
BOOL __cdecl PAGEABLE__VCOMM_SetCommState (COMMPORTHANDLE hPort, _DCB * pdcb, DWORD actionMask) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_SetReadCallback (COMMPORTHANDLE hPort, DWORD RxTrigger, PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl INIT__VCOMM_SetReadCallback (COMMPORTHANDLE hPort, DWORD RxTrigger, PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl DEBUG__VCOMM_SetReadCallback (COMMPORTHANDLE hPort, DWORD RxTrigger, PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl STATIC__VCOMM_SetReadCallback (COMMPORTHANDLE hPort, DWORD RxTrigger, PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl PAGEABLE__VCOMM_SetReadCallback (COMMPORTHANDLE hPort, DWORD RxTrigger, PCOMMNOTIFYPROC pProc, PVOID refData) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_SetupComm (COMMPORTHANDLE hPort, PBYTE pRxQueue, DWORD cbRxQueue, PBYTE pTxQueue, DWORD cbTxQueue) ;
BOOL __cdecl INIT__VCOMM_SetupComm (COMMPORTHANDLE hPort, PBYTE pRxQueue, DWORD cbRxQueue, PBYTE pTxQueue, DWORD cbTxQueue) ;
BOOL __cdecl DEBUG__VCOMM_SetupComm (COMMPORTHANDLE hPort, PBYTE pRxQueue, DWORD cbRxQueue, PBYTE pTxQueue, DWORD cbTxQueue) ;
BOOL __cdecl STATIC__VCOMM_SetupComm (COMMPORTHANDLE hPort, PBYTE pRxQueue, DWORD cbRxQueue, PBYTE pTxQueue, DWORD cbTxQueue) ;
BOOL __cdecl PAGEABLE__VCOMM_SetupComm (COMMPORTHANDLE hPort, PBYTE pRxQueue, DWORD cbRxQueue, PBYTE pTxQueue, DWORD cbTxQueue) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_SetWriteCallback (COMMPORTHANDLE hPort, DWORD TxTrigger, PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl INIT__VCOMM_SetWriteCallback (COMMPORTHANDLE hPort, DWORD TxTrigger, PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl DEBUG__VCOMM_SetWriteCallback (COMMPORTHANDLE hPort, DWORD TxTrigger, PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl STATIC__VCOMM_SetWriteCallback (COMMPORTHANDLE hPort, DWORD TxTrigger, PCOMMNOTIFYPROC pProc, PVOID refData) ;
BOOL __cdecl PAGEABLE__VCOMM_SetWriteCallback (COMMPORTHANDLE hPort, DWORD TxTrigger, PCOMMNOTIFYPROC pProc, PVOID refData) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_TransmitCommChar (COMMPORTHANDLE hPort, BYTE character) ;
BOOL __cdecl INIT__VCOMM_TransmitCommChar (COMMPORTHANDLE hPort, BYTE character) ;
BOOL __cdecl DEBUG__VCOMM_TransmitCommChar (COMMPORTHANDLE hPort, BYTE character) ;
BOOL __cdecl STATIC__VCOMM_TransmitCommChar (COMMPORTHANDLE hPort, BYTE character) ;
BOOL __cdecl PAGEABLE__VCOMM_TransmitCommChar (COMMPORTHANDLE hPort, BYTE character) ;
#endif

#ifdef WFW311_OR_LATER
BOOL __cdecl LOCK__VCOMM_WriteComm (COMMPORTHANDLE hPort, PVOID buf, DWORD nRequest, PDWORD pNWritten) ;
BOOL __cdecl INIT__VCOMM_WriteComm (COMMPORTHANDLE hPort, PVOID buf, DWORD nRequest, PDWORD pNWritten) ;
BOOL __cdecl DEBUG__VCOMM_WriteComm (COMMPORTHANDLE hPort, PVOID buf, DWORD nRequest, PDWORD pNWritten) ;
BOOL __cdecl STATIC__VCOMM_WriteComm (COMMPORTHANDLE hPort, PVOID buf, DWORD nRequest, PDWORD pNWritten) ;
BOOL __cdecl PAGEABLE__VCOMM_WriteComm (COMMPORTHANDLE hPort, PVOID buf, DWORD nRequest, PDWORD pNWritten) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VCOMM_Map_Win32DCB_To_Ring0 (PWIN32DCB pWin32dcb, P_DCB pdcb) ;
BOOL __stdcall INIT_VCOMM_Map_Win32DCB_To_Ring0 (PWIN32DCB pWin32dcb, P_DCB pdcb) ;
BOOL __stdcall DEBUG_VCOMM_Map_Win32DCB_To_Ring0 (PWIN32DCB pWin32dcb, P_DCB pdcb) ;
BOOL __stdcall STATIC_VCOMM_Map_Win32DCB_To_Ring0 (PWIN32DCB pWin32dcb, P_DCB pdcb) ;
BOOL __stdcall PAGEABLE_VCOMM_Map_Win32DCB_To_Ring0 (PWIN32DCB pWin32dcb, P_DCB pdcb) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VCOMM_Map_Ring0DCB_To_Win32 (P_DCB pdcb, PWIN32DCB pWin32dcb) ;
BOOL __stdcall INIT_VCOMM_Map_Ring0DCB_To_Win32 (P_DCB pdcb, PWIN32DCB pWin32dcb) ;
BOOL __stdcall DEBUG_VCOMM_Map_Ring0DCB_To_Win32 (P_DCB pdcb, PWIN32DCB pWin32dcb) ;
BOOL __stdcall STATIC_VCOMM_Map_Ring0DCB_To_Win32 (P_DCB pdcb, PWIN32DCB pWin32dcb) ;
BOOL __stdcall PAGEABLE_VCOMM_Map_Ring0DCB_To_Win32 (P_DCB pdcb, PWIN32DCB pWin32dcb) ;
#endif

#ifdef WIN40_OR_LATER
PCTNHANDLER __cdecl LOCK__VCOMM_Get_Contention_Handler (PCHAR szDeviceName) ;
PCTNHANDLER __cdecl INIT__VCOMM_Get_Contention_Handler (PCHAR szDeviceName) ;
PCTNHANDLER __cdecl DEBUG__VCOMM_Get_Contention_Handler (PCHAR szDeviceName) ;
PCTNHANDLER __cdecl STATIC__VCOMM_Get_Contention_Handler (PCHAR szDeviceName) ;
PCTNHANDLER __cdecl PAGEABLE__VCOMM_Get_Contention_Handler (PCHAR szDeviceName) ;
#endif

#ifdef WIN40_OR_LATER
HANDLE __cdecl LOCK__VCOMM_Map_Name_To_Resource (PCHAR szResource) ;
HANDLE __cdecl INIT__VCOMM_Map_Name_To_Resource (PCHAR szResource) ;
HANDLE __cdecl DEBUG__VCOMM_Map_Name_To_Resource (PCHAR szResource) ;
HANDLE __cdecl STATIC__VCOMM_Map_Name_To_Resource (PCHAR szResource) ;
HANDLE __cdecl PAGEABLE__VCOMM_Map_Name_To_Resource (PCHAR szResource) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__CONFIGMG_Get_Version (void) ;
DWORD __cdecl INIT__CONFIGMG_Get_Version (void) ;
DWORD __cdecl DEBUG__CONFIGMG_Get_Version (void) ;
DWORD __cdecl STATIC__CONFIGMG_Get_Version (void) ;
DWORD __cdecl PAGEABLE__CONFIGMG_Get_Version (void) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Initialize (DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Initialize (DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Initialize (DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Initialize (DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Initialize (DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Locate_DevNode (DEVNODE * pNode, PCHAR szDevId, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Locate_DevNode (DEVNODE * pNode, PCHAR szDevId, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Locate_DevNode (DEVNODE * pNode, PCHAR szDevId, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Locate_DevNode (DEVNODE * pNode, PCHAR szDevId, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Locate_DevNode (DEVNODE * pNode, PCHAR szDevId, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Parent (DEVNODE * pParentNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Parent (DEVNODE * pParentNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Parent (DEVNODE * pParentNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Parent (DEVNODE * pParentNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Parent (DEVNODE * pParentNode, DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Child (DEVNODE * pChildNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Child (DEVNODE * pChildNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Child (DEVNODE * pChildNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Child (DEVNODE * pChildNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Child (DEVNODE * pChildNode, DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Sibling (DEVNODE * pSiblingNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Sibling (DEVNODE * pSiblingNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Sibling (DEVNODE * pSiblingNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Sibling (DEVNODE * pSiblingNode, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Sibling (DEVNODE * pSiblingNode, DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Device_ID_Size (PDWORD pLen, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Device_ID_Size (PDWORD pLen, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Device_ID_Size (PDWORD pLen, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Device_ID_Size (PDWORD pLen, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Device_ID_Size (PDWORD pLen, DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Device_ID (DEVNODE node, PVOID buf, DWORD len, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Device_ID (DEVNODE node, PVOID buf, DWORD len, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Device_ID (DEVNODE node, PVOID buf, DWORD len, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Device_ID (DEVNODE node, PVOID buf, DWORD len, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Device_ID (DEVNODE node, PVOID buf, DWORD len, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Depth (PDWORD pDepth, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Depth (PDWORD pDepth, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Depth (PDWORD pDepth, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Depth (PDWORD pDepth, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Depth (PDWORD pDepth, DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Private_DWord (PDWORD pPrivate, DEVNODE nodeIn, DEVNODE nodeFor, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Private_DWord (PDWORD pPrivate, DEVNODE nodeIn, DEVNODE nodeFor, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Private_DWord (PDWORD pPrivate, DEVNODE nodeIn, DEVNODE nodeFor, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Private_DWord (PDWORD pPrivate, DEVNODE nodeIn, DEVNODE nodeFor, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Private_DWord (PDWORD pPrivate, DEVNODE nodeIn, DEVNODE nodeFor, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Set_Private_DWord (DEVNODE nodeIn, DEVNODE  nodeFor, DWORD value, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Set_Private_DWord (DEVNODE nodeIn, DEVNODE  nodeFor, DWORD value, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Set_Private_DWord (DEVNODE nodeIn, DEVNODE  nodeFor, DWORD value, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Set_Private_DWord (DEVNODE nodeIn, DEVNODE  nodeFor, DWORD value, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Set_Private_DWord (DEVNODE nodeIn, DEVNODE  nodeFor, DWORD value, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Create_DevNode (DEVNODE * pNode, PVOID devID, DEVNODE parentNode, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Create_DevNode (DEVNODE * pNode, PVOID devID, DEVNODE parentNode, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Create_DevNode (DEVNODE * pNode, PVOID devID, DEVNODE parentNode, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Create_DevNode (DEVNODE * pNode, PVOID devID, DEVNODE parentNode, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Create_DevNode (DEVNODE * pNode, PVOID devID, DEVNODE parentNode, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Query_Remove_SubTree (DEVNODE ancestor, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Query_Remove_SubTree (DEVNODE ancestor, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Query_Remove_SubTree (DEVNODE ancestor, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Query_Remove_SubTree (DEVNODE ancestor, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Query_Remove_SubTree (DEVNODE ancestor, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Remove_SubTree (DEVNODE ancestor, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Remove_SubTree (DEVNODE ancestor, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Remove_SubTree (DEVNODE ancestor, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Remove_SubTree (DEVNODE ancestor, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Remove_SubTree (DEVNODE ancestor, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Register_Device_Driver (DEVNODE node, CMCONFIGHANDLER handler, DWORD refData, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Register_Device_Driver (DEVNODE node, CMCONFIGHANDLER handler, DWORD refData, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Register_Device_Driver (DEVNODE node, CMCONFIGHANDLER handler, DWORD refData, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Register_Device_Driver (DEVNODE node, CMCONFIGHANDLER handler, DWORD refData, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Register_Device_Driver (DEVNODE node, CMCONFIGHANDLER handler, DWORD refData, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Register_Enumerator (DEVNODE node, CMENUMHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Register_Enumerator (DEVNODE node, CMENUMHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Register_Enumerator (DEVNODE node, CMENUMHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Register_Enumerator (DEVNODE node, CMENUMHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Register_Enumerator (DEVNODE node, CMENUMHANDLER handler, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Register_Arbitrator (PREGISTERID pRid, RESOURCEID id, CMARBHANDLER handler, DWORD refData, DEVNODE arbNode, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Register_Arbitrator (PREGISTERID pRid, RESOURCEID id, CMARBHANDLER handler, DWORD refData, DEVNODE arbNode, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Register_Arbitrator (PREGISTERID pRid, RESOURCEID id, CMARBHANDLER handler, DWORD refData, DEVNODE arbNode, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Register_Arbitrator (PREGISTERID pRid, RESOURCEID id, CMARBHANDLER handler, DWORD refData, DEVNODE arbNode, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Register_Arbitrator (PREGISTERID pRid, RESOURCEID id, CMARBHANDLER handler, DWORD refData, DEVNODE arbNode, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Deregister_Arbitrator (REGISTERID rid, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Deregister_Arbitrator (REGISTERID rid, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Deregister_Arbitrator (REGISTERID rid, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Deregister_Arbitrator (REGISTERID rid, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Deregister_Arbitrator (REGISTERID rid, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Add_Res_Des (PRES_DES prdResDes, LOG_CONF lcLogConf, RESOURCEID resID, PVOID pResData, DWORD resLen, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Add_Res_Des (PRES_DES prdResDes, LOG_CONF lcLogConf, RESOURCEID resID, PVOID pResData, DWORD resLen, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Add_Res_Des (PRES_DES prdResDes, LOG_CONF lcLogConf, RESOURCEID resID, PVOID pResData, DWORD resLen, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Add_Res_Des (PRES_DES prdResDes, LOG_CONF lcLogConf, RESOURCEID resID, PVOID pResData, DWORD resLen, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Add_Res_Des (PRES_DES prdResDes, LOG_CONF lcLogConf, RESOURCEID resID, PVOID pResData, DWORD resLen, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Modify_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, RESOURCEID resID, PVOID pResData, DWORD resLen, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Modify_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, RESOURCEID resID, PVOID pResData, DWORD resLen, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Modify_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, RESOURCEID resID, PVOID pResData, DWORD resLen, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Modify_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, RESOURCEID resID, PVOID pResData, DWORD resLen, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Modify_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, RESOURCEID resID, PVOID pResData, DWORD resLen, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Free_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Free_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Free_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Free_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Free_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Next_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, RESOURCEID resFor, PRESOURCEID pResID, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Next_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, RESOURCEID resFor, PRESOURCEID pResID, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Next_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, RESOURCEID resFor, PRESOURCEID pResID, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Next_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, RESOURCEID resFor, PRESOURCEID pResID, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Next_Res_Des (PRES_DES prdResDes, RES_DES rdResDes, RESOURCEID resFor, PRESOURCEID pResID, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Performance_Info (PCMPERFINFO pPerfinfo, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Performance_Info (PCMPERFINFO pPerfinfo, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Performance_Info (PCMPERFINFO pPerfinfo, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Performance_Info (PCMPERFINFO pPerfinfo, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Performance_Info (PCMPERFINFO pPerfinfo, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Res_Des_Data_Size (PDWORD pSize, RES_DES rdResDes, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Res_Des_Data_Size (PDWORD pSize, RES_DES rdResDes, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Res_Des_Data_Size (PDWORD pSize, RES_DES rdResDes, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Res_Des_Data_Size (PDWORD pSize, RES_DES rdResDes, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Res_Des_Data_Size (PDWORD pSize, RES_DES rdResDes, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Res_Des_Data (RES_DES rd, PVOID buf, DWORD bufLen, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Res_Des_Data (RES_DES rd, PVOID buf, DWORD bufLen, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Res_Des_Data (RES_DES rd, PVOID buf, DWORD bufLen, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Res_Des_Data (RES_DES rd, PVOID buf, DWORD bufLen, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Res_Des_Data (RES_DES rd, PVOID buf, DWORD bufLen, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Process_Events_Now (DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Process_Events_Now (DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Process_Events_Now (DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Process_Events_Now (DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Process_Events_Now (DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Create_Range_List (PRANGE_LIST prlh, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Create_Range_List (PRANGE_LIST prlh, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Create_Range_List (PRANGE_LIST prlh, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Create_Range_List (PRANGE_LIST prlh, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Create_Range_List (PRANGE_LIST prlh, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Add_Range (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Add_Range (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Add_Range (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Add_Range (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Add_Range (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Delete_Range (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Delete_Range (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Delete_Range (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Delete_Range (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Delete_Range (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Test_Range_Available (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Test_Range_Available (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Test_Range_Available (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Test_Range_Available (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Test_Range_Available (DWORD startVal, DWORD endVal, RANGE_LIST rlh, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Dup_Range_List (RANGE_LIST rlhold, RANGE_LIST rlhnew, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Dup_Range_List (RANGE_LIST rlhold, RANGE_LIST rlhnew, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Dup_Range_List (RANGE_LIST rlhold, RANGE_LIST rlhnew, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Dup_Range_List (RANGE_LIST rlhold, RANGE_LIST rlhnew, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Dup_Range_List (RANGE_LIST rlhold, RANGE_LIST rlhnew, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Free_Range_List (RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Free_Range_List (RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Free_Range_List (RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Free_Range_List (RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Free_Range_List (RANGE_LIST rlh, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Invert_Range_List (RANGE_LIST rlhOld, RANGE_LIST rlhNew, DWORD maxVal, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Invert_Range_List (RANGE_LIST rlhOld, RANGE_LIST rlhNew, DWORD maxVal, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Invert_Range_List (RANGE_LIST rlhOld, RANGE_LIST rlhNew, DWORD maxVal, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Invert_Range_List (RANGE_LIST rlhOld, RANGE_LIST rlhNew, DWORD maxVal, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Invert_Range_List (RANGE_LIST rlhOld, RANGE_LIST rlhNew, DWORD maxVal, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Intersect_Range_List (RANGE_LIST rlh1, RANGE_LIST rlh2, RANGE_LIST rlhNew, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Intersect_Range_List (RANGE_LIST rlh1, RANGE_LIST rlh2, RANGE_LIST rlhNew, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Intersect_Range_List (RANGE_LIST rlh1, RANGE_LIST rlh2, RANGE_LIST rlhNew, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Intersect_Range_List (RANGE_LIST rlh1, RANGE_LIST rlh2, RANGE_LIST rlhNew, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Intersect_Range_List (RANGE_LIST rlh1, RANGE_LIST rlh2, RANGE_LIST rlhNew, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_First_Range (RANGE_LIST rlh, PDWORD pStart,PDWORD pEnd, PRANGE_ELEMENT preElement, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_First_Range (RANGE_LIST rlh, PDWORD pStart,PDWORD pEnd, PRANGE_ELEMENT preElement, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_First_Range (RANGE_LIST rlh, PDWORD pStart,PDWORD pEnd, PRANGE_ELEMENT preElement, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_First_Range (RANGE_LIST rlh, PDWORD pStart,PDWORD pEnd, PRANGE_ELEMENT preElement, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_First_Range (RANGE_LIST rlh, PDWORD pStart,PDWORD pEnd, PRANGE_ELEMENT preElement, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Next_Range (PRANGE_ELEMENT preElement, PDWORD pStart, PDWORD pEnd, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Next_Range (PRANGE_ELEMENT preElement, PDWORD pStart, PDWORD pEnd, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Next_Range (PRANGE_ELEMENT preElement, PDWORD pStart, PDWORD pEnd, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Next_Range (PRANGE_ELEMENT preElement, PDWORD pStart, PDWORD pEnd, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Next_Range (PRANGE_ELEMENT preElement, PDWORD pStart, PDWORD pEnd, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Dump_Range_List (RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Dump_Range_List (RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Dump_Range_List (RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Dump_Range_List (RANGE_LIST rlh, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Dump_Range_List (RANGE_LIST rlh, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Load_DLVxDs (DEVNODE node, PCHAR szFiles, DWORD loadType, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Load_DLVxDs (DEVNODE node, PCHAR szFiles, DWORD loadType, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Load_DLVxDs (DEVNODE node, PCHAR szFiles, DWORD loadType, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Load_DLVxDs (DEVNODE node, PCHAR szFiles, DWORD loadType, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Load_DLVxDs (DEVNODE node, PCHAR szFiles, DWORD loadType, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_DDBs (PPPVMMDDB pppDDB, PDWORD pCount, DWORD loadType, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_DDBs (PPPVMMDDB pppDDB, PDWORD pCount, DWORD loadType, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_DDBs (PPPVMMDDB pppDDB, PDWORD pCount, DWORD loadType, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_DDBs (PPPVMMDDB pppDDB, PDWORD pCount, DWORD loadType, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_DDBs (PPPVMMDDB pppDDB, PDWORD pCount, DWORD loadType, DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_CRC_CheckSum (PVOID buf, DWORD dwSize, PDWORD pSeed, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_CRC_CheckSum (PVOID buf, DWORD dwSize, PDWORD pSeed, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_CRC_CheckSum (PVOID buf, DWORD dwSize, PDWORD pSeed, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_CRC_CheckSum (PVOID buf, DWORD dwSize, PDWORD pSeed, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_CRC_CheckSum (PVOID buf, DWORD dwSize, PDWORD pSeed, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Register_DevLoader (PVMMDDB pDDB, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Register_DevLoader (PVMMDDB pDDB, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Register_DevLoader (PVMMDDB pDDB, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Register_DevLoader (PVMMDDB pDDB, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Register_DevLoader (PVMMDDB pDDB, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Reenumerate_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Reenumerate_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Reenumerate_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Reenumerate_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Reenumerate_DevNode (DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Setup_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Setup_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Setup_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Setup_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Setup_DevNode (DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Reset_Children_Marks (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Reset_Children_Marks (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Reset_Children_Marks (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Reset_Children_Marks (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Reset_Children_Marks (DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_DevNode_Status (PDWORD pStatus, PDWORD pProblem, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_DevNode_Status (PDWORD pStatus, PDWORD pProblem, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_DevNode_Status (PDWORD pStatus, PDWORD pProblem, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_DevNode_Status (PDWORD pStatus, PDWORD pProblem, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_DevNode_Status (PDWORD pStatus, PDWORD pProblem, DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Remove_Unmarked_Children (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Remove_Unmarked_Children (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Remove_Unmarked_Children (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Remove_Unmarked_Children (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Remove_Unmarked_Children (DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_ISAPNP_To_CM (PBYTE pBuf, DEVNODE node, DWORD logDev, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_ISAPNP_To_CM (PBYTE pBuf, DEVNODE node, DWORD logDev, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_ISAPNP_To_CM (PBYTE pBuf, DEVNODE node, DWORD logDev, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_ISAPNP_To_CM (PBYTE pBuf, DEVNODE node, DWORD logDev, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_ISAPNP_To_CM (PBYTE pBuf, DEVNODE node, DWORD logDev, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_CallBack_Device_Driver (CMCONFIGHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_CallBack_Device_Driver (CMCONFIGHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_CallBack_Device_Driver (CMCONFIGHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_CallBack_Device_Driver (CMCONFIGHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_CallBack_Device_Driver (CMCONFIGHANDLER handler, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_CallBack_Enumerator (CMENUMHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_CallBack_Enumerator (CMENUMHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_CallBack_Enumerator (CMENUMHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_CallBack_Enumerator (CMENUMHANDLER handler, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_CallBack_Enumerator (CMENUMHANDLER handler, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Alloc_Log_Conf (PCMCONFIG pccBuffer, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Alloc_Log_Conf (PCMCONFIG pccBuffer, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Alloc_Log_Conf (PCMCONFIG pccBuffer, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Alloc_Log_Conf (PCMCONFIG pccBuffer, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Alloc_Log_Conf (PCMCONFIG pccBuffer, DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_DevNode_Key_Size (PDWORD pLen, DEVNODE node, PCHAR szSubKey, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_DevNode_Key_Size (PDWORD pLen, DEVNODE node, PCHAR szSubKey, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_DevNode_Key_Size (PDWORD pLen, DEVNODE node, PCHAR szSubKey, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_DevNode_Key_Size (PDWORD pLen, DEVNODE node, PCHAR szSubKey, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_DevNode_Key_Size (PDWORD pLen, DEVNODE node, PCHAR szSubKey, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_DevNode_Key (DEVNODE node, PCHAR szSubkey, PVOID buf, DWORD len, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_DevNode_Key (DEVNODE node, PCHAR szSubkey, PVOID buf, DWORD len, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_DevNode_Key (DEVNODE node, PCHAR szSubkey, PVOID buf, DWORD len, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_DevNode_Key (DEVNODE node, PCHAR szSubkey, PVOID buf, DWORD len, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_DevNode_Key (DEVNODE node, PCHAR szSubkey, PVOID buf, DWORD len, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Read_Registry_Value (DEVNODE node, PCHAR szSubkey, PCHAR szValueName, DWORD expectedType, PVOID buf, PDWORD pLen, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Read_Registry_Value (DEVNODE node, PCHAR szSubkey, PCHAR szValueName, DWORD expectedType, PVOID buf, PDWORD pLen, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Read_Registry_Value (DEVNODE node, PCHAR szSubkey, PCHAR szValueName, DWORD expectedType, PVOID buf, PDWORD pLen, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Read_Registry_Value (DEVNODE node, PCHAR szSubkey, PCHAR szValueName, DWORD expectedType, PVOID buf, PDWORD pLen, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Read_Registry_Value (DEVNODE node, PCHAR szSubkey, PCHAR szValueName, DWORD expectedType, PVOID buf, PDWORD pLen, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Write_Registry_Value (DEVNODE node, PCHAR szSubkey, PCHAR szValueName, DWORD dwType, PVOID buffer, DWORD len, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Write_Registry_Value (DEVNODE node, PCHAR szSubkey, PCHAR szValueName, DWORD dwType, PVOID buffer, DWORD len, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Write_Registry_Value (DEVNODE node, PCHAR szSubkey, PCHAR szValueName, DWORD dwType, PVOID buffer, DWORD len, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Write_Registry_Value (DEVNODE node, PCHAR szSubkey, PCHAR szValueName, DWORD dwType, PVOID buffer, DWORD len, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Write_Registry_Value (DEVNODE node, PCHAR szSubkey, PCHAR szValueName, DWORD dwType, PVOID buffer, DWORD len, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Disable_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Disable_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Disable_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Disable_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Disable_DevNode (DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Enable_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Enable_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Enable_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Enable_DevNode (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Enable_DevNode (DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Move_DevNode (DEVNODE nodeFrom, DEVNODE nodeTo, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Move_DevNode (DEVNODE nodeFrom, DEVNODE nodeTo, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Move_DevNode (DEVNODE nodeFrom, DEVNODE nodeTo, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Move_DevNode (DEVNODE nodeFrom, DEVNODE nodeTo, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Move_DevNode (DEVNODE nodeFrom, DEVNODE nodeTo, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Set_Bus_Info (DEVNODE node, CMBUSTYPE btType, DWORD infoLen, PVOID pInfo, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Set_Bus_Info (DEVNODE node, CMBUSTYPE btType, DWORD infoLen, PVOID pInfo, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Set_Bus_Info (DEVNODE node, CMBUSTYPE btType, DWORD infoLen, PVOID pInfo, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Set_Bus_Info (DEVNODE node, CMBUSTYPE btType, DWORD infoLen, PVOID pInfo, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Set_Bus_Info (DEVNODE node, CMBUSTYPE btType, DWORD infoLen, PVOID pInfo, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Bus_Info (DEVNODE node, PCMBUSTYPE pbtType, PDWORD pInfoLen, PVOID pInfo, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Bus_Info (DEVNODE node, PCMBUSTYPE pbtType, PDWORD pInfoLen, PVOID pInfo, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Bus_Info (DEVNODE node, PCMBUSTYPE pbtType, PDWORD pInfoLen, PVOID pInfo, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Bus_Info (DEVNODE node, PCMBUSTYPE pbtType, PDWORD pInfoLen, PVOID pInfo, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Bus_Info (DEVNODE node, PCMBUSTYPE pbtType, PDWORD pInfoLen, PVOID pInfo, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Set_HW_Prof (DWORD config, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Set_HW_Prof (DWORD config, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Set_HW_Prof (DWORD config, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Set_HW_Prof (DWORD config, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Set_HW_Prof (DWORD config, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Recompute_HW_Prof (DWORD dwDock, DWORD serial, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Recompute_HW_Prof (DWORD dwDock, DWORD serial, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Recompute_HW_Prof (DWORD dwDock, DWORD serial, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Recompute_HW_Prof (DWORD dwDock, DWORD serial, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Recompute_HW_Prof (DWORD dwDock, DWORD serial, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Query_Change_HW_Prof (DWORD dwDock, DWORD serial, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Query_Change_HW_Prof (DWORD dwDock, DWORD serial, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Query_Change_HW_Prof (DWORD dwDock, DWORD serial, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Query_Change_HW_Prof (DWORD dwDock, DWORD serial, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Query_Change_HW_Prof (DWORD dwDock, DWORD serial, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Device_Driver_Private_DWord (DEVNODE node, PDWORD pPriv, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Device_Driver_Private_DWord (DEVNODE node, PDWORD pPriv, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Device_Driver_Private_DWord (DEVNODE node, PDWORD pPriv, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Device_Driver_Private_DWord (DEVNODE node, PDWORD pPriv, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Device_Driver_Private_DWord (DEVNODE node, PDWORD pPriv, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Set_Device_Driver_Private_DWord (DEVNODE node, DWORD priv, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Set_Device_Driver_Private_DWord (DEVNODE node, DWORD priv, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Set_Device_Driver_Private_DWord (DEVNODE node, DWORD priv, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Set_Device_Driver_Private_DWord (DEVNODE node, DWORD priv, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Set_Device_Driver_Private_DWord (DEVNODE node, DWORD priv, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_HW_Prof_Flags (PCHAR szDevNodeName, DWORD config, PDWORD pValue, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_HW_Prof_Flags (PCHAR szDevNodeName, DWORD config, PDWORD pValue, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_HW_Prof_Flags (PCHAR szDevNodeName, DWORD config, PDWORD pValue, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_HW_Prof_Flags (PCHAR szDevNodeName, DWORD config, PDWORD pValue, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_HW_Prof_Flags (PCHAR szDevNodeName, DWORD config, PDWORD pValue, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Set_HW_Prof_Flags (PCHAR szDevNodeName, DWORD config, DWORD value, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Set_HW_Prof_Flags (PCHAR szDevNodeName, DWORD config, DWORD value, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Set_HW_Prof_Flags (PCHAR szDevNodeName, DWORD config, DWORD value, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Set_HW_Prof_Flags (PCHAR szDevNodeName, DWORD config, DWORD value, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Set_HW_Prof_Flags (PCHAR szDevNodeName, DWORD config, DWORD value, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Read_Registry_Log_Confs (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Read_Registry_Log_Confs (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Read_Registry_Log_Confs (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Read_Registry_Log_Confs (DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Read_Registry_Log_Confs (DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Run_Detection (DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Run_Detection (DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Run_Detection (DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Run_Detection (DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Run_Detection (DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Call_At_Appy_Time (CMAPPYCALLBACKHANDLER pCallback, PVOID refData, DWORD flags, DWORD timeout) ;
CONFIGRET __cdecl INIT__CONFIGMG_Call_At_Appy_Time (CMAPPYCALLBACKHANDLER pCallback, PVOID refData, DWORD flags, DWORD timeout) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Call_At_Appy_Time (CMAPPYCALLBACKHANDLER pCallback, PVOID refData, DWORD flags, DWORD timeout) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Call_At_Appy_Time (CMAPPYCALLBACKHANDLER pCallback, PVOID refData, DWORD flags, DWORD timeout) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Call_At_Appy_Time (CMAPPYCALLBACKHANDLER pCallback, PVOID refData, DWORD flags, DWORD timeout) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Set_Private_Problem (DEVNODE node, PVOID refData, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Set_Private_Problem (DEVNODE node, PVOID refData, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Set_Private_Problem (DEVNODE node, PVOID refData, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Set_Private_Problem (DEVNODE node, PVOID refData, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Set_Private_Problem (DEVNODE node, PVOID refData, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Hardware_Profile_Info (ULONG ulIndex, PHWPROFILEINFO pHWProfileInfo, ULONG ulFlags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Hardware_Profile_Info (ULONG ulIndex, PHWPROFILEINFO pHWProfileInfo, ULONG ulFlags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Hardware_Profile_Info (ULONG ulIndex, PHWPROFILEINFO pHWProfileInfo, ULONG ulFlags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Hardware_Profile_Info (ULONG ulIndex, PHWPROFILEINFO pHWProfileInfo, ULONG ulFlags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Hardware_Profile_Info (ULONG ulIndex, PHWPROFILEINFO pHWProfileInfo, ULONG ulFlags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Query_Arbitrator_Free_Size (PDWORD pSize, DEVNODE node, RESOURCEID resID, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Query_Arbitrator_Free_Size (PDWORD pSize, DEVNODE node, RESOURCEID resID, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Query_Arbitrator_Free_Size (PDWORD pSize, DEVNODE node, RESOURCEID resID, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Query_Arbitrator_Free_Size (PDWORD pSize, DEVNODE node, RESOURCEID resID, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Query_Arbitrator_Free_Size (PDWORD pSize, DEVNODE node, RESOURCEID resID, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Query_Arbitrator_Free_Data (PVOID pData, DWORD len, DEVNODE node, RESOURCEID resID, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Query_Arbitrator_Free_Data (PVOID pData, DWORD len, DEVNODE node, RESOURCEID resID, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Query_Arbitrator_Free_Data (PVOID pData, DWORD len, DEVNODE node, RESOURCEID resID, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Query_Arbitrator_Free_Data (PVOID pData, DWORD len, DEVNODE node, RESOURCEID resID, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Query_Arbitrator_Free_Data (PVOID pData, DWORD len, DEVNODE node, RESOURCEID resID, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Sort_NodeList (NODELIST_HEADER nlh, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Sort_NodeList (NODELIST_HEADER nlh, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Sort_NodeList (NODELIST_HEADER nlh, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Sort_NodeList (NODELIST_HEADER nlh, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Sort_NodeList (NODELIST_HEADER nlh, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Yield (DWORD nMicroseconds, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Yield (DWORD nMicroseconds, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Yield (DWORD nMicroseconds, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Yield (DWORD nMicroseconds, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Yield (DWORD nMicroseconds, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Lock (DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Lock (DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Lock (DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Lock (DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Lock (DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Unlock (DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Unlock (DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Unlock (DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Unlock (DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Unlock (DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Add_Empty_Log_Conf (PLOG_CONF plc, DEVNODE node, DWORD priority, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Add_Empty_Log_Conf (PLOG_CONF plc, DEVNODE node, DWORD priority, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Add_Empty_Log_Conf (PLOG_CONF plc, DEVNODE node, DWORD priority, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Add_Empty_Log_Conf (PLOG_CONF plc, DEVNODE node, DWORD priority, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Add_Empty_Log_Conf (PLOG_CONF plc, DEVNODE node, DWORD priority, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Free_Log_Conf (LOG_CONF lcToFree, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Free_Log_Conf (LOG_CONF lcToFree, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Free_Log_Conf (LOG_CONF lcToFree, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Free_Log_Conf (LOG_CONF lcToFree, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Free_Log_Conf (LOG_CONF lcToFree, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_First_Log_Conf (PLOG_CONF plc, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_First_Log_Conf (PLOG_CONF plc, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_First_Log_Conf (PLOG_CONF plc, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_First_Log_Conf (PLOG_CONF plc, DEVNODE node, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_First_Log_Conf (PLOG_CONF plc, DEVNODE node, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_Next_Log_Conf (PLOG_CONF plc, LOG_CONF lc, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_Next_Log_Conf (PLOG_CONF plc, LOG_CONF lc, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_Next_Log_Conf (PLOG_CONF plc, LOG_CONF lc, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_Next_Log_Conf (PLOG_CONF plc, LOG_CONF lc, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_Next_Log_Conf (PLOG_CONF plc, LOG_CONF lc, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Fail_Change_HW_Prof (DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Fail_Change_HW_Prof (DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Fail_Change_HW_Prof (DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Fail_Change_HW_Prof (DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Fail_Change_HW_Prof (DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Call_Enumerator_Function (DEVNODE dnDevNode, ENUMFUNC efFunc, DWORD RefData, PVOID pBuffer, DWORD ulBufferSize, DWORD ulFlags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Call_Enumerator_Function (DEVNODE dnDevNode, ENUMFUNC efFunc, DWORD RefData, PVOID pBuffer, DWORD ulBufferSize, DWORD ulFlags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Call_Enumerator_Function (DEVNODE dnDevNode, ENUMFUNC efFunc, DWORD RefData, PVOID pBuffer, DWORD ulBufferSize, DWORD ulFlags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Call_Enumerator_Function (DEVNODE dnDevNode, ENUMFUNC efFunc, DWORD RefData, PVOID pBuffer, DWORD ulBufferSize, DWORD ulFlags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Call_Enumerator_Function (DEVNODE dnDevNode, ENUMFUNC efFunc, DWORD RefData, PVOID pBuffer, DWORD ulBufferSize, DWORD ulFlags) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VMD_Post_Pointer_Message (DWORD DeltaX, DWORD DeltaY, BYTE ButtonStatus, MOUSE_INSTANCE * pMI) ;
VOID __stdcall INIT_VMD_Post_Pointer_Message (DWORD DeltaX, DWORD DeltaY, BYTE ButtonStatus, MOUSE_INSTANCE * pMI) ;
VOID __stdcall DEBUG_VMD_Post_Pointer_Message (DWORD DeltaX, DWORD DeltaY, BYTE ButtonStatus, MOUSE_INSTANCE * pMI) ;
VOID __stdcall STATIC_VMD_Post_Pointer_Message (DWORD DeltaX, DWORD DeltaY, BYTE ButtonStatus, MOUSE_INSTANCE * pMI) ;
VOID __stdcall PAGEABLE_VMD_Post_Pointer_Message (DWORD DeltaX, DWORD DeltaY, BYTE ButtonStatus, MOUSE_INSTANCE * pMI) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VMD_Set_Cursor_Proc (CURSORPROCINFO * pCursProcInfo, PCursorProc_THUNK pThunk) ;
VOID __stdcall INIT_VMD_Set_Cursor_Proc (CURSORPROCINFO * pCursProcInfo, PCursorProc_THUNK pThunk) ;
VOID __stdcall DEBUG_VMD_Set_Cursor_Proc (CURSORPROCINFO * pCursProcInfo, PCursorProc_THUNK pThunk) ;
VOID __stdcall STATIC_VMD_Set_Cursor_Proc (CURSORPROCINFO * pCursProcInfo, PCursorProc_THUNK pThunk) ;
VOID __stdcall PAGEABLE_VMD_Set_Cursor_Proc (CURSORPROCINFO * pCursProcInfo, PCursorProc_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VMD_Call_Cursor_Proc (DWORD FunctionIndex, VMHANDLE hVM) ;
BOOL __stdcall INIT_VMD_Call_Cursor_Proc (DWORD FunctionIndex, VMHANDLE hVM) ;
BOOL __stdcall DEBUG_VMD_Call_Cursor_Proc (DWORD FunctionIndex, VMHANDLE hVM) ;
BOOL __stdcall STATIC_VMD_Call_Cursor_Proc (DWORD FunctionIndex, VMHANDLE hVM) ;
BOOL __stdcall PAGEABLE_VMD_Call_Cursor_Proc (DWORD FunctionIndex, VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VMD_Set_Mouse_Data (DWORD MouseType, DWORD IRQ, BOOL bBall, DWORD BallID, DWORD ButtonCount, DWORD PortNumber) ;
VOID __stdcall INIT_VMD_Set_Mouse_Data (DWORD MouseType, DWORD IRQ, BOOL bBall, DWORD BallID, DWORD ButtonCount, DWORD PortNumber) ;
VOID __stdcall DEBUG_VMD_Set_Mouse_Data (DWORD MouseType, DWORD IRQ, BOOL bBall, DWORD BallID, DWORD ButtonCount, DWORD PortNumber) ;
VOID __stdcall STATIC_VMD_Set_Mouse_Data (DWORD MouseType, DWORD IRQ, BOOL bBall, DWORD BallID, DWORD ButtonCount, DWORD PortNumber) ;
VOID __stdcall PAGEABLE_VMD_Set_Mouse_Data (DWORD MouseType, DWORD IRQ, BOOL bBall, DWORD BallID, DWORD ButtonCount, DWORD PortNumber) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VMD_Get_Mouse_Data (PBYTE MouseType, PBYTE IRQ, PDWORD BallID, PBYTE ButtonCount, PBYTE PortNumber) ;
VOID __stdcall INIT_VMD_Get_Mouse_Data (PBYTE MouseType, PBYTE IRQ, PDWORD BallID, PBYTE ButtonCount, PBYTE PortNumber) ;
VOID __stdcall DEBUG_VMD_Get_Mouse_Data (PBYTE MouseType, PBYTE IRQ, PDWORD BallID, PBYTE ButtonCount, PBYTE PortNumber) ;
VOID __stdcall STATIC_VMD_Get_Mouse_Data (PBYTE MouseType, PBYTE IRQ, PDWORD BallID, PBYTE ButtonCount, PBYTE PortNumber) ;
VOID __stdcall PAGEABLE_VMD_Get_Mouse_Data (PBYTE MouseType, PBYTE IRQ, PDWORD BallID, PBYTE ButtonCount, PBYTE PortNumber) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VMD_Post_Absolute_Pointer_Message (DWORD AbsX, DWORD AbsY, BYTE ButtonStatus) ;
VOID __stdcall INIT_VMD_Post_Absolute_Pointer_Message (DWORD AbsX, DWORD AbsY, BYTE ButtonStatus) ;
VOID __stdcall DEBUG_VMD_Post_Absolute_Pointer_Message (DWORD AbsX, DWORD AbsY, BYTE ButtonStatus) ;
VOID __stdcall STATIC_VMD_Post_Absolute_Pointer_Message (DWORD AbsX, DWORD AbsY, BYTE ButtonStatus) ;
VOID __stdcall PAGEABLE_VMD_Post_Absolute_Pointer_Message (DWORD AbsX, DWORD AbsY, BYTE ButtonStatus) ;
#endif

//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_IFSMgr_Get_Version (void) ;
DWORD __cdecl INIT_IFSMgr_Get_Version (void) ;
DWORD __cdecl DEBUG_IFSMgr_Get_Version (void) ;
DWORD __cdecl STATIC_IFSMgr_Get_Version (void) ;
DWORD __cdecl PAGEABLE_IFSMgr_Get_Version (void) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_RegisterMount (pIFSFunc pFunc, DWORD FSD_IFS_Version, DWORD FSDType) ;
INT __cdecl INIT_IFSMgr_RegisterMount (pIFSFunc pFunc, DWORD FSD_IFS_Version, DWORD FSDType) ;
INT __cdecl DEBUG_IFSMgr_RegisterMount (pIFSFunc pFunc, DWORD FSD_IFS_Version, DWORD FSDType) ;
INT __cdecl STATIC_IFSMgr_RegisterMount (pIFSFunc pFunc, DWORD FSD_IFS_Version, DWORD FSDType) ;
INT __cdecl PAGEABLE_IFSMgr_RegisterMount (pIFSFunc pFunc, DWORD FSD_IFS_Version, DWORD FSDType) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_RegisterNet (pIFSFunc pFunc, DWORD FSD_IFS_Version, DWORD NetID) ;
INT __cdecl INIT_IFSMgr_RegisterNet (pIFSFunc pFunc, DWORD FSD_IFS_Version, DWORD NetID) ;
INT __cdecl DEBUG_IFSMgr_RegisterNet (pIFSFunc pFunc, DWORD FSD_IFS_Version, DWORD NetID) ;
INT __cdecl STATIC_IFSMgr_RegisterNet (pIFSFunc pFunc, DWORD FSD_IFS_Version, DWORD NetID) ;
INT __cdecl PAGEABLE_IFSMgr_RegisterNet (pIFSFunc pFunc, DWORD FSD_IFS_Version, DWORD NetID) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_RegisterMailSlot (pIFSFunc pFunc, DWORD FSD_IFS_Version) ;
INT __cdecl INIT_IFSMgr_RegisterMailSlot (pIFSFunc pFunc, DWORD FSD_IFS_Version) ;
INT __cdecl DEBUG_IFSMgr_RegisterMailSlot (pIFSFunc pFunc, DWORD FSD_IFS_Version) ;
INT __cdecl STATIC_IFSMgr_RegisterMailSlot (pIFSFunc pFunc, DWORD FSD_IFS_Version) ;
INT __cdecl PAGEABLE_IFSMgr_RegisterMailSlot (pIFSFunc pFunc, DWORD FSD_IFS_Version) ;
#endif

#define Enable_Touch_1st_Meg() VMMcall(Enable_Touch_1st_Meg)
#define Disable_Touch_1st_Meg() VMMcall(Disable_Touch_1st_Meg)
#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_IFSMgr_Get_NetTime (PDWORD pAdditionalmSec) ;
DWORD __cdecl INIT_IFSMgr_Get_NetTime (PDWORD pAdditionalmSec) ;
DWORD __cdecl DEBUG_IFSMgr_Get_NetTime (PDWORD pAdditionalmSec) ;
DWORD __cdecl STATIC_IFSMgr_Get_NetTime (PDWORD pAdditionalmSec) ;
DWORD __cdecl PAGEABLE_IFSMgr_Get_NetTime (PDWORD pAdditionalmSec) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_IFSMgr_Get_DOSTime (PDWORD pAdditionalmSec) ;
DWORD __cdecl INIT_IFSMgr_Get_DOSTime (PDWORD pAdditionalmSec) ;
DWORD __cdecl DEBUG_IFSMgr_Get_DOSTime (PDWORD pAdditionalmSec) ;
DWORD __cdecl STATIC_IFSMgr_Get_DOSTime (PDWORD pAdditionalmSec) ;
DWORD __cdecl PAGEABLE_IFSMgr_Get_DOSTime (PDWORD pAdditionalmSec) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_SetupConnection (pioreq pir, DWORD Options, DWORD ResType) ;
VOID __cdecl INIT_IFSMgr_SetupConnection (pioreq pir, DWORD Options, DWORD ResType) ;
VOID __cdecl DEBUG_IFSMgr_SetupConnection (pioreq pir, DWORD Options, DWORD ResType) ;
VOID __cdecl STATIC_IFSMgr_SetupConnection (pioreq pir, DWORD Options, DWORD ResType) ;
VOID __cdecl PAGEABLE_IFSMgr_SetupConnection (pioreq pir, DWORD Options, DWORD ResType) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_DerefConnection (pioreq pir) ;
VOID __cdecl INIT_IFSMgr_DerefConnection (pioreq pir) ;
VOID __cdecl DEBUG_IFSMgr_DerefConnection (pioreq pir) ;
VOID __cdecl STATIC_IFSMgr_DerefConnection (pioreq pir) ;
VOID __cdecl PAGEABLE_IFSMgr_DerefConnection (pioreq pir) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_IFSMgr_ServerDOSCall (VMHANDLE hVM, DWORD function, PDPL32 pDpl32, PCLIENT_STRUCT pRegs) ;
BOOL __stdcall INIT_IFSMgr_ServerDOSCall (VMHANDLE hVM, DWORD function, PDPL32 pDpl32, PCLIENT_STRUCT pRegs) ;
BOOL __stdcall DEBUG_IFSMgr_ServerDOSCall (VMHANDLE hVM, DWORD function, PDPL32 pDpl32, PCLIENT_STRUCT pRegs) ;
BOOL __stdcall STATIC_IFSMgr_ServerDOSCall (VMHANDLE hVM, DWORD function, PDPL32 pDpl32, PCLIENT_STRUCT pRegs) ;
BOOL __stdcall PAGEABLE_IFSMgr_ServerDOSCall (VMHANDLE hVM, DWORD function, PDPL32 pDpl32, PCLIENT_STRUCT pRegs) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_CompleteAsync (pioreq pir) ;
VOID __cdecl INIT_IFSMgr_CompleteAsync (pioreq pir) ;
VOID __cdecl DEBUG_IFSMgr_CompleteAsync (pioreq pir) ;
VOID __cdecl STATIC_IFSMgr_CompleteAsync (pioreq pir) ;
VOID __cdecl PAGEABLE_IFSMgr_CompleteAsync (pioreq pir) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_RegisterHeap (pIFSFunc func, DWORD flag) ;
VOID __cdecl INIT_IFSMgr_RegisterHeap (pIFSFunc func, DWORD flag) ;
VOID __cdecl DEBUG_IFSMgr_RegisterHeap (pIFSFunc func, DWORD flag) ;
VOID __cdecl STATIC_IFSMgr_RegisterHeap (pIFSFunc func, DWORD flag) ;
VOID __cdecl PAGEABLE_IFSMgr_RegisterHeap (pIFSFunc func, DWORD flag) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK_IFSMgr_GetHeap (DWORD heapsize) ;
PVOID __cdecl INIT_IFSMgr_GetHeap (DWORD heapsize) ;
PVOID __cdecl DEBUG_IFSMgr_GetHeap (DWORD heapsize) ;
PVOID __cdecl STATIC_IFSMgr_GetHeap (DWORD heapsize) ;
PVOID __cdecl PAGEABLE_IFSMgr_GetHeap (DWORD heapsize) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_RetHeap (PVOID arg) ;
VOID __cdecl INIT_IFSMgr_RetHeap (PVOID arg) ;
VOID __cdecl DEBUG_IFSMgr_RetHeap (PVOID arg) ;
VOID __cdecl STATIC_IFSMgr_RetHeap (PVOID arg) ;
VOID __cdecl PAGEABLE_IFSMgr_RetHeap (PVOID arg) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_CheckHeap (PVOID pMemptr, PCHAR pFilename, DWORD Line) ;
INT __cdecl INIT_IFSMgr_CheckHeap (PVOID pMemptr, PCHAR pFilename, DWORD Line) ;
INT __cdecl DEBUG_IFSMgr_CheckHeap (PVOID pMemptr, PCHAR pFilename, DWORD Line) ;
INT __cdecl STATIC_IFSMgr_CheckHeap (PVOID pMemptr, PCHAR pFilename, DWORD Line) ;
INT __cdecl PAGEABLE_IFSMgr_CheckHeap (PVOID pMemptr, PCHAR pFilename, DWORD Line) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_CheckHeapItem (PVOID pMemptr, PCHAR pFilename, DWORD Line) ;
INT __cdecl INIT_IFSMgr_CheckHeapItem (PVOID pMemptr, PCHAR pFilename, DWORD Line) ;
INT __cdecl DEBUG_IFSMgr_CheckHeapItem (PVOID pMemptr, PCHAR pFilename, DWORD Line) ;
INT __cdecl STATIC_IFSMgr_CheckHeapItem (PVOID pMemptr, PCHAR pFilename, DWORD Line) ;
INT __cdecl PAGEABLE_IFSMgr_CheckHeapItem (PVOID pMemptr, PCHAR pFilename, DWORD Line) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_FillHeapSpare (void) ;
VOID __cdecl INIT_IFSMgr_FillHeapSpare (void) ;
VOID __cdecl DEBUG_IFSMgr_FillHeapSpare (void) ;
VOID __cdecl STATIC_IFSMgr_FillHeapSpare (void) ;
VOID __cdecl PAGEABLE_IFSMgr_FillHeapSpare (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_Block (DWORD key) ;
VOID __cdecl INIT_IFSMgr_Block (DWORD key) ;
VOID __cdecl DEBUG_IFSMgr_Block (DWORD key) ;
VOID __cdecl STATIC_IFSMgr_Block (DWORD key) ;
VOID __cdecl PAGEABLE_IFSMgr_Block (DWORD key) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_Wakeup (DWORD key) ;
VOID __cdecl INIT_IFSMgr_Wakeup (DWORD key) ;
VOID __cdecl DEBUG_IFSMgr_Wakeup (DWORD key) ;
VOID __cdecl STATIC_IFSMgr_Wakeup (DWORD key) ;
VOID __cdecl PAGEABLE_IFSMgr_Wakeup (DWORD key) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_Yield (void) ;
VOID __cdecl INIT_IFSMgr_Yield (void) ;
VOID __cdecl DEBUG_IFSMgr_Yield (void) ;
VOID __cdecl STATIC_IFSMgr_Yield (void) ;
VOID __cdecl PAGEABLE_IFSMgr_Yield (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_SchedEvent (pevent pev, DWORD time) ;
VOID __cdecl INIT_IFSMgr_SchedEvent (pevent pev, DWORD time) ;
VOID __cdecl DEBUG_IFSMgr_SchedEvent (pevent pev, DWORD time) ;
VOID __cdecl STATIC_IFSMgr_SchedEvent (pevent pev, DWORD time) ;
VOID __cdecl PAGEABLE_IFSMgr_SchedEvent (pevent pev, DWORD time) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_QueueEvent (pevent pev) ;
VOID __cdecl INIT_IFSMgr_QueueEvent (pevent pev) ;
VOID __cdecl DEBUG_IFSMgr_QueueEvent (pevent pev) ;
VOID __cdecl STATIC_IFSMgr_QueueEvent (pevent pev) ;
VOID __cdecl PAGEABLE_IFSMgr_QueueEvent (pevent pev) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_KillEvent (pevent pev) ;
VOID __cdecl INIT_IFSMgr_KillEvent (pevent pev) ;
VOID __cdecl DEBUG_IFSMgr_KillEvent (pevent pev) ;
VOID __cdecl STATIC_IFSMgr_KillEvent (pevent pev) ;
VOID __cdecl PAGEABLE_IFSMgr_KillEvent (pevent pev) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_FreeIOReq (pioreq pir) ;
VOID __cdecl INIT_IFSMgr_FreeIOReq (pioreq pir) ;
VOID __cdecl DEBUG_IFSMgr_FreeIOReq (pioreq pir) ;
VOID __cdecl STATIC_IFSMgr_FreeIOReq (pioreq pir) ;
VOID __cdecl PAGEABLE_IFSMgr_FreeIOReq (pioreq pir) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_MakeMailSlot (PVOID pmsfunc, path_t path, PDWORD pmshand) ;
INT __cdecl INIT_IFSMgr_MakeMailSlot (PVOID pmsfunc, path_t path, PDWORD pmshand) ;
INT __cdecl DEBUG_IFSMgr_MakeMailSlot (PVOID pmsfunc, path_t path, PDWORD pmshand) ;
INT __cdecl STATIC_IFSMgr_MakeMailSlot (PVOID pmsfunc, path_t path, PDWORD pmshand) ;
INT __cdecl PAGEABLE_IFSMgr_MakeMailSlot (PVOID pmsfunc, path_t path, PDWORD pmshand) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_DeleteMailSlot (HANDLE pHandle) ;
INT __cdecl INIT_IFSMgr_DeleteMailSlot (HANDLE pHandle) ;
INT __cdecl DEBUG_IFSMgr_DeleteMailSlot (HANDLE pHandle) ;
INT __cdecl STATIC_IFSMgr_DeleteMailSlot (HANDLE pHandle) ;
INT __cdecl PAGEABLE_IFSMgr_DeleteMailSlot (HANDLE pHandle) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_WriteMailSlot (path_t pmsname, PCHAR pData, WORD len, CHAR lana, string_t rname, PVOID xtra) ;
INT __cdecl INIT_IFSMgr_WriteMailSlot (path_t pmsname, PCHAR pData, WORD len, CHAR lana, string_t rname, PVOID xtra) ;
INT __cdecl DEBUG_IFSMgr_WriteMailSlot (path_t pmsname, PCHAR pData, WORD len, CHAR lana, string_t rname, PVOID xtra) ;
INT __cdecl STATIC_IFSMgr_WriteMailSlot (path_t pmsname, PCHAR pData, WORD len, CHAR lana, string_t rname, PVOID xtra) ;
INT __cdecl PAGEABLE_IFSMgr_WriteMailSlot (path_t pmsname, PCHAR pData, WORD len, CHAR lana, string_t rname, PVOID xtra) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_IFSMgr_PopUp (VMHANDLE hVM, DWORD mbFlags, DWORD msgnum, DWORD deviceID) ;
DWORD __cdecl INIT_IFSMgr_PopUp (VMHANDLE hVM, DWORD mbFlags, DWORD msgnum, DWORD deviceID) ;
DWORD __cdecl DEBUG_IFSMgr_PopUp (VMHANDLE hVM, DWORD mbFlags, DWORD msgnum, DWORD deviceID) ;
DWORD __cdecl STATIC_IFSMgr_PopUp (VMHANDLE hVM, DWORD mbFlags, DWORD msgnum, DWORD deviceID) ;
DWORD __cdecl PAGEABLE_IFSMgr_PopUp (VMHANDLE hVM, DWORD mbFlags, DWORD msgnum, DWORD deviceID) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_printf (PCHAR fmt) ;
VOID __cdecl INIT_IFSMgr_printf (PCHAR fmt) ;
VOID __cdecl DEBUG_IFSMgr_printf (PCHAR fmt) ;
VOID __cdecl STATIC_IFSMgr_printf (PCHAR fmt) ;
VOID __cdecl PAGEABLE_IFSMgr_printf (PCHAR fmt) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_AssertFailed (PCHAR pmsg, PCHAR pfname, DWORD line) ;
VOID __cdecl INIT_IFSMgr_AssertFailed (PCHAR pmsg, PCHAR pfname, DWORD line) ;
VOID __cdecl DEBUG_IFSMgr_AssertFailed (PCHAR pmsg, PCHAR pfname, DWORD line) ;
VOID __cdecl STATIC_IFSMgr_AssertFailed (PCHAR pmsg, PCHAR pfname, DWORD line) ;
VOID __cdecl PAGEABLE_IFSMgr_AssertFailed (PCHAR pmsg, PCHAR pfname, DWORD line) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_LogEntry (PCHAR pname, DWORD info1, DWORD info2, INT sDepth, DWORD dwMask) ;
VOID __cdecl INIT_IFSMgr_LogEntry (PCHAR pname, DWORD info1, DWORD info2, INT sDepth, DWORD dwMask) ;
VOID __cdecl DEBUG_IFSMgr_LogEntry (PCHAR pname, DWORD info1, DWORD info2, INT sDepth, DWORD dwMask) ;
VOID __cdecl STATIC_IFSMgr_LogEntry (PCHAR pname, DWORD info1, DWORD info2, INT sDepth, DWORD dwMask) ;
VOID __cdecl PAGEABLE_IFSMgr_LogEntry (PCHAR pname, DWORD info1, DWORD info2, INT sDepth, DWORD dwMask) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK_IFSMgr_DebugMenu (PVOID pm) ;
PVOID __cdecl INIT_IFSMgr_DebugMenu (PVOID pm) ;
PVOID __cdecl DEBUG_IFSMgr_DebugMenu (PVOID pm) ;
PVOID __cdecl STATIC_IFSMgr_DebugMenu (PVOID pm) ;
PVOID __cdecl PAGEABLE_IFSMgr_DebugMenu (PVOID pm) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK_IFSMgr_DebugVars (void) ;
PVOID __cdecl INIT_IFSMgr_DebugVars (void) ;
PVOID __cdecl DEBUG_IFSMgr_DebugVars (void) ;
PVOID __cdecl STATIC_IFSMgr_DebugVars (void) ;
PVOID __cdecl PAGEABLE_IFSMgr_DebugVars (void) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_GetDebugString (PCHAR prompt, PCHAR buffer, DWORD len) ;
INT __cdecl INIT_IFSMgr_GetDebugString (PCHAR prompt, PCHAR buffer, DWORD len) ;
INT __cdecl DEBUG_IFSMgr_GetDebugString (PCHAR prompt, PCHAR buffer, DWORD len) ;
INT __cdecl STATIC_IFSMgr_GetDebugString (PCHAR prompt, PCHAR buffer, DWORD len) ;
INT __cdecl PAGEABLE_IFSMgr_GetDebugString (PCHAR prompt, PCHAR buffer, DWORD len) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_IFSMgr_GetDebugHexNum (PCHAR prompt, PDWORD phexval) ;
BOOL __cdecl INIT_IFSMgr_GetDebugHexNum (PCHAR prompt, PDWORD phexval) ;
BOOL __cdecl DEBUG_IFSMgr_GetDebugHexNum (PCHAR prompt, PDWORD phexval) ;
BOOL __cdecl STATIC_IFSMgr_GetDebugHexNum (PCHAR prompt, PDWORD phexval) ;
BOOL __cdecl PAGEABLE_IFSMgr_GetDebugHexNum (PCHAR prompt, PDWORD phexval) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_NetFunction (pioreq pir, PCLIENT_STRUCT pRegs, INT proID, int flags) ;
INT __cdecl INIT_IFSMgr_NetFunction (pioreq pir, PCLIENT_STRUCT pRegs, INT proID, int flags) ;
INT __cdecl DEBUG_IFSMgr_NetFunction (pioreq pir, PCLIENT_STRUCT pRegs, INT proID, int flags) ;
INT __cdecl STATIC_IFSMgr_NetFunction (pioreq pir, PCLIENT_STRUCT pRegs, INT proID, int flags) ;
INT __cdecl PAGEABLE_IFSMgr_NetFunction (pioreq pir, PCLIENT_STRUCT pRegs, INT proID, int flags) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_DoDelAllUses (pioreq pir) ;
VOID __cdecl INIT_IFSMgr_DoDelAllUses (pioreq pir) ;
VOID __cdecl DEBUG_IFSMgr_DoDelAllUses (pioreq pir) ;
VOID __cdecl STATIC_IFSMgr_DoDelAllUses (pioreq pir) ;
VOID __cdecl PAGEABLE_IFSMgr_DoDelAllUses (pioreq pir) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_SetErrString (PCHAR errstr, DWORD errcode, DWORD table) ;
INT __cdecl INIT_IFSMgr_SetErrString (PCHAR errstr, DWORD errcode, DWORD table) ;
INT __cdecl DEBUG_IFSMgr_SetErrString (PCHAR errstr, DWORD errcode, DWORD table) ;
INT __cdecl STATIC_IFSMgr_SetErrString (PCHAR errstr, DWORD errcode, DWORD table) ;
INT __cdecl PAGEABLE_IFSMgr_SetErrString (PCHAR errstr, DWORD errcode, DWORD table) ;
#endif

#ifdef WIN40_OR_LATER
PCHAR __cdecl LOCK_IFSMgr_GetErrString (DWORD errorcode, DWORD table) ;
PCHAR __cdecl INIT_IFSMgr_GetErrString (DWORD errorcode, DWORD table) ;
PCHAR __cdecl DEBUG_IFSMgr_GetErrString (DWORD errorcode, DWORD table) ;
PCHAR __cdecl STATIC_IFSMgr_GetErrString (DWORD errorcode, DWORD table) ;
PCHAR __cdecl PAGEABLE_IFSMgr_GetErrString (DWORD errorcode, DWORD table) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK_IFSMgr_SetReqHook (DWORD Request, PIFSREQ_HANDLER pFunc, PIFSREQ_THUNK pThunk) ;
PVOID __cdecl INIT_IFSMgr_SetReqHook (DWORD Request, PIFSREQ_HANDLER pFunc, PIFSREQ_THUNK pThunk) ;
PVOID __cdecl DEBUG_IFSMgr_SetReqHook (DWORD Request, PIFSREQ_HANDLER pFunc, PIFSREQ_THUNK pThunk) ;
PVOID __cdecl STATIC_IFSMgr_SetReqHook (DWORD Request, PIFSREQ_HANDLER pFunc, PIFSREQ_THUNK pThunk) ;
PVOID __cdecl PAGEABLE_IFSMgr_SetReqHook (DWORD Request, PIFSREQ_HANDLER pFunc, PIFSREQ_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK_IFSMgr_SetPathHook (PIFSPATH_HANDLER pFunc, PIFSPATH_THUNK pThunk) ;
PVOID __cdecl INIT_IFSMgr_SetPathHook (PIFSPATH_HANDLER pFunc, PIFSPATH_THUNK pThunk) ;
PVOID __cdecl DEBUG_IFSMgr_SetPathHook (PIFSPATH_HANDLER pFunc, PIFSPATH_THUNK pThunk) ;
PVOID __cdecl STATIC_IFSMgr_SetPathHook (PIFSPATH_HANDLER pFunc, PIFSPATH_THUNK pThunk) ;
PVOID __cdecl PAGEABLE_IFSMgr_SetPathHook (PIFSPATH_HANDLER pFunc, PIFSPATH_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_UseAdd (pioreq pir, INT proId, netuse_info* pinfo) ;
INT __cdecl INIT_IFSMgr_UseAdd (pioreq pir, INT proId, netuse_info* pinfo) ;
INT __cdecl DEBUG_IFSMgr_UseAdd (pioreq pir, INT proId, netuse_info* pinfo) ;
INT __cdecl STATIC_IFSMgr_UseAdd (pioreq pir, INT proId, netuse_info* pinfo) ;
INT __cdecl PAGEABLE_IFSMgr_UseAdd (pioreq pir, INT proId, netuse_info* pinfo) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_UseDel (pioreq pir, INT proId, netuse_info* pinfo) ;
INT __cdecl INIT_IFSMgr_UseDel (pioreq pir, INT proId, netuse_info* pinfo) ;
INT __cdecl DEBUG_IFSMgr_UseDel (pioreq pir, INT proId, netuse_info* pinfo) ;
INT __cdecl STATIC_IFSMgr_UseDel (pioreq pir, INT proId, netuse_info* pinfo) ;
INT __cdecl PAGEABLE_IFSMgr_UseDel (pioreq pir, INT proId, netuse_info* pinfo) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_InitUseAdd (PUSEINFO2 ui2, INT proId, INT fStatic, INT lana) ;
INT __cdecl INIT_IFSMgr_InitUseAdd (PUSEINFO2 ui2, INT proId, INT fStatic, INT lana) ;
INT __cdecl DEBUG_IFSMgr_InitUseAdd (PUSEINFO2 ui2, INT proId, INT fStatic, INT lana) ;
INT __cdecl STATIC_IFSMgr_InitUseAdd (PUSEINFO2 ui2, INT proId, INT fStatic, INT lana) ;
INT __cdecl PAGEABLE_IFSMgr_InitUseAdd (PUSEINFO2 ui2, INT proId, INT fStatic, INT lana) ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_IFSMgr_ChangeDir () ;
UNKNOWN __cdecl INIT_IFSMgr_ChangeDir () ;
UNKNOWN __cdecl DEBUG_IFSMgr_ChangeDir () ;
UNKNOWN __cdecl STATIC_IFSMgr_ChangeDir () ;
UNKNOWN __cdecl PAGEABLE_IFSMgr_ChangeDir () ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_DelAllUses (INT proId, INT force) ;
INT __cdecl INIT_IFSMgr_DelAllUses (INT proId, INT force) ;
INT __cdecl DEBUG_IFSMgr_DelAllUses (INT proId, INT force) ;
INT __cdecl STATIC_IFSMgr_DelAllUses (INT proId, INT force) ;
INT __cdecl PAGEABLE_IFSMgr_DelAllUses (INT proId, INT force) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_CDROM_Attach (ULONG Drive, PULONG pVRP, INT fDoMount) ;
INT __cdecl INIT_IFSMgr_CDROM_Attach (ULONG Drive, PULONG pVRP, INT fDoMount) ;
INT __cdecl DEBUG_IFSMgr_CDROM_Attach (ULONG Drive, PULONG pVRP, INT fDoMount) ;
INT __cdecl STATIC_IFSMgr_CDROM_Attach (ULONG Drive, PULONG pVRP, INT fDoMount) ;
INT __cdecl PAGEABLE_IFSMgr_CDROM_Attach (ULONG Drive, PULONG pVRP, INT fDoMount) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_CDROM_Detach (ULONG Drive, ULONG VRP) ;
INT __cdecl INIT_IFSMgr_CDROM_Detach (ULONG Drive, ULONG VRP) ;
INT __cdecl DEBUG_IFSMgr_CDROM_Detach (ULONG Drive, ULONG VRP) ;
INT __cdecl STATIC_IFSMgr_CDROM_Detach (ULONG Drive, ULONG VRP) ;
INT __cdecl PAGEABLE_IFSMgr_CDROM_Detach (ULONG Drive, ULONG VRP) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_Win32DupHandle (pid_t srcPid, pid_t dstPid, PULONG pDupHandle, UCHAR flags, ULONG globalNWHandle, PULONG fReturnFlags) ;
INT __cdecl INIT_IFSMgr_Win32DupHandle (pid_t srcPid, pid_t dstPid, PULONG pDupHandle, UCHAR flags, ULONG globalNWHandle, PULONG fReturnFlags) ;
INT __cdecl DEBUG_IFSMgr_Win32DupHandle (pid_t srcPid, pid_t dstPid, PULONG pDupHandle, UCHAR flags, ULONG globalNWHandle, PULONG fReturnFlags) ;
INT __cdecl STATIC_IFSMgr_Win32DupHandle (pid_t srcPid, pid_t dstPid, PULONG pDupHandle, UCHAR flags, ULONG globalNWHandle, PULONG fReturnFlags) ;
INT __cdecl PAGEABLE_IFSMgr_Win32DupHandle (pid_t srcPid, pid_t dstPid, PULONG pDupHandle, UCHAR flags, ULONG globalNWHandle, PULONG fReturnFlags) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_IFSMgr_Ring0_FileIO (void) ;
DWORD __cdecl INIT_IFSMgr_Ring0_FileIO (void) ;
DWORD __cdecl DEBUG_IFSMgr_Ring0_FileIO (void) ;
DWORD __cdecl STATIC_IFSMgr_Ring0_FileIO (void) ;
DWORD __cdecl PAGEABLE_IFSMgr_Ring0_FileIO (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_IFSMgr_Win32_Get_Ring0_Handle (HANDLE h, HANDLE* pR0Handle, PULONG pFilePos) ;
BOOL __cdecl INIT_IFSMgr_Win32_Get_Ring0_Handle (HANDLE h, HANDLE* pR0Handle, PULONG pFilePos) ;
BOOL __cdecl DEBUG_IFSMgr_Win32_Get_Ring0_Handle (HANDLE h, HANDLE* pR0Handle, PULONG pFilePos) ;
BOOL __cdecl STATIC_IFSMgr_Win32_Get_Ring0_Handle (HANDLE h, HANDLE* pR0Handle, PULONG pFilePos) ;
BOOL __cdecl PAGEABLE_IFSMgr_Win32_Get_Ring0_Handle (HANDLE h, HANDLE* pR0Handle, PULONG pFilePos) ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_IFSMgr_Get_Drive_Info () ;
UNKNOWN __cdecl INIT_IFSMgr_Get_Drive_Info () ;
UNKNOWN __cdecl DEBUG_IFSMgr_Get_Drive_Info () ;
UNKNOWN __cdecl STATIC_IFSMgr_Get_Drive_Info () ;
UNKNOWN __cdecl PAGEABLE_IFSMgr_Get_Drive_Info () ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_IFSMgr_Ring0GetDriveInfo (DWORD DriveAis0, PDWORD pDriveInfo) ;
BOOL __stdcall INIT_IFSMgr_Ring0GetDriveInfo (DWORD DriveAis0, PDWORD pDriveInfo) ;
BOOL __stdcall DEBUG_IFSMgr_Ring0GetDriveInfo (DWORD DriveAis0, PDWORD pDriveInfo) ;
BOOL __stdcall STATIC_IFSMgr_Ring0GetDriveInfo (DWORD DriveAis0, PDWORD pDriveInfo) ;
BOOL __stdcall PAGEABLE_IFSMgr_Ring0GetDriveInfo (DWORD DriveAis0, PDWORD pDriveInfo) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_BlockNoEvents (DWORD key) ;
VOID __cdecl INIT_IFSMgr_BlockNoEvents (DWORD key) ;
VOID __cdecl DEBUG_IFSMgr_BlockNoEvents (DWORD key) ;
VOID __cdecl STATIC_IFSMgr_BlockNoEvents (DWORD key) ;
VOID __cdecl PAGEABLE_IFSMgr_BlockNoEvents (DWORD key) ;
#endif

#ifdef WIN40_OR_LATER
dos_time __cdecl LOCK_IFSMgr_NetToDosTime (DWORD NetTime) ;
dos_time __cdecl INIT_IFSMgr_NetToDosTime (DWORD NetTime) ;
dos_time __cdecl DEBUG_IFSMgr_NetToDosTime (DWORD NetTime) ;
dos_time __cdecl STATIC_IFSMgr_NetToDosTime (DWORD NetTime) ;
dos_time __cdecl PAGEABLE_IFSMgr_NetToDosTime (DWORD NetTime) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_IFSMgr_DosToNetTime (dos_time DosTime) ;
DWORD __cdecl INIT_IFSMgr_DosToNetTime (dos_time DosTime) ;
DWORD __cdecl DEBUG_IFSMgr_DosToNetTime (dos_time DosTime) ;
DWORD __cdecl STATIC_IFSMgr_DosToNetTime (dos_time DosTime) ;
DWORD __cdecl PAGEABLE_IFSMgr_DosToNetTime (dos_time DosTime) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_IFSMgr_DosToWin32Time (dos_time DosTime, _FILETIME* pFT) ;
VOID __stdcall INIT_IFSMgr_DosToWin32Time (dos_time DosTime, _FILETIME* pFT) ;
VOID __stdcall DEBUG_IFSMgr_DosToWin32Time (dos_time DosTime, _FILETIME* pFT) ;
VOID __stdcall STATIC_IFSMgr_DosToWin32Time (dos_time DosTime, _FILETIME* pFT) ;
VOID __stdcall PAGEABLE_IFSMgr_DosToWin32Time (dos_time DosTime, _FILETIME* pFT) ;
#endif

#ifdef WIN40_OR_LATER
dos_time __cdecl LOCK_IFSMgr_Win32ToDosTime (_FILETIME Win32Time_) ;
dos_time __cdecl INIT_IFSMgr_Win32ToDosTime (_FILETIME Win32Time_) ;
dos_time __cdecl DEBUG_IFSMgr_Win32ToDosTime (_FILETIME Win32Time_) ;
dos_time __cdecl STATIC_IFSMgr_Win32ToDosTime (_FILETIME Win32Time_) ;
dos_time __cdecl PAGEABLE_IFSMgr_Win32ToDosTime (_FILETIME Win32Time_) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_IFSMgr_NetToWin32Time (DWORD NetTime, _FILETIME* pFT) ;
VOID __stdcall INIT_IFSMgr_NetToWin32Time (DWORD NetTime, _FILETIME* pFT) ;
VOID __stdcall DEBUG_IFSMgr_NetToWin32Time (DWORD NetTime, _FILETIME* pFT) ;
VOID __stdcall STATIC_IFSMgr_NetToWin32Time (DWORD NetTime, _FILETIME* pFT) ;
VOID __stdcall PAGEABLE_IFSMgr_NetToWin32Time (DWORD NetTime, _FILETIME* pFT) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_IFSMgr_Win32ToNetTime (_FILETIME Win32Time) ;
DWORD __cdecl INIT_IFSMgr_Win32ToNetTime (_FILETIME Win32Time) ;
DWORD __cdecl DEBUG_IFSMgr_Win32ToNetTime (_FILETIME Win32Time) ;
DWORD __cdecl STATIC_IFSMgr_Win32ToNetTime (_FILETIME Win32Time) ;
DWORD __cdecl PAGEABLE_IFSMgr_Win32ToNetTime (_FILETIME Win32Time) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_IFSMgr_MetaMatch (string_t pUniPattern, string_t pUniName, int matchSem) ;
DWORD __cdecl INIT_IFSMgr_MetaMatch (string_t pUniPattern, string_t pUniName, int matchSem) ;
DWORD __cdecl DEBUG_IFSMgr_MetaMatch (string_t pUniPattern, string_t pUniName, int matchSem) ;
DWORD __cdecl STATIC_IFSMgr_MetaMatch (string_t pUniPattern, string_t pUniName, int matchSem) ;
DWORD __cdecl PAGEABLE_IFSMgr_MetaMatch (string_t pUniPattern, string_t pUniName, int matchSem) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_TransMatch (pioreq pir, srch_entry* pse, string_t pattern, _WIN32_FIND_DATA * pFindBuf) ;
INT __cdecl INIT_IFSMgr_TransMatch (pioreq pir, srch_entry* pse, string_t pattern, _WIN32_FIND_DATA * pFindBuf) ;
INT __cdecl DEBUG_IFSMgr_TransMatch (pioreq pir, srch_entry* pse, string_t pattern, _WIN32_FIND_DATA * pFindBuf) ;
INT __cdecl STATIC_IFSMgr_TransMatch (pioreq pir, srch_entry* pse, string_t pattern, _WIN32_FIND_DATA * pFindBuf) ;
INT __cdecl PAGEABLE_IFSMgr_TransMatch (pioreq pir, srch_entry* pse, string_t pattern, _WIN32_FIND_DATA * pFindBuf) ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_IFSMgr_CallProvider () ;
UNKNOWN __cdecl INIT_IFSMgr_CallProvider () ;
UNKNOWN __cdecl DEBUG_IFSMgr_CallProvider () ;
UNKNOWN __cdecl STATIC_IFSMgr_CallProvider () ;
UNKNOWN __cdecl PAGEABLE_IFSMgr_CallProvider () ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_UniToBCS (PUCHAR pBCSStr, string_t pUniStr, DWORD len, DWORD maxLength, INT charSet, _QWORD* pResult) ;
VOID __cdecl INIT_UniToBCS (PUCHAR pBCSStr, string_t pUniStr, DWORD len, DWORD maxLength, INT charSet, _QWORD* pResult) ;
VOID __cdecl DEBUG_UniToBCS (PUCHAR pBCSStr, string_t pUniStr, DWORD len, DWORD maxLength, INT charSet, _QWORD* pResult) ;
VOID __cdecl STATIC_UniToBCS (PUCHAR pBCSStr, string_t pUniStr, DWORD len, DWORD maxLength, INT charSet, _QWORD* pResult) ;
VOID __cdecl PAGEABLE_UniToBCS (PUCHAR pBCSStr, string_t pUniStr, DWORD len, DWORD maxLength, INT charSet, _QWORD* pResult) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_UniToBCSPath (PUCHAR pBCSPath, PathElement* pUniPath, DWORD maxLength, INT charSet, _QWORD* pResult) ;
VOID __cdecl INIT_UniToBCSPath (PUCHAR pBCSPath, PathElement* pUniPath, DWORD maxLength, INT charSet, _QWORD* pResult) ;
VOID __cdecl DEBUG_UniToBCSPath (PUCHAR pBCSPath, PathElement* pUniPath, DWORD maxLength, INT charSet, _QWORD* pResult) ;
VOID __cdecl STATIC_UniToBCSPath (PUCHAR pBCSPath, PathElement* pUniPath, DWORD maxLength, INT charSet, _QWORD* pResult) ;
VOID __cdecl PAGEABLE_UniToBCSPath (PUCHAR pBCSPath, PathElement* pUniPath, DWORD maxLength, INT charSet, _QWORD* pResult) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_BCSToUni (string_t pUniStr, PUCHAR pBCSStr, DWORD len, INT charSet, _QWORD* pResult) ;
VOID __cdecl INIT_BCSToUni (string_t pUniStr, PUCHAR pBCSStr, DWORD len, INT charSet, _QWORD* pResult) ;
VOID __cdecl DEBUG_BCSToUni (string_t pUniStr, PUCHAR pBCSStr, DWORD len, INT charSet, _QWORD* pResult) ;
VOID __cdecl STATIC_BCSToUni (string_t pUniStr, PUCHAR pBCSStr, DWORD len, INT charSet, _QWORD* pResult) ;
VOID __cdecl PAGEABLE_BCSToUni (string_t pUniStr, PUCHAR pBCSStr, DWORD len, INT charSet, _QWORD* pResult) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_UniToUpper (string_t pUniUpStr, string_t pUniStr, DWORD len) ;
INT __cdecl INIT_UniToUpper (string_t pUniUpStr, string_t pUniStr, DWORD len) ;
INT __cdecl DEBUG_UniToUpper (string_t pUniUpStr, string_t pUniStr, DWORD len) ;
INT __cdecl STATIC_UniToUpper (string_t pUniUpStr, string_t pUniStr, DWORD len) ;
INT __cdecl PAGEABLE_UniToUpper (string_t pUniUpStr, string_t pUniStr, DWORD len) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_UniCharToOEM (WORD uniChar) ;
DWORD __cdecl INIT_UniCharToOEM (WORD uniChar) ;
DWORD __cdecl DEBUG_UniCharToOEM (WORD uniChar) ;
DWORD __cdecl STATIC_UniCharToOEM (WORD uniChar) ;
DWORD __cdecl PAGEABLE_UniCharToOEM (WORD uniChar) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_CreateBasis (string_t pBasisName, string_t pUniName, DWORD len) ;
DWORD __cdecl INIT_CreateBasis (string_t pBasisName, string_t pUniName, DWORD len) ;
DWORD __cdecl DEBUG_CreateBasis (string_t pBasisName, string_t pUniName, DWORD len) ;
DWORD __cdecl STATIC_CreateBasis (string_t pBasisName, string_t pUniName, DWORD len) ;
DWORD __cdecl PAGEABLE_CreateBasis (string_t pBasisName, string_t pUniName, DWORD len) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_MatchBasisName (string_t pBasisName, string_t pName) ;
INT __cdecl INIT_MatchBasisName (string_t pBasisName, string_t pName) ;
INT __cdecl DEBUG_MatchBasisName (string_t pBasisName, string_t pName) ;
INT __cdecl STATIC_MatchBasisName (string_t pBasisName, string_t pName) ;
INT __cdecl PAGEABLE_MatchBasisName (string_t pBasisName, string_t pName) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_AppendBasisTail (string_t pBasisName, INT tail) ;
INT __cdecl INIT_AppendBasisTail (string_t pBasisName, INT tail) ;
INT __cdecl DEBUG_AppendBasisTail (string_t pBasisName, INT tail) ;
INT __cdecl STATIC_AppendBasisTail (string_t pBasisName, INT tail) ;
INT __cdecl PAGEABLE_AppendBasisTail (string_t pBasisName, INT tail) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_FcbToShort (string_t p83Name, string_t pFCBName, BOOL trailDot) ;
INT __cdecl INIT_FcbToShort (string_t p83Name, string_t pFCBName, BOOL trailDot) ;
INT __cdecl DEBUG_FcbToShort (string_t p83Name, string_t pFCBName, BOOL trailDot) ;
INT __cdecl STATIC_FcbToShort (string_t p83Name, string_t pFCBName, BOOL trailDot) ;
INT __cdecl PAGEABLE_FcbToShort (string_t p83Name, string_t pFCBName, BOOL trailDot) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_ShortToFcb (string_t pFCBName, string_t p83Name, DWORD len) ;
INT __cdecl INIT_ShortToFcb (string_t pFCBName, string_t p83Name, DWORD len) ;
INT __cdecl DEBUG_ShortToFcb (string_t pFCBName, string_t p83Name, DWORD len) ;
INT __cdecl STATIC_ShortToFcb (string_t pFCBName, string_t p83Name, DWORD len) ;
INT __cdecl PAGEABLE_ShortToFcb (string_t pFCBName, string_t p83Name, DWORD len) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_ParsePath (pioreq pir) ;
INT __cdecl INIT_IFSMgr_ParsePath (pioreq pir) ;
INT __cdecl DEBUG_IFSMgr_ParsePath (pioreq pir) ;
INT __cdecl STATIC_IFSMgr_ParsePath (pioreq pir) ;
INT __cdecl PAGEABLE_IFSMgr_ParsePath (pioreq pir) ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_Query_PhysLock () ;
UNKNOWN __cdecl INIT_Query_PhysLock () ;
UNKNOWN __cdecl DEBUG_Query_PhysLock () ;
UNKNOWN __cdecl STATIC_Query_PhysLock () ;
UNKNOWN __cdecl PAGEABLE_Query_PhysLock () ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK__VolFlush (DWORD Volume, INT Flags) ;
INT __cdecl INIT__VolFlush (DWORD Volume, INT Flags) ;
INT __cdecl DEBUG__VolFlush (DWORD Volume, INT Flags) ;
INT __cdecl STATIC__VolFlush (DWORD Volume, INT Flags) ;
INT __cdecl PAGEABLE__VolFlush (DWORD Volume, INT Flags) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_NotifyVolumeArrival (INT drive) ;
VOID __cdecl INIT_NotifyVolumeArrival (INT drive) ;
VOID __cdecl DEBUG_NotifyVolumeArrival (INT drive) ;
VOID __cdecl STATIC_NotifyVolumeArrival (INT drive) ;
VOID __cdecl PAGEABLE_NotifyVolumeArrival (INT drive) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_NotifyVolumeRemoval (INT drive) ;
VOID __cdecl INIT_NotifyVolumeRemoval (INT drive) ;
VOID __cdecl DEBUG_NotifyVolumeRemoval (INT drive) ;
VOID __cdecl STATIC_NotifyVolumeRemoval (INT drive) ;
VOID __cdecl PAGEABLE_NotifyVolumeRemoval (INT drive) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_QueryVolumeRemoval (DWORD drive, INT fDialog) ;
INT __cdecl INIT_QueryVolumeRemoval (DWORD drive, INT fDialog) ;
INT __cdecl DEBUG_QueryVolumeRemoval (DWORD drive, INT fDialog) ;
INT __cdecl STATIC_QueryVolumeRemoval (DWORD drive, INT fDialog) ;
INT __cdecl PAGEABLE_QueryVolumeRemoval (DWORD drive, INT fDialog) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_FSDUnmountCFSD (ULONG IFSResHandle) ;
INT __cdecl INIT_IFSMgr_FSDUnmountCFSD (ULONG IFSResHandle) ;
INT __cdecl DEBUG_IFSMgr_FSDUnmountCFSD (ULONG IFSResHandle) ;
INT __cdecl STATIC_IFSMgr_FSDUnmountCFSD (ULONG IFSResHandle) ;
INT __cdecl PAGEABLE_IFSMgr_FSDUnmountCFSD (ULONG IFSResHandle) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK_IFSMgr_GetConversionTablePtrs (void) ;
PVOID __cdecl INIT_IFSMgr_GetConversionTablePtrs (void) ;
PVOID __cdecl DEBUG_IFSMgr_GetConversionTablePtrs (void) ;
PVOID __cdecl STATIC_IFSMgr_GetConversionTablePtrs (void) ;
PVOID __cdecl PAGEABLE_IFSMgr_GetConversionTablePtrs (void) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_CheckAccessConflict (int disable, fmode_t* pfmode, PVOID pfn, PVOID FSDCookie) ;
INT __cdecl INIT_IFSMgr_CheckAccessConflict (int disable, fmode_t* pfmode, PVOID pfn, PVOID FSDCookie) ;
INT __cdecl DEBUG_IFSMgr_CheckAccessConflict (int disable, fmode_t* pfmode, PVOID pfn, PVOID FSDCookie) ;
INT __cdecl STATIC_IFSMgr_CheckAccessConflict (int disable, fmode_t* pfmode, PVOID pfn, PVOID FSDCookie) ;
INT __cdecl PAGEABLE_IFSMgr_CheckAccessConflict (int disable, fmode_t* pfmode, PVOID pfn, PVOID FSDCookie) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_LockFile (PPVOID ppFSDLockListHead, ULONG LockOffset, ULONG LockLength, ULONG LockOwner, PVOID pOpenFileInstance, ULONG fLockSemantics) ;
INT __cdecl INIT_IFSMgr_LockFile (PPVOID ppFSDLockListHead, ULONG LockOffset, ULONG LockLength, ULONG LockOwner, PVOID pOpenFileInstance, ULONG fLockSemantics) ;
INT __cdecl DEBUG_IFSMgr_LockFile (PPVOID ppFSDLockListHead, ULONG LockOffset, ULONG LockLength, ULONG LockOwner, PVOID pOpenFileInstance, ULONG fLockSemantics) ;
INT __cdecl STATIC_IFSMgr_LockFile (PPVOID ppFSDLockListHead, ULONG LockOffset, ULONG LockLength, ULONG LockOwner, PVOID pOpenFileInstance, ULONG fLockSemantics) ;
INT __cdecl PAGEABLE_IFSMgr_LockFile (PPVOID ppFSDLockListHead, ULONG LockOffset, ULONG LockLength, ULONG LockOwner, PVOID pOpenFileInstance, ULONG fLockSemantics) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_UnlockFile (PPVOID ppFSDLockListHead, ULONG LockOffset, ULONG LockLength, ULONG LockOwner, PVOID pOpenFileInstance, ULONG fLockSemantics) ;
INT __cdecl INIT_IFSMgr_UnlockFile (PPVOID ppFSDLockListHead, ULONG LockOffset, ULONG LockLength, ULONG LockOwner, PVOID pOpenFileInstance, ULONG fLockSemantics) ;
INT __cdecl DEBUG_IFSMgr_UnlockFile (PPVOID ppFSDLockListHead, ULONG LockOffset, ULONG LockLength, ULONG LockOwner, PVOID pOpenFileInstance, ULONG fLockSemantics) ;
INT __cdecl STATIC_IFSMgr_UnlockFile (PPVOID ppFSDLockListHead, ULONG LockOffset, ULONG LockLength, ULONG LockOwner, PVOID pOpenFileInstance, ULONG fLockSemantics) ;
INT __cdecl PAGEABLE_IFSMgr_UnlockFile (PPVOID ppFSDLockListHead, ULONG LockOffset, ULONG LockLength, ULONG LockOwner, PVOID pOpenFileInstance, ULONG fLockSemantics) ;
#endif

#ifdef WIN40_OR_LATER
PPVOID __cdecl LOCK_IFSMgr_RemoveLocks (PPVOID ppFSDLockListHead, ULONG LockOwner, PVOID pOpenFileInstance) ;
PPVOID __cdecl INIT_IFSMgr_RemoveLocks (PPVOID ppFSDLockListHead, ULONG LockOwner, PVOID pOpenFileInstance) ;
PPVOID __cdecl DEBUG_IFSMgr_RemoveLocks (PPVOID ppFSDLockListHead, ULONG LockOwner, PVOID pOpenFileInstance) ;
PPVOID __cdecl STATIC_IFSMgr_RemoveLocks (PPVOID ppFSDLockListHead, ULONG LockOwner, PVOID pOpenFileInstance) ;
PPVOID __cdecl PAGEABLE_IFSMgr_RemoveLocks (PPVOID ppFSDLockListHead, ULONG LockOwner, PVOID pOpenFileInstance) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_CheckLocks (PVOID pFSDLockListHead, ULONG fileOffset, ULONG lockLen, ULONG process, PVOID pOpenFileInstance, ULONG fOper) ;
INT __cdecl INIT_IFSMgr_CheckLocks (PVOID pFSDLockListHead, ULONG fileOffset, ULONG lockLen, ULONG process, PVOID pOpenFileInstance, ULONG fOper) ;
INT __cdecl DEBUG_IFSMgr_CheckLocks (PVOID pFSDLockListHead, ULONG fileOffset, ULONG lockLen, ULONG process, PVOID pOpenFileInstance, ULONG fOper) ;
INT __cdecl STATIC_IFSMgr_CheckLocks (PVOID pFSDLockListHead, ULONG fileOffset, ULONG lockLen, ULONG process, PVOID pOpenFileInstance, ULONG fOper) ;
INT __cdecl PAGEABLE_IFSMgr_CheckLocks (PVOID pFSDLockListHead, ULONG fileOffset, ULONG lockLen, ULONG process, PVOID pOpenFileInstance, ULONG fOper) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_CountLocks (PVOID pFSDLockListHead, PVOID pOpenFileInstance) ;
INT __cdecl INIT_IFSMgr_CountLocks (PVOID pFSDLockListHead, PVOID pOpenFileInstance) ;
INT __cdecl DEBUG_IFSMgr_CountLocks (PVOID pFSDLockListHead, PVOID pOpenFileInstance) ;
INT __cdecl STATIC_IFSMgr_CountLocks (PVOID pFSDLockListHead, PVOID pOpenFileInstance) ;
INT __cdecl PAGEABLE_IFSMgr_CountLocks (PVOID pFSDLockListHead, PVOID pOpenFileInstance) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_ReassignLockFileInst (PVOID pFSDLockListHead, PVOID pOldOpenFileInstance, PVOID pNewOpenFileInstance) ;
VOID __cdecl INIT_IFSMgr_ReassignLockFileInst (PVOID pFSDLockListHead, PVOID pOldOpenFileInstance, PVOID pNewOpenFileInstance) ;
VOID __cdecl DEBUG_IFSMgr_ReassignLockFileInst (PVOID pFSDLockListHead, PVOID pOldOpenFileInstance, PVOID pNewOpenFileInstance) ;
VOID __cdecl STATIC_IFSMgr_ReassignLockFileInst (PVOID pFSDLockListHead, PVOID pOldOpenFileInstance, PVOID pNewOpenFileInstance) ;
VOID __cdecl PAGEABLE_IFSMgr_ReassignLockFileInst (PVOID pFSDLockListHead, PVOID pOldOpenFileInstance, PVOID pNewOpenFileInstance) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IFSMgr_UnassignLockList (PVOID pFSDLockListHead) ;
VOID __cdecl INIT_IFSMgr_UnassignLockList (PVOID pFSDLockListHead) ;
VOID __cdecl DEBUG_IFSMgr_UnassignLockList (PVOID pFSDLockListHead) ;
VOID __cdecl STATIC_IFSMgr_UnassignLockList (PVOID pFSDLockListHead) ;
VOID __cdecl PAGEABLE_IFSMgr_UnassignLockList (PVOID pFSDLockListHead) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_MountChildVolume (ULONG VRP, ULONG Drive) ;
INT __cdecl INIT_IFSMgr_MountChildVolume (ULONG VRP, ULONG Drive) ;
INT __cdecl DEBUG_IFSMgr_MountChildVolume (ULONG VRP, ULONG Drive) ;
INT __cdecl STATIC_IFSMgr_MountChildVolume (ULONG VRP, ULONG Drive) ;
INT __cdecl PAGEABLE_IFSMgr_MountChildVolume (ULONG VRP, ULONG Drive) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_UnmountChildVolume (ULONG VRP, ULONG Drive) ;
INT __cdecl INIT_IFSMgr_UnmountChildVolume (ULONG VRP, ULONG Drive) ;
INT __cdecl DEBUG_IFSMgr_UnmountChildVolume (ULONG VRP, ULONG Drive) ;
INT __cdecl STATIC_IFSMgr_UnmountChildVolume (ULONG VRP, ULONG Drive) ;
INT __cdecl PAGEABLE_IFSMgr_UnmountChildVolume (ULONG VRP, ULONG Drive) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_SwapDrives (ULONG VRP, ULONG Drive1, ULONG Drive2) ;
INT __cdecl INIT_IFSMgr_SwapDrives (ULONG VRP, ULONG Drive1, ULONG Drive2) ;
INT __cdecl DEBUG_IFSMgr_SwapDrives (ULONG VRP, ULONG Drive1, ULONG Drive2) ;
INT __cdecl STATIC_IFSMgr_SwapDrives (ULONG VRP, ULONG Drive1, ULONG Drive2) ;
INT __cdecl PAGEABLE_IFSMgr_SwapDrives (ULONG VRP, ULONG Drive1, ULONG Drive2) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_FSDMapFHtoIOREQ (pioreq pir, WORD DOSHandle) ;
INT __cdecl INIT_IFSMgr_FSDMapFHtoIOREQ (pioreq pir, WORD DOSHandle) ;
INT __cdecl DEBUG_IFSMgr_FSDMapFHtoIOREQ (pioreq pir, WORD DOSHandle) ;
INT __cdecl STATIC_IFSMgr_FSDMapFHtoIOREQ (pioreq pir, WORD DOSHandle) ;
INT __cdecl PAGEABLE_IFSMgr_FSDMapFHtoIOREQ (pioreq pir, WORD DOSHandle) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_FSDParsePath (pioreq pir) ;
INT __cdecl INIT_IFSMgr_FSDParsePath (pioreq pir) ;
INT __cdecl DEBUG_IFSMgr_FSDParsePath (pioreq pir) ;
INT __cdecl STATIC_IFSMgr_FSDParsePath (pioreq pir) ;
INT __cdecl PAGEABLE_IFSMgr_FSDParsePath (pioreq pir) ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_IFSMgr_FSDAttachSFT () ;
UNKNOWN __cdecl INIT_IFSMgr_FSDAttachSFT () ;
UNKNOWN __cdecl DEBUG_IFSMgr_FSDAttachSFT () ;
UNKNOWN __cdecl STATIC_IFSMgr_FSDAttachSFT () ;
UNKNOWN __cdecl PAGEABLE_IFSMgr_FSDAttachSFT () ;
#endif

#ifdef WIN40_OR_LATER
LONG __cdecl LOCK_IFSMgr_GetTimeZoneBias (void) ;
LONG __cdecl INIT_IFSMgr_GetTimeZoneBias (void) ;
LONG __cdecl DEBUG_IFSMgr_GetTimeZoneBias (void) ;
LONG __cdecl STATIC_IFSMgr_GetTimeZoneBias (void) ;
LONG __cdecl PAGEABLE_IFSMgr_GetTimeZoneBias (void) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_PNPEvent (DWORD Message, DWORD Resource, DWORD flags) ;
INT __cdecl INIT_IFSMgr_PNPEvent (DWORD Message, DWORD Resource, DWORD flags) ;
INT __cdecl DEBUG_IFSMgr_PNPEvent (DWORD Message, DWORD Resource, DWORD flags) ;
INT __cdecl STATIC_IFSMgr_PNPEvent (DWORD Message, DWORD Resource, DWORD flags) ;
INT __cdecl PAGEABLE_IFSMgr_PNPEvent (DWORD Message, DWORD Resource, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_RegisterCFSD (pIFSFunc pFunc, DWORD FSD_IFS_Version, string_t* ppDeviceNames) ;
INT __cdecl INIT_IFSMgr_RegisterCFSD (pIFSFunc pFunc, DWORD FSD_IFS_Version, string_t* ppDeviceNames) ;
INT __cdecl DEBUG_IFSMgr_RegisterCFSD (pIFSFunc pFunc, DWORD FSD_IFS_Version, string_t* ppDeviceNames) ;
INT __cdecl STATIC_IFSMgr_RegisterCFSD (pIFSFunc pFunc, DWORD FSD_IFS_Version, string_t* ppDeviceNames) ;
INT __cdecl PAGEABLE_IFSMgr_RegisterCFSD (pIFSFunc pFunc, DWORD FSD_IFS_Version, string_t* ppDeviceNames) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_Win32MapExtendedHandleToSFT (ULONG ExtHandle, DWORD ExtPSP, PWORD pSFN) ;
INT __cdecl INIT_IFSMgr_Win32MapExtendedHandleToSFT (ULONG ExtHandle, DWORD ExtPSP, PWORD pSFN) ;
INT __cdecl DEBUG_IFSMgr_Win32MapExtendedHandleToSFT (ULONG ExtHandle, DWORD ExtPSP, PWORD pSFN) ;
INT __cdecl STATIC_IFSMgr_Win32MapExtendedHandleToSFT (ULONG ExtHandle, DWORD ExtPSP, PWORD pSFN) ;
INT __cdecl PAGEABLE_IFSMgr_Win32MapExtendedHandleToSFT (ULONG ExtHandle, DWORD ExtPSP, PWORD pSFN) ;
#endif

#ifdef WIN40_OR_LATER
ULONG __cdecl LOCK_IFSMgr_DbgSetFileHandleLimit (ULONG maxHandles) ;
ULONG __cdecl INIT_IFSMgr_DbgSetFileHandleLimit (ULONG maxHandles) ;
ULONG __cdecl DEBUG_IFSMgr_DbgSetFileHandleLimit (ULONG maxHandles) ;
ULONG __cdecl STATIC_IFSMgr_DbgSetFileHandleLimit (ULONG maxHandles) ;
ULONG __cdecl PAGEABLE_IFSMgr_DbgSetFileHandleLimit (ULONG maxHandles) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_Win32MapSFTToExtendedHandle (WORD DOSHandle, PULONG pExtHandle) ;
INT __cdecl INIT_IFSMgr_Win32MapSFTToExtendedHandle (WORD DOSHandle, PULONG pExtHandle) ;
INT __cdecl DEBUG_IFSMgr_Win32MapSFTToExtendedHandle (WORD DOSHandle, PULONG pExtHandle) ;
INT __cdecl STATIC_IFSMgr_Win32MapSFTToExtendedHandle (WORD DOSHandle, PULONG pExtHandle) ;
INT __cdecl PAGEABLE_IFSMgr_Win32MapSFTToExtendedHandle (WORD DOSHandle, PULONG pExtHandle) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_FSDGetCurrentDrive (pioreq pir) ;
INT __cdecl INIT_IFSMgr_FSDGetCurrentDrive (pioreq pir) ;
INT __cdecl DEBUG_IFSMgr_FSDGetCurrentDrive (pioreq pir) ;
INT __cdecl STATIC_IFSMgr_FSDGetCurrentDrive (pioreq pir) ;
INT __cdecl PAGEABLE_IFSMgr_FSDGetCurrentDrive (pioreq pir) ;
#endif

#ifdef WIN40_OR_LATER
ppIFSFileHookFunc __cdecl LOCK_IFSMgr_InstallFileSystemApiHook (pIFSFileHookFunc HookProc) ;
ppIFSFileHookFunc __cdecl INIT_IFSMgr_InstallFileSystemApiHook (pIFSFileHookFunc HookProc) ;
ppIFSFileHookFunc __cdecl DEBUG_IFSMgr_InstallFileSystemApiHook (pIFSFileHookFunc HookProc) ;
ppIFSFileHookFunc __cdecl STATIC_IFSMgr_InstallFileSystemApiHook (pIFSFileHookFunc HookProc) ;
ppIFSFileHookFunc __cdecl PAGEABLE_IFSMgr_InstallFileSystemApiHook (pIFSFileHookFunc HookProc) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_RemoveFileSystemApiHook (pIFSFileHookFunc HookProc) ;
INT __cdecl INIT_IFSMgr_RemoveFileSystemApiHook (pIFSFileHookFunc HookProc) ;
INT __cdecl DEBUG_IFSMgr_RemoveFileSystemApiHook (pIFSFileHookFunc HookProc) ;
INT __cdecl STATIC_IFSMgr_RemoveFileSystemApiHook (pIFSFileHookFunc HookProc) ;
INT __cdecl PAGEABLE_IFSMgr_RemoveFileSystemApiHook (pIFSFileHookFunc HookProc) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_IFSMgr_RunScheduledEvents (void) ;
BOOL __stdcall INIT_IFSMgr_RunScheduledEvents (void) ;
BOOL __stdcall DEBUG_IFSMgr_RunScheduledEvents (void) ;
BOOL __stdcall STATIC_IFSMgr_RunScheduledEvents (void) ;
BOOL __stdcall PAGEABLE_IFSMgr_RunScheduledEvents (void) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_CheckDelResource (PVOID pIFSResourceHandle, int force, int fDelFlag) ;
INT __cdecl INIT_IFSMgr_CheckDelResource (PVOID pIFSResourceHandle, int force, int fDelFlag) ;
INT __cdecl DEBUG_IFSMgr_CheckDelResource (PVOID pIFSResourceHandle, int force, int fDelFlag) ;
INT __cdecl STATIC_IFSMgr_CheckDelResource (PVOID pIFSResourceHandle, int force, int fDelFlag) ;
INT __cdecl PAGEABLE_IFSMgr_CheckDelResource (PVOID pIFSResourceHandle, int force, int fDelFlag) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_Win32GetVMCurdir (ULONG drive,PCHAR pCurdirBuf) ;
INT __cdecl INIT_IFSMgr_Win32GetVMCurdir (ULONG drive,PCHAR pCurdirBuf) ;
INT __cdecl DEBUG_IFSMgr_Win32GetVMCurdir (ULONG drive,PCHAR pCurdirBuf) ;
INT __cdecl STATIC_IFSMgr_Win32GetVMCurdir (ULONG drive,PCHAR pCurdirBuf) ;
INT __cdecl PAGEABLE_IFSMgr_Win32GetVMCurdir (ULONG drive,PCHAR pCurdirBuf) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_SetupFailedConnection (pioreq pir, PULONG pProdId) ;
INT __cdecl INIT_IFSMgr_SetupFailedConnection (pioreq pir, PULONG pProdId) ;
INT __cdecl DEBUG_IFSMgr_SetupFailedConnection (pioreq pir, PULONG pProdId) ;
INT __cdecl STATIC_IFSMgr_SetupFailedConnection (pioreq pir, PULONG pProdId) ;
INT __cdecl PAGEABLE_IFSMgr_SetupFailedConnection (pioreq pir, PULONG pProdId) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK__GetMappedErr (DWORD function, DWORD ExtErr) ;
INT __cdecl INIT__GetMappedErr (DWORD function, DWORD ExtErr) ;
INT __cdecl DEBUG__GetMappedErr (DWORD function, DWORD ExtErr) ;
INT __cdecl STATIC__GetMappedErr (DWORD function, DWORD ExtErr) ;
INT __cdecl PAGEABLE__GetMappedErr (DWORD function, DWORD ExtErr) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_ShortToLossyFcb (string_t pFCBName, string_t p83Name, DWORD theLength) ;
INT __cdecl INIT_ShortToLossyFcb (string_t pFCBName, string_t p83Name, DWORD theLength) ;
INT __cdecl DEBUG_ShortToLossyFcb (string_t pFCBName, string_t p83Name, DWORD theLength) ;
INT __cdecl STATIC_ShortToLossyFcb (string_t pFCBName, string_t p83Name, DWORD theLength) ;
INT __cdecl PAGEABLE_ShortToLossyFcb (string_t pFCBName, string_t p83Name, DWORD theLength) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_GetLockState (ULONG Drive, PULONG pLockType, PULONG pLockFlags, PULONG pLockOwner) ;
INT __cdecl INIT_IFSMgr_GetLockState (ULONG Drive, PULONG pLockType, PULONG pLockFlags, PULONG pLockOwner) ;
INT __cdecl DEBUG_IFSMgr_GetLockState (ULONG Drive, PULONG pLockType, PULONG pLockFlags, PULONG pLockOwner) ;
INT __cdecl STATIC_IFSMgr_GetLockState (ULONG Drive, PULONG pLockType, PULONG pLockFlags, PULONG pLockOwner) ;
INT __cdecl PAGEABLE_IFSMgr_GetLockState (ULONG Drive, PULONG pLockType, PULONG pLockFlags, PULONG pLockOwner) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_BcsToBcs (PUCHAR pDest, PUCHAR pSrc, int dstCharSet, int srcCharSet, ULONG MaxLen, _QWORD* pResult) ;
VOID __cdecl INIT_BcsToBcs (PUCHAR pDest, PUCHAR pSrc, int dstCharSet, int srcCharSet, ULONG MaxLen, _QWORD* pResult) ;
VOID __cdecl DEBUG_BcsToBcs (PUCHAR pDest, PUCHAR pSrc, int dstCharSet, int srcCharSet, ULONG MaxLen, _QWORD* pResult) ;
VOID __cdecl STATIC_BcsToBcs (PUCHAR pDest, PUCHAR pSrc, int dstCharSet, int srcCharSet, ULONG MaxLen, _QWORD* pResult) ;
VOID __cdecl PAGEABLE_BcsToBcs (PUCHAR pDest, PUCHAR pSrc, int dstCharSet, int srcCharSet, ULONG MaxLen, _QWORD* pResult) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_SetLoopback (PCHAR pszUNCPath, PCHAR pszLocalPath) ;
INT __cdecl INIT_IFSMgr_SetLoopback (PCHAR pszUNCPath, PCHAR pszLocalPath) ;
INT __cdecl DEBUG_IFSMgr_SetLoopback (PCHAR pszUNCPath, PCHAR pszLocalPath) ;
INT __cdecl STATIC_IFSMgr_SetLoopback (PCHAR pszUNCPath, PCHAR pszLocalPath) ;
INT __cdecl PAGEABLE_IFSMgr_SetLoopback (PCHAR pszUNCPath, PCHAR pszLocalPath) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_ClearLoopback (PCHAR pszUNCPath) ;
INT __cdecl INIT_IFSMgr_ClearLoopback (PCHAR pszUNCPath) ;
INT __cdecl DEBUG_IFSMgr_ClearLoopback (PCHAR pszUNCPath) ;
INT __cdecl STATIC_IFSMgr_ClearLoopback (PCHAR pszUNCPath) ;
INT __cdecl PAGEABLE_IFSMgr_ClearLoopback (PCHAR pszUNCPath) ;
#endif

#ifdef WIN40_OR_LATER
INT __cdecl LOCK_IFSMgr_ParseOneElement (string_t pPathElem) ;
INT __cdecl INIT_IFSMgr_ParseOneElement (string_t pPathElem) ;
INT __cdecl DEBUG_IFSMgr_ParseOneElement (string_t pPathElem) ;
INT __cdecl STATIC_IFSMgr_ParseOneElement (string_t pPathElem) ;
INT __cdecl PAGEABLE_IFSMgr_ParseOneElement (string_t pPathElem) ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_BcsToBcsUpper () ;
UNKNOWN __cdecl INIT_BcsToBcsUpper () ;
UNKNOWN __cdecl DEBUG_BcsToBcsUpper () ;
UNKNOWN __cdecl STATIC_BcsToBcsUpper () ;
UNKNOWN __cdecl PAGEABLE_BcsToBcsUpper () ;
#endif

#ifdef WIN40_OR_LATER
HANDLE __cdecl LOCK_R0_OpenCreateFile (BOOL bInContext, PCHAR filename, WORD mode, WORD createAttribs, BYTE action, BYTE flags, PWORD pError, PBYTE pAction) ;
HANDLE __cdecl INIT_R0_OpenCreateFile (BOOL bInContext, PCHAR filename, WORD mode, WORD createAttribs, BYTE action, BYTE flags, PWORD pError, PBYTE pAction) ;
HANDLE __cdecl DEBUG_R0_OpenCreateFile (BOOL bInContext, PCHAR filename, WORD mode, WORD createAttribs, BYTE action, BYTE flags, PWORD pError, PBYTE pAction) ;
HANDLE __cdecl STATIC_R0_OpenCreateFile (BOOL bInContext, PCHAR filename, WORD mode, WORD createAttribs, BYTE action, BYTE flags, PWORD pError, PBYTE pAction) ;
HANDLE __cdecl PAGEABLE_R0_OpenCreateFile (BOOL bInContext, PCHAR filename, WORD mode, WORD createAttribs, BYTE action, BYTE flags, PWORD pError, PBYTE pAction) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_R0_ReadFile (BOOL bInContext, HANDLE h, PVOID buffer, DWORD Count, DWORD FileOffset, PWORD pError) ;
DWORD __cdecl INIT_R0_ReadFile (BOOL bInContext, HANDLE h, PVOID buffer, DWORD Count, DWORD FileOffset, PWORD pError) ;
DWORD __cdecl DEBUG_R0_ReadFile (BOOL bInContext, HANDLE h, PVOID buffer, DWORD Count, DWORD FileOffset, PWORD pError) ;
DWORD __cdecl STATIC_R0_ReadFile (BOOL bInContext, HANDLE h, PVOID buffer, DWORD Count, DWORD FileOffset, PWORD pError) ;
DWORD __cdecl PAGEABLE_R0_ReadFile (BOOL bInContext, HANDLE h, PVOID buffer, DWORD Count, DWORD FileOffset, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_R0_WriteFile (BOOL bInContext, HANDLE h, PVOID buffer, DWORD Count, DWORD FileOffset, PWORD pError) ;
DWORD __cdecl INIT_R0_WriteFile (BOOL bInContext, HANDLE h, PVOID buffer, DWORD Count, DWORD FileOffset, PWORD pError) ;
DWORD __cdecl DEBUG_R0_WriteFile (BOOL bInContext, HANDLE h, PVOID buffer, DWORD Count, DWORD FileOffset, PWORD pError) ;
DWORD __cdecl STATIC_R0_WriteFile (BOOL bInContext, HANDLE h, PVOID buffer, DWORD Count, DWORD FileOffset, PWORD pError) ;
DWORD __cdecl PAGEABLE_R0_WriteFile (BOOL bInContext, HANDLE h, PVOID buffer, DWORD Count, DWORD FileOffset, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_CloseFile (HANDLE h, PWORD pError) ;
BOOL __cdecl INIT_R0_CloseFile (HANDLE h, PWORD pError) ;
BOOL __cdecl DEBUG_R0_CloseFile (HANDLE h, PWORD pError) ;
BOOL __cdecl STATIC_R0_CloseFile (HANDLE h, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_CloseFile (HANDLE h, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_R0_GetFileSize (HANDLE h, PWORD pError) ;
DWORD __cdecl INIT_R0_GetFileSize (HANDLE h, PWORD pError) ;
DWORD __cdecl DEBUG_R0_GetFileSize (HANDLE h, PWORD pError) ;
DWORD __cdecl STATIC_R0_GetFileSize (HANDLE h, PWORD pError) ;
DWORD __cdecl PAGEABLE_R0_GetFileSize (HANDLE h, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
HANDLE __cdecl LOCK_R0_FindFirstFile (PCHAR path, WORD MatchAttrs, _WIN32_FIND_DATA_BCS* pFindData, PWORD pError) ;
HANDLE __cdecl INIT_R0_FindFirstFile (PCHAR path, WORD MatchAttrs, _WIN32_FIND_DATA_BCS* pFindData, PWORD pError) ;
HANDLE __cdecl DEBUG_R0_FindFirstFile (PCHAR path, WORD MatchAttrs, _WIN32_FIND_DATA_BCS* pFindData, PWORD pError) ;
HANDLE __cdecl STATIC_R0_FindFirstFile (PCHAR path, WORD MatchAttrs, _WIN32_FIND_DATA_BCS* pFindData, PWORD pError) ;
HANDLE __cdecl PAGEABLE_R0_FindFirstFile (PCHAR path, WORD MatchAttrs, _WIN32_FIND_DATA_BCS* pFindData, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_FindNextFile (HANDLE h, _WIN32_FIND_DATA_BCS* pFindData, PWORD pError) ;
BOOL __cdecl INIT_R0_FindNextFile (HANDLE h, _WIN32_FIND_DATA_BCS* pFindData, PWORD pError) ;
BOOL __cdecl DEBUG_R0_FindNextFile (HANDLE h, _WIN32_FIND_DATA_BCS* pFindData, PWORD pError) ;
BOOL __cdecl STATIC_R0_FindNextFile (HANDLE h, _WIN32_FIND_DATA_BCS* pFindData, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_FindNextFile (HANDLE h, _WIN32_FIND_DATA_BCS* pFindData, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_FindCloseFile (HANDLE h, PWORD pError) ;
BOOL __cdecl INIT_R0_FindCloseFile (HANDLE h, PWORD pError) ;
BOOL __cdecl DEBUG_R0_FindCloseFile (HANDLE h, PWORD pError) ;
BOOL __cdecl STATIC_R0_FindCloseFile (HANDLE h, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_FindCloseFile (HANDLE h, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_GetFileAttributes (PCHAR filename, PWORD pAttribs, PWORD pError) ;
BOOL __cdecl INIT_R0_GetFileAttributes (PCHAR filename, PWORD pAttribs, PWORD pError) ;
BOOL __cdecl DEBUG_R0_GetFileAttributes (PCHAR filename, PWORD pAttribs, PWORD pError) ;
BOOL __cdecl STATIC_R0_GetFileAttributes (PCHAR filename, PWORD pAttribs, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_GetFileAttributes (PCHAR filename, PWORD pAttribs, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_SetFileAttributes (PCHAR filename, WORD attribs, PWORD pError) ;
BOOL __cdecl INIT_R0_SetFileAttributes (PCHAR filename, WORD attribs, PWORD pError) ;
BOOL __cdecl DEBUG_R0_SetFileAttributes (PCHAR filename, WORD attribs, PWORD pError) ;
BOOL __cdecl STATIC_R0_SetFileAttributes (PCHAR filename, WORD attribs, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_SetFileAttributes (PCHAR filename, WORD attribs, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_RenameFile (PCHAR OldName, PCHAR NewName, PWORD pError) ;
BOOL __cdecl INIT_R0_RenameFile (PCHAR OldName, PCHAR NewName, PWORD pError) ;
BOOL __cdecl DEBUG_R0_RenameFile (PCHAR OldName, PCHAR NewName, PWORD pError) ;
BOOL __cdecl STATIC_R0_RenameFile (PCHAR OldName, PCHAR NewName, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_RenameFile (PCHAR OldName, PCHAR NewName, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_DeleteFile (PCHAR FileName, WORD delAttribs, PWORD pError) ;
BOOL __cdecl INIT_R0_DeleteFile (PCHAR FileName, WORD delAttribs, PWORD pError) ;
BOOL __cdecl DEBUG_R0_DeleteFile (PCHAR FileName, WORD delAttribs, PWORD pError) ;
BOOL __cdecl STATIC_R0_DeleteFile (PCHAR FileName, WORD delAttribs, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_DeleteFile (PCHAR FileName, WORD delAttribs, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_LockFile (HANDLE h, DWORD ProId, DWORD RegionOffset, DWORD RegionLength, PWORD pError) ;
BOOL __cdecl INIT_R0_LockFile (HANDLE h, DWORD ProId, DWORD RegionOffset, DWORD RegionLength, PWORD pError) ;
BOOL __cdecl DEBUG_R0_LockFile (HANDLE h, DWORD ProId, DWORD RegionOffset, DWORD RegionLength, PWORD pError) ;
BOOL __cdecl STATIC_R0_LockFile (HANDLE h, DWORD ProId, DWORD RegionOffset, DWORD RegionLength, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_LockFile (HANDLE h, DWORD ProId, DWORD RegionOffset, DWORD RegionLength, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_UnlockFile (HANDLE h, DWORD ProId, DWORD RegionOffset, DWORD RegionLength, PWORD pError) ;
BOOL __cdecl INIT_R0_UnlockFile (HANDLE h, DWORD ProId, DWORD RegionOffset, DWORD RegionLength, PWORD pError) ;
BOOL __cdecl DEBUG_R0_UnlockFile (HANDLE h, DWORD ProId, DWORD RegionOffset, DWORD RegionLength, PWORD pError) ;
BOOL __cdecl STATIC_R0_UnlockFile (HANDLE h, DWORD ProId, DWORD RegionOffset, DWORD RegionLength, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_UnlockFile (HANDLE h, DWORD ProId, DWORD RegionOffset, DWORD RegionLength, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_GetDiskFreeSpace (BYTE DriveAis1, PWORD pnSectorsPerCluster, PWORD pnFreeClusters, PWORD pnBytesPerSector, PWORD pTotalClusters, PWORD pError) ;
BOOL __cdecl INIT_R0_GetDiskFreeSpace (BYTE DriveAis1, PWORD pnSectorsPerCluster, PWORD pnFreeClusters, PWORD pnBytesPerSector, PWORD pTotalClusters, PWORD pError) ;
BOOL __cdecl DEBUG_R0_GetDiskFreeSpace (BYTE DriveAis1, PWORD pnSectorsPerCluster, PWORD pnFreeClusters, PWORD pnBytesPerSector, PWORD pTotalClusters, PWORD pError) ;
BOOL __cdecl STATIC_R0_GetDiskFreeSpace (BYTE DriveAis1, PWORD pnSectorsPerCluster, PWORD pnFreeClusters, PWORD pnBytesPerSector, PWORD pTotalClusters, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_GetDiskFreeSpace (BYTE DriveAis1, PWORD pnSectorsPerCluster, PWORD pnFreeClusters, PWORD pnBytesPerSector, PWORD pTotalClusters, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_ReadAbsoluteDisk (BYTE DriveAis0, DWORD nSectors, DWORD StartSector, PVOID buffer, PWORD pError) ;
BOOL __cdecl INIT_R0_ReadAbsoluteDisk (BYTE DriveAis0, DWORD nSectors, DWORD StartSector, PVOID buffer, PWORD pError) ;
BOOL __cdecl DEBUG_R0_ReadAbsoluteDisk (BYTE DriveAis0, DWORD nSectors, DWORD StartSector, PVOID buffer, PWORD pError) ;
BOOL __cdecl STATIC_R0_ReadAbsoluteDisk (BYTE DriveAis0, DWORD nSectors, DWORD StartSector, PVOID buffer, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_ReadAbsoluteDisk (BYTE DriveAis0, DWORD nSectors, DWORD StartSector, PVOID buffer, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_R0_WriteAbsoluteDisk (BYTE DriveAis0, DWORD nSectors, DWORD StartSector, PVOID buffer, PWORD pError) ;
BOOL __cdecl INIT_R0_WriteAbsoluteDisk (BYTE DriveAis0, DWORD nSectors, DWORD StartSector, PVOID buffer, PWORD pError) ;
BOOL __cdecl DEBUG_R0_WriteAbsoluteDisk (BYTE DriveAis0, DWORD nSectors, DWORD StartSector, PVOID buffer, PWORD pError) ;
BOOL __cdecl STATIC_R0_WriteAbsoluteDisk (BYTE DriveAis0, DWORD nSectors, DWORD StartSector, PVOID buffer, PWORD pError) ;
BOOL __cdecl PAGEABLE_R0_WriteAbsoluteDisk (BYTE DriveAis0, DWORD nSectors, DWORD StartSector, PVOID buffer, PWORD pError) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_VWIN32_Get_Version (void) ;
DWORD __cdecl INIT_VWIN32_Get_Version (void) ;
DWORD __cdecl DEBUG_VWIN32_Get_Version (void) ;
DWORD __cdecl STATIC_VWIN32_Get_Version (void) ;
DWORD __cdecl PAGEABLE_VWIN32_Get_Version (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_VWIN32_DIOCCompletionRoutine (DWORD hInternal) ;
VOID __cdecl INIT_VWIN32_DIOCCompletionRoutine (DWORD hInternal) ;
VOID __cdecl DEBUG_VWIN32_DIOCCompletionRoutine (DWORD hInternal) ;
VOID __cdecl STATIC_VWIN32_DIOCCompletionRoutine (DWORD hInternal) ;
VOID __cdecl PAGEABLE_VWIN32_DIOCCompletionRoutine (DWORD hInternal) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__VWIN32_QueueUserApc (PVOID pR3Proc, DWORD Param, THREADHANDLE hThread) ;
BOOL __cdecl INIT__VWIN32_QueueUserApc (PVOID pR3Proc, DWORD Param, THREADHANDLE hThread) ;
BOOL __cdecl DEBUG__VWIN32_QueueUserApc (PVOID pR3Proc, DWORD Param, THREADHANDLE hThread) ;
BOOL __cdecl STATIC__VWIN32_QueueUserApc (PVOID pR3Proc, DWORD Param, THREADHANDLE hThread) ;
BOOL __cdecl PAGEABLE__VWIN32_QueueUserApc (PVOID pR3Proc, DWORD Param, THREADHANDLE hThread) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__VWIN32_Get_Thread_Context (PTCB ptcb, PVOID pcontext) ;
BOOL __cdecl INIT__VWIN32_Get_Thread_Context (PTCB ptcb, PVOID pcontext) ;
BOOL __cdecl DEBUG__VWIN32_Get_Thread_Context (PTCB ptcb, PVOID pcontext) ;
BOOL __cdecl STATIC__VWIN32_Get_Thread_Context (PTCB ptcb, PVOID pcontext) ;
BOOL __cdecl PAGEABLE__VWIN32_Get_Thread_Context (PTCB ptcb, PVOID pcontext) ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK__VWIN32_Emulate_Npx () ;
UNKNOWN __cdecl INIT__VWIN32_Emulate_Npx () ;
UNKNOWN __cdecl DEBUG__VWIN32_Emulate_Npx () ;
UNKNOWN __cdecl STATIC__VWIN32_Emulate_Npx () ;
UNKNOWN __cdecl PAGEABLE__VWIN32_Emulate_Npx () ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK__VWIN32_CheckDelayedNpxTrap () ;
UNKNOWN __cdecl INIT__VWIN32_CheckDelayedNpxTrap () ;
UNKNOWN __cdecl DEBUG__VWIN32_CheckDelayedNpxTrap () ;
UNKNOWN __cdecl STATIC__VWIN32_CheckDelayedNpxTrap () ;
UNKNOWN __cdecl PAGEABLE__VWIN32_CheckDelayedNpxTrap () ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VWIN32_LeaveCrstR0 (DWORD critsec) ;
VOID __stdcall INIT_VWIN32_LeaveCrstR0 (DWORD critsec) ;
VOID __stdcall DEBUG_VWIN32_LeaveCrstR0 (DWORD critsec) ;
VOID __stdcall STATIC_VWIN32_LeaveCrstR0 (DWORD critsec) ;
VOID __stdcall PAGEABLE_VWIN32_LeaveCrstR0 (DWORD critsec) ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK__VWIN32_FaultPopup () ;
UNKNOWN __cdecl INIT__VWIN32_FaultPopup () ;
UNKNOWN __cdecl DEBUG__VWIN32_FaultPopup () ;
UNKNOWN __cdecl STATIC__VWIN32_FaultPopup () ;
UNKNOWN __cdecl PAGEABLE__VWIN32_FaultPopup () ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_VWIN32_GetContextHandle () ;
UNKNOWN __cdecl INIT_VWIN32_GetContextHandle () ;
UNKNOWN __cdecl DEBUG_VWIN32_GetContextHandle () ;
UNKNOWN __cdecl STATIC_VWIN32_GetContextHandle () ;
UNKNOWN __cdecl PAGEABLE_VWIN32_GetContextHandle () ;
#endif

#ifdef WIN40_OR_LATER
HANDLE __cdecl LOCK_VWIN32_GetCurrentProcessHandle (void) ;
HANDLE __cdecl INIT_VWIN32_GetCurrentProcessHandle (void) ;
HANDLE __cdecl DEBUG_VWIN32_GetCurrentProcessHandle (void) ;
HANDLE __cdecl STATIC_VWIN32_GetCurrentProcessHandle (void) ;
HANDLE __cdecl PAGEABLE_VWIN32_GetCurrentProcessHandle (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__VWIN32_SetWin32Event (HANDLE hEvent) ;
BOOL __cdecl INIT__VWIN32_SetWin32Event (HANDLE hEvent) ;
BOOL __cdecl DEBUG__VWIN32_SetWin32Event (HANDLE hEvent) ;
BOOL __cdecl STATIC__VWIN32_SetWin32Event (HANDLE hEvent) ;
BOOL __cdecl PAGEABLE__VWIN32_SetWin32Event (HANDLE hEvent) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__VWIN32_PulseWin32Event (HANDLE hEvent) ;
BOOL __cdecl INIT__VWIN32_PulseWin32Event (HANDLE hEvent) ;
BOOL __cdecl DEBUG__VWIN32_PulseWin32Event (HANDLE hEvent) ;
BOOL __cdecl STATIC__VWIN32_PulseWin32Event (HANDLE hEvent) ;
BOOL __cdecl PAGEABLE__VWIN32_PulseWin32Event (HANDLE hEvent) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__VWIN32_ResetWin32Event (HANDLE hEvent) ;
BOOL __cdecl INIT__VWIN32_ResetWin32Event (HANDLE hEvent) ;
BOOL __cdecl DEBUG__VWIN32_ResetWin32Event (HANDLE hEvent) ;
BOOL __cdecl STATIC__VWIN32_ResetWin32Event (HANDLE hEvent) ;
BOOL __cdecl PAGEABLE__VWIN32_ResetWin32Event (HANDLE hEvent) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VWIN32_WaitSingleObject (HANDLE hEvent, DWORD TimeOut, BOOL bAlertable) ;
DWORD __cdecl INIT__VWIN32_WaitSingleObject (HANDLE hEvent, DWORD TimeOut, BOOL bAlertable) ;
DWORD __cdecl DEBUG__VWIN32_WaitSingleObject (HANDLE hEvent, DWORD TimeOut, BOOL bAlertable) ;
DWORD __cdecl STATIC__VWIN32_WaitSingleObject (HANDLE hEvent, DWORD TimeOut, BOOL bAlertable) ;
DWORD __cdecl PAGEABLE__VWIN32_WaitSingleObject (HANDLE hEvent, DWORD TimeOut, BOOL bAlertable) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VWIN32_WaitMultipleObjects (DWORD nEvents, PHANDLE Events, DWORD TimeOut, BYTE Flags, BOOL bWaitAPC) ;
DWORD __cdecl INIT__VWIN32_WaitMultipleObjects (DWORD nEvents, PHANDLE Events, DWORD TimeOut, BYTE Flags, BOOL bWaitAPC) ;
DWORD __cdecl DEBUG__VWIN32_WaitMultipleObjects (DWORD nEvents, PHANDLE Events, DWORD TimeOut, BYTE Flags, BOOL bWaitAPC) ;
DWORD __cdecl STATIC__VWIN32_WaitMultipleObjects (DWORD nEvents, PHANDLE Events, DWORD TimeOut, BYTE Flags, BOOL bWaitAPC) ;
DWORD __cdecl PAGEABLE__VWIN32_WaitMultipleObjects (DWORD nEvents, PHANDLE Events, DWORD TimeOut, BYTE Flags, BOOL bWaitAPC) ;
#endif

#ifdef WIN40_OR_LATER
THREADHANDLE __stdcall LOCK__VWIN32_CreateRing0Thread (DWORD dwStkSiz, DWORD fArg,PVOIDFUNCOFDW startFunction, PVOIDFUNCOFDW failCallback, PHANDLE pRing3Handle) ;
THREADHANDLE __stdcall INIT__VWIN32_CreateRing0Thread (DWORD dwStkSiz, DWORD fArg,PVOIDFUNCOFDW startFunction, PVOIDFUNCOFDW failCallback, PHANDLE pRing3Handle) ;
THREADHANDLE __stdcall DEBUG__VWIN32_CreateRing0Thread (DWORD dwStkSiz, DWORD fArg,PVOIDFUNCOFDW startFunction, PVOIDFUNCOFDW failCallback, PHANDLE pRing3Handle) ;
THREADHANDLE __stdcall STATIC__VWIN32_CreateRing0Thread (DWORD dwStkSiz, DWORD fArg,PVOIDFUNCOFDW startFunction, PVOIDFUNCOFDW failCallback, PHANDLE pRing3Handle) ;
THREADHANDLE __stdcall PAGEABLE__VWIN32_CreateRing0Thread (DWORD dwStkSiz, DWORD fArg,PVOIDFUNCOFDW startFunction, PVOIDFUNCOFDW failCallback, PHANDLE pRing3Handle) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__VWIN32_CloseVxDHandle (HANDLE hVXD) ;
BOOL __cdecl INIT__VWIN32_CloseVxDHandle (HANDLE hVXD) ;
BOOL __cdecl DEBUG__VWIN32_CloseVxDHandle (HANDLE hVXD) ;
BOOL __cdecl STATIC__VWIN32_CloseVxDHandle (HANDLE hVXD) ;
BOOL __cdecl PAGEABLE__VWIN32_CloseVxDHandle (HANDLE hVXD) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VWIN32_ActiveTimeBiasSet (void) ;
BOOL __stdcall INIT_VWIN32_ActiveTimeBiasSet (void) ;
BOOL __stdcall DEBUG_VWIN32_ActiveTimeBiasSet (void) ;
BOOL __stdcall STATIC_VWIN32_ActiveTimeBiasSet (void) ;
BOOL __stdcall PAGEABLE_VWIN32_ActiveTimeBiasSet (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VWIN32_EnterCrstR0 (DWORD critsec) ;
DWORD __stdcall INIT_VWIN32_EnterCrstR0 (DWORD critsec) ;
DWORD __stdcall DEBUG_VWIN32_EnterCrstR0 (DWORD critsec) ;
DWORD __stdcall STATIC_VWIN32_EnterCrstR0 (DWORD critsec) ;
DWORD __stdcall PAGEABLE_VWIN32_EnterCrstR0 (DWORD critsec) ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_VWIN32_BlueScreenPopup () ;
UNKNOWN __cdecl INIT_VWIN32_BlueScreenPopup () ;
UNKNOWN __cdecl DEBUG_VWIN32_BlueScreenPopup () ;
UNKNOWN __cdecl STATIC_VWIN32_BlueScreenPopup () ;
UNKNOWN __cdecl PAGEABLE_VWIN32_BlueScreenPopup () ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VWIN32_TerminateApp (HANDLE hProc) ;
VOID __stdcall INIT_VWIN32_TerminateApp (HANDLE hProc) ;
VOID __stdcall DEBUG_VWIN32_TerminateApp (HANDLE hProc) ;
VOID __stdcall STATIC_VWIN32_TerminateApp (HANDLE hProc) ;
VOID __stdcall PAGEABLE_VWIN32_TerminateApp (HANDLE hProc) ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK__VWIN32_QueueKernelAPC () ;
UNKNOWN __cdecl INIT__VWIN32_QueueKernelAPC () ;
UNKNOWN __cdecl DEBUG__VWIN32_QueueKernelAPC () ;
UNKNOWN __cdecl STATIC__VWIN32_QueueKernelAPC () ;
UNKNOWN __cdecl PAGEABLE__VWIN32_QueueKernelAPC () ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VWIN32_SysErrorBox (PVSEB pVseb) ;
DWORD __stdcall INIT_VWIN32_SysErrorBox (PVSEB pVseb) ;
DWORD __stdcall DEBUG_VWIN32_SysErrorBox (PVSEB pVseb) ;
DWORD __stdcall STATIC_VWIN32_SysErrorBox (PVSEB pVseb) ;
DWORD __stdcall PAGEABLE_VWIN32_SysErrorBox (PVSEB pVseb) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__VWIN32_IsClientWin32 (void) ;
BOOL __cdecl INIT__VWIN32_IsClientWin32 (void) ;
BOOL __cdecl DEBUG__VWIN32_IsClientWin32 (void) ;
BOOL __cdecl STATIC__VWIN32_IsClientWin32 (void) ;
BOOL __cdecl PAGEABLE__VWIN32_IsClientWin32 (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__VWIN32_Set_Thread_Context (PTCB ptcb, PVOID pcontext) ;
BOOL __cdecl INIT__VWIN32_Set_Thread_Context (PTCB ptcb, PVOID pcontext) ;
BOOL __cdecl DEBUG__VWIN32_Set_Thread_Context (PTCB ptcb, PVOID pcontext) ;
BOOL __cdecl STATIC__VWIN32_Set_Thread_Context (PTCB ptcb, PVOID pcontext) ;
BOOL __cdecl PAGEABLE__VWIN32_Set_Thread_Context (PTCB ptcb, PVOID pcontext) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__VWIN32_CopyMem (PVOID src, PVOID dest, DWORD count, PDWORD pNmoved) ;
BOOL __cdecl INIT__VWIN32_CopyMem (PVOID src, PVOID dest, DWORD count, PDWORD pNmoved) ;
BOOL __cdecl DEBUG__VWIN32_CopyMem (PVOID src, PVOID dest, DWORD count, PDWORD pNmoved) ;
BOOL __cdecl STATIC__VWIN32_CopyMem (PVOID src, PVOID dest, DWORD count, PDWORD pNmoved) ;
BOOL __cdecl PAGEABLE__VWIN32_CopyMem (PVOID src, PVOID dest, DWORD count, PDWORD pNmoved) ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK__VWIN32_Npx_Exception (void) ;
UNKNOWN __cdecl INIT__VWIN32_Npx_Exception (void) ;
UNKNOWN __cdecl DEBUG__VWIN32_Npx_Exception (void) ;
UNKNOWN __cdecl STATIC__VWIN32_Npx_Exception (void) ;
UNKNOWN __cdecl PAGEABLE__VWIN32_Npx_Exception (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_PCCARD_Get_Version (void) ;
DWORD __cdecl INIT_PCCARD_Get_Version (void) ;
DWORD __cdecl DEBUG_PCCARD_Get_Version (void) ;
DWORD __cdecl STATIC_PCCARD_Get_Version (void) ;
DWORD __cdecl PAGEABLE_PCCARD_Get_Version (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_PCCARD_Card_Services (BYTE func, WORD handle, PVOID pointer, WORD argLen, PVOID pArg, PWORD pStatus, PWORD pReturnHandle) ;
BOOL __stdcall INIT_PCCARD_Card_Services (BYTE func, WORD handle, PVOID pointer, WORD argLen, PVOID pArg, PWORD pStatus, PWORD pReturnHandle) ;
BOOL __stdcall DEBUG_PCCARD_Card_Services (BYTE func, WORD handle, PVOID pointer, WORD argLen, PVOID pArg, PWORD pStatus, PWORD pReturnHandle) ;
BOOL __stdcall STATIC_PCCARD_Card_Services (BYTE func, WORD handle, PVOID pointer, WORD argLen, PVOID pArg, PWORD pStatus, PWORD pReturnHandle) ;
BOOL __stdcall PAGEABLE_PCCARD_Card_Services (BYTE func, WORD handle, PVOID pointer, WORD argLen, PVOID pArg, PWORD pStatus, PWORD pReturnHandle) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_IOS_Get_Version (void) ;
DWORD __cdecl INIT_IOS_Get_Version (void) ;
DWORD __cdecl DEBUG_IOS_Get_Version (void) ;
DWORD __cdecl STATIC_IOS_Get_Version (void) ;
DWORD __cdecl PAGEABLE_IOS_Get_Version (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_IOS_BD_Register_Device (PBDD pBDD) ;
BOOL __stdcall INIT_IOS_BD_Register_Device (PBDD pBDD) ;
BOOL __stdcall DEBUG_IOS_BD_Register_Device (PBDD pBDD) ;
BOOL __stdcall STATIC_IOS_BD_Register_Device (PBDD pBDD) ;
BOOL __stdcall PAGEABLE_IOS_BD_Register_Device (PBDD pBDD) ;
#endif

#ifdef WIN40_OR_LATER
PBDD __stdcall LOCK_IOS_Find_Int13_Drive (BYTE unit) ;
PBDD __stdcall INIT_IOS_Find_Int13_Drive (BYTE unit) ;
PBDD __stdcall DEBUG_IOS_Find_Int13_Drive (BYTE unit) ;
PBDD __stdcall STATIC_IOS_Find_Int13_Drive (BYTE unit) ;
PBDD __stdcall PAGEABLE_IOS_Find_Int13_Drive (BYTE unit) ;
#endif

#ifdef WIN40_OR_LATER
PBDD __stdcall LOCK_IOS_Get_Device_List (void) ;
PBDD __stdcall INIT_IOS_Get_Device_List (void) ;
PBDD __stdcall DEBUG_IOS_Get_Device_List (void) ;
PBDD __stdcall STATIC_IOS_Get_Device_List (void) ;
PBDD __stdcall PAGEABLE_IOS_Get_Device_List (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_IOS_SendCommand (PVOID BCBorIOR, PVOID BDDorDCB) ;
VOID __stdcall INIT_IOS_SendCommand (PVOID BCBorIOR, PVOID BDDorDCB) ;
VOID __stdcall DEBUG_IOS_SendCommand (PVOID BCBorIOR, PVOID BDDorDCB) ;
VOID __stdcall STATIC_IOS_SendCommand (PVOID BCBorIOR, PVOID BDDorDCB) ;
VOID __stdcall PAGEABLE_IOS_SendCommand (PVOID BCBorIOR, PVOID BDDorDCB) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IOS_Register (PDRP pDrp) ;
VOID __cdecl INIT_IOS_Register (PDRP pDrp) ;
VOID __cdecl DEBUG_IOS_Register (PDRP pDrp) ;
VOID __cdecl STATIC_IOS_Register (PDRP pDrp) ;
VOID __cdecl PAGEABLE_IOS_Register (PDRP pDrp) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_IOS_Requestor_Service (PIRS pIrs) ;
VOID __cdecl INIT_IOS_Requestor_Service (PIRS pIrs) ;
VOID __cdecl DEBUG_IOS_Requestor_Service (PIRS pIrs) ;
VOID __cdecl STATIC_IOS_Requestor_Service (PIRS pIrs) ;
VOID __cdecl PAGEABLE_IOS_Requestor_Service (PIRS pIrs) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_IOS_Exclusive_Access (BYTE driveNum, BYTE driveType, HANDLE hVM, BOOL bLockIt) ;
BOOL __stdcall INIT_IOS_Exclusive_Access (BYTE driveNum, BYTE driveType, HANDLE hVM, BOOL bLockIt) ;
BOOL __stdcall DEBUG_IOS_Exclusive_Access (BYTE driveNum, BYTE driveType, HANDLE hVM, BOOL bLockIt) ;
BOOL __stdcall STATIC_IOS_Exclusive_Access (BYTE driveNum, BYTE driveType, HANDLE hVM, BOOL bLockIt) ;
BOOL __stdcall PAGEABLE_IOS_Exclusive_Access (BYTE driveNum, BYTE driveType, HANDLE hVM, BOOL bLockIt) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_IOS_Send_Next_Command (PDCB_COMMON pDcb) ;
VOID __stdcall INIT_IOS_Send_Next_Command (PDCB_COMMON pDcb) ;
VOID __stdcall DEBUG_IOS_Send_Next_Command (PDCB_COMMON pDcb) ;
VOID __stdcall STATIC_IOS_Send_Next_Command (PDCB_COMMON pDcb) ;
VOID __stdcall PAGEABLE_IOS_Send_Next_Command (PDCB_COMMON pDcb) ;
#endif

#ifdef WIN40_OR_LATER
TIMEOUTHANDLE __stdcall LOCK_IOS_Set_Async_Time_Out (DWORD Time, CONST VOID * Refdata, PASYNCTIMEOUT_HANDLER TimeoutCallback, PASYNCTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall INIT_IOS_Set_Async_Time_Out (DWORD Time, CONST VOID * Refdata, PASYNCTIMEOUT_HANDLER TimeoutCallback, PASYNCTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall DEBUG_IOS_Set_Async_Time_Out (DWORD Time, CONST VOID * Refdata, PASYNCTIMEOUT_HANDLER TimeoutCallback, PASYNCTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall STATIC_IOS_Set_Async_Time_Out (DWORD Time, CONST VOID * Refdata, PASYNCTIMEOUT_HANDLER TimeoutCallback, PASYNCTIMEOUT_THUNK pThunk) ;
TIMEOUTHANDLE __stdcall PAGEABLE_IOS_Set_Async_Time_Out (DWORD Time, CONST VOID * Refdata, PASYNCTIMEOUT_HANDLER TimeoutCallback, PASYNCTIMEOUT_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_IOS_Signal_Semaphore_No_Switch (SEMHANDLE sem) ;
VOID __stdcall INIT_IOS_Signal_Semaphore_No_Switch (SEMHANDLE sem) ;
VOID __stdcall DEBUG_IOS_Signal_Semaphore_No_Switch (SEMHANDLE sem) ;
VOID __stdcall STATIC_IOS_Signal_Semaphore_No_Switch (SEMHANDLE sem) ;
VOID __stdcall PAGEABLE_IOS_Signal_Semaphore_No_Switch (SEMHANDLE sem) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_IOSIdleStatus (void) ;
BOOL __cdecl INIT_IOSIdleStatus (void) ;
BOOL __cdecl DEBUG_IOSIdleStatus (void) ;
BOOL __cdecl STATIC_IOSIdleStatus (void) ;
BOOL __cdecl PAGEABLE_IOSIdleStatus (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_IOSMapIORSToI24 (DWORD IorErrorCode, DWORD IorFunction) ;
DWORD __stdcall INIT_IOSMapIORSToI24 (DWORD IorErrorCode, DWORD IorFunction) ;
DWORD __stdcall DEBUG_IOSMapIORSToI24 (DWORD IorErrorCode, DWORD IorFunction) ;
DWORD __stdcall STATIC_IOSMapIORSToI24 (DWORD IorErrorCode, DWORD IorFunction) ;
DWORD __stdcall PAGEABLE_IOSMapIORSToI24 (DWORD IorErrorCode, DWORD IorFunction) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_IOSMapIORSToI21 (DWORD IorErrorCode) ;
DWORD __stdcall INIT_IOSMapIORSToI21 (DWORD IorErrorCode) ;
DWORD __stdcall DEBUG_IOSMapIORSToI21 (DWORD IorErrorCode) ;
DWORD __stdcall STATIC_IOSMapIORSToI21 (DWORD IorErrorCode) ;
DWORD __stdcall PAGEABLE_IOSMapIORSToI21 (DWORD IorErrorCode) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_IOS_BD_Command_Complete (PIOR pIor) ;
VOID __stdcall INIT_IOS_BD_Command_Complete (PIOR pIor) ;
VOID __stdcall DEBUG_IOS_BD_Command_Complete (PIOR pIor) ;
VOID __stdcall STATIC_IOS_BD_Command_Complete (PIOR pIor) ;
VOID __stdcall PAGEABLE_IOS_BD_Command_Complete (PIOR pIor) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_MMDEVLDR_Register_Device_Driver (DEVNODE dnDevNode, PVOID fnConfigHandler, DWORD dwUserData) ;
VOID __cdecl INIT_MMDEVLDR_Register_Device_Driver (DEVNODE dnDevNode, PVOID fnConfigHandler, DWORD dwUserData) ;
VOID __cdecl DEBUG_MMDEVLDR_Register_Device_Driver (DEVNODE dnDevNode, PVOID fnConfigHandler, DWORD dwUserData) ;
VOID __cdecl STATIC_MMDEVLDR_Register_Device_Driver (DEVNODE dnDevNode, PVOID fnConfigHandler, DWORD dwUserData) ;
VOID __cdecl PAGEABLE_MMDEVLDR_Register_Device_Driver (DEVNODE dnDevNode, PVOID fnConfigHandler, DWORD dwUserData) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__MMDEVLDR_SetDevicePresence (DEVNODE dn, PCHAR pszReg, BOOL fPresent) ;
VOID __cdecl INIT__MMDEVLDR_SetDevicePresence (DEVNODE dn, PCHAR pszReg, BOOL fPresent) ;
VOID __cdecl DEBUG__MMDEVLDR_SetDevicePresence (DEVNODE dn, PCHAR pszReg, BOOL fPresent) ;
VOID __cdecl STATIC__MMDEVLDR_SetDevicePresence (DEVNODE dn, PCHAR pszReg, BOOL fPresent) ;
VOID __cdecl PAGEABLE__MMDEVLDR_SetDevicePresence (DEVNODE dn, PCHAR pszReg, BOOL fPresent) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__MMDEVLDR_SetEnvironmentString (PCHAR pszName,	PCHAR pszValue) ;
VOID __cdecl INIT__MMDEVLDR_SetEnvironmentString (PCHAR pszName,	PCHAR pszValue) ;
VOID __cdecl DEBUG__MMDEVLDR_SetEnvironmentString (PCHAR pszName,	PCHAR pszValue) ;
VOID __cdecl STATIC__MMDEVLDR_SetEnvironmentString (PCHAR pszName,	PCHAR pszValue) ;
VOID __cdecl PAGEABLE__MMDEVLDR_SetEnvironmentString (PCHAR pszName,	PCHAR pszValue) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__MMDEVLDR_GetEnvironmentString (PCHAR pszName, PCHAR pszValue, UINT uBufSize) ;
BOOL __cdecl INIT__MMDEVLDR_GetEnvironmentString (PCHAR pszName, PCHAR pszValue, UINT uBufSize) ;
BOOL __cdecl DEBUG__MMDEVLDR_GetEnvironmentString (PCHAR pszName, PCHAR pszValue, UINT uBufSize) ;
BOOL __cdecl STATIC__MMDEVLDR_GetEnvironmentString (PCHAR pszName, PCHAR pszValue, UINT uBufSize) ;
BOOL __cdecl PAGEABLE__MMDEVLDR_GetEnvironmentString (PCHAR pszName, PCHAR pszValue, UINT uBufSize) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__MMDEVLDR_RemoveEnvironmentString (PCHAR pszName) ;
VOID __cdecl INIT__MMDEVLDR_RemoveEnvironmentString (PCHAR pszName) ;
VOID __cdecl DEBUG__MMDEVLDR_RemoveEnvironmentString (PCHAR pszName) ;
VOID __cdecl STATIC__MMDEVLDR_RemoveEnvironmentString (PCHAR pszName) ;
VOID __cdecl PAGEABLE__MMDEVLDR_RemoveEnvironmentString (PCHAR pszName) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__MMDEVLDR_AddEnvironmentString (PCHAR pszName, PCHAR pszValue) ;
VOID __cdecl INIT__MMDEVLDR_AddEnvironmentString (PCHAR pszName, PCHAR pszValue) ;
VOID __cdecl DEBUG__MMDEVLDR_AddEnvironmentString (PCHAR pszName, PCHAR pszValue) ;
VOID __cdecl STATIC__MMDEVLDR_AddEnvironmentString (PCHAR pszName, PCHAR pszValue) ;
VOID __cdecl PAGEABLE__MMDEVLDR_AddEnvironmentString (PCHAR pszName, PCHAR pszValue) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VPICD_Get_Virtualization_Count (DWORD irq0toF) ;
DWORD __stdcall INIT_VPICD_Get_Virtualization_Count (DWORD irq0toF) ;
DWORD __stdcall DEBUG_VPICD_Get_Virtualization_Count (DWORD irq0toF) ;
DWORD __stdcall STATIC_VPICD_Get_Virtualization_Count (DWORD irq0toF) ;
DWORD __stdcall PAGEABLE_VPICD_Get_Virtualization_Count (DWORD irq0toF) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDMAD_Phys_Mask_Channel (DMAHANDLE hDMA) ;
VOID __stdcall INIT_VDMAD_Phys_Mask_Channel (DMAHANDLE hDMA) ;
VOID __stdcall DEBUG_VDMAD_Phys_Mask_Channel (DMAHANDLE hDMA) ;
VOID __stdcall STATIC_VDMAD_Phys_Mask_Channel (DMAHANDLE hDMA) ;
VOID __stdcall PAGEABLE_VDMAD_Phys_Mask_Channel (DMAHANDLE hDMA) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDMAD_Phys_Unmask_Channel (DMAHANDLE hDMA, VMHANDLE hVM) ;
VOID __stdcall INIT_VDMAD_Phys_Unmask_Channel (DMAHANDLE hDMA, VMHANDLE hVM) ;
VOID __stdcall DEBUG_VDMAD_Phys_Unmask_Channel (DMAHANDLE hDMA, VMHANDLE hVM) ;
VOID __stdcall STATIC_VDMAD_Phys_Unmask_Channel (DMAHANDLE hDMA, VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VDMAD_Phys_Unmask_Channel (DMAHANDLE hDMA, VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VDMAD_Unvirtualize_Channel (DMAHANDLE hDMA) ;
BOOL __stdcall INIT_VDMAD_Unvirtualize_Channel (DMAHANDLE hDMA) ;
BOOL __stdcall DEBUG_VDMAD_Unvirtualize_Channel (DMAHANDLE hDMA) ;
BOOL __stdcall STATIC_VDMAD_Unvirtualize_Channel (DMAHANDLE hDMA) ;
BOOL __stdcall PAGEABLE_VDMAD_Unvirtualize_Channel (DMAHANDLE hDMA) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDMAD_Set_IO_Address (DWORD Channel, DWORD Port) ;
VOID __stdcall INIT_VDMAD_Set_IO_Address (DWORD Channel, DWORD Port) ;
VOID __stdcall DEBUG_VDMAD_Set_IO_Address (DWORD Channel, DWORD Port) ;
VOID __stdcall STATIC_VDMAD_Set_IO_Address (DWORD Channel, DWORD Port) ;
VOID __stdcall PAGEABLE_VDMAD_Set_IO_Address (DWORD Channel, DWORD Port) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_Debug_Convert_Hex_Binary (BYTE val) ;
DWORD __stdcall INIT_Debug_Convert_Hex_Binary (BYTE val) ;
DWORD __stdcall DEBUG_Debug_Convert_Hex_Binary (BYTE val) ;
DWORD __stdcall STATIC_Debug_Convert_Hex_Binary (BYTE val) ;
DWORD __stdcall PAGEABLE_Debug_Convert_Hex_Binary (BYTE val) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_Debug_Convert_Hex_Decimal (BYTE val) ;
DWORD __stdcall INIT_Debug_Convert_Hex_Decimal (BYTE val) ;
DWORD __stdcall DEBUG_Debug_Convert_Hex_Decimal (BYTE val) ;
DWORD __stdcall STATIC_Debug_Convert_Hex_Decimal (BYTE val) ;
DWORD __stdcall PAGEABLE_Debug_Convert_Hex_Decimal (BYTE val) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK__Debug_Flags_Service (DWORD assertions) ;
VOID __stdcall INIT__Debug_Flags_Service (DWORD assertions) ;
VOID __stdcall DEBUG__Debug_Flags_Service (DWORD assertions) ;
VOID __stdcall STATIC__Debug_Flags_Service (DWORD assertions) ;
VOID __stdcall PAGEABLE__Debug_Flags_Service (DWORD assertions) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Debug_Test_Cur_VM (VMHANDLE hVM) ;
VOID __stdcall INIT_Debug_Test_Cur_VM (VMHANDLE hVM) ;
VOID __stdcall DEBUG_Debug_Test_Cur_VM (VMHANDLE hVM) ;
VOID __stdcall STATIC_Debug_Test_Cur_VM (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_Debug_Test_Cur_VM (VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_Debug_Test_Valid_Handle (VMHANDLE hVM) ;
VOID __stdcall INIT_Debug_Test_Valid_Handle (VMHANDLE hVM) ;
VOID __stdcall DEBUG_Debug_Test_Valid_Handle (VMHANDLE hVM) ;
VOID __stdcall STATIC_Debug_Test_Valid_Handle (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_Debug_Test_Valid_Handle (VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VCache_FindBlock (BYTE FsdID, BYTE Opt, DWORD key1, DWORD key2, PHANDLE phCacheBlock, PBOOL pbBufferIsLocked, PPVOID pBufferAddress) ;
BOOL __stdcall INIT_VCache_FindBlock (BYTE FsdID, BYTE Opt, DWORD key1, DWORD key2, PHANDLE phCacheBlock, PBOOL pbBufferIsLocked, PPVOID pBufferAddress) ;
BOOL __stdcall DEBUG_VCache_FindBlock (BYTE FsdID, BYTE Opt, DWORD key1, DWORD key2, PHANDLE phCacheBlock, PBOOL pbBufferIsLocked, PPVOID pBufferAddress) ;
BOOL __stdcall STATIC_VCache_FindBlock (BYTE FsdID, BYTE Opt, DWORD key1, DWORD key2, PHANDLE phCacheBlock, PBOOL pbBufferIsLocked, PPVOID pBufferAddress) ;
BOOL __stdcall PAGEABLE_VCache_FindBlock (BYTE FsdID, BYTE Opt, DWORD key1, DWORD key2, PHANDLE phCacheBlock, PBOOL pbBufferIsLocked, PPVOID pBufferAddress) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VCache_FreeBlock (HANDLE hCacheBlock) ;
VOID __stdcall INIT_VCache_FreeBlock (HANDLE hCacheBlock) ;
VOID __stdcall DEBUG_VCache_FreeBlock (HANDLE hCacheBlock) ;
VOID __stdcall STATIC_VCache_FreeBlock (HANDLE hCacheBlock) ;
VOID __stdcall PAGEABLE_VCache_FreeBlock (HANDLE hCacheBlock) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VCache_GetSize (BYTE FsdID, PDWORD pNumberOfCacheBlocks) ;
DWORD __stdcall INIT_VCache_GetSize (BYTE FsdID, PDWORD pNumberOfCacheBlocks) ;
DWORD __stdcall DEBUG_VCache_GetSize (BYTE FsdID, PDWORD pNumberOfCacheBlocks) ;
DWORD __stdcall STATIC_VCache_GetSize (BYTE FsdID, PDWORD pNumberOfCacheBlocks) ;
DWORD __stdcall PAGEABLE_VCache_GetSize (BYTE FsdID, PDWORD pNumberOfCacheBlocks) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VCache_GetStats (PDWORD pNumberOfMisses, PDWORD pNumberOfHits, PPVOID pBaseAddr, PDWORD pNumberOfDiscardedBlocks) ;
VOID __stdcall INIT_VCache_GetStats (PDWORD pNumberOfMisses, PDWORD pNumberOfHits, PPVOID pBaseAddr, PDWORD pNumberOfDiscardedBlocks) ;
VOID __stdcall DEBUG_VCache_GetStats (PDWORD pNumberOfMisses, PDWORD pNumberOfHits, PPVOID pBaseAddr, PDWORD pNumberOfDiscardedBlocks) ;
VOID __stdcall STATIC_VCache_GetStats (PDWORD pNumberOfMisses, PDWORD pNumberOfHits, PPVOID pBaseAddr, PDWORD pNumberOfDiscardedBlocks) ;
VOID __stdcall PAGEABLE_VCache_GetStats (PDWORD pNumberOfMisses, PDWORD pNumberOfHits, PPVOID pBaseAddr, PDWORD pNumberOfDiscardedBlocks) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_VCache_Get_Version (void) ;
DWORD __cdecl INIT_VCache_Get_Version (void) ;
DWORD __cdecl DEBUG_VCache_Get_Version (void) ;
DWORD __cdecl STATIC_VCache_Get_Version (void) ;
DWORD __cdecl PAGEABLE_VCache_Get_Version (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VCache_Hold (HANDLE hCacheBlock) ;
VOID __stdcall INIT_VCache_Hold (HANDLE hCacheBlock) ;
VOID __stdcall DEBUG_VCache_Hold (HANDLE hCacheBlock) ;
VOID __stdcall STATIC_VCache_Hold (HANDLE hCacheBlock) ;
VOID __stdcall PAGEABLE_VCache_Hold (HANDLE hCacheBlock) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VCache_MakeMRU (HANDLE hCacheBlock) ;
VOID __stdcall INIT_VCache_MakeMRU (HANDLE hCacheBlock) ;
VOID __stdcall DEBUG_VCache_MakeMRU (HANDLE hCacheBlock) ;
VOID __stdcall STATIC_VCache_MakeMRU (HANDLE hCacheBlock) ;
VOID __stdcall PAGEABLE_VCache_MakeMRU (HANDLE hCacheBlock) ;
#endif

#ifdef WIN40_OR_LATER
BYTE __stdcall LOCK_VCache_Register (PVOID pBufferDiscardProc, DWORD nBlocks) ;
BYTE __stdcall INIT_VCache_Register (PVOID pBufferDiscardProc, DWORD nBlocks) ;
BYTE __stdcall DEBUG_VCache_Register (PVOID pBufferDiscardProc, DWORD nBlocks) ;
BYTE __stdcall STATIC_VCache_Register (PVOID pBufferDiscardProc, DWORD nBlocks) ;
BYTE __stdcall PAGEABLE_VCache_Register (PVOID pBufferDiscardProc, DWORD nBlocks) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VCache_SwapBuffers (HANDLE hCacheBlock1,  HANDLE hCacheBlock2) ;
BOOL __stdcall INIT_VCache_SwapBuffers (HANDLE hCacheBlock1,  HANDLE hCacheBlock2) ;
BOOL __stdcall DEBUG_VCache_SwapBuffers (HANDLE hCacheBlock1,  HANDLE hCacheBlock2) ;
BOOL __stdcall STATIC_VCache_SwapBuffers (HANDLE hCacheBlock1,  HANDLE hCacheBlock2) ;
BOOL __stdcall PAGEABLE_VCache_SwapBuffers (HANDLE hCacheBlock1,  HANDLE hCacheBlock2) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VCache_TestHold (HANDLE hCacheBlock) ;
DWORD __stdcall INIT_VCache_TestHold (HANDLE hCacheBlock) ;
DWORD __stdcall DEBUG_VCache_TestHold (HANDLE hCacheBlock) ;
DWORD __stdcall STATIC_VCache_TestHold (HANDLE hCacheBlock) ;
DWORD __stdcall PAGEABLE_VCache_TestHold (HANDLE hCacheBlock) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VCache_Unhold (HANDLE hCacheBlock) ;
BOOL __stdcall INIT_VCache_Unhold (HANDLE hCacheBlock) ;
BOOL __stdcall DEBUG_VCache_Unhold (HANDLE hCacheBlock) ;
BOOL __stdcall STATIC_VCache_Unhold (HANDLE hCacheBlock) ;
BOOL __stdcall PAGEABLE_VCache_Unhold (HANDLE hCacheBlock) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VCache_CloseLookupCache ( HLOOKUP _hnd_ ) ;
DWORD __cdecl INIT__VCache_CloseLookupCache ( HLOOKUP _hnd_ ) ;
DWORD __cdecl DEBUG__VCache_CloseLookupCache ( HLOOKUP _hnd_ ) ;
DWORD __cdecl STATIC__VCache_CloseLookupCache ( HLOOKUP _hnd_ ) ;
DWORD __cdecl PAGEABLE__VCache_CloseLookupCache ( HLOOKUP _hnd_ ) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VCache_CreateLookupCache (PCHAR szName, DWORD nMaxElems, DWORD Flags, HLOOKUP* phlookup) ;
DWORD __cdecl INIT__VCache_CreateLookupCache (PCHAR szName, DWORD nMaxElems, DWORD Flags, HLOOKUP* phlookup) ;
DWORD __cdecl DEBUG__VCache_CreateLookupCache (PCHAR szName, DWORD nMaxElems, DWORD Flags, HLOOKUP* phlookup) ;
DWORD __cdecl STATIC__VCache_CreateLookupCache (PCHAR szName, DWORD nMaxElems, DWORD Flags, HLOOKUP* phlookup) ;
DWORD __cdecl PAGEABLE__VCache_CreateLookupCache (PCHAR szName, DWORD nMaxElems, DWORD Flags, HLOOKUP* phlookup) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VCache_DeleteLookupCache (PCHAR szName) ;
DWORD __cdecl INIT__VCache_DeleteLookupCache (PCHAR szName) ;
DWORD __cdecl DEBUG__VCache_DeleteLookupCache (PCHAR szName) ;
DWORD __cdecl STATIC__VCache_DeleteLookupCache (PCHAR szName) ;
DWORD __cdecl PAGEABLE__VCache_DeleteLookupCache (PCHAR szName) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VCache_Lookup ( HLOOKUP hLookup, ULONG keylen, PVOID pKey, PULONG pdatalen, PVOID pData) ;
DWORD __cdecl INIT__VCache_Lookup ( HLOOKUP hLookup, ULONG keylen, PVOID pKey, PULONG pdatalen, PVOID pData) ;
DWORD __cdecl DEBUG__VCache_Lookup ( HLOOKUP hLookup, ULONG keylen, PVOID pKey, PULONG pdatalen, PVOID pData) ;
DWORD __cdecl STATIC__VCache_Lookup ( HLOOKUP hLookup, ULONG keylen, PVOID pKey, PULONG pdatalen, PVOID pData) ;
DWORD __cdecl PAGEABLE__VCache_Lookup ( HLOOKUP hLookup, ULONG keylen, PVOID pKey, PULONG pdatalen, PVOID pData) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__VCache_UpdateLookup (HLOOKUP hLookup, ULONG keylen, PVOID pKey, ULONG datalen, PVOID pData) ;
DWORD __cdecl INIT__VCache_UpdateLookup (HLOOKUP hLookup, ULONG keylen, PVOID pKey, ULONG datalen, PVOID pData) ;
DWORD __cdecl DEBUG__VCache_UpdateLookup (HLOOKUP hLookup, ULONG keylen, PVOID pKey, ULONG datalen, PVOID pData) ;
DWORD __cdecl STATIC__VCache_UpdateLookup (HLOOKUP hLookup, ULONG keylen, PVOID pKey, ULONG datalen, PVOID pData) ;
DWORD __cdecl PAGEABLE__VCache_UpdateLookup (HLOOKUP hLookup, ULONG keylen, PVOID pKey, ULONG datalen, PVOID pData) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK_VCache_RelinquishPage (void) ;
PVOID __cdecl INIT_VCache_RelinquishPage (void) ;
PVOID __cdecl DEBUG_VCache_RelinquishPage (void) ;
PVOID __cdecl STATIC_VCache_RelinquishPage (void) ;
PVOID __cdecl PAGEABLE_VCache_RelinquishPage (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VCache_UseThisPage (PVOID linear) ;
VOID __stdcall INIT_VCache_UseThisPage (PVOID linear) ;
VOID __stdcall DEBUG_VCache_UseThisPage (PVOID linear) ;
VOID __stdcall STATIC_VCache_UseThisPage (PVOID linear) ;
VOID __stdcall PAGEABLE_VCache_UseThisPage (PVOID linear) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VCache_RecalcSums (HANDLE hCacheBlock, DWORD SectorSize) ;
VOID __stdcall INIT_VCache_RecalcSums (HANDLE hCacheBlock, DWORD SectorSize) ;
VOID __stdcall DEBUG_VCache_RecalcSums (HANDLE hCacheBlock, DWORD SectorSize) ;
VOID __stdcall STATIC_VCache_RecalcSums (HANDLE hCacheBlock, DWORD SectorSize) ;
VOID __stdcall PAGEABLE_VCache_RecalcSums (HANDLE hCacheBlock, DWORD SectorSize) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VCache_TestHandle (BYTE FsdID, HANDLE hCacheBlock) ;
VOID __stdcall INIT_VCache_TestHandle (BYTE FsdID, HANDLE hCacheBlock) ;
VOID __stdcall DEBUG_VCache_TestHandle (BYTE FsdID, HANDLE hCacheBlock) ;
VOID __stdcall STATIC_VCache_TestHandle (BYTE FsdID, HANDLE hCacheBlock) ;
VOID __stdcall PAGEABLE_VCache_TestHandle (BYTE FsdID, HANDLE hCacheBlock) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VCache_VerifySums (HANDLE hCacheBlock, DWORD SectorSize) ;
BOOL __stdcall INIT_VCache_VerifySums (HANDLE hCacheBlock, DWORD SectorSize) ;
BOOL __stdcall DEBUG_VCache_VerifySums (HANDLE hCacheBlock, DWORD SectorSize) ;
BOOL __stdcall STATIC_VCache_VerifySums (HANDLE hCacheBlock, DWORD SectorSize) ;
BOOL __stdcall PAGEABLE_VCache_VerifySums (HANDLE hCacheBlock, DWORD SectorSize) ;
#endif

#ifdef WIN40_OR_LATER
WORD __stdcall LOCK_DOSMGR_Alloc_Local_Sys_VM_Mem (WORD nParagraphs) ;
WORD __stdcall INIT_DOSMGR_Alloc_Local_Sys_VM_Mem (WORD nParagraphs) ;
WORD __stdcall DEBUG_DOSMGR_Alloc_Local_Sys_VM_Mem (WORD nParagraphs) ;
WORD __stdcall STATIC_DOSMGR_Alloc_Local_Sys_VM_Mem (WORD nParagraphs) ;
WORD __stdcall PAGEABLE_DOSMGR_Alloc_Local_Sys_VM_Mem (WORD nParagraphs) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_DOSMGR_Init_UMB_Area (void) ;
BOOL __stdcall INIT_DOSMGR_Init_UMB_Area (void) ;
BOOL __stdcall DEBUG_DOSMGR_Init_UMB_Area (void) ;
BOOL __stdcall STATIC_DOSMGR_Init_UMB_Area (void) ;
BOOL __stdcall PAGEABLE_DOSMGR_Init_UMB_Area (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__GetRegistryKey (DWORD dwType, PCHAR szDeviceName, DWORD dwFlags, PHKEY phDevKey) ;
DWORD __cdecl INIT__GetRegistryKey (DWORD dwType, PCHAR szDeviceName, DWORD dwFlags, PHKEY phDevKey) ;
DWORD __cdecl DEBUG__GetRegistryKey (DWORD dwType, PCHAR szDeviceName, DWORD dwFlags, PHKEY phDevKey) ;
DWORD __cdecl STATIC__GetRegistryKey (DWORD dwType, PCHAR szDeviceName, DWORD dwFlags, PHKEY phDevKey) ;
DWORD __cdecl PAGEABLE__GetRegistryKey (DWORD dwType, PCHAR szDeviceName, DWORD dwFlags, PHKEY phDevKey) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Get_Nest_Exec_Status (PDWORD pR3EIP, PDWORD pR3CS, PDWORD pR3SSSP, PDWORD pR0ESP) ;
BOOL __stdcall INIT_Get_Nest_Exec_Status (PDWORD pR3EIP, PDWORD pR3CS, PDWORD pR3SSSP, PDWORD pR0ESP) ;
BOOL __stdcall DEBUG_Get_Nest_Exec_Status (PDWORD pR3EIP, PDWORD pR3CS, PDWORD pR3SSSP, PDWORD pR0ESP) ;
BOOL __stdcall STATIC_Get_Nest_Exec_Status (PDWORD pR3EIP, PDWORD pR3CS, PDWORD pR3SSSP, PDWORD pR0ESP) ;
BOOL __stdcall PAGEABLE_Get_Nest_Exec_Status (PDWORD pR3EIP, PDWORD pR3CS, PDWORD pR3SSSP, PDWORD pR0ESP) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__AddReclaimableItem (ReclaimStruc* pReclaim, DWORD Flags) ;
BOOL __cdecl INIT__AddReclaimableItem (ReclaimStruc* pReclaim, DWORD Flags) ;
BOOL __cdecl DEBUG__AddReclaimableItem (ReclaimStruc* pReclaim, DWORD Flags) ;
BOOL __cdecl STATIC__AddReclaimableItem (ReclaimStruc* pReclaim, DWORD Flags) ;
BOOL __cdecl PAGEABLE__AddReclaimableItem (ReclaimStruc* pReclaim, DWORD Flags) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK__AtEventTime (void) ;
BOOL __cdecl INIT__AtEventTime (void) ;
BOOL __cdecl DEBUG__AtEventTime (void) ;
BOOL __cdecl STATIC__AtEventTime (void) ;
BOOL __cdecl PAGEABLE__AtEventTime (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_Close_Boot_Log (void) ;
VOID __cdecl INIT_Close_Boot_Log (void) ;
VOID __cdecl DEBUG_Close_Boot_Log (void) ;
VOID __cdecl STATIC_Close_Boot_Log (void) ;
VOID __cdecl PAGEABLE_Close_Boot_Log (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_EnableDisable_Boot_Log (BOOL bEnable) ;
VOID __stdcall INIT_EnableDisable_Boot_Log (BOOL bEnable) ;
VOID __stdcall DEBUG_EnableDisable_Boot_Log (BOOL bEnable) ;
VOID __stdcall STATIC_EnableDisable_Boot_Log (BOOL bEnable) ;
VOID __stdcall PAGEABLE_EnableDisable_Boot_Log (BOOL bEnable) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__EnumReclaimableItem (DWORD enumKey, ReclaimStruc* pReclaim,  DWORD Flags) ;
DWORD __cdecl INIT__EnumReclaimableItem (DWORD enumKey, ReclaimStruc* pReclaim,  DWORD Flags) ;
DWORD __cdecl DEBUG__EnumReclaimableItem (DWORD enumKey, ReclaimStruc* pReclaim,  DWORD Flags) ;
DWORD __cdecl STATIC__EnumReclaimableItem (DWORD enumKey, ReclaimStruc* pReclaim,  DWORD Flags) ;
DWORD __cdecl PAGEABLE__EnumReclaimableItem (DWORD enumKey, ReclaimStruc* pReclaim,  DWORD Flags) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __cdecl LOCK_Open_Boot_Log (void) ;
BOOL __cdecl INIT_Open_Boot_Log (void) ;
BOOL __cdecl DEBUG_Open_Boot_Log (void) ;
BOOL __cdecl STATIC_Open_Boot_Log (void) ;
BOOL __cdecl PAGEABLE_Open_Boot_Log (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK__SetReclaimableItem (DWORD enumKey, ReclaimStruc* pReclaim, DWORD Flags) ;
DWORD __cdecl INIT__SetReclaimableItem (DWORD enumKey, ReclaimStruc* pReclaim, DWORD Flags) ;
DWORD __cdecl DEBUG__SetReclaimableItem (DWORD enumKey, ReclaimStruc* pReclaim, DWORD Flags) ;
DWORD __cdecl STATIC__SetReclaimableItem (DWORD enumKey, ReclaimStruc* pReclaim, DWORD Flags) ;
DWORD __cdecl PAGEABLE__SetReclaimableItem (DWORD enumKey, ReclaimStruc* pReclaim, DWORD Flags) ;
#endif

PVOID __stdcall LOCK_Hook_Device_Service_Ex (DWORD Service, PDeviceServiceEx_HANDLER pFunc, PDeviceServiceEx_THUNK pThunk) ;
PVOID __stdcall INIT_Hook_Device_Service_Ex (DWORD Service, PDeviceServiceEx_HANDLER pFunc, PDeviceServiceEx_THUNK pThunk) ;
PVOID __stdcall DEBUG_Hook_Device_Service_Ex (DWORD Service, PDeviceServiceEx_HANDLER pFunc, PDeviceServiceEx_THUNK pThunk) ;
PVOID __stdcall STATIC_Hook_Device_Service_Ex (DWORD Service, PDeviceServiceEx_HANDLER pFunc, PDeviceServiceEx_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_Hook_Device_Service_Ex (DWORD Service, PDeviceServiceEx_HANDLER pFunc, PDeviceServiceEx_THUNK pThunk) ;

#ifdef WIN40_OR_LATER
DWORD __cdecl LOCK_PERF_Get_Version (void) ;
DWORD __cdecl INIT_PERF_Get_Version (void) ;
DWORD __cdecl DEBUG_PERF_Get_Version (void) ;
DWORD __cdecl STATIC_PERF_Get_Version (void) ;
DWORD __cdecl PAGEABLE_PERF_Get_Version (void) ;
#endif

#ifdef WIN40_OR_LATER
PERFHANDLE __cdecl LOCK_PERF_Server_Register (PERF_SERVER_0* p) ;
PERFHANDLE __cdecl INIT_PERF_Server_Register (PERF_SERVER_0* p) ;
PERFHANDLE __cdecl DEBUG_PERF_Server_Register (PERF_SERVER_0* p) ;
PERFHANDLE __cdecl STATIC_PERF_Server_Register (PERF_SERVER_0* p) ;
PERFHANDLE __cdecl PAGEABLE_PERF_Server_Register (PERF_SERVER_0* p) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_PERF_Server_Deregister (PERFHANDLE hReg) ;
VOID __cdecl INIT_PERF_Server_Deregister (PERFHANDLE hReg) ;
VOID __cdecl DEBUG_PERF_Server_Deregister (PERFHANDLE hReg) ;
VOID __cdecl STATIC_PERF_Server_Deregister (PERFHANDLE hReg) ;
VOID __cdecl PAGEABLE_PERF_Server_Deregister (PERFHANDLE hReg) ;
#endif

#ifdef WIN40_OR_LATER
STATHANDLE __cdecl LOCK_PERF_Server_Add_Stat (PERFHANDLE hReg, PERF_STAT_0* pStat) ;
STATHANDLE __cdecl INIT_PERF_Server_Add_Stat (PERFHANDLE hReg, PERF_STAT_0* pStat) ;
STATHANDLE __cdecl DEBUG_PERF_Server_Add_Stat (PERFHANDLE hReg, PERF_STAT_0* pStat) ;
STATHANDLE __cdecl STATIC_PERF_Server_Add_Stat (PERFHANDLE hReg, PERF_STAT_0* pStat) ;
STATHANDLE __cdecl PAGEABLE_PERF_Server_Add_Stat (PERFHANDLE hReg, PERF_STAT_0* pStat) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_PERF_Server_Remove_Stat (STATHANDLE hStat) ;
VOID __cdecl INIT_PERF_Server_Remove_Stat (STATHANDLE hStat) ;
VOID __cdecl DEBUG_PERF_Server_Remove_Stat (STATHANDLE hStat) ;
VOID __cdecl STATIC_PERF_Server_Remove_Stat (STATHANDLE hStat) ;
VOID __cdecl PAGEABLE_PERF_Server_Remove_Stat (STATHANDLE hStat) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Unhook_Device_Service_Ex (DWORD Service, PDeviceServiceEx_HANDLER HookProc, PDeviceServiceEx_THUNK pThunk) ;
BOOL __stdcall INIT_Unhook_Device_Service_Ex (DWORD Service, PDeviceServiceEx_HANDLER HookProc, PDeviceServiceEx_THUNK pThunk) ;
BOOL __stdcall DEBUG_Unhook_Device_Service_Ex (DWORD Service, PDeviceServiceEx_HANDLER HookProc, PDeviceServiceEx_THUNK pThunk) ;
BOOL __stdcall STATIC_Unhook_Device_Service_Ex (DWORD Service, PDeviceServiceEx_HANDLER HookProc, PDeviceServiceEx_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_Unhook_Device_Service_Ex (DWORD Service, PDeviceServiceEx_HANDLER HookProc, PDeviceServiceEx_THUNK pThunk) ;
#endif

VOID __stdcall LOCK_Call_Previous_Hook_Proc (PDSFRAME pFrame, PVOID pThunk) ;
VOID __stdcall INIT_Call_Previous_Hook_Proc (PDSFRAME pFrame, PVOID pThunk) ;
VOID __stdcall DEBUG_Call_Previous_Hook_Proc (PDSFRAME pFrame, PVOID pThunk) ;
VOID __stdcall STATIC_Call_Previous_Hook_Proc (PDSFRAME pFrame, PVOID pThunk) ;
VOID __stdcall PAGEABLE_Call_Previous_Hook_Proc (PDSFRAME pFrame, PVOID pThunk) ;

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_dummy_935 (void) ;
VOID __stdcall INIT_dummy_935 (void) ;
VOID __stdcall DEBUG_dummy_935 (void) ;
VOID __stdcall STATIC_dummy_935 (void) ;
VOID __stdcall PAGEABLE_dummy_935 (void) ;
#endif

PVOID __stdcall LOCK_Hook_Device_Service_C (DWORD Service, PVOID pHandler, HDSC_Thunk* pThunk) ;
PVOID __stdcall INIT_Hook_Device_Service_C (DWORD Service, PVOID pHandler, HDSC_Thunk* pThunk) ;
PVOID __stdcall DEBUG_Hook_Device_Service_C (DWORD Service, PVOID pHandler, HDSC_Thunk* pThunk) ;
PVOID __stdcall STATIC_Hook_Device_Service_C (DWORD Service, PVOID pHandler, HDSC_Thunk* pThunk) ;
PVOID __stdcall PAGEABLE_Hook_Device_Service_C (DWORD Service, PVOID pHandler, HDSC_Thunk* pThunk) ;

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_Unhook_Device_Service_C (DWORD Service, HDSC_Thunk* pThunk) ;
BOOL __stdcall INIT_Unhook_Device_Service_C (DWORD Service, HDSC_Thunk* pThunk) ;
BOOL __stdcall DEBUG_Unhook_Device_Service_C (DWORD Service, HDSC_Thunk* pThunk) ;
BOOL __stdcall STATIC_Unhook_Device_Service_C (DWORD Service, HDSC_Thunk* pThunk) ;
BOOL __stdcall PAGEABLE_Unhook_Device_Service_C (DWORD Service, HDSC_Thunk* pThunk) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Add_ID (DEVNODE dnDevNode, PFARCHAR pszID, DWORD flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Add_ID (DEVNODE dnDevNode, PFARCHAR pszID, DWORD flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Add_ID (DEVNODE dnDevNode, PFARCHAR pszID, DWORD flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Add_ID (DEVNODE dnDevNode, PFARCHAR pszID, DWORD flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Add_ID (DEVNODE dnDevNode, PFARCHAR pszID, DWORD flags) ;
#endif

#ifdef WIN40_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Register_Enumerator_Function (DEVNODE node, CMENUMFUNCTION handler, ULONG flags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Register_Enumerator_Function (DEVNODE node, CMENUMFUNCTION handler, ULONG flags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Register_Enumerator_Function (DEVNODE node, CMENUMFUNCTION handler, ULONG flags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Register_Enumerator_Function (DEVNODE node, CMENUMFUNCTION handler, ULONG flags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Register_Enumerator_Function (DEVNODE node, CMENUMFUNCTION handler, ULONG flags) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_SHELL_Update_User_Activity (void) ;
VOID __cdecl INIT_SHELL_Update_User_Activity (void) ;
VOID __cdecl DEBUG_SHELL_Update_User_Activity (void) ;
VOID __cdecl STATIC_SHELL_Update_User_Activity (void) ;
VOID __cdecl PAGEABLE_SHELL_Update_User_Activity (void) ;
#endif

BOOL __stdcall LOCK_VDMAD_Unlock_DMA_Region_No_Dirty (CONST VOID * LinearAddress, DWORD nBytes) ;
BOOL __stdcall INIT_VDMAD_Unlock_DMA_Region_No_Dirty (CONST VOID * LinearAddress, DWORD nBytes) ;
BOOL __stdcall DEBUG_VDMAD_Unlock_DMA_Region_No_Dirty (CONST VOID * LinearAddress, DWORD nBytes) ;
BOOL __stdcall STATIC_VDMAD_Unlock_DMA_Region_No_Dirty (CONST VOID * LinearAddress, DWORD nBytes) ;
BOOL __stdcall PAGEABLE_VDMAD_Unlock_DMA_Region_No_Dirty (CONST VOID * LinearAddress, DWORD nBytes) ;

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VWIN32_GetCurrentDirectory (PCHAR buf) ;
BOOL __stdcall INIT_VWIN32_GetCurrentDirectory (PCHAR buf) ;
BOOL __stdcall DEBUG_VWIN32_GetCurrentDirectory (PCHAR buf) ;
BOOL __stdcall STATIC_VWIN32_GetCurrentDirectory (PCHAR buf) ;
BOOL __stdcall PAGEABLE_VWIN32_GetCurrentDirectory (PCHAR buf) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VXDLDR_UnloadMe (WORD VxD_ID, PCHAR VxD_Name) ;
BOOL __stdcall INIT_VXDLDR_UnloadMe (WORD VxD_ID, PCHAR VxD_Name) ;
BOOL __stdcall DEBUG_VXDLDR_UnloadMe (WORD VxD_ID, PCHAR VxD_Name) ;
BOOL __stdcall STATIC_VXDLDR_UnloadMe (WORD VxD_ID, PCHAR VxD_Name) ;
BOOL __stdcall PAGEABLE_VXDLDR_UnloadMe (WORD VxD_ID, PCHAR VxD_Name) ;
#endif

#ifdef WIN40_OR_LATER
LRESULT __cdecl LOCK__PELDR_InitCompleted (HPEMODULE hModule) ;
LRESULT __cdecl INIT__PELDR_InitCompleted (HPEMODULE hModule) ;
LRESULT __cdecl DEBUG__PELDR_InitCompleted (HPEMODULE hModule) ;
LRESULT __cdecl STATIC__PELDR_InitCompleted (HPEMODULE hModule) ;
LRESULT __cdecl PAGEABLE__PELDR_InitCompleted (HPEMODULE hModule) ;
#endif

#ifdef WIN40_OR_LATER
LRESULT __cdecl LOCK__PELDR_AddExportTable (PHPEEXPORTTABLE phExportTable, PSTR pszModuleName, ULONG cExportedFunctions, ULONG cExportedNames, ULONG ulOrdinalBase, PVOID *pExportNameList, PUSHORT pExportOrdinals, PVOID *pExportAddrs, PHLIST phList) ;
LRESULT __cdecl INIT__PELDR_AddExportTable (PHPEEXPORTTABLE phExportTable, PSTR pszModuleName, ULONG cExportedFunctions, ULONG cExportedNames, ULONG ulOrdinalBase, PVOID *pExportNameList, PUSHORT pExportOrdinals, PVOID *pExportAddrs, PHLIST phList) ;
LRESULT __cdecl DEBUG__PELDR_AddExportTable (PHPEEXPORTTABLE phExportTable, PSTR pszModuleName, ULONG cExportedFunctions, ULONG cExportedNames, ULONG ulOrdinalBase, PVOID *pExportNameList, PUSHORT pExportOrdinals, PVOID *pExportAddrs, PHLIST phList) ;
LRESULT __cdecl STATIC__PELDR_AddExportTable (PHPEEXPORTTABLE phExportTable, PSTR pszModuleName, ULONG cExportedFunctions, ULONG cExportedNames, ULONG ulOrdinalBase, PVOID *pExportNameList, PUSHORT pExportOrdinals, PVOID *pExportAddrs, PHLIST phList) ;
LRESULT __cdecl PAGEABLE__PELDR_AddExportTable (PHPEEXPORTTABLE phExportTable, PSTR pszModuleName, ULONG cExportedFunctions, ULONG cExportedNames, ULONG ulOrdinalBase, PVOID *pExportNameList, PUSHORT pExportOrdinals, PVOID *pExportAddrs, PHLIST phList) ;
#endif

#ifdef WIN40_OR_LATER
LRESULT __cdecl LOCK__PELDR_FreeModule (HPEMODULE hModule, PHLIST phList) ;
LRESULT __cdecl INIT__PELDR_FreeModule (HPEMODULE hModule, PHLIST phList) ;
LRESULT __cdecl DEBUG__PELDR_FreeModule (HPEMODULE hModule, PHLIST phList) ;
LRESULT __cdecl STATIC__PELDR_FreeModule (HPEMODULE hModule, PHLIST phList) ;
LRESULT __cdecl PAGEABLE__PELDR_FreeModule (HPEMODULE hModule, PHLIST phList) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK__PELDR_GetEntryPoint (HPEMODULE hModule) ;
PVOID __cdecl INIT__PELDR_GetEntryPoint (HPEMODULE hModule) ;
PVOID __cdecl DEBUG__PELDR_GetEntryPoint (HPEMODULE hModule) ;
PVOID __cdecl STATIC__PELDR_GetEntryPoint (HPEMODULE hModule) ;
PVOID __cdecl PAGEABLE__PELDR_GetEntryPoint (HPEMODULE hModule) ;
#endif

#ifdef WIN40_OR_LATER
HPEMODULE __cdecl LOCK__PELDR_GetModuleHandle (PSTR pFileName) ;
HPEMODULE __cdecl INIT__PELDR_GetModuleHandle (PSTR pFileName) ;
HPEMODULE __cdecl DEBUG__PELDR_GetModuleHandle (PSTR pFileName) ;
HPEMODULE __cdecl STATIC__PELDR_GetModuleHandle (PSTR pFileName) ;
HPEMODULE __cdecl PAGEABLE__PELDR_GetModuleHandle (PSTR pFileName) ;
#endif

#ifdef WIN40_OR_LATER
LRESULT __cdecl LOCK__PELDR_GetModuleUsage (HPEMODULE  hModule) ;
LRESULT __cdecl INIT__PELDR_GetModuleUsage (HPEMODULE  hModule) ;
LRESULT __cdecl DEBUG__PELDR_GetModuleUsage (HPEMODULE  hModule) ;
LRESULT __cdecl STATIC__PELDR_GetModuleUsage (HPEMODULE  hModule) ;
LRESULT __cdecl PAGEABLE__PELDR_GetModuleUsage (HPEMODULE  hModule) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __cdecl LOCK__PELDR_GetProcAddress (HPEMODULE hModule, PVOID FuncName, PHLIST phList) ;
PVOID __cdecl INIT__PELDR_GetProcAddress (HPEMODULE hModule, PVOID FuncName, PHLIST phList) ;
PVOID __cdecl DEBUG__PELDR_GetProcAddress (HPEMODULE hModule, PVOID FuncName, PHLIST phList) ;
PVOID __cdecl STATIC__PELDR_GetProcAddress (HPEMODULE hModule, PVOID FuncName, PHLIST phList) ;
PVOID __cdecl PAGEABLE__PELDR_GetProcAddress (HPEMODULE hModule, PVOID FuncName, PHLIST phList) ;
#endif

#ifdef WIN40_OR_LATER
LRESULT __cdecl LOCK__PELDR_LoadModule (PHPEMODULE  phModule, PSTR pFileName, PHLIST phList) ;
LRESULT __cdecl INIT__PELDR_LoadModule (PHPEMODULE  phModule, PSTR pFileName, PHLIST phList) ;
LRESULT __cdecl DEBUG__PELDR_LoadModule (PHPEMODULE  phModule, PSTR pFileName, PHLIST phList) ;
LRESULT __cdecl STATIC__PELDR_LoadModule (PHPEMODULE  phModule, PSTR pFileName, PHLIST phList) ;
LRESULT __cdecl PAGEABLE__PELDR_LoadModule (PHPEMODULE  phModule, PSTR pFileName, PHLIST phList) ;
#endif

#ifdef WIN40_OR_LATER
LRESULT __cdecl LOCK__PELDR_LoadModuleEx (PHPEMODULE  phModule, PSTR pFileName, PHLIST phList, DWORD dwFlag) ;
LRESULT __cdecl INIT__PELDR_LoadModuleEx (PHPEMODULE  phModule, PSTR pFileName, PHLIST phList, DWORD dwFlag) ;
LRESULT __cdecl DEBUG__PELDR_LoadModuleEx (PHPEMODULE  phModule, PSTR pFileName, PHLIST phList, DWORD dwFlag) ;
LRESULT __cdecl STATIC__PELDR_LoadModuleEx (PHPEMODULE  phModule, PSTR pFileName, PHLIST phList, DWORD dwFlag) ;
LRESULT __cdecl PAGEABLE__PELDR_LoadModuleEx (PHPEMODULE  phModule, PSTR pFileName, PHLIST phList, DWORD dwFlag) ;
#endif

#ifdef WIN40_OR_LATER
LRESULT __cdecl LOCK__PELDR_RemoveExportTable (HPEEXPORTTABLE hExportTable, PHLIST phList) ;
LRESULT __cdecl INIT__PELDR_RemoveExportTable (HPEEXPORTTABLE hExportTable, PHLIST phList) ;
LRESULT __cdecl DEBUG__PELDR_RemoveExportTable (HPEEXPORTTABLE hExportTable, PHLIST phList) ;
LRESULT __cdecl STATIC__PELDR_RemoveExportTable (HPEEXPORTTABLE hExportTable, PHLIST phList) ;
LRESULT __cdecl PAGEABLE__PELDR_RemoveExportTable (HPEEXPORTTABLE hExportTable, PHLIST phList) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VCache_AdjustMinimum (BYTE FsdID, DWORD NewBufferQuota) ;
BOOL __stdcall INIT_VCache_AdjustMinimum (BYTE FsdID, DWORD NewBufferQuota) ;
BOOL __stdcall DEBUG_VCache_AdjustMinimum (BYTE FsdID, DWORD NewBufferQuota) ;
BOOL __stdcall STATIC_VCache_AdjustMinimum (BYTE FsdID, DWORD NewBufferQuota) ;
BOOL __stdcall PAGEABLE_VCache_AdjustMinimum (BYTE FsdID, DWORD NewBufferQuota) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VCache_CheckAvail (BYTE FsdID, DWORD NumberOfBuffersNeeded) ;
DWORD __stdcall INIT_VCache_CheckAvail (BYTE FsdID, DWORD NumberOfBuffersNeeded) ;
DWORD __stdcall DEBUG_VCache_CheckAvail (BYTE FsdID, DWORD NumberOfBuffersNeeded) ;
DWORD __stdcall STATIC_VCache_CheckAvail (BYTE FsdID, DWORD NumberOfBuffersNeeded) ;
DWORD __stdcall PAGEABLE_VCache_CheckAvail (BYTE FsdID, DWORD NumberOfBuffersNeeded) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VCache_Deregister (BYTE FsdID) ;
VOID __stdcall INIT_VCache_Deregister (BYTE FsdID) ;
VOID __stdcall DEBUG_VCache_Deregister (BYTE FsdID) ;
VOID __stdcall STATIC_VCache_Deregister (BYTE FsdID) ;
VOID __stdcall PAGEABLE_VCache_Deregister (BYTE FsdID) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VCache_Enum (BYTE FsdID, PCacheEnum_HANDLER pCallback, PCacheEnum_THUNK pThunk, DWORD ref1, DWORD ref2, DWORD ref3) ;
VOID __stdcall INIT_VCache_Enum (BYTE FsdID, PCacheEnum_HANDLER pCallback, PCacheEnum_THUNK pThunk, DWORD ref1, DWORD ref2, DWORD ref3) ;
VOID __stdcall DEBUG_VCache_Enum (BYTE FsdID, PCacheEnum_HANDLER pCallback, PCacheEnum_THUNK pThunk, DWORD ref1, DWORD ref2, DWORD ref3) ;
VOID __stdcall STATIC_VCache_Enum (BYTE FsdID, PCacheEnum_HANDLER pCallback, PCacheEnum_THUNK pThunk, DWORD ref1, DWORD ref2, DWORD ref3) ;
VOID __stdcall PAGEABLE_VCache_Enum (BYTE FsdID, PCacheEnum_HANDLER pCallback, PCacheEnum_THUNK pThunk, DWORD ref1, DWORD ref2, DWORD ref3) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_VKD_Enable_Keyboard (void) ;
VOID __cdecl INIT_VKD_Enable_Keyboard (void) ;
VOID __cdecl DEBUG_VKD_Enable_Keyboard (void) ;
VOID __cdecl STATIC_VKD_Enable_Keyboard (void) ;
VOID __cdecl PAGEABLE_VKD_Enable_Keyboard (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VKD_Disable_Keyboard (VMHANDLE hVM) ;
VOID __stdcall INIT_VKD_Disable_Keyboard (VMHANDLE hVM) ;
VOID __stdcall DEBUG_VKD_Disable_Keyboard (VMHANDLE hVM) ;
VOID __stdcall STATIC_VKD_Disable_Keyboard (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VKD_Disable_Keyboard (VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
WORD __stdcall LOCK_VKD_Get_Shift_State (VMHANDLE hVM) ;
WORD __stdcall INIT_VKD_Get_Shift_State (VMHANDLE hVM) ;
WORD __stdcall DEBUG_VKD_Get_Shift_State (VMHANDLE hVM) ;
WORD __stdcall STATIC_VKD_Get_Shift_State (VMHANDLE hVM) ;
WORD __stdcall PAGEABLE_VKD_Get_Shift_State (VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VKD_Put_Byte (BYTE MsgByte) ;
BOOL __stdcall INIT_VKD_Put_Byte (BYTE MsgByte) ;
BOOL __stdcall DEBUG_VKD_Put_Byte (BYTE MsgByte) ;
BOOL __stdcall STATIC_VKD_Put_Byte (BYTE MsgByte) ;
BOOL __stdcall PAGEABLE_VKD_Put_Byte (BYTE MsgByte) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VKD_Set_Shift_State (VMHANDLE hVM, WORD theShiftState) ;
BOOL __stdcall INIT_VKD_Set_Shift_State (VMHANDLE hVM, WORD theShiftState) ;
BOOL __stdcall DEBUG_VKD_Set_Shift_State (VMHANDLE hVM, WORD theShiftState) ;
BOOL __stdcall STATIC_VKD_Set_Shift_State (VMHANDLE hVM, WORD theShiftState) ;
BOOL __stdcall PAGEABLE_VKD_Set_Shift_State (VMHANDLE hVM, WORD theShiftState) ;
#endif

PVCDControl_HANDLER __stdcall LOCK_VCD_InitThunk_Control (PVCDControl_HANDLER pFunc, PVCDControl_THUNK pThunk) ;
PVCDControl_HANDLER __stdcall INIT_VCD_InitThunk_Control (PVCDControl_HANDLER pFunc, PVCDControl_THUNK pThunk) ;
PVCDControl_HANDLER __stdcall DEBUG_VCD_InitThunk_Control (PVCDControl_HANDLER pFunc, PVCDControl_THUNK pThunk) ;
PVCDControl_HANDLER __stdcall STATIC_VCD_InitThunk_Control (PVCDControl_HANDLER pFunc, PVCDControl_THUNK pThunk) ;
PVCDControl_HANDLER __stdcall PAGEABLE_VCD_InitThunk_Control (PVCDControl_HANDLER pFunc, PVCDControl_THUNK pThunk) ;

PVCDIrq_HANDLER __stdcall LOCK_VCD_InitThunk_Irq (PVCDIrq_HANDLER pFunc, PVCDIrq_THUNK pThunk) ;
PVCDIrq_HANDLER __stdcall INIT_VCD_InitThunk_Irq (PVCDIrq_HANDLER pFunc, PVCDIrq_THUNK pThunk) ;
PVCDIrq_HANDLER __stdcall DEBUG_VCD_InitThunk_Irq (PVCDIrq_HANDLER pFunc, PVCDIrq_THUNK pThunk) ;
PVCDIrq_HANDLER __stdcall STATIC_VCD_InitThunk_Irq (PVCDIrq_HANDLER pFunc, PVCDIrq_THUNK pThunk) ;
PVCDIrq_HANDLER __stdcall PAGEABLE_VCD_InitThunk_Irq (PVCDIrq_HANDLER pFunc, PVCDIrq_THUNK pThunk) ;

PVCDIo_HANDLER __stdcall LOCK_VCD_InitThunk_Io (PVCDIo_HANDLER pFunc, PVCDIo_THUNK pThunk) ;
PVCDIo_HANDLER __stdcall INIT_VCD_InitThunk_Io (PVCDIo_HANDLER pFunc, PVCDIo_THUNK pThunk) ;
PVCDIo_HANDLER __stdcall DEBUG_VCD_InitThunk_Io (PVCDIo_HANDLER pFunc, PVCDIo_THUNK pThunk) ;
PVCDIo_HANDLER __stdcall STATIC_VCD_InitThunk_Io (PVCDIo_HANDLER pFunc, PVCDIo_THUNK pThunk) ;
PVCDIo_HANDLER __stdcall PAGEABLE_VCD_InitThunk_Io (PVCDIo_HANDLER pFunc, PVCDIo_THUNK pThunk) ;

COMHANDLE __stdcall LOCK_VCD_Acquire_Port (DWORD PortNumber, PCHAR VxDName, DWORD Flags) ;
COMHANDLE __stdcall INIT_VCD_Acquire_Port (DWORD PortNumber, PCHAR VxDName, DWORD Flags) ;
COMHANDLE __stdcall DEBUG_VCD_Acquire_Port (DWORD PortNumber, PCHAR VxDName, DWORD Flags) ;
COMHANDLE __stdcall STATIC_VCD_Acquire_Port (DWORD PortNumber, PCHAR VxDName, DWORD Flags) ;
COMHANDLE __stdcall PAGEABLE_VCD_Acquire_Port (DWORD PortNumber, PCHAR VxDName, DWORD Flags) ;

VOID __stdcall LOCK_VCD_Free_Port_Windows_Style (COMHANDLE hPort, VMHANDLE hVM) ;
VOID __stdcall INIT_VCD_Free_Port_Windows_Style (COMHANDLE hPort, VMHANDLE hVM) ;
VOID __stdcall DEBUG_VCD_Free_Port_Windows_Style (COMHANDLE hPort, VMHANDLE hVM) ;
VOID __stdcall STATIC_VCD_Free_Port_Windows_Style (COMHANDLE hPort, VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VCD_Free_Port_Windows_Style (COMHANDLE hPort, VMHANDLE hVM) ;

BOOL __stdcall LOCK_VCD_Steal_Port_Windows_Style (COMHANDLE hPort, VMHANDLE hVM) ;
BOOL __stdcall INIT_VCD_Steal_Port_Windows_Style (COMHANDLE hPort, VMHANDLE hVM) ;
BOOL __stdcall DEBUG_VCD_Steal_Port_Windows_Style (COMHANDLE hPort, VMHANDLE hVM) ;
BOOL __stdcall STATIC_VCD_Steal_Port_Windows_Style (COMHANDLE hPort, VMHANDLE hVM) ;
BOOL __stdcall PAGEABLE_VCD_Steal_Port_Windows_Style (COMHANDLE hPort, VMHANDLE hVM) ;

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VCD_Find_COM_Index (DWORD BasePort) ;
DWORD __stdcall INIT_VCD_Find_COM_Index (DWORD BasePort) ;
DWORD __stdcall DEBUG_VCD_Find_COM_Index (DWORD BasePort) ;
DWORD __stdcall STATIC_VCD_Find_COM_Index (DWORD BasePort) ;
DWORD __stdcall PAGEABLE_VCD_Find_COM_Index (DWORD BasePort) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VCD_Set_Port_Global_Special (DWORD ComPort, BOOL bLocalPort) ;
BOOL __stdcall INIT_VCD_Set_Port_Global_Special (DWORD ComPort, BOOL bLocalPort) ;
BOOL __stdcall DEBUG_VCD_Set_Port_Global_Special (DWORD ComPort, BOOL bLocalPort) ;
BOOL __stdcall STATIC_VCD_Set_Port_Global_Special (DWORD ComPort, BOOL bLocalPort) ;
BOOL __stdcall PAGEABLE_VCD_Set_Port_Global_Special (DWORD ComPort, BOOL bLocalPort) ;
#endif

#ifdef WIN40_OR_LATER
COMHANDLE __stdcall LOCK_VCD_Virtualize_Port_Dynamic (DWORD ExtraVcdCom, DWORD ExtraPerVM, DWORD PortNumber, DWORD Flags, VCD_COM * pProcList, DWORD BaseIRQ) ;
COMHANDLE __stdcall INIT_VCD_Virtualize_Port_Dynamic (DWORD ExtraVcdCom, DWORD ExtraPerVM, DWORD PortNumber, DWORD Flags, VCD_COM * pProcList, DWORD BaseIRQ) ;
COMHANDLE __stdcall DEBUG_VCD_Virtualize_Port_Dynamic (DWORD ExtraVcdCom, DWORD ExtraPerVM, DWORD PortNumber, DWORD Flags, VCD_COM * pProcList, DWORD BaseIRQ) ;
COMHANDLE __stdcall STATIC_VCD_Virtualize_Port_Dynamic (DWORD ExtraVcdCom, DWORD ExtraPerVM, DWORD PortNumber, DWORD Flags, VCD_COM * pProcList, DWORD BaseIRQ) ;
COMHANDLE __stdcall PAGEABLE_VCD_Virtualize_Port_Dynamic (DWORD ExtraVcdCom, DWORD ExtraPerVM, DWORD PortNumber, DWORD Flags, VCD_COM * pProcList, DWORD BaseIRQ) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VCD_Unvirtualize_Port_Dynamic (COMHANDLE hPort) ;
VOID __stdcall INIT_VCD_Unvirtualize_Port_Dynamic (COMHANDLE hPort) ;
VOID __stdcall DEBUG_VCD_Unvirtualize_Port_Dynamic (COMHANDLE hPort) ;
VOID __stdcall STATIC_VCD_Unvirtualize_Port_Dynamic (COMHANDLE hPort) ;
VOID __stdcall PAGEABLE_VCD_Unvirtualize_Port_Dynamic (COMHANDLE hPort) ;
#endif

#ifdef WIN40_OR_LATER
COMHANDLE __stdcall LOCK_VCD_Acquire_Port_Windows_Style (DWORD PortNumber, VMHANDLE hVM, DWORD Flags, PVOID PortDataOrNotifyProc, PVOID OffsetOrRefData) ;
COMHANDLE __stdcall INIT_VCD_Acquire_Port_Windows_Style (DWORD PortNumber, VMHANDLE hVM, DWORD Flags, PVOID PortDataOrNotifyProc, PVOID OffsetOrRefData) ;
COMHANDLE __stdcall DEBUG_VCD_Acquire_Port_Windows_Style (DWORD PortNumber, VMHANDLE hVM, DWORD Flags, PVOID PortDataOrNotifyProc, PVOID OffsetOrRefData) ;
COMHANDLE __stdcall STATIC_VCD_Acquire_Port_Windows_Style (DWORD PortNumber, VMHANDLE hVM, DWORD Flags, PVOID PortDataOrNotifyProc, PVOID OffsetOrRefData) ;
COMHANDLE __stdcall PAGEABLE_VCD_Acquire_Port_Windows_Style (DWORD PortNumber, VMHANDLE hVM, DWORD Flags, PVOID PortDataOrNotifyProc, PVOID OffsetOrRefData) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VCD_Free_Port (COMHANDLE hComm) ;
VOID __stdcall INIT_VCD_Free_Port (COMHANDLE hComm) ;
VOID __stdcall DEBUG_VCD_Free_Port (COMHANDLE hComm) ;
VOID __stdcall STATIC_VCD_Free_Port (COMHANDLE hComm) ;
VOID __stdcall PAGEABLE_VCD_Free_Port (COMHANDLE hComm) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDD_Check_Update_Soon (VMHANDLE hVM) ;
VOID __stdcall INIT_VDD_Check_Update_Soon (VMHANDLE hVM) ;
VOID __stdcall DEBUG_VDD_Check_Update_Soon (VMHANDLE hVM) ;
VOID __stdcall STATIC_VDD_Check_Update_Soon (VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VDD_Check_Update_Soon (VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __stdcall LOCK_VDD_Get_GrabRtn (void) ;
PVOID __stdcall INIT_VDD_Get_GrabRtn (void) ;
PVOID __stdcall DEBUG_VDD_Get_GrabRtn (void) ;
PVOID __stdcall STATIC_VDD_Get_GrabRtn (void) ;
PVOID __stdcall PAGEABLE_VDD_Get_GrabRtn (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDD_Hide_Cursor (BOOL bHide, PVOID pCB) ;
VOID __stdcall INIT_VDD_Hide_Cursor (BOOL bHide, PVOID pCB) ;
VOID __stdcall DEBUG_VDD_Hide_Cursor (BOOL bHide, PVOID pCB) ;
VOID __stdcall STATIC_VDD_Hide_Cursor (BOOL bHide, PVOID pCB) ;
VOID __stdcall PAGEABLE_VDD_Hide_Cursor (BOOL bHide, PVOID pCB) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDD_Msg_SetCursPos (DWORD Row, DWORD Col, VMHANDLE hVM) ;
VOID __stdcall INIT_VDD_Msg_SetCursPos (DWORD Row, DWORD Col, VMHANDLE hVM) ;
VOID __stdcall DEBUG_VDD_Msg_SetCursPos (DWORD Row, DWORD Col, VMHANDLE hVM) ;
VOID __stdcall STATIC_VDD_Msg_SetCursPos (DWORD Row, DWORD Col, VMHANDLE hVM) ;
VOID __stdcall PAGEABLE_VDD_Msg_SetCursPos (DWORD Row, DWORD Col, VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VMD_Enable_Disable_Mouse_Events (VMHANDLE hVM, BOOL bEnable) ;
VOID __stdcall INIT_VMD_Enable_Disable_Mouse_Events (VMHANDLE hVM, BOOL bEnable) ;
VOID __stdcall DEBUG_VMD_Enable_Disable_Mouse_Events (VMHANDLE hVM, BOOL bEnable) ;
VOID __stdcall STATIC_VMD_Enable_Disable_Mouse_Events (VMHANDLE hVM, BOOL bEnable) ;
VOID __stdcall PAGEABLE_VMD_Enable_Disable_Mouse_Events (VMHANDLE hVM, BOOL bEnable) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_PageFile_Init_File (PDWORD pMaxSize, PDWORD pCurrentSize) ;
VOID __stdcall INIT_PageFile_Init_File (PDWORD pMaxSize, PDWORD pCurrentSize) ;
VOID __stdcall DEBUG_PageFile_Init_File (PDWORD pMaxSize, PDWORD pCurrentSize) ;
VOID __stdcall STATIC_PageFile_Init_File (PDWORD pMaxSize, PDWORD pCurrentSize) ;
VOID __stdcall PAGEABLE_PageFile_Init_File (PDWORD pMaxSize, PDWORD pCurrentSize) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_PageFile_Get_Version (PDWORD pVersion, PDWORD pMaxSize, PBYTE pPagerType) ;
BOOL __stdcall INIT_PageFile_Get_Version (PDWORD pVersion, PDWORD pMaxSize, PBYTE pPagerType) ;
BOOL __stdcall DEBUG_PageFile_Get_Version (PDWORD pVersion, PDWORD pMaxSize, PBYTE pPagerType) ;
BOOL __stdcall STATIC_PageFile_Get_Version (PDWORD pVersion, PDWORD pMaxSize, PBYTE pPagerType) ;
BOOL __stdcall PAGEABLE_PageFile_Get_Version (PDWORD pVersion, PDWORD pMaxSize, PBYTE pPagerType) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_PageFile_Clean_Up (void) ;
VOID __cdecl INIT_PageFile_Clean_Up (void) ;
VOID __cdecl DEBUG_PageFile_Clean_Up (void) ;
VOID __cdecl STATIC_PageFile_Clean_Up (void) ;
VOID __cdecl PAGEABLE_PageFile_Clean_Up (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_PageFile_Grow_File (DWORD cPages, PDWORD pMaxSize) ;
DWORD __stdcall INIT_PageFile_Grow_File (DWORD cPages, PDWORD pMaxSize) ;
DWORD __stdcall DEBUG_PageFile_Grow_File (DWORD cPages, PDWORD pMaxSize) ;
DWORD __stdcall STATIC_PageFile_Grow_File (DWORD cPages, PDWORD pMaxSize) ;
DWORD __stdcall PAGEABLE_PageFile_Grow_File (DWORD cPages, PDWORD pMaxSize) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_PageFile_Read_Or_Write (BYTE RorW, DWORD filepos, PVOID pPage) ;
BOOL __stdcall INIT_PageFile_Read_Or_Write (BYTE RorW, DWORD filepos, PVOID pPage) ;
BOOL __stdcall DEBUG_PageFile_Read_Or_Write (BYTE RorW, DWORD filepos, PVOID pPage) ;
BOOL __stdcall STATIC_PageFile_Read_Or_Write (BYTE RorW, DWORD filepos, PVOID pPage) ;
BOOL __stdcall PAGEABLE_PageFile_Read_Or_Write (BYTE RorW, DWORD filepos, PVOID pPage) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK_PageFile_Cancel (void) ;
VOID __cdecl INIT_PageFile_Cancel (void) ;
VOID __cdecl DEBUG_PageFile_Cancel (void) ;
VOID __cdecl STATIC_PageFile_Cancel (void) ;
VOID __cdecl PAGEABLE_PageFile_Cancel (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_PageFile_Test_IO_Valid (void) ;
BOOL __stdcall INIT_PageFile_Test_IO_Valid (void) ;
BOOL __stdcall DEBUG_PageFile_Test_IO_Valid (void) ;
BOOL __stdcall STATIC_PageFile_Test_IO_Valid (void) ;
BOOL __stdcall PAGEABLE_PageFile_Test_IO_Valid (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_PageFile_Get_Size_Info (PDWORD pFilesize, PDWORD pAbsMaxSize, PDWORD pMaxPossSize, PDWORD pMinSize, PPCHAR ppFilename) ;
BOOL __stdcall INIT_PageFile_Get_Size_Info (PDWORD pFilesize, PDWORD pAbsMaxSize, PDWORD pMaxPossSize, PDWORD pMinSize, PPCHAR ppFilename) ;
BOOL __stdcall DEBUG_PageFile_Get_Size_Info (PDWORD pFilesize, PDWORD pAbsMaxSize, PDWORD pMaxPossSize, PDWORD pMinSize, PPCHAR ppFilename) ;
BOOL __stdcall STATIC_PageFile_Get_Size_Info (PDWORD pFilesize, PDWORD pAbsMaxSize, PDWORD pMaxPossSize, PDWORD pMinSize, PPCHAR ppFilename) ;
BOOL __stdcall PAGEABLE_PageFile_Get_Size_Info (PDWORD pFilesize, PDWORD pAbsMaxSize, PDWORD pMaxPossSize, PDWORD pMinSize, PPCHAR ppFilename) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_PageSwap_Get_Version (PDWORD pVersion, PDWORD pMaxSize, PBYTE pPagerType) ;
BOOL __stdcall INIT_PageSwap_Get_Version (PDWORD pVersion, PDWORD pMaxSize, PBYTE pPagerType) ;
BOOL __stdcall DEBUG_PageSwap_Get_Version (PDWORD pVersion, PDWORD pMaxSize, PBYTE pPagerType) ;
BOOL __stdcall STATIC_PageSwap_Get_Version (PDWORD pVersion, PDWORD pMaxSize, PBYTE pPagerType) ;
BOOL __stdcall PAGEABLE_PageSwap_Get_Version (PDWORD pVersion, PDWORD pMaxSize, PBYTE pPagerType) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_PageSwap_Test_IO_Valid (void) ;
BOOL __stdcall INIT_PageSwap_Test_IO_Valid (void) ;
BOOL __stdcall DEBUG_PageSwap_Test_IO_Valid (void) ;
BOOL __stdcall STATIC_PageSwap_Test_IO_Valid (void) ;
BOOL __stdcall PAGEABLE_PageSwap_Test_IO_Valid (void) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_PageSwap_Read_Or_Write (BYTE RorW, DWORD filepos, PVOID pPage) ;
BOOL __stdcall INIT_PageSwap_Read_Or_Write (BYTE RorW, DWORD filepos, PVOID pPage) ;
BOOL __stdcall DEBUG_PageSwap_Read_Or_Write (BYTE RorW, DWORD filepos, PVOID pPage) ;
BOOL __stdcall STATIC_PageSwap_Read_Or_Write (BYTE RorW, DWORD filepos, PVOID pPage) ;
BOOL __stdcall PAGEABLE_PageSwap_Read_Or_Write (BYTE RorW, DWORD filepos, PVOID pPage) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_PageSwap_Grow_File (DWORD cPages, PDWORD pMaxSize) ;
DWORD __stdcall INIT_PageSwap_Grow_File (DWORD cPages, PDWORD pMaxSize) ;
DWORD __stdcall DEBUG_PageSwap_Grow_File (DWORD cPages, PDWORD pMaxSize) ;
DWORD __stdcall STATIC_PageSwap_Grow_File (DWORD cPages, PDWORD pMaxSize) ;
DWORD __stdcall PAGEABLE_PageSwap_Grow_File (DWORD cPages, PDWORD pMaxSize) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_PageSwap_Init_File (PBYTE pPagerType) ;
BOOL __stdcall INIT_PageSwap_Init_File (PBYTE pPagerType) ;
BOOL __stdcall DEBUG_PageSwap_Init_File (PBYTE pPagerType) ;
BOOL __stdcall STATIC_PageSwap_Init_File (PBYTE pPagerType) ;
BOOL __stdcall PAGEABLE_PageSwap_Init_File (PBYTE pPagerType) ;
#endif

DWORD __stdcall LOCK_Allocate_PM_App_CB_Area (DWORD theSize) ;
DWORD __stdcall INIT_Allocate_PM_App_CB_Area (DWORD theSize) ;
DWORD __stdcall DEBUG_Allocate_PM_App_CB_Area (DWORD theSize) ;
DWORD __stdcall STATIC_Allocate_PM_App_CB_Area (DWORD theSize) ;
DWORD __stdcall PAGEABLE_Allocate_PM_App_CB_Area (DWORD theSize) ;

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__Call_On_My_Stack (PVOID pFunc, DWORD lParam, PDWORD pTopOfStack, DWORD StackSizeBytes) ;
VOID __cdecl INIT__Call_On_My_Stack (PVOID pFunc, DWORD lParam, PDWORD pTopOfStack, DWORD StackSizeBytes) ;
VOID __cdecl DEBUG__Call_On_My_Stack (PVOID pFunc, DWORD lParam, PDWORD pTopOfStack, DWORD StackSizeBytes) ;
VOID __cdecl STATIC__Call_On_My_Stack (PVOID pFunc, DWORD lParam, PDWORD pTopOfStack, DWORD StackSizeBytes) ;
VOID __cdecl PAGEABLE__Call_On_My_Stack (PVOID pFunc, DWORD lParam, PDWORD pTopOfStack, DWORD StackSizeBytes) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__Debug_Printf_Service (PCHAR format,...) ;
VOID __cdecl INIT__Debug_Printf_Service (PCHAR format,...) ;
VOID __cdecl DEBUG__Debug_Printf_Service (PCHAR format,...) ;
VOID __cdecl STATIC__Debug_Printf_Service (PCHAR format,...) ;
VOID __cdecl PAGEABLE__Debug_Printf_Service (PCHAR format,...) ;
#endif

#ifdef WIN40_OR_LATER
PCHAR __cdecl LOCK__strupr (PCHAR psz) ;
PCHAR __cdecl INIT__strupr (PCHAR psz) ;
PCHAR __cdecl DEBUG__strupr (PCHAR psz) ;
PCHAR __cdecl STATIC__strupr (PCHAR psz) ;
PCHAR __cdecl PAGEABLE__strupr (PCHAR psz) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDD_Do_Physical_IO (DWORD OutData, VMHANDLE hVM, DWORD IOType, DWORD Port) ;
VOID __stdcall INIT_VDD_Do_Physical_IO (DWORD OutData, VMHANDLE hVM, DWORD IOType, DWORD Port) ;
VOID __stdcall DEBUG_VDD_Do_Physical_IO (DWORD OutData, VMHANDLE hVM, DWORD IOType, DWORD Port) ;
VOID __stdcall STATIC_VDD_Do_Physical_IO (DWORD OutData, VMHANDLE hVM, DWORD IOType, DWORD Port) ;
VOID __stdcall PAGEABLE_VDD_Do_Physical_IO (DWORD OutData, VMHANDLE hVM, DWORD IOType, DWORD Port) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VDD_Get_DISPLAYINFO (DISPLAYINFO * pDI, DWORD diSize) ;
BOOL __stdcall INIT_VDD_Get_DISPLAYINFO (DISPLAYINFO * pDI, DWORD diSize) ;
BOOL __stdcall DEBUG_VDD_Get_DISPLAYINFO (DISPLAYINFO * pDI, DWORD diSize) ;
BOOL __stdcall STATIC_VDD_Get_DISPLAYINFO (DISPLAYINFO * pDI, DWORD diSize) ;
BOOL __stdcall PAGEABLE_VDD_Get_DISPLAYINFO (DISPLAYINFO * pDI, DWORD diSize) ;
#endif

#ifdef WIN40_OR_LATER
PDWORD __stdcall LOCK_VDD_Get_Mini_Dispatch_Table (PDWORD pNumberOfFuncs) ;
PDWORD __stdcall INIT_VDD_Get_Mini_Dispatch_Table (PDWORD pNumberOfFuncs) ;
PDWORD __stdcall DEBUG_VDD_Get_Mini_Dispatch_Table (PDWORD pNumberOfFuncs) ;
PDWORD __stdcall STATIC_VDD_Get_Mini_Dispatch_Table (PDWORD pNumberOfFuncs) ;
PDWORD __stdcall PAGEABLE_VDD_Get_Mini_Dispatch_Table (PDWORD pNumberOfFuncs) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDD_Get_Special_VM_IDs (VMHANDLE * pBlueVM, VMHANDLE * pPlanarVM) ;
VOID __stdcall INIT_VDD_Get_Special_VM_IDs (VMHANDLE * pBlueVM, VMHANDLE * pPlanarVM) ;
VOID __stdcall DEBUG_VDD_Get_Special_VM_IDs (VMHANDLE * pBlueVM, VMHANDLE * pPlanarVM) ;
VOID __stdcall STATIC_VDD_Get_Special_VM_IDs (VMHANDLE * pBlueVM, VMHANDLE * pPlanarVM) ;
VOID __stdcall PAGEABLE_VDD_Get_Special_VM_IDs (VMHANDLE * pBlueVM, VMHANDLE * pPlanarVM) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDD_Get_VM_Info (VMHANDLE * pCRTCOwner, VMHANDLE * pMemCOwner) ;
VOID __stdcall INIT_VDD_Get_VM_Info (VMHANDLE * pCRTCOwner, VMHANDLE * pMemCOwner) ;
VOID __stdcall DEBUG_VDD_Get_VM_Info (VMHANDLE * pCRTCOwner, VMHANDLE * pMemCOwner) ;
VOID __stdcall STATIC_VDD_Get_VM_Info (VMHANDLE * pCRTCOwner, VMHANDLE * pMemCOwner) ;
VOID __stdcall PAGEABLE_VDD_Get_VM_Info (VMHANDLE * pCRTCOwner, VMHANDLE * pMemCOwner) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VDD_Register_Extra_Screen_Selector (WORD selector, VMHANDLE hVM) ;
BOOL __stdcall INIT_VDD_Register_Extra_Screen_Selector (WORD selector, VMHANDLE hVM) ;
BOOL __stdcall DEBUG_VDD_Register_Extra_Screen_Selector (WORD selector, VMHANDLE hVM) ;
BOOL __stdcall STATIC_VDD_Register_Extra_Screen_Selector (WORD selector, VMHANDLE hVM) ;
BOOL __stdcall PAGEABLE_VDD_Register_Extra_Screen_Selector (WORD selector, VMHANDLE hVM) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDD_Register_Virtual_Port (BYTE lengthened, DWORD iPort) ;
VOID __stdcall INIT_VDD_Register_Virtual_Port (BYTE lengthened, DWORD iPort) ;
VOID __stdcall DEBUG_VDD_Register_Virtual_Port (BYTE lengthened, DWORD iPort) ;
VOID __stdcall STATIC_VDD_Register_Virtual_Port (BYTE lengthened, DWORD iPort) ;
VOID __stdcall PAGEABLE_VDD_Register_Virtual_Port (BYTE lengthened, DWORD iPort) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDD_Set_Sleep_Flag_Addr (PBYTE pFlag) ;
VOID __stdcall INIT_VDD_Set_Sleep_Flag_Addr (PBYTE pFlag) ;
VOID __stdcall DEBUG_VDD_Set_Sleep_Flag_Addr (PBYTE pFlag) ;
VOID __stdcall STATIC_VDD_Set_Sleep_Flag_Addr (PBYTE pFlag) ;
VOID __stdcall PAGEABLE_VDD_Set_Sleep_Flag_Addr (PBYTE pFlag) ;
#endif

#ifdef WIN40_OR_LATER
BOOL __stdcall LOCK_VDD_Takeover_VGA_Port (DWORD iPort, PVOID * pPOldHandler, PIO_HANDLER pFunc, PIO_THUNK pThunk) ;
BOOL __stdcall INIT_VDD_Takeover_VGA_Port (DWORD iPort, PVOID * pPOldHandler, PIO_HANDLER pFunc, PIO_THUNK pThunk) ;
BOOL __stdcall DEBUG_VDD_Takeover_VGA_Port (DWORD iPort, PVOID * pPOldHandler, PIO_HANDLER pFunc, PIO_THUNK pThunk) ;
BOOL __stdcall STATIC_VDD_Takeover_VGA_Port (DWORD iPort, PVOID * pPOldHandler, PIO_HANDLER pFunc, PIO_THUNK pThunk) ;
BOOL __stdcall PAGEABLE_VDD_Takeover_VGA_Port (DWORD iPort, PVOID * pPOldHandler, PIO_HANDLER pFunc, PIO_THUNK pThunk) ;
#endif

PGeneric_HANDLER __stdcall LOCK_Init_Generic_Thunk (PGeneric_HANDLER pFunc, PGeneric_THUNK pThunk) ;
PGeneric_HANDLER __stdcall INIT_Init_Generic_Thunk (PGeneric_HANDLER pFunc, PGeneric_THUNK pThunk) ;
PGeneric_HANDLER __stdcall DEBUG_Init_Generic_Thunk (PGeneric_HANDLER pFunc, PGeneric_THUNK pThunk) ;
PGeneric_HANDLER __stdcall STATIC_Init_Generic_Thunk (PGeneric_HANDLER pFunc, PGeneric_THUNK pThunk) ;
PGeneric_HANDLER __stdcall PAGEABLE_Init_Generic_Thunk (PGeneric_HANDLER pFunc, PGeneric_THUNK pThunk) ;

#ifdef WIN40_OR_LATER
LISTHANDLE __cdecl LOCK__VCOMM_Enumerate_DevNodes (void) ;
LISTHANDLE __cdecl INIT__VCOMM_Enumerate_DevNodes (void) ;
LISTHANDLE __cdecl DEBUG__VCOMM_Enumerate_DevNodes (void) ;
LISTHANDLE __cdecl STATIC__VCOMM_Enumerate_DevNodes (void) ;
LISTHANDLE __cdecl PAGEABLE__VCOMM_Enumerate_DevNodes (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VDMAD_Get_Virt_Count (DMAHANDLE hDMA) ;
DWORD __stdcall INIT_VDMAD_Get_Virt_Count (DMAHANDLE hDMA) ;
DWORD __stdcall DEBUG_VDMAD_Get_Virt_Count (DMAHANDLE hDMA) ;
DWORD __stdcall STATIC_VDMAD_Get_Virt_Count (DMAHANDLE hDMA) ;
DWORD __stdcall PAGEABLE_VDMAD_Get_Virt_Count (DMAHANDLE hDMA) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDMAD_Set_Channel_Callbacks (DMAHANDLE hDMA, DWORD ANDmask, DWORD ORmask) ;
VOID __stdcall INIT_VDMAD_Set_Channel_Callbacks (DMAHANDLE hDMA, DWORD ANDmask, DWORD ORmask) ;
VOID __stdcall DEBUG_VDMAD_Set_Channel_Callbacks (DMAHANDLE hDMA, DWORD ANDmask, DWORD ORmask) ;
VOID __stdcall STATIC_VDMAD_Set_Channel_Callbacks (DMAHANDLE hDMA, DWORD ANDmask, DWORD ORmask) ;
VOID __stdcall PAGEABLE_VDMAD_Set_Channel_Callbacks (DMAHANDLE hDMA, DWORD ANDmask, DWORD ORmask) ;
#endif

#ifdef WIN40_OR_LATER
WORD __stdcall LOCK_VDMAD_Get_Phys_Status (void) ;
WORD __stdcall INIT_VDMAD_Get_Phys_Status (void) ;
WORD __stdcall DEBUG_VDMAD_Get_Phys_Status (void) ;
WORD __stdcall STATIC_VDMAD_Get_Phys_Status (void) ;
WORD __stdcall PAGEABLE_VDMAD_Get_Phys_Status (void) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VDMAD_Get_Phys_Count (DMAHANDLE hDMA) ;
DWORD __stdcall INIT_VDMAD_Get_Phys_Count (DMAHANDLE hDMA) ;
DWORD __stdcall DEBUG_VDMAD_Get_Phys_Count (DMAHANDLE hDMA) ;
DWORD __stdcall STATIC_VDMAD_Get_Phys_Count (DMAHANDLE hDMA) ;
DWORD __stdcall PAGEABLE_VDMAD_Get_Phys_Count (DMAHANDLE hDMA) ;
#endif

#ifdef WIN40_OR_LATER
PVOID __stdcall LOCK_VDMAD_Get_Max_Phys_Page (void) ;
PVOID __stdcall INIT_VDMAD_Get_Max_Phys_Page (void) ;
PVOID __stdcall DEBUG_VDMAD_Get_Max_Phys_Page (void) ;
PVOID __stdcall STATIC_VDMAD_Get_Max_Phys_Page (void) ;
PVOID __stdcall PAGEABLE_VDMAD_Get_Max_Phys_Page (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __stdcall LOCK_VDMAD_Set_Virt_Count (DMAHANDLE hDMA, DWORD count) ;
VOID __stdcall INIT_VDMAD_Set_Virt_Count (DMAHANDLE hDMA, DWORD count) ;
VOID __stdcall DEBUG_VDMAD_Set_Virt_Count (DMAHANDLE hDMA, DWORD count) ;
VOID __stdcall STATIC_VDMAD_Set_Virt_Count (DMAHANDLE hDMA, DWORD count) ;
VOID __stdcall PAGEABLE_VDMAD_Set_Virt_Count (DMAHANDLE hDMA, DWORD count) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__EnterMustComplete (void) ;
VOID __cdecl INIT__EnterMustComplete (void) ;
VOID __cdecl DEBUG__EnterMustComplete (void) ;
VOID __cdecl STATIC__EnterMustComplete (void) ;
VOID __cdecl PAGEABLE__EnterMustComplete (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__LeaveMustComplete (void) ;
VOID __cdecl INIT__LeaveMustComplete (void) ;
VOID __cdecl DEBUG__LeaveMustComplete (void) ;
VOID __cdecl STATIC__LeaveMustComplete (void) ;
VOID __cdecl PAGEABLE__LeaveMustComplete (void) ;
#endif

#ifdef WIN40_OR_LATER
VOID __cdecl LOCK__ResumeExecMustComplete (void) ;
VOID __cdecl INIT__ResumeExecMustComplete (void) ;
VOID __cdecl DEBUG__ResumeExecMustComplete (void) ;
VOID __cdecl STATIC__ResumeExecMustComplete (void) ;
VOID __cdecl PAGEABLE__ResumeExecMustComplete (void) ;
#endif

DWORD __cdecl LOCK_VTDI_Get_Version (void) ;
DWORD __cdecl INIT_VTDI_Get_Version (void) ;
DWORD __cdecl DEBUG_VTDI_Get_Version (void) ;
DWORD __cdecl STATIC_VTDI_Get_Version (void) ;
DWORD __cdecl PAGEABLE_VTDI_Get_Version (void) ;

PVOID __cdecl LOCK_VTDI_Get_Info (PCHAR TCPName) ;
PVOID __cdecl INIT_VTDI_Get_Info (PCHAR TCPName) ;
PVOID __cdecl DEBUG_VTDI_Get_Info (PCHAR TCPName) ;
PVOID __cdecl STATIC_VTDI_Get_Info (PCHAR TCPName) ;
PVOID __cdecl PAGEABLE_VTDI_Get_Info (PCHAR TCPName) ;

UNKNOWN __cdecl LOCK_VTDI_Start_Timer () ;
UNKNOWN __cdecl INIT_VTDI_Start_Timer () ;
UNKNOWN __cdecl DEBUG_VTDI_Start_Timer () ;
UNKNOWN __cdecl STATIC_VTDI_Start_Timer () ;
UNKNOWN __cdecl PAGEABLE_VTDI_Start_Timer () ;

UNKNOWN __cdecl LOCK_VTDI_Stop_Timer () ;
UNKNOWN __cdecl INIT_VTDI_Stop_Timer () ;
UNKNOWN __cdecl DEBUG_VTDI_Stop_Timer () ;
UNKNOWN __cdecl STATIC_VTDI_Stop_Timer () ;
UNKNOWN __cdecl PAGEABLE_VTDI_Stop_Timer () ;

UNKNOWN __stdcall LOCK_dummy_1034 (void) ;
UNKNOWN __stdcall INIT_dummy_1034 (void) ;
UNKNOWN __stdcall DEBUG_dummy_1034 (void) ;
UNKNOWN __stdcall STATIC_dummy_1034 (void) ;
UNKNOWN __stdcall PAGEABLE_dummy_1034 (void) ;

UNKNOWN __stdcall LOCK_dummy_1035 (void) ;
UNKNOWN __stdcall INIT_dummy_1035 (void) ;
UNKNOWN __stdcall DEBUG_dummy_1035 (void) ;
UNKNOWN __stdcall STATIC_dummy_1035 (void) ;
UNKNOWN __stdcall PAGEABLE_dummy_1035 (void) ;

UNKNOWN __cdecl LOCK_VTDI_Schedule_Event () ;
UNKNOWN __cdecl INIT_VTDI_Schedule_Event () ;
UNKNOWN __cdecl DEBUG_VTDI_Schedule_Event () ;
UNKNOWN __cdecl STATIC_VTDI_Schedule_Event () ;
UNKNOWN __cdecl PAGEABLE_VTDI_Schedule_Event () ;

UNKNOWN __cdecl LOCK_VTDI_Cancel_Event () ;
UNKNOWN __cdecl INIT_VTDI_Cancel_Event () ;
UNKNOWN __cdecl DEBUG_VTDI_Cancel_Event () ;
UNKNOWN __cdecl STATIC_VTDI_Cancel_Event () ;
UNKNOWN __cdecl PAGEABLE_VTDI_Cancel_Event () ;

UNKNOWN __cdecl LOCK_VTDI_Block () ;
UNKNOWN __cdecl INIT_VTDI_Block () ;
UNKNOWN __cdecl DEBUG_VTDI_Block () ;
UNKNOWN __cdecl STATIC_VTDI_Block () ;
UNKNOWN __cdecl PAGEABLE_VTDI_Block () ;

UNKNOWN __cdecl LOCK_VTDI_Signal () ;
UNKNOWN __cdecl INIT_VTDI_Signal () ;
UNKNOWN __cdecl DEBUG_VTDI_Signal () ;
UNKNOWN __cdecl STATIC_VTDI_Signal () ;
UNKNOWN __cdecl PAGEABLE_VTDI_Signal () ;

UNKNOWN __cdecl LOCK_VTDI_Register () ;
UNKNOWN __cdecl INIT_VTDI_Register () ;
UNKNOWN __cdecl DEBUG_VTDI_Register () ;
UNKNOWN __cdecl STATIC_VTDI_Register () ;
UNKNOWN __cdecl PAGEABLE_VTDI_Register () ;

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_VTDI_Unload () ;
UNKNOWN __cdecl INIT_VTDI_Unload () ;
UNKNOWN __cdecl DEBUG_VTDI_Unload () ;
UNKNOWN __cdecl STATIC_VTDI_Unload () ;
UNKNOWN __cdecl PAGEABLE_VTDI_Unload () ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_VTDI_Initialize () ;
UNKNOWN __cdecl INIT_VTDI_Initialize () ;
UNKNOWN __cdecl DEBUG_VTDI_Initialize () ;
UNKNOWN __cdecl STATIC_VTDI_Initialize () ;
UNKNOWN __cdecl PAGEABLE_VTDI_Initialize () ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_VTDI_Register_UnloadProc () ;
UNKNOWN __cdecl INIT_VTDI_Register_UnloadProc () ;
UNKNOWN __cdecl DEBUG_VTDI_Register_UnloadProc () ;
UNKNOWN __cdecl STATIC_VTDI_Register_UnloadProc () ;
UNKNOWN __cdecl PAGEABLE_VTDI_Register_UnloadProc () ;
#endif

#ifdef WIN40_OR_LATER
UNKNOWN __cdecl LOCK_VTDI_Register_LoadProc () ;
UNKNOWN __cdecl INIT_VTDI_Register_LoadProc () ;
UNKNOWN __cdecl DEBUG_VTDI_Register_LoadProc () ;
UNKNOWN __cdecl STATIC_VTDI_Register_LoadProc () ;
UNKNOWN __cdecl PAGEABLE_VTDI_Register_LoadProc () ;
#endif

#ifdef WIN40_OR_LATER
PVOID __stdcall LOCK_VPICD_Thunk_HWInt_Ex (PVPICD_HWInt_Ex_HANDLER Callback, PVPICD_HWInt_Ex_THUNK pThunk) ;
PVOID __stdcall INIT_VPICD_Thunk_HWInt_Ex (PVPICD_HWInt_Ex_HANDLER Callback, PVPICD_HWInt_Ex_THUNK pThunk) ;
PVOID __stdcall DEBUG_VPICD_Thunk_HWInt_Ex (PVPICD_HWInt_Ex_HANDLER Callback, PVPICD_HWInt_Ex_THUNK pThunk) ;
PVOID __stdcall STATIC_VPICD_Thunk_HWInt_Ex (PVPICD_HWInt_Ex_HANDLER Callback, PVPICD_HWInt_Ex_THUNK pThunk) ;
PVOID __stdcall PAGEABLE_VPICD_Thunk_HWInt_Ex (PVPICD_HWInt_Ex_HANDLER Callback, PVPICD_HWInt_Ex_THUNK pThunk) ;
#endif

#ifdef WIN40_OR_LATER
EVENTHANDLE __stdcall LOCK_Call_Restricted_Event_Ex (LONG boost, HANDLE hVMorThread, DWORD flags, PVOID refData, PRestrictedEventEx_HANDLER pCallback, DWORD timeout, PRestrictedEventEx_THUNK pThunk) ;
EVENTHANDLE __stdcall INIT_Call_Restricted_Event_Ex (LONG boost, HANDLE hVMorThread, DWORD flags, PVOID refData, PRestrictedEventEx_HANDLER pCallback, DWORD timeout, PRestrictedEventEx_THUNK pThunk) ;
EVENTHANDLE __stdcall DEBUG_Call_Restricted_Event_Ex (LONG boost, HANDLE hVMorThread, DWORD flags, PVOID refData, PRestrictedEventEx_HANDLER pCallback, DWORD timeout, PRestrictedEventEx_THUNK pThunk) ;
EVENTHANDLE __stdcall STATIC_Call_Restricted_Event_Ex (LONG boost, HANDLE hVMorThread, DWORD flags, PVOID refData, PRestrictedEventEx_HANDLER pCallback, DWORD timeout, PRestrictedEventEx_THUNK pThunk) ;
EVENTHANDLE __stdcall PAGEABLE_Call_Restricted_Event_Ex (LONG boost, HANDLE hVMorThread, DWORD flags, PVOID refData, PRestrictedEventEx_HANDLER pCallback, DWORD timeout, PRestrictedEventEx_THUNK pThunk) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __cdecl LOCK_NTKERN_Get_Version (void) ;
NTSTATUS __cdecl INIT_NTKERN_Get_Version (void) ;
NTSTATUS __cdecl DEBUG_NTKERN_Get_Version (void) ;
NTSTATUS __cdecl STATIC_NTKERN_Get_Version (void) ;
NTSTATUS __cdecl PAGEABLE_NTKERN_Get_Version (void) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __stdcall LOCK_NtKernCreateFile (PHANDLE FileHandle,ACCESS_MASK DesiredAccess,POBJECT_ATTRIBUTES ObjectAttributes,PIO_STATUS_BLOCK IoStatusBlock,PLARGE_INTEGER AllocationSize,ULONG FileAttributes,ULONG ShareAccess,ULONG CreateDisposition,ULONG CreateOptions,PVOID EaBuffer,ULONG EaLength) ;
NTSTATUS __stdcall INIT_NtKernCreateFile (PHANDLE FileHandle,ACCESS_MASK DesiredAccess,POBJECT_ATTRIBUTES ObjectAttributes,PIO_STATUS_BLOCK IoStatusBlock,PLARGE_INTEGER AllocationSize,ULONG FileAttributes,ULONG ShareAccess,ULONG CreateDisposition,ULONG CreateOptions,PVOID EaBuffer,ULONG EaLength) ;
NTSTATUS __stdcall DEBUG_NtKernCreateFile (PHANDLE FileHandle,ACCESS_MASK DesiredAccess,POBJECT_ATTRIBUTES ObjectAttributes,PIO_STATUS_BLOCK IoStatusBlock,PLARGE_INTEGER AllocationSize,ULONG FileAttributes,ULONG ShareAccess,ULONG CreateDisposition,ULONG CreateOptions,PVOID EaBuffer,ULONG EaLength) ;
NTSTATUS __stdcall STATIC_NtKernCreateFile (PHANDLE FileHandle,ACCESS_MASK DesiredAccess,POBJECT_ATTRIBUTES ObjectAttributes,PIO_STATUS_BLOCK IoStatusBlock,PLARGE_INTEGER AllocationSize,ULONG FileAttributes,ULONG ShareAccess,ULONG CreateDisposition,ULONG CreateOptions,PVOID EaBuffer,ULONG EaLength) ;
NTSTATUS __stdcall PAGEABLE_NtKernCreateFile (PHANDLE FileHandle,ACCESS_MASK DesiredAccess,POBJECT_ATTRIBUTES ObjectAttributes,PIO_STATUS_BLOCK IoStatusBlock,PLARGE_INTEGER AllocationSize,ULONG FileAttributes,ULONG ShareAccess,ULONG CreateDisposition,ULONG CreateOptions,PVOID EaBuffer,ULONG EaLength) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __stdcall LOCK_NtKernClose (HANDLE FileHandle) ;
NTSTATUS __stdcall INIT_NtKernClose (HANDLE FileHandle) ;
NTSTATUS __stdcall DEBUG_NtKernClose (HANDLE FileHandle) ;
NTSTATUS __stdcall STATIC_NtKernClose (HANDLE FileHandle) ;
NTSTATUS __stdcall PAGEABLE_NtKernClose (HANDLE FileHandle) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __stdcall LOCK_NtKernReadFile (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock,PVOID Buffer,ULONG Len,PLARGE_INTEGER ByteOffset,PULONG Key) ;
NTSTATUS __stdcall INIT_NtKernReadFile (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock,PVOID Buffer,ULONG Len,PLARGE_INTEGER ByteOffset,PULONG Key) ;
NTSTATUS __stdcall DEBUG_NtKernReadFile (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock,PVOID Buffer,ULONG Len,PLARGE_INTEGER ByteOffset,PULONG Key) ;
NTSTATUS __stdcall STATIC_NtKernReadFile (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock,PVOID Buffer,ULONG Len,PLARGE_INTEGER ByteOffset,PULONG Key) ;
NTSTATUS __stdcall PAGEABLE_NtKernReadFile (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock,PVOID Buffer,ULONG Len,PLARGE_INTEGER ByteOffset,PULONG Key) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __stdcall LOCK_NtKernWriteFile (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext,	PIO_STATUS_BLOCK IoStatusBlock,PVOID Buffer,ULONG Len,PLARGE_INTEGER ByteOffset,PULONG Key) ;
NTSTATUS __stdcall INIT_NtKernWriteFile (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext,	PIO_STATUS_BLOCK IoStatusBlock,PVOID Buffer,ULONG Len,PLARGE_INTEGER ByteOffset,PULONG Key) ;
NTSTATUS __stdcall DEBUG_NtKernWriteFile (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext,	PIO_STATUS_BLOCK IoStatusBlock,PVOID Buffer,ULONG Len,PLARGE_INTEGER ByteOffset,PULONG Key) ;
NTSTATUS __stdcall STATIC_NtKernWriteFile (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext,	PIO_STATUS_BLOCK IoStatusBlock,PVOID Buffer,ULONG Len,PLARGE_INTEGER ByteOffset,PULONG Key) ;
NTSTATUS __stdcall PAGEABLE_NtKernWriteFile (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine,PVOID ApcContext,	PIO_STATUS_BLOCK IoStatusBlock,PVOID Buffer,ULONG Len,PLARGE_INTEGER ByteOffset,PULONG Key) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __stdcall LOCK_NtKernDeviceIoControl (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength, PVOID OutputBuffer,ULONG OutputBufferLength) ;
NTSTATUS __stdcall INIT_NtKernDeviceIoControl (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength, PVOID OutputBuffer,ULONG OutputBufferLength) ;
NTSTATUS __stdcall DEBUG_NtKernDeviceIoControl (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength, PVOID OutputBuffer,ULONG OutputBufferLength) ;
NTSTATUS __stdcall STATIC_NtKernDeviceIoControl (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength, PVOID OutputBuffer,ULONG OutputBufferLength) ;
NTSTATUS __stdcall PAGEABLE_NtKernDeviceIoControl (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength, PVOID OutputBuffer,ULONG OutputBufferLength) ;
#endif

#ifdef WIN41_OR_LATER
ULONG __cdecl LOCK_NtKernGetWorkerThread (ULONG ThreadType) ;
ULONG __cdecl INIT_NtKernGetWorkerThread (ULONG ThreadType) ;
ULONG __cdecl DEBUG_NtKernGetWorkerThread (ULONG ThreadType) ;
ULONG __cdecl STATIC_NtKernGetWorkerThread (ULONG ThreadType) ;
ULONG __cdecl PAGEABLE_NtKernGetWorkerThread (ULONG ThreadType) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __stdcall LOCK_NtKernLoadDriver (PUNICODE_STRING DriverServiceName) ;
NTSTATUS __stdcall INIT_NtKernLoadDriver (PUNICODE_STRING DriverServiceName) ;
NTSTATUS __stdcall DEBUG_NtKernLoadDriver (PUNICODE_STRING DriverServiceName) ;
NTSTATUS __stdcall STATIC_NtKernLoadDriver (PUNICODE_STRING DriverServiceName) ;
NTSTATUS __stdcall PAGEABLE_NtKernLoadDriver (PUNICODE_STRING DriverServiceName) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __stdcall LOCK_NtKernQueueWorkItem (PWORK_QUEUE_ITEM workitem,WORK_QUEUE_TYPE worktype) ;
NTSTATUS __stdcall INIT_NtKernQueueWorkItem (PWORK_QUEUE_ITEM workitem,WORK_QUEUE_TYPE worktype) ;
NTSTATUS __stdcall DEBUG_NtKernQueueWorkItem (PWORK_QUEUE_ITEM workitem,WORK_QUEUE_TYPE worktype) ;
NTSTATUS __stdcall STATIC_NtKernQueueWorkItem (PWORK_QUEUE_ITEM workitem,WORK_QUEUE_TYPE worktype) ;
NTSTATUS __stdcall PAGEABLE_NtKernQueueWorkItem (PWORK_QUEUE_ITEM workitem,WORK_QUEUE_TYPE worktype) ;
#endif

#ifdef WIN41_OR_LATER
DEVNODE __cdecl LOCK_NtKernPhysicalDeviceObjectToDevNode (PDEVICE_OBJECT PhysicalDeviceObject) ;
DEVNODE __cdecl INIT_NtKernPhysicalDeviceObjectToDevNode (PDEVICE_OBJECT PhysicalDeviceObject) ;
DEVNODE __cdecl DEBUG_NtKernPhysicalDeviceObjectToDevNode (PDEVICE_OBJECT PhysicalDeviceObject) ;
DEVNODE __cdecl STATIC_NtKernPhysicalDeviceObjectToDevNode (PDEVICE_OBJECT PhysicalDeviceObject) ;
DEVNODE __cdecl PAGEABLE_NtKernPhysicalDeviceObjectToDevNode (PDEVICE_OBJECT PhysicalDeviceObject) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __stdcall LOCK_NtKernSetPhysicalCacheTypeRange (ULONG BaseAddressHigh, ULONG BaseAddressLow, ULONG NumberOfBytex, ULONG CacheType) ;
NTSTATUS __stdcall INIT_NtKernSetPhysicalCacheTypeRange (ULONG BaseAddressHigh, ULONG BaseAddressLow, ULONG NumberOfBytex, ULONG CacheType) ;
NTSTATUS __stdcall DEBUG_NtKernSetPhysicalCacheTypeRange (ULONG BaseAddressHigh, ULONG BaseAddressLow, ULONG NumberOfBytex, ULONG CacheType) ;
NTSTATUS __stdcall STATIC_NtKernSetPhysicalCacheTypeRange (ULONG BaseAddressHigh, ULONG BaseAddressLow, ULONG NumberOfBytex, ULONG CacheType) ;
NTSTATUS __stdcall PAGEABLE_NtKernSetPhysicalCacheTypeRange (ULONG BaseAddressHigh, ULONG BaseAddressLow, ULONG NumberOfBytex, ULONG CacheType) ;
#endif

#ifdef WIN41_OR_LATER
PDRIVER_OBJECT __cdecl LOCK_NtKernWin9XLoadDriver (PCHAR FileName,PCHAR RegisteryPath) ;
PDRIVER_OBJECT __cdecl INIT_NtKernWin9XLoadDriver (PCHAR FileName,PCHAR RegisteryPath) ;
PDRIVER_OBJECT __cdecl DEBUG_NtKernWin9XLoadDriver (PCHAR FileName,PCHAR RegisteryPath) ;
PDRIVER_OBJECT __cdecl STATIC_NtKernWin9XLoadDriver (PCHAR FileName,PCHAR RegisteryPath) ;
PDRIVER_OBJECT __cdecl PAGEABLE_NtKernWin9XLoadDriver (PCHAR FileName,PCHAR RegisteryPath) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __stdcall LOCK_NtKernCancelIoFile (HANDLE FileHandle,PIO_STATUS_BLOCK IoStatusBlock) ;
NTSTATUS __stdcall INIT_NtKernCancelIoFile (HANDLE FileHandle,PIO_STATUS_BLOCK IoStatusBlock) ;
NTSTATUS __stdcall DEBUG_NtKernCancelIoFile (HANDLE FileHandle,PIO_STATUS_BLOCK IoStatusBlock) ;
NTSTATUS __stdcall STATIC_NtKernCancelIoFile (HANDLE FileHandle,PIO_STATUS_BLOCK IoStatusBlock) ;
NTSTATUS __stdcall PAGEABLE_NtKernCancelIoFile (HANDLE FileHandle,PIO_STATUS_BLOCK IoStatusBlock) ;
#endif

#ifdef WIN41_OR_LATER
UNKNOWN __cdecl LOCK_NtKernGetVPICDHandleFromInterruptObj () ;
UNKNOWN __cdecl INIT_NtKernGetVPICDHandleFromInterruptObj () ;
UNKNOWN __cdecl DEBUG_NtKernGetVPICDHandleFromInterruptObj () ;
UNKNOWN __cdecl STATIC_NtKernGetVPICDHandleFromInterruptObj () ;
UNKNOWN __cdecl PAGEABLE_NtKernGetVPICDHandleFromInterruptObj () ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __stdcall LOCK_NtKernInternalDeviceIoControl (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength, PVOID OutputBuffer,ULONG OutputBufferLength) ;
NTSTATUS __stdcall INIT_NtKernInternalDeviceIoControl (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength, PVOID OutputBuffer,ULONG OutputBufferLength) ;
NTSTATUS __stdcall DEBUG_NtKernInternalDeviceIoControl (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength, PVOID OutputBuffer,ULONG OutputBufferLength) ;
NTSTATUS __stdcall STATIC_NtKernInternalDeviceIoControl (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength, PVOID OutputBuffer,ULONG OutputBufferLength) ;
NTSTATUS __stdcall PAGEABLE_NtKernInternalDeviceIoControl (HANDLE FileHandle,HANDLE Event,PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext,PIO_STATUS_BLOCK IoStatusBlock,ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength, PVOID OutputBuffer,ULONG OutputBufferLength) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __cdecl LOCK__KeSaveFloatingPointState (PKFLOATING_SAVE pkfs) ;
NTSTATUS __cdecl INIT__KeSaveFloatingPointState (PKFLOATING_SAVE pkfs) ;
NTSTATUS __cdecl DEBUG__KeSaveFloatingPointState (PKFLOATING_SAVE pkfs) ;
NTSTATUS __cdecl STATIC__KeSaveFloatingPointState (PKFLOATING_SAVE pkfs) ;
NTSTATUS __cdecl PAGEABLE__KeSaveFloatingPointState (PKFLOATING_SAVE pkfs) ;
#endif

#ifdef WIN41_OR_LATER
NTSTATUS __cdecl LOCK__KeRestoreFloatingPointState (PKFLOATING_SAVE pkfs) ;
NTSTATUS __cdecl INIT__KeRestoreFloatingPointState (PKFLOATING_SAVE pkfs) ;
NTSTATUS __cdecl DEBUG__KeRestoreFloatingPointState (PKFLOATING_SAVE pkfs) ;
NTSTATUS __cdecl STATIC__KeRestoreFloatingPointState (PKFLOATING_SAVE pkfs) ;
NTSTATUS __cdecl PAGEABLE__KeRestoreFloatingPointState (PKFLOATING_SAVE pkfs) ;
#endif

#ifdef WIN40_OR_LATER
DWORD __stdcall LOCK_VTD_Delay (DWORD Ticks) ;
DWORD __stdcall INIT_VTD_Delay (DWORD Ticks) ;
DWORD __stdcall DEBUG_VTD_Delay (DWORD Ticks) ;
DWORD __stdcall STATIC_VTD_Delay (DWORD Ticks) ;
DWORD __stdcall PAGEABLE_VTD_Delay (DWORD Ticks) ;
#endif

#ifdef WIN41_OR_LATER
DWORD __cdecl LOCK__PCI_Get_Version (void) ;
DWORD __cdecl INIT__PCI_Get_Version (void) ;
DWORD __cdecl DEBUG__PCI_Get_Version (void) ;
DWORD __cdecl STATIC__PCI_Get_Version (void) ;
DWORD __cdecl PAGEABLE__PCI_Get_Version (void) ;
#endif

#ifdef WIN41_OR_LATER
DWORD __cdecl LOCK__PCI_Read_Config (BYTE Bus, BYTE DevFunc, BYTE bOff) ;
DWORD __cdecl INIT__PCI_Read_Config (BYTE Bus, BYTE DevFunc, BYTE bOff) ;
DWORD __cdecl DEBUG__PCI_Read_Config (BYTE Bus, BYTE DevFunc, BYTE bOff) ;
DWORD __cdecl STATIC__PCI_Read_Config (BYTE Bus, BYTE DevFunc, BYTE bOff) ;
DWORD __cdecl PAGEABLE__PCI_Read_Config (BYTE Bus, BYTE DevFunc, BYTE bOff) ;
#endif

#ifdef WIN41_OR_LATER
VOID __cdecl LOCK__PCI_Write_Config (BYTE Bus, BYTE DevFunc, BYTE bOff, DWORD dwValue) ;
VOID __cdecl INIT__PCI_Write_Config (BYTE Bus, BYTE DevFunc, BYTE bOff, DWORD dwValue) ;
VOID __cdecl DEBUG__PCI_Write_Config (BYTE Bus, BYTE DevFunc, BYTE bOff, DWORD dwValue) ;
VOID __cdecl STATIC__PCI_Write_Config (BYTE Bus, BYTE DevFunc, BYTE bOff, DWORD dwValue) ;
VOID __cdecl PAGEABLE__PCI_Write_Config (BYTE Bus, BYTE DevFunc, BYTE bOff, DWORD dwValue) ;
#endif

#ifdef WIN41_OR_LATER
BOOL __cdecl LOCK__PCI_Lock_Unlock (DWORD dnDevNode, ULONG ulFlags) ;
BOOL __cdecl INIT__PCI_Lock_Unlock (DWORD dnDevNode, ULONG ulFlags) ;
BOOL __cdecl DEBUG__PCI_Lock_Unlock (DWORD dnDevNode, ULONG ulFlags) ;
BOOL __cdecl STATIC__PCI_Lock_Unlock (DWORD dnDevNode, ULONG ulFlags) ;
BOOL __cdecl PAGEABLE__PCI_Lock_Unlock (DWORD dnDevNode, ULONG ulFlags) ;
#endif

#ifdef WIN41_OR_LATER
DWORD __cdecl LOCK__PCCARD_Access_CIS_Memory (DWORD dnDevNode, PUCHAR pBuffer, DWORD dwOffset, DWORD dwLength, DWORD fFlags) ;
DWORD __cdecl INIT__PCCARD_Access_CIS_Memory (DWORD dnDevNode, PUCHAR pBuffer, DWORD dwOffset, DWORD dwLength, DWORD fFlags) ;
DWORD __cdecl DEBUG__PCCARD_Access_CIS_Memory (DWORD dnDevNode, PUCHAR pBuffer, DWORD dwOffset, DWORD dwLength, DWORD fFlags) ;
DWORD __cdecl STATIC__PCCARD_Access_CIS_Memory (DWORD dnDevNode, PUCHAR pBuffer, DWORD dwOffset, DWORD dwLength, DWORD fFlags) ;
DWORD __cdecl PAGEABLE__PCCARD_Access_CIS_Memory (DWORD dnDevNode, PUCHAR pBuffer, DWORD dwOffset, DWORD dwLength, DWORD fFlags) ;
#endif

#ifdef WIN41_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_DevNode_PowerCapabilities (DEVNODE dnDevNode, PULONG pulPowerCapabilities, ULONG ulFlags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_DevNode_PowerCapabilities (DEVNODE dnDevNode, PULONG pulPowerCapabilities, ULONG ulFlags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_DevNode_PowerCapabilities (DEVNODE dnDevNode, PULONG pulPowerCapabilities, ULONG ulFlags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_DevNode_PowerCapabilities (DEVNODE dnDevNode, PULONG pulPowerCapabilities, ULONG ulFlags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_DevNode_PowerCapabilities (DEVNODE dnDevNode, PULONG pulPowerCapabilities, ULONG ulFlags) ;
#endif

#ifdef WIN41_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Set_DevNode_PowerCapabilities (DEVNODE dnDevNode, ULONG ulPowerCapabilities, ULONG ulFlags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Set_DevNode_PowerCapabilities (DEVNODE dnDevNode, ULONG ulPowerCapabilities, ULONG ulFlags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Set_DevNode_PowerCapabilities (DEVNODE dnDevNode, ULONG ulPowerCapabilities, ULONG ulFlags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Set_DevNode_PowerCapabilities (DEVNODE dnDevNode, ULONG ulPowerCapabilities, ULONG ulFlags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Set_DevNode_PowerCapabilities (DEVNODE dnDevNode, ULONG ulPowerCapabilities, ULONG ulFlags) ;
#endif

#ifdef WIN41_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Get_DevNode_PowerState (DEVNODE dnDevNode, PFARULONG pulPowerState, ULONG ulFlags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Get_DevNode_PowerState (DEVNODE dnDevNode, PFARULONG pulPowerState, ULONG ulFlags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Get_DevNode_PowerState (DEVNODE dnDevNode, PFARULONG pulPowerState, ULONG ulFlags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Get_DevNode_PowerState (DEVNODE dnDevNode, PFARULONG pulPowerState, ULONG ulFlags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Get_DevNode_PowerState (DEVNODE dnDevNode, PFARULONG pulPowerState, ULONG ulFlags) ;
#endif

#ifdef WIN41_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Set_DevNode_PowerState (DEVNODE dnDevNode, ULONG ulPowerState,   ULONG ulFlags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Set_DevNode_PowerState (DEVNODE dnDevNode, ULONG ulPowerState,   ULONG ulFlags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Set_DevNode_PowerState (DEVNODE dnDevNode, ULONG ulPowerState,   ULONG ulFlags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Set_DevNode_PowerState (DEVNODE dnDevNode, ULONG ulPowerState,   ULONG ulFlags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Set_DevNode_PowerState (DEVNODE dnDevNode, ULONG ulPowerState,   ULONG ulFlags) ;
#endif

#ifdef WIN41_OR_LATER
CONFIGRET __cdecl LOCK__CONFIGMG_Register_DevNode_For_Idle_Detection (  DEVNODE dnDevNode, ULONG ulConservationTime,   ULONG ulPerformanceTime, PULONG pulCounterVariable,   ULONG ulState,   ULONG ulFlags) ;
CONFIGRET __cdecl INIT__CONFIGMG_Register_DevNode_For_Idle_Detection (  DEVNODE dnDevNode, ULONG ulConservationTime,   ULONG ulPerformanceTime, PULONG pulCounterVariable,   ULONG ulState,   ULONG ulFlags) ;
CONFIGRET __cdecl DEBUG__CONFIGMG_Register_DevNode_For_Idle_Detection (  DEVNODE dnDevNode, ULONG ulConservationTime,   ULONG ulPerformanceTime, PULONG pulCounterVariable,   ULONG ulState,   ULONG ulFlags) ;
CONFIGRET __cdecl STATIC__CONFIGMG_Register_DevNode_For_Idle_Detection (  DEVNODE dnDevNode, ULONG ulConservationTime,   ULONG ulPerformanceTime, PULONG pulCounterVariable,   ULONG ulState,   ULONG ulFlags) ;
CONFIGRET __cdecl PAGEABLE__CONFIGMG_Register_DevNode_For_Idle_Detection (  DEVNODE dnDevNode, ULONG ulConservationTime,   ULONG ulPerformanceTime, PULONG pulCounterVariable,   ULONG ulState,   ULONG ulFlags) ;
#endif

