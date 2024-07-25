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

// dkkey.cpp - implementation of class DKKey
//
// Copyright (c) 1994, Compuware Corporation  All Rights Reserved

#include "vdoskey.h"
//////////////////////////////////////////////////////////////////////////
#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

#define ALT_FLAG	 1
#define CTRL_FLAG	 2
#define ANYVAL		-1

// COMMANDDEF is a structure used to map keys to DOSKEY commands.
typedef struct 
{
	DKCOMMAND m_cmd;		// DOSKEY command
	char	m_cmdasc;		// ascii code
	char	m_cmdscan;		// scan code
	INT	m_cmdflags;		// shift flags
} COMMANDDEF;

COMMANDDEF cmdTable[] = {
	{CMD_TERMINATOR,	0xD,	ANYVAL,	ANYVAL},   // enter
	{CMD_SEPARATOR,		0x14,	ANYVAL,	ANYVAL},   // ^T
	{CMD_ESCAPE,		0x1B,	ANYVAL,	ANYVAL},   // escape
	{CMD_ABORT,		0x3,	ANYVAL,	ANYVAL},   // ^C
	{CMD_PREVIOUS,		ANYVAL,	0x48,	0},	   // up
	{CMD_NEXT,		ANYVAL,	0x50,	0},	   // down
	{CMD_SHOWHISTORY,	0,	0x41,	0},	   // F7
	{CMD_CYCLEMATCH,	0,	0x42,	0},	   // F8
	{CMD_PROMPT,		0,	0x43,	0},	   // F9
	{CMD_DISPLAYOLDEST,	ANYVAL,	0x49,	0},	   // PgUp
	{CMD_DISPLAYNEWEST,	ANYVAL,	0x51,	0},	   // PgDn
	{CMD_HOME,		ANYVAL,	0x47,	0},	   // Home
	{CMD_END,		ANYVAL,	0x4F,	0},	   // End
	{CMD_LEFTCHAR,		ANYVAL,	0x4B,	0},	   // left
	{CMD_RIGHTCHAR,		ANYVAL,	0x4D,	0},	   // right
	{CMD_LEFTWORD,		ANYVAL,	0x73,	CTRL_FLAG},// ^left
	{CMD_RIGHTWORD,		ANYVAL,	0x74,	CTRL_FLAG},// ^right
	{CMD_RECALLCHAR,	0,	0x3B,	0},	   // F1
	{CMD_RECALLUPTOCHAR,	0,	0x3C,	0},	   // F2
	{CMD_RECALLREMAINDER,	0,	0x3D,	0},	   // F3
	{CMD_DELETETOCHAR,	0,	0x3E,	0},	   // F4
	{CMD_STORELINE,		0,	0x3F,	0},	   // F5
	{CMD_QUOTEEOF,		0,	0x40,	0},	   // F6
	{CMD_RUBOUT,		0x8,	ANYVAL,	0},	   // backsp
	{CMD_DELETE,		ANYVAL,	0x53,	0},	   // del
	{CMD_DELETETOEND,	ANYVAL,	0x75,	CTRL_FLAG},// ^End
	{CMD_TOGGLEINSERT,	0,	0x52,	0}	   // insert
};

#define NCOMMANDS ( sizeof(cmdTable) / sizeof(COMMANDDEF) )

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKKey::get
//	Get a key from the keyboard. Uses nested execution services.
//	Sets all data members of the object.
//
// Parameters
//	pRegs		pointer to client register structure
//
VOID DKKey::get(CLIENT_STRUCT* pRegs)
{
	Begin_Nest_Exec();

	_clientAH = 0x10;
	Exec_Int(0x16);
	m_scan = ((volatile CLIENT_STRUCT*)pRegs)->CBRS.Client_AH;
	m_ascii = ((volatile CLIENT_STRUCT*)pRegs)->CBRS.Client_AL;

	if (m_ascii == TAB)
		m_ascii = SPACE;


	_clientAH = 0x2;
	Exec_Int(0x16);
	m_bCtrl = ((((volatile CLIENT_STRUCT*)pRegs)->CBRS.Client_AL & 4) != 0);
	m_bAlt  = ((((volatile CLIENT_STRUCT*)pRegs)->CBRS.Client_AL & 8) != 0);

	End_Nest_Exec();

}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKKey::map
//	Map a DKKey object to a VDOSKEY command.  Uses lookup table.
//
// Return Value
//	Returns the COMMANDDEF corresponding to the current state of the
//	object.
//
DKCOMMAND DKKey::map()
{
	COMMANDDEF* pCmd;
	INT i;

	for (pCmd = &cmdTable[0], i=0; i < NCOMMANDS; pCmd++, i++)
	{
		if ( (pCmd->m_cmdasc != ANYVAL) &&
		     (pCmd->m_cmdasc != m_ascii) )
			continue;

		if ( (pCmd->m_cmdscan != ANYVAL) &&
		     (pCmd->m_cmdscan != m_scan) )
			continue;

		if (pCmd->m_cmdflags == ANYVAL)
			return pCmd->m_cmd;

		if ( (pCmd->m_cmdflags & ALT_FLAG) && !m_bAlt)
			continue;

		if ( (pCmd->m_cmdflags & CTRL_FLAG) && !m_bCtrl)
			continue;

		if ( !(pCmd->m_cmdflags & ALT_FLAG) && m_bAlt)
			continue;

		if ( !(pCmd->m_cmdflags & CTRL_FLAG) && m_bCtrl)
			continue;

		return pCmd->m_cmd;
	}

	if ( (m_ascii == 0) || (m_bAlt) || (m_bCtrl) )
		return CMD_IGNORE;
	else
		return CMD_INPUTCHAR;
}
