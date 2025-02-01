#include <windows.h>
#include "s3color.h"

//Proto main to get by a stupid linker error.
int main(int argc, char ** arv)
{
    return 0;
}
/*****************************/
/* Color conversion routines */
/*****************************/


LONG FitRangeAtoB(LONG Avalue, LONG Amin, LONG Amax, LONG Bmin, LONG Bmax)
{  return ( ((Avalue-Amin)*(Bmax-Bmin)/(Amax-Amin)) + Bmin);
}


LONG HWtoDD_Bright(DWORD dwReg)
{  LONG Avalue = dwReg & 0x000000FF;
   if (Avalue >= 128)
     Avalue -= 256;

   if (Avalue <= HW_DEF_BRIGHT)
   {  return ( FitRangeAtoB(Avalue, HW_MIN_BRIGHT, HW_DEF_BRIGHT,
                        			DD_MIN_BRIGHT, DD_DEF_BRIGHT) );
   }
   else
   {  return ( FitRangeAtoB(Avalue, HW_DEF_BRIGHT, HW_MAX_BRIGHT,
                        			DD_DEF_BRIGHT, DD_MAX_BRIGHT) );
   }
}


LONG HWtoDD_Cont(DWORD dwReg)
{  LONG Avalue = (dwReg & 0x00001F00) >> 8;

   if (Avalue <= HW_DEF_CONT)
   {  return ( FitRangeAtoB(Avalue, HW_MIN_CONT, HW_DEF_CONT,
                        			DD_MIN_CONT, DD_DEF_CONT) );
   }
   else
   {  return ( FitRangeAtoB(Avalue, HW_DEF_CONT, HW_MAX_CONT,
                        			DD_DEF_CONT, DD_MAX_CONT) );
   }
}


LONG HWtoDD_Hue(DWORD dwReg)
{  LONG lHue;
   LONG lFactor1 = (dwReg & 0x001F0000) >> 16;
   LONG lFactor2 = (dwReg & 0x1F000000) >> 24;
   float fFactor1, fFactor2, temp;

   /* convert factors to signed */
   if (lFactor1 >= 16)
     lFactor1 -= 32;
   if (lFactor2 >= 16)
     lFactor2 -= 32;

   fFactor1 = (FLOAT)lFactor1;   fFactor2 = (FLOAT)lFactor2;

   /* calculate Sat */
   temp = (float)sqrt( fFactor1*fFactor1 + fFactor2*fFactor2 );
   temp = (float)((float)acos(fFactor1 / temp) * 180.0 / (float)PI);

   /* round it */
   if (temp < 0)
     lHue = (LONG)(temp-0.5);
   else
     lHue = (LONG)(temp+0.5);

   /* set sign */
   if ( fFactor2 < 0 )
     lHue = -lHue;

   if (lHue <= HW_DEF_HUE)
   {  return ( FitRangeAtoB(lHue, HW_MIN_HUE, HW_DEF_HUE,
                        		  DD_MIN_HUE, DD_DEF_HUE) );
   }
   else
   {  return ( FitRangeAtoB(lHue, HW_DEF_HUE, HW_MAX_HUE,
                        		  DD_DEF_HUE, DD_MAX_HUE) );
   }
}


LONG HWtoDD_Sat(DWORD dwReg)
{  LONG lSat;
   float temp;
   LONG factor1 = (dwReg & 0x001F0000) >> 16;
   LONG factor2 = (dwReg & 0x1F000000) >> 24;

   /* convert factors to signed */
   if (factor1 >= 16)
     factor1 -= 32;
   if (factor2 >= 16)
     factor2 -= 32;

   temp = (float)sqrt( factor1*factor1 + factor2*factor2 );
   /* round it */
   if (temp<0)
     lSat = (LONG)(temp-0.5);
   else
     lSat = (LONG)(temp+0.5);

   if (lSat <= HW_DEF_SAT)
   {  return ( FitRangeAtoB(lSat, HW_MIN_SAT, HW_DEF_SAT,
                        		  DD_MIN_SAT, DD_DEF_SAT) );
   }
   else
   {  return ( FitRangeAtoB(lSat, HW_DEF_SAT, HW_MAX_SAT,
                        		  DD_DEF_SAT, DD_MAX_SAT) );
   }
}


DWORD  DDtoHW_BrightCont(LONG lDDBright, LONG lDDCont)
{  DWORD dwHWBright, dwHWCont;

   if (lDDBright <= HW_DEF_BRIGHT)
   {  dwHWBright = FitRangeAtoB(lDDBright, DD_MIN_BRIGHT, DD_DEF_BRIGHT,
                           			       HW_MIN_BRIGHT, HW_DEF_BRIGHT);
   }
   else
   {  dwHWBright = FitRangeAtoB(lDDBright, DD_DEF_BRIGHT, DD_MAX_BRIGHT,
                        			       HW_DEF_BRIGHT, HW_MAX_BRIGHT);
   }
   if (lDDCont <= HW_DEF_CONT)
   {  dwHWCont = FitRangeAtoB(lDDCont, DD_MIN_CONT, DD_DEF_CONT,
                        			   HW_MIN_CONT, HW_DEF_CONT);
   }
   else
   {  dwHWCont = FitRangeAtoB(lDDCont, DD_DEF_CONT, DD_MAX_CONT,
                        			   HW_DEF_CONT, HW_MAX_CONT);
   }

   return ( ((dwHWCont & 0x0000001F) << 8) | (dwHWBright & 0x000000FF) );
}


DWORD  DDtoHW_HueSat(LONG lDDHue, LONG lDDSat)
{  DWORD dwHWHue, dwHWSat;
   DWORD factor1, factor2;
   float temp;

   if (lDDHue <= HW_DEF_HUE)
   {  dwHWHue = FitRangeAtoB(lDDHue, DD_MIN_HUE, DD_DEF_HUE,
                        			    HW_MIN_HUE, HW_DEF_HUE);
   }
   else
   {  dwHWHue = FitRangeAtoB(lDDHue, DD_DEF_HUE, DD_MAX_HUE,
                        			    HW_DEF_HUE, HW_MAX_HUE);
   }
   if (lDDSat <= HW_DEF_SAT)
   {  dwHWSat = FitRangeAtoB(lDDSat, DD_MIN_SAT, DD_DEF_SAT,
                        			   HW_MIN_SAT, HW_DEF_SAT);
   }
   else
   {  dwHWSat = FitRangeAtoB(lDDSat, DD_DEF_SAT, DD_MAX_SAT,
                        			   HW_DEF_SAT, HW_MAX_SAT);
   }

   temp = dwHWSat * (float)cos((LONG)dwHWHue * PI / 180.0);
   /* round it */
   if (temp<0)
     factor1 = (LONG)(temp-0.5);
   else
     factor1 = (LONG)(temp+0.5);

   temp = dwHWSat * (float)sin((LONG)dwHWHue * PI / 180.0);
   /* round it */
   if (temp<0)
     factor2 = (LONG)(temp-0.5);
   else
     factor2 = (LONG)(temp+0.5);

   return ( ((factor1 & 0x0000001F) << 16) |
            ((factor2 & 0x0000001F) << 24) );
}
