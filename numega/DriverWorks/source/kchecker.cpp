// kchecker.cpp - DriverWorks/BoundsChecker interface
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

// Global data
namespace BoundsChecker
{
	KDriver*		DriverInstance=NULL;
	BCHKD_INFO 		BchkdInfo={0};
}



//=============================================================================
//
// BoundsChecker::Init
//
// Initializes the interface between BCHKD and DriverWorks.
// This must be called at DriverEntry time.
//
NTSTATUS BoundsChecker::Init(PDRIVER_OBJECT pDriver)
{
	NTSTATUS status;
	ULONG_PTR Information;

	// initialize globals
	RtlZeroMemory( &BchkdInfo, sizeof BCHKD_INFO );

	// init the BchkdInfo struct with the PDRIVER_OBJECT of this driver
	BchkdInfo.ThisDriver = pDriver;

	// init the Bchkd entry point to the local dummy function
	BchkdInfo.EventReportRoutine = DummyReportRoutine;

	// Create a lower device object to send init request to
	KLowerDevice Bchkd(L"\\Device\\Bchkd", FILE_READ_DATA);
	status = Bchkd.ConstructorStatus();
	if ( !NT_SUCCESS(status) )
		return status;

	// Allocate an IRP and initialize it for the call to BCHKD
	KIrp I(KIrp::Allocate(Bchkd.StackRequirement()));
	I.MajorFunction(NEXT) = IRP_MJ_INTERNAL_DEVICE_CONTROL;
	I.MinorFunction(NEXT) = 0;
	I.IoctlCode(NEXT) = ULONG(IOCTL_BCHKD_GET_REPORT_ENTRY);
	I.IoctlOutputBufferSize(NEXT) = sizeof BCHKD_INFO;
	I.UserBuffer() = &BchkdInfo;

	// Send the request
	status = Bchkd.CallWaitComplete(I, TRUE, &Information);

	// release the IRP
	KIrp::Deallocate(I);

	// Test if the call failed or the version is not acceptable
	if ( !NT_SUCCESS(status) || (BchkdInfo.EventReportRoutine == NULL) )
		BchkdInfo.EventReportRoutine = DummyReportRoutine;

	else if ((MIN_ALLOWED_BCHKD_VERSION > BchkdInfo.Version) ||
	         (MAX_ALLOWED_BCHKD_VERSION < BchkdInfo.Version) )
	{
		BchkdInfo.EventReportRoutine = DummyReportRoutine;
		status = STATUS_REVISION_MISMATCH;
	}

	return status;
}
//=============================================================================

#pragma code_seg()

//=============================================================================
//
// BoundsChecker::DummyReportRoutine
//
// The address of this function is written into BchkdInfo if the initialization
// of the interface to BCHKD fails. This way, the calling code does not have to
// be conditionalized on the success of the initialization.
//
VOID __cdecl BoundsChecker::DummyReportRoutine(ULONG EventCode, PDRIVER_OBJECT pDriver,...)
{
	return;
	UNREFERENCED_PARAMETER(EventCode);
	UNREFERENCED_PARAMETER(pDriver);
}
