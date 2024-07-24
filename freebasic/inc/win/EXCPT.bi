#pragma once

#if (not defined(__FB_64BIT__)) and (defined(__FB_WIN32__) or defined(__FB_CYGWIN__))
	extern "C"
#endif

#define _CRTAPI1 __cdecl
#define _CRTAPI2 __cdecl

type _EXCEPTION_DISPOSITION as long
enum
	ExceptionContinueExecution
	ExceptionContinueSearch
	ExceptionNestedException
	ExceptionCollidedUnwind
end enum

type EXCEPTION_DISPOSITION as _EXCEPTION_DISPOSITION

#if defined(__FB_DOS__) or ((not defined(__FB_64BIT__)) and (defined(__FB_DARWIN__) or ((not defined(__FB_ARM__)) and (defined(__FB_LINUX__) or defined(__FB_FREEBSD__) or defined(__FB_OPENBSD__) or defined(__FB_NETBSD__)))))
	'' TODO: EXCEPTION_DISPOSITION __cdecl _except_handler ( struct _EXCEPTION_RECORD *ExceptionRecord, void * EstablisherFrame, struct _CONTEXT *ContextRecord, void * DispatcherContext );
#elseif (not defined(__FB_64BIT__)) and (defined(__FB_WIN32__) or defined(__FB_CYGWIN__))
	declare function _except_handler(byval ExceptionRecord as _EXCEPTION_RECORD ptr, byval EstablisherFrame as any ptr, byval ContextRecord as _CONTEXT ptr, byval DispatcherContext as any ptr) as EXCEPTION_DISPOSITION
#endif

const EXCEPTION_EXECUTE_HANDLER = 1
const EXCEPTION_CONTINUE_SEARCH = 0
const EXCEPTION_CONTINUE_EXECUTION = -1
#define _INC_EXCPT

#if (not defined(__FB_64BIT__)) and (defined(__FB_WIN32__) or defined(__FB_CYGWIN__))
	end extern
#endif
