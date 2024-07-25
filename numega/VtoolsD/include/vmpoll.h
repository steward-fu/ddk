/*
 *   (C) Copyright MICROSOFT Corp., 1988-1990
 *   (C) Copyright Compuware Corporation, 1994
 *
 */


//
//   Int 2Fh call-out API when system is idle
//	AX = 1607h
//	BX = VMPoll_Device_ID
//	CX = VMPoll_Call_Out_Sys_Idle
//
//   If TSR or device driver wants to "eat" the idle call, they should
//   set AX = 0 and *not* chain to other Int 2Fh hooks.	Otherwise, chain.
//
#define	VMPoll_Call_Out_Sys_Idle     0		// CX = 0
