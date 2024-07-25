// parport.cpp - a parallel port driver
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

// This is a replacement for the system's parport.sys, based on the
// DDK sample. It serves as a contention arbitrator and interrupt
// dispatcher for all parallel port class drivers.

////////////////////////////////////////////////////////////////////////

#define VDW_MAIN
#include "function.h"
#include <vdw.h>
#include <parallel.h>

#include "parport.h"
#include "parlog.h"
#include "pardev.h"

////////////////////////////////////////////////////////////////////////

BOOLEAN MemoryOverlap(
    PHYSICAL_ADDRESS    A,
    ULONG               SpanOfA,
    PHYSICAL_ADDRESS    B,
    ULONG               SpanOfB
    );

////////////////////////////////////////////////////////////////////////
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(ParallelDriver, L"LOADED PARALLEL DRIVER RESOURCES")

////////////////////////////////////////////////////////////////////////
// DriverEntry
//
NTSTATUS ParallelDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
 	CONFIG_LIST ConfigList;
	CONFIG_DATA *pCfgData;

	// generate configuration data list
	GetConfigurationData(ConfigList);

	// walk the list of found ports and generate device object for each
	while ( !ConfigList.IsEmpty() )
	{
		pCfgData = ConfigList.RemoveHead();

		ParallelDevice* pDevice = new (NonPagedPool) ParallelDevice(pCfgData);

		if ( !pDevice || !NT_SUCCESS(pDevice->ConstructorStatus()) )
		{
			KErrorLogEntry::SimpleError(
				STATUS_SUCCESS,
				PAR_INSUFFICIENT_RESOURCES,
				9);

			delete pDevice;
		}

		delete pCfgData;
	}

	// if no devices generated, return error
	if ( !GetDeviceListHead() )
		return STATUS_NO_SUCH_DEVICE;

	KImageSection::UnlockAll();

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
// GetConfigurationData
//
// Runs a query to find all the parallel ports
//
VOID ParallelDriver::GetConfigurationData(CONFIG_LIST& CfgList)
{
	QUERY_LIST	queryList;
	PCONFIG_DATA pCandidate;
	PCONFIG_DATA pAlreadyIn;
	BOOLEAN bOverlap;

	queryList.m_ControllersFound = 
		IoGetConfigurationInformation()->ParallelCount;

	// Generate the list. Creation of the query object invokes callback
	// ConfigQueryCallback for each port.

	KConfigurationQuery 
		ParallelPortQuery(ConfigQueryCallback, &queryList, ParallelController);

	// Now rebuild the list, rejecting overlaps and duplicates.

	while ( !queryList.m_List.IsEmpty() )
	{
		pCandidate = queryList.m_List.RemoveHead();
		bOverlap = FALSE;

		for (pAlreadyIn=CfgList.Head(); pAlreadyIn; pAlreadyIn=CfgList.Next(pAlreadyIn))		
		{

			bOverlap = MemoryOverlap(
				pCandidate->Controller, 
				pCandidate->SpanOfController,
				pAlreadyIn->Controller,
				pAlreadyIn->SpanOfController);

			if (bOverlap)
			{
				KErrorLogEntry::SimpleError(
					STATUS_SUCCESS,
					PAR_USER_OVERRIDE,
					1);

				delete pCandidate->DeviceName;
				delete pCandidate;
				break;
			}
		}
		if ( !bOverlap )
			CfgList.InsertTail(pCandidate);
    }
}

////////////////////////////////////////////////////////////////////////
// ConfigQueryCallback
//
// This is the callback routine called by the system when the
// KConfigurationQuery object is constructed by GetConfigurationData
//
NTSTATUS ParallelDriver::ConfigQueryCallback(
	QUERY_RESULT* bus,
	QUERY_RESULT* controller,
	QUERY_RESULT* peripheral,
	PVOID Context
	)
{
	ASSERT (bus != NULL);
	ASSERT (controller != NULL);

	if (controller->m_ResourceCount == 0)
		return STATUS_SUCCESS;

	// allocate a config list entry for this port

	PCONFIG_DATA pCfgData = new (PagedPool) CONFIG_DATA;
	if ( !pCfgData )
	{
		KErrorLogEntry::SimpleError(
			STATUS_SUCCESS,
			PAR_INSUFFICIENT_RESOURCES,
			2);

		return STATUS_INSUFFICIENT_RESOURCES;
	}	

    RtlZeroMemory(pCfgData, sizeof(CONFIG_DATA));

    pCfgData->InterfaceType = bus->m_type.bus;
    pCfgData->BusNumber = bus->m_number;

    BOOLEAN foundPort=FALSE;
    BOOLEAN foundInterrupt=FALSE;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR partial;

	// Search the resources on this port for a port and an
	// interrupt.

    for (ULONG i = 0; i < controller->m_ResourceCount; i++) 
	{
        partial = &controller->m_Resources[i];

        if (partial->Type == CmResourceTypePort) 
		{
            foundPort = TRUE;

            pCfgData->SpanOfController = partial->u.Port.Length;
            pCfgData->Controller = partial->u.Port.Start;
            pCfgData->AddressSpace = partial->Flags;

        } 
        else if (partial->Type == CmResourceTypeInterrupt) 
        {
            foundInterrupt = TRUE;

            pCfgData->InterruptLevel = partial->u.Interrupt.Level;
            pCfgData->InterruptVector = partial->u.Interrupt.Vector;
            pCfgData->InterruptAffinity = partial->u.Interrupt.Affinity;

            if (partial->Flags & CM_RESOURCE_INTERRUPT_LATCHED)
                pCfgData->InterruptMode = Latched;
            else 
                pCfgData->InterruptMode = LevelSensitive;
        }
    }

    if (!foundPort || !foundInterrupt) 
    {
		KErrorLogEntry::SimpleError(
			STATUS_SUCCESS,
			PAR_NOT_ENOUGH_CONFIG_INFO,
			3
			);

		delete pCfgData;
        return STATUS_SUCCESS;
    }

    QUERY_LIST* QList = (QUERY_LIST*)Context;

	pCfgData->DeviceName = 
		new (PagedPool) KUnitizedName(
			DD_PARALLEL_PORT_BASE_NAME_U,
			QList->m_ControllersFound);

	if ( !pCfgData->DeviceName || !(PWSTR)pCfgData->DeviceName )
	{
		KErrorLogEntry::SimpleError(
			STATUS_SUCCESS,
			PAR_INSUFFICIENT_RESOURCES,
			5);
	
		delete pCfgData;
		return STATUS_SUCCESS;
    }

	// attach the new node to the configuration list

    QList->m_List.InsertTail(pCfgData);
    QList->m_ControllersFound++;

    return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
// MemoryOverlap
//
// Compare two memory ranges for coincidence or overlap
//
BOOLEAN MemoryOverlap(
    PHYSICAL_ADDRESS    A,
    ULONG               SpanOfA,
    PHYSICAL_ADDRESS    B,
    ULONG               SpanOfB
    )
{
    LARGE_INTEGER   a, b;
    LARGE_INTEGER   lower, higher;
    ULONG           lowerSpan;

    a.LowPart = A.LowPart;
    a.HighPart = A.HighPart;
    b.LowPart = B.LowPart;
    b.HighPart = B.HighPart;

    if (a.QuadPart == b.QuadPart) 
        return TRUE;

    if (a.QuadPart > b.QuadPart) 
    {
        higher = a;
        lower = b;
        lowerSpan = SpanOfB;
    } 
    else 
    {
        higher = b;
        lower = a;
        lowerSpan = SpanOfA;
    }

    if (higher.QuadPart - lower.QuadPart >= lowerSpan)
        return FALSE;

    return TRUE;
}

#pragma code_seg()

