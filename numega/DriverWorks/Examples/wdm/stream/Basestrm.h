// basestrm.h - base class for streams of Video Capture sample
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

#ifndef __SAMPLE_BASESTRM_H__
#define __SAMPLE_BASESTRM_H__

// -- TVS is the trace object used throughout the vidcap stream class objects
//		it is defined and used if __VIDCAP_TRACEON__ is defined in the compilation
//		unit BaseStrm.cpp --

#ifdef __VIDCAP_TRACEON__
	extern KTrace TVS;
#endif

#include <ksmedia.h>
#include "srblist.h"

class VidcapBaseStream : public KStream
{
public:

	///////////////////////////////////////////////////////////////////////
	//	
	//	BEGIN -- KStream methods overridden 
	//
	///////////////////////////////////////////////////////////////////////

protected:

	virtual void OnGetState(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnSetState(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void DispatchControl(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnProposeDataFormat(PHW_STREAM_REQUEST_BLOCK pSrb);

public:
	virtual NTSTATUS OnOpen(PHW_STREAM_REQUEST_BLOCK Srb);
	virtual void CancelRequest(PHW_STREAM_REQUEST_BLOCK pSrb);

	///////////////////////////////////////////////////////////////////////
	//	
	//	END -- KStream methods overridden 
	//
	///////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	//	
	//	BEGIN -- Our VidCapBaseStream methods 
	//
	///////////////////////////////////////////////////////////////////////
public:

	VidcapBaseStream();

protected:

	// -- our version of InitializeBaseClass called from derived stream classes --
	void InitializeBaseClass(const HW_STREAM_INFORMATION& info, ULONG uStreamHeaderMediaSpecific);

	void Lock();
	void Unlock();

	void QueueSrb(PHW_STREAM_REQUEST_BLOCK pSrb);
	PHW_STREAM_REQUEST_BLOCK DequeueSrb(void);

	void GetConnectionProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	void GetDroppedFramesProperty(PHW_STREAM_REQUEST_BLOCK pSrb);

	// -- gets the video stream connection properites for use in derived classes -- 
	const static KSPROPERTY_ITEM m_VideoStreamConnectionProperties[];
	static PKSPROPERTY_ITEM VideoStreamConnectionProperties()
				{ return (PKSPROPERTY_ITEM)m_VideoStreamConnectionProperties; };

	// -- gets the video stream dropped frames property for use in derived classes --
	const static KSPROPERTY_ITEM m_VideoStreamDroppedFramesProperties[];
	static PKSPROPERTY_ITEM VideoStreamDroppedFramesProperties()
				{ return (PKSPROPERTY_ITEM)m_VideoStreamDroppedFramesProperties; };

	// -- gets the video stream properties for use in derived classes --
	const static KSPROPERTY_SET m_VideoStreamProperties[];
	static PKSPROPERTY_SET VideoStreamProperties()
				{ return (PKSPROPERTY_SET)m_VideoStreamProperties; };
	
	const static ULONG	m_VideoStreamPropertiesCount;
	const static ULONG VideoStreamPropertiesCount()
				{ return m_VideoStreamPropertiesCount; };

	
	// -- used in this sample for generating video frames --
	static void STREAMAPI VideoTimerRoutine(PVOID pContext);

	// -- can be overridden in derived classes to generate video for that class --
	virtual void OnVideoTimer(); 

	typedef enum 
	{
		IMAGE_XFER_NTSC_EIA_100AMP_100SAT,
		IMAGE_XFER_NTSC_EIA_75AMP_100SAT,
		IMAGE_XFER_BLACK,
		IMAGE_XFER_WHITE,
		IMAGE_XFER_GRAY_INCREASING
	} ImageXferCommands;

	void CaptureFrame();
	void ImageSynth (PHW_STREAM_REQUEST_BLOCK pSrb, ImageXferCommands Command);

public:

	void CancelAllRequests(void);
	NTSTATUS GetDataIntersection(PHW_STREAM_REQUEST_BLOCK pSrb);

	BOOL VerifyFormat(PKSDATAFORMAT pKSDataFormatToVerify);

	static BOOL CompareGUIDsAndFormatSize(
		PKSDATARANGE DataRange1,
		PKSDATARANGE DataRange2,
		BOOL fCompareFormatSize
		);

	///////////////////////////////////////////////////////////////////////
	//	
	//	END -- Our VidCapBaseStream methods 
	//
	///////////////////////////////////////////////////////////////////////

protected:

	// Data SRB queue
	KList<IRP>	m_RequestQueue;
	ULONG		m_RequestQueueCount;

	// Control SRB queue
	SrbList 	m_ControlSrbList;

	// Other stuff
	KSpinLock	m_Lock;		// General purpose lock

    PKS_VIDEOINFOHEADER						m_pVideoInfoHeader;   // format (variable size!)
    KS_FRAME_INFO							m_FrameInfo;          // PictureNumber, etc.
    ULONG									m_fDiscontinuity;     // Discontinuity since last valid
    UCHAR									m_LineBuffer[720 * 3];// working buffer (RGB24)

    // Clock 
    REFERENCE_TIME							m_QST_Now;            // KeQuerySystemTime currently
    REFERENCE_TIME							m_QST_NextFrame;      // When to capture the next frame
    REFERENCE_TIME							m_QST_StreamTime;     // Stream time reported by master clock

};


inline void VidcapBaseStream::Lock(void)
{
	m_Lock.Lock();
}

inline void VidcapBaseStream::Unlock(void)
{
	m_Lock.Unlock();
}

#endif // __SAMPLE_BASESTRM_H__

