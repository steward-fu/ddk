//
//
//   (C) Copyright MICROSOFT Corp., 1989-1990
//   (C) Copyright Compuware Corporation, 1994
//

#ifndef __vddgrb_h_
#define __vddgrb_h_


//***************
// Shell_Event definitions for the VDD
//
//   DO NOT CHANGE THESE EVER WITHOUT REDOING THE SHELL AND THE GRABBER
//	AND THE VDD!!!
//
#define	VDA_Type_Chng		((0x0400+20)+2)

#define	VDA_Display_Event	((0x0400+20)+6)

// On VDA_Display_Event event,
//   VMDOSAPP instance which gets the message has display event
//   wParam is a "subfunction code" which identifies the event.
//
#define	VDA_DE_DispChngMin	0
#define	VDA_DE_DispChngMax	80
#define	VDA_DE_ControlChngMin	VDA_DE_DispChngMax
#define	VDA_DE_ControlChngMax	160
#define	VDA_DE_ScreenGrabMin	VDA_DE_ControlChngMax
#define	VDA_DE_ScreenGrabMax	240

#define	VDA_DE_SpecialMin	0x0FFAF
#define	VDA_DE_SpecialMax	0x0FFFF
//
// Subfunctions >= VDA_DE_DispChngMin, < VDA_DE_DispChngMax indicate
//   that the display has changed without a change to the video controller
//   state.
//
// Subfunctions >= VDA_DE_ControlChngMin, < VDA_DE_ControlChngMax indicate
//   that the video controller state has changed.
//
// Subfunctions >= VDA_DE_ScreenGrabMin, < VDA_DE_ScreenGrabMax indicate
//   that a screen grab event has occured.
//
// Subfunctions >= VDA_DE_SpecialMin, < VDA_DE_SpecialMax indicate
//   private GRABBER events which VMDOSAPP just passes through to the
//   GRABBER
//
// This parameter is passed through VMDOSAPP to the display specific GRABBER.
//   "ranges" on the parameter allow specific info to be passed to the GRABBER
//   while allowing VMDOSAPP to know about the general high level aspect of the
//   event.
//


#endif			//  __vddgrb_h_
