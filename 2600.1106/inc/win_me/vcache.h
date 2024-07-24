//******************************************************************************
//
//   (C) Copyright MICROSOFT Corp., 1993-1999
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
#define VCLF_LOCKED		0x00000010
#define VCL_MAXCACHENAMELEN	80		

#ifndef NOSERVICE
/*XLATOFF*/
#define	VCache_Service	Declare_Service
#define VCache_DEVICE_ID	VCache_Device_ID
#pragma warning (disable:4003)	// turn off not enough params warning
/*XLATON*/
/*MACROS*/
Begin_Service_Table( VCache )
VCache_Service		(VCache_Get_Version, LOCAL)
VCache_Service		(VCache_Register, LOCAL)
VCache_Service		(VCache_GetSize, LOCAL)
VCache_Service		(VCache_CheckAvail, LOCAL)
VCache_Service		(VCache_FindBlock, LOCAL)
VCache_Service		(VCache_FreeBlock, LOCAL)
VCache_Service		(VCache_MakeMRU, LOCAL)
VCache_Service		(VCache_Hold, LOCAL)
VCache_Service		(VCache_Unhold, LOCAL)
VCache_Service		(VCache_Enum, LOCAL)
VCache_Service		(VCache_TestHandle, LOCAL)
VCache_Service		(VCache_VerifySums, LOCAL)	/* OBSOLETE */
VCache_Service		(VCache_RecalcSums, LOCAL)	/* OBSOLETE */
VCache_Service		(VCache_TestHold, LOCAL)
VCache_Service		(VCache_GetStats, LOCAL)
VCache_Service		(VCache_Deregister, LOCAL)
VCache_Service		(VCache_AdjustMinimum, LOCAL)
VCache_Service		(VCache_SwapBuffers, LOCAL)
VCache_Service		(VCache_RelinquishPage, LOCAL)
VCache_Service		(VCache_UseThisPage, LOCAL)
VCache_Service		(_VCache_CreateLookupCache)
VCache_Service		(_VCache_CloseLookupCache)
VCache_Service		(_VCache_DeleteLookupCache)
VCache_Service		(_VCache_Lookup)
VCache_Service		(_VCache_UpdateLookup)
VCache_Service          (VCache_MakeLRU, LOCAL)
VCache_Service          (VCache_RemoveFromLRU, LOCAL)
VCache_Service          (VCache_MakeMedian, LOCAL)
VCache_Service          (VCache_UnmapCacheBlock, LOCAL)
VCache_Service          (VCache_MapCacheBlock, LOCAL)
VCache_Service          (VCache_GetDebugWorkArea, LOCAL)
End_Service_Table(VCache)
/*ENDMACROS*/
#endif


//**	VCache_Get_Version - Get version number
//
//      The version number returned is updated whenever
//      the set of VCache services changes.
//
//	Entry	None
//      Exit    EAX = version number (1.00 for this release)
//              EBX = options (see VCache_OPTION_* equates below)
//		'C' clear
//	Uses	EAX, EBX, Flags
//
//      Most FSDs have hard-coded 0x100 as the expected VCACHE
//      version number.  Because we have added some new interfaces
//      (ie, VCache_MakeLRU and VCache_RemoveFromLRU) and some new
//      options to some of the old interfaces (ie, VCache_Register and
//      VCache_FindBlock), VxDs can determine the availability of these
//      features by examining (ebx).

#define VCache_VERSION  0x100           // version number hard-coded in FSDs

#define VCache_OPTION_NONE              0x00 // no special options
#define VCache_OPTION_REGEX_SERVICES    0x01 // VCache_Register extensions
#define VCache_OPTION_NEWLRU_SERVICES   0x02 // new LRU services


//**    VCache_Register - register an FSD with VCache
//
//	An FSD must call this function to register a buffer discard
//	function with VCache.  VCache will return a one byte ID for the
//	FSD to use to identify itself when issuing FindBlock requests.
//
//	This service may only be called during DEVICE_INIT.
//
//      Entry   (esi) = buffer discard handler (*cannot be -1*)
//              (ecx) = minimum number of buffers to reserve for this FSD
//      OR
//              (esi) = buffer discard handler (*cannot be -1*)
//              (ecx) = -1 (to identify extended VCache_Register calls)
//              (edx) = minimum number of buffers to reserve for this FSD
//              (edi) = maximum number of buffers (as a percentage, 1-100)
//              (al)  = one or more VCFLAG_* flags
//
//      Exit    (al) = FSD ID and carry clear if successful, else carry set
//
//      Uses    eax, flags
//
//      Flags   VCFLAG_COMPRESS:
//
//              Allows VCache to compress selected cache blocks;  this is
//              normally performed on the oldest unheld cache blocks first,
//              and generally only during idle-time, but clients must assume
//              than any unheld buffer could be compressed at any time.
//
//              VCFLAG_NOSTAT:
//
//              Prevents VCache from updating I/O statistics when a cache
//              block hit or miss occurs.  Use this flag when there is no
//              direct correlation between the data and disk (or other) I/O.
//
//              VCFLAG_COMPRESS_NOTIFY:
//
//              Forces VCache to notify a discard handler whenever a cache
//              block is compressed.  FSDs that record specific cache block
//              handles must not refer to B_BufPtr once a block has been
//              compressed.  However, the practice of recording cache block
//              handles is strongly discouraged;  callers should instead
//              always use VCache_FindBlock to obtain a current handle for
//              their data.  VCache_FindBlock also guarantees that the data,
//              if found, will be uncompressed on return (unless VCFB_Peek was
//              specified).
//
//      Notes regarding discard handlers:
//
//              Discard handlers are passed a cache block handle in (esi),
//              and if VCache_OPTION_REGEX_SERVICES is supported, they are also
//              passed one of the VC_* constants listed below in (al).  They
//              can modify any registers.

#define VC_DEREG                        0x00
#define VC_DISCARD                      0x01
#define VC_COMPRESS                     0x02

#define VCFLAG_NONE                     0x00
#define VCFLAG_COMPRESS                 0x01    // allow compression of buffers
#define VCFLAG_NOSTAT                   0x02    // no stat accumulation for this FSD
#define VCFLAG_COMPRESS_NOTIFY          0x04    // notify discard handler for compressed blocks
#ifdef	MAPCACHE
#define VCFLAG_UNMAPCACHE_NOTIFY		0x08    // notify FSD unmap cache block handler
#endif


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
//              VCFB_LowPri - low priority create
//		VCFB_MustCreate - creation must succeed if there are
//                  any clean, unheld buffers (only valid with VCFB_Create);
//                  this essentially forces a purely LRU search of available
//                  buffers with no age restrictions (no random replacement)
//		VCFB_RemoveFromLRU - remove buffer from LRU list entirely
//                  (for buffers held indefinitely; not valid with VCFB_MakeMRU)
//              VCFB_Peek - test for presence without returning buffer address
//
//	Entry	(al) = options
//		(ah) = file system identifier
//		(ebx) = key1 (sector, byte #, etc)
//		(edi) = key2 (volume id, file id, etc)
//              NOTE: (0, 0) is not a legal key value.
//	Exit	'C' set if not found
//		  (esi) = cache block handle (if one was created)
//		  Note that a newly created cache block is placed in
//		  the LRU list as follows:
//		    VCFB_MakeMRU    VCFB_LowPri	    Position
//			  0		 0	    Median
//			  1		 0	    MRU
//			  0		 1	    LRU
//			  1		 1	    (Not Allowed)
//		'C' clear if found
//		  (esi) = cache block handle
//		(eax) = address of buffer (if ESI not 0)
//		NOTE: for new buffers the FSD private fields will be zero'ed.
//      Uses    eax, esi, flags
//      Notes   VCache_FindBlock guarantees that the data, if found, will always
//              be uncompressed on return (unless VCFB_Peek was specified).

#define VCFB_None               0x00
#define VCFB_Create             0x01
#define VCFB_Hold               0x02
#define	VCFB_MakeMRU		0x04
#define VCFB_LowPri             0x08
#define VCFB_MustCreate         0x10
#define VCFB_LRUOnly            VCFB_MustCreate
#define	VCFB_RemoveFromLRU 	0x20
#define VCFB_Peek               0x40


//**	VCache_FreeBlock - put cache block onto free list
//
//      This invalidates the cache block handle and puts the associated cache
//      block page on VCache's free page list.  FreeBlock assumes that the file
//      system has already performed any cleanup required, so the buffer flush
//      routine will not be called.
//
//	Entry	(esi) = cache block handle
//	Exit	none
//	Uses	ecx, edx, flags


//**	VCache_MakeMRU - make block "most recently used"
//
//	This service is used when an FSD wants to update a buffer's access
//	information without the overhead of calling VCache_FindBuffer.
//
//      You can also use this service to reinsert buffers into the LRU list
//      that were removed using VCache_RemoveFromLRU.
//
//      Entry   (esi) = cache block handle
//	Exit	buffer usage info updated
//      Uses    eax, ecx, edx


//**    VCache_MakeLRU - make block "least recently used"
//
//      This service is similar to VCache_MakeMRU, but it makes the specified
//      buffer LRU.  Use it in lieu of VCache_FreeBlock to keep low-priority
//      in the cache as long as possible.
//
//      You can also use this service to reinsert buffers into the LRU list
//      that were removed using VCache_RemoveFromLRU.
//
//	Entry	(esi) = cache block handle
//	Exit	buffer usage info updated
//      Uses    eax, ecx, edx


//**    VCache_RemoveFromLRU - remove block from LRU list
//
//      This service is used when an FSD wants to remove a buffer from
//      the LRU list (ie, it intends to hold the buffer for long time).
//
//      This can also be done as part of a VCache_FindBlock call, by using
//      the VCFB_RemoveFromLRU flag, provided a buffer was either found
//      or created.
//
//      Entry   (esi) = cache block handle
//      Exit    buffer removed from LRU list
//      Uses    ecx, edx


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


//**	VCache_GetDebugWorkArea - get debug work area
//
//	This routine returns the address and size of the specified
//	buffer's debug work area.
//
//	ENTRY	(esi) = cache block handle
//
//	EXIT	(EAX) = work area address
//		(ECX) = work area size (0 if none)
//
//	USES	EAX, ECX, Flags

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

/*XLATOFF*/
typedef struct _VCACHE_BLOCK {
    DWORD   Reserved1[ 2 ];
    DWORD   FSKey1;
    DWORD   FSKey2;
    PVOID   BufPtr;
    DWORD   FSData[ 7 ];
    WORD    Reserved2;
    BYTE    Dirty;
} VCACHE_BLOCK, *PVCACHE_BLOCK;
/*XLATON*/

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

//**** End of services reserved for memory manager
/*XLATOFF*/

#pragma warning (disable:4035)	// turn off "no return value"


//**    The 'Lookup Cache' is a separate caching subsystem that allows
//      arbitrary keys to be associated with arbitrary data.  This data
//      is stored in the registry and persistent.  Note that the
//      caches themselves live in locked memory, the registry is used
//      as a backup store and is updated periodically as a background
//      process - so _VCache_Lookup and _VCache_UpdateLookup can be called
//      at event time.


//**    VCache_CreateLookupCache
//
//      Creates a new (or opens an existing) lookup cache.
//
//      Entry:
//              lpszName  - name of the lookup cache;
//                          must be a legal registry key name.
//              nMaxElems - maximum number of elements in the cache.  Once this
//                          limit has been reached, the addition of new keys
//                          will result in old keys being aged out, LRU style.
//              Flags     - reserved, MBZ.
//              phlookup  - pointer to DWORD that will receive the opened cache
//                          handle.
//      Exit:
//              eax       - zero on success, or an appropriate Win32 error value

DWORD VXDINLINE
_VCache_CreateLookupCache(char    *lpszName,
                          DWORD    nMaxElems,
                          DWORD    Flags,
                          HLOOKUP *phlookup)
{
	unsigned long	retval;

	_asm	push	phlookup
	_asm	push	Flags
	_asm	push	nMaxElems
	_asm	push	lpszName
	VxDCall( _VCache_CreateLookupCache )
        _asm    add     esp, 4*4
        _asm    mov     retval, eax
	return	retval;
}

//**	_VCache_CloseLookupCache
//
//      Closes an open lookup cache.
//
//      Exit:
//              eax       - zero on success, or an appropriate Win32 error value

unsigned long VXDINLINE
_VCache_CloseLookupCache(HLOOKUP _hnd_)
{
	unsigned long	retval;

        _asm    mov     eax, _hnd_
	_asm	push	eax
	VxDCall( _VCache_CloseLookupCache )
        _asm    add     esp, 4
        _asm    mov     retval, eax
	return	retval;
}

//**	_VCache_DeleteLookupCache
//
//      Deletes a lookup cache
//
//      Entry:
//              _hnd_     - handle of previously opened cache.
//
//      Exit:
//              eax       - zero on success, or an appropriate Win32 error value

unsigned long  VXDINLINE
_VCache_DeleteLookupCache(char *lpszName)
{
	unsigned long	retval;

	_asm	push	lpszName
	VxDCall( _VCache_DeleteLookupCache )
        _asm    add     esp, 4
        _asm    mov     retval, eax
	return	retval;

}

//**	_VCache_Lookup
//
//      Given an existing key value, moves that key to the head of the cache's
//      LRU list and retrieves the associated data.
//
//      Entry:
//              hLookup   - handle of previously opened cache
//              keylen    - length of supplied key value
//              pKey      - pointer to key value
//              datalen   - length of destination data buffer
//              pData     - pointer to destination data buffer
//
//      Exit:
//              eax       - zero on success, or an appropriate Win32 error value

unsigned long  VXDINLINE
_VCache_Lookup(HLOOKUP hLookup, unsigned long keylen, void *pKey, unsigned long *pdatalen,void *pData)
{
	unsigned long	retval;

	_asm	push	pData
	_asm	push	pdatalen
	_asm	push	pKey
	_asm	push	keylen
	_asm	push	hLookup
	VxDCall( _VCache_Lookup )
        _asm    add     esp, 5*4
        _asm    mov     retval, eax
	return retval;
}


//**	_VCache_UpdateLookup
//
//      If an existing key is present, its associated data is updated with
//      pData.  Otherwise a new key entry is generated, aging the LRU entry if
//      the cache is full.
//
//      Entry:
//              hLookup   - handle of previously opened cache
//              keylen    - length of supplied key value
//              pKey      - pointer to key value
//              datalen   - length of destination data buffer
//              pData     - pointer to destination data buffer
//
//      Exit:
//              eax       - zero on success, or an appropriate Win32 error value

unsigned long VXDINLINE
_VCache_UpdateLookup(HLOOKUP hLookup, unsigned long keylen, void *pKey, unsigned long datalen, void *pData)
{
	unsigned long	retval;

	_asm	push 	pData
	_asm	push 	datalen
	_asm	push	pKey
	_asm	push	keylen
	_asm	push	hLookup
	VxDCall( _VCache_UpdateLookup )
        _asm    add     esp, 5*4
        _asm    mov     retval, eax
	return retval;
}

#ifdef	MAPCACHE
//**	VCache_UnmapCacheBlock - unmap the cache block previously mapped by the memory manager
//
//	UnmapCacheBlock returns with CARRY indicating the state of the unmap operation
//
//	Entry	(esi) = cache block handle
//	Exit	Carry clr - indicates cache block was unmapped
//			Carry set - indicates cache block was NOT unmapped
//
//			A failure due to the cache block handle being invalid or a failure
//			due to the fact that a VCACHE client has not registered a callback
//			routine to perform the unmapping.
//
//	Uses	eax,flags
#endif

#pragma warning (default:4035)	// default "no return value"
#pragma warning (default:4003)  // not enough params

/*XLATON*/
