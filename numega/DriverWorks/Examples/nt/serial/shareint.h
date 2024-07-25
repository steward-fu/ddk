// shareint.h - include file for class SharedInterrupt
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

#ifndef __SHAREINT__
#define __SHAREINT__

// Class Uart allows sharing of interrupts, and so needs a way to 
// manage interrupt objects. Each instance of class SharedInterrupt 
// references an interrupt object and holds a list of Uart devices
// that are sharing that interrupt.
//
// Class Uart first calls static member function LookUp to determine
// if the interrupt of a specified vector is already in the list. If
// so, it adds itself to the sharer list of the returned SharedInterrupt
// by calling member function AddSharer.
//
// If not in the list, class Uart creates a new KInterrupt object and
// uses it to construct a new SharedInterrupt.
//
// The destructor for Uart removes itself from the sharer list of its
// interrupt, and deletes the shared interrupt if there are no more
// sharers.
//
// It is the job of the driver to initialize the list of all shared
// interrupts. It must allocate a new KList<SharedInterrupt> and set
// SharedInterrupt::m_List to the address of the allocated list.
//

class Uart;	 // forward

/////////////////////////////////////////////////////////////////////////


class SharedInterrupt
{
public:
	SharedInterrupt(KInterrupt* pIntr);
	~SharedInterrupt(void);
	VOID AddSharer(Uart* Sharer);
	VOID RemoveSharer(Uart* Sharer);

	static SharedInterrupt* LookUp(ULONG Vector);

	LIST_ENTRY				m_ListEntry;
	KInterrupt*				m_Interrupt;
	KInterruptSafeList<Uart> m_Sharers;
	ULONG					m_Vector;
	ULONG					m_ActiveCount;

// The driver must create the list and set the pointer before using 
// any members.
	static KList<SharedInterrupt>* m_List;
};

/////////////////////////////////////////////////////////////////////////
// Constructor
//
inline SharedInterrupt::SharedInterrupt(KInterrupt* pIntr) :
	m_Sharers(pIntr)
{
	m_Vector = pIntr->BusVector();
	m_Interrupt = pIntr;
	m_List->InsertHead(this);
	m_ActiveCount = 0;
}

/////////////////////////////////////////////////////////////////////////
// LookUp 
//
// Searches the list of shared interrupts for one with a given vector
//
inline SharedInterrupt* SharedInterrupt::LookUp(ULONG Vector)
{
	SharedInterrupt* p = m_List->Head();
	while (p != NULL)
		if ( p->m_Vector == Vector)
			return p;
		else
			p = m_List->Next(p);

	return NULL;
}

/////////////////////////////////////////////////////////////////////////
// Destructor
//
inline SharedInterrupt::~SharedInterrupt(void)
{
	delete m_Interrupt;
}

/////////////////////////////////////////////////////////////////////////
// Add a sharing Uart to the list 
//
inline VOID SharedInterrupt::AddSharer(Uart* Sharer)
{
	m_Sharers.InsertHead(Sharer);
}

/////////////////////////////////////////////////////////////////////////
// Remove a sharing Uart from the list
//
inline VOID SharedInterrupt::RemoveSharer(Uart* Sharer)
{
	m_Sharers.Remove(Sharer);
}

#endif // __SHAREINT__
