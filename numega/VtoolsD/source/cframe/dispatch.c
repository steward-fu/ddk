//	DISPATCH.C
//
//	Provide dummy control dispatcher entry
//
//	Copyright (c) 1994 by Compuware Corporation
//

#include <vtoolsc.h>

BOOL __cdecl ControlDispatcher(
    DWORD dwControlMessage,	// Control message number from VMM (eax)
    DWORD EBX,		// Usually current or system VM handle
    DWORD EDX,		// Additional arguments for some messages
    DWORD ESI,		// appear in these
    DWORD EDI,		// registers
    DWORD ECX)
{
    return TRUE;
}

