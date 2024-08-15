;;
;; Author:  Steward Fu <steward.fu@gmail.com>
;; Updated: 2024/08/15
;; Website: https://steward-fu.github.io/website/index.htm
;;

;//VOID ProbeForRead(
;//  _In_ PVOID  Address,
;//  _In_ SIZE_T Length,
;//  _In_ ULONG  Alignment
;//);
ProbeForRead proto STDCALL :DWORD, :DWORD, :DWORD

;//VOID ProbeForWrite(
;//  _In_ PVOID  Address,
;//  _In_ SIZE_T Length,
;//  _In_ ULONG  Alignment
;//);
ProbeForWrite proto STDCALL :DWORD, :DWORD, :DWORD

;//++
;//
;// PVOID
;// MmGetSystemAddressForMdlSafe (
;//     __in PMDL MDL,
;//     __in MM_PAGE_PRIORITY PRIORITY
;//     )
;//
;// Routine Description:
;//
;//     This routine returns the mapped address of an MDL. If the
;//     Mdl is not already mapped or a system address, it is mapped.
;//
;// Arguments:
;//
;//     MemoryDescriptorList - Pointer to the MDL to map.
;//
;//     Priority - Supplies an indication as to how important it is that this
;//                request succeed under low available PTE conditions.
;//
;// Return Value:
;//
;//     Returns the base address where the pages are mapped.  The base address
;//     has the same offset as the virtual address in the MDL.
;//
;//     Unlike MmGetSystemAddressForMdl, Safe guarantees that it will always
;//     return NULL on failure instead of bugchecking the system.
;//
;//     This macro is not usable by WDM 1.0 drivers as 1.0 did not include
;//     MmMapLockedPagesSpecifyCache.  The solution for WDM 1.0 drivers is to
;//     provide synchronization and set/reset the MDL_MAPPING_CAN_FAIL bit.
;//
;//--
;#define MmGetSystemAddressForMdlSafe(MDL, PRIORITY)                    \
;     (((MDL)->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA |                    \
;                        MDL_SOURCE_IS_NONPAGED_POOL)) ?                \
;                             ((MDL)->MappedSystemVa) :                 \
;                             (MmMapLockedPagesSpecifyCache((MDL),      \
;                                                           KernelMode, \
;                                                           MmCached,   \
;                                                           NULL,       \
;                                                           FALSE,      \
;                                                           (PRIORITY))))
MmGetSystemAddressForMdlSafe macro _Mdl:REQ, _PRIORITY:REQ
    push ebx
    push ecx
    IFDIFI <_Mdl>, <eax>
		mov eax, _Mdl
	ENDIF
    mov ebx, MDL_MAPPED_TO_SYSTEM_VA
    or ebx, MDL_SOURCE_IS_NONPAGED_POOL
    movzx ecx, (MDL ptr [eax]).MdlFlags
    and ebx, ecx
    .if ebx != 0
        mov eax, (MDL ptr [eax]).MappedSystemVa
    .else
        mov ebx, _PRIORITY
        invoke MmMapLockedPagesSpecifyCache, eax, KernelMode, MmCached, NULL, FALSE, ebx
    .endif
    pop ecx
    pop ebx
endm
