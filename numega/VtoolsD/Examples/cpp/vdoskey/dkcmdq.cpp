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

// dkcmdq.cpp - implementation of class DKCommandQueue
//
// Copyright (c) 1994, Compuware Corporation  All Rights Reserved

#include "vdoskey.h"

//////////////////////////////////////////////////////////////////////////
#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandQueue::DKCommandQueue
//	Constructor for DKCommandQueue.
//
DKCommandQueue::DKCommandQueue() :
	VList(sizeof(char*), LF_USE_HEAP+LF_ALLOC_ERROR)
{}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandQueue::enqueue
//	Adds a command string at the back of the queue.
//
// Parameters
//	pString		pointer to string to queue; the string is copied to 
//			private storage and attached to the list.
// Return Value
//	Returns TRUE if the string is successfully queued.
//
BOOL DKCommandQueue::enqueue(char* pString)
{
	char* pHeap = new char[strlen(pString)+1];
	PVOID nodePtr;

	if (pHeap)
	{
		strcpy(pHeap, pString);
		nodePtr = newNode();
		if (nodePtr)
		{
			*(char**)nodePtr = pHeap;
			attachTail(nodePtr);
			return TRUE;
		}
	}

	return FALSE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandQueue::dequeue
//	Dequeue a command string and copy it into the supplied buffer.
//
// Parameters
// 	pBuffer		buffer to receive command string.
//
// Return Value
//	Returns TRUE if the queue was NOT empty.
//
BOOL DKCommandQueue::dequeue(char* pBuffer)
{
	PVOID pNode;

	if ( (pNode=removeFirst()) != 0)
	{
		char* pString = *(char**)pNode;
		strcpy(pBuffer, pString);
		deleteNode(pNode);
		return TRUE;
	}
	else
		return FALSE;
}
