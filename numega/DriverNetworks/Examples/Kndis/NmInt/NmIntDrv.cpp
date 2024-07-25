// NmIntDrv.cpp - NmIntDriver class implementation
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================


#include <kndis.h>
#include "NmIntDrv.h"
#include "NmIntAdap.h"
#include "NmIntBind.h"

#if KNDIS_W9X_COMPATIBLE
#error("Warning KNDIS_W9X_COMPATIBLE=1. Framework will NOT call NdisMRegisterUnloadHandler and driver cannot unload.")
#endif

#if DBG
INT	nmintDebugLevel = NMINT_INFO;
#endif


//=============================================================================
// Begin INIT time code
//
// Code in section INIT is discarded after the driver initializes
//#pragma code_seg("INIT")

// The wizard inserts the following macro (declares the driver class)

DECLARE_MINIDRIVER_CLASS(NmIntDriver)


//=============================================================================
// NmIntDriver	Implementation

//=============================================================================
// NmIntDriver::DriverEntry
//
// This method registers adapter characteristics (a handler pointer table)
// with NDIS. Usually that's all this function has to do.
//
// Returns:
//	Status code of registration. Usually, it's NDIS_STATUS_SUCCESS.
//  Returning any other code prevents the driver from being loaded.
//
NTSTATUS NmIntDriver::DriverEntry(IN PVOID)
{
	TRACE("NmIntDriver::DriverEntry\n");

	KNDIS_FILTERDRIVER_CHARACTERISTICS<NmIntAdapter, NmIntBinding> Chars;
	return Chars.Register(*this, L"NmInt");

}


//=============================================================================
// NmIntDriver::OnOpenBinding
//
//=============================================================================
PWSTR NmIntDriver::OnOpenBinding(NmIntBinding* b, PNDIS_STRING p)
{
	TRACE("NmIntDriver::OnOpenBinding\n");

	PWSTR pRet = NULL;

	BINDING_PAIR* pBP = new BINDING_PAIR(b,p);
	if (pBP && pBP->m_wsVirtInst.Buffer) {
		pRet = pBP->m_wsVirtInst.Buffer;
		m_lstBindings.InsertHead(pBP);
	} else {
		ASSERT(pBP);
	}

	return pRet;
}


//=============================================================================
// NmIntDriver::OnOpenAdapter
//
//=============================================================================
void NmIntDriver::OnOpenAdapter(NmIntAdapter* a)
{
	TRACE("NmIntDriver::OnOpenAdapter\n");

	// Find the instance of BINDING_PAIR where b 
	// matches a binding pointer in the list and update 
	// the pointer to the adapter
	m_Lock.Lock();

	BINDING_PAIR* pBP = m_lstBindings.Head();
	ASSERT(pBP);

	while(pBP) {

		if (*pBP == a->m_wsInstance) {
			if (!pBP->m_pAdapter) {
				pBP->m_pAdapter = a;

				// should already have a binding since 
				// ProtocolBindAdapter is called first before 
				// we initiate a MiniportIntialize by the 
				// call to NdisIMInitializeDeviceInstanceEx
				ASSERT(pBP->m_pBinding);

				pBP->m_pBinding->AddAdapter(a);
				a->AddBinding(pBP->m_pBinding);
				break;
			}
		}

		pBP = m_lstBindings.Next(pBP);
	}

	m_Lock.Unlock();
}


//=============================================================================
// NmIntDriver::FindAdapter
//
//=============================================================================
NmIntAdapter* NmIntDriver::FindAdapter(PNDIS_STRING p) 
{

	// Don't acquire a lock!
	// since only called while a lock is held

	NmIntAdapter* pAdap = NULL;
	BINDING_PAIR* pBP   = m_lstBindings.Head();
	ASSERT(pBP);

	while(pBP) {
		if (*pBP == *p) {
			if (pBP->m_pAdapter) {
				pAdap = pBP->m_pAdapter;
				break;
			}
		}

		pBP = m_lstBindings.Next(pBP);
	}

	return pAdap;
}


//=============================================================================
// NmIntDriver::BindingComplete
//
//=============================================================================
void NmIntDriver::BindingComplete()
{
	TRACE("NmIntDriver::BindingComplete\n");

	// Walk over the list of our bindings and make sure all 
	// Adapters pointers are initialized.  Then call method of
	// adapter object to initialize its array of bindings.  Call
	// method of binding object to initialize its array of adapters.

	m_Lock.Lock();

	BINDING_PAIR* pBP = m_lstBindings.Head();

	while(pBP) {

		if (!pBP->m_pAdapter) {

			NmIntAdapter* a = FindAdapter(&pBP->m_wsVirtInst);

			// it is normal not to find an adapter, if that adapter
			// is disabled.

			if (a) {
				pBP->m_pAdapter = a;
				pBP->m_pBinding->AddAdapter(a);
				pBP->m_pAdapter->AddBinding(pBP->m_pBinding);
				break;
			}
		}

		pBP = m_lstBindings.Next(pBP);
	}

	m_Lock.Unlock();
}


//=============================================================================
// NmIntDriver::OnCloseBinding
//
//=============================================================================
void NmIntDriver::OnCloseBinding(NmIntBinding* b)
{
	TRACE("NmIntDriver::OnCloseBinding\n");

	// Binding is being un-bind-ed, so need to remove all
	// references to it for all adapter objects

	// also, remove all BINDING_PAIR objects 
	// that contain b and delete

	KList<BINDING_PAIR> lstCleanUp;

	m_Lock.Lock();

	BINDING_PAIR* pBP = m_lstBindings.Head();

	while(pBP) {
		BINDING_PAIR* p = pBP;

		pBP = m_lstBindings.Next(pBP);

		if (p->m_pBinding == b) {
			// we need to do some processing of this entry
			// BUT it cannot be done while holding the lock
			m_lstBindings.Remove(p);
			lstCleanUp.InsertHead(p);

			if (p->m_pAdapter) {
				// If the adapter has other binding instances
				// do not perform deintialize when cleaning up (see below)
				if (0 != p->m_pAdapter->RemoveBinding(b)) {
					p->m_pAdapter = NULL;
				}
			}
		}
	}

	m_Lock.Unlock();

	while(pBP = lstCleanUp.RemoveHead()) {

		if (pBP->m_pAdapter) {
			// This call will result in our virtual 
			// adapter being halt-ed
			pBP->m_pAdapter->m_State = NmIntAdapter::DEINITIALIZING;
			NDIS_STATUS Status = pBP->m_pAdapter->DeInitialize();
			ASSERT(NDIS_STATUS_SUCCESS==Status);
		}

		delete pBP;
	}
}


//=============================================================================
// NmIntDriver::OnCloseAdapter
//
//=============================================================================
void NmIntDriver::OnCloseAdapter(NmIntAdapter* a)
{
	TRACE("NmIntDriver::OnCloseAdapter\n");

	// Adapter is being halted, 
	// Find and remove all BINDING_PAIR where a 
	// matches an adapter pointer in the list 
	// and delete the BINDING_PAIR

	// For all b call b->RemoveAdapter(a)

	m_Lock.Lock();

	BINDING_PAIR* pBP = m_lstBindings.Head();

	while(pBP) {

		if (pBP->m_pAdapter == a) {
			if (0 == pBP->m_pBinding->RemoveAdapter(a)) {
			}

			pBP->m_pAdapter = NULL;
		}

		pBP = m_lstBindings.Next(pBP);
	}

	m_Lock.Unlock();
}


//=============================================================================
// NmIntDriver::BINDING_PAIR::BINDING_PAIR
//
//=============================================================================
NmIntDriver::BINDING_PAIR::BINDING_PAIR(NmIntBinding* pBinding, PNDIS_STRING pws) :
	m_pBinding(pBinding),
		m_pAdapter(NULL)
{
	ASSERT(pws);
	m_wsVirtInst.MaximumLength = 0;
	m_wsVirtInst.Length = 0;
	m_wsVirtInst.Buffer = new WCHAR[pws->MaximumLength];
	if (m_wsVirtInst.Buffer) {
		NdisMoveMemory(m_wsVirtInst.Buffer,pws->Buffer,pws->MaximumLength);
		m_wsVirtInst.MaximumLength = pws->MaximumLength;
		m_wsVirtInst.Length = pws->Length;
	}
}


//=============================================================================
// NmIntDriver::BINDING_PAIR::~BINDING_PAIR
//
//=============================================================================
NmIntDriver::BINDING_PAIR::~BINDING_PAIR() 
{
	delete m_wsVirtInst.Buffer;
}
