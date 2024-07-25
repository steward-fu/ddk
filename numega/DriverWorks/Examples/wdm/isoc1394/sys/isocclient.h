// IsocClient.h
// Definition of Isoc1394Transfer class
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998-2003 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//


#ifndef __Isoc1394Transfer_h__
#define __Isoc1394Transfer_h__


// Forward declaration
class Isoc1394Device;


class Isoc1394Transfer : public K1394IsochTransfer
{
public:
	Isoc1394Transfer() {}
	~Isoc1394Transfer();

	VOID OnStartComplete(NTSTATUS Status);
	VOID OnBufferComplete(
		NTSTATUS				Status, 
		K1394IsochBufferList*	pBufferList
		);

	NTSTATUS Initialize(
		BOOLEAN				bWrite, 
		K1394LowerDevice*	plowerDevice, 
		Isoc1394Device*		pDevice,
		ULONG				nSpeed
		);

	NTSTATUS BufferStatus() { return m_BufferStatus; }


protected:
	K1394LowerDevice *		m_pLowerDevice;
	Isoc1394Device *		m_pDevice;
	K1394IsochResource		m_Resource;
	NTSTATUS				m_BufferStatus;

	KSpinLock				m_BufferSpinlock;
	ULONG					m_BuffersAttached;

	friend class Isoc1394Device;
};


#endif // __Isoc1394Transfer_h__