// VideoStream.h: interface for the VideoStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOSTREAM_H__2912BFC5_AE98_410B_AC14_61DA26A60A62__INCLUDED_)
#define AFX_VIDEOSTREAM_H__2912BFC5_AE98_410B_AC14_61DA26A60A62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ksmedia.h"
#include "KSubscriber.h"


#define MAX_VMODES               6  // Support at most 6 modes of V_MODE_INQ_0


extern KDebugOnlyTrace t;


class VideoStream : public KStream, public KSubscriberImpl<VideoStream>
{
public:
	VideoStream();
	virtual ~VideoStream();

	// Overrideable methods of base class KStream 
public:
	virtual void InitializeStream();
	virtual NTSTATUS OnOpen(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual NTSTATUS OnClose(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnSetState(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnGetState(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnRead(PHW_STREAM_REQUEST_BLOCK pSrb);

	// Subscriber event handlers

	void OnStartListenComplete(NTSTATUS Status, PVOID) {
		m_StartEvent.Set();
	}

	void OnStopListenComplete(NTSTATUS Status, PVOID) {		
		Device()->CancelAllBuffers();
		m_StopEvent.Set();
	}

	void OnReadFrameComplete(NTSTATUS Status, ULONG ulLen, PVOID Context);

	// Helper methods
	NTSTATUS GetDataIntersection(PHW_STREAM_REQUEST_BLOCK pSrb);
	BOOL CompareGUIDsAndFormatSize(
		PKSDATARANGE DataRange1,
		PKSDATARANGE DataRange2,
		BOOL fCompareFormatSize
		);
	BOOL VerifyFormat(PKSDATAFORMAT pKSDataFormatToVerify);

	void GetConnectionProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	void GetDroppedFramesProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	
	CamLowerDevice* Device(); 

	// Data Members
protected:
	const static HW_STREAM_INFORMATION		m_StreamInfo;
	const static PKSDATAFORMAT				m_StreamFormats[MAX_VMODES];
	const static KS_DATARANGE_VIDEO			m_StreamFormatVideo;

	PKS_VIDEOINFOHEADER		m_pVideoInfoHeader;

	KS_FRAME_INFO	m_FrameInfo;

	KEvent			m_StartEvent;
	KEvent			m_StopEvent;

	// Protects against simultaneous read and state change
	KMutex			m_Mutex;

	ULONGLONG		m_FrameCaptured;
};

#endif // !defined(AFX_VIDEOSTREAM_H__2912BFC5_AE98_410B_AC14_61DA26A60A62__INCLUDED_)
