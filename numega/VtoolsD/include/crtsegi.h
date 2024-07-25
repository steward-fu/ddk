//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  _ultoa64
#define _ultoa64 INIT__ultoa64

#undef  ultoa64
#define ultoa64 INIT__ultoa64

#undef  _ltoa64
#define _ltoa64 INIT__ltoa64

#undef  ltoa64
#define ltoa64 INIT__ltoa64

#undef  MapPointerToFlat
#define MapPointerToFlat INIT_MapPointerToFlat

#undef  ustrcpy
#define ustrcpy INIT_ustrcpy

#undef  ustrlen
#define ustrlen INIT_ustrlen

#undef  __bc_memcmp
#define __bc_memcmp INIT___bc_memcmp

#undef  _bc_memcmp
#define _bc_memcmp INIT___bc_memcmp

#undef  __bc_memcpy
#define __bc_memcpy INIT___bc_memcpy

#undef  _bc_memcpy
#define _bc_memcpy INIT___bc_memcpy

#undef  __bc_memset
#define __bc_memset INIT___bc_memset

#undef  _bc_memset
#define _bc_memset INIT___bc_memset

#undef  Voutportb
#define Voutportb INIT_Voutportb

#undef  Voutportw
#define Voutportw INIT_Voutportw

#undef  Voutportd
#define Voutportd INIT_Voutportd

#undef  Vinportb
#define Vinportb INIT_Vinportb

#undef  Vinportw
#define Vinportw INIT_Vinportw

#undef  Vinportd
#define Vinportd INIT_Vinportd

#undef  i_close
#define i_close INIT_i_close

#undef  i_remove
#define i_remove INIT_i_remove

#undef  i_read
#define i_read INIT_i_read

#undef  i_write
#define i_write INIT_i_write

#undef  __write_bin
#define __write_bin INIT___write_bin

#undef  _write_bin
#define _write_bin INIT___write_bin

#undef  __write_text
#define __write_text INIT___write_text

#undef  _write_text
#define _write_text INIT___write_text

#undef  i_lseek
#define i_lseek INIT_i_lseek

#undef  i_open
#define i_open INIT_i_open

#undef  vsprintf
#define vsprintf INIT_vsprintf

#undef  _vsnprintf
#define _vsnprintf INIT__vsnprintf

#undef  vsnprintf
#define vsnprintf INIT__vsnprintf

#undef  _snprintf
#define _snprintf INIT__snprintf

#undef  snprintf
#define snprintf INIT__snprintf

#undef  realloc
#define realloc INIT_realloc

#undef  calloc
#define calloc INIT_calloc

#undef  malloc
#define malloc INIT_malloc

#undef  dprintf
#define dprintf INIT_dprintf

#undef  nprintf
#define nprintf INIT_nprintf

#undef  sprintf
#define sprintf INIT_sprintf

#undef  _setjmp
#define _setjmp INIT__setjmp

#undef  setjmp
#define setjmp INIT__setjmp

#undef  longjmp
#define longjmp INIT_longjmp

#undef  strchr
#define strchr INIT_strchr

#undef  strcspn
#define strcspn INIT_strcspn

#undef  _strdup
#define _strdup INIT__strdup

#undef  strdup
#define strdup INIT__strdup

#undef  _stricmp
#define _stricmp INIT__stricmp

#undef  stricmp
#define stricmp INIT__stricmp

#undef  __bc_strlen
#define __bc_strlen INIT___bc_strlen

#undef  _bc_strlen
#define _bc_strlen INIT___bc_strlen

#undef  strncat
#define strncat INIT_strncat

#undef  strncmp
#define strncmp INIT_strncmp

#undef  strncpy
#define strncpy INIT_strncpy

#undef  _strnicmp
#define _strnicmp INIT__strnicmp

#undef  strnicmp
#define strnicmp INIT__strnicmp

#undef  _strnset
#define _strnset INIT__strnset

#undef  strnset
#define strnset INIT__strnset

#undef  _strupr
#define _strupr INIT__strupr

#undef  strupr
#define strupr INIT__strupr

#undef  _strlwr
#define _strlwr INIT__strlwr

#undef  strlwr
#define strlwr INIT__strlwr

#undef  _strrev
#define _strrev INIT__strrev

#undef  strrev
#define strrev INIT__strrev

#undef  strrchr
#define strrchr INIT_strrchr

#undef  strspn
#define strspn INIT_strspn

#undef  strpbrk
#define strpbrk INIT_strpbrk

#undef  strstr
#define strstr INIT_strstr

#undef  atol
#define atol INIT_atol

#undef  _ltoa
#define _ltoa INIT__ltoa

#undef  ltoa
#define ltoa INIT__ltoa

#undef  strtol
#define strtol INIT_strtol

#undef  _ultoa
#define _ultoa INIT__ultoa

#undef  ultoa
#define ultoa INIT__ultoa

#undef  strtoul
#define strtoul INIT_strtoul

#undef  sscanf
#define sscanf INIT_sscanf

#undef  _swab
#define _swab INIT__swab

#undef  swab
#define swab INIT__swab

#undef  memmove
#define memmove INIT_memmove

#undef  _memicmp
#define _memicmp INIT__memicmp

#undef  memicmp
#define memicmp INIT__memicmp

#undef  memchr
#define memchr INIT_memchr

#undef  _memccpy
#define _memccpy INIT__memccpy

#undef  memccpy
#define memccpy INIT__memccpy

#undef  _CTAB
#define _CTAB INIT__CTAB

#undef  CTAB
#define CTAB INIT__CTAB

extern char _CTAB[256];
#undef  free
#define free INIT_free

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
