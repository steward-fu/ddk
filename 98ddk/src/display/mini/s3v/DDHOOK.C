/*****************************************************************************
 *;++
 * Name           : $RCSfile$
 * Title          : Direct Draw Hook
 * C Author       : Jim Page
 * Created        : 03/02/96
 *
 * Copyright      : 1995 by VideoLogic Limited. All rights reserved.
 *                  No part of this software, either material or conceptual 
 *                  may be copied or distributed, transmitted, transcribed,
 *                  stored in a retrieval system or translated into any 
 *                  human or computer language in any form by any means,
 *                  electronic, mechanical, manual or other-wise, or 
 *                  disclosed to third parties without the express written
 *                  permission of VideoLogic Limited, Unit 8, HomePark
 *                  Industrial Estate, King's Langley, Hertfordshire,
 *                  WD4 8LZ, U.K.
 *
 * Description    : Pseudo stackable direct draw hook services  
 *                   
 * Program Type   : C module (.dll)
 *
 * RCS info:
 *
 * $Date:   04 Mar 1997 14:49:08  $
 * $Revision:   1.0  $
 * $Locker$
 * $Log:   Q:/SOFTDEV/VCS/WIN9x/display/mini/s3_3/DDHOOK.C_v  $
//
//   Rev 1.0   04 Mar 1997 14:49:08   mbenso
//Initial revision.
//
//   Rev 1.0   01 Jul 1996 14:23:54   bchung
//Initial revision.
 *
 *;--
 *****************************************************************************/

#include "inc16\windows.h"
#include "ddrawi.h"
#include "ddhook.h"

typedef struct tagHOOKDRIVER
{
	HINSTANCE					hDriver;
	BYTE						szDriverName[128];
	LPFNSTACKHALINFO			StackHALInfo;
	
} HOOKDRIVER, *PHOOKDRIVER;

#define MAX_DRIVERS 8

static BOOL gbInitialised = FALSE;
static HOOKDRIVER gDriverList[MAX_DRIVERS];
static int gnDrivers = 0;

#if !DEBUG
#define ODS / ## /
#else

#define START_STR "DDHOOK: "
#define END_STR "\n"

void ODS (LPSTR szFormat, ...)
{
    static int (WINAPI *fpwvsprintf)(LPSTR lpszOut, LPCSTR lpszFmt, const void FAR* lpParams) = NULL;
    HANDLE	hnd = NULL;
    char	str[256];

    if (fpwvsprintf == NULL)
    {
		hnd = GetModuleHandle("USER");
		if (hnd == NULL)
		{
		    return;
		}

		fpwvsprintf = (LPVOID) GetProcAddress(hnd, "wvsprintf");

		if (fpwvsprintf == NULL)
		{
		    return;
		}
    }

    lstrcpy (str, START_STR);
    fpwvsprintf (str + lstrlen (str), szFormat, (LPVOID) (&szFormat+1));
    lstrcat (str, END_STR);
    OutputDebugString (str);
}
#endif

BOOL DDHookInit ()
{
	BOOL bRet = FALSE;
	
	if (!gbInitialised)
	{
		int k;
//	    BYTE  bData[128];
				
		ODS ("DDHookInit: entry");

		// get details from system.ini
		
		gnDrivers = GetPrivateProfileInt ("DDHOOK", "NumDrivers", 0, "system.ini");
		
		if (gnDrivers > MAX_DRIVERS)
		{
			gnDrivers = MAX_DRIVERS;
		}
		
		ODS ("DDHookInit: %d drivers registered", gnDrivers);
		
		// go through drivers and load them

		for (k = 0; k < gnDrivers; ++k)
		{
			static char szEntry[32] = "Driver ";
			
			bRet = FALSE;
			
			szEntry[6] = '0' + k;
			GetPrivateProfileString ("DDHOOK", szEntry, "", gDriverList[k].szDriverName, 
															sizeof (gDriverList[k].szDriverName), "system.ini");
			
			ODS ("DDHookInit: driver %d (%s)", k, (LPSTR) gDriverList[k].szDriverName);

			if (strlen (gDriverList[k].szDriverName))
			{
				gDriverList[k].hDriver = LoadLibrary (gDriverList[k].szDriverName);
				
				if (gDriverList[k].hDriver)
				{
					(FARPROC) gDriverList[k].StackHALInfo = GetProcAddress (gDriverList[k].hDriver, "StackHALInfo");
					
					if (gDriverList[k].StackHALInfo)
					{
						ODS ("\tOK!");
						bRet = TRUE;
					}
					else
					{
						ODS ("\tStackHALInfo entry point not found");
						FreeLibrary (gDriverList[k].hDriver);
					}
				}
				else
				{
					ODS ("\terror loading driver");
				}
			}
			else
			{
				ODS ("\tbad name");
			}
		}
	}

	if (!bRet)
	{
		DDHookDeinit ();
	}
	else
	{
		gbInitialised = TRUE;
	}
	
	ODS ("DDHookInit: exit (%d)", bRet);

	return (bRet);
}

void DDHookDeinit ()
{
	int k;
	
	ODS ("DDHookDeinit");

	for (k = 0; k < gnDrivers; ++k)
	{
		if (gDriverList[k].hDriver)
		{
			FreeLibrary (gDriverList[k].hDriver);
		}
	
		gDriverList[k].hDriver = NULL;
	}
}

LPDDHALINFO DDHookOnDDCreateDriverObject (LPDDHALINFO lpddHALInfo, int bReset)
{
	int k;
	LPDDHALINFO lpTempHALInfo, lpNewHALInfo = lpddHALInfo;
	
	// make sure drivers are loaded
	
	if (!gbInitialised)
	{
		ODS ("Initialising");

		if (!DDHookInit ())
		{
			return (lpNewHALInfo);
		}
	}
	
	// Go through each driver Stacking up the HALInfo
	
	for (k = 0; k < gnDrivers; ++k)
	{
		lpTempHALInfo = gDriverList[k].StackHALInfo (lpNewHALInfo, bReset);
		
		if (lpTempHALInfo)
		{
			ODS ("Driver %s active", gDriverList[k].szDriverName);
			lpNewHALInfo = lpTempHALInfo;
		}
		else
		{
			ODS ("Driver %s not active", gDriverList[k].szDriverName);
			// NULL lpTempHALInfo means driver has done nothing
		}
	}
	
	return (lpNewHALInfo);	
}


/* end of $RCSfile$ */
