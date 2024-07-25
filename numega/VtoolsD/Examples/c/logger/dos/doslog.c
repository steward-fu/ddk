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

// doslog.c - DOS event log reader/writer
//
// Copyright (c) 1994, Compuware Corporation 

// DOSLOG is a component of the demonstration VxD LOGGER.
// This DOS program uses the VxD services provided by LOGGER
// to read or write the event log.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable:4704)	// in-line asm warning
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
//		Project defines
//
#define MAX_EVENT_SIZE 511


//----------------------------------------------------------------
//		Prototypes
//
VOID Usage(void);
VOID Options(int argc, char *argv[]);
VOID ProcessEvents(void);
VOID Initialize(void);
BOOL ReadEvent(void);

//----------------------------------------------------------------
//		Global variables
//
BOOL	bReading;	// TRUE if reading events, FALSE if writing
INT	nEvents = 1;	// Number of events to process
INT	nGoodEvents;	// Number of events processed succesfully
void far * devAddr;	// Device entry point address
INT	eventLength;	// Number of bytes in an event
CHAR	eventBuffer[MAX_EVENT_SIZE];




//----------------------------------------------------------------
//	Main entry point
//
//
int main(int argc, char *argv[])
{
	Options(argc, argv);
	Initialize();
	ProcessEvents();
	printf("\nDOSLOG %s %d %s out of %d requested\n",
	       (bReading ? "read" : "wrote"),
	       nGoodEvents,
	       (nGoodEvents == 1 ? "event" : "events"),
	       nEvents);

	return 0;
}


//----------------------------------------------------------------
//	Initialize
//
//
VOID Initialize()
{

#ifdef TEST
	return;
#endif

// Get the device initialization point (fail if error)

	_asm {
	    mov	ax, 1684h	// Find Device V86 API from V86
	    mov	bx, 3180h	// NUMEGA_TEST_ID
	    int	2fh
	    mov	word ptr devAddr, di
	    mov	word ptr devAddr+2, es
	}
	if (devAddr == 0) {
	    fprintf(stderr, "Failed to get device entry point,\n"
			    "check to ensure that the LOGGER VxD is loaded\n");
	    exit(1);
	}
}

//----------------------------------------------------------------
//	Read event
//
//
BOOL ReadEvent()
{

#ifdef TEST
  printf("read one event\n");
  return TRUE;
#endif
	_asm {
	    mov	ah, 3			// Get one event
	    mov bx, offset eventBuffer	// into buffer
	    call [devAddr]
	    mov eventLength, cx
	}

	if (eventLength == 0) return FALSE;

	eventBuffer[eventLength] = '\0';	// Null-terminate event
	printf("<%s> ", eventBuffer);		// Print event
	return TRUE;
}


//----------------------------------------------------------------
//	Write event
//
//
BOOL WriteEvent(int n)
{
	char theEvent[MAX_EVENT_SIZE];

#ifdef TEST
 printf("write one event\n");
 return TRUE;
#endif
	sprintf(theEvent, "DOS event %d", n);

	_asm {
	    mov	ah, 2		// Write one string
	    lea bx, theEvent	// into buffer
	    call [devAddr]
	}
	return TRUE;
}


//----------------------------------------------------------------
//	Process events
//
//
VOID ProcessEvents()
{
	INT	i;
	BOOL	bOK;


	for (i=0; i<nEvents; i++)
	{
		if (bReading)
			bOK = ReadEvent();
		else
			bOK = WriteEvent(i);
		if (!bOK) return;
		nGoodEvents++;
	}
}



//----------------------------------------------------------------
//	Print usage message and exit
//
//
VOID Usage()
{
	fprintf(stderr, "Usage: DOSLOG [r,w] <number of events>\n"
			"       r means read event(s)\n"
			"       w means write event(s)\n"
			"       default number of events is 1\n");
	exit(1);
}


//----------------------------------------------------------------
//	Parse options from command line
//	Set global variables to reflect options
//
VOID Options(int argc, char *argv[])
{
	char	*s;


	if (argc <= 1) Usage();

	s = argv[1];
	if (strlen(s) != 1) Usage();

// token is R or W
	if      (*s == 'R' || *s == 'r') bReading = TRUE;
	else if (*s == 'W' || *s == 'w') bReading = FALSE;
	else Usage();

	if (argc == 3)
	{
		nEvents = atoi(argv[2]);
		if (nEvents <= 0) Usage();
	}

	if (argc > 3) Usage();
}
