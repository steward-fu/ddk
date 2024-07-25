// kfilter.h - include file for class KFilterDevice
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================
#ifndef KFilterDEVICE
#define KFilterDEVICE

class KFilterDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	KFilterDevice(PCWSTR NameOfDeviceToFilter, ULONG DevType, ULONG DevFlags);

	// form 2 has access parameter
	KFilterDevice(PCWSTR NameOfDeviceToFilter, ULONG DevType, ULONG DevFlags,
				  ACCESS_MASK DesiredAccess
				 );

	using KDevice::Attach;

public:
	~KFilterDevice(void);

	NTSTATUS PassThrough(KIrp I, BOOLEAN bSetCompletion=FALSE, PVOID Context=NULL);

	virtual NTSTATUS OnIrpComplete(KIrp I, PVOID Context);

	PFILE_OBJECT pFileObject(void)	{ return m_pFileObject; }

	static NTSTATUS LinkTo(OnIrpComplete)(PDEVICE_OBJECT,PIRP,PVOID);

	DEVMEMBER_DISPATCHERS	// must follow all virtual function declarations

	KLowerDevice* m_TargetDevice;

protected:
	NTSTATUS Attach(
		PCWSTR name,
		KLowerDevice** ppLowerDev,
		BOOLEAN bInheritAlignmentReq,
		ACCESS_MASK DesiredAccess
		);

	PFILE_OBJECT  m_pFileObject;
};

#endif
