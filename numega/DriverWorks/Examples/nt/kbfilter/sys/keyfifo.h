// keyfifo.h - include file class KeyDataFifo
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

// Class KeyDataFifo buffers keyboard data from the filter device,
// prior to begin read by the test application via the control device.
// The control device exposes the FIFO to the filter device, and 
// the filter device uses member function Write to put keyboard data 
// into the FIFO. Member function Write notices if the FIFO was
// empty at the time the write was done, and, if so, calls the
// function whose address is stored in data member m_OnDataAvailable.
// This pointer is set up by the constructor. The caller of the 
// constructor provides an entry point for notification of when there
// is data to be read. This enables the control device to notify
// the application that it should perform a read to collect the
// data that is in the FIFO.


/////////////////////////////////////////////////////////////////////////////
// Class definition
//
class KeyDataFifo : public KLockableFifo<KEYBOARD_INPUT_DATA> 
{
	SAFE_DESTRUCTORS
public:
	KeyDataFifo(VOID (*DataAvailableCallback)(PVOID), ULONG FifoSize);
	ULONG ControlRead(KEYBOARD_INPUT_DATA* pData, ULONG nItems);
	ULONG ControlWrite(KEYBOARD_INPUT_DATA* pData, ULONG nItems);
	VOID Reset(void);

	VOID (*m_OnDataAvailable)(PVOID context);
	PVOID m_ControlContext;
};

/////////////////////////////////////////////////////////////////////////////
// Constructor
inline KeyDataFifo::KeyDataFifo(
	VOID (*DataAvailableCallback)(PVOID),	// addr of notification function
	ULONG FifoSize							// size of FIFO to allocate
	) :

	KLockableFifo<KEYBOARD_INPUT_DATA>(FifoSize, NonPagedPool)
{
	m_OnDataAvailable = DataAvailableCallback;
}


/////////////////////////////////////////////////////////////////////////////
// ControlWrite
//
// Writes data into the FIFO. If the FIFO was empty at entry, then the
// notification function is called.
//
inline ULONG KeyDataFifo::ControlWrite(KEYBOARD_INPUT_DATA* pData, ULONG nItems)
{
	ULONG count;
	Lock();

	BOOLEAN bEmpty = IsEmpty();

	count = Write(pData, nItems);

	if (bEmpty && (count != 0))
		m_OnDataAvailable(m_ControlContext);

	Unlock();

	return count;
}

/////////////////////////////////////////////////////////////////////////////
// ControlRead
//
// Reads data from the FIFO
inline ULONG KeyDataFifo::ControlRead(KEYBOARD_INPUT_DATA* pData, ULONG MaxItemsToRead)
{
	ULONG itemsRead;

	Lock();

	itemsRead = Read( pData, MaxItemsToRead);

	Unlock();

	return itemsRead;
}

/////////////////////////////////////////////////////////////////////////////
// Reset
//
inline VOID KeyDataFifo::Reset(void)
{
	LockedFlush();	
}
