// kstdwmi.cpp -  Windows Management Instrumentation standard framework interface
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

#include <vdw.h>

#if _WDM_

KWmiContext<KStdWmiInterface>*& KStdWmiInterface::GetContextFromDevice(PDEVICE_OBJECT pSystemDevice)
{
	KPnpDevice* pPnpDev = static_cast<KPnpDevice*>(KDevicePTR(pSystemDevice));
	return pPnpDev->m_Wmi;
}

PUNICODE_STRING KStdWmiInterface::GetDriverRegistryPath(void)
{
	return KDriver::DriverInstance()->RegistryPath();
}

PDEVICE_OBJECT KStdWmiInterface::GetPdo(PDEVICE_OBJECT Fdo)
{
	KPnpDevice* pPnpDev = static_cast<KPnpDevice*>(KDevicePTR(Fdo));
	return pPnpDev->PDO();
}

PVOID KStdWmiInterface::DefaultMethodObject(PDEVICE_OBJECT pSystemDevice)
{
	return static_cast<KPnpDevice*>(KDevicePTR(pSystemDevice));
}


NTSTATUS KStdWmiInterface::DispatchSystemControl(PDEVICE_OBJECT pSystemDevice, PIRP pIrp)
{
	KIrp I(pIrp);
	SYSCTL_IRP_DISPOSITION Disposition;
	KPnpDevice* pPnpDev = static_cast<KPnpDevice*>(KDevicePTR(pSystemDevice));

	ASSERT (pSystemDevice != NULL);

	NTSTATUS status;
	status = WmiSystemControl(*GetContextFromDevice(pSystemDevice), pSystemDevice, pIrp, &Disposition);

	switch (Disposition)
	{
	case IrpNotWmi:
		ASSERT (FALSE);

	default:
		ASSERT (FALSE);

	case IrpForward:
		I.ForceReuseOfCurrentStackLocationInCalldown();
		return pPnpDev->m_TopOfStackDevice->PnpCall(pPnpDev, I);

	case IrpNotCompleted:
		return I.PnpComplete(status);

	case IrpProcessed:
		pPnpDev->DecrementOutstandingRequestCount();
		return status;
	}
}

#endif // _WDM_
