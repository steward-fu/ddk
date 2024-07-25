// HwcapFilter.cpp: implementation of the HwcapFilter class.
//
//////////////////////////////////////////////////////////////////////

#include	<vdw.h>
#include	<kavstream.h>

#include "Hwcap.h"




///////////////////////////////////////////////////////////////////////
// HwcapFilter::OnCreate
//
// Parameters:
// 	I                    - IRP_MJ_CREATE
// Returns:
// 	NT Status
// Comments:
//  In this pin-centric sample we've got not much to do over here
//  NOTE: Hardware Resources should be acquired no sooner than 
//  on pin's KSSTATE_ACQUIRE.
//
NTSTATUS HwcapFilter::OnCreate(KIrp I) 
{
	g_Trace << "HwcapFilter::OnCreate()" << EOL;

	// TODO: custom initialization

	return STATUS_SUCCESS;
}


