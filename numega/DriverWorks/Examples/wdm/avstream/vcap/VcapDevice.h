// VcapDevice.h: interface for the VcapDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCAPDEVICE_H__AFFA0C3E_D929_4A95_BE4F_C90AB554F49A__INCLUDED_)
#define AFX_VCAPDEVICE_H__AFFA0C3E_D929_4A95_BE4F_C90AB554F49A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// VcapDevice
class VcapDevice : public KsDevice
{
	SAFE_DESTRUCTORS
public:
	VcapDevice() {}
	~VcapDevice() {}


// PNP Handlers Of Interest - none. Basically, it means no dispatch table
// will be registered and no device is supported. This is equivalent to
// simply using KsDevice in the descriptor and not bothering deriving
// the useless class. Deriving from KsDevice, however, gives some flexibility
// to add device support in the future.	

};

//////////////////////////////////////////////////////////////////////////
// Definitions of custom DEVICE properties. 
// Place them under #ifdef KAVSTREAM_MAIN to allow dimension-less array 
// initialization avoiding multiple definitions

#ifdef KAVSTREAM_MAIN
// ...
#endif // KAVSTREAM_MAIN


#endif // !defined(AFX_VCAPDEVICE_H__AFFA0C3E_D929_4A95_BE4F_C90AB554F49A__INCLUDED_)
