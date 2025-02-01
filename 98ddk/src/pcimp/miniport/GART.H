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
 *  GART.H    -   GART miniport header file.
 *
 *  Notes:
 *
 *  This dispatches the GART services to the chipset specific functions.
 *
 */

typedef PVOID GART_PHYSICAL_ADDRESS;

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
PCIMP_GARTInit(ULONG ulInstance, PVOID pHandle, PVOID *ppRefData);

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
PCIMP_GARTExit(PVOID pHandle, PVOID pRefData);

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
PCIMP_GARTReset(PVOID pHandle, PVOID pRefData, ULONG ulPhysPageCount, ULONG ulInvalidAddr1, ULONG ulInvalidAddr2, PULONG pulFeatures);

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
PCIMP_GARTReserve(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, ULONG ulAlignMask, PVOID *ppMapHandle, PULONG pulGARTDev, ULONG ulFlags);

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
PCIMP_GARTMap(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PULONG pMapHandle, ULONG ulByteOffset, PUCHAR pSystemLin);

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
PCIMP_GARTFlush(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PULONG pMapHandle, ULONG ulByteOffset, PVOID pSystemLin);

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
PCIMP_GARTUnMap(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PULONG pMapHandle, ULONG ulByteOffset);

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
PCIMP_GARTFree(PVOID pHandle, PVOID pRefData, ULONG ulNumPages, PULONG pMapHandle);

