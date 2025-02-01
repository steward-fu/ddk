//
// Sample code to suport ICT on MS drivers.
//

#ifdef MSBUILD

#include "inc16\windows.h"
#include "s3_ddtk.h"

typedef struct	tagDMMFUNCTION
{
	WORD 		wFunction;			// DMM extenstion number
} DMMFUNCTION, FAR* LPDMMFUNCTION;

typedef DMMFUNCTION 				DMMQUERYDRIVER;
typedef DMMQUERYDRIVER FAR* 		LPDMMQUERYDRIVER;

// MS Driver
#define DMMMSCOMMAND				0x8001	// driver escape
#define DMM_QUERYDRIVER 			0x0000	// driver query
#define DMMACK						0x4B4F	// DMM driver acknowledge

BOOL IsDiamondHardware();

typedef LPVOID LPPDEVICE;

BOOL __loadds FAR PASCAL Diamond_Control(LPPDEVICE lpDevice, WORD wFunction, LPBYTE lpIn, LPBYTE lpOut)
{
	LPDMMFUNCTION	lpDMMStruct = (LPDMMFUNCTION) lpIn;
	BOOL			retVal = FALSE;
	
	if( wFunction == DMMMSCOMMAND )
	{
		if( lpDMMStruct->wFunction == DMM_QUERYDRIVER )
		{
			// Here we detect Diamond Hardware and we return bHandled.
			if( IsDiamondHardware() ) 
				retVal = DMMACK;
		}
	}

	return (retVal);
}

extern S3THKINFO sData;

#define BIOS_DIAMOND 4

BOOL IsDiamondHardware (void)
{
   WORD fResult = FALSE;

   {
      _asm {
         pushad
         mov ax, 0x1DAA
         mov bx, 0xFDEC
         int 0x10
         cmp bx, 0xCEDF
         jne lblFALSE
         mov fResult, 0x01
lblFALSE:
         popad
      }
   }
   return fResult;
}

#endif // #ifdef MSBUILD

