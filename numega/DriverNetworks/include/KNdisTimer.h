// KNdisTimer.h: interface for the KNdisTimerCallback class.
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#if !defined(AFX_KNDISTIMER_H__79CDEE63_6D27_11D3_8FE4_00C04F7445B7__INCLUDED_)
#define AFX_KNDISTIMER_H__79CDEE63_6D27_11D3_8FE4_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
// KNdisTimer
//
// Provides NDIS timer callback services.
//
// KNdisTimer class is designed to provide a timer callback into
// a timeout member function of a target ("adapter") class.
//
// Before a timer object can be used it has to be initialized.
// The Initialize() member function prepares the timer block for
// callbacks to a supplied static member function, which usually calls
// a callback member function in a target ("adapter") class.
//
// Macro KNDIS_DECLARE_TIMERCALLBACK and KNDIS_MEMBER_CALLBACK are used as
// shorthands for specifying the target adapter class, static callback function and
// the timeout handler in the adapter class.
//
// The programming sequence for KNdis timers is as follows:
//
// (1) Declare a timer object in your class (usually, KNdisMiniAdapter-derived);
// (2) Declare a member timer expiration callback using KNDIS_DECLARE_TIMERCALLBACK;
// (3) Initialize the timer in you class ctor (initializer) using KNDIS_MEMBER_CALLBACK;
// (4) Implement the callback member function, which has prototype
//			void Timeout();
// E.g.
//		class Adapter : public KNdisMiniAdapter {
//			KNdisTimer	m_Timer;							// declare timer
//			KNDIS_DECLARE_TIMERCALLBACK(Adapter, Timeout);  // declare callback
//			void Timeout() { /* implement callback */ }
//		public:
//			NDIS_STATUS Initialize() {
//				  . . .
//					// initialize timer
//					m_Timer.Initialize(this, KNDIS_MEMBER_CALLBACK(Timeout));
//				  . . .
//				}
//			void SomeMethod() {
//					m_Timer.Set(1000);	// Timeout() will be called in 1 sec
//				}
//		};
//
// NOTE 1: KNdisTimer is used uniformly for both miniport and protocol drivers, i.e.
// it hides the differences in the underlying NDIS API for these types of drivers.
//
// NOTE 2: For NDIS miniports, all the timer objects should be created
// in adapter's Initialize() handler.
//
// NOTE 3: In miniport drivers, KNdisTimer-s are usually built into a
// KNdisMiniAdapter-derived ("adapter") class, and the callback is the
// member function of the class. However, KNdisTimer-s can be used with
// an arbitrary class A, as long as A exposes a public conversion
// operator NDIS_HANDLE(), which returns a valid MiniportAdapterHandle.
//
// For higher-layer NDIS drivers, there is no such limitation.
//
class KNdisTimer {
	SAFE_DESTRUCTORS
public:
// Constructor (uninitialized: requires calling Initialize())
	KNdisTimer()  { m_bInitialized=FALSE; }

// Destructor
	~KNdisTimer() { if (m_bInitialized) Cancel(); /* NDIS won't check and crash */ }


///////////////////////////////////////////////////////////////////////
// Initialize the timer
//
// Parameters:
// 	Adapter              - Target "adapter" object of type A
// 	handler              - Pointer to a static callback function
// Returns:
// 	none
// Comments:
// 	For miniports, class A must expose a public conversion operator NDIS_HANDLE(),
//	which returns a valid MiniportAdapterHandle. For For higher-layer NDIS drivers,
//  A can be any class.
//	Apply KNDIS_MEMBER_CALLBACK(handler) macro to the 2nd parameter to automatically
//	link the the static callback to the timer callback member A::handler().
//
	template <class A>
	void Initialize(A* Adapter, PNDIS_TIMER_FUNCTION handler)
				{
#if defined (NDIS_MINIPORT_DRIVER)
				   NdisMInitializeTimer(
					   &m_TimerBlock,
					   *Adapter,		// NDIS_HANDLE() operator must be defined for A!
				       handler,
					   Adapter			// context
					   );
#else
				   NdisInitializeTimer(
					   &m_TimerBlock,
				       handler,
					   Adapter			// context
					   );
#endif
				   m_bInitialized=TRUE;
				}
// Start tickin'
	inline void Set(UINT MillisecondsToDelay);
	inline void SetPeriodic(UINT MillisecondsToDelay);
// Cancel tickin'
	inline BOOLEAN Cancel();

// Tests if the object was initialized
	BOOLEAN IsValid() const { return m_bInitialized; }

// Invalidate the object.
	void Invalidate() {}

protected:
#if defined (NDIS_MINIPORT_DRIVER)
	NDIS_MINIPORT_TIMER
#else
	NDIS_TIMER
#endif
			m_TimerBlock;
	BOOLEAN	m_bInitialized;	// can't be too defensive...
};


///////////////////////////////////////////////////////////////////////////////
// Macro for dealing with KNdisTimer-s

///////////////////////////////////////////////////////////////////////////////
// KNDIS_DECLARE_TIMERCALLBACK, KNDIS_MEMBER_CALLBACK
//
// A flavor of DriverWorks' MEMBER_DPC/LinkTo macro tune.
//
// Use KNDIS_DECLARE_TIMERCALLBACK inside the class declaration containing the member
// callback in the form void Callback().
// Apply KNDIS_MEMBER_CALLBACK to the 2nd parameter of KNdisTimer::Initialize()
//
// Alternatevely, use KNDIS_DECLARE_TIMER in the class declaration to declare
// that all at once.
//
#define KNDIS_DECLARE_TIMERCALLBACK(Class, member)		    	\
 static void SysCallback2##member(PVOID, PVOID cxt, PVOID, PVOID) \
 { reinterpret_cast<Class*>(cxt)->member (); }

#define KNDIS_DECLARE_TIMER(Class, Callback)	\
	KNDIS_DECLARE_TIMERCALLBACK(Class, Callback)		\
	void Callback();							\
	KNdisTimer

///////////////////////////////////////////////////////////////////////////////
// Implementations



///////////////////////////////////////////////////////////////////////
// Set the timer
//
// Parameters:
// 	MillisecondsToDelay  - the interval, in milliseconds, after which the timer expires
// Returns:
// 	none
// Comments:
// 	The callback function set on Initialize() will be called in MillisecondsToDelay
//	unless Cancel() is called.
void KNdisTimer::Set(UINT MillisecondsToDelay)
{
	ASSERT(m_bInitialized);
#if defined (NDIS_MINIPORT_DRIVER)
	NdisMSetTimer(&m_TimerBlock, MillisecondsToDelay);
#else
	NdisSetTimer(&m_TimerBlock, MillisecondsToDelay);
#endif
}


///////////////////////////////////////////////////////////////////////
// Cancel the timer
//
// Parameters:
// 	none
// Returns:
// 	TRUE if the timer was cancelled; FALSE if the timer has expired before.
// Comments:
// 	none
BOOLEAN KNdisTimer::Cancel()
{
	ASSERT(m_bInitialized);
	BOOLEAN TimerCancelled;
#if defined (NDIS_MINIPORT_DRIVER)
	NdisMCancelTimer(&m_TimerBlock, &TimerCancelled);
#else
	NdisCancelTimer(&m_TimerBlock, &TimerCancelled);
#endif
	return TimerCancelled;
}


///////////////////////////////////////////////////////////////////////
// Set periodic timer
//
// Parameters:
// 	MillisecondsToDelay  - the interval, in milliseconds, after which the timer periodically called
// Returns:
// 	none
// Comments:
// 	This method is supported for miniport drivers only
void KNdisTimer::SetPeriodic(UINT MillisecondsToDelay)
{
#if defined (NDIS_MINIPORT_DRIVER)
	ASSERT(m_bInitialized);
	NdisMSetPeriodicTimer(&m_TimerBlock, MillisecondsToDelay);
#else
	ASSERT(!"Supported");
	UNREFERENCED_PARAMETER(MillisecondsToDelay);
#endif
}

#endif // !defined(AFX_KNDISTIMER_H__79CDEE63_6D27_11D3_8FE4_00C04F7445B7__INCLUDED_)
