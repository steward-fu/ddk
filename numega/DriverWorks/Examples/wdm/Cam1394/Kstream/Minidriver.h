// Minidriver.h: interface for the Stream1394Minidriver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MINIDRIVER_H__DCC94BB7_7309_4DDC_951F_1A994EED028C__INCLUDED_)
#define AFX_MINIDRIVER_H__DCC94BB7_7309_4DDC_951F_1A994EED028C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Stream1394Minidriver : public KStreamMinidriver  
{
	// Construction/Destruction
public:
	Stream1394Minidriver() : m_pAdapter(NULL){}

	SAFE_DESTRUCTORS

	// Overrideable methods of base class KStreamMinidriver 
public:
	NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);

protected:
	NTSTATUS OnCreateAdapter(
				PORT_CONFIGURATION_INFORMATION* pConfigInfo,
				PVOID AdapterObjectStorage,
				PHW_STREAM_REQUEST_BLOCK pSrb
				);

	// Data Members
protected:
	// pointer to the Adapter object
	Stream1394Adapter*	m_pAdapter;
};

#endif // !defined(AFX_MINIDRIVER_H__DCC94BB7_7309_4DDC_951F_1A994EED028C__INCLUDED_)
