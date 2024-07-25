// ioschdev.h - include file for device class of USB Firmware Loading Driver
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#ifndef _ISOCHDEV_
#define _ISOCHDEV_

class UsbIsocDevice;

#define ISOC_NUM_READ_URBS			4
#define ISOC_NUM_PACKETS_PER_URB    32
#define ISOC_PACKET_SIZE            64
#define ISOC_URB_DATA_SIZE          (ISOC_NUM_PACKETS_PER_URB * ISOC_PACKET_SIZE)
#define ISOC_READ_FIFO_SIZE         (ISOC_URB_DATA_SIZE * 8)

typedef struct _ISOC_CONTEXT
{
    _ISOC_CONTEXT *pNext;

    BOOLEAN        bUsed;

    UsbIsocDevice *pDev;

	PIRP           pIrp;
    PURB           pUrb;
    PVOID          pData;
	KMemory       *pMdl;

} ISOC_CONTEXT, *PISOC_CONTEXT;


// NOTE: We create our own macro here, since we
//       want our completion routines to use our
//       self-defined ISOC_CONTEXT structure.
//
#define MY_MEMBER_COMPLETEIRP(classname, func)\
    NTSTATUS func(KIrp I, PISOC_CONTEXT pContext);\
    static NTSTATUS LinkTo(func)(              \
        PDEVICE_OBJECT pDevObj,                \
        PIRP pIrp,                             \
        PVOID context)                         \
    {                                          \
        PISOC_CONTEXT pContext = (PISOC_CONTEXT)context;\
        UsbIsocDevice *p = pContext ? pContext->pDev : NULL;\
        NTSTATUS status;                       \
        if (!pContext || !p ) {                \
            return STATUS_UNSUCCESSFUL;        \
        }                                      \
        KIrp I(pIrp);                          \
        status=p->func(I,pContext);            \
        return status;                         \
    }



class UsbIsocDevice : public KPnpDevice
{
    SAFE_DESTRUCTORS
public:
    UsbIsocDevice(PDEVICE_OBJECT Pdo, ULONG unit);
    ~UsbIsocDevice();

    DEVMEMBER_DISPATCHERS

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS InternalDeviceControl(KIrp I);	// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);			  		// COMMENT_ONLY
	virtual NTSTATUS Write(KIrp I);			  		// COMMENT_ONLY
#endif

    virtual NTSTATUS OnStartDevice(KIrp I);
    virtual NTSTATUS OnStopDevice(KIrp I);
    virtual NTSTATUS OnRemoveDevice(KIrp I);
    virtual NTSTATUS DefaultPnp(KIrp I);
    virtual NTSTATUS DefaultPower(KIrp I);

	// Member functions and variables needed for reading
	NTSTATUS StartIsocReads();
	VOID     DeleteAllReadContexts();
	NTSTATUS QueueIsocRead(PISOC_CONTEXT pContext);

	MY_MEMBER_COMPLETEIRP(UsbIsocDevice, UsbIsocReadComplete);

	// Member function needed for writing
	MY_MEMBER_COMPLETEIRP(UsbIsocDevice, UsbIsocWriteComplete);

#ifdef __COMMENT_ONLY
	NTSTATUS UsbIsocReadComplete(KIrp I, PISOC_CONTEXT pContext);
	NTSTATUS UsbIsocWriteComplete(KIrp I, PISOC_CONTEXT pContext);
#endif

    KLockableFifo<UCHAR>* m_pReadFifo;
    PISOC_CONTEXT   m_pReadContexts;
    ULONG           m_readOverflow;


    ULONG           m_unitNum;      // Unit number for this device (0-9)

    KUsbLowerDevice m_Usb;

    KUsbInterface   m_Interface;

    KUsbPipe        m_Pipe0;        // Bulk Data In  Pipe
    KUsbPipe        m_Pipe1;        // Bulk Data Out Pipe

    USB_ENDPOINT    m_Endpoints[2]; // endpoint descriptions for
                                    // ActivateConfiguration, initialized
                                    // by ctor
};

#endif
