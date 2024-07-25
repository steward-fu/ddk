// VidcapOutputStream - include file for output streams of Video Capture sample
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

#include "basestrm.h"

#define NUM_PREVIEW_STREAM_FORMATS 1
#define NUM_PREVIEW_STREAM_PROPERTIES 1


class VidcapOutputStream : public VidcapBaseStream
{
public:

	VidcapOutputStream(void);


	// -- stream initialization code --
	virtual void InitializeStream();

protected:

	// -- methods called from DispatchData method for hardware read/write --
	virtual void OnRead(PHW_STREAM_REQUEST_BLOCK pSrb);

	// -- video format definitions --
	const static  KS_DATARANGE_VIDEO_PALETTE	m_StreamFormatRGB8Bpp; 
	const static  KS_DATARANGE_VIDEO			m_StreamFormatRGB24Bpp_Capture; 

	// -- format array --
	const static PKSDATAFORMAT				m_StreamFormats[];

	// -- stream information passed to base class contructor! --
	const static HW_STREAM_INFORMATION		m_StreamInfo;
};
