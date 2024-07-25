/*
 *
 *   (C) Copyright MICROSOFT Corp. 1986-1990
 *   (C) Copyright Compuware Corporation, 1994
 *
 */

#ifndef __vdmad_h_
#define __vdmad_h_


#define	Function_Mask   0x0F0
#define	Channel_Mask	0x07

//
// mode bits
//

#define	DMA_type_verify	0x00
#define	DMA_type_write	0x04
#define	DMA_type_read	0x08
#define	DMA_AutoInit	0x10
#define	DMA_AdrDec	0x20

#define	DMA_demand_mode	0x00
#define	DMA_single_mode	0x40
#define	DMA_block_mode	0x80
#define	DMA_cascade	0xc0

#define	DMA_mode_mask	0xc0   // mask to isolate controller mode bits (above)

#define	DMA_chan_sel	0x03

// state flag bits (overlay the unused channel selection bits)
#define	DMA_masked	0x01
#define	DMA_masked_bit	0
#define	DMA_requested	0x02
#define	DMA_req_bit	1


// extended mode bits
#define	Programmed_IO	0x01
#ifdef WIN40
#define	PS2_AutoInit	0x02
#endif
#define	Transfer_Data	0x04
#define	Write_Mem	0x08
#define	_16_bit_xfer	0x40
#define	_16_bit_xfer_bit	6


typedef struct DMA_Descriptor_Struc {
	DWORD	DDS_size;
	DWORD	DDS_linear;
	WORD	DDS_seg;
	WORD	DDS_bufferID;
	DWORD	DDS_physical;
} DMA_DESCRIPTOR, *PDMA_DESCRIPTOR;

//#define	DDS_sel word ptr [DDS_seg]

typedef struct Extended_DDS_Struc {
	DWORD	DDS_size;
	DWORD	DDS_linear;
	WORD	DDS_seg;
	WORD	RESERVED;
	WORD	DDS_avail;
	WORD	DDS_used;
} EXTENDED_DDS, *PEXTENDED_DDS;


#define	DMA_Buf_Copy		0x02	// set in copy into/out of buffer
#define	DMA_Buf_Copy_bit	1	// required in lock or unlock
#define	DMA_No_Alloc_Buf	0x04	// set if buffer should not be
#define	DMA_No_Alloc_Buf_bit	2	// alloc'ed if lock not possible
#define	DMA_Align_64K		0x10	// set if lock must not cross
#define	DMA_Align_64K_bit	4	// 64k page boundary
#define	DMA_Align_128K		0x20	// set if lock must not cross
#define	DMA_Align_128K_bit	5 	// 128k page boundary
#define	DMA_Get_PgTable		0x40	// set if page table copy for
#define	DMA_Get_PgTable_bit	6	// scatter/gather lock
#define	DMA_Allow_NPs		0x80	// set if not present pages are
#define	DMA_Allow_NPs_bit	7		// allowed in scatter/gather lock
						// when bit 6 also set

#define	DMA_Not_Contiguous	0x01
#define	DMA_Not_Aligned		0x02
#define	DMA_Lock_Failed		0x03
#define	DMA_No_Buffer		0x04
#define	DMA_Buffer_Too_Small	0x05
#define	DMA_Buffer_In_Use	0x06
#define	DMA_Invalid_Region	0x07
#define	DMA_Region_Not_Locked	0x08
#define	DMA_Table_Too_Small	0x09
#define	DMA_Invalid_Buffer	0x0A
#define	DMA_Copy_Out_Range	0x0B
#define	DMA_Invalid_Channel	0x0C
#define	DMA_Disable_Cnt_Overflow 0x0D
#define	DMA_Disable_Cnt_Underflow 0x0E
#define	DMA_Func_Not_Supported	0x0F
#define	DMA_NonZero_Reserved_Flags 0x10


//
// Flags definitions for VDMAD_Scatter_Lock and VDMAD_Scatter_Unlock
//
#define	DMA_SL_Get_PgTable	0x0001	// set if page table copy for
#define	DMA_SL_Get_PgTable_bit	0	// scatter/gather lock
#define	DMA_SL_Allow_NPs	0x0002	// set if not present pages are
#define	DMA_SL_Allow_NPs_bit	1	// allowed in scatter/gather lock
						// when bit 0 also set
#define	DMA_SL_Dont_Dirty	0x0004	// set if pages should NOT be
#define	DMA_SL_Dont_Dirty_bit	2	// marked as dirty for
						// VDMAD_Scatter_Unlock

#define VDMAD_Service Declare_Service

Begin_Service_Table (VDMAD)

VDMAD_Service	(VDMAD_Get_Version)
VDMAD_Service	(VDMAD_Virtualize_Channel)
VDMAD_Service	(VDMAD_Get_Region_Info)
VDMAD_Service	(VDMAD_Set_Region_Info)
VDMAD_Service	(VDMAD_Get_Virt_State)
VDMAD_Service	(VDMAD_Set_Virt_State)
VDMAD_Service	(VDMAD_Set_Phys_State)
VDMAD_Service	(VDMAD_Mask_Channel)
VDMAD_Service	(VDMAD_UnMask_Channel)
VDMAD_Service	(VDMAD_Lock_DMA_Region)
VDMAD_Service	(VDMAD_Unlock_DMA_Region)
VDMAD_Service	(VDMAD_Scatter_Lock)
VDMAD_Service	(VDMAD_Scatter_Unlock)
VDMAD_Service	(VDMAD_Reserve_Buffer_Space)
VDMAD_Service	(VDMAD_Request_Buffer)
VDMAD_Service	(VDMAD_Release_Buffer)
VDMAD_Service	(VDMAD_Copy_To_Buffer)
VDMAD_Service	(VDMAD_Copy_From_Buffer)
VDMAD_Service	(VDMAD_Default_Handler)
VDMAD_Service	(VDMAD_Disable_Translation)
VDMAD_Service	(VDMAD_Enable_Translation)
VDMAD_Service	(VDMAD_Get_EISA_Adr_Mode)
VDMAD_Service	(VDMAD_Set_EISA_Adr_Mode)
VDMAD_Service	(VDMAD_Unlock_DMA_Region_No_Dirty)
VDMAD_Service	(VDMAD_Phys_Mask_Channel)
VDMAD_Service	(VDMAD_Phys_Unmask_Channel)
VDMAD_Service   (VDMAD_Unvirtualize_Channel)
VDMAD_Service	(VDMAD_Set_IO_Address)
VDMAD_Service   (VDMAD_Get_Phys_Count)
VDMAD_Service   (VDMAD_Get_Phys_Status)
VDMAD_Service   (VDMAD_Get_Max_Phys_Page)
VDMAD_Service   (VDMAD_Set_Channel_Callbacks)
VDMAD_Service   (VDMAD_Get_Virt_Count)
VDMAD_Service   (VDMAD_Set_Virt_Count)

End_Service_Table (VDMAD)

#define VDMAD_CALLBACKS_READNOTIFY  1 // extended read notify
#define VDMAD_CALLBACKS_VALID       1

#define VDMAD_CALLBACK_READCOUNT    1
#define VDMAD_CALLBACK_READPAGE		2
#define VDMAD_CALLBACK_READBASE     3

#endif		// __vdmad_h_
