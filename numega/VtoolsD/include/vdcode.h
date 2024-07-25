//		vdcode.h
//
// Copyright (c) 1994 by Compuware Corporation
// All rights reserved


// Don't include again if this segment is currently active
#if	__CUR_CODE_SEG__   !=	4
#undef	__CUR_CODE_SEG__
#define __CUR_CODE_SEG__	4

#ifdef __BORLANDC__

#pragma codeseg _DBOCODE "DBOCODE"
#undef  strlen
#define strlen DEBUG___bc_strlen
#undef  memset
#define memset DEBUG___bc_memset
#undef  memcpy
#define memcpy DEBUG___bc_memcpy
#undef  memcmp
#define memcmp DEBUG___bc_memcmp

#else
#pragma code_seg("_DBOCODE", "DBOCODE")
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <segdefd.h>
#include <crtsegd.h>

#ifdef __cplusplus
}
#endif

#endif
