// kstdwmi.h -  Windows Management Instrumentation standard framework interface
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

#ifndef __KSTDWMI_H__
#define __KSTDWMI_H__

#if _WDM_


template <class T> class KWmiContext;

class KStdWmiInterface
{
public:
	static KWmiContext<KStdWmiInterface>*& GetContextFromDevice(PDEVICE_OBJECT pSystemDevice);
	static PUNICODE_STRING GetDriverRegistryPath(void);
	static PDEVICE_OBJECT GetPdo(PDEVICE_OBJECT Fdo);
	static NTSTATUS DispatchSystemControl(PDEVICE_OBJECT pSystemDevice, PIRP pIrp);
	static PVOID DefaultMethodObject(PDEVICE_OBJECT pSystemDevice);
};

#endif // _WDM_
#endif // __KSTDWMI_H__
