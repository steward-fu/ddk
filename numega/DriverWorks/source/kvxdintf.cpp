// kvxdintf.cpp - VxD interface for WDM driver (on Win9x)
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

#include <vdw.h>

#if _WDM_
#ifdef _X86_

BOOLEAN __cdecl VMM_Add_DDB(PDDB p);
BOOLEAN __cdecl VMM_Remove_DDB(PDDB p);

#define __emit__(x) _asm _emit x

KVxDInterface::~KVxDInterface(void)
{
	if (m_pDdb)
	{
		VMM_Remove_DDB(m_pDdb);
		delete m_pDdb;
		m_pDdb = NULL;
	}
}

KVxDInterface::KVxDInterface(
	PCHAR VxDName,
	VXDCONTROLDISPATCHER CtrlDispatcher,
	PVOID CtrlContext,
	BOOLEAN* pSuccess
	)
{
	m_pDdb = NULL;
	*pSuccess = Initialize(VxDName, CtrlDispatcher, CtrlContext);
}

BOOLEAN KVxDInterface::Initialize(
	PCHAR VxDName,
	VXDCONTROLDISPATCHER CtrlDispatcher,
	PVOID CtrlContext
	)
{
	if ( ( m_pDdb != NULL) || (VxDName == NULL) || (strlen(VxDName) == 0) )
		return FALSE;

	if ( ! _bWindows98_ )
		return FALSE;

	m_pDdb = new (NonPagedPool) DDB;

	if ( !m_pDdb )
		return FALSE;

	memset(m_pDdb, 0, sizeof DDB);
	strncpy((PCHAR)m_pDdb->DDB_Name, VxDName, 8);

	for (int i=0; i < 8; i++)
	{
		if (m_pDdb->DDB_Name[i] == '\0')
			m_pDdb->DDB_Name[i] = ' '; // convert null to space
	}

// set up thunk
	m_Thunk.m_PushPCarryReturn = 0x68;
	m_Thunk.m_CarryReturn = ULONG(&m_CarryReturn);
	m_Thunk.m_PushCtx = 0x68;
	m_Thunk.m_Contextvalue = ULONG(CtrlContext);
	m_Thunk.m_Pushad = 0x60;
	m_Thunk.m_CallRel = 0xE8;
	m_Thunk.m_CtrlHandlerRelative =
		ULONG(CtrlDispatcher) - ULONG(&m_Thunk.m_CtrlHandlerRelative) - 4;
	m_Thunk.m_LoadCarryReturn = 0x0d8b; // mov ecx, dword ptr [nextloc]
	m_Thunk.m_CarryReturnAddr = ULONG(&m_CarryReturn);
	m_Thunk.m_ShiftCarry = 0xE9D1;
	m_Thunk.m_ret = 0xc3;

	m_pDdb->DDB_Control_Proc = ULONG(&m_Thunk);

	if ( !VMM_Add_DDB(m_pDdb) )
	{
		delete m_pDdb;
		m_pDdb = NULL;
		return FALSE;
	}

	return TRUE;
}

__declspec(naked) VOID __cdecl __Add_DDB(PDDB p)
{
	_asm mov	eax, [esp+4]	;; pick up arg
	_asm push	edi
	_asm mov	edi, eax
	_asm int	0x20			;; call VMM

	__emit__(0xf7);
	__emit__(0x00);
	__emit__(0x01);
	__emit__(0x00);

	_asm pop	edi
	_asm jc		failed
	_asm mov	eax,1
	_asm ret

failed:
	_asm xor	eax, eax
	_asm ret
	UNREFERENCED_PARAMETER(p);
}


BOOLEAN __cdecl VMM_Add_DDB(PDDB p)
{
	ULONG	retval;
	__Add_DDB(p);
	_asm	mov retval, eax

	return BOOLEAN(retval);
}


__declspec(naked) VOID __cdecl __Remove_DDB(PDDB p)
{
	_asm mov	eax, [esp+4]	;; pick up arg
	_asm push	edi
	_asm mov	edi, eax

	_asm int	0x20			;; call VMM

	__emit__(0xf8);
	__emit__(0x00);
	__emit__(0x01);
	__emit__(0x00);

	_asm pop	edi
	_asm jc		failed
	_asm mov	al, 1
	_asm ret
failed:
	_asm xor	eax, eax
	_asm ret
	UNREFERENCED_PARAMETER(p);
}

BOOLEAN __cdecl VMM_Remove_DDB(PDDB p)
{
	ULONG	retval;

	__Remove_DDB(p);
	_asm mov retval, eax

	return BOOLEAN(retval);
}


__declspec(naked) VOID __cdecl KVxDCall::__Get_DDB(ULONG DeviceID, PDDB *p)
{
	_asm pushad
	_asm mov eax, [esp+32+4]	//esp+4 = DeviceID
	_asm mov edi, 0			//Device Name, must be zero when device ID is specified

	VxDCall(Get_DDB);

	_asm mov ebx, [esp+32+8]	//esp+8 = pDDB
	_asm mov [ebx], ecx		//ecx gets the DDB, or zero if it failled
	_asm popad
	_asm ret
	UNREFERENCED_PARAMETER(DeviceID);
	UNREFERENCED_PARAMETER(p);
}


__declspec(naked) VOID __cdecl KVxDCall::__Get_DDB(CHAR DeviceName[8], PDDB *p)
{
	_asm pushad
	_asm mov eax, 0			//Device ID, zero when name is specified
	_asm mov edi, [esp+32+4]   //esp+4 = Device Name

	VxDCall(Get_DDB);

	_asm mov ebx, [esp+32+8]	//esp+8 = pDDB
	_asm mov [ebx], ecx			//ecx gets the DDB, or zero if it failled
	_asm popad

	_asm ret
	UNREFERENCED_PARAMETER(DeviceName);
	UNREFERENCED_PARAMETER(p);
}

__declspec(naked) VOID __cdecl KVxDCall::__Directed_Sys_Control(ULONG ControlMessage,
													  PDDB pDDB)
{
	//ControlMessage = esp+4
	//pDDB = esp + 8
	_asm pushad  //subtracts 32 from esp

	//Put the control message in EAX
	_asm mov eax, [esp+32+4]
	//put the DDB in ECX
	_asm mov ecx, [esp+32+8]

	VxDCall(Directed_Sys_Control);

	_asm popad

	_asm ret
	UNREFERENCED_PARAMETER(ControlMessage);
	UNREFERENCED_PARAMETER(pDDB);
}

__declspec(naked) VOID __cdecl KVxDCall::__Directed_Sys_Control(ULONG ControlMessage,
													  PDDB pDDB,
													  PREGISTERS pRegisters)
{
	//ControlMessage = esp+4
	//pDDB = esp + 8
	//pRegisters= esp + 12
	_asm pushad  //subtracts 32 from esp

	//Put the control message in EAX
	_asm mov eax, [esp+32+4]
	//put the DDB in ECX
	_asm mov ecx, [esp+32+8]

	//fill in the other registers as the user requested
	_asm{
		mov ebx, [esp+32+12] //EBX = pRegisters

			mov edx, [ebx+12]
			mov edi, [ebx+16]
			mov esi, [ebx+20]
			mov ebp, [ebx+24]
			mov ebx, [ebx+4]
	}

	VxDCall(Directed_Sys_Control);

	//Fill in the registers structure
	_asm{
		mov [esp+32+4], ebx //reuse the ControlMessage stack location
			mov ebx, [esp+32+12] // ebx = pRegisters

			mov [ebx], eax
			mov eax, [esp+32+4]
			mov [ebx+4], eax	//value of ebx
			mov [ebx+8], ecx
			mov [ebx+12], edx
			mov [ebx+16], edi
			mov [ebx+20], esi
			mov [ebx+24], ebp
	}

	_asm popad

	_asm ret
	UNREFERENCED_PARAMETER(ControlMessage);
	UNREFERENCED_PARAMETER(pDDB);
	UNREFERENCED_PARAMETER(pRegisters);
}

#endif // _X86_
#endif // _WDM_
