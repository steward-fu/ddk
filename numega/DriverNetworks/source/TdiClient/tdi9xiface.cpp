// tdi9xiface.h		-		Low-level interface to TDI VxD
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
//=============================================================================

#include	<tdiclient.h>

#ifdef TDI_WINDOWS_NT
#error("This file is for W9X builds only!")
#endif

/////////////////////////////////////////////////////////////////////
// GetTdiDispatchTable()
//
// Retrieve TDI dispatch table from tdi.vxd
//
#ifndef KNDIS_DEBUG_OS
 #define VTDI_GET_DISPATCHTABLE  VTDI_Get_Info /* service 8 */
#else
 #define VTDI_GET_DISPATCHTABLE VTDI_Initialize /* service 0xA */
#endif
static inline TDIDispatchTable* GetTdiDispatchTable(const char* szProvider)
{
    TDIDispatchTable* pT=NULL;
	_asm	pusha
    VxDCall (VTDI_Get_Version);
    _asm    cmp ax,0
    _asm	jz	exit
	_asm	mov	eax, szProvider
	_asm	push eax
//	VxDCall (VTDI_GET_DISPATCHTABLE); // ?error?
    VxDCall (VTDI_Get_Info)
    _asm	add	esp,4
	_asm	mov pT, eax
exit:
	_asm	popa
	return	pT;
}


///////////////////////////////////////////////////////////////////////////////////
// KTDInterface::KTDInterface
//
// TDI Interface instance ctor.
//
// Parameters:
//		szProvider		-	Device-provider name
//		nIRP			-	Number of IRPs allocated for simultaneously
//							pending TDI operations. Not used in Wi9X.
//	Returns:
//		TDI_STATUS
//
KTDInterface::KTDInterface(const char* szProvider, UINT /*nIRP*/)
{
    // TODO: find provider in the list or choose the default one

   	m_pTDI = sm_pProviders[0];
    TDI_ASSERT(m_pTDI);
}

/////////////////////////////////////////////////////////////////////
// TDI Providers
// Currently supports only Win95 VTDI.VXD "MSTCP" interface
//
const char KTDInterface::szDefaultProvider[] = TCP_DEVICE_NAME;
TDIDispatchTable* KTDInterface::sm_pProviders[] = { NULL };

BOOLEAN KTDInterface::Initialize(void)
{
    // TODO: build a list of TDI providers...

	return ((sm_pProviders[0] = GetTdiDispatchTable(szDefaultProvider)) != NULL);
}


ULONG K9xSemaphore::CreateSemaphore()
{
	ULONG semaphore;

	_asm	pushad
	_asm    mov ecx, 0  ;; //Initial token count
	VxDCall (Create_Semaphore);
	_asm    jc error
	_asm	mov semaphore, eax
	_asm    jmp done

	_asm	error:		;; //Couldn't allocate the semaphore
	semaphore = 0;

	_asm	done:
	_asm	popad

	return semaphore;
}


VOID K9xSemaphore::DestroySemaphore(ULONG semaphore)
{
	_asm	pushad
	_asm    mov eax, semaphore
	VxDCall (Destroy_Semaphore);
	_asm	popad
}

VOID K9xSemaphore::WaitSemaphore(ULONG semaphore)
{
	_asm	pushad
	_asm    mov eax, semaphore
	VxDCall (Wait_Semaphore);
	_asm	popad
}

VOID K9xSemaphore::SignalSemaphore(ULONG semaphore)
{
	_asm	pushad
	_asm    mov eax, semaphore
	VxDCall (Signal_Semaphore);
	_asm	popad
}
