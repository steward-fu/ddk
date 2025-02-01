/*
 * (C) Copyright 1996, by S3, Incorporated - All rights reserved
 *
 * S3data.h: S3 hardware data info
 *
 */

#ifndef MOBILE_FAMILY
#define MOBILE_FAMILY      0x01
#endif
#define UMA_FAMILY         0x02
#define TRIO64V_FAMILY     0x04
#define TRIOV2_FAMILY      0x08
#define VIRGE_FAMILY       0x10
#define VIRGEDXGX_FAMILY   0x20

#define MAX_STRING_SIZE      128
#define PATH_SIZE            MAX_STRING_SIZE
#define K2V_OK               0
#define K2V_ERR             -1


// Bus Types:

#define BUS_ISA         1               // ISA bus
#define BUS_LOCAL       2               // local bus
#define BUS_PCI                         3

#define CRTC_REG                        0x3d4
#define SEQ_REG                 0x3c4
#define GP_STAT                 0x9ae8
#define HDW_BUSY                        0x0200
#define CRTC_VBLANK_INT_EN                      0x2     //S3?
#define CRTC_VBLANK_INT                         0x4     //S3?

#define MAX_NUM_HEAPS   4
#define MIN_RECT_SIZE   32
#define DSTWIN_SIZES            5

#define OVERLAY_DESTROYED   0x4026
#define NOTIFY_MODE_CHANGE  0x4027

// Equate for Chip Ids

#define S3911           0x80            // Major chip id 911/924
#define S3924           0x80            // Major chip id 911/924
#define S3801           0xA0
#define S3805           0xa0            // Major chip id 801/805
#define S3928           0x90            // Major chip id 928
#define S3928PCI        0xb0            // Major chip id 928 PCI
#define S3864           0xc0            // Major chip id 864
#define S3964           0xd0            // Major chip id 964
#define S3764           0xe0     // Major chip id 764 (Trio) and up
// Equates for DeviceID (from CR2E and CR2D)

#define D_S3732         0x1088
#define D_S3764 0x1188   // device id for 764/765
#define D_S3866 0x8088   // device id for 866
#define D_S3868 0x9088   // device id for 868
#define D_S3968 0xb088   // device id for 968
#define D_S3968n        0xf088   // device id for newer revision 968
#define D_S3M65 0x1288   // device id for M65
#define D_S3M66 0x1588   // device id for M66
#define D_S3767 0x1488   // device id for 767
#define D_S3551 0x0289   // device id for 551 (Yosemite)
#define D_S3VIRGE       0x3156          // Virge
#define D_S3VIRGEVX     0x3d88          // Virge VX
#define D_S3VIRGEDXGX   0x018a          // Virge DX/GX
#define D_S3VIRGEGX2    0x108a          // Virge GX2
#define D_S3VIRGEGX2P   0x128a          // GX2+ w/o Macrovision
#define D_S3VIRGEGX2PM  0x118a          // GX2+ w/ Macrovision
#define D_S3M3          0x008c          // M3
#define D_S3M5          0x018c          // M5
#define D_S3M5PM        0x038c          // M5+ w/ Macrovision
#define D_S3M5P         0x028c          // M5+ w/o Macrovision
#define D_S3VIRGEFamily 0x008a

;
#define REVA            0x0      // minor rev Id
#define REVB            0x1      // minor rev Id
#define REVC            0x2      // minor rev Id
#define REVD            0x3      // minor rev Id
#define REVE            0x4      // minor rev Id
#define REVF            0x5      // minor rev Id
#define REVG            0x6      // minor rev Id
#define REVH            0x7      // minor rev Id
#define REVI            0x8      // minor rev Id
#define REV_765  0x40    // revision id for 765


// DAC Types:
#define DAC_ATT20C409   0x1
#define DAC_ATT20C491   0x2
#define DAC_BT481A              0x4
#define DAC_BT481               0x4
#define DAC_IMSG174             0x10
#define DAC_SC15025             0x20
#define DAC_MU9C4910V   0x40
#define DAC_ATT20C498   0x80
#define DAC_STG1700             0x100
#define DAC_S3DAC               0x200
#define DAC_TI3026              0x400
#define DAC_IBM9314             0x800
#define DAC_INTERNAL            0x8
#define DAC_ATT         ( 0x1000 | DAC_ATT20C409 | DAC_ATT20C491 | DAC_ATT20C498 | DAC_IMSG174 | DAC_SC15025 | DAC_MU9C4910V |  DAC_STG1700 | DAC_S3DAC)      // ATT or clone
#define DAC_BT485       ( 0x2000 | DAC_BT481A | DAC_BT481 | DAC_TI3026)
#define DAC_RGB525      ( 0x4000 | DAC_IBM9314)             // IBM RGB525/514
#define DAC_TI025       ( 0x8000)               // Texas Instruments 3020/3025

//Memory Types
#define EDO1cyc     0
#define SGRAM       1
#define ED02cyc     2
#define FPAGE       3
#define SDRAM       4

// Equates for resolutions, mirror of those in s3.inc

#define MODE_RES_320x200   0x0
#define MODE_RES_320x240   0x1
#define MODE_RES_400x300   0x2
#define MODE_RES_512x384   0x3
#define MODE_RES_640x400   0x4
#define MODE_RES_640x480   0x5
#define MODE_RES_720x480   0x6
#define MODE_RES_800x480   0x7
#define MODE_RES_800x600   0x8
#define MODE_RES_1024x600  0x9
#define MODE_RES_1024x768  0xa
#define MODE_RES_1152x864  0xb
#define MODE_RES_1280x1024 0xc
#define MODE_RES_1600x1200 0xd
#define MODE_RES_1280x480  0xe
#define MODE_RES_1600x600  0xf
#define MODE_RES_2048x600  0x10
#define MODE_RES_2048x768  0x11


// Equates for S3DriverData.VGAFlag

#define IN_VGA_MEMORY_MODE 0x01
#define IN_DOS_FULL_SCREEN 0x02

// Equates for display control registers

#define DISPLAY_CONTROL_REG     0x31
#define LCD_IGA1_DUOVIEW        0x14
#define LCD_IGA1_SIM            0x10
#define CRT_IGA1                0x12
#define LCD_IGA2                0x2

// Equates for M5 architecture configuration register

#define ARCH_CONFIG_REG         0x30
#define SPSRC_IGA2              0x2
#define TV_IGA2                 0x4


//Equates for display mode and DuoView registers

#define DISPLAY_REG             0x6b
#define CRT_ONLY                0x1
#define FP_ONLY                 0x2
#define TV_ONLY                 0x4
#define DUOVIEW                 0x80
#define FP_CRT_SIM              0x3
#define FP_TV_SIM               0x6
#define MODE_MASK               0xf
#define NEW_DISPLAY             0x40h

#define RGB_OUTPUT              0
#define NTSC_OUTPUT             0x80
#define PAL_OUTPUT              0xc0
#define DISPLAY_CONTROL_FUNCTION        0x6f02

// Equates for bStreamProcessorFlags

#define SP_ENABLED          0x02
#define SP_IN_ENHANCE_VGA   0x04
#define SP_IN_ENHANCE_MODE  0x0c
#define SP_MASK             0x0c
#define SP_DISABLE_OVERLAY  0x10
#define SP_HIDE             0x20


//Panel type, Equate bPanelType

#define S3_MOBILE_STN_PANEL     0
#define S3_MOBILE_TFT_PANEL     1

// Equates for FIFO control registers in M65 rev. E

#define FIFO_IO_REG_LO          0x3e
#define FIFO_IO_REG_HI          0x3f
#define FIFO_CONTROL_BIT        0x80
#define FIFO_CONTROL_MASK       0x7f



// Expansion and centering registers

#define FP_HORZ_EXP_1           0x54
#define H_EXP                   0xc
#define H_CENTERING             0x10
#define H_EXP_MASK              0xf3
#define H_GRPH_EXP_MASK         0xc

#define FP_HORZ_EXP_2           0x55
#define ALL_H_EXP               0x1c
#define ALL_H_EXP_MASK          0xe3

#define FP_VERT_EXP_1           0x56
#define V_EXP                   0xc
#define V_CENTERING             0x10
#define V_EXP_MASK              0x1f

#define FP_VERT_EXP_2           0x57
#define ALL_V_EXP               0x7c
#define ALL_V_EXP_MASK          0x83

// Vertical display registers
#define VERT_1_REG              0x12
#define VERT_2_REG              0x7
#define VERT_3_REG              0x5e
#define VERT_MASK               0xbd

// Horizontal display registers
#define HORZ_1_REG              0x1
#define HORZ_2_REG              0x5d
#define HORZ_MASK               0xfe

// Clocking mode register
#define CLOCKING_MODE           0x1
#define SCREEN_BLANK            0x20

typedef struct
{
  RECT        sr_rect;
  WORD        sr_width;
  WORD        sr_height;
} SCREENRGN;

// S3 Support Features

#define PIXEL_FORMATTER    1L
#define STREAMS_PROCESSOR  2L
#define BIMODAL_HW         4L
#define MMIOONLY_HW        8L
#define READ_SCANLINE      0x10L

// FourCC Values

#define FOURCC_YUY2   0x32595559
#define FOURCC_Y211   0x31313259

// Stream processor register definitions

#define P_CONTROL               0x8180  // Primary Stream Control register
#define   CKEY_LOW              0x8184  // Color/Chroma Key Control
#define S_CONTROL               0x8190  // Secondary Stream Control
#define   CKEY_HI               0x8194  // Chroma Key Upper Bound
#define S_HK1K2                 0x8198  // Secondary Stream Stretch/Filter const
#define   BLEND_CONTROL         0x81a0  // Blend Control
#define P_0                     0x81c0  // Primary Stream Frame Buffer Address 0
#define P_1                     0x81c4  // Primary Stream Frame Buffer Address 1
#define P_STRIDE                0x81c8  // Primary Stream Stride
#define   LPB_DB                0x81cc  // Double buffer and LPB Support
#define S_0                     0x81d0  // Secondary Stream Frame Buffer Addr 0
#define S_1                     0x81d4  // Secondary Stream Frame Buffer Addr 1
#define S_STRIDE                0x81d8  // Secondary Stream Stride
#define   OPAQUE_CONTROL        0x81dc  // Opaque Overlay Control
#define S_VK1                   0x81e0  // K1 Vertical Scale Factor
#define S_VK2                   0x81e4  // K2 Vertical Scale Factor
#define S_VDDA                  0x81e8  // DDA Vertical Accumulator Init Value
#define   FIFO_CONTROL          0x81ec  // Stream FIFO and RAS Controls
//###
//#define         FIFO_CONTROL          0x8200  // Stream FIFO and RAS Controls
#define P_XY                    0x81f0  // Primary Stream Window Start Coord
#define P_WH                    0x81f4  // Primary Stream Window Size
#define S_XY                    0x81f8  // Secondary Stream Window Start Coord
#define S_WH                    0x81fc  // Secondary Stream Window Size
#define COLOR_ADJUST_REG 0x819c // Color Adjustment Register

#define P_Format_Shift          24
#define P_Format_Mask           (7L << P_Format_Shift)
#define P_RGB8                  (0L << P_Format_Shift)
#define P_RGB15                 (3L << P_Format_Shift)
#define P_RGB16                 (5L << P_Format_Shift)
#define P_RGB24                 (6L << P_Format_Shift)
#define P_RGB32                 (7L << P_Format_Shift)
#define P_Filter_Shift          28
#define P_Filter_Mask           (7L << P_Filter_Shift)
#define P_1x                    (0L << P_Filter_Shift)
#define P_2x                    (1L << P_Filter_Shift)
#define P_2xBiLinear            (2L << P_Filter_Shift)

#define BVCr_Shift              0
#define BVCr_Mask               (255L << BVCr_Shift)
#define GUCb_Shift              8
#define GUCb_Mask               (255L << GUCb_Shift)
#define RY_Shift                16
#define RY_Mask                 (255L << RY_Shift)
#define Compare_Shift           24
#define Compare_Mask            (255L << Compare_Shift)
#define CompareBits7            (0L  << Compare_Shift)
#define CompareBits6t7          (1L  << Compare_Shift)
#define CompareBits5t7          (2L  << Compare_Shift)
#define CompareBits4t7          (3L  << Compare_Shift)
#define CompareBits3t7          (4L  << Compare_Shift)
#define CompareBits2t7          (5L  << Compare_Shift)
#define CompareBits1t7          (6L  << Compare_Shift)
#define CompareBits0t7          (7L  << Compare_Shift)
#define KeyFrom_Shift           28
#define KeyFrom_Mask            (1L << KeyFrom_Shift)
#define KeyFromStream           (0L << KeyFrom_Shift)
#define KeyFromCompare          (1L << KeyFrom_Shift)
#define ValueColorKey           (0L << (KeyFrom_Shift+1))
#define IndexColorKey           (1L << (KeyFrom_Shift+1))

//For M3/M5/GX2 streams processor style
#define KMS_Shift               29
#define WindowKeying            (0L << KMS_Shift )
#define AlphaKeying             (1L << KMS_Shift )
#define ColorKeying             (2L << KMS_Shift )
#define ChromaKeying            (3L << KMS_Shift )
#define KeySelectP              (0L << KMS_Shift+2)
#define KeySelectS              (1L << KMS_Shift+2)


#define HDDA_Shift              0
#define HDDA_Mask               (((1L << 12)-1) << HDDA_Shift)
#define VHDDA_Mask      (((1L << 13)-1) << HDDA_Shift)
#define S_Format_Shift          24
#define S_Format_Mask           (7L << S_Format_Shift)
#define S_YCrCb422              (1L << S_Format_Shift)
#define S_YUV422                (2L << S_Format_Shift)
#define S_RGB15                 (3L << S_Format_Shift)
#define S_YUV211                (4L << S_Format_Shift)
#define S_RGB16                 (5L << S_Format_Shift)
#define S_RGB24                 (6L << S_Format_Shift)
#define S_RGB32                 (7L << S_Format_Shift)
#define S_YVYU                  (0L << S_Format_Shift)
#define S_YUV411                (8L << S_Format_Shift)
#define S_Filter_Shift          28
#define S_Filter_Mask           (7L << S_Filter_Shift)
#define S_1x                    (0L << S_Filter_Shift)
#define S_Upto2x                (1L << S_Filter_Shift)
#define S_2xTo4x                (2L << S_Filter_Shift)
#define S_Beyond4x              (3L << S_Filter_Shift)

#define HighVCr_Shift           0
#define HighVCr_Mask            (255L << HighVCr_Shift)
#define HighUCb_Shift           8
#define HighUCb_Mask            (255L << HighUCb_Shift)
#define HighY_Shift             16
#define HighY_Mask              (255L << HighY_Shift)

#define HK1_Shift               0
#define HK1_Mask                (((1L << 11) - 1) << HK1_Shift)
#define VHK1_Mask               (((1L << 12) - 1) << HK1_Shift)
#define HK2_Shift               16
#define HK2_Mask                (((1L << 11) - 1) << HK2_Shift)
#define VHK2_Mask               (((1L << 12) - 1) << HK2_Shift)
#define VK1_Shift               0
#define VK1_Mask                (((1L << 11) - 1) << VK1_Shift)
#define VVK1_Mask               (((1L << 12) - 1) << VK1_Shift)
#define VK2_Shift               0
#define VK2_Mask                (((1L << 11) - 1) << VK2_Shift)
#define VVK2_Mask               (((1L << 12) - 1) << VK2_Shift)
#define VDDA_Shift              0
#define VDDA_Mask               (((1L << 12) - 1) << VDDA_Shift)
#define VVDDA_Mask      (((1L << 13) - 1) << VDDA_Shift)

#define Ks_Shift                2
#define Ks_Mask                 (7L << Ks_Shift)
#define Kp_Shift                10
#define Kp_Mask                 (7L << Kp_Shift)
#define Compose_Shift           24
#define Compose_Mask            (7L << Compose_Shift)
#define SOnP                    (0L << Compose_Shift)
#define POnS                    (1L << Compose_Shift)
#define Dissolve                (2L << Compose_Shift)
#define Fade                    (3L << Compose_Shift)
#define KeyOnP                  (5L << Compose_Shift)
#define KeyOnS                  (6L << Compose_Shift)

#define FifoAlloc_Shift         0
#define FifoAlloc_Mask          (31L << StrFifoAlloc_Shift)
#define FifoAlloc24_0           (0L  << StrFifoAlloc_Shift)
#define FifoAlloc16_8           (8L  << StrFifoAlloc_Shift)
#define FifoAlloc12_12          (12L << StrFifoAlloc_Shift)
#define FifoAlloc8_16           (16L << StrFifoAlloc_Shift)
#define FifoAlloc0_24           (24L << StrFifoAlloc_Shift)

#define S_FifoThresh_Shift      5
#define S_FifoThresh_Mask       (31L << S_FifoThresh_Shift)

#define P_FifoThresh_Shift      10
//###
//#define P_FifoThresh_Shift    12
#define P_FifoThresh_Mask       (31L << P_FifoThresh_Shift)

#define RASLowTime_Shift        15
#define RASLowTime_Mask         (1L << RASLowTime_Shift)
#define RASLowTimeFromCR68      (0L << RASLowTime_Shift)
#define RASLowTime2_5           (1L << RASLowTime_Shift)

#define RASPreCharge_Shift      16
#define RASPreCharge_Mask       (1L << RASPreCharge_Shift)
#define RASPreChargeFromCR68    (0L << RASPreCharge_Shift)
#define RASPreCharge1_5         (1L << RASPreCharge_Shift)

#define RASInactive_Shift       17
#define RASInactive_Mask        (1L << RASInactive_Shift)
#define RASInactiveLow          (0L << RASInactive_Shift)
#define RASInactiveHigh         (1L << RASInactive_Shift)

#define MemoryCycle_Shift       18
#define MemoryCycle_Mask        (1L << MemoryCycle_Shift)
#define MemoryCycle2            (0L << MemoryCycle_Shift)
#define MemoryCycle1            (1L << MemoryCycle_Shift)

#define H_Shift                 0
#define H_Mask                  (0x07ffL << H_Shift)
#define W_Shift                 16
#define W_Mask                  (0x07ffL << W_Shift)

#define Y_Shift                 0
#define Y_Mask                  (0x07ffL << Y_Shift)
#define X_Shift                 16
#define X_Mask                  (0x07ffL << X_Shift)

#define P_Select_Shift          0
#define P_Select_Mask           (1L << P_Select_Shift)
#define P_Select0               (0L << P_Select_Shift)
#define P_Select1               (1L << P_Select_Shift)
#define S_Select_Shift          1
#define S_Select_Mask           (3L << S_Select_Shift)
#define S_Select0               (0L << S_Select_Shift)
#define S_Select1               (1L << S_Select_Shift)
#define S_Select00Or11          (2L << S_Select_Shift)
#define S_Select01Or10          (3L << S_Select_Shift)
#define L_Select_Shift          4
#define L_Select_Mask           (1L << L_Select_Shift)
#define L_Select0               (0L << L_Select_Shift)
#define L_Select1               (1L << L_Select_Shift)
#define L_SelWait_Shift         5
#define L_SelWait_Mask          (1L << L_SelWait_Shift)
#define L_SelWaitNo             (0L << L_SelWait_Shift)
#define L_SelWaitYes            (1L << L_SelWait_Shift)
#define L_SelAutoToggle_Shift   6
#define L_SelAutoToggle_Mask    (1L << L_SelAutoToggle_Shift)
#define L_SelAutoToggleNo       (0L << L_SelAutoToggle_Shift)
#define L_SelAutoToggleYes      (1L << L_SelAutoToggle_Shift)
#define L_FramesToDrop_Shift    8
#define L_FramesToDrop_Mask     (3L << L_FramesToDrop_Shift)

#define OpqStart_Shift          3
#define OpqStart_Mask           (((1L << 10) - 1) << OpqStart_Shift)
#define OpqEnd_Shift            19
#define OpqEnd_Mask             (((1L << 10) - 1) << OpqEnd_Shift)
#define OpqTopSel_Shift         30
#define OpqTopSel_Mask          (1L << OpqTopSel_Shift)
#define OpqTopSelS_             (0L << OpqTopSel_Shift)
#define OpqTopSelP_             (1L << OpqTopSel_Shift)
#define OpqCtrl_Shift           31
#define OpqCtrl_Mask            (1L << OpqCtrl_Shift)
#define OpqDisabled             (0L << OpqCtrl_Shift)
#define OpqEnabled              (1L << OpqCtrl_Shift)

// The following defines are for VL and PCI system configuration
#define K2V_SRD_LPB_MASK        0x03
#define K2V_SRD_LPB             0x00
#define K2V_SRD_FC              K2V_SRD_LPB_MASK
#define K2V_SRD_COMP            0x02

#define K2V_CR5C_SRC_MASK       0x03
#define K2V_CR5C_SRC_DIGITIZER  0x02
#define K2V_CR5C_SRC_MPEG       0x01

#define K2V_SR1C_MASK           0x03
#define K2V_SR1C_VL             0x01
#define K2V_SR1C_PCI            0x02

// Useful defines

#define FOURCC_YUY2   0x32595559
#define FOURCC_Y211   0x31313259

#define DDOVER_ALPHA \
        (DDOVER_ALPHASRC | \
         DDOVER_ALPHASRCCONSTOVERRIDE | \
         DDOVER_ALPHADEST | \
         DDOVER_ALPHADESTCONSTOVERRIDE)

#define DDOVER_KEY \
        (DDOVER_KEYSRC | \
         DDOVER_KEYSRCOVERRIDE |\
         DDOVER_KEYDEST | \
         DDOVER_KEYDESTOVERRIDE)

#define KEYING                  0x1
#define BLENDING                0x2
#define OPAQUEING               0x4

// Useful macros

//macros for Virge/VX
#define VHDDA(w0,w1)  (((2*(w0-1)-(w1-1)) << HDDA_Shift) & VHDDA_Mask )
#define VVDDA(h1)     (((1-h1)            << VDDA_Shift) & VVDDA_Mask )
#define VHK1(w0)      (((w0 - 1)          << HK1_Shift)  & VHK1_Mask  )
#define VHK2(w0,w1)   (((w0 - w1)         << HK2_Shift)  & VHK2_Mask  )
#define VHK1K2(w0,w1) (VHK1(w0) | VHK2(w0, w1))
#define VVK1(h0)      (((h0 - 1)          << VK1_Shift)  & VVK1_Mask  )
#define VVK2(h0,h1)   (((h0 - h1)         << VK2_Shift)  & VVK2_Mask  )

#define HDDA(w0,w1)  (((2*(w0-1)-(w1-1)) << HDDA_Shift) & HDDA_Mask )
#define VDDA(h1)     (((1-h1)            << VDDA_Shift) & VDDA_Mask )
#define HK1(w0)      (((w0 - 1)          << HK1_Shift)  & HK1_Mask  )
#define HK2(w0,w1)   (((w0 - w1)         << HK2_Shift)  & HK2_Mask  )
#define HK1K2(w0,w1) (HK1(w0) | HK2(w0, w1))
#define VK1(h0)      (((h0 - 1)          << VK1_Shift)  & VK1_Mask  )
#define VK2(h0,h1)   (((h0 - h1)         << VK2_Shift)  & VK2_Mask  )
#define XY(x,y)      ((((x+1)<<X_Shift)&X_Mask) | (((y+1)<<Y_Shift)&Y_Mask))
#define WH(w,h)      ((((w-1)<<W_Shift)&W_Mask) | (((h)<<H_Shift)&H_Mask))

#define IsRGB15(this) \
        ((this->dwRBitMask == 0x7c00) && \
         (this->dwGBitMask == 0x03e0) && \
         (this->dwBBitMask == 0x001f))

#define IsRGB16(this) \
        ((this->dwRBitMask == 0xf800) && \
         (this->dwGBitMask == 0x07e0) && \
         (this->dwBBitMask == 0x001f))

#define IsRGB24(this) \
        ((this->dwRBitMask == 0x00ff0000) && \
         (this->dwGBitMask == 0x0000ff00) && \
         (this->dwBBitMask == 0x000000ff))

#define IsRGB32(this) \
        ((this->dwRBitMask == 0x00ff0000) && \
         (this->dwGBitMask == 0x0000ff00) && \
         (this->dwBBitMask == 0x000000ff))

#define RGB15to32( c ) \
        (((c & 0x7c00L) << 9) | \
         ((c & 0x03e0L) << 6) | \
         ((c & 0x001fL) << 3))

#define RGB16to32( c ) \
        (((c & 0xf800L) << 8) | \
         ((c & 0x07e0L) << 5) | \
         ((c & 0x001fL) << 3))

// Equates for Pixel Formatter (Video Engine) 868/968

#define INPUT_RGB8      0x00000000
#define INPUT_RGB15     0x00600000
#define INPUT_RGB16     0x00700000
#define INPUT_RGB32     0x00300000
#define INPUT_YUV422    0x00480000
#define INPUT_YCrCb422  0x00400000
#define INPUT_RAW       0x00500000
#define OUTPUT_RGB8     0x00000000
#define OUTPUT_RGB15    0x00060000
#define OUTPUT_RGB16    0x00070000
#define OUTPUT_RGB32    0x00030000
#define OUTPUT_YUY2     0x000C0000
#define OUTPUT_RAW      0x00050000

#define CSCENABLE       0x40000000
#define STRETCH         0x00000000
#define SHRINK          0x80000000
#define SCREEN          0x00000000
#define HOST            0x40000000
#define FILTERENABLE    0x80000000
#define BILINEAR        0x00000000
#define LINEAR02420     0x00004000
#define LINEAR12221     0x00008000

#define PF_BUSY         0x80000000

#define PF_NOP          0x00018080
#define PF_CONTROL      0x00018088
#define PF_DDA          0x0001808C
#define PF_STEP         0x00018090
#define PF_CROP         0x00018094
#define PF_SRCADDR      0x00018098
#define PF_DSTADDR      0x0001809C

#define GE_CMD          0x00008118

#define GE_BUSY         0x00000200
#define GE_FIFO_EMPTY   0x00000400

BYTE FAR PASCAL CR_Read(BYTE wIndex);
void FAR PASCAL CR_Write(BYTE wIndex, BYTE wValue);
void FAR PASCAL CR_RMW(BYTE wIndex, BYTE wClear, BYTE wSet);
BYTE FAR PASCAL SR_Read(BYTE wIndex);
void FAR PASCAL SR_Write(BYTE wIndex, BYTE wValue);
void FAR PASCAL SR_RMW(BYTE wIndex, BYTE wClear, BYTE wSet);
