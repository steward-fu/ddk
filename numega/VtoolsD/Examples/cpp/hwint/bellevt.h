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

// BELLEVT.H - include file for class BellEvent, see bellevt.cpp for details
// Copyright (c) 1995, Compuware Corporation

class BellEvent : public VGlobalEvent
{
public:
	BellEvent(VOID);	
	VOID handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);
};
