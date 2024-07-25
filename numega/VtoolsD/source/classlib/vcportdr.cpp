// vcportdr.cpp - implementation of class VCommPortDriver (VCOMM port driver)
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

#include <vtoolscp.h>

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

#ifndef __BORLANDC__
extern "C" PDRIVERCTRLFUNC __cdecl createDCThunk(VCommPortDriver*, BYTE* pThunk, VOID (VCommPortDriver::*pFunc)(DWORD,DWORD,DEVNODE,PVOID,PVOID,...));
extern "C" PCOMMPORTOPENFUNC __cdecl createOpenThunk(VCommPortDriver*, BYTE* pThunk, PPORTDATA (__cdecl VCommPortDriver::*pFunc)(DWORD,PCHAR,VMHANDLE,PDWORD));
extern "C" BOOL (* __cdecl createCloseThunk(VCommPortDriver*, BYTE* pThunk, BOOL  (__cdecl VCommPortDriver::*pFunc)(DWORD,PPORTDATA)))();
extern "C" BOOL (* __cdecl createPortFunctionThunk(VCommPort*, VCommPort*, PBYTE, BOOL, VOID (VCommPort::*pFunc)()))();
#else
extern "C" PDRIVERCTRLFUNC __cdecl createDCThunk(VCommPortDriver*, BYTE* pThunk, VOID (VCommPortDriver::*pFunc)(DWORD,DWORD,DEVNODE,PVOID,PVOID,...));
extern "C" PCOMMPORTOPENFUNC __cdecl createOpenThunk(VCommPortDriver*, BYTE* pThunk, PPORTDATA _cdecl (VCommPortDriver::*pFunc)(DWORD,PCHAR,VMHANDLE,PDWORD));
extern "C" BOOL __cdecl (* __cdecl createCloseThunk(VCommPortDriver*, BYTE* pThunk, BOOL _cdecl (VCommPortDriver::*pFunc)(DWORD,PPORTDATA)))();
extern "C" BOOL __cdecl (* __cdecl createPortFunctionThunk(VCommPort*, VCommPort*, PBYTE, BOOL, VOID (VCommPort::*pFunc)()))();
#endif

VCommPort* VCommPortDriver::m_PortList = NULL;
BOOL VCommPortDriver::m_bDoAcquireRelease = TRUE;

// Function
//	VCommPortDriver::Register - Register the port driver with VCOMM
//
// Syntax
//	BOOL VCommPortDriver::Register()
//
// Remarks
// 	Call this member function from the initialization code of your port
//	driver after creating all the port objects that the driver supports.
//	This function calls VCOMM_Register_Port_Driver to notify VCOMM that
//	your port driver has loaded. Before this function returns, VCOMM
//	invokes, via linkage provided by the class library, member function
//	Initialize for each port object.
//
// Return Value
//	Returns TRUE if successful; otherwise FALSE
//
BOOL VCommPortDriver::Register()
{
	return _VCOMM_Register_Port_Driver(createDCThunk(this, m_dcthunk, &VCommPortDriver::DriverControl));
}

// Function
//	VCommPortDriver::DriverControl - driver callback
//
// Syntax
//	VOID _cdecl VCommPortDriver::DriverControl(DWORD thunkRet,  
//	      DWORD function, DEVNODE handle, PVOID refData, PVOID args, ...)
// Parameters
//	thunkRet	facilitates linkage between VCOMM and thunk
//	function	operation code
//	handle		device node handle (Windows 95 only)
//	refData		key to pass to VCOMM_Add_Port
//	args		operation specific parameters
//
// Remarks
//	This member function is private and not overridable.
//
//	This function corresponds to Inquire in the Windows 3.11 DDK (NDDK). 
//
//	VCOMM calls this routine with parameter 'function' equal to zero to
//	instruct the driver to initialize the ports.
//
//	For Windows 3.11, this function is called only once. For Windows 95,
//	It is called for each port associated with the driver via the 
//	Registry.
//
VOID _cdecl VCommPortDriver::DriverControl(DWORD thunkRet, DWORD function, DEVNODE handle, PVOID refData, PVOID args, ...)
{
	VCommPort* pPort;
	DWORD ioBase;
	DWORD irq;
	PCHAR pPortName;
	PCOMMPORTOPENFUNC pOpenFunc;
#ifdef DEBUG
	dout << "Entering DriverControl, function=" << function << endl;
#endif
	switch (function)
	{
	case DC_Initialize:

		// Args ioBase, irq, and pPortName only meaningful for
		// Windows 95

		ioBase = ((DWORD*)&args)[0];
		irq = ((DWORD*)&args)[1];
		pPortName = ((PCHAR*)&args)[2];
#ifdef DEBUG
		dout << "DC_Initialize: handle=" << (DWORD)handle
			<< " iobase=" << ioBase << " irq=" << irq 
#ifdef WIN40
			<< " name=" << (pPortName ? pPortName : "<null>")
#endif
			<< endl;
#endif

#ifdef WIN40
		if (handle == 0)
		{
#endif	// Begin Windows 3.11 code -----------------------------------------

			for (pPort = m_PortList; pPort != NULL; pPort = pPort->m_chain)
			{
				if (pPort->Initialize(0, 0, 0))
				{
					pOpenFunc = createOpenThunk(this, m_openthunk, &VCommPortDriver::PortOpen);
					_VCOMM_Add_Port(refData, pOpenFunc, pPort->m_name);
				}
			}			
#ifdef WIN40 // End Windows 3.11 code -------------------------------------
		}
		else
		{
			pPort = LookupPort(pPortName);

			#ifdef WIN40
	 			// -- we want to create ports dynamically based
				//		upon the registry entries that are already set up --
				if ((NULL == pPort) && (NULL != pPortName))
	 			   pPort = CreatePort(pPortName);
			#endif

			if (pPort != NULL) 
			{
				if (pPort->Initialize(handle, ioBase, irq))
				{
					pOpenFunc = createOpenThunk(this, m_openthunk, &VCommPortDriver::PortOpen);
					_VCOMM_Add_Port(refData, pOpenFunc, pPort->m_name);
				}
			}
			else
				DEBUGEVENT(DBG_WARNING, "DeviceControl did not locate port");
		}
#endif
		break;
	default:
		DEBUGEVENT(DBG_ERROR, "Unknown function passed to DriverControl");
	}
}

// Function
//	VCommPortDriver::PortOpen - open port callback
//
// Syntax
//	PPORTDATA _cdecl VCommPortDriver::PortOpen(DWORD thunkRet, 	
//		PCHAR szPortName, VMHANDLE hVM, PDWORD pError)
//
// Parameters
//	thunkRet		facilitates linkage with thunk
//	szPortName		name of port being opened
//	hVM			handle of VM opening port
//	pError			pointer to location to receive error code
//
// Remarks
//	This member function is private and not overridable.
//
//	The purpose of this routine is to link VCOMM to a port object. It
//	looks up the specified port by name in the driver's port list.
//	If found, it acquires the port from VCOMM to avoid contention, and
//	then invokes the port object's member Open.
//
//
// Return Value
//	If successful, returns a pointer to the PORTDATA structure for the
//	port that was opened. Otherwise returns NULL.
//
//
PPORTDATA _cdecl VCommPortDriver::PortOpen(DWORD thunkRet, PCHAR szPortName, VMHANDLE hVM, PDWORD pError)
{
	VCommPort base;
	VCommPort* pPort;

	pPort = LookupPort(szPortName);

	if (pPort == NULL)
	{
		*pError = IE_HARDWARE;
		return NULL;
	}

	if ((pPort->m_data.ValidPortData == 'SMTF')  || 
	    (pPort->m_data.ValidPortData == 'FTMS'))	// depends on compiler
	{
		*pError = (pPort->m_PortNumber == 0xffffffff) ? IE_HARDWARE : IE_OPEN;
        pPort->m_data.dwLastError = *pError;
		return NULL; 
	}

	if (m_bDoAcquireRelease)
	{
#ifdef WIN40
// New contention handling mechanism for Windows 95. The contention handler for the 
// port is obtained during construction. After construction, data member
// m_StealFlag should be init'ed if there is a contention handler.

		if (pPort->m_ContentionHandler)
		{
			pPort->m_handle =  pPort->m_ContentionHandler(
				ACQUIRE_RESOURCE, 
				pPort->m_ResourceHandle, 
				createPortFunctionThunk(pPort, &base, pPort->m_CtnNotifyThunk, TRUE, (VOID (VCommPort::*)())&VCommPort::CtnNotifyHandler),
				0, pPort->m_StealFlag);
		}
#else
		pPort->m_handle = (DWORD) _VCOMM_Acquire_Port(&pPort->m_data, pPort->m_PortNumber, hVM, 0, szPortName);
#endif

		if (pPort->m_handle == 0)
		{
			DEBUGEVENT(DBG_WARNING,"Failed to acquire port");
			*pError = IE_DEFAULT;
			return NULL;
		}
	}
	if (!pPort->Open(hVM, pError))
	{
		DEBUGEVENT(DBG_WARNING, "Failed to open port");
		return NULL;
	}
	else
	{
		pPort->InitPortFunctions();
		pPort->m_data.PDfunctions->pPortClose = createCloseThunk(this, m_closethunk, &VCommPortDriver::PortClose);
		pPort->m_ownerVM = hVM;
		return &pPort->m_data;
	}
}

// Function
//	VCommPortDriver::PortClose - close port callback
//
// Syntax
//	BOOL _cdecl VCommPortDriver::PortClose(DWORD thunkRet, 
//		PPORTDATA pPortData)
//
// Parameters
//	thunkRet	facilitates linkage with thunk
//	pPortData	PORTDATA structure of port object being closed
//
// Remarks
//	This member function is private and not overridable.
//
//	VCOMM calls this function when the port is being closed. It
//	looks up the port in the driver's port list, calls the port
//	object's member Close, and then calls VCOMM to release the port.
//
// Return Value
//	Returns success (TRUE).
//
//
BOOL _cdecl VCommPortDriver::PortClose(DWORD thunkRet, PPORTDATA pPortData)
{
	VCommPort base;
	VCommPort* pPort = LookupPort(pPortData);

	if ( !pPort)
		return FALSE;

	BOOL bPortClosed = pPort->Close();
	BOOL bPortReleased;

	if (!bPortClosed)
	{
		DEBUGEVENT(DBG_WARNING,"Failed to close port");
		// Error message is set by pPort->Close()
		return FALSE;
	}
	
	if (m_bDoAcquireRelease)
#ifdef WIN40
		if (pPort->m_ContentionHandler)
		{
			bPortReleased = pPort->m_ContentionHandler(
				RELEASE_RESOURCE, 
				pPort->m_handle, 
				createPortFunctionThunk(pPort, &base, pPort->m_CtnNotifyThunk, TRUE, (VOID (VCommPort::*)())&VCommPort::CtnNotifyHandler)
				);

			if (!bPortReleased)
			{
				DEBUGEVENT(DBG_WARNING,"Failed to release port");
				pPort->m_data.dwLastError = IE_DEFAULT;
				return FALSE;
			}
		}
#else
		_VCOMM_Release_Port(pPort->m_handle, pPort->m_ownerVM);
#endif	
	return TRUE;
}

// Function
//	VCommPortDriver::DoAcquireRelease - set acquire/release flag
//
// Syntax
//	VOID VCommPortDriver::DoAcquireRelease(BOOL b)
//
// Parameters
//		TRUE if driver should call VCOMM to acquire and release the
//		port on open and close calls. FALSE if it should not do so.
//
// Remarks
//	This function enables the port driver to control whether or not
//	VCOMM's acquire/release protocol should be used when opening and
//	closing ports.
//
VOID VCommPortDriver::DoAcquireRelease(BOOL b)
{
	m_bDoAcquireRelease = b;
}

// Function
//	VCommPortDriver::LookupPort - look up port by name
//
// Syntax
//	VCommPort* VCommPortDriver::LookupPort(PCHAR szPortName)
//
// Parameters
//	szPortName	name of port to look up (e.g. "COM2")
//
// Remarks
//	Searches the driver's port list for the specified port
//
// Return Value
//	If found, returns a pointer to the port object; otherwise returns
//	NULL.
//
VCommPort* VCommPortDriver::LookupPort(PCHAR szPortName)
{
	VCommPort* p;

	if (szPortName == NULL)
		return NULL;

	for (p=m_PortList; p != NULL; p = p->m_chain)
		if (_stricmp(szPortName, p->m_name) == 0)
			return p;
	return NULL;
}

// Function
//	VCommPortDriver::LookupPort - look up port by PORTDATA struct
//
// Syntax
//	VCommPort* VCommPortDriver::LookupPort(PPORTDATA pPortData)
//
// Parameters
//	pPortData	PPORTDATA of port to look up 
//
// Remarks
//	Searches the driver's port list for the specified port
//
// Return Value
//	If found, returns a pointer to the port object; otherwise returns
//	NULL.
//
VCommPort* VCommPortDriver::LookupPort(PPORTDATA pPortData)
{
	VCommPort* p;

	for (p=m_PortList; p != NULL; p = p->m_chain)
		if (&p->m_data == pPortData)
			return p;
	return NULL;
}


// Function
//	VCommPortDriver::OnSysDynamicDeviceExit - process control message
//
// Syntax
//	BOOL VCommPortDriver::OnSysDynamicDeviceExit(void)
//
// Remarks
//	Calls a function to delete all the port objects belonging to the driver
//
// Return Value
//	Returns TRUE
//	
BOOL VCommPortDriver::OnSysDynamicDeviceExit(void)
{
	DeleteAllPorts();

	return TRUE;
}

// Function
//	VCommPortDriver::DeleteAllPorts - delete all VCommPort objects in list
//
// Syntax
//	VOID VCommPortDriver::DeleteAllPorts(void)
//
// Remarks
//	Walk the list of ports owned by this driver and delete them
//
VOID VCommPortDriver::DeleteAllPorts(void)
{
	VCommPort* link = m_PortList;
	VCommPort* next;

	while (link)
	{
		next = link->m_chain;
		delete link;
		link = next;
	}
}


// Function
//	VCommPortDriver::DeletePort - delete a port and remove it from list
//
// Syntax
//	VOID VCommPortDriver::DeletePort(VCommPort* pPort)
//
// Parameters
//	pPort	address of port to delete
//
// Remarks
//	Searches the driver's port list for the specified port and deletes it
//
VOID VCommPortDriver::DeletePort(VCommPort* pPort)
{
	VCommPort* link;
	VCommPort** pprev;

	for (link=m_PortList, pprev=&m_PortList; link != NULL; link=link->m_chain)
		if (link == pPort)
		{
			*pprev = link->m_chain;
			delete pPort;
			return;
		}
		else
			pprev = &link->m_chain;
}



