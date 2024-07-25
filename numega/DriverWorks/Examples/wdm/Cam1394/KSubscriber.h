// KSubscriber.h: interface for the KSubscriber class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KSUBSCRIBER_H__42D9AAF0_3632_4577_9EBE_07831BCBB3DB__INCLUDED_)
#define AFX_KSUBSCRIBER_H__42D9AAF0_3632_4577_9EBE_07831BCBB3DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum _COMPLETE_ID {

	START_LISTEN_COMPLETE=0,
	STOP_LISTEN_COMPLETE,
	READ_FRAME_COMPLETE,
	GET_PROPERTY_COMPLETE,
	SET_PROPERTY_COMPLETE

} COMPLETE_ID;


// This class defines subscriber callback interface to CamLowerDevice class. 
class KSubscriber  
{
public:
	KSubscriber(){}
	virtual ~KSubscriber(){}

	virtual void OnComplete(COMPLETE_ID, NTSTATUS, ULONG, PVOID) = 0;
};


template <class T>
class KSubscriberImpl : public KSubscriber
{
public:
	KSubscriberImpl(){}
	virtual ~KSubscriberImpl(){}

	virtual void OnComplete(COMPLETE_ID Id, NTSTATUS Status, ULONG Len, PVOID pCtx) {

		switch(Id) {

		case START_LISTEN_COMPLETE:
			static_cast<T*>(this)->OnStartListenComplete(Status, pCtx);
			break;

		case STOP_LISTEN_COMPLETE:
			static_cast<T*>(this)->OnStopListenComplete(Status, pCtx);
			break;

		case READ_FRAME_COMPLETE:
			static_cast<T*>(this)->OnReadFrameComplete(Status, Len, pCtx);
			break;

		case GET_PROPERTY_COMPLETE:
			static_cast<T*>(this)->OnGetPropertyComplete(Status, Len, pCtx);
			break;

		case SET_PROPERTY_COMPLETE:
			static_cast<T*>(this)->OnSetPropertyComplete(Status, Len, pCtx);
			break;

		default:
			ASSERT(!"KSubscriberImpl::OnComplete - Invalid COMPLETE_ID");
			break;
		}
	}

	// Default implementations of event handlers 
	// Derived classes must implement to handle a specific event
	// These are provided to allow derived classes to subscribe to specific events and
	// not all of the events.
	void OnStartListenComplete(NTSTATUS, PVOID){ ASSERT(!"OnStartListenComplete NOT IMPLEMENTED!"); }
	void OnStopListenComplete(NTSTATUS, PVOID){ ASSERT(!"OnStopListenComplete NOT IMPLEMENTED!"); }
	void OnReadFrameComplete(NTSTATUS, ULONG, PVOID){ ASSERT(!"OnReadFrameComplete NOT IMPLEMENTED!"); }
	void OnGetPropertyComplete(NTSTATUS, ULONG, PVOID){ ASSERT(!"OnGetPropertyComplete NOT IMPLEMENTED!"); }
	void OnSetPropertyComplete(NTSTATUS, ULONG, PVOID){ ASSERT(!"OnSetPropertyComplete NOT IMPLEMENTED!"); }
};


#endif // !defined(AFX_KSUBSCRIBER_H__42D9AAF0_3632_4577_9EBE_07831BCBB3DB__INCLUDED_)
