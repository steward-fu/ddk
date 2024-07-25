// E100bexSw.h: - Software Data structures and symbolic constants.
//
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
//
// Portions copied from Microsoft Windows 2000 DDK sample driver containing the
// following copyright
//
/****************************************************************************
** COPYRIGHT (C) 1994-1997 INTEL CORPORATION                               **
** DEVELOPED FOR MICROSOFT BY INTEL CORP., HILLSBORO, OREGON               **
** HTTP://WWW.INTEL.COM/                                                   **
** THIS FILE IS PART OF THE INTEL ETHEREXPRESS PRO/100B(TM) AND            **
** ETHEREXPRESS PRO/100+(TM) NDIS 5.0 MINIPORT SAMPLE DRIVER               **
****************************************************************************/


#if !defined(AFX_E100BEXSW_H__0E5098B0_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
#define AFX_E100BEXSW_H__0E5098B0_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// MP_TCB flags
#define fMP_TCB_IN_USE                         0x00000001
#define fMP_TCB_USE_LOCAL_BUF                  0x00000002
#define fMP_TCB_MULTICAST                      0x00000004  // a hardware workaround using multicast

// MP_RFD flags
#define fMP_RFD_RECV_PEND                      0x00000001
#define fMP_RFD_ALLOC_PEND                     0x00000002
#define fMP_RFD_RECV_READY                     0x00000004
#define fMP_RFD_RESOURCES                      0x00000008

// MP_ADAPTER flags
#define fMP_ADAPTER_SCATTER_GATHER             0x00000001
#define fMP_ADAPTER_RECV_LOOKASIDE             0x00000002
#define fMP_ADAPTER_INTERRUPT_IN_USE           0x00000004
#define fMP_ADAPTER_SECONDARY                  0x00000008

#if OFFLOAD
// MP_ SHARED flags
#define fMP_SHARED_MEM_IN_USE                  0x00000100
#endif

#define fMP_ADAPTER_NON_RECOVER_ERROR          0x00800000
#define fMP_ADAPTER_RESET_IN_PROGRESS          0x01000000
#define fMP_ADAPTER_NO_CABLE                   0x02000000
#define fMP_ADAPTER_HARDWARE_ERROR             0x04000000
#define fMP_ADAPTER_REMOVE_IN_PROGRESS         0x08000000
#define fMP_ADAPTER_HALT_IN_PROGRESS           0x10000000
#define fMP_ADAPTER_LINK_DETECTION             0x20000000

#define fMP_ADAPTER_FAIL_SEND_MASK             0x1ff00000
#define fMP_ADAPTER_NOT_READY_MASK             0x3ff00000


////////////////////////////////////////////////////////////////////
// StallExecution
//	This global helper stalls the CPU processor for the indicated msec.
// Parameters:
//		nMsecDelay - the number of milliseconds to delay
// IRQL:
//		Any
// Return Mode:
//		Synchronous
// NOTE
//		This method is a wrapper for NdisStallExecution.  It is called in
//		100 usec increments.
//
inline VOID StallExecution(UINT nMsecDelay)
{
	nMsecDelay *= 10;
	while (nMsecDelay--)
		// Delay in 100 usec increments
		NdisStallExecution(100);
}


//-------------------------------------------------------------------------
// NON_TRANSMIT_CB -- Generic Non-Transmit Command Block
//-------------------------------------------------------------------------
typedef struct _NON_TRANSMIT_CB
{
	union
	{
		MULTICAST_CB_STRUC  Multicast;
		CONFIG_CB_STRUC     Config;
		IA_CB_STRUC         Setup;
		DUMP_CB_STRUC       Dump;
	}   NonTxCb;

} NON_TRANSMIT_CB, *PNON_TRANSMIT_CB;


//-------------------------------------------------------------------------
// COALESCE -- This structure describes a coalesce buffer resource
//-------------------------------------------------------------------------
typedef struct _COALESCE
{
	LIST_ENTRY			m_ListEntry;
	PVOID				OwningTcb;
	KNdisBuffer			NdisBuffer;
	KNdisSharedMemory	*pAllocMem;	// AllocVa and AllocPa
	ULONG				BufferSize;

} COALESCE, *PCOALESCE;


//-------------------------------------------------------------------------
// D100SwTcb -- Software Transmit Control Block.  This structure contains
// all of the variables that are related to a specific Transmit Control
// block (TCB)
//-------------------------------------------------------------------------
typedef struct _D100SwTcb
{
	// Link to the next SwTcb in the list
	LIST_ENTRY			m_ListEntry;

	// CheckForHang
	ULONG				Count;

	// The NDIS packet that this TCB is sending
	PNDIS_PACKET        Packet;

	// virtual and physical pointers to the hardware TCB
	PTXCB_STRUC         HwTcb;            // ptr to HW TCB VA
	ULONG               HwTcbPhys;        // ptr to HW TCB PA
	// virtual pointer to the previous hardware TCB in the chain
	PTXCB_STRUC         PrevHwTcb;        // ptr to previous HW TCB VA

	// virtual and physical pointers to the TBD array for this TCB
	PTBD_STRUC          HwTbd;
	ULONG               HwTbdPhys;

	// When a virtual buffer is decomposed into it's constituent
	// physical vectors, each vector is stored here.
	UINT                PhysBufCount;
	NDIS_PHYSICAL_ADDRESS_UNIT PhysDesc[NIC_MAX_PHYS_BUF_COUNT];

	// If there are more physical segments then map registers,
	// then use a coalesce buffer.
	PCOALESCE			MpTxBuf;
	UINT                CoalesceBufferLen;

	// Describes the length of the packet as sent by the protocol.
	UINT                PacketLength;

	// Pointer to the first buffer in the packet
	PNDIS_BUFFER        FirstBuffer;

	// The number of buffers that the packet has
	UINT                BufferCount;

#if DBG
	UINT                TcbNum;
#endif

} D100SwTcb, *PD100SwTcb;


// Each entry in the error log will be tagged with a unique event code so that
// we'll be able to grep the driver source code for that specific event, and
// get an idea of why that particular event was logged.  Each time a new
// "D100LogError" statement is added to the code, a new Event tag should be
// added below.
typedef enum _D100_EVENT_VIEWER_CODES
{
		EVENT_0,                    // couldn't register the specified interrupt
		EVENT_1,                    // One of our PCI cards didn't get required resources
		EVENT_2,                    // bad node address (it was a multicast address)
		EVENT_3,                    // failed self-test
		EVENT_4,                    // Wait for SCB failed
		EVENT_5,                    // NdisRegisterAdapter failed for the MAC driver
		EVENT_6,                    // WaitSCB failed
		EVENT_7,                    // Command complete status was never posted to the SCB
		EVENT_8,                    // Couldn't find a phy at over-ride address 0
		EVENT_9,                    // Invalid duplex or speed setting with the detected phy
		EVENT_10,                   // Couldn't memory map the CSR.
		EVENT_11,                   // couldn't allocate enough map registers
		EVENT_12,                   // couldn't allocate enough recv cached memory
		EVENT_13,                   // couldn't allocate enough recv uncached shared memory
		EVENT_14,                   // couldn't allocate enough xmit cached memory
		EVENT_15,                   // couldn't allocate enough cb uncached shared memory
		EVENT_16,                   // Didn't find any PCI boards
		EVENT_17,          // 11    // Multiple PCI were found, but none matched our id.
		EVENT_18,          // 12    // NdisMPciAssignResources Error
		EVENT_19,          // 13    // Didn't Find Any PCI Boards that matched our subven/subdev
		EVENT_20,          // 14    // ran out of cached memory to allocate in async allocation
		EVENT_21,          // 15    // NdisMInitializeScatterGatherDma failed
		EVENT_30           // 1e    // WAIT_TRUE timed out
} D100_EVENT_VIEWER_CODES;


#endif // !defined(AFX_E100BEXSW_H__0E5098B0_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)

