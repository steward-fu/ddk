#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winioctl.h>
#include <strsafe.h>
#include <setupapi.h>
 
int main(int argc, char* argv[])
{
    DWORD len = 0;
    DWORD dwRet = 0;
    HANDLE hFile = NULL;
    char szBuffer[255] = { "I am error" };
    
    hFile = CreateFile("\\\\.\\MyDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
 
    len = strlen(szBuffer);
    printf("WR: %s\n", szBuffer);
    printf("Length: %d\n", len);
    WriteFile(hFile, szBuffer, len, &dwRet, NULL);
 
    memset(szBuffer, 0, sizeof(szBuffer));
    ReadFile(hFile, szBuffer, sizeof(szBuffer), &dwRet, NULL);
    printf("RD: %s\n", szBuffer);
    printf("Length: %d\n", dwRet);
    CloseHandle(hFile);
    return 0;
}
