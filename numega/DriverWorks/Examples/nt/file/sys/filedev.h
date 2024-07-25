// filedev.h - includef file for class KFile example
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

class FileTestDevice : public KDevice
{
public:
	FileTestDevice(void);
	DEVMEMBER_DISPATCHERS

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	virtual NTSTATUS CreateClose(KIrp I); 			// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
#endif

	KFile m_File;
};
