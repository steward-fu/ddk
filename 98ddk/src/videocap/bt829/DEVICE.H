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

#ifndef __DEVICE_H
#define __DEVICE_H

#include "decoder.h"
#include "scaler.h"
#include "xbar.h"
#include "capmain.h"
#include "capdebug.h"

class Device
{
private:
    Device (const Device&);
    Device ();
    NTSTATUS GetRegistryValue(   HANDLE,
                        PWCHAR,
                        ULONG,
                        PWCHAR,
                        ULONG);
    VOID UseRegistryValues(PPORT_CONFIGURATION_INFORMATION);
    UINT GetI2cAddress(PPORT_CONFIGURATION_INFORMATION);
    BOOL stringsEqual(PWCHAR, PWCHAR);
    PHW_DEVICE_EXTENSION pHwDevExt;
    int hue;
    int saturation;
    int contrast;
    int brightness;
    int defaultDecoderWidth;
    int defaultDecoderHeight;
    int VBIEN;
    int VBIFMT;
    BOOL isCodeInDataStream;
    BOOL is16;
    int outputEnablePolarity;
    MRect destRect;
    Connector source;
    RegisterB devRegIDCODE;
    RegField devFieldPART_REV;
    RegField devFieldPART_ID;
    Decoder *decoder;
    Scaler *scaler;
    CrossBar *xbar;

public:
    Device(PPORT_CONFIGURATION_INFORMATION);
    void SaveState();
    void RestoreState();
    void SetRect(MRect &);
    int GetDecoderWidth();
    int GetDecoderHeight();
    int GetDefaultDecoderWidth();
    int GetDefaultDecoderHeight();
    int GetPartID();
    int GetPartRev();
    int VBISurfaceOriginX;
    int VBISurfaceOriginY;
    int VBISurfacePitch;
    int VideoSurfaceOriginX;
    int VideoSurfaceOriginY;
    int VideoSurfacePitch;

    //
    // --------------- decoder functions
    //

    void SoftwareReset(void) 
        {decoder->SoftwareReset();}

    void SetOutputEnabled(BOOL b)
        {decoder->SetOutputEnabled(b);}
    
    BOOL IsOutputEnabled(void)
        {return decoder->IsOutputEnabled();}

    void SetHighOdd(BOOL b)
        {decoder->SetHighOdd(b);}

    BOOL IsHighOdd(void)
        {return decoder->IsHighOdd();}

    ErrorCode SetVideoInput( Connector c)
        {source = c; return decoder->SetVideoInput(c);}

    int GetVideoInput( void )
        {return decoder->GetVideoInput();}

    ErrorCode SetBrightness( int i)
        {brightness = i; return decoder->SetBrightness(i);}

    int GetBrightness( void )
        {return decoder->GetBrightness();}

    ErrorCode SetContrast( int i)
        {contrast = i; return decoder->SetContrast(i);}

    int GetContrast( void )
        {return decoder->GetContrast();}

    ErrorCode SetSaturation( int i)
        {saturation = i; return decoder->SetSaturation(i);}

    int GetSaturation( void )
        {return decoder->GetSaturation();}

    ErrorCode SetHue( int i)
        {hue = i; return decoder->SetHue(i);}

    int GetHue( void )
        {return decoder->GetHue();}

    void Set16BitDataStream( BOOL b)
        {decoder->Set16BitDataStream(b);}
    
    BOOL Is16BitDataStream( void )
        {return decoder->Is16BitDataStream();}

    void SetCodeInsertionEnabled( BOOL b)
        {decoder->SetCodeInsertionEnabled(b);}
    
    BOOL IsCodeInsertionEnabled( void )
        {return decoder->IsCodeInsertionEnabled();}

    BOOL Is525LinesVideo(void)
        {return decoder->Is525LinesVideo();}

    BOOL IsVideoPresent(void)
        {return decoder->IsVideoPresent();}

    BOOL IsDeviceInHLock(void)
        {return decoder->IsDeviceInHLock();}

    void SetOutputEnablePolarity(int i)
        {decoder->SetOutputEnablePolarity(i);}
    
    int GetOutputEnablePolarity( void )
        {return decoder->GetOutputEnablePolarity();}

    //
    // --------------- scaler functions
    //
    void SetVBIEN( BOOL b)
        {scaler->SetVBIEN(b);}
    
    BOOL IsVBIEN( void )
        {return scaler->IsVBIEN();}

    void SetVBIFMT( BOOL b)
        {scaler->SetVBIFMT(b);}
    
    BOOL IsVBIFMT( void )
        {return scaler->IsVBIFMT();}

    //
    // --------------- xbar functions
    //
    BOOL GoodPins( int InPin, int OutPin )
        {return xbar->GoodPins(InPin, OutPin );}

    BOOL TestRoute( int InPin, int OutPin )
        {return xbar->TestRoute(InPin, OutPin);}

    void Route( int OutPin, int InPin )
        {xbar->Route(OutPin, InPin);}

    int GetNoInputs()
        {return xbar->GetNoInputs();}

    int GetNoOutputs()
        {return xbar->GetNoOutputs();}

    ULONG GetPinInfo( int dir, int idx, ULONG &related )
        {return xbar->GetPinInfo(dir, idx, related);}

    int GetRoute( int OutPin )
        {return xbar->GetRoute(OutPin);}

    KSPIN_MEDIUM * GetPinMedium( int dir, int idx)
        {return xbar->GetPinMedium(dir, idx);}

    void Reset();
    ~Device();
};  

#endif /* __DEVICE_H */
