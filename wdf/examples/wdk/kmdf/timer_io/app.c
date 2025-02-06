#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
  
#define IOCTL_START CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STOP  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
  
int main(int argc, char **argv)
{
    DWORD dwRet = 0;
    HANDLE hFile = NULL;
  
    hFile = CreateFile("\\\\.\\MyDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    DeviceIoControl(hFile, IOCTL_START, NULL, 0, NULL, 0, &dwRet, NULL);
    Sleep(3000);
    DeviceIoControl(hFile, IOCTL_STOP, NULL, 0, NULL, 0, &dwRet, NULL);
    CloseHandle(hFile);
    return 0;
}
