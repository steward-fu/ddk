// MyBindingPolicy.h: interface for the CMyBindingPolicy class.
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


#if !defined(AFX_MYBINDINGPOLICY_H__B247FA33_E65D_11D4_B048_00C04F6A5DA9__INCLUDED_)
#define AFX_MYBINDINGPOLICY_H__B247FA33_E65D_11D4_B048_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//=========================================================================
// CMyBindingPolicy		
//
//=========================================================================
class CMyBindingPolicy : public CImBindingPolicy  
{
public:

// Constructors/Destructors

	CMyBindingPolicy(CNetCfgComponent& Protocol, CNetCfg& NetCfg);
	virtual ~CMyBindingPolicy();

// Handlers for Binding Notifications

	virtual bool OnAcceptAdapter(CNetCfgComponent& Adapter);
	virtual bool OnAcceptProtocol(CNetCfgComponent& Protocol);
	virtual bool OnDisableBinding(CNetCfgComponent& Protocol, CNetCfgComponent& Adapter);
	virtual bool OnEnableBinding(CNetCfgComponent& Protocol, CNetCfgComponent& Adapter);
	virtual void OnBindAdapter(CNetCfgComponent& RealMp);
};


#endif // !defined(AFX_MYBINDINGPOLICY_H__B247FA33_E65D_11D4_B048_00C04F6A5DA9__INCLUDED_)
