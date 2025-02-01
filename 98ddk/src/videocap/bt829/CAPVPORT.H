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

#ifndef __CAPVPORT_H
#define __CAPVPORT_H

DEFINE_GUID( DDVPTYPE_BROOKTREE,     0x1352A560L,0xDA61,0x11CF,0x9B,0x06,0x00,0xA0,0xC9,0x03,0xA3,0xB8);

/*
 * When this is set by the driver and passed to the client, this
 * indicates that the video port is capable of treating even fields
 * like odd fields and visa versa.  When this is set by the client,
 * this indicates that the video port should treat even fields like odd
 * fields.
 */
#define DDVPCONNECT_INVERTPOLARITY      0x00000004l

// derived from "fourcc.h"

#define MAKE_FOURCC( ch0, ch1, ch2, ch3 )                       \
        ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
        ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

#define FOURCC_YUV422   MAKE_FOURCC('S','4','2','2')
#define FOURCC_VBID	    MAKE_FOURCC('V','B','I','D')
#define FOURCC_YUY2     MAKE_FOURCC('Y','U','Y','2')
#define FOURCC_UYVY     MAKE_FOURCC('U','Y','V','Y')
#define FOURCC_YV12     MAKE_FOURCC('Y','V','1','2')
#define FOURCC_YUV12    FOURCC_YV12
#define FOURCC_Y12G     MAKE_FOURCC('Y','1','2','G')
#define FOURCC_YV10     MAKE_FOURCC('Y','V','1','0')
#define FOURCC_YUV10    FOURCC_YV10
#define FOURCC_YVU9     MAKE_FOURCC('Y','V','U','9')
#define FOURCC_IF09     MAKE_FOURCC('I','F','0','9')
#define FOURCC_Y10F     MAKE_FOURCC('Y','1','0','F')
#define FOURCC_Y12F     MAKE_FOURCC('Y','1','2','F')
#define FOURCC_YVUM     MAKE_FOURCC('Y','V','U','M')

#endif // __CAPVPORT_H