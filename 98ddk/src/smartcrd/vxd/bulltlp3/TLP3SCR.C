/*++

Copyright (c) 1998 Bull Smart Cards & Terminals

Module Name:

    TLP3Scr.c

Abstract:

    PC/SC Template Driver

Environment:

    Win 95

--*/

#include <TLP3Scr.h>

#pragma CM_PAGEABLE_DATA
#pragma CM_PAGEABLE_CODE

DEVICE_DATA DriverData[ TLP3SCR_MAX_DEVICE ];
static DWORD NumDevices = 0;

CM_VXD_RESULT
CM_SYSCTRL
TLP3SysDynamicDeviceInit(
    PVMMDDB pDDB
    )
/*++

TLP3SysDynamicDeviceInit:
    called when the driver is loaded

Arguments:
    pDDB    pointer to device descriptor block

Return Value:
    VXD_SUCCESS

--*/
{
    DWORD dwFunctionRet;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3SysDynamicDeviceInit: Enter\n",
        TLP3SCR_NAME)
        );

    //  mark DriverData for all devices as unused
    SysFillMemory( (PUCHAR)DriverData, 0, sizeof( DEVICE_DATA ) * TLP3SCR_MAX_DEVICE );

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3SysDynamicDeviceInit: Exit\n",
        TLP3SCR_NAME)
        );

    return (VXD_SUCCESS);
}

CM_VXD_RESULT
CM_SYSCTRL
TLP3SysDynamicDeviceExit(
    PVMMDDB pDDB
    )
/*++

TLP3SysDynamicDeviceExit:
    call when the driver is unloaded

Arguments:
    pDDB    pointer to device descriptor block

Return Value:
    VXD_SUCCESS

--*/
{
    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3SysDynamicDeviceExit\n",
        TLP3SCR_NAME)
        );

    return VXD_SUCCESS;
}

//  ---------------------------------------------------------------------------
//  DEVICE INITIALIZATION
//  ---------------------------------------------------------------------------

CONFIGRET CM_SYSCTRL
TLP3PnPNewDevnode(
    DEVNODE DevNode,
    LOAD_TYPE LoadType
    )
/*++

DrvPnPNewDevnode:
    called if a new reader is detected.

Arguments:
    DevNode     device node of new device
    LoadType    requested load type

Return Value:
    CR_DEFAULT for not supported load types, else the return value of the
    configuration manager

--*/
{
    CONFIGRET   CRStatus = CR_SUCCESS;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3PnPNewDevnode: Enter\n",
        TLP3SCR_NAME)
        );

    switch( LoadType )
    {
        case DLVXD_LOAD_DEVLOADER:
            CRStatus = CM_Register_Device_Driver(
                DevNode,
                TLP3Configure,
                0,
                CM_REGISTER_DEVICE_DRIVER_REMOVABLE |
                CM_REGISTER_DEVICE_DRIVER_DISABLEABLE
                );
            break;

        default:
            CRStatus = CR_DEFAULT;
            break;
    };

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3PnPNewDevnode: Exit %X\n",
        TLP3SCR_NAME,
        CRStatus)
        );


    return CRStatus;
}

CONFIGRET
TLP3Configure(
    CONFIGFUNC          ConfigFct,
    SUBCONFIGFUNC       SubConfigFct,
    DEVNODE             DeviceNode,
    DWORD               ReferenceData,
    ULONG               Flags
    )
/*++

TLP3Configure:
    called by the configuration manager to change the actual configuration
    of a device

Arguments:
    ConfigFct           requested function
    SubConfigFct        requested subfunction (ignored)
    DeviceNode          device node of the device to configure
    ReferenceData       not used
    Flags               not used

Return Value:
    CR_SUCCESS
    CR_DEFAULT          fct not supported
    CR_OUT_OF_MEMORY    no empty slot
    CR_FAILURE          no device found

--*/
{
    CONFIGRET CRStatus = CR_SUCCESS;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3Configure: Enter ConfigFunc = %X\n",
        TLP3SCR_NAME,
        ConfigFct)
        );

    switch( ConfigFct )
    {
        //
        //  start device
        //
        case CONFIG_START:
            CRStatus = MapNtStatusToWinError( TLP3StartDevice( DeviceNode ));
            break;

        //
        //  stop device
        //
        case CONFIG_REMOVE:
        case CONFIG_STOP:
            CRStatus = MapNtStatusToWinError( TLP3StopDevice( DeviceNode ));
            break;

        default:
            CRStatus = CR_DEFAULT;
    }

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3Configure: Exit %X\n",
        TLP3SCR_NAME,
        CRStatus)
        );

    return CRStatus;
}

NTSTATUS
TLP3StartDevice(
    DEVNODE DeviceNode
    )
/*++

TLP3StartDevice:
    Initialize a new device

Arguments:

    DeviceNode  device node of the new device

Return Value:
    return value of the smclib

--*/
{
    NTSTATUS NTStatus = STATUS_SUCCESS;
    PDEVICE_DATA DeviceData = NULL;
    UINT DeviceNo = 0;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3StartDevice: Enter\n",
        TLP3SCR_NAME)
        );

    //
    //  find a free entry in DriverData array
    //
    do {

        if( DriverData[DeviceNo].DeviceNode == 0 )
        {
            DeviceData = &DriverData[DeviceNo];
            break;
        }

    } while( ++DeviceNo < TLP3SCR_MAX_DEVICE );

    //
    //  free entry found?
    //
    if( DeviceData != NULL )
    {
        UCHAR DeviceName[16];
        PVMMDDB pDeviceDDB = NULL;

        DeviceData->DeviceNode = DeviceNode;

        if (DeviceNo == 0)
        {
            CONFIGRET  cfFunctionRet = CR_SUCCESS;
            PPPVMMDDB pppDDB;
            ULONG ulCount;

            //
            // Since this driver already represents a device, we DON'T create
            // a new DDB for this existing device
            //
            _Sprintf(DeviceName, "BULLTLP3.VXD");

            cfFunctionRet = CM_Get_DDBs(
                pppDDB,
                &ulCount,
                DLVXD_LOAD_DRIVER,
                DeviceNode,
                0
                );

            if (cfFunctionRet != CR_SUCCESS) {

                SmartcardDebug(
                    DEBUG_TRACE,
                    ("%s!TLP3StartDevice: CM_Get_DDBs ret ERROR = %X\n",
                    TLP3SCR_NAME,
                    cfFunctionRet)
                    );

            } else {

                pDeviceDDB = **pppDDB;
            }

        } else {

            //
            // Create a new device name
            //
            _Sprintf(DeviceName, "TLP3SCR%d", DeviceNo);
        }

        if (DeviceNo != 0) {

            // Now create a new device DDB and thereby a new instance
            pDeviceDDB = VxD_CreateDevice(
                DeviceName,
                TLP3ControlProcedure);
        }

        if (DeviceNo != 0 && pDeviceDDB == NULL) {

            NTStatus = STATUS_UNSUCCESSFUL;
        }

        if( NT_SUCCESS( NTStatus )) {

            //
            // Now we can initialize the device.
            // (open and configure a serial port, etc.)
            //
            NTStatus = TLP3InitSingleDevice(
                DeviceNo,
                DeviceName,
                pDeviceDDB);
        }
    }

    //
    //  perform rollback if something was wrong
    //
    if( !NT_SUCCESS( NTStatus )) {

        TLP3DeleteDevice(DeviceNo);
    }
    else {

        // Increase the number of device instances
        NumDevices++;
    }

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3StartDevice: Exit %X\n",
        TLP3SCR_NAME,
        NTStatus)
        );

    return( NTStatus );
}

NTSTATUS
TLP3InitSingleDevice(
    UINT DeviceNo,
    PUCHAR DeviceName,
    PVMMDDB DeviceDDB
    )
/*++

TLP3InitSingleDevice:
    Initialize a new single device

Arguments:
    DeviceNo        Device Number
    DeviceName      Device Name
    DeviceDDB       pointer of the device data block


Return Value:
    return value of the smclib
    STATUS_INSUFFICIENT_RESOURCES

--*/
{
    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
    PREADER_EXTENSION pReaderExtension;


    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3InitSingleDevice: Enter\n",
        TLP3SCR_NAME)
        );

    //
    // Allocate space for the smart card extension
    //
    DriverData[DeviceNo].PSmartcardExtension = (PSMARTCARD_EXTENSION) _HeapAllocate(
        sizeof(SMARTCARD_EXTENSION),
        HEAPZEROINIT
        );

    //
    // Allocate space for my reader extension
    //
    pReaderExtension = (PREADER_EXTENSION) _HeapAllocate(
        sizeof(READER_EXTENSION),
        HEAPZEROINIT
        );

    //
    // Continue if allocation didn't fail
    //
    if (DriverData[DeviceNo].PSmartcardExtension && pReaderExtension)
    {
        DriverData[DeviceNo].PSmartcardExtension->ReaderExtension = pReaderExtension;
        DriverData[DeviceNo].PSmartcardExtension->Version = SMCLIB_VERSION;
        DriverData[DeviceNo].DDB = DeviceDDB;
    }
    else
    {
        if (DriverData[DeviceNo].PSmartcardExtension)
        {
            _HeapFree(DriverData[DeviceNo].PSmartcardExtension, 0);
        }
        if (pReaderExtension)
        {
            _HeapFree(pReaderExtension, 0);
        }
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    status = TLP3GetComPort(
        DriverData[DeviceNo].PSmartcardExtension,
        DriverData[DeviceNo].DeviceNode
        );

    if (status != STATUS_SUCCESS)
    {
        if (DriverData[DeviceNo].PSmartcardExtension)
        {
            _HeapFree(DriverData[DeviceNo].PSmartcardExtension, 0);
        }
        if (pReaderExtension)
        {
            _HeapFree(pReaderExtension, 0);
        }
        return status;
    }

    //
    // initilize the smartcard extension for a given device
    //
    TLP3SetupSmartcardExtension(DeviceNo);

    status = SmartcardInitialize(
        DriverData[DeviceNo].PSmartcardExtension
        );

    if (status != STATUS_SUCCESS)
    {
        if (DriverData[DeviceNo].PSmartcardExtension)
        {
            _HeapFree(DriverData[DeviceNo].PSmartcardExtension, 0);
        }
        if (pReaderExtension)
        {
            _HeapFree(pReaderExtension, 0);
        }
        SmartcardExit(DriverData[DeviceNo].PSmartcardExtension);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    status = SmartcardCreateLink(
        DriverData[DeviceNo].PSmartcardExtension->ReaderExtension->LinkName,
        DeviceName
        );

    if (status != STATUS_SUCCESS)
    {
        if (DriverData[DeviceNo].PSmartcardExtension->ReaderExtension->LinkName[0])
        {
            status = SmartcardDeleteLink(
                DriverData[DeviceNo].PSmartcardExtension->ReaderExtension->LinkName
                );
        }
        SmartcardExit(DriverData[DeviceNo].PSmartcardExtension);
        if (DriverData[DeviceNo].PSmartcardExtension)
        {
            _HeapFree(DriverData[DeviceNo].PSmartcardExtension, 0);
        }
        if (pReaderExtension)
        {
            _HeapFree(pReaderExtension, 0);
        }
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    //  Init communication Port
    //
    if (TLP3InitCommPort(DriverData[DeviceNo].PSmartcardExtension) != TRUE)
    {
        if (DriverData[DeviceNo].PSmartcardExtension->ReaderExtension->LinkName[0])
        {
            SmartcardDeleteLink(
                DriverData[DeviceNo].PSmartcardExtension->ReaderExtension->LinkName
                );
        }
        SmartcardExit(DriverData[DeviceNo].PSmartcardExtension);
        if (DriverData[DeviceNo].PSmartcardExtension)
        {
            _HeapFree(DriverData[DeviceNo].PSmartcardExtension, 0);
        }
        if (pReaderExtension)
        {
            _HeapFree(pReaderExtension, 0);
        }
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Init of the Card Tracking
    //
    status = TLP3StartSerialEventTracking(DeviceNo);
    if (status != STATUS_SUCCESS)
    {
        if (DriverData[DeviceNo].PSmartcardExtension->ReaderExtension->LinkName[0])
        {
            SmartcardDeleteLink(
                DriverData[DeviceNo].PSmartcardExtension->ReaderExtension->LinkName
                );
        }
        SmartcardExit(DriverData[DeviceNo].PSmartcardExtension);
        if (DriverData[DeviceNo].PSmartcardExtension)
        {
            _HeapFree(DriverData[DeviceNo].PSmartcardExtension, 0);
        }
        if (pReaderExtension)
        {
            _HeapFree(pReaderExtension, 0);
        }
        return status;
    }

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3InitSingleDevice: Exit\n",
        TLP3SCR_NAME)
        );

    return STATUS_SUCCESS;
}

NTSTATUS
TLP3GetComPort(
    PSMARTCARD_EXTENSION SmartcardExtension,
    DEVNODE DevNode
    )
/*++
TLP3GetComPort:

    Read the port number of our reader in the registry

Arguments:

    SmartcardExtension      Pointer to smart card data struct.
    DevNode            Driver registry parh

Return Value:

    STATUS_SUCCESS          if the Comm Port number is found
    STATUS_NO_SUCH_DEVICE   if the Comm Port number is not found


--*/
{
    DEVNODE ParentDevNode;
    char pcParentKey[MAX_VMM_REG_KEY_LEN];
    char pcPort[6];
    ULONG cbLength;
    CONFIGRET cfFunctionRet = CR_SUCCESS;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3GetComPort: Enter\n",
        TLP3SCR_NAME)
        );

    //
    // Get Parent DevNode
    //
    cfFunctionRet = CM_Get_Parent(&ParentDevNode, DevNode, 0);
    ASSERT(cfFunctionRet == CR_SUCCESS);

    if (cfFunctionRet == CR_SUCCESS)
    {
        //
        // Retrive Parent Key
        //
        cfFunctionRet = CM_Get_DevNode_Key(
            ParentDevNode,
            NULL,
            pcParentKey,
            sizeof(pcParentKey),
            CM_REGISTRY_HARDWARE
            );

        ASSERT(cfFunctionRet == CR_SUCCESS);
    }

    if (cfFunctionRet == CR_SUCCESS)
    {
        SmartcardDebug(
            DEBUG_TRACE,
            ("%s!TLP3GetComPort: Parent Key = %s\n",
            TLP3SCR_NAME,
            pcParentKey)
            );

        cbLength = sizeof(pcPort);

        //
        // Retrieves the COMM PORT value in the registry.
        //
        cfFunctionRet = CM_Read_Registry_Value(
            ParentDevNode,
            NULL,
            "PORTNAME",
            REG_SZ,
            pcPort,
            &cbLength,
            CM_REGISTRY_HARDWARE
            );

        ASSERT(cfFunctionRet == CR_SUCCESS);

        if(cfFunctionRet == CR_SUCCESS)
        {
            SysCopyMemory(
                SmartcardExtension->ReaderExtension->pcCommNumber,
                pcPort,
                sizeof(pcPort)
                );

            SmartcardExtension->ReaderCapabilities.Channel = pcPort[3] - '0';

            SmartcardDebug(
                DEBUG_TRACE,
                ("%s!TLP3GetComPort: ComPort = %s\n",
                TLP3SCR_NAME,
                SmartcardExtension->ReaderExtension->pcCommNumber)
                );
        }
    }

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3GetComPort: Exit\n",
        TLP3SCR_NAME)
        );

    if (cfFunctionRet == CR_SUCCESS)
    {
        return STATUS_SUCCESS;
    }
    else
    {
        return STATUS_NO_SUCH_DEVICE;
    }
}

VOID
_stdcall
TLP3SetupSmartcardExtension(
    UINT DeviceNo
    )
/*++
TLP3SetupSmartcardExtension:

    initilize the smartcard extension for a given device

Arguments:

    DeviceNo    Device Number

Return Value:


--*/
{
    // this is a list of our supported data rates
    static ULONG dataRatesSupported[] = { 9600, 19200, 38400, 57600, 115200 };
    PSMARTCARD_EXTENSION SmartcardExtension;
    PREADER_EXTENSION readerExtension;

    SmartcardExtension = DriverData[DeviceNo].PSmartcardExtension;
    readerExtension = SmartcardExtension->ReaderExtension;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3SetupSmartcardExtension: Enter\n",
        TLP3SCR_NAME)
        );

    // Write the version of the lib we use to the smartcard extension
//    SmartcardExtension->Version = SMCLIB_VERSION;
    SmartcardExtension->SmartcardRequest.BufferSize =
    SmartcardExtension->SmartcardReply.BufferSize = MIN_BUFFER_SIZE;

    // Set up call back functions
    SmartcardExtension->ReaderFunction[RDF_CARD_POWER] = TLP3ReaderPower;
    SmartcardExtension->ReaderFunction[RDF_TRANSMIT] = TLP3Transmit;
    SmartcardExtension->ReaderFunction[RDF_CARD_TRACKING] = TLP3CardTracking;
    SmartcardExtension->ReaderFunction[RDF_SET_PROTOCOL] = TLP3SetProtocol;
    SmartcardExtension->ReaderFunction[RDF_IOCTL_VENDOR] = TLP3VendorIoctl;

    //
    // Set the vendor information
    //
    SysCopyMemory(
        SmartcardExtension->VendorAttr.VendorName.Buffer,
        TLP3_VENDOR_NAME,
        sizeof( TLP3_VENDOR_NAME )
        );

    SmartcardExtension->VendorAttr.VendorName.Length = sizeof( TLP3_VENDOR_NAME );

    SysCopyMemory(
        SmartcardExtension->VendorAttr.IfdType.Buffer,
        TLP3_PRODUCT_NAME,
        sizeof( TLP3_PRODUCT_NAME )
        );

    SmartcardExtension->VendorAttr.IfdType.Length = sizeof( TLP3_PRODUCT_NAME );
    SmartcardExtension->VendorAttr.UnitNo = DeviceNo;

    //
    // Set the reader capabilities
    //


    // Clk frequency in KHz encoded as little endian integer
    SmartcardExtension->ReaderCapabilities.CLKFrequency.Default = 3571;
    SmartcardExtension->ReaderCapabilities.CLKFrequency.Max = 3571;

    SmartcardExtension->ReaderCapabilities.DataRate.Default =
    SmartcardExtension->ReaderCapabilities.DataRate.Max =
        dataRatesSupported[0];


    // reader could support higher data rates
    SmartcardExtension->ReaderCapabilities.DataRatesSupported.List =
        dataRatesSupported;
    SmartcardExtension->ReaderCapabilities.DataRatesSupported.Entries =
        sizeof(dataRatesSupported) / sizeof(dataRatesSupported[0]);

    SmartcardExtension->ReaderCapabilities.MaxIFSD = 254;

    // Now setup information in our deviceExtension
    SmartcardExtension->ReaderCapabilities.CurrentState =
        (ULONG) SCARD_UNKNOWN;

    // This reader supports T=0 and T=1
    SmartcardExtension->ReaderCapabilities.SupportedProtocols =
        SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1;

    //
    // Reader type serial, keyboard, ....
    //
    SmartcardExtension->ReaderCapabilities.ReaderType =
        SCARD_READER_TYPE_SERIAL;

    SmartcardExtension->ReaderCapabilities.MechProperties = 0;

    //
    // Set serial configuration parameters
    //
    readerExtension->SerialConfigData.theDCB.DCBLength   = sizeof(_DCB);
    readerExtension->SerialConfigData.theDCB.BaudRate = CBR_9600;

    readerExtension->SerialConfigData.theDCB.StopBits = TWOSTOPBITS;
    readerExtension->SerialConfigData.theDCB.Parity = EVENPARITY;
    readerExtension->SerialConfigData.theDCB.ByteSize = 8;
    readerExtension->SerialConfigData.theDCB.BitMask = fBinary;
    readerExtension->SerialConfigData.WaitMask = EV_CTS|EV_DSR;


    // set timeouts
    readerExtension->SerialConfigData.CommTimeouts.ReadIntervalTimeout =
        READ_INTERVAL_TIMEOUT_DEFAULT;
    readerExtension->SerialConfigData.CommTimeouts.ReadTotalTimeoutConstant =
        READ_TOTAL_TIMEOUT_CONSTANT_DEFAULT;
    readerExtension->SerialConfigData.CommTimeouts.ReadTotalTimeoutMultiplier = 0;

    // set special characters
    readerExtension->SerialConfigData.theDCB.ErrorChar = 0;
    readerExtension->SerialConfigData.theDCB.XonChar = 0;
    readerExtension->SerialConfigData.theDCB.XoffChar = 0;

    // Set handflow
    readerExtension->SerialConfigData.theDCB.XonLim = 0;
    readerExtension->SerialConfigData.theDCB.XoffLim = 0;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3SetupSmartcardExtension: Exit\n",
        TLP3SCR_NAME)
        );
}

BOOL
_cdecl
TLP3InitCommPort(
    PSMARTCARD_EXTENSION SmartcardExtension
    )
/*++

TLP3InitCommPort
    This function tries to open the seriell port
    and then initialize the port with parameters cuntained in
    SmartcardExtension structue

Arguments:
  SmartcardExtension    pointer to the smart card extension structure

Return Value:
  TRUE      if successful
  FALSE     otherwise

--*/
{
    HPORT lhPort = 0;
    DWORD actionmask = 0;
    DWORD indata = 0;
    DWORD outdata = 0;
    BOOL bRet = FALSE;
    PREADER_EXTENSION readerExtension = SmartcardExtension->ReaderExtension;
    _QSB qsb;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3InitCommPort: Enter\n",
        TLP3SCR_NAME)
        );

    //
    // Open the Commport
    //
    lhPort = (HPORT) VCOMM_OpenComm(
        SmartcardExtension->ReaderExtension->pcCommNumber,
        (ULONG)-1
        );

    if((lhPort == IE_BADID ) || (lhPort == IE_OPEN ))
    {
        SmartcardDebug(
            DEBUG_DRIVER,
            ("%s!TLP3InitCommPort: Error VCOMM_OpenComm %s\n",
            TLP3SCR_NAME,
            SmartcardExtension->ReaderExtension->pcCommNumber)
            );

        return FALSE;
    }

    SmartcardExtension->ReaderExtension->hCommPort = lhPort;

    //
    // initialize the COMM timeout structure and set the
    // timeouts
    //
    bRet  = VCOMM_GetSetCommTimeouts(
        SmartcardExtension->ReaderExtension->hCommPort,
        &readerExtension->SerialConfigData.CommTimeouts,
        OPENEXCLUSIVE
        );

    ASSERT(bRet != FALSE);

    if (bRet)
    {
        actionmask =
            fBaudRate |
            fByteSize |
            fParity |
            fStopBits |
            fBitMask |
            fErrorChar;

        bRet = VCOMM_SetCommState(
            SmartcardExtension->ReaderExtension->hCommPort,
            &readerExtension->SerialConfigData.theDCB,
            actionmask);

        ASSERT(bRet != FALSE);
    }

    if (bRet)
    {
        //
        // Set inqueue buffer and outqueue buffer for the Commport
        //
        bRet = VCOMM_SetupComm(
            SmartcardExtension->ReaderExtension->hCommPort,
            readerExtension->SerialConfigData.qreceive,
            sizeof(readerExtension->SerialConfigData.qreceive),
            readerExtension->SerialConfigData.qtransmit,
            sizeof(readerExtension->SerialConfigData.qtransmit),
            &qsb
            );

        ASSERT(bRet != FALSE);
    }

    if (bRet)
    {
        //
        // set DTR for the reader
        //
        bRet = VCOMM_EscapeCommFunction(
            SmartcardExtension->ReaderExtension->hCommPort,
            SETDTR,
            indata,
            (long)&outdata
            );

        ASSERT(bRet != FALSE);
    }

    if (bRet)
    {
        //
        // set RTS for the reader
        //
        bRet = VCOMM_EscapeCommFunction(
            SmartcardExtension->ReaderExtension->hCommPort,
            SETRTS,
            indata,
            (long)&outdata
            );

        ASSERT(bRet != FALSE);
    }

    if(bRet == FALSE)
    {
        bRet = VCOMM_CloseComm(
            SmartcardExtension->ReaderExtension->hCommPort
            );

        ASSERT(bRet != FALSE);
        return FALSE;
    }

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3InitCommPort: Exit\n",
        TLP3SCR_NAME)
        );

    return TRUE;
}

NTSTATUS
TLP3StartSerialEventTracking(
    UINT DeviceNo
    )
/*++

TLP3StartSerialEventTracking

    This routine initialized card tracking. It calls the serial driver to
    set a wait mask for CTS tracking. After that it installs a completion
    routine to be called when CTS changes state.

Arguments:

  SmartcardExtension    pointer to the smart card extension structure

Return Value:

  STATUS_SUCCESS                  if successful
  STATUS_INVALID_DEVICE_STATE

--*/
{
    NTSTATUS status;
    BOOLEAN bRet;
    DWORD ModemStatus;

    PSMARTCARD_EXTENSION SmartcardExtension = DriverData[DeviceNo].PSmartcardExtension;
    HPORT hCommPort = SmartcardExtension->ReaderExtension->hCommPort;


    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3StartSerialEventTracking: Enter\n",
        TLP3SCR_NAME)
        );

    // Request notification for every CTS changed
    bRet = VCOMM_SetCommEventMask(
        hCommPort,            
        EV_CTS|EV_DSR,        
        NULL
        );                

    ASSERT(bRet != FALSE);

    if (bRet)
    {
        bRet = VCOMM_EnableCommNotification(
            hCommPort,          
            TLP3SerialEvent,   
            DeviceNo
            );  

        ASSERT(bRet != FALSE);
    }

    if (bRet)
    {
        bRet = VCOMM_GetModemStatus( hCommPort, &ModemStatus);
        ASSERT(bRet != FALSE);
    }

    if (bRet)
    {
        if(ModemStatus & MS_CTS_ON)
        {
            // Card is inserted
            SmartcardExtension->ReaderCapabilities.CurrentState =
                SCARD_SWALLOWED;

            SmartcardExtension->CardCapabilities.Protocol.Selected =
                SCARD_PROTOCOL_UNDEFINED;

            SmartcardDebug(
                DEBUG_DRIVER,
                ("%s!TLP3StartSerialEventTracking: Smart card inserted COM%x\n",
                TLP3SCR_NAME,
                SmartcardExtension->ReaderCapabilities.Channel)
                );
        }
        else
        {
            // Card is removed
            SmartcardExtension->ReaderCapabilities.CurrentState =
                SCARD_ABSENT;

            SmartcardExtension->CardCapabilities.Protocol.Selected =
                SCARD_PROTOCOL_UNDEFINED;

            SmartcardDebug(
                DEBUG_DRIVER,
                ("%s!TLP3StartSerialEventTracking: Smart card removed COM%x\n",
                TLP3SCR_NAME,
                SmartcardExtension->ReaderCapabilities.Channel)
                );
        }
    }

    if(bRet == FALSE)
    {
        DWORD dwdummy;

        //
        // Disable Notification
        //
        VCOMM_EnableCommNotification(
            hCommPort,
            (void *)NULL,
            (LONG)NULL
            );
        //
        // reset the eventmask for EV_CTS
        //
        bRet = VCOMM_GetCommEventMask(
            hCommPort,
            EV_CTS|EV_DSR,
            &dwdummy
            );

        return STATUS_INVALID_DEVICE_STATE;
    }

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3StartSerialEventTracking: Exit\n",
        TLP3SCR_NAME)
        );

    return STATUS_SUCCESS;
}

void
_cdecl
TLP3SerialEvent(
    HPORT hPort,
    UINT DeviceNo,
    DWORD lEvent,
    DWORD lSubEvent
    )
/*++

TLP3SerialEvent

    This routine is called when CTS changes its status, which means a card was
    inserted or removed.

Arguments:

  hPort                 handle to the Commport
  pSmartcardExtension   pointer to the smart card extension structure
  lEvent                Event which occurs
  lSubEvent             Subevent like EV_CTS, EV_RXCHAR, ...


Return Value:


--*/
{
    DWORD   events;                     // Events received since last entry
    DWORD   ModemStatus;
    PSMARTCARD_EXTENSION SmartcardExtension = DriverData[DeviceNo].PSmartcardExtension;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3SerialEvent: Enter DeviceNo = %x\n",
        TLP3SCR_NAME,
        DeviceNo)
        );

    ASSERT(SmartcardExtension != 0);

    //
    // check if the event we receive is a card presence event
    //
    if (lSubEvent ==  EV_CTS && lEvent == CN_EVENT)
    {

        SmartcardExtension->CardCapabilities.ATR.Length = 0;

        // Read and clear event mask for any change of state of CTS
        VCOMM_GetCommEventMask(hPort, EV_CTS, &events);
        VCOMM_GetModemStatus( hPort, &ModemStatus);

        if (ModemStatus & MS_CTS_ON)
        {
            //
            // Card is inserted
            //
            SmartcardExtension->ReaderCapabilities.CurrentState =
                SCARD_SWALLOWED;

            SmartcardExtension->CardCapabilities.Protocol.Selected =
                SCARD_PROTOCOL_UNDEFINED;

            SmartcardDebug(
                DEBUG_DRIVER,
                ("%s!TLP3SerialEvent: Smart card inserted COM%x\n",
                TLP3SCR_NAME,
                SmartcardExtension->ReaderCapabilities.Channel)
                );

            if (SmartcardExtension->ReaderExtension->SerialConfigData.WaitMask == TRUE)
            {
                SmartcardExtension->ReaderExtension->SerialConfigData.WaitMask = FALSE;
            }
        }
        else
        {
            //
            // Card is removed
            //

            SmartcardExtension->ReaderCapabilities.CurrentState =
                SCARD_ABSENT;

            SmartcardExtension->CardCapabilities.Protocol.Selected =
                SCARD_PROTOCOL_UNDEFINED;

            SmartcardDebug(
                DEBUG_DRIVER,
                ("%s!TLP3SerialEvent: Smart card removed COM%x\n",
                TLP3SCR_NAME,
                SmartcardExtension->ReaderCapabilities.Channel)
                );

            if (SmartcardExtension->ReaderExtension->SerialConfigData.WaitMask == TRUE)
            {
                SmartcardExtension->ReaderExtension->SerialConfigData.WaitMask = FALSE;
            }
        }

        if ((SmartcardExtension->OsData->NotificationOverlappedData != NULL) &&
            (SmartcardExtension->ReaderExtension->PowerRequest != TRUE))
        {
            SmartcardCompleteCardTracking(SmartcardExtension);
        }

        //
        // We also look if the DSR still at the high level
        //
        if (ModemStatus & MS_DSR_ON)
        {
            SmartcardDebug(
                DEBUG_DRIVER,
                ("%s!TLP3SerialEvent: DSR is ON in a EV_CTS\n",
                TLP3SCR_NAME)
                );
        }
        else
        {
            SmartcardDebug(
                DEBUG_DRIVER,
                ("%s!TLP3SerialEvent: DSR is OFF in a EV_CTS\n",
                TLP3SCR_NAME)
                );

            TLP3DeleteDevice(DeviceNo);
        }
    }
    else if (lSubEvent ==  EV_DSR && lEvent == CN_EVENT)
    {
        //
        // check if the event we receive is a reader removed event
        //

        //
        // The Reader has been removed
        //
        VCOMM_GetModemStatus( hPort, &ModemStatus);

        // Read and clear event mask for any change of state of DSR
        VCOMM_GetCommEventMask(hPort, EV_DSR, &events);

        SmartcardDebug(
            DEBUG_DRIVER,
            ("%s!TLP3SerialEvent: Reader has been removed\n",
            TLP3SCR_NAME)
            );

        //
        // We also look if the DSR still at the higth level
        //
        if (ModemStatus & MS_DSR_ON)
        {
            SmartcardDebug(
                DEBUG_DRIVER,
                ("%s!TLP3SerialEvent: DSR is ON in a EV_DSR\n",
                TLP3SCR_NAME)
                );
        }
        else
        {
            SmartcardDebug(
                DEBUG_DRIVER,
                ("%s!TLP3SerialEvent: DSR is OFF in a EV_DSR\n",
                TLP3SCR_NAME)
                );

            TLP3DeleteDevice(DeviceNo);
        }

    }
    else if (lSubEvent ==  (EV_DSR|EV_CTS) && lEvent == CN_EVENT)
    {
        //
        // check if the events we receive iare a card presence event
        // and a reader removed event.
        //

        SmartcardExtension->CardCapabilities.ATR.Length = 0;

        // Read and clear event mask for any change of state of CTS and EV_DSR
        VCOMM_GetCommEventMask(hPort, EV_DSR|EV_CTS, &events);
        VCOMM_GetModemStatus( hPort, &ModemStatus);

        if (ModemStatus & MS_CTS_ON)
        {
            //
            // Card is inserted and the Reader has been removed
            //

            SmartcardExtension->ReaderCapabilities.CurrentState =
                SCARD_SWALLOWED;

            SmartcardExtension->CardCapabilities.Protocol.Selected =
                SCARD_PROTOCOL_UNDEFINED;

            if (SmartcardExtension->ReaderExtension->SerialConfigData.WaitMask == TRUE)
            {
                SmartcardExtension->ReaderExtension->SerialConfigData.WaitMask = FALSE;
            }

        }
        else
        {
            //
            // Card is removed and the Reader has been removed
            //

            SmartcardExtension->ReaderCapabilities.CurrentState =
                SCARD_ABSENT;

            SmartcardExtension->CardCapabilities.Protocol.Selected =
                SCARD_PROTOCOL_UNDEFINED;

            if (SmartcardExtension->ReaderExtension->SerialConfigData.WaitMask == TRUE)
            {
                SmartcardExtension->ReaderExtension->SerialConfigData.WaitMask = FALSE;
            }
        }

        if ((SmartcardExtension->OsData->NotificationOverlappedData != NULL) &&
            (SmartcardExtension->ReaderExtension->PowerRequest != TRUE))
        {
            SmartcardCompleteCardTracking(SmartcardExtension);
        }

        //
        // We also look if the DSR still at the high level
        //
        if (ModemStatus & MS_DSR_ON)
        {
            SmartcardDebug(
                DEBUG_DRIVER,
                ("%s!TLP3SerialEvent: DSR is ON in a EV_DSR|EV_CTS\n",
                TLP3SCR_NAME)
                );
        }
        else
        {
            SmartcardDebug(
                DEBUG_DRIVER,
                ("%s!TLP3SerialEvent: DSR is OFF in a EV_DSR|EV_CTS\n",
                TLP3SCR_NAME)
                );

            TLP3DeleteDevice(DeviceNo);
        }
    }

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3SerialEvent: Exit\n",
        TLP3SCR_NAME)
        );
}

BOOL
_cdecl
TLP3ConfigureSerialPort(
    PSMARTCARD_EXTENSION SmartcardExtension
    )

/*++

TLP3ConfigureSerialPort

    This routine will appropriately configure the serial port.

Arguments:

    SmartcardExtension      Pointer to smart card struct

Return Value:

    NTSTATUS

--*/

{
    DWORD actionmask = 0;
    BOOL bRet = FALSE;
    DWORD indata = 0;
    DWORD outdata = 0;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3ConfigureSerialPort: Enter\n",
        TLP3SCR_NAME)
        );

    //
    // initialize the COMM timeout structure and set the timeouts
    //
    bRet  = VCOMM_GetSetCommTimeouts(
        SmartcardExtension->ReaderExtension->hCommPort,
        &SmartcardExtension->ReaderExtension->SerialConfigData.CommTimeouts,
        OPENEXCLUSIVE
        );

    ASSERT(bRet != FALSE);

    if (bRet)
    {
        actionmask =
            fBaudRate |
            fByteSize |
            fParity |
            fStopBits |
            fBitMask |
            fErrorChar;

        bRet = VCOMM_SetCommState(
            SmartcardExtension->ReaderExtension->hCommPort,
            &SmartcardExtension->ReaderExtension->SerialConfigData.theDCB,
            actionmask);

        ASSERT(bRet != FALSE);
    }

    if (bRet)
    {
        // set DTR for the reader
        bRet = VCOMM_EscapeCommFunction(
            SmartcardExtension->ReaderExtension->hCommPort,
            SETDTR,
            indata,
            (long)&outdata
            );

        ASSERT(bRet != FALSE);
    }

    if (bRet)
    {
        // set RTS for the reader
        bRet = VCOMM_EscapeCommFunction(
            SmartcardExtension->ReaderExtension->hCommPort,
            SETRTS,
            indata,
            (long)&outdata
            );

        ASSERT(bRet != FALSE);
    }

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3ConfigureSerialPort: Exit\n",
        TLP3SCR_NAME)
        );

    return bRet;
}

VOID
_stdcall
TLP3StopSerialEventTracking(
    ULONG DeviceNo
)
/*++
TLP3StopSerialEventTracking:
    Disable the card tracking.

Arguments:
    DeviceNo  index to DriverData array

Return Value:
    void

--*/
{
    DWORD dwdummy;
    HPORT hCommPort =  DriverData[DeviceNo].PSmartcardExtension->ReaderExtension->hCommPort;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3StopSerialEventTracking: Enter\n",
        TLP3SCR_NAME)
        );

    // Disable Notification
    VCOMM_EnableCommNotification(
        hCommPort,
        (void *)NULL,
        (LONG)NULL
        );

    // reset the eventmask for EV_CTS and EV_DSR
    VCOMM_GetCommEventMask(
        hCommPort,
        EV_CTS|EV_DSR,
        &dwdummy
        );

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3StopSerialEventTracking: Exit\n",
        TLP3SCR_NAME)
        );
}

void
_cdecl
TLP3_SignalSemaphore (
    void
    )
/*++

Routine Description:

  This function is called after a timer delay was over
  It signals the waiting semaphore that the timeout is over

Arguments:

Return Value:

--*/
{
    PSMARTCARD_EXTENSION pSmartcardExtension;

    //
    // the edx register contains the pointer to the smart card extension
    //
    _asm mov pSmartcardExtension, edx

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3SignalSemaphore: Enter\n",
        TLP3SCR_NAME)
        );

    ASSERT(pSmartcardExtension->ReaderExtension->hDelaySemaphore != 0);

    if(pSmartcardExtension->ReaderExtension->hDelaySemaphore != 0)
    {
        Signal_Semaphore(
            pSmartcardExtension->ReaderExtension->hDelaySemaphore
            );
    }

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3SignalSemaphore: Exit\n",
        TLP3SCR_NAME)
        );

}

BOOL
TLP3Delay(
    PSMARTCARD_EXTENSION pSmartcardExtension,
    ULONG ulMillisecTimeout
    )
/*++

Routine Description:

  This function permits to wait a the specified timeout

Arguments:

  ulMillisecTimeout     millisecondes to wait


Return Value:

--*/
{
    HTIMEOUT htimer = 0;

    //Create Semaphore
    pSmartcardExtension->ReaderExtension->hDelaySemaphore =
        Create_Semaphore(0);

    ASSERT(pSmartcardExtension->ReaderExtension->hDelaySemaphore != 0);

    if(pSmartcardExtension->ReaderExtension->hDelaySemaphore <= 0) {
     	
        return  FALSE;
    }

    //
    // Set of a global timeout. After the timeout end, Signal_Semaphore_To_Be_Abble_To_Read
    // will be called. This function will release the Semaphore
    //
    htimer = Set_Global_Time_Out (
        TLP3_SignalSemaphore,
        ulMillisecTimeout,
        (ULONG)pSmartcardExtension
        );

    ASSERT(htimer != 0);

    if( htimer == 0 )
    {
        Destroy_Semaphore(
            pSmartcardExtension->ReaderExtension->hDelaySemaphore
            );

        return FALSE;
    }

    // Wait the Semaphore release
    Wait_Semaphore (
        pSmartcardExtension->ReaderExtension->hDelaySemaphore,
        BLOCK_SVC_INTS |
        BLOCK_ENABLE_INTS |
        BLOCK_THREAD_IDLE
        );

    Destroy_Semaphore(
        pSmartcardExtension->ReaderExtension->hDelaySemaphore//*phWaitReadDataSemaphore
        );

    return TRUE;
}

NTSTATUS
TLP3StopDevice(
    DEVNODE DeviceNode
    )
/*++
TLP3StopDevice:
    Unload a device

Arguments:
    DeviceNode  device node of the device to stop

Return Value:
    return value of the smclib

--*/
{
    NTSTATUS NTStatus = STATUS_SUCCESS;
    ULONG DeviceNo = 0;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3StopDevice: Enter\n",
        TLP3SCR_NAME)
        );

    //
    //  identify device by devnode
    //
    do {
        if( DriverData[ DeviceNo ].DeviceNode == DeviceNode )
        {
            TLP3DeleteDevice(DeviceNo);
            break;
        }
    } while( ++DeviceNo < TLP3SCR_MAX_DEVICE );

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3StopDevice: Exit\n",
        TLP3SCR_NAME)
        );


    return( NTStatus );
}

VOID
_stdcall
TLP3DeleteDevice(
    ULONG DeviceNo
)
/*++
TLP3DeleteDevice:
    free all resources, links etc for a given DeviceNo.

Arguments:
    DeviceNo  index to DriverData array

Return Value:
    void

--*/
{
    PDEVICE_DATA DeviceData;
    PSMARTCARD_EXTENSION pSmartcardExtension = DriverData[DeviceNo].PSmartcardExtension;
    BOOL bRet = FALSE;
    NTSTATUS status = STATUS_DEVICE_DATA_ERROR;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3DeleteDevice: Enter\n",
        TLP3SCR_NAME)
        );

    if (DeviceNo != 0)
    {
        VxD_DeleteDevice( DriverData[DeviceNo].DDB );
    }

    //
    // Tell the serial driver that we don't need the
    // call back (IoCompletion) anymore
    //
    pSmartcardExtension->ReaderExtension->SerialConfigData.WaitMask = 0;

    // Disable the card tracking.
    TLP3StopSerialEventTracking(DeviceNo);

    // Close the Commport
    VCOMM_CloseComm(
        pSmartcardExtension->ReaderExtension->hCommPort
        );

    //  delete device name 
    if( pSmartcardExtension->ReaderExtension->LinkName[0] != '\0' )
    {
        SmartcardDeleteLink (pSmartcardExtension->ReaderExtension->LinkName);
    }

    if( pSmartcardExtension->OsData != NULL )
    {
        //  complete pending tracking requests
        if( pSmartcardExtension->OsData->NotificationOverlappedData != NULL )
        {
            SmartcardCompleteCardTracking( pSmartcardExtension );
        }
        SmartcardExit( pSmartcardExtension );
    }

    // Free Memory
    if (pSmartcardExtension) {

        _HeapFree(pSmartcardExtension, 0);
    }

    if (pSmartcardExtension->ReaderExtension) {

        _HeapFree(pSmartcardExtension->ReaderExtension, 0);
    }

    //  zero the DriverData entry to mark it unused
    SysFillMemory((PUCHAR) &DriverData[ DeviceNo ], 0, sizeof( DEVICE_DATA ));

    NumDevices--;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3DeleteDevice: Exit\n",
        TLP3SCR_NAME)
        );

    return;
}

ULONG
_stdcall
TLP3DeviceIOControl(
    DWORD           Service,
    DWORD           DDB,
    DWORD           DeviceHandle,
    PDIOCPARAMETERS IOCTLParameter
    )
/*++

TLP3DeviceIOControl:

    Device IO Control dispatch function. The called device is identified by
    the Internal2 member of DIOCPARAMETERS, which contains the DDB address
    of the requested device

Arguments:

    Service
    DDB             device data block  of the requested driver
    DeviceHandle    handle to the device
    IOCTLParameter  ioctl parameter structure

Return Value:
    return value of the smclib

--*/
{
    NTSTATUS NTStatus = STATUS_INVALID_DEVICE_REQUEST;
    DWORD IOStatus = NO_ERROR, Device = 0;
    PDEVICE_DATA DeviceData = &DriverData[0];

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3DeviceIOControl: Enter Fct: %X  Device: %X\n",
        TLP3SCR_NAME,
        IOCTLParameter->dwIoControlCode,
        IOCTLParameter->Internal2)
        );

    //
    //  identify slot by DDB (internal parameter 2 of ioctl control structure)
    //
    do
    {
        if ( DriverData[Device].DDB == (PVMMDDB) DDB )
        {
            DeviceData = &DriverData[ Device ];
            break;
        }
    }
    while ( ++Device < NumDevices );

    //
    //  device found?
    //
    if( DeviceData != NULL )
    {
        PSMARTCARD_EXTENSION    pSmartcardExtension;
        pSmartcardExtension = DeviceData->PSmartcardExtension;

        switch( IOCTLParameter->dwIoControlCode )
        {
            case IOCTL_OPEN:

                SmartcardDebug(
                    DEBUG_IOCTL,
                    ("%s!TLP3DeviceIOControl: IOCTL_OPEN\n",
                    TLP3SCR_NAME)
                    );

                NTStatus = STATUS_SUCCESS;

                if( DeviceData->OpenFlag == FALSE )
                {
                    DeviceData->OpenFlag = TRUE;
                    NTStatus = STATUS_SUCCESS;
                }
                else
                    NTStatus = STATUS_INVALID_DEVICE_REQUEST;
                break;

            case IOCTL_CLOSE:

                SmartcardDebug(
                    DEBUG_IOCTL,
                    ("%s!TLP3DeviceIOControl: IOCTL_CLOSE\n",
                    TLP3SCR_NAME)
                    );

                 //
                // We verify if there is a device present before
                // Complete Card Tracking and put the OpenFlag to FALSE
                //
                if (NumDevices != 0)
                {
                    if (pSmartcardExtension->OsData->NotificationOverlappedData != NULL)
                    {
                    SmartcardCompleteCardTracking(pSmartcardExtension);
                    }

                    NTStatus = STATUS_SUCCESS;
                    DeviceData->OpenFlag = FALSE;
                }
                else
                {
                    NTStatus = STATUS_UNSUCCESSFUL;
                }
                break;


            default:

                // smclib processes all other requests
                SmartcardDebug(
                    DEBUG_IOCTL,
                    ("%s!TLP3DeviceIOControl: transferring to smclib\n",
                    TLP3SCR_NAME)
                    );

                //
                // We verify if there is a device present before
                // the lib process all other request
                //
                if (NumDevices != 0)
                {
                    NTStatus = SmartcardDeviceControl(
                        pSmartcardExtension,
                        IOCTLParameter 
                        );
                }
                else
                {
                    NTStatus = STATUS_UNSUCCESSFUL;
                }
                break;
        }
    }


    //
    //  map to win95 status
    //
    IOStatus = MapNtStatusToWinError( NTStatus );

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3DeviceIOControl: Exit  IOStatus = %X\n",
        TLP3SCR_NAME,
        IOStatus)
        );

    return IOStatus;
}
