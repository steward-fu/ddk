//		vscode.h
//
// Copyright (c) 1994 by Compuware Corporation
// All rights reserved

// Don't include again if this segment is currently active

#if	__CUR_CODE_SEG__   !=	8
#undef	__CUR_CODE_SEG__
#define __CUR_CODE_SEG__	8

#ifdef __BORLANDC__

#pragma codeseg _STEXT "SCODE"
#undef  strlen
#define strlen STATIC___bc_strlen
#undef  memset
#define memset STATIC___bc_memset
#undef  memcpy
#define memcpy STATIC___bc_memcpy
#undef  memcmp
#define memcmp STATIC___bc_memcmp

#else
#pragma code_seg("_STEXT", "SCODE")
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "segdefs.h"
#include "crtsegs.h"

#ifdef __cplusplus
}
#endif

#endif
