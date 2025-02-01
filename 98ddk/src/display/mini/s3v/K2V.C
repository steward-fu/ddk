/*
 * (C) Copyright 1995, by S3, Incorporated - All rights reserved
 *
 */

//#BC1  01/25/96        New algorithm to calculate refresh rate

#include <windows.h>
#include <stdlib.h>
#include "s3_ddtk.h"
#include "s3data.h"

BAND band;
PBAND pBand = &band;
char section[] = "display";
char inifile[] = "system.ini";


// D E F I N E S  +  S T R U C T U R E S ------------------------------------

// C O N S T A N T S --------------------------------------------------------

LPCSTR szDevice[] = //key string to specify .ini file
{
        "HWCODEC",
        "SWCODEC",
        "DIGITIZER"
};

LPCSTR szDeviceIni[] = //default .ini file
{
        "xxxx_HWC.INI",
        "xxxx_SWC.INI",
        "xxxx_DIG.INI"
};

#define FIFO_ARRAY_SIZE  6
#define NUMENTRIES(x)  (sizeof(x)/sizeof(x[0]))
#define NumModeKey     NUMENTRIES(ModeKey)
#define NumRefreshKey  NUMENTRIES(RefreshKey)
#define NumMemTypeKey  NUMENTRIES(MemTypeKey)
#define NumMemSizeKey  NUMENTRIES(MemSizeKey)
#define WITHIN_RANGE(x, lo, hi) (((x)>=(lo))&&((x)<=(hi)))
#define NOT_WITHIN_RANGE(x, lo, hi) (!WITHIN_RANGE(x, lo, hi))

LPCSTR ModeKey[] =
{
        "1024x768x24", //bMode == 0
        "1024x768x16", //bMode == 1
        "1024x768x8" , //bMode == 2
        "800x600x24" , //bMode == 3
        "800x600x16" , //bMode == 4
        "800x600x8"  , //bMode == 5
        "640x480x24" , //bMode == 6
        "640x480x16" , //bMode == 7
        "640x480x8"  , //bMode == 8
        "640x480x16" , //bMode == 9 (use 640x480x16 .ini settings for 640x400 mode)
        "640x480x8"  , //bMode == 10 (use 640x480x8 .ini settings for 640x400 mode)
        "512x384x16" , //bMode == 11
        "512x384x8"  ,  //bMode == 12
    "320x200x16" , //bMode == 13
    "320x200x8"  ,  //bMode == 14
        "1152x864x8" , //bMode == 17
        "1280x1024x8", //bMode == 18
    "1280x1024x16", //bMode == 19
        "1600x1200x8", //bMode == 20
};
LPCSTR RefreshKey[] = { "43", "60", "70", "72", "75", "85"};
LPCSTR MemTypeKey[] = { "1edo", "X", "2edo", "fast" };
LPCSTR MemSizeKey[] = { "1" , "2", "4", "6", "8"};
int MemSpeedKey[] = { 40, 50, 60 };

// G L O B A L S ------------------------------------------------------------

char szIniFile[MAX_STRING_SIZE];
char szSection[MAX_STRING_SIZE];
char szProfile[MAX_STRING_SIZE];
char szString[MAX_STRING_SIZE];
char szTmp[MAX_STRING_SIZE];

K2VTABLE k2vTable[MAX_DEVICE] =
{
        { 200, 0,  200, 0x0L,
          400, 0,  400, 0x0L,
          600, 0,  600, 0x0L,
          800, 0,  800, 0x0L,
         1024, 0, 1024, 0x0L  },

        { 200, 0,  200, 0x0L,
          400, 0,  400, 0x0L,
          600, 0,  600, 0x0L,
          800, 0,  800, 0x0L,
         1024, 0, 1024, 0x0L  },

        { 200, 0,  200, 0x0L,
          400, 0,  400, 0x0L,
          600, 0,  600, 0x0L,
          800, 0,  800, 0x0L,
         1024, 0, 1024, 0x0L  },
};

BYTE fam;
int id;


// E X T E R N S ------------------------------------------------------------

extern WORD     PASCAL wScreenWidth;
extern WORD     PASCAL wScreenHeight;
extern WORD     PASCAL wBpp;
extern BYTE PASCAL DeviceId_High;
extern WORD PASCAL wDeviceId;
extern BYTE PASCAL bFamilyFlags;
extern BYTE     PASCAL  bPanelType;     // vga.asm
extern S3MOBILEDATA PASCAL S3MobileData;

/*---------------------------------------------------------------------------
 */
int  GetOptionInt(char *key, int deflt)
{
   return(GetPrivateProfileInt(section, key, deflt, inifile));
}

/*
** GetOptionFlag()
**
** if KEY="on" or "yes", then set bit FLAG of PFLAGS to true
** if KEY="no" or "off", then set bit FLAG of PFLAGS to false
**    else leave bit FLAG or PFLAGS alone
**
** inputs: key, pflags, flag
** outputs: pflags
*/
void GetOptionFlag(char* key, int *pflags, int flag)
{
   char string[8];

   if(GetPrivateProfileString(section, key, "", string, 8, inifile))
   {
      if ((string[1] == 'E') || (string[1] == 'N') ||
         (string[1] == 'e') || (string[1] == 'n'))
      {
         /* YES or ON*/
         *pflags |= flag;
      }
      else
      {
         /* any other value */
         *pflags &= ~flag;
      }
   }

   return;
}

#define K2V_WriteCrReg CR_Write
#define K2V_ReadCrReg CR_Read
#define K2V_WriteSrReg SR_Write
#define K2V_ReadSrReg SR_Read

/*---------------------------------------------------------------------------
 *
 * GetMode
 *
 */
static int GetMode( int screenHeight, int pixelDepth)
{
        switch( screenHeight)
        {
                case 1200:
                    if ( S3MobileData.wSPStyle & SP_M5 )
                    {
                        switch( pixelDepth)
                        {
                            case 8:     return( 20);     break;
                                default:        return( K2V_ERR);
                        }
                    }
                    else
                        return( K2V_ERR);
                case 1024:
            if (wDeviceId==D_S3VIRGE )
                return( K2V_ERR);
                        switch( pixelDepth)
                        {
                case 16:    return( 19);     break;
                case 8:     return( 18);     break;
                                default:        return( K2V_ERR);
                        }
                case 864:
                        switch( pixelDepth)
                        {
                case 8:     return (17);    break;
                                default:        return( K2V_ERR);
                        }
                case 768:
                        switch( pixelDepth)
                        {
                                case 24:        return( 0);     break;
                                case 16:        return( 1);     break;
                case 8:     return( 2); break;
                                default:        return( K2V_ERR);
                        }
                case 600:
                        switch( pixelDepth)
                        {
                                case 24:        return( 3);     break;
                                case 16:        return( 4);     break;
                case 8:     return( 5); break;
                                default:        return( K2V_ERR);
                        }
                case 480:
                        switch( pixelDepth)
                        {
                                case 24:        return( 6);     break;
                                case 16:        return( 7);     break;
                case 8:     return( 8); break;
                                default:        return( K2V_ERR);
                        }
                case 400:
                        switch( pixelDepth)
                        {
                case 24:    return(6);  break;
                                case 16:        return(9);      break;
                case 8:     return(10); break;
                                default:        return( K2V_ERR);
                        }
                case 384:
        case 300:
                        switch( pixelDepth)
                        {
                case 24:    return(6);  break;
                                case 16:        return(11);     break;
                case 8:     return(12); break;
                                default:        return( K2V_ERR);
                        }
                case 240:
                case 200:
                        switch( pixelDepth)
                        {
                case 24:    return(13); break;
                case 16:    return(13); break;
                case 8:     return(14); break;
                                default:        return( K2V_ERR);
                        }
        }
        return( K2V_ERR);
}

/*
** GetDclksDuringBlank()
**
** TrioV2 and future products fetch Primary and Secondary stream data into
** the FIFOs during the blanking period. In order to know how much data
** can be fetched during blanking, we need to know the length of the
** blanking period. This function calculates that length from the CR registers
*/
int GetDclksDuringBlank(void)
{
   int cclkBlankStart;
   int cclkBlankEnd;
   int cclksDuringBlank;
   int cclkDelay;
   BYTE cr5d;

   if ( S3MobileData.wSPStyle & SP_M5 )
   {
       if (S3MobileData.dwMobileCaps&SP_IGA1)
        //Make sure that data of paired register read come from IGA1
           K2V_WriteSrReg(0x26,0x40);
       else
        //Make sure that data of paired register read come from IGA2
         K2V_WriteSrReg(0x26,0x4F);
   }

   cr5d = K2V_ReadCrReg(0x5d);

   cclkBlankStart = K2V_ReadCrReg(0x02);
   cclkBlankStart += (cr5d & 0x04) << 6;
   cclkBlankStart &= 0x7F;

   cclkBlankEnd = K2V_ReadCrReg(0x03) & 0x1F;
   cclkBlankEnd += (K2V_ReadCrReg(0x05) & 0x80) >> 1;
   cclkBlankEnd += (cr5d & 0x08) << 3;

   if (cclkBlankEnd < cclkBlankStart)
   {
      cclksDuringBlank = (512 - cclkBlankStart) + cclkBlankEnd;
   }
   else
   {
      cclksDuringBlank = (cclkBlankEnd - cclkBlankStart);
   }

   cclkDelay = K2V_ReadCrReg(0x85) & 0x07;

#define CCLKS_BLANKFIX  6
   cclkDelay += CCLKS_BLANKFIX;

   cclksDuringBlank -= cclkDelay;

   if ( S3MobileData.wSPStyle & SP_M5 )
   {
       //Make sure that data of paired register read come from IGA1
       K2V_WriteSrReg(0x26,0x40);
   }

   return(cclksDuringBlank*8);

}

/*
** GetDCLK()
**
** Calculate the pixel (dot) clock from the SR registers.
*/
int GetDCLK(void)
{
   int m, n, r;
   long dclock;

   if ( (S3MobileData.dwMobileCaps & MOBILE_FAMILY) ||
        ( S3MobileData.wSPStyle & SP_M5 ) )
   {
        n = K2V_ReadSrReg(0x12);        //Use DClk PLL
        m = K2V_ReadSrReg(0x13);

        r = (n >> 6) & 0x03;
        r |= ((K2V_ReadSrReg(0x29)&0x1)<<2); //overflow bit

        if(r==0)
            r=1;
        else
            r = 2<<(r-1);

        n = n & 0x3f;
        if ( bFamilyFlags & MOBILE_FAMILY )
            dclock = ((long)(m+2)*27000L)/((long)((n+2)*r)*100L);
        else
            dclock = ((long)(m+2)*14318L)/((long)((n+2)*r)*100L);

   }
   else
   {

            n = K2V_ReadSrReg(0x12);
            m = K2V_ReadSrReg(0x13) & 0x7f;

        if (wDeviceId==D_S3VIRGE)
                r = (n >> 5) & 0x03;
                else
                r = (n >> 5) & 0x07;

            if(r==0)
               r=1;
            else
               r = 2<<(r-1);

            n = n & 0x1f;

           dclock = ((long)(m+2)*14318L)/((long)((n+2)*r)*100L);
   }

   dclock = dclock / 10L;
   if ((dclock % 10) >= 7)
      dclock++;

   return((int)dclock);


}


/*
** GetDCLK2()
**
** Calculate the pixel (dot) clock from the SR registers for IGA2.
*/
int __loadds FAR PASCAL GetDCLK2(void)
{
   int m, n, r;
   long dclock;

   n = K2V_ReadSrReg(0x0e);        //Use DClk PLL
   m = K2V_ReadSrReg(0x0f);

   r = (n >> 6) & 0x03;
   r |= (K2V_ReadSrReg(0x29)&0x4); //overflow bit

   if(r==0)
       r=1;
   else
       r = 2<<(r-1);

   n = n & 0x3f;

   if ( bFamilyFlags & MOBILE_FAMILY )
       dclock = ((long)(m+2)*27000L)/((long)((n+2)*r)*100L);
   else
       dclock = ((long)(m+2)*14318L)/((long)((n+2)*r)*100L);

   dclock = dclock / 10L;

   return((int)dclock);
}

/*---------------------------------------------------------------------------
 *
 * GetRefresh
 *
 */
#define REFRESH_CNT         5
#define K2V_MIN_REFRESH 44
#define K2V_MAX_REFRESH 85

static int refreshXlate_480[] = { 1, 2, 3, 4, 4, 4 };
static int refreshXlate_600[] = { 1, 1, 2, 3, 4, 4 };
static int refreshXlate_768[] = { 0, 0, 1, 2, 3, 4 };

static int GetRefresh( int screenHeight)
{
        int     refresh;
        int     refreshIni;

        refresh = 0;

        //Check user override
        refreshIni = GetPrivateProfileInt( "display", "refresh", 0, "system.ini");

        if( refreshIni)
        {
                if ((refreshIni<K2V_MIN_REFRESH) ||
                        (refreshIni>K2V_MAX_REFRESH))
                        return(K2V_ERR);

                if( refreshIni > 44) refresh++;
                if( refreshIni > 60) refresh++;
                if( refreshIni > 72) refresh++;
                if( refreshIni > 75) refresh++;
                if( refreshIni > 85) refresh++;
    }
        else
        {
      // New method to calculate refresh rate
      int m, n, r;
      long hTotal, vTotal;
      long dclk, refreshRate;

      dclk = GetDCLK();

      if (!(S3MobileData.dwMobileCaps & MOBILE_FAMILY ))
         {
          n = K2V_ReadCrReg(0x0);
          m = K2V_ReadCrReg(0x5D) & 0x01;

          hTotal = (long)((n|(m << 8)) + 5);

          // check for 15/16bpp mode
          n = K2V_ReadCrReg(0x67) & 0xf0;
          if ((n == 0x50) || (n == 0x30))
              hTotal = (hTotal << 2);
          else
              hTotal = (hTotal << 3);

          n = K2V_ReadCrReg(0x06);
          m = (K2V_ReadCrReg(0x07) & 0x01) | ((K2V_ReadCrReg(0x07) & 0x20) >> 4);
          r = ((K2V_ReadCrReg(0x5E) & 0x01) << 2);

          vTotal = (long)((n | (m << 8) | (r << 8)) + 2);
      } else {

          //If FP active and on IGA1: use FP params to derive refresh else use CRT
          if ( (K2V_ReadSrReg(0x31) & 0x12) == 0x10 )
          {
              n = K2V_ReadSrReg(0x60);         //Read FP HTotal reg
              m = (K2V_ReadSrReg(0x66) & 0x01);

              hTotal = (long)((n|(m << 8)) + 5);
              hTotal = (hTotal << 3);

              n = K2V_ReadSrReg(0x68);         //Read FP VTotal reg
              m = (K2V_ReadSrReg(0x6e) & 0x07);

              vTotal = (long)((n | (m << 8)) + 2);
          }
          else
          {
              //Make sure that data of paired register read come from IGA1
              K2V_WriteSrReg(0x26,0x40);
              n = K2V_ReadCrReg(0x0);          //Read CRT HTotal reg
              m = K2V_ReadCrReg(0x5D) & 0x01;

              hTotal = (long)((n|(m << 8)) + 5);

              // check for 15/16bpp mode
              n = K2V_ReadCrReg(0x67) & 0xf0;
              hTotal = (hTotal << 3);

              n = K2V_ReadCrReg(0x06);         //Read CRT VTotal reg
              m = (K2V_ReadCrReg(0x07) & 0x01) | ((K2V_ReadCrReg(0x07) & 0x20) >> 4);
              r = ((K2V_ReadCrReg(0x5E) & 0x01) << 2);

              vTotal = (long)((n | (m << 8) | (r << 8)) + 2);
          }
      }

      refreshRate = (long)(dclk*10000000L)/(long)(hTotal*vTotal);

      refreshIni = (int)refreshRate/10;
      if ((refreshRate % 10) >= 5)
          refreshIni++;

                //check for interlace mode
      if (K2V_ReadCrReg(0x42) & 0x20)
          refreshIni /= 2;

      if( refreshIni > 44) refresh++;
      if( refreshIni > 60) refresh++;
      if( refreshIni > 70) refresh++;
      if( refreshIni > 72) refresh++;
      if( refreshIni > 75) refresh++;
      if( refreshIni > 85) refresh = 3; //default to 70 Hz
        }
        return( refresh);
}

static int GetRefreshRate( int screenHeight)
{
        int     refreshIni;

        //Check user override
        refreshIni = GetPrivateProfileInt( "display", "refresh", 0, "system.ini");

        if( refreshIni)
        {
                if ((refreshIni<K2V_MIN_REFRESH) ||
                        (refreshIni>K2V_MAX_REFRESH))
                        return(K2V_ERR);

    }
        else
        {
      // Calculate refresh rate:  Dclk/(HorzTotal * VertTotal)
      int m, n, r;
      long hTotal, vTotal;
      long dclk, refreshRate;

      dclk = GetDCLK();

      if (!(S3MobileData.dwMobileCaps & MOBILE_FAMILY ))
                   {
          n = K2V_ReadCrReg(0x0);
          m = K2V_ReadCrReg(0x5D) & 0x01;

          hTotal = (long)((n|(m << 8)) + 5);

          // check for 15/16bpp mode
          n = K2V_ReadCrReg(0x67) & 0xf0;
          if ((n == 0x50) || (n == 0x30))
              hTotal = (hTotal << 2);
          else
              hTotal = (hTotal << 3);

          n = K2V_ReadCrReg(0x06);
          m = (K2V_ReadCrReg(0x07) & 0x01) | ((K2V_ReadCrReg(0x07) & 0x20) >> 4);
          r = ((K2V_ReadCrReg(0x5E) & 0x01) << 2);

          vTotal = (long)((n | (m << 8) | (r << 8)) + 2);
      } else {

          //If FP active and on IGA1: use FP params to derive refresh else use CRT
          if ( (K2V_ReadSrReg(0x31) & 0x12) == 0x10 )
          {
              n = K2V_ReadSrReg(0x60);         //Read FP HTotal reg
              m = (K2V_ReadSrReg(0x66) & 0x01);

              hTotal = (long)((n|(m << 8)) + 5);
              hTotal = (hTotal << 3);

              n = K2V_ReadSrReg(0x68);         //Read FP VTotal reg
              m = (K2V_ReadSrReg(0x6e) & 0x07);

              vTotal = (long)((n | (m << 8)) + 2);
          }
          else
          {
              //Make sure that data of paired register read come from IGA1
              K2V_WriteSrReg(0x26,0x40);

              n = K2V_ReadCrReg(0x0);          //Read CRT HTotal reg
              m = K2V_ReadCrReg(0x5D) & 0x01;

              hTotal = (long)((n|(m << 8)) + 5);

              // check for 15/16bpp mode
              n = K2V_ReadCrReg(0x67) & 0xf0;
              hTotal = (hTotal << 3);

              n = K2V_ReadCrReg(0x06);         //Read CRT VTotal reg
              m = (K2V_ReadCrReg(0x07) & 0x01) | ((K2V_ReadCrReg(0x07) & 0x20) >> 4);
              r = ((K2V_ReadCrReg(0x5E) & 0x01) << 2);

              vTotal = (long)((n | (m << 8) | (r << 8)) + 2);
          }
      }

      refreshRate = (long)(dclk*10000000L)/(long)(hTotal*vTotal);

      refreshIni = (int)refreshRate/10;
      if ((refreshRate % 10) >= 5)
          refreshIni++;

                //check for interlace mode
      if (K2V_ReadCrReg(0x42) & 0x20)
          refreshIni /= 2;

        }
        return( refreshIni);
}

/*Calculate the refresh rate for Mobile IGA2*/
static int GetRefreshRate2( )
{
    int     refreshIni;

    //Check user override
    refreshIni = GetPrivateProfileInt( "display", "refresh2", 0, "system.ini");

    if( refreshIni)
    {
        if ((refreshIni<K2V_MIN_REFRESH) ||
            (refreshIni>K2V_MAX_REFRESH))
            return(K2V_ERR);

    }
    else
    {
         // Calculate refresh rate:  Dclk/(HorzTotal * VertTotal)
        int m, n, r;
        long hTotal, vTotal;
        long dclk, refreshRate;

        dclk = GetDCLK2();
        //Make sure that data of paired register read come from IGA2
        K2V_WriteSrReg(0x26, 0x4F);

        n = K2V_ReadCrReg(0x0);          //Read CRT HTotal reg
        m = K2V_ReadCrReg(0x5D) & 0x01;

        hTotal = (long)((n|(m << 8)) + 5);

        // check for 15/16bpp mode
        n = K2V_ReadCrReg(0x67) & 0xf0;
        hTotal = (hTotal << 3);

        n = K2V_ReadCrReg(0x06);         //Read CRT VTotal reg
        m = (K2V_ReadCrReg(0x07) & 0x01) | ((K2V_ReadCrReg(0x07) & 0x20) >> 4);
        r = ((K2V_ReadCrReg(0x5E) & 0x01) << 2);

        vTotal = (long)((n | (m << 8) | (r << 8)) + 2);


        refreshRate = (long)(dclk*10000000L)/(long)(hTotal*vTotal);

        refreshIni = (int)refreshRate/10;

        if ((refreshRate % 10) >= 5)
            refreshIni++;

        //check for interlace mode
        if (K2V_ReadCrReg(0x42) & 0x20)
            refreshIni /= 2;

        //Make sure that data of paired register read come from IGA1
        K2V_WriteSrReg(0x26,0x40);

    }

    return( refreshIni);
}


/*---------------------------------------------------------------------------
 *
 * GetMemType
 *
 */
static int GetMemType( void)
{
        int iMemType;

        if ( S3MobileData.wSPStyle & SP_M5 )
        {
            iMemType = ((K2V_ReadCrReg(0x36) & 0x0C) >> 3);
            return(iMemType);
        }

        iMemType = ((K2V_ReadCrReg(0x36) & 0x0C) >> 2);
        return(iMemType);
}

/*---------------------------------------------------------------------------
 *
 * GetMemSize
 *
 */
#define K2V_CR36_MEM_MASK 0xe0
#define K2V_CR36_MEM_1MB  0xc0
#define K2V_CR36_MEM_2MB  0x80

//### added support for up to 4M
static int GetMemSize( void)
{
        int mem_size;
//ASPEN
        int dram_size;
        if ( S3MobileData.wSPStyle & SP_M5)
        {
                if (K2V_ReadCrReg(0x36) & 0xC0)
                     return 2;
                else
                     return 4;

        }

    if (wDeviceId==D_S3VIRGEVX) {
                //It's ASPEN, we have to do some calculation.
                mem_size=(((K2V_ReadCrReg(0x36) & 0x60) >> 5)+1)*2;
                dram_size=(((K2V_ReadCrReg(0x37) & 0x60) >> 5)^0x3)*2;
                mem_size=mem_size-dram_size;
                if (mem_size == 2) return 1;
                else if (mem_size == 4) return 2;
                else if (mem_size == 6) return 3;
                else return 4;
   }
        else
        {
        mem_size = K2V_ReadCrReg(0x36) & K2V_CR36_MEM_MASK;

        if (mem_size == K2V_CR36_MEM_1MB) return 0;
        else if (mem_size == K2V_CR36_MEM_2MB) return 1;
        else return 2;
        }
}

/*---------------------------------------------------------------------------
 *
 * GetMCLK
 *
 */
int GetMCLK(void)
{
   int m, n, r;
   long mclock;
   int result;

   n = K2V_ReadSrReg(0x10);
   m = K2V_ReadSrReg(0x11) & 0x7f;

   r = (n >> 5) & 0x03;

   if(r==0)
      r=1;
   else
      r = 2<<(r-1);

   n = n & 0x1f;

   if ( bFamilyFlags & MOBILE_FAMILY )
       mclock = ((long)(m+2)*27000L)/((long)((n+2)*r)*100L);
   else
       mclock = ((long)(m+2)*14318L)/((long)((n+2)*r)*100L);

   result = (int)(mclock/10L);
   if( (mclock % 10) >= 7 )
      result++;

   return(result);
}

/*---------------------------------------------------------------------------
 *
 * GetMemSpeed
 *
 */
static int K2V_GetProfile( LPSTR lpControl, int dev, int mode, int refresh,
                           int type, int speed, int size, LPSTR lpString);

static int GetMemSpeed( int mode, int refresh, int type, int size)
{
        int m, n, r;
        int speed;
        long mclock;

   if (!(S3MobileData.dwMobileCaps & MOBILE_FAMILY ))
      {

            n = K2V_ReadSrReg(0x10);
            m = K2V_ReadSrReg(0x11) & 0x7f;

            //Get memory speed:
            r = (n >> 5) & 0x03;
            if( r==0)
                    r=1;
            else
                    r = 2<<(r-1);

            n = n & 0x1f;

            mclock = ((long)(m+2)*14318L)/((long)((n+2)*r)*100L);

   }
   else
   {

       if ( (K2V_ReadSrReg(0x27) & 0x03) == 1 ) {

                n = K2V_ReadSrReg(0x12);            //Use DClk PLL
                m = K2V_ReadSrReg(0x13);

           //Get memory speed:
                r = (n >> 6);
                if( r==0)
                        r=1;
                else
                        r = 2<<(r-1);

                n = n & 0x3f;
       }
       else {
                n = K2V_ReadSrReg(0x10);            //Use MClk PLL
                m = K2V_ReadSrReg(0x11) & 0x7f;

           //Get memory speed:
                r = (n >> 5) & 0x03;
                if( r==0)
                        r=1;
                else
                        r = 2<<(r-1);

                n = n & 0x1f;
       }

       if ( bFamilyFlags & MOBILE_FAMILY )
           mclock = ((long)(m+2)*27000L)/((long)((n+2)*r)*100L);
       else
           mclock = ((long)(m+2)*14318L)/((long)((n+2)*r)*100L);

       switch ( K2V_ReadSrReg(0x27)&0x0c )
       {
           case 0x0c:
               mclock = mclock/4;
               break;

           case 0x08:
               mclock = mclock/3;
               break;

           case 0x04:
               mclock = mclock/2;
               break;
       }
   }

        speed = (int)mclock/10;
        if( (mclock % 10) >= 5 )
                speed++;

        for ( ; speed >= 40; speed-- )
        {
                if( K2V_GetProfile( (LPSTR)NULL, HWCODEC, mode, refresh, type, speed, size, szString) ||
                        K2V_GetProfile( (LPSTR)NULL, SWCODEC, mode, refresh, type, speed, size, szString) ||
                        K2V_GetProfile( (LPSTR)NULL, DIGITIZER, mode, refresh, type, speed, size, szString) )
                        break;

                if ( speed == 51 )
                        return( 40);
        }
        if ( speed < 40 )
            return( 40);


        return(speed);
}

/*---------------------------------------------------------------------------
 *
 * atohexdw
 *
 */
const char *SEP = " ,\t\n";

static DWORD atohexdw(char *alphanum_str)
{
   DWORD hexnum;
   char *sptr;
   char c;

   if ( !(sptr = (char *) strchr(alphanum_str, 'x')) )
       if ( !(sptr = (char *) strchr(alphanum_str, 'X')) )
           sptr = alphanum_str;
   if ( *sptr == 'x' || *sptr == 'X' )
       sptr++;

   for ( hexnum = 0; *sptr; sptr++ ) {
       c = toupper(*sptr);
       if ( 'A' <= c && c <= 'F' ) {
       hexnum <<= 4;
       hexnum += (c - 'A') + 10;
       }
       else if ( '0' <= c && c <= '9' ) {
       hexnum <<= 4;
       hexnum += (c - '0');
       }
       else
       break;
   }
   return(hexnum);
}

/*---------------------------------------------------------------------------
 *
 * GetFifoValue
 *
 */
static void GetFifoValue(int dev, char *sptr)
{
        int i;

        sptr = (char *) strtok(sptr, SEP);

        for( i = 0; sptr && (i<DSTWIN_SIZES); i++)
        {
                k2vTable[dev].wStep[i].fifo.value = atohexdw(sptr); //Get fifo value

                if ( sptr = (char *) strtok(NULL, SEP) )
                        k2vTable[dev].wStep[i].fifo.dstWidth = atoi(sptr); //Get window width

                sptr = (char *) strtok(NULL, SEP); //Get next token
        }
}

/*---------------------------------------------------------------------------
 *
 * GetWidthRatio
 *
 */
static void GetWidthRatio(int dev, char *sptr)
{
        int     i;

        sptr = (char *) strtok(sptr, SEP);

        for( i = 0; sptr && (i<DSTWIN_SIZES); i++)
        {
                k2vTable[dev].wStep[i].ratio.value = atoi(sptr); //Get window ratio

                if ( sptr = (char *) strtok(NULL, SEP) )
                        k2vTable[dev].wStep[i].ratio.dstWidth = atoi(sptr); //Get window width

                sptr = (char *) strtok(NULL, SEP); //Get next token
        }
}

static void btoh(BYTE byte_in, char *sptr)
{
        BYTE tmp, tmp1;

        tmp = byte_in / 16;
        tmp1 = byte_in % 16;
        if (tmp < 10)
                tmp += 0x30;
        else
                tmp += 0x37;
        if (tmp1 < 10)
                tmp1 += 0x30;
        else
                tmp1 += 0x37;
        *sptr = tmp;
        *(sptr+1) = tmp1;
}
/*---------------------------------------------------------------------------
 *
 * K2V_GetProfile
 *
 */
static int K2V_GetProfile( LPSTR lpControl, int dev, int mode, int refresh,
                           int type, int speed, int size, LPSTR lpString)
{
        BYTE bCR2D, bCR2E;
        //get ini file name
        if( !GetPrivateProfileString( "display", szDevice[dev], "", szIniFile, MAX_STRING_SIZE, "system.ini"))
        {
                memset( szIniFile, 0, sizeof ( szIniFile ) );
                lstrcpy( szIniFile, szDeviceIni[dev]);
                bCR2D = K2V_ReadCrReg(0x2d);
                bCR2E = K2V_ReadCrReg(0x2e);
                btoh(bCR2D, szIniFile);
                btoh(bCR2E, szIniFile+2);
                if ( (S3MobileData.dwMobileCaps & MOBILE_FAMILY)
                     && (!(bPanelType & S3_MOBILE_TFT_PANEL))
                     && (S3MobileData.bCurrentDisplay&FP_ONLY) )
                   //Mobile STN panel and FP only
                   szIniFile[4]='D';

        }

        //compose section name
        memset( szSection, 0, sizeof ( szSection ) );
        if( lpControl == (LPSTR)NULL )
        {
                lstrcpy( szSection, szDevice[dev]);
        }
        else
        {
                lstrcpy( szSection, lpControl);
                lstrcat( szSection, "_");
                lstrcat( szSection, szDevice[dev]);
        }

        //compose key profile
        memset( szProfile, 0, sizeof ( szProfile ) );
        lstrcpy( szProfile, ModeKey[mode]);
        lstrcat( szProfile, "_");
        lstrcat( szProfile,     RefreshKey[refresh]);
        lstrcat( szProfile, "_");
        lstrcat( szProfile,     MemTypeKey[type]);
        lstrcat( szProfile, "_");
        _itoa(speed, szTmp, 10);
        lstrcat( szProfile,     szTmp);
        lstrcat( szProfile, "_");
        lstrcat( szProfile,     MemSizeKey[size]);

        //get the profile string
        if (bCR2D == 0x8A)
                while (!GetPrivateProfileString( szSection, szProfile, "", lpString, MAX_STRING_SIZE, szIniFile))
                {
                        memset( szIniFile, 0, sizeof ( szIniFile ) );
                        lstrcpy( szIniFile, szDeviceIni[dev]);

                        if ( bCR2E > 1 )
                                bCR2E--;
                        else
                        {
                                bCR2D=0x56;
                                bCR2E=0x31;
                        }
                        btoh(bCR2D, szIniFile);
                        btoh(bCR2E, szIniFile+2);
                }
        return( GetPrivateProfileString( szSection, szProfile, "", lpString, MAX_STRING_SIZE, szIniFile));
}

/*---------------------------------------------------------------------------
 *
 * InitK2vTable
 *
 * Initialize K2VTABLE from RFMK2V.INI
 *
 */
int __loadds FAR PASCAL InitK2vTable( void)
//DWORD __loadds FAR PASCAL InitK2vTable( void)
{
        int i;
        int sMode;
        int sRefresh;
        int mType;
        int mSize;
        int mSpeed;
   int dclk;
   int mclk;
   int dclksDuringBlank;
   BYTE fInterlaced;


        //Get memory information
        mType    = GetMemType();
        if( NOT_WITHIN_RANGE( mType, 0, NumMemTypeKey-1))
                return( K2V_ERR);

        mSize    = GetMemSize();
        if( NOT_WITHIN_RANGE( mSize, 0, NumMemSizeKey-1))
                return( K2V_ERR);


   mclk = GetMCLK();

   //Fill the data structure for the input
   pBand->SPStyle = S3MobileData.wSPStyle;
   pBand->DuoViewFlags = (WORD)(S3MobileData.dwMobileCaps&0xffff);

   Bandwidth_Init(pBand, DeviceId_High, (DeviceId_High << 8) | (wDeviceId>>8), mType, mclk, mSize);

   if (!(pBand->SWFlags & SW_CALCULATE))
   {

        //Get screen information

        sMode = GetMode( wScreenHeight, wBpp);
        if( NOT_WITHIN_RANGE( sMode, 0, NumModeKey-1))
                return( K2V_ERR);

        sRefresh = GetRefresh( wScreenHeight);
        if( NOT_WITHIN_RANGE( sRefresh, 0, NumRefreshKey-1))
                return( K2V_ERR);

        //Adjust the sRefresh for 10x7 modes:
        if( (sMode == 0 || sMode == 1) && sRefresh == 2)
                sRefresh = 5;

        mSpeed   = GetMemSpeed( sMode, sRefresh, mType, mSize);
        if( NOT_WITHIN_RANGE( mSpeed, 30, 70))
                return( K2V_ERR);


        //Initialize the table
        for( i=0; i<MAX_DEVICE; i++)
        {
                // get width ratio from the profile string
                K2V_GetProfile( (LPSTR)NULL, i, sMode, sRefresh, mType, mSpeed, mSize, szString);
                GetWidthRatio( i, szString);

                // get fifo values from the profile string
                K2V_GetProfile( "FIFO", i, sMode, sRefresh, mType, mSpeed, mSize, szString);
                GetFifoValue( i, szString);
        }

   }

   dclksDuringBlank = GetDclksDuringBlank();

   dclk = GetDCLK();

   fInterlaced = K2V_ReadCrReg(0x42) & 0x20;

   pBand->GRefreshRate = GetRefreshRate( wScreenHeight);

   if ( (S3MobileData.dwMobileCaps&DUAL_IMAGE) ||
        (S3MobileData.dwMobileCaps&DUOVIEW_SAMEIMAGE) )
   {
        pBand->MHzDCLK2 = GetDCLK2();
        pBand->GRefreshRate2 = GetRefreshRate2();
        pBand->GSizeX2 = 0;
        pBand->GSizeY2 = 0;
   }

   Bandwidth_SetPrimary(pBand, dclk, wScreenWidth, wScreenHeight,
                        wBpp, fInterlaced, dclksDuringBlank);

   /* Initialize calculations with dummy values */
   pBand->IsSecFormatYUV = 1;   //Suppose MPEG2
   Bandwidth_SetSecondary(pBand, ID_SWCODEC, 16, 720, 240, 720, 240, 720, 240);


        return( K2V_OK);
}

/*---------------------------------------------------------------------------
 *
 * GetK2vRatio
 *
 * For given device, return maximum horizontal stretch ratio from K2VTABLE.
 *
 */
DWORD __loadds FAR PASCAL GetK2vRatio( WORD deviceType)
{
        int i;
        DWORD dwTmp;
        DWORD dwMaxRatio = 1000L;
        DWORD result;

        for (i = 0; i < DSTWIN_SIZES; i++)
        {
                dwTmp = k2vTable[deviceType].wStep[i].ratio.value;
                if( dwMaxRatio < dwTmp)
                        dwMaxRatio = dwTmp;
        }
   result = dwMaxRatio;

   if ((result == 0) || (pBand->SWFlags & SW_CALCULATE))
   {
      result = pBand->HStretch;
   }

        return(result);
}

/*---------------------------------------------------------------------------
 *
 * Get K2V FIFO and RAS Controls (MM81EC)
 *
 * For given device type and the destination width,
 * return optimum FIFO setting from K2VTABLE.
 *
 */
DWORD __loadds FAR PASCAL GetK2vFifo( WORD deviceType, WORD wSize)
{
        int i;
        DWORD result;

        for( i = 0; i < DSTWIN_SIZES; i++)
                if( wSize <= k2vTable[deviceType].wStep[i].fifo.dstWidth)
                        break;

        if( i >= DSTWIN_SIZES) i--;

   result = k2vTable[deviceType].wStep[i].fifo.value;

   if ((result == 0) || (pBand->SWFlags & SW_CALCULATE))
   {
      result = pBand->FifoCtrl;
   }


        return(result);
}

