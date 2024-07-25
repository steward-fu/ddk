//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#ifndef __SOCKETS_H__
#define __SOCKETS_H__

class VDisk;
class VScsiDevice;

//////////////////////////////////////////////////////////////////////////////
// iSocket class 
//		helper class to send/recv network packets
//
class iSocket : public KStreamSocket 
{
	// constructors
public:
	SAFE_DESTRUCTORS;
    iSocket(VDisk* pDevice) : m_pDevice(pDevice) {}
    ~iSocket() { disconnect(); }

	// Methods
	void Write(ULONG block, ULONG length, PVOID buffer);
	void Read(ULONG block, ULONG length, PVOID buffer);

	virtual void OnDisconnect(uint OptionsLength, PVOID Options, BOOLEAN bAbort);
	virtual uint OnReceive(uint Indicated, uchar *Data, uint Available, uchar **RcvBuffer, uint* RcvBufferLen);
	virtual void OnReceiveComplete(TDI_STATUS Status, uint Indicated, uchar *Data);
	virtual void On_sendComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount);
	virtual void On_connectComplete(PVOID, TDI_STATUS, uint);

protected:
	VDisk*			m_pDevice;			// pointer to disk device

	ULONG			m_nBytes;			// number of bytes received so far
	ULONG			m_length;			// number of bytes to read to get the whole packet
	PUCHAR			m_buffer;			// buffer to read data to or write data from
};

//////////////////////////////////////////////////////////////////////////////
// ScanSocket class 
//		socket to scan for available network disks
//
class ScanSocket : public KDatagramSocket
{
	// constructors
public:
	SAFE_DESTRUCTORS;
	ScanSocket(VScsiDevice* pDevice) : m_pDevice(pDevice) {}
	~ScanSocket(){}

	VOID Scan(ULONG address);

	virtual uint OnReceive(
		uint AddressLength,
		PTRANSPORT_ADDRESS pTA,
		uint OptionsLength,
		PVOID Options,
		uint Indicated,
		uchar* Data,
		uint Available,
		uchar** RcvBuffer,
		uint* RcvBufferLen);

	virtual void On_sendtoComplete(
		PVOID pCxt, 
		TDI_STATUS Status, 
		uint ByteCount);

protected:
	VScsiDevice*		m_pDevice;
};


#endif // __SOCKETS_H__
