// Minidriver.cpp: implementation of the Stream1394Minidriver class.
//
//////////////////////////////////////////////////////////////////////

#define VDW_MAIN	
#include <vdw.h>
#include <kstream.h>
#include "Adapter.h"
#include "Minidriver.h"

// Global trace object
KDebugOnlyTrace t("Stream1394Driver");

///////////////////////////////////////////////////////////////////
// Begin INIT time code
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(Stream1394Minidriver, NULL)


//////////////////////////////////////////////////////////////////////
// overrideable methods of base class KStreamMinidriver
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// Stream1394Minidriver::DriverEntry
// 
// This is the first routine in the driver that the system calls.
// Register with the stream class driver by calling Stream1394Minidriver::Register 
//		(wraps system call StreamClassRegisterAdapter).  
//
NTSTATUS Stream1394Minidriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
	t << "Stream1394Minidriver::DriverEntry\nCompiled " __TIME__ " : " __DATE__ "\n";

	// Call the Register method which registers the driver with the
	// stream class driver.  The Register method will call system method
	// StreamClassRegisterAdapter.
	return Register(
		RegistryPath,	// void* pContext2,
		FALSE,			// BOOLEAN bIsBusMaster,
		FALSE,			// BOOLEAN bIsDma24Bit,
		FALSE,			// BOOLEAN bSerializeRequests,   // note 1394 drivers set this to FALSE...
		FALSE,			// BOOLEAN bHasHardwareInterrupt,
		3,				// ULONG uBufferAlignment,
		0,				// ULONG uDmaBufferSize,
		sizeof(Stream1394Adapter),	// ULONG uMaxAdapterObjectSize,
		sizeof(IRB),	// ULONG uPerRequestExtensionSize,
		0				// ULONG uPerAdapterInstanceExtensionSize
		);
}

// End INIT time code
#pragma code_seg()


////////////////////////////////////////////////////////////////
// Stream1394Minidriver::OnCreateAdapter
// 
// Called when the class driver creates a new 
// adapter object SRB_INITIALIZE_DEVICE.  The adapter object memory
// is ALREADY allocated	so we construct the object in memory allocated
// by the stream class driver.  We then call the initialization method
// on the Adapter which will initialize internal adapter information
// (including the Adapter object's member streams).
//
NTSTATUS Stream1394Minidriver::OnCreateAdapter(
	PORT_CONFIGURATION_INFORMATION* pConfigInfo,
	PVOID AdapterStorage,
	PHW_STREAM_REQUEST_BLOCK pSrb
	)
{
	t << "Stream1394Minidriver::OnCreateAdapter\n";

	NTSTATUS Status = STATUS_SUCCESS;

	// call new operator to initialize the adapter object
	// using the memory previously allocated by the stream class driver
	m_pAdapter = new (AdapterStorage) Stream1394Adapter(
		this,			// KStreamMinidriver* Driver,
		pConfigInfo		// PORT_CONFIGURATION_INFORMATION* pConfigInfo,
		);

	if (m_pAdapter) {
		m_pAdapter->Initialize();
	}
	else {

		ASSERT(!"m_pAdapter=NULL");
		Status = STATUS_UNSUCCESSFUL;
	}

	return Status;
}
