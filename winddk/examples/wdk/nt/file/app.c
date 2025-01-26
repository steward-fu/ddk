#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <strsafe.h>
#include <setupapi.h>
 
int main(int argc, char* argv[])
{
    DWORD dwRet = 0;
    char szBuffer[32] = { "I am error" };
    HANDLE hFile = INVALID_HANDLE_VALUE;
 
    hFile = CreateFile("\\\\.\\MyDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    WriteFile(hFile, szBuffer, strlen(szBuffer), &dwRet, NULL);
    printf("WR: %s, %d\n", szBuffer, dwRet);

    memset(szBuffer, 0, sizeof(szBuffer));
    ReadFile(hFile, szBuffer, sizeof(szBuffer), &dwRet, NULL);
    printf("RD: %s, %d\n", szBuffer, dwRet);
    CloseHandle(hFile);
    return 0;
}
