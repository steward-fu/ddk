// TailLightDevice.h
//
// Generated by DriverWizard version DriverStudio 3.0.0 (Build 826)
// Requires Compuware's DriverWorks classes
//

#ifndef __TailLightDevice_h__
#define __TailLightDevice_h__

// @@++ADDED_CODE
#include <kusb.h>
// Add usbd.lib to linker project settings
// @@--ADDED_CODE

class TailLightDevice : public KWdmFilterDevice
{
	SAFE_DESTRUCTORS
public:
	TailLightDevice(PDEVICE_OBJECT Pdo, ULONG unit);

	virtual NTSTATUS InternalDeviceControl(KIrp I);	

	MEMBER_PASSTHROUGH_COMPLETE(TailLightDevice, IrpCompletionRoutine);

#ifdef _COMMENT_ONLY

		// The following member functions are actually defined by the
		// a DEVMEMBER_xxx or MEMBER_xxx macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	NTSTATUS IrpCompletionRoutine(KIrp I);
	VOID ThreadRoutine(void);
#endif

	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);

// @@++CHANGED_CODE
	KUsbLowerDevice	m_Lower;
// @@--CHANGED_CODE
	ULONG			m_Unit;

// @@++ADDED_CODE
protected:
	NTSTATUS SetTailLight(bool bOn);
	MEMBER_THREAD(TailLightDevice,ThreadRoutine);
	NTSTATUS StartThread();
	void StopThread();

	KSystemThread	m_Thread;
	KEvent			m_Event;
	bool			m_bOn;

// @@--ADDED_CODE

private:

	//Prevent the compiler from generating default copy 
	//constructor and assignment operator
	TailLightDevice(TailLightDevice&);
	operator=(TailLightDevice&);
};


#endif