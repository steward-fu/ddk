// kcallback.h - include file for class KCallback
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

/* Overview

Class KCallback abstracts the functionality of the NT/WDM callback object. The DDK
interfaces to this object are ExCreateCallback, ExRegisterCallback,
ExUnregisterCallback, and ExNotifyCallback.

A callback is a named object to which is attached a list of functions, each of
which is called when the callback is activated. A driver publicizes the
availability of a particular event by naming the callback with a name known to
other drivers.  Other drivers subscribe to the callback by opening it and calling
member function Subscribe, passing to this method the address of the function
to be called when the callback is activated.

To cause the object to call all of its subscribers, a driver calls object member
function CallSubscribers. The registered callback function of each subscriber is
invoked with parameters that include a context value provided at registration
time, as well as parameters passed to CallSubscribers.

Callback objects are generally stored in the \callback branch of
the system object directory.

A driver that subscribes to a callback must unsubscribe when service is no longer
desired.

The driver that creates the callback can limit the number of subscribers to one.

Member Functions

KCallback			Constructor
~KCallback			Destructor
Create				Create a callback object (2 forms)
Open				Open an existing callback object (2 forms)
Subscribe			Register a function to be called when the callback is activated
Unsubscribe			Terminate callback subscription
CallSubscribers		Invoke registered callback function of all subscribers
operator PCALLBACK_OBJECT
					Cast overload to retrieve system callback object
*/

class KCallback
{
	SAFE_DESTRUCTORS
public:
	KCallback(void);

	~KCallback(void);

	NTSTATUS Create(
		PCWSTR Name,
		BOOLEAN AllowMultipleSubscribers=TRUE,
		ULONG Attributes=OBJ_CASE_INSENSITIVE|OBJ_PERMANENT
		);
	NTSTATUS Create(
		PUNICODE_STRING Name,
		BOOLEAN AllowMultipleSubscribers=TRUE,
		ULONG Attributes=OBJ_CASE_INSENSITIVE|OBJ_PERMANENT
		);

	NTSTATUS Open(PUNICODE_STRING Name, ULONG Attributes=OBJ_CASE_INSENSITIVE);
	NTSTATUS Open(PCWSTR Name, ULONG Attributes=OBJ_CASE_INSENSITIVE);

	PVOID Subscribe(PCALLBACK_FUNCTION ToBeCalled, PVOID Context);
	VOID Unsubscribe(PVOID Token);
	VOID CallSubscribers(PVOID Arg1=NULL, PVOID Arg2=NULL);

	operator PCALLBACK_OBJECT (void);

protected:
	PCALLBACK_OBJECT m_Callback;
};

/*
KCallback::KCallback

	KCallback(void)

Constructor for KCallback object

Comments

The constructor performs rudimentary initialization of the object. You
must call either Create or Open before actually using the object.

See also

Create, Open, ~KCallback

*/
inline KCallback::KCallback(void)
{
	m_Callback=NULL;
}

/*
KCallback::~KCallback

	~KCallback(void)

Destructor for class KCallback.

Comments

The destructor dereferences the system object. When the object's reference count
goes to zero, the system removes it.

Drivers that subscribe to a callback must unsubscribe before destroying the object.

See also

Unsubscribe

*/
inline KCallback::~KCallback(void)
{
	if ( m_Callback )
		ObDereferenceObject(m_Callback);
}

/*
KCallback::Create

 form 1
  NTSTATUS
	Create(
		PCWSTR Name
		BOOLEAN AllowMultipleSubscribers=TRUE,
		ULONG Attributes=OBJ_CASE_INSENSITVE|OBJ_PERMANENT
		)

 form 2
  NTSTATUS
	Create
		PUNICODE_STRING Name
		BOOLEAN AllowMultipleSubscribers=TRUE,
		ULONG Attributes=OBJ_CASE_INSENSITVE|OBJ_PERMANENT
		)

 Creates a callback object

 Parameters

	Name
		Name of the object. This must specify the complete object
		table path, e.g. L"\\Callback\\MyCallback"

	AllowMultipleSubscribers
		Input as TRUE if the object should allow multiple subscribers. If FALSE,
		the object allows only a single subscriber. Default is TRUE.

	Attributes
		Object attributes. Default is OBJ_CASE_INSENSITVE|OBJ_PERMANENT

  Returns
	Returns STATUS_SUCCESS

  Comments

	The function returns STATUS_SUCCESS if the object is successfully created,
	or if it already exists. In the latter case, the attributes may be ignored.

  See Also
	Open, CallSubscribers

*/
inline NTSTATUS KCallback::Create(PCWSTR Name, BOOLEAN AllowMultipleSubscribers, ULONG Attributes)
{
	return Create( KUstring(Name), AllowMultipleSubscribers, Attributes );
}

inline NTSTATUS KCallback::Create(PUNICODE_STRING Name, BOOLEAN AllowMultipleSubscribers, ULONG Attributes)
{
	OBJECT_ATTRIBUTES oa;

	InitializeObjectAttributes(&oa, Name, Attributes, NULL, NULL);

	return ExCreateCallback(&m_Callback, &oa, TRUE, AllowMultipleSubscribers);
}

/*
KCallback::Open

 form 1
  NTSTATUS
	Open(
		PCWSTR Name
		ULONG Attributes=OBJ_CASE_INSENSITVE
		)

 form 2
  NTSTATUS
	Open
		PUNICODE_STRING Name
		ULONG Attributes=OBJ_CASE_INSENSITVE
		)

 Opens an existing callback object

 Parameters

	Name
		Name of the object. This must specify the complete object
		table path, e.g. L"\\Callback\\MyCallback"

	Attributes
		Object attributes. Default is OBJ_CASE_INSENSITVE

  Returns
	Returns STATUS_SUCCESS if the object is found

  Comments

	The function returns STATUS_SUCCESS if the object is successfully opened.
	This function never creates a new callback object.

  See Also
	Create, Subscribe

*/
inline NTSTATUS KCallback::Open(PCWSTR Name, ULONG Attributes)
{
	return Open( KUstring(Name), Attributes );
}

inline NTSTATUS KCallback::Open(PUNICODE_STRING Name, ULONG Attributes)
{
	OBJECT_ATTRIBUTES oa;

	InitializeObjectAttributes(&oa, Name, Attributes, NULL, NULL);

	return ExCreateCallback(&m_Callback, &oa, FALSE, FALSE);
}

/*
KCallback::Subscribe

  PVOID
	Subscribe(
		PCALLBACK_FUNCTION ToBeCalled,
		PVOID Context
		)

 Subscribes to a callback object

 Parameters

	ToBeCalled
		Address of the function to be called when the object calls
		its subscribers. See comments below.

	Context
		Value passed to callback routine

  Returns
	Returns the token required to terminate the subscription. Save the
	return value for later passing to Unsubscribe.

  Comments

	The prototype of the callback function is given by:

	VOID
		(*PCALLBACK_FUNCTION ) (
		    IN PVOID CallbackContext,
		    IN PVOID Argument1,
		    IN PVOID Argument2
		    );

		CallbackContext is the context value passed to Subscribe.
		Argument1 and Argument2 are passed when the callback is activated,
		i.e., they are determined at the time the request to invoke all
		callback functions is made.

  See Also
	Open, Unsubscribe, CallSubscribers
*/
inline PVOID KCallback::Subscribe(PCALLBACK_FUNCTION ToBeCalled, PVOID Context)
{
	ASSERT (m_Callback != NULL);
	return ExRegisterCallback( m_Callback, ToBeCalled, Context );
}

/*
KCallback::Unsubscribe

  NTSTATUS
	Unsubscribe(
		PVOID Token
		)

 Terminates an existing subscription to a callback object

 Parameters

	Token
		Value returned from Subscribe

  Comments
	It is crucial to unsubscribe before the object is destroyed.

  See Also
	Subscribe
*/
inline VOID KCallback::Unsubscribe(PVOID Token)
{
	ASSERT (m_Callback != NULL);
	ExUnregisterCallback(Token);
}

/*
KCallback::CallSubscribers

 VOID
	CallSubscribers(
		PVOID Token
		)

 Causes the callback routine of each of the object's subscribers to be
 invoked

 Parameters

	Token
		Value returned from Subscribe

 Comments
	The prototype of the callback function is given by:

	VOID
		(*PCALLBACK_FUNCTION ) (
		    IN PVOID CallbackContext,
		    IN PVOID Argument1,
		    IN PVOID Argument2
		    );

		CallbackContext is the context value passed to Subscribe.
		Argument1 and Argument2 are the values passed to CallSubscribers.

 See Also
	Subscribe
*/
inline VOID KCallback::CallSubscribers(PVOID Arg1, PVOID Arg2)
{
	ASSERT (m_Callback != NULL);
	ExNotifyCallback( m_Callback, Arg1, Arg2 );
}

/*
KCallback::operator PCALLBACK_OBJECT

	operator PCALLBACK_OBJECT(
		PVOID Token
		)

 Overload cast operator to extract pointer to system callback object
 embedded in this instance.

 Comments
	The callback object is opaque.

*/
inline KCallback::operator PCALLBACK_OBJECT (void)
{
	return m_Callback;
}
