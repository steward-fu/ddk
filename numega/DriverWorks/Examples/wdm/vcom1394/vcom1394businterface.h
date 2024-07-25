// VCom1394BusInterface.h - device interface classes for VCOM1394

// This GUID identifies the device interface class used by the VCom1394Bus device
// {786BBA18-A2EF-430F-924D-E360A9756EC3}
DEFINE_GUID(GUID_DEVINTERFACE_VCOM1394BUS, 0x786bba18, 0xa2ef, 0x430f, 0x92, 0x4d, 0xe3, 0x60, 0xa9, 0x75, 0x6e, 0xc3);

// This GUID identifies the bus type to the PnP manager
// {095089A4-AE1E-4ba4-9377-A9B4838B4528}
DEFINE_GUID(VCOM1394BUS_GUID, 
          0x095089a4, 0xae1e, 0x4ba4, 0x93, 0x77, 0xa9, 0xb4, 0x83, 0x8b, 0x45, 0x28);

#define VCOM1394BUS_IOCTL_GET_PORTS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#if !defined(MAX_LOCAL_NODES)
#define MAX_LOCAL_NODES 64
#endif // MAX_LOCAL_NODES

// Min and max number of virtual COM ports
CONST ULONG MINIMUM_NUMBER_OF_PORTS = 1;
CONST ULONG MAXIMUM_NUMBER_OF_PORTS = 10;

typedef struct _VCOM1394BUS_PORT_INFORMATION
{
    ULONG   MaximumNumberOfPorts;
    ULONG   MinimumNumberOfPorts;
    ULONG   CurrentNumberOfPorts;
} VCOM1394_PORT_INFORMATION, *PVCOM1394_PORT_INFORMATION;
