/*
 *	Microsoft Confidential
 *	Copyright (C) Microsoft Corporation 1991
 *	All Rights Reserved.
 */

/***	ERROR.H - DOS Error Codes
 *
 *    The DOS 2.0 and above "XENIX-style" calls return error codes
 *    through AX.  If an error occurred then the carry bit will be set
 *    and the error code is in AX.  If no error occurred then the
 *    carry bit is reset and AX contains returned info.
 *
 *    Since the set of error codes is being extended as we extend the operating
 *    system, we have provided a means for applications to ask the system for a
 *    recommended course of action when they receive an error.
 *
 *    The GetExtendedError system call returns a universal error, an error
 *    location and a recommended course of action.  The universal error code is
 *    a symptom of the error REGARDLESS of the context in which GetExtendedError
 *    is issued.
 */


/*
 *	2.0 error codes
 */

#define	NO_ERROR			0	/* Success */
#define	ERROR_INVALID_FUNCTION		1
#define	ERROR_FILE_NOT_FOUND		2
#define	ERROR_PATH_NOT_FOUND		3
#define	ERROR_TOO_MANY_OPEN_FILES	4
#define	ERROR_ACCESS_DENIED		5
#define	ERROR_INVALID_HANDLE		6
#define	ERROR_ARENA_TRASHED		7
#define	ERROR_NOT_ENOUGH_MEMORY		8
#define	ERROR_INVALID_BLOCK		9
#define	ERROR_BAD_ENVIRONMENT		10
#define	ERROR_BAD_FORMAT		11
#define	ERROR_INVALID_ACCESS		12
#define	ERROR_INVALID_DATA		13
/***** reserved			EQU	14	; *****/
#define	ERROR_INVALID_DRIVE		15
#define	ERROR_CURRENT_DIRECTORY		16
#define	ERROR_NOT_SAME_DEVICE		17
#define	ERROR_NO_MORE_FILES		18

/*
 *	These are the universal int 24 mappings for
 *	the old INT 24 set of errors.
 */

#define	ERROR_WRITE_PROTECT		19
#define	ERROR_BAD_UNIT			20
#define	ERROR_NOT_READY			21
#define	ERROR_BAD_COMMAND		22
#define	ERROR_CRC			23
#define	ERROR_BAD_LENGTH		24
#define	ERROR_SEEK			25
#define	ERROR_NOT_DOS_DISK		26
#define	ERROR_SECTOR_NOT_FOUND		27
#define	ERROR_OUT_OF_PAPER		28
#define	ERROR_WRITE_FAULT		29
#define	ERROR_READ_FAULT		30
#define	ERROR_GEN_FAILURE		31

/*
 *	The 3.0 and 4.0 error codes reported through INT 24.
 */

#define	ERROR_SHARING_VIOLATION		32
#define	ERROR_LOCK_VIOLATION		33
#define	ERROR_WRONG_DISK		34
#define	ERROR_FCB_UNAVAILABLE		35
#define	ERROR_SHARING_BUFFER_EXCEEDED	36
#define	ERROR_CODE_PAGE_MISMATCHED	37    /* DOS 4.00 */
#define	ERROR_HANDLE_EOF		38    /* DOS 4.00 */
#define	ERROR_HANDLE_DISK_FULL		39    /* DOS 4.00 */

/*
 * These are the new volume locking error codes reported for Windows 95
 */
#define	error_volume_locked		40
#define	error_invalid_volume_lock	41
#define	error_volume_lock_failed	42

/*
 *	OEM network-related errors are 50-79.
 */

#define	ERROR_NOT_SUPPORTED		50

#define	ERROR_NET_ACCESS_DENIED		65
#define ERROR_BAD_NET_NAME		67 /* Network name not found */

/*
 *	End of INT 24 reportable errors
 */


#define	ERROR_FILE_EXISTS		80
#define	ERROR_DUP_FCB			81
#define	ERROR_CANNOT_MAKE		82
#define	ERROR_FAIL_I24			83

/*
 *	3.0 and 4.0 network related error codes.
 */

#define	ERROR_OUT_OF_STRUCTURES		84
#define	ERROR_ALREADY_ASSIGNED		85
#define	ERROR_INVALID_PASSWORD		86
#define	ERROR_INVALID_PARAMETER		87
#define	ERROR_NET_WRITE_FAULT		88
#define	ERROR_SYS_COMP_NOT_LOADED	90	/* DOS 4.00 */

/*
 *	The following errors exist in C12 version of error.h. Apparently
 *	ERROR_SYS_COMP_NOT_LOADED got stepped on.
 */

#define ERROR_NOT_FROZEN        90
#define ERR_TSTOVFL         91  /* timer service table overflow */
#define ERR_TSTDUP          92  /* timer service table duplicate */
#define ERROR_NO_ITEMS          93  /* There were no items to operate upon */
#define ERROR_INTERRUPT         95  /* interrupted system call */

#define ERROR_TOO_MANY_SEMAPHORES   100
#define ERROR_EXCL_SEM_ALREADY_OWNED    101
#define ERROR_SEM_IS_SET        102
#define ERROR_TOO_MANY_SEM_REQUESTS 103
#define ERROR_INVALID_AT_INTERRUPT_TIME 104

#define ERROR_SEM_OWNER_DIED        105 /* waitsem found owner died */
#define ERROR_SEM_USER_LIMIT        106 /* too many procs have this sem */
#define ERROR_DISK_CHANGE       107 /* insert disk b into drive a */
#define ERROR_DRIVE_LOCKED      108 /* drive locked by another process */
#define ERROR_BROKEN_PIPE       109 /* write on pipe with no reader */
#define ERROR_OPEN_FAILED       110 /* open/created failed due to */

/*
 *	OS/2-compatible error codes.
 *	NOTE: values in the range 100 to 599 may be defined
 *	by OS/2.  Do NOT add NEW error codes in this range.
 */

#define ERROR_BUFFER_OVERFLOW		111	/* Buffer too small for data */

/*
 *	More C12 errors.
 */
#define ERROR_DISK_FULL         112 /* not enough space on the disk */
                        /* (DOSNEWSIZE/w_NewSize) */
#define ERROR_NO_MORE_SEARCH_HANDLES    113 /* can't allocate another search */
                        /* structure and handle. */
                        /* (DOSFINDFIRST/w_FindFirst) */
#define ERROR_INVALID_TARGET_HANDLE 114 /* Target handle in DOSDUPHANDLE */
                        /* is invalid */
#define ERROR_PROTECTION_VIOLATION  115 /* Bad user virtual address */
#define ERROR_VIOKBD_REQUEST        116
#define ERROR_INVALID_CATEGORY      117 /* Category for DEVIOCTL in not */
                        /* defined */
#define ERROR_INVALID_VERIFY_SWITCH 118 /* invalid value passed for */
                        /* verify flag */
#define ERROR_BAD_DRIVER_LEVEL      119 /* DosDevIOCTL looks for a level */
                        /* four driver.   If the driver */
                        /* is not level four we return */
                        /* this code */
#define ERROR_CALL_NOT_IMPLEMENTED  120 /* returned from stub api calls. */
                        /* This call will disappear when */
                        /* all the api's are implemented. */
#define ERROR_SEM_TIMEOUT       121 /* Time out happened from the */
                        /* semaphore api functions. */
#define ERROR_INSUFFICIENT_BUFFER   122 /* Some call require the  */
                        /* application to pass in a buffer */
                        /* filled with data.  This error is */
                        /* returned if the data buffer is too */
                        /* small.  For example: DosSetFileInfo */
                        /* requires 4 bytes of data.  If a */
                        /* two byte buffer is passed in then */
                        /* this error is returned.   */
                        /* error_buffer_overflow is used when */
                        /* the output buffer in not big enough. */
#define ERROR_INVALID_NAME      123 /* illegal character or malformed */
                        /* file system name */
#define ERROR_INVALID_LEVEL     124 /* unimplemented level for info */
                        /* retrieval or setting */
#define ERROR_NO_VOLUME_LABEL       125 /* no volume label found with */
                        /* DosQFSInfo command */
#define ERROR_MOD_NOT_FOUND     126 /* w_getprocaddr,w_getmodhandle */
#define ERROR_PROC_NOT_FOUND        127 /* w_getprocaddr */

#define ERROR_WAIT_NO_CHILDREN      128 /* CWait finds to children */

#define ERROR_CHILD_NOT_COMPLETE    129 /* CWait children not dead yet */

/*This is a temporary fix for the 4-19-86 build this should be changed when */
/* we get the file from MS */
#define ERROR_DIRECT_ACCESS_HANDLE  130 /* handle operation is invalid */
                        /* for direct disk access */
                        /* handles */
#define ERROR_NEGATIVE_SEEK     131 /* application tried to seek  */
                        /* with negative offset */
#define ERROR_SEEK_ON_DEVICE        132 /* application tried to seek */
                        /* on device or pipe */
/* */
/* The following are errors generated by the join and subst workers */
/* */
#define ERROR_IS_JOIN_TARGET        133
#define ERROR_IS_JOINED         134
#define ERROR_IS_SUBSTED        135
#define ERROR_NOT_JOINED        136
#define ERROR_NOT_SUBSTED       137
#define ERROR_JOIN_TO_JOIN      138
#define ERROR_SUBST_TO_SUBST        139
#define ERROR_JOIN_TO_SUBST     140
#define ERROR_SUBST_TO_JOIN     141
#define ERROR_BUSY_DRIVE        142
#define ERROR_SAME_DRIVE        143
#define ERROR_DIR_NOT_ROOT      144
#define ERROR_DIR_NOT_EMPTY     145
#define ERROR_IS_SUBST_PATH     146
#define ERROR_IS_JOIN_PATH      147
#define ERROR_PATH_BUSY         148
#define ERROR_IS_SUBST_TARGET       149
#define ERROR_SYSTEM_TRACE      150 /* system trace error */
#define ERROR_INVALID_EVENT_COUNT   151 /* DosMuxSemWait errors */
#define ERROR_TOO_MANY_MUXWAITERS   152
#define ERROR_INVALID_LIST_FORMAT   153
#define ERROR_LABEL_TOO_LONG        154
#define ERROR_TOO_MANY_TCBS     155
#define ERROR_SIGNAL_REFUSED        156
#define ERROR_DISCARDED         157
#define ERROR_NOT_LOCKED        158
#define ERROR_BAD_THREADID_ADDR     159
#define ERROR_BAD_ARGUMENTS     160
#define ERROR_BAD_PATHNAME      161
#define ERROR_SIGNAL_PENDING        162
#define ERROR_UNCERTAIN_MEDIA       163
#define ERROR_MAX_THRDS_REACHED     164
#define ERROR_MONITORS_NOT_SUPPORTED    165

#define ERROR_INVALID_SEGMENT_NUMBER    180
#define ERROR_INVALID_CALLGATE      181
#define ERROR_INVALID_ORDINAL       182
#define ERROR_ALREADY_EXISTS        183
#define ERROR_NO_CHILD_PROCESS      184
#define ERROR_CHILD_ALIVE_NOWAIT    185
#define ERROR_INVALID_FLAG_NUMBER   186
#define ERROR_SEM_NOT_FOUND     187

/*  following error codes have added to make the loader error
    messages distinct
*/

#define ERROR_INVALID_STARTING_CODESEG  188
#define ERROR_INVALID_STACKSEG      189
#define ERROR_INVALID_MODULETYPE    190
#define ERROR_INVALID_EXE_SIGNATURE 191
#define ERROR_EXE_MARKED_INVALID    192
#define ERROR_BAD_EXE_FORMAT        193
#define ERROR_ITERATED_DATA_EXCEEDS_64k 194
#define ERROR_INVALID_MINALLOCSIZE  195
#define ERROR_DYNLINK_FROM_INVALID_RING 196
#define ERROR_IOPL_NOT_ENABLED      197
#define ERROR_INVALID_SEGDPL        198
#define ERROR_AUTODATASEG_EXCEEDS_64k   199
#define ERROR_RING2SEG_MUST_BE_MOVABLE  200
#define ERROR_RELOC_CHAIN_XEEDS_SEGLIM  201
#define ERROR_INFLOOP_IN_RELOC_CHAIN    202

#define ERROR_ENVVAR_NOT_FOUND      203
#define ERROR_NOT_CURRENT_CTRY      204
#define ERROR_NO_SIGNAL_SENT        205
#define ERROR_FILENAME_EXCED_RANGE  206 /* if filename > 8.3 */
#define ERROR_RING2_STACK_IN_USE    207 /* for FAPI */
#define ERROR_META_EXPANSION_TOO_LONG   208 /* if "*a" > 8.3 */

#define ERROR_INVALID_SIGNAL_NUMBER 209
#define ERROR_THREAD_1_INACTIVE     210
#define ERROR_INFO_NOT_AVAIL        211 /*@@ PTM 5550 */
#define ERROR_LOCKED            212
#define ERROR_BAD_DYNALINK      213 /*@@ PTM 5760 */
#define ERROR_TOO_MANY_MODULES      214
#define ERROR_NESTING_NOT_ALLOWED   215


#define ERROR_BAD_PIPE			230	/* No such pipe or bad oper. */
#define ERROR_PIPE_BUSY			231	/* Pipe is busy */
#define ERROR_PIPE_NOT_CONNECTED	233	/* Server disconnected pipe */
#define ERROR_MORE_DATA 		234	/* More data is available */


/*
 *	DOS 6.0 error codes.
 */

#define	ERROR_IO_PENDING		600	/* BUGBUG: NT value? */
#define	ERROR_TIMEOUT			601	/* BUGBUG: NT value? */
#define	ERROR_PIPE_CLOSED		602	/* BUGBUG: NT value? */
#define	ERROR_PIPE_LISTENING		603	/* BUGBUG: NT value? */
#define	ERROR_TOO_MANY_INSTANCES	604	/* BUGBUG: NT value? */
#define	ERROR_INVALID_PROVIDER		605
#define	ERROR_VOLUME_EXISTS		606	/* MOUNT Volume already exists */
#define	ERROR_VOLUME_HARD_ERROR		607	/* MOUNT Hard error reading volume */
#define	ERROR_VOLUME_UNRECOGNIZED	608	/* MOUNT Fsd doesn't recognize media file system */


/*
 *	Long-name API error codes.  The values were selected for backwards
 *  compatibility with previous versions of DOS.
 */

#define ERROR_LN_GENERIC_NOT_SUP	0x7100	/* Generic long-name API not supported   */
#define ERROR_LN_FIND_CLOSE_NOT_SUP	0x7200  /* Long-name FindClose API not supported */

/*
 * Error codes for DVD copy protection
 */

#define ERROR_DVD_SESSION_FAILURE	0xDD00	/* the AGID (SessionID) is not valid, or
											   there are no AGIDs available in the system*/
#define ERROR_DVD_KEY_NOT_PRESENT	0xDD01	/* the file / disc is not protected with a key*/
#define ERROR_DVD_KEY_NOT_VALID		0xDD02	/* the key provided was not valid			 */
#define ERROR_DVD_INVALID_REGION	0xDD03	/* region of the media is incorrect for the
												player */
#define ERROR_DVD_INVALID_RESET_COUNT 0xDD04/* no region reset counts available*/


/*
 *	Int24 Error Codes
 */

#define	ERROR_I24_WRITE_PROTECT		0
#define	ERROR_I24_BAD_UNIT		1
#define	ERROR_I24_NOT_READY		2
#define	ERROR_I24_BAD_COMMAND		3
#define	ERROR_I24_CRC			4
#define	ERROR_I24_BAD_LENGTH		5
#define	ERROR_I24_SEEK			6
#define	ERROR_I24_NOT_DOS_DISK		7
#define	ERROR_I24_SECTOR_NOT_FOUND	8
#define	ERROR_I24_OUT_OF_PAPER		9
#define	ERROR_I24_WRITE_FAULT		0x0A
#define	ERROR_I24_READ_FAULT		0x0B
#define	ERROR_I24_GEN_FAILURE		0x0C
#define	ERROR_I24_DISK_CHANGE		0x0D
#define	ERROR_I24_WRONG_DISK		0x0F
#define	ERROR_I24_UNCERTAIN_MEDIA	0x10
#define	ERROR_I24_CHAR_CALL_INTERRUPTED	0x11
#define	ERROR_I24_NO_MONITOR_SUPPORT	0x12
#define	ERROR_I24_INVALID_PARAMETER	0x13

/*
 *	THE FOLLOWING ARE MASKS FOR THE AH REGISTER ON Int 24
 *
 *	NOTE: ABORT is ALWAYS allowed
 */

#define	ALLOWED_FAIL			0x08
#define	ALLOWED_RETRY			0x10
#define	ALLOWED_IGNORE			0x20

#define	I24_OPERATION			0x01	/* Z if READ,NZ if Write */
#define	I24_AREA			0x06	/* 0x00 if DOS
						 * 0x02 if FAT
						 * 0x04 if root DIR
						 * 0x06 if DATA
						 */
#define	I24_CLASS			0x80	/* Z = DISK, NZ = FAT or char */


/***	GetExtendedError CLASSes ACTIONs LOCUSs
 *
 *	The GetExtendedError call takes an error code and returns CLASS,
 *	ACTION and LOCUS codes to help programs determine the proper action
 *	to take for error codes that they don't explicitly understand.
 */


/*
 *	Values for error CLASS.
 */

#define	ERRCLASS_OUTRES			1	/* Out of Resource */
#define	ERRCLASS_TEMPSIT		2	/* Temporary Situation */
#define	ERRCLASS_AUTH			3	/* Permission problem */
#define	ERRCLASS_INTRN			4	/* Internal System Error */
#define	ERRCLASS_HRDFAIL		5	/* Hardware Failure */
#define	ERRCLASS_SYSFAIL		6	/* System Failure */
#define	ERRCLASS_APPERR			7	/* Application Error */
#define	ERRCLASS_NOTFND			8	/* Not Found */
#define	ERRCLASS_BADFMT			9	/* Bad Format */
#define	ERRCLASS_LOCKED			10	/* Locked */
#define	ERRCLASS_MEDIA			11	/* Media Failure */
#define	ERRCLASS_ALREADY		12	/* Collision with Existing Item */
#define	ERRCLASS_UNK			13	/* Unknown/other */
#define	ERRCLASS_CANT			14
#define	ERRCLASS_TIME			15

/*
 *	Values for error ACTION.
 */

#define	ERRACT_RETRY			1	/* Retry */
#define	ERRACT_DLYRET			2	/* Retry after delay */
#define	ERRACT_USER			3	/* Ask user to regive info */
#define	ERRACT_ABORT			4	/* abort with clean up */
#define	ERRACT_PANIC			5	/* abort immediately */
#define	ERRACT_IGNORE			6	/* ignore */
#define	ERRACT_INTRET			7	/* Retry after User Intervention */

/*
 *	Values for error LOCUS.
 */

#define	ERRLOC_UNK			1	/* No appropriate value */
#define	ERRLOC_DISK			2	/* Random Access Mass Storage */
#define	ERRLOC_NET			3	/* Network */
#define	ERRLOC_SERDEV			4	/* Serial Device */
#define	ERRLOC_MEM			5	/* Memory */

/*
 *	error codes to be returned in (al) on int 25/26
 */

#define	NO_RESPONSE		0x80
#define	SEEK_FAILED		0x40
#define	CNTRLR_FAILED	0x20
#define	DATA_ERROR		0x10
#define	DMA_FAILURE		0x08
#define	SEC_NOT_FOUND	0x04
#define	BAD_ADDRESS		0x02
#define	BAD_COMMAND		0x01

/* Abnormal termination codes */

#define TC_NORMAL   0
#define TC_HARDERR  1
#define TC_GP_TRAP  2
#define TC_SIGNAL   3

