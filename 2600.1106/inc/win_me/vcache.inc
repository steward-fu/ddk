ifndef VCache_Device_ID
VCache_Device_ID	EQU	048BH
endif
.errnz VCache_Device_ID - 048Bh

VCLF_LOCKED	EQU	00000010H
VCL_MAXCACHENAMELEN	EQU	80
ifndef NOSERVICE
Begin_Service_Table  VCache
VCache_Service		 VCache_Get_Version, LOCAL
VCache_Service		 VCache_Register, LOCAL
VCache_Service		 VCache_GetSize, LOCAL
VCache_Service		 VCache_CheckAvail, LOCAL
VCache_Service		 VCache_FindBlock, LOCAL
VCache_Service		 VCache_FreeBlock, LOCAL
VCache_Service		 VCache_MakeMRU, LOCAL
VCache_Service		 VCache_Hold, LOCAL
VCache_Service		 VCache_Unhold, LOCAL
VCache_Service		 VCache_Enum, LOCAL
VCache_Service		 VCache_TestHandle, LOCAL
VCache_Service		 VCache_VerifySums, LOCAL
VCache_Service		 VCache_RecalcSums, LOCAL
VCache_Service		 VCache_TestHold, LOCAL
VCache_Service		 VCache_GetStats, LOCAL
VCache_Service		 VCache_Deregister, LOCAL
VCache_Service		 VCache_AdjustMinimum, LOCAL
VCache_Service		 VCache_SwapBuffers, LOCAL
VCache_Service		 VCache_RelinquishPage, LOCAL
VCache_Service		 VCache_UseThisPage, LOCAL
VCache_Service		 _VCache_CreateLookupCache
VCache_Service		 _VCache_CloseLookupCache
VCache_Service		 _VCache_DeleteLookupCache
VCache_Service		 _VCache_Lookup
VCache_Service		 _VCache_UpdateLookup
VCache_Service           VCache_MakeLRU, LOCAL
VCache_Service           VCache_RemoveFromLRU, LOCAL
VCache_Service           VCache_MakeMedian, LOCAL
VCache_Service           VCache_UnmapCacheBlock, LOCAL
VCache_Service           VCache_MapCacheBlock, LOCAL
VCache_Service           VCache_GetDebugWorkArea, LOCAL
End_Service_Table VCache
endif
VCache_VERSION	EQU	100H
VCache_OPTION_NONE	EQU	00H
VCache_OPTION_REGEX_SERVICES	EQU	01H
VCache_OPTION_NEWLRU_SERVICES	EQU	02H
VC_DEREG	EQU	00H
VC_DISCARD	EQU	01H
VC_COMPRESS	EQU	02H
VCFLAG_NONE	EQU	00H
VCFLAG_COMPRESS	EQU	01H
VCFLAG_NOSTAT	EQU	02H
VCFLAG_COMPRESS_NOTIFY	EQU	04H
ifdef	MAPCACHE
VCFLAG_UNMAPCACHE_NOTIFY	EQU	08H
endif
VCFB_None	EQU	00H
VCFB_Create	EQU	01H
VCFB_Hold	EQU	02H
VCFB_MakeMRU	EQU	04H
VCFB_LowPri	EQU	08H
VCFB_MustCreate	EQU	10H
VCFB_LRUOnly	EQU	VCFB_MustCreate
VCFB_RemoveFromLRU	EQU	20H
VCFB_Peek	EQU	40H
B_FSKey1	equ	dword ptr 08H
B_FSKey2	equ	dword ptr 0CH
B_BufPtr 	equ	dword ptr 10H

B_FSDData	equ	014H
SizeFSDData	equ	(7*4)

B_Dirty		equ	byte ptr 032H

VCACHE_BUFSIZE	EQU	4096
