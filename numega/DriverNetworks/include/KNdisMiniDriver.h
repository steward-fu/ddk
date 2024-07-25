// KNdisMiniDriver.h: interface for the KNdisMiniDriver class.
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
//
// Class KNdisMiniDriver is the base class for NDIS Miniports that
// client's miniport drivers are to be derived from.
//

#if !defined(AFX_KNDISMINIDRIVER_H__B7577253_2027_11D3_8F9B_00C04F7445B7__INCLUDED_)
#define AFX_KNDISMINIDRIVER_H__B7577253_2027_11D3_8F9B_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Global entry point
extern "C"
NTSTATUS DriverEntry(
	PDRIVER_OBJECT DriverObject,
	PUNICODE_STRING RegistryPath);

///////////////////////////////////////////////////////////////////
// KNdisMiniDriver
//
// KNdisMiniDriver is the base class for NDIS Miniport drivers written
// using DriverNetworks framework.
//
// Each NDIS miniport driver has one and only one class derived from
// KNdisMiniDriver. Such a class is a singleton, i.e. a single instance
// of the class exists per driver. DriverNetworks framework takes
// care	of the C++ run time initialization, creation of the single instance
// of the driver class and passing control to the derived class. The derived
// class is responsible for registering its miniport adapter(s) with NDIS.
//
// KNdisMiniDriver is an abstract class, i.e. drivers must override (=implement)
// the only pure virtual method, KNdisMiniDriver::DriverEntry().
// DriverEntry() has very little to do except registering the miniport adapter
// (KNdisMiniAdapter-derived) class. In the following example:
//
//	NTSTATUS MyDriver::DriverEntry(IN PVOID)
//	{
//		KNDIS_MINIPORT_CHARACTERISTICS<MyAdapter> Chars;
//		return	Chars.Register(*this);
//	}
//
// driver class MyDriver registers miniport represented by class MyAdapter.
// There is no need to do much more at this point. The actual work of a miniport
// driver usually starts in the Initialize() handler of a KNdisMiniAdapter-derived class.
// The Initialize() handler is usually called from NDIS immediately following the
// characteristics registration.
//
// DriverNetworks framework requires that one of the driver's source .cpp files
// include the following macro:
//
//	DECLARE_MINIDRIVER_CLASS(DriverClass)
//
// where DriverClass is a KNdisMiniDriver-derived class (e.g. MyDriver)
//
// NOTE that the Network Driver Wizard creates the mentioned source code snippets
// automatically. So, the developer rarely should write or even change
// anything in the initialization code above.
//
// NOTE thatNDIS intermediate drivers use KNdisIMiniDriver as the base driver class.
//
class KNdisMiniDriver : public KObject<NDIS_HANDLE, 'kndr'>
{
	virtual SAFE_DESTRUCTORS
public:

	///////////////////////////////////////////////////////////////////////
	// Driver Entry handler (pure virtual)
	//
	// Parameters:
	// 	RegPath                - Opaque pointer. Do not use it.
	// Returns:
	// 	NDIS_STATUS_SUCCESS return code allows the driver's initilization to proceed.
	//  Any other code fails the load.
	// Comments:
	//	DriverEntry() *must* be implemented by derived class
	// 	DriverNetworks framework calls DriverEntry() of a KNdisMiniDriver-derived class
	//	immediately *after* the C++ run-time initialization is done and NDIS wrapper is initialized.
	//	The derived class' DriverEntry() must register its miniport with NDIS using KNDIS_XXX_CHARACTERISTICS
	//	class.

	virtual NTSTATUS DriverEntry(IN PVOID) = 0;

	virtual VOID Unload();

	// accessors
	static KNdisMiniDriver* DriverInstance(void);

protected:
	// Ctor/dtor - not exposed publicly
	KNdisMiniDriver(void);
	~KNdisMiniDriver();

	// NDIS miniport specific init/deinit routines
	static NTSTATUS DriverInit(PDRIVER_OBJECT,PUNICODE_STRING);
	static void DriverDeinit();

	// single instance of the KNdisMiniDriver-derived class
	static KNdisMiniDriver* m_TheDriver;

	// Unload handler
	static VOID DriverUnload(PDRIVER_OBJECT);

	// allow global driver entry access stuff
	friend NTSTATUS DriverEntry(PDRIVER_OBJECT, PUNICODE_STRING);

	// allow the stubbed exports use the class
	friend struct KNdisExportStubs;

	////////////////////////////////////////////////////////////////////
	// Create instance of KNdisMiniDriver-derived object. The definition
	// of this method is injected into user's code by means of
	// DECLARE_MINIDRIVER_CLASS macro.
	static void CreateDriverInstance(void);

	////////////////////////////////////////////////////////////////////
	// Placement new/delete
	//
	// To circumwent the problem with NDIS miniport unload notification
	// we provide a static (.bss) storage for the singleton. It is called
	// only once from CreateDriverInstance(). See DECLARE_MINIDRIVER_CLASS.
public:
	template <class A> void* __cdecl operator new(size_t n, A*)
	{
		static char storage [sizeof(A)];
		ASSERT(sizeof(storage)>=n);
		return storage;
	}

	// Dtor to match ctor signature (for EH-compatibility; neither are used)
	void __cdecl operator delete(void*) {}
	template <class A> void __cdecl operator delete(void*, A*) {}

public:
	// "Driver object" pointer saved during construction.
	// The only use of it is in W98 WDM miniports, which have to use
	// IoCreateDevice since NdisMRegisterDevice is not available at W98.
	// In all other configurations this is an opaque pointer, which
	// should not be used.
	inline PDRIVER_OBJECT DriverObject() const { return m_DriverObject; }
private:
	PDRIVER_OBJECT	m_DriverObject;
};

//
///////////////////////////////////////////////////////////////////
// KNdisIMiniDriver
//
// KNdisIMiniDriver is the base class for NDIS Intermediate ("filter")
// drivers written using DriverNetworks framework.
//
// Each NDIS IM driver has one and only one class derived from
// KNdisIMiniDriver. Such a class is a singleton, i.e. a single instance
// of the class exists per driver. DriverNetworks framework takes
// care	of the C++ run time initialization, creation of the single instance
// of the driver class and passing control to the derived class. The derived
// class is responsible for registering its "miniport" and "protocol"
// personalities with NDIS.
//
// KNdisIMiniDriver is an abstract class derived from KNdisMiniDriver. Drivers
// must override (=implement) the only pure virtual method, KNdisMiniDriver::DriverEntry().
// DriverEntry() has very little to do except registering the miniport adapter
// (KNdisFilterAdapter-derived) and protocol (KNdisProtocolBinding-derived)
// classes. In the following example:
//
//	NTSTATUS MyDriver::DriverEntry(IN PVOID)
//	{
//		KNDIS_FILTERDRIVER_CHARACTERISTICS<MyFilter> Chars;
//		return Chars.Register(*this, L"CoolFilter");
//	}
//
// driver class MyDriver registers a filter represented by class KNdisFilterAdapter-derived
// class MyFilter. The second parameter in Register() is the service name of the
// driver in the Registry.
// There is no need to do much more at this point. The actual work of a IM (filter)
// driver usually starts in the Initialize() handler of a KNdisFilterAdapter-derived class.
// The Initialize() handler is usually called from NDIS immediately following the
// characteristics registration.
//
// DriverNetworks framework requires that one of the driver's source .cpp files
// include the following macro:
//
//	DECLARE_MINIDRIVER_CLASS(DriverClass)
//
// where DriverClass is a KNdisIMiniDriver-derived class (e.g. MyDriver)
//
// NOTE that the QuickMp wizard app creates the mentioned source code snippets
// automatically. So, the developer rarely should write or even change
// anything in the initialization code above.
//
class KNdisIMiniDriver : public KNdisMiniDriver {
	SAFE_DESTRUCTORS
public:
	// Register Layered Miniport
	template <class M, class P>
	NDIS_STATUS Register(KNDIS_IMINIPORT_CHARACTERISTICS<M>& mport,
						 KNDIS_PROTOCOL_CHARACTERISTICS<P>& proto)
		{
			NDIS_STATUS status = mport.Register(m_Handle, &m_hMpDriver);
			if (status == NDIS_STATUS_SUCCESS) status = proto.Register(&m_hPtDriver);
			if (status == NDIS_STATUS_SUCCESS) NdisIMAssociateMiniport(m_hMpDriver, m_hPtDriver);
			else ASSERT(!"KNdisIMiniDriver::Register");
			return status;
		}

	// Deregister Layered Miniport
	NDIS_STATUS Deregister()
		{ NDIS_STATUS status=0;
		  if (m_hMpDriver)
		  #if (KNDIS_DDK_VERSION >= 5)
			NdisIMDeregisterLayeredMiniport(m_hMpDriver);
		  #else
			// TBD: should it be at all?
		  #endif
		  m_hMpDriver=NULL;
		  if (m_hPtDriver) NdisDeregisterProtocol(&status, m_hPtDriver); m_hPtDriver=NULL;
		  return status;
		}

public:
	// Get "miniport" handler of the driver
	NDIS_HANDLE GetMiniportHandle() const { return m_hMpDriver; }
	// Get "protocol" handler of the driver
	NDIS_HANDLE GetProtocolHandle() const { return m_hPtDriver; }

protected:
// ctor
	KNdisIMiniDriver() : KNdisMiniDriver()
		{ m_hMpDriver=m_hPtDriver=NULL; }
// dtor
	~KNdisIMiniDriver()
		{  Deregister(); }

	// It is the same as KNdisMiniDriver but - unlike miniports - a layered
	// IM driver requires deregistration methods for its both miniport and protocol
	// "personalities". Thus, a need to store 2 extra handles. Those handles are
	// required by some driver-scope NDIS calls and, also, used for deinitialization.
	NDIS_HANDLE	m_hMpDriver;
	NDIS_HANDLE	m_hPtDriver;
};

////////////////////////////////////////////////////////////////////////
//	purecall		C++ run time support routine
//
//	This must be forced into one of the driver's sources. Linking with
//  ntoskrnl.lib (which is the 1st lib in the lib list) would resolve it,
//  but under W95/98 kernels, _purecall() export is not available.
//
#define KNDIS_PURECALL	\
	extern "C" int _cdecl _purecall() { ASSERT(FALSE); return 0; }


/////////////////////////////////////////////////////////////////////
// Non binary compatible NDIS export stubs (namespace)
//
// The definitions of the following methods are injected by the wizard
// into target src code by means of DECLARE_MINIDRIVER_CLASS macro.
// They could be built/target dependent, so they can't be included in
// the static kndisxx.lib.
//
// This includes implementations for NDIS exports not found in all platforms
// and referenced from within DriverNetworks library source files.
struct KNdisExportStubs {
	// NdisMRegisterUnloadHandler:
	static void RegisterUnloadHandler();
	// NdisIMCopySendCompletePerPacketInfo:
	static void CopySendCompletePerPacketInfo(PNDIS_PACKET DstPacket, PNDIS_PACKET SrcPacket);
};

////////////////////////////////////////////////////////////////////////////
// DECLARE_MINIDRIVER_CLASS Macro
//
// This macro appears once per miniport (IM) driver's source code.
// It injects platform/target dependent method implementations referred to
// from the C++ initializer built into KNdisMiniDriver.
//
// KNDIS_W9X_COMPATIBLE preprocessor symbol defines whether binary compatible
// (lowest common denominator) or W2K only version of the driver is to be built.
//
// The DECLARE_MINIDRIVER_CLASS_... injections can be extended to cover differences
// in platforms (most notably, NdisMRegisterUnloadHandler export).
//
// TODO: Add other NDIS5 platform incompatibilities in here.
//
#define DECLARE_MINIDRIVER_CLASS_COMPATIBLE(DriverClass)				\
	KNDIS_PURECALL														\
	void KNdisMiniDriver::CreateDriverInstance(void)					\
		{ new (reinterpret_cast<DriverClass*>(0)) DriverClass; }		\
	void KNdisExportStubs::RegisterUnloadHandler(){}						\
	void KNdisExportStubs::CopySendCompletePerPacketInfo(PNDIS_PACKET, PNDIS_PACKET) {}	\

#define DECLARE_MINIDRIVER_CLASS_W2K(DriverClass)						\
	void KNdisMiniDriver::CreateDriverInstance(void)					\
		{ new (reinterpret_cast<DriverClass*>(0)) DriverClass ; }		\
	void KNdisExportStubs::RegisterUnloadHandler(){						\
		NdisMRegisterUnloadHandler(KNdisMiniDriver::DriverInstance()->m_Handle, KNdisMiniDriver::DriverUnload);\
		}																\
	void KNdisExportStubs::CopySendCompletePerPacketInfo(PNDIS_PACKET DstPacket, PNDIS_PACKET SrcPacket) {	\
			NdisIMCopySendCompletePerPacketInfo(DstPacket, SrcPacket);	\
		} \

#if KNDIS_W9X_COMPATIBLE || (KNDIS_DDK_VERSION<5) || defined(NDIS40_MINIPORT)
#define DECLARE_MINIDRIVER_CLASS	DECLARE_MINIDRIVER_CLASS_COMPATIBLE
#else
#define DECLARE_MINIDRIVER_CLASS	DECLARE_MINIDRIVER_CLASS_W2K
#endif

#endif // !defined(AFX_KNDISMINIDRIVER_H__B7577253_2027_11D3_8F9B_00C04F7445B7__INCLUDED_)
