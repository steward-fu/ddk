//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

#include "defaults.h"
#include "device.h"
#include "mediums.h"

Device::Device(PPORT_CONFIGURATION_INFORMATION ConfigInfo) :
        // Corresponds to KS_DEFAULTs
        hue(128),
        saturation(128),
        contrast(128),
        brightness(128),
        source(ConTuner),
        VBIEN(FALSE),
        VBIFMT(FALSE),

        // Beware of these hardcoded values

        // Now set via registry
        // decoderWidth(DefWidth),
        defaultDecoderHeight(240),

        devRegIDCODE (0x17, RO, (PHW_DEVICE_EXTENSION)ConfigInfo->HwDeviceExtension),
        devFieldPART_REV (devRegIDCODE, 0, 4, RO),
        devFieldPART_ID (devRegIDCODE, 4, 4, RO)
{
    pHwDevExt = (PHW_DEVICE_EXTENSION)ConfigInfo->HwDeviceExtension;
    
    pHwDevExt->chipAddr = GetI2cAddress(ConfigInfo);

    // Now that we have the i2c address, we should be able to talk to 
    // the chip.

    // This is the first attempt to actually use i2c services. The
    // Bt829 should have a PartID of 1110b (0xe).
    pHwDevExt->chipID = GetPartID();
    pHwDevExt->chipRev = GetPartRev();
    if (pHwDevExt->chipID != 0xe)
    {
        return;
    }

    decoder = new Decoder((PHW_DEVICE_EXTENSION)ConfigInfo->HwDeviceExtension);
    scaler = new Scaler((PHW_DEVICE_EXTENSION)ConfigInfo->HwDeviceExtension);
    xbar = new(CrossBar);

    UseRegistryValues(ConfigInfo);

    // BUGBUG. According to Brooktree, 4 is the magic dividing line
    // between 829 and 829a. Apparently, there is an 829b on the
    // horizon, but I don't have the details yet.
    // This is meant to be a kind of fail-safe
    if (pHwDevExt->chipRev < 4) {
        outputEnablePolarity = 0;
    }
 
    if (defaultDecoderWidth != 360 && defaultDecoderWidth != 720)
    {
        DBGERROR(("Unexpected defaultDecoderWidth: %d.\n", defaultDecoderWidth));
        TRAP();
    }

    destRect = MRect(0, 0, defaultDecoderWidth, defaultDecoderHeight);

    RestoreState();

    // by default, outputs will be tri-stated.
    // Transitioning to the run state will
    // enable them
    SetOutputEnabled(FALSE);
}

Device::~Device()
{
    delete decoder;
    delete scaler;
    delete xbar;
}

void Device::SaveState()
{
    // save picture attributes
    hue = GetHue();
    saturation = GetSaturation();
    contrast =  GetContrast();
    brightness = GetBrightness();

    // save video source
    source = (Connector) GetVideoInput();

    // save configuration of data stream to video port
    isCodeInDataStream = IsCodeInsertionEnabled();
    is16 = Is16BitDataStream();
    
    // save VBI related settings
    VBIEN = IsVBIEN();
    VBIFMT = IsVBIFMT();

    // save scaling dimensions
    scaler->GetDigitalWin(destRect);
}

void Device::RestoreState()
{
    Reset();
    
    // (re)initialize image 
    decoder->SetInterlaced(FALSE);
    SetHue(hue);
    SetSaturation(saturation);
    SetContrast(contrast);
    SetBrightness(brightness);

    // (re)initialize video source
    SetVideoInput(source);

    SetOutputEnablePolarity(outputEnablePolarity);

    // (re)initialize corresponding xbar setting.
    // 'source' is one-based; Route expects zero-based
    Route(0, source-1);

    // (re)initialize configuration of data stream to video port
    SetCodeInsertionEnabled(isCodeInDataStream);
    Set16BitDataStream(is16);

    // restore VBI settings
    SetVBIEN(VBIEN);
    SetVBIFMT(VBIFMT);

    // initialize scaling dimensions
    SetRect(destRect);
}

void Device::SetRect(MRect &rect)
{
    destRect = rect;
    scaler->SetAnalogWin(rect);
    scaler->SetDigitalWin(rect);

    // for Debugging
#ifdef DEBUG
    scaler->DumpSomeState();
#endif
}

void Device::Reset()
{
    SoftwareReset();
}

int Device::GetDecoderWidth()
{
    MRect tmpRect;
    scaler->GetDigitalWin(tmpRect);

    return tmpRect.right;
}

int Device::GetDecoderHeight()
{
    MRect tmpRect;
    scaler->GetDigitalWin(tmpRect);

    return tmpRect.bottom;
}

int Device::GetDefaultDecoderWidth()
{
    return defaultDecoderWidth;
}

int Device::GetDefaultDecoderHeight()
{
    return defaultDecoderHeight;
}

int Device::GetPartID( void )
{
  return ((int)devFieldPART_ID);
}

int Device::GetPartRev( void )
{
  return ((int)devFieldPART_REV);
}

NTSTATUS
Device::GetRegistryValue(
                   IN HANDLE Handle,
                   IN PWCHAR KeyNameString,
                   IN ULONG KeyNameStringLength,
                   IN PWCHAR Data,
                   IN ULONG DataLength
)
/*++

Routine Description:

    Reads the specified registry value

Arguments:

    Handle - handle to the registry key
    KeyNameString - value to read
    KeyNameStringLength - length of string
    Data - buffer to read data into
    DataLength - length of data buffer

Return Value:

    NTSTATUS returned as appropriate

--*/
{
    NTSTATUS        Status = STATUS_INSUFFICIENT_RESOURCES;
    UNICODE_STRING  KeyName;
    ULONG           Length;
    PKEY_VALUE_FULL_INFORMATION FullInfo;

    RtlInitUnicodeString(&KeyName, KeyNameString);

    Length = sizeof(KEY_VALUE_FULL_INFORMATION) +
        KeyNameStringLength + DataLength;

    FullInfo = (struct _KEY_VALUE_FULL_INFORMATION *)ExAllocatePool(PagedPool, Length);

    if (FullInfo) {
        Status = ZwQueryValueKey(Handle,
                                 &KeyName,
                                 KeyValueFullInformation,
                                 FullInfo,
                                 Length,
                                 &Length);

        if (NT_SUCCESS(Status)) {

            if (DataLength >= FullInfo->DataLength) {
                RtlCopyMemory(Data, ((PUCHAR) FullInfo) + FullInfo->DataOffset, FullInfo->DataLength);

            } else {

                TRAP();
                Status = STATUS_BUFFER_TOO_SMALL;
            }                   // buffer right length

        }                       // if success
        ExFreePool(FullInfo);

    }                           // if fullinfo
    return Status;

}

#define MAX_REG_STRING_LENGTH  128

UINT
Device::GetI2cAddress(PPORT_CONFIGURATION_INFORMATION ConfigInfo)
/*++

--*/

{
    NTSTATUS        Status;
    HANDLE          handle;
    UINT            addr = 0;

    WCHAR   I2cAddressString[] =        L"I2cAddress";
    WCHAR   buf[MAX_REG_STRING_LENGTH];

    PHW_DEVICE_EXTENSION pHwDevExt =
                        (PHW_DEVICE_EXTENSION)ConfigInfo->HwDeviceExtension;

    ASSERT(KeGetCurrentIrql() <= PASSIVE_LEVEL);

    Status = IoOpenDeviceRegistryKey(ConfigInfo->PhysicalDeviceObject,
                                     PLUGPLAY_REGKEY_DRIVER,
                                     STANDARD_RIGHTS_ALL,
                                     &handle);

    if (NT_SUCCESS(Status)) {
        Status = GetRegistryValue(handle,
                                    I2cAddressString,
                                    sizeof(I2cAddressString),
                                    buf,
                                    sizeof(buf));

        if ((NT_SUCCESS(Status)) && (buf)) {
            if (stringsEqual(buf, L"0x8a"))
                addr = 0x8a;
            else if (stringsEqual(buf, L"0x88"))
                addr = 0x88;
            else {
                TRAP();
            }
        }
        else
        {
            TRAP();
        }

        //
        // close the registry handle.
        //

        ZwClose(handle);

    }                           // status = success
    return addr;
}

VOID
Device::UseRegistryValues(PPORT_CONFIGURATION_INFORMATION ConfigInfo)
/*++

Routine Description:

    Reads all registry values for the device

Arguments:

    PhysicalDeviceObject - pointer to the PDO

Return Value:

     None.

--*/

{
    NTSTATUS        Status;
    HANDLE          handle;

    WCHAR   MUX0String[] =              L"MUX0";
    WCHAR   MUX1String[] =              L"MUX1";
    WCHAR   MUX2String[] =              L"MUX2";
    WCHAR   DataWidthString[] =         L"DataWidth";
    WCHAR   EmbeddedControlString[] =   L"EmbeddedControl";
    WCHAR   OutputEnablePolarityString[] = L"OutputEnablePolarity";
    WCHAR   DefaultDecoderWidthString[] = L"DefaultDecoderWidth";
    WCHAR   DebugLevelString[] =        L"DebugLevel";
    WCHAR   buf[MAX_REG_STRING_LENGTH];

    PHW_DEVICE_EXTENSION pHwDevExt =
                        (PHW_DEVICE_EXTENSION)ConfigInfo->HwDeviceExtension;

    ASSERT(KeGetCurrentIrql() <= PASSIVE_LEVEL);

    Status = IoOpenDeviceRegistryKey(ConfigInfo->PhysicalDeviceObject,
                                     PLUGPLAY_REGKEY_DRIVER,
                                     STANDARD_RIGHTS_ALL,
                                     &handle);

    //
    // now get all of the registry settings for
    // initializing the decoder
    //

     if (NT_SUCCESS(Status)) {
        // =========================
        // Does NOT check that the registry settings "make sense";
        // e.g., that all three inputs aren't set to SVideo.


        // =========================
        // Do MUX0
        // =========================
        Status = GetRegistryValue(handle,
                                    MUX0String,
                                    sizeof(MUX0String),
                                    buf,
                                    sizeof(buf));

        if ((NT_SUCCESS(Status)) && (buf))
        {
            if (stringsEqual(buf, L"svideo"))
                {xbar->InputPins[0] = _XBAR_PIN_DESCRIPTION( KS_PhysConn_Video_SVideo,     -1, &CrossbarMediums[2]);}
            else if (stringsEqual(buf, L"tuner"))
                {xbar->InputPins[0] = _XBAR_PIN_DESCRIPTION( KS_PhysConn_Video_Tuner,      -1, &CrossbarMediums[1]);}
            else if (stringsEqual(buf, L"composite"))
                {xbar->InputPins[0] = _XBAR_PIN_DESCRIPTION( KS_PhysConn_Video_Composite,  -1, &CrossbarMediums[0]);}
            else if (stringsEqual(buf, L"none"))
            {
                TRAP(); 
            }
            else
            {
                TRAP();
            }
        }
        else
        {
            TRAP();
        }


        // =========================
        // Do MUX1
        // =========================
        Status = GetRegistryValue(handle,
                                    MUX1String,
                                    sizeof(MUX1String),
                                    buf,
                                    sizeof(buf));

        if ((NT_SUCCESS(Status)) && (buf))
        {
            if (stringsEqual(buf, L"svideo"))
                {xbar->InputPins[1] = _XBAR_PIN_DESCRIPTION( KS_PhysConn_Video_SVideo,     -1, &CrossbarMediums[2]);}
            else if (stringsEqual(buf, L"tuner"))
                {xbar->InputPins[1] = _XBAR_PIN_DESCRIPTION( KS_PhysConn_Video_Tuner,      -1, &CrossbarMediums[1]);}
            else if (stringsEqual(buf, L"composite"))
                {xbar->InputPins[1] = _XBAR_PIN_DESCRIPTION( KS_PhysConn_Video_Composite,  -1, &CrossbarMediums[0]);}
            else if (stringsEqual(buf, L"none"))
            {
                TRAP();
            }
            else
            {
                TRAP();
            }
        }
        else
        {
            TRAP();
        }


        // =========================
        // Do MUX2
        // =========================
        Status = GetRegistryValue(handle,
                                    MUX2String,
                                    sizeof(MUX2String),
                                    buf,
                                    sizeof(buf));

        if ((NT_SUCCESS(Status)) && (buf))
        {
            if (stringsEqual(buf, L"svideo"))
                {xbar->InputPins[2] = _XBAR_PIN_DESCRIPTION( KS_PhysConn_Video_SVideo,     -1, &CrossbarMediums[2]);}
            else if (stringsEqual(buf, L"tuner"))
                {xbar->InputPins[2] = _XBAR_PIN_DESCRIPTION( KS_PhysConn_Video_Tuner,      -1, &CrossbarMediums[1]);}
            else if (stringsEqual(buf, L"composite"))
                {xbar->InputPins[2] = _XBAR_PIN_DESCRIPTION( KS_PhysConn_Video_Composite,  -1, &CrossbarMediums[0]);}
            else if (stringsEqual(buf, L"none"))
            {
                TRAP();
            }
            else
            {
                TRAP();
            }
        }
        else
        {
            TRAP();
        }



       // =========================
        // 8 or 16 bit data width
        // =========================
        Status = GetRegistryValue(handle,
                                    DataWidthString,
                                    sizeof(DataWidthString),
                                    buf,
                                    sizeof(buf));

        if ((NT_SUCCESS(Status)) && (buf)) {
            if (stringsEqual(buf, L"8"))
                is16 = FALSE;
            else if (stringsEqual(buf, L"16"))
                is16 = TRUE;
            else {
                TRAP();
            }
        }
        else
        {
            TRAP();
        }


        // =========================
        // Control codes embedded in data stream?
        // =========================
        Status = GetRegistryValue(handle,
                                    EmbeddedControlString,
                                    sizeof(EmbeddedControlString),
                                    buf,
                                    sizeof(buf));

        if ((NT_SUCCESS(Status)) && (buf)) {
            if (stringsEqual(buf, L"TRUE"))
                isCodeInDataStream = TRUE;
            else if (stringsEqual(buf, L"FALSE"))
                isCodeInDataStream = FALSE;
            else {
                TRAP();
            }
        }
        else
        {
            TRAP();
        }


        // This is an 829 vs 829a issue. If the registery
        // entry is not there, default to 829 behavior
        
        // =========================
        // Is Pin 85 wired as a logical one or zero?
        // =========================
        Status = GetRegistryValue(handle,
                                    OutputEnablePolarityString,
                                    sizeof(OutputEnablePolarityString),
                                    buf,
                                    sizeof(buf));

        if ((NT_SUCCESS(Status)) && (buf)) {
            if (stringsEqual(buf, L"1"))
                outputEnablePolarity = 1;
            else if (stringsEqual(buf, L"0"))
                outputEnablePolarity = 0;
            else {
                TRAP();
            }
        }
        else
        {
            outputEnablePolarity = 0;
        }

        // This is an AIW vs AIW Pro issue. If the registery
        // entry is not there, default to reasonable figure
        
        Status = GetRegistryValue(handle,
                                    DefaultDecoderWidthString,
                                    sizeof(DefaultDecoderWidthString),
                                    buf,
                                    sizeof(buf));

        if ((NT_SUCCESS(Status)) && (buf)) {
            UNICODE_STRING us;
            
            RtlInitUnicodeString(&us, buf);
            Status = RtlUnicodeStringToInteger(&us, 10, (PULONG) &defaultDecoderWidth);
            
            // failsafe
            if (defaultDecoderWidth == 0)
            {
                defaultDecoderWidth = 360;
            }
        }
        else
        {
            defaultDecoderWidth = 360;
        }


        //
        // close the registry handle.
        //

        ZwClose(handle);

    }                           // status = success
}

BOOL Device::stringsEqual(PWCHAR pwc1, PWCHAR pwc2)
{
    UNICODE_STRING us1, us2;
    RtlInitUnicodeString(&us1, pwc1);
    RtlInitUnicodeString(&us2, pwc2);

    // case INsensitive
    return (RtlEqualUnicodeString(&us1, &us2, TRUE));
}
// ==========================================
