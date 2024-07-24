/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/
/*
 *  GART.C    -   XYZ chipset's GART miniport routines.
 *
 *  Notes:
 *
 */

#include "pch.h"

#define APSIZE  0
#define APBASE  0

typedef struct _GARTINSTANCEDATA        GARTINSTANCEDATA, *PGARTINSTANCEDATA;

//
// GART instance data.
//
struct  _GARTINSTANCEDATA {
	ULONG                   ulInstance;             // Must be first
	ULONG			ulGARTRegionBase;
	ULONG			ulGARTRegionSize;
	ULONG			ulReserveValue;
	ULONG                   ulFreeValue;
	PULONG			pulLinearGARTTableBase;
	ULONG			ulGARTTableEntryCount;
	};

VOID
FillMemory(PULONG pWhere, ULONG ulCount, ULONG ulValue)
{
	for (;ulCount; ulCount--, pWhere++)
		*pWhere=ulValue;
}

/*******************************************************************************
 *
 *      PCIMP_GARTInit - Initializes the GART miniport
 *
 *      Exported.
 *
 *      ENTRY:  ulInstance is the miniport instance.
 *
 *              pHandle is the handle on the GART PCI device.
 *
 *              ppRefData returns the miniport's ref data.
 *
 *      EXIT:   PCIMP_SUCCESS if successful, PCIMP_FAILURE otherwise.
 *
*******************************************************************************/
PCIMPRET CDECL
PCIMP_GARTInit(ULONG ulInstance, PVOID pHandle, PVOID *ppRefData)
{
	ULONG                   ulGARTSize, ulGARTBase;
	PGARTINSTANCEDATA       pGARTInstance;

	UCHAR   bBusGART, bDevFuncGART;

	//
	// Get our info.
	//
	PCIGetHandleInfo(pHandle, (PULONG)&bBusGART, (PULONG)&bDevFuncGART);

	//
	// Get the current GART max size.
	//
	ulGARTSize=ReadConfigUlong(bBusGART, bDevFuncGART, APSIZE);

	ulGARTBase=ReadConfigUlong(bBusGART, bDevFuncGART, APBASE) & ~0xF;
	
	if ((pGARTInstance=ExAllocatePool(PagedPool, sizeof(GARTINSTANCEDATA)))==NULL)
		return(PCIMP_FAILURE);

	pGARTInstance->ulGARTRegionBase=ulGARTBase;
	pGARTInstance->ulGARTRegionSize=ulGARTSize;

	*ppRefData=pGARTInstance;

	return(PCIMP_SUCCESS);
}

/*******************************************************************************
 *
 *      PCIMP_GARTExit - Exits GART miniport
 *
 *      Exported.
 *
 *      ENTRY:  pHandle is the handle on the GART PCI device.
 *
 *              pRefData is the miniport's ref data.
 *
 *      EXIT:   None.
 *
*******************************************************************************/
VOID CDECL
PCIMP_GARTExit(PVOID pHandle, PVOID pRefData)
{
	//
	// Nothing to do.
	//
}

/******************************************************************************
 *
 *      PCIMP_GARTReset - Resets the GART
 *
 *      Exported.
 *
 *      ENTRY:  pHandle is the handle on the GART PCI device.
 *
 *              pRefData indicates which GART we are using.
 *
 *              ulPhysPageCount is the number of physical pages in the system.
 *
 *              ulInvalidAddr1 is the physical address for free GART entries.
 *
 *              ulInvalidAddr2 is the physical address for reserved GART
 *              entries.
 *
 *              pulFeatures is the pointer to the variable that will receive
 *              the GART features.
 *
 *      EXIT:   PCIMP_SUCCESS iff successful, PCIMP_FAILURE iff not.
 *
 ******************************************************************************/
PCIMPRET CDECL
PCIMP_GARTReset(PVOID pHandle, PVOID pRefData, ULONG ulPhysPageCount, ULONG ulInvalidAddr1, ULONG ulInvalidAddr2, PULONG pulFeatures)
{
	ULONG                   ulGARTTableSize;
	PHYSICAL_ADDRESS        pa;
	UCHAR                   bBusGART, bDevFuncGART;
	PGARTINSTANCEDATA pGARTInstance=(PGARTINSTANCEDATA)pRefData;

	//
	// Features will be determined by Microsoft for each chipset. This is for internal use only.
	//
	*pulFeatures=0;

	//
	// Set our special values.
	//
	pGARTInstance->ulReserveValue=ulInvalidAddr1;
	pGARTInstance->ulFreeValue=0;

	//
	// Compute the GART table size.
	//
	ulGARTTableSize=(pGARTInstance->ulGARTRegionSize/PAGE_SIZE)*sizeof(ULONG);

	//
	// Round up the GART table size.
	//
	ulGARTTableSize=(ulGARTTableSize+PAGE_SIZE-1) & (~(PAGE_SIZE-1));

	//
	// Compute the GART entry count.
	//
	pGARTInstance->ulGARTTableEntryCount=ulGARTTableSize/sizeof(ULONG);

	//
	// Allocate the linear address for the GART table.
	//
	pa.HighPart=MAXULONG;
	pa.LowPart=MAXULONG;
	if ((pGARTInstance->pulLinearGARTTableBase=PCIAllocatePages(ulGARTTableSize/PAGE_SIZE, 0, PCIPAGECONTIG))==NULL)
		return(PCIMP_FAILURE);

	//
	// Initialize by freeing everything.
	//
	PCIMP_GARTFree(pHandle, pGARTInstance, pGARTInstance->ulGARTTableEntryCount, pGARTInstance->pulLinearGARTTableBase);

	//
	// Get our info.
	//
	PCIGetHandleInfo(pHandle, (PULONG)&bBusGART, (PULONG)&bDevFuncGART);

	//
	// Do whatever it takes to setup the GART base pointer in the chipset.
	//

	return(PCIMP_SUCCESS);
}

/******************************************************************************
 *
 *      PCIMP_GARTReserve - Reserves entries in the GART
 *
 *      Exported.
 *
 *      ENTRY:  pHandle is the handle on the GART PCI device.
 *
 *              pRefData indicates which GART we are using.
 *
 *              ulNumPages is the number of physical pages for which entries
 *              need to be allocated in the GART.
 *
 *              ulAlignment is the desired alignment.
 *
 *              ppMapHandle is a pointer to variable that receives the handle
 *              to be used during Map/UnMap and Flush.
 *
 *              pulGARTDev is a pointer to a variable that receives the base
 *              physical address in the AGP aperture for GART entries
 *              allocated.
 *
 *              ulFlags specifies the type of memory for which entries need
 *              to be allocated.
 *
 *      EXIT:   PCIMP_SUCCESS iff successful.
 *
 ******************************************************************************/
PCIMPRET CDECL
PCIMP_GARTReserve(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, ULONG ulAlignMask, PVOID *ppMapHandle, PULONG pulGARTDev, ULONG ulFlags)
{
	ULONG   ulGARTHandle, i;
	BOOLEAN fFound;
	PGARTINSTANCEDATA pGARTInstance=(PGARTINSTANCEDATA)pRefData;

	//
	// Convert alignment into an increment.
	//
	ulAlignMask++;

	//
	// Scan the GART.
	//
	ulGARTHandle=0;
	while (ulGARTHandle<pGARTInstance->ulGARTTableEntryCount) {

		//
		// Check at ulGARTHandle;
		//
		fFound=TRUE;
		for (i=0;i<ulNumPages;i++) {

			//
			// Is it occupied?
			//
			if (*(pGARTInstance->pulLinearGARTTableBase+ulGARTHandle+i)!=0) {

				//
				// Yes, so we did not found a contiguous
				// region starting at ulGARTHandle.
				//
				fFound=FALSE;
				break;
			}
		}

		//
		// 
		//
		if (fFound) {

			*pulGARTDev=pGARTInstance->ulGARTRegionBase+(ulGARTHandle*PAGE_SIZE);
			*ppMapHandle=(PVOID)(pGARTInstance->pulLinearGARTTableBase+ulGARTHandle);
			PCIMP_GARTUnMap(pHandle, pGARTInstance, ulNumPages, *ppMapHandle, 0);
			return(PCIMP_SUCCESS);
		}

		//
		// We failed at i, so the best next entry is at i+1. To
		// normalize, we need to add ulAlignMask-1 and truncate.
		//
		ulGARTHandle+=(i+ulAlignMask);
		ulGARTHandle&=(~(ulAlignMask-1));
	}

	//
	// We are full. Sorry.
	//
	return(PCIMP_FAILURE);
}

/******************************************************************************
 *
 *      PCIMP_GARTMap - Map entries in the GART
 *
 *      Exported.
 *
 *      ENTRY:  pHandle is the handle on the GART PCI device.
 *
 *              pRefData indicates which GART we are using.
 *
 *              ulNumPages is the number of pages for which entries need to
 *              be map in the GART.
 *
 *              pMapHandle is the reserve ref data.
 *
 *              ulByteOffset is the offset from the original reserve call.
 *
 *              pSystemLin is the linear address of the system pages to map
 *              in.
 *
 *      EXIT:   None.
 *
 ******************************************************************************/
VOID CDECL
PCIMP_GARTMap(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PULONG pMapHandle, ULONG ulByteOffset, PUCHAR pSystemLin)
{
	pMapHandle+=(ulByteOffset>>PAGE_SHIFT);
	for (;ulNumPages; ulNumPages--, pMapHandle++, pSystemLin+=PAGE_SIZE)
		*pMapHandle=PCILinToPhys((PVOID)(ULONG)pSystemLin);
}

/******************************************************************************
 *
 *      PCIMP_GARTFlush - Flush entries in the GART
 *
 *      Exported.
 *
 *      ENTRY:  pHandle is the handle on the GART PCI device.
 *
 *              pRefData indicates which GART we are using.
 *
 *              ulNumPages is the number of pages for which entries need to
 *              be flushed in the GART.
 *
 *              pMapHandle is the reserve ref data.
 *
 *              ulByteOffset is the offset from the original reserve call.
 *
 *              pSystemLin is the linear address of the system pages to flush.
 *
 *      EXIT:   None.
 *
 ******************************************************************************/
VOID CDECL
PCIMP_GARTFlush(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PULONG pMapHandle, ULONG ulByteOffset, PVOID pSystemLin)
{
	//
	// Flush if the chipset can.
	//
}

/******************************************************************************
 *
 *      PCIMP_GARTUnMap - Unmap entries in the GART
 *
 *      Exported.
 *
 *      ENTRY:  pHandle is the handle on the GART PCI device.
 *
 *              pRefData indicates which GART we are using.
 *
 *              ulNumPages is the number of pages for which entries need to
 *              be unmap in the GART.
 *
 *              pMapHandle is the reserve ref data.
 *
 *              ulByteOffset is the offset from the original reserve call.
 *
 *      EXIT:   None.
 *
 ******************************************************************************/
VOID CDECL
PCIMP_GARTUnMap(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PULONG pMapHandle, ULONG ulByteOffset)
{
	FillMemory(pMapHandle+(ulByteOffset>>PAGE_SHIFT), ulNumPages, ((PGARTINSTANCEDATA)pRefData)->ulReserveValue);
}

/******************************************************************************
 *
 *      PCIMP_GARTFree - Free entries in the GART
 *
 *      Exported.
 *
 *      ENTRY:  pHandle is the handle on the GART PCI device.
 *
 *              pRefData indicates which GART we are using.
 *
 *              ulNumPages is the number of pages for which entries need to
 *              be unmap in the GART.
 *
 *              pMapHandle is the reserve ref data.
 *
 *      EXIT:   None.
 *
 ******************************************************************************/
VOID CDECL
PCIMP_GARTFree(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PULONG pMapHandle)
{
	FillMemory(pMapHandle, ulNumPages, ((PGARTINSTANCEDATA)pRefData)->ulFreeValue);
}
