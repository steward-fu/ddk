// NmIntDrv.h 
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


#ifndef _NmIntDrv_h	
#define _NmIntDrv_h

// Forwards
class NmIntBinding;
class NmIntAdapter;


//=============================================================================
// NmIntDriver	Declaration
//
class NmIntDriver : public KNdisIMiniDriver
{
protected:
	// must implement the entry point
	virtual NTSTATUS DriverEntry(IN PVOID RegistryPath);

public:
	//helper methods to manage internal binding relationships
	PWSTR OnOpenBinding(NmIntBinding* b, PNDIS_STRING p);
	void OnCloseBinding(NmIntBinding* b);
	void OnOpenAdapter(NmIntAdapter* a);
	void OnCloseAdapter(NmIntAdapter* a);
	void BindingComplete();

	void Lock()		{m_Lock.Lock();}
	void Unlock()	{m_Lock.Unlock();}

protected:

	// lookup adapter in binding list
	NmIntAdapter* FindAdapter(PNDIS_STRING p);

	// Helper structure which maintains an association (binding)
	// between Adapters and Bindings instances.  
	// This is used as an element of a list to track internal bindings of 
	// the intermediate driver.  
	//
	struct BINDING_PAIR {

		NDIS_STRING		m_wsVirtInst;
		NmIntAdapter*	m_pAdapter;
		NmIntBinding*	m_pBinding;
		LIST_ENTRY		m_ListEntry;

		// Equality (keyed by VirtualAdapInstance string)
		inline BOOLEAN operator == (NDIS_STRING& S) 
		{ 
			return ((m_wsVirtInst.Length == S.Length) && 
				 !memcmp(m_wsVirtInst.Buffer, S.Buffer, S.Length));
//			return NdisEqualUnicodeString(&S,&m_wsVirtInst,TRUE); 
		}

		// Equality (keyed by binding instance)
		inline bool operator == (const NmIntBinding* B) 
			{ return B==m_pBinding; }

		// Equality (keyed by adapter instance)
		inline bool operator == (const NmIntAdapter* A) 
			{ return A==m_pAdapter; }

		BINDING_PAIR(NmIntBinding* pBinding, PNDIS_STRING pws);

		~BINDING_PAIR(); 
	};

	KNdisSpinLock		m_Lock;
	KList<BINDING_PAIR> m_lstBindings;
};


#if DBG
//
// Debug levels: lower values indicate higher urgency
//
#define NMINT_EXTRA_LOUD       20
#define NMINT_VERY_LOUD        10
#define NMINT_LOUD             8
#define NMINT_INFO             6
#define NMINT_WARN             4
#define NMINT_ERROR            2
#define NMINT_FATAL            0

extern INT               nmintDebugLevel;


#define NMINTTRACE(lev, Fmt)                                   \
    {                                                        \
        if ((lev) <= nmintDebugLevel)                          \
        {                                                    \
            DbgPrint("NMINT: ");                            \
            DbgPrint Fmt;                                    \
        }                                                    \
    }
#else

#define NMINTTRACE(lev, Fmt)

#endif //DBG


#endif