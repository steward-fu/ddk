//		vlcode.h
//
// Copyright (c) 1994 by Compuware Corporation
// All rights reserved


// Don't include again if this segment is currently active

#if	__CUR_CODE_SEG__   !=	2

#undef	__CUR_CODE_SEG__
#define __CUR_CODE_SEG__	2

#ifdef __BORLANDC__

#pragma codeseg _LTEXT "LCODE"
#undef  strlen
#define strlen LOCK___bc_strlen
#undef  memset
#define memset LOCK___bc_memset
#undef  memcpy
#define memcpy LOCK___bc_memcpy
#undef  memcmp
#define memcmp LOCK___bc_memcmp

#else
#pragma code_seg("_LTEXT", "LCODE")
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <segdefl.h>
#include <crtsegl.h>

#ifdef __cplusplus
}
#endif

#endif

