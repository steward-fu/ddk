/*++
    Copyright (c) 2000,2002 Microsoft Corporation

    Module Name:
        wacompen.c

    Abstract:
        Contains wacom specific functions.

    Environment:
        Kernel mode

    Author:
        Michael Tsang (MikeTs) 13-Mar-2000

    Revision History:
--*/

#include "pch.h"
#define MODULE_ID                       5

#define BYTEINDEX_XMAX                  61
#define BYTEINDEX_YMAX                  71
#define BYTEINDEX_PRESSUREMAX           81
#define BYTEINDEX_XTILTMIN              90
#define BYTEINDEX_XTILTMAX              92
#define BYTEINDEX_YTILTMIN              98
#define BYTEINDEX_YTILTMAX              100

UCHAR gReportDescriptor[] = {
    0x05, 0x0d,                         // USAGE_PAGE (Digitizers)          0
    0x09, 0x02,                         // USAGE (Pen)                      2
    0xa1, 0x01,                         // COLLECTION (Application)         4
    0x85, REPORTID_PEN,                 //   REPORT_ID (Pen)                6
    0x09, 0x20,                         //   USAGE (Stylus)                 8
    0xa1, 0x00,                         //   COLLECTION (Physical)          10
    0x09, 0x42,                         //     USAGE (Tip Switch)           12
    0x09, 0x44,                         //     USAGE (Barrel Switch)        14
    0x09, 0x3c,                         //     USAGE (Invert)               16
    0x09, 0x45,                         //     USAGE (Eraser Switch)        18
    0x15, 0x00,                         //     LOGICAL_MINIMUM (0)          20
    0x25, 0x01,                         //     LOGICAL_MAXIMUM (1)          22
    0x75, 0x01,                         //     REPORT_SIZE (1)              24
    0x95, 0x04,                         //     REPORT_COUNT (4)             26
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         28
    0x95, 0x01,                         //     REPORT_COUNT (1)             30
    0x81, 0x03,                         //     INPUT (Cnst,Var,Abs)         32
    0x09, 0x32,                         //     USAGE (In Range)             34
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         36
    0x95, 0x02,                         //     REPORT_COUNT (2)             38
    0x81, 0x03,                         //     INPUT (Cnst,Var,Abs)         40
    0x05, 0x01,                         //     USAGE_PAGE (Generic Desktop) 42
    0x09, 0x30,                         //     USAGE (X)                    44
    0x75, 0x10,                         //     REPORT_SIZE (16)             46
    0x95, 0x01,                         //     REPORT_COUNT (1)             48
    0xa4,                               //     PUSH                         50
    0x55, 0x0d,                         //     UNIT_EXPONENT (-3)           51
    0x65, 0x33,                         //     UNIT (Inch,EngLinear)        53
    0x35, 0x00,                         //     PHYSICAL_MINIMUM (0)         55
    0x46, 0x3a, 0x20,                   //     PHYSICAL_MAXIMUM (8250)      57
    0x26, 0xf8, 0x52,                   //     LOGICAL_MAXIMUM (21240)      60
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         63
    0x09, 0x31,                         //     USAGE (Y)                    65
    0x46, 0x2c, 0x18,                   //     PHYSICAL_MAXIMUM (6188)      67
    0x26, 0x6c, 0x3e,                   //     LOGICAL_MAXIMUM (15980)      70
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         73
    0xb4,                               //     POP                          75
    0x05, 0x0d,                         //     USAGE_PAGE (Digitizers)      76
    0x09, 0x30,                         //     USAGE (Tip Pressure)         78
    0x26, 0xff, 0x00,                   //     LOGICAL_MAXIMUM (255)        80
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         83
    0x75, 0x08,                         //     REPORT_SIZE (8)              85
    0x09, 0x3d,                         //     USAGE (X Tilt)               87
    0x15, 0x81,                         //     LOGICAL_MINIMUM (-127)       89
    0x25, 0x7f,                         //     LOGICAL_MAXIMUM (127)        91
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         93
    0x09, 0x3e,                         //     USAGE (Y Tilt)               95
    0x15, 0x81,                         //     LOGICAL_MINIMUM (-127)       97
    0x25, 0x7f,                         //     LOGICAL_MAXIMUM (127)        99
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         101/103
#ifdef _TIMESTAMP_
    0x06, 0x00, 0xff,                   //     USAGE_PAGE (Vendor Defined)  0
    0x09, 0x80,                         //     USAGE (Vendor Usage 0x80)    3
    0x09, 0x81,                         //     USAGE (Vendor Usage 0x81)    5
    0x15, 0x00,                         //     LOGICAL_MINIMUM (0)          7
    0x27, 0xff, 0xff, 0xff, 0xff,       //     LOGICAL_MAXIMUM (0xffffffff) 9
    0x75, 0x20,                         //     REPORT_SIZE (32)             14
    0x95, 0x02,                         //     REPORT_COUNT (2)             16
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         18/20
#endif
    0xc0,                               //   END_COLLECTION                 0
    0xc0,                               // END_COLLECTION                   1/2
    //
    // Dummy mouse collection starts here
    //
    0x05, 0x01,                         // USAGE_PAGE (Generic Desktop)     0
    0x09, 0x02,                         // USAGE (Mouse)                    2
    0xa1, 0x01,                         // COLLECTION (Application)         4
    0x85, REPORTID_MOUSE,               //   REPORT_ID (Mouse)              6
    0x09, 0x01,                         //   USAGE (Pointer)                8
    0xa1, 0x00,                         //   COLLECTION (Physical)          10
    0x05, 0x09,                         //     USAGE_PAGE (Button)          12
    0x19, 0x01,                         //     USAGE_MINIMUM (Button 1)     14
    0x29, 0x02,                         //     USAGE_MAXIMUM (Button 2)     16
    0x15, 0x00,                         //     LOGICAL_MINIMUM (0)          18
    0x25, 0x01,                         //     LOGICAL_MAXIMUM (1)          20
    0x75, 0x01,                         //     REPORT_SIZE (1)              22
    0x95, 0x02,                         //     REPORT_COUNT (2)             24
    0x81, 0x02,                         //     INPUT (Data,Var,Abs)         26
    0x95, 0x06,                         //     REPORT_COUNT (6)             28
    0x81, 0x03,                         //     INPUT (Cnst,Var,Abs)         30
    0x05, 0x01,                         //     USAGE_PAGE (Generic Desktop) 32
    0x09, 0x30,                         //     USAGE (X)                    34
    0x09, 0x31,                         //     USAGE (Y)                    36
    0x15, 0x81,                         //     LOGICAL_MINIMUM (-127)       38
    0x25, 0x7f,                         //     LOGICAL_MAXIMUM (127)        40
    0x75, 0x08,                         //     REPORT_SIZE (8)              42
    0x95, 0x02,                         //     REPORT_COUNT (2)             44
    0x81, 0x06,                         //     INPUT (Data,Var,Rel)         46
    0xc0,                               //   END_COLLECTION                 48
    0xc0                                // END_COLLECTION                   49/50
};
ULONG gdwcbReportDescriptor = sizeof(gReportDescriptor);

HID_DESCRIPTOR gHidDescriptor =
{
    sizeof(HID_DESCRIPTOR),             //bLength
    HID_HID_DESCRIPTOR_TYPE,            //bDescriptorType
    HID_REVISION,                       //bcdHID
    0,                                  //bCountry - not localized
    1,                                  //bNumDescriptors
    {                                   //DescriptorList[0]
        HID_REPORT_DESCRIPTOR_TYPE,     //bReportType
        sizeof(gReportDescriptor)       //wReportLength
    }
};

PWSTR gpwstrManufacturerID = L"Wacom";
PWSTR gpwstrProductID = L"ISD V4";
PWSTR gpwstrSerialNumber = L"0";

//
// Local function prototypes.
//
NTSTATUS INTERNAL
QueryDeviceInfo(
    IN PDEVICE_EXTENSION DevExt
    );

#ifdef ALLOC_PRAGMA
  #pragma alloc_text(PAGE, OemInitDevice)
  #pragma alloc_text(PAGE, OemWakeupDevice)
  #pragma alloc_text(PAGE, OemStandbyDevice)
  #pragma alloc_text(PAGE, OemInitSerialPort)
  #pragma alloc_text(PAGE, QueryDeviceInfo)
#endif  //ifdef ALLOC_PRAGMA

/*++
    @doc    INTERNAL

    @func   NTSTATUS | OemInitDevice | Initialize pen tablet device.

    @parm   IN PDEVICE_EXTENSION | devext | Points to the device extension.

    @rvalue SUCCESS | returns STATUS_SUCCESS
    @rvalue FAILURE | returns NT status code
--*/

NTSTATUS INTERNAL
OemInitDevice(
     IN PDEVICE_EXTENSION devext
    )
{
    TRACEPROC("OemInitDevice", 2)
    NTSTATUS status;

    PAGED_CODE();
    TRACEENTER(("(devext=%p)\n", devext));

    status = QueryDeviceInfo(devext);
    if (!NT_SUCCESS(status))
    {
        LogError(ERRLOG_INIT_DIGITIZER_FAILED,
                 status,
                 UNIQUE_ERRID(0x10),
                 NULL,
                 NULL);
        TRACEERROR(("failed to query digitizer info (status=%x).\n",
                    status));
    }
    else
    {
        HANDLE hkey;

        status = IoOpenDeviceRegistryKey(devext->pdo,
                                         PLUGPLAY_REGKEY_DEVICE,
                                         STANDARD_RIGHTS_READ,
                                         &hkey);
        if (NT_SUCCESS(status))
        {
            PRESSURE PressureThresholds;

            status = RegQueryValue(hkey,
                                   L"PressureThresholds",
                                   REG_BINARY,
                                   (PUCHAR)&PressureThresholds,
                                   sizeof(PressureThresholds),
                                   NULL);
            if (NT_SUCCESS(status))
            {
                if ((PressureThresholds.wTip <=
                     devext->OemData.wPressureMax) &&
                    (PressureThresholds.wEraser <=
                     devext->OemData.wPressureMax))
                {
                    TRACEINFO(1, ("RegData: TipThreshold=%d, EraserThreshold=%d\n",
                                  PressureThresholds.wTip,
                                  PressureThresholds.wEraser));
                    devext->OemData.PressureThresholds = PressureThresholds;
                }
                else
                {
                    TRACEWARN(("Invalid pressure thresholds (Tip=%d,Eraser=%d)\n",
                               PressureThresholds.wTip,
                               PressureThresholds.wEraser));
                }

                if (devext->OemData.PressureThresholds.wTip <
                    MIN_PRESSURE_THRESHOLD)
                {
                    devext->OemData.PressureThresholds.wTip =
                        MIN_PRESSURE_THRESHOLD;
                }

                if (devext->OemData.PressureThresholds.wEraser <
                    MIN_PRESSURE_THRESHOLD)
                {
                    devext->OemData.PressureThresholds.wEraser =
                        MIN_PRESSURE_THRESHOLD;
                }
            }
            else
            {
                //
                // Pressure threshold is optional, it's OK if we don't find
                // it.  We will use default.
                //
                TRACEWARN(("Failed to read pressure thresholds (status=%x).\n",
                           status));
            }
            ZwClose(hkey);
        }
        else
        {
            //
            // It's OK to fail.  We will use default values.
            //
            TRACEWARN(("Failed to open device registry (status=%x).\n",
                       status));
        }

        status = SerialSyncReadWritePort(FALSE,
                                         devext,
                                         "1",
                                         1,
                                         NULL,
                                         NULL);
        if (!NT_SUCCESS(status))
        {
            LogError(ERRLOG_INIT_DIGITIZER_FAILED,
                     status,
                     UNIQUE_ERRID(0x20),
                     NULL,
                     NULL);
            TRACEERROR(("failed to initialize digitizer (status=%x).\n",
                        status));
        }
    }

    TRACEEXIT(("=%x\n", status));
    return status;
}       //OemInitDevice

/*++
    @doc    INTERNAL

    @func   NTSTATUS | OemWakeupDevice | OEM specific wake up code.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.

    @rvalue SUCCESS | returns STATUS_SUCCESS
    @rvalue FAILURE | returns NT status code
--*/

NTSTATUS INTERNAL
OemWakeupDevice(
    IN PDEVICE_EXTENSION DevExt
    )
{
    TRACEPROC("OemWakeupDevice", 2)
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();
    TRACEENTER(("(DevExt=%p)\n", DevExt));

    if (DevExt->dwfHPen & HPENF_DEVICE_STARTED)
    {
        status = SerialAsyncReadWritePort(FALSE, DevExt, "+", 1);
        if (!NT_SUCCESS(status))
        {
            LogError(ERRLOG_WAKE_DIGITIZER_FAILED,
                     status,
                     UNIQUE_ERRID(0x30),
                     NULL,
                     NULL);
            TRACEWARN(("failed to wake the digitizer (status=%x).\n", status));
        }
    }

    TRACEEXIT(("=%x\n", status));
    return status;
}       //OemWakeupDevice

/*++
    @doc    INTERNAL

    @func   NTSTATUS | OemStandbyDevice | OEM specific wake up code.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.

    @rvalue SUCCESS | returns STATUS_SUCCESS
    @rvalue FAILURE | returns NT status code
--*/

NTSTATUS INTERNAL
OemStandbyDevice(
    IN PDEVICE_EXTENSION DevExt
    )
{
    TRACEPROC("OemStandbyDevice", 2)
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();
    TRACEENTER(("(DevExt=%p)\n", DevExt));

    if (DevExt->dwfHPen & HPENF_DEVICE_STARTED)
    {
        status = SerialAsyncReadWritePort(FALSE, DevExt, "-", 1);
        if (!NT_SUCCESS(status))
        {
            LogError(ERRLOG_SLEEP_DIGITIZER_FAILED,
                     status,
                     UNIQUE_ERRID(0x40),
                     NULL,
                     NULL);
            TRACEWARN(("failed to send StandBy command (status=%x).\n",
                       status));
        }
    }

    TRACEEXIT(("=%x\n", status));
    return status;
}       //OemStandbyDevice

/*++
    @doc    INTERNAL

    @func   NTSTATUS | OemInitSerialPort | Initialize com port for
            communication.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.

    @rvalue SUCCESS | returns STATUS_SUCCESS
    @rvalue FAILURE | returns NT status code
--*/

NTSTATUS INTERNAL
OemInitSerialPort(
    IN PDEVICE_EXTENSION DevExt
    )
{
    TRACEPROC("OemInitSerialPort", 2)
    NTSTATUS status;
    IO_STATUS_BLOCK iosb;

    PAGED_CODE();
    TRACEENTER(("(DevExt=%p)\n", DevExt));

    //
    // Set the com port to basic operating mode: reads/writes one byte at
    // time, no handshake flow control or timeouts.
    //
    status = SerialSyncSendIoctl(IOCTL_SERIAL_INTERNAL_BASIC_SETTINGS,
                                 DevExt->LowerDevObj,
                                 NULL,
                                 0,
                                 &DevExt->OemData.PrevSerialSettings,
                                 sizeof(DevExt->OemData.PrevSerialSettings),
                                 TRUE,
                                 &iosb);
    if (!NT_SUCCESS(status))
    {
        LogError(ERRLOG_INIT_SERIAL_FAILED,
                 status,
                 UNIQUE_ERRID(0x50),
                 NULL,
                 NULL);
        TRACEERROR(("failed to set basic settings of COM (status=%x).\n",
                    status));
    }
    else
    {
        SERIAL_BAUD_RATE sbr;

        DevExt->dwfHPen |= HPENF_SERIAL_INITED;
        sbr.BaudRate = OEM_SERIAL_BAUDRATE;
        status = SerialSyncSendIoctl(IOCTL_SERIAL_SET_BAUD_RATE,
                                     DevExt->LowerDevObj,
                                     &sbr,
                                     sizeof(sbr),
                                     NULL,
                                     0,
                                     FALSE,
                                     &iosb);
        if (!NT_SUCCESS(status))
        {
            LogError(ERRLOG_INIT_SERIAL_FAILED,
                     status,
                     UNIQUE_ERRID(0x60),
                     NULL,
                     NULL);
            TRACEERROR(("failed to set COM baudrate (status=%x).\n", status));
        }
        else
        {
            SERIAL_LINE_CONTROL slc;

            slc.WordLength = OEM_SERIAL_WORDLEN;
            slc.Parity = OEM_SERIAL_PARITY;
            slc.StopBits = OEM_SERIAL_STOPBITS;
            status = SerialSyncSendIoctl(IOCTL_SERIAL_SET_LINE_CONTROL,
                                         DevExt->LowerDevObj,
                                         &slc,
                                         sizeof(slc),
                                         NULL,
                                         0,
                                         FALSE,
                                         &iosb);
            if (!NT_SUCCESS(status))
            {
                LogError(ERRLOG_INIT_SERIAL_FAILED,
                         status,
                         UNIQUE_ERRID(0x70),
                         NULL,
                         NULL);
                TRACEERROR(("failed to set COM LineControl (status=%x).\n",
                            status));
            }
            else
            {
                SERIAL_HANDFLOW HandFlow;

                status = SerialSyncSendIoctl(IOCTL_SERIAL_GET_HANDFLOW,
                                             DevExt->LowerDevObj,
                                             NULL,
                                             0,
                                             &HandFlow,
                                             sizeof(HandFlow),
                                             FALSE,
                                             &iosb);
                if (!NT_SUCCESS(status))
                {
                    LogError(ERRLOG_INIT_SERIAL_FAILED,
                             status,
                             UNIQUE_ERRID(0x80),
                             NULL,
                             NULL);
                    TRACEERROR(("failed to get HandFlow (status=%x).\n",
                                status));
                }
                else
                {
                    HandFlow.ControlHandShake &= ~SERIAL_DTR_MASK;
                    HandFlow.ControlHandShake |= SERIAL_DTR_CONTROL;
                    HandFlow.FlowReplace &= ~SERIAL_RTS_MASK;
                    HandFlow.FlowReplace |= SERIAL_RTS_CONTROL;
                    status = SerialSyncSendIoctl(IOCTL_SERIAL_SET_HANDFLOW,
                                                 DevExt->LowerDevObj,
                                                 &HandFlow,
                                                 sizeof(HandFlow),
                                                 NULL,
                                                 0,
                                                 FALSE,
                                                 &iosb);
                    if (!NT_SUCCESS(status))
                    {
                        LogError(ERRLOG_INIT_SERIAL_FAILED,
                                 status,
                                 UNIQUE_ERRID(0x90),
                                 NULL,
                                 NULL);
                        TRACEERROR(("failed to set HandFlow (status=%x).\n",
                                    status));
                    }
                    else
                    {
                        //
                        // Enable FIFO receive trigger at 8 bytes
                        //
                        ULONG Data = SERIAL_IOC_FCR_FIFO_ENABLE |
                                     SERIAL_IOC_FCR_RCVR_RESET |
                                     SERIAL_IOC_FCR_XMIT_RESET |
                                     SERIAL_IOC_FCR_RCVR_TRIGGER_08_BYTES;

                        status = SerialSyncSendIoctl(IOCTL_SERIAL_SET_FIFO_CONTROL,
                                                     DevExt->LowerDevObj,
                                                     &Data,
                                                     sizeof(Data),
                                                     NULL,
                                                     0,
                                                     FALSE,
                                                     &iosb);
                        if (!NT_SUCCESS(status))
                        {
                            LogError(ERRLOG_INIT_SERIAL_FAILED,
                                     status,
                                     UNIQUE_ERRID(0xa0),
                                     NULL,
                                     NULL);
                            TRACEERROR(("failed to set COM FIFO control (status=%x).\n",
                                        status));
                        }
                        else
                        {
                            Data = SERIAL_PURGE_RXCLEAR;
                            status = SerialSyncSendIoctl(IOCTL_SERIAL_PURGE,
                                                         DevExt->LowerDevObj,
                                                         &Data,
                                                         sizeof(Data),
                                                         NULL,
                                                         0,
                                                         FALSE,
                                                         &iosb);
                            if (!NT_SUCCESS(status))
                            {
                                LogError(ERRLOG_INIT_SERIAL_FAILED,
                                         status,
                                         UNIQUE_ERRID(0xb0),
                                         NULL,
                                         NULL);
                                TRACEERROR(("failed to flush COM receive buffer (status=%x).\n",
                                            status));
                            }
                        }
                    }
                }
            }
        }
    }

    TRACEEXIT(("=%x\n", status));
    return status;
}       //OemInitSerialPort

/*++
    @doc    INTERNAL

    @func   NTSTATUS | QueryDeviceInfo | Query pen tablet device information.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.

    @rvalue SUCCESS | returns STATUS_SUCCESS
    @rvalue FAILURE | returns NT status code
--*/

NTSTATUS INTERNAL
QueryDeviceInfo(
    IN PDEVICE_EXTENSION DevExt
    )
{
    TRACEPROC("QueryDeviceInfo", 2)
    NTSTATUS status;

    PAGED_CODE();
    TRACEENTER(("(DevExt=%p)\n", DevExt));

    status = SerialSyncReadWritePort(FALSE, DevExt, "0", 1, NULL, NULL);
    if (!NT_SUCCESS(status))
    {
        LogError(ERRLOG_QUERY_DEVINFO_FAILED,
                 status,
                 UNIQUE_ERRID(0xc0),
                 NULL,
                 NULL);
        TRACEWARN(("failed to stop the digitizer (status=%x).\n", status));
    }
    else
    {
        LARGE_INTEGER Timeout;

        Timeout.QuadPart = Int32x32To64(1000, -10000);
        KeDelayExecutionThread(KernelMode, FALSE, (LARGE_INTEGER *)&Timeout);
        status = SerialSyncReadWritePort(FALSE, DevExt, "*", 1, NULL, NULL);
        if (!NT_SUCCESS(status))
        {
            LogError(ERRLOG_QUERY_DEVINFO_FAILED,
                     status,
                     UNIQUE_ERRID(0xd0),
                     NULL,
                     NULL);
            TRACEWARN(("failed to send the Query command (status=%x).\n",
                       status));
        }
        else
        {
            QUERYDATA Data;
            ULONG BytesRead;
            #define INSTATUS_CTRLDATA (INSTATUS_SYNC | INSTATUS_CONTROL_DATA)

            // Spec. says 160ms max., set timeout to 200ms just to be safe.
            Timeout.QuadPart = Int32x32To64(1000, -10000);
            while ((status = SerialSyncReadWritePort(TRUE,
                                                     DevExt,
                                                     (PUCHAR)&Data,
                                                     1,
                                                     &Timeout,
                                                     &BytesRead)) ==
                    STATUS_SUCCESS)
            {
                if (Data.QueryData.bStatus == INSTATUS_CTRLDATA)
                {
                    break;
                }
            }

            if (status == STATUS_SUCCESS)
            {
                status = SerialSyncReadWritePort(TRUE,
                                                 DevExt,
                                                 ((PUCHAR)&Data) + 1,
                                                 sizeof(Data) - 1,
                                                 &Timeout,
                                                 &BytesRead);
                if (status == STATUS_SUCCESS)
                {
                    if (BytesRead == sizeof(Data) - 1)
                    {
                        DevExt->OemData.wXMax =
                            (NORMALIZE_DATA(Data.QueryData.wXMax) << 2) |
                            LOW_X_BITS(Data.QueryData.wPressureMax);
                        *((PUSHORT)&gReportDescriptor[BYTEINDEX_XMAX]) =
                            DevExt->OemData.wXMax;

                        DevExt->OemData.wYMax =
                            (NORMALIZE_DATA(Data.QueryData.wYMax) << 2) |
                            LOW_Y_BITS(Data.QueryData.wPressureMax);
                        *((PUSHORT)&gReportDescriptor[BYTEINDEX_YMAX]) =
                            DevExt->OemData.wYMax;

                        DevExt->OemData.wPressureMax =
                            NORMALIZE_PRESSURE(Data.QueryData.wPressureMax);
                        if (DevExt->OemData.wPressureMax != 0)
                        {
                            *((PUSHORT)&gReportDescriptor[BYTEINDEX_PRESSUREMAX]) =
                                DevExt->OemData.wPressureMax;
                        }

                        DevExt->OemData.bXTiltMax =
                            Data.QueryData.bXTiltMax & ~INBDATA_SYNC;
                        if (DevExt->OemData.bXTiltMax != 0)
                        {
                            gReportDescriptor[BYTEINDEX_XTILTMIN] =
                                -DevExt->OemData.bXTiltMax;
                            gReportDescriptor[BYTEINDEX_XTILTMAX] =
                                DevExt->OemData.bXTiltMax;
                        }

                        DevExt->OemData.bYTiltMax =
                            Data.QueryData.bYTiltMax & ~INBDATA_SYNC;
                        if (DevExt->OemData.bYTiltMax != 0)
                        {
                            gReportDescriptor[BYTEINDEX_YTILTMIN] =
                                -DevExt->OemData.bYTiltMax;
                            gReportDescriptor[BYTEINDEX_YTILTMAX] =
                                DevExt->OemData.bYTiltMax;
                        }

                        DevExt->OemData.wFirmwareVer =
                            NORMALIZE_DATA(Data.QueryData.wFirmwareVer);

                        //
                        // Initialize the default pressure thresholds and
                        // max baseline pressures.
                        //
                        DevExt->OemData.PressureBaseLines.wTip =
                        DevExt->OemData.PressureBaseLines.wEraser =
                            DEF_PRESSURE_THRESHOLD;
                        DevExt->OemData.PressureThresholds.wTip =
                        DevExt->OemData.PressureThresholds.wEraser =
                            MIN_PRESSURE_THRESHOLD;

                        TRACEINFO(1, ("XMax=%x(%d),YMax=%x(%d),PressureMax=%x(%d),XTiltMax=%x(%d),YTiltMax=%x(%d),FirmwareVer=%x(%d),TipPressThreshold=%d,EraserPressThreshold=%d\n",
                                      DevExt->OemData.wXMax,
                                      DevExt->OemData.wXMax,
                                      DevExt->OemData.wYMax,
                                      DevExt->OemData.wYMax,
                                      DevExt->OemData.wPressureMax,
                                      DevExt->OemData.wPressureMax,
                                      DevExt->OemData.bXTiltMax,
                                      DevExt->OemData.bXTiltMax,
                                      DevExt->OemData.bYTiltMax,
                                      DevExt->OemData.bYTiltMax,
                                      DevExt->OemData.wFirmwareVer,
                                      DevExt->OemData.wFirmwareVer,
                                      DevExt->OemData.PressureThresholds.wTip,
                                      DevExt->OemData.PressureThresholds.wEraser));
                    }
                    else
                    {
                        status = STATUS_DEVICE_DATA_ERROR;
                        LogError(ERRLOG_QUERY_DEVINFO_FAILED,
                                 status,
                                 UNIQUE_ERRID(0xe0),
                                 NULL,
                                 NULL);
                        TRACEWARN(("invalid status response (size=%d,pData=%p).\n",
                                   BytesRead, &Data));
                    }
                }
                else
                {
                    LogError(ERRLOG_QUERY_DEVINFO_FAILED,
                             status,
                             UNIQUE_ERRID(0xf0),
                             NULL,
                             NULL);
                    TRACEWARN(("failed to read response packet (status=%x).\n",
                               status));
                }
            }
            else
            {
                LogError(ERRLOG_QUERY_DEVINFO_FAILED,
                         status,
                         UNIQUE_ERRID(0x110),
                         NULL,
                         NULL);
                TRACEWARN(("failed to read 1st response byte (status=%x).\n",
                           status));
            }
        }
    }

    TRACEEXIT(("=%x\n", status));
    return status;
}       //QueryDeviceInfo

/*++
    @doc    INTERNAL

    @func   BOOLEAN | OemIsResyncDataValid | Check if the data in the resync
            buffer is valid.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.

    @rvalue SUCCESS | Returns TRUE.
    @rvalue FAILURE | Returns FALSE.
--*/

BOOLEAN INTERNAL
OemIsResyncDataValid(
    IN PDEVICE_EXTENSION DevExt
    )
{
    TRACEPROC("OemIsResyncDataValid", 2)
    BOOLEAN rc;

    TRACEENTER(("(DevExt=%p)\n", DevExt));

    rc = IsValidPacket(&DevExt->OemData.ResyncData[0]);
    if ((rc == FALSE) ||
        (DevExt->OemData.BytesInBuff > sizeof(OEM_INPUT_REPORT)))
    {
        PUCHAR pb = (PUCHAR)&DevExt->OemData.ResyncData[0] +
                    DevExt->OemData.BytesInBuff - 1;
        PUCHAR pbEnd = rc? (PUCHAR)&DevExt->OemData.ResyncData[1]:
                           (PUCHAR)&DevExt->OemData.ResyncData[0];

        //
        // Even if we seem to have a valid packet in the resync buffer, we
        // still need to scan the next packet if any.  If the next packet
        // has a sync bit out of place, the first packet could still be
        // invalid and we better throw it away.
        //
        while (pb > pbEnd)
        {
            if (*pb & INSTATUS_SYNC)
            {
                TRACEINFO(3,
                          ("invalid buffer (len=%d,status0=%x,x0=%x,y0=%x,Pressure0=%x,XTilt0=%x,YTilt0=%x,status1=%x,x1=%x,y1=%x,Pressure1=%x,XTilt1=%x,YTilt1=%x)\n",
                           DevExt->OemData.BytesInBuff,
                           DevExt->OemData.ResyncData[0].InputReport.bStatus,
                           DevExt->OemData.ResyncData[0].InputReport.wXData,
                           DevExt->OemData.ResyncData[0].InputReport.wYData,
                           DevExt->OemData.ResyncData[0].InputReport.wPressureData,
                           DevExt->OemData.ResyncData[0].InputReport.bXTiltData,
                           DevExt->OemData.ResyncData[0].InputReport.bYTiltData,
                           DevExt->OemData.ResyncData[1].InputReport.bStatus,
                           DevExt->OemData.ResyncData[1].InputReport.wXData,
                           DevExt->OemData.ResyncData[1].InputReport.wYData,
                           DevExt->OemData.ResyncData[1].InputReport.wPressureData,
                           DevExt->OemData.ResyncData[1].InputReport.bXTiltData,
                           DevExt->OemData.ResyncData[1].InputReport.bYTiltData));
                DevExt->OemData.BytesInBuff = (ULONG)
                        ((PUCHAR)&DevExt->OemData.ResyncData[0] +
                         DevExt->OemData.BytesInBuff - pb);
                RtlMoveMemory(&DevExt->OemData.ResyncData[0],
                              pb,
                              DevExt->OemData.BytesInBuff);
                TRACEINFO(3,
                          ("Resync'd buffer (len=%d,status=%x,x=%x,y=%x,Pressure=%x,XTilt=%x,YTilt=%x)\n",
                           DevExt->OemData.BytesInBuff,
                           DevExt->OemData.ResyncData[0].InputReport.bStatus,
                           DevExt->OemData.ResyncData[0].InputReport.wXData,
                           DevExt->OemData.ResyncData[0].InputReport.wYData,
                           DevExt->OemData.ResyncData[0].InputReport.wPressureData,
                           DevExt->OemData.ResyncData[0].InputReport.bXTiltData,
                           DevExt->OemData.ResyncData[0].InputReport.bYTiltData));
              #ifdef DEBUG
                {
                    ULONG dwcDeletedBytes = (ULONG)
                                (pb - (PUCHAR)&DevExt->OemData.ResyncData[0]);

                    DevExt->OemData.dwcLostBytes +=
                        (dwcDeletedBytes > sizeof(OEM_INPUT_REPORT))?
                                sizeof(OEM_INPUT_REPORT)*2 - dwcDeletedBytes:
                                sizeof(OEM_INPUT_REPORT) - dwcDeletedBytes;
                }
              #endif
                rc = FALSE;
                break;
            }
            --pb;
        }

        if ((rc == FALSE) && (pb <= pbEnd))
        {
            //
            // We didn't have a valid packet and we couldn't find the sync
            // bit of the next packet, so the whole resync buffer is invalid.
            //
            DevExt->OemData.BytesInBuff = 0;
        }
    }

    TRACEEXIT(("=%x\n", rc));
    return rc;
}       //OemIsResyncDataValid

/*++
    @doc    INTERNAL

    @func   NTSTATUS | OemNormalizeInputData | Normalize the input data.

    @parm   IN PDEVICE_EXTENSION | DevExt | Points to the device extension.
    @parm   IN OUT POEM_INPUT_REPORT | InData | Points to the input data packet.

    @rvalue SUCCESS | Returns STATUS_SUCCESS.
    @rvalue FAILURE | Returns STATUS_DATA_ERROR.
--*/

NTSTATUS INTERNAL
OemNormalizeInputData(
    IN     PDEVICE_EXTENSION DevExt,
    IN OUT POEM_INPUT_REPORT InData
    )
{
    TRACEPROC("OemNormalizeInputData", 2)
    NTSTATUS status = STATUS_SUCCESS;
    UCHAR bCurStatus;
    BOOLEAN fInRange;
    USHORT wPressureThreshold;
    PUSHORT pwPressureBaseLine;
#ifdef DEBUG
    ULONGLONG CurrentTime;
#endif

    TRACEENTER(("(DevExt=%p,InData=%p,Status=%x,XData=%x,YData=%x)\n",
                DevExt, InData, InData->InputReport.bStatus,
                InData->InputReport.wXData, InData->InputReport.wYData));

    bCurStatus = InData->InputReport.bStatus;
    fInRange = (bCurStatus & INSTATUS_IN_RANGE) != 0;
    wPressureThreshold = (bCurStatus & INSTATUS_INVERTED)?
                                DevExt->OemData.PressureThresholds.wEraser:
                                DevExt->OemData.PressureThresholds.wTip;
    pwPressureBaseLine = (bCurStatus & INSTATUS_INVERTED)?
                                &DevExt->OemData.PressureBaseLines.wEraser:
                                &DevExt->OemData.PressureBaseLines.wTip;

    InData->InputReport.wXData =
        (NORMALIZE_DATA(InData->InputReport.wXData) << 2) |
        LOW_X_BITS(InData->InputReport.wPressureData);
    InData->InputReport.wYData =
        (NORMALIZE_DATA(InData->InputReport.wYData) << 2) |
        LOW_Y_BITS(InData->InputReport.wPressureData);
    InData->InputReport.wPressureData = fInRange?
                                        NORMALIZE_PRESSURE(
                                            InData->InputReport.wPressureData):
                                        0;
    InData->InputReport.bXTiltData &= ~INBDATA_SYNC;
    InData->InputReport.bYTiltData &= ~INBDATA_SYNC;

    if (!(DevExt->OemData.bLastStatus & INSTATUS_IN_RANGE) && fInRange)
    {
        //
        // We just came into proximity, let's determine our baseline pressure
        // again. This could drift upward from the last baseline pressure
        // because the "pressure spring" may not recover back to its original
        // shape.
        //
        if (InData->InputReport.wPressureData < DEF_PRESSURE_THRESHOLD)
        {
            *pwPressureBaseLine = InData->InputReport.wPressureData;
        }
    }
    else if (InData->InputReport.wPressureData < *pwPressureBaseLine)
    {
        //
        // This allows the baseline pressure to drift downward as the
        // "pressure spring" recovers.
        //
        *pwPressureBaseLine = InData->InputReport.wPressureData;
    }
    //
    // Zero adjust and re-scale the pressure according to baseline.
    //
    REBASE_PRESSURE(DevExt,
                    InData->InputReport.wPressureData,
                    *pwPressureBaseLine);

    //
    // Ignore the PEN_TIP_DOWN bit from the firmware and generate our
    // own using our pressure threshold.
    //
    InData->InputReport.bStatus &= ~INSTATUS_PEN_TIP_DOWN;
    if (InData->InputReport.wPressureData >= wPressureThreshold)
    {
        InData->InputReport.bStatus |= INSTATUS_PEN_TIP_DOWN;
        REBASE_PRESSURE(DevExt,
                        InData->InputReport.wPressureData,
                        wPressureThreshold);
    }
    else
    {
        InData->InputReport.wPressureData = 0;
    }
#ifdef DEBUG
    if ((bCurStatus^InData->InputReport.bStatus) & INSTATUS_PEN_TIP_DOWN)
    {
        TRACEINFO(3, ("Pressure Thresholds different: Status=%x, Pressure=%d, Baseline=%d, Threshold=%d\n",
                      bCurStatus, InData->InputReport.wPressureData,
                      *pwPressureBaseLine, wPressureThreshold));
    }
#endif

    if ((InData->InputReport.bStatus & INSTATUS_INVERTED) &&
        (InData->InputReport.bStatus & INSTATUS_PEN_TIP_DOWN))
    {
        //
        // The eraser is down.
        //
        InData->InputReport.bStatus &= ~INSTATUS_PEN_TIP_DOWN;
        InData->InputReport.bStatus |= INSTATUS_ERASER_DOWN;
    }
    else
    {
        //
        // ERASER_DOWN is a generated bit not from the hardware.  We clear it
        // so that it wouldn't contain any garbage.
        //
        InData->InputReport.bStatus &= ~INSTATUS_ERASER_DOWN;
    }

    if (!fInRange)
    {
        //
        // We are going out of proximity, make sure pen and eraser are not
        // down. There is a problem with the tip pressure support that the
        // tip pressure may still be above the "pen down" threshold when it
        // is out-of-proxmity.  So the next time the pen comes back into
        // proximity, it then realizes there was a pen up and generates a
        // pen up at the re-entry location which creates a hyper jump from
        // the exit proximity point to the re-entry proximity point.  By
        // clearing the bits here, we are preventing this problem by
        // simulating a pen/erase up event on the last point before we exit
        // proximity.
        //
        InData->InputReport.bStatus &= ~(INSTATUS_PEN_TIP_DOWN |
                                         INSTATUS_ERASER_DOWN);
    }

    if (InData->InputReport.wXData > DevExt->OemData.wXMax)
    {
      #ifdef DEBUG
        if (InData->InputReport.wXData > DevExt->OemData.wMaxX)
        {
            DevExt->OemData.wMaxX = InData->InputReport.wXData;
        }
      #endif
        InData->InputReport.wXData = DevExt->OemData.wXMax;
    }

    if (InData->InputReport.wYData > DevExt->OemData.wYMax)
    {
      #ifdef DEBUG
        if (InData->InputReport.wYData > DevExt->OemData.wMaxY)
        {
            DevExt->OemData.wMaxY = InData->InputReport.wYData;
        }
      #endif
        InData->InputReport.wYData = DevExt->OemData.wYMax;
    }
    DevExt->OemData.bLastStatus = bCurStatus;

#ifdef _TIMESTAMP_
    DevExt->OemData.LastCounter = KeQueryPerformanceCounter(NULL);
#endif
#ifdef DEBUG
    CurrentTime = KeQueryInterruptTime();
    if ((DevExt->OemData.LastReport.InputReport.bStatus ^
         InData->InputReport.bStatus) & INSTATUS_PEN_TIP_DOWN)
    {
        //
        // The tip switch changes state
        //
        if (InData->InputReport.bStatus & INSTATUS_PEN_TIP_DOWN)
        {
            DevExt->OemData.dwcSamples = 0;
            DevExt->OemData.dwcLostBytes = 0;
            DevExt->OemData.StartTime = CurrentTime;
        }
        else
        {
            CurrentTime -= DevExt->OemData.StartTime;
            CurrentTime /= 10000;
            TRACEINFO(1, ("Samples=%d,Elapsed=%d,Rate=%d,BytesLost=%d\n",
                          DevExt->OemData.dwcSamples,
                          (ULONG)CurrentTime,
                          CurrentTime? DevExt->OemData.dwcSamples*1000/
                                       CurrentTime:
                                       0,
                          DevExt->OemData.dwcLostBytes));
        }
    }

    DevExt->OemData.dwcSamples++;

    DevExt->OemData.LastReportTime = CurrentTime;
    RtlCopyMemory(&DevExt->OemData.LastReport,
                  InData,
                  sizeof(DevExt->OemData.LastReport));
#endif
    PoSetSystemState(ES_USER_PRESENT);

    TRACEINFO(3, ("status=%x,x=%x(%d),y=%x(%d),Pressure=%x(%d),XTilt=%x(%d),YTilt=%x(%d)\n",
                  InData->InputReport.bStatus,
                  InData->InputReport.wXData,
                  InData->InputReport.wXData,
                  InData->InputReport.wYData,
                  InData->InputReport.wYData,
                  InData->InputReport.wPressureData,
                  InData->InputReport.wPressureData,
                  InData->InputReport.bXTiltData,
                  InData->InputReport.bXTiltData,
                  InData->InputReport.bYTiltData,
                  InData->InputReport.bYTiltData));

    TRACEEXIT(("=%x (Status=%x,XData=%x(%d),YData=%x(%d)\n",
               status,
               InData->InputReport.bStatus,
               InData->InputReport.wXData,
               InData->InputReport.wXData,
               InData->InputReport.wYData,
               InData->InputReport.wYData,
               InData->InputReport.wPressureData,
               InData->InputReport.wPressureData,
               InData->InputReport.bXTiltData,
               InData->InputReport.bXTiltData,
               InData->InputReport.bYTiltData,
               InData->InputReport.bYTiltData));
    return status;
}       //OemNormalizeInputData
