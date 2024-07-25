// EzUsbDevice.h
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1999 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#ifndef __EzUsbDevice_h__
#define __EzUsbDevice_h__

#include "usb.h"

//This device supports the maxiumum number of endpoints allowed
//which is 32. However the default control endpoints are abstracted
//by KUsbLowerDevice instance, so there will be a maximum of 30
//KUsbPipe objects.
#define EZUSB_MAX_ENDPOINTS 30

//Forward Declaration
class EzUsbDevice;

typedef struct _EZUSB_ISOCH_COMPLETION_INFO
{
	EzUsbDevice* m_pClass;
	PIRP m_Irp;
	EzUsbIsoTransfer* m_pKUsbDM;
	PUCHAR m_DescrBuff;
	ULONG m_NumDescr;

} EZUSB_ISOCH_COMPLETION_INFO, *PEZUSB_ISOCH_COMPLETION_INFO;

//=============================================================================
//	class EzUsbDevice
//
class EzUsbDevice : public KPnpDevice
{
// Constructors/Destructors
public:
	SAFE_DESTRUCTORS;
	EzUsbDevice(PDEVICE_OBJECT Pdo, ULONG Unit);
	~EzUsbDevice();
private:
	EzUsbDevice(EzUsbDevice&);
	EzUsbDevice& operator=(EzUsbDevice&);

// Member Functions
public:
	DEVMEMBER_DISPATCHERS
	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);
	virtual NTSTATUS OnDevicePowerUp(KIrp I);
	virtual NTSTATUS OnDeviceSleep(KIrp I);
	virtual	NTSTATUS OnQueryCapabilities(KIrp I);
	MEMBER_COMPLETEIRP(EzUsbDevice, OnQueryCapabilitiesComplete)
protected:
	NTSTATUS DownloadFirmware(void);
	NTSTATUS DownloadRAM(
		PINTEL_HEX_RECORD phexRecord, 
		bool fExternal
		);
	NTSTATUS DownloadIntelHex(PINTEL_HEX_RECORD ptr);
	NTSTATUS AnchorDownload(
	   USHORT wOffset,
	   PUCHAR pDownloadBuffer,
	   ULONG nDownloadSize,
	   ULONG nTransferLength,
	   ULONG nTransferCount
	   );
	NTSTATUS Reset8051(UCHAR byResetBit);
	NTSTATUS VendorRequest(
		PVENDOR_REQUEST_IN pVendorRequest, 
		ULONG_PTR& nLength
		);
	NTSTATUS VendorRequest(KIrp I);
	NTSTATUS BulkReadWrite(KIrp I);
	KUsbPipe* FindPipe(ULONG nPipeNum);
	NTSTATUS IsoReadWrite(KIrp I);
	NTSTATUS StartIsoStream(KIrp I);
	NTSTATUS ReadIsoBuffer(KIrp I);
	NTSTATUS CheckIsoParameters(PISO_TRANSFER_CONTROL p);
	static ON_INTERRUPT_STATUS OnInterrupt(
			PURB pUrb,
			PVOID pContext
			);
	static VOID __stdcall IsoStreamCompleteCallback(PVOID pContext);
	bool IsFx2();
	void TestBusInterface();

// Member Data
protected:
	// Unit number for this device 
	ULONG m_Unit;
	// Configuration
	KUsbLowerDevice	m_Lower;
	// Interface
	KUsbInterface m_UsbInterface;

	// Pipes for EZUSB Device
	//NOTE: most drivers would not have an array of KUsbPipe objects
	//but rather an instance for each endpoint.  However, in this kit
	//the application has the knowledge of which endpoint to use for
	//an operation.  The driver chooses a KUsbPipe object from the
	//array at run time.
	KUsbPipe m_Endpoints[EZUSB_MAX_ENDPOINTS];

	//Most drivers would have KUsbPipe data members for each endpoint
	//KUsbPipe m_Endpoint1_OUT;
	//KUsbPipe m_Endpoint2_IN;
	//KUsbPipe m_Endpoint2_OUT;

	EzUsbDriverManagedIsoTransfer* m_pIsoDM;

	USBD_STATUS m_LastFailedUrbStatus;

//used for interrupt transfers
	KUsbInterruptTransfer m_IntXfer;
	PUCHAR pIntBuff;
	ULONG IntBuffLen;

#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20))))
	KUsbBusInterface	m_BusIntf;
	BOOLEAN				m_fBusIntfAvailable;
#endif

#ifdef _COMMENT_ONLY

		// The following member functions are actually defined by the
		// a DEVMEMBER_xxx or MEMBER_xxx macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// Developer Studio using the class browser.

	virtual NTSTATUS Create(KIrp I);				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS InternalDeviceControl(KIrp I);	// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
#endif
};


#endif
