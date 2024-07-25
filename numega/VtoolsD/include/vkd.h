//   (C) Copyright MICROSOFT Corp., 1988-1990
//   (C) Copyright Compuware Corporation, 1994
//

#ifndef __vkd_h_
#define __vkd_h_

#define	VKD_Version	0x020A

//shift state bits for both global & local shift states

#define SS_Shift	0x0001	// left or right
#define SS_LShift	0x0002 
#define SS_RShift	0x0200 
#define SS_Ctrl 	0x0080  // left or right
#define SS_LCtrl	0x0004 
#define SS_RCtrl	0x0400 
#define SS_Alt		0x0100  // left or right
#define SS_LAlt 	0x0008 
#define SS_RAlt 	0x0800 
#define SS_CapLock	0x0040 
#define SS_NumLock	0x0020 
#define SS_ScrlLock	0x0010 
#define SS_CapLock_Dn	0x4000  // SS_CapLock shl 8
#define SS_NumLock_Dn	0x2000  // SS_NumLock shl 8
#define SS_ScrlLock_Dn	0x1000  // SS_ScrlLock shl 8
#define SS_Unused	0x8000 

#define SS_Toggle_mask	    (SS_CapLock + SS_NumLock + SS_ScrlLock)
#define SS_Toggle_Dn_mask   (SS_CapLock_Dn + SS_NumLock_Dn + SS_ScrlLock_Dn)
#define SS_LShift_mask	    (SS_LShift + SS_LCtrl + SS_LAlt)
#define SS_Shift_mask	    (SS_LShift_mask + SS_RShift + SS_RCtrl + SS_RAlt)

#define	SS_Either_Shift     (SS_LShift | SS_RShift)
#define	SS_Either_Ctrl	    (SS_LCtrl | SS_RCtrl)
#define	SS_Either_Alt	    (SS_LAlt | SS_RAlt)

// LED state bits
#define	LED_ScLok	1		    // Scroll Lock
#define	LED_NmLok	2		    // Num Lock
#define	LED_CpLok	4		    // Caps Lock
#define	Toggle_To_LED	4		    // "state shr Toggle_To_LED" converts
					    // state bits into kybd LED bits

enum scanType_t {SCAN_NORMAL=0, SCAN_EXTENDED=1, SCAN_EITHER=0xFF};

// VKD_Define_Hot_Key equates & macro

#define	ExtendedKey_B	0x0001
#define ExtendedKey	0x0100
#define	AllowExtended_B 0x00FF
#define	AllowExtended	0xFF00

#define CallOnPress	0x01 	// Call call-back when key press is detected
#define CallOnRelease	0x02 	// Call call-back when key release is detected
				// (keyboard may still be in hot-key hold state)
#define CallOnRepeat	0x04	// Call call-back when repeated press is detected
#define CallOnComplete	0x08	// Call call-back when the hot key state is
				// ended (all shift modifier keys are released)
				// or when a different hot key is entered
				// (i.e. pressing ALT 1 2, if both ALT-1
				//  and ALT-2 are defined hot keys, then ALT-1's
				//  call back will be called before ALT-2's to
				// indicate that the ALT-1 is complete even
				// though the ALT key is still down)

#define CallOnUpDwn	(CallOnPress | CallOnRelease)
				// Call call-back on both press and release
#define CallOnAll	0	// Call call-back on press, release, auto-repeat
				// and complete
#define PriorityNotify	0x10 	// notification calls to the call-back can only
				// happen when interrupts are enabled and
				// the critical section is un-owned
#define Local_Key	0x80 	// key can be locally enabled/disabled

enum keyAction_t     {	Hot_Key_Pressed=0,
			Hot_Key_Released=1,
			Hot_Key_Repeated=2,
			Hot_Key_Completed=3
		};

#define	Hot_Key_SysVM_Notify		0x80
#define	Hot_Key_SysVM_Notify_bit	7

#define HKSS_Shift  (((~(SS_Either_Shift + SS_Toggle_mask)) << 16) + SS_Shift)
#define HKSS_Ctrl   (((~(SS_Either_Ctrl + SS_Toggle_mask)) << 16) + SS_Ctrl)
#define HKSS_Alt    (((~(SS_Either_Alt + SS_Toggle_mask)) << 16) + SS_Alt)
#define HKSS_CtrlAlt (((~(SS_Either_Alt + SS_Either_Ctrl + SS_Toggle_mask)) << 16) + SS_Alt + SS_Ctrl)

#include <VKDpaste.h>

// paste completion flags passed to the paste call-back

#define	Paste_Complete	0	// paste successful
#define	Paste_Aborted	1	// paste aborted by user
#define	Paste_VM_Term	2	// paste aborted because VM terminated

//
// Modifier byte for Message Keys, retrieved with services VKD_Get_Msg_Key and
// VKD_Peek_Msg_Key
//
#define	MK_Shift	0x02
#define	MK_Ctrl 	0x04
#define	MK_Alt		0x08
#define	MK_Extended	0x80

#define VKD_Service Declare_Service

Begin_Service_Table(VKD)

VKD_Service(VKD_Get_Version)
VKD_Service(VKD_Define_Hot_Key)
VKD_Service(VKD_Remove_Hot_Key)
VKD_Service(VKD_Local_Enable_Hot_Key)
VKD_Service(VKD_Local_Disable_Hot_Key)
VKD_Service(VKD_Reflect_Hot_Key)
VKD_Service(VKD_Cancel_Hot_Key_State)
VKD_Service(VKD_Force_Keys)
VKD_Service(VKD_Get_Kbd_Owner)
VKD_Service(VKD_Define_Paste_Mode)
VKD_Service(VKD_Start_Paste)
VKD_Service(VKD_Cancel_Paste)
VKD_Service(VKD_Get_Msg_Key)
VKD_Service(VKD_Peek_Msg_Key)
VKD_Service(VKD_Flush_Msg_Key_Queue)

#ifdef WIN40

VKD_Service(VKD_Enable_Keyboard)
VKD_Service(VKD_Disable_KeyBoard)
VKD_Service(VKD_Get_Shift_State)
VKD_Service(VKD_Filter_Keyboard_Input)
VKD_Service(VKD_Put_Byte)
VKD_Service(VKD_Set_Shift_State)

#endif		// WIN40

End_Service_Table(VKD)

#endif		// __vkd_h_
