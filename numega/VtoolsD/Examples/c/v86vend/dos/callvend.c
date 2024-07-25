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

// dosvend.c - DOS vendor api caller
//
// Copyright (c) 1994, Compuware Corporation 

// DOSVEND is a component of the V86VEND example.
// This DOS program finds and calls the vendor-specific
// entry point provided by the V86VEND VxD.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable:4704) // in-line asm warning
#endif

//----------------------------------------------------------------
//		System defines
//
typedef void VOID;
typedef int  BOOL;
typedef int  INT;
typedef char CHAR;

#define TRUE  1
#define FALSE 0


//----------------------------------------------------------------
//	Main entry point
//
//
void main(int argc, char *argv[])
{
#ifdef __BORLANDC__
#pragma warn -par
#endif

	char *pVendor = "Compuware unique vendor string";

	void far * devAddr;	// Device entry point address

	
// Get the device initialization point (fail if error)

	_asm {
	    mov	ax, 168Ah	// Find Vendor Entry from VxD
	    mov si, pVendor	// Point to special string
	    int	2fh
	    cmp al, 0		// Found?
	    jne NotFound		    
			 
	    mov	word ptr devAddr, di
	    mov	word ptr devAddr+2, es
	}
	if (devAddr == 0) {
	    fprintf(stderr, "Got bad vendor entry point (0)\n");
	    exit(1);
	}

	_asm {
	    call [devAddr]	// Call the VxD
	}

	printf("Sucessfully called vendor entry point for string <%s>\n", pVendor);
	exit(0);

NotFound:
	printf("Could not find vendor entry point for string <%s>\n", pVendor);
	exit(1);

}
