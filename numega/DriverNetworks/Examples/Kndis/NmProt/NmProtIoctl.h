// NmProtIoctl.h
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
// Define control codes for NmProt driver
//

#ifndef __NmProtIoctl__h_
#define __NmProtIoctl__h_


#define FSCTL_NDISPROT_BASE      FILE_DEVICE_NETWORK

#define _NDISPROT_CTL_CODE(_Function, _Method, _Access)  \
			CTL_CODE(FSCTL_NDISPROT_BASE, _Function, _Method, _Access)

#define IOCTL_NDISPROT_OPEN_DEVICE   \
			_NDISPROT_CTL_CODE(0x200, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISPROT_QUERY_OID_VALUE   \
			_NDISPROT_CTL_CODE(0x201, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISPROT_SET_OID_VALUE   \
			_NDISPROT_CTL_CODE(0x205, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISPROT_QUERY_BINDING   \
			_NDISPROT_CTL_CODE(0x203, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISPROT_BIND_WAIT   \
			_NDISPROT_CTL_CODE(0x204, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISPROT_INDICATE_STATUS   \
			_NDISPROT_CTL_CODE(0x206, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)



//
//  Structure to go with IOCTL_NDISPROT_QUERY_OID_VALUE.
//  The Data part is of variable length, determined by
//  the input buffer length passed to DeviceIoControl.
//
typedef struct _NDISPROT_QUERY_OID
{
	NDIS_OID        Oid;
	UCHAR           Data[sizeof(ULONG)];

} NDISPROT_QUERY_OID, *PNDISPROT_QUERY_OID;

//
//  Structure to go with IOCTL_NDISPROT_SET_OID_VALUE.
//  The Data part is of variable length, determined
//  by the input buffer length passed to DeviceIoControl.
//
typedef struct _NDISPROT_SET_OID
{
	NDIS_OID        Oid;
	UCHAR           Data[sizeof(ULONG)];

} NDISPROT_SET_OID, *PNDISPROT_SET_OID;


//
//  Structure to go with IOCTL_NDISPROT_QUERY_BINDING.
//  The input parameter is BindingIndex, which is the
//  index into the list of bindings active at the driver.
//  On successful completion, we get back a device name
//  and a device descriptor (friendly name).
//
typedef struct _NDISPROT_QUERY_BINDING
{
	ULONG            BindingIndex;		// 0-based binding number
	ULONG            DeviceNameOffset;	// from start of this struct
	ULONG            DeviceNameLength;	// in bytes
	ULONG            DeviceDescrOffset;	// from start of this struct
	ULONG            DeviceDescrLength;	// in bytes

} NDISPROT_QUERY_BINDING, *PNDISPROT_QUERY_BINDING;

//
//  Structure to go with IOCTL_NDISPROT_INDICATE_STATUS.
//  NDISPROT copies the status indicated by the NIC and
//  also the data indicated in the StatusBuffer.
//
typedef struct _NDISPROT_INDICATE_STATUS
{
	ULONG            IndicatedStatus;	// NDIS_STATUS
	ULONG            StatusBufferOffset;// from start of this struct
	ULONG            StatusBufferLength;// in bytes
} NDISPROT_INDICATE_STATUS, *PNDISPROT_INDICATE_STATUS;




#define NUIOO_PACKET_FILTER			(NDIS_PACKET_TYPE_DIRECTED|	\
									NDIS_PACKET_TYPE_MULTICAST|	\
									NDIS_PACKET_TYPE_BROADCAST|	\
									NDIS_PACKET_TYPE_PROMISCUOUS)



//
//  Definitions for Flags above.
//
#define NUIOO_BIND_IDLE             0x00000000
#define NUIOO_BIND_OPENING          0x00000001
#define NUIOO_BIND_FAILED           0x00000002
#define NUIOO_BIND_ACTIVE           0x00000004
#define NUIOO_BIND_CLOSING          0x00000008
#define NUIOO_BIND_FLAGS            0x0000000F  // State of the binding

#define NUIOO_OPEN_IDLE             0x00000000
#define NUIOO_OPEN_ACTIVE           0x00000010
#define NUIOO_OPEN_FLAGS            0x000000F0  // State of the I/O open

#define NUIOO_RESET_IN_PROGRESS     0x00000100
#define NUIOO_NOT_RESETTING         0x00000000
#define NUIOO_RESET_FLAGS           0x00000100

#define NUIOO_MEDIA_CONNECTED       0x00000000
#define NUIOO_MEDIA_DISCONNECTED    0x00000200
#define NUIOO_MEDIA_FLAGS           0x00000200

#define NUIOO_READ_SERVICING        0x00100000  // Is the read service
												// routine running?
#define NUIOO_READ_FLAGS            0x00100000

#define NUIOO_UNBIND_RECEIVED       0x10000000  // Seen NDIS Unbind?
#define NUIOO_UNBIND_FLAGS          0x10000000

//
//  Flags
//
#define NPROT_SET_FLAGS(_FlagsVar, _Mask, _BitsToSet)    \
		(_FlagsVar) = ((_FlagsVar) & ~(_Mask)) | (_BitsToSet)

#define NPROT_TEST_FLAGS(_FlagsVar, _Mask, _BitsToCheck)    \
		(((_FlagsVar) & (_Mask)) == (_BitsToCheck))







#define NDIS_STATUS_TO_NT_STATUS(_NdisStatus, _pNtStatus)                           \
{                                                                                   \
	/*                                                                              \
	 *  The following NDIS status codes map directly to NT status codes.            \
	 */                                                                             \
	if (((NDIS_STATUS_SUCCESS == (_NdisStatus)) ||                                  \
		(NDIS_STATUS_PENDING == (_NdisStatus)) ||                                   \
		(NDIS_STATUS_BUFFER_OVERFLOW == (_NdisStatus)) ||                           \
		(NDIS_STATUS_FAILURE == (_NdisStatus)) ||                                   \
		(NDIS_STATUS_RESOURCES == (_NdisStatus)) ||                                 \
		(NDIS_STATUS_NOT_SUPPORTED == (_NdisStatus))))                              \
	{                                                                               \
		*(_pNtStatus) = (NTSTATUS)(_NdisStatus);                                    \
	}                                                                               \
	else if (NDIS_STATUS_BUFFER_TOO_SHORT == (_NdisStatus))                         \
	{                                                                               \
		/*                                                                          \
		 *  The above NDIS status codes require a little special casing.            \
		 */                                                                         \
		*(_pNtStatus) = STATUS_BUFFER_TOO_SMALL;                                    \
	}                                                                               \
	else if (NDIS_STATUS_INVALID_LENGTH == (_NdisStatus))                           \
	{                                                                               \
		*(_pNtStatus) = STATUS_INVALID_BUFFER_SIZE;                                 \
	}                                                                               \
	else if (NDIS_STATUS_INVALID_DATA == (_NdisStatus))                             \
	{                                                                               \
		*(_pNtStatus) = STATUS_INVALID_PARAMETER;                                   \
	}                                                                               \
	else if (NDIS_STATUS_ADAPTER_NOT_FOUND == (_NdisStatus))                        \
	{                                                                               \
		*(_pNtStatus) = STATUS_NO_MORE_ENTRIES;                                     \
	}                                                                               \
	else if (NDIS_STATUS_ADAPTER_NOT_READY == (_NdisStatus))                        \
	{                                                                               \
		*(_pNtStatus) = STATUS_DEVICE_NOT_READY;                                    \
	}                                                                               \
	else                                                                            \
	{                                                                               \
		*(_pNtStatus) = STATUS_UNSUCCESSFUL;                                        \
	}                                                                               \
}

#endif	//__NmProtIoctl__h_
