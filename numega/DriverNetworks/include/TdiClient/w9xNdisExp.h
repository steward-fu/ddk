// w9xNdisExp.h
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
//=============================================================================
// This file forces EXPORT definitions for NDIS APIs
// mapped to macro-s in non-BINARY_COMPATIBLE builts.
// This is due to the fact that W9X TDI Clients built
// as non-BINARY_COMPATIBLE .SYS files (NDIS miniports and WDMs)
// include DDK's ndis.h, which treats NDIS_BUFFERs as MDLs using
// macros. This would cause smoke on W9X.

#undef NdisFreeBuffer
#undef NdisQueryBuffer
#undef NdisQueryBufferSafe
#undef NdisQueryBufferOffset
#undef NdisGetFirstBufferFromPacket
#undef NDIS_BUFFER_TO_SPAN_PAGES
#undef NdisGetBufferPhysicalArraySize

EXPORT
VOID
NdisFreeBuffer(
	IN	PNDIS_BUFFER			Buffer
	);

EXPORT
VOID
NdisQueryBuffer(
	IN	PNDIS_BUFFER			Buffer,
	OUT PVOID *					VirtualAddress OPTIONAL,
	OUT PUINT					Length
	);

EXPORT
VOID
NdisQueryBufferSafe(
	IN	PNDIS_BUFFER			Buffer,
	OUT PVOID *					VirtualAddress OPTIONAL,
	OUT PUINT					Length,
	IN	UINT					Priority
	);

EXPORT
VOID
NdisQueryBufferOffset(
	IN	PNDIS_BUFFER			Buffer,
	OUT PUINT					Offset,
	OUT PUINT					Length
	);

VOID
NdisGetFirstBufferFromPacket(
	IN	PNDIS_PACKET			Packet,
	OUT PNDIS_BUFFER *			FirstBuffer,
	OUT PVOID *					FirstBufferVA,
	OUT PUINT					FirstBufferLength,
	OUT	PUINT					TotalBufferLength
	);

EXPORT
ULONG
NDIS_BUFFER_TO_SPAN_PAGES(
	IN	PNDIS_BUFFER				Buffer
	);

EXPORT
VOID
NdisGetBufferPhysicalArraySize(
	IN	PNDIS_BUFFER				Buffer,
	OUT PUINT						ArraySize
	);
