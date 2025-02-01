
#include <windows.h>
#include "imm.h"
#include "fakeime.h"

#ifdef DEBUG

#ifdef FAKEIMEM
const LPTSTR g_szRegInfoPath = "software\\microsoft\\fakeime\\m";
#else
const LPTSTR g_szRegInfoPath = "software\\microsoft\\fakeime\\a";
#endif

int DebugPrint(LPCTSTR lpszFormat, ...)
{
	int nCount;
	TCHAR szMsg[1024];

	va_list marker;
	va_start(marker, lpszFormat);
	nCount = wvsprintf(szMsg, lpszFormat, marker);
	va_end(marker);
	OutputDebugString(szMsg);
	return nCount;
}

DWORD PASCAL GetDwordFromSetting(LPSTR lpszFlag)
{
	HKEY hkey;
	DWORD dwRegType, dwData, dwDataSize, dwRet;

	dwData = 0;
	dwDataSize=sizeof(DWORD);
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_szRegInfoPath, 0, KEY_READ, &hkey)) {
		dwRet = RegQueryValueEx(hkey, lpszFlag, NULL, &dwRegType, (LPBYTE)&dwData, &dwDataSize);
		RegCloseKey(hkey);
	}
	DebugPrint("Getting: %s=%#8.8x: dwRet=%#8.8x\n", lpszFlag, dwData, dwRet);
	return dwData;
}

void SetDwordToSetting(LPCTSTR lpszFlag, DWORD dwFlag)
{
	HKEY hkey;
	DWORD dwDataSize, dwRet;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_szRegInfoPath, 0, KEY_WRITE, &hkey)) {
		dwRet = RegSetValueEx(hkey, lpszFlag, 0, REG_DWORD, (CONST BYTE *)&dwFlag, sizeof(DWORD));
		RegCloseKey(hkey);
	}
	DebugPrint("Setting: %s=%#8.8x: dwRet=%#8.8x\n", lpszFlag, dwFlag, dwRet);
}

void PASCAL SetGlobalFlags()
{
    dwLogFlag = GetDwordFromSetting("LogFlag");
    dwDebugFlag = GetDwordFromSetting("DebugFlag");
}

void PASCAL ImeLog(DWORD dwFlag, LPSTR lpStr)
{
    char szBuf[80];

    if (dwFlag & dwLogFlag)
    {
        if (dwDebugFlag & DEBF_THREADID)
        {
            DWORD dwThreadId = GetCurrentThreadId();
            wsprintf(szBuf, "ThreadID = %X ", dwThreadId);
            OutputDebugString(szBuf);
        }

        OutputDebugString(lpStr);
        OutputDebugString("\r\n");
    }
}

#ifdef FAKEIMEM
void PASCAL MyOutputDebugStringW(LPWSTR lpw)
{
    DWORD dwSize = (lstrlenW(lpw) + 1) * 2;
    LPSTR lpStr;
    int n;
    
    lpStr = GlobalAlloc(GPTR, dwSize);

    if (!lpStr)
         return;


    n = WideCharToMultiByte(CP_ACP, 0, lpw, lstrlenW(lpw), lpStr, dwSize, NULL, NULL);

    *(lpStr + n) = '\0';

    OutputDebugString(lpStr);
    GlobalFree((HANDLE)lpStr);
}
#endif

#endif //DEBUG
