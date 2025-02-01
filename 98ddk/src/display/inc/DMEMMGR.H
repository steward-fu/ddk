/*==========================================================================;
 *
 *  Copyright (C) 1994-1997 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	dmemmgr.h
 *  Content:	Direct Memory Manager include file
 ***************************************************************************/

#ifndef __DMEMMGR_INCLUDED__
#define __DMEMMGR_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * pointer to video meory
 */
typedef unsigned long	FLATPTR;


/*
 * Structure for querying extended heap alignment requirements
 */

typedef struct _SURFACEALIGNMENT
{
    union
    {
        struct
        {
            DWORD       dwStartAlignment;
            DWORD       dwPitchAlignment;
            DWORD       dwReserved1;
            DWORD       dwReserved2;
        } Linear;
        struct
        {
            DWORD       dwXAlignment;
            DWORD       dwYAlignment;
            DWORD       dwReserved1;
            DWORD       dwReserved2;
        } Rectangular;
    };
} SURFACEALIGNMENT;
typedef struct _SURFACEALIGNMENT FAR *LPSURFACEALIGNMENT;

typedef struct _HEAPALIGNMENT
{
    DWORD                dwSize;
    DDSCAPS              ddsCaps;       /* Indicates which alignment fields are valid.*/
    DWORD                dwReserved;
    SURFACEALIGNMENT     ExecuteBuffer; /* Surfaces tagged with DDSCAPS_EXECUTEBUFFER */
    SURFACEALIGNMENT     Overlay;       /* Surfaces tagged with DDSCAPS_OVERLAY       */
    SURFACEALIGNMENT     Texture;       /* Surfaces tagged with DDSCAPS_TEXTURE       */
    SURFACEALIGNMENT     ZBuffer;       /* Surfaces tagged with DDSCAPS_ZBUFFER       */
    SURFACEALIGNMENT     AlphaBuffer;   /* Surfaces tagged with DDSCAPS_ALPHA         */
    SURFACEALIGNMENT     Offscreen;     /* Surfaces tagged with DDSCAPS_OFFSCREENPLAIN*/
    SURFACEALIGNMENT     FlipTarget;    /* Surfaces whose bits are potential primaries i.e. back buffers*/
} HEAPALIGNMENT;
typedef struct _HEAPALIGNMENT FAR *LPHEAPALIGNMENT;

/*
 * video memory manager structures
 */
typedef struct _VMEML
{
    struct _VMEML 	FAR *next;
    FLATPTR		ptr;
    DWORD		size;
} VMEML, FAR *LPVMEML, FAR * FAR *LPLPVMEML;

typedef struct _VMEMR
{
    struct _VMEMR 	FAR *next;
    struct _VMEMR       FAR *prev;
    /*
     * The pUp, pDown, pLeft and pRight members were removed in DX5
     */
    struct _VMEMR 	FAR *pUp;
    struct _VMEMR 	FAR *pDown;
    struct _VMEMR 	FAR *pLeft;
    struct _VMEMR 	FAR *pRight;
    FLATPTR		ptr;
    DWORD		size;
    DWORD               x;
    DWORD               y;
    DWORD               cx;
    DWORD               cy;
    DWORD		flags;
    FLATPTR             pBits;
} VMEMR, FAR *LPVMEMR, FAR * FAR *LPLPVMEMR;

#ifdef NT_KERNEL_HEAPS
typedef void VMEMHEAP;
#else
typedef struct _VMEMHEAP
{
    DWORD		        dwFlags;
    DWORD                       stride;
    LPVOID		        freeList;
    LPVOID		        allocList;
    DWORD                       dwTotalSize;
    FLATPTR                     fpGARTLin;      /* AGP: GART linear base of heap (app. visible)   */
    FLATPTR                     fpGARTDev;      /* AGP: GART device base of heap (driver visible) */
    DWORD                       dwCommitedSize; /* AGP: Number of bytes commited to heap          */
    /*
     * Extended alignment data:
     * Filled in by DirectDraw in response to a GetHeapAlignment HAL call.
     */
    DWORD                       dwCoalesceCount;
    HEAPALIGNMENT               Alignment;
} VMEMHEAP;
#endif

typedef VMEMHEAP FAR *LPVMEMHEAP;

#define VMEMHEAP_LINEAR			0x00000001l /* Heap is linear                    */
#define VMEMHEAP_RECTANGULAR		0x00000002l /* Heap is rectangular               */
#define VMEMHEAP_ALIGNMENT  		0x00000004l /* Heap has extended alignment info  */

/*
 * These legacy DLL exports don't handle nonlocal heaps
 */
extern FLATPTR WINAPI VidMemAlloc( LPVMEMHEAP pvmh, DWORD width, DWORD height );
extern void WINAPI VidMemFree( LPVMEMHEAP pvmh, FLATPTR ptr );

/*
 * This DLL export can be used by drivers to allocate aligned surfaces from heaps which
 * they have previously exposed to DDRAW.DLL. This function can allocate from nonlocal heaps.
 */
extern FLATPTR WINAPI HeapVidMemAllocAligned( 
                struct _VIDMEM* lpVidMem,
                DWORD dwWidth, 
                DWORD dwHeight, 
                LPSURFACEALIGNMENT lpAlignment , 
                LPLONG lpNewPitch );



#ifdef __cplusplus
};
#endif

#endif
