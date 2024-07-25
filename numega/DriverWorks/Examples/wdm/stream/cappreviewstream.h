// VidcapPreviewStream - include file for preview streams of Video Capture sample
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

#ifndef __CAPPREVIEWSTREAM_H__
#define __CAPPREVIEWSTREAM_H__

#include "basestrm.h"

class VidcapPreviewStream : public VidcapBaseStream
{
public:

	VidcapPreviewStream(void);

	// -- stream initialization --
	virtual void InitializeStream();

protected:

	// -- methods called from DispatchData method for hardware read/write --
	virtual void OnRead(PHW_STREAM_REQUEST_BLOCK pSrb);

	// -- video format definitions --
	const static  KS_DATARANGE_VIDEO_PALETTE	m_StreamFormatRGB8Bpp; 
	const static  KS_DATARANGE_VIDEO			m_StreamFormatRGB24Bpp_Preview; 

	// -- format array --
	const static PKSDATAFORMAT				m_StreamFormats[];

	// -- stream information passed to base class contructor! --
	const static HW_STREAM_INFORMATION		m_StreamInfo;
};

#endif
