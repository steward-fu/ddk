/*****************************************************************************
 *
 *   (C) Copyright MICROSOFT Corp., 1993
 *
 *   Title:	VMMREG.H - Include file for VMM/Loader Registry Services
 *
 *   Version:	1.00
 *
 *   Date:	03-June-1993
 *
 *   Author:	Nagara
 *
 *-----------------------------------------------------------------------------
 *
 *   Change log:
 *
 ******************************************************************************
 */

#ifndef	_VMMREG_H
#define _VMMREG_H

typedef DWORD	VMMHKEY;
typedef	VMMHKEY	*PVMMHKEY;
typedef DWORD	VMMREGRET;			// return type for the REG Functions

#define	MAX_VMM_REG_KEY_LEN	256	// includes the \0 terminator

#ifndef REG_SZ		// define only if not there already

#define REG_SZ		0x0001
#define REG_BINARY	0x0003

#endif

#ifndef HKEY_LOCAL_MACHINE	// define only if not there already

#define HKEY_CLASSES_ROOT		0x80000000
#define HKEY_CURRENT_USER		0x80000001
#define HKEY_LOCAL_MACHINE		0x80000002
#define HKEY_USERS			0x80000003
#define HKEY_PERFORMANCE_DATA		0x80000004
#define HKEY_CURRENT_CONFIG		0x80000005
#define HKEY_DYN_DATA			0x80000006

#endif


// ERROR CODES returned by Registry
// NOTE THAT THESE ARE ALSO DEFINED IN WINERROR.H 
// and so VMMREG.H should be included after WINERROR.H

#ifndef ERROR_BADDB
#define ERROR_BADDB                      1009L
#endif

#ifndef ERROR_MORE_DATA
#define ERROR_MORE_DATA                  234L    
#endif

#ifndef ERROR_BADKEY
#define ERROR_BADKEY			 1010L
#endif

#ifndef ERROR_CANTOPEN
#define ERROR_CANTOPEN                   1011L
#endif

#ifndef ERROR_CANTREAD
#define ERROR_CANTREAD                   1012L
#define ERROR_CANTWRITE                  1013L
#endif

#ifndef ERROR_REGISTRY_CORRUPT
#define ERROR_REGISTRY_CORRUPT           1015L
#define ERROR_REGISTRY_IO_FAILED         1016L
#endif

#ifndef ERROR_KEY_DELETED
#define ERROR_KEY_DELETED                1018L
#endif

#ifndef ERROR_OUTOFMEMORY
#define ERROR_OUTOFMEMORY		   14L
#endif

#ifndef ERROR_INVALID_PARAMETER
#define ERROR_INVALID_PARAMETER		   87L
#endif

#ifndef ERROR_LOCK_FAILED
#define ERROR_LOCK_FAILED                167L
#endif

#ifndef ERROR_NO_MORE_ITEMS
#define ERROR_NO_MORE_ITEMS		  259L
#endif	


#ifndef  ERROR_SUCCESS           
#define ERROR_SUCCESS           0L
#endif

// END ERROR CODES

/*XLATOFF*/
#ifndef Not_VxD

/*NOINC*/
#ifndef _PROVIDER_STRUCTS_DEFINED
#define _PROVIDER_STRUCTS_DEFINED

#define PROVIDER_KEEPS_VALUE_LENGTH	0x1
struct val_context {
    int valuelen;		// the total length of this value
    PVOID value_context;	// provider's context
    PVOID val_buff_ptr;	// where in the ouput buffer the value is.
};

typedef struct val_context *PVALCONTEXT;

typedef struct pvalue {		      // Provider supplied value/context.
    PCHAR pv_valuename;          // The value name pointer
    DWORD pv_valuelen;
    PVOID pv_value_context;
    DWORD pv_type;
}PVALUE;

typedef struct pvalue *PPVALUE;

typedef VMMREGRET (_cdecl *PQUERYHANDLER)(PVOID, PVALCONTEXT, DWORD, PVOID, DWORD *, DWORD);

typedef struct provider_info {
    PQUERYHANDLER pi_R0_1val;
    PQUERYHANDLER pi_R0_allvals;
    PQUERYHANDLER pi_R3_1val;
    PQUERYHANDLER pi_R3_allvals;
    DWORD pi_flags;    // capability flags (none defined yet).
    PVOID pi_key_context;
}PROVIDER;

typedef PROVIDER *PPROVIDER;

struct value_ent {
    PCHAR ve_valuename;
    DWORD ve_valuelen;
    DWORD ve_valueptr;
    DWORD ve_type;
};

typedef struct value_ent VALENT;
typedef VALENT *PVALENT;

#endif // not(_PROVIDER_STRUCTS_DEFINED)
/*INC*/

#endif // Not_VxD

/*XLATON*/

/* ASM
;**************************************************************
; Macros for Realmode loader registry Services
;
;**************************************************************
LDR_RegOpenKey	Macro	hKey,OffSubKey,SegSubKey,OffphKey,SegphKey
	push	SegphKey
	push	OffphKey		; lphKey
	push	SegSubKey
	push	OffSubKey		; lpszSubKey
	push	dword ptr hKey		; hKey
	mov	ax,LDRSRV_RegOpenKey
	call	dword ptr [_ServiceEntry]
	add	sp,3*4		; for 3 parameters on stack
ENDM
;**************************************************************
LDR_RegCloseKey	Macro	hKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegCloseKey
	call	dword ptr [_ServiceEntry]
	add	sp,1*4		; for 1 parameter on stack
ENDM
;**************************************************************
LDR_RegCreateKey	Macro	hKey,OffSubKey,SegSubKey,OffphKey,SegphKey
	push	SegphKey
	push	OffphKey		; lphKey
	push	SegSubKey
	push	OffSubKey		; lpszSubKey
	push	dword ptr hKey		; hKey
	mov	ax,LDRSRV_RegCreateKey
	call	dword ptr [_ServiceEntry]
	add	sp,3*4		; for 3 parameters on stack
ENDM
;**************************************************************
LDR_RegDeleteKey	Macro	hKey,OffSubKey,SegSubKey
	push	SegSubKey
	push	OffSubKey		; lpszSubKey
	push	dword ptr hKey		; hKey
	mov	ax,LDRSRV_RegDeleteKey
	call	dword ptr [_ServiceEntry]
	add	sp,2*4		; for 2 parameters on stack
ENDM
;**************************************************************
LDR_RegEnumKey	Macro	hKey,iSubKey,OffszName,SegszName,BufLen
	push	dword ptr BufLen
	push	SegszName
	push	OffszName
	push	dword ptr iSubKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegEnumKey
	call	dword ptr [_ServiceEntry]
	add	sp,4*4		; for 4 parameters on stack
ENDM
;**************************************************************
LDR_RegQueryValue	Macro	hKey,OffSubKey,SegSubKey,OffValue,SegValue,OffcbValue,SegcbValue
	push	SegcbValue
	push	OffcbValue
	push	SegValue
	push	OffValue
	push	SegSubKey
	push	OffSubKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegQueryValue
	call	dword ptr [_ServiceEntry]
	add	sp,4*4		; for 4 parameters on stack
ENDM
;**************************************************************
LDR_RegSetValue	Macro	hKey,OffSubKey,SegSubKey,dwType,OffData,SegData,cbData
	push	dword ptr cbData
	push	SegData
	push	OffData
	push	dword ptr dwType
	push	SegSubKey
	push	OffSubKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegSetValue
	call	dword ptr [_ServiceEntry]
	add	sp,5*4		; for 4 parameters on stack
ENDM
;**************************************************************
LDR_RegDeleteValue	Macro	hKey,OffValue,SegValue
	push	SegValue
	push	OffValue		; lpszValue
	push	dword ptr hKey		; hKey
	mov	ax,LDRSRV_RegDeleteValue
	call	dword ptr [_ServiceEntry]
	add	sp,2*4		; for 2 parameters on stack
ENDM
;**************************************************************
LDR_RegEnumValue	Macro hKey,iValue,OffValue,SegValue,OffcbValue,SegcbValue,RegReserved,OffdwType,SegdwType,OffData,SegData,OffcbData,SegcbData
	push	SegcbData
	push	OffcbData
	push	SegData
	push	OffData
	push	SegdwType
	push	OffdwType
	push	dword ptr RegReserved
	push	SegcbValue
	push	OffcbValue
	push	SegValue
	push	OffValue
	push	dword ptr iValue
	push	dword ptr hKey		; hKey
	mov	ax,LDRSRV_RegEnumValue
	call	dword ptr [_ServiceEntry]
	add	sp,8*4		; for 8 parameters on stack
ENDM
;**************************************************************
LDR_RegQueryValueEx	Macro	hKey,OffValue,SegValue,RegReserved,OffdwType,SegdwType,OffData,SegData,OffcbData,SegcbData
	push	SegcbData
	push	OffcbData
	push	SegData
	push	OffData
	push	SegdwType
	push	OffdwType
	push	dword ptr RegReserved
	push	SegValue
	push	OffValue
	push	dword ptr hKey
	mov	ax,LDRSRV_RegQueryValueEx
	call	dword ptr [_ServiceEntry]
	add	sp,6*4		; for 6 parameters on stack
ENDM
;**************************************************************
LDR_RegSetValueEx	Macro	hKey,OffValue,SegValue,RegReserved,dwType,OffData,SegData,cbData
	push	dword ptr cbData
	push	SegData
	push	OffData
	push	dword ptr dwType
	push	dword ptr RegReserved
	push	SegValue
	push	OffValue
	push	dword ptr hKey
	mov	ax,LDRSRV_RegSetValueEx
	call	dword ptr [_ServiceEntry]
	add	sp,6*4		; for 6 parameters on stack
ENDM
;**************************************************************
LDR_RegFlushKey		Macro	hKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegFlushKey
	call	dword ptr [_ServiceEntry]
	add	sp,1*4		; for 1 parameter on stack
ENDM
;**************************************************************
*/
#endif		/* _VMMREG_H */
