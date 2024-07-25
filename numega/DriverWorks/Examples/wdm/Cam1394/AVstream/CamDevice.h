// CamDevice.h: interface for the CamDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMDEVICE_H__81295F82_CAAF_4E74_9900_10775BCF992E__INCLUDED_)
#define AFX_CAMDEVICE_H__81295F82_CAAF_4E74_9900_10775BCF992E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CamDevice : public KsDevice  
{
	SAFE_DESTRUCTORS
public:

	CamDevice(){}
	virtual ~CamDevice(){}

	NTSTATUS OnAdd();
	NTSTATUS OnStart(KIrp I);
	NTSTATUS OnRemove(KIrp I);

protected:

	CamLowerDevice	m_Device;

	friend class CamPin;
	friend class CamFilter;
};

#endif // !defined(AFX_CAMDEVICE_H__81295F82_CAAF_4E74_9900_10775BCF992E__INCLUDED_)
