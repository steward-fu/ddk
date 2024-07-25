// capinstream.h - include file for video input stream of Video Capture sample
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

#ifndef __CAPINSTREAM_H__
#define __CAPINSTREAM_H__


#include "basestrm.h"

class VidcapInputStream : public VidcapBaseStream
{
public:
	VidcapInputStream(void);

	
	// -- stream initialization code --
	virtual void InitializeStream();

protected:

	// -- methods called from DispatchData method for hardware read/write --
	virtual void OnWrite(PHW_STREAM_REQUEST_BLOCK pSrb);

	virtual void OnSetState(PHW_STREAM_REQUEST_BLOCK pSrb);


	const static KS_DATARANGE_ANALOGVIDEO	m_StreamFormatAnalogVideo; 
	const static PKSDATAFORMAT				m_StreamFormats[];
	const static KSPIN_MEDIUM				m_Medium;
	const static HW_STREAM_INFORMATION		m_StreamInfo;
};

#endif

