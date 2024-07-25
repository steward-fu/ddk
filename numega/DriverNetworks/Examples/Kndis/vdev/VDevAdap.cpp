// VdevAdap.cpp: implementation of the VdevAdapter class.
//
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
//////////////////////////////////////////////////////////////////////

#include "VdevAdap.h"


#if VDEV_EH_DEMO
#ifndef _CPPUNWIND
#error	"This file requires compiler's C++ exception option: /GX"
#endif
#include <kexception.h>
#endif


KNdisDebugTrace T("VDEV");


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VdevAdapter::VdevAdapter() :
	KNdisMiniAdapter(), m_DeviceHandle(NULL), m_pDeviceObject(NULL)

{
	// TODO: initialize your private data members
	//       Do NOT call any NDIS functions in here. Defer to Initialize()
}

VdevAdapter::~VdevAdapter()
{
	// TODO: deallocate all the resources allocated in constructor/Initialize()

}

////////////////////////////////////////////////////////////////////
// NDIS callback handlers
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// VdevAdapter::Initialize
//
// MiniportInitialize is a required function that sets up a NIC (or 
// virtual NIC) for network I/O operations, claims all hardware resources
// necessary to the NIC in the registry, and allocates resources the driver
// needs to carry out network I/O operations
//
// Parameters:
//		Medium	
//			Reference to KNdisMedium object that we have to set the medium 
//			type our NIC supports
//		Config
//			Reference to KNdisConfig object that we use to query various
//			configuration parameters
// IRQL: 
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// TODO: 
// 1) Select supported medium in Medium object
// 2) Read config parameters if any using Config object
// 3) Allocate h/w resources, pools, etc.
// 4) Register with NDIS using SetAttributes()
// 5) Register Shutdown Handler
// 6) Initialize and enable the NIC.
// NOTE: Be ready to process INTs even before the function exits
//
NDIS_STATUS VdevAdapter::Initialize
				(IN OUT KNdisMedium& Medium, IN KNdisConfig& Config)
{
	T.Trace("VdevAdapter::Initialize() Entered: CreateDevice Option %u, EH Option %u\n",
		VDEV_WIN2K, VDEV_EH_DEMO);
	
	// Select our the Medium:

	if (! Medium.Select(VDEV_MEDIUM_TYPE) )
		KNDIS_RETURN_ERROR (NDIS_STATUS_UNSUPPORTED_MEDIA);

	// Get network address (if any) from the Registry. If specified
	// it will be used instead of burned permanent address:

	if (Config.ReadNetworkAddress(m_CurrentAddress) != NDIS_STATUS_SUCCESS)
		m_CurrentAddress.Invalidate();

	// Read some other config stuff from the Registry, e.g.

	ULONG	uMode;
	Config.Read(KNDIS_STRING_CONST("Mode"), &uMode);

	
    // Now let NDIS know about the BUS the NIC is on. Here's where the NDIS/Adapter-instance
	// handshake takes place. This should happen *before* allocating the h/w resources:
	SetAttributesEx(NdisInterfaceInternal,
		NDIS_ATTRIBUTE_DESERIALIZE);

	// TODO: Read perm address from the card instead
	m_PermanentAddress = m_CurrentAddress;

	// Set default filter and MAC options. In principle, we should also
	// relay that to our card...
	m_uPacketFilter = NDIS_PACKET_TYPE_DIRECTED |
					  NDIS_PACKET_TYPE_MULTICAST |
					  NDIS_PACKET_TYPE_BROADCAST;

	m_uMacOptions = NDIS_MAC_OPTION_TRANSFERS_NOT_PEND  |
				    NDIS_MAC_OPTION_RECEIVE_SERIALIZED  |
					NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
					NDIS_MAC_OPTION_NO_LOOPBACK;

	// Create i/o device. If more then one instance of vdev adapter
	// can exist one shoould add code to handle this as well as
	// racing conditions on deinitilization
	
	NDIS_STATUS  Status = CreateMyDevice();

	T.Trace("VdevAdapter Create Device: %s\n", ((Status)?"FAILED":"OK"));

#if VDEV_EH_DEMO
	// Call Exception Handling Demo code
	try {
		for (int i=0; i<5; i++)	
			DoSomethingBad(i);
	}
	catch (NDIS_STATUS s){
		T.Trace("VdevAdapter::Initialize() caught status %X thrown by DoSomethingBad()\n", s);
	}
#endif

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// VdevAdapter::Halt
//
// MiniportHalt is a required function that deallocates resources when
// the NIC is removed and halts the NIC.
//
// Parameters:
//		None
// IRQL: 
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// NOTE: Miniports has no "unload" for the driver. The Halt() is the last
//       function called into a miniport, so everything should be stopped 
//		 and freed right here. The KNDIS framework takes care of destroying
//		 the adapter object itself, so this member function is merery required
//       to undo things done by the Initialize() above - in the inversed order.
//
//		 Alternatevely, the driver writer can move all the deallocations to the
//       destructor: the destructor will be called immediatelly after Halt() 
//		 returns. Our sample follows this practice.
//
VOID VdevAdapter::Halt(VOID)
{
	T.Trace("VdevAdapter::Halt() Entered\n");

#if VDEV_WIN2K
	// win2k
	if (m_DeviceHandle) {
		NdisMDeregisterDevice(m_DeviceHandle);
		m_DeviceHandle = NULL;
	}

#else
	// win98	
	if (m_pDeviceObject)
		IoDeleteDevice(m_pDeviceObject);		
#endif
	m_pDeviceObject=NULL;
}

////////////////////////////////////////////////////////////////////
// VdevAdapter::Reset
//
// MiniportReset is a required function that issues a hardware reset
// to the NIC and/or resets the driver's software state.
//
// Parameters:
//		AddressingReset
//			Points to a variable that MiniportReset sets to TRUE if the
//			NDIS library should call MiniportSetInformation to restore
//			addressing information to the current values.
// IRQL: 
//		DISPATCH_LEVEL
// Return Mode:
//		Asynchronous
NDIS_STATUS VdevAdapter::Reset
			(OUT PBOOLEAN AddressingReset)
{
	T.Trace("VdevAdapter::Reset() Entered\n");
	// TODO:	Reset the card

	return NDIS_STATUS_SUCCESS;
}
	
////////////////////////////////////////////////////////////////////
// VdevAdapter::Shutdown
//
// MiniportShutdown does nothing more than restore the NIC to its 
// initial state (before the miniport's DriverEntry function runs)
//
// Parameters:
//		None
// IRQL: 
//		If MiniportShutdown is called due to a user-initiated system shutdown,
//		it runs at IRQL PASSIVE_LEVEL in a system-thread context. If it is called
//		due to an unrecoverable error, MiniportShutdown runs at an arbitrary IRQL
//		and in the context of whatever component raised the error. 
// Return Mode:
//		Synchronous
VOID VdevAdapter::Shutdown(VOID)
{
	T.Trace("VdevAdapter::Shutdown() Entered\n");

	// TODO:	Reset the card for good
}

////////////////////////////////////////////////////////////////////
// VdevAdapter::Send
//
// Transfers a protocol-supplied packet over the network
//
// Parameters:
//		Packet 
//			Points to a packet descriptor specifying the data to be transmitted. 
//		Flags 
//			Specifies the packet flags, if any, set by the protocol. 
// IRQL: 
//		DISPATCH_LEVEL
// Return Mode:
//		Asynchronous
// NOTE:
//		The return code determines the ownership of the packet and further
//		interaction between NDIS and our driver. Specifically,
//
//      NDIS_STATUS_SUCCESS - Done with.
//      NDIS_STATUS_PENDING - We keep it until further notice.
//      NDIS_STATUS_RESOURCES - We didn't have the resouces *now*, so tell NDIS
//                              please hold it on until further notice.
// NOTE: Deserialized Miniports should never return NDIS_STATUS_RESOURCES to NDIS,
// and must instead queue it internally.
//
NDIS_STATUS VdevAdapter::Send(
			IN	PNDIS_PACKET			Packet,
			IN	UINT					Flags
			)
{
	T.Trace("VdevAdapter::Send() Entered\n");
	T << Packet;

	// declare for convenience
	KNdisPacket pkt(Packet);

	// TODO:	Manage the packet into the hardware. 
	// Return NDIS_STATUS_SUCCESS if the packet got submitted to the h/w and 
	// NDIS_STATUS_PENDING if it's been queued into m_TxQueue.
	//

	m_GenStats->xmit_ok++;

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// VdevAdapter::ReturnPacket
//
// This method is called when NDIS returns a packet previuosly
// indicated by IndicateReceive() back to the miniport
//
// Parameters:
//		Packet 
//			Points to a packet descriptor specifying the packet 
// IRQL: 
//		DISPATCH_LEVEL
// Return Mode:
//		n/a
// NOTE:
//		The packet returns to the "free list" in the Rx area. 
//		Reclaim() does the necessary reset of the chained buffer
//		and OOB data if any.
VOID VdevAdapter::ReturnPacket(IN PNDIS_PACKET Packet)
{
	T.Trace("VdevAdapter::ReturnPacket() Entered \n");
	T << Packet;

	// declare for convenience
	KNdisPacket pkt(Packet);

	// TODO:	Reclaim the packet back to an internal list
	//

}

////////////////////////////////////////////////////////////////////
// VdevAdapter::CheckForHang
//		Optional function that reports the state of the NIC or monitors
//		 the responsiveness of an underlying device driver.
// Parameters:
//		none
// IRQL: 
//		IRQL DISPATCH_LEVEL
// Return Mode:
//		Synchronous
// NOTE:
//		By default, the NDIS library calls MiniportCheckForHang 
//		approximately every two seconds. If MiniportCheckForHang returns
//		TRUE, NDIS then calls the driver's MiniportReset function. 
BOOLEAN VdevAdapter::CheckForHang() 
{
	// TODO:	Check the h/w for hang
	return FALSE;
}

////////////////////////////////////////////////////////////////////
// VdevAdapter::CreateMyDevice
//		Create an i/o device object accessible from the user mode.
//		NOTE the special case for W98 where NdisMRegisterDevice()
//		is not available.
// Parameters:
//		none
// IRQL: 
//		PASSIVE_LEVEL
// Return:
//		STATUS_SUCCESS if device was created ok
NDIS_STATUS VdevAdapter::CreateMyDevice()
{
	NDIS_STATUS  Status;

#if VDEV_WIN2K
	// Win2k, XP:
	// Fill in the dispatch table array and register device
    static PDRIVER_DISPATCH MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];
	NdisZeroMemory(MajorFunction, sizeof(MajorFunction));
    MajorFunction[IRP_MJ_CREATE] = 
    MajorFunction[IRP_MJ_CLOSE] = 
    MajorFunction[IRP_MJ_CLEANUP] = 
    MajorFunction[IRP_MJ_READ] = 
    MajorFunction[IRP_MJ_WRITE] = 
    MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoDispatch;

	Status = NdisMRegisterDevice(
					*KNdisMiniDriver::DriverInstance(),
					KNDIS_STRING_CONST("\\Device\\MyNdisDevice"),
					KNDIS_STRING_CONST("\\DosDevices\\MyNdisDevice"),
					MajorFunction,
					&m_pDeviceObject,
					&m_DeviceHandle);


#else
	// Win98:
	// Fill in the dispatch table and create device
	PDRIVER_OBJECT pDriverObject = KNdisMiniDriver::DriverInstance()->DriverObject();
    pDriverObject->MajorFunction[IRP_MJ_CREATE] = 
    pDriverObject->MajorFunction[IRP_MJ_CLOSE] = 
    pDriverObject->MajorFunction[IRP_MJ_CLEANUP] = 
    pDriverObject->MajorFunction[IRP_MJ_READ] = 
    pDriverObject->MajorFunction[IRP_MJ_WRITE] = 
    pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoDispatch;

	Status = IoCreateDevice(
					pDriverObject,
					0,
					KNDIS_STRING_CONST("\\Device\\MyNdisDevice"),
					FILE_DEVICE_NETWORK,
					0,
					FALSE,
					&m_pDeviceObject);

	if (STATUS_SUCCESS == Status)
		m_pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;     // Tell the IO system that we are ready

#endif

	return Status;
}
	

////////////////////////////////////////////////////////////////////
// VdevAdapter::IoDispatch
//		Dispatches IRPs coming to device(s) exposed by this driver. 
//		Static method.
// Parameters:
//		DeviceObject		- device object
// IRQL: 
//		PASSIVE_LEVEL
// Return:
//		STATUS
NTSTATUS VdevAdapter::IoDispatch (IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS Status = STATUS_SUCCESS;
	KIrp	I(Irp);

	T.Trace("VdevAdapter::IoDispatch func 0x%02x \n", I.MajorFunction());

	switch (I.MajorFunction()) {
    case IRP_MJ_CREATE:			// TODO
    case IRP_MJ_CLOSE:			// TODO
    case IRP_MJ_CLEANUP:		// TODO
    case IRP_MJ_READ:			// TODO		
    case IRP_MJ_WRITE:			// TODO
    case IRP_MJ_DEVICE_CONTROL: // TODO
			I.Information() = 0;
			break;
	default: 
		Status = STATUS_UNSUCCESSFUL;

	};

	return I.Complete(Status);
}



////////////////////////////////////////////////////////////////////
// VdevAdapter::DoSomethingBad
//	This is a contrived demo function to demonstrate the use of some
//	C++ exception handling techniques in the kernel.
//
// NOTE: If no SE translator is present, a C exception may only be 
//		 caught by catch(...). In this sample, we do install a SE/C++
//		 translator, implemented by KException. This allows Structural Exception 
//		 be caught by a catch (KException) handler. 
//		 KException and KExceptionAccessViolation give a detailed access 
//		 to the attributes of the ecxeption.
//		
// Parameters:
//		What		- Specifies what way to throw:
//					0: Throwing a trivial type. Catches internally.
//					1: Throwing an object. Catches internally.
//					2: Access violation. Catches internally.
//					3: Kernel API raising exception. Catches internally.
//					4+ Throws NDIS_STATUS_NOT_SUPPORTED to the caller
// IRQL: 
//		PASSIVE_LEVEL
// Return:
//		None. Might throw NDIS_STATUS
//
void VdevAdapter::DoSomethingBad(int What)
{
#if VDEV_EH_DEMO

	T.Trace("VdevAdapter::DoSomethingBad(%i)\n", What);

	try {
		// Wrong pointer: dereference causes an exception
		char* ptr = NULL;

		// Object: this object would leak if we used the "C" exception mechanism (SEH).
		//		   Using C++ EH allows object's destructors be called.
		KNdisString	str("aString");

		switch (What) {
			case 0:	// Throwing a trivial type
				throw NDIS_STATUS_FAILURE;

			case 1:	// Throwing an object
				throw KNdisString("Exception Demo String");

			case 2:	// Access violation. Without EH this would cause a blue screen.
					// Now it will be caught by the catch(KExceptionAccessViolation) handler.

				 *ptr = 'X';
				 break;

			case 3:	// Using Kernel API raising exception. 
					// In "C", the call must be enclosed in a __try/__except block. 
					// Now it will be caught by the catch(KExceptionAccessViolation) handler.
				 ::ProbeForWrite(ptr-1, 1, 1);
				 break;

			default: 
				 break;	// Everything else throws
		}

	} // try

	catch (NDIS_STATUS s)				// Catching a trivial type
	{
		T.Trace("Caught Status %X\n", s);
		return;
	}
	catch (KNdisString estr)			// Catching an object
	{
		T.Trace("Caught Object <%ws>\n", PWSTR(estr));
		return;
	}
	catch (KExceptionAccessViolation& e) // SE: Access Violation, including Probes (page fault)	
	{
	    T.Trace("Caught Kernel Fault at Address %p trying to access %p on %s\n",
		  e.At(), e.Address(), e.IsWrite()?"WRITE":"READ");
		return;
	}
	catch (KException& e)				// SE: Kernel Exception other than access violation
										// NOTE: if no KExceptionAccessViolation handler is used
	{                                   //       this handler would catch the violation
	    T.Trace("Caught Kernel Exception %X at Address %p\n", 
		  e.Status(), e.At());
		return;
	}
	catch (...)							// All other types of user-defined exceptions.
	{									// NOTE: if no KException is used, this default handler
										//       would also catch the Structural Exceptions.

	    T.Trace("Caught Default Exception");
		return;
	}

	throw NDIS_STATUS_NOT_SUPPORTED;

#else
	UNREFERENCED_PARAMETER(What);
#endif // VDEV_EH_DEMO

}


// end of file
 

