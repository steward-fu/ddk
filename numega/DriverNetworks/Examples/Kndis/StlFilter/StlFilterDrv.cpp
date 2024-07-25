//////////////////////////////////////////////////////////////////////
// stlfilter.cpp: KNdisIMiniDriver-derived implementation
//

#define _CTYPE_DISABLE_MACROS
#define _INC_WCHAR

#include <KNdisFilterWrapper.h>	// required for IM drivers
#include "StlFilterDrv.h"
#include "StlFilterBind.h"
#include <kstl.h>
#include <string>
#include <hash_map>
#include "StlFilterAdap.h"

///////////////////////////////////////////////////////////////////
// Begin INIT time code
//
// Code in section INIT is discarded after the driver initializes
//#pragma code_seg("INIT")

// The wizard inserts the following macro (declares the driver class)

DECLARE_MINIDRIVER_CLASS(StlFilterDriver)


//////////////////////////////////////////////////////////////////////
// StlFilterDriver	Implementation

/////////////////////////////////////////////////////////////////////
// StlFilterDriver::DriverEntry
//
// This method registers adapter characteristics (a handler pointer table)
// with NDIS. Usually that's all this function has to do.
//
// Returns:
//	Status code of registration. Usually, it's NDIS_STATUS_SUCCESS.
//  Returning any other code prevents the driver from being loaded.
NTSTATUS StlFilterDriver::DriverEntry(IN PVOID)
{
	TRACE("StlFilterDriver::DriverEntry\n");

	KNDIS_FILTERDRIVER_CHARACTERISTICS<StlFilterAdapter, StlFilterBinding> Chars;
	return Chars.Register(*this, L"StlFilter");
}

