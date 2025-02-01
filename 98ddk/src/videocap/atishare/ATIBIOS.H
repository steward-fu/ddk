/*^^*
* File:			ATIBIOS.H
*
* Purpose:		Provide definitions for BIOS structures used in the ATI private
*				interfacese exposed via GPIO Interface
*
* Reference:	Ilya Klebanov
*
* Notes:		This file is provided under strict non-disclosure agreements
*				it is and remains the property of ATI Technologies Inc.
*				Any use of this file or the information it contains to
*				develop products commercial or otherwise must be with the
*				permission of ATI Technologies Inc.
*
* Copyright (C) December 1997, ATI Technologies Inc.
*^^*/

#ifndef _ATIBIOS_H_
#define _ATIBIOS_H_

typedef struct tag_ATI_MULTIMEDIAINFO
{
	UCHAR	MMInfoByte0;
	UCHAR	MMInfoByte1;
	UCHAR	MMInfoByte2;
	UCHAR	MMInfoByte3;
	UCHAR	MMInfoByte4;
	UCHAR	MMInfoByte5;
	UCHAR	MMInfoByte6;
	UCHAR	MMInfoByte7;

} ATI_MULTIMEDIAINFO, * PATI_MULTIMEDIAINFO;



typedef struct tag_ATI_HARDWAREINFO
{
	UCHAR	I2CHardwareMethod;
	UCHAR	ImpactTVSupport;
	UCHAR	VideoPortType;

} ATI_HARDWAREINFO, * PATI_HARDWAREINFO;

// this structure definition left for compatability purposes with MiniVDD checked in
// for Windows98 Beta3. The latest MiniVDD exposes set of Private Interfaces instead
// of copying the information into the Registry.
typedef struct
{
    UINT    uiSize;
    UINT    uiVersion;
    UINT    uiCardNumber;
    UINT    uiBoardRevision;
    UINT    uiTunerType;
    UINT    uiVideoInputConnectorType;
    UINT    uiVideoOutputConnectorType;
    UINT    uiCDInputConnector;
    UINT    uiCDOutputConnector;
    UINT    uiVideoPassThrough;
    UINT    uiVideoDecoderType;
    UINT    uiVideoDecoderCrystals;
    UINT    uiVideoOutCrystalFrequency;
    UINT    uiAudioCircuitType;
    UCHAR   uchATIProdType;
    UCHAR   uchOEM;
    UCHAR   uchOEMVersion;
    UCHAR   uchReserved3;
    UCHAR   uchReserved4;

} CWDDE32BoardIdBuffer, * PCWDDE32BoardIdBuffer;

#endif	// _ATIBIOS_H_
