//	V86VEND.C
//
//	Provide V86 Vendor-specific entry point service
//
//	Copyright (c) 1994 by Compuware Corporation
//

#include <vtoolsc.h>

PCHAR			pSavedVendorString = NULL;	// User unique string
SEGOFFSET		V86Callback	   = 0;		// V86 Callback Address
V86Call_THUNK		V86Call_Thunk	   = {0};	// Thunk for callback
V86Int_THUNK		V86Int_Thunk	   = {0};	// Thunk for Int 2F handler

//	OnInt2F
//
//	Called when Int 2F issued in V86 mode.
//
BOOL __stdcall OnInt2F(DWORD intNum, VMHANDLE hVM, PCLIENT_STRUCT pcrs)
{

// If it is an int 2F fcn 168Ah, and the string matches, return callback address

	if (pcrs->CWRS.Client_AX == 0x168A &&
	    (strcmp(Map_Flat(CLIENT_DS, CLIENT_SI), pSavedVendorString) == 0))
	{
		DEBUGTRACE("V86 Vendor Entry requested");

		pcrs->CRS.Client_ES = GET_SEGMENT(V86Callback);
		pcrs->CWRS.Client_DI = GET_OFFSET(V86Callback);
		pcrs->CBRS.Client_AL = 0;

		return TRUE;	// Ours
	}

	return FALSE;		// Not ours, chain to next handler
}

//	Allocate V86 Vendor Entry Point
//
// Args:
//	pVendorString	Points to unique (case sensitive) string for this VxD
//			This pointer is preserved, so the string must be
//			in persistant storage.
//	pCallback	Points to handler for this callback.
// Ret:
//			TRUE if successfull, else FALSE
// Remarks:
//	This function hooks V86 Int 2F.  If an Int 2f/ AX = 168A is detected,
//	then the string pointed to by ds:esi is examined.  If the string
//	matches pVendorString, then a callback address is returned to the
//	V86 client in es:di, and al is set to 0.
//
//	When the callback address is called, the user's pCallback is
//	invoked.
//
//	Note that this function may only be called once per VxD (currently).
//
BOOL __stdcall Set_V86_Vendor_Entry(CONST char * pVendorString,
				    PV86Call_HANDLER pCallback)
{
// This function uses static storage, and thus will only work once...
	if (pSavedVendorString) return FALSE;
	pSavedVendorString = pVendorString;

// Allocate a callback
	V86Callback = Allocate_V86_Call_Back(NULL, pCallback, &V86Call_Thunk);
	if (V86Callback == 0) {
		DEBUGWARN("No Callbacks, could not create V86 Vendor Entry");
		return FALSE;
	}

// Hook V86 Int 2F and return result

#ifdef DEBUG
	if (Hook_V86_Int_Chain(0x2F, OnInt2F, &V86Int_Thunk))
	{
		DEBUGTRACE("V86 Vendor Entry established");
		return TRUE;
	}
	else
	{
		DEBUGWARN("Failed to hook interrupt chain, could not create V86 Vendor Entry");
		return FALSE;
	}
#else
	return Hook_V86_Int_Chain(0x2F, OnInt2F, &V86Int_Thunk);
#endif
}
