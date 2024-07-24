/***MD sdmerror.h - System Detection Manager Error Codes
 *
 *  This module contains System Detection Manager error codes.
 *
 *  Copyright (c) 1992,1993 Microsoft Corporation
 *  Author:	Michael Tsang (MikeTs)
 *  Created	03/14/94
 *
 *  MODIFICATION HISTORY
 */


#ifndef _INC_SDERROR
#define _INC_SDERROR


/*** System Detection Manager Errors (return values)
 */

#define IDSERR_NONE		0		//no error (success)
#define SDMERR_NONE		IDSERR_NONE
#define IDSERR_INVALID_PARAM	0x0001		//invalid service parameter
#define SDMERR_INVALID_PARAM	(0x80000000 | IDSERR_INVALID_PARAM)
#define IDSERR_OUTOFMEM 	0x0002		//out of memory
#define SDMERR_OUTOFMEM 	(0x80000000 | IDSERR_OUTOFMEM)
#define IDSERR_REG_ACCESS	0x0003		//error accessing registry
#define SDMERR_REG_ACCESS	(0x80000000 | IDSERR_REG_ACCESS)
#define IDSERR_SD_ALREADYOPEN	0x0004		//system detection already open
#define SDMERR_SD_ALREADYOPEN	(0x80000000 | IDSERR_SD_ALREADYOPEN)
#define IDSERR_SD_NOTOPEN	0x0005		//system detection not yet open
#define SDMERR_SD_NOTOPEN	(0x80000000 | IDSERR_SD_NOTOPEN)
#define IDSERR_HOOK_R0CODE	0x0006		//failed to hook Ring0 code
#define SDMERR_HOOK_R0CODE	(0x80000000 | IDSERR_HOOK_R0CODE)
#define IDSERR_DUPINF		0x0007		//inf already registered
#define SDMERR_DUPINF		(0x80000000 | IDSERR_DUPINF)
#define IDSERR_OPENINF		0x0008		//cannot open inf file
#define SDMERR_OPENINF		(0x80000000 | IDSERR_OPENINF)
#define IDSERR_RES_STR		0x0009		//invalid resource string
#define SDMERR_RES_STR		(0x80000000 | IDSERR_RES_STR)
#define IDSERR_INF_LINE 	0x000a		//invalid inf line
#define SDMERR_INF_LINE 	(0x80000000 | IDSERR_INF_LINE)
#define IDSERR_INF_CLASSATTR	0x000b		//invalid class attribute
#define SDMERR_INF_CLASSATTR	(0x80000000 | IDSERR_INF_CLASSATTR)
#define IDSERR_INF_MODLINE	0x000c		//invalid module line
#define SDMERR_INF_MODLINE	(0x80000000 | IDSERR_INF_MODLINE)
#define IDSERR_INF_MODNAME	0x000d		//module name too long
#define SDMERR_INF_MODNAME	(0x80000000 | IDSERR_INF_MODNAME)
#define IDSERR_INF_FUNCSECT	0x000e		//cannot find function section
#define SDMERR_INF_FUNCSECT	(0x80000000 | IDSERR_INF_FUNCSECT)
#define IDSERR_INF_FUNCLINE	0x000f		//invalid function line
#define SDMERR_INF_FUNCLINE	(0x80000000 | IDSERR_INF_FUNCLINE)
#define IDSERR_LOADMOD		0x0010		//error loading module
#define SDMERR_LOADMOD		(0x80000000 | IDSERR_LOADMOD)
#define IDSERR_INF_CLASSNAME	0x0011		//inf err in class name
#define SDMERR_INF_CLASSNAME	(0x80000000 | IDSERR_INF_CLASSNAME)
#define IDSERR_NOMORE_CLASS	0x0012		//no more class to enumerate
#define SDMERR_NOMORE_CLASS	(0x80000000 | IDSERR_NOMORE_CLASS)
#define IDSERR_CONFIGMG 	0x0013		//configmg error
#define SDMERR_CONFIGMG 	(0x80000000 | IDSERR_CONFIGMG)
#define IDSERR_DIFFERENT_DEV	0x0014		//reported diff dev than verify.
#define SDMERR_DIFFERENT_DEV	(0x80000000 | IDSERR_DIFFERENT_DEV)
#define IDSERR_DANGER_INVALID	0x0015		//invalid danger regions
#define SDMERR_DANGER_INVALID	(0x80000000 | IDSERR_DANGER_INVALID)
#define IDSERR_IOMEM_INVALID	0x0016		//invalid iomem regions
#define SDMERR_IOMEM_INVALID	(0x80000000 | IDSERR_IOMEM_INVALID)
#define IDSERR_IOMEM_CONFLICT	0x0017		//iomem resource conflict
#define SDMERR_IOMEM_CONFLICT	(0x80000000 | IDSERR_IOMEM_CONFLICT)
#define IDSERR_IRQDMA_INVALID	0x0018		//invalid irqdma resources
#define SDMERR_IRQDMA_INVALID	(0x80000000 | IDSERR_IRQDMA_INVALID)
#define IDSERR_IRQDMA_CONFLICT	0x0019		//irqdma resource conflict
#define SDMERR_IRQDMA_CONFLICT	(0x80000000 | IDSERR_IRQDMA_CONFLICT)
#define IDSERR_INF_BUSTYPE	0x001a		//invalid detection bus type
#define SDMERR_INF_BUSTYPE	(0x80000000 | IDSERR_INF_BUSTYPE)
#define IDSERR_DETFUNC_DMSERR	0x001b		//DMS error in detection
#define SDMERR_DETFUNC_DMSERR	(0x80000000 | IDSERR_DETFUNC_DMSERR)
#define IDSERR_DETFUNC_REGERR	0x001c		//reg error in detection code
#define SDMERR_DETFUNC_REGERR	(0x80000000 | IDSERR_DETFUNC_REGERR)
#define IDSERR_DETFUNC_ERROR	0x001d		//detection function error
#define SDMERR_DETFUNC_ERROR	(0x80000000 | IDSERR_DETFUNC_ERROR)
#define IDSERR_GENINSTALL	0x001e		//GenInstallEx error
#define SDMERR_GENINSTALL	(0x80000000 | IDSERR_GENINSTALL)
#define IDSERR_VAR_NOTFOUND	0x001f		//detmod var not found
#define SDMERR_VAR_NOTFOUND	(0x80000000 | IDSERR_VAR_NOTFOUND)
#define IDSERR_BUFF_TOOSMALL	0x0020		//buffer too small
#define SDMERR_BUFF_TOOSMALL	(0x80000000 | IDSERR_BUFF_TOOSMALL)
#define IDSERR_DEV_ALREADYREG	0x0021		//device ID already registered
#define SDMERR_DEV_ALREADYREG	(0x80000000 | IDSERR_DEV_ALREADYREG)
#define IDSERR_NOT_EISA 	0x0022		//not an EISA system
#define SDMERR_NOT_EISA 	(0x80000000 | IDSERR_NOT_EISA)
#define IDSERR_GETEISACONFIG	0x0023		//error getting EISA config
#define SDMERR_GETEISACONFIG	(0x80000000 | IDSERR_GETEISACONFIG)
#define IDSERR_INF_CLASS	0x0024		//error in getting INF class
#define SDMERR_INF_CLASS	(0x80000000 | IDSERR_INF_CLASS)
#define IDSERR_COPYLOGCONFIG	0x0025		//error in copying logconfig
#define SDMERR_COPYLOGCONFIG	(0x80000000 | IDSERR_COPYLOGCONFIG)
#define IDSERR_NOMORE_FUNC	0x0026		//no more detection functions
#define SDMERR_NOMORE_FUNC	(0x80000000 | IDSERR_NOMORE_FUNC)
#define IDSERR_NOT_MCA		0x0027		//not a MCA system
#define SDMERR_NOT_MCA		(0x80000000 | IDSERR_NOT_MCA)
#define IDSERR_ABORT		0x0028		//detection is aborted
#define SDMERR_ABORT		(0x80000000 | IDSERR_ABORT)
#define IDSERR_GETFUNCADDR	0x0029		//failed to get function entry
#define SDMERR_GETFUNCADDR	(0x80000000 | IDSERR_GETFUNCADDR)
#define IDSERR_HOOKINT86X	0x002a		//failed to hook Int86x worker
#define SDMERR_HOOKINT86X	(0x80000000 | IDSERR_HOOKINT86X)
#define IDSERR_REGDOSDEV	0x002b		//failed to reg. dos dev info.
#define SDMERR_REGDOSDEV	(0x80000000 | IDSERR_REGDOSDEV)
#define IDSERR_INF_RISKCAT	0x002c		//invalid risk category
#define SDMERR_INF_RISKCAT	(0x80000000 | IDSERR_INF_RISKCAT)
#define IDSERR_NODOSDEV 	0x002d		//no such dos device
#define SDMERR_NODOSDEV 	(0x80000000 | IDSERR_NODOSDEV)
#define IDSERR_IOMEM_NOTFOUND	0x002e		//iomem block not found
#define SDMERR_IOMEM_NOTFOUND	(0x80000000 | IDSERR_IOMEM_NOTFOUND)
#define IDSERR_IRQDMA_NOTFOUND	0x002f		//irqdma block not found
#define SDMERR_IRQDMA_NOTFOUND	(0x80000000 | IDSERR_IRQDMA_NOTFOUND)
#define IDSERR_OPENCRASHLOG	0x0030		//failed to open crash log
#define SDMERR_OPENCRASHLOG	(0x80000000 | IDSERR_OPENCRASHLOG)
#define IDSERR_NODOSTSR 	0x0031		//no such dos TSR
#define SDMERR_NODOSTSR 	(0x80000000 | IDSERR_NODOSTSR)
#define IDSERR_REGHW_DUP	0x0032		//duplicate hw registration
#define SDMERR_REGHW_DUP	(0x80000000 | IDSERR_REGHW_DUP)
#define IDSERR_CALLRMPROC	0x0033		//call real mode proc error
#define SDMERR_CALLRMPROC	(0x80000000 | IDSERR_CALLRMPROC)
#define IDSERR_OPENDETLOG	0x0034		//failed to open detect log
#define SDMERR_OPENDETLOG	(0x80000000 | IDSERR_OPENDETLOG)
#define IDSERR_REGDOSTSR	0x0035		//failed to reg. dos tsr info.
#define SDMERR_REGDOSTSR	(0x80000000 | IDSERR_REGDOSTSR)
#define IDSERR_INT86XCRASH	0x0036		//Int86x call crashed before
#define SDMERR_INT86XCRASH	(0x80000000 | IDSERR_INT86XCRASH)

#endif	//_INC_SDERROR
