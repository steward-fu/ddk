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

// vdoskey.cpp - VDOSKEY main module
//
// Copyright (c) 1994, Compuware Corporation  All Rights Reserved

// VDOSKEY is a VxD that replaces the DOS utility DOSKEY. The advantage of
// VDOSKEY over the real mode TSR is that it occupies no space at all in
// the DOS 640KB address space.

// VDOSKEY may be loaded either statically or dynamically.

// The basic approach is to trap INT 21h/function 0Ah (get buffered input),
// and use nested execution services to collect input while providing
// line-editing and command history services.

// Classes
//
//	DKMachine	A DKMachine object corresponds to a virtual machine.
//			Each machine maintains a command queue, a command
//			history, and an insert mode flag.  The member 
//			functions handle system control messsages and 
//			provide input from the keyboard of command lines.
//		
//	DKInt21		A DKInt21 object provides a pre-chain hook of INT 21h
//			occurring in V86 mode programs (such as command.com).
//			Its handler detects function 0Ah (Buffered Keyboard
//			Input) and invokes member function getInput of
//			the current DKMachine object.
//
//	DKKey		A DKKey object corresponds to a key typed by a user. 
//			Its data members include the ascii code and scan code
//			for the key, as well as flags indicating whether
//			a control key or alt key was pressed when the key
//			was pressed.   Its member functions are get(), which
//			reads a key from the keyboard, and map(), which maps
//			the key to a function.
//
//	DKCommandLine	A DKCommandLine object provides an interface to the
//			display.  It includes member functions for operations
//			such as positioning the cursor, clearing the input
//			line from the display, and changing the cursor size.
//
//	DKBuffer	A DKBuffer stores a string.  Its data member store
//			not only the string data, but also the current 
//			position in the string, and a flag to indicate 
//			whether the current insert mode is insert or 
//			overwrite.  
//			
//			VDOSKEY uses a DKBuffer to buffer the current input
//			line, and also to hold the command template.  The
//			command template is used with functions
//			CMD_RECALLCHAR,	CMD_RECALLUPTOCHAR, and
//			CMD_RECALLREMAINDER, which correspond to DOSKEY
//			functions accessed by keys F1, F2, and F3.
//
//	DKHistory	A DKHistory object is a specialized list of strings,
//			each of which corresponds to a previously typed
//			command.  The member functions provide the ability
//			to add and remove strings to the history, and to
//			sequentially recall commands.  VDOSKEY creates a
//			DKHistory object for each virtual machine.
//
//	DKCommandQueue	A DKCommandQueue object holds a series of commands
//			that the user entered on a single line using the
//			DOSKEY separator key (^T).  VDOSKEY keeps a queue
//			for each virtual machine.  If it is not empty when
//			the getInput routine is entered, a command is 
//			dequeued and used as the next command line.

// This example program demonstrates the use of the following classes
// from the VToolsD class library: 
//	VDevice, VVirtualMachine, VList, VPreChainV86Int

//////////////////////////////////////////////////////////////////////////

#define DEVICE_MAIN
#include "vdoskey.h"

#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT

Declare_Virtual_Device(VDOSKEY)

DKInt21* pDosKeyInt21=0;		// V86 (pre-chain) INT 21h object

//////////////////////////////////////////////////////////////////////////
// DKDevice::OnDeviceInit
//	Initializes VDOSKEY by:
//		- creating the DKInt21 object to trap INT 21h/0Ah
//		- creating an instance of DKMachine corresponding to sys VM
//		- creating an instance of DKHistory to serve as a base list
//		  for allocation and deallocation of history list nodes.
// Parameters
//	hSysVM		handle of system virtual machine
//	pszCmdTail	pointer to Windows command tail
//
// Return Value
//	Returns TRUE if device is successfully initialized
//
BOOL DKDevice::OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail)
{
	DKInt21* pDosKeyInt21;

	if ( (pDosKeyInt21 = new DKInt21()) == 0)
		goto fail1;

	if ( (DKHistoryNode::m_pBaseList = new DKHistory(0)) == 0)
		goto fail2;

	if ( !pDosKeyInt21->hook())
		goto fail3;

	return TRUE;

fail3:	delete DKHistoryNode::m_pBaseList;
fail2:	delete pDosKeyInt21;
fail1:	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKDevice::OnCreateVM
//	Creates a new instance of DKMachine.  Each DKMachine carries its
//	own command history.
//
// Return Value
//	Returns TRUE if the DKMachine is successfully created.
//
BOOL DKDevice::OnCreateVM(VMHANDLE hVM)
{
	return (new DKMachine(hVM) != 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKInt21::DKInt21
//	Constructor for DKInt21 object.  This object is used to trap 
//	INT 21h/Function 0Ah from V86 mode.
//
DKInt21::DKInt21() : VPreChainV86Int(0x21) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKInt21::handler
//	This is the INT 21h handler for V86 mode.  When function 0Ah
//	(Get Buffered Keyboard Input) is detected), it uses the VM
//	handle to get the pointer to the DKMachine object, and calls
//	DKMachine member function getInput to collect input.
//
// Parameters
//	hVM	handle of the current virtual machine
//	pRegs	pointer to client register structure
//		Client DS:DX points to buffer to receive input line
//	into	this will always be 21h
//
BOOL DKInt21::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, DWORD intno)
{
	char* buffer;
	BYTE count;
	DKMachine* pVM = (DKMachine*)VVirtualMachine::getVM(hVM);

	if ((_clientAH == BUFFERED_KEYBOARD_INPUT) && (pVM != 0))
	{
		buffer = (PCHAR)MAPFLAT(CRS.Client_DS, CWRS.Client_DX);

		// return count in buffer[1]
		buffer[1] = pVM->getInput(pRegs, buffer+2, *(BYTE*)buffer);

		return TRUE;	// Tells VMM that interrupt was handled
	}

	return FALSE;	// Tells VMM that interrupt was not handled
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKMachine::DKMachine
//	Constructor for DKMachine.  This is invoked by DKDevice::OnCreateVM.
//	Initializes data members.
//
DKMachine::DKMachine(VMHANDLE hVM) : VVirtualMachine(hVM)
{
	m_queue = 0;
	m_history = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKMachine::~DKMachine
//	Destructor for DKMachine.  This is invoked by DKMachine::OnDestroyVM.
//	Deletes any remaining data member allocations.
//
DKMachine::~DKMachine()
{
	if (m_queue)
		delete m_queue;
	if (m_history)
		delete m_history;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKMachine::OnVMInit
//	Initialization for DKMachine.  Allocate a DKCommandQueue object and
//	a DKHistory object.  Set the default insert/overwrite mode to 
//	overwrite.
//
// Return Value
// 	Returns TRUE if the command queue and command history are 
//	successfully allocated.
//
BOOL DKMachine::OnVMInit()
{
	m_queue = new DKCommandQueue();
	m_history = new DKHistory(24);
	m_bInsert = FALSE;

	return (m_queue && m_history);
}

//////////////////////////////////////////////////////////////////////////
#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKMachine::getInput
//	This member function controls the collection of the input line
//	and the processing of special command keys. 
//
// Parameters
//	pRegs		pointer to client register structure
//	buf		buffer to receive input line
//	bufSize		size of buf in bytes
//
// Return Value
//	The number of bytes entered in the command line.
//
BYTE DKMachine::getInput(CLIENT_STRUCT* pRegs, char* buf, INT bufSize)
{
	DKKey 		key;
	DKCommandLine	cmdLine(getHandle(), pRegs);
	DKBuffer	cmdBuffer(MAXBUF+2, FALSE);
	DKBuffer	cmdTemplate(MAXBUF+2, FALSE);
	DKCOMMAND	cmd;
	BOOL		bTerminated;
	CLIENT_STRUCT	saveRegs;
	char* 		pBuf;
	char*		pTemplate;
	POSITION	linePos;
	BYTE		bufLength;
	INT		bufPos;
	INT		i;
	BOOL		bInWhite;
	BOOL		bModifyTemplate;
	char		number[4];
	POSITION	homePos;
	POSITION	delta;

// Save the current client register structure, because it will be used by
// numerous nested execution services.

	Save_Client_State(&saveRegs);

// Set the cursor size to either block or underline, depending on the
// current insert mode (insert/overwrite).
	cmdLine.cursorSize(m_bInsert = FALSE);

// VDOSKEY allows the user to enter multiple commands on a single input
// line by separating them with a special character (^T).  If a previous
// input line queued up several command strings in this virtual machine,
// we will use the next one now as the input line.  Check the queue for
// for this virtual machine, and dequeue the next command if one is present.
	if (!m_queue->isEmpty())
	{
		m_queue->dequeue(buf);
		cmdLine.putString(buf);
		goto epilog;
	}

// The command template is used with commands that recall portions of the
// the previous input line.  Initialize the template to the newest entry in
// the command history for this virtual machine, and set the buffer position
// for the template to its first character.
	cmdTemplate.setCommand(m_history->newest());
	cmdTemplate.move(0);

// Initialize the command history recall function.
	m_history->initRecall();

// This is the main loop for input. Get a key and process it until done.

	for (bTerminated=FALSE; !bTerminated; )
	{
		key.get(pRegs);
		switch (cmd=key.map())
		{
		case CMD_TERMINATOR:	// newline terminates input
			bTerminated = TRUE;
			break;

		case CMD_ESCAPE:	// escape clears input
			doEscape:
			cmdLine.clear();
			cmdBuffer.clear();
			cmdTemplate.setCommand(m_history->newest());
			cmdTemplate.move(0);
			m_history->setRecallMatch(0);
			m_history->initRecall();
			break;

		case CMD_ABORT:		// ^C aborts input
			doAbort:
			cmdLine.putString("^C");
			cmdBuffer.clear();
			bTerminated = TRUE;
			break;

		case CMD_PREVIOUS:	// recall line from history
			m_history->setRecallMatch(0);
			setInput(m_history->recall(FALSE),cmdLine,cmdBuffer,
				cmdTemplate);
			break;

		case CMD_NEXT:		// recall next line in history
			m_history->setRecallMatch(0);
			pBuf = m_history->recall(TRUE);
			if (pBuf)
				setInput(pBuf,cmdLine,cmdBuffer,cmdTemplate);
			else
			{
				cmdLine.clear();
				cmdBuffer.clear();
				cmdTemplate.clear();
				m_history->initRecall();
			}
			break;

		case CMD_SHOWHISTORY:	// display all lines in history
			m_history->initRecall();
			m_history->setRecallMatch(0);
			pBuf = m_history->oldest();
			for (i=1; pBuf; i++)
			{
				cmdLine.newline();
				linePos = cmdLine.cursorPosition();
				cmdLine.setCursor(linePos);
				cmdLine.putString(_itoa(i, number, 10));
				cmdLine.putString(": ");
				cmdLine.putString(pBuf);
				pBuf = m_history->recall(TRUE);
			}
			bTerminated = TRUE;
			cmdBuffer.clear();
			break;

		case CMD_CYCLEMATCH:	// match a history line to current 
			bufPos = cmdBuffer.pos();

// Recall of a command string may cause the display to scroll, so we have to
// record the current display position as a delta from the home position in
// order to restore it correctly after the recall.
			linePos = cmdLine.cursorPosition();
			cmdLine.cursorHome();
			homePos = cmdLine.cursorPosition();
			delta;
			delta.row = linePos.row-homePos.row;
			delta.col = linePos.col-homePos.col;
			cmdLine.setCursor(linePos);

			m_history->setRecallMatch(cmdBuffer.getPtr(),
				cmdBuffer.pos());
			pBuf = m_history->recall(FALSE);
			if (!pBuf)
			{
				m_history->initRecall();
				pBuf = m_history->recall(FALSE);
			}
			setInput(pBuf,cmdLine,cmdBuffer,cmdTemplate);

// Restore position
			cmdLine.cursorHome();
			linePos = cmdLine.cursorPosition();
			linePos.row += delta.row; linePos.col += delta.col;
			cmdLine.setCursor(linePos);

			cmdBuffer.move(bufPos);
			cmdTemplate.move(bufPos);
			break;

		case CMD_PROMPT:	// Prompt for number of line to recall
			setInput("", cmdLine, cmdBuffer, cmdTemplate);
			cmdLine.putString("Line number: ");
			i = 0;
			while (1)	// Loop here to input decimal value
			{
				key.get(pRegs);
				cmd = key.map();
				if (cmd == CMD_TERMINATOR) break;
				switch (cmd)
				{
				case CMD_ESCAPE: goto doEscape;
				case CMD_ABORT:  goto doAbort;
				case CMD_INPUTCHAR:
					cmdLine.putChar(key.m_ascii);
					cmdLine.cursorRight();
					if ( (key.m_ascii >= '0') &&
					     (key.m_ascii <= '9') )
						i += i*10 + key.m_ascii-'0';
				}
			}

			m_history->initRecall();
			m_history->setRecallMatch(0);
			pBuf = m_history->oldest();
			for (; i > 1; i--)
				pBuf = m_history->recall(TRUE);

			if (pBuf == 0) pBuf = m_history->newest();
			if (pBuf == 0) pBuf = "";

			setInput(pBuf, cmdLine, cmdBuffer, cmdTemplate);

			break;

		case CMD_DISPLAYOLDEST:	// recall oldest line in history
			setInput(m_history->oldest(), cmdLine, cmdBuffer,
				cmdTemplate);
			break;

		case CMD_DISPLAYNEWEST:	// recall newest line in history
			setInput(m_history->newest(), cmdLine, cmdBuffer,
				cmdTemplate);
			break;

		case CMD_HOME:		// position to start of input line
			cmdLine.cursorHome();
			cmdBuffer.toStart();
			cmdTemplate.toStart();
			break;

		case CMD_END:		// position to end of input line
			if (cmdBuffer.length() > 0)
			{
				cmdLine.cursorEnd();
				cmdBuffer.toEnd();
				cmdTemplate.move(cmdBuffer.pos());
			}
			break;

		case CMD_LEFTCHAR:	// move one character to the left
			if (cmdBuffer.pos() != 0)
			{
				cmdLine.cursorLeft();
				cmdBuffer.left();
				cmdTemplate.left();
			}
			break;

		case CMD_RIGHTCHAR:	// move one character to the right
		     	doRight:
			if (cmdBuffer.pos() < cmdBuffer.length() )
			{
				cmdLine.cursorRight();
				cmdBuffer.right();
				cmdTemplate.right();
			}
			break;

		case CMD_LEFTWORD:	// move left one word 

			cmdBuffer.left();
			cmdLine.cursorLeft();
			bInWhite = (cmdBuffer.get() == SPACE);
			while (cmdBuffer.pos() > 0)
			{
				cmdBuffer.left();
				cmdTemplate.left();
				cmdLine.cursorLeft();
				if (!bInWhite)
				{
					if (cmdBuffer.get() == SPACE)
					{
						cmdBuffer.right();
						cmdTemplate.right();
						cmdLine.cursorRight();
						break;
					}
				}
				else if (cmdBuffer.get() != SPACE)
					bInWhite = FALSE;
			}
			break;


		case CMD_RIGHTWORD:	// move right one word

			bInWhite = (cmdBuffer.get() == SPACE);
			while (cmdBuffer.pos() < cmdBuffer.length())
			{
				cmdBuffer.right();
				cmdTemplate.right();
				cmdLine.cursorRight();
				if (bInWhite)
				{
					if (cmdBuffer.get() != SPACE)
						break;
				}
				else if (cmdBuffer.get() == SPACE)
					bInWhite = TRUE;
			}

			break;

		case CMD_RUBOUT:	// erase char to left of current
			bModifyTemplate=(cmdBuffer.pos()!=cmdBuffer.length());
			cmdBuffer.left();
			cmdLine.cursorLeft();

			if (bModifyTemplate)
			{
				cmdTemplate.left();
				deleteCharacter(cmdLine, cmdBuffer, cmdTemplate);
			}
			else
				deleteCharacter(cmdLine, cmdBuffer, *(DKBuffer*)0);

			cmdTemplate.move(cmdBuffer.pos());
			break;

		case CMD_DELETE:	// delete char at current position
			deleteCharacter(cmdLine, cmdBuffer, cmdTemplate);
			break;

		case CMD_DELETETOEND:	// delete from current position to end
			cmdLine.clearToEnd();
			cmdBuffer.delToEnd();
			break;

		case CMD_TOGGLEINSERT:	// toggle insert/overwrite mode for VM
			m_bInsert = !m_bInsert;
			cmdLine.cursorSize(m_bInsert);
			break;

		case CMD_RECALLCHAR:	// recall one character from template
			if (cmdTemplate.pos() < cmdTemplate.length())
				if (cmdBuffer.pos() < cmdBuffer.length())
					goto doRight;
				else
				{
					key.m_ascii = cmdTemplate.get();
					goto doInput;
				}
			break;

		case CMD_RECALLUPTOCHAR:// recall from template up to char
			key.get(pRegs);	// get the character to recall up to
			cmd = key.map();
			switch (cmd)
			{
			case CMD_ESCAPE: goto doEscape;
			case CMD_ABORT:  goto doAbort;
			case CMD_INPUTCHAR:
				bufPos = cmdTemplate.pos();
				pBuf = cmdTemplate.getPtr() + bufPos;
			  	for (i=0; *pBuf != 0; i++, pBuf++)
					if (*pBuf == key.m_ascii)
						break;
				if (*pBuf == 0)
					break;
				if ( (bufPos+i) < cmdBuffer.length())
				{
					cmdBuffer.move(bufPos+i);
					cmdTemplate.move(bufPos+i);
					cmdLine.cursorRight(i);
				}
				else
				{
					setInput(cmdTemplate.getPtr(), cmdLine, cmdBuffer,
						cmdTemplate);
					cmdBuffer.move(bufPos+i);
					cmdBuffer.delToEnd();
					cmdLine.cursorHome();
					cmdLine.cursorRight(bufPos+i);
					cmdLine.clearToEnd();
				}
			}
			break;

		case CMD_RECALLREMAINDER:// recall remaining chars from template
			setInput(cmdTemplate.getPtr(), cmdLine, cmdBuffer,
				cmdTemplate);
			break;

		case CMD_DELETETOCHAR:	// delete up to char
			key.get(pRegs);
			cmd = key.map();
			switch (cmd)
			{
			case CMD_ESCAPE: goto doEscape;
			case CMD_ABORT:  goto doAbort;
			case CMD_INPUTCHAR:
				bufPos = cmdBuffer.pos();
				pBuf = cmdBuffer.getPtr() + bufPos;
			  	for (i=0; *pBuf != 0; i++, pBuf++)
					if (*pBuf == key.m_ascii)
						break;
				if (*pBuf)
				    for (; i > 0; i--)
				    	deleteCharacter(cmdLine,cmdBuffer,
						cmdTemplate);
			}
			break;

		case CMD_STORELINE:	// move current input line to history
			if (cmdBuffer.length() > 0)
			{
				m_history->addCommand(cmdBuffer.getPtr());
				cmdBuffer.clear();
				bTerminated = TRUE;
			}
			break;

		case CMD_QUOTEEOF:	// input ^Z
			key.m_ascii = 26;
			// fallthru
		case CMD_SEPARATOR:	// command separator - process after
			// fallthru
		case CMD_INPUTCHAR:	// add character to input line at
					//   current position
			doInput:
			if (m_bInsert)
			{
				linePos = cmdLine.cursorPosition();
				INT bufPos = cmdBuffer.pos();

				cmdBuffer.insert(key.m_ascii);
				if (cmd == CMD_INPUTCHAR)
					cmdTemplate.insert(key.m_ascii);
				pBuf = cmdBuffer.getPtr() + bufPos;

				cmdLine.clearToEnd();
				cmdLine.putString(pBuf);

				POSITION newPos = cmdLine.cursorPosition();

// Check for scroll.  If, after insertion, the current column is zero and
// we are at the same row, then the original row must have scrolled up one.
				if ( (newPos.col == 0) &&
				     (newPos.row == linePos.row) )
					linePos.row--;
				cmdLine.setCursor(linePos);
			}
			else
			{
				cmdBuffer.overWrite(key.m_ascii);
				cmdTemplate.overWrite(key.m_ascii);
				cmdLine.putChar(key.m_ascii);
			}

			cmdBuffer.right();
			cmdTemplate.move(cmdBuffer.pos());
			cmdLine.cursorRight();
			m_history->setRecallMatch(0);
			m_history->initRecall();

			break;

		case CMD_IGNORE:
			break;
		}
	}	// End of main input loop

	pBuf = cmdBuffer.getPtr();	// point to start of input line

	if (strlen(pBuf) > 0)		// if not empty
	{
		m_history->addCommand(pBuf);	// add line to history

// Now scan the input line for separators. Each string on the line is
// added to the command queue.  If there are no separators, just one
// command is added to the queue.
		while (*pBuf != 0)
		{
			char* startCmd = pBuf;
			char end;

			while ( (*pBuf != SEPARATOR) && (*pBuf != 0))
				pBuf++;

			end = *pBuf;
			*pBuf = 0;
			m_queue->enqueue(startCmd);

			if (end == SEPARATOR)
				*pBuf++;
		}

// Dequeue the first command into the caller's buffer.
		m_queue->dequeue(buf);
	}
	else
		*buf = 0;		// if there was no input

epilog:
	bufLength = strlen(buf);	
	buf[bufLength++] = ENTER;	// append newline to buffer
	buf[bufLength] = 0;

	m_history->initRecall();
	m_history->setRecallMatch(0);

	Restore_Client_State(&saveRegs);	// restore client registers
	return bufLength;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKMachine::setInput
//	Member function getInput calls this function at various points to
//	set the current input line to a specified string.
//
// Parameters
//	s		pointer to string to set command line to
//	cmdLine		current DKCommandLine object
//	cmdBuffer	current DKBuffer object being used for input
//	cmdTemplate	current DKBuffer object being used as template
//
VOID DKMachine::setInput(char* s, DKCommandLine& cmdLine, DKBuffer& cmdBuffer,
				DKBuffer& cmdTemplate)
{
	if (s)
	{
		cmdBuffer.setCommand(s);
		cmdTemplate.setCommand(s);
		cmdLine.clear();
		cmdLine.putString(s);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKMachine::deleteCharacter
//	Delete a character at the current position from display, buffer,
//	and template.
//
// Parameters
//	cmdLine		the current DKCommandLine object
//	cmdBuffer	the current DKBuffer object for the input line
//	cmdTemplate	the current DKBuffer object for the command template
//
VOID DKMachine::deleteCharacter(DKCommandLine& cmdLine, DKBuffer& cmdBuffer,
				DKBuffer& cmdTemplate)
{
	POSITION linePos;

	cmdBuffer.del();

	cmdLine.erase();
	linePos = cmdLine.cursorPosition();
	cmdLine.clearToEnd();
	cmdLine.putString(cmdBuffer.getPtr() + cmdBuffer.pos());
	cmdLine.setCursor(linePos);

	if (&cmdTemplate)
		cmdTemplate.del();
}


