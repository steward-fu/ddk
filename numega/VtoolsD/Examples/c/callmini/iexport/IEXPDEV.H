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

// iexpdev.h - include file for device class of exporting driver
// Copyright (c) 1996 Compuware Corporation

class IexportDevice : public KDevice
{
public:
	IexportDevice(void);		// constructor
	DEVMEMBER_DISPATCHERS
};
