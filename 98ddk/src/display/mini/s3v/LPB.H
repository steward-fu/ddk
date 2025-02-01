
/*
** LPB Register Definitions
**
** V1: Trio64V+, Aurora64V+, ViRGE, ViRGE VX
** V2: Trio64V2, ViRGE DX/GX
** V3: M3/M5, ViRGE GX2
**
** Use H2INC.EXE to generate LPB.INC from LPB.H
*/

#define LPB_BASE_OFFSET         0x0000FF00

#define LPB_MODE                    0x0000FF00
#define     LPB_ENABLE              0x00000001
#define     LPB_TYPE_SHIFT          1
#define     LPB_TYPE_MASK           0x0000000E
#define         LPB_TYPE_CP3            0
#define         LPB_TYPE_VIDEO16        1
#define         LPB_TYPE_VIDEO8         2
#define         LPB_TYPE_CL480          3
#define         LPB_TYPE_PASSTHRU       4
#define     LPB_DORESET             0x00000010
#define     LPB_SKIPFIELDS          0x00000020
#define     LPB_BYTESWAP            0x00000040
#define     LPB_YUV422DROP_V2       0x00000080
#define     LPB_VSYNC_HIGH          0x00000200
#define     LPB_HSYNC_HIGH          0x00000400
#define     LPB_DOVSYNC             0x00000800
#define     LPB_DOHSYNC             0x00001000
#define     LPB_DOLOADBASEADDR      0x00002000
#define     LPB_AUTOSYNCPASSTHRU_V2 0x00004000
#define     LPB_BURST_SIZE_MASK     0x00030000
#define     LPB_BURST_SIZE_SHIFT    16
#define         LPB_BURST_1             0
#define         LPB_BURST_2             1
#define         LPB_BURST_3             2
#define         LPB_BURST_ALL           3
#define     LPB_VBICAPTURE_V3       0x00080000
#define     LPB_EARLYHSYNC_V2       0x00100000
#define     LPB_FIFOSLOTS_SHIFT     21
#define     LPB_FIFOSLOTS_MASK_V1   0x00600000
#define     LPB_FIFOSLOTS_MASK_V2   0x00E00000
#define     LPB_FIFOSLOTS_MASK_V3   0x00000000
#define     LPB_CLOCK_LCLK          0x01000000
#define     LPB_IGNOREHALFLINE      0x02000000
#define     LPB_INVERTLCLK          0x04000000
#define     LPB_BASE1ACTIVE_V3      0x08000000
#define     LPB_EVENFIELD_V2        0x10000000
#define     LPB_INVERTFIELD_V2      0x20000000
#define     LPB_DEINTERLACE_V2      0x40000000
#define     LPB_V16DEINTERLACE_V3   0x80000000

#define LPB_FIFO_STATUS         0x0000FF04
#define     LPB_OSLOTS_SHIFT        0
#define     LPB_OFLOTS_MASK         0x0000000F
#define     LPB_OFULL               0x00000800
#define     LPB_OEMPTY              0x00001000
#define     LPB_OALMOSTEMPTY        0x00002000
#define     LPB_V0FULL              0x00100000
#define     LPB_V0EMPTY             0x00200000
#define     LPB_V0ALMOSTEMPTY       0x00400000
#define     LPB_V1FULL              0x20000000
#define     LPB_V1EMPTY             0x40000000
#define     LPB_V1ALMOSTEMPTY       0x80000000

#define LPB_INTERRUPT_FLAGS     0x0000FF08
#define     LPB_INT_OEMPTY          0x00000001
#define     LPB_INT_HSYNC           0x00000002
#define     LPB_INT_VSYNC           0x00000004
#define     LPB_INT_I2CSTART        0x00000008
#define     LPB_INT_VSYNCEND_V3     0x00000080
#define     LPB_IENABLE_OEMPTY      0x00010000
#define     LPB_IENABLE_HSYNC       0x00020000
#define     LPB_IENABLE_VSYNC       0x00040000
#define     LPB_IENABLE_I2CSTART    0x00080000
#define     LPB_IENABLE_VSYNCEND_V3 0x00800000
#define     LPB_SPCLK_LOW           0x01000000

#define LPB_BASE0               0x0000FF0C
#define     LPB_BASE0_SHIFT         0
#define     LPB_BASE0_MASK          0x003FFFFF

#define LPB_BASE1               0x0000FF10
#define     LPB_BASE1_SHIFT         0
#define     LPB_BASE1_MASK          0x003FFFFF

#define LPB_CMD                 0x0000FF14
#define     LPB_CMD_ADDR_SHIFT      0
#define     LPB_CMD_ADDR_MASK       0x001FFFFF
#define     LPB_CMD_TYPE_SHIFT      21
#define     LPB_CMD_TYPE_MASK       0x00E00000
#define         LPB_CMD_WRITE           0
#define         LPB_CMD_READ            1
#define         LPB_CMD_OFIFO           5

#define LPB_CMD_DATA            0x0000FF18

#define LPB_GPIO                0x0000FF1C
#define     LPB_GPOUT_SHIFT         0
#define     LPB_GPOUT_MASK          0x0000000F
#define     LPB_GPIN_SHIFT          4
#define     LPB_GPIN_MASK           0x000000F0

#define LPB_I2C                 0x0000FF20
#define     LPB_I2C_CLK_WRITE       0x00000001
#define     LPB_I2C_DATA_WRITE      0x00000002
#define     LPB_I2C_CLK_READ        0x00000004
#define     LPB_I2C_DATA_READ       0x00000008
#define     LPB_I2C_ENABLE          0x00000010
#define     LPB_I2C_BIT0MIRROR      0x00000100
#define     LPB_I2C_BIT1MIRROR      0x00000200
#define     LPB_I2C_BIT2MIRROR      0x00000400
#define     LPB_I2C_BIT3MIRROR      0x00000800
#define     LPB_I2C_BIT4MIRROR      0x00001000

#define LPB_INPUT_SIZE          0x0000FF24
#define     LPB_INPUT_WIDTH_SHIFT   0
#define     LPB_INPUT_WIDTH_MASK    0x00000FFF
#define     LPB_INPUT_HEIGHT_SHIFT  16
#define     LPB_INPUT_HEIGHT_MASK   0x01FF0000

#define LPB_INPUT_OFFSETS       0x0000FF28
#define     LPB_INPUT_HOFFSET_SHIFT 0
#define     LPB_INPUT_HOFFSET_MASK  0x00000FFF
#define     LPB_INPUT_VOFFSET_SHIFT 16
#define     LPB_INPUT_VOFFSET_MASK  0x01FF0000

#define LPB_HDECIMATION_PATTERN 0x0000FF2C

#define LPB_VDECIMATION_PATTERN 0x0000FF30

#define LPB_OUTPUT_STRIDE       0x0000FF34
#define     LPB_OUTPUT_STRIDE_SHIFT 0
#define     LPB_OUTPUT_STRIDE_MASK  0x00000FFF

#define LPB_INPUT_FIFO          0x0000FF40

#define LPB_REGS_SIZE           0x0000005C

#define LPB_FLAGS_INVERTPOLARITY 0x01

#ifndef MASM6

/*
** Allocated in the sData (S3THKINFO) datastructure
*/
typedef struct _LPB {
    BYTE        jInputWidthPixelAlign;
    BYTE        jGUIDIndex;
    BYTE        jFamily;
    BYTE        jFlags;
    DWORD       dwBaseOffset;
    DWORD       *HDecimationTable;
    DWORD       *VDecimationTable;
    WORD        HDecimationSteps;
    WORD        VDecimationSteps;
    DWORD       dwVDecimationPattern;   // the current VDecimation pattern
    DWORD       dwNumBuffers;           // number of attached, flippable buffers
    DWORD       dwBufferSize;           // size of buffers in KB
    FLATPTR     fpCurrentSurface;
    DDVIDEOPORTCAPS ddVideoPortCaps;    // the capabilities of this instance
} LPB;
typedef LPB* PLPB;

DWORD PASCAL LPB_CanCreate(LPDDHAL_CANCREATEVPORTDATA lpInput);
DWORD PASCAL LPB_Create(LPDDHAL_CREATEVPORTDATA lpInput);
DWORD PASCAL LPB_Flip(LPDDHAL_FLIPVPORTDATA lpInput);
DWORD PASCAL LPB_GetBandwidth(LPDDHAL_GETVPORTBANDWIDTHDATA lpInput);
DWORD PASCAL LPB_GetInputFormats(LPDDHAL_GETVPORTINPUTFORMATDATA lpInput);
DWORD PASCAL LPB_GetOutputFormats(LPDDHAL_GETVPORTOUTPUTFORMATDATA lpInput);
DWORD PASCAL LPB_GetField(LPDDHAL_GETVPORTFIELDDATA lpInput);
DWORD PASCAL LPB_GetLine(LPDDHAL_GETVPORTLINEDATA lpInput);
DWORD PASCAL LPB_GetConnect(LPDDHAL_GETVPORTCONNECTDATA lpInput);
DWORD PASCAL LPB_Destroy(LPDDHAL_DESTROYVPORTDATA lpInput);
DWORD PASCAL LPB_GetFlipStatus(LPDDHAL_GETVPORTFLIPSTATUSDATA lpInput);
DWORD PASCAL LPB_Update(LPDDHAL_UPDATEVPORTDATA lpInput);
DWORD PASCAL LPB_WaitForSync(LPDDHAL_WAITFORVPORTSYNCDATA lpInput);
DWORD PASCAL LPB_GetSignalStatus(LPDDHAL_GETVPORTSIGNALDATA lpInput);
DWORD PASCAL LPB_ColorControl(LPDDHAL_VPORTCOLORDATA lpInput);

extern const DDHAL_DDVIDEOPORTCALLBACKS LPBVideoPortCallbacks;

#endif
