#ifndef _WIN32
#define NOUSER
#define NOGDI
#define NOGDIOBJ
#define NOGDICAPMASKS
#include <windows.h>

#define NOPTRC
#define PTTYPE POINT
#include <gdidefs.inc>
#include <dibeng.inc>
#include <minivdd.h>
#endif // _WIN32
#include "s3data.h"            // for chip IDs

UINT  CreateSel(DWORD base, DWORD limit);
void  FreeSel(UINT Sel);
extern DWORD dwDeviceHandle;
/***************************************************************************
 *
 * DEBUG stuff
 *
 ***************************************************************************/

#ifdef DEBUG
extern void FAR __cdecl DPF(LPSTR szFormat, ...);
#define BREAK() DebugBreak();
#else
#define DPF         1 ? (void)0 : (void)
#define BREAK()
#define BREAK()
#endif
//***************************************************************************

   typedef struct {
    short int     XRes;     // x res, ie 1024 for 1024x768 etc
    unsigned char rate;     // the refresh rate, ie 60Hz
    unsigned char index;    // the CRTC register to change
    unsigned char mask;     // the bits of the register to change
    unsigned char value;    // the value of the register
}   RefreshRateEntry;

#define BCODE   __based(__segname("_CODE"))

BOOL PASCAL FindStringInROM(DWORD dwBase, UINT length, char BCODE *string);

//
// these values were tested on a ViregeVX(velocity 3d from STB).
//
RefreshRateEntry BCODE Series4Refresh[] = {
    {640,  60, 0x52, 0x70, 0x00},
    {640,  72, 0x52, 0x70, 0x10},
    {640,  75, 0x52, 0x70, 0x20},
    {640,  85, 0x52, 0x70, 0x30},

    {800,  56, 0x52, 0x80, 0x00}, {800,  56, 0x41, 0x03, 0x00},
    {800,  60, 0x52, 0x80, 0x80}, {800,  60, 0x41, 0x03, 0x00},
    {800,  72, 0x52, 0x80, 0x00}, {800,  72, 0x41, 0x03, 0x01},
    {800,  75, 0x52, 0x80, 0x80}, {800,  75, 0x41, 0x03, 0x01},
    {800,  85, 0x52, 0x80, 0x00}, {800,  85, 0x41, 0x03, 0x02},

    {1024, 43, 0x41, 0x1C, 0x00},
    {1024, 60, 0x41, 0x1C, 0x04},
    {1024, 70, 0x41, 0x1C, 0x08},
    {1024, 75, 0x41, 0x1C, 0x0c},
    {1024, 85, 0x41, 0x1C, 0x10},

    {1280, 45, 0x41, 0xE0, 0x00}, 
    {1280, 60, 0x41, 0xE0, 0x20}, 
    {1280, 75, 0x41, 0xE0, 0x60}, 
    {1280, 85, 0x41, 0xE0, 0x80}, 

    {0, 0, 0, 0, 0}
};

DWORD VDDCall(DWORD DeviceHandle, DWORD function, DWORD flags, LPVOID buffer, DWORD buffer_size);

//------------------------------------------------------------------------------------------
BYTE SavedRefreshIndex[2];
BYTE SavedRefreshIndexValue[2];
BYTE SavedRefreshIndexMask[2];
int SavedIndexCount= 0;
//------------------------------------------------------------------------------------------
void far pascal RestoreOldRefresh(int ChipID)
{
   int i;

   if (SavedIndexCount == 0)
      return;
   outp(0x03D4, 0x39);         // unlock the S3 registers
   outp(0x03D5, 0xA5);

   for (i = 0; i < SavedIndexCount; i++)
   {
      outp(0x3D4, SavedRefreshIndex[i]);
      outp(0x3D5, (inp(0x3D5) & ~SavedRefreshIndexMask[i]) | SavedRefreshIndexValue[i]);
   }
}
//------------------------------------------------------------------------------------------
void far pascal SetRefresh(int ChipID)
{
    DISPLAYINFO DisplayInfo;
    int XRes, YRes;
    int RefreshRateMax, RefreshRateMin;

    RefreshRateEntry BCODE *p;
    RefreshRateEntry BCODE *x;

    VDDCall(dwDeviceHandle, VDD_GET_DISPLAY_CONFIG, 0,
        &DisplayInfo, sizeof(DisplayInfo));

    XRes = DisplayInfo.diXRes;
    YRes = DisplayInfo.diYRes;
    RefreshRateMax = DisplayInfo.diRefreshRateMax;
    RefreshRateMin = DisplayInfo.diRefreshRateMin;

    DPF("RefreshRateMax: %d",RefreshRateMax);
    DPF("RefreshRateMin: %d",RefreshRateMin);
    DPF("XRes: %d",XRes);
    DPF("YRes: %d",YRes);

    SavedIndexCount = 0;                     // flag says no refresh saved

    if (RefreshRateMax == 0 || XRes == 0)
        return;

    if (XRes == 1152)
        XRes = 1024;

    p = Series4Refresh;

    //
    // scan the table and find the entry to use
    //
    for (x=NULL ;p->XRes != 0; p++)
    {
        if (p->XRes == XRes &&
            p->rate <= RefreshRateMax &&
            p->rate >= RefreshRateMin)
        {
            if (x == NULL || x->rate < p->rate)
                x = p;
        }
    }

    if (x)
    {
        int Save3D4, rate;
        Save3D4 = inp(0x3D4);       // save this index value

        DPF("Setting refresh rate %dHz", (int)x->rate);

        outp(0x03D4, 0x39);         // unlock the S3 registers
        outp(0x03D5, 0xA5);

        //
        // there may be multiple table entries with the same rate
        // ie 800x600
        //
        for (rate=x->rate,SavedIndexCount = 0; x->rate==rate; x++,SavedIndexCount++)
        {
            outp(0x3D4, x->index);

            SavedRefreshIndex[SavedIndexCount] = x->index;
            SavedRefreshIndexValue[SavedIndexCount] = inp(0x3D5);
            SavedRefreshIndexMask[SavedIndexCount] = x->mask;
            DPF("\tSetting refresh rate %dHz: index:%x, value:%x", (int)x->rate,x->index,(inp(0x3D5) & ~x->mask) | x->value);
            outp(0x3D5, (inp(0x3D5) & ~x->mask) | x->value);
        }

        outp(0x03D4, Save3D4);      // restore index
    }
}

BOOL PASCAL FindStringInROM(DWORD dwBase, UINT length, char BCODE *string)
{
    LPSTR   lpROM;
    char    c0, c1;
    int     i;
    BOOL    found;

    c0 = string[0];
    c1 = string[1];

    lpROM = MAKELP(CreateSel(dwBase, length-1), 0);

    for (found=FALSE; length > 0 && !found; lpROM++,length--)
    {
        if (lpROM[0] == c0 && lpROM[1] == c1)
        {
            for (i=2;string[i] && string[i]==lpROM[i]; i++)
                ;

            if (string[i]==0)
                found = TRUE;
        }
    }

    FreeSel(SELECTOROF(lpROM));
    return found;
}

/***************************************************************************
 * CreateSel
 * FreeSel
 ***************************************************************************/
DWORD PhysToLinear(DWORD PhysAddress, DWORD Size);

#pragma optimize("", off)

UINT CreateSel(DWORD base, DWORD limit)
{
    UINT Sel;

    Sel = AllocSelector(SELECTOROF((LPVOID)&dwDeviceHandle));

    if (Sel == 0)
        return 0;

    SetSelectorBase(Sel, base);

    // SetSelectorLimit(FlatSel, -1);
    _asm    mov     ax,0008h            ; DPMI set limit
    _asm    mov     bx,Sel
    _asm    mov     dx,word ptr limit[0]
    _asm    mov     cx,word ptr limit[2]
    _asm    int     31h

    return Sel;
}
#pragma optimize("", on)

void FreeSel(UINT Sel)
{
    if (Sel)
    {
        SetSelectorLimit(Sel, 0);
        FreeSelector(Sel);
    }
}

UINT FAR PASCAL _loadds CreatePhysSel(DWORD PhysAddress, DWORD PhysSize)
{
    return CreateSel(PhysToLinear(PhysAddress, PhysSize), PhysSize-1);
}

/***************************************************************************
 * PhysToLinear
 ***************************************************************************/
#pragma optimize("", off)
DWORD PhysToLinear(DWORD PhysAddress, DWORD PhysSize)
{
    DWORD LinearAddress;

    PhysSize = PhysSize-1;      // we want limit, not size for DPMI

    _asm
    {
        mov     cx, word ptr PhysAddress[0]
        mov     bx, word ptr PhysAddress[2]
        mov     di, word ptr PhysSize[0]
        mov     si, word ptr PhysSize[2]
        mov     ax, 0800h               ; DPMI phys to linear
        int     31h
        mov     word ptr LinearAddress[0], cx
        mov     word ptr LinearAddress[2], bx
    }

    return LinearAddress;
}
#pragma optimize("", on)


