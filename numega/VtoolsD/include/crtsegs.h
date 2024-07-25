//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  _ultoa64
#define _ultoa64 STATIC__ultoa64

#undef  ultoa64
#define ultoa64 STATIC__ultoa64

#undef  _ltoa64
#define _ltoa64 STATIC__ltoa64

#undef  ltoa64
#define ltoa64 STATIC__ltoa64

#undef  MapPointerToFlat
#define MapPointerToFlat STATIC_MapPointerToFlat

#undef  ustrcpy
#define ustrcpy STATIC_ustrcpy

#undef  ustrlen
#define ustrlen STATIC_ustrlen

#undef  __bc_memcmp
#define __bc_memcmp STATIC___bc_memcmp

#undef  _bc_memcmp
#define _bc_memcmp STATIC___bc_memcmp

#undef  __bc_memcpy
#define __bc_memcpy STATIC___bc_memcpy

#undef  _bc_memcpy
#define _bc_memcpy STATIC___bc_memcpy

#undef  __bc_memset
#define __bc_memset STATIC___bc_memset

#undef  _bc_memset
#define _bc_memset STATIC___bc_memset

#undef  Voutportb
#define Voutportb STATIC_Voutportb

#undef  Voutportw
#define Voutportw STATIC_Voutportw

#undef  Voutportd
#define Voutportd STATIC_Voutportd

#undef  Vinportb
#define Vinportb STATIC_Vinportb

#undef  Vinportw
#define Vinportw STATIC_Vinportw

#undef  Vinportd
#define Vinportd STATIC_Vinportd

#undef  i_close
#define i_close STATIC_i_close

#undef  i_remove
#define i_remove STATIC_i_remove

#undef  i_read
#define i_read STATIC_i_read

#undef  i_write
#define i_write STATIC_i_write

#undef  __write_bin
#define __write_bin STATIC___write_bin

#undef  _write_bin
#define _write_bin STATIC___write_bin

#undef  __write_text
#define __write_text STATIC___write_text

#undef  _write_text
#define _write_text STATIC___write_text

#undef  i_lseek
#define i_lseek STATIC_i_lseek

#undef  i_open
#define i_open STATIC_i_open

#undef  vsprintf
#define vsprintf STATIC_vsprintf

#undef  _vsnprintf
#define _vsnprintf STATIC__vsnprintf

#undef  vsnprintf
#define vsnprintf STATIC__vsnprintf

#undef  _snprintf
#define _snprintf STATIC__snprintf

#undef  snprintf
#define snprintf STATIC__snprintf

#undef  realloc
#define realloc STATIC_realloc

#undef  calloc
#define calloc STATIC_calloc

#undef  malloc
#define malloc STATIC_malloc

#undef  dprintf
#define dprintf STATIC_dprintf

#undef  nprintf
#define nprintf STATIC_nprintf

#undef  sprintf
#define sprintf STATIC_sprintf

#undef  _setjmp
#define _setjmp STATIC__setjmp

#undef  setjmp
#define setjmp STATIC__setjmp

#undef  longjmp
#define longjmp STATIC_longjmp

#undef  strchr
#define strchr STATIC_strchr

#undef  strcspn
#define strcspn STATIC_strcspn

#undef  _strdup
#define _strdup STATIC__strdup

#undef  strdup
#define strdup STATIC__strdup

#undef  _stricmp
#define _stricmp STATIC__stricmp

#undef  stricmp
#define stricmp STATIC__stricmp

#undef  __bc_strlen
#define __bc_strlen STATIC___bc_strlen

#undef  _bc_strlen
#define _bc_strlen STATIC___bc_strlen

#undef  strncat
#define strncat STATIC_strncat

#undef  strncmp
#define strncmp STATIC_strncmp

#undef  strncpy
#define strncpy STATIC_strncpy

#undef  _strnicmp
#define _strnicmp STATIC__strnicmp

#undef  strnicmp
#define strnicmp STATIC__strnicmp

#undef  _strnset
#define _strnset STATIC__strnset

#undef  strnset
#define strnset STATIC__strnset

#undef  _strupr
#define _strupr STATIC__strupr

#undef  strupr
#define strupr STATIC__strupr

#undef  _strlwr
#define _strlwr STATIC__strlwr

#undef  strlwr
#define strlwr STATIC__strlwr

#undef  _strrev
#define _strrev STATIC__strrev

#undef  strrev
#define strrev STATIC__strrev

#undef  strrchr
#define strrchr STATIC_strrchr

#undef  strspn
#define strspn STATIC_strspn

#undef  strpbrk
#define strpbrk STATIC_strpbrk

#undef  strstr
#define strstr STATIC_strstr

#undef  atol
#define atol STATIC_atol

#undef  _ltoa
#define _ltoa STATIC__ltoa

#undef  ltoa
#define ltoa STATIC__ltoa

#undef  strtol
#define strtol STATIC_strtol

#undef  _ultoa
#define _ultoa STATIC__ultoa

#undef  ultoa
#define ultoa STATIC__ultoa

#undef  strtoul
#define strtoul STATIC_strtoul

#undef  sscanf
#define sscanf STATIC_sscanf

#undef  _swab
#define _swab STATIC__swab

#undef  swab
#define swab STATIC__swab

#undef  memmove
#define memmove STATIC_memmove

#undef  _memicmp
#define _memicmp STATIC__memicmp

#undef  memicmp
#define memicmp STATIC__memicmp

#undef  memchr
#define memchr STATIC_memchr

#undef  _memccpy
#define _memccpy STATIC__memccpy

#undef  memccpy
#define memccpy STATIC__memccpy

#undef  _CTAB
#define _CTAB STATIC__CTAB

#undef  CTAB
#define CTAB STATIC__CTAB

extern char _CTAB[256];
#undef  free
#define free STATIC_free

#define _S_IWRITE		0x00000000
#define _S_IREAD		0x00000001
#define _O_RDONLY   	0x00000000
#define _O_WRONLY   	0x00000001
#define _O_RDWR     	0x00000002
#define _O_APPEND		0x00000008
#define _O_TEXT     	0x00004000
#define _O_BINARY   	0x00008000
#define _O_CREAT    	0x00000100
#define _O_TRUNC    	0x00000200
#define _O_EXCL   	0x00000400
#define SEEK_SET		0x00000000
#define SEEK_CUR		0x00000001
#define SEEK_END		0x00000002
#define set_errno(x)
#undef  atoi
#define atoi atol
#undef  _itoa
#define _itoa _ltoa
#undef  isalnum
#define isalnum(c)	(_CTAB[c] & (_C_ALP | _C_DEC))
#undef  isalpha
#define isalpha(c)	(_CTAB[c] & (_C_ALP))
#undef  iscntrl
#define iscntrl(c)	(_CTAB[c] & (_C_CTL))
#undef  __iscsym
#define __iscsym(c)	( (c == '_') || ((_CTAB[c]) & (_C_ALP | _C_DEC)))
#undef  __iscsymf
#define __iscsymf(c)	( (c == '_') || ((_CTAB[c]) & _C_ALP))
#undef  isdigit
#define isdigit(c)	(_CTAB[c] & (_C_DEC))
#undef  islower
#define islower(c)	(_CTAB[c] & (_C_LOW))
#undef  ispunct
#define ispunct(c)	(_CTAB[c] & (_C_PCT))
#undef  isspace
#define isspace(c)	(_CTAB[c] & (_C_SPC))
#undef  isupper
#define isupper(c)	(_CTAB[c] & (_C_UPP))
#undef  isxdigit
#define isxdigit(c)	(_CTAB[c] & (_C_HEX))
#undef  isgraph
#define isgraph(c)	(_CTAB[c] & (_C_PCT | _C_ALP | _C_DEC))
#undef  isprint
#define isprint(c)	(_CTAB[c] & (_C_PCT | _C_ALP | _C_DEC | _C_BLK))
