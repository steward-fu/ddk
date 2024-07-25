/*********************************************************************
 *   (C) Copyright MICROSOFT Corp., 1993-1995
 *   Title:     PERF.H - Include file for perf monitor
 *   Version:   1.00
 *   Date:
 *   Author:  FCF
 *----------------------------------------------------------------------
 *   Change log:
 *   DATE        REV DESCRIPTION
 *   ----------- --- ---------------------------------------------------
 */

/* defines */

#define MAXNAMELEN      50  /* maximum number of characters in

                   service name, stat name, or key names */
#define MAXCOMPLEXSUBSTAT  8  /* maximum number of stats making
                   up a complex stat */
  
/* structures and flags used for the ring-0 interface */

typedef struct perf_server_0 {
    unsigned long  psrv0_Level;  /* Must be zero for this level */
    unsigned long  psrv0_Flags;
    char          *psrv0_pszServerName;
    char         *psrv0_pszServerNodeName;
    void         *psrv0_pControlFunc;

} PERF_SERVER_0;

typedef struct perf_stat_0 {
    unsigned long  pst0_Level;  /* Must be zero for this level */
    unsigned long  pst0_Flags;
    char         *pst0_pszStatName;
    char         *pst0_pszStatNodeName;
    char         *pst0_reserved;      // must be NULL
    char         *pst0_pszStatDescription;
    void         *pst0_pStatFunc;
  unsigned long  pst0_ScaleFactor;  // set PSTF_FACTOR_xxx flag to use
} PERF_STAT_0;

/* Values for psrv0_Flags follow */

/* Values for pst0_Flags follow */

/* pst0_pStatFunc points either directly to data (always a DWORD for now) */
/* or, if PSTF_FUNCPTR_BIT is set, to a _cdecl function.  This function   */
/* accepts a stat handle as it's argument and returns the stat in eax     */

#define PSTF_FUNCPTR    0x00000001
#define PSTF_COUNT    0x00000000
#define PSTF_RATE    0x00000002

/* If either of these flags are set, then the output will be scaled
   (either multiplied or divided) by pst0_ScaleFactor.  */
#define PSTF_FACTOR_MULTIPLY  0x00000040
#define PSTF_FACTOR_DIVIDE    0x00000080

typedef DWORD STATHANDLE;
typedef DWORD PERFHANDLE;

#define PMSG_START_STAT        0x11
#define PMSG_STOP_STAT        0x12


/* ASM

; Ring-0 macros to aid stat registration

Reg_Perf_Srv MACRO level:REQ, flags:REQ, servername:REQ, nodename:REQ, controlfunc:REQ
  local  nothere
  VxDcall  PERF_Get_Version
  or  eax, eax
  jz  nothere

  IF (OPATTR(controlfunc)) AND 00010000y   ;; register
    push  controlfunc
  ELSE
    push  OFFSET32 controlfunc
  ENDIF

  IF (OPATTR(nodename)) AND 00010000y    ;; register
    push  nodename
  ELSE
    push  OFFSET32 nodename
  ENDIF

  IF (OPATTR(servername)) AND 00010000y    ;; register

    push  servername
  ELSE
    push  OFFSET32 servername
  ENDIF
  
  push  flags
        push  level
  push  esp
  VxDcall  PERF_Server_Register
  add  esp, 6*4
nothere:
  ENDM

Reg_Perf_Stat MACRO srvhandle:REQ, level:REQ, flags:REQ, name:REQ, nodename:REQ, reserved:REQ, desc:REQ, func:REQ
  IF (OPATTR(func)) AND 00010000y    ;; register
    push  func
  ELSE
    push  OFFSET32 func
  ENDIF

  IF (OPATTR(desc)) AND 00010000y    ;; register
    push  desc

  ELSE
    push  OFFSET32 desc
  ENDIF

  IF (OPATTR(reserved)) AND 00010000y  ;; register
    push  reserved
  ELSE
    push  OFFSET32 reserved
  ENDIF

  IF (OPATTR(nodename)) AND 00010000y  ;; register
    push  nodename
  ELSE
    push  OFFSET32 nodename
  ENDIF

  IF (OPATTR(name)) AND 00010000y    ;; register
    push  name
  ELSE
    push  OFFSET32 name
  ENDIF
  
  push  flags
  push  level
  push  esp
  push  srvhandle
  VxDcall  PERF_Server_Add_Stat

  add  esp, 9*4
  ENDM

Begin_Service_Table PERF
PERF_Service PERF_Get_Version, LOCAL
PERF_Service PERF_Server_Register, LOCAL
PERF_Service PERF_Server_Deregister, LOCAL
PERF_Service PERF_Server_Add_Stat, LOCAL
PERF_Service PERF_Server_Remove_Stat, LOCAL
End_Service_Table PERF

*/

#define HKEY_PERF_ROOT      HKEY_LOCAL_MACHINE
#define PERF_REG_KEY      "System\\CurrentControlSet\\Control\\PerfStats\\Enum"
#define PERF_REG_OTHER_KEY    "System\\CurrentControlSet\\Control\\PerfStats\\Other"

#define PERF_REG_NAME_SRV_NAME    "Name"
#define PERF_REG_NAME_VXD_NAME    "VxDName"
#define PERF_REG_NAME_STAT_NAME    "Name"
#define PERF_REG_NAME_STAT_COMPLEX  "Complex"
#define PERF_REG_NAME_STAT_DESC    "Description"
#define PERF_REG_NAME_STAT_DIFF    "Differentiate"

#define PERF_REG_VAL_STAT_TRUE    "TRUE"
#define PERF_REG_VAL_STAT_FALSE    "FALSE"
#define PERF_STAT_PREFIX      "STAT"

/* complex stat defines */

#define PSTF_COMPLEX    0x00000100



