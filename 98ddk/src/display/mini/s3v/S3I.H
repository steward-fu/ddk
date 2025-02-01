#ifndef _S3I_H
#define _S3I_H

//----------------------------------------------------------------------------
// S3.INC
// Copyright (c) 1992 S3 Inc.
//----------------------------------------------------------------------------
//---------------------------------------------------------------------------
// E Q U A T E S
//---------------------------------------------------------------------------
// Following are equates for the various VGA ports:

#define INPUT_STATUS_1_REG      0x3da
#define ATTRIBUTE_REG           0x3c0
#define ATTRIBUTE_READ_REG      0x3c1
#define MISC_OUT_REG            0x3c2
#define MISC_OUT_READ_REG       0x3cc
#define FEATURE_REG             0x3da
#define FEATURE_READ_REG        0x3ca
#define CRTC_REG                0x3d4
#define SEQ_REG                 0x3c4
#define GCR_REG                 0x3ce
#define PEL_READ_REG            0x3c7
#define PEL_WRITE_REG           0x3c8
#define PEL_DATA_REG            0x3c9
#define PAGING_REG_INDEX        0x35            // specific to the S3-911

#define MULTILFUNCTION_CONTROL 0xbee8
#define ENABLE_COLOR_COMPARE   0xe100
#define DISABLE_COLOR_COMPARE  0xe000
#define FORGROUND_SELECTED     0xa000
#define TOP_TO_BOTTOM          0x00a0
#define BOTTOM_TO_TOP          0x0000
#define SCR_COPY_MIX           ( DP_MIX_S & DP_FRGD_MIX ) | ( DP_MIX_D & DP_BKGD_MIX )
#define COLOR_FILL_MIX         ( DP_MIX_S & DP_FRGD_MIX ) | ( DP_MIX_F & DP_BKGD_MIX )
#define SCRN_SCRN_BLT          0xc053
#define COLOR_FILL_COMMAND     0x40b3

#define VBLANK_MASK            0x08            // bit set => vertical retrace
#define DISPLAY_MODE_INACTIVE  0x01

// If driver is being assembled for the regular address space ALTREGMASK is
// defined = 0, else it is defined as 3A0h to yield the alternate port address.

// ALT_REGS are broke on the 928
//#define ALT_REG_MASK            0x3a0h
#define ALT_REG_MASK            0


//
// ViRGE Graphics Engine Registers Offset and masks
//

#define VI_DATA_PORT_1ST        0x0000              // first data transfer port
#define VI_DATA_PORT_LAST       0x7FFF              // last data transfer port
#define VI_SRC_BASE             0xA4D4
#define VI_DEST_BASE            0xA4D8
#define VI_CLIP_L_R             0xA4DC
#define VI_CLIP_T_B             0xA4E0
#define VI_DEST_SRC_STR         0xA4E4
#define VI_PAT_FG_CLR           0xA4F4
#define VI_SRC_BG_CLR           0xA4F8
#define VI_SRC_FG_CLR           0xA4FC
#define VI_2D_CMD_SET           0xA500
#define     VI_2DCMD_AE         0x01                // autoexecute
#define     VI_2DCMD_HC         0x02                // hardware clipping
#define     VI_2DCMD_DCLR       0x1c                // destination color format
#define     VI_2DCMD_DE         0x20                // draw enable
#define     VI_2DCMD_MS         0x40                // mono source
#define     VI_2DCMD_IDS        0x80                // image data source
#define         VI_IDS_VMEM     0                   // from video memory
#define         VI_IDS_CPU      0x80                // frp, CPU
#define     VI_2DCMD_MP         0x100               // mono pattern
#define     VI_2DCMD_TP         0x200               // transparent
#define     VI_2DCMD_ITA        0xc00               // image transfer alignment
#define         VI_ITA_BYTE     0                   // byte aligned
#define         VI_ITA_WORD     0x400               // word aligned
#define         VI_ITA_DWORD    0x800               // dword aligned
#define     VI_2DCMD_FDO        0x3000              // first DWORD offset
#define     VI_2DCMD_ROPS       0x01fe0000L
#define     VI_2DCMD_XP         0x02000000L         // x positive
#define     VI_2DCMD_YP         0x04000000L         // y positive
#define     VI_2DCMD_CMD        0x78000000L         // 2d command
#define         VI_CMD_BITBLT   0x00000000L         // 2D bitBlt
#define         VI_CMD_RECTFILL 0x10000000L         // 2D rectangle fill
#define         VI_CMD_LINEDRAW 0x18000000L         // 2D line draw
#define         VI_CMD_POLYFILL 0x28000000L         // 2D polygon fill
#define         VI_CMD_NOP      0x78000000L         // nop
#define     VI_2DCMD_23D        0x80000000L         // 2D(0) or 3D(1) command
#define VI_RWIDTH_HEIGHT        0xA504
#define VI_RSRC_XY              0xA508
#define VI_RDEST_XY             0xA50C


// The S3-911 Enhanced Registers:


// As defined in table 11-1 (pg 11-6) of Vision964 manual
#define	BKGND_COLOR	0xa2e8
#define	BKGND_MIX	0xb6e8
#define	CMD		0x9ae8
#define	COLOR_CMP	0xb2e8
#define	CUR_X		0x86e8
#define	CUR_Y		0x82e8
#define	DESTY_AXSTP	0x8ae8
#define	DESTX_DIASTP	0x8ee8
#define	ERR_TERM	0x92e8
#define	FRGD_COLOR	0xa6e8
#define	FRGD_MIX	0xbae8
#define	GP_STAT		0x9ae8
#define	MAJ_AXIS_PCNT	0x96e8
#define	MIN_AXIS_PCNT	0xbee8
#define	PIXEL_CNTL	0xbee8
#define	PIX_TRANS	0xe2e8		// also 0xe2ea for 32-bit
#define	RD_MASK		0xaee8
#define	SHORT_STROKE	0x9ee8
#define	WRT_MASK    	0xaae8

#define	MMIO_BASE	0xa0000

#define	MIX_NEW		   00111b
#define	MIX_XOR		   00101b
#define	COLOR_FGRD	00100000b
#define	COLOR_DSPLY	01100000b


#define CLIPLEFT        0x20
#define CLIPTOP         0x10
#define CLIPRIGHT       0x40
#define CLIPBOTTOM      0x30

// Some 8514 Commands:

#define USER_DEFINED_PATTERN    0xa080
#define BLOCK_MOVE              0xc0f3
// CursorType
#define SOFTWARECURSOR  0
#define HARDWARECURSOR  1


// Here are equates related to FIFO space:

#define ONE_WORD                0x0080
#define TWO_WORDS               0x0040
#define THREE_WORDS             0x0020
#define FOUR_WORDS              0x0010
#define FIVE_WORDS              0x0008
#define SIX_WORDS               0x0004
#define SEVEN_WORDS             0x0002
#define EIGHT_WORDS             0x0001

#define NINE_WORDS              0x8000
#define TEN_WORDS               0x4000
#define ELEVEN_WORDS            0x2000
#define TWELVE_WORDS            0x1000
#define THIRTEEN_WORDS          0x0800

#define HDW_BUSY		0x0200


// For macro expansion -- FIFO_EMPTY_##x
#define FIFO_EMPTY_1		ONE_WORD
#define FIFO_EMPTY_2		TWO_WORDS
#define FIFO_EMPTY_3		THREE_WORDS
#define FIFO_EMPTY_4		FOUR_WORDS
#define FIFO_EMPTY_5		FIVE_WORDS
#define FIFO_EMPTY_6		SIX_WORDS
#define FIFO_EMPTY_7		SEVEN_WORDS
#define FIFO_EMPTY_8		EIGHT_WORDS
#define FIFO_EMPTY_9		NINE_WORDS
#define FIFO_EMPTY_10		TEN_WORDS
#define FIFO_EMPTY_11		ELEVEN_WORDS
#define FIFO_EMPTY_12		TWELVE_WORDS
#define FIFO_EMPTY_13		THIRTEEN_WORDS

//=============================================================
//
// DST Shifts
//

#define SHIFT_DST_PITCH 22  // DST_OFF_PITCH
#define SHIFT_DST_X     16  // DST_Y_X
#define SHIFT_DST_WIDTH 16  // DST_HEIGHT_WIDTH DST_X_WIDTH

//
// DST_WIDTH
//

#define DST_WIDTH_FILL_DIS 0x80000000L

//
// DST_BRES_LNTH
//

#define DST_BRES_LNTH_LINE_DIS 0x80000000L

//
// DST_CNTL
//

#define DST_X_DIR      0x00000001L
#define DST_Y_DIR      0x00000002L
#define DST_Y_MAJOR    0x00000004L
#define DST_X_TILE     0x00000008L
#define DST_Y_TILE     0x00000010L
#define DST_LAST_PEL   0x00000020L
#define DST_POLYGON_EN 0x00000040L
#define DST_24_ROT_EN  0x00000080L
#define DST_24_ROT     0x00000700L
#define DST_BRES_SIGN  0x00000800L

//
//  SRC Shifts
//

#define SHIFT_SRC_PITCH   22  // SRC_OFF_PITCH
#define SHIFT_SRC_X       16  // SRC_Y_X
#define SHIFT_SRC_WIDTH1  16  // SRC_HEIGHT1_WIDTH1
#define SHIFT_SRC_X_START 16  // SRC_Y_X_START
#define SHIFT_SRC_WIDTH2  16  // SRC_HEIGHT2_WIDTH2

//
// SRC_CNTL
//

#define SRC_PATT_EN     0x00000001L
#define SRC_PATT_ROT_EN 0x00000002L
#define SRC_LINEAR_EN   0x00000004L
#define SRC_BYTE_ALIGN  0x00000008L
#define SRC_LINE_X_DIR  0x00000010L

//
// PAT_CNTL
//

#define PAT_MONO_EN    0x00000001L
#define PAT_CLR_4x2_EN 0x00000002L
#define PAT_CLR_8x1_EN 0x00000004L

//
// SC Shifts
//

#define SHIFT_SC_RIGHT  16
#define SHIFT_SC_BOTTOM 16

//
// DP_PIX_WIDTH
//

#define DP_DST_PIX_WIDTH  0x00000007L
#define DP_SRC_PIX_WIDTH  0x00000700L
#define DP_HOST_PIX_WIDTH 0x00070000L
#define DP_BYTE_PIX_ORDER 0x01000000L

#define DP_PIX_WIDTH_MONO  0x00000000L
#define DP_PIX_WIDTH_4BPP  0x00010101L
#define DP_PIX_WIDTH_8BPP  0x00020202L
#define DP_PIX_WIDTH_15BPP 0x00030303L
#define DP_PIX_WIDTH_16BPP 0x00040404L
#define DP_PIX_WIDTH_24BPP 0x00020202L
#define DP_PIX_WIDTH_32BPP 0x00060606L

//
// DP_MIX
//

//#define DP_BKGD_MIX 0x0000001FL
//#define DP_FRGD_MIX 0x001F0000L

#define DP_BKGD_MIX 0x000000FFL
#define DP_FRGD_MIX 0x0000001FL

//#define DP_MIX_D    0x00030003L
#define DP_MIX_D    0x00600060L
#define DP_MIX_F    0x00200020L

#define DP_MIX_Dn   0x00000000L
#define DP_MIX_0    0x00010001L
#define DP_MIX_1    0x00020002L
#define DP_MIX_Sn   0x00040004L
#define DP_MIX_DSx  0x00050005L
#define DP_MIX_DSxn 0x00060006L
#define DP_MIX_S    0x00070007L
#define DP_MIX_DSan 0x00080008L
#define DP_MIX_DSno 0x00090009L
#define DP_MIX_SDno 0x000A000AL
#define DP_MIX_DSo  0x000B000BL
#define DP_MIX_DSa  0x000C000CL
#define DP_MIX_SDna 0x000D000DL
#define DP_MIX_DSna 0x000E000EL
#define DP_MIX_DSon 0x000F000FL
#define DP_MIX_0x17 0x00170017L

//
// DP_SRC
//

#define DP_BKGD_SRC 0x00000007L
#define DP_FRGD_SRC 0x00000700L
#define DP_MONO_SRC 0x00030000L

#define DP_SRC_BKGD 0x00000000L
#define DP_SRC_FRGD 0x00000101L
#define DP_SRC_HOST 0x00020202L
#define DP_SRC_VRAM 0x00030303L
#define DP_SRC_PATT 0x00010404L

//=============================================================


// Equates and Structures for Int 31h Interface

#define Int31_Sel_Mgt          0x00
#define    SelMgt_Alloc_Sel    0x00
#define    SelMgt_Free_Sel     0x01
#define    SelMgt_Seg_To_Sel   0x02
#define    SelMgt_Get_LDT_Base 0x03
#define    SelMgt_Lock_Sel     0x04
#define    SelMgt_Unlock_Sel   0x05
#define    SelMgt_Get_Base     0x06
#define    SelMgt_Set_Base     0x07
#define    SelMgt_Set_Limit    0x08
#define    SelMgt_Set_Acc_Bits 0x09
#define    SelMgt_Alias_Sel    0x0A
#define    SelMgt_Get_Desc     0x0B
#define    SelMgt_Set_Desc     0x0C
#define    SelMgt_Get_Spec_Sel 0x0D

#define Int31_DOS_Mem_Mgt      0x01
#define    DOSMem_Allocate     0x00
#define    DOSMem_Free         0x01
#define    DOSMem_Resize       0x02

#define Int31_Int_Serv         0x02
#define    Int_Get_Real_Vec    0x00
#define    Int_Set_Real_Vec    0x01
#define    Int_Get_Excep_Vec   0x02
#define    Int_Set_Excep_Vec   0x03
#define    Int_Get_PMode_Vec   0x04
#define    Int_Set_PMode_Vec   0x05

#define Int31_Trans_Serv       0x03
#define    Trans_Sim_Int       0x00
#define    Trans_Far_Call      0x01
#define    Trans_Call_Int_Proc 0x02
#define    Trans_Call_Back     0x03
#define    Trans_Free_CB       0x04
#define    Trans_Get_Save_Addr 0x05
#define    Trans_Get_Sw_Addr   0x06

#define Int31_Get_Version      0x04

#define Int31_Mem_Mgt          0x05
#define    MemMgt_Get_Info     0x00
#define    MemMgt_Allocate     0x01
#define    MemMgt_Free         0x02
#define    MemMgt_Resize       0x03

#define Int31_Page_Lock        0x06
#define    Lock_Region         0x00
#define    Unlock_Region       0x01
#define    Mark_Pageable       0x02
#define    Mark_Not_Pageable   0x03
#define    Get_Page_Size       0x04

#define Int31_Demand_Page_Tune 0x07
#define    Page_Candidate      0x00
#define    Page_Discard        0x01
#define    DPMI_Candidate      0x02
#define    DPMI_Discard        0x03

#define Int31_Map_Phys_Addr    0x08

#define Int31_Virt_Int_State   0x09
#define    Get_Clear_Int_State 0x00
#define    Get_Set_Int_State   0x01
#define    Get_Int_State       0x02

#define Int31_MCP_Control      0x0E
#define    Get_Status          0x00
#define    Set_Emulation       0x01


//----------------------------------------------------------------------------
// S T R U C S
//----------------------------------------------------------------------------
typedef struct _SEGMENTDESCRIPTOR {
        WORD	sdLimit;
        WORD	sdBase0_15;
        BYTE	sdBase16_23;
        BYTE	sdAccess;
        BYTE	sdSegmentType;
        BYTE	sdBase24_31;
} SEGMENTDESCRIPTOR;

union LongWord
{
  DWORD dword;
  struct
  {
    BYTE low;
    BYTE middle;
    BYTE high;
    BYTE notUsed;
  } byte;
};

union IndexReg
{
  WORD word;
  struct
  {
    BYTE index;
    BYTE value;
  } byte;
};



// Definitions for the access byte in a descriptor

// Following fields are common to segment and control descriptors

#define D_PRES          0x80            // present in memory
#define D_NOTPRES       0x00            // not present in memory

#define D_DPL0          0x00            // Ring 0
#define D_DPL1          0x20            // Ring 1
#define D_DPL2          0x40            // Ring 2
#define D_DPL3          0x60            // Ring 3

#define D_SEG           0x10            // Segment descriptor
#define D_CTRL          0x00            // Control descriptor

#define D_GRAN_BYTE     0x00            // Segment length is byte granular
#define D_GRAN_PAGE     0x80            // Segment length is page granular
#define D_DEF16         0x00            // Default operation size is 16 bits
#define D_DEF32         0x40            // Default operation size is 32 bits


// Following fields are specific to segment descriptors

#define D_CODE          0x08             // code
#define D_DATA          0x00             // data

#define D_RX            0x02             // if code, readable
#define D_X             0x00             // if code, exec only
#define D_W             0x02             // if data, writable
#define D_R             0x00             // if data, read only

#define D_ACCESSED      1               // segment accessed bit


// Useful combination access rights bytes

#define ST_RW_DATA      (D_PRES+D_SEG+D_DATA+D_W)
#define ST_R_DATA       (D_PRES+D_SEG+D_DATA+D_R)
#define ST_CODE         (D_PRES+D_SEG+D_CODE+D_RX)

#define D_PAGE32        (D_GRAN_PAGE+D_DEF32) // 32 bit Page granular


// Masks for selector fields

#define SFM_SELECTOR    0xfff8          // selector index
#define SFM_SEL_LOW     0xf8            // mask for low byte of sel indx
#define SFM_TABLE       0x04            // table bit
#define SFM_RPL         0x03            // privilige bits
#define SFM_RPL_CLR     ~SFM_RPL        // clear ring bits

//----------------------------------------------------------------------------
// M A C R O S
//----------------------------------------------------------------------------
#define MakeEngineIdle()		while(inp(GP_STAT+1) & 0x02)
#define CheckFIFOSpace(SpaceNeeded)	while(inp(GP_STAT) & SpaceNeeded)
#define OutCMD(Command)			outp(COMMAND_FLAG_PORT, Command)

#define EX_MCTL_1       0x53
#define ENB_MMIO        0x10

#define EnableMMIO()	{outp(CRTC_REG, EX_MCTL_1); outp(CRTC_REG+1, inp(CRTC_REG+1)|ENB_MMIO);}
#define DisableMMIO()	{outp(CRTC_REG, EX_MCTL_1); outp(CRTC_REG+1, inp(CRTC_REG+1)&~ENB_MMIO);}

#ifdef S3VIRGE
#define BIMODAL 1
#else
#define BIMODAL 0	//DO NOT ASSUME IT'S BIMODAL
#endif

#if BIMODAL

#define BEGIN_ACCESS
#define END_ACCESS

#else

#define BEGIN_ACCESS					                    \
 if( !BIMODAL_HW_PRESENT )					              \
 {								                             \
   if ( *(sData->lpLinearCurrentChipMode) != MEMORY )         \
   {                                                 \
	MakeEngineIdle();					                    \
	outp(CRTC_REG, 0x30);			                    \
   if (sData->wChipId != S3911)	\
   {			                    \
      if ( (sData->wChipId != S3801) && (inp(CRTC_REG+1) != 0x95) ) DisableMMIO();       \
   	outp(CRTC_REG, 0x40);			                    \
   	outp(CRTC_REG+1, inp(CRTC_REG+1)&(~0x01));        \
      if ( sData->wChipId != S3764 )       \
   	{																  \
         outp(CRTC_REG, 0x58);					              \
         outp(CRTC_REG+1, inp(CRTC_REG+1) | 0x10);		     \
   	}																  \
   }		\
   outp(CRTC_REG, 0x30);                             \
   sData->wPrevChipMode=*(sData->lpLinearCurrentChipMode);           \
   *(sData->lpLinearCurrentChipMode)=MEMORY;                  \
   }                                                 \
 }

#define END_ACCESS                                   \
 if( !BIMODAL_HW_PRESENT )					              \
 {		                                               \
   if ( *(sData->lpLinearCurrentChipMode) != ENGINE )         \
   {                                                 \
   if (sData->wChipId != S3911)	\
   {			                    \
	outp(CRTC_REG, 0x58);	                          \
	outp(CRTC_REG+1, inp(CRTC_REG+1)&(~0x10));		  \
	outp(CRTC_REG, 0x40);					              \
	outp(CRTC_REG+1, (inp(CRTC_REG+1)&(~0x08))| 0x01);\
   outp(CRTC_REG, 0x30);                             \
   if ( (sData->wChipId != S3801) && (inp(CRTC_REG+1) != 0x95) && (sData->ddpf.dwRGBBitCount != 24)) EnableMMIO();        \
   }		\
   sData->wPrevChipMode=*(sData->lpLinearCurrentChipMode);           \
   *(sData->lpLinearCurrentChipMode)=ENGINE;                  \
   }                                                 \
 }								

#endif

#define GetValidDDPF(this, lpDPF) \
{if(this->dwFlags & DDRAWISURF_HASPIXELFORMAT ) \
     lpDPF = &this->lpGbl->ddpfSurface; \
 else \
     lpDPF = &sData->ddpf; \
}

#endif // _S3I_H
