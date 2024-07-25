// kresreq.h - include file for resource requests and assignment
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
#define _INVALID_ 0xffffffff

////////////////////////////////////////////////////////////////////////

struct __resource
{
	LIST_ENTRY m_ListEntry;
	ULONG Ordinal ;
	IO_RESOURCE_DESCRIPTOR m_resdes;
};

enum ResourceReleaseMethod_t
{
	UseIoAssignResources,
	UseIoReportResourceUsage
};

////////////////////////////////////////////////////////////////////////


class KDevice; //forward

#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))

class KResourceRequest
{

// Construction
public:
	SAFE_DESTRUCTORS

	// isa, pci
	KResourceRequest(
		INTERFACE_TYPE bustype,
		ULONG busnumber,
		ULONG slot
		);

	// pcmcia
	KResourceRequest(
		INTERFACE_TYPE bustype,
		PCWSTR Manufacturer,
		PCWSTR Device
		);

	// pnpisa
	KResourceRequest(
		INTERFACE_TYPE bustype,
		PCWSTR VendorPrefix,
		USHORT ProductNumber,
		USHORT LogicalDevice,
		ULONG Instance=0
		);

	~KResourceRequest(void);

// Methods
public:
	NTSTATUS AddPort(
		PHYSICAL_ADDRESS MinimumAddress,
		PHYSICAL_ADDRESS MaximumAddress,
		ULONG Length,
		ULONG Alignment,
		UCHAR Option=0,
		UCHAR ShareDisposition=CmResourceShareDriverExclusive,
		USHORT Flags=CM_RESOURCE_PORT_IO
		);

	NTSTATUS AddPort(
		ULONGLONG MinimumAddress,
		ULONGLONG MaximumAddress,
		ULONG Length,
		ULONG Alignment,
		UCHAR Option=0,
		UCHAR ShareDisposition=CmResourceShareDriverExclusive,
		USHORT Flags=CM_RESOURCE_PORT_IO
		);

	NTSTATUS AddMemory(
		PHYSICAL_ADDRESS MinimumAddress,
		PHYSICAL_ADDRESS MaximumAddress,
		ULONG Length,
		ULONG Alignment,
		UCHAR Option=0,
		UCHAR ShareDisposition=CmResourceShareDriverExclusive,
		USHORT Flags=CM_RESOURCE_MEMORY_READ_WRITE
		);

	NTSTATUS AddMemory(
		ULONGLONG MinimumAddress,
		ULONGLONG MaximumAddress,
		ULONG Length,
		ULONG Alignment,
		UCHAR Option=0,
		UCHAR ShareDisposition=CmResourceShareDriverExclusive,
		USHORT Flags=CM_RESOURCE_MEMORY_READ_WRITE
		);

	NTSTATUS AddDma(
		ULONG MinimumChannel,
		ULONG MaximumChannel,
		UCHAR Option=0,
		UCHAR ShareDisposition=CmResourceShareDriverExclusive
		);

	NTSTATUS AddIrq(
		ULONG MinimumVector,
		ULONG MaximumVector,
		UCHAR Option=0,
		UCHAR ShareDisposition=CmResourceShareDriverExclusive,
		USHORT Flags=CM_RESOURCE_INTERRUPT_LATCHED
		);

	VOID Alternate(KResourceRequest* pAlternate);


#if !_WDM_	// not supported in WDM
	NTSTATUS Submit(KDevice* pDevice, PUNICODE_STRING RegistryPath);
	static NTSTATUS Release(KDevice* pDevice=NULL, PUNICODE_STRING RegistryPath=NULL);
#endif

	PCM_RESOURCE_LIST Assignments(void) { return m_Assignments; }
	ULONG AssignmentsSize(void);

protected:
	KList<__resource>* AddList(KList<__resource>* pList);
	PIO_RESOURCE_LIST AddList(
		PIO_RESOURCE_LIST pIoResList,
		KList<__resource>* pList
		);

// Data
public:
	LIST_ENTRY m_ListEntry;
	PCM_RESOURCE_LIST m_Assignments;
	BOOLEAN m_ConflictDetected;
protected:
	KList<__resource> m_resources;
	KList<KResourceRequest> m_alternates;
	INTERFACE_TYPE m_bustype;
	ULONG m_busnumber;
	ULONG m_slot;

	PWSTR m_MfgString;
	PWSTR m_DeviceString;
	BOOLEAN m_CtorStrings;
	ULONG m_instance;		// for pnpisa
	WCHAR m_VendorPrefix[4];
};

#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))

////////////////////////////////////////////////////////

class KResourceAssignment
{

// Construction
public:
	SAFE_DESTRUCTORS
#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
	KResourceAssignment(
		KResourceRequest* pRequest,
		UCHAR Type,
		ULONG Ordinal = 0
		);
#endif
	KResourceAssignment(
		PCM_RESOURCE_LIST pResourceList,
		UCHAR Type,
		ULONG Ordinal = 0
		);
	KResourceAssignment(
		PCM_RESOURCE_LIST pTranslatedList,
		PCM_RESOURCE_LIST pRawList,
		UCHAR Type,
		ULONG Ordinal=0
		);


protected:
	VOID __construct(
		PCM_RESOURCE_LIST pResourceList,
		UCHAR Type,
		ULONG Ordinal = 0
		);

	VOID __construct(
		PCM_RESOURCE_LIST pTranslatedList,
		PCM_RESOURCE_LIST pRawList,
		UCHAR Type,
		ULONG Ordinal = 0
		);

// Methods
public:
	UCHAR Share(void);
	BOOLEAN Status(void);
	UCHAR Type(void);
	UCHAR Flags(void);
	ULONG Length(void);
	PHYSICAL_ADDRESS Start(void);
	KAFFINITY Affinity(void);
	ULONG Level(void);
	ULONG Vector(void);
	ULONG Channel(void);

// Data
public:
	PCM_PARTIAL_RESOURCE_DESCRIPTOR m_resdes;
};

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))

inline VOID KResourceRequest::Alternate(KResourceRequest* pAlternate)
{
	m_alternates.InsertTail(pAlternate);
}


inline NTSTATUS KResourceRequest::AddMemory(
	PHYSICAL_ADDRESS MinimumAddress,
	PHYSICAL_ADDRESS MaximumAddress,
	ULONG Length,
	ULONG Alignment,
	UCHAR Option,
	UCHAR ShareDisposition,
	USHORT Flags
	)
{
	return AddMemory(
		MinimumAddress.QuadPart,
		MaximumAddress.QuadPart,
		Length,
		Alignment,
		Option,
		ShareDisposition,
		Flags
		);
}

inline NTSTATUS KResourceRequest::AddPort(
	PHYSICAL_ADDRESS MinimumAddress,
	PHYSICAL_ADDRESS MaximumAddress,
	ULONG Length,
	ULONG Alignment,
	UCHAR Option,
	UCHAR ShareDisposition,
	USHORT Flags
	)
{
	return AddPort(
		MinimumAddress.QuadPart,
		MaximumAddress.QuadPart,
		Length,
		Alignment,
		Option,
		ShareDisposition,
		Flags
		);
}


inline KResourceAssignment::KResourceAssignment(
	KResourceRequest* pRequest,
	UCHAR Type,
	ULONG Ordinal
	)
{
	PCM_RESOURCE_LIST pList = pRequest->m_Assignments;

	__construct(pList, Type, Ordinal);
}
#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))


inline KResourceAssignment::KResourceAssignment(
	PCM_RESOURCE_LIST pList,
	UCHAR Type,
	ULONG Ordinal
	)
{
	__construct(pList, Type, Ordinal);
}


inline KResourceAssignment::KResourceAssignment(
	PCM_RESOURCE_LIST pTranslatedList,
	PCM_RESOURCE_LIST pRawList,
	UCHAR Type,
	ULONG Ordinal
	)
{
	__construct(pTranslatedList, pRawList, Type, Ordinal);
}


inline UCHAR KResourceAssignment::Share(void)
{
	return m_resdes->ShareDisposition;
}

inline BOOLEAN KResourceAssignment::Status()
{
	return (m_resdes != NULL);
}

inline UCHAR KResourceAssignment::Type()
{
	return static_cast<UCHAR>(m_resdes ? m_resdes->Type : _INVALID_);
}

inline UCHAR KResourceAssignment::Flags()
{
	return static_cast<UCHAR>(m_resdes ? m_resdes->Flags : _INVALID_);
}

inline ULONG KResourceAssignment::Length()
{
	if ( !m_resdes )
		return _INVALID_;
	else if  (m_resdes->Type == CmResourceTypePort)
		return m_resdes->u.Port.Length;
	else
		return m_resdes->u.Memory.Length;
}

inline PHYSICAL_ADDRESS KResourceAssignment::Start()
{
	PHYSICAL_ADDRESS invalid;

	if ( !m_resdes )
	{
		invalid.u.LowPart = invalid.u.HighPart = _INVALID_;
		return invalid;
	}

	if  (m_resdes->Type == CmResourceTypePort)
		return m_resdes->u.Port.Start;
	else
		return m_resdes->u.Memory.Start;
}

inline KAFFINITY KResourceAssignment::Affinity()
{
	return m_resdes ? m_resdes->u.Interrupt.Affinity : 0;
}

inline ULONG KResourceAssignment::Level()
{
	return m_resdes ? m_resdes->u.Interrupt.Level : _INVALID_;
}

inline ULONG KResourceAssignment::Vector()
{
	return m_resdes ? m_resdes->u.Interrupt.Vector : _INVALID_;
}

inline ULONG KResourceAssignment::Channel()
{
	return m_resdes ? m_resdes->u.Dma.Channel : _INVALID_;
}
