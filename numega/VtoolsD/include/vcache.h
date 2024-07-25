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

//******************************************************************************
//
//   Title:	VCACHE.H - Virtual File Cache Service Declarations
//


#ifndef VCache_Device_ID
#define	VCache_Device_ID 	0x048B
#endif

/*ASM
.errnz VCache_Device_ID - 048Bh
*/

typedef	unsigned long		HLOOKUP;
#define VCLF_LOCKED			0x00000010
#define VCL_MAXCACHENAMELEN	80		

#ifndef NOSERVICE
/*XLATOFF*/
#define	VCache_Service	Declare_Service
#define VCache_DEVICE_ID	VCache_Device_ID
#pragma warning (disable:4003)	// turn off not enough params warning
/*XLATON*/
/*MACROS*/
Begin_Service_Table( VCache )
VCache_Service		(VCache_Get_Version)
VCache_Service		(VCache_Register)
VCache_Service		(VCache_GetSize)
VCache_Service		(VCache_CheckAvail)
VCache_Service		(VCache_FindBlock)
VCache_Service		(VCache_FreeBlock)
VCache_Service		(VCache_MakeMRU)
VCache_Service		(VCache_Hold)
VCache_Service		(VCache_Unhold)
VCache_Service		(VCache_Enum)
VCache_Service		(VCache_TestHandle)
VCache_Service		(VCache_VerifySums)
VCache_Service		(VCache_RecalcSums)
VCache_Service		(VCache_TestHold)
VCache_Service		(VCache_GetStats)
VCache_Service		(VCache_Deregister)
VCache_Service		(VCache_AdjustMinimum)
VCache_Service		(VCache_SwapBuffers)
VCache_Service		(VCache_RelinquishPage)
VCache_Service		(VCache_UseThisPage)
VCache_Service		(_VCache_CreateLookupCache)
VCache_Service		(_VCache_CloseLookupCache)
VCache_Service		(_VCache_DeleteLookupCache)
VCache_Service		(_VCache_Lookup)
VCache_Service		(_VCache_UpdateLookup)
End_Service_Table(VCache)
/*ENDMACROS*/
#endif

//**	VCache_Get_Version - Get version number
//
//	The version number returned during development will be bumped
//	any time something in the VCache service interface changes.
//	When we ship, it will get set to something like 30Ah.
//
//	Entry	None
//	Exit	EAX = version number (3.11 for this release)
//		EBX = options
//		'C' clear
//	Uses	EAX, EBX, Flags
//

#define	VCache_VERSION	0x100


//**	VCache_Register - register an FSD with cache
//
//	An FSD must call this function to register a buffer discard
//	function with VCache.  VCache will return a one byte ID for the
//	FSD to use to identify itself when issuing FindBlock requests.
//
//	This service may only be called during DEVICE_INIT.
//
//	Entry	(esi) = buffer discard procedure (*cannot be -1*)
//		(ecx) = minimum number of blocks to reserve for this FSD.
//	Exit	(al) = FSD ID
//	Uses	eax, flags


//**	VCache_GetSize - return cache size
//
//	VCache_GetSize returns the number of blocks in the cache.
//
//	Entry	(ah) = FSD ID or 0
//	Exit	If (ah) = 0
//		    (eax) = current # of blocks in cache
//		If (ah) = FSD ID
//		    (eax) = FSD's current quota (max. # of blocks
//			    it may own at a time)
//		(edx) = maximum # of cache blocks
//	Uses	eax, edx


//**	VCache_CheckAvail - check space available
//
//	An FSD can call CheckAvail to decide whether to cache a data
//	transfer or use a direct transfer instead.
//
//	Entry	(ah) = FSD ID
//		(ecx) = # of buffers needed
//	Exit	'C' clear if (ecx) buffers available
//		'C' set if fewer buffers available
//		  (eax) = # of buffers available at this time
//	Uses	eax, ebx, ecx, edx, flags


//**	VCache_FindBlock - lookup data in virtual cache
//
//	FindBlock will lookup a block of data in the cache.  The action taken
//	is based on the options specified.  The options are:
//		VCFB_Create - create buffer if not found
//		VCFB_Hold - hold buffer if found
//		VCFB_MakeMRU - make buffer most recently used
//		VCFB_LowPri - low priority create
//		VCFB_MustCreate - creation must succeed if there are
//		    any clean, unheld buffers (only valid with VCFB_Create)
//		obsolete value (ignored):
//		VCFB_RemoveFromLRU - remove buffer from LRU list entirely
//				     (for buffers that will be held for a long
//				     time).  Cannot be used with VCFB_MakeMRU.
//			
//
//
//	Entry	(al) = options
//		(ah) = file system identifier
//		(ebx) = key1 (sector, byte #, etc)
//		(edi) = key2 (volume id, file id, etc)
//		    NOTE: (0, 0) is not a legal key value.
//	Exit	'C' set if not found
//		  (esi) = cache block handle (if one was created)
//		'C' clear if found
//		  (esi) = cache block handle
//		  'Z' set if buffer not locked
//		(eax) = address of buffer (if ESI not 0)
//		NOTE: for new buffers the FSD private fields will be zero'ed.
//	Uses	eax, esi, flags (ecx & edx used if creating a buffer)

#define	VCFB_Create			0x01
#define	VCFB_Hold			0x02
#define	VCFB_MakeMRU		0x04
#define	VCFB_LowPri			0x08
#define	VCFB_MustCreate		0x10
#define	VCFB_RemoveFromLRU 	0x20

//**	VCache_FreeBlock - put cache block onto free list
//
//	FreeBlock invalidates the contents of a buffer and puts that the LRU
//	end of the MRU/LRU list. FreeBlock assumes that the file system has
//	already performed any cleanup required, so the buffer flush routine
//	will not be called.
//
//	Entry	(esi) = cache block handle
//	Exit	none
//	Uses	ecx, edx, flags


//**	VCache_MakeMRU - make block "most recently used"
//
//	This service is used when an FSD wants to update a buffer's access
//	information without the overhead of calling VCache_FindBuffer.
//
//	Entry	(esi) = cache block handle
//	Exit	buffer usage info updated
//	Uses	eax, ecx, edx, flags


//**	VCache_Hold - hold a buffer
//
//	Hold increments the buffer's hold counter to prevent the buffer from
//	being recycled.
//
//	Entry	(esi) = cache block handle
//	Exit	none
//	Uses	flags (carry preserved)


//**	VCache_TestHold - test hold status of a buffer
//
//	TestHold returns the buffer's hold counter.
//
//	Entry	(esi) = cache block handle
//	Exit	(eax) = buffer hold count
//		    Zero flag set if (EAX) = 0
//		    Zero flag clear if (EAX) != 0
//	Uses	eax, flags


//**	VCache_Unhold - release a buffer hold
//
//	Unhold decrements the buffer's hold counter.  When the hold counter
//	is 0, the buffer can be recycled.
//
//	NOTE: if the buffer is recycled the file system will be notified so
//	that it can flush the buffers contents if necessary or to maintain
//	any private data pertaining to the buffer.
//
//	Entry	(esi) = cache block handle
//	Exit	'ZR' set if buffer no longer held
//	Uses	flags (carry preserved)


//**	VCache_Enum - enumerate cache blocks
//
//	This routine enumerates all of the cache blocks that belong to
//	a particular FSD.  The function passed will be invoked for each
//	block as follows:
//		Entry	(esi) = cache block handle
//			'ZR' clear if block is held
//			'ZR' set if no holds on the block
//			(ebx, ecx, ebp) = from invokation of VCache_Enum
//		Exit	none
//	Note that if the function changes ebx, ecx, ebp then the changed values
//	will be passed to subsequent invokations of the function.  IE: This
//	service does not preserve the values of ebx, ecx, & ebp across the call.
//
//	Entry	(ah) = FSD ID
//		(edx) = function to call for each buffer
//		(ebx, ecx, ebp) = passed to function unchanged
//	Exit	none
//	Uses	all


//**	VCache_GetStats - return cache statistics
//
//	VCache_GetStats returns some statistics used by the memory manager
//	to tune how much memory is available for VCache and how much
//	for paging.  When the statistics are queried, the counts are cleared.
//	Therefore, if anyone other than the memory manager uses this service
//	system performance will suffer.
//
//	Entry	none
//	Exit	(ebx) = number of cache misses to one of the last 26 discarded
//			cache blocks since the last call to VCache_GetStats.
//		(ecx) = number of cache hits to one of the last 26 LRU cache
//			blocks since the last call to VCache_GetStats.
//		(edx) = base address of range reserved for vcache
//		(edi) = number of cache blocks discarded since the last call
//			to VCache_GetStats
//	Uses	ebx, ecx, edi


//**	VCache_Deregister - deregister an FSD with cache
//
//	An FSD must call this function to tell VCACHE that it no
//	longer needs any cache resources.
//
//	We do not recycle deregisterd Cache IDs because we do
//	not want to give out the same ID twice.
//
//	ENTRY	(AH) = FSD ID
//
//	EXIT	NONE
//
//	USES	EAX, EBX, ECX, EDX, EDI, ESI, Flags


//**	VCache_AdjustMinimum - adjust minimum buffer quota
//
//	This function adjusts the client's minimum buffer quota.
//
//	ENTRY	(AH) = FSD ID
//		(ECX) = new buffer quota
//
//	EXIT	Carry clear
//		    Adjustment made
//		Carry set
//		    Adjustment failed
//
//	USES	EAX, EBX, ECX, EDX, EDI, ESI, Flags


//**	VCache_SwapBuffers - swap a pair of cache block's buffer
//
//	Given two cache block handles, this procedure
//	swaps the buffers associated with the handles.
//	The blocks must be owned by the same client and
//	they must both be held.
//
//	ENTRY	(ESI) = fist cache handle
//		(EDI) = second cache handle
//
//	EXIT	Carry clear
//		    Swap performed
//		Carry set
//		    Swap not performed
//
//	USES	EAX, Flags

//**** Debug services ***

//**	VCache_TestHandle - test buffer handle
//
//	Verifies that ESI is a valid cache block handle.
//
//	Entry	(ah) = FSD ID
//		(esi) = cache block handle
//	Exit	none
//	Uses	none


//**	VCache_VerifySums - verify buffer checksums
//
//	This routine will verify that the correct dirty bits for a buffer
//	are set.  This is done by checksumming the buffer's contents on a
//	sector by sector basis and comparing against stored sums.  The
//	checksums are set by calling VCache_RecalcSums for the block.
//
//	We only check the sectors which aren't marked dirty
//
//	Entry	(eax) = sector size for this block
//		(esi) = cache block handle
//	EXIT	'Z' set if checksums match
//	Uses	all but ESI


//**	VCache_RecalcSums - recompute buffer checksums
//
//	This routine will compute a sector by sector checksum of a buffer's
//	contents and save the sums for later verification by calling
//	VCache_VerifySums.  An FSD should recalc the sums whenever the
//	contents of a buffer are change but the buffer is not going to be
//	made dirty.  EG: by reading data into the buffer.
//
//	Entry	(eax) = sector size for this block
//		(esi) = cache block handle
//	Exit	none
//	Uses	flags

//**** End of debug services ***


//* Offsets of fields in the cache block handle:

/*ASM
B_FSKey1	equ	dword ptr 08H
B_FSKey2	equ	dword ptr 0CH
B_BufPtr 	equ	dword ptr 10H

B_FSDData	equ	014H
SizeFSDData	equ	(7*4)	; seven dwords for FSD use

B_Dirty		equ	byte ptr 032H
*/

// Size of VCache buffers (one page):

#define		VCACHE_BUFSIZE 	4096		// 80x86 uses 4K pages

//**** Services to be used by the memory manager only ****

//**	VCache_RelinquishPage - return a page to the memory manager
//
//	This routine tries to find a page it can return to the 
//	memory manager. In general it will only fail if all pages
//	are currently reserved, held or dirty.
//
//	Entry   none
//	Exit	(eax) = linear address of page or 0
//	Uses	flags

//**	VCache_UseThisPage - a page is given to vcache
//
//	By virtue of this routine the memory manager makes a new page
//	available to vcache.
//
//	Entry	(eax) = address of page
//	Exit	none
//	Uses	flags

