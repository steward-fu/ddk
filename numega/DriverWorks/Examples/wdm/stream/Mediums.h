// mediums.h - mediums guid defs for Video Capture sample
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

                          
/*  -----------------------------------------------------------

    Topology of all devices:

                            PinDir  FilterPin#    M_GUID#
    TVTuner                 
        TVTunerVideo        out         0            0
        TVTunerAudio        out         1            1
        TVTunerRadioAudio   out         2            2
    TVAudio
        TVTunerAudio        in          0            1
        TVAudio             out         1            3
    Crossbar
        TVTunerVideo        in          0            0
        TVAudio             in          5            3
        TVTunerRadioAudio   in          6            2
        AnalogVideoIn       out         9            4
        AudioOut            out         10           NULL
    Capture
        AnalogVideoIn       in          0            4
        

All other pins are marked as promiscuous connections via GUID_NULL
------------------------------------------------------------------ */        
        
// Define the GUIDs which will be used to create the Mediums
#define M_GUID0 0x8dad65e0, 0x122b, 0x11d1, 0x90, 0x5f, 0x0, 0x0, 0xc0, 0xcc, 0x16, 0xba
#define M_GUID1 0x8dad65e1, 0x122b, 0x11d1, 0x90, 0x5f, 0x0, 0x0, 0xc0, 0xcc, 0x16, 0xba
#define M_GUID2 0x8dad65e2, 0x122b, 0x11d1, 0x90, 0x5f, 0x0, 0x0, 0xc0, 0xcc, 0x16, 0xba
#define M_GUID3 0x8dad65e3, 0x122b, 0x11d1, 0x90, 0x5f, 0x0, 0x0, 0xc0, 0xcc, 0x16, 0xba
#define M_GUID4 0x8dad65e4, 0x122b, 0x11d1, 0x90, 0x5f, 0x0, 0x0, 0xc0, 0xcc, 0x16, 0xba
#define M_GUID5 0x8dad65e5, 0x122b, 0x11d1, 0x90, 0x5f, 0x0, 0x0, 0xc0, 0xcc, 0x16, 0xba

