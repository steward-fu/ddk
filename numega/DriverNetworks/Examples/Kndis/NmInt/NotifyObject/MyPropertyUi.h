// MyPropertyUi.h: interface for the CMyPropertyUi class.
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================


#if !defined(AFX_MYPROPERTYUI_H__ECC5A9C9_38B2_11D5_B069_00C04F6A5DA9__INCLUDED_)
#define AFX_MYPROPERTYUI_H__ECC5A9C9_38B2_11D5_B069_00C04F6A5DA9__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMyPropertyUi : public CPropertyUi  
{
public:
// Constructors/Destructors
	CMyPropertyUi(CNetCfgComponent& Comp);
	virtual ~CMyPropertyUi();

// Notification handlers - delegated by CNotifyObject<>
	virtual HRESULT MergePropPages(DWORD*,BYTE**,UINT*,HWND,LPCWSTR*);
	virtual HRESULT ApplyRegistryChanges(void);
	virtual HRESULT ApplyProperties(void);

protected:
// Helper Methods for registry access
	void ReadRegistryParameters(void);
	bool WriteRegistryParameters(void);

// Data Members
protected:

	// Property Page Window to display in the Network Control Panel
	CMyPropertyPage m_MyPage;

	// Configuration properties loaded/saved from/to registry
	wstring m_wsBundleID;
	wstring m_wsParam1;

	// Flags to indicate if the parameter was changed by the user
	bool m_fBundleIdChanged;
	bool m_fParam1Changed;
};


#endif // !defined(AFX_MYPROPERTYUI_H__ECC5A9C9_38B2_11D5_B069_00C04F6A5DA9__INCLUDED_)
