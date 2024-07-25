// kdpc.h - include file for class KDeferredCall
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

#ifndef __DPC__
#define __DPC__

class KDeferredCall
{
	SAFE_DESTRUCTORS
public:
	VOID Setup(PKDEFERRED_ROUTINE Callback, PVOID Context=NULL);
	BOOLEAN Request(PVOID Argument1=NULL, PVOID Argument2=NULL);
	BOOLEAN WithdrawRequest(void);

protected:
	KDPC m_Dpc;
};

inline VOID KDeferredCall::Setup( PKDEFERRED_ROUTINE Callback, PVOID Context)
{
	KeInitializeDpc(&m_Dpc, Callback, Context);
}

inline BOOLEAN KDeferredCall::Request(PVOID Argument1, PVOID Argument2)
{
	return KeInsertQueueDpc(&m_Dpc, Argument1, Argument2);
}

inline BOOLEAN KDeferredCall::WithdrawRequest(void)
{
	return KeRemoveQueueDpc(&m_Dpc);
}

//////////////////////////////////////////////////////////////////////
//
// Macro MEMBER_DPC
//
// To make a callback member in any class, use the following macro inside
// the definition of the class. If the callback member name is X, then
// the call to KDeferredCall::Setup must be passed LinkTo(X) as the callback
// address, and the address of the object (i.e."this") of which the
// callback is a member.
//
// The first parameter to the macro is the name of class in which the
// DPC callback member is being declared. The second parameter is the
// name of the callback function.
//
// The callback member declared by this macro has the following prototype:
//
//		VOID DpcCallbackMember(PVOID arg1, PVOID arg2)
//
// where arg1 and arg2 are the parameters passed to KDeferredCall::Request.
//
// This macro may be used inside any class definition, including classes
// derived from KDevice and KDeferredCall. It actually declares two members:
// One is a non-static member which is used as a callback, and the other
// is a static member whose declaration matches that required by the
// system for a DPC callback. The static member invokes the non-static
// member, relying on the fact that the object address was passed as
// parameter Context to KDeferredCall::Setup.
//
#define MEMBER_DPC(objclass, func)					\
	VOID __stdcall func(PVOID Arg1, PVOID Arg2);	\
	static VOID __stdcall LinkTo(func)(				\
		PKDPC dpc,									\
		PVOID context,								\
		PVOID Arg1,									\
		PVOID Arg2)									\
	{												\
		((objclass*)context)->func(Arg1, Arg2);		\
		UNREFERENCED_PARAMETER(dpc);				\
	}

#define MEMBER_DPCWITHCONTEXT(contextname, func)	\
	VOID __stdcall func(PVOID Arg1, PVOID Arg2, contextname* pContext);	\
	static VOID __stdcall LinkTo(func)(				\
		PKDPC dpc,									\
		PVOID context,								\
		PVOID Arg1,									\
		PVOID Arg2)									\
	{												\
		((contextname*)context)->					\
			m_pClass->func( Arg1, Arg2, (contextname*)context ); \
		UNREFERENCED_PARAMETER(dpc);				\
	}

#endif
