/*** ssdefs.h - PCCard Socket Services Definitions
 *
 *  Copyright (c) 1992,1993 Microsoft Corporation
 *  Author:     Michael Tsang (MikeTs)
 *  Created     08/28/95
 *
 *  MODIFICATION HISTORY
 */


#ifndef _SSDEFS_H
#define _SSDEFS_H


/*** Constants
 */

//Socket Services Function Codes
#define SSFN_GET_ADP_CNT        0x80
//0x81 and 0x82 reserved for historical purposes
#define SSFN_GET_SS_INFO	0x83
#define SSFN_INQ_ADAPTER	0x84
#define SSFN_GET_ADAPTER	0x85
#define SSFN_SET_ADAPTER	0x86
#define SSFN_INQ_WINDOW		0x87
#define SSFN_GET_WINDOW		0x88
#define SSFN_SET_WINDOW		0x89
#define SSFN_GET_PAGE		0x8a
#define SSFN_SET_PAGE		0x8b
#define SSFN_INQ_SOCKET		0x8c
#define SSFN_GET_SOCKET		0x8d
#define SSFN_SET_SOCKET		0x8e
#define SSFN_GET_STATUS		0x8f
#define SSFN_RESET_SOCKET	0x90
//0x91-0x94 reserved for historical purposes
#define SSFN_INQ_EDC		0x95
#define SSFN_GET_EDC		0x96
#define SSFN_SET_EDC		0x97
#define SSFN_START_EDC		0x98
#define SSFN_PAUSE_EDC		0x99
#define SSFN_RESUME_EDC		0x9a
#define SSFN_STOP_EDC		0x9b
#define SSFN_READ_EDC		0x9c
#define SSFN_GET_VENDOR_INFO	0x9d
#define SSFN_ACK_INTERRUPT	0x9e
#define SSFN_PRIOR_HANDLER      0x9f
#define SSFN_SS_ADDR	        0xa0
#define SSFN_ACCESS_OFFSETS	0xa1
#define SSFN_ACCESS_CONFIG	0xa2
#define SSFN_INQ_BWINDOW	0xa3
#define SSFN_GET_BWINDOW	0xa4
#define SSFN_SET_BWINDOW	0xa5
//0xa6-0xad reserved for expansion
#define SSFN_VEND_SPECIFIC      0xae
#define SSFN_CARD_SERVICES      0xaf

#define SS_INT                  0x1a    //Socket Service Int Vector

//Socket Services Return Codes
#define SSERR_SUCCESS		0x00
#define SSERR_BAD_ADAPTER	0x01
#define SSERR_BAD_ATTRIBUTE	0x02
#define SSERR_BAD_BASE		0x03
#define SSERR_BAD_EDC		0x04
//0x05 reserved for historical purposes
#define SSERR_BAD_IRQ		0x06
#define SSERR_BAD_OFFSET	0x07
#define SSERR_BAD_PAGE		0x08
#define SSERR_READ_FAILURE	0x09
#define SSERR_BAD_SIZE		0x0a
#define SSERR_BAD_SOCKET	0x0b
//0x0c reserved for historical purposes
#define SSERR_BAD_TYPE		0x0d
#define SSERR_BAD_VCC		0x0e
#define SSERR_BAD_VPP		0x0f
//0x10 reserved for historical purposes
#define SSERR_BAD_WINDOW	0x11
#define SSERR_WRITE_FAILURE	0x12
//0x13 reserved for historical purposes
#define SSERR_NO_CARD		0x14
#define SSERR_BAD_SERVICE	0x15
#define SSERR_BAD_MODE		0x16
#define SSERR_BAD_SPEED		0x17
#define SSERR_BUSY		0x18

//Power Level Bit Mask
#define PL_VCC                  0x80
#define PL_VPP1                 0x40
#define PL_VPP2                 0x20

//Adapter Capabilities Mask
#define AC_IND                  0x01
#define AC_PWR                  0x02
#define AC_DBW                  0x04
#define AC_CARDBUS              0x08

//Adapter State
#define AS_POWERDOWN            0x01
#define AS_MAINTAIN             0x02

//Generic Window Capability Bits
#define WC_COMMON               0x01
#define WC_ATTRIBUTE            0x02
#define WC_IO                   0x04
#define WC_WAIT                 0x80

//Generic Bridge Window Capability Bits
#define WC_MEMORY               0x01

//Bridge, Memory and I/O Capability Bits
#define WC_BASE                 0x0001
#define WC_SIZE                 0x0002
#define WC_WENABLE              0x0004
#define WC_8BIT                 0x0008
#define WC_16BIT                0x0010
#define WC_BALIGN               0x0020
#define WC_POW2                 0x0040

#define WC_FETCHABLE            0x0080  //InqBridgeWindow
#define WC_CACHABLE             0x0100  //InqBridgeWindow

//Memory Window (Page) Capabilities Only
#define WC_CALIGN               0x0080
#define WC_PAVAIL               0x0100
#define WC_PSHARED              0x0200
#define WC_PENABLED             0x0400
#define WC_WP                   0x0800

//I/O Window Capabilities Only
#define WC_INPACK               0x0080
#define WC_EISA                 0x0100
#define WC_CENABLE              0x0200

//Generic Window State Bits
#define WS_IO                   0x01
#define WS_ENABLED              0x02
#define WS_16BIT                0x04    //memory and I/O only

//Bridge Window State Bits
#define WS_PREFETCH             0x08
#define WS_CACHABLE             0x18    //includes WS_PREFETCH

//Memory Window State Bits
#define WS_PAGED                0x08

//I/O Window State Bits
#define WS_EISA                 0x08
#define WS_CENABLE              0x10

//Page State Bits
#define PS_ATTRIBUTE            0x01
#define PS_ENABLED              0x02
#define PS_WP                   0x04

//IRQ Level Bit Masks (low word of 32-bit mask)
#define IRQ_0                   0x0001
#define IRQ_1                   0x0002
#define IRQ_2                   0x0004
#define IRQ_3                   0x0008
#define IRQ_4                   0x0010
#define IRQ_5                   0x0020
#define IRQ_6                   0x0040
#define IRQ_7                   0x0080
#define IRQ_8                   0x0100
#define IRQ_9                   0x0200
#define IRQ_10                  0x0400
#define IRQ_11                  0x0800
#define IRQ_12                  0x1000
#define IRQ_13                  0x2000
#define IRQ_14                  0x4000
#define IRQ_15                  0x8000

//IRQ Level Bit Masks (high word of 32-bit mask)
#define IRQ_NMI                 0x0001
#define IRQ_IO                  0x0002
#define IRQ_BUSERR              0x0004

//IRQ State Bit Masks
#define IRQ_MASK                0x1f
#define IRQ_RESERVED            0x20
#define IRQ_HIGH                0x40
#define IRQ_ENABLED             0x80

//Socket Bit Masks
#define SBM_WP                  0x01
#define SBM_LOCKED              0x02
#define SBM_EJECT               0x04
#define SBM_INSERT              0x08
#define SBM_BVD1                0x10
#define SBM_BVD2                0x20
#define SBM_RDYBSY              0x40
#define SBM_CD                  0x80

#define SBM_LOCK                0x10
#define SBM_BATT                0x20
#define SBM_BUSY                0x40
#define SBM_XIP                 0x80

//EDC Definitions
#define EC_UNI                  0x01
#define EC_BI                   0x02
#define EC_REGISTER             0x04
#define EC_MEMORY               0x08
#define EC_PAUSABLE             0x10

#define EC_WRITE                0x02

#define EC_CHECK8               0x01
#define EC_SDLC16               0x02
#define EC_SDLC32               0x04

//Voltage Control Values
#define VCTL_VCC_MASK           0x07
#define VCTL_CISREAD            0x10
#define VCTL_OVERRIDE           0x20

#define VCTL_SENSE_MSK          0xc0    //used to isolate voltage sense
#define VCTL_50V                0x00
#define VCTL_33V                0x40
#define VCTL_XXV                0x80

#define VPP_MASK                0x07

//Interface Bit Masks
#define IF_TYPE_MASK            0x03    //Get/SetSocket
#define IF_CARDBUS              0x00    //GetSocket
#define IF_MEMORY               0x01    //Get/Inq/SetSocket
#define IF_IO                   0x02    //Get/Inq/SetSocket
#define IF_CUSTOM		0x03	//Get/SetSocket

#define IF_CB                   0x04    //InqSocket
#define IF_DMA                  0x08    //InqSocket
#define IF_VSKEY                0x10    //InqSocket
#define IF_33VCC                0x20    //InqSocket
#define IF_XXVCC                0x40    //InqSocket

#define DREQ_MASK               0x0c    //Get/SetSocket
#define DREQ_NONE               0x00    //Get/SetSocket
#define DREQ_SPKR               0x04    //Get/SetSocket
#define DREQ_IOIS16             0x08    //Get/SetSocket
#define DREQ_INPACK             0x0c    //Get/SetSocket

#define DMA_CHAN_MASK           0xf0    //Get/SetSocket
#define DMA_CHAN0               0x00    //Get/SetSocket
#define DMA_CHAN1               0x10    //Get/SetSocket
#define DMA_CHAN2               0x20    //Get/SetSocket
#define DMA_CHAN3               0x30    //Get/SetSocket
#define DMA_CHAN4               0x40    //Get/SetSocket
#define DMA_CHAN5               0x50    //Get/SetSocket
#define DMA_CHAN6               0x60    //Get/SetSocket
#define DMA_CHAN7               0x70    //Get/SetSocket
#define DMA_CHAN8               0x80    //Get/SetSocket
#define DMA_CHAN9               0x90    //Get/SetSocket
#define DMA_CHAN10              0xa0    //Get/SetSocket
#define DMA_CHAN11              0xb0    //Get/SetSocket
#define DMA_CHAN12              0xc0    //Get/SetSocket
#define DMA_CHAN13              0xd0    //Get/SetSocket
#define DMA_CHAN14              0xe0    //Get/SetSocket
#define DMA_CHAN15              0xf0    //Get/SetSocket

//Speed bits
#define SPEED_SIGN              0x80
#define SPEED_MANTISSA          0x78
#define SPEED_EXPONENT          0x07

//Socket bits
#define SKT_NUMMASK		0x0f
#define SKT_WIDTH16		0x00
#define SKT_WIDTH32		0x10

//Misc. Constants
#define EFLAGS_CARRY    0x00000001
#define EFLAGS_IF       0x00000200
#define NUM_PWR_ENTRIES	4
#define NUM_IF_ENTRIES	1

//Custom Interface index
#define IFID_ZV		0x00000141

/*** Type and Structure Definitions
 */

/*XLATOFF*/
typedef BYTE  ADP;
typedef WORD  BASE16;
typedef DWORD BASE32;
typedef WORD  BCD;
typedef BYTE  COUNT;
typedef BYTE  EDC;
typedef BYTE  FLAGS8;
typedef WORD  FLAGS16;
typedef DWORD FLAGS32;
typedef BYTE  IRQ;
typedef WORD  COFFSET;
typedef BYTE  WPAGE;
typedef BYTE  PWRINDEX;
typedef BYTE  RETCODE;
typedef WORD  SIG;
typedef WORD  WSIZE16;
typedef DWORD WSIZE32;
typedef BYTE  SOCKET;
typedef BYTE  SPEED;
typedef BYTE  WINDOW;
typedef WORD  SKTBITS;

typedef struct pwrentry_s       //power level and valid signals (InqAdapter)
{
    PWRINDEX bPowerLevel;
    FLAGS8   bValidSignals;
} PWRENTRY;

typedef struct achartbl_s       //InqAdapter
{
    FLAGS16 wAdpCaps;
    FLAGS32 dwActiveHi;
    FLAGS32 dwActiveLo;
} ACHARTBL;

typedef struct schartbl_s       //InqSocket
{
    FLAGS16 wSktCaps;
    FLAGS32 dwActiveHi;
    FLAGS32 dwActiveLo;
    FLAGS16 wDMAChannels;
    WORD    wNumCustomIF;
    DWORD   CustomIFTable[NUM_IF_ENTRIES];
} SCHARTBL;

typedef struct adapterinfo_s    //InqAdapter
{
    struct achartbl_s ACharTable;
    WORD wNumPwrEntries;
    struct pwrentry_s PwrTable[NUM_PWR_ENTRIES];
} ADAPTERINFO;

typedef struct memwintbl_s      //InqWindow for memory windows
{
    FLAGS16 wMemWndCaps;        //windows capabilities flags
    BASE16  wFirstByte;         //system address of first byte
    BASE16  wLastByte;          //system address of last byte
    WSIZE16 wMinSize;           //minimum window size
    WSIZE16 wMaxSize;           //maximum window size
    WSIZE16 wReqGran;           //size granularity
    WSIZE16 wReqBase;           //window base alignment requirement
    WSIZE16 wReqOffset;         //alignment requirement for offsets
    SPEED   bSlowest;           //slowest access speed for window
    SPEED   bFastest;           //fastest access speed for window
} MEMWINTBL;

typedef struct memwin32tbl_s	//InqWindow for big memory windows
{
    FLAGS16 wMemWndCaps;        //windows capabilities flags
    BASE32  dwFirstByte;        //system address of first byte
    BASE32  dwLastByte;         //system address of last byte
    WSIZE32 dwMinSize;          //minimum window size
    WSIZE32 dwMaxSize;          //maximum window size
    WSIZE32 dwReqGran;          //size granularity
    WSIZE32 dwReqBase;          //window base alignment requirement
    WSIZE32 dwReqOffset;        //alignment requirement for offsets
    SPEED   bSlowest;           //slowest access speed for window
    SPEED   bFastest;           //fastest access speed for window
} MEMWIN32TBL;

typedef struct iowintbl_s       //InqWindow for I/O windows
{
    FLAGS16 wIOWndCaps;         //windows capabilities flags
    BASE16  wFirstByte;         //system address of first byte
    BASE16  wLastByte;          //system address of last byte
    WSIZE16 wMinSize;           //minimum window size
    WSIZE16 wMaxSize;           //maximum window size
    WSIZE16 wReqGran;           //size granularity
    COUNT   bAddrLines;         //address lines decoded by window
    FLAGS8  bEISASlot;          //upper 4 I/O address lines for EISA
} IOWINTBL;

typedef struct iowin32tbl_s     //InqWindow for I/O windows
{
    FLAGS16 wIOWndCaps;         //windows capabilities flags
    BASE32  dwFirstByte;        //system address of first byte
    BASE32  dwLastByte;         //system address of last byte
    WSIZE32 dwMinSize;          //minimum window size
    WSIZE32 dwMaxSize;          //maximum window size
    WSIZE32 dwReqGran;          //size granularity
    COUNT   bAddrLines;         //address lines decoded by window
    FLAGS8  bEISASlot;          //upper 4 I/O address lines for EISA
} IOWIN32TBL;
/*XLATON*/

typedef struct dwframe_s {
    DWORD r_edi;
    DWORD r_esi;
    DWORD r_ebp;
    DWORD r_esp;
    DWORD r_ebx;
    DWORD r_edx;
    DWORD r_ecx;
    DWORD r_eax;
    DWORD r_eflags;
} DWFRAME;

typedef struct wframe_s {
    WORD  r_di;
    WORD  wh_di;
    WORD  r_si;
    WORD  wh_si;
    WORD  r_bp;
    WORD  wh_bp;
    WORD  r_sp;
    WORD  wh_sp;
    WORD  r_bx;
    WORD  wh_bx;
    WORD  r_dx;
    WORD  wh_dx;
    WORD  r_cx;
    WORD  wh_cx;
    WORD  r_ax;
    WORD  wh_ax;
    WORD  r_flags;
    WORD  wh_flags;
} WFRAME;

typedef struct bframe_s {
    DWORD b_edi;
    DWORD b_esi;
    DWORD b_ebp;
    DWORD b_esp;
    BYTE  r_bl;
    BYTE  r_bh;
    WORD  bh_bx;
    BYTE  r_dl;
    BYTE  r_dh;
    WORD  bh_dx;
    BYTE  r_cl;
    BYTE  r_ch;
    WORD  bh_cx;
    BYTE  r_al;
    BYTE  r_ah;
    WORD  bh_ax;
    DWORD b_eflags;
} BFRAME;

/*XLATOFF*/
typedef union paramframe_s
{
    struct dwframe_s dw;
    struct wframe_s  w;
    struct bframe_s  b;
} PARAMFRAME;
typedef PARAMFRAME *PPARAMFRAME;


/*** Macros
 */

//Register bindings for socket services parameters
#define P_EFLAGS(p)             ((p)->dw.r_eflags)

#define P_FUNC(p)               ((p)->b.r_ah)
#define P_RETCODE(p)            ((p)->b.r_ah)
#define P_ADP(p)                ((p)->b.r_al)

#define P_COMPLIANCE(p)         ((p)->w.r_bx)
#define P_WINDOW(p)             ((p)->b.r_bh)
#define P_NUMWINS(p)            ((p)->b.r_bh)
#define P_SCINTCAPS(p)          ((p)->b.r_bh)
#define P_SCINTMASK(p)          ((p)->b.r_bh)
#define P_CARDSTATE(p)          ((p)->b.r_bh)
#define P_NUMSKTS(p)            ((p)->b.r_bl)
#define P_WINCAPS(p)            ((p)->b.r_bl)
#define P_SOCKET(p)             ((p)->b.r_bl)
#define P_PAGE(p)               ((p)->b.r_bl)
#define P_TYPE(p)               ((p)->b.r_bl)

#define P_NUMEDCS(p)            ((p)->w.r_cx)
#define P_SOCKETS(p)            ((p)->w.r_cx)
#define P_WINSIZE(p)            ((p)->dw.r_ecx)
#define P_NUMADPS(p)            ((p)->b.r_ch)
#define P_VCONTROL(p)           ((p)->b.r_ch)
#define P_FIRSTADP(p)           ((p)->b.r_cl)
#define P_VPPLEVELS(p)          ((p)->b.r_cl)

#define P_NUMBWINS(p)           ((p)->w.r_dx)
#define P_RELEASE(p)            ((p)->w.r_dx)
#define P_WIN32(p)		((p)->b.r_dh)
#define P_STATE(p)              ((p)->b.r_dh)
#define P_SCRPTCAPS(p)          ((p)->b.r_dh)
#define P_SPEED(p)              ((p)->b.r_dl)
#define P_CTLINDCAPS(p)         ((p)->b.r_dl)
#define P_CTLIND(p)             ((p)->b.r_dl)
#define P_PAGESTATE(p)          ((p)->b.r_dl)

#define P_SCROUTING(p)          ((p)->w.r_di)
#define P_BASE(p)               ((p)->dw.r_edi)
#define P_OFFSET(p)             ((p)->w.r_di)
#define P_IF_ROUTING(p)         ((p)->w.r_di)
#define P_PBUFFER(p)            ((p)->dw.r_edi)

#define P_IFINDEX(p)		((p)->w.r_bp)

#define SETUPCARRY(p)   {                                               \
                            if (P_RETCODE(p) == SSERR_SUCCESS)          \
                                P_EFLAGS(p) &= ~EFLAGS_CARRY;           \
                            else                                        \
                                P_EFLAGS(p) |= EFLAGS_CARRY;            \
                        }
/*XLATON*/

#endif  //ifndef _SSDEFS_H
