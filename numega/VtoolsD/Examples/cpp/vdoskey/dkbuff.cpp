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

// dkbuff.cpp - implementation of class DKBuffer for VDOSKEY example
//
// Copyright (c) 1994, Compuware Corporation  All Rights Reserved

#include "vdoskey.h"

////////////////////////////////////////////////////////////////////////
#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKBuffer::DKBuffer
//	Constructor for DKBuffer.
//
// Parameters
//	maxLen		maximum number of characters buffer holds
//	bInsert		default insert/overwrite mode (TRUE==insert)
//
DKBuffer::DKBuffer(INT maxLen, BOOL bInsert)
{
	m_pBuf = new char[maxLen+1];
	m_maxLen = maxLen;
	m_curPos = 0;
	m_curLength = 0;
	m_bInsert = bInsert;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKBuffer::~DKBuffer
//	Destructor for DKBuffer. Deletes character array.
//
DKBuffer::~DKBuffer()
{
	if (m_pBuf)
		delete m_pBuf;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKBuffer::insert
//	Insert a character into the buffer at the current position.  
//	We have to move all characters between the current position and 
//	the end one position to the right.
//
// Parameters
//	ch	ascii code of character to insert
//
VOID DKBuffer::insert(char ch)
{
	INT i;

	if (m_curLength == m_maxLen)
		return;

	for (i = m_curLength; i > m_curPos; i--)
		m_pBuf[i] = m_pBuf[i-1];

	m_pBuf[m_curPos] = ch;
	m_curLength++;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKBuffer::del
//	Delete the character at the current position. Move all characters
//	that were to the right of the current character one position to the
//	left.
//
VOID DKBuffer::del()
{
	INT i;

	m_pBuf[m_curLength] = 0;
	if (m_curPos >= m_curLength)
		return;

	for (i=m_curPos; i < m_curLength; i++)
		m_pBuf[i] = m_pBuf[i+1];

	m_pBuf[--m_curLength] = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKBuffer::overWrite
//	Overwrites the character at the current position with the supplied
//	character (cf. insert).
//
// Parameters
//	ch	ascii code of character to write at current position
//
VOID DKBuffer::overWrite(char asc)
{
	m_pBuf[m_curPos] = asc;
	if (m_curPos >= m_curLength-1)
		m_curLength = m_curPos+1;

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKBuffer::setCommand
//	Set the contents of the buffer to the supplied string and set
//	the current position to the end of the buffer.
//
// Parameters
//	s	pointer to string
//
VOID DKBuffer::setCommand(char* s)
{
	strcpy(m_pBuf, s);
	m_curLength = strlen(s);
	m_curPos = m_curLength;
}
