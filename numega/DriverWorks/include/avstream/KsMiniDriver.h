// KsMiniDriver.h: interface for the KsMiniDriver class.
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

#if !defined(AFX_KSMINIDRIVER_H__A276F895_3106_447B_A778_027A6E8A5298__INCLUDED_)
#define AFX_KSMINIDRIVER_H__A276F895_3106_447B_A778_027A6E8A5298__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);

// This appears once in the source code

#define DECLARE_KSMINIDRIVER_CLASS(DriverClass)					\
	void KsMiniDriver::CreateDriverInstance(void)				\
		{ new (reinterpret_cast<DriverClass*>(0)) DriverClass;}


//////////////////////////////////////////////////////////////////////
// KsMiniDriver
//
// Used as a base class with DriverEntry() to be implemented.
// DriverEntry must call Initialize().
//
class KsMiniDriver
{
	virtual SAFE_DESTRUCTORS
public:

	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath) = 0;
	virtual VOID Unload() {}

	// accessors
	static KsMiniDriver* DriverInstance(void) { return m_TheDriver; }

	inline operator PDRIVER_OBJECT() const { return m_DriverObject; }

	////////////////////////////////////////////////////////////////////
	// Placement new/delete

	template <class A> void* __cdecl operator new(size_t n, A*)
	{
		static char storage [sizeof(A)];
		ASSERT(sizeof(storage)>=n);
		return storage;
	}

	// Dtor to match ctor signature (for EH-compatibility; neither are used)
	void __cdecl operator delete(void*) {}
	template <class A> void __cdecl operator delete(void*, A*) {}

protected:
	// Ctor/dtor - not exposed publicly
	KsMiniDriver(void)
		{ ASSERT(m_TheDriver==0); // singleton
	      m_TheDriver = this;
		}

	virtual ~KsMiniDriver();

	// single instance of the KsMiniDriver-derived class
	static KsMiniDriver* m_TheDriver;

	// Custom Unload handler
	static VOID DriverUnload(PDRIVER_OBJECT);

	// allow global driver entry access stuff
	friend NTSTATUS DriverEntry(PDRIVER_OBJECT, PUNICODE_STRING);

	////////////////////////////////////////////////////////////////////
	// Create instance of KsMiniDriver-derived object. The definition
	// of this method is injected into user's code by means of
	// DECLARE_KSMINIDRIVER_CLASS macro.

	static void CreateDriverInstance(void);

private:
	PDRIVER_OBJECT	m_DriverObject;
	PDRIVER_UNLOAD  m_ClassUnloader;
};


#endif // !defined(AFX_KSMINIDRIVER_H__A276F895_3106_447B_A778_027A6E8A5298__INCLUDED_)
