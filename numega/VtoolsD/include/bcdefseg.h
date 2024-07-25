// bcdefseg.h - set up default segments for Borland C/C++
// (c) Copyright 1994, by Compuware Corporation

#define LOCKED_SEG		1
#define INIT_SEG		2
#define PAGEABLE_SEG		3
#define STATIC_SEG		4
#define DEBUGONLY_SEG		5

#ifndef DEFSEG
#define DEFSEG 1
#endif

#if	DEFSEG == LOCKED_SEG
#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT
#define	DEFCODE _LTEXT
#define	DEFDATA _LDATA

#elif	DEFSEG == INIT_SEG
#include INIT_CODE_SEGMENT
#include INIT_DATA_SEGMENT

#elif	DEFSEG == PAGEABLE_SEG
#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

#elif	DEFSEG == STATIC_SEG
#include STATIC_CODE_SEGMENT
#include STATIC_DATA_SEGMENT

#elif	DEFSEG == DEBUGONLY_SEG
#include DEBUG_CODE_SEGMENT
#include DEBUG_DATA_SEGMENT

#endif

