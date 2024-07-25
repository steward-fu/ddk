// iospace.cpp - implementation of NT I/O Space functions
// Copyright (c) 1997 Compuware Corporation

#include <vdw.h>

extern "C" ULONG _fltused = 0;

#define PAGEOFFSET(a) (ULONG(a) & 0xfff)
#define _NPAGES_(p, k)  ((PAGENUM((char*)p+(k-1)) - PAGENUM(p)) + 1)
#if !defined PAGENUM
#define PAGENUM(p) (((ULONG)(p)) >> 12)
#endif

PVOID NTAPI MmMapIoSpace (
    IN PHYSICAL_ADDRESS PhysicalAddress,
    IN ULONG NumberOfBytes,
    IN MEMORY_CACHING_TYPE CacheType
    )
{
	ASSERT(PhysicalAddress.u.HighPart == 0);

	ULONG nPages = _NPAGES_(PhysicalAddress.u.LowPart, NumberOfBytes);
	PVOID PageAddress = PageReserve(
		PR_SYSTEM,
		nPages,
		0
		);

	if (PageAddress == PVOID(0xFFFFFFFF))
		return NULL;

	BOOL status = PageCommitPhys(
		PAGENUM(PageAddress),
		nPages,
		PAGENUM(PhysicalAddress.u.LowPart),
		PC_INCR|PC_USER|PC_WRITEABLE
		);

	if ( !status )
	{
		PageFree(ULONG(PageAddress), 0);
		return NULL;
	}

	LinPageLock(PAGENUM(PageAddress), nPages, 0);

	return PVOID( ULONG(PageAddress) + PAGEOFFSET(PhysicalAddress.u.LowPart) );
}

VOID NTAPI MmUnmapIoSpace (
    IN PVOID BaseAddress,
    IN ULONG NumberOfBytes
    )
{
	ULONG nPages = _NPAGES_(BaseAddress, NumberOfBytes);
	ULONG iPage = PAGENUM(BaseAddress);

	LinPageUnLock(iPage, nPages, 0);
	PageDecommit(iPage, nPages, 0);
	PageFree( MEMHANDLE(ULONG(BaseAddress)&~0xFFF), 0 );
}

NAKED UCHAR NTAPI READ_REGISTER_UCHAR(
    PUCHAR  Register
    )
{
	_asm mov	eax, [esp+4]	// Register
	_asm mov	al, [eax]
	_asm ret	4
#if defined __BORLANDC__
	return 0; // never executed - suppresses warning
#endif
}


NAKED USHORT NTAPI READ_REGISTER_USHORT(
    PUSHORT Register
    )
{
	_asm mov	eax, [esp+4]	// Register
	_asm mov 	ax, [eax]
	_asm ret 	4
#if defined __BORLANDC__
	return 0; // never executed - suppresses warning
#endif
}

NAKED ULONG NTAPI READ_REGISTER_ULONG(
    PULONG  Register
    )
{
	_asm mov	eax, [esp+4]	// Register
	_asm mov 	eax, [eax]
	_asm ret 	4
#if defined __BORLANDC__
	return 0; // never executed - suppresses warning
#endif
}

VOID NTAPI READ_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    )
{
	memcpy(Buffer, Register, Count);
}

VOID NTAPI READ_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    )
{
	memcpy(Buffer, Register, Count*sizeof(USHORT));
}

VOID NTAPI READ_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    )
{
	memcpy(Buffer, Register, Count*sizeof(ULONG));
}

NAKED VOID NTAPI WRITE_REGISTER_UCHAR(
    PUCHAR  Register,
    UCHAR   Value
    )
{
	_asm mov	eax, [esp+4]	// Register
	_asm mov	cl, [esp+8]		// Value
	_asm mov	[eax], cl
	_asm ret	8
}

NAKED VOID NTAPI WRITE_REGISTER_USHORT(
    PUSHORT Register,
    USHORT  Value
    )
{
	_asm mov	eax, [esp+4]	// Register
	_asm mov	cx, [esp+8]		// Value
	_asm mov	[eax], cx
	_asm ret	8
}

NAKED VOID NTAPI WRITE_REGISTER_ULONG(
    PULONG  Register,
    ULONG   Value
    )
{
	_asm mov	eax, [esp+4]	// Register
	_asm mov	ecx, [esp+8]	// Value
	_asm mov	[eax], ecx
	_asm ret	8
}

VOID NTAPI WRITE_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    )
{
	memcpy(Register, Buffer, Count);
}

VOID NTAPI WRITE_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    )
{
	memcpy(Register, Buffer, Count*sizeof(USHORT));
}

VOID NTAPI WRITE_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    )
{
	memcpy(Register, Buffer, Count*sizeof(ULONG));
}

NAKED UCHAR NTAPI READ_PORT_UCHAR(
    PUCHAR  Port
    )
{
	_asm mov	dx, [esp+4]		// Port
	_asm in		al, dx
	_asm ret	4
#if defined __BORLANDC__
	return 0; // never executed - suppresses warning
#endif
}

NAKED USHORT NTAPI READ_PORT_USHORT(
    PUSHORT Port
    )
{
	_asm mov	dx, [esp+4]		// Port
	_asm in		ax, dx
	_asm ret	4
#if defined __BORLANDC__
	return 0; // never executed - suppresses warning
#endif
}

NAKED ULONG NTAPI READ_PORT_ULONG(
    PULONG  Port
    )
{
	_asm mov	dx, [esp+4]		// Port
	_asm in		eax, dx
	_asm ret	4
#if defined __BORLANDC__
	return 0; // never executed - suppresses warning
#endif
}

NAKED VOID NTAPI READ_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    )
{
	_asm push	edi
	_asm mov	edi, [esp+0xC]			// Buffer
	_asm mov 	ecx, [esp+0x10]			// Count
	_asm movzx	edx, WORD PTR [esp+8]	// Port
	_asm rep	insb
	_asm pop	edi
	_asm ret	0xC
}

NAKED VOID NTAPI READ_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    )
{
	_asm push	edi
	_asm mov	edi, [esp+0xC]			// Buffer
	_asm mov 	ecx, [esp+0x10]			// Count
	_asm movzx	edx, WORD PTR [esp+8]	// Port
	_asm rep	insw
	_asm pop	edi
	_asm ret	0xC
}

NAKED VOID NTAPI READ_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    )
{
	_asm push	edi
	_asm mov	edi, [esp+0xC]			// Buffer
	_asm mov 	ecx, [esp+0x10]			// Count
	_asm movzx	edx, WORD PTR [esp+8]	// Port
	_asm rep	insd
	_asm pop	edi
	_asm ret	0xC
}

NAKED VOID NTAPI WRITE_PORT_UCHAR(
    PUCHAR  Port,
    UCHAR   Value
    )
{
	_asm mov	al, [esp+8]		// Value
	_asm mov	dx, [esp+4]		// Port
	_asm out	dx, al
	_asm ret	8
}

NAKED VOID NTAPI WRITE_PORT_USHORT(
    PUSHORT Port,
    USHORT  Value
    )
{
	_asm mov	ax, [esp+8]		// Value
	_asm mov	dx, [esp+4]		// Port
	_asm out	dx, ax
	_asm ret	8
}

NAKED VOID NTAPI WRITE_PORT_ULONG(
    PULONG  Port,
    ULONG   Value
    )
{
	_asm mov	eax, [esp+8]	// Value
	_asm mov	dx, [esp+4]		// Port
	_asm out	dx, eax
	_asm ret	8
}

NAKED VOID NTAPI WRITE_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    )
{
	_asm push	esi
	_asm mov	ecx, [esp+0x10]			// Count
	_asm mov	esi, [esp+0xC]			// Buffer
	_asm movzx	edx, WORD PTR [esp+8]	// Port
	_asm rep 	outsb
	_asm pop	esi
	_asm ret	0xC
}

NAKED VOID NTAPI WRITE_PORT_BUFFER_USHORT(
    PUSHORT Port,	// +8
    PUSHORT Buffer,	// +0c
    ULONG   Count	// +10
    )
{
	_asm push	esi
	_asm mov	ecx, [esp+0x10]			// Count
	_asm mov	esi, [esp+0xC]			// Buffer
	_asm movzx	edx, WORD PTR [esp+8]	// Port
	_asm rep 	outsw
	_asm pop	esi
	_asm ret	0xC
}

NAKED VOID NTAPI WRITE_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    )
{
	_asm push	esi
	_asm mov	ecx, [esp+0x10]			// Count
	_asm mov	esi, [esp+0xC]			// Buffer
	_asm movzx	edx, WORD PTR [esp+8]	// Port
	_asm rep 	outsd
	_asm pop	esi
	_asm ret	0xC
}

BOOLEAN NTAPI HalTranslateBusAddress(
    IN INTERFACE_TYPE  InterfaceType,
    IN ULONG BusNumber,
    IN PHYSICAL_ADDRESS BusAddress,
    IN OUT PULONG AddressSpace,
    OUT PPHYSICAL_ADDRESS TranslatedAddress
    )
{
	*TranslatedAddress = BusAddress;
	return TRUE;
}


NTSTATUS NTAPI ZwOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )
{
	*SectionHandle = HANDLE(ObjectAttributes);
	return STATUS_SUCCESS;
}

NTSTATUS NTAPI ZwUnmapViewOfSection(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress
    )
{
	return 0;
}

NTSTATUS NTAPI ObReferenceObjectByHandle(                                      
    IN HANDLE Handle,                                           
    IN ACCESS_MASK DesiredAccess,                               
    IN POBJECT_TYPE ObjectType OPTIONAL,                        
    IN KPROCESSOR_MODE AccessMode,                              
    OUT PVOID *Object,                                          
    OUT POBJECT_HANDLE_INFORMATION HandleInformation OPTIONAL   
    )
{
	ASSERT ( HandleInformation == NULL );
	return STATUS_SUCCESS;
}

VOID NTAPI ObDereferenceObject(
    IN PVOID Object
    )
{
}

