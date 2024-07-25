// KNdisMedium.h: interface for the KNdisMedium class.
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#if !defined(AFX_KNDISMEDIUM_H__CCD29DA5_2F19_11D3_8F9F_00C04F7445B7__INCLUDED_)
#define AFX_KNDISMEDIUM_H__CCD29DA5_2F19_11D3_8F9F_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KNdisMedium
//
// Represents an array of NDIS medium types. KNdisMedium is a helper class
// to simplify processing the NDIS_MEDIUM[] array passed to NDIS drivers
// by NDIS during driver's initialization.
//
// DriverNetworks framework usually instantiates an instance of KNdisMedium during
// driver initialization and passes a reference to it to miniport (intermediate)
// driver's Initialize() handler (see KNdisMiniAdapter and KNdisFilterAdapter).
// The driver is supposed to examine the medium array and select a medium type
// that it can support for the given adapter.
//
class KNdisMedium
{
	SAFE_DESTRUCTORS
public:
// Constructor simply caches the content
	inline KNdisMedium(PNDIS_MEDIUM MediumArray, UINT MediumArraySize);
	inline KNdisMedium();

// Finds index of the medium of interest
	inline BOOLEAN	Select(NDIS_MEDIUM Medium);

// Retrieves selected index if any
	inline UINT Selected() const;

// Cast to PNDIS_MEDIUM (conversion)
	operator PNDIS_MEDIUM() const { return (PNDIS_MEDIUM) m_Medium; }

// Return selected medium
	NDIS_MEDIUM SelectedMedium() const { return *this[m_Index]; }

// Returns the size in bytes of the medium array
	UINT Size() const { return m_MediumSize; }

// Assignment from an NDIS_MEDIUM array (used by protocols to OpenAdapter)
// (Doesn't work with Ver 5 compilers)
#if _MSC_VER >= 1200
	template<typename T> void operator=(T& arr)
		{ ASSERT(sizeof(arr)<=sizeof(m_Medium));
	      NdisMoveMemory(m_Medium, arr, sizeof(arr));
		  m_MediumSize=sizeof(arr)/sizeof(NDIS_MEDIUM);
		}
#endif

// Return medium type at given index
	NDIS_MEDIUM operator[](UINT ix) const { return (ix<m_MediumSize) ? m_Medium[ix] : NDIS_MEDIUM(-1); }

protected:
	UINT			m_MediumSize;
	UINT			m_Index;
	NDIS_MEDIUM		m_Medium[NdisMediumMax];

public:

// Returns a text string for the specified medium (DBG builds only)
#if DBG
	static const char* Text(NDIS_MEDIUM m)
		{
			static const char* str[] = {
				"NdisMedium802_3",
				"NdisMedium802_5",
				"NdisMediumFddi",
				"NdisMediumWan",
				"NdisMediumLocalTalk",
				"NdisMediumDix",
				"NdisMediumArcnetRaw",
				"NdisMediumArcnet878_2",
				"NdisMediumAtm",
				"NdisMediumWirelessWan",
				"NdisMediumIrda",
				"NdisMediumBpc",
				"NdisMediumCoWan",
				"NdisMedium1394"
			};
			return ((UINT)m<NdisMediumMax) ? str[(UINT)m]:"NdisMedium???";
		};
// Returns a text string for the selected medium (DBG builds only)
		const inline char* Text() const { return Text(SelectedMedium()); }
#else
	static const char* Text(NDIS_MEDIUM) { return ""; }
	const inline char* Text() const { return ""; }
#endif

};


//////////////////////////////////////////////////////////////////////
// Implementation


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	MediumArray          - pointer to medium array (usually provided by NDIS)
// 	MediumArraySize      - size of the array in bytes
// Returns:
// 	none
// Comments:
// 	The constructor simply caches the supplied array for further use
KNdisMedium::KNdisMedium(PNDIS_MEDIUM MediumArray, UINT MediumArraySize) :
	m_MediumSize(MediumArraySize), m_Index(UINT(-1))
{
	NdisMoveMemory(&m_Medium, MediumArray, MediumArraySize*sizeof(NDIS_MEDIUM));
}


///////////////////////////////////////////////////////////////////////
// Constructor (uninitialized)
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Creates an uninitialized array
KNdisMedium::KNdisMedium() :
	m_MediumSize(0), m_Index(UINT(-1))
{
}


///////////////////////////////////////////////////////////////////////
// Select the specified medium type
//
// Parameters:
// 	Medium               - Medium type to select
// Returns:
// 	TRUE if the specified medium was found in the array
// Comments:
// 	NDIS Miniport drivers usually call Select() in adapter's
//	Initialize() handler.
BOOLEAN KNdisMedium::Select(NDIS_MEDIUM Medium)
{
	for (m_Index=0; m_Index<m_MediumSize; m_Index++)
		if (m_Medium[m_Index] == Medium)
			return TRUE;
	return FALSE;
}


///////////////////////////////////////////////////////////////////////
// Get the index of the selected medium type
//
// Parameters:
// 	none
// Returns:
// 	Zero-based index in the medium array for the medium previously selected
//	with Select(). -1 is return when no medium has been yet selected.
// Comments:
// 	none
UINT KNdisMedium::Selected() const
{
	return (m_Index >= m_MediumSize) ? UINT(-1) : m_Index;
}

#endif // !defined(AFX_KNDISMEDIUM_H__CCD29DA5_2F19_11D3_8F9F_00C04F7445B7__INCLUDED_)
