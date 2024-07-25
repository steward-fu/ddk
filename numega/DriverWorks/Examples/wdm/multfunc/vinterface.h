// vinterface.h - device interface classes for Vbus

// This GUID identifies the device interface class used by the VbusDevice device

// TODO:	If your driver supports a standard interface, use the GUID that identifies
//			the interface class, rather than using the one defined below

//
DEFINE_GUID(VBUSDEVICE_CLASS_GUID, 
0x35b4db96, 0xc49, 0x4c3e, 0x91, 0xdf, 0xf7, 0x81, 0x9f, 0x12, 0x7f, 0x35);

// {3B10FA59-8397-4619-8999-BF0D5D5310D0}
DEFINE_GUID(GUID_VIRTUAL_BUS_TYPE, 
0x3b10fa59, 0x8397, 0x4619, 0x89, 0x99, 0xbf, 0xd, 0x5d, 0x53, 0x10, 0xd0);

// {72BF5222-F2F3-45bd-ADD0-5BA2C864FCCE}
DEFINE_GUID(GUID_VBUS_READ_INTERFACE, 
0x72bf5222, 0xf2f3, 0x45bd, 0xad, 0xd0, 0x5b, 0xa2, 0xc8, 0x64, 0xfc, 0xce);

// {16A815A0-3A9B-4ac1-B0D9-D7531DC5B5D3}
DEFINE_GUID(GUID_VBUS_WRITE_INTERFACE, 
0x16a815a0, 0x3a9b, 0x4ac1, 0xb0, 0xd9, 0xd7, 0x53, 0x1d, 0xc5, 0xb5, 0xd3);

// {C0B2591B-5D6D-480c-8904-7197CEF1216C}
DEFINE_GUID(VREADDEVICE_CLASS_GUID, 
0xc0b2591b, 0x5d6d, 0x480c, 0x89, 0x4, 0x71, 0x97, 0xce, 0xf1, 0x21, 0x6c);

// {408ED468-F712-4178-AA6E-9BCD359A20D4}
DEFINE_GUID(VWRITEDEVICE_CLASS_GUID, 
0x408ed468, 0xf712, 0x4178, 0xaa, 0x6e, 0x9b, 0xcd, 0x35, 0x9a, 0x20, 0xd4);


#ifndef __VINTERFACE_H__
#define __VINTERFACE_H__

typedef VOID (__stdcall *PVBUS_ADDREF)(PVOID Context);
typedef VOID (__stdcall *PVBUS_RELEASE)(PVOID Context);

typedef BOOLEAN (__stdcall *PVBUS_READ_INTERFACE)(ULONG Offset, PVOID Buffer, ULONG Size, PULONG pRead);
typedef BOOLEAN (__stdcall *PVBUS_WRITE_INTERFACE)(ULONG Offset, PVOID Buffer, ULONG Size, PULONG pWritten);

struct _VBUS_COMMON_INTERFACE
{
	USHORT				Size;
	USHORT				Version;

	PVOID				Context;

	PVBUS_ADDREF		AddRef;
	PVBUS_RELEASE		Release;
};

#ifdef __cplusplus
#define DERIVE_FROM(_struct_) : _struct_ {
#else
#define DERIVE_FROM(_struct_) { struct _struct_;
#endif

struct _VBUS_READ_INTERFACE DERIVE_FROM(_VBUS_COMMON_INTERFACE)

	PVBUS_READ_INTERFACE	Read;
};

struct _VBUS_WRITE_INTERFACE DERIVE_FROM(_VBUS_COMMON_INTERFACE)

	PVBUS_WRITE_INTERFACE	Write;
};




#endif // __VINTERFACE_H__