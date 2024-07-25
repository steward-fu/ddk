// CamLowerDevice.h: interface for the CamLowerDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMLOWERDEVICE_H__FF27B3F2_9D15_4088_822F_6B354C8D9CC6__INCLUDED_)
#define AFX_CAMLOWERDEVICE_H__FF27B3F2_9D15_4088_822F_6B354C8D9CC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "KSubscriber.h"


// Forward declaration
class CamLowerDevice;


// client callback function signature for asychronous requests
typedef void (*PFUNC) (
	NTSTATUS Status,
	ULONG ulLen,
	PVOID Context
	);


// struct used for completion context for start/stop isoch listen requests
typedef struct _ISOCH_COMPLETE_CONTEXT 
{
	CamLowerDevice*		m_pClass;	// pointer to this class instance
	KSubscriber*		m_pSubscriber;	// pointer to subscriber class instance which receives notification of completion
	PIRP				m_pIrp;		// pointer to IRP used as transport for IRB
	PVOID				m_pCtx;		// client context for client fnc
	ULONG				m_ulValue;	// storage for value required during isoch operations
	KMemory*			m_pMdl;		// storage for MDL used during isoch operations
} ISOCH_COMPLETE_CONTEXT, *PISOCH_COMPLETE_CONTEXT;


// struct used for completion context for Isoch Attach/Detach Buffers and Buffer completion
typedef struct _BUFFER_CONTEXT
{
	LIST_ENTRY				m_ListEntry;
	K1394IsochBufferList*	pBufferList;
	BOOLEAN					Cancelled;
	CamLowerDevice*			m_pClass;	// pointer to this class instance
} BUFFER_CONTEXT, *PBUFFER_CONTEXT;


//typedef K1394IsochDescriptor<BUFFER_CONTEXT> KIsochDescriptor;
typedef KIrbWithContext<REQUEST_ISOCH_STOP, ISOCH_COMPLETE_CONTEXT> KIrbIsochStop;
typedef KIrbWithContext<REQUEST_ISOCH_LISTEN, ISOCH_COMPLETE_CONTEXT> KIrbIsochListen;
typedef KIrbWithContext<REQUEST_ASYNC_WRITE, ISOCH_COMPLETE_CONTEXT> KIrbAsyncWrite;
typedef KIrbWithContext<REQUEST_ASYNC_WRITE, ISOCH_COMPLETE_CONTEXT> KIrbIsochStopXmit;
typedef KIrbWithContext<REQUEST_ISOCH_ATTACH_BUFFERS, BUFFER_CONTEXT> KIrbIsochBuffers;


#define STATIC(FuncName) st##FuncName


#define MEMBER_COMPLETE_IRB_WITH_CONTEXT(IrbType, FuncName)			\
	static NTSTATUS __stdcall STATIC(FuncName)(						\
		PDEVICE_OBJECT		,										\
		PIRP				Irp,									\
		PVOID				Context									\
		){															\
		KIrp I(Irp);												\
		ASSERT(PIRP(I));											\
		IrbType* pIrb = (IrbType*) Context;							\
		ASSERT(pIrb);												\
		CamLowerDevice* p = pIrb->m_Context.m_pClass;				\
		ASSERT(p);													\
		return p->FuncName(pIrb, I.Status());						\
		}															\
	NTSTATUS FuncName(IrbType*,NTSTATUS);


typedef enum  
{
	CAMERA_STATE_UNDEFINED = 0,
	CAMERA_STATE_INITIALIZED,
	CAMERA_STATE_START_ISOCH_LISTEN,
	CAMERA_STATE_SET_FRAME_RATE,
	CAMERA_STATE_SET_MODE,
	CAMERA_STATE_SET_FORMAT,
	CAMERA_STATE_SET_CHANNEL,
	CAMERA_STATE_SET_ENABLE,
	CAMERA_STATE_STARTED

} CAMERA_STATE, *PCAMERA_STATE;


class CamLowerDevice : public K1394LowerDevice  
{
	// Constructors
public:
	CamLowerDevice();
	virtual ~CamLowerDevice();
	SAFE_DESTRUCTORS;

	// Member Functions
public:

	// Initialization/setup methods and analogs
	NTSTATUS InitializeCamera();
	NTSTATUS ReallocateIsochResources();
	NTSTATUS DeallocateIsochResources();
	NTSTATUS AllocateIsochResources();
	NTSTATUS InitializeHardware();

	// Isoch operations
	NTSTATUS ReadFrame(PMDL pMdl, KSubscriber* pSubscriber, PVOID Context);
	NTSTATUS StopListen(KSubscriber* pSubscriber, PVOID Context=NULL);
	NTSTATUS StartListen(KSubscriber* pSubscriber, PVOID Context=NULL);
	NTSTATUS CancelBuffer(PBUFFER_CONTEXT p);
	NTSTATUS CancelAllBuffers();

	// Hardware access helper methods
	void DelayCamera(LONG dwMsTime);
	NTSTATUS ResetCamera();
	void DisplayCameraRegisters();
	void DisplayRegister(ULONG dwOffset, PCHAR pszRegName);

protected:
	NTSTATUS ReadRegister(ULONG dwOffset, PULONG pdwValue);
	NTSTATUS WriteRegister(ULONG dwOffset, ULONG& dwValue);
	NTSTATUS WriteRegister(
		PIRP pIrp,
		KIrbAsyncWrite* pIrb, 
		ULONG dwOffset, 
		PMDL Mdl, 
		PIO_COMPLETION_ROUTINE pfn, 
		PVOID ctx
		);
public:
	// Set camera properties
	NTSTATUS SetFrameRate(ULONG dwFrameRate);
	NTSTATUS SetVideoInfo(ULONG dwFrameRate, ULONG dwVideoMode);

	// Accessors
	ULONG FrameRate() { return m_FrameRate; }
	ULONG CurrentReadSize() { return IsochInfoTable[m_Index].m_Size; }
	CAMERA_STATE State() { return m_State; }
	bool IsStarted() { return m_State == CAMERA_STATE_STARTED; }

	MEMBER_1394_BUS_RESET_NOTIFICATION_ROUTINE(CamLowerDevice,BusReset)

protected:

	// Completion routines for asynchronous 1394 requests
	MEMBER_COMPLETE_IRB_WITH_CONTEXT(KIrbIsochStopXmit, CompleteStopXmit);
	MEMBER_COMPLETE_IRB_WITH_CONTEXT(KIrbIsochStop, CompleteStop);
	MEMBER_COMPLETE_IRB_WITH_CONTEXT(KIrbIsochListen, CompleteListen);
	MEMBER_COMPLETE_IRB_WITH_CONTEXT(KIrbAsyncWrite, CompleteCommand);  
	MEMBER_COMPLETE_IRB_WITH_CONTEXT(KIrbIsochBuffers, CompleteAttach);
	MEMBER_COMPLETE_IRB_WITH_CONTEXT(KIrbIsochBuffers, CompleteDetach);

	static void __stdcall stCompleteBuffer(PVOID Context1, PVOID Context2) {
		CamLowerDevice* p = (CamLowerDevice*) Context1; ASSERT(p); 
		p->CompleteBuffer(Context2);
	}

	void CompleteBuffer(PVOID Context);

	// Member Data
protected:
	ULONG					m_GenerationCount;
	//Isochronous bandwidth
	K1394IsochBandwidth		m_Bandwidth;
	//Isochronous channel	
	K1394IsochChannel		m_Channel;  
	//Isochronous resource
	K1394IsochResource		m_Resource;

    // Base Register of the camera (lower 32 bit portion only)
    ULONG			m_BaseRegister;

	// Maximum speed to communicate with the device
	ULONG			m_Speed;

	// Host Controller capabilities flags
	ULONG			m_HostCaps;

	// Current Frame Rate
	ULONG			m_FrameRate;

	// current Video Mode 0-7
	ULONG			m_VideoMode;     
	
	// current Video Format 0-7
	ULONG			m_VideoFormat;   

	// current index into IsochInfoTable	
	ULONG			m_Index;

	// Maximum number of isoch buffers that can be attached
	ULONG			m_MaxIsochBuffers;

	// state of isoch listen
	CAMERA_STATE	m_State;

	// flag indicates if start listen request succeeded
	bool			m_bListening;

	// outstanding isoc reads
	KInterlockedList<BUFFER_CONTEXT>	m_PendingReadList;

};

#endif // !defined(AFX_CAMLOWERDEVICE_H__FF27B3F2_9D15_4088_822F_6B354C8D9CC6__INCLUDED_)
