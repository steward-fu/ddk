//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//	TuneProp.H
//	WDM Tuner MiniDriver. 
//		Philips Tuner. 
//			Tuner Hardware access definitions.
//==========================================================================;

#ifndef _TUNERHDW_H_
#define _TUNERHDW_H_


// Special case for Temic Tuner, Channels 63, 64
#define kTemicControl			0x8E34
#define kAirChannel63			0x32B0
#define kAirChannel64			0x3310

// Upper low and upper mid range band definitions
#define kUpperLowBand			0x0CB0
#define kUpperMidBand			0x1F10
#define kUpperLowBand_PALD		0x0CE4
#define kUpperMidBand_PALD		0x1ED4
#define kUpperLowBand_SECAM		0x09E2
#define kUpperMidBand_SECAM		0x14D2 

// Low, Mid and High band control definitions
#define kLowBand				0x8EA2
#define kMidBand				0x8E94
#define kHighBand				0x8E31
#define kLowBand_SECAM			0x8EA6
#define kMidBand_SECAM			0x8E96
#define kHighBand_SECAM			0x8E36
#define kLowBand_PALBG			0x8EA4
#define kMidBand_PALBG			0x8E94
#define kHighBand_PALBG			0x8E34
#define kLowBand_NTSC_FM		0x8EA0
#define kMidBand_NTSC_FM		0x8E90
#define kHighBand_NTSC_FM		0x8E30


#endif	// _TUNERHDW_H_