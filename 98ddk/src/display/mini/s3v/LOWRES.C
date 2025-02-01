
#include <windows.h>
//#include <conio.h>



#ifdef DEBUG
#define DPF		Msg
void __cdecl Msg( LPSTR szFormat, ... );
#else
#define DPF		1 ? (void)0 : (void)
#endif

/*
 * external driver state variables
 */

extern BYTE	 PASCAL DeviceId_High;
extern WORD	 PASCAL wDeviceId;
extern WORD	 PASCAL wRefreshRate;
extern WORD PASCAL wPhysResolution;	//from vga.asm

#define CRTC_VERT_RETRACE_END      0x11

#define CRTC                       0x3D4
#define SEQX                       0x3C4
#define GENMO_R                    0x3CC
#define GENMO_W                    0x3C2

#define MISCELLANOUS_OUTPUT_W      0x3c2
#define MISCELLANOUS_OUTPUT_R      0x3cc

#define ASPEN	  0x3d88    // device id for ASPEN
#define ViRGE	  0x3156    // device id for ViRGE

#define OutPortX(port,xxx,val) { \
    BYTE _off = (BYTE)(xxx);     \
    BYTE _data = (BYTE)(val);    \
        _asm mov dx, port        \
        _asm mov al, _off        \
        _asm mov ah, _data       \
        _asm out dx, ax          \
    }

#define SetPortXBits(port,xxx,bits,val) { \
    BYTE _off = (BYTE)xxx;       \
    BYTE _mask = (BYTE)~(bits);  \
    BYTE _set  = (BYTE)(val);    \
        _asm mov dx, port        \
        _asm mov al, _off        \
        _asm out dx, al          \
        _asm inc dx              \
        _asm in  al, dx          \
        _asm and al, _mask       \
        _asm or  al, _set        \
        _asm out dx, al          \
    }

#define Set2PortBits(portR,portW,bits,val) { \
    BYTE _mask = (BYTE)~(bits);  \
    BYTE _set  = (BYTE)(val);    \
        _asm mov dx, portR       \
        _asm in  al, dx          \
        _asm mov dx, portW       \
        _asm and al, _mask       \
        _asm or  al, _set        \
        _asm out dx, al          \
    }



#define DivideCRTCBy2(port)      \
{                                \
  _outp(CRTC,port);              \
  _outp(CRTC+1,_inp(CRTC+1)/2);  \
}

#define DivideCRTCBy2WithOffset(port,offset)        \
{                                                   \
  _outp(CRTC,port);                                 \
  _outp(CRTC+1,(_inp(CRTC+1) + offset)/2 - offset); \
}


// register manipulation macros
//
#define OutCRTC(reg,val)           OutPortX(CRTC,(reg),(val))
#define SetCRTCBits(reg,bits,val)  SetPortXBits(CRTC,(reg),(bits),(val))
#define ClearCRTCBits(reg,bits)    SetPortXBits(CRTC,(reg),(bits),0)
#define OutSEQ(reg,val)            OutPortX(SEQX,(reg),(val))
#define SetSEQBits(reg,bits,val)   SetPortXBits(SEQX,(reg),(bits),(val))
#define SetGENMOBits(bits,val)     Set2PortBits(GENMO_R,GENMO_W,(bits),(val))

#pragma warning(disable:4704)
#pragma warning(disable:4035)

WORD setCRT640x400x16[] =
{
  0xbf06,
  0x1F07,
  0x9c10,
  0x8e11,
  0x8F12,
  0x9615,
  0xb916,

  0xffff
};

//###
WORD setCRT320x200x8[] =
{
  0x2d00,
  0x2701,
  0x2802,
  0x0003,
  0x2a04,
  0x1005,
  0xbf06,
  0x1f07,
  0xc009,
  0x9c10,
  0x8e11,
  0x8f12,
  0x2813,
  0x9615,
  0xb916,
  0x2c3b,
  0x0067,
  0xffff
};

WORD setCRT320x200x8vx[] =
{
  0x3400,
  0x2701,
  0x2802,
  0x0003,
  0x2a04,
  0x1005,
  0xbf06,
  0x1f07,
  0xc009,
  0x9c10,
  0x8e11,
  0x8f12,
  0x2813,
  0x9615,
  0xb916,
  0x343b,
  0x0067,
  0xffff
};

WORD setCRT320x200x16vx[] =
{
  0x2d00,
  0x2701,
  0x2802,
  0x0003,
  0x2a04,
  0x1005,
  0xbf06,
  0x1f07,
  0xc009,
  0x9c10,
  0x8e11,
  0x8f12,
  0x9615,
  0xb916,
  0x303b,
  0x2067,
  0xffff
};

WORD setCRT320x240x8[] =
{
  0x2d00,
  0x2701,
  0x2802,
  0x0003,
  0x2a04,
  0x1005,
  0x0606,
  0x3e07,
  0xc009,
  0xe810,
  0x8b11,
  0xdf12,
  0x2813,
  0xdf15,
  0x0616,
  0x2c3b,
  0x0067,
  0xffff
};

WORD setCRT320x240x8vx[] =
{
  0x3100,
  0x2701,
  0x2802,
  0x0003,
  0x2a04,
  0x1005,
  0x0606,
  0x3e07,
  0xc009,
  0xe810,
  0x8b11,
  0xdf12,
  0x2813,
  0xdf15,
  0x0616,
  0x343b,
  0x0067,
  0xffff
};

WORD setCRT320x240x16[] = //setmode 10d, update vert.
{
  0x0606,
  0x3e07,
  0xe810,
  0x8b11,
  0xdf12,
  0xdf15,
  0x0616,
  0xffff
};

WORD setCRT320x240x16vx[] = //setmode 10d, update vert.
{
  0x2d00,
  0x2701,
  0x2802,
  0x0003,
  0x2a04,
  0x1005,
  0x0606,
  0x3e07,
  0xe810,
  0x8b11,
  0xdf12,
  0xdf15,
  0x0616,
  0x303b,
  0xffff
};

WORD setCRT320x240x24[] = //setmode 10f, update vert.
{
  0x0606,
  0x3e07,
  0xe810,
  0x8b11,
  0xdf12,
  0xdf15,
  0x0616,

  0xffff
};

WORD setCRT400x300x8[] =
{
  0x3d00,	// CRTC Registers
  0x3101,
  0x3202,
  0x0003,
  0x3504,
  0x0005,
  0x3e3b,
  0x005d,
  0x7206,
  0xf007,
  0x6109,
  0x5810,
  0x8c11,
  0x5712,
  0x5715,
  0x7216,
  0x3213,
  0xffff
};

WORD setCRT400x300x8vx[] =
{
  0x3f00,   // CRTC Registers
  0x3101,
  0x3202,
  0x0003,
  0x3504,
  0x0005,
  0x3e3b,
  0x005d,
  0x7206,
  0xf007,
  0x6109,
  0x5810,
  0x8c11,
  0x5712,
  0x5715,
  0x7216,
  0x3213,
  0xffff
};

WORD setCRT400x300x16[] =
{
  0x7f00,	// CRTC Registers
  0x6301,
  0x6402,
  0x0103,
  0x6a04,
  0x1a05,
  0x7b3b,
  0x005d,
  0x7206,
  0xf007,
  0x6109,
  0x5810,
  0x8c11,
  0x5712,
  0x5715,
  0x7216,
  0x6413,
  0xffff
};

WORD setCRT400x300x24[] =
{
  0x3d00,	// CRTC Registers
  0x3101,
  0x3202,
  0x0003,
  0x3504,
  0x0005,
  0x3b3b,
  0x005d,
  0x7206,
  0xf007,
  0x6109,
  0x5810,
  0x8c11,
  0x5712,
  0x5715,
  0x7216,
  0x9613,
  0x005e,
  0xffff
};

WORD setCRT400x300x16vx[] =
{
  0x3d00,	// CRTC Registers
  0x3101,
  0x3202,
  0x0003,
  0x3504,
  0x0005,
  0x3d3b,
  0x005d,
  0x7206,
  0xf007,
  0x6109,
  0x5810,
  0x8c11,
  0x5712,
  0x5715,
  0x7216,
  0x6413,
  0xffff
};

int setCRT720x480x8x60[] =
{
  0x6d00,
  0x5901,
  0x5a02,
  0x8a03,
  0x6104,
  0x8805,
  0x683b,
  0x5a13,

  0xffff
};

int setCRT720x480x8x72[] =
{
  0x6f00,
  0x5901,
  0x5a02,
  0x8703,
  0x6204,
  0x8605,   //KLW
  0x0706,
  0x693b,
  0x5a13,

  0xffff
};

int setCRT720x480x8x75[] =
{
  0x6f00,
  0x5901,
  0x5a02,
  0x8a03,
  0x6304,
  0x8605,   //KLW
  0x653b,
  0x5a13,

  0xffff
};

int setCRT720x480x8x85[] =
{
  0x7000,
  0x5901,
  0x5a02,
  0x8a03,
  0x6004,
  0x8705,   //KLW
  0x663b,
  0x5a13,

  0xffff
};

int setCRT720x480x16x60[] =
{
  0xdf00,
  0xb301,
  0xb402,
  0x8503,
  0xbb04,
  0x1005,
  0xdb3b,
  0xb413,

  0xffff
};

int setCRT720x480x16x72[] =
{
  0xe100,
  0xb301,
  0xb402,
  0xb804,
  0xdc3b,
  0xb413,

  0xffff
};

int setCRT720x480x16x75[] =
{
  0xe300,
  0xb301,
  0xb402,
  0xb804,
  0x0016,
  0xd43b,
  0xb413,

  0xffff
};

int setCRT720x480x16x85[] =
{
  0xe500,
  0xb301,
  0xb402,
  0xbb04,
  0x1205,
  0xd63b,
  0xb413,

  0xffff
};

int setCRT720x480x24x60[] =
{
  0x6d00,
  0x5901,
  0x5a02,
  0x8a03,
  0x6104,
  0x8805,
  0x683b,
  0x6813,

  0xffff
};

int setCRT720x480x24x72[] =
{
  0x6f00,
  0x5901,
  0x5a02,
  0x8703,
  0x6204,
  0x8605,   //KLW
  0x693b,
  0x6813,

  0xffff
};

int setCRT720x480x24x75[] =
{
  0x6f00,
  0x5901,
  0x5a02,
  0x8a03,
  0x6304,
  0x8605,   //KLW
  0x653b,
  0x6813,

  0xffff
};

int setCRT720x480x24x85[] =
{
  0x7000,
  0x5901,
  0x5a02,
  0x8a03,
  0x6004,
  0x8705,   //KLW
  0x663b,
  0x6813,

  0xffff
};

BOOL FAR PASCAL ProgramCRTCLowRes (
    WORD wXRes,
    WORD wYRes,
    WORD wPixDepth)
{
    int ii;
    LPWORD	lpwPatch = NULL;
    DWORD dwclock;
    DPF("Programming Low Resolution Mode %d: %dx%d, %d bpp",wPhysResolution,wXRes,wYRes,wPixDepth);
    dwclock=0;
    if ( wDeviceId==ASPEN ) // For VirgeVX only use bios for 320x200x24 to fix STB bios problems.
    {
        if ( (wXRes == 320) && (wYRes == 200) && (wPixDepth == 24) )
            return (1);
    }
    else if ( (wXRes == 320) && (wYRes == 200) && (wPixDepth != 8) )
        //320x200x16 & 320x200x24 are handled by the video bios.
		return (1);

    ClearCRTCBits (CRTC_VERT_RETRACE_END, 0x80);  // unlock

    if ( (wXRes == 320) || (wXRes == 400) || (wXRes == 640) )
    {
    switch ( wXRes )
	{
        case 320:
            switch ( wPixDepth )
            {
                case 8:                     //320x200x8 & 320x240x8
                    if (wYRes==200)
                    {
                        lpwPatch = setCRT320x200x8;
                        if ( wDeviceId==ASPEN )
                        {
                            lpwPatch = setCRT320x200x8vx;
                            dwclock=0x032c80;
                        }
                    }
                    else
                    {
                        lpwPatch = setCRT320x240x8;
                        if ( wDeviceId==ASPEN )
                        {
                            lpwPatch = setCRT320x240x8vx;
                            dwclock=0x033080;
                        }
                    }
                    break;

                case 16:                    //320x240x16
                    if ( wYRes==200 )
                    {
                        if ( wDeviceId==ASPEN )
                            lpwPatch = setCRT320x200x16vx;
                    }
                    else
                    {
                        lpwPatch =  setCRT320x240x16;
                        if ( wDeviceId==ASPEN )
                            lpwPatch = setCRT320x240x16vx;
                    }
                    break;

                case 24:                    //320x240x24
                    lpwPatch = setCRT320x240x24;
                    break;
            } // switch (wPixDepth) 320x
            break;

        case 400:
            switch( wPixDepth )
            {
                case 8:
                    lpwPatch = setCRT400x300x8;
                    dwclock  = 0x00134144;
                    if ( wDeviceId == ASPEN )
                    {
                        lpwPatch = setCRT400x300x8vx;
                        dwclock = 0x00032c80;
                    }
                    break;
                case 16:
                    lpwPatch = setCRT400x300x16;
                    dwclock  = 0x00134144;
                    if ( wDeviceId==ASPEN )
                    {
                        lpwPatch = setCRT400x300x16vx;
                        dwclock = 0x00032b80;
                    }
                    break;
                case 24:
                    lpwPatch = setCRT400x300x24;
                    dwclock  = 0x00134144;
                    if ( wDeviceId == ASPEN )
                        dwclock = 0x00032b80;
                    break;
            } // switch (wPixDepth) 400x300
            break;

        case 640:
            if ( wPixDepth !=8 )
            {
                lpwPatch = setCRT640x400x16;
                if ( wDeviceId != ASPEN )
                    dwclock  = 0x00032861;
            }
            else
                return (1); //640x400x8 is handled by the video bios.
            break;
    } // switch (wXRes)

    for (ii = 0; lpwPatch[ii] != 0xFFFF; ++ii)
        _outpw(CRTC, lpwPatch[ii]);

    if ( dwclock )
    {
        _outpw(SEQX,(WORD)((dwclock & 0xff)<<8)|0x12);
        _outpw(SEQX,(WORD)(dwclock & 0xff00)|0x13);
        _outpw(SEQX,(WORD)((dwclock & 0xff0000)>>8)|0x15);
    }
    } // if (wXRes)

    if ( (wXRes == 720 ) && (wYRes == 480 ) )
      {

	if (wRefreshRate == 60 )
	  {
	     OutSEQ(0x12,0x62);
	     OutSEQ(0x13,0x3e);
	     if (wPixDepth == 8)
	      lpwPatch = setCRT720x480x8x60;
	     else
	      if (wPixDepth == 16)
		lpwPatch = setCRT720x480x16x60;
	       else
		  lpwPatch = setCRT720x480x24x60;
	  }
	else
	  if (wRefreshRate == 72 )
	    {
	      OutSEQ(0x12,0x46);
	      OutSEQ(0x13,0x4c);

	      if (wPixDepth == 8)
	       lpwPatch = setCRT720x480x8x72;
	      else
	       if (wPixDepth == 16)
		 lpwPatch = setCRT720x480x16x72;
		 else
		    lpwPatch = setCRT720x480x24x72;
	    }
	  else
	    if ( wRefreshRate == 85 )
	      {
		OutSEQ(0x12,0x48);
		OutSEQ(0x13,0x6e);
		if (wPixDepth == 8)
		 lpwPatch = setCRT720x480x8x85;
		else
		 if (wPixDepth == 16)
		   lpwPatch = setCRT720x480x16x85;
		   else
		      lpwPatch = setCRT720x480x24x85;

	      }
	    else
	      {
		OutSEQ(0x12,0x46);
		OutSEQ(0x13,0x4c);
		if (wPixDepth == 8)
		 lpwPatch = setCRT720x480x8x75;
		else
		 if (wPixDepth == 16)
		   lpwPatch = setCRT720x480x16x75;
		   else
		      lpwPatch = setCRT720x480x24x75;
	      }
      for (ii = 0; lpwPatch[ii] != 0xFFFF; ++ii)
	_outpw(CRTC, lpwPatch[ii]);
      }

   SetCRTCBits (CRTC_VERT_RETRACE_END,0, 0x80);  // unlock
}

