// VideoProcAmp.h: interface for the VideoProcAmp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOPROCAMP_H__8F4968C5_7941_4BAE_95CA_40F4DB8675EA__INCLUDED_)
#define AFX_VIDEOPROCAMP_H__8F4968C5_7941_4BAE_95CA_40F4DB8675EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#endif // !defined(AFX_VIDEOPROCAMP_H__8F4968C5_7941_4BAE_95CA_40F4DB8675EA__INCLUDED_)
