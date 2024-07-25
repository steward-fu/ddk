// hellodev.h - include file for device class of basic driver
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

// All drivers define at least one subclass of KDevice. Most commonly,
// drivers create instances of the class in DriverEntry. The subclass
// must declare a constructor.

class MostSimpleDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	MostSimpleDevice(void);		// constructor

// This macro declares virtual member functions for the I/O request
// handlers. The definition of the macro depends on the contents of
// function.h.

	DEVMEMBER_DISPATCHERS

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	virtual NTSTATUS CreateClose(KIrp I); 			// COMMENT_ONLY
#endif

};
