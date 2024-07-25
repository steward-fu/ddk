// controls.h - control classes for Video Capture sample
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

#ifndef __SAMPLE_CONTROLS_H__
#define __SAMPLE_CONTROLS_H__

#include <ksmedia.h>

class Crossbar
{
public:
	Crossbar() : m_InputConnected(-1) {};

	NTSTATUS Register(PDEVICE_OBJECT pdo);

	void GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	void SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);

	LONG m_InputConnected;

	static const ULONG NumProperties();
	static PKSPROPERTY_ITEM PropertyList();

protected:
	// -------------------------------------------------------------------
	// XBar pin definitions
	// -------------------------------------------------------------------

	typedef struct _XBAR_PIN_DESCRIPTION {
		ULONG       PinType;
		ULONG       SynthImageCommand;    // This driver simulates different inputs by synthesizing images
		ULONG       RelatedPinIndex;
		const KSPIN_MEDIUM *Medium;       // Describes hardware connectivity
	} XBAR_PIN_DESCRIPTION, *PXBAR_PIN_DESCRIPTION;

	static XBAR_PIN_DESCRIPTION XBarInputPins[];
	static XBAR_PIN_DESCRIPTION XBarOutputPins[];

	const ULONG NumberOfXBarInputs() const;
	const ULONG NumberOfXBarOutputs() const;
	const ULONG NumberOfXBarPinsTotal() const
	{
		return (NumberOfXBarInputs()+NumberOfXBarOutputs());
	}

	static const KSPIN_MEDIUM CrossbarMediums[];
	static const BOOL CrossbarPinDirection [];
	static const KSPROPERTY_ITEM XBarProperties[];

};

class Tuner
{
public:

	Tuner() : m_Frequency(0),
			  m_VideoStandard(0),
			  m_Quality(0),
			  m_Input(0),
			  m_Busy(0) {};

	NTSTATUS Register(PDEVICE_OBJECT pdo);

	void GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	void SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);

	ULONG m_Frequency;
	ULONG m_VideoStandard;
	ULONG m_Quality;
	ULONG m_Input;
	ULONG m_Busy;

	static const ULONG NumProperties();
	static PKSPROPERTY_ITEM PropertyList();

protected:
	static const KSPIN_MEDIUM TVTunerMediums[];
	static const BOOL TVTunerPinDirection [];
	static const KSPROPERTY_ITEM TVTunerProperties[];

};


class AudioControl
{
public:
	AudioControl() : m_Mode(0) {};

	NTSTATUS Register(PDEVICE_OBJECT pdo);

	void GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	void SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);

	ULONG m_Mode;

	static const ULONG NumProperties();
	static PKSPROPERTY_ITEM PropertyList();

protected:
	static const KSPIN_MEDIUM TVAudioMediums[];
	static const BOOL TVAudioPinDirection [];
	static const KSPROPERTY_ITEM TVAudioProperties[];
	
};

class VideoProcAmp
{
public:

	VideoProcAmp();

	void GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	void SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);

	LONG m_Brightness;	
	LONG m_BrightnessFlags;
	LONG m_Contrast;
	LONG m_ContrastFlags;

	static const ULONG NumProperties();
	static PKSPROPERTY_ITEM PropertyList();

protected:
	static const ULONG BrightnessDefault;
	static const ULONG ContrastDefault;
	static const KSPROPERTY_STEPPING_LONG BrightnessRangeAndStep[]; 
	static const KSPROPERTY_MEMBERSLIST BrightnessMembersList[]; 
	static const KSPROPERTY_VALUES BrightnessValues;
	static const KSPROPERTY_STEPPING_LONG ContrastRangeAndStep[];
	static const KSPROPERTY_MEMBERSLIST ContrastMembersList[];
	static const KSPROPERTY_VALUES ContrastValues;
	static const KSPROPERTY_ITEM VideoProcAmpProperties[];
};

class CameraControl
{
public:
	CameraControl();

	void GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	void SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);

	LONG m_Focus;
	LONG m_FocusFlags;
	LONG m_Zoom;
	LONG m_ZoomFlags;

	static const ULONG NumProperties();
	static PKSPROPERTY_ITEM PropertyList();

protected:
	static const ULONG ZoomDefault;
	static const ULONG FocusDefault;
	static const KSPROPERTY_STEPPING_LONG ZoomRangeAndStep[];
	static const KSPROPERTY_MEMBERSLIST ZoomMembersList[];
	static const KSPROPERTY_VALUES ZoomValues;
	static const KSPROPERTY_STEPPING_LONG FocusRangeAndStep[];
	static const KSPROPERTY_MEMBERSLIST FocusMembersList[];
	static const KSPROPERTY_VALUES FocusValues;
	static const KSPROPERTY_ITEM	CameraControlProperties[];

};

class AnalogVideoDecoder
{
public:
	AnalogVideoDecoder() : m_VideoStandard(KS_AnalogVideo_NTSC_M),
						   m_OutputEnable(FALSE),
						   m_VCRTiming(FALSE) {};


	void GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	void SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);

	LONG m_VideoStandard;
	LONG m_OutputEnable;
	LONG m_VCRTiming;

	static const ULONG NumProperties();
	static PKSPROPERTY_ITEM PropertyList();

protected:
	static const KSPROPERTY_ITEM AnalogVideoDecoderProperties[];

};

class VideoControl
{
public:
	VideoControl() : m_Mode(0) {};
	

	void GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	void SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);

	LONG m_Mode;

	static const ULONG NumProperties();
	static PKSPROPERTY_ITEM PropertyList();

protected:
	static const KSPROPERTY_ITEM VideoControlProperties[];

};

class ChannelChange
{
public:
	ChannelChange() { memset(&m_Info, 0, sizeof(KS_TVTUNER_CHANGE_INFO)); };
	

	void GetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	void SetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);

	KS_TVTUNER_CHANGE_INFO m_Info;
};

#endif // __SAMPLE_CONTROLS_H__
