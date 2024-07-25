// kusb.h - include file for class KUsbObject and related classes
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

extern "C" {
#if ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION <= 0x10))
	#pragma warning (disable:4200)
	#include <usbdi.h>
	#include <usbdlib.h>
	#pragma warning (default:4200)
#else
	#include <usb.h>
	#include <usbdrivr.h>
	#include <usbdlib.h>
#endif
}


#define DEFAULT_CONFIG_SIZE	4096

//Macro to get the size of the PreconfiguredInterfaceListEntry
#define GET_INTF_LIST_ENTRY_SIZE(numEndpoints) (sizeof(PreconfiguredInterfaceListEntry) + \
                        (sizeof(USBD_PIPE_INFORMATION)*(numEndpoints)) \
                         - sizeof(USBD_PIPE_INFORMATION))

// status codes for KUsbLowerDevice::ActivateConfiguration
enum AC_STATUS
{
	AC_SUCCESS = 0,
	AC_COULD_NOT_LOCATE_INTERFACE,
	AC_COULD_NOT_PRECONFIGURE_INTERFACE,
	AC_CONFIGURATION_REQUEST_FAILED,
	AC_FAILED_TO_INITIALIZE_INTERFACE_OBJECT,
	AC_FAILED_TO_LOCATE_ENDPOINT_ADDRESS,
	AC_NO_SUCH_CONFIGURATION,
	AC_FAILED_TO_GET_DESCRIPTOR,
	AC_FAILED_TO_OPEN_PIPE_OBJECT
};

enum SA_STATUS // return values for KUsbInterface::SetAlternate
{
	SA_SUCCESS = 0,
	SA_INSUFFICIENT_RESOURCES,
	SA_SELECT_INTERFACE_REQUEST_FAILED,
	SA_FAILED_TO_LOCATE_ENDPOINT_ADDRESS,
	SA_COULD_NOT_LOCATE_INTERFACE,
	SA_FAILED_TO_UPDATE_INTF_LIST
};

class KUsbLowerDevice; // Forward

struct SyncCompleteSubmitUrbContext_t
{
	KEvent*				pEvent;
	NTSTATUS			Status;
#if DBG
	KUsbLowerDevice*	pLD;
	PURB				pUrb;
#endif
};

struct AsyncCompleteSubmitUrbContext_t
{
	PIO_COMPLETION_ROUTINE CompletionRoutine;
	PVOID				Context;
#if DBG
	KUsbLowerDevice*	pLD;
	PURB				pUrb;
#endif
};

////////////////////////////////////////////////////////////////////////////
// Internal use
//
struct PreconfiguredInterfaceListEntry
{
	LIST_ENTRY m_ListEntry;
	PUSB_INTERFACE_DESCRIPTOR m_Descriptor;
	USBD_INTERFACE_INFORMATION m_InterfaceInformation;
};

//////////////////////////////////////////////////////////////////////////////
class KUsbInterface; // forward
class KUsbPipe; // forward
struct USB_ENDPOINT; // forward
//////////////////////////////////////////////////////////////////////////////
// class KUsbLowerDevice
//
// This class abstracts the default control pipe component of the USB device
// and the connection between device drivers in the device stack
//
class KUsbLowerDevice : public KPnpLowerDevice
{
// Constructors/Destructors
public:
	KUsbLowerDevice(void);

#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
	KUsbLowerDevice(
		KDevice* AttachingDevice,
		PDEVICE_OBJECT PhysicalDeviceObject,
		NTSTATUS* pStatus
		);

	NTSTATUS Initialize(
		KDevice* AttachingDevice,
		PDEVICE_OBJECT PhysicalDeviceObject
		);
#endif // !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)

	KUsbLowerDevice(
		PDEVICE_OBJECT TopOfStackDeviceObject,
		PDEVICE_OBJECT PhysicalDeviceObject,
		NTSTATUS* pStatus
		);

	NTSTATUS Initialize(
		PDEVICE_OBJECT TopOfStackDeviceObject,
		PDEVICE_OBJECT PhysicalDeviceObject
		);

	SAFE_DESTRUCTORS
	~KUsbLowerDevice(void);
	void ReleaseResources(void);

private:
	//The copy constructor and operator= are declared private to prevent the compiler
	//from generating them. These functions are not implemented.
	KUsbLowerDevice(KUsbLowerDevice&);
	KUsbLowerDevice& operator=(KUsbLowerDevice&);

// Configuration methods
public:
	NTSTATUS Preconfigure(
		UCHAR ConfigIndex=0,
		ULONG MaxConfigSize=DEFAULT_CONFIG_SIZE
		);

	PUSB_INTERFACE_DESCRIPTOR LocateInterface(
		PVOID* pStart,
		PUSB_ENDPOINT_DESCRIPTOR* ppEndpoints,
		LONG InterfaceNumber = -1,
		LONG AlternateSetting = -1,
		LONG InterfaceClass = -1,
		LONG InterfaceSubClass = -1,
		LONG InterfaceProtocol = -1
		);

	NTSTATUS PreconfigureInterface(
		PUSB_INTERFACE_DESCRIPTOR pInterfaceDesc
		);

	NTSTATUS Configure(void);

	NTSTATUS Unconfigure(
		PIO_COMPLETION_ROUTINE CompletionRoutine=NULL,
		PVOID Context=NULL
		);

	AC_STATUS ActivateConfiguration(
		UCHAR ConfigurationValue,
		ULONG nEndpointsDescribed,
		USB_ENDPOINT* Endpoints,
		USB_ENDPOINT** ppProblem=NULL,
		ULONG MaxConfigSize=1024
		);

	AC_STATUS ActivateConfiguration(
		UCHAR ConfigurationValue,
		ULONG MaxConfigSize=1024
		);

	NTSTATUS DeActivateConfiguration(
		PIO_COMPLETION_ROUTINE pfnCompletionRoutine = NULL,
		PVOID pContext = NULL
		);

	//used internally
	NTSTATUS RegisterPipe(KUsbPipe* pipe);

	//used internally
	NTSTATUS RegisterInterface(KUsbInterface* intf);

protected:
	NTSTATUS Configure(
		ULONG nEndpoints,
		USB_ENDPOINT* Endpoints,
		PUSB_ENDPOINT_DESCRIPTOR pFirstDescritor
		);

	NTSTATUS Configure(
		KArray<KUsbPipe*>& PipeArray,
		PUSB_ENDPOINT_DESCRIPTOR pFirstEndptDesc
		);

	void InitializePreConfiguredInterface(
		PUSB_INTERFACE_DESCRIPTOR pInterfaceDesc,
		PreconfiguredInterfaceListEntry* pLE
		);

	NTSTATUS UpdateConfiguredInterface(
		PUSB_INTERFACE_DESCRIPTOR pInterfaceDesc
		);

// Default Control Pipe Requests
public:
	NTSTATUS GetDeviceDescriptor(PUSB_DEVICE_DESCRIPTOR pDevDesc);

	NTSTATUS GetStringDescriptor(
		UCHAR Index,
		PWCHAR pStr,
		UCHAR MaxLen,
		SHORT LangId = 0x409
		);

	ULONG GetCurrentFrameNumber(void);

	NTSTATUS GetStatus(
		PUSHORT pStatus,
		PIO_COMPLETION_ROUTINE CompletionRoutine=NULL,
		PVOID Context=NULL
		);

	NTSTATUS SetFeature(
		USHORT Feature,
		PIO_COMPLETION_ROUTINE CompletionRoutine=NULL,
		PVOID Context=NULL
		);

	NTSTATUS ClearFeature(
		USHORT Feature,
		PIO_COMPLETION_ROUTINE CompletionRoutine=NULL,
		PVOID Context=NULL
		);

// Methods to call lower driver with an URB
public:
	NTSTATUS SubmitUrb(
		PURB pUrb,
		PIO_COMPLETION_ROUTINE CompletionRoutine = NULL,
		PVOID Context=NULL,
		ULONG mSecTimeOut=0
		);

	NTSTATUS SubmitUrb(
		KIrp I,
		PURB pUrb,
		PIO_COMPLETION_ROUTINE CompletionRoutine = NULL,
		PVOID Context=NULL,
		ULONG mSecTimeOut=0
		);

protected:
	static NTSTATUS __stdcall SyncCompleteSubmitUrb(
		PDEVICE_OBJECT	DeviceObject,
		PIRP			Irp,
		PVOID			Context
		);

	static NTSTATUS __stdcall AsyncCompleteSubmitUrb(
		PDEVICE_OBJECT	DeviceObject,
		PIRP			Irp,
		PVOID			Context
		);

// Methods to create/initialize URBs
public:
	PURB BuildClassRequest(
		PUCHAR TransferBuffer,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL,
		USHORT Index=0,
		USHORT Function=URB_FUNCTION_CLASS_DEVICE,
		PURB pUrb=NULL
		);

	PURB BuildClassRequest(
		KMemory& TransferBufferMDL,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL,
		USHORT Index=0,
		USHORT Function=URB_FUNCTION_CLASS_DEVICE,
		PURB pUrb=NULL
		);

	PURB BuildVendorRequest(
		PUCHAR TransferBuffer,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL,
		USHORT Index=0,
		USHORT Function=URB_FUNCTION_VENDOR_DEVICE,
		PURB pUrb=NULL
		);

	PURB BuildVendorRequest(
		KMemory& TransferBufferMDL,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL,
		USHORT Index=0,
		USHORT Function=URB_FUNCTION_VENDOR_DEVICE,
		PURB pUrb=NULL
		);

protected:
	PURB _build_class_or_vendor_request(
		USHORT Function,
		BOOLEAN bIn,
		BOOLEAN bShortOk,
		ULONG TransferBufferLength,
		PVOID TransferBuffer,
		PMDL TransferBufferMDL,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		USHORT Index,
		PURB Link,
		PURB pExistingUrb = NULL
		);

	NTSTATUS _get_status(
		UCHAR RequestType,
		USHORT Index,
		PUSHORT pStatus,
		PIO_COMPLETION_ROUTINE CompletionRoutine,
		PVOID CompletionContext
		);

	NTSTATUS _set_or_clear_feature(
		UCHAR RequestType,
		USHORT Index,
		USHORT Feature,
		PIO_COMPLETION_ROUTINE CompletionRoutine,
		PVOID Context
		);

// Accessors
public:
	KArray<class KUsbPipe *>& PipeArray(void){ return m_PipeArray; }
	KArray<class KUsbInterface *>& InterfaceArray(void){ return m_IntfArray; }

// Data members
protected:
	USBD_CONFIGURATION_HANDLE m_ConfigurationHandle;
	PUSBD_INTERFACE_LIST_ENTRY m_ConfiguredInterfaces;
	KList<PreconfiguredInterfaceListEntry> m_InterfaceList;
	KArray<class KUsbPipe *> m_PipeArray;
	KArray<class KUsbInterface *> m_IntfArray;

public:
	PUSB_CONFIGURATION_DESCRIPTOR m_Config;

// Friends
	friend class KUsbPipe;
	friend class KUsbInterface;
};

//////////////////////////////////////////////////////////////////////////////
// class KUsbInterface
//
// This class abstracts a USB interface
//
class KUsbInterface
{
// Constructors/Destructors
public:
	KUsbInterface(
		KUsbLowerDevice& Device,
		UCHAR InterfaceNumber,
		NTSTATUS* pStatus
		);

	KUsbInterface(void);

	NTSTATUS Initialize(
		KUsbLowerDevice& Device,
		UCHAR InterfaceNumber
		);

	KUsbInterface(
		KUsbLowerDevice& dev,
		UCHAR InterfaceNumber,
		UCHAR ConfigurationValue,
		UCHAR InitialAlternateSetting,
		NTSTATUS* pStatus
		);

	NTSTATUS Initialize(
		KUsbLowerDevice& dev,
		UCHAR InterfaceNumber,
		UCHAR ConfigurationValue,
		UCHAR InitialAlternateSetting
		);

	SAFE_DESTRUCTORS

	BOOLEAN IsValid(void){return m_pLowerDevice != NULL;}

// Methods to open and close interface object (establish binding between KUsbInterface object
// and an active USB interface)
public:
	BOOLEAN IsOpen(void){return m_Information != NULL;}
	NTSTATUS Open(void);
	void Close(void);

// Accessors
public:
	UCHAR InterfaceNumber(void);
	UCHAR AlternateSetting(void);
	UCHAR Class(void);
	UCHAR Subclass(void);
	UCHAR Protocol(void);
	ULONG NumberOfPipes(void);
	PUSBD_PIPE_INFORMATION Pipes(int Index=0);
	KUsbLowerDevice* UsbLowerDevice(void){return m_pLowerDevice;}

// Methods to create/initialize URBs
public:
	PURB BuildClassRequest(
		PUCHAR TransferBuffer,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL,
		PURB pUrb=NULL
		);

	PURB BuildClassRequest(
		KMemory& TransferBufferMDL,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL,
		PURB pUrb=NULL
		);

	PURB BuildVendorRequest(
		PUCHAR TransferBuffer,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL,
		PURB pUrb=NULL
		);

	PURB BuildVendorRequest(
		KMemory& TransferBufferMDL,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL,
		PURB pUrb=NULL
		);

	PURB BuildSelectAlternateRequest(
		UCHAR AlternateSetting
		);

	PURB BuildSelectAlternateRequest(
		UCHAR AlternateSetting,
		PUSB_INTERFACE_DESCRIPTOR pIntfDesc
		);

// Interface Requests (send a USB command for an interface)
public:
	SA_STATUS SelectAlternate(
		UCHAR alternate,
		ULONG nEndpointsDescribed,
		USB_ENDPOINT* Endpoints,
		USB_ENDPOINT** ppProblem=NULL
		);

	SA_STATUS SelectAlternate(
		UCHAR AlternateSetting
		);

	NTSTATUS GetStatus(
		PUSHORT pStatus,
		PIO_COMPLETION_ROUTINE CompletionRoutine=NULL,
		PVOID Context=NULL
		);

	NTSTATUS SetFeature(
		USHORT Feature,
		PIO_COMPLETION_ROUTINE CompletionRoutine=NULL,
		PVOID Context=NULL
		);

	NTSTATUS ClearFeature(
		USHORT Feature,
		PIO_COMPLETION_ROUTINE CompletionRoutine=NULL,
		PVOID Context=NULL
		);

// Methods to call lower driver with an URB
public:
	NTSTATUS SubmitUrb(
		PURB pUrb,
		PIO_COMPLETION_ROUTINE CompletionRoutine = NULL,
		PVOID Context=NULL,
		ULONG mSecTimeOut=0
		);

	NTSTATUS SubmitUrb(
		KIrp I,
		PURB pUrb,
		PIO_COMPLETION_ROUTINE CompletionRoutine = NULL,
		PVOID Context=NULL,
		ULONG mSecTimeOut=0
		);

// Data Members
public:
	PUSBD_INTERFACE_INFORMATION m_Information;
	UCHAR m_CurrentAlternateSetting;

protected:
	UCHAR m_ConfigurationValue;
	UCHAR m_InterfaceNumber;

	KUsbLowerDevice* m_pLowerDevice;
	USBD_CONFIGURATION_HANDLE m_ConfigurationHandle;
	PUSB_INTERFACE_DESCRIPTOR m_Descriptor;

// friends
	friend class KUsbLowerDevice;
};

//////////////////////////////////////////////////////////////////////////////
// class KUsbPipe
//
//  This class abstracts the USB pipe (endpoint connection)
//
class KUsbPipe
{
// Constructors/Destructors
public:
	KUsbPipe(KUsbInterface& Interface, int Index=0);

	KUsbPipe(void);

	VOID Initialize(KUsbInterface& Interface, int Index=0);

	KUsbPipe(
		KUsbLowerDevice& dev,
		UCHAR ea,
		ULONG MaxTransferSize = 0
		);

	NTSTATUS Initialize(
		KUsbLowerDevice& dev,
		UCHAR ea,
		ULONG MaxTransferSize = 0
		);

	SAFE_DESTRUCTORS

// Accessors
public:
	USBD_PIPE_TYPE Type(void);
	USHORT MaximumPacketSize(void);
	UCHAR EndpointAddress(void);
	UCHAR PollInterval(void);
	USBD_PIPE_HANDLE Handle(void);
	ULONG MaximumTransferSize(void);
	VOID SetMaximumTransferSize(ULONG MaxSize);
	KUsbInterface* Interface(void){return m_pInterface;}

// USB Pipe Requests (Send a USB command for a specific pipe)
public:
	NTSTATUS Reset(void);
	NTSTATUS Abort(void);

	NTSTATUS GetStatus(
		PUSHORT pStatus,
		PIO_COMPLETION_ROUTINE CompletionRoutine=NULL,
		PVOID Context=NULL
		);

	NTSTATUS SetFeature(
		USHORT Feature,
		PIO_COMPLETION_ROUTINE CompletionRoutine=NULL,
		PVOID Context=NULL
		);

	NTSTATUS ClearFeature(
		USHORT Feature,
		PIO_COMPLETION_ROUTINE CompletionRoutine=NULL,
		PVOID Context=NULL
		);

// Methods to create/initialize URBs
public:
	PURB BuildInterruptTransfer(
		KMemory& Mdl,
		ULONG Length,
		BOOLEAN bShortOk=TRUE,
		PURB Link=NULL,
		PURB pUrb=NULL,
		BOOLEAN bIn=TRUE
		);

	PURB BuildInterruptTransfer(
		PVOID Buffer,
		ULONG Length,
		BOOLEAN bShortOk=TRUE,
		PURB Link=NULL,
		PURB pUrb=NULL,
		BOOLEAN bIn=TRUE
		);

	PURB BuildIsochronousTransfer(
		ULONG NumberOfPackets,
		ULONG PacketSize,
		BOOLEAN bIn,
		BOOLEAN bASAP,
		ULONG StartFrame,
		PVOID Buffer,
		ULONG Length,
		PURB pUrb = NULL
		);

	PURB BuildIsochronousTransfer(
		ULONG NumberOfPackets,
		ULONG PacketSize,
		BOOLEAN bIn,
		BOOLEAN bASAP,
		ULONG StartFrame,
		KMemory& Mdl,
		ULONG Length,
		PURB pUrb = NULL
		);

	PURB BuildControlTransfer(
		PVOID Buffer,
		ULONG Length,
		BOOLEAN bIn,
		PURB Link=NULL,
		PURB pUrb=NULL
		);

	PURB BuildControlTransfer(
		KMemory& Mdl,
		ULONG Length,
		BOOLEAN bIn,
		PURB Link=NULL,
		PURB pUrb=NULL
		);

	PURB BuildBulkTransfer(
		PVOID Buffer,
		ULONG Length,
		BOOLEAN bIn,
		PURB Link=NULL,
		BOOLEAN bShortOk=FALSE,
		PURB pUrb=NULL
		);

	PURB BuildBulkTransfer(
		KMemory& Mdl,
		ULONG Length,
		BOOLEAN bIn,
		PURB Link=NULL,
		BOOLEAN bShortOk=FALSE,
		PURB pUrb=NULL
		);

	PURB BuildClassRequest(
		PUCHAR TransferBuffer,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL
		);

	PURB BuildClassRequest(
		KMemory& TransferBufferMDL,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL
		);

	PURB BuildVendorRequest(
		PUCHAR TransferBuffer,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL,
		PURB pUrb=NULL
		);

	PURB BuildVendorRequest(
		KMemory& TransferBufferMDL,
		ULONG TransferBufferLength,
		UCHAR RequestTypeReservedBits,
		UCHAR Request,
		USHORT Value,
		BOOLEAN bIn=FALSE,
		BOOLEAN bShortOk=FALSE,
		PURB Link=NULL,
		PURB pUrb=NULL
		);

protected:
	PURB __build_iso_xfer(
		ULONG NumberOfPackets,
		ULONG PacketSize,
		BOOLEAN bIn,
		BOOLEAN bASAP,
		ULONG StartFrame,
		ULONG Length,
		PURB pUrb
		);

// Methods to call lower driver with an URB
public:
	NTSTATUS SubmitUrb(
		PURB pUrb,
		PIO_COMPLETION_ROUTINE CompletionRoutine = NULL,
		PVOID Context=NULL,
		ULONG mSecTimeOut=0
		);

	NTSTATUS SubmitUrb(
		KIrp I,
		PURB pUrb,
		PIO_COMPLETION_ROUTINE CompletionRoutine = NULL,
		PVOID Context=NULL,
		ULONG mSecTimeOut=0
		);

// Methods to open and close pipe object (establish binding between KUsbPipe object
// and an active USB pipe)
public:
	BOOLEAN IsOpen(void){return m_Information != NULL;}

	NTSTATUS Open(
		KUsbInterface& Intf
		);

	void Close(void);

// Data Members
public:
	UCHAR m_EndpointAddress;
	ULONG m_SetMaxTransferSize;

protected:
	PUSBD_PIPE_INFORMATION m_Information;
	KUsbInterface* m_pInterface;
	int m_Index;
};

//////////////////////////////////////////////////////////////////
// This structure is used to supply configuration info to
// KUsbLowerDevice::ActivateConfiguration
//
struct USB_ENDPOINT
{
	USB_ENDPOINT(
		UCHAR			ConfigurationValue,
		UCHAR			InterfaceNumber,
		UCHAR			InterfaceAlternateSetting,
		UCHAR			EndpointAddress,
		KUsbInterface* 	pInterfaceObjectToInitialize,
		KUsbPipe* 		pPipeObjectToInitialize
		);

	USB_ENDPOINT(void) {}

	VOID Initialize(
		UCHAR			ConfigurationValue,
		UCHAR			InterfaceNumber,
		UCHAR			InterfaceAlternateSetting,
		UCHAR			EndpointAddress,
		KUsbInterface* 	pInterfaceObjectToInitialize,
		KUsbPipe* 		pPipeObjectToInitialize
		);

	UCHAR 			m_ConfigurationValue;
	UCHAR			m_InterfaceNumber;
	UCHAR			m_InterfaceAlternate;
	UCHAR			m_EndpointAddress;
	KUsbInterface*	m_pInterfaceObject;
	KUsbPipe* 		m_pPipeObject;
};

typedef USB_ENDPOINT* PUSB_ENDPOINT;


//////////////////////////////////////////////////////////////////////////////
// In-lines


//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Constructors for USB_ENDPOINT
//

inline USB_ENDPOINT::USB_ENDPOINT(
	UCHAR			ConfigurationValue,
	UCHAR			InterfaceNumber,
	UCHAR			InterfaceAlternateSetting,
	UCHAR			EndpointAddress,
	KUsbInterface* 	pInterfaceObjectToInitialize,
	KUsbPipe* 		pPipeObjectToInitialize
	)
{
	Initialize(
		ConfigurationValue,
		InterfaceNumber,
		InterfaceAlternateSetting,
		EndpointAddress,
		pInterfaceObjectToInitialize,
		pPipeObjectToInitialize
		);
}

inline VOID USB_ENDPOINT::Initialize(
	UCHAR			ConfigurationValue,
	UCHAR			InterfaceNumber,
	UCHAR			InterfaceAlternateSetting,
	UCHAR			EndpointAddress,
	KUsbInterface* 	pInterfaceObjectToInitialize,
	KUsbPipe* 		pPipeObjectToInitialize
	)
{
	m_ConfigurationValue = ConfigurationValue;
	m_InterfaceNumber = InterfaceNumber;
	m_InterfaceAlternate = InterfaceAlternateSetting;
	m_EndpointAddress = EndpointAddress;
	m_pInterfaceObject = pInterfaceObjectToInitialize;
	m_pPipeObject = pPipeObjectToInitialize;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Constructors for KUsbInterface
//

inline KUsbInterface::KUsbInterface(void)
{
	m_Descriptor = NULL;
	m_Information = NULL;
	m_pLowerDevice = NULL;
}

inline KUsbInterface::KUsbInterface(
	KUsbLowerDevice& dev,
	UCHAR InterfaceNumber,
	UCHAR ConfigurationValue,
	UCHAR InitialAlternateSetting,
	NTSTATUS* pStatus
	)
{
	NTSTATUS status = Initialize(
		dev,
		InterfaceNumber,
		ConfigurationValue,
		InitialAlternateSetting
		);

	if( pStatus )
		*pStatus = status;
}

inline NTSTATUS KUsbInterface::Initialize(
	KUsbLowerDevice& dev,
	UCHAR InterfaceNumber,
	UCHAR ConfigurationValue,
	UCHAR InitialAlternateSetting
	)
{
	NTSTATUS status = dev.RegisterInterface(this);

	ASSERT( STATUS_SUCCESS == status );

	if( STATUS_SUCCESS == status )
	{
		m_ConfigurationValue = ConfigurationValue;
		m_InterfaceNumber = InterfaceNumber;
		m_CurrentAlternateSetting = InitialAlternateSetting;
		m_pLowerDevice = &dev;
	}
	else
	{
		m_ConfigurationValue = 0;
		m_InterfaceNumber = 0;
		m_CurrentAlternateSetting = 0;
		m_pLowerDevice = NULL;
	}

	return status;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Constructors for KUsbLowerDevice
//

inline KUsbLowerDevice::KUsbLowerDevice(void) :

	KPnpLowerDevice(),
	m_IntfArray(),
	m_PipeArray()

{
	m_Config = NULL;
	m_ConfiguredInterfaces = NULL;
}

#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
inline KUsbLowerDevice::KUsbLowerDevice(
	KDevice* AttachingDevice,
	PDEVICE_OBJECT PhysicalDeviceObject,
	NTSTATUS* pStatus
	) :

	KPnpLowerDevice(AttachingDevice, PhysicalDeviceObject, pStatus),
	m_IntfArray(0, 3, NonPagedPool),
	m_PipeArray(0, 3, NonPagedPool)

{
	m_Config = NULL;
	m_ConfiguredInterfaces = NULL;
}

inline NTSTATUS KUsbLowerDevice::Initialize(
	KDevice* AttachingDevice,
	PDEVICE_OBJECT PhysicalDeviceObject
	)
{
	m_IntfArray.Initialize(0, 3, NonPagedPool);
	m_PipeArray.Initialize(0, 3, NonPagedPool);
	return KPnpLowerDevice::Initialize(AttachingDevice, PhysicalDeviceObject);
}
#endif // !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)

inline KUsbLowerDevice::KUsbLowerDevice(
	PDEVICE_OBJECT TopOfStackDeviceObject,
	PDEVICE_OBJECT PhysicalDeviceObject,
	NTSTATUS* pStatus
	) :

	KPnpLowerDevice(TopOfStackDeviceObject, PhysicalDeviceObject, pStatus),
	m_IntfArray(0, 3, NonPagedPool),
	m_PipeArray(0, 3, NonPagedPool)

{
	m_Config = NULL;
	m_ConfiguredInterfaces = NULL;
}

inline NTSTATUS KUsbLowerDevice::Initialize(
	PDEVICE_OBJECT TopOfStackDeviceObject,
	PDEVICE_OBJECT PhysicalDeviceObject
	)
{
	m_IntfArray.Initialize(0, 3, NonPagedPool);
	m_PipeArray.Initialize(0, 3, NonPagedPool);
	return KPnpLowerDevice::Initialize(TopOfStackDeviceObject, PhysicalDeviceObject);
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Constructors for class KUsbPipe
//

inline KUsbPipe::KUsbPipe(KUsbInterface& Interface, int Index)
{
	ASSERT ((ULONG)Index < Interface.NumberOfPipes());

	m_Information = Interface.Pipes(Index);
	m_Index = Index;
	m_pInterface = &Interface;
	m_SetMaxTransferSize = 0;

	BOUNDS_CHECKER(CONSTRUCTED_USB_PIPE, (this, &Interface, Index));
}

inline KUsbPipe::KUsbPipe(void)
{
	m_Information = NULL;
	m_Index = -1;
	m_pInterface = NULL;
	m_SetMaxTransferSize = 0;
}

inline VOID KUsbPipe::Initialize(KUsbInterface& Interface, int Index)
{
	ASSERT ((ULONG)Index < Interface.NumberOfPipes());

	m_Information = Interface.Pipes(Index);
	m_Index = Index;
	m_pInterface = &Interface;
	m_SetMaxTransferSize = 0;

	BOUNDS_CHECKER(CONSTRUCTED_USB_PIPE, (this, &Interface, Index));
}

inline KUsbPipe::KUsbPipe(
	KUsbLowerDevice& dev,
	UCHAR ea,
	ULONG MaxTransferSize
	)
{
	Initialize(dev, ea, MaxTransferSize);
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// GetStatus, SetFeature, ClearFeature requests
//

inline NTSTATUS KUsbLowerDevice::GetStatus(
	PUSHORT pDeviceStatus,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context
	)
{
	return _get_status(
		URB_FUNCTION_GET_STATUS_FROM_DEVICE,
		0,
		pDeviceStatus,
		CompletionRoutine,
		Context
		);
}

inline NTSTATUS KUsbInterface::GetStatus(
	PUSHORT pInterfaceStatus,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context
	)
{
	ASSERT(UsbLowerDevice());

	return UsbLowerDevice()->_get_status(
		URB_FUNCTION_GET_STATUS_FROM_INTERFACE,
		m_Descriptor->bInterfaceNumber,
		pInterfaceStatus,
		CompletionRoutine,
		Context
		);
}

inline NTSTATUS KUsbPipe::GetStatus(
	PUSHORT pEndpointStatus,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context
	)
{
	ASSERT(m_pInterface);
	ASSERT(m_pInterface->UsbLowerDevice());

	return m_pInterface->UsbLowerDevice()->_get_status(
		URB_FUNCTION_GET_STATUS_FROM_ENDPOINT,
		m_Information->EndpointAddress,
		pEndpointStatus,
		CompletionRoutine,
		Context
		);
}

inline NTSTATUS KUsbLowerDevice::SetFeature(
	USHORT Feature,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context
	)
{
	return _set_or_clear_feature(
		URB_FUNCTION_SET_FEATURE_TO_DEVICE,
		0,
		Feature,
		CompletionRoutine,
		Context
		);
}

inline NTSTATUS KUsbLowerDevice::ClearFeature(
	USHORT Feature,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context
	)
{
	return _set_or_clear_feature(
		URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE,
		0,
		Feature,
		CompletionRoutine,
		Context
		);
}

inline NTSTATUS KUsbInterface::SetFeature(
	USHORT Feature,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context
	)
{
	ASSERT(UsbLowerDevice());

	return UsbLowerDevice()->_set_or_clear_feature(
		URB_FUNCTION_SET_FEATURE_TO_INTERFACE,
		m_Descriptor->bInterfaceNumber,
		Feature,
		CompletionRoutine,
		Context
		);
}

inline NTSTATUS KUsbInterface::ClearFeature(
	USHORT Feature,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context
	)
{
	ASSERT(UsbLowerDevice());

	return UsbLowerDevice()->_set_or_clear_feature(
		URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE,
		m_Descriptor->bInterfaceNumber,
		Feature,
		CompletionRoutine,
		Context
		);
}

inline NTSTATUS KUsbPipe::SetFeature(
	USHORT Feature,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context
	)
{
	ASSERT(m_pInterface);
	ASSERT(m_pInterface->UsbLowerDevice());

	return m_pInterface->UsbLowerDevice()->_set_or_clear_feature(
		URB_FUNCTION_SET_FEATURE_TO_ENDPOINT,
		m_Information->EndpointAddress,
		Feature,
		CompletionRoutine,
		Context
		);
}

inline NTSTATUS KUsbPipe::ClearFeature(
	USHORT Feature,
	PIO_COMPLETION_ROUTINE CompletionRoutine,
	PVOID Context
	)
{
	ASSERT(m_pInterface);
	ASSERT(m_pInterface->UsbLowerDevice());

	return m_pInterface->UsbLowerDevice()->_set_or_clear_feature(
		URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT,
		m_Information->EndpointAddress,
		Feature,
		CompletionRoutine,
		Context
		);
}


/////////////////////////////////////////////////////////////////////////////////////
// Accessors for KUsbPipe
//

inline USBD_PIPE_TYPE KUsbPipe::Type(void)
{
	ASSERT(m_Information);
	return m_Information->PipeType;
}

inline USHORT KUsbPipe::MaximumPacketSize(void)
{
	ASSERT(m_Information);
	if(!IsOpen()) return 0;
	return m_Information->MaximumPacketSize;
}

inline UCHAR KUsbPipe::EndpointAddress(void)
{
	ASSERT(m_Information);
	if(!IsOpen()) return 0;
	return m_Information->EndpointAddress;
}

inline UCHAR KUsbPipe::PollInterval(void)
{
	ASSERT(m_Information);
	if(!IsOpen()) return 0;
	return m_Information->Interval;
}

inline USBD_PIPE_HANDLE KUsbPipe::Handle(void)
{
	ASSERT(m_Information);
	if(!IsOpen()) return 0;
	return m_Information->PipeHandle;
}

inline ULONG KUsbPipe::MaximumTransferSize(void)
{
	ASSERT(m_Information);
	if(!IsOpen()) return 0;
	return m_Information->MaximumTransferSize;
}


/////////////////////////////////////////////////////////////////////////////////////
// Accessors for KUsbInterface
//

inline UCHAR KUsbInterface::InterfaceNumber(void)
{
	ASSERT(m_Information);
	return m_Information->InterfaceNumber;
}

inline UCHAR KUsbInterface::AlternateSetting(void)
{
	ASSERT (m_Information);
	return m_Information->AlternateSetting;
}

inline UCHAR KUsbInterface::Class(void)
{
	ASSERT (m_Information);
	return m_Information->Class;
}

inline UCHAR KUsbInterface::Subclass(void)
{
	ASSERT (m_Information);
	return m_Information->SubClass;
}

inline UCHAR KUsbInterface::Protocol(void)
{
	ASSERT (m_Information);
	return m_Information->Protocol;
}

inline ULONG KUsbInterface::NumberOfPipes(void)
{
	ASSERT (m_Information);
	return m_Information->NumberOfPipes;
}

inline PUSBD_PIPE_INFORMATION KUsbInterface::Pipes(int Index)
{
	ASSERT (m_Information);
	ASSERT ((unsigned)Index < m_Information->NumberOfPipes);

	return &m_Information->Pipes[Index];
}


///////////////////////////////////////////////////////////////////////////////
// SubmitUrb methods
//

inline NTSTATUS KUsbInterface::SubmitUrb(
	PURB pUrb,
	PIO_COMPLETION_ROUTINE pfnCompletionRoutine,
	PVOID pContext,
	ULONG mSecTimeOut
	)
{
	ASSERT(UsbLowerDevice());

	return UsbLowerDevice()->SubmitUrb(
		pUrb,
		pfnCompletionRoutine,
		pContext,
		mSecTimeOut
		);
}

inline NTSTATUS KUsbInterface::SubmitUrb(
	KIrp I,
	PURB pUrb,
	PIO_COMPLETION_ROUTINE pfnCompletionRoutine,
	PVOID pContext,
	ULONG mSecTimeOut
	)
{
	ASSERT(UsbLowerDevice());

	return UsbLowerDevice()->SubmitUrb(
		I,
		pUrb,
		pfnCompletionRoutine,
		pContext,
		mSecTimeOut
		);
}

inline NTSTATUS KUsbPipe::SubmitUrb(
	PURB pUrb,
	PIO_COMPLETION_ROUTINE pfnCompletionRoutine,
	PVOID pContext,
	ULONG mSecTimeOut
	)
{
	ASSERT(m_pInterface);
	ASSERT(m_pInterface->UsbLowerDevice());

	return m_pInterface->UsbLowerDevice()->SubmitUrb(
		pUrb,
		pfnCompletionRoutine,
		pContext,
		mSecTimeOut
		);
}

inline NTSTATUS KUsbPipe::SubmitUrb(
	KIrp I,
	PURB pUrb,
	PIO_COMPLETION_ROUTINE pfnCompletionRoutine,
	PVOID pContext,
	ULONG mSecTimeOut
	)
{
	ASSERT(m_pInterface);
	ASSERT(m_pInterface->UsbLowerDevice());

	return m_pInterface->UsbLowerDevice()->SubmitUrb(
		I,
		pUrb,
		pfnCompletionRoutine,
		pContext,
		mSecTimeOut
		);
}


///////////////////////////////////////////////////////////////////////////////
// Other KUsbPipe member functions
///////////////////////////////////////////////////////////////////////////////

inline NTSTATUS KUsbPipe::Reset(void)
{
	URB u;

	u.UrbHeader.Function = URB_FUNCTION_RESET_PIPE;
	u.UrbHeader.Length = sizeof( _URB_PIPE_REQUEST );
	u.UrbPipeRequest.PipeHandle = m_Information->PipeHandle;

	NTSTATUS status = SubmitUrb((PURB)&u);
	BOUNDS_CHECKER(RESET_USB_PIPE, (this, status));
	return status;
}

inline NTSTATUS KUsbPipe::Abort(void)
{
	URB u;

	u.UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
	u.UrbHeader.Length = sizeof( _URB_PIPE_REQUEST );
	u.UrbPipeRequest.PipeHandle = m_Information->PipeHandle;

	NTSTATUS status = SubmitUrb((PURB)&u);
	BOUNDS_CHECKER(ABORT_USB_PIPE, (this, status));

	return status;
}

inline NTSTATUS KUsbPipe::Open(
	KUsbInterface& Interface
	)
{
	for (UCHAR i = 0; i < Interface.NumberOfPipes(); i++)
	{
		if( Interface.Pipes(i)->EndpointAddress == m_EndpointAddress )
		{
			m_pInterface = &Interface;
			m_Information = Interface.Pipes(i);
			m_Index = i;
			return STATUS_SUCCESS;
		}
	}

	return STATUS_UNSUCCESSFUL;
}

inline void KUsbPipe::Close(void)
{
	m_pInterface = NULL;
	m_Information = NULL;
	m_Index = 0;
}

inline VOID KUsbPipe::SetMaximumTransferSize(ULONG MaxSize)
{
	m_SetMaxTransferSize = MaxSize;
}


////////////////////////////////////////////////////////////////////////////////
// Build Class and Vendor Requests for KUsbLowerDevice
//

inline PURB KUsbLowerDevice::BuildClassRequest(
	PUCHAR TransferBuffer,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link,
	USHORT Index,
	USHORT Function,
	PURB pUrb
	)
{
	return _build_class_or_vendor_request(
		Function,
		bIn,
		bShortOk,
		TransferBufferLength,
		TransferBuffer,
		NULL,
		RequestTypeReservedBits,
		Request,
		Value,
		Index,
		Link,
		pUrb
		);
}

inline PURB KUsbLowerDevice::BuildClassRequest(
	KMemory& TransferBufferMDL,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link,
	USHORT Index,
	USHORT Function,
	PURB pUrb
	)
{
	return _build_class_or_vendor_request(
		Function,
		bIn,
		bShortOk,
		TransferBufferLength,
		NULL,
		TransferBufferMDL,
		RequestTypeReservedBits,
		Request,
		Value,
		Index,
		Link,
		pUrb
		);
}

inline PURB KUsbLowerDevice::BuildVendorRequest(
	PUCHAR TransferBuffer,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link,
	USHORT Index,
	USHORT Function,
	PURB pUrb
	)
{
	return _build_class_or_vendor_request(
		Function,
		bIn,
		bShortOk,
		TransferBufferLength,
		TransferBuffer,
		NULL,
		RequestTypeReservedBits,
		Request,
		Value,
		Index,
		Link,
		pUrb
		);
}

inline PURB KUsbLowerDevice::BuildVendorRequest(
	KMemory& TransferBufferMDL,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link,
	USHORT Index,
	USHORT Function,
	PURB pUrb
	)
{
	return _build_class_or_vendor_request(
		Function,
		bIn,
		bShortOk,
		TransferBufferLength,
		NULL,
		TransferBufferMDL,
		RequestTypeReservedBits,
		Request,
		Value,
		Index,
		Link,
		pUrb
		);
}


////////////////////////////////////////////////////////////////////////////////
// Build Class and Vendor Requests for KUsbInterface
//
inline PURB KUsbInterface::BuildClassRequest(
	PUCHAR TransferBuffer,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link,
	PURB pUrb
	)
{
	ASSERT(UsbLowerDevice());

	return UsbLowerDevice()->_build_class_or_vendor_request(
		URB_FUNCTION_CLASS_INTERFACE,
		bIn,
		bShortOk,
		TransferBufferLength,
		TransferBuffer,
		NULL,
		RequestTypeReservedBits,
		Request,
		Value,
		m_Information->InterfaceNumber,
		Link,
		pUrb
		);
}

inline PURB KUsbInterface::BuildClassRequest(
	KMemory& TransferBufferMDL,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link,
	PURB pUrb
	)
{
	ASSERT(UsbLowerDevice());

	return UsbLowerDevice()->_build_class_or_vendor_request(
		URB_FUNCTION_CLASS_INTERFACE,
		bIn,
		bShortOk,
		TransferBufferLength,
		NULL,
		TransferBufferMDL,
		RequestTypeReservedBits,
		Request,
		Value,
		m_Information->InterfaceNumber,
		Link,
		pUrb
		);
}

inline PURB KUsbInterface::BuildVendorRequest(
	PUCHAR TransferBuffer,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link,
	PURB pUrb
	)
{
	ASSERT(UsbLowerDevice());

	return UsbLowerDevice()->_build_class_or_vendor_request(
		URB_FUNCTION_VENDOR_INTERFACE,
		bIn,
		bShortOk,
		TransferBufferLength,
		TransferBuffer,
		NULL,
		RequestTypeReservedBits,
		Request,
		Value,
		m_Information->InterfaceNumber,
		Link,
		pUrb
		);
}

inline PURB KUsbInterface::BuildVendorRequest(
	KMemory& TransferBufferMDL,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link,
	PURB pUrb
	)
{
	ASSERT(UsbLowerDevice());

	return UsbLowerDevice()->_build_class_or_vendor_request(
		URB_FUNCTION_VENDOR_INTERFACE,
		bIn,
		bShortOk,
		TransferBufferLength,
		NULL,
		TransferBufferMDL,
		RequestTypeReservedBits,
		Request,
		Value,
		m_Information->InterfaceNumber,
		Link,
		pUrb
		);
}


////////////////////////////////////////////////////////////////////////////////
// Build Class and Vendor Requests for KUsbPipe
//
inline PURB KUsbPipe::BuildClassRequest(
	PUCHAR TransferBuffer,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link
	)
{
	ASSERT(m_pInterface);
	ASSERT(m_pInterface->UsbLowerDevice());

	return m_pInterface->UsbLowerDevice()->_build_class_or_vendor_request(
		URB_FUNCTION_CLASS_ENDPOINT,
		bIn,
		bShortOk,
		TransferBufferLength,
		TransferBuffer,
		NULL,
		RequestTypeReservedBits,
		Request,
		Value,
		static_cast<USHORT>(m_Index),
		Link
		);
}

inline PURB KUsbPipe::BuildClassRequest(
	KMemory& TransferBufferMDL,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link
	)
{
	ASSERT(m_pInterface);
	ASSERT(m_pInterface->UsbLowerDevice());

	return m_pInterface->UsbLowerDevice()->_build_class_or_vendor_request(
		URB_FUNCTION_CLASS_ENDPOINT,
		bIn,
		bShortOk,
		TransferBufferLength,
		NULL,
		TransferBufferMDL,
		RequestTypeReservedBits,
		Request,
		Value,
		static_cast<USHORT>(m_Index),
		Link
		);
}

inline PURB KUsbPipe::BuildVendorRequest(
	PUCHAR TransferBuffer,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link,
	PURB pUrb
	)
{
	ASSERT(m_pInterface);
	ASSERT(m_pInterface->UsbLowerDevice());

	return m_pInterface->UsbLowerDevice()->_build_class_or_vendor_request(
		URB_FUNCTION_VENDOR_ENDPOINT,
		bIn,
		bShortOk,
		TransferBufferLength,
		TransferBuffer,
		NULL,
		RequestTypeReservedBits,
		Request,
		Value,
		static_cast<USHORT>(m_Index),
		Link,
		pUrb
		);
}

inline PURB KUsbPipe::BuildVendorRequest(
	KMemory& TransferBufferMDL,
	ULONG TransferBufferLength,
	UCHAR RequestTypeReservedBits,
	UCHAR Request,
	USHORT Value,
	BOOLEAN bIn,
	BOOLEAN bShortOk,
	PURB Link,
	PURB pUrb
	)
{
	ASSERT(m_pInterface);
	ASSERT(m_pInterface->UsbLowerDevice());

	return m_pInterface->UsbLowerDevice()->_build_class_or_vendor_request(
		URB_FUNCTION_VENDOR_ENDPOINT,
		bIn,
		bShortOk,
		TransferBufferLength,
		NULL,
		TransferBufferMDL,
		RequestTypeReservedBits,
		Request,
		Value,
		static_cast<USHORT>(m_Index),
		Link,
		pUrb
		);
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Methods to Create/Initialize Isochronous Transfer URBs for class KUsbPipe
//

inline PURB KUsbPipe::BuildIsochronousTransfer(
	ULONG NumberOfPackets,
	ULONG PacketSize,
	BOOLEAN bIn,
	BOOLEAN bASAP,
	ULONG StartFrame,
	PVOID Buffer,
	ULONG Length,
	PURB pUrb
	)
{
	PURB p = __build_iso_xfer(
		NumberOfPackets,
		PacketSize,
		bIn,
		bASAP,
		StartFrame,
		Length,
		pUrb
		);

	if (p != NULL)
	{
	    p->UrbIsochronousTransfer.TransferBufferMDL = NULL;
	    p->UrbIsochronousTransfer.TransferBuffer = Buffer;
	}

	return p;
}

inline PURB KUsbPipe::BuildIsochronousTransfer(
	ULONG NumberOfPackets,
	ULONG PacketSize,
	BOOLEAN bIn,
	BOOLEAN bASAP,
	ULONG StartFrame,
	KMemory& Mdl,
	ULONG Length,
	PURB pUrb
	)
{
	PURB p = __build_iso_xfer(
		NumberOfPackets,
		PacketSize,
		bIn,
		bASAP,
		StartFrame,
		Length,
		pUrb
		);

	if (p != NULL)
	{
	    p->UrbIsochronousTransfer.TransferBufferMDL = Mdl;
	    p->UrbIsochronousTransfer.TransferBuffer = NULL;
	}

	return p;
}


///////////////////////////////////////////////////////////////////////////////
// Other KUsbLowerDevice member functions
///////////////////////////////////////////////////////////////////////////////

inline NTSTATUS KUsbLowerDevice::Configure(void)
{
	return Configure(0, NULL, NULL);
}

inline ULONG KUsbLowerDevice::GetCurrentFrameNumber(void)
{
	_URB_GET_CURRENT_FRAME_NUMBER u;

	u.Hdr.Function = URB_FUNCTION_GET_CURRENT_FRAME_NUMBER;
	u.Hdr.Length = sizeof( _URB_GET_CURRENT_FRAME_NUMBER );

	NTSTATUS status = SubmitUrb((PURB)&u);

	if( NT_SUCCESS(status) )
		return u.FrameNumber;
	else
		return 0;
}


inline NTSTATUS KUsbLowerDevice::RegisterInterface(KUsbInterface* intf)
{
	ASSERT( m_IntfArray.IsValid() );
	if(! m_IntfArray.IsValid() )
		return STATUS_UNSUCCESSFUL;

	ULONG index;
	return m_IntfArray.InsertNext(intf, &index);
}


///////////////////////////////////////////////////////////////////////////////
// Other KUsbInterface member functions
///////////////////////////////////////////////////////////////////////////////

inline NTSTATUS KUsbInterface::Open(void)
{
	ASSERT( IsValid() );

	for (int i = 0; i < m_pLowerDevice->m_Config->bNumInterfaces; i++)
		if (m_pLowerDevice->m_ConfiguredInterfaces[i].Interface->InterfaceNumber == m_InterfaceNumber)
		{
			m_Descriptor = m_pLowerDevice->m_ConfiguredInterfaces[i].InterfaceDescriptor;
			m_Information = m_pLowerDevice->m_ConfiguredInterfaces[i].Interface;
			m_ConfigurationHandle = m_pLowerDevice->m_ConfigurationHandle;

			return STATUS_SUCCESS;
		}

	return STATUS_NO_SUCH_DEVICE;
}

inline void KUsbInterface::Close(void)
{
	m_Descriptor = NULL;
	m_Information = NULL;
	m_ConfigurationHandle = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Global helper functions for trace output
///////////////////////////////////////////////////////////////////////////////

#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)

//Global functions to display descriptors
KTrace& operator << (KTrace& t, const USB_DEVICE_DESCRIPTOR& d);
KTrace& operator << (KTrace& t, const USB_CONFIGURATION_DESCRIPTOR& d);
KTrace& operator << (KTrace& t, const USB_INTERFACE_DESCRIPTOR& d);
KTrace& operator << (KTrace& t, const USB_ENDPOINT_DESCRIPTOR& d);
KTrace& operator << (KTrace& t, const USB_STRING_DESCRIPTOR& d);

KTrace& operator << (KTrace& t, const USBD_PIPE_INFORMATION& pipe);
KTrace& operator << (KTrace& t, const USBD_INTERFACE_INFORMATION& intf);

//Global functions to display URBs
KTrace& operator << (KTrace& t, const _URB_HEADER& u);
KTrace& operator << (KTrace& t, const _URB_CONTROL_DESCRIPTOR_REQUEST& u);
KTrace& operator << (KTrace& t, const _URB_CONTROL_GET_INTERFACE_REQUEST& u);
KTrace& operator << (KTrace& t, const _URB_CONTROL_VENDOR_OR_CLASS_REQUEST& u);
KTrace& operator << (KTrace& t, const _URB_ISOCH_TRANSFER& u);
KTrace& operator << (KTrace& t, const _USBD_ISO_PACKET_DESCRIPTOR& d);
KTrace& operator << (KTrace& t, const _URB_SELECT_CONFIGURATION& u);
KTrace& operator << (KTrace& t, const _URB_SELECT_INTERFACE& u);
KTrace& operator << (KTrace& t, const _URB_PIPE_REQUEST& u);
KTrace& operator << (KTrace& t, const _URB_GET_FRAME_LENGTH& u);
KTrace& operator << (KTrace& t, const _URB_SET_FRAME_LENGTH& u);
KTrace& operator << (KTrace& t, const _URB_GET_CURRENT_FRAME_NUMBER& u);
KTrace& operator << (KTrace& t, const _URB_CONTROL_TRANSFER& u);
KTrace& operator << (KTrace& t, const _URB_CONTROL_GET_CONFIGURATION_REQUEST& u);
KTrace& operator << (KTrace& t, const _URB_CONTROL_GET_STATUS_REQUEST& u);
KTrace& operator << (KTrace& t, const _URB_BULK_OR_INTERRUPT_TRANSFER& u);
KTrace& operator << (KTrace& t, const _URB_HCD_AREA& hcd);
KTrace& operator << (KTrace& t, const _URB_CONTROL_FEATURE_REQUEST& u);
KTrace& operator << (KTrace& t, const URB& urb);

#endif // !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
