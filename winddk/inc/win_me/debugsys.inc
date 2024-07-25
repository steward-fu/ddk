ifndef _DEBUGSYS_H
_DEBUGSYS_H	EQU	1
D386_RM_Int	EQU	68H
D386_Id	EQU	0F386H
D386_MIN	EQU	43H
D386_Identify	EQU	43H
D386_Prepare_PMode	EQU	44H
D386_Real_Mode_Init	EQU	45H
D386_Set_Switches	EQU	46H
D386_Execute_Cond	EQU	47H
D386_Free_Segment	EQU	48H
D386_Set_Baudrate	EQU	49H
D386_Reinit	EQU	4aH
D386_Def_Deb_Segs	EQU	4bH
D386_Set_Com_Port	EQU	4cH
D386_Link_Sym	EQU	4dH
D386_Unlink_Sym	EQU	4eH
D386_Remove_Segs	EQU	4fH
D386_Load_Segment	EQU	50H
D386_Display_Char	EQU	51H
D386_Display_Str	EQU	52H
D386_IsVxDInstalled	EQU	53H
D386_VxDInstall	EQU	54H
D386_RegisterDotCmd	EQU	55H
D386_DeRegisterDotCmd	EQU	56H
D386_Printf	EQU	57H
D386_Link_Sym_Phys	EQU	58H
D386_CheckMap	EQU	59H
D386_SetAutoLoadSym	EQU	5aH
D386_SetTeftiPort	EQU	5bH
D386_ExecDebugCommand	EQU	5cH
D386_LoadCodeDataHigh	EQU	5dH
D386_SetWinVersion	EQU	5eH
D386_ScanChar	EQU	5fH
D386_UnGetChar	EQU	60H
D386_Stop	EQU	61H
ifdef WDEB98
D386_Set_Baudrate_Ex	EQU	62H
D386_SetBuildInfo	EQU	63H
D386_Set_Switches_Ex	EQU	64H
D386_MAX	EQU	64H
else
D386_MAX	EQU	61H
endif
ST_code_sel	EQU	0H
ST_data_sel	EQU	1H
ST_code_seg	EQU	10H
ST_data_seg	EQU	11H
ST_dual_code	EQU	40H
ST_dual_data	EQU	41H
ST_device_code	EQU	80H
ST_device_data	EQU	81H
D386_Device_Params      STRUC
DD_logical_seg          DW      ?
DD_actual_sel           DW      ?
DD_base                 DD      ?
DD_length               DD      ?
DD_name                 DF      ?
DD_sym_name             DF      ?
DD_alias_sel            DW      ?
D386_Device_Params      ENDS

WDEB_INT2F_STARTING	EQU	0H
WDEB_INT2F_ENDING	EQU	1H
WDEB_INT2F_NESTED_STARTING	EQU	2H
WDEB_INT2F_NESTED_ENDING	EQU	3H
PMINIT_INIT_IDT	EQU	0H
PMINIT_INIT_PAGING	EQU	1H
PMINIT_ENABLE_DEBUG_QUERYS	EQU	2H
PMINIT_INIT_SPARE_PTE	EQU	3H
PMINIT_SET_ENTER_EXIT_VMM	EQU	4H
PMINIT_GET_SIZE_PHYS	EQU	5H
PMINIT_SET_BASE_SPARE_PTE	EQU	6H
PMINIT_ENABLE_MEMORY_CONTEXT	EQU	7H
PMINIT_MAX	EQU	7H
WdebVCPIInfo    STRUC
fnVCPI  DF      ?
rdsVCPI DW      ?
laVTP   DD      ?
Port67  DW      ?
WdebVCPIInfo    ENDS


VTP	STRUC
zaCr3VTP	DD	?
laGdtrVTP	DD	?
laIdtrVTP	DD	?
selLdtVTP	DW	?
selTrVTP	DW	?
ipVTP	DW	?
unusedVTP	DW	?
csVTP	DW	?
VTP	ENDS
VCPI_RM_CALLOUT_INT	EQU	67H
VCPI_PROT_ENTRY	EQU	0DE0CH
Debug_Serv_Int	EQU	41H
DS_Out_Char	EQU	0H
DS_In_Char	EQU	1H
DS_Out_Str	EQU	2H
DS_Is_Char	EQU	3H
DS_Out_Str16	EQU	12H
DS_ForcedGO16	EQU	40H
DS_LinkMap	EQU	45H
DS_UnlinkMap	EQU	46H
DS_CheckMap	EQU	47H
DS_IsAutoLoadSym	EQU	48H
DS_DebLoaded	EQU	4FH
DS_DebPresent	EQU	0F386H
DS_LoadSeg	EQU	50H
DS_LoadSeg_32	EQU	0150H
DS_MoveSeg	EQU	51H
DS_FreeSeg	EQU	52H
DS_FreeSeg_32	EQU	0152H
DS_DGH	EQU	56H
DS_DFL	EQU	57H
DS_DLL	EQU	58H
DS_StartTask	EQU	59H
DS_Kernel_Vars	EQU	5aH
DS_VCPI_Notify	EQU	5bH
DS_ReleaseSeg	EQU	5cH
DS_User_Vars	EQU	5dH
DS_POSTLOAD	EQU	60H
DS_EXITCALL	EQU	62H
DS_INT2	EQU	63H
DS_LOADDLL	EQU	64H
DS_DELMODULE	EQU	65H
DS_LOGERROR	EQU	66H
DS_LOGPARAMERROR	EQU	67H
DS_RIN	EQU	09H
DS_BANKLINE	EQU	0AH
DS_NEWTASK	EQU	0BH
DS_FLUSHTASK	EQU	0CH
DS_SWITCHOUT	EQU	0DH
DS_SWITCHIN	EQU	0EH
DS_IntRings	EQU	20H
DS_IncludeSegs	EQU	21H
MaxDebugSegs	EQU	20
DS_CondBP	EQU	0F001H
DS_ForcedBP	EQU	0F002H
DS_ForcedGO	EQU	0F003H
DS_HardINT1	EQU	0F004H
DS_Out_Symbol	EQU	0FH
DS_Disasm_Ins	EQU	10H
DS_JumpTableStart	EQU	70H
DS_RegisterDotCommand	EQU	70H
DS_RegisterDotCommand16	EQU	71H
DS_DeRegisterDotCommand	EQU	72H
DS_Printf	EQU	73H
DS_Printf16	EQU	74H
DS_GetRegisterSet	EQU	75H
DS_SetAlternateRegisterSet	EQU	76H
DS_GetCommandLineChar	EQU	77H
DS_EvaluateExpression	EQU	78H
DS_VerifyMemory	EQU	79H
DS_PrintRegisters	EQU	7aH
DS_PrintStackDump	EQU	7bH
DS_SetThreadID	EQU	7cH
DS_ExecDebugCommand	EQU	7dH
DS_GetDebuggerInfo	EQU	7eH
DS_CheckFault	EQU	7fH
DS_SetBreak	EQU	80H
DS_RedirectExec	EQU	81H
DS_PassOnDebugCommand	EQU	82H
DS_TrapFault	EQU	83H
DS_SetStackTraceCallBack	EQU	84H
DS_RemoveSegs	EQU	85H
DS_DefineDebugSegs	EQU	86H
DS_SetBaudRate	EQU	87H
DS_SetComPort	EQU	88H
DS_ChangeTaskNum	EQU	89H
DS_ExitCleanup	EQU	8aH
DS_InstallVGAHandler	EQU	8bH
DS_GetComBase	EQU	8cH
DS_GetSymbol	EQU	8dH
DS_CopyMem	EQU	8eH
ifdef WDEB98
DS_Reserved0	EQU	8fH
DS_IsCompatibleWDeb	EQU	90H
DS_SetBaudRateEx	EQU	91H
DS_GetSetDebuggerState	EQU	92H
DS_TestDebugComPort	EQU	93H
DS_Reserved4	EQU	94H
DS_Reserved5	EQU	95H
DS_Reserved6	EQU	96H
DS_Reserved7	EQU	97H
DS_Reserved8	EQU	98H
DS_Reserved9	EQU	99H
DS_Reserved10	EQU	9AH
DS_Reserved11	EQU	9BH
DS_Reserved12	EQU	9CH
DS_Reserved13	EQU	9DH
DS_Reserved14	EQU	9EH
DS_Reserved15	EQU	9FH
DS_InstallVxDThunk	EQU	0A0H
DS_ThunkDownTo16	EQU	0A1H
DS_JumpTableEnd	EQU	0A1H
else
DS_JumpTableEnd	EQU	8eH
endif

SaveRegs_Struc	STRUC
Debug_EAX	DD	?
Debug_EBX	DD	?
Debug_ECX	DD	?
Debug_EDX	DD	?
Debug_ESP	DD	?
Debug_EBP	DD	?
Debug_ESI	DD	?
Debug_EDI	DD	?
Debug_ES	DW	?
Debug_SS	DW	?
Debug_DS	DW	?
Debug_FS	DW	?
Debug_GS	DW	?
Debug_EIP	DD	?
Debug_CS	DW	?
dwReserved	DD	?
Debug_EFlags	DD	?
Debug_CR0	DD	?
Debug_GDT	DB	SIZE QWORD DUP (?)
Debug_IDT	DB	SIZE QWORD DUP (?)
Debug_LDT	DW	?
Debug_TR	DW	?
Debug_CR2	DD	?
Debug_CR3	DD	?
Debug_DR0	DD	?
Debug_DR1	DD	?
Debug_DR2	DD	?
Debug_DR3	DD	?
Debug_DR6	DD	?
Debug_DR7	DD	?
Debug_DR7_2	DD	?
Debug_TR6	DD	?
Debug_TR7	DD	?
Debug_TrapNumber	DW	?
Debug_ErrorCode	DW	?
SaveRegs_Struc	ENDS
DebInfoBuf      STRUC
DIB_MajorVersion        DB      ?
DIB_MinorVersion        DB      ?
DIB_Revision    DB      ?
DIB_Reserved    DB      ?
DIB_DebugTrap16 DD      ?
DIB_DebugTrap32 DF      ?
DIB_DebugBreak16        DD      ?
DIB_DebugBreak32        DF      ?
DIB_DebugCtrlC16        DD      ?
DIB_DebugCtrlC32        DF      ?
DebInfoBuf      ENDS



BreakStruc	STRUC
BS_BreakEIP	DD	?
BS_BreakCS	DW	?
BS_BreakESP	DD	?
BS_BreakSS	DW	?
BreakStruc	ENDS
RedirectExecStruc       STRUC
RDE_fpbufDebugCommand   DF      ?
RDE_cbDebugCommand      DW      ?
RDE_fpszInput   DF      ?
RDE_usFlags     DW      ?
RDE_cbOutput    DD      ?
RDE_fpbufOutput DF      ?
RedirectExecStruc       ENDS

REPEAT_FOREVER_CHAR	EQU	0feH

AddrS	STRUC
AddrOff	DD	?
AddrSeg	DW	?
AddrType	DB	?
AddrSize	DB	?
AddrTask	DW	?
AddrS	ENDS
EXPR_TYPE_SEG	EQU	0001H
EXPR_TYPE_SEL	EQU	0009H
EXPR_TYPE_LIN	EQU	0002H
EXPR_TYPE_PHY	EQU	000AH
EXPR_TYPE_LOG	EQU	0008H
EXPR_TYPE_MOD	EQU	000BH
DEBUG_FAULT_TYPE_V86	EQU	0001H
DEBUG_FAULT_TYPE_PM	EQU	0002H
DEBUG_FAULT_TYPE_RING0	EQU	0004H
DEBUG_FAULT_TYPE_FIRST	EQU	0008H
DEBUG_FAULT_TYPE_LAST	EQU	0010H
Win386_Query_Int	EQU	22H
Win386_Alive	EQU	0
Win386_Q_Ack	EQU	0F386H
Win386_Query	EQU	1
Win386_PhysToLinr	EQU	2
Win386_AddrValid	EQU	3
Win386_MapVM	EQU	4
Win386_UnmapVM	EQU	5
Win386_GetDLAddr	EQU	6
Win386_GetVXDName	EQU	7
Win386_GetPDE	EQU	8
Win386_GetFrame	EQU	9
Win386_GetLDTAddress	EQU	10
Win386_GetThreadID	EQU	11
Win386_GetTSHandler	EQU	12
Win386_GetArplHandler	EQU	13
Max_Win386_Services	EQU	13
endif
