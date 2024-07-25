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

// VCMDLOG.cpp - main module for VxD VCMDLOG
// (c) Copyright 1996, Compuware Corporation

#define DEVICE_MAIN
#include "vcmdlog.h"
Declare_Virtual_Device(VCMDLOG)
#undef DEVICE_MAIN

VcmdlogDevice* ThisDevice;

BOOL VcmdlogDevice::OnSysDynamicDeviceInit(void)
{
	m_OpenCount = 0;
	m_pInt21 = 0;
	m_LogFileHandle = (HANDLE)-1;

	ThisDevice = (VcmdlogDevice*)TheDevice;

	return TRUE;
}

BOOL VcmdlogDevice::OnSysDynamicDeviceExit(void)
{
	if (m_pInt21)
		delete m_pInt21;

	return TRUE;
}


DWORD VcmdlogDevice::OnW32DeviceIoControl(PIOCTLPARAMS p)
{
	switch (p->dioc_IOCtlCode)
	{
		case DIOC_OPEN:
		case DIOC_CLOSEHANDLE:
			break;

		case IOCTL_VCMDLOG_OPEN:
		{
			WORD error;
			HANDLE h;

			BYTE actionTaken;

			if (m_OpenCount == 0)
			{
				m_pInt21 = new VcmdInt21();
				if ( !m_pInt21 )
					return DEVIOCTL_ERROR;

				if ( !m_pInt21->hook() )
				{
					delete m_pInt21;
					return DEVIOCTL_ERROR;
				}
			}		

			m_OpenCount++;

	// constants are defined in IFS.H			

			h = R0_OpenCreateFile(
	/*bInContext*/	FALSE,
	/*filename*/	(PCHAR)p->dioc_InBuf,
	/*mode*/		OPEN_ACCESS_WRITEONLY|OPEN_SHARE_DENYWRITE|OPEN_FLAGS_COMMIT,
	/*createAttr*/	ATTR_NORMAL,
	/*action*/		0x12,
	/*flags*/		0,
	/*pError*/		&error,
	/*pAction*/		&actionTaken
					);

			if (error == 0)
			{
				m_LogFileHandle = h;
				m_LogFileOffset = 0;
			}
			else
				m_LogFileHandle = (HANDLE)-1;

			break;
		}
		case IOCTL_VCMDLOG_CLOSE:
		{
			WORD error;

			if (m_LogFileHandle != (HANDLE)-1)
				R0_CloseFile(m_LogFileHandle, &error);

			if (error == 0)
				dprintf("VCMDLOG: close of log file succeeded\n");
	
			m_OpenCount--;
			if (m_OpenCount == 0)
			{
				m_LogFileHandle = (HANDLE)-1;
				delete m_pInt21;
				m_pInt21 = 0;
			}			
			break;
		}
		default:
			return DEVIOCTL_ERROR;
	}

	return DEVIOCTL_NOERROR;
}

VcmdInt21::VcmdInt21(void)  : VPreChainV86Int(0x21) {}

BOOL VcmdInt21::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, DWORD iInt)
{
	CHAR progname[80];
	PCHAR clientcmd;
	CHAR cmdline[128];
	CHAR* p;
	DWORD seg, off;
 	DWORD len;
	DWORD nWritten;
	WORD error;

	if (_clientAH == 0x4b)
	{
		strcpy(progname, (PCHAR)Map_Flat(CLIENT_DS, CLIENT_DX));
		
		p = (PCHAR)Map_Flat(CLIENT_ES, CLIENT_BX);
		p += 2;
		off = *(WORD*)p;
		p += 2;
		seg = *(WORD*)p;
		p = (PCHAR)  ( (seg << 4) + off);
		len = *p;
		p++;

		clientcmd = p;				
		strcpy(cmdline, progname);
		strcat(cmdline, " ");
		strncat(cmdline, p, len);
		strcat(cmdline, "\r\n");

		if (ThisDevice->m_LogFileHandle != (HANDLE)-1)
		{
			nWritten = R0_WriteFile(
				FALSE,
				ThisDevice->m_LogFileHandle,
				cmdline,
				strlen(cmdline),	
				ThisDevice->m_LogFileOffset,
				&error
				);
			if (error == 0)
			{
				ThisDevice->m_LogFileOffset += nWritten;
			}
		}
	}

	return FALSE;	
}

