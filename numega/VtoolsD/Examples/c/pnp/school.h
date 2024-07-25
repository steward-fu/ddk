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

// SCHOOL.h - include file for VxD SCHOOL
// Copyright (c) 1996, Compuware Corporation 
// Adapted from code written by Walter Oney for
// Microsoft Systems Journal January 1996.
// Reprinted with permission.

#ifndef NotVxD

#include <vtoolsc.h>

#define SCHOOL_Major		1
#define SCHOOL_Minor		0
#define SCHOOL_DeviceID		UNDEFINED_DEVICE_ID
#define SCHOOL_Init_Order	UNDEFINED_INIT_ORDER

// Dummy private resource identifier. This would be appropriate for the fifth
// resource created by an OEM to whom Microsoft had assigned OEM ID 0x10.
// DON'T JUST USE THIS ID FOR YOUR OWN PRIVATE RESOURCE!

#define ResType_Telepath ((0x10 << 5) | 5)

typedef struct
{		 
	int allocated;		// index of allocated channel (-1 means none)
	ULONG requested;	// mask for requested channels
} TELEPATH_RESOURCE;		

///////////////////////////////////////////////////////////////////////
// Telepathic I/O channel resource arbitrator
///////////////////////////////////////////////////////////////////////

typedef struct nodelistheader_s *NODELIST_HEADER;
typedef struct nodelist_s *PNODELISTITEM;

typedef struct
{								// allocation placeholder
	PNODELISTITEM node;			// current node
	TELEPATH_RESOURCE* pres;	// current resource descriptor
} ALLOCPLACE, *PALLOCPLACE;		// allocation placeholder


#endif // NotVxD

#define SCHOOL_CALLBACK_ENUM	CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_NEITHER, FILE_ANY_ACCESS)

