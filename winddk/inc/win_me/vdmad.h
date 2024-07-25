/*****************************************************************************
 *
 *   (C) Copyright MICROSOFT Corp., 1988-1990
 *
 *   Title: VDMAD.H - Include file for Virtual Machine Manager
 *
 *   Version:   1.00
 *
 *   Date:  05-May-1988
 *
 *   Author:    RAP
 *
 *-----------------------------------------------------------------------------
 *
 *   Change log:
 *
 *   DATE    REV DESCRIPTION
 *   ----------- --- -----------------------------------------------------------
 *   16-Dec-1986 RAP Original
 *   28-Nov-1988 RAP v 2.0
 *   18-Aug-1995 BAW C version
 */

#ifndef _VDMAD_H_
#define _VDMAD_H_

#ifndef  Not_VxD

/*XLATOFF*/
#define VDMAD_Service Declare_Service
#pragma warning (disable:4003)      // turn off not enough params warning
/*XLATON*/

/*MACROS*/
Begin_Service_Table(VDMAD, VxD)

VDMAD_Service(VDMAD_Get_Version, LOCAL)
VDMAD_Service(VDMAD_Virtualize_Channel, LOCAL)
VDMAD_Service(VDMAD_Get_Region_Info, LOCAL)
VDMAD_Service(VDMAD_Set_Region_Info, LOCAL)
VDMAD_Service(VDMAD_Get_Virt_State, LOCAL)
VDMAD_Service(VDMAD_Set_Virt_State, LOCAL)
VDMAD_Service(VDMAD_Set_Phys_State, LOCAL)
VDMAD_Service(VDMAD_Mask_Channel, LOCAL)
VDMAD_Service(VDMAD_UnMask_Channel, LOCAL)
VDMAD_Service(VDMAD_Lock_DMA_Region, LOCAL)
VDMAD_Service(VDMAD_Unlock_DMA_Region, LOCAL)
VDMAD_Service(VDMAD_Scatter_Lock, LOCAL)
VDMAD_Service(VDMAD_Scatter_Unlock, LOCAL)
VDMAD_Service(VDMAD_Reserve_Buffer_Space, LOCAL)
VDMAD_Service(VDMAD_Request_Buffer, LOCAL)
VDMAD_Service(VDMAD_Release_Buffer, LOCAL)
VDMAD_Service(VDMAD_Copy_To_Buffer, LOCAL)
VDMAD_Service(VDMAD_Copy_From_Buffer, LOCAL)
VDMAD_Service(VDMAD_Default_Handler, LOCAL)
VDMAD_Service(VDMAD_Disable_Translation, LOCAL)
VDMAD_Service(VDMAD_Enable_Translation, LOCAL)
VDMAD_Service(VDMAD_Get_EISA_Adr_Mode, LOCAL)
VDMAD_Service(VDMAD_Set_EISA_Adr_Mode, LOCAL)
VDMAD_Service(VDMAD_Unlock_DMA_Region_No_Dirty, LOCAL)

/*
 * the following services are new for version 4.0 of VDMAD
 */

VDMAD_Service(VDMAD_Phys_Mask_Channel, LOCAL)
VDMAD_Service(VDMAD_Phys_Unmask_Channel, LOCAL)
VDMAD_Service(VDMAD_Unvirtualize_Channel, LOCAL)
VDMAD_Service(VDMAD_Set_IO_Address)
VDMAD_Service(VDMAD_Get_Phys_Count, LOCAL)
VDMAD_Service(VDMAD_Get_Phys_Status, LOCAL)
VDMAD_Service(VDMAD_Get_Max_Phys_Page, LOCAL)
VDMAD_Service(VDMAD_Set_Channel_Callbacks, LOCAL)
VDMAD_Service(VDMAD_Get_Virt_Count, LOCAL)
VDMAD_Service(VDMAD_Set_Virt_Count, LOCAL)
VDMAD_Service(VDMAD_Get_Virt_Address, LOCAL)
VDMAD_Service(VDMAD_Set_Virt_Address, LOCAL)

End_Service_Table(VDMAD, VxD)
/*ENDMACROS*/

#define Function_Mask   0xF0
#define Channel_Mask    0x07

/*
 *  mode bits
 */

#define DMA_type_verify   0x00
#define DMA_type_write    0x04
#define DMA_type_read     0x08
#define DMA_AutoInit      0x10
#define DMA_AdrDec        0x20

#define DMA_demand_mode   0x00
#define DMA_single_mode   0x40
#define DMA_block_mode    0x80
#define DMA_cascade       0xC0

#define DMA_mode_mask     0xC0 /* mask to isolate controller mode bits (above) */

#define DMA_chan_sel      0x03

/*
 *  state flag bits (overlay the unused channel selection bits)
 */

#define DMA_masked        0x01
#define DMA_masked_bit    0
#define DMA_requested     0x02
#define DMA_req_bit       1

/*
 *  extended mode bits
 */

#define Programmed_IO     0x01
#define PS2_AutoInit      0x02
#define Transfer_Data     0x04
#define Write_Mem         0x08
#define _16_bit_xfer      0x40
#define _16_bit_xfer_bit  6

#endif /* Not_VxD */

struct DMA_Descriptor_Struc{
   ULONG  DDS_size ;
   ULONG  DDS_linear ;
   WORD   DDS_seg ;
   WORD   DDS_bufferID ;
   ULONG  DDS_physical ;
} ;

/* ASM
DDS_sel equ word ptr [DDS_seg]
*/

struct Extended_DDS_Struc{
   ULONG  EDDS_size ;
   ULONG  EDDS_linear ;
   WORD   EDDS_seg ;
   WORD   EDDS_reserved ;
   WORD   DDS_avail ;
   WORD   DDS_used ;
} ;

#define DMA_Buf_Copy            0x00000002   /* set in copy into/out of buffer */
#define DMA_Buf_Copy_bit        1            /* required in lock or unlock     */
#define DMA_No_Alloc_Buf        0x00000004   /* set if buffer should not be    */
#define DMA_No_Alloc_Buf_bit    2            /* alloc'ed if lock not possible  */
#define DMA_Align_64K           0x00000010   /* set if lock must not cross     */
#define DMA_Align_64K_bit       4            /* 64k page boundary              */
#define DMA_Align_128K          0x00000020   /* set if lock must not cross     */
#define DMA_Align_128K_bit      5            /* 128k page boundary             */
#define DMA_Get_PgTable         0x00000040   /* set if page table copy for     */
#define DMA_Get_PgTable_bit     6            /* scatter/gather lock            */
#define DMA_Allow_NPs           0x00000080   /* set if not present pages are   */
#define DMA_Allow_NPs_bit       7            /* allowed in scatter/gather lock */
                                             /* when bit 6 also set            */
#define DMA_Not_Contiguous          0x01
#define DMA_Not_Aligned             0x02
#define DMA_Lock_Failed             0x03
#define DMA_No_Buffer               0x04
#define DMA_Buffer_Too_Small        0x05
#define DMA_Buffer_In_Use           0x06
#define DMA_Invalid_Region          0x07
#define DMA_Region_Not_Locked       0x08
#define DMA_Table_Too_Small         0x09
#define DMA_Invalid_Buffer          0x0A
#define DMA_Copy_Out_Range          0x0B
#define DMA_Invalid_Channel         0x0C
#define DMA_Disable_Cnt_Overflow    0x0D
#define DMA_Disable_Cnt_Underflow   0x0E
#define DMA_Func_Not_Supported      0x0F
#define DMA_NonZero_Reserved_Flags  0x10

/*
 *  Flags definitions for VDMAD_Scatter_Lock and VDMAD_Scatter_Unlock
 */

#define DMA_SL_Get_PgTable      0x000000001 /* set if page table copy for     */
#define DMA_SL_Get_PgTable_bit  0           /* scatter/gather lock            */
#define DMA_SL_Allow_NPs        0x000000002 /* set if not present pages are   */
#define DMA_SL_Allow_NPs_bit    1           /* allowed in scatter/gather lock */
                                            /* when bit 0 also set            */
#define DMA_SL_Dont_Dirty       0x000000004 /* set if pages should NOT be     */
#define DMA_SL_Dont_Dirty_bit   2           /* marked as dirty for            */
                                            /* VDMAD_Scatter_Unlock           */


#define VDMAD_Callbacks_ReadNotify  0x00000001 /* extended read notify */
#define VDMAD_Callbacks_AllUnmaskedWritesNotify	0x00000002
#define VDMAD_Callbacks_EDXhasportinfo 0x00000004
#define VDMAD_Callbacks_CntrlrStatus 0x00000008
#define VDMAD_ForcedCallbacks       0x00008000
#define VDMAD_Callbacks_Valid       0x0000800f

#define VDMAD_Callback_ReadCount    0x00000001
#define VDMAD_Callback_ReadPage     0x00000002
#define VDMAD_Callback_ReadBase     0x00000003

/*XLATOFF*/

// data types

#ifndef DECLARE_HANDLE
#ifdef STRICT
#define DECLARE_HANDLE(name)    struct name##__ { int unused; }; \
                                typedef const struct name##__ * name
#else   /* STRICT */
#define DECLARE_HANDLE(name)    typedef DWORD name
#endif  /* !STRICT */
#endif

DECLARE_HANDLE( HDMA ) ;

/*XLATON*/

#endif /* _VDMAD_H_ */
