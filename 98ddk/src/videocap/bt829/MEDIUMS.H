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

//
// This file defines interconnections between components via Mediums
//

#ifndef __MEDIUMS_H__
#define __MEDIUMS_H__

/*
-----------------------------------------------------------

    Topology of all devices:

                            PinDir  FilterPin#
    Crossbar
        CompositeIn         in          0
        TunerIn             in          1
        SVideo              in          2
        Decoder             out         3
        
-----------------------------------------------------------
*/

// {6001AFE0-39A7-11d1-821D-0000F8300212}
#define STATIC_MEDIUM_ATIXBAR_VIDEOCOMPOUT \
0x6001afe0, 0x39a7, 0x11d1, 0x82, 0x1d, 0x0, 0x0, 0xf8, 0x30, 0x2, 0x12
//  0x6001afe0, 0x39a7, 0x11d1, 0x82, 0x1d, 0x0, 0x0, 0xf8, 0x30, 0x2, 0x12
DEFINE_GUIDSTRUCT("6001AFE0-39A7-11d1-821D-0000F8300212", MEDIUM_ATIXBAR_VIDEOCOMPOUT);
#define MEDIUM_ATIXBAR_VIDEOCOMPOUT DEFINE_GUIDNAMED(MEDIUM_ATIXBAR_VIDEOCOMPOUT)

// {AE8F28C0-3346-11d1-821D-0000F8300212}
#define STATIC_MEDIUM_ATIXBAR_VIDEOTUNEROUT \
    0xae8f28c0, 0x3346, 0x11d1,  0x82, 0x1d, 0x0, 0x0, 0xf8, 0x30, 0x2, 0x12 
DEFINE_GUIDSTRUCT("AE8F28C0-3346-11d1-821D-0000F8300212", MEDIUM_ATIXBAR_VIDEOTUNEROUT);
#define MEDIUM_ATIXBAR_VIDEOTUNEROUT DEFINE_GUIDNAMED(MEDIUM_ATIXBAR_VIDEOTUNEROUT)

// {6001AFE1-39A7-11d1-821D-0000F8300212}
#define STATIC_MEDIUM_ATIXBAR_SVIDEOOUT \
0x6001afe1, 0x39a7, 0x11d1, 0x82, 0x1d, 0x0, 0x0, 0xf8, 0x30, 0x2, 0x12 
DEFINE_GUIDSTRUCT("6001AFE1-39A7-11d1-821D-0000F8300212", MEDIUM_ATIXBAR_SVIDEOOUT);
#define MEDIUM_ATIXBAR_SVIDEOOUT DEFINE_GUIDNAMED(MEDIUM_ATIXBAR_SVIDEOOUT)

// {CEA3DBE0-0A58-11d1-A317-00A0C90C484A}
#define STATIC_MEDIUM_VIDEO_BT829_ANALOGVIDEOIN \
    0xcea3dbe0, 0xa58, 0x11d1, 0xa3, 0x17, 0x0, 0xa0, 0xc9, 0xc, 0x48, 0x4a
DEFINE_GUIDSTRUCT("CEA3DBE0-0A58-11d1-A317-00A0C90C484A", MEDIUM_VIDEO_BT829_ANALOGVIDEOIN);
#define MEDIUM_VIDEO_BT829_ANALOGVIDEOIN DEFINE_GUIDNAMED(MEDIUM_VIDEO_BT829_ANALOGVIDEOIN)

#ifdef DEFINE_MEDIUMS
KSPIN_MEDIUM CrossbarMediums[] = {
    {STATIC_MEDIUM_ATIXBAR_VIDEOCOMPOUT,        0, 0},  // Pin 0
    {STATIC_MEDIUM_ATIXBAR_VIDEOTUNEROUT,       0, 0},  // Pin 1
    {STATIC_MEDIUM_ATIXBAR_SVIDEOOUT,           0, 0},  // Pin 2
    {STATIC_MEDIUM_VIDEO_BT829_ANALOGVIDEOIN,   0, 0},  // Pin 3
};

BOOL CrossbarPinDirection [] = {
    FALSE,                      // Input  Pin 0
    FALSE,                      // Input  Pin 1
    FALSE,                      // Input  Pin 2
    TRUE,                       // Output Pin 3
};

// -----------------------------------------------

KSPIN_MEDIUM CaptureMediums[] = {
    {STATIC_GUID_NULL,                          0, 0},  // Pin 0  Vid Capture
    {STATIC_GUID_NULL,                          0, 0},  // Pin 1  Vid VP
    {STATIC_GUID_NULL,                          0, 0},  // Pin 2  VBI Capture
    {STATIC_GUID_NULL,                          0, 0},  // Pin 3  VBI VP
    {STATIC_MEDIUM_VIDEO_BT829_ANALOGVIDEOIN,   0, 0},  // Pin 4  Analog Video In
};

BOOL CapturePinDirection [] = {
    TRUE,                       // Output Pin 0
    TRUE,                       // Output Pin 1
    TRUE,                       // Output Pin 2
    TRUE,                       // Output Pin 3
    FALSE,                      // Input  Pin 4
};

#else

extern KSPIN_MEDIUM CrossbarMediums[];
extern KSPIN_MEDIUM CaptureMediums[];

extern BOOL CrossbarPinDirection [];
extern BOOL CapturePinDirection [];

#endif

#endif // __MEDIUMS_H__
