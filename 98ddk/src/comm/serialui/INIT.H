//
// init.h: Declares data, defines and struct types for DLL entry point
//          module.
//
//

#ifndef __INIT_H__
#define __INIT_H__


/////////////////////////////////////////////////////  INCLUDES

/////////////////////////////////////////////////////  DEFINES

/////////////////////////////////////////////////////  MACROS

/////////////////////////////////////////////////////  TYPEDEFS


/////////////////////////////////////////////////////  EXPORTED DATA

extern HINSTANCE g_hinst;


#ifdef DEBUG

extern UINT g_uBreakFlags;       // Controls when to int 3
extern UINT g_uTraceFlags;       // Controls what trace messages are spewed
extern UINT g_uDumpFlags;        // Controls what structs get dumped

#endif

/////////////////////////////////////////////////////  PUBLIC PROTOTYPES

#endif // __INIT_H__

