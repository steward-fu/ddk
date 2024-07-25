// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vmachine.h - include file for virtual machine class

#ifndef __vmachine_h_
#define __vmachine_h_

typedef struct cb_s* VMHANDLE;
typedef struct pmcb_s* PACB;	// ptr to prot mode application control block 

class VVirtualMachine : public VControlMessageDispatcher
{
public:
	VVirtualMachine(VMHANDLE);
	~VVirtualMachine();

	VMHANDLE getHandle() {return m_handle;}
	BOOL isSystemVM() {return m_bIsSystemVM;}

	static VVirtualMachine* getVM(VMHANDLE);
	static VMHANDLE currentVM()
		{return (VMHANDLE)Get_Cur_VM_Handle();}


	// Member functions for Control Messages
protected:
	virtual BOOL OnSysVMInit();
	virtual VOID OnSysVMTerminate();
	virtual BOOL OnVMCriticalInit();
	virtual BOOL OnVMInit();
	virtual VOID OnVMTerminate();
	virtual VOID OnVMNotExecuteable(DWORD flags);
	virtual VOID OnDestroyVM();
	virtual VOID OnVMSuspend();
	virtual BOOL OnVMResume();
	virtual VOID OnSetDeviceFocus(DWORD devID,DWORD flags,VMHANDLE aVM);
	virtual VOID OnBeginMessageMode();
	virtual VOID OnEndMessageMode();
	virtual BOOL OnQueryDestroy();
	virtual BOOL OnBeginPMApp(DWORD flags, PACB pAcb);
	virtual VOID OnEndPMApp(PACB pAcb);
	virtual VOID OnCloseVMNotify(DWORD flags);
#ifdef WIN40
	virtual VOID OnSysVMTerminate2();
	virtual VOID OnVMTerminate2();
	virtual VOID OnVMNotExecuteable2(DWORD flags);
	virtual VOID OnDestroyVM2();
	virtual VOID OnVMSuspend2();
	virtual VOID OnEndMessageMode2();
	virtual VOID OnEndPMApp2(PACB pAcb);
	virtual VOID OnCloseVMNotify2(DWORD flags);
#endif
	
public:
	// Control Message Dispatcher
	virtual DWORD OnControlMessage(
			       DWORD _Edi, 
			       DWORD _Esi, 
			       DWORD _Ebp, 
			       DWORD _Esp,
			       DWORD _Ebx,
			       DWORD _Edx,
			       DWORD _Ecx,
			       DWORD _Eax);

protected:
	// Data Members

	VMHANDLE m_handle;
	BOOL m_bIsSystemVM;
};

#endif

