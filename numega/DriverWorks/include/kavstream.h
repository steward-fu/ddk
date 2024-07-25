// KAVStream.h: Top-level header file for DriverWorks AVStream support
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

#if !defined(AFX_KAVSTREAM_H__73771975_FD52_45BC_9546_B9B34DDB511F__INCLUDED_)
#define AFX_KAVSTREAM_H__73771975_FD52_45BC_9546_B9B34DDB511F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// NOTE: Include vdw.h first

// ks.sys headers


#define _NEW_DELETE_OPERATORS_	// bypass kcom.h new/delete

#undef _SYS_GUID_OPERATOR_EQ_	// to skip over a stupid GUID warning in kcom.h
#define _GUID_OPERATORS_

#include <windef.h>
#include <stdio.h>
#include <stdlib.h>
#include <windef.h>

#define NOBITMAP
#include <mmreg.h>
#undef NOBITMAP
#include <unknown.h>
#include <ks.h>
#include <ksmedia.h>
#include <kcom.h>

// Type tricks to manage default implementation in the bases

/////////////////////////////////////////////////////////////////////////////
// DefaultItem
//
// Represents static data item of type T in the base classes:
// KsDevice, KsFilter, KsPin, where T is usually one of the KS descriptor types.
// The framework exposes NULL implementation to AVStream for descriptor T unless
// the user overrides the item by supplying actual implementation in his/her
// KsDevice, KsFilter and KsPin-derived class.
//
// This template is used only by the Framework classes.
//
template <typename T>
class DefaultItem
{
	char	_x[sizeof(T)/2]; // NOTE: data items should be larger than char.
public:
	operator  T() { return * reinterpret_cast<T*>(_x); }
	T& operator[](int) { return *reinterpret_cast<T*>(_x); }

	static T get();				// strawman function returning type T
};

/////////////////////////////////////////////////////////////////////////////
// DefaultType
//
// Signifies a default implementation of an optional internal type defined
// in one of the base classes - usually by using
//	typedef DefaultType SomeContextClass_t;
// The NOT_DEFAULT_TYPE macro below detects if the user provided an actual
// implementation for the type with this name is his derived class by
// masking the default empty "implementation"
// Typically, the user would override  the DefaultType to
// implement an optional context structures associated with certain objects.
//
struct DefaultType {};

/////////////////////////////////////////////////////////////////////////////
// TypeCheck
//
// Helper detecting type convertability ("sameness") from T to U.
// Idea borrowed from http://www.cuj.com/experts/1810/alexandr.htm?topic=experts.
// NOTE: for fundamental types, e.g. TypeCheck<double,int> this will
// cause C4244. It's ok with DefaultType.
//
template <typename T, typename U>
class TypeCheck
{
	struct char2 { char x[2]; };
	static char  get(...);
	static char2 get(U);
	static T getT();
public:
	enum { KindOf = sizeof(get(getT()))==sizeof(char2) };
};



// Status returned by default, "non-implemented", dispatch handlers

#define DEFAULT_STATUS	DefaultItem<NTSTATUS>

// Check for default handlers and static data implementations.

#define NOT_DEFAULT_STATUS(status)	(sizeof(status) != sizeof(DEFAULT_STATUS))

#define NOT_DEFAULT_ITEM(data, type)	(sizeof(data) != sizeof(DefaultItem<type>))

#define NOT_DEFAULT_TYPE(type)	(!TypeCheck<type, DefaultType>::KindOf)


/////////////////////////////////////////////////////////////////////////////
// ks_assign_ptr
//
// Assigns pointers type-safe: rvalue can be either correct UDT ptr type or default;
// Others will generate compiler "cannot convert" error. Used in Ks descriptors setup
//
template <typename T>
	static inline void ks_assign_ptr(const T*& dst, const T* src) {dst=src;}
template <typename T>
	static inline void ks_assign_ptr(const T*& dst, const DefaultItem<T>* src){dst=(T*)src;}
template <typename T>
	static inline void ks_assign_ptr(T*& dst, const DefaultItem<T>* src){dst=(T*)src;}


// Default implementation of base class dispatching handlers.
// They shouldn't be called run-time (which would indicate at a Framework bug).

#define DEFAULT_IMPLEMENTATION { ASSERT(!"Dispatch"); return *reinterpret_cast<DEFAULT_STATUS*>(0); }


// Checks for default handler implementations. The first is for parameterless
// handlers such as Reset(void), and the following for handler with parameters
// of types P1, P2, etc.

#define NOT_DEFAULT_HANDLER(Class, Handler)	\
		NOT_DEFAULT_STATUS(reinterpret_cast<Class*>(0)->Handler())

#define NOT_DEFAULT_HANDLER_P1(Class, Handler, P1)	\
		NOT_DEFAULT_STATUS(reinterpret_cast<Class*>(0)->Handler	\
				(DefaultItem<P1>::get()))

#define NOT_DEFAULT_HANDLER_P2(Class, Handler, P1, P2)	\
		NOT_DEFAULT_STATUS(reinterpret_cast<Class*>(0)->Handler \
				(DefaultItem<P1>::get(), DefaultItem<P2>::get() ))

#define NOT_DEFAULT_HANDLER_P3(Class, Handler, P1, P2, P3)	\
		NOT_DEFAULT_STATUS(reinterpret_cast<Class*>(0)->Handler \
				(DefaultItem<P1>::get(), DefaultItem<P2>::get(), DefaultItem<P3>::get() ))

#define NOT_DEFAULT_HANDLER_P4(Class, Handler, P1, P2, P3, P4)	\
		NOT_DEFAULT_STATUS(reinterpret_cast<Class*>(0)->Handler \
				(DefaultItem<P1>::get(), DefaultItem<P2>::get(), DefaultItem<P3>::get(), DefaultItem<P4>::get() ))

#define NOT_DEFAULT_HANDLER_P5(Class, Handler, P1, P2, P3, P4, P5)	\
		NOT_DEFAULT_STATUS(reinterpret_cast<Class*>(0)->Handler \
				(DefaultItem<P1>::get(), DefaultItem<P2>::get(), DefaultItem<P3>::get(), DefaultItem<P4>::get(), DefaultItem<P5>::get() ))

#define NOT_DEFAULT_HANDLER_P6(Class, Handler, P1, P2, P3, P4, P5, P6)	\
		NOT_DEFAULT_STATUS(reinterpret_cast<Class*>(0)->Handler \
				(DefaultItem<P1>::get(), DefaultItem<P2>::get(), DefaultItem<P3>::get(), DefaultItem<P4>::get(), DefaultItem<P5>::get(), DefaultItem<P6>::get() ))

#define NOT_DEFAULT_HANDLER_P7(Class, Handler, P1, P2, P3, P4, P5, P6, P7)	\
		NOT_DEFAULT_STATUS(reinterpret_cast<Class*>(0)->Handler \
				(DefaultItem<P1>::get(), DefaultItem<P2>::get(), DefaultItem<P3>::get(), DefaultItem<P4>::get(), DefaultItem<P5>::get(), DefaultItem<P6>::get(), DefaultItem<P7>::get() ))


// Ks Framework headers

#include	".\avstream\KsObject.h"
#include	".\avstream\KsMiniDriver.h"
#include	".\avstream\KsDevice.h"
#include	".\avstream\KsFilter.h"
#include	".\avstream\KsStream.h"
#include	".\avstream\KsPin.h"
#include	".\avstream\KsFormats.h"
#include	".\avstream\KsDma.h"


#endif // !defined(AFX_KAVSTREAM_H__73771975_FD52_45BC_9546_B9B34DDB511F__INCLUDED_)
