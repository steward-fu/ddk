
#ifndef __VIRGE1_H

#define __VIRGE1_H

#define HDR_SIZE                  4
#define CMD_SIZE                  4
#define DATA_SIZE               124

//moved from d3dctxt.c
#define S3DTK_VIRGEID           0x5631
#define S3DTK_VIRGEVXID         0x883d
#define S3DTK_VIRGEDXGXID       0x8a01
#define S3DTK_VIRGEGX2          0x8a10 
#define S3DTK_VIRGEGX2PLUS1     0x8a11
#define S3DTK_VIRGEGX2PLUS2     0x8a12
#define S3DTK_VIRGEM5           0x8c01 
#define S3DTK_VIRGEM5PLUS1      0x8c02   
#define S3DTK_VIRGEM5PLUS2      0x8c03


/* VGA Controller  */

#define __BASE_REG_MEM_SPACE   0x8000L

#define __CR53                   0x53

#define __CR53_4_3_MASK          0x0E7

#define __CR53_MMIO_DISABLE     0x00
#define __CR53_MMIO_NEW         0x08
#define __CR53_MMIO_OLD         0x10
#define __CR53_MMIO_NEW_OLD     0x18

#define __CR53_5_MASK            0x0DF
#define __CR53_MMIO_A800         0x00
#define __CR53_MMIO_B800         0x20


/* Virge1 */

#define VI1_DMA                  0x8580L

#define VI1_DMA_SysMemAddr       0x00L
#define VI1_DMA_CtlDirXferLen    0x04L
#define VI1_DMA_BaseAddr         0x10L
#define VI1_DMA_WrtPtr           0x14L
#define VI1_DMA_RdPtr            0x18L

#define VI1_MPC                  0x8200L

#define VI1_MPC_FifoCtl          0x00L
#define VI1_MPC_MiuCtl           0x04L
#define VI1_MPC_Timeout1         0x08L
#define VI1_MPC_Timeout2         0x0CL
#define VI1_MPC_DmaRdBaseAddr    0x20L
#define VI1_MPC_DmaRdStrideWidth 0x24L

#define VI1_LPB                  0xFF00L

#define VI1_LPB_Mode             0x00L
#define VI1_LPB_OutFifoStatus    0x04L
#define VI1_LPB_IntrFlags        0x08L
#define VI1_LPB_FrameBufAddr0    0x0CL
#define VI1_LPB_FrameBufAddr1    0x10L
#define VI1_LPB_DirRdWrtAddr     0x14L
#define VI1_LPB_DirRdWrtData     0x18L
#define VI1_LPB_GenIOPort        0x1CL
#define VI1_LPB_SerialPort       0x20L
#define VI1_LPB_VideoInWinSize   0x24L
#define VI1_LPB_VideoDataOff     0x28L
#define VI1_LPB_HorizDecimCtl    0x2CL
#define VI1_LPB_VertDecimCtl     0x30L
#define VI1_LPB_LineOff          0x34L
#define VI1_LPB_OutFifoData      0x40L


#define VI1_SPC                  0x8180L

#define VI1_SPC_PrimStrmCtl      0x00L
#define VI1_SPC_ClrChrmCtl       0x04L
#define VI1_SPC_SecondStrmCtl    0x10L
#define VI1_SPC_ChrmKeyUpBnd     0x14L
#define VI1_SPC_SecondStrmConsts 0x18L
#define VI1_SPC_BlendCtl         0x20L


#define VI1_STRMADDR                        0x81C0L

#define VI1_STRMADDR_PrimStrmFrmBufAddr0    0x0L
#define VI1_STRMADDR_PrimStrmFrmBufAddr1    0x4L
#define VI1_STRMADDR_PrimStrmStride         0x8L
#define VI1_STRMADDR_DblBufLPBSupport       0xCL
#define VI1_STRMADDR_SecondStrmFrmBufAddr0  0x10L
#define VI1_STRMADDR_SecondStrmFrmBufAddr1  0x14L
#define VI1_STRMADDR_SecondStrmStride       0x18L
#define VI1_STRMADDR_OpqOvlCtl              0x1CL
#define VI1_STRMADDR_K1VertSclFctr          0x20L
#define VI1_STRMADDR_K2VertSclFctr          0x24L
#define VI1_STRMADDR_DDAVertAccumInitVal    0x28L
#define VI1_STRMADDR_PrimStrmWinStrtCoord   0x30L
#define VI1_STRMADDR_PrimStrmWinSz          0x34L
#define VI1_STRMADDR_SecondStrmWinStrtCoord 0x38L
#define VI1_STRMADDR_SecondStrmWinSz        0x3CL


#define SP_PRI_INPT_FMT_SHIFT       24
#define SP_PRI_INPT_FMT_MASK        (7L << SP_PRI_INPT_FMT_SHIFT)
#define SP_PRI_INP_8CLUT            (0L << SP_PRI_INPT_FMT_SHIFT)
#define SP_PRI_INP_16_1555          (3L << SP_PRI_INPT_FMT_SHIFT)
#define SP_PRI_INP_16_565           (5L << SP_PRI_INPT_FMT_SHIFT)
#define SP_PRI_INP_24_888           (6L << SP_PRI_INPT_FMT_SHIFT)
#define SP_PRI_INP_32_X888          (7L << SP_PRI_INPT_FMT_SHIFT)


#define SP_SEC_INPT_FMT_SHIFT       24
#define SP_SEC_INPT_FMT_MASK        (7L << SP_SEC_INPT_FMT_SHIFT)
#define SP_SEC_INP_YCbCr422     (1L << SP_SEC_INPT_FMT_SHIFT)
#define SP_SEC_INP_YUV422           (2L << SP_SEC_INPT_FMT_SHIFT)
#define SP_SEC_INP_16_1555          (3L << SP_SEC_INPT_FMT_SHIFT)
#define SP_SEC_INP_YUV211           (4L << SP_SEC_INPT_FMT_SHIFT)
#define SP_SEC_INP_16_565           (5L << SP_SEC_INPT_FMT_SHIFT)
#define SP_SEC_INP_24_888           (6L << SP_SEC_INPT_FMT_SHIFT)
#define SP_SEC_INP_32_X888          (7L << SP_SEC_INPT_FMT_SHIFT)

/* command settings */

#define cmdAE_ENABLE               1L
#define cmdHWCLIP_EN               2L
#define cmdDEST_FMT_MASK           0xFFFFFFE3L
#define cmdDEST_FMT_8BPP_PAL       0L
#define cmdDEST_FMT_ZRGB1555       0x4L
#define cmdDEST_FMT_RGB888         0x8L
#define cmdTEX_CLR_FMT_MASK        0xFFFFFF1FL
#define cmdTEX_CLR_FMT_ARGB8888    0L
#define cmdTEX_CLR_FMT_ARGB4444    0x20L
#define cmdTEX_CLR_FMT_ARGB1555    0x40L
#define cmdTEX_CLR_FMT_Alph4Blnd4  0x60L
#define cmdTEX_CLR_FMT_Blnd4LoNbl  0x80L
#define cmdTEX_CLR_FMT_Blnd4HiNbl  0xA0L
#define cmdTEX_CLR_FMT_8BPP_ClrInd 0xC0L
#define cmdTEX_CLR_FMT_YUV211      0xE0L
#define cmdTEX_MIPMAP_MASK         0xFFFFF0FFL
#define cmdTEX_FILT_MODE_MASK      0xFFFF8FFFL
#define cmdTEX_FILT_MODE_1MTpp     0L
#define cmdTEX_FILT_MODE_2MTpp     0x1000L
#define cmdTEX_FILT_MODE_4MTpp     0x2000L
#define cmdTEX_FILT_MODE_8MTpp     0x3000L
#define cmdTEX_FILT_MODE_1Tpp      0x4000L
#define cmdTEX_FILT_MODE_2VTpp     0x5000L
#define cmdTEX_FILT_MODE_4Tpp      0x6000L
#define cmdLIT_TEX_MODE_MASK       0xFFFE7FFFL
#define cmdLIT_TEX_MODE_CmplxRflct 0L
#define cmdLIT_TEX_MODE_Modulate   0x8000L
#define cmdLIT_TEX_MODE_Decal      0x10000L
#define cmdFOG_EN                0x20000L
#define cmdALP_BLD_CTL_NoAlph      0L
#define cmdALP_BLD_CTL_TexAlph     0x80000L
#define cmdALP_BLD_CTL_SrcAlph     0x0C0000L
#define cmdALP_BLD_CTL_MASK        0xfff3ffffL

#define cmdZ_BUF_CMP_NeverPass     0L
#define cmdZ_BUF_CMP_ZsGtZfb       0x100000L
#define cmdZ_BUF_CMP_ZsEqZfb       0x200000L
#define cmdZ_BUF_CMP_ZsGeZfb       0x300000L
#define cmdZ_BUF_CMP_ZsLsZfb       0x400000L
#define cmdZ_BUF_CMP_ZsNeZfb       0x500000L
#define cmdZ_BUF_CMP_ZsLeZfb       0x600000L
#define cmdZ_BUF_CMP_AlwaysPass    0x700000L
#define cmdZ_BUF_CMP_MASK          0xFF8FFFFFL
#define cmdZ_UP_EN                 0x800000L
#define cmdZ_UP_DIS                0xFF7FFFFFL
#define cmdZ_BUF_MODE_MUX_ZPass    0x1000000L
#define cmdZ_BUF_MODE_MUX_DrawPass 0x2000000L
#define cmdZ_BUF_OFF               0x3000000L
#define cmdZ_BUF_MODE_MASK         0xFCFFFFFFL
#define cmdWRAP_EN                 0x4000000L
#define cmdCMD_TYPE_Gouraud        0L
#define cmdCMD_TYPE_LitTex         0x8000000L
#define cmdCMD_TYPE_UnlitTex       0x10000000L
#define cmdCMD_TYPE_LitTexPersp    0x28000000L
#define cmdCMD_TYPE_UnlitTexPersp  0x30000000L
#define cmdCMD_TYPE_3DLine         0x40000000L
#define cmdCMD_TYPE_NOOP           0x78000000L
#define cmdCMD_TYPE_MASK           0x87FFFFFFL
#define cmd2D_CMD                  0L
#define cmd3D_CMD                  0x80000000L


#define LINE_3D                  0x0B000L
#define TRI_3D                   0x0B400L

#define LINE_TRI_3D_SETUP        0x0D4L
#define LINE_3D_DATA             0x144L
#define TRI_3D_DATA              0x104L

#define Z_BASE             0x0L
#define DEST_BASE          0x4L
#define CLIP_L_R           0x8L
#define CLIP_T_B           0xCL
#define DEST_SRC_STR       0x10L
#define Z_STRIDE           0x14L
#define TEX_BASE           0x18L
#define TEX_BDR_CLR        0x1CL
#define DC_FADE_CLR        0x20L
#define TEX_BG_CLR         0x24L
#define TEX_FG_CLR         0x28L

//#define CMD_SET            0x2CL
#define CMD_OFFSET         0x0100 //lym only make sense for DMA style


#if 0
#define LINE_3D_dGdY_dBdY  0x144L
#define LINE_3D_dAdY_dRdY  0x148L
#define LINE_3D_GS_BS      0x14CL
#define LINE_3D_AS_RS      0x150L
#define LINE_3D_dZ         0x158L
#define LINE_3D_ZStart     0x15CL
#define LINE_3D_XEnd0_End1 0x16CL
#define LINE_3D_dX         0x170L
#define LINE_3D_XStart     0x174L
#define LINE_3D_YStart     0x178L
#define LINE_3D_YCnt       0x17CL
#else
#define LINE_3D_dGdY_dBdY  0x0L
#define LINE_3D_dAdY_dRdY  0x4L
#define LINE_3D_GS_BS      0x8L
#define LINE_3D_AS_RS      0xcL
#define LINE_3D_fill0      0x10L
#define LINE_3D_dZ         0x14L
#define LINE_3D_ZStart     0x18L
#define LINE_3D_fill1      0x1cL
#define LINE_3D_fill2      0x20L
#define LINE_3D_fill3      0x24L
#define LINE_3D_XEnd0_End1 0x28L
#define LINE_3D_dX         0x2cL
#define LINE_3D_XStart     0x30L
#define LINE_3D_YStart     0x34L
#define LINE_3D_YCnt       0x38L
#endif


#define TRI_3D_bV          0x0L
#define TRI_3D_bU          0x4L
#define TRI_3D_dWdX        0x8L
#define TRI_3D_dWdY        0xCL
#define TRI_3D_WS          0x10L
#define TRI_3D_dDdX        0x14L
#define TRI_3D_dVdX        0x18L
#define TRI_3D_dUdX        0x1CL
#define TRI_3D_dDdY        0x20L
#define TRI_3D_dVdY        0x24L
#define TRI_3D_dUdY        0x28L
#define TRI_3D_DS          0x2CL
#define TRI_3D_VS          0x30L
#define TRI_3D_US          0x34L
#define TRI_3D_dGdX_dBdX   0x38L
#define TRI_3D_dAdX_dRdX   0x3CL
#define TRI_3D_dGdY_dBdY   0x40L
#define TRI_3D_dAdY_dRdY   0x44L
#define TRI_3D_GS_BS       0x48L
#define TRI_3D_AS_RS       0x4CL
#define TRI_3D_dZdX        0x50L
#define TRI_3D_dZdY        0x54L
#define TRI_3D_ZS02        0x58L
#define TRI_3D_dXdY12      0x5CL
#define TRI_3D_XEnd12      0x60L
#define TRI_3D_dXdY01      0x64L
#define TRI_3D_XEnd01      0x68L
#define TRI_3D_dXdY02      0x6CL
#define TRI_3D_XStart02    0x70L
#define TRI_3D_YStart      0x74L
#define TRI_3D_Y01_Y12     0x78L

typedef struct _vi13D_SETUP {
    unsigned long  stp_Z_BASE;
    unsigned long  stp_DEST_BASE;
    unsigned long  stp_CLIP_LftRt;
    unsigned long  stp_CLIP_TopBtm;
    unsigned long  stp_DstSrc_Stride;
    unsigned long  stp_Z_STRIDE;
    unsigned long  stp_TEX_BASE;
    unsigned long  stp_TEX_BDR_CLR;
    unsigned long  stp_FOG_CLR;
    unsigned long  stp_TEX_BG_CLR;
    unsigned long  stp_TEX_FG_CLR;
} vi13D_SETUP;

typedef struct _vi1LINE {

    short vi1L3dB;
    short vi1L3dG;
    short vi1L3dR;
    short vi1L3dA;
    short vi1L3sB;
    short vi1L3sG;
    short vi1L3sR;
    short vi1L3sA;
    unsigned long vi1Lrsrv1;
    long  vi1L3dZ;
    long  vi1L3sZ;
    unsigned long vi1Lrsrv2;
    unsigned long vi1Lrsrv3;
    unsigned long vi1Lrsrv4;
    short vi1L3End1X;
    short vi1L3End0X;
    long  vi1L3dX;
    long  vi1L3sX;
    short vi1L3sY;
    short vi1Lrsrv5;
    short vi1L3cntY;
    short vi1L3Dir;

} vi1LINE;

typedef struct _vi1TRIANGLE {

    unsigned long vi1T3bV;
    unsigned long vi1T3bU;
    unsigned long vi1T3dWdX;
    unsigned long vi1T3dWdY;
    unsigned long vi1T3sW;
    unsigned long vi1T3dDdX;
    unsigned long vi1T3dVdX;
    unsigned long vi1T3dUdX;
    unsigned long vi1T3dDdY;
    unsigned long vi1T3dVdY;
    unsigned long vi1T3dUdY;
    unsigned long vi1T3sD;
    unsigned long vi1T3sV;
    unsigned long vi1T3sU;
    unsigned long vi1T3dGdXdBdX;
    unsigned long vi1T3dAdXdRdX;
    unsigned long vi1T3dGdYdBdY;
    unsigned long vi1T3dAdYdRdY;
    unsigned long vi1T3sGsB;
    unsigned long vi1T3sAsR;
    unsigned long vi1T3dZdX;
    unsigned long vi1T3dZdY;
    unsigned long vi1T3sZ;
    unsigned long v11T3dXdY_12;
    unsigned long vi1T3eX_12;
    unsigned long v11T3dXdY_01;
    unsigned long vi1T3eX_01;
    unsigned long v11T3dXdY_02;
    unsigned long vi1T3sX;
    unsigned long vi1T3sY;
    unsigned long vi1T3cY_01_12;

} vi1TRIANGLE;

#define vi13D_LINE     vi1LINE
#define vi13D_TRIANGLE vi1TRIANGLE

typedef struct _vi13D_LINE_SETUP
{
    vi13D_SETUP vi13d_setup;
    vi13D_LINE  vi13d_line;
} vi13D_LINE_SETUP;


typedef struct _vi13D_TRIANGLE_SETUP
{
    vi13D_SETUP     vi13d_setup;
    vi13D_TRIANGLE  vi13d_triangle;
} vi13D_TRIANGLE_SETUP;

#define VIRGE1_READ  0L
#define VIRGE1_WRITE 0x80000000L

#define VIRGE1_ERR_OK          1L
#define VIRGE1_ERR_UNSUPPORTED 10L


#define SYS_PRIM_WIDTH_DFLT        1024
#define SYS_PRIM_HEIGHT_DFLT       768
#define SYS_PRIM_CLRDEPTH_DFLT     16
#define SYS_PRIM_BYTESPERPIX_DFLT  2
#define SYS_PRIM_BASEADDR_DFLT     0


typedef struct _vi13D_CMD
{

    unsigned long   cmAE_ENABLE;
    unsigned long   cmCLIP_EN;
    unsigned long   cmDEST_FMT;
    unsigned long   cmTEX_CLR_FMT;
    unsigned long   cmNUM_MIP_LE;
    unsigned long   cmTEX_FILT_MODE;
    unsigned long   cmLIT_TEX_MODE;
    unsigned long   cmDEPTH_EN;
    unsigned long   cmALP_BLD_CTL;
    unsigned long   cmZ_BUF_CMP;
    unsigned long   cmZ_UP_EN;
    unsigned long   cmZ_BUF_MODE;
    unsigned long   cmWRAP_EN;
    unsigned long   cmCMD_TYPE;
    unsigned long   cm3D2D_CMD;

} vi13D_CMD;

typedef struct _vi1Prim {

    unsigned short   primWidth;
    unsigned short   primHeight;
    unsigned short   primClrDepth;
    unsigned short   primBytesPerPix;
    unsigned long    primBaseAddr;

} vi1Prim;

typedef struct _vi1DEV_CONFIG {

    vi1Prim     devPrim;
    vi13D_SETUP dev3dSetup;

} vi1DEV_CONFIG;



#endif
