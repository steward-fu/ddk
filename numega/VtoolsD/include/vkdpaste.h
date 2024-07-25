/*
 *   (C) Copyright MICROSOFT Corp., 1989-1990
 *   (C) Copyright Compuware Corporation, 1994
 *
 */


#ifndef __vkdpaste_h_
#define __vkdpaste_h_

//
// structure of entries in paste buffer
//
typedef struct Paste_Rec {
	BYTE	Paste_Char;		// OEM char value
	BYTE	Paste_ScanCode;		// keyboard scan code
	WORD	Paste_ShiftState;	// shift state
} PASTE_REC;

// Paste_Key   equ word ptr Paste_Char


//
// API services:
//
#define VKD_API_Version 0
	//   ENTER:  EAX = 0
	//   EXIT:   AH = major version #
	//	    AL = minor version #
	//	    Carry clear

//
// Whenever the PM API changes in a backwards-compatible way,
// increment the low byte.
//
// Whenever the PM API changes in a way that is not backwards-compatible,
// increment the high byte and reset the low byte to zero.

#define VKD_PM_API_Version_Major 02h
#ifdef WIN40
#define VKD_PM_API_Version_Minor 01h
#else
#define	VKD_PM_API_Version_Minor 00h
#endif

#define VKD_PM_API_Version (VKD_PM_API_Version_Major * 256 + VKD_PM_API_Version_Minor)

#define VKD_Force_Key 1
	//   ENTER:  EAX = 1
	//	    EBX = VM handle  (0 for current)
	//	    CH = scan code
	//	    CL = repeat count
	//	    EDX = shift state  (-1 means no change)
	//   EXIT:   Carry set, if failed

//
// shift state equates.  All other bits are reserved and must be zero.
//
#define	FKSS_Insert_Down	0x8000
#define	FKSS_CapLck_Down	0x4000
#define	FKSS_NumLck_Down	0x2000
#define	FKSS_ScrLck_Down	0x1000
#define	FKSS_Insert_On		0x0080
#define	FKSS_CapLck_On		0x0040
#define	FKSS_NumLck_On		0x0020
#define	FKSS_ScrLck_On		0x0010
#define	FKSS_Alt_Down		0x0008
#define	FKSS_Ctrl_Down		0x0004
#define	FKSS_Shift_Down		0x0002
#ifdef WIN40
#define	FKSS_Extended_Key	0x0001 // New for API version 2.1
#endif



#endif			// __vkdpaste_h_
