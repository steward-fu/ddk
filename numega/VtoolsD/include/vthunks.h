//		Vthunks.h
//
//	Definitions for thunking
//
//	Copyright (c) 1994 by Compuware Corporation
//

#ifndef __vthunks_h_
#define __vthunks_h_


//	DeclareThunk generates the following typedefs (example)
//
// typedef V86Int_HANDLER *PV86Int_HANDLER;
// typedef DWORD V86Int_THUNK[4];
// typedef V86Int_THUNK *PV86Int_THUNK;


#define DeclareThunk(name, tsize) \
	typedef name##_HANDLER *P##name##_HANDLER;	\
	typedef DWORD name##_THUNK[tsize];		\
	typedef name##_THUNK *P##name##_THUNK;



typedef VOID  __stdcall Generic_HANDLER(PDSFRAME pDSF);

DeclareThunk(Generic, 4)


typedef BOOL __stdcall V86Int_HANDLER(DWORD Intnum,
				      VMHANDLE hVM,
				      PCLIENT_STRUCT pRegs);

DeclareThunk(V86Int, 7)

typedef BOOL __stdcall IPF_HANDLER(VMHANDLE hVM,
				   PIPF_DATA ipf);
DeclareThunk(IPF, 4)

typedef VOID __stdcall V86Call_HANDLER(VMHANDLE hVM,
				       PVOID RefData,
				       PCLIENT_STRUCT pRegs);
DeclareThunk(V86Call, 3)

typedef VOID __stdcall PMCall_HANDLER(VMHANDLE hVM,
				      PVOID RefData,
				      PCLIENT_STRUCT pRegs);
DeclareThunk(PMCall, 3)

typedef VOID __stdcall ExecVM_HANDLER(VMHANDLE hVM,
				      PVOID RefData,
				      PCLIENT_STRUCT pRegs);
DeclareThunk(ExecVM, 3)

//typedef VOID __stdcall GlobalEvent_HANDLER(VMHANDLE hVM,
//					   PVOID RefData,
//					   PCLIENT_STRUCT pRegs);
//DeclareThunk(GlobalEvent, 3)

typedef PVOID __stdcall VMMFault_HANDLER(VMHANDLE hVM,
					 PVOID Stackframe);
DeclareThunk(VMMFault, 13)

typedef PVOID __stdcall V86Fault_HANDLER(VMHANDLE hVM,
					 PCLIENT_STRUCT Stackframe);
DeclareThunk(V86Fault, 13)

typedef PVOID __stdcall PMFault_HANDLER(VMHANDLE hVM,
					PCLIENT_STRUCT Stackframe);
DeclareThunk(PMFault, 13)

typedef PVOID __stdcall V86Api_HANDLER(VMHANDLE hVM,
				       PCLIENT_STRUCT pRegs);
DeclareThunk(V86Api, 3)

typedef PVOID __stdcall PMApi_HANDLER(VMHANDLE hVM,
				      PCLIENT_STRUCT pRegs);
DeclareThunk(PMApi, 3)

typedef VOID __stdcall V86Break_HANDLER(VMHANDLE hVM,
					 PCLIENT_STRUCT pRegs,
					 PVOID Refdata,
					 SEGOFFSET BreakAddress,
					 PVOID BreakLinearAddress);
DeclareThunk(V86Break, 3)

typedef BOOL __stdcall Idle_HANDLER(VMHANDLE hVM,
				    PCLIENT_STRUCT pRegs);
DeclareThunk(Idle, 4)


typedef VOID __stdcall DeviceService_HANDLER(PDSFRAME pDS);
DeclareThunk(DeviceService, 7)

typedef BOOL __stdcall DeviceServiceEx_HANDLER(PDSFRAME pDS);
#ifdef __BORLANDC__
DeclareThunk(DeviceServiceEx, 10)
#else
DeclareThunk(DeviceServiceEx, 9)
#endif

typedef VOID __stdcall VKDStartPaste_HANDLER(VMHANDLE hVM,
					     PVOID Refdata,
					     DWORD Flags);
DeclareThunk(VKDStartPaste, 3)

typedef VOID __stdcall VKDHotkey_HANDLER(DWORD Key,
					 HOTKEYHANDLE hHotkey,
					 DWORD ShiftState,
					 PVOID Refdata,
					 DWORD DelayTime);
DeclareThunk(VKDHotkey, 3)
					     
typedef VOID __stdcall VDMADVirtualize_HANDLER(VMHANDLE hVM,
					       DMAHANDLE hDMA);
DeclareThunk(VDMADVirtualize, 3)

typedef VOID __stdcall SHELLEvent_HANDLER(BOOL bPosted,
					  PVOID Refdata,
					  PCLIENT_STRUCT pRegs);
DeclareThunk(SHELLEvent, 4)
					  
typedef VOID __stdcall SHELLMessage_HANDLER(DWORD ResponseCode,
					    PVOID Refdata);
DeclareThunk(SHELLMessage, 3)

typedef VOID __stdcall Event_HANDLER(VMHANDLE hVM,
				     PVOID Refdata,
				     PCLIENT_STRUCT pRegs);
DeclareThunk(Event, 3)

typedef VOID __stdcall VMReturns_HANDLER(VMHANDLE hVM,
					PVOID Refdata,
					PCLIENT_STRUCT pRegs,
					DWORD Flags);
DeclareThunk(VMReturns, 3)

typedef VOID __stdcall PriorityVMEvent_HANDLER(VMHANDLE hVM,
					       PVOID Refdata,
					       PCLIENT_STRUCT pRegs,
					       DWORD Flags);
DeclareThunk(PriorityVMEvent, 3)

typedef VOID __stdcall TaskSwitched_HANDLER(VMHANDLE hVM,
					    VMHANDLE hOldVM);
DeclareThunk(TaskSwitched, 3)

typedef PVOID __stdcall HWInt_HANDLER(VMHANDLE hVM,
				      IRQHANDLE hIRQ);
DeclareThunk(HWInt, 4)

typedef PVOID __stdcall NMI_HANDLER(VMHANDLE hVM,
				    PCLIENT_STRUCT pRegs);
DeclareThunk(NMI, 3)
//typedef PVOID __stdcall NMI_HANDLER(void);
//DeclareThunk(NMI, 2)

typedef VOID __stdcall V86Page_HANDLER(VMHANDLE hVM,
				       DWORD PageNumber);
DeclareThunk(V86Page, 3)

typedef VOID __stdcall NMIEvent_HANDLER(VMHANDLE hVM,
					PCLIENT_STRUCT pRegs);
DeclareThunk(NMIEvent, 3)

typedef VOID __stdcall VPICD_EOI_HANDLER(VMHANDLE hVM,
					 IRQHANDLE hIRQ);
DeclareThunk(VPICD_EOI, 3)

typedef BOOL __stdcall VPICD_HWInt_HANDLER(VMHANDLE hVM,
					   IRQHANDLE hIRQ);
DeclareThunk(VPICD_HWInt, 4)

#ifdef WIN40					   
typedef BOOL __stdcall VPICD_HWInt_Ex_HANDLER(VMHANDLE hVM,
					   IRQHANDLE hIRQ, DWORD RefData);
DeclareThunk(VPICD_HWInt_Ex, 4)
#endif
					   
typedef VOID __stdcall VPICD_VirtInt_HANDLER(VMHANDLE hVM,
					     IRQHANDLE hIRQ);
DeclareThunk(VPICD_VirtInt, 3)

typedef VOID __stdcall VPICD_Iret_HANDLER(VMHANDLE hVM,
					  IRQHANDLE hIRQ,
					  BOOL bTimedOut);
DeclareThunk(VPICD_Iret, 4)

typedef VOID __stdcall VPICD_MaskChange_HANDLER(VMHANDLE hVM,
						IRQHANDLE hIRQ,
						BOOL bMasking);
DeclareThunk(VPICD_MaskChange, 3)

typedef DWORD __stdcall IO_HANDLER(VMHANDLE hVM,
				   DWORD IOType,
				   DWORD Port,
				   PCLIENT_STRUCT pRegs,
				   DWORD Data);
DeclareThunk(IO, 3)
				   
typedef VOID __stdcall EXCEPTION_HANDLER(PDSFRAME pDS);
DeclareThunk(EXCEPTION, 4)

typedef VOID __stdcall TIMEOUT_HANDLER(VMHANDLE hVM,
				       PCLIENT_STRUCT pRegs,
				       PVOID refdata,
				       DWORD Extra);
DeclareThunk(TIMEOUT, 3)

typedef VOID __stdcall THREADTIMEOUT_HANDLER(THREADHANDLE hThread,
				       VMHANDLE hVM,
				       PCLIENT_STRUCT pRegs,
				       PVOID refdata,
				       DWORD Extra);
DeclareThunk(THREADTIMEOUT, 3)


typedef VOID __stdcall ASYNCTIMEOUT_HANDLER(PVOID refdata, DWORD Extra);
DeclareThunk(ASYNCTIMEOUT, 3)

typedef VOID __stdcall THREADSWITCH_HANDLER(THREADHANDLE hOldThread, 
				       THREADHANDLE hCurThread);

DeclareThunk(THREADSWITCH, 3)

typedef VOID __stdcall RestrictedEvent_HANDLER(VMHANDLE hVM,
				     THREADHANDLE hThread,
				     PVOID Refdata,
				     PCLIENT_STRUCT pRegs);
DeclareThunk(RestrictedEvent, 3)

typedef VOID __stdcall RestrictedEventEx_HANDLER(
					 VMHANDLE hVM,
				     THREADHANDLE hThread,
				     PVOID Refdata,
				     PCLIENT_STRUCT pRegs,
					 BOOL bTimedOut
				     );
DeclareThunk(RestrictedEventEx, 4)


typedef VOID __stdcall ThreadEvent_HANDLER(VMHANDLE hVM,
				     THREADHANDLE hThread,
				     PVOID Refdata,
				     PCLIENT_STRUCT pRegs);
DeclareThunk(ThreadEvent, 3)

typedef BOOL __stdcall FREEPHYS_HANDLER(DWORD nPages, BOOL bPagesNeeded);
DeclareThunk(FREEPHYS, 4)

typedef VOID __stdcall BLOCKDEV_HW_Int_HANDLER(PBDD pBdd, VMHANDLE hVM,
					 IRQHANDLE hIRQ);
DeclareThunk(BLOCKDEV_HW_Int, 3)

typedef VOID __stdcall BLOCKDEV_Sync_Cmd_HANDLER(PALLREGS pAR);
DeclareThunk(BLOCKDEV_Sync_Cmd, 3)
					   
typedef VOID __stdcall BLOCKDEV_Command_HANDLER(PBDD pBdd, PBCB pBcb);
DeclareThunk(BLOCKDEV_Command, 3)

typedef VOID (_cdecl *PPostMessage_HANDLER)(DWORD dwPostMessageReturnCode, PVOID refdata);

#ifdef WFW311_OR_WIN40
typedef PPORTDATA _cdecl COMMPORTOPENFUNC(PCHAR szPortName, VMHANDLE hVM, PDWORD pError);
typedef COMMPORTOPENFUNC* PCOMMPORTOPENFUNC;

#ifndef DEVNODE
#define DEVNODE DWORD
#endif

typedef VOID DRIVERCTRLFUNC(DWORD driverFunc, DEVNODE node, DWORD refData, ...);
typedef DRIVERCTRLFUNC* PDRIVERCTRLFUNC;

typedef VOID _cdecl COMMNOTIFYPROC(PPORTDATA hPort, PVOID refData, DWORD event, DWORD subEvent);
typedef COMMNOTIFYPROC* PCOMMNOTIFYPROC;

#endif


#ifdef WIN40
typedef CONFIGRET _cdecl ENUMHANDLER(DWORD cfFunc, DWORD cfSubFunc, DEVNODE toNode, DEVNODE aboutNode, DWORD flags);
typedef ENUMHANDLER* PENUMHANDLER;
#endif

#ifdef WIN40
typedef DWORD _stdcall CursorProc_HANDLER(DWORD FunctionNumber, VMHANDLE hVM);
DeclareThunk(CursorProc, 4)
#endif

#ifdef WIN40
typedef PVOID __cdecl IFSREQ_HANDLER(VMHANDLE hVM, DWORD funcID, PCLIENT_STRUCT pRegs, PDWORD pProviderID);
#ifdef __BORLANDC__
DeclareThunk(IFSREQ, 9);
#else
DeclareThunk(IFSREQ, 8);
#endif
#endif

#ifdef WIN40
typedef PVOID __cdecl IFSPATH_HANDLER(PDWORD pProviderID, PWORD* ppLastConsumed, PDWORD pLenLastConsumed, PWORD* ppSourcePath, PWORD* ppDestPath);
#ifdef __BORLANDC__
DeclareThunk(IFSPATH, 14);
#else
DeclareThunk(IFSPATH, 13);
#endif
#endif

#ifdef WIN40
typedef VOID __stdcall PropHook_HANDLER(VMHANDLE hVM, PDWORD pData, PDWORD pOrdinal);
DeclareThunk(PropHook, 3);
#endif //WIN40

#define HDSC_ThunkSize 5

typedef DWORD HDSC_Thunk[HDSC_ThunkSize];
typedef HDSC_Thunk* PHDSC_Thunk;

typedef VOID (*PVOIDFUNC)(void);
typedef VOID (*PVOIDFUNCOFDW)(DWORD);

#ifdef WIN40
// Changes to ref data parameters by handler are not discarded - they will stay in effect
typedef VOID  __cdecl CacheEnum_HANDLER(HANDLE hCacheBlock, DWORD ref1, DWORD ref2, DWORD ref3, BOOL bBlockIsFree);
DeclareThunk(CacheEnum, 7);
#endif

// VCD - 20 callbacks, 3 flavors of thunks
typedef VOID __stdcall VCDControl_HANDLER(DWORD MsgNumber, VMHANDLE hNew, VMHANDLE hPrev, VCD_COM* pCom);
DeclareThunk(VCDControl, 3);
typedef VOID __stdcall VCDIrq_HANDLER(IRQHANDLE hIrq, VMHANDLE hVM, VCD_COM* pCom);
DeclareThunk(VCDIrq, 3);
typedef BYTE __stdcall VCDIo_HANDLER(VMHANDLE hVM, VCD_COM* pCom, DWORD Port, BOOL bOutput, BYTE OutData);
DeclareThunk(VCDIo, 3);

#endif	//__vthunks_h_

