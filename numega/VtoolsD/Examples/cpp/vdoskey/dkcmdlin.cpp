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

// dkcmdlin.cpp - implementation of class DKCommandLine for VDOSKEY example
//
// Copyright (c) 1994, Compuware Corporation  All Rights Reserved

#include "vdoskey.h"

//////////////////////////////////////////////////////////////////////////
#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// POSITION::operator ++
//	Increment operator for class POSITION.  Moves the position object
//	one character to the right.  If it reaches the end of the row,
//	moves it to column zero of the next row.
//
POSITION& POSITION::operator ++()
{
	BYTE displayWidth;
	VMHANDLE hVM = VVirtualMachine::currentVM();

// The number of columns is stored in the VM's memory at 0:0x44A

	displayWidth = *(BYTE*)(hVM->CB_High_Linear+0x44a);

	col++;
	if (col >= displayWidth)
	{
		row++;
		col = 0;
	}

	return *this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// POSITION::operator --
//	Decrement operator for class POSITION.  Moves the position object
//	one character to the left.  If at the beginning of a row (col 0),
//	moves to the last column of the previous row.
//
POSITION& POSITION::operator --()
{
	BYTE displayWidth;
	VMHANDLE hVM = VVirtualMachine::currentVM();

// The number of columns is stored in the VM's memory at 0:0x44A

	displayWidth = *(BYTE*)(hVM->CB_High_Linear+0x44a);

	if (col == 0)
	{
		if (row)
		{
			row--;
			col = displayWidth-1;
		}
	}
	else
		col--;

	return *this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// POSITION::operator >
//	Greater-Than operator for class POSITION. Returns TRUE if the 
//	the object's position is right or below that of the compared object.
//
BOOL POSITION::operator >(POSITION& pos)
{
	if (this->row > pos.row)
		return TRUE;
	else if (this->row == pos.row)
		return (this->col > pos.col);
	else
		return FALSE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// POSITION::operator <
//	Less-Than operator for class POSITION. Returns TRUE if the 
//	the object's position is left or above that of the compared object.
//
BOOL POSITION::operator <(POSITION& pos)
{
	if (this->row < pos.row)
		return TRUE;
	else if (this->row == pos.row)
		return (this->col < pos.col);
	else
		return FALSE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// POSITION::operator==
//	Equal-To operator for class POSITION. Returns TRUE if the 
//	the object's position the same as that of the compared object.
//
BOOL POSITION::operator ==(POSITION& pos)
{
	return ( (this->row == pos.row) && (this->col == pos.col) );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::DKCommandLine
//	Constructor for class DKCommandLine. The virtual machine handle
//	and register structure are supplied in order to support nested
//	execution calls made by member functions.
//
// Parameters
//	hVM	handle of current virtual machine
//	pRegs	pointer to client register structure
//
DKCommandLine::DKCommandLine(VMHANDLE hVM, CLIENT_STRUCT* pRegs)
{
	m_hVM = hVM;
	m_regs = pRegs;
	m_start = m_end = m_current = cursorPosition();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::cursorPosition
//	Retrieves the current cursor position. Accesses the BIOS data
//	area in the virtual machine directly.
//
POSITION DKCommandLine::cursorPosition()
{
	POSITION p;

	INT currentDisplayPage = *(BYTE*)(m_hVM->CB_High_Linear+0x462);
	p = ((POSITION*)(m_hVM->CB_High_Linear+0x450))[currentDisplayPage];
	return p;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::clear
//	Clears the current command line.
//
VOID DKCommandLine::clear()
{
	m_current = m_start;
	clearToEnd();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::clearToEnd
//	Clears the command line from the current position to the end.
//
VOID DKCommandLine::clearToEnd()
{
	POSITION pos;

	for (pos = m_current; !(pos > m_end); ++pos)
	{
		moveTo(pos);
		erase();
	}

	moveTo(m_current);
	m_end = m_current;
	--m_end;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::cursorHome
//	Moves the cursor to the original position.
//
VOID DKCommandLine::cursorHome()
{
	moveTo(m_start);
	m_current = m_start;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::cursorEnd
//	Moves the cursor to the current end of the command line.
//
VOID DKCommandLine::cursorEnd()
{
	POSITION p = m_end;
	moveTo(++p);
	m_current = p;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::cursorLeft
//	Moves the cursor to the left, wrapping back to the previous row 
//	if necessary.
//
// Parameters
//	nCols		number of columns to move left 
//
VOID DKCommandLine::cursorLeft(INT nCols)
{
	while (nCols-- && !(m_current == m_start))
		--m_current;

	moveTo(m_current);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::cursorRight
//	Moves the cursor to the right, wrapping down to the next row if
//	necessary.
//
// Parameters
//	nCols		number of columns to move right
//
VOID DKCommandLine::cursorRight(INT nCols)
{
	BYTE rowBeforeNewline;

	int displayWidth = *(BYTE*)(m_hVM->CB_High_Linear+0x44a);

	while (nCols--)
	{
		if (m_current.col == displayWidth-1) // if at end of row
		{

// We detect a scroll by noticing the row does not change after newline.
// When this is detected, we adjust the row of the start and end positions.
			rowBeforeNewline = m_current.row;
			newline();
			m_current = cursorPosition();
			if (rowBeforeNewline == m_current.row)
			{
				m_start.row--;
				m_end.row--;
			}
		}
		else
			++m_current;
	}

	moveTo(m_current);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::setCursor
//	Move the cursor and update its position.
//
// Parameters
//	pos		position to which the cursor is to be moved.
VOID DKCommandLine::setCursor(POSITION& pos)
{
	moveTo(pos);
	m_current = pos;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::erase
//	Erase the character at the current position by emitting a space.
//
VOID DKCommandLine::erase()
{
	POSITION pos = m_current;
	putChar(' ');
	if (m_end == pos)
		--m_end;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::putChar
//	Output a character at the current position.  This member function
//	uses nested execution services call the BIOS to output the character.
//
// Parameters
//	asc	ascii code of character to output.
//
VOID DKCommandLine::putChar(char asc)
{
	if (! (m_current < m_end))
		m_end = m_current;

	int page = *(BYTE*)(m_hVM->CB_High_Linear+0x462);

	m_regs->CBRS.Client_AH = 0xA;
	m_regs->CBRS.Client_AL = asc;
	m_regs->CWRS.Client_BX = page << 8;
	m_regs->CWRS.Client_CX = 1;

	Begin_Nest_Exec();
	Exec_Int(0x10);
	End_Nest_Exec();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::moveTo
//	Move the cursor.  Uses nested execution services to call the BIOS
//	to move the cursor.
//
// Parameters
//	pos	position to which the cursor is to be moved.
//
VOID DKCommandLine::moveTo(POSITION& pos)
{
	int page = *(BYTE*)(m_hVM->CB_High_Linear+0x462);
	WORD rc = ((WORD)pos.row << 8) | pos.col;

	m_regs->CWRS.Client_AX = 0x200;
	m_regs->CWRS.Client_BX = page << 8;
	m_regs->CWRS.Client_DX = rc;
	Begin_Nest_Exec();
	Exec_Int(0x10);
	End_Nest_Exec();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::putString
//	Output a null terminated string of characters at the current
//	position.
//
// Parameters
//	pBuf	pointer to the string to output
//
VOID DKCommandLine::putString(char* pBuf)
{
	while (*pBuf != 0)
	{
		putChar(*pBuf);
		cursorRight();
		pBuf++;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::newline
//	Output carriage return and newline sequence.
//
VOID DKCommandLine::newline()
{
	m_regs->CWRS.Client_AX = 0x0E0D;
	Begin_Nest_Exec();
	Exec_Int(0x10);

	m_regs->CWRS.Client_AX = 0x0E0A;

	Exec_Int(0x10);
	End_Nest_Exec();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine::cursorSize
//	Set the cursor either block or underline.
//
// Parameters
//	bBlock		if TRUE, set block cursor, otherwise set underline
//
VOID DKCommandLine::cursorSize(BOOL bBlock)
{
	m_regs->CBRS.Client_AH = 1;
	if (bBlock)
		m_regs->CWRS.Client_CX = 0x307;
	else
		m_regs->CWRS.Client_CX = 0x707;
	Begin_Nest_Exec();
	Exec_Int(0x10);
	End_Nest_Exec();
}
