/*
	Pageswap.h

   (C) Copyright MICROSOFT Corp., 1988-1990
   (C) Copyright Compuware Corporation, 1994

*/

#ifndef __pageswap_h_
#define __pageswap_h_

#ifdef WIN40
// Buffer descriptor for asynchronous I/O

typedef PAGESWAPBUFFERDESC_tag {
	DWORD	PS_BD_Next;
	BYTE	PS_BD_Cmd;
	BYTE	PS_BD_Priority;
	BYTE	PS_BD_Status;
	BYTE	PS_BD_nPages;
	DWORD	PS_BD_Buffer_Ptr;
	DWORD	PS_BD_File_Page;
	DWORD	PS_BD_Call_Back;
	DWORD	PS_BD_Semaphore;
} PAGESWAPBUFFERDESC;

#endif

#ifdef WIN31
#define	PS_ZERO_INIT_MASK	0x0001
#define	PS_ZERO_INIT_BIT	0x00
#define	PS_FIXED_PAGE_MASK	0x0008
#define	PS_FIXED_PAGE_BIT	0x03
#define	PS_FIRST_PAGE_IN_MASK	0x0020
#define	PS_FIRST_PAGE_IN_BIT	0x05
#define	PS_DIRTY_MASK		0x0040
#define	PS_DIRTY_BIT		0x06
#define	PS_EVER_DIRTY_MASK	0x8000
#define	PS_EVER_DIRTY_BIT	0x0F

#define	PS_NoXchg_Mask	0x80000000
#define	PS_NoXchg_Bit	0x1F	// DO NOT xchg phys addrs on PageSwap_Out


// Buffer descriptor for asynchronous I/O

#define	SIZE_PS_BD_RESERVED 44

typedef PAGESWAPBUFFERDESC_tag {
	DWORD	PS_BD_Next;
	BYTE	PS_BD_Cmd;
	BYTE	PS_BD_Priority;
	BYTE	PS_BD_Status;
	BYTE	PS_BD_nPages;
	DWORD	PS_BD_Buffer_Ptr;
	DWORD	PS_BD_File_Page;
	DWORD	PS_BD_Call_Back;
	BYTE	PS_BD_Reserved[SIZE_PS_BD_RESERVED];
} PAGESWAPBUFFERDESC;

// List values for PS_BD_List

#define	PS_BDL_FREE		1
#define	PS_BDL_PENDING_WB	2
#define	PS_BDL_SUBMITTED_RA	3
#define	PS_BDL_LOCKED		4

#define	PS_BDP_INVALID_DATA	(-1)

#endif


#endif			//  __pageswap_h_
