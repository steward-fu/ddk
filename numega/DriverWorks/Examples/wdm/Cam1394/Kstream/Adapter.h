// Adapter.h: interface for the Stream1394Adapter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADAPTER_H__B9915959_BE27_47FA_A7C9_E59FD897BA8C__INCLUDED_)
#define AFX_ADAPTER_H__B9915959_BE27_47FA_A7C9_E59FD897BA8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Camera.h"
#include "CamLowerDevice.h"
#include "VideoStream.h"
#include "CameraControl.h"
#include "VideoProcAmp.h"


class Stream1394Adapter : public KStreamAdapter  
{
	// Construction/Destruction
public:
	Stream1394Adapter(
		KStreamMinidriver* Driver,
		PORT_CONFIGURATION_INFORMATION* pConfigInfo
		);

	SAFE_DESTRUCTORS

	// Overrideable methods of base class KStreamAdapter 
public:
	virtual KStream* GetStream(ULONG StreamNumber);
	virtual void OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnSetProperty(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnGetDataIntersection(PHW_STREAM_REQUEST_BLOCK pSrb);
	virtual void OnSurpriseRemoval(PHW_STREAM_REQUEST_BLOCK pSrb);


	// Methods
public:
	virtual	void Initialize();

	CamLowerDevice* Device() { return &m_Device; }

	// Data Members
protected:
	VideoStream		m_Video;
	CameraControl	m_CameraControl;
	VideoProcAmp	m_ProcAmp;
	CamLowerDevice	m_Device;

	static const KSTOPOLOGY		AdapterTopology;
	static const KSPROPERTY_SET AdapterPropertyTable[];
	static const GUID			Categories[];
};

#endif // !defined(AFX_ADAPTER_H__B9915959_BE27_47FA_A7C9_E59FD897BA8C__INCLUDED_)
