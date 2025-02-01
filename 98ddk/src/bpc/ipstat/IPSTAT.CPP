// Copyright (C) 1997-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture Programmer's Reference.
// For more information about writing applications that use
// Broadcast Architecture, see the Broadcast Architecture 
// Programmer's Reference.

//==========================================================================;
//
// Displays IP addresses/ports receiving data
//
// davefe
//==========================================================================;

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#include "iks.h"
#include "stats.h"
#include "wdmioctl.h"

#define STATBUFS 256
#define DRIVER_NAME "\\\\.\\NABTSIP"

int __cdecl main(int argc, char **argv)
{
	char szName[256];
	HANDLE hFile = NULL;

	printf("IPSTAT	1.00\n");

	BOOL    bStatus = FALSE;
	DWORD dwAccess = GENERIC_READ | GENERIC_WRITE;
#ifdef OLAP
	DWORD dwFlags = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED;
#else
	DWORD dwFlags = FILE_ATTRIBUTE_NORMAL;
#endif	
	hFile = CreateFile( 
            DRIVER_NAME, 
			dwAccess, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, 
			NULL, 
			OPEN_EXISTING,     
			dwFlags,
			NULL 
            );

    if ((HANDLE)-1 != hFile) {
        bStatus = TRUE;
		printf("\n0x%X: Opened Driver", hFile);
		
    } else {
		printf("\nCould not open driver");
        hFile = NULL;
		return 0;
    }

	PNAB_STATS pNS = (PNAB_STATS)GlobalAlloc(GMEM_FIXED, sizeof(NAB_STATS) * STATBUFS);
	PNAB_STATS pCurrent;
	DWORD dwReturned;

#ifdef OLAP

	OVERLAPPED  Overlapped = {0};
	
    if (!( Overlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL ))) {
		nStatus = GetLastError();
		printf("\nError %d creating event", nStatus);
        
    } else  {
		printf("\nOverlapped...");
		bStatus = DeviceIoControl( hFile, IOCTL_NAB_IP_STATS, NULL, 0, 
                                   (LPBYTE)pNS, sizeof(NAB_STATS) * STATBUFS, 
								   &dwReturned, &Overlapped);
		bStatus = FALSE;
		while (!bStatus) {
			bStatus = GetOverlappedResult(hFile, &Overlapped, &dwBytes, FALSE );
			if (bStatus == ERROR_IO_INCOMPLETE || bStatus == ERROR_IO_PENDING) {
				if (ERROR_IO_INCOMPLETE == nStatus) 
					printf("\nERROR_IO_INCOMPLETE");
				else if (ERROR_IO_PENDING)
					printf("\nERROR_IO_PENDING");
                Sleep(100); 
                bStatus = 0;
            }
		}
		printf("\nbStatus = %d; dwBytes = %d", bStatus, dwBytes);
	}
#else
	printf("\nNon-Overlapped...");
	bStatus = DeviceIoControl(hFile, IOCTL_NAB_IP_STATS, NULL, 0, 
                                   (LPBYTE)pNS, sizeof(NAB_STATS) * STATBUFS, 
								   &dwReturned, NULL);
#endif
	if (bStatus) {
		int i, count;
		pCurrent = pNS;
		FILETIME ft;
		FILETIME ftLocal;
		SYSTEMTIME st;
		SYSTEMTIME stNow;


		printf("\nDeviceIoControl succeeded: %lu bytes", dwReturned);
		GetLocalTime(&stNow);
		printf("\nLocal Time Now: %.02d/%.02d/%.02d %.02d:%.02d:%.02d", 
			stNow.wMonth, stNow.wDay, stNow.wYear,
			stNow.wHour, stNow.wMinute, stNow.wSecond);
		printf("\nAddresses...\n");

		count = dwReturned / sizeof(NAB_STATS);
		
		for (i = 0; i < count; i++) {
			if (pCurrent->ipAddr.ucHighMSB > 223) {			
				ft.dwLowDateTime = pCurrent->liLastTimeUsed.LowPart;
				ft.dwHighDateTime = pCurrent->liLastTimeUsed.HighPart;
				FileTimeToLocalFileTime(&ft, &ftLocal);
				FileTimeToSystemTime(&ftLocal, &st);

				printf("\n%.02d/%.02d/%.02d %.02d:%.02d:%.02d  ", 
					st.wMonth, st.wDay, st.wYear,
					st.wHour, st.wMinute, st.wSecond);

				printf("%d.%d.%d.%d : %d\n", pCurrent->ipAddr.ucHighMSB,
										pCurrent->ipAddr.ucHighLSB,
										pCurrent->ipAddr.ucLowMSB,
										pCurrent->ipAddr.ucLowLSB,
										MAKEWORD(pCurrent->udpPort.PortLSB,pCurrent->udpPort.PortMSB));
			
				++pCurrent;
			}
		}
	
	} else
		printf("\nDeviceIoControl failed: %d", bStatus);

	if (NULL != hFile)
		bStatus = CloseHandle(hFile);

    return bStatus;
    
}

