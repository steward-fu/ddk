// WmiMouseFilter.h - declares structures underlying KWmiDataBlock instances
//                    for the mouse filter
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
//

// The corresponding CIM class definitions are in MouseFilter.mof.
// The GUID definitions are in MouseFilterGuids.h

// 8 byte alignment is required for WMI
#pragma pack(push, 8)

struct MouseActivity
{
	MouseActivity(void) :
		m_nRightClicks(0),
		m_nMiddleClicks(0),
		m_nLeftClicks(0),
		m_TotalXMovement(0),
		m_TotalYMovement(0)
	{ }

	ULONG	m_nRightClicks;
	ULONG	m_nMiddleClicks;
	ULONG	m_nLeftClicks;
	ULONG	m_TotalXMovement;
	ULONG	m_TotalYMovement;
};


struct MouseControl
{
	MouseControl(void) :
		m_XScale(1),
		m_YScale(1),
		m_ReverseXY(0)
	{ }

	LONG	m_XScale;
	LONG	m_YScale;
	BOOLEAN	m_ReverseXY;
};


struct MouseDeviceEvent
{
	MouseDeviceEvent(void) : 
		m_Deltax(0),
		m_Deltay(0)
	{}

	ULONG m_Deltax;
	ULONG m_Deltay;
};


struct MousePdoInfo
{
	KWmiString* m_pPdoName;
	KWmiString* m_pPdoRegistryPath;

	MousePdoInfo(void) { m_pPdoName = m_pPdoRegistryPath = NULL; }
	~MousePdoInfo(void)
	{
		if ( m_pPdoName != NULL ) delete m_pPdoName;
		if ( m_pPdoRegistryPath != NULL ) delete m_pPdoRegistryPath;
	}
	NTSTATUS Initialize(PDEVICE_OBJECT Pdo);
};

#pragma pack(pop)

