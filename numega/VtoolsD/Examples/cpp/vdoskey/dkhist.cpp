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

// dkhist.cpp - implementation of class DKHistory
//
// Copyright (c) 1994, Compuware Corporation  All Rights Reserved

#include "vdoskey.h"

DKHistory* DKHistoryNode::m_pBaseList=0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKHistory::DKHistory
//	Constructor for class DKHistory.
//
// Parameters
//	maxCommands	The maximum number of commands to be kept in the
//			history list at any given time.
//
DKHistory::DKHistory(INT maxCommands) :
	VList(sizeof(DKHistoryNode), LF_USE_HEAP + LF_ALLOC_ERROR)
{
	m_maxRecall = maxCommands;
	m_currentRecall = 0;
	m_currentNode = 0;
	m_curLength = 0;
	m_bInit = TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKHistory::~DKHistory
//	Destructor for class DKHistory.  Deletes all saved commands.
//
DKHistory::~DKHistory()
{
	while (first())
		removeTail();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKHistory::addCommand
//	Add the supplied string to the command history object.  If the
//	history currently holds the maximum number of commands, the oldest
//	will be bumped off.
//
// Parameters
//	pCmd	pointer to string to add
//
BOOL DKHistory::addCommand(char* pCmd)
{
	char* pHeap = new char[strlen(pCmd)+1];
	DKHistoryNode* pNode;
	DKHistoryNode* pHead;

	if (pHeap)
	{
		strcpy(pHeap, pCmd);
		pNode = new DKHistoryNode;
		if (pNode)
		{
			pHead = (DKHistoryNode*)first();
			attach(pNode);
			if (pHead)
				pHead->m_link = pNode;

			pNode->m_pCommand = pHeap;
			pNode->m_link = 0;

			if (m_curLength == m_maxRecall)
				removeTail();
			else
				m_curLength++;

			return TRUE;
		}
	}

	return FALSE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKHistory::removeTail
//	Remove the node at the end of the history list.
//
VOID DKHistory::removeTail()
{
	DKHistoryNode* pNode = (DKHistoryNode*)first();
	DKHistoryNode* pTail=0;

	while (pNode)
	{
		pTail = pNode;
		pNode = (DKHistoryNode*)next(pNode);
	}

	if (pTail)
	{
		char* pCmd = pTail->m_pCommand;
		delete pCmd;
		remove(pTail);
		delete pTail;

		if (m_currentNode == pTail)
			oldest();
	}
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKHistory::newest
//	Get a pointer to the most recently added command string.
// 
// Return Value
//	Returns a pointer to the newest string, or NULL if the history
//	list is empty.
//
char* DKHistory::newest()
{
	m_currentNode = (DKHistoryNode*)first();
	if (m_currentNode != 0)
		return m_currentNode->m_pCommand;
	else
		return 0;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKHistory::oldest
//	Get a pointer to the oldest command string in the history list.
//
// Return Value
//	Returns a pointer to the oldest saved command, or NULL if the list
//	is empty.
//
char* DKHistory::oldest()
{
	DKHistoryNode* pNode = (DKHistoryNode*)first();
	DKHistoryNode* pTail = 0;

	while (pNode)
	{
		pTail = pNode;
		pNode = (DKHistoryNode*)next(pNode);
	}

	if (pTail)
	{
		m_currentNode = pTail;
		return pTail->m_pCommand;
	}
	else
	{
		m_currentNode = 0;
		return 0;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKHistory::initRecall
//	Set the history list to its start state.
//
VOID DKHistory::initRecall()
{
	m_currentNode = 0;
	m_bInit = TRUE;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKHistory::setRecallMatch
//	Set the string that member function recall uses to recall saved
// 	command strings.  This is one of two functions that serve this
//	purpose.  This one takes a buffer and a count as parameters. It
//	invokes the other function by the same name after null terminating
//	the supplied string.
//
// Parameters
//	match		pointer to string to serve as recall template
//	nChars		length of string
//
VOID DKHistory::setRecallMatch(char* match, INT nChars)
{
	char save = match[nChars];
	match[nChars] = 0;
	setRecallMatch(match);
	match[nChars] = save;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKHistory::setRecallMatch
//	Set the string that member function recall uses to recall saved
// 	command strings.  This is one of two functions that serve this
//	purpose.  This one takes a null terminated string as a parameter
//
// Parameters
//	match		pointer to null terminated string to serve as recall
//			template
//
VOID DKHistory::setRecallMatch(char* match)
{
	if (m_currentRecall != 0)
		delete m_currentRecall;

	if (!match || (strlen(match) == 0) )
		m_currentRecall = 0;
	else
	{
		if ( (m_currentRecall = new char[strlen(match)+1]) != 0)
			strcpy(m_currentRecall, match);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKHistory::recall
//	Fetch a command from the history.  The state of the DKHistory object
//	affects how this is done.  First, there is the concept of "current
//	history line", which correponds to data member m_currentNode.
//	Second, the current match string (m_currentMatch) determines what
//	which strings are recalled.  Only those strings that begin with
//	the match string are eligible.  The match string may be null, in
//	which case all strings match.
//
// Parameters
//	bNext		if TRUE, get next item in list (relative to current)
//			otherwise get previous. Next corresponds to DOSKEY
//			command invoked by DownArrow, and previous to 
//			UpArrow (or F8).
//
// Return Value
//	Returns a pointer to the recalled string, or NULL if none.
//
char* DKHistory::recall(BOOL bNext)
{
	char* pCmd;
	INT matchLength;
	DKHistoryNode* saveNode = m_currentNode;

// We have to have a current node to do the recall.  If m_currentNode
// is zero, it means the DKHistory object is in the start state or we
// reached the end of the list on a previous recall. 

	if (m_currentNode == 0)
	{
		if (bNext)
			return 0;
		else if (m_bInit)
			m_currentNode = (DKHistoryNode*)first();
		else // end reached on previous recall - make oldest current
		{
			oldest();
			if (m_currentNode == 0)
			{
				m_currentNode = saveNode;
				return 0;
			}
		}

	}
	else if (bNext)	// use back link
		m_currentNode = m_currentNode->m_link;

	else // use forward link
		m_currentNode = (DKHistoryNode*)next(m_currentNode);

// matchLength is the number of characters in the match string that must
// match in the recalled line.

	if (m_currentRecall)
		matchLength = strlen(m_currentRecall);
	else
		matchLength = 0;

// Now walk the list looking for a match.
	m_bInit = FALSE;
	while (m_currentNode)
	{
		pCmd = m_currentNode->m_pCommand;
		if (pCmd)
		{
			INT cmdLength = strlen(pCmd);

			if ((matchLength == 0) ||
			    ((cmdLength >= matchLength) &&
			     (memcmp(pCmd, m_currentRecall, matchLength) == 0)))
				return pCmd;
		}

		if (bNext)
			m_currentNode = m_currentNode->m_link;
		else
			m_currentNode = (DKHistoryNode*)next(m_currentNode);
	};

// Fall out of loop if no match found.
	m_currentNode = saveNode;
	return 0;
}
