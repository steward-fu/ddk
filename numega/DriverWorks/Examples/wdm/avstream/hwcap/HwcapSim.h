// HwcapSim.h: interface for the HwcapSimulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HWCAPTIMER_H__1D5D52D6_C501_4B5E_A8A8_8F00CEC7CDE9__INCLUDED_)
#define AFX_HWCAPTIMER_H__1D5D52D6_C501_4B5E_A8A8_8F00CEC7CDE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HwcapDmaEngine.h"

//////////////////////////////////////////////////////////////////////
// HwcapSimulator
//
// Implements video capture hardware simulation based on periodic timer.
//
class HwcapSimulator : protected KTimedCallback 
{
	SAFE_DESTRUCTORS
public:
	HwcapSimulator();
	~HwcapSimulator();

	void Start();
	void Stop();

	// Interrupt Sink Interface:
	struct IInterrupt {
		virtual void DPC() = 0;
	};

	NTSTATUS Configure(PKS_VIDEOINFOHEADER pHeader, IInterrupt* Sink);

	LONGLONG InterruptCount() const { return (LONGLONG) m_Ticks; }

	LONG IsOn() const { return m_bOn; }

	ULONG SetupDma(PUCHAR* VirtualAddress, KSSTREAM_POINTER_OFFSET& Mapping);
	ULONG CompleteDma();

	void WaitForStop() { m_StopEvent.Wait(KernelMode, FALSE, NULL, Suspended); }
	
protected:
	MEMBER_DPC (HwcapSimulator, TimerCallback);

#ifdef __COMMENT_ONLY
	VOID TimerCallback(PVOID Arg1, PVOID Arg2);		
#endif __COMMENT_ONLY

	REFERENCE_TIME	m_Ticks;
	REFERENCE_TIME	m_Interval;
	LARGE_INTEGER	m_StartTime;
	IInterrupt*		m_Sink;			// object to pass control on "interrupt"

	HwcapImage*		m_pImage;		// Image Generator
	PUCHAR			m_FrameBuffer;	// cached image
	ULONG			m_FrameBufferSize;

	LONG			m_bOn;			// state
	KEvent			m_StopEvent;

	// Scatter-Gather Control

	HwcapDmaEngine	m_DmaEngine;
};

//////////////////////////////////////////////////////////////////////
// HwcapHardwareResources
//
// A trivial hw resource container. In this contrived example
// HwcapHardwareResources acts simply as a N-size container of 
// resources R.
//
// In a real hw driver, the resources would be a sets of interrupts, 
// memory, and i/o ranges assigned (perhaps dynamically) to
// each pin as they are created.
// 
// 
template <class R, const ULONG N>
class HwcapHardwareResources
{
public:
	HwcapHardwareResources()  
		{ memset(m_Resource, 0, sizeof(m_Resource)); }
	~HwcapHardwareResources() 
		{ for (ULONG i=0; i<N; i++) delete m_Resource[i]; }

	R* Acquire(PKSPIN pin) 
		{ 
		ULONG Id = pin->Id;
		if (Id < N)
			if (!m_Resource[Id])
				return (m_Resource[Id]=new R);
		ASSERT(!"Acquire Resource");
		return NULL;
		}

	void Release(R* r)
		{
		for (ULONG i=0; i<N; i++) 
			if (m_Resource[i] == r) 
			{ delete r; m_Resource[i]=NULL; return; }
		ASSERT(!"Disconnect Resource");
		}

protected:
	R*			m_Resource[N];
};


#endif // !defined(AFX_HWCAPTIMER_H__1D5D52D6_C501_4B5E_A8A8_8F00CEC7CDE9__INCLUDED_)

