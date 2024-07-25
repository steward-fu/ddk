// Port.h: interface for the CPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PORT_H__82EEF106_36AB_404F_80E6_A1671DA4107B__INCLUDED_)
#define AFX_PORT_H__82EEF106_36AB_404F_80E6_A1671DA4107B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KNdis.h"

///////////////////////////////////////////////////////////////////////
// Forward declarations
class CoIsdnAdapter;
class KNdisConfig;
class CCard;

class CPort  
{
public:
	CPort(CoIsdnAdapter *pAdapter, CCard *pCard);
	~CPort();

    NDIS_STATUS FinalConstruct(KNdisConfig& Config);
    ULONG NumChannels() const;

protected:
    NDIS_STATUS ReadConfiguration(KNdisConfig& Config);
    NDIS_STATUS CreateObjects(KNdisConfig& config);
    
protected:
    // Higher level adapter object
    CoIsdnAdapter *m_pAdapter;
    // Ownning card object
    CCard *m_pCard;

    // Port configuration data
    ULONG m_SwitchType;
    ULONG m_NumChannels;
};

#endif // !defined(AFX_PORT_H__82EEF106_36AB_404F_80E6_A1671DA4107B__INCLUDED_)
