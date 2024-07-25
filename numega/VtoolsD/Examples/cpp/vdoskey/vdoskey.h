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

// vdoskey.h - include file for VDOSKEY example
//
// Copyright (c) 1994  Compuware Corporation  All Rights Reserved


////////////////////////////////////////////////////////////////////////
// Basic defintions for VToolsD framework

#include <vtoolscp.h>
#define DEVICE_CLASS		DKDevice

#define VDOSKEY_DeviceID	UNDEFINED_DEVICE_ID
#define VDOSKEY_Major		1
#define VDOSKEY_Minor		0
#define VDOSKEY_Init_Order	UNDEFINED_INIT_ORDER

////////////////////////////////////////////////////////////////////////

#define BUFFERED_KEYBOARD_INPUT 0xA	// DOS function for keyboard input
#define MAXBUF		127		// max characters to input
#define SPACE		0x20		// ascii codes
#define TAB		0x09
#define ENTER		0xD
#define SEPARATOR	0x14		// ^T


// DKDevice - device class for VDOSKEY

class DKDevice : public VDevice
{
public:
	virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail);
	virtual BOOL OnCreateVM(VMHANDLE hVM);
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// VDOSKEY functions

typedef enum  {
	CMD_TERMINATOR,		// terminate input line
	CMD_SEPARATOR,		// separate command strings on input line
	CMD_ESCAPE,		// cancel current input, clear line
	CMD_ABORT,		// abort current input line
	CMD_PREVIOUS,		// recall previous line in history
	CMD_NEXT,		// recall next line in history
	CMD_SHOWHISTORY,	// show all lines in history
	CMD_CYCLEMATCH,		// match current input to history
	CMD_PROMPT,		// prompt for number of history line to recall
	CMD_DISPLAYOLDEST,	// display oldest line in history
	CMD_DISPLAYNEWEST,	// display newest line in history
	CMD_HOME,		// position to start of input lne
	CMD_END,		// position to end of input line
	CMD_LEFTCHAR,		// move one character to the left
	CMD_RIGHTCHAR,		// move one character to the right
	CMD_LEFTWORD,		// move one word to the left
	CMD_RIGHTWORD,		// move one word to the right
	CMD_RUBOUT,		// delete previous character 
	CMD_DELETE,		// delete current character
	CMD_DELETETOEND,	// delete from current postion to end of line
	CMD_TOGGLEINSERT,	// toggle insert/overwrite mode
	CMD_RECALLCHAR,		// recall character from template
	CMD_RECALLUPTOCHAR,	// recall from template up to specified char
	CMD_RECALLREMAINDER,	// recall template
	CMD_DELETETOCHAR,	// delete from current position up to char
	CMD_STORELINE,		// store current line in history without exec
	CMD_QUOTEEOF,		// input ^Z
	CMD_INPUTCHAR,		// add character to input at current position
	CMD_IGNORE		// ignore key
} DKCOMMAND;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  DKKey
// 	A DKKey object corresponds to a key typed by a user. 
//	Its data members include the ascii code and scan code
//	for the key, as well as flags indicating whether
//	a control key or alt key was pressed when the key
//	was pressed.   Its member functions are get(), which
//	reads a key from the keyboard, and map(), which maps
//	the key to a function.

class DKKey
{
public:
	VOID	get(CLIENT_STRUCT* pRegs);
	DKCOMMAND map();
	BYTE	m_ascii;
	BYTE	m_scan;
	BOOL	m_bCtrl;
	BOOL	m_bAlt;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKHistory	
//	A DKHistory object is a specialized list of strings,
//	each of which corresponds to a previously typed
//	command.  The member functions provide the ability
//	to add and remove strings to the history, and to
//	sequentially recall commands.  VDOSKEY creates a
//	DKHistory object for each virtual machine.  

//	The list is doubly linked to allow walking in either direction.
//	The forward link is provided by the base class VList, and the
//	back link by DKHistoryNode data member m_link.

class DKHistoryNode;

class DKHistory : public VList
{
public:
	DKHistory(INT maxCommands);
	~DKHistory();
	VOID	initRecall();
	VOID	setRecallMatch(char*);
	VOID	setRecallMatch(char*, INT);
	char*	recall(BOOL bNext=TRUE);
	BOOL	addCommand(char*);
	char*	oldest();
	char*	newest();

protected:
	VOID	removeTail();
	PCHAR	m_currentRecall;
	DKHistoryNode* m_currentNode;
	INT	m_maxRecall;
	INT	m_curLength;
	BOOL	m_bInit;
};

struct DKHistoryNode
{
	void* _cdecl operator new(size_t) {return m_pBaseList->newNode();};
	void  _cdecl operator delete(void*p) {m_pBaseList->deleteNode(p);};

	char*	m_pCommand;
	DKHistoryNode* m_link;
	static DKHistory* m_pBaseList;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandQueue
//	A DKCommandQueue object holds a series of commands
//	that the user entered on a single line using the
//	DOSKEY separator key (^T).  VDOSKEY keeps a queue
//	for each virtual machine.  If it is not empty when
//	the getInput routine is entered, a command is 
//	dequeued and used as the next command line.

class DKCommandQueue : public VList
{
public:
	DKCommandQueue();
	BOOL enqueue(char* pCommand);
	BOOL dequeue(char* pCommand);
	BOOL isEmpty() {return (first()==0);};
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// POSITION 
//	This class is a display position vector. It includes overloaded 
//	operators to advance or retract, and to compare two POSITIONS.
//
class POSITION
{
public:
	BYTE col;
	BYTE row;

	POSITION& operator ++();
	POSITION& operator --();
	BOOL operator < (POSITION&);
	BOOL operator > (POSITION&);
	BOOL operator == (POSITION&);
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKCommandLine
//	A DKCommandLine object provides an interface to the
//	display.  It includes member functions for operations
//	such as positioning the cursor, clearing the input
//	line from the display, and changing the cursor size.
//
class DKCommandLine
{
public:
	DKCommandLine(VMHANDLE hVM, CLIENT_STRUCT* pRegs);
	VOID	clear();
	VOID	clearToEnd();
	VOID	putChar(char ascii);
	VOID	putString(char* s);
	VOID	erase();
	VOID	cursorHome();
	VOID	cursorEnd();
	VOID	cursorLeft(INT cCols=1);
	VOID	cursorRight(INT cCols=1);
	VOID	cursorSize(BOOL bBlock);
	VOID	setCursor(POSITION&);
	POSITION cursorPosition();
	VOID	newline();

protected:
	VOID	moveTo(POSITION&);
	POSITION m_start;
	POSITION m_end;
	POSITION m_current;
	VMHANDLE m_hVM;
	CLIENT_STRUCT* m_regs;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKBuffer	
//	A DKBuffer stores a string.  Its data member store
//	not only the string data, but also the current 
//	position in the string, and a flag to indicate 
//	whether the current insert mode is insert or 
//	overwrite.  
//			
//	VDOSKEY uses a DKBuffer to buffer the current input
//	line, and also to hold the command template.  The
//	command template is used with functions
//	CMD_RECALLCHAR,	CMD_RECALLUPTOCHAR, and
//	CMD_RECALLREMAINDER, which correspond to DOSKEY
//	functions accessed by keys F1, F2, and F3.
//
class DKBuffer
{
public:
	DKBuffer(INT maxLen, BOOL bInsert=TRUE);
	~DKBuffer();
	VOID	clear() {m_curPos = 0; delToEnd();};
	INT	length() {return m_curLength;};
	VOID	left() {if (m_curPos) --m_curPos;};
	VOID	right() {m_curPos = min(m_curPos+1, m_maxLen-1);};
	INT	pos() {return m_curPos;};
	VOID	insert(char);
	VOID	overWrite(char);
	VOID	move(INT pos) {m_curPos = pos;};
	VOID	toStart() {m_curPos = 0;};
	VOID	toEnd() {m_curPos = m_curLength;};
	char	get() {return m_pBuf[m_curPos];};
	VOID	del();
	VOID	delToEnd() {m_curLength = m_curPos; m_pBuf[m_curLength] = 0;};
	char*	getPtr() {m_pBuf[m_curLength] = 0; return m_pBuf;};
	VOID	setCommand(char*);

protected:
	char*	m_pBuf;
	INT	m_maxLen;   	
	INT	m_curPos;
	INT	m_curLength;
	BOOL	m_bInsert;
};


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKMachine	
//	A DKMachine object corresponds to a virtual machine.
//	Each machine maintains a command queue, a command
//	history, and an insert mode flag.  The member 
//	functions handle system control messsages and 
//	provide input from the keyboard of command lines.
//		
class DKMachine : public VVirtualMachine
{
public:
	DKMachine(VMHANDLE);
	~DKMachine();
	BYTE getInput(CLIENT_STRUCT* pRegs, char* buf, INT bufSize);
	VOID setInput(char* command, DKCommandLine&, DKBuffer&, DKBuffer&);
	VOID deleteCharacter(DKCommandLine&, DKBuffer&, DKBuffer&);
	virtual BOOL OnVMInit();
	virtual VOID OnDestroyVM() {delete this;}

protected:
	DKCommandQueue*	m_queue;
	DKHistory*	m_history;
	BOOL		m_bInsert;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DKInt21
//	A DKInt21 object provides a pre-chain hook of INT 21h
//	occurring in V86 mode programs (such as command.com).
//	Its handler detects function 0Ah (Buffered Keyboard
//	Input) and invokes member function getInput of
//	the current DKMachine object.
//
class DKInt21 : public VPreChainV86Int
{
public:
	DKInt21();
	virtual BOOL handler(VMHANDLE, CLIENT_STRUCT*, DWORD);
};
