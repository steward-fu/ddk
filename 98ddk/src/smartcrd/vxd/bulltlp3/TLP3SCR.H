/*++

Copyright (c) 1998 Bull CP8

Module Name:

    TLP3Scr.h

Abstract:

    PC/SC Template Driver

Environment:

    Win 95

--*/
#if !defined( __TLP3SCR_H__ )
#define __TLP3SCR_H__

#define WANTVXDWRAPS

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <basedef.h>
#include <vmm.h>
#include <debug.h>
#include <vcomm.h>
#include <vxdwraps.h>
#include <vwin32.h>
#include <winerror.h>
#include <configmg.h>


#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

#include <smclib.h>




//  ---------------------------------------------------------------------------
//  CONSTANTS
//  ---------------------------------------------------------------------------
#define TLP3_VENDOR_NAME            "Bull"
#define TLP3_PRODUCT_NAME           "SmarTLP3"

#define TLP3SCR_NAME               "TLP3SCR"
#define TLP3SCR_DEVICE_NAME        "TLP3SCR_0"
#define TLP3SCR_MAX_DEVICE      4
#define SMARTCARD_READ          SCARD_CTL_CODE(1000)
#define SMARTCARD_WRITE         SCARD_CTL_CODE(1001)


//
// mode to open the COM
//
#define OPENEXCLUSIVE       1


#define READ_INTERVAL_TIMEOUT_DEFAULT       1000
#define READ_TOTAL_TIMEOUT_CONSTANT_DEFAULT 3000

#define GT_WAITING_TIME_ADJUSTEMENT         2000
#define WAITING_TIME_ADJUSTEMENT            2000
#define MIN_WAITING_TIME                    50

#define READ_INTERVAL_TIMEOUT_ATR           0
#define READ_TOTAL_TIMEOUT_CONSTANT_ATR     50

#define SERIAL_PURGE_RX     1
#define SERIAL_PURGE_TX     0



#define READER_CMD_POWER_DOWN   'O'
#define READER_CMD_COLD_RESET   'C'
#define READER_CMD_WARM_RESET   'W'


//  ---------------------------------------------------------------------------
//  TYPEDEF'S FOR HANDLES
//  ---------------------------------------------------------------------------

typedef ULONG   IRQHANDLE,      *PIRQHANDLE;
typedef ULONG   EVENTHANDLE,    *PEVENTHANDLE;
typedef ULONG   VMHANDLE,       VMHANDLE;

#define INVALID_IRQHANDLE       (( IRQHANDLE )( -1 ))
#define INVALID_EVENTHANDLE     (( EVENTHANDLE )( -1 ))
#define INVALID_VMHANDLE        (( VMHANDLE )( -1 ))

#ifndef HPORT
#define HPORT   DWORD
#endif

//  ---------------------------------------------------------------------------
//  DRIVER DATA DEFINITIONS
//  ---------------------------------------------------------------------------

//
// Define the reader specific portion of the smart card extension
//
typedef struct _SERIAL_READER_CONFIG {

    _DCB    theDCB;
    //
    // read/write timeouts
    //
    COMMTIMEOUTS    CommTimeouts;

    char qreceive[MIN_BUFFER_SIZE];            // port receive queue
    char qtransmit[MIN_BUFFER_SIZE];          // port transmit queue

    //
    // Event serial reader uses to signal insert/removal
    //
    ULONG WaitMask;

} SERIAL_READER_CONFIG, *PSERIAL_READER_CONFIG;


typedef struct _READER_EXTENSION {

    BOOLEAN GetModemStatus;
    ULONG   ModemStatus;


    //
    // COMM PORT Number where the reader is connected
    //
    char pcCommNumber[6];


    ULONG   SerialIoControlCode;

    //
    // Flag that indicates that the caller requests a power-down or a reset
    //
    BOOLEAN PowerRequest;


    //
    // buffer for the link to the registry
    //
    UCHAR LinkName[64];

    //
    // serial port Handle
    //
    HPORT hCommPort;

    //
    // Serial port configuration
    //
    SERIAL_READER_CONFIG SerialConfigData;

    //
    // semaphore for timer delays and data events
    //
    VMM_SEMAPHORE hDelaySemaphore;


} READER_EXTENSION, *PREADER_EXTENSION;

typedef struct _DEVICE_DATA
{
    //
    // Pointer to device descripton block
    //
    PVMMDDB                 DDB;

    //
    // Flag that shows if this reader has been opened
    //
    BOOLEAN                 OpenFlag;

    PSMARTCARD_EXTENSION     PSmartcardExtension;

    DEVNODE                 DeviceNode;
    //char                    LinkName[ 64 ];
    //READER_EXTENSION        ReaderExtension;
} DEVICE_DATA, *PDEVICE_DATA;

//  ---------------------------------------------------------------------------
//  IOCTL DEFINITIONS
//  ---------------------------------------------------------------------------

#define IOCTL_OPEN              0
#define IOCTL_CLOSE             (~IOCTL_OPEN)
//#define IOCTL_TLP3SCR_COMMAND   SCARD_CTL_CODE( 0x8000 )
//#define IOCTL_GET_VERSIONS      SCARD_CTL_CODE( 0x8001 )
//#define IOCTL_SET_TIMEOUT       SCARD_CTL_CODE( 0x8002 )

//typedef struct _VERSION_CONTROL
//{
//    ULONG   SmclibVersion;
//    UCHAR   DriverMajor,
//            DriverMinor,
//            FirmwareMajor,
//            FirmwareMinor;
//} VERSION_CONTROL, *PVERSION_CONTROL;

//#define SIZEOF_VERSION_CONTROL  sizeof( VERSION_CONTROL )

//  ---------------------------------------------------------------------------
//  FUNCTION PROTOTYPES
//  ---------------------------------------------------------------------------

CONFIGRET
TLP3Configure(
    CONFIGFUNC          ConfigFct,
    SUBCONFIGFUNC       SubConfigFct,
    DEVNODE             DeviceNode,
    DWORD               ReferenceData,
    ULONG               Flags
    );

NTSTATUS
TLP3StartDevice(
    DEVNODE DevNode
    );

NTSTATUS
TLP3InitSingleDevice(
    UINT DeviceNo,
    PUCHAR DeviceName,
    PVMMDDB DeviceDDB
    );

NTSTATUS
TLP3RetriveComNbFromRegistry(
    PSMARTCARD_EXTENSION SmartcardExtension,
    DEVNODE DevNode
    );

NTSTATUS
TLP3GetComPort(
    PSMARTCARD_EXTENSION SmartcardExtension,
    DEVNODE DevNode
    );

VOID
_stdcall
TLP3SetupSmartcardExtension(
    UINT DeviceNo
    );

BOOL
_cdecl
TLP3InitCommPort(
    PSMARTCARD_EXTENSION SmartcardExtension
    );

NTSTATUS
TLP3StartSerialEventTracking(
    UINT DeviceNo
    );

void
_cdecl
TLP3SerialEvent(
    HPORT hPort,
    UINT DeviceNo,
    DWORD lEvent,
    DWORD lSubEvent
    );

BOOL
_cdecl
TLP3ConfigureSerialPort(
    PSMARTCARD_EXTENSION SmartcardExtension
    );


NTSTATUS
TLP3CardTracking(
    PSMARTCARD_EXTENSION SmartcardExtension
    );

NTSTATUS
TLP3ReaderPower(
    PSMARTCARD_EXTENSION SmartcardExtension
    );


NTSTATUS
TLP3SerialIo(
    PSMARTCARD_EXTENSION SmartcardExtension
    );

NTSTATUS
TLP3TransmitT0(
    PSMARTCARD_EXTENSION SmartcardExtension
    );

NTSTATUS
TLP3Transmit(
    PSMARTCARD_EXTENSION SmartcardExtension
    );

NTSTATUS
TLP3VendorIoctl(
    PSMARTCARD_EXTENSION SmartcardExtension
    );

NTSTATUS
TLP3SetProtocol(
    PSMARTCARD_EXTENSION SmartcardExtension
    );

VOID
_stdcall
TLP3StopSerialEventTracking(
    ULONG DeviceNo
);

NTSTATUS
TLP3StopDevice(
    DEVNODE DevNode
    );

VOID
_stdcall
TLP3DeleteDevice(
    ULONG DeviceNo
    );

BOOL
TLP3Delay(
    PSMARTCARD_EXTENSION pSmartcardExtension,
    ULONG ulMillisecTimeout
    );

    void
_cdecl
TLP3_SignalSemaphore (
    void
    );



//
//  OS interface
//

#define NT_SUCCESS( Stat )  (( Stat ) == STATUS_SUCCESS )

//
//  typedef's for handler & completion functions
//
/*
typedef
void (__stdcall *EVENT_HANDLER )( void );

typedef
BOOL (__stdcall *EVENT_COMPLETION )( PVOID pReferenceData );

typedef
void ( __stdcall *IRQ_HANDLER )( void );

typedef
BOOL ( __stdcall *IRQ_COMPLETION )( IRQHANDLE IRQHandle, VMHANDLE VMHandle );

//
//  prototypes for wrapper functions ( module TLP3ScrWrap.asm )
//
void
__stdcall
VPICD_PhysEOI( IRQHANDLE hIRQ );

void
__stdcall
VWIN32_DIOCCompletionRoutine( EVENTHANDLE hEvent );
*/
ULONG
__stdcall
SysCompareMemory( PUCHAR pData1, PUCHAR pData2, ULONG Len );

ULONG
__stdcall
SysCopyMemory( PUCHAR pDest, PUCHAR pSrc, ULONG Len );

ULONG
__stdcall
SysFillMemory( PUCHAR pDest, UCHAR Value, ULONG Len );

void
__stdcall
SysDelay( ULONG Timeout );

/*
void
__stdcall
OutportByte( USHORT Port, UCHAR Data );

UCHAR
__stdcall
InportByte( USHORT Port );

#define DELAY_WRITE_TLP3SCR_REG    1
#define DELAY_TLP3SCR_WAIT         10

#define WRITE_PORT_UCHAR( Port, Data )  OutportByte( (USHORT) Port, (UCHAR) Data )
#define READ_PORT_UCHAR( Port )         InportByte( (USHORT) Port )

*/
//
//  Prototype for the driver control procedure ( module TLP3Strt.asm )
//
void
TLP3ControlProcedure( void );


#endif      // __TLP3SCR_H__


