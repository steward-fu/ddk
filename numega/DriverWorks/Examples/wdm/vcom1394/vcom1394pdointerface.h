// VCom1394Pdoioctl.h
//
// Define control codes for VCOM1394 driver
//

#ifndef __VCOM1394PDOIOCTL__H_
#define __VCOM1394PDOIOCTL__H_

// This GUID identifies the device interface class used by the VCom1394Bus device
// {71390771-9D09-4fb1-A36C-D18D86A11D61}
DEFINE_GUID(GUID_DEVINTERFACE_VCOM1394PDO, 0x71390771, 0x9d09, 0x4fb1, 0xa3, 0x6c, 0xd1, 0x8d, 0x86, 0xa1, 0x1d, 0x61);

#define VCOM1394PDO_IOCTL_GET_PORT_LIST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define VCOM1394PDO_IOCTL_SELECT_PORT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define VCOM1394PDO_IOCTL_GET_CONNECTED_PORT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

// VCOM1394 device register definitions
typedef struct _VCOM_PORT
{
    ULONG           NodeNumber;            // 0x00 - node number
    ULONG           MachineName[33];       // 0X01 - machine name
    ULONG           PortName[4];           // 0X23 - port name
    ADDRESS_OFFSET  PortAddress;           // 0x26 - register address port
} VCOM_PORT, *PVCOM_PORT;

// peer node list struct
typedef struct _VCOM_PORT_LIST {
    ULONG       NumberOfPorts;
    VCOM_PORT   Port[MAX_LOCAL_NODES * MAXIMUM_NUMBER_OF_PORTS];
} VCOM_PORT_LIST, *PVCOM_PORT_LIST;


#endif // __VCOM1394BUSIOCTL__H_

