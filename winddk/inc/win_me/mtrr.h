
#ifndef	_MTRR_H_
#define	_MTRR_H_

#define	MTRR_DEVICE_ID	DDS_DEVICE_ID

/*INT32*/

/*XLATOFF*/
#define MTRR_Service	Declare_Service
#define MTRR_StdCall_Service Declare_SCService
#pragma	warning (disable:4003)

/*XLATON*/

#ifndef	_NTDDK_
typedef ULONG	MEMORY_CACHING_TYPE;

#define		MmNonCached			0
#define		MmCached			1
#define		MmFrameBufferCached		2
#define		MmHardwareCoherentCached        3
#define		MmMaximumCacheType		4
#endif

/*MACROS*/
Begin_Service_Table(MTRR, VxD)

MTRR_Service		(_MTRR_Get_Version, LOCAL)
MTRR_StdCall_Service    (MTRRSetPhysicalCacheTypeRange, 4)
MTRR_Service		(MTRRIsPatSupported, LOCAL)
MTRR_Service		(MTRR_PowerState_Change, LOCAL)

End_Service_Table(MTRR, VxD)

/*ENDMACROS*/

/*XLATOFF*/

#ifndef WANTVXDWRAPS

WORD VXDINLINE
MTRRGetVersion() {

   WORD w;

   Touch_Register(eax);
   Touch_Register(ecx);
   Touch_Register(edx);
   
   VxDCall(_MTRR_Get_Version);
   _asm mov [w], ax;
   return (w);

}

ULONG VXDINLINE
MTRRSetPhysicalCacheTypeRange(
   PVOID PhysicalAddress,
   ULONG NumberOfBytes,
   MEMORY_CACHING_TYPE CacheType) {

   ULONG	ulResult;

   Touch_Register(eax);
   Touch_Register(ecx);
   Touch_Register(edx);
   
   _asm push CacheType;
   _asm push NumberOfBytes;
   _asm push 0;
   _asm push PhysicalAddress;
   VxDCall(MTRRSetPhysicalCacheTypeRange);
   _asm	mov [ulResult], eax;
   
   return (ulResult);
}

#endif

BOOLEAN VXDINLINE
IsPatSupported() {

   BOOLEAN b;
   WORD w;
   
   Touch_Register(eax);
   Touch_Register(ecx);
   Touch_Register(edx);
    
   VxDCall(_MTRR_Get_Version);
   _asm mov w, ax;
   if (!w)
       return 0;
   VxDCall(MTRRIsPatSupported);
   _asm mov [b], al;
   return (b);
}

/*XLATON*/

#endif // ifndef _MTRR_H_
