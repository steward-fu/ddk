/*
 *
 *   (C) Copyright MICROSOFT Corp., 1989-1990
 *   (C) Copyright Compuware Corporation, 1994
 *
 */

#ifndef __vmd_h_
#define __vmd_h_

#ifdef WIN31

//
//   Mouse type definitions
//
#define	VMD_Type_Undefined	0
#define	VMD_Type_Bus		1
#define	VMD_Type_Serial		2
#define	VMD_Type_InPort		3
#define	VMD_Type_PS2		4
#define	VMD_Type_HP		5
#define	VMD_Type_Serial_w_port	(VMD_Type_Serial | 0x80)

//
//   Call-Out API definitions (passed in Client_CX)
//
#define	VMD_CO_API_Test_Inst		0
#define	VMD_CO_API_Get_Call_Back	1

#else




#define	VMD_Major_Version 0x04
#define	VMD_Minor_Version 0x00

#define	VMOUSE_Device_ID VMD_Device_ID
#define	VMOUSE_DEVICE_ID VMD_DEVICE_ID

#define	VMDINT33_Major_Version 0x08
#define	VMDINT33_Minor_Version 0x30


//
// The services provided by VMD for V86 and Prot mode apps.
//

#define	VMDAPI_GET_VERSION		0
#define	VMDAPI_UPDATE_MOUSE_STATE	1 // available in SysVM only
#define	VMDAPI_QUERY_WINDOWED_MOUSE_SUPPORT	2 // available in SysVM only
#define	VMDAPI_SET_MOUSE_EVENT_CALLBACK	3 // available in SysVM only
#define	VMDAPI_SET_MOUSE_FOCUS		4 // available in SysVM only
#define	VMDAPI_GET_MOUSE_INFO		5 // available in SysVM only
#define	VMDAPI_SET_MOUSE_TYPE		0x100
#define	VMDAPI_DETECT_MOUSE		0x101
#define	VMDAPI_GET_ADDON		0x102
#define	VMDAPI_MOUSEDRV_CONSISTENT	0x103

//
// Types of mice recognized by VMD.
//
#define	VMD_Type_Undefined	0
#define	VMD_Type_Bus		1
#define	VMD_Type_Serial		2
#define	VMD_Type_InPort		3
#define	VMD_Type_PS2		4
#define	VMD_Type_HP		5
#define	VMD_Type_Serial_w_port	(VMD_Type_Serial | 0x80)

//
// Data structure describing mouse device Flags, IO Base, IRQ number,
//  Mouse type, Mouse port number, mini driver reference, DevNode handle,
//  IRQ handle, interrupt routine, reset routine and disable routine.
// The mini mouse driver receives this data structure, does its detection
//  and sets appropriate flags and then fills in the HW int, reset and
//  disable routine addresses(zero address means NULL routine). The
//  reference data can be used in any way the mini driver wishes.
//

typedef struct Mouse_Instance {

	WORD	MI_Flags;		// status flags (defined below)
	WORD	MI_IO_Base;		// base to read
	BYTE	MI_IRQNumber;		// irq#
	BYTE	MI_PortNum;		// port number (serial mice ?)
	WORD	MI_MouseType;		// type of mouse (defined above)
	DWORD	MI_Reference;		// reference data
	DWORD	MI_hDevNode;		// dev node being handled
	DWORD	MI_hIRQ;		// VPICD's irq handle
	DWORD	MI_HWINT;		// hardware int proc
	DWORD	MI_Reset;		// Reset routine
	DWORD	MI_Disable;		// disable routine
	DWORD	MI_Initialize;		// initialization routine
						// It detects and virtualizes hardware.
	DWORD	MI_LoadHandle;		// load handle of loaded VxD.

} MOUSE_INSTANCE;

// The MI_Flags definitions
#define	MIF_Alloc_Bit		0	// Instance is allocated
#define	MIF_Alloc		(1 << MIF_Alloc_Bit)
#define	MIF_Active_Bit		1	// Interrupt is active
#define	MIF_Active		(1 << MIF_Active_Bit)
#define	MIF_Detected_Bit	2	// Device detected
#define	MIF_Detected		(1 << MIF_Detected_Bit)
#define	MIF_BadDevNode_Bit	3	// Device not detected
#define	MIF_BadDevNode		(1 << MIF_BadDevNode_Bit)
#define	MIF_NewConfig_Bit	4	// No DevNode for device
#define	MIF_NewConfig		(1 << MIF_NewConfig_Bit)
#define	MIF_BadIO_Bit		5	// Wrong IO in DevNode
#define	MIF_BadIO		(1 << MIF_BadIO_Bit)
#define	MIF_BadIRQ_Bit		6	// Wrong IRQ in DevNode
#define	MIF_BadIRQ		(1 << MIF_BadIRQ_Bit)
#define	MIF_NoIRQ_Bit		7	// Do not virtualize IRQ
#define	MIF_NoIRQ		(1 << MIF_NoIRQ_Bit)
#define	MIF_BUSMOUSE_Bit	8	// BUSMOUSE
#define	MIF_BUSMOUSE		(1 << MIF_BUSMOUSE_Bit)
#define	MIF_INPMOUSE_Bit	9	// INPORT
#define	MIF_INPMOUSE		(1 << MIF_INPMOUSE_Bit)
#define	MIF_AUXMOUSE_Bit	10	// AUX (PS/2) mouse
#define	MIF_AUXMOUSE		(1 << MIF_AUXMOUSE_Bit)
#define	MIF_OTHERMOUSE_Bit	11	// 3rd party mouse
#define	MIF_OTHERMOUSE		(1 << MIF_OTHERMOUSE_Bit)

//
// The DevNode for a Mouse device looks like follows:
//
// [...\*PNPXXXX]
// DeviceDesc=MouseType mouse
// Manufacturer=Name
// Class=SYSTEM
// MouseType=type of mouse (Serial,PS2,Inport,Bus etc.)
//
// BootConfig=.....
// [...\*PNPXXXX\DRV]
// DriverDesc=description of driver
// StaticVxD=*VMOUSE
// DriverName=DriverName	(Optional: present iff hardware handler is external)
// PropDriver=Proprietory driver name
//
// [...\*PNPXXXX\logConf]
// 0=...
//
// Till Device_Init, VMOUSE returns all New_DevNode calls with
// CR_DEVLOADER_NOT_READY.
//
// After the Device_Init:
//
// When VMOUSE gets a new devnode call for such a node,
// If the DriverName==*VMOUSE,
//  look at the MouseType and call the appropriate routine to detect the mouse
//  and configure the mouse support.
// else
//  load the VxD. Then call the SYS_DYNAMIC_DEVICE_INIT proc for the VxD
//  with EDX = 'VMOU' and ESI -> Mouse_Instance structure, EDI=0 . The VxD
//  returns with the Mouse_Instane struct initialized.
//  If a mouse is detected and configured successfully for this devNode,
//  then VCOMM looks at the PropDriver entry and loads it (If not already
//  loaded) and calls its SYS_DYNAMIC_DEVICE_INIT procedure with EDX = 'VMOU'
//  and ESI -> Mouse_Instance and EDI = 1.
// If by the start of the Init_Complete process, VMOUSE did not detect a mouse,
//  it searches for all mice it can handle and configure the mouse etc.
//  Now it remembers the type of the mouse found AND uses this information
//  at the start of the next boot (after it has gone through the above
//  process) for reducing the boot time.
//

typedef struct Mouse_Point_Struc {
	WORD	PS_Y;
	WORD	PS_X;
} MOUSE_POINT;

//
// Structs related to Cursor VxDs
//

//
// DisplayModeInfo struct is used by VMD to get information regarding
// the current display mode from Cursor Drawing VxD.
//

typedef struct DISPLAYMODEINFO_struct { 

	WORD	wSize;
	WORD	wFlags;
	MOUSE_POINT DMI_Virtual_Cursor_Maximum;
	BYTE	XChar;
	BYTE	YChar;
	MOUSE_POINT DMI_Cell_Dimensions;
	BYTE	cBitsPerPixel;
	WORD	wPageSize;
	BYTE	MaxBlocks;
	BYTE	XCursorMax;
	BYTE	YCursorMax;
	BYTE	bMaxTrail;
	BYTE	cDrawPages;

} DISPLAYMODEINFO;

//
// Bits of wFlags
//
#define	MI_GRAPHICSCURSOR	0x0001
#define	MI_COLORCURSOR		0x0002

//
// CursorProcInfo is used by Cursor drawing VxD to register a procedure
// which will be called by VMD to perform certain functions.
//

typedef struct CursorProcInfo {
	DWORD	CursorProcAddress;
	DWORD	NextCursorProcInfo;
	WORD	Video_Mode_Low;
	WORD	Video_Mode_High;
	BYTE	Priority;
	WORD	Flags;
} CURSORPROCINFO;

//
// Values for flags
//
#define	DC_SupportsOverride	0x0001

//
// TextCursor is used by VMD to inform the cursor drawing VxD about the
// the TEXT cursor set by an app.
//
// TextCursor is represented as follows:
//

typedef struct TEXTCURSOR_struct {
	WORD	cbSize;
	DWORD	dwOffset32Blocks;
	BYTE	bFlags;
	BYTE	cBlocks;
	WORD	XORMask;
	WORD	ANDMask;
} TEXTCURSOR;

//
// Values for bFlags
//

#define	BF_HARDWARE 0x0001

//
// GraphicsCursor is used by VMD to inform the cursor drawing VxD about
// the GRAPHICS cursor set by an app.
//
// GraphicsCursor is represented as follows:
//

typedef struct GRAPHICSCURSOR_struct {
	WORD	cbSize;
	DWORD	dwOffset32Masks;
	WORD	wWidth;
	WORD	wHeight;
	WORD	wHotX;
	WORD	wHotY;
	WORD	wType;			// monochrome or not
} GRAPHICSCURSOR;

//
// Struct for int 10 notifications. This is used by a cursor drawing VxD
// to tell VMD that it should be notified if a certain INT 10 Function
// call is made by an app.
//

typedef struct Int10Notify_Struc {
	BYTE	FunctionNumber;
	DWORD	NextNotifyStruc;
} INT10NOTIFY;

//
// Ordinals of the functions supported by the Cursor VxD
//

#define	SETTEXTCURSOR		0
#define	SETGRAPHICSCURSOR	1
#define	MOVECURSOR		2
#define	REMOVECURSOR		3
#define	SETDISPLAYMODE		4
#define	SETDISPLAYPAGE		5
#define	SAVERESTORESTATE	6
#define	GETDISPLAYMODEINFO	7
#define	QUERYDISPLAYMODE	8
#define	SETCONDITIONALREGION	9
#define	QUERYMEMORYUNDERCURSOR	10
#define	SETCURSORTRAIL		11
#define	RESETCURSORDRIVER	12

#define	DEFAULT_AND_MASK	0x77FF
#define	DEFAULT_XOR_MASK	0x7700

#define	TEXT_SMALL_BLOCK_SIZE	1

#define	TEXT_BYTES_PER_BLOCK	6	//The maximum number of bytes per block
					// in a text cursor.

#define	TEXT_SMALL_MASK_SIZE	(TEXT_SMALL_BLOCK_SIZE * TEXT_BYTES_PER_BLOCK)
					//The number of bytes of block data
					// a small text cursor needs.

#define	GR_SMALL_WIDTH	16	//The width (in pixels) of a small
					// graphics cursor.
#define	GR_SMALL_HEIGHT	16	//The height (in pixels) of a small
					// graphics cursor.

#define	GR_SMALL_MASK_SIZE	(GR_SMALL_WIDTH * GR_SMALL_HEIGHT / 8)
					//The size of a mask in a small
					// graphics cursor.

/*

BlockData	MACRO	xOff:REQ, yOff:REQ, xorMask, andMask

	BYTE	xOff, yOff		//Define the offset.

	IFNB	<xorMask>
		WORD	xorMask, andMask
	ENDIF

	ENDM
*/


typedef struct SMALLTEXTCURSOR_struct {		// A small text cursor.
	TEXTCURSOR tc;
	BYTE	Blocks[TEXT_SMALL_MASK_SIZE];
} SMALLTEXTCURSOR;

typedef struct SMALLGRAPHICSCURSOR_struct {		// A small monochrome graphics cursor.
	GRAPHICSCURSOR gc;
	BYTE Masks[GR_SMALL_MASK_SIZE*2];
} SMALLGRAPHICSCURSOR;

typedef union SMALLCURSOR_union {			// A small cursor, either text or
	SMALLGRAPHICSCURSOR	smgc;
	SMALLTEXTCURSOR		smtc;
} SMALLCURSOR;

#endif		// WIN40



#define	VMOUSE_Service	Declare_Service

Begin_Service_Table(VMD)

VMOUSE_Service	(VMD_Get_Version)
VMOUSE_Service	(VMD_Set_Mouse_Type)
VMOUSE_Service	(VMD_Get_Mouse_Owner)

#ifdef WIN40

VMOUSE_Service	(VMD_Post_Pointer_Message)
VMOUSE_Service	(VMD_Set_Cursor_Proc)
VMOUSE_Service	(VMD_Call_Cursor_Proc)
VMOUSE_Service	(VMD_Set_Mouse_Data)
VMOUSE_Service	(VMD_Get_Mouse_Data)
VMOUSE_Service	(VMD_Manipulate_Pointer_Message)
VMOUSE_Service	(VMD_Set_Middle_Button)
VMOUSE_Service	(VMD_Enable_Disable_Mouse_Events)
VMOUSE_Service	(VMD_Post_Absolute_Pointer_Message)

#endif

End_Service_Table(VMD)

#endif		//  __vmd_h_
