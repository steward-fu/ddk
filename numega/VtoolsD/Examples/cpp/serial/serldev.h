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

/*************************************************************

	File:					SerlDev.h
	Original Author:		John A. Kowalonek
	Created:				May 1, 1998

	Description:

	VSerialPort class sample program.  Derived from Walter Oney
	Serial MiniDriver example and ported to conform with Compuware VToolsD class
	library.

	Version History:

    5/1/98	jak		Initial Definition... used CPort.h, CSerialPort.h from 
					Walter Oney code.  used Comx.h from VToolsD.

	Notes:

	I have provided my OWN Register and DriverControl methods to allow me to
	support dynamic port creation through the DC_Initialize method.... 
	
	(c) Copyright 1998, Compuware Corporation  All Rights Reserved.
		Portions (c) Copyright 1995 by Walter Oney, Licensed Material.  
************************************************************ */

#include <vtoolscp.h>

#define DEVICE_CLASS		SerialDevice
#define SERIAL_DeviceID		UNDEFINED_DEVICE_ID
#ifdef WIN40
#define SERIAL_Init_Order	PORT_INIT_ORDER
#else
#define SERIAL_Init_Order	UNDEFINED_INIT_ORDER
#endif
#define SERIAL_Major		1
#define SERIAL_Minor		0

// Notice that the device class is derived from VCommPortDriver, not
// from VDevice.

class SerialDevice : public VCommPortDriver
{
public:
	virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR szCmdtail);
	virtual BOOL OnSysDynamicDeviceInit();

	
protected:

	// -- creates a new port when needed by DriverControl
	//		in Win95.  Default implementation doesn't do anything.
	//		Your method should construct one of your VCommPort
	//		derived objects	and return a pointer to it! --
	virtual VCommPort* CreatePort(PCHAR szPortName);

};
