/**********************************************************
* Copyright S3, 1997. All rights reserved.
* Copyright Microsoft, 1996. All rights reserved.
***********************************************************
*       File Name:  LPB.C
*
*       Our S3 LPB (Local Peripheral Bus) is a type of Video Port.
*       This file is an implementation of the Direct Draw
*       Video Port interfaces which programs the S3 LPB.
*
* Created: 12/20/96 Bernie Thompson
*
***********************************************************/

/* #includes ---------------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <ddrawi.h>

#include "ddkernel.h"
#include "d3dhal.h"
#include "s3_ddtk.h"
#include "mpc.h"

const GUID *guids[] =
{
    &DDVPTYPE_E_HREFH_VREFH,
    &DDVPTYPE_E_HREFH_VREFL,
    &DDVPTYPE_E_HREFL_VREFH,
    &DDVPTYPE_E_HREFL_VREFL,
//    &DDVPTYPE_CCIR656,
//    &DDVPTYPE_BROOKTREE,
//    &DDVPTYPE_PHILIPS
};
#define MAX_VP_TYPES    (sizeof(guids)/sizeof(void*))
#define HREFH_VREFH 0
#define HREFH_VREFL 1
#define HREFL_VREFH 2
#define HREFL_VREFL 3
//#define CCIR656     4
//#define BROOKTREE   5
//#define PHILIPS     6

const DDPIXELFORMAT InputFormats[] =
{
    {sizeof(DDPIXELFORMAT),DDPF_FOURCC, mmioFOURCC('Y','U','Y','2'),16,(DWORD)-1,(DWORD)-1,(DWORD)-1},
    {sizeof(DDPIXELFORMAT),DDPF_FOURCC, mmioFOURCC('U','Y','V','Y'),16,(DWORD)-1,(DWORD)-1,(DWORD)-1}
};
#define MAX_INPUT_FORMATS (sizeof(InputFormats)/sizeof(DDPIXELFORMAT))
#define INPUTFORMAT_YUY2    0
#define INPUTFORMAT_UYVY    1

const DDVIDEOPORTCAPS ddLPBVideoPortCaps =
{
    sizeof(DDVIDEOPORTCAPS),    // size of the DDVIDEOPORTCAPS structure
    DDVPD_WIDTH             |
    DDVPD_HEIGHT            |
    DDVPD_ID                |
    DDVPD_CAPS              |
    DDVPD_FX                |
    DDVPD_AUTOFLIP          |
    DDVPD_ALIGN             |
    0,
    1024,                       // VP max width (limited by FF24)
    1024,                       // max VBI data width (limited by FF24)
    512,                        // VP max height (limited by FF24)
    0,                          // VP ID (0 - (dwMaxVideoPorts-1))
    DDVPCAPS_AUTOFLIP       |   // LPB supports 2 autoflip buffers
    DDVPCAPS_INTERLACED     |
    DDVPCAPS_NONINTERLACED  |
    DDVPCAPS_READBACKFIELD  |
//  DDVPCAPS_READBACKLINE   |   // LPB doesn't support this
//  DDVPCAPS_SHAREABLE      |   // LPB doesn't support this
    DDVPCAPS_SKIPEVENFIELDS |   // when does FF00_5 start dropping ?BJT
//  DDVPCAPS_SKIPODDFIELDS  |   // when does FF00_5 start dropping ?BJT
//  DDVPCAPS_SYNCMASTER     |   // LPB doesn't support this
//  DDVPCAPS_VBISURFACE     |   // GX2 places VBI data in same surface
//  DDVPCAPS_COLORCONTROL   |   // LPB doesn't support this. SP does.
    0,
//  DDVPFX_CROPTOPDATA      |   // We don't know VBI size without being told
    DDVPFX_CROPX            |
    DDVPFX_CROPY            |
    DDVPFX_INTERLEAVE       |
//  DDVPFX_MIRRORLEFTRIGHT  |   // LPB doesn't support this
//  DDVPFX_MIRRORUPDOWN     |   // LPB doesn't support this
//  DDVPFX_PRESHRINKX       |   // LPB can't arbitrarily shrink
//  DDVPFX_PRESHRINKY       |   // LPB can't arbitrarily shrink
    DDVPFX_PRESHRINKXS      |   // LPB shrinks by byte/pixel dropping
    DDVPFX_PRESHRINKYS      |   // LPB shrinks by byte/pixel dropping
    DDVPFX_PRESHRINKXB      |   // down to 1/2, 1/4, 1/8 or 1/16
    DDVPFX_PRESHRINKYB      |   // as above
//  DDVPFX_PRESTRETCHX      |   // LPB never stretches
//  DDVPFX_PRESTRETCHY      |   // LPB never stretches
//  DDVPFX_PRESTRETCHXN     |   // LPB never stretches
//  DDVPFX_PRESTRETCHYN     |   // LPB never stretches
//  DDVPFX_VBICONVERT       |   // LPB doesn't have special VBI conversion
//  DDVPFX_VBINOSCALE       |   // TBD: GX2 has some VBI support
    0,
    2,                          // Number of autoflippable surfaces
    8,                          // Byte restriction of placement within surface
    1,                          // Byte restriction of width after prescaling
    1,                          // Byte restriction of left cropping
    1,                          // Byte restriction of cropping width
    16,                         // shrink in steps of original width
    32,                         // shrink in steps of original height
    0,
    0,
    0
};

const DDHAL_DDVIDEOPORTCALLBACKS LPBVideoPortCallbacks =
{
    sizeof(DDHAL_DDVIDEOPORTCALLBACKS),
    DDHAL_VPORT32_CANCREATEVIDEOPORT |
    DDHAL_VPORT32_CREATEVIDEOPORT |
    DDHAL_VPORT32_FLIP |
    DDHAL_VPORT32_GETBANDWIDTH |
    DDHAL_VPORT32_GETINPUTFORMATS |
    DDHAL_VPORT32_GETOUTPUTFORMATS |
    DDHAL_VPORT32_GETFIELD |
    DDHAL_VPORT32_GETLINE |
    DDHAL_VPORT32_GETCONNECT |
    DDHAL_VPORT32_DESTROY |
    DDHAL_VPORT32_GETFLIPSTATUS |
    DDHAL_VPORT32_UPDATE |
    DDHAL_VPORT32_WAITFORSYNC |
    DDHAL_VPORT32_GETSIGNALSTATUS |
    DDHAL_VPORT32_COLORCONTROL |
    0,
    &LPB_CanCreate,
    &LPB_Create,
    &LPB_Flip,
    &LPB_GetBandwidth,
    &LPB_GetInputFormats,
    &LPB_GetOutputFormats,
    (LPVOID)0,
    &LPB_GetField,
    &LPB_GetLine,
    &LPB_GetConnect,
    &LPB_Destroy,
    &LPB_GetFlipStatus,
    &LPB_Update,
    &LPB_WaitForSync,
    &LPB_GetSignalStatus,
    &LPB_ColorControl
};

const DWORD dwConnectionCaps=
                                DDVPCONNECT_INVERTPOLARITY      |
                                DDVPCONNECT_DISCARDSVREFDATA    |
//                                DDVPCONNECT_HALFLINE            |
                                DDVPCONNECT_INTERLACED          |
//                                DDVPCONNECT_SHAREEVEN           |
                                DDVPCONNECT_VACT                |
//                                DDVPCONNECT_DOUBLECLOCK         |
//                                DDVPCONNECT_SHAREODD            |
                                0
                                ;

/*
** FF30 Vertical Decimation table
*/
const DWORD DV[] = {
    0x7fffffff, 0x7fff7fff, 0x7f7f7fff, 0x7f7f7f7f,
    0x777f7f7f, 0x777f777f, 0x7777777f, 0x77777777,
    0x57777777, 0x57775777, 0x57575777, 0x57575757,
    0x55575757, 0x55575557, 0x55555557, 0x55555555,
    0x15555555, 0x15551555, 0x15151555, 0x15151515,
    0x11151515, 0x11151115, 0x11111115, 0x11111111,
    0x01111111, 0x01110111, 0x01010111, 0x01010101,
    0x00010101, 0x00010001, 0x00000001, 0x00000000
};

/*
** FF2C Horizontal Decimation tables
** Varies depending on:
**      8 or 16 bit LPB width
**      BYTE or YUV Pixel dropping
**      Special chip bugs
*/
const DWORD DH_16_BYTE[] = {
    0x3fffffff, 0x3fff3fff, 0x3f3f3fff, 0x3f3f3f3f,
    0x333f3f3f, 0x333f333f, 0x3333333f, 0x33333333,
    0x03333333, 0x03330333, 0x03030333, 0x03030303,
    0x00030303, 0x00030003, 0x00000003, 0x00000000
};
const DWORD DH_8_BYTE[] = {
    0x3cffffff, 0x3cff3cff, 0x3c3c3cff, 0x3c3c3c3c,
    0x003c3c3c, 0x003c003c, 0x0000003c, 0x00000000
};

const DWORD DH_16_YUV[] = {
    0x7fff7fff, 0x7f7f7f7f, 0x777f777f, 0x77777777,
    0x57775777, 0x57575757, 0x55575557, 0x55555555,
    0x15551555, 0x15151515, 0x11151115, 0x11111111,
    0x01110111, 0x01010101, 0x00010001, 0x00000000
};

const DWORD DH_16_YUV_WORDALIGN[] = {
    0x7ffeffff, 0x7f7f7f7f, 0x7777f77f, 0x77777777,
    0x57757777, 0x57575757, 0x55557557, 0x55555555,
    0x15515555, 0x15151515, 0x11115115, 0x11111111,
    0x01101111, 0x01010101, 0x00008001, 0x00000000
};

const DWORD DH_8_VXMPEG[] = {
    0x7f7f7f7f, 0x77777777, 0x57575757, 0x55555555,
    0x15151515, 0x11111111, 0x01010101, 0x00000000
};

/* our private instance data */
#define VP_GETSDATA(x) VERIFYSDATA((LPS3THKINFO)(x->lpDD->lpGbl->dwReserved3))
#define pLPB (&(sData->LPBData))

/* debugging support */
#ifdef DEBUG
#define TRACE   Msg
#define DPF             Msg
#define INT3 _asm int 3
extern void __cdecl Msg( LPSTR szFormat, ... );
#define assert(test) ((test) ? (void) 0 : DPF("* FAILURE * assertion %s: %s line %d",#test,__FILE__,__LINE__))
#else
#define INT3    (void)0
#define DPF             1 ? (void)0 : (void)
#define TRACE   1 ? (void)0 : (void)
#define assert  1 ? (void)0 : (void)
#endif

/* local service functions */
static int IndexFromGUID(GUID *pGUID);
#define RoundUp(x,y) ((x+(y-1)) & ~(y-1)) // round x up to nearest y
void  SR_RMW(LPS3THKINFO sData, BYTE jIndex, BYTE jClear, BYTE jSet);
void  CR_RMW(LPS3THKINFO sData, BYTE jIndex, BYTE jClear, BYTE jSet);
void LPB_Enable(LPS3THKINFO sData);
void LPB_Reset(LPS3THKINFO sData);
void LPB_Disable(LPS3THKINFO sData);
void LPB_Freeze(LPS3THKINFO sData);
void LPB_UnFreeze(LPS3THKINFO sData);
DWORD LPB_Get(LPS3THKINFO sData, DWORD reg);
void  LPB_Set(LPS3THKINFO sData, DWORD reg, DWORD value);
void  LPB_RMW(LPS3THKINFO sData, DWORD reg, DWORD dwClear, DWORD dwSet);

DWORD LPB_DriverInit(LPS3THKINFO sData)
{
    HKEY  hKey;
    DWORD type, size;
    LONG  ret;
    char  buff[16];
    DWORD id = sData->wChipId;

    size = sizeof(buff);

    pLPB->HDecimationTable = (DWORD*)DH_16_YUV;
    pLPB->HDecimationSteps = 16;
    pLPB->VDecimationTable = (DWORD*)DV;
    pLPB->VDecimationSteps = 32;
    pLPB->jInputWidthPixelAlign = 16;
    pLPB->dwVDecimationPattern = 0xFFFFFFFF;

    memcpy(&(pLPB->ddVideoPortCaps), &ddLPBVideoPortCaps, sizeof(ddLPBVideoPortCaps));

    assert(sData->dwSize == sizeof(S3THKINFO));

    sData->ddHALInfo.ddCaps.dwMaxVideoPorts = 1;
    sData->ddHALInfo.ddCaps.dwCurrVideoPorts = 0;

    switch(id)
    {
        case 0x8811:
        case 0x5631:
        case 0x883D:
        case 0x8812:
        case 0x8814:
            // Hardware with no odd/even field detection
            pLPB->jFamily = 1;
            pLPB->ddVideoPortCaps.dwCaps &= ~(DDVPCAPS_READBACKFIELD);
            pLPB->ddVideoPortCaps.dwFX &= ~(DDVPFX_INTERLEAVE | DDVPFX_VBINOSCALE);
            pLPB->HDecimationTable = (DWORD*)DH_8_BYTE;
            pLPB->HDecimationSteps = 8;
            break;
        case 0x8901:
        case 0x8902:
        case 0x8A01:
            /*
            ** Hardware with odd/even field detection
            */
            pLPB->jFamily = 2;
            break;

        case 0x8C00:
        case 0x8C01:
        case 0x8A10:
        default:
            pLPB->jFamily = 3;
            break;
    }

    RegOpenKeyEx((HKEY)HKEY_CURRENT_CONFIG, "Display\\Settings", 0, KEY_READ, &hKey);

    ret = RegQueryValueEx(hKey, "INTERLEAVE", 0, &type, buff, &size);
    if ( ret == ERROR_SUCCESS )
    {
        if (lstrcmp(buff,"OFF") == 0)
            pLPB->ddVideoPortCaps.dwFX &= ~DDVPFX_INTERLEAVE;
        if (lstrcmp(buff,"ON") == 0)
            pLPB->ddVideoPortCaps.dwFX |= DDVPFX_INTERLEAVE;
    }

    /*
    ** For a given decoder and the S3, the odd/even polarity seems
    ** to vary, depending on how the hardware pins and wiring
    ** is implemented.  If text in video playback is very flashy,
    ** it is quite possible that this registry setting will have to
    ** be modified on that configuration to invert the polarity.
    ** hopefully these kinds of problems will be remidied by VIP
    ** in future hardware.
    */
    pLPB->jFlags &= ~LPB_FLAGS_INVERTPOLARITY;
    ret = RegQueryValueEx(hKey, "INVERTPOLARITY", 0, &type, buff, &size);
    if ( ret == ERROR_SUCCESS )
    {
        if (lstrcmp(buff,"OFF") == 0)
            pLPB->jFlags &= ~LPB_FLAGS_INVERTPOLARITY;
        if (lstrcmp(buff,"ON") == 0)
            pLPB->jFlags |= LPB_FLAGS_INVERTPOLARITY;
    }
    return 0;
}

/*
 * CanCreateVideoPort32
 *
 * This function is required
 */
DWORD PASCAL LPB_CanCreate(LPDDHAL_CANCREATEVPORTDATA lpInput)
{
    LPDDVIDEOPORTCONNECT pConnect;
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    TRACE("LPB_CanCreate");

    /*
     * If the lpInput->lpDDVideoPortDesc can not be supported, set
     * lpInput->ddRVal to the correct error code and return
     * DDHAL_DRIVER_HANDLED
     */
    pConnect = &(lpInput->lpDDVideoPortDesc->VideoPortType);

    // Check if connection type is a supported one
    if (IndexFromGUID(&(pConnect->guidTypeID)) == -1)
    {
        lpInput->ddRVal = DDERR_INVALIDDIRECTDRAWGUID;  // TBD: Better error code
        return DDHAL_DRIVER_HANDLED;
    }

    // everything is OK
    lpInput->ddRVal = 0;
    return DDHAL_DRIVER_NOTHANDLED;
}


/*
 * CreateVideoPort32
 *
 * This function is optional
 */
DWORD PASCAL LPB_Create(LPDDHAL_CREATEVPORTDATA lpInput)
{
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    /* This function allows the HAL to set internal flags and
     * become aware that a video port has been created. It should
     * always return DDHAL_DRIVER_NOTHANDLED.
     */

    TRACE("LPB_Create 1.3");

    pLPB->jGUIDIndex = IndexFromGUID(&(lpInput->lpVideoPort->ddvpDesc.VideoPortType.guidTypeID));

    /*
    ** check if 1CYCLELPB needs to be set to avoid pajama system hang
    */
    if (sData->band.HWFlags & HW_1CYCLELPB)
    {
        LPB_RMW(sData,0x81EC,0,0x00040000);
    }

    /*
    ** Configure pins for LPB, instead of VESA Feature Connector
    ** TBD: Clean up this doc. It is not yet accurate
    ** Configure LPB to force configuration pins 33 & 34 on the LPB connector.
    ** These pins are normally 'set' by the decoder/digitizer board.
    ** 33 is EN1 34 is EN2
    ** EN2  EN1   Description
    ** 0    0     Feature Connector mode (external board drives monitor HSYNC/VSYNC)
    ** 0    1     CP3 Mode (S3's hardware MPEG-2 decoder)
    ** 1    0     Normal LPB operation
    ** 1    1     Reserved
    */

    switch(sData->wChipId)
    {
        case 0x8812:
        case 0x8C00:
        case 0x8C01:
        case 0x8A10:
            /*
            ** "Mobile" products have different pin definitions for SR0D and CR5C
            */
            SR_RMW(sData, 0x0d, 3, 1);
            CR_RMW(sData, 0x5c, 3, 0);
            break;
        default:
            SR_RMW(sData, 0x0d, 3, 0);
            /*
            ** Set Normal LPB operation
            ** some Compaq pre-production hardware requires 'reserved' mode.
            ** those machines will have bits 0+1 set by system BIOS at boot.
            ** don't clear bit 0, to allow these machines to work
            ** (for now, should clear both bits when noone cares about the
            ** preproduction hardware anymore).
            */
            CR_RMW(sData, 0x5c, 2, 2);
            break;
    }

    SR_RMW(sData, 0x1c, 3, 2);  // Enable GOP0 and GOP1 pins (disable ENFEAT and STWR)

    /*
    ** Disable LPB until we receive a START
    */
    LPB_Disable(sData);

    lpInput->ddRVal = 0;
    return DDHAL_DRIVER_NOTHANDLED;
}

/*
 * FlipVideoPort32
 *
 * This function is required
 */
DWORD PASCAL LPB_Flip(LPDDHAL_FLIPVPORTDATA lpInput)
{
    DWORD dwBase;
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    TRACE("LPB_Flip");

    /*
     * Program registers to do the actual flip
     */
    dwBase = lpInput->lpSurfTarg->lpGbl->fpVidMem - sData->dwScreenFlatAddr;

    /* Account for "placement of video data within surface" */
    dwBase += pLPB->dwBaseOffset;

    LPB_Set(sData, LPB_BASE0, dwBase);

    /*
     * Save information for BOTH lpSurfCurr so we can know in the future
     * when the physical flip has occurred.
     */
    pLPB->fpCurrentSurface = dwBase;

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
 * GetVideoPortBandwidth32
 *
 * This function is required
 */
DWORD PASCAL LPB_GetBandwidth(LPDDHAL_GETVPORTBANDWIDTHDATA lpInput)
{
    DDVIDEOPORTBANDWIDTH *lpOutput = lpInput->lpBandwidth;
    DDVIDEOPORTINFO *pInfo = &(lpInput->lpVideoPort->ddvpInfo);
    DDVIDEOPORTDESC *pDesc = &(lpInput->lpVideoPort->ddvpDesc);
    BAND TempBand;                   // Private local copy we can modify
    SWORD x,y;
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    TRACE("LPB_GetBandwidth");

    lpOutput->dwSize = sizeof(DDVIDEOPORTBANDWIDTH);
    lpOutput->dwCaps = DDVPBCAPS_DESTINATION;

    if (!(lpInput->dwFlags & DDVPB_TYPE))
    {

        /*
         * Determine how much bandwidth is available for the specified
         * output format and fill in the information.
         */

        /*
        ** Use Bandwidth datastructure already initialized by DirectDraw, so we
        ** don't need to query all the hardware parameters needed to call
        ** Bandwidth_Init and Bandwidth_SetPrimary.  We need to make our own
        ** copy, since we'll be modifying it with our SetSecondary() call.
        */
        memcpy(&TempBand, &(sData->band),sizeof(BAND));

        x = (SWORD)pDesc->dwFieldWidth;
        y = (SWORD)pDesc->dwFieldHeight;

        Bandwidth_SetSecondary(&TempBand,
                                ID_LVIDEO,
                                16,
                                x,
                                y,
                                x,
                                y,
                                x,
                                y);

        // Bandwidth_SetSecondary(&TempBand, ID_LVIDEO, 16, 320, 240, 320, 240, 320, 240);
        /*
        ** Return values calculated by mpc.c
        ** We don't currently calculate seperate values with or without colorkey.
        ** TBD: We should enhance the bandwidth calculations to do that.
        ** If we do, we can return 4 different values below, rather than just 2.
        */

        lpOutput->dwOverlay = TempBand.HStretch;
        lpOutput->dwColorkey = TempBand.HStretch;
        lpOutput->dwYInterpolate = TempBand.HStretchVFilter;
        lpOutput->dwYInterpAndColorkey = TempBand.HStretchVFilter;
    }

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}


/*
 * GetVideoPortInputFormat32
 *
 * This function is required
 */
DWORD PASCAL LPB_GetInputFormats(LPDDHAL_GETVPORTINPUTFORMATDATA lpInput)
{
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    /*
     * The HAL is guaranteed that the buffer in lpInput->lpddpfFormat
     * is large enough to hold the information
     */
    lpInput->dwNumFormats = MAX_INPUT_FORMATS;

    if (lpInput->lpddpfFormat != NULL)
    {
        TRACE("LPB_GetInputFormat: Returning first %d input formats",lpInput->dwNumFormats);
        memcpy (lpInput->lpddpfFormat, InputFormats, (sizeof(DDPIXELFORMAT)*lpInput->dwNumFormats));
    }
    else
    {
        TRACE("LPB_GetInputFormat: %d input formats available",lpInput->dwNumFormats);
    }

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}


/*
 * GetVideoPortOutputFormat32
 *
 * This function is required
 */
DWORD PASCAL LPB_GetOutputFormats(LPDDHAL_GETVPORTOUTPUTFORMATDATA lpInput)
{
    LPS3THKINFO sData = VP_GETSDATA(lpInput);
    /*
    ** LPB Needs YUV data to be YUY2.  So return the output format for both
    ** YUY2 and UYVY as YUY2
    */

    lpInput->dwNumFormats = 1;
    if (lpInput->lpddpfOutputFormats != NULL)
    {
        TRACE("LPB_GetOutputFormats: returning YUY2");
        memcpy (lpInput->lpddpfOutputFormats, &InputFormats[INPUTFORMAT_YUY2],
                sizeof (DDPIXELFORMAT));
    }
    else
    {
        TRACE("LPB_GetOutputFormats: 1 output format available");
    }

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}


/*
 * GetVideoPortConnect32
 *
 * Returns the GUIDs describing the connections that can be supported.
 * Think of a GUID simply as a 128 bit FOURCC that you don't have to
 * register.  This function is required.
 */
DWORD PASCAL LPB_GetConnect(LPDDHAL_GETVPORTCONNECTDATA lpInput)
{
    DWORD i, j;
    LPDDVIDEOPORTCONNECT pEntry;
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    TRACE("LPB_GetConnect");

    /*
     * If lpInput->lpGUIDs is NULL, simply return the number of GUIDS
     * supported
     */

    lpInput->dwNumEntries = MAX_VP_TYPES*2;

    if (lpInput->lpConnect != NULL)
    {
        /*
         * Otherwise, copy the supported GUIDs into the specifed buffer.
         * The HAL is guarenteed that the buffer is large enough to hold
         * the GUIDs.
         */
        for (i=0;i <= 1; i++)   // do twice.. 8 bit and 16 bit connections
        {
            for (j=0;j < MAX_VP_TYPES; j++)
            {
                pEntry = &(lpInput->lpConnect[j+(i*MAX_VP_TYPES)]);
                memcpy(&(pEntry->guidTypeID), guids[j], sizeof(GUID));
                pEntry->dwPortWidth = (i+1) * 8;
                pEntry->dwFlags = dwConnectionCaps;
            }
        }
    }

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}


/*
 * DestroyVideoPort32
 *
 * This optional function notifies the HAL when the video port
 * has been destroyed.
 */
DWORD PASCAL LPB_Destroy(LPDDHAL_DESTROYVPORTDATA lpInput)
{
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    TRACE("LPB_Destroy");

    pLPB->dwVDecimationPattern = 0xFFFFFFFF;
    LPB_Freeze(sData);
    LPB_Reset(sData);
    LPB_Disable(sData);

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_NOTHANDLED;
}


/*
 * GetVideoPortFlipStatus32
 *
 * This required function allows DDRAW to restrict access to a surface
 * until the physical flip has occurred, allowing doubled buffered capture.
 */
DWORD PASCAL LPB_GetFlipStatus(LPDDHAL_GETVPORTFLIPSTATUSDATA lpInput)
{
    DWORD dwBase;
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    TRACE("LPB_GetFlipStatus");

    /*
     * If the physical flip has not yet occurred, return
     * DDERR_WASSTILLDRAWING
     */
    dwBase = LPB_Get(sData, LPB_BASE0);

    if (dwBase != pLPB->fpCurrentSurface)
    {
        lpInput->ddRVal = DDERR_WASSTILLDRAWING;
    } else {
        lpInput->ddRVal = DD_OK;
    }

    return DDHAL_DRIVER_HANDLED;
}

/*
** UpdateVideoPort32
**
** This required function sets up the video port
*/
DWORD PASCAL LPB_Update(LPDDHAL_UPDATEVPORTDATA lpInput)
{
    LPDDRAWI_DDRAWSURFACE_LCL   lpSrcLcl;// Source surface struct
    DWORD dwMode = 0;
    DWORD dwBytesPerPixel;
    DWORD dwStride;
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    TRACE("LPB_Update, %ld autoflip surfaces",lpInput->dwNumAutoflip);

    /*
    ** Determine whether hardware autoflipping (DDVP_AUTOFLIP) will be used
    ** If we get called with DDVP_AUTOFLIP set, that means we're being asked
    ** about hardware autoflip.  Software autoflip will not be reflected in
    ** the DDVP_AUTOFLIP flags.
    */
    if (lpInput->lpVideoInfo->dwVPFlags & DDVP_AUTOFLIP)
    {
        BOOL bAutoflip = FALSE;

        if (lpInput->dwNumAutoflip == 2)
        {
            if (!(lpInput->lpVideoPort->ddvpDesc.VideoPortType.dwFlags & DDVPCONNECT_INTERLACED))
            {
                // If it's not interlaced odd/even fields (in other words, it's progressive data),
                // we can hardware double buffer (hardware autoflip two surfaces)
                bAutoflip = TRUE;
            }
            else if (lpInput->lpVideoInfo->dwVPFlags & DDVP_SKIPEVENFIELDS)
            {
                // If we're skipping the even fields, we can hardware double buffer
                bAutoflip = TRUE;
            }
            else if ((pLPB->jFamily >= 3) &&
                     (!(lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE)))
            {
                // We can hardware double buffer interlaced, non-interleaved surfaces
                // on GX2 and later
                bAutoflip = TRUE;
            }
        }

        if (bAutoflip == FALSE)
        {
            // If we fail here, VPE will fall back to software autoflipping
            // using interrupts and the Kernel Mode Video Transports in the
            // MiniVDD

            TRACE("LPB_Update Fail, hardware autoflip not supported in this case");
            lpInput->ddRVal = DDERR_WRONGMODE; // TBD: Better error code
            return DDHAL_DRIVER_HANDLED;
        }
    }

    /*
    ** Get bytes per pixel of video input
    */
    dwBytesPerPixel = (lpInput->lpVideoInfo->lpddpfInputFormat->dwYUVBitCount / 8);

    /* Get Surface pointer */
    lpSrcLcl = lpInput->lpVideoPort->lpSurface->lpLcl;

    /* Stride of surface */
    dwStride = lpSrcLcl->lpGbl->wWidth * dwBytesPerPixel;

    pLPB->dwNumBuffers = lpInput->dwNumAutoflip;
    pLPB->dwBufferSize = (dwStride * lpSrcLcl->lpGbl->wHeight);

    /* Account for offset "placement of video data within surface" */
    pLPB->dwBaseOffset = lpInput->lpVideoInfo->dwOriginX +  (dwStride *
                           lpInput->lpVideoInfo->dwOriginY);

    if (lpInput->dwFlags == DDRAWI_VPORTSTOP)
    {
        /*
        ** Turn off the video port
        */

        pLPB->dwVDecimationPattern = 0xFFFFFFFF;
        LPB_Freeze(sData);
        LPB_Reset(sData);
        LPB_Disable(sData);
    }

    if (lpInput->dwFlags == DDRAWI_VPORTUPDATE ||
        lpInput->dwFlags == DDRAWI_VPORTSTART )
    {
        DWORD dwSrcPixelWidth, dwDstPixelWidth;
        DWORD dwSrcClockWidth;
        DWORD dwSrcHeight, dwDstHeight;
        DWORD dwTemp;
        DWORD dwHOffset, dwVOffset;
        DWORD dwLClksPerPixel;
        DWORD dwHDecimation;
        DWORD dwVDecimation;
        DWORD dwPortWidth;

        dwPortWidth = lpInput->lpVideoPort->ddvpDesc.VideoPortType.dwPortWidth;

        pLPB->dwVDecimationPattern = 0xFFFFFFFF;
        LPB_Freeze(sData);
        LPB_Enable(sData);
        LPB_Reset(sData);

        /*
        ** Set the proper bits for the all-important LPB_MODE register
        ** Any one bit set incorrectly will almost certainly cause the
        ** video to be totally wrong
        */
        dwMode  |=
                  LPB_ENABLE
                | LPB_CLOCK_LCLK;


        if (lpInput->lpVideoPort->ddvpDesc.VideoPortType.dwFlags & DDVPCONNECT_INTERLACED)
        {
          dwMode |= LPB_IGNOREHALFLINE;
        }

        if (!(lpInput->lpVideoInfo->dwVPFlags & DDVP_CONVERT))
        {
            dwMode |= LPB_BYTESWAP;     // The only kind of conversion we do
        }

        if (pLPB->jFamily > 1)
        {

            dwMode |= LPB_EARLYHSYNC_V2 | LPB_YUV422DROP_V2;

            // invert our polarity by default, since VPE defines the
            // 'normal' polarity as the Brooktree method (half-lines on
            // even fields) of communicating polarity through HSYNC/VSYNC
            // timing, and we use the Phillips method (half-lines on odd
            // fields) by default.
            if (pLPB->jFlags & LPB_FLAGS_INVERTPOLARITY)
            {
              dwMode |= LPB_INVERTFIELD_V2;
            }
        }

        switch (pLPB->jGUIDIndex)
        {

            case HREFH_VREFL:
                dwMode |= LPB_HSYNC_HIGH;
                break;

            case HREFL_VREFH:
                dwMode |= LPB_VSYNC_HIGH;
                break;

            case HREFH_VREFH:
                dwMode |= LPB_HSYNC_HIGH | LPB_VSYNC_HIGH;
                break;

            case HREFL_VREFL:
                break;

            default:
                // for the 4 HREF_VREF GUIDs above, odd/even
                // polarity is communicated through vsync/hsync
                // timing.  And we need to invert polarity by
                // default, since half-lines on even fields is
                // the normal case, and our default is half-lines
                // on odd.  BUT if we are getting the odd/even
                // polarity through another mechanism (the odd
                // pin on Aurora, or the VIP bytestream method),
                // where there are no halflines to handle,
                // we mustn't invert polarity by default.
                dwMode &= ~LPB_INVERTFIELD_V2;
                DPF("Bad GUID Index");
                break;
        }

        if (pLPB->jFamily > 1)
        {

            // give the VPE client the chance to invert the polarity
            if (lpInput->lpVideoPort->ddvpDesc.VideoPortType.dwFlags & DDVPCONNECT_INVERTPOLARITY)
            {
                if (dwMode & LPB_INVERTFIELD_V2)
                    dwMode &= ~LPB_INVERTFIELD_V2;
                else
                    dwMode | LPB_INVERTFIELD_V2;
            }
        }

        /*
        ** TBD: How to guarantee which of the fields will be dropped?
        */
        if ((lpInput->lpVideoInfo->dwVPFlags & DDVP_SKIPEVENFIELDS) ||
            (lpInput->lpVideoInfo->dwVPFlags & DDVP_SKIPODDFIELDS))
        {
            dwMode |= LPB_SKIPFIELDS;
        }

        if (dwPortWidth == 8)
        {
            dwMode |= ((LPB_TYPE_VIDEO8 << LPB_TYPE_SHIFT) & LPB_TYPE_MASK);
        }
        else
        {

            dwMode |= ((LPB_TYPE_VIDEO16 << LPB_TYPE_SHIFT) & LPB_TYPE_MASK);

        }

        if (lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE)
        {
            DPF("Interleaving fields in off-screen surface");
            dwMode |= LPB_DEINTERLACE_V2;

            if ((pLPB->jFamily > 2) && (dwPortWidth == 16))
            {
                /*
                ** enable more reliable detection of odd/even in Video 16 mode
                */
                dwMode |= LPB_V16DEINTERLACE_V3;
            }
        }

        /*
        ** Set LPB_Mode
        */
        LPB_RMW(sData, LPB_MODE, 0, dwMode);

        /* Get physical offset of surface from start of frame buffer for LPB_BASE
        **
        ** This doesn't seem safe, because the two linear pointers may be
        ** mapped differently .. but it seems that DirectDraw doesn't provide
        ** any other way to get the physical address, since they handle
        ** memory management, and pick where surfaces are located, and only
        ** seem to provide fpVidMem.  So this is the standard method used
        ** here and elsewhere in DirectDraw.
        */
        dwTemp = lpSrcLcl->lpGbl->fpVidMem - sData->dwScreenFlatAddr;

        /* Account for "placement of video data within surface" */
        dwTemp += pLPB->dwBaseOffset;

        /* DDRAW should enforce alignment. Verify anyway to be sure */
        assert(!(dwTemp & (lpInput->lpDD->lpGbl->lpDDVideoPortCaps->dwAlignVideoPortBoundary-1)));
        LPB_Set(sData, LPB_BASE0, dwTemp);

        if (lpInput->lpVideoInfo->dwVPFlags & DDVP_AUTOFLIP)
        {
            /*
            ** If hardware double buffering, set base address
            ** for second buffer
            */
            TRACE("Using hardware double buffer");
            dwTemp = lpInput->lplpDDSurface[1]->lpLcl->lpGbl->fpVidMem;
            dwTemp -= sData->dwScreenFlatAddr;
            dwTemp += pLPB->dwBaseOffset;

            /* DDRAW should enforce alignment. Verify anyway to be sure. */
            assert(!(dwTemp & (lpInput->lpDD->lpGbl->lpDDVideoPortCaps->dwAlignVideoPortBoundary-1)));
            LPB_Set(sData, LPB_BASE1, dwTemp);
        }

        /*
        ** DirectDraw terminology as it relates to our LPB and Streams Processor
        **
        **  LPB INPUT signal is "PreScale" (same as "Source" if we don't decimate)
        **  LPB OUPUT surface and SP INPUT surface are "Source"
        **  SP OUTPUT surface is "Destination"
        */

        dwHDecimation = 0;
        dwVDecimation = 0;

        /*
        ** For DDVP_CROP to pass down from DDRAW engine,
        ** the croping rectangle needs to be less than Vport width, height
        ** otherwise, the DDRAW engine will reject the Update/Start video call
        ** As usual, we don't need to do this parameter checking ourselves
        */
        dwSrcPixelWidth = (lpInput->lpVideoInfo->dwVPFlags & DDVP_CROP) ?
                          (lpInput->lpVideoInfo->rCrop.right - lpInput->lpVideoInfo->rCrop.left) :
                          lpInput->lpVideoPort->ddvpDesc.dwFieldWidth;
        dwSrcHeight = (lpInput->lpVideoInfo->dwVPFlags & DDVP_CROP) ?
                      (lpInput->lpVideoInfo->rCrop.bottom - lpInput->lpVideoInfo->rCrop.top) :
                      lpInput->lpVideoPort->ddvpDesc.dwFieldHeight;

        TRACE("Source Width = %ld", dwSrcPixelWidth);
        TRACE("Source Height = %ld", dwSrcHeight);

        if (lpInput->lpVideoInfo->dwVPFlags & DDVP_PRESCALE)
        {
            DWORD dwXStep;
            DWORD dwYStep;
            DWORD dwPixelStep, dwStep;
            DWORD dwStepIndex;

            dwDstPixelWidth = lpInput->lpVideoInfo->dwPrescaleWidth;
            dwDstHeight= lpInput->lpVideoInfo->dwPrescaleHeight;

            TRACE("dwPrescaleWidth = %ld", dwDstPixelWidth);
            TRACE("dwPrescaleHeight = %ld", dwDstHeight);

            /*
            ** Calculate H Decimation (FF2C)
            */
            dwXStep = lpInput->lpDD->lpGbl->lpDDVideoPortCaps->dwPreshrinkXStep;

            /*
            ** Verify that the prescaled width satisfies
            ** our prescaling step restrictions
            */
            assert(!(dwDstPixelWidth % (dwSrcPixelWidth / dwXStep)));
            assert(!(dwDstPixelWidth & (lpInput->lpDD->lpGbl->lpDDVideoPortCaps->dwAlignVideoPortPrescaleWidth-1)));

            // Take care rounding condition...
            dwPixelStep = dwDstPixelWidth*dwXStep;
            dwStepIndex = (dwPixelStep%dwSrcPixelWidth) ?
                          dwPixelStep/dwSrcPixelWidth:
                          dwPixelStep/dwSrcPixelWidth - 1;

            TRACE("Horizontal dwStepIndex = %ld out of %ld", dwStepIndex,dwXStep-1);
            if (dwStepIndex < 0)
            {
                dwHDecimation = 0xFFFFFFFF;
            }
            else
            {
                dwHDecimation = pLPB->HDecimationTable[dwStepIndex];
            }

            /*
            ** Calculate V Decimation (FF30)
            */
            dwYStep = lpInput->lpDD->lpGbl->lpDDVideoPortCaps->dwPreshrinkYStep;

            /*
            ** Verify that the prescaled height satisfies
            ** our prescaling step restrictions
            */
            assert(!(dwDstHeight % (dwSrcHeight / dwYStep)));

            // Take care rounding condition...
            dwStep = dwDstHeight*dwYStep;
            dwStepIndex = (dwStep%dwSrcHeight) ?
                          dwStep/dwSrcHeight:
                          dwStep/dwSrcHeight - 1;

            TRACE("Vertical dwStepIndex = %ld out of %ld", dwStepIndex, dwYStep-1);
            if (dwStepIndex < 0)
            {
                dwVDecimation = 0xFFFFFFFF;
            }
            else
            {
                dwVDecimation = pLPB->VDecimationTable[dwStepIndex];
            }
        }
        else
        {
            dwDstPixelWidth = dwSrcPixelWidth;
            dwDstHeight = dwSrcHeight;
        }

        // tuck away vertical decimation value.  It won't actually be programmed until
        // updateoverlay is called to initialize the memory timing and FIFO parameters
        pLPB->dwVDecimationPattern = dwVDecimation;
        LPB_Set(sData, LPB_HDECIMATION_PATTERN, dwHDecimation);

        // VIDEO8 == 8 bits per LClk, VIDEO16 == 16 bits per LClk
        dwLClksPerPixel = dwBytesPerPixel - (dwPortWidth / 8) + 1;

        /* Input width given to hardware must be aligned */
        dwSrcPixelWidth = RoundUp(dwSrcPixelWidth, pLPB->jInputWidthPixelAlign);

        /*
        ** LPB_INPUT_SIZE (Width) is in units of LPB clocks
        ** so if LPB is in Video8 I2C mode (1 byte per LPB clock)
        ** and BytesPerPixel is 2, we have to multiply width by 2
        */
        dwSrcClockWidth = dwSrcPixelWidth * dwLClksPerPixel;

        /* must subtract 2 clocks before programming hardware */
        dwSrcClockWidth -= 2;

        dwTemp = ((dwSrcHeight << LPB_INPUT_HEIGHT_SHIFT) & LPB_INPUT_HEIGHT_MASK);
        dwTemp |= ((dwSrcClockWidth << LPB_INPUT_WIDTH_SHIFT) & LPB_INPUT_WIDTH_MASK);

        LPB_Set(sData, LPB_INPUT_SIZE, dwTemp);

        LPB_Set(sData, LPB_OUTPUT_STRIDE, dwStride);

        /*
        ** Program INPUT_OFFSETS
        **
        ** Some data on the top and left edge of the video data is junk which
        ** should be discarded (cropped)
        **
        ** Assumptions:
        **   - We won't be told to crop VBI data, that's seperate in dwVBIHeight
        **   - lpVideoInfo->rCrop units are pixels
        */
        dwHOffset = 0;
        dwVOffset = 0;

        if (lpInput->lpVideoInfo->dwVPFlags & DDVP_CROP)
        {
            dwHOffset += (lpInput->lpVideoInfo->rCrop.left * dwLClksPerPixel);
            dwHOffset = ((dwHOffset << LPB_INPUT_HOFFSET_SHIFT) & LPB_INPUT_HOFFSET_MASK);

            dwVOffset = ((lpInput->lpVideoInfo->rCrop.top << LPB_INPUT_VOFFSET_SHIFT) & LPB_INPUT_VOFFSET_MASK);

            if ((dwPortWidth == 8) &&
                (pLPB->jGUIDIndex == HREFL_VREFL) &&
                (dwHOffset == 0x7E))
            {
                /*
                ** REMOVE AT SOME POINT WHEN SGS THOMPSON DVD CAN BE 'UNSUPPORTED'
                ** Signature of prototype (broken) SGS Thompson DVD reference
                ** We should remove this hack if it causes any problems
                ** Since there is some chance that another device would
                ** match this same signature.
                */
                TRACE("SGS Thompson DVD extra byte per line");
                dwHOffset += 1;
            }

            DPF("DDVP_CROP set HDELAY=%lx VDELAY=%lx", dwHOffset, dwVOffset);
        }

        LPB_Set(sData, LPB_INPUT_OFFSETS, dwVOffset | dwHOffset);

        LPB_UnFreeze(sData);

    }

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
 * WaitForVideoPortSync32
 *
 * Wait for the LPB Video VSYNC (note: this is entirely
 * different from the screen refresh VGA VSYNC).
 *
 * TBD: No hard polling loops.  Must use lpInput->dwTimeOut
 */
DWORD PASCAL LPB_WaitForSync(LPDDHAL_WAITFORVPORTSYNCDATA lpInput)
{
    DWORD time;
    DWORD dwResult = DDHAL_DRIVER_NOTHANDLED;
    LPS3THKINFO sData = VP_GETSDATA(lpInput);
    /*
     * Make sure the video port is ON.  If not, set
     * lpInput->ddRVal to DDERR_VIDEONOTACTIVE and return.
     */

    if ((lpInput->dwFlags == DDVPWAIT_BEGIN) ||
        (lpInput->dwFlags == DDVPWAIT_END))
    {
        TRACE("LPB_WaitForSync: DDVPWAIT_BEGIN");
        /*
         * Wait for the beginning of the video VSYNC
         */

        LPB_RMW(sData, LPB_INTERRUPT_FLAGS, 0, LPB_INT_VSYNC);         // clear the interrupt bit
        time = GetTickCount();
        while (!(LPB_Get(sData, LPB_INTERRUPT_FLAGS) | LPB_INT_VSYNC)) // wait for it to be set again
            if (GetTickCount() > (time+lpInput->dwTimeOut)) break;

        lpInput->ddRVal = DD_OK;
        dwResult = DDHAL_DRIVER_HANDLED;
    }

    if (lpInput->dwFlags == DDVPWAIT_END)
    {
        int i;

        TRACE("LPB_WaitForSync: DDVPWAIT_END");

        /*
         * Wait for the end of the video VSYNC
         * We've already waited for the beginning of VSYNC, so then
         * We'll watch each EOL come in until we're past the VBI region.
         * This isn't actually kosher, because interrupts are on ..
         * we might get preempted, and miss lines.  But this is the
         * best we can do, lacking an "IN_VSYNC" bit to read in the LPB.
         */
        for (i=0; i<(int)lpInput->lpVideoPort->ddvpInfo.dwVBIHeight;i++)
        {
            LPB_RMW(sData, LPB_INTERRUPT_FLAGS, 0, LPB_INT_HSYNC);
            while(!(LPB_Get(sData, LPB_INTERRUPT_FLAGS) | LPB_INT_HSYNC));
        }

        lpInput->ddRVal = DD_OK;
        dwResult = DDHAL_DRIVER_HANDLED;
    }

    if (dwResult == DDHAL_DRIVER_NOTHANDLED)
    {
        lpInput->ddRVal = DDERR_UNSUPPORTED;
    }

    return dwResult;
}

DWORD PASCAL LPB_GetSignalStatus(LPDDHAL_GETVPORTSIGNALDATA lpInput)
{
    int i;
    DWORD dwSample;
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    TRACE("LPB_GetSignalStatus");

    /*
    ** Poll to see if data coming into LPB is changing
    */

    dwSample = LPB_Get(sData, LPB_INPUT_FIFO);
    for (i=0; i<32000; i++)
    {
        if (LPB_Get(sData, LPB_INPUT_FIFO) != dwSample)
            break;
    }

    if (i == 64000)
    {
        lpInput->dwStatus = DDVPSQ_NOSIGNAL;
    } else {
        lpInput->dwStatus = DDVPSQ_SIGNALOK;
    }

    return DDHAL_DRIVER_HANDLED;
}


/*
 * GetVideoPortField32
 *
 * This function is only required if readback of the current
 * field is supported.
 */
DWORD PASCAL LPB_GetField(LPDDHAL_GETVPORTFIELDDATA lpInput)
{
    DWORD dwMode;
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    TRACE("LPB_GetField");

    /*
    ** The Caps should prevent this function from getting called
    ** on chips without odd/even field detection, but we'll just
    ** make sure on the debug version, since we're using V2 specific
    ** registers below.
    */
    assert(pLPB->jFamily > 1);

    /*
     * Make sure the video port is ON.  If not, set
     * lpInput->ddRVal to DDERR_VIDEONOTACTIVE and return.
     */
    dwMode = LPB_Get(sData, LPB_MODE);
    if (!(dwMode & LPB_ENABLE))
    {
        lpInput->ddRVal = DDERR_VIDEONOTACTIVE;
        return DDHAL_DRIVER_HANDLED;
    }

    /*
     * Readback whether the field is even or odd.  lpInput->bField is
     * set to TRUE if the current field is even.
     */
    lpInput->bField = dwMode & LPB_EVENFIELD_V2;
    if (dwMode & LPB_INVERTFIELD_V2) lpInput->bField = !(lpInput->bField);

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}


/*
 * GetVideoPortLine32
 *
 * This function is only required if readback of the current
 * video line number (0 relative) is supported.
 */
DWORD PASCAL LPB_GetLine(LPDDHAL_GETVPORTLINEDATA lpInput)
{
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    TRACE("LPB_GetLine");

    /*
     * (1)Make sure the video port is ON.  If not, set
     * lpInput->ddRVal to DDERR_VIDEONOTACTIVE and return.
     * (2)Readback the current line number and put it in
     * lpInput->dwLine
     */

    lpInput->dwLine = 0; // our hardware don't yet support reading back the line

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

DWORD PASCAL LPB_ColorControl(LPDDHAL_VPORTCOLORDATA lpInput)
{
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    TRACE("LPB_ColorControl");

    /*
    ** LPB has no color control functionality
    */

    return DDHAL_DRIVER_HANDLED;
}

/*
 ** SyncVideoPortData32
 *
 *  FILENAME: C:\win9x\display\mini\s3\S3_DD32.C
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *  RETURNS:
 *
 */
DWORD __stdcall SyncVideoPortData32(LPDDHAL_SYNCVIDEOPORTDATA lpInput)
{
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    DPF("SyncVideoPortData");

    lpInput->dwOriginOffset = pLPB->dwBaseOffset;

    /*
    ** !! KMVT Field dropping functionality requires a
    ** place to cache the "real" decimation pattern.  So
    ** we (S3) will reserve dwDriverReserved1 for this
    ** purpose.
    */
    lpInput->dwDriverReserved1 = pLPB->dwVDecimationPattern;

    /*
    ** Use the 2nd and 3rd driver reserved fields to
    ** tell KMVT how many buffers have been allocated, and how big they are
    */
    lpInput->dwDriverReserved2 = pLPB->dwNumBuffers;
    lpInput->dwDriverReserved3 = pLPB->dwBufferSize;

    return DDHAL_DRIVER_HANDLED;
}

/*
** I/O Functions
*/
void SR_RMW(LPS3THKINFO sData, BYTE jIndex, BYTE jClear, BYTE jSet)
{
    BYTE jSaved;
    BYTE jData;

    jSaved = ((volatile BYTE *)sData->lpMMReg)[0x83C4];
    ((volatile BYTE *)sData->lpMMReg)[0x83C4]=jIndex;
    jData = ((volatile BYTE *)sData->lpMMReg)[0x83C5];
    jData = jData & ~jClear | jSet;
    ((volatile BYTE *)sData->lpMMReg)[0x83C5]=jData;
    ((volatile BYTE *)sData->lpMMReg)[0x83C4]=jSaved;

    return;
}

void CR_RMW(LPS3THKINFO sData, BYTE jIndex, BYTE jClear, BYTE jSet)
{
    BYTE jSaved;
    BYTE jData;

    jSaved = ((volatile BYTE *)sData->lpMMReg)[0x83D4];
    ((volatile BYTE *)sData->lpMMReg)[0x83D4]=jIndex;
    jData = ((volatile BYTE *)sData->lpMMReg)[0x83D5];
    jData = jData & ~jClear | jSet;
    ((volatile BYTE *)sData->lpMMReg)[0x83D5]=jData;
    ((volatile BYTE *)sData->lpMMReg)[0x83D4]=jSaved;

    return;
}

void LPB_Enable(LPS3THKINFO sData)
{
    DWORD dwTemp;

    if (pLPB->jFamily > 1)
    {
        dwTemp = LPB_EARLYHSYNC_V2;
    }

    LPB_Set(sData, LPB_MODE, LPB_ENABLE | LPB_TYPE_PASSTHRU | dwTemp);

    return;
}

void LPB_Disable(LPS3THKINFO sData)
{
    LPB_Set(sData, LPB_MODE, LPB_TYPE_PASSTHRU);

    return;
}

void LPB_Reset(LPS3THKINFO sData)
{
    /*
    ** set and clear the reset bit ..
    */
    LPB_RMW(sData, LPB_MODE, 0, LPB_DORESET);
    LPB_RMW(sData, LPB_MODE, LPB_DORESET, 0);

    return;
}

void LPB_Freeze(LPS3THKINFO sData)
{
    /*
    ** set full decimation
    */
    LPB_Set(sData, LPB_VDECIMATION_PATTERN, 0xFFFFFFFF);

    return;
}

void LPB_UnFreeze(LPS3THKINFO sData)
{
    /*
    ** restore current decimation value
    */
    LPB_Set(sData, LPB_VDECIMATION_PATTERN, sData->LPBData.dwVDecimationPattern);

    return;
}


void LPB_Set(LPS3THKINFO sData, DWORD reg, DWORD value)
{
    DWORD dwTemp;

    ((volatile DWORD *)sData->lpMMReg)[(reg)/4]=value;
    /*
    ** issue dummy write and read back the register for 2 reasons:
    ** 1) For debug, to see if the write took
    ** 2) For retail, to break up the writes so PCI can't burst them.
    **    this is to work around Trio64V+ stepping issue A-14,
    **    which also happens on the ViRGE line.
    */
    ((volatile DWORD *)sData->lpMMReg)[0x8FFF/4]=
    ((volatile DWORD *)sData->lpMMReg)[0x8FFF/4];
    dwTemp = LPB_Get(sData, reg);

#ifdef DEBUG
    if (dwTemp != value)
    {
        DPF("LPB_Set failed trying to write %lX to %lX, result:%lX", value, reg, dwTemp);
    }
    else
    {
        TRACE("Wrote %lX to %lX", value, reg);
    }
#endif
    return;
}

DWORD LPB_Get(LPS3THKINFO sData, DWORD reg)
{
    return ((volatile DWORD *)sData->lpMMReg)[(reg)/4];
}

void LPB_RMW(LPS3THKINFO sData, DWORD reg, DWORD dwClear, DWORD dwSet)
{
    LPB_Set(sData, reg, LPB_Get(sData, reg) & ~dwClear | dwSet);
}

/*
** IndexFromGUID
**
** Get the index into our supported connection table,
** given a GUID.  returns 0 if it isn't a supported
** connection type
*/
int IndexFromGUID(GUID *pGUID)
{
    int i, index;

    index = -1;                          // assume not found

    for (i=0; i<MAX_VP_TYPES; i++)
    {
        if (memcmp(guids[i], pGUID, sizeof(GUID)) == 0)
        {
            index = i;                  // found a match
            break;
        }
    }

    TRACE("IndexFromGUID found GUID %d",i);

    return index;
}

