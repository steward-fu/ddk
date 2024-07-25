// VirtualAddRemove.cpp
//
// This console application demonstrates how to open create/remove a 
// virtual 1394 device on XP and later platforms.
//

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <winioctl.h>
#include <ntdd1394.h>

#include "..\VCOM1394BusInterface.h"


#define MAX_STR_LEN 1024

// This function is found in module OpenByIntf.cpp
//HANDLE OpenByInterface(GUID* pClassGuid, DWORD instance, PDWORD pError);

typedef void VOIDFUNC();

// Prototypes
void Usage(void);
void ShowIoctlValues(void);

//  TODO:
//      You can redefine the IOCTL handler prototypes as needed, adding
//      appropriate parameters that can be collected from the command line.
//      To do this you must modify the command line parsing logic.  An
//      example of this is shown in comments throughout the test application.
//
//=== Parameterized IOCTL Example ===
// void Test_IOCTL_PARAMETERIZED(int nVal, ULONG dwVal);
BOOL CreateV1394Device(ULONG Index);
BOOL DestroyV1394Device(ULONG Index);


void CloseIfOpen(void);

// Global data


#define N_IOCODES   1

// Names of IOCTL codes
//
char *IOnames[N_IOCODES+1] =
{

//=== Parameterized IOCTL Example ===
//  "IOCTL_PARAMETERIZED",
    ""
};

// IOCTL codes
//
int IOcodes[N_IOCODES+1] =
{

//=== Parameterized IOCTL Example ===
//  IOCTL_PARAMETERIZED,
    0
};


// Handle to device opened in driver.
//
HANDLE  hDevice = INVALID_HANDLE_VALUE;


////////////////////////////////////////////////////////////////////////
// Exit
//
//      Print a message and exit
//
   void Exit(int res)
{
    printf("Exiting...\n\n");
    CloseIfOpen();
    exit(res);
}


////////////////////////////////////////////////////////////////////////
// Main entry point
//
//
int __cdecl main(int argc, char *argv[])
{
    int     nArgIndex;              // Walk through command line arguments
    int     nArgIncrement = 0;
    int     val;

//=== Parameterized IOCTL Example ===

    printf("Application VirtualAddRemove starting....\n");

    // Parse the command line

    if (argc < 2) Usage();

    nArgIndex = 1;
    while (nArgIndex < argc)
    {
        // Parse ahead to determine numeric value of argument

        if (nArgIndex+1 >= argc) Usage();
        if (!isdigit((UCHAR)argv[nArgIndex+1][0])) Usage();
        val = atoi(argv[nArgIndex+1]);

        switch (argv[nArgIndex][0])
        {
            case 'c':
            case 'C':
                if(!CreateV1394Device(val)) {
                    Exit(1);
                }
                Exit(0);
                
            case 'd':
            case 'D':
                if(!DestroyV1394Device(val)) {
                    Exit(1);
                }
                Exit(0);

            case '?':
            case 'h':
            default:
                Usage();
        }
        nArgIndex += nArgIncrement;
    }

    return 0;
}


////////////////////////////////////////////////////////////////////////
// CloseIfOpen
//
//      Close the device if we previously opened a handle to it.
//
void CloseIfOpen(void)
{
    if (hDevice != INVALID_HANDLE_VALUE)
    {
        // Close the handle to the driver
        if (!CloseHandle(hDevice))
        {
            printf("ERROR: CloseHandle returns %0x.\n", GetLastError());
        }
        hDevice = INVALID_HANDLE_VALUE;
    }
}


////////////////////////////////////////////////////////////////////////
// CreateV1394Device
//
//      Virtual 1394 device creation routine
//
//
//
BOOL CreateV1394Device(ULONG Index)
{
    CHAR                        busName[MAX_STR_LEN];
    PIEEE1394_API_REQUEST       p1394ApiReq;
    PIEEE1394_VDEV_PNP_REQUEST  pDevPnpReq;
    CHAR                        deviceId[] = "VCOM1394_BUS_DEVICE";
    ULONG                       length; 

    printf("Creating 1394 Virtual Device on bus %d\n", Index);

    sprintf(busName, "\\\\.\\1394BUS%d", Index);
    
    hDevice = CreateFile(busName,
                         GENERIC_WRITE | GENERIC_READ,
                         FILE_SHARE_WRITE | FILE_SHARE_READ,
                         NULL,
                         OPEN_EXISTING,
                         0,
                         NULL);

    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("Failed to open handle to 1394 bus device, (%d)\n", GetLastError());

        return FALSE;
    }

    p1394ApiReq = 
        (PIEEE1394_API_REQUEST)LocalAlloc(LPTR, 
                                          sizeof(IEEE1394_API_REQUEST) + 
                                            strlen((PCHAR)deviceId));

    p1394ApiReq->RequestNumber = IEEE1394_API_ADD_VIRTUAL_DEVICE;
    // for device to be persistent across reboots
    p1394ApiReq->Flags |= IEEE1394_REQUEST_FLAG_PERSISTENT;
    // For the virtual device to use the host controller's unique
    // instance ID you can use the following commented out flag.  
    // If used here it must be used in RemoveV1394Device as well.
    //p1394ApiReq->Flags |= IEEE1394_REQUEST_FLAG_USE_LOCAL_HOST_EUI;

    pDevPnpReq = &p1394ApiReq->u.AddVirtualDevice;

    pDevPnpReq->fulFlags = 0;
    pDevPnpReq->Reserved = 0;
    pDevPnpReq->InstanceId.LowPart = Index;
    pDevPnpReq->InstanceId.HighPart = 0;

    strncpy((PCHAR)&pDevPnpReq->DeviceId, (PCHAR)deviceId, strlen((PCHAR)deviceId));

    if (!DeviceIoControl(hDevice,
                         IOCTL_IEEE1394_API_REQUEST,
                         p1394ApiReq,
                         sizeof(IEEE1394_API_REQUEST) + (DWORD)strlen((PCHAR)deviceId),
                         NULL,
                         0,
                         &length,
                         NULL)) {
        printf("IOCTL_IEEE1394_API_REQUEST failed:  %d\n", GetLastError());
        return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
// DestroyV1394Device
//
//      Virtual 1394 device removal routine
//
//
//
BOOL DestroyV1394Device(ULONG Index)
{
    CHAR                        busName[MAX_STR_LEN];
    PIEEE1394_API_REQUEST       p1394ApiReq;
    PIEEE1394_VDEV_PNP_REQUEST  pDevPnpReq;
    CHAR                        deviceId[] = "VCOM1394_BUS_DEVICE";
    ULONG                       length; 


    sprintf(busName, "\\\\.\\1394BUS%d", Index);

    hDevice = CreateFile(busName,
                         GENERIC_WRITE | GENERIC_READ,
                         FILE_SHARE_WRITE | FILE_SHARE_READ,
                         NULL,
                         OPEN_EXISTING,
                         0,
                         NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    p1394ApiReq = 
        (PIEEE1394_API_REQUEST)LocalAlloc(LPTR, 
                                          sizeof(IEEE1394_API_REQUEST) + 
                                            strlen((PCHAR)deviceId));

    p1394ApiReq->RequestNumber = IEEE1394_API_REMOVE_VIRTUAL_DEVICE;

    pDevPnpReq = &p1394ApiReq->u.RemoveVirtualDevice;

    pDevPnpReq->fulFlags = 0;
    pDevPnpReq->Reserved = 0;
    pDevPnpReq->InstanceId.LowPart = Index;
    pDevPnpReq->InstanceId.HighPart = 0;

    strncpy((PCHAR)&pDevPnpReq->DeviceId, (PCHAR)deviceId, strlen((PCHAR)deviceId));
    
    if (!DeviceIoControl(hDevice,
                         IOCTL_IEEE1394_API_REQUEST,
                         p1394ApiReq,
                         sizeof(IEEE1394_API_REQUEST) + (DWORD)strlen((PCHAR)deviceId),
                         NULL,
                         0,
                         &length,
                         NULL)) {

        printf("IOCTL_IEEE1394_API_REQUEST failed:  %d\n", GetLastError());
        return FALSE;
    }
    return TRUE;
}


////////////////////////////////////////////////////////////////////////
// Usage
//
//      Print a usage message describing arguments to this program
//
void Usage(void)
{
    printf("Usage: VirtualAddRemove [c n] [d n] [i n]\n");
    printf("       i initiates an IO Control Code message with specified index value\n");
    ShowIoctlValues();
    printf("Example:\n");
    printf("    VirtualAddRemove c 0\n");
    printf("        create 1394 virtual device on 1394 bus zero\n");

    Exit(1);
}



#define IOCTL_INBUF_SIZE    512
#define IOCTL_OUTBUF_SIZE   512

//=== Parameterized IOCTL Example ===
//void Test_IOCTL_PARAMETERIZED(int nVal, ULONG dwVal)
//{
//  Function body same as other IOCTL handlers, with command line
//  parameters 'nVal' and 'dwVal' available as input.
//}

////////////////////////////////////////////////////////////////////////
// ShowIoctlValues
//
//      Print list of IO Control Code values for usage display
//
void ShowIoctlValues(void)
{
    int i;

    for (i=0; i<N_IOCODES; i++)
    {
        if (i==0)
            printf( "         IO control code index\n");
        printf( "           %d is code %s [%x]\n", i, IOnames[i], IOcodes[i]);

//=== Parameterized IOCTL Example ===
//      if (IOcodes[i] == IOCTL_PARAMETERIZED)
//      {
//          printf( "               and has two arguments: <arg1 desc.> <arg1 desc.>\n");
//          printf( "               Example: i %d <IOCTL index> <ex. arg1> <ex. arg2>\n", i);
//      }
    }
}
