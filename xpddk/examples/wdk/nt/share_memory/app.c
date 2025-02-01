#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winioctl.h>
#include <strsafe.h>
#include <setupapi.h>
 
#define IOCTL_MAP_PTR   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_UNMAP_PTR CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
 
int main(int argc, char *argv[])
{
    DWORD dwRet = 0;
    HANDLE hFile = NULL;
    unsigned long dwTmp = 0;
    unsigned char *u8Ptr = NULL;
 
    hFile = CreateFile("\\\\.\\MyDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    DeviceIoControl(hFile, IOCTL_MAP_PTR, NULL, 0, &dwTmp, sizeof(dwTmp), &dwRet, NULL);
    u8Ptr = (unsigned char *)dwTmp;
    printf("u8Ptr:0x%x, value:%d\n", u8Ptr, u8Ptr[0]);
    Sleep(10000);
    DeviceIoControl(hFile, IOCTL_UNMAP_PTR, &dwTmp, sizeof(dwTmp), NULL, 0, &dwRet, NULL);
    CloseHandle(hFile);
    return 0;
}
