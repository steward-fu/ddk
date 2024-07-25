//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  _ultoa64
#define _ultoa64 LOCK__ultoa64

#undef  ultoa64
#define ultoa64 LOCK__ultoa64

#undef  _ltoa64
#define _ltoa64 LOCK__ltoa64

#undef  ltoa64
#define ltoa64 LOCK__ltoa64

#undef  MapPointerToFlat
#define MapPointerToFlat LOCK_MapPointerToFlat

#undef  ustrcpy
#define ustrcpy LOCK_ustrcpy

#undef  ustrlen
#define ustrlen LOCK_ustrlen

#undef  __bc_memcmp
#define __bc_memcmp LOCK___bc_memcmp

#undef  _bc_memcmp
#define _bc_memcmp LOCK___bc_memcmp

#undef  __bc_memcpy
#define __bc_memcpy LOCK___bc_memcpy

#undef  _bc_memcpy
#define _bc_memcpy LOCK___bc_memcpy

#undef  __bc_memset
#define __bc_memset LOCK___bc_memset

#undef  _bc_memset
#define _bc_memset LOCK___bc_memset

#undef  Voutportb
#define Voutportb LOCK_Voutportb

#undef  Voutportw
#define Voutportw LOCK_Voutportw

#undef  Voutportd
#define Voutportd LOCK_Voutportd

#undef  Vinportb
#define Vinportb LOCK_Vinportb

#undef  Vinportw
#define Vinportw LOCK_Vinportw

#undef  Vinportd
#define Vinportd LOCK_Vinportd

#undef  i_close
#define i_close LOCK_i_close

#undef  i_remove
#define i_remove LOCK_i_remove

#undef  i_read
#define i_read LOCK_i_read

#undef  i_write
#define i_write LOCK_i_write

#undef  __write_bin
#define __write_bin LOCK___write_bin

#undef  _write_bin
#define _write_bin LOCK___write_bin

#undef  __write_text
#define __write_text LOCK___write_text

#undef  _write_text
#define _write_text LOCK___write_text

#undef  i_lseek
#define i_lseek LOCK_i_lseek

#undef  i_open
#define i_open LOCK_i_open

#undef  vsprintf
#define vsprintf LOCK_vsprintf

#undef  _vsnprintf
#define _vsnprintf LOCK__vsnprintf

#undef  vsnprintf
#define vsnprintf LOCK__vsnprintf

#undef  _snprintf
#define _snprintf LOCK__snprintf

#undef  snprintf
#define snprintf LOCK__snprintf

#undef  realloc
#define realloc LOCK_realloc

#undef  calloc
#define calloc LOCK_calloc

#undef  malloc
#define malloc LOCK_malloc

#undef  dprintf
#define dprintf LOCK_dprintf

#undef  nprintf
#define nprintf LOCK_nprintf

#undef  sprintf
#define sprintf LOCK_sprintf

#undef  _setjmp
#define _setjmp LOCK__setjmp

#undef  setjmp
#define setjmp LOCK__setjmp

#undef  longjmp
#define longjmp LOCK_longjmp

#undef  strchr
#define strchr LOCK_strchr

#undef  strcspn
#define strcspn LOCK_strcspn

#undef  _strdup
#define _strdup LOCK__strdup

#undef  strdup
#define strdup LOCK__strdup

#undef  _stricmp
#define _stricmp LOCK__stricmp

#undef  stricmp
#define stricmp LOCK__stricmp

#undef  __bc_strlen
#define __bc_strlen LOCK___bc_strlen

#undef  _bc_strlen
#define _bc_strlen LOCK___bc_strlen

#undef  strncat
#define strncat LOCK_strncat

#undef  strncmp
#define strncmp LOCK_strncmp

#undef  strncpy
#define strncpy LOCK_strncpy

#undef  _strnicmp
#define _strnicmp LOCK__strnicmp

#undef  strnicmp
#define strnicmp LOCK__strnicmp

#undef  _strnset
#define _strnset LOCK__strnset

#undef  strnset
#define strnset LOCK__strnset

#undef  _strupr
#define _strupr LOCK__strupr

#undef  strupr
#define strupr LOCK__strupr

#undef  _strlwr
#define _strlwr LOCK__strlwr

#undef  strlwr
#define strlwr LOCK__strlwr

#undef  _strrev
#define _strrev LOCK__strrev

#undef  strrev
#define strrev LOCK__strrev

#undef  strrchr
#define strrchr LOCK_strrchr

#undef  strspn
#define strspn LOCK_strspn

#undef  strpbrk
#define strpbrk LOCK_strpbrk

#undef  strstr
#define strstr LOCK_strstr

#undef  atol
#define atol LOCK_atol

#undef  _ltoa
#define _ltoa LOCK__ltoa

#undef  ltoa
#define ltoa LOCK__ltoa

#undef  strtol
#define strtol LOCK_strtol

#undef  _ultoa
#define _ultoa LOCK__ultoa

#undef  ultoa
#define ultoa LOCK__ultoa

#undef  strtoul
#define strtoul LOCK_strtoul

#undef  sscanf
#define sscanf LOCK_sscanf

#undef  _swab
#define _swab LOCK__swab

#undef  swab
#define swab LOCK__swab

#undef  memmove
#define memmove LOCK_memmove

#undef  _memicmp
#define _memicmp LOCK__memicmp

#undef  memicmp
#define memicmp LOCK__memicmp

#undef  memchr
#define memchr LOCK_memchr

#undef  _memccpy
#define _memccpy LOCK__memccpy

#undef  memccpy
#define memccpy LOCK__memccpy

#undef  _CTAB
#define _CTAB LOCK__CTAB

#undef  CTAB
#define CTAB LOCK__CTAB

extern char _CTAB[256];
#undef  free
#define free LOCK_free

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
