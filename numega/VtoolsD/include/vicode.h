//		vicode.h
//
// Copyright (c) 1994 by Compuware Corporation
// All rights reserved


// Don't include again if this segment is currently active

#if	__CUR_CODE_SEG__   !=	1

#undef	__CUR_CODE_SEG__
#define __CUR_CODE_SEG__	1

#ifdef __BORLANDC__

#pragma codeseg _ITEXT "ICODE"
#undef  strlen
#define strlen INIT___bc_strlen
#undef  memset
#define memset INIT___bc_memset
#undef  memcpy
#define memcpy INIT___bc_memcpy
#undef  memcmp
#define memcmp INIT___bc_memcmp

#else
#pragma code_seg("_ITEXT", "ICODE")
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <segdefi.h>
#include <crtsegi.h>

#ifdef __cplusplus
}
#endif

#endif
