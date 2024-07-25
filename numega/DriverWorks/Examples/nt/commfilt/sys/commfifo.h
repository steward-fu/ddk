// commfifo.h - include file class CommDataFifo
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

// Class CommDataFifo is a fifo for comm input data passed from the
// filter device to the control device of the filter example driver.
// It is a subclass of KLockableFifo<RECORD>. 

// A feature of the class is that the constructor accepts the address of
// a function to be called when the fifo goes from empty to not empty. Public
// data member m_ControlContext can be written by clients, and this value
// is passed to the notification routine. This mechanism enables the control
// device to be notified when data is transferred, so that it can notify 
// the test application.

class CommDataFifo : public KLockableFifo<RECORD>
{
	SAFE_DESTRUCTORS
public:
	CommDataFifo(VOID (*DataAvailableCallback)(PVOID), ULONG FifoSize);
	ULONG ControlRead(RECORD* pData, ULONG nItems);
	ULONG ControlWrite(RECORD* pData, ULONG nItems);
	VOID Reset(void);

	VOID (*m_OnDataAvailable)(PVOID context);
	PVOID m_ControlContext;
};

////////////////////////////////////////////////////////////////////////
// Constructor
//
inline CommDataFifo::CommDataFifo(VOID (*DataAvailableCallback)(PVOID), ULONG FifoSize) :
	KLockableFifo<RECORD>(FifoSize, NonPagedPool)
{
	m_OnDataAvailable = DataAvailableCallback;
}

////////////////////////////////////////////////////////////////////////
// Write
//
// Write data to the fifo. Call the notification function if the fifo was
// empty at the time of the call.
//
inline ULONG CommDataFifo::ControlWrite(RECORD* pData, ULONG nItems)
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

////////////////////////////////////////////////////////////////////////
// Read
//
// Read data from the fifo
//
inline ULONG CommDataFifo::ControlRead(RECORD* pData, ULONG MaxItemsToRead)
{
	ULONG itemsRead;

	Lock();

	itemsRead = Read( pData, MaxItemsToRead);

	Unlock();

	return itemsRead;
}

////////////////////////////////////////////////////////////////////////
// Reset
//
// Flush the fifo.
//
inline VOID CommDataFifo::Reset(void)
{
	LockedFlush();	
}
