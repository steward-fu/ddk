//		vpcode.h
//
// Copyright (c) 1994 by Compuware Corporation
// All rights reserved


// Don't include again if this segment is currently active

#if	__CUR_CODE_SEG__   !=	16

#undef	__CUR_CODE_SEG__
#define __CUR_CODE_SEG__	16

#ifdef __BORLANDC__

#pragma codeseg _PTEXT "PCODE"
#undef  strlen
#define strlen PAGEABLE___bc_strlen
#undef  memset
#define memset PAGEABLE___bc_memset
#undef  memcpy
#define memcpy PAGEABLE___bc_memcpy
#undef  memcmp
#define memcmp PAGEABLE___bc_memcmp

#else
#pragma code_seg("_PTEXT", "PCODE")
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <segdefp.h>
#include <crtsegp.h>

#ifdef __cplusplus
}
#endif

#endif

