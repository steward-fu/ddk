// kexception.h: interface and implementation of the KException class.
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
#if !defined(AFX_KEXCEPTION_H__13A0B76C_AE3E_4A75_B11A_C08DD1AB63DD__INCLUDED_)
#define AFX_KEXCEPTION_H__13A0B76C_AE3E_4A75_B11A_C08DD1AB63DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////
// KException
//
// KException translates a Structured Exception into a C++
// exception. It allows to treat a SE as a C++ exception and use the
// C++ syntax to catch both kernel-raised and user-defined exceptions.
//
// DriverWorks C++ exception handling does not require the use of KException.
// Without KException, the kernel-raised exceptions such as access violations
// will be caught by the default exception handler, catch(...).
// KException gives the developer access to a more detailed information on
// the cause of the exception:
//
// try	{ ... }
// catch (KException& e) { /* SE handler */ }
// catch (Other e) { /* user-defined exception */ }
//
// In order to activate the SE translator, driver code has to call once
// static KException::Install(), usually from its DriverEntry().
//
// In order to use C++ exceptions in general, and KException in particular, the
// driver project must be compiled with the exceptions on (/GX) and link
// with CppExcpt.lib shipped with DriverWorks.
//
// NDIS Miniport drivers can use KException as long as they are compiled
// with BINARY_COMPATIBLE=0 or as NDIS WDM drivers. In either case,
// NDIS drivers should link with CppExcpt.lib and kndiswdm.lib.
//
class KException
{
public:

	static void Install();
	NTSTATUS Status() const;
	PVOID const At() const;

protected:
	KException(EXCEPTION_POINTERS const &info);

	static void __cdecl Translator(ULONG Code, EXCEPTION_POINTERS *info);

	PVOID m_ip;
	NTSTATUS m_status;

};

/////////////////////////////////////////////////////////////////////
// KExceptionAccessViolation
//
// KExceptionAccessViolation represents a special case of SE
// caused by an attempt to read from or write to a non-present
// virtual address. In addition to KException's accessors it
// provides accessors to the address being violated.
//
// KExceptionAccessViolation also catches exceptions thrown by
// kernel "probing" APIs such as ProbeForRead/Write,
// MmProbeAndLockPages, etc.
//
// Since KExceptionAccessViolation is derived from KException
// the latter can be used to catch all the exceptions,
// including the access violations. KExceptionAccessViolation,
// however provides 2 additional methods, Address() and IsWrite()
// to get more information on the exception.
//
class KExceptionAccessViolation : public KException
{
public:
	PVOID const Address() const;
	bool IsWrite() const;

protected:
	friend KException;

	KExceptionAccessViolation(EXCEPTION_POINTERS const &info);

	PVOID m_addr;
	bool  m_bWrite;
};


////////////////////////////////////////////////////////////////////
// Implementations: accessors


inline
NTSTATUS KException::Status() const
{
	return m_status;
}
inline
PVOID const KException::At() const
{
	return m_ip;
}

inline
PVOID const KExceptionAccessViolation::Address() const
{
	return m_addr;
}
inline
bool KExceptionAccessViolation::IsWrite() const
{
	return m_bWrite;
}

#endif // !defined(AFX_KEXCEPTION_H__13A0B76C_AE3E_4A75_B11A_C08DD1AB63DD__INCLUDED_)
