// kexception.cpp: Implementation of the KException class.
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#include	"vdw.h"
#include	"kexception.h"

/////////////////////////////////////////////////////////////////////
// See comments in kexception.h

/////////////////////////////////////////////////////////////////////
// SE Translator Declaration. Implemented in CppExcpt.lib
//
// Those are normally defined in <eh.h>, which we don't want to include

extern "C" {
typedef void (__cdecl *_se_translator_function)(ULONG ExceptionCode, EXCEPTION_POINTERS* pExceptionPointers);
_se_translator_function __cdecl set_se_translator(_se_translator_function);
} // "C"


// ctor
KException::KException(EXCEPTION_POINTERS const &info)
{
   EXCEPTION_RECORD const &exception = *(info.ExceptionRecord);
   m_ip = exception.ExceptionAddress;
   m_status = exception.ExceptionCode;
}

// Translator installer - static function
void KException::Install()
{
   set_se_translator(Translator);
}


// ctor
KExceptionAccessViolation::KExceptionAccessViolation(EXCEPTION_POINTERS const &info) :
	KException(info)
{
   EXCEPTION_RECORD const &exception = *(info.ExceptionRecord);
   m_addr = (PVOID) exception.ExceptionInformation[1];
   m_bWrite = (exception.ExceptionInformation[0]==1);
}

// Translator - static function
void __cdecl KException::Translator(ULONG Code, EXCEPTION_POINTERS *info)
{
	if (Code != STATUS_ACCESS_VIOLATION)
		throw KException(*info);
	else
		throw KExceptionAccessViolation(*info);
}
