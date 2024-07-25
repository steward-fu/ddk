// CamMiniDriver.h: interface for the CamMiniDriver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMMINIDRIVER_H__43407E9F_0E89_4E15_9023_398458765016__INCLUDED_)
#define AFX_CAMMINIDRIVER_H__43407E9F_0E89_4E15_9023_398458765016__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CamMiniDriver : public KsMiniDriver  
{
public:
	SAFE_DESTRUCTORS

	CamMiniDriver(){}
	virtual ~CamMiniDriver(){}

	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
};


#endif // !defined(AFX_CAMMINIDRIVER_H__43407E9F_0E89_4E15_9023_398458765016__INCLUDED_)
