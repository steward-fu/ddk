//	PMVEND.C
//
//	Provide PM Vendor-specific entry point service
//
//	Copyright (c) 1994 by Compuware Corporation
//

#include <vtoolsc.h>

BOOL			bPMVInitialized    = FALSE;	// PM Vendor init'ed
PCHAR			pPMSavedVendorString = NULL;	// VxD unique string
SEGOFFSET		PMCallback	   = 0;		// PM Callback Address
PMCall_THUNK		PMCall_Thunk	   = {0};	// Thunk for callback

// In order to intercept Int 2F in protected mode, we allocate a callback,
// and hook the vector to the callback address.

PMCall_THUNK		PMInt_Thunk	   = {0};	// Thunk for Int 2F handler
SEGOFFSET		PMIntCallback	   = 0;		// Int 2F callback address
DWORD			Previous2FOffset   = 0;		// Old int-2f offset
WORD			Previous2FSegment  = 0;		// Old int-2f segment

// The following structures are used to track PM vendor entry information
// on a per VM basis.

typedef struct _vector_t
{
	DWORD dwOffset;	
	WORD wSelector;
} VECTOR, *PVECTOR;

typedef struct _PMVendorEntry_t
{
	struct _PMVendorEntry_t* pmve_chain;
	VMHANDLE pmve_hVM;
	VECTOR pmve_prev2f;
} PerVMVendorEntryInfo;

PerVMVendorEntryInfo* VMInfoHead = NULL;  // head of chain for per VM info

//	OnPMInt2F
//
//	Called when Int 2F issued in protected mode.
//
VOID __stdcall OnPMInt2F(VMHANDLE hVM, PVOID Refdata, PCLIENT_STRUCT pcrs)
{

	PerVMVendorEntryInfo* VMdata;

// If it is an int 2F fcn 168Ah, and the string matches, return callback address

	if (pcrs->CWRS.Client_AX == 0x168A &&
	    (strcmp(Map_Flat(CLIENT_DS, CLIENT_SI), pPMSavedVendorString) == 0))
	{

		DEBUGTRACE("Protected Mode Vendor Entry requested");

		pcrs->CRS.Client_ES = GET_SEGMENT(PMCallback);
		pcrs->CRS.Client_EDI = GET_OFFSET(PMCallback);
		pcrs->CBRS.Client_AL = 0;

		Simulate_Iret();
		return;
	}

// Not ours, chain to previous handler in this VM

	for (VMdata = VMInfoHead; VMdata != NULL; VMdata = VMdata->pmve_chain)	
		if (VMdata->pmve_hVM == hVM)
		{
			Simulate_Far_Jmp(VMdata->pmve_prev2f.wSelector, 
					 VMdata->pmve_prev2f.dwOffset);
			return;
		}

	DEBUGERROR("?! No VM entry for INT 2f chain");
}


//	Set PM Vendor Entry Point
//
// Args:
//	pVendorString	Points to unique (case sensitive) string for this VxD
//	pCallback	Points to handler for this callback.
// Ret:
//			TRUE if successfull, else FALSE
// Remarks:
//	This function hooks PM Int 2F.  If an Int 2f/ AX = 168A is detected,
//	then the string pointed to by ds:esi is examined.  If the string
//	matches pVendorString, then a callback address is returned to the
//	PM client in es:di, and al is set to 0.
//
//	When the callback address is called, the caller's pCallback is
//	invoked.
//
//	This function should be called when the VxD receives the control
//	message BEGIN_PM_APP.
//
BOOL __stdcall Set_PM_Vendor_Entry(CONST char * pVendorString,
				   PPMCall_HANDLER pCallback)
{
	PerVMVendorEntryInfo* VMdata;

// If the VxD has not yet initialized the PM Vendor entry, do so now.
	if (!bPMVInitialized)
	{

// Allocate a callback for applications to call

		PMCallback = Allocate_PM_Call_Back(NULL, pCallback, &PMCall_Thunk);
		if (PMCallback == 0) 
		{
			DEBUGWARN("No Callbacks, could not create PM Vendor Entry");
			return FALSE;
		}

// Store the supplied string

		if ( (pPMSavedVendorString = (PCHAR)_strdup(pVendorString)) == NULL)
		{
			DEBUGWARN("Unable to allocate storage for vendor ID string");
			return FALSE;
		}

// Allocate a callback for hooking the PM Int-2F chain

		PMIntCallback = Allocate_PM_Call_Back(NULL, OnPMInt2F, &PMInt_Thunk);
		if (PMIntCallback == 0) 
		{
			DEBUGWARN("PM Vendor Entry could get INT 2f callback");
			return FALSE;
		}

		bPMVInitialized = TRUE;
	}
	else
	{

// If already initialized, check for new string

		if (strcmp(pPMSavedVendorString, pVendorString) != 0)
		{
			DEBUGWARN("Changing PM vendor ID string");
			free(pPMSavedVendorString);
			pPMSavedVendorString = (PCHAR)_strdup(pVendorString);
			if (pPMSavedVendorString == NULL)
			{
				DEBUGWARN("Unable to allocate storage for vendor ID string");
				return FALSE;
			}
		}
	}

// See if there is already a struct allocated for this VM
	for (VMdata = VMInfoHead; VMdata != NULL; VMdata = VMdata->pmve_chain)
		if (VMdata->pmve_hVM == Get_Cur_VM_Handle())
			goto VMok;

// If no struct for this VM, allocate one and link it into the chain

	VMdata = (PerVMVendorEntryInfo*)malloc(sizeof(PerVMVendorEntryInfo));
	if (VMdata == NULL)
	{
		DEBUGWARN("No memory for PM vendor per VM data");
		return FALSE;
	}
	VMdata->pmve_hVM = Get_Cur_VM_Handle();
	VMdata->pmve_chain = VMInfoHead;
	VMInfoHead = VMdata;
VMok:

// Get previous INT 2F handler
	Get_PM_Int_Vector(0x2F, &VMdata->pmve_prev2f.wSelector, 
				&VMdata->pmve_prev2f.dwOffset);

// Install our Int 2F handler
	Set_PM_Int_Vector(0x2F, (WORD) GET_SEGMENT(PMIntCallback), GET_OFFSET(PMIntCallback));

	DEBUGTRACE("PM Vendor Entry established");
	return TRUE;
}


//	End PM Vendor Entry Point
//
// Args:
//	<none>
// Ret:
//	<none>			
// Remarks:
//	This function cleans up the VM specific data structures associated
//	with a PM vendor entry point. A VxD calls this entry point when
//	it receives control message END_PM_APP.
//	
VOID __stdcall End_PM_Vendor_Entry()
{
	VMHANDLE thisVM = Get_Cur_VM_Handle();
	PerVMVendorEntryInfo *VMdata, **pprev;
	DWORD i2foff;
	WORD i2fsel;

	DEBUGTRACE("Ending PM vendor entry for current VM");

// Find the entry for this VM

	for (pprev = &VMInfoHead, VMdata = VMInfoHead; 
	     VMdata != NULL; 
	     pprev = &VMdata->pmve_chain, VMdata = VMdata->pmve_chain) 
		if (VMdata->pmve_hVM == thisVM)
			break;

	if (VMdata == NULL)
	{
		DEBUGWARN("End_PM_Vendor_Entry did not find entry for VM");
		return;			// not found - nothing to do
	}

	Set_PM_Int_Vector(0x2F, VMdata->pmve_prev2f.wSelector,
				VMdata->pmve_prev2f.dwOffset);

	*pprev = VMdata->pmve_chain;	// unlink from chain
	free(VMdata);			// discard
}
