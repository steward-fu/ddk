// kwmi.cpp - support routines for WMI (DDK emulation)
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

#if _WDM_	// not supported for non-WDM drivers

///////////////////////////////////////////////////////////////////////////////////////
//
//	WmiCompleteRequest
//		Complete a WMI request
//
//	Parameters:
//		DeviceObject
//			The device object to which the IRP was targeted
//		pIrp
//			The request to be completed
//		Status
//			The status of the request
//		BufferUsed
//			Number of bytes supplied by caller to complete the request
//		PriorityBoost
//			Passed throught to IoCompleteRequest
//
//	Return Value:
//		Returns status
//
//	Comments:
//		Emulates the behavior the DDK function.
//
NTSTATUS
   WmiCompleteRequest(
       IN PDEVICE_OBJECT DeviceObject,
       IN PIRP pIrp,
       IN NTSTATUS Status,
       IN ULONG BufferUsed,
       IN CCHAR PriorityBoost
       )
{
	KIrp I(pIrp);
	PIO_STACK_LOCATION pStack = I.CurrentStackLocation();
	UCHAR MinorFunction = pStack->MinorFunction;
	PWNODE_HEADER pWnodeHeader = (PWNODE_HEADER)pStack->Parameters.WMI.Buffer;
	ULONG_PTR Information = 0;

	PWNODE_ALL_DATA pAll;
	ULONG DataOffset;
	int i;
	OFFSETINSTANCEDATAANDLENGTH* pOidal;
	PULONG pLengthArray;
	ULONG InstanceCount;

	switch (MinorFunction)
	{
	case IRP_MN_QUERY_ALL_DATA:

		// set up a pointer for the WNODE_ALL_DATA struct
		pAll = PWNODE_ALL_DATA(pWnodeHeader);

		// extract the offset from the start of the WNODE_ALL_DATA to the data supplied by the caller
		DataOffset = pAll->DataBlockOffset;

		// Information is the total number of bytes used to process the request
		Information = DataOffset + BufferUsed;

		// Validate that the buffer was big enough
		if ( NT_SUCCESS(Status) && (Information > pStack->Parameters.WMI.BufferSize))
		{
			Status = STATUS_BUFFER_TOO_SMALL;
		}

		// If the buffer was too small, set up a WNODE_TOO_SMALL
		if ( Status == STATUS_BUFFER_TOO_SMALL )
		{
			PWNODE_TOO_SMALL(pWnodeHeader)->SizeNeeded = ULONG(Information);
			Information = sizeof(WNODE_TOO_SMALL);
			pWnodeHeader->Flags = WNODE_FLAG_TOO_SMALL;
			Status = STATUS_SUCCESS;
			break;
		}
		// For all other errors, fail and exit
		else if ( !NT_SUCCESS(Status) )
		{
			Information = 0;
			break;
		}

		// Add the timestamp to the WNODE_ALL_DATA
		KeQuerySystemTime(&pWnodeHeader->TimeStamp);

		// Pull out the instance count (number of blocks)
		InstanceCount = pAll->InstanceCount;

		// Set the header information
		pWnodeHeader->BufferSize = ULONG(Information);

		// The caller supplies an array of ULONG, one element for each instance.
		// We have to convert this to an array of OFFSETINSTANCEDATAANDLENGTH,
		// which requires two loops.

		pOidal = pAll->OffsetInstanceDataAndLength;
		pLengthArray = PULONG(pOidal);

		// In the first loop, set the lengths
		for (i = InstanceCount-1;  i >= 0 ; i--)
		{
			pOidal[i].LengthInstanceData = pLengthArray[i];
		}

		// In the second loop, set the offsets
		for (; InstanceCount > 0 ; InstanceCount--, pOidal++)
		{
			pOidal->OffsetInstanceData = DataOffset;
			DataOffset += pOidal->LengthInstanceData;

			// round up to 8-byte boundary
			DataOffset += 7;
			DataOffset &= ~7;
		}

		break;

	case IRP_MN_QUERY_SINGLE_INSTANCE:
	case IRP_MN_EXECUTE_METHOD:

		// First calculate how much buffer was needed
		if (MinorFunction == IRP_MN_QUERY_SINGLE_INSTANCE)
		{
			Information = PWNODE_SINGLE_INSTANCE(pWnodeHeader)->DataBlockOffset + BufferUsed;
			PWNODE_SINGLE_INSTANCE(pWnodeHeader)->SizeDataBlock = BufferUsed;
		}
		else	// IRP_MN_EXECUTE_METHOD
		{
			Information = PWNODE_METHOD_ITEM(pWnodeHeader)->DataBlockOffset + BufferUsed;
			PWNODE_METHOD_ITEM(pWnodeHeader)->SizeDataBlock = BufferUsed;
		}

		// If the request was successful, set the timestamp and the header size count
		if ( NT_SUCCESS(Status) )
		{
			pWnodeHeader->BufferSize = ULONG(Information);
			KeQuerySystemTime(&pWnodeHeader->TimeStamp);
		}
		else // an error occurred
		{
			// If the error was buffer too small, set up a WNODE_TOO_SMALL,
			// and return STATUS_SUCCESS
			if ( Status == STATUS_BUFFER_TOO_SMALL )
			{
				PWNODE_TOO_SMALL(pWnodeHeader)->SizeNeeded = ULONG(Information);
				Information = pWnodeHeader->BufferSize = sizeof(WNODE_TOO_SMALL);
				pWnodeHeader->Flags = WNODE_FLAG_TOO_SMALL;
				Status = STATUS_SUCCESS;
			}
			else // for all other errors, let it pass through
			{
				pWnodeHeader->BufferSize = 0;
				Information = 0;
			}
		}

		break;

	case IRP_MN_REGINFO:
#if (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))
	case IRP_MN_REGINFO_EX:
#endif
		ASSERT(!"IRP_MN_REGINFO must not be passed to WmiCompleteRequest");
		break;

	default:
		// in other cases, nothing to do
		break;

	}	// end switch

	// Go ahead and complete the IRP

	I.Status() = Status;
	I.Information() = Information;
	return I.Complete(Status, PriorityBoost);

	UNREFERENCED_PARAMETER(DeviceObject);
}

///////////////////////////////////////////////////////////////////////////////////////
//
//	WmiSystemControl
//		Dispatch IRP_MJ_SYSTEM_CONTROL
//
//	Parameters:
//		WmiLibContext
//			The driver's context structure
//		DeviceObject
//			The device to which the IRP is targeted
//		pIrp
//			The IRP to dispatch
//		pIrpDisposition
//			Address of variable to receive IRP disposition on return
//
//	Returns
//		Status.
//
//	Comments
//		Emulates DDK function (WMILIB.SYS).
//
NTSTATUS
   WmiSystemControl(
       IN PWMILIB_CONTEXT WmiLibContext,
       IN PDEVICE_OBJECT pDeviceObject,
       IN PIRP pIrp,
       OUT PSYSCTL_IRP_DISPOSITION pIrpDisposition
       )
{
	KIrp I(pIrp);
	PIO_STACK_LOCATION pStack = I.CurrentStackLocation();
	UCHAR MinorFunction = pStack->MinorFunction;

	ULONG GuidIndex = ULONG(-1);
	ULONG ThisInstance = ULONG(-1);
	ULONG ClassInstanceCount = ULONG(-1);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG BufferSize = pStack->Parameters.WMI.BufferSize;
	ULONG DataBlockOffset;
	PWMIREGGUID pRegGuid;

	PWNODE_HEADER pWnodeHeader = PWNODE_HEADER(pStack->Parameters.WMI.Buffer);
	PWNODE_ALL_DATA pAll = PWNODE_ALL_DATA(pWnodeHeader);
	PWNODE_SINGLE_INSTANCE pSingle = PWNODE_SINGLE_INSTANCE(pWnodeHeader);
	PWNODE_SINGLE_ITEM pItem = PWNODE_SINGLE_ITEM(pWnodeHeader);
	PWNODE_METHOD_ITEM pMethod = PWNODE_METHOD_ITEM(pWnodeHeader);
	PWMIREGINFO pRegInfo = PWMIREGINFO(pWnodeHeader);

	// Validate the minor function
#if (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))
	if (MinorFunction != 0xFF && (MinorFunction > IRP_MN_REGINFO_EX || MinorFunction == 0xA))
#else
	if (MinorFunction != 0xFF && MinorFunction > IRP_MN_EXECUTE_METHOD)
#endif
	{
		*pIrpDisposition = IrpNotWmi;
		return STATUS_SUCCESS;
	}

	// Make sure this IRP is for the indicated device and context is not NULL
	if ( (MinorFunction == 0xFF)
#if !defined(VXD_COMPATLIB) && !((WDM_MAJORVERSION ==1) && (WDM_MINORVERSION == 0))
		|| (pStack->Parameters.WMI.ProviderId != IoWMIDeviceObjectToProviderId(pDeviceObject))
#else
		|| (pStack->Parameters.WMI.ProviderId != (ULONG)pDeviceObject)

#endif
		|| (WmiLibContext == NULL) )
	{
		*pIrpDisposition = IrpForward;
		return STATUS_SUCCESS;
	}

	// Assume we will process the IRP
	*pIrpDisposition = IrpProcessed;

#if (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))
	if ( MinorFunction != IRP_MN_REGINFO  && MinorFunction != IRP_MN_REGINFO_EX)
#else
	if ( MinorFunction != IRP_MN_REGINFO )
#endif
	{
		// Look up the IRP's GUID in the array attached to the WMILIB_CONTEXT structure


		PWMIGUIDREGINFO pCurGuidInfo = WmiLibContext->GuidList;
		GUID* GuidToFind = (GUID*)pStack->Parameters.WMI.DataPath;
		ULONG i;

		for (i=0; i < WmiLibContext->GuidCount; i++, pCurGuidInfo++)
		{
			if ( IsEqualGUID(*pCurGuidInfo->Guid, *GuidToFind) )
			{
				GuidIndex = i;
				ClassInstanceCount = pCurGuidInfo->InstanceCount;
				break;
			}
		}

		// If the GUID is not in the context's list, fail and return.

		if ( i >= WmiLibContext->GuidCount )
		{
			*pIrpDisposition = IrpNotCompleted;
			I.Status() = STATUS_WMI_GUID_NOT_FOUND;
			I.Information() = 0;
			return STATUS_WMI_GUID_NOT_FOUND;
		}

		// Some minor functions require that the InstanceIndex be validated

		switch ( MinorFunction )
		{
		case IRP_MN_QUERY_SINGLE_INSTANCE:
		case IRP_MN_CHANGE_SINGLE_INSTANCE:
		case IRP_MN_CHANGE_SINGLE_ITEM:
		case IRP_MN_EXECUTE_METHOD:

			if ( MinorFunction != IRP_MN_EXECUTE_METHOD)
				ThisInstance = PWNODE_SINGLE_INSTANCE(pStack->Parameters.WMI.Buffer)->InstanceIndex;
			else
				ThisInstance = PWNODE_METHOD_ITEM(pStack->Parameters.WMI.Buffer)->InstanceIndex;

			if ( pWnodeHeader->Flags & WNODE_FLAG_STATIC_INSTANCE_NAMES )
			{
				if ( ThisInstance >= ClassInstanceCount)
				{
					status = STATUS_WMI_INSTANCE_NOT_FOUND;
					*pIrpDisposition = IrpNotCompleted;
					I.Information() = 0;
					I.Status() = status;
					return status;
				}
			}

			break;

		default:
			break;

		}
	}

///////////////////////////////////// BEGIN MAIN SWITCH /////////////////////////////////////

	switch ( MinorFunction )
	{
	case IRP_MN_QUERY_ALL_DATA:

		if ( BufferSize < sizeof(WNODE_ALL_DATA) )
		{
			*pIrpDisposition = IrpNotCompleted;
			I.Information() = 0;
			I.Status() = STATUS_UNSUCCESSFUL;
			return status;
		}

		pWnodeHeader->Flags &= ~WNODE_FLAG_FIXED_INSTANCE_SIZE;
		pAll->InstanceCount = ClassInstanceCount;

		DataBlockOffset = FIELD_OFFSET(WNODE_ALL_DATA, OffsetInstanceDataAndLength) +
			ClassInstanceCount*sizeof(OFFSETINSTANCEDATAANDLENGTH);

		pAll->DataBlockOffset = DataBlockOffset;

		if 	(BufferSize <= DataBlockOffset)
		{
			status = WmiLibContext->QueryWmiDataBlock(
				pDeviceObject,
				pIrp,
				GuidIndex,
				0,
				ClassInstanceCount,
				NULL,
				0,
				NULL
				);
		}
		else
		{
			status = WmiLibContext->QueryWmiDataBlock(
				pDeviceObject,
				pIrp,
				GuidIndex,
				0,
				ClassInstanceCount,
				PULONG(pAll->OffsetInstanceDataAndLength),
				BufferSize - DataBlockOffset,
				PUCHAR(pAll) + DataBlockOffset
				);
		}

		// pWnodeHeader->BufferSize is set in WmiCompleteRequest

		break;

	case IRP_MN_QUERY_SINGLE_INSTANCE:

		status = WmiLibContext->QueryWmiDataBlock(
			pDeviceObject,
			pIrp,
			GuidIndex,
			ThisInstance,
			1, 	// instance count
			&pSingle->SizeDataBlock,
			BufferSize - pSingle->DataBlockOffset,
			PUCHAR(pSingle) + pSingle->DataBlockOffset
			);

		// pWnodeHeader->BufferSize is set in WmiCompleteRequest

		break;

	case IRP_MN_CHANGE_SINGLE_INSTANCE:

		if ( WmiLibContext->SetWmiDataBlock == NULL )
		{
			status = STATUS_INVALID_DEVICE_REQUEST;
			*pIrpDisposition = IrpNotCompleted;
			I.Status() = status;
			I.Information() = 0;
		}
		else
		{
			status = WmiLibContext->SetWmiDataBlock(
				pDeviceObject,
				pIrp,
				GuidIndex,
				ThisInstance,
				pSingle->SizeDataBlock,
				PUCHAR(pSingle) + pSingle->DataBlockOffset
				);
		}
		break;

	case IRP_MN_CHANGE_SINGLE_ITEM:

		if ( WmiLibContext->SetWmiDataItem == NULL )
		{
			status = STATUS_INVALID_DEVICE_REQUEST;
			*pIrpDisposition = IrpNotCompleted;
			I.Status() = status;
			I.Information() = 0;
		}
		else
		{
			status = WmiLibContext->SetWmiDataItem(
				pDeviceObject,
				pIrp,
				GuidIndex,
				ThisInstance,
				pItem->ItemId,
				BufferSize - pItem->DataBlockOffset,
				PUCHAR(pItem) + pItem->DataBlockOffset
				);
		}
		break;

	case IRP_MN_ENABLE_EVENTS:
	case IRP_MN_DISABLE_EVENTS:
		if ( WmiLibContext->WmiFunctionControl == NULL )
		{
			status = STATUS_INVALID_DEVICE_REQUEST;
			*pIrpDisposition = IrpNotCompleted;
			I.Status() = status;
			I.Information() = 0;
		}
		else
		{
			status = WmiLibContext->WmiFunctionControl(
				pDeviceObject,
				pIrp,
				GuidIndex,
				WmiEventControl,
				(MinorFunction == IRP_MN_ENABLE_EVENTS)
				);
		}
		break;

	case IRP_MN_ENABLE_COLLECTION:
	case IRP_MN_DISABLE_COLLECTION:
		if ( WmiLibContext->WmiFunctionControl == NULL )
		{
			status = STATUS_INVALID_DEVICE_REQUEST;
			*pIrpDisposition = IrpNotCompleted;
		}
		else
		{
			status = WmiLibContext->WmiFunctionControl(
				pDeviceObject,
				pIrp,
				GuidIndex,
				WmiDataBlockControl,
				(MinorFunction == IRP_MN_ENABLE_COLLECTION)
				);
		}
		break;

	case IRP_MN_EXECUTE_METHOD:
		if ( WmiLibContext->ExecuteWmiMethod == NULL )
		{
			status = STATUS_INVALID_DEVICE_REQUEST;
			*pIrpDisposition = IrpNotCompleted;
		}
		else
		{
			status = WmiLibContext->ExecuteWmiMethod(
				pDeviceObject,
				pIrp,
				GuidIndex,
				ThisInstance,
				pMethod->MethodId,
				pMethod->SizeDataBlock,
				BufferSize - pMethod->DataBlockOffset,
				PUCHAR(pSingle) + pMethod->DataBlockOffset
				);
		}

		break;

	case IRP_MN_REGINFO:
#if (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))
	case IRP_MN_REGINFO_EX:
#endif
		ULONG RegFlags;
		UNICODE_STRING InstanceName;
		PUNICODE_STRING pRegistryPath;
		UNICODE_STRING MofResourceName;
		PDEVICE_OBJECT Pdo;

		ULONG MofOffset;
		ULONG TotalSize;
		ULONG RegistryPathOffset;
		ULONG InstanceNameOffset;
		ULONG GuidCount;

		*pIrpDisposition = IrpNotCompleted;

		InstanceName.Buffer = NULL;
		InstanceName.Length = InstanceName.MaximumLength = 0;
		MofResourceName.Buffer = NULL;
		MofResourceName.Length = MofResourceName.MaximumLength = 0;
		pRegistryPath = NULL;
		Pdo = NULL;

		ULONG i;

		status = WmiLibContext->QueryWmiRegInfo(
			pDeviceObject,
			&RegFlags,
			&InstanceName,
			&pRegistryPath,
			&MofResourceName,
			&Pdo
			);

		pRegInfo->GuidCount = WmiLibContext->GuidCount;

		if ( NT_SUCCESS(status) && (RegFlags & WMIREG_FLAG_INSTANCE_PDO) && (Pdo == NULL) )
		{
			status = STATUS_INVALID_DEVICE_REQUEST;
		}

		if  (!NT_SUCCESS(status) )
		{
			if ( InstanceName.Buffer != NULL )
			{
				ExFreePool(InstanceName.Buffer);
			}

			I.Information() = 0;
			break;
		}

		GuidCount = WmiLibContext->GuidCount;

	// Here is the layout of the IRP
	//
	//		WMIREGINFO
	//		WMIREGGUIDW[]
	//		INSTANCE NAME (if RegFlags & WMIREG_FLAG_INSTANCE_BASENAME)
	//		MOF RESOURCE NAME (if not null)
	//		REGISTRY PATH

		// Calculate InstanceName offset
		InstanceNameOffset = ULONG(ULONG_PTR(&PWMIREGINFO(0)->WmiRegGuid) + sizeof(WMIREGGUID)*GuidCount);

		// Calculate MofResourceName offset
		MofOffset = InstanceNameOffset + InstanceName.Length;
		if ( RegFlags & WMIREG_FLAG_INSTANCE_BASENAME )
		{
			RegFlags |= WMIREG_FLAG_INSTANCE_LIST;
			MofOffset += sizeof(WCHAR);
		}

		// Calculate RegistryPath offset
		RegistryPathOffset = MofOffset;
		if ( MofResourceName.Length )
		{
			RegistryPathOffset += MofResourceName.Length + sizeof(WCHAR);
		}

		// Calculate total size required
		TotalSize = RegistryPathOffset;
		if (pRegistryPath != NULL)
		{
			TotalSize += pRegistryPath->Length + sizeof(WCHAR);
		}

		// Check if we overflowed the buffer
		if ( TotalSize > pStack->Parameters.WMI.BufferSize )
		{
			status = STATUS_BUFFER_TOO_SMALL;
			*(PULONG)pStack->Parameters.WMI.Buffer = TotalSize;
			I.Information() = sizeof(ULONG_PTR);
			break;
		}

		// Fill in the WMIREGINFO

		I.Information() = pRegInfo->BufferSize = TotalSize;
		pRegInfo->NextWmiRegInfo = NULL;

		if ( pRegistryPath && pRegistryPath->Length )
			pRegInfo->RegistryPath = RegistryPathOffset;
		else
			pRegInfo->RegistryPath = 0;

		if ( MofResourceName.Length )
			pRegInfo->MofResourceName = MofOffset;
		else
			pRegInfo->MofResourceName = 0;

		pRegInfo->GuidCount = GuidCount;

		// Now loop for each GUID to fill in the WMIREGGUID array

		for (pRegGuid = pRegInfo->WmiRegGuid, i = 0; i < GuidCount; i++, pRegGuid++)
		{
			pRegGuid->Guid = *WmiLibContext->GuidList[i].Guid;
			pRegGuid->Flags = RegFlags | WmiLibContext->GuidList[i].Flags;
			pRegGuid->InstanceCount = WmiLibContext->GuidList[i].InstanceCount;

			if ( pRegGuid->Flags & WMIREG_FLAG_INSTANCE_BASENAME )
			{
				pRegGuid->BaseNameOffset = InstanceNameOffset;
			}
			else if ( pRegGuid->Flags & WMIREG_FLAG_INSTANCE_PDO )
			{
				pRegGuid->Pdo = ULONG_PTR(Pdo);
#if (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))
				if(MinorFunction == IRP_MN_REGINFO_EX)
					ObReferenceObject(Pdo);
#endif
			}
		}

		PWCHAR Bufptr;

		// Copy the instance name

		if ( (RegFlags & WMIREG_FLAG_INSTANCE_BASENAME) && (InstanceName.Buffer != NULL) )
		{
			Bufptr = PWCHAR(PUCHAR(pRegInfo) + InstanceNameOffset);
			*Bufptr = InstanceName.Length;
			Bufptr++;

			memcpy(Bufptr, InstanceName.Buffer, InstanceName.Length);
		}

		// Copy the registry path

		if ( pRegistryPath && pRegistryPath->Length )
		{
			Bufptr = PWCHAR(PUCHAR(pRegInfo) + RegistryPathOffset);

			*Bufptr = pRegistryPath->Length;
			Bufptr ++;

			memcpy(Bufptr, pRegistryPath->Buffer, pRegistryPath->Length);
		}

		// Copy the MOF resource name

		if ( MofResourceName.Length != 0 )
		{
			Bufptr = PWCHAR(PUCHAR(pRegInfo) + MofOffset);
			*Bufptr = MofResourceName.Length;
			Bufptr++;

			memcpy(Bufptr, MofResourceName.Buffer, MofResourceName.Length);
		}

		// Free the instance name if present

		if ( InstanceName.Buffer != NULL )
		{
			ExFreePool(InstanceName.Buffer);
		}

		break;

	default:
		ASSERT(FALSE);
		break;

	}	// end minor function switch

	if ( *pIrpDisposition == IrpNotCompleted )
	{
		I.Status() = status;
	}

	return status;
}

#endif // _WDM_
