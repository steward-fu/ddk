#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
  
#define IOCTL_TEST      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED,   FILE_ANY_ACCESS)
#define IOCTL_GET_BUF   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED,   FILE_ANY_ACCESS)
#define IOCTL_SET_BUF   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED,   FILE_ANY_ACCESS)
#define IOCTL_GET_DIR   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_SET_DIR   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_IN_DIRECT,  FILE_ANY_ACCESS)
#define IOCTL_GET_NEI   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_NEITHER,    FILE_ANY_ACCESS)
#define IOCTL_SET_NEI   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_NEITHER,    FILE_ANY_ACCESS)
 
int main(int argc, char **argv)
{
    DWORD dwRet = 0;
    HANDLE hFile = NULL;
    char szBuffer[255] = { "I am error" };
  
    hFile = CreateFile("\\\\.\\MyDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
 
    if (argc > 1) {
        dwRet = strlen(szBuffer);
        printf("SET: %s, %d\n", szBuffer, dwRet);
        if (!strcmp(argv[1], "BUF")) {
            DeviceIoControl(hFile, IOCTL_SET_BUF, szBuffer, dwRet, NULL, 0, &dwRet, NULL);
             
            dwRet = 0;
            memset(szBuffer, 0, sizeof(szBuffer));
            DeviceIoControl(hFile, IOCTL_GET_BUF, NULL, 0, szBuffer, sizeof(szBuffer), &dwRet, NULL);
        }
        else if (!strcmp(argv[1], "DIR")) {
            DeviceIoControl(hFile, IOCTL_SET_DIR, szBuffer, dwRet, NULL, 0, &dwRet, NULL);
             
            dwRet = 0;
            memset(szBuffer, 0, sizeof(szBuffer));
            DeviceIoControl(hFile, IOCTL_GET_DIR, NULL, 0, szBuffer, sizeof(szBuffer), &dwRet, NULL);
        }
        else if (!strcmp(argv[1], "NEI")) {
            DeviceIoControl(hFile, IOCTL_SET_NEI, szBuffer, dwRet, NULL, 0, &dwRet, NULL);
             
            dwRet = 0;
            memset(szBuffer, 0, sizeof(szBuffer));
            DeviceIoControl(hFile, IOCTL_GET_NEI, NULL, 0, szBuffer, sizeof(szBuffer), &dwRet, NULL);
        }
        printf("GET: %s, %d\n", szBuffer, dwRet);
    }
    else {
        DeviceIoControl(hFile, IOCTL_TEST, NULL, 0, NULL, 0, &dwRet, NULL);    
    }
    CloseHandle(hFile);
    return 0;
}
