// Copyright (c) 1995, Compuware Corporation All Rights Reserved
//
// vdispatc.h - include file for class VControlMessageDispatcher
//

#ifndef __dispatch_h_
#define __dispatch_h_

class VControlMessageDispatcher
{
public:
	virtual DWORD OnControlMessage(
			       DWORD _Edi, 
			       DWORD _Esi, 
			       DWORD _Ebp, 
			       DWORD _Esp,
			       DWORD _Ebx,
			       DWORD _Edx,
			       DWORD _Ecx,
			       DWORD _Eax)=0;

};


#endif
