// CameraControl.h: interface for the CameraControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMERACONTROL_H__0AFDEFE9_C848_4142_A809_8EFBD02F6FEF__INCLUDED_)
#define AFX_AMERACONTROL_H__0AFDEFE9_C848_4142_A809_8EFBD02F6FEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#endif // !defined(AFX_AMERACONTROL_H__0AFDEFE9_C848_4142_A809_8EFBD02F6FEF__INCLUDED_)
