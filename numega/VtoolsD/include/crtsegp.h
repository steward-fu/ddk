//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  _ultoa64
#define _ultoa64 PAGEABLE__ultoa64

#undef  ultoa64
#define ultoa64 PAGEABLE__ultoa64

#undef  _ltoa64
#define _ltoa64 PAGEABLE__ltoa64

#undef  ltoa64
#define ltoa64 PAGEABLE__ltoa64

#undef  MapPointerToFlat
#define MapPointerToFlat PAGEABLE_MapPointerToFlat

#undef  ustrcpy
#define ustrcpy PAGEABLE_ustrcpy

#undef  ustrlen
#define ustrlen PAGEABLE_ustrlen

#undef  __bc_memcmp
#define __bc_memcmp PAGEABLE___bc_memcmp

#undef  _bc_memcmp
#define _bc_memcmp PAGEABLE___bc_memcmp

#undef  __bc_memcpy
#define __bc_memcpy PAGEABLE___bc_memcpy

#undef  _bc_memcpy
#define _bc_memcpy PAGEABLE___bc_memcpy

#undef  __bc_memset
#define __bc_memset PAGEABLE___bc_memset

#undef  _bc_memset
#define _bc_memset PAGEABLE___bc_memset

#undef  Voutportb
#define Voutportb PAGEABLE_Voutportb

#undef  Voutportw
#define Voutportw PAGEABLE_Voutportw

#undef  Voutportd
#define Voutportd PAGEABLE_Voutportd

#undef  Vinportb
#define Vinportb PAGEABLE_Vinportb

#undef  Vinportw
#define Vinportw PAGEABLE_Vinportw

#undef  Vinportd
#define Vinportd PAGEABLE_Vinportd

#undef  i_close
#define i_close PAGEABLE_i_close

#undef  i_remove
#define i_remove PAGEABLE_i_remove

#undef  i_read
#define i_read PAGEABLE_i_read

#undef  i_write
#define i_write PAGEABLE_i_write

#undef  __write_bin
#define __write_bin PAGEABLE___write_bin

#undef  _write_bin
#define _write_bin PAGEABLE___write_bin

#undef  __write_text
#define __write_text PAGEABLE___write_text

#undef  _write_text
#define _write_text PAGEABLE___write_text

#undef  i_lseek
#define i_lseek PAGEABLE_i_lseek

#undef  i_open
#define i_open PAGEABLE_i_open

#undef  vsprintf
#define vsprintf PAGEABLE_vsprintf

#undef  _vsnprintf
#define _vsnprintf PAGEABLE__vsnprintf

#undef  vsnprintf
#define vsnprintf PAGEABLE__vsnprintf

#undef  _snprintf
#define _snprintf PAGEABLE__snprintf

#undef  snprintf
#define snprintf PAGEABLE__snprintf

#undef  realloc
#define realloc PAGEABLE_realloc

#undef  calloc
#define calloc PAGEABLE_calloc

#undef  malloc
#define malloc PAGEABLE_malloc

#undef  dprintf
#define dprintf PAGEABLE_dprintf

#undef  nprintf
#define nprintf PAGEABLE_nprintf

#undef  sprintf
#define sprintf PAGEABLE_sprintf

#undef  _setjmp
#define _setjmp PAGEABLE__setjmp

#undef  setjmp
#define setjmp PAGEABLE__setjmp

#undef  longjmp
#define longjmp PAGEABLE_longjmp

#undef  strchr
#define strchr PAGEABLE_strchr

#undef  strcspn
#define strcspn PAGEABLE_strcspn

#undef  _strdup
#define _strdup PAGEABLE__strdup

#undef  strdup
#define strdup PAGEABLE__strdup

#undef  _stricmp
#define _stricmp PAGEABLE__stricmp

#undef  stricmp
#define stricmp PAGEABLE__stricmp

#undef  __bc_strlen
#define __bc_strlen PAGEABLE___bc_strlen

#undef  _bc_strlen
#define _bc_strlen PAGEABLE___bc_strlen

#undef  strncat
#define strncat PAGEABLE_strncat

#undef  strncmp
#define strncmp PAGEABLE_strncmp

#undef  strncpy
#define strncpy PAGEABLE_strncpy

#undef  _strnicmp
#define _strnicmp PAGEABLE__strnicmp

#undef  strnicmp
#define strnicmp PAGEABLE__strnicmp

#undef  _strnset
#define _strnset PAGEABLE__strnset

#undef  strnset
#define strnset PAGEABLE__strnset

#undef  _strupr
#define _strupr PAGEABLE__strupr

#undef  strupr
#define strupr PAGEABLE__strupr

#undef  _strlwr
#define _strlwr PAGEABLE__strlwr

#undef  strlwr
#define strlwr PAGEABLE__strlwr

#undef  _strrev
#define _strrev PAGEABLE__strrev

#undef  strrev
#define strrev PAGEABLE__strrev

#undef  strrchr
#define strrchr PAGEABLE_strrchr

#undef  strspn
#define strspn PAGEABLE_strspn

#undef  strpbrk
#define strpbrk PAGEABLE_strpbrk

#undef  strstr
#define strstr PAGEABLE_strstr

#undef  atol
#define atol PAGEABLE_atol

#undef  _ltoa
#define _ltoa PAGEABLE__ltoa

#undef  ltoa
#define ltoa PAGEABLE__ltoa

#undef  strtol
#define strtol PAGEABLE_strtol

#undef  _ultoa
#define _ultoa PAGEABLE__ultoa

#undef  ultoa
#define ultoa PAGEABLE__ultoa

#undef  strtoul
#define strtoul PAGEABLE_strtoul

#undef  sscanf
#define sscanf PAGEABLE_sscanf

#undef  _swab
#define _swab PAGEABLE__swab

#undef  swab
#define swab PAGEABLE__swab

#undef  memmove
#define memmove PAGEABLE_memmove

#undef  _memicmp
#define _memicmp PAGEABLE__memicmp

#undef  memicmp
#define memicmp PAGEABLE__memicmp

#undef  memchr
#define memchr PAGEABLE_memchr

#undef  _memccpy
#define _memccpy PAGEABLE__memccpy

#undef  memccpy
#define memccpy PAGEABLE__memccpy

#undef  _CTAB
#define _CTAB PAGEABLE__CTAB

#undef  CTAB
#define CTAB PAGEABLE__CTAB

extern char _CTAB[256];
#undef  free
#define free PAGEABLE_free

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
