/*****************************************************************************
 *
 *  (C) Copyright MICROSOFT Corp., 1988-1990
 *
 *  Title:      SHELLFSC.H - EQUATES related to the File Sys Change stuff
 *
 *  Version:    1.00
 *
 *  Date:       19-Sep-1988
 *
 *  Author:     ARR
 *
 *----------------------------------------------------------------------------
 *
 *  Change log:
 *
 *     DATE     REV                 DESCRIPTION
 *  ----------- --- ----------------------------------------------------------
 *  19-Sep-1988 ARR Original
 *
 *****************************************************************************/

#define VDA_FileSysChange       (0x0400+20+11)

/*
 *  On VDA_FileSysChange, wParam is not used
 *    Particular VMDOSAPP instance which gets the message is the VM that has
 *    changed the file system
 *
 *  NOTE THERE MUST BE A CALL BACK ON THIS EVENT.
 *
 *  The reference data MUST BE A pointer to a structure of the following type:
 *
 *  It is the responsibility of the caller to "FREE" this structure (if needed)
 *    on the call back.
 *
 *  Caller can extend this structure if needed by tacking extra info on the end.
 */

#define MAXFSCINFOSIZE          256

typedef struct VMDA_FileSysChng {
    WORD VMDA_FSC_Func;
    BYTE VMDA_FSC_Buffer[MAXFSCINFOSIZE];
} VMDA_FileSysChng;

/*
 *  Equates for VMDA_FSC_Func
 */
#define VMDA_FSC_CREATE         0
#define VMDA_FSC_DELETE         1
#define VMDA_FSC_RENAME         2
#define VMDA_FSC_ATTRIBUTES     3
#define VMDA_FSC_NETCONNECT     4
#define VMDA_FSC_NETDISCONNECT  5
#define VMDA_FSC_REFRESH        6
#define VMDA_FSC_MKDIR          7
#define VMDA_FSC_RMDIR          8
