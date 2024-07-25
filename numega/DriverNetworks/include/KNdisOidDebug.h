// KNdisOidDebug.h: Misc stuff to debug OID processing
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

#if !defined(AFX_KNDISOIDDEBUG_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_)
#define AFX_KNDISOIDDEBUG_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_

////////////////////////////////////////////////////////////////////
// OID Tracing Control Bits
enum KNDIS_OIDTRACE_FLAGS {
	KNDIS_OIDTRACE_NONE = 0,	// disable all
	KNDIS_OIDTRACE_QUERY = 1,	// queries only
	KNDIS_OIDTRACE_SET = 2,		// set only
	KNDIS_OIDTRACE_STATUS = 4,	// return status
	KNDIS_OIDTRACE_ERROR = 8,	// error status only

	KNDIS_OIDTRACE_ALL = ULONG(-1) // enable all
};

////////////////////////////////////////////////////////////////////
// Prints and checks OID-related structures
//
// KNdisOidDebug is helper class used in DEBUG ("checked") builds.
//
// In debug builds, by default, the framework automatically prints to the debug output
// all the OID query and set requests coming to the miniport (IM) driver.
// To disable the OID debug print alltogether, please add
//
//	#define	KNDIS_OID_NDEBUG
//
// to your project's settings.
//
// Alternatively, one can control OID tracing run-time using KNdisOidDebug::EnableTrace
// method.
//
// NOTE that using KNdisOidDebug adds a significant amount of memory (30+ Kbytes)
// used by the driver in run-time. The release ("free") builds does not use this class.
//
class KNdisOidDebug {
public:
	// Returns ASCII for the oid
	static const char* Name(const NDIS_OID oid);

	// Checks the order of the "supported OID list"
	static bool InOrder(NDIS_OID List[], ULONG nElements);

	// Control OID tracing in debug builds (Mask is OR'd KNDIS_OIDTRACE_XXX)
	static inline void EnableTrace(ULONG Mask=KNDIS_OIDTRACE_ALL) {sm_uTraceMask=Mask;}
	// Get OID tracing mask (Mask is OR'd KNDIS_OIDTRACE_XXX)
	static inline ULONG TraceMask() { return sm_uTraceMask; }

protected:
	// normalized arrays generated by OidGen.exe
	static char* sm_szOIDs[];
	static char* sm_szOIDsx[];

	static char sm_szUnknown[];

	static ULONG sm_uTraceMask;
};

///////////////////////////////////////////////////////////////////////////////
// OID Debug Macros
///////////////////////////////////////////////////////////////////////////////
#if DBG
#ifndef KNDIS_OID_NDEBUG
# define TRACE_OID	TRACE
# define TRACE_OID_WANTED(mask)		(KNdisOidDebug::TraceMask() & mask)
# define KNDIS_GET_OID_NAME(oid)	KNdisOidDebug::Name(oid)
# define KNDIS_CHECK_OID_LIST(list)	KNdisOidDebug::InOrder(list, sizeof(list)/sizeof(NDIS_OID))
#else
# define KNDIS_GET_OID_NAME(oid)		""
# define KNDIS_CHECK_OID_LIST(list)	TRUE
# define TRACE_OID
# define TRACE_OID_WANTED(mask)		(mask)
#endif
#else // DBG
# define KNDIS_GET_OID_NAME(oid)		""
# define KNDIS_CHECK_OID_LIST(list)	TRUE
# ifndef TRACE_OID
#  define TRACE_OID
# endif
# define TRACE_OID_WANTED(mask)		(mask)
#endif // DBG

#endif // AFX_KNDISOIDDEBUG_H__6C3A3815_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_