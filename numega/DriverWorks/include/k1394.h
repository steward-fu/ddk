// k1394.h - include file for class K1394Base and related classes
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
#ifndef __K1394_h__
#define __K1394_h__

#pragma warning (disable:4200)
#include <1394.h>
#pragma warning (default:4200)

#include "k1394Irb.h"

//Forward declarations
class K1394IsochBandwidth;
class K1394IsochChannel;
class K1394IsochResource;

//////////////////////////////////////////////////////////////////////////////
// Class K1394LowerDevice wraps the 1394 bus driver by providing
// functions to allocate and free bus resources, to retreive bus
// information, and to submit IRBs to the bus driver.
//
class K1394LowerDevice : public KPnpLowerDevice
{
public:
	// Constructors
#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
	K1394LowerDevice(
		KDevice*		AttachingDevice,
		PDEVICE_OBJECT	PhysicalDeviceObject,
		NTSTATUS*		pStatus
		);
#endif //!defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)

	K1394LowerDevice(
		PDEVICE_OBJECT	TopOfStackDeviceObject,
		PDEVICE_OBJECT	PhysicalDeviceObject,
		NTSTATUS*		pStatus
		);

	K1394LowerDevice(void);

	////////////////////////////////////////////
	//Initialize functions for use with the constructor with no parameters.
#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
	NTSTATUS Initialize(
		KDevice*		AttachingDevice,
		PDEVICE_OBJECT	PhysicalDeviceObject
		);
#endif //!defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)

	NTSTATUS Initialize(
		PDEVICE_OBJECT	TopOfStackDeviceObject,
		PDEVICE_OBJECT	PhysicalDeviceObject
		);

	/////////////////////////////////////////////
	//Destructor
	~K1394LowerDevice(void);

	//////////////////////////////////////////////
	//SubmitIrb
	NTSTATUS K1394LowerDevice::SubmitIrb(
		PIRP					pIrp,
		PIRB					pIrb,
		PIO_COMPLETION_ROUTINE	CompletionRoutine,
		PVOID					Context);

	NTSTATUS SubmitIrb(
		PIRB					pIrb,
		PIO_COMPLETION_ROUTINE	CompletionRoutine,
		PVOID					Context);

	NTSTATUS SubmitIrb(
		PIRB					pIrb,
		PIRP					pIrp = NULL);


	//Allocate Bandwidth
	NTSTATUS Allocate(K1394IsochBandwidth&	Bandwidth,
					  ULONG					Speed,
					  ULONG					MaxBytesPerFrame);

	//Free Bandwidth
	NTSTATUS Free(K1394IsochBandwidth& Bandwidth);

	//Change Bandwidth
	NTSTATUS Change(K1394IsochBandwidth& Bandwidth, ULONG MaxBytesPerFrame);

	//Reallocate Bandwidth
	NTSTATUS Reallocate(K1394IsochBandwidth& Bandwidth);

	//Allocate a channel
	NTSTATUS Allocate(K1394IsochChannel&	Channel,
					  ULONG					ChannelNumber = ISOCH_ANY_CHANNEL);

	//Reallocate a channel
	NTSTATUS Reallocate(K1394IsochChannel& Channel);

	//Free a channel
	NTSTATUS Free(K1394IsochChannel& Channel);

	///////////////////////////////////////////////////////////////////
	//Isochronous resource
	NTSTATUS Allocate(
		K1394IsochResource&	Resource,
		ULONG				Speed,
		ULONG				Flags,
		ULONG				Channel,
		ULONG				MaxBytesPerFrame,
		ULONG				NumberOfBuffers,
		ULONG				MaxBufferSize,
		ULONG				QuadletsToStrip = 0);

	NTSTATUS Allocate(
		K1394IsochResource&		Resource,
		K1394IsochBandwidth&	Bandwidth,
		K1394IsochChannel&		Channel,
		ULONG					Flags,
		ULONG					NumberOfBuffers,
		ULONG					MaxBufferSize,
		ULONG					QuadletsToStrip = 0);

	NTSTATUS Free(K1394IsochResource& Resource);

	//*** Informational operations******
	NTSTATUS ConfigRom(PVOID ConfigRom, PULONG Length);

	LARGE_INTEGER HostUniqueID();

	NTSTATUS PowerInformation(PULONG Watts, PULONG Voltage);

	NTSTATUS HostCapabilities(PULONG CapabilityFlags,
							   PULONG MaxAsyncReadSize,
							   PULONG MaxAsyncWriteSize);

	NTSTATUS AddressMappingRoutine(PGET_LOCAL_HOST_INFO4 pMappingRoutine);

	NTSTATUS SetGapCount(ULONG GapCountLowerBound);

	NTSTATUS GetSpeedBetweenDevices(
		ULONG			DeviceObjectCount,
		PDEVICE_OBJECT*	pDeviceObjects,
		ULONG			SourceDevice,
		PULONG			pSpeed);

	NTSTATUS GetGenerationCount(PULONG pGenerationCount);

	NTSTATUS BusReset(BOOLEAN bForceRootNode = FALSE);

	NTSTATUS RegisterBusResetNotification(
		PBUS_BUS_RESET_NOTIFICATION		ResetRoutine,
		PVOID							ResetContext
		);

	NTSTATUS DeregisterBusResetNotification(
		PBUS_BUS_RESET_NOTIFICATION		ResetRoutine,
		PVOID							ResetContext
		);

protected:
	//Completion context for SubmitIrb
	typedef struct _ASYNC_COMPLETE_IRB
	{
		PIO_COMPLETION_ROUTINE pCompletionRoutine;
		PVOID pContext;

		_ASYNC_COMPLETE_IRB(PIO_COMPLETION_ROUTINE pcr, PVOID pc) :
			pCompletionRoutine(pcr),
			pContext(pc) {}
	} ASYNC_COMPLETE_IRB, *PASYNC_COMPLETE_IRB;

	//Completion routines for SubmitIrb
	static NTSTATUS __stdcall
		K1394LowerDevice::SyncCompleteSubmitIrb(
		PDEVICE_OBJECT		DeviceObject,
		PIRP				Irp,
		PVOID				Context
		);

	static NTSTATUS __stdcall
		K1394LowerDevice::AsyncCompleteSubmitIrb(
		PDEVICE_OBJECT		DeviceObject,
		PIRP				Irp,
		PVOID				Context
		);

private:
	K1394LowerDevice(K1394LowerDevice&){}				// do not allow copy
	K1394LowerDevice& operator=(K1394LowerDevice&){}	// do not allow assignment
};

///////////////////////////////////////////////////////////////////////
//Class K1394Configuration provides storage and accessors for some
// information from the device's config ROM, including the vendor name,
// model name, unit directory, and unit dependent directory.
//
class K1394Configuration
{
	SAFE_DESTRUCTORS
public:
	typedef struct _UNIT_DIRECTORY
	{
		IO_ADDRESS	PhysicalAddress;
		PVOID		pDirectory;
		ULONG		DirectoryLength;

		operator ULONG() { return DirectoryLength;}
		operator IO_ADDRESS() { return PhysicalAddress; }
		operator PVOID() { return pDirectory; }

	} UNIT_DIRECTORY, *PUNIT_DIRECTORY;

public:
	K1394Configuration();
	K1394Configuration(K1394LowerDevice* pLowerDevice) { Initialize(pLowerDevice); }
	~K1394Configuration();

	NTSTATUS Initialize(K1394LowerDevice* pLowerDevice);

	BOOLEAN IsAllocated() { return m_IsInitialized; }

	KUstring VendorName() { return m_VendorName; }
	KUstring ModelName() { return m_ModelName; }
	UNIT_DIRECTORY& UnitDirectory() { return m_UnitDirectory; }
	UNIT_DIRECTORY&	UnitDependentDirectory() { return m_UnitDependentDirectory; }
	CONFIG_ROM&		ConfigRom() { return m_ConfigRom; }

protected:
	bool			m_IsInitialized;

	KUstring		m_VendorName;
	KUstring		m_ModelName;
	UNIT_DIRECTORY	m_UnitDirectory;
	UNIT_DIRECTORY	m_UnitDependentDirectory;
	CONFIG_ROM		m_ConfigRom;

private:
	K1394Configuration(K1394Configuration&){}				// do not allow copy
	K1394Configuration& operator=(K1394Configuration&){}	// do not allow assignment
};

////////////////////////////////////////////
// In-lines
///////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// K1394LowerDevice
//////////////////////////////////////////////////////////////////////////////

#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
inline K1394LowerDevice::K1394LowerDevice(
										  KDevice* AttachingDevice,
										  PDEVICE_OBJECT PhysicalDeviceObject,
										  NTSTATUS* pStatus
										  ) :
KPnpLowerDevice(AttachingDevice, PhysicalDeviceObject, pStatus)
{
}
#endif //!defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)

//If this constructor is used, the Initialize function MUST be called
//before the class is used.

inline K1394LowerDevice::K1394LowerDevice(void):
KPnpLowerDevice(){}

///////////////////////////////////////////
//This is the constructor in cases where the FDO has already been
//created and attached to the device stack.

inline K1394LowerDevice::K1394LowerDevice(
										  PDEVICE_OBJECT TopOfStackDeviceObject,
										  PDEVICE_OBJECT PhysicalDeviceObject,
										  NTSTATUS* pStatus
										  ) :
KPnpLowerDevice(TopOfStackDeviceObject, PhysicalDeviceObject, pStatus)
{
}


#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
inline
NTSTATUS K1394LowerDevice::Initialize(
									  KDevice* AttachingDevice,
									  PDEVICE_OBJECT PhysicalDeviceObject
									  )
{
	NTSTATUS status = KPnpLowerDevice::Initialize(AttachingDevice, PhysicalDeviceObject);
	return status;
}
#endif //!defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)

inline
NTSTATUS K1394LowerDevice::Initialize(
									  PDEVICE_OBJECT TopOfStackDeviceObject,
									  PDEVICE_OBJECT PhysicalDeviceObject
									  )
{
	NTSTATUS status = KPnpLowerDevice::Initialize(TopOfStackDeviceObject, PhysicalDeviceObject);
	return status;
}

inline NTSTATUS K1394LowerDevice::AddressMappingRoutine(PGET_LOCAL_HOST_INFO4 pMappingRoutine)
{
	KIrb<REQUEST_GET_LOCAL_HOST_INFO> Irb(GET_PHYS_ADDR_ROUTINE, pMappingRoutine);

	return  SubmitIrb(Irb);
}


//////////////////////////////////////////////////////////////////////////////
// K1394LowerDevice::~K1394LowerDevice
//
// Destructor for K1394Lowerdevice

inline	K1394LowerDevice::~K1394LowerDevice(void){}

////////////////////////////////////////////////////////////////////////
//K1394LowerDevice::SubmitIrb
//
// Asynchronously submits an IRB to the lower device using the provided
//  IRP
//
// Parameters:
//	pIrp - Irp to be passed to the lower device
//	pIrb - Irb to be passed to the lower device
//  CompletionRoutine - routine to be called when the IRP completes
//	Context - Context pointer to be passed to the completion routine
//
// Returns:
//	The status returned by the lower device
//
// Comments:
inline NTSTATUS K1394LowerDevice::SubmitIrb(
		PIRP					pIrp,
		PIRB					pIrb,
		PIO_COMPLETION_ROUTINE	CompletionRoutine,
		PVOID					Context)
{
	KIrp I(pIrp);

	//Fill in the IRP
	I.MajorFunction(NEXT) = IRP_MJ_INTERNAL_DEVICE_CONTROL;
	I.IoctlCode(NEXT) = IOCTL_1394_CLASS;
	I.Irb(NEXT) = pIrb;
	I.SetCompletionRoutine(CompletionRoutine, Context, TRUE, TRUE, TRUE);

	return Call(I);
}

//////////////////////////////////////////////////////////////////////////////
// Class K1394GenerationCount wraps the 1394 bus reset generation counter
//
class K1394GenerationCount
{
	SAFE_DESTRUCTORS
public:
	K1394GenerationCount() :
	  m_pBus(NULL),
	  m_bBusResetNotification(false),
	  m_nGenerationCount(0) {}

	K1394GenerationCount(K1394LowerDevice* pBus) :
	  m_bBusResetNotification(false),
	  m_nGenerationCount(0)
	{
		  Initialize(pBus);
	}

	//////////////////////////////////////////////////
	// initialization routine
	NTSTATUS Initialize(K1394LowerDevice* pBus)
	{
		m_pBus = pBus;

		NTSTATUS status = m_pBus->RegisterBusResetNotification(BusResetNotification, this);

		if ( NT_SUCCESS(status) )
		{
			m_bBusResetNotification = true;

			status = m_pBus->GetGenerationCount(&m_nGenerationCount);
		}

		return status;
	}

	////////////////////////////////////////////
	//Deinitialize function to free resources
	NTSTATUS Deinitialize()
	{
		NTSTATUS status = STATUS_SUCCESS;

		if ( m_bBusResetNotification )
		{
			status = m_pBus->DeregisterBusResetNotification(BusResetNotification, this);
			if ( NT_SUCCESS(status) )
				m_bBusResetNotification = false;
		}

		return status;
	}

	////////////////////////////////////////////
	// Access function to m_nGenerationCount
	inline operator ULONG()
	{
		return m_nGenerationCount;
	}

	/////////////////////////////////////////////
	//Destructor
	~K1394GenerationCount(void)
	{
		Deinitialize();
	}

protected:
	//////////////////////////////////////////////////
	// internal bus reset notification callback
	// It is used to maintain internal state in synch
	// with 1394 bus
	static void __stdcall BusResetNotification(PVOID pContext)
	{
		K1394GenerationCount* pCount = reinterpret_cast<K1394GenerationCount*>(pContext);

		KIrbWithContext<REQUEST_GET_GENERATION_COUNT, PULONG>* pIrb =
			new KIrbWithContext<REQUEST_GET_GENERATION_COUNT, PULONG>;

		if ( pIrb )
		{
			pIrb->m_Context = &pCount->m_nGenerationCount;
			pCount->m_pBus->SubmitIrb(pIrb, GenerationCountRoutine, pIrb);
		}
	}

	//////////////////////////////////////////////////
	// Generation count request completion routine
	static NTSTATUS __stdcall GenerationCountRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID pContext)
	{
		KIrbWithContext<REQUEST_GET_GENERATION_COUNT, PULONG>* pIrb =
			reinterpret_cast<KIrbWithContext<REQUEST_GET_GENERATION_COUNT, PULONG>*>(pContext);

		*pIrb->m_Context = pIrb->GenerationCount();
		delete pIrb;

		return STATUS_SUCCESS;

		UNREFERENCED_PARAMETER(DeviceObject);
		UNREFERENCED_PARAMETER(Irp);
	}

protected:
	//////////////////////////////////////////////////
	// data
	K1394LowerDevice*	m_pBus;							// pointer to 1394 lower device
	bool				m_bBusResetNotification;		// true if we registered a bus reset notification callback
														// this class can use it at any IRQL level.
private:
	//////////////////////////////////////////////////
	// data
	ULONG				m_nGenerationCount;				// internally maintained generation count, so a client of

private:
	K1394GenerationCount(K1394GenerationCount&){}				// do not allow copy
	K1394GenerationCount& operator=(K1394GenerationCount&){}	// do not allow assignment
};

//////////////////////////////////////////////////////////////////////////////
// Macros
//////////////////////////////////////////////////////////////////////
//
// Macro MEMBER_1394_BUS_RESET_NOTIFICATION_ROUTINE
//
// To make a 1394 bus reset notification routine a member in any
// class, use the following macro inside the definition of the class. If the
// bus reset notification routine member name is X, then the
// ResetRoutine parameter in the K1394LowerDevice::BusResetNotification function
// as LinkTo(X). Furthermore, the ResetContext parameter of that same function
// must be the "this" pointer of the class containing the bus reset
// notification routine.
//
// The first parameter to the macro is the name of class in which the
// bus reset notification routine member is being declared. The second
// parameter is the name of the bus reset notification routine function.
//
// The bus reset notification routine member declared by this macro has
// the following prototype:
//
// VOID class_name::function_name(void);
//
// NOTE This routine will be called at DISPATCH_LEVEL
//
#define MEMBER_1394_BUS_RESET_NOTIFICATION_ROUTINE(objclass, func)		\
	VOID __stdcall func(void);											\
	static VOID __stdcall LinkTo(func)(IN PVOID Context)				\
			{															\
				((objclass*)Context)->func();							\
			}

////////////////////////////////////////////////////////////////
// MEMBER_1394_IRB_COMPLETION_ROUTINE
//
// This macro sets up a callback routine to be called when the
// 1394 bus is done processing an IRB.  It can be used as a callback
// for any of the SubmitIrb functions, or as a completion routine
// for K1394IsochTransfer::AddBufferList and
// K1394IsochTransfer::RemoveBufferList

#define MEMBER_1394_IRB_COMPLETION_ROUTINE(objclass, func)					\
	NTSTATUS __stdcall func(KIrp I);										\
	static NTSTATUS __stdcall LinkTo(func)(	PDEVICE_OBJECT DeviceObject,	\
											PIRP Irp,						\
											PVOID Context)					\
	{																		\
		KIrp I(Irp);														\
		return ((objclass*)Context)->func(I);								\
	}

#endif
