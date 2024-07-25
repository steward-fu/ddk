// capstream.h - include file for stream minidriver sample
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

#ifndef __CAPSTREAM_H__
#define __CAPSTREAM_H__

#define NUMBER_OF_STREAMS 3

#include "srblist.h"
#include "controls.h"
#include "basestrm.h"
#include "capinstream.h"
#include "capoutstream.h"
#include "cappreviewstream.h"

// The TestStreamAdapter object becomes the HW device extension structure.
// This is the structure that will be allocated in HW_INITIALIZATION by the stream class driver
// Any information that is used in processing a device request (as opposed to
// a STREAM based request) should be in this structure.  A pointer to this
// object is passed in all requests to the minidriver. (See
// HW_STREAM_REQUEST_BLOCK in STRMINI.H)	
//

class VideoCaptureAdapter : public KStreamAdapter
{
	virtual SAFE_DESTRUCTORS

public:
	VideoCaptureAdapter(
		KStreamMinidriver* Driver,
		PORT_CONFIGURATION_INFORMATION* pConfigInfo);

	// -- opportunity to initialize member variables 
	//		especially streams --
	virtual void Initialize();

	NTSTATUS Register(void);

	void ChangeChannel(PKS_TVTUNER_CHANGE_INFO pChannelChangeInfo)
	{
		m_ChannelChange.m_Info = *pChannelChangeInfo;
	};

protected:

	virtual KStream* GetStream(ULONG StreamNumber);

	virtual void OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnSetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnChangePowerState(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnGetDataIntersection(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnInitializationComplete(PHW_STREAM_REQUEST_BLOCK pSrb);

protected:
	static const GUID Categories[];
	static const KSTOPOLOGY AdapterTopology;
	static const KSPIN_MEDIUM CaptureMediums[];
	static const BOOL CapturePinDirection [];
	static const KSPROPERTY_SET AdapterPropertyTable[];

private:

	// request queue
    SrbList		            m_SrbList;	

	// streams
	VidcapInputStream		m_Input;
	VidcapOutputStream		m_Capture;
	VidcapPreviewStream		m_Preview;

	// Controls
	Tuner					m_Tuner;
	Crossbar				m_Crossbar;
	AudioControl			m_Audio;
	VideoProcAmp			m_ProcAmp;	
	CameraControl			m_CameraControl;
	AnalogVideoDecoder		m_AVDecoder;
	VideoControl			m_VideoControl;
	ChannelChange			m_ChannelChange;

	// other data
	KSpinLock 				m_Lock;
	DEVICE_POWER_STATE		m_PowerState;
};

struct SRB_EXTENSION 
{
	LIST_ENTRY			m_ListEntry;
};


class VideoCaptureDriver : public KStreamMinidriver 
{
	SAFE_DESTRUCTORS
public:

	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);

protected:

	virtual NTSTATUS OnCreateAdapter(
		PORT_CONFIGURATION_INFORMATION* pConfigInfo,
		PVOID DeviceObjectStorage,
		PHW_STREAM_REQUEST_BLOCK Srb
		);

private:
	VideoCaptureAdapter* m_pAdapter;

};


#endif __CAPSTREAM_H__

