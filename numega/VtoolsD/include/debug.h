/*
   (C) Copyright MICROSOFT Corp. 1989-1990
   (c) Copyright Compuware Corporation, 1994
*/


#ifndef __debug_h_
#define __debug_h_

#ifdef WIN31
#define	VM_HANDLE(hvm) (TRUE)
#else
#define VM_HANDLE(hvm)		(((struct cb_s *)hvm)->CB_Signature == VMCB_ID)
#define THREAD_HANDLE(ptcb)	(((struct tcb_s *)ptcb)->TCB_Signature == SCHED_OBJ_ID_THREAD)
#endif


/* Could support:

Assert_Ints_Disabled
Assert_Ints_Enabled
Assert_Cur_VM_Handle
Assert_Client_Ptr

*/


/******************************************************************************
 *
 *   TrashThis
 *
 *  DESCRIPTION:
 *
 *	In DEBUG, trashes a variable to ensure that the
 *	caller is not relying on its value being preserved, or containing
 *	any particular value.  Does nothing in RETAIL.
 *
 *  PARAMETERS:
 *
 *  C:
 *	TrashThis1(a) - destroy variable `a'
 *	TrashThis2(a,b) - destroy variables `a' and `b'
 *	 ...
 *
 *****************************************************************************/

#ifdef	DEBUG
#define TrashThis1(a) (*(PDWORD)&(a) |= 0xFFFFFF80)
#define TrashThis2(a,b) (TrashThis1(a), TrashThis1(b))
#define TrashThis3(a,b,c) (TrashThis1(a), TrashThis2(b,c))
#define TrashThis4(a,b,c,d) (TrashThis1(a), TrashThis3(b,c,d))
#else
#define TrashThis1(a)
#define TrashThis2(a,b)
#define TrashThis3(a,b,c)
#define TrashThis4(a,b,c,d)
#endif


#ifdef DEBUG

#define	Begin_Touch_1st_Meg()	Enable_Touch_1st_Meg()
#define	End_Touch_1st_Meg()	Disable_Touch_1st_Meg()

#else

#define	Begin_Touch_1st_Meg()
#define	End_Touch_1st_Meg()

#endif

#ifdef WIN40
#ifdef DEBUG

#define ENTER_NOBLOCK() 	_Debug_Flags_Service(DFS_ENTER_NOBLOCK)
#define EXIT_NOBLOCK() 		_Debug_Flags_Service(DFS_EXIT_NOBLOCK)
#define ASSERT_CLD()		_Debug_Flags_Service(DFS_TEST_CLD)
#define ASSERT_MIGHT_BLOCK()	_Debug_Flags_Service(DFS_TEST_BLOCK)

/******************************************************************************
BeginDoc
 *
 *  BeginCProc
 *
 *  DESCRIPTION:
 *	This macro inserts inline assembly code to validate the current
 *	machine state, in the same manner that BeginProc does for assembly.
 *
 *	The following flags are supplied by default.
 *
 *	    DFS_LOG
 *	    DFS_TEST_CLD
 *
 *	You may combine any of the following flags.
 *
 *	   -DFS_LOG		// note minus sign!
 *	    DFS_NEVER_REENTER
 *	    DFS_TEST_REENTER
 *	   -DFS_TEST_CLD	// note minus sign!
 *	    DFS_NOT_SWAPPING
 *	    DFS_TEST_BLOCK
 *
 *  EXAMPLES:
 *
 *	BeginCProc(0);			// use the defaults
 *
 *	BeginCProc(DFS_TEST_REENTER);	// also check VMM reentrancy
 *
 *	BeginCProc(-DFS_LOG);		// don't log this procedure
 *
 *	BeginCProc(DFS_TEST_REENTER-DFS_LOG);
 *					// check VMM reentrancy, but don't log
 *
EndDoc
 *****************************************************************************/


#define BEGIN_CPROC(f) _Debug_Flags_Service(f+DFS_LOG+DFS_TEST_CLD)

#else					// in retail, everything is a NOP

#define ENTER_NOBLOCK()
#define EXIT_NOBLOCK()
#define ASSERT_CLD()
#define ASSERT_MIGHT_BLOCK()
#define BEGIN_CPROC(f)

#endif	// DEBUG

#endif	// WIN40


#pragma warning (default:4003)		// turn on not enough params warning


#endif			// __debug_h_
