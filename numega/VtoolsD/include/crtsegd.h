//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

#undef  _ultoa64
#define _ultoa64 DEBUG__ultoa64

#undef  ultoa64
#define ultoa64 DEBUG__ultoa64

#undef  _ltoa64
#define _ltoa64 DEBUG__ltoa64

#undef  ltoa64
#define ltoa64 DEBUG__ltoa64

#undef  MapPointerToFlat
#define MapPointerToFlat DEBUG_MapPointerToFlat

#undef  ustrcpy
#define ustrcpy DEBUG_ustrcpy

#undef  ustrlen
#define ustrlen DEBUG_ustrlen

#undef  __bc_memcmp
#define __bc_memcmp DEBUG___bc_memcmp

#undef  _bc_memcmp
#define _bc_memcmp DEBUG___bc_memcmp

#undef  __bc_memcpy
#define __bc_memcpy DEBUG___bc_memcpy

#undef  _bc_memcpy
#define _bc_memcpy DEBUG___bc_memcpy

#undef  __bc_memset
#define __bc_memset DEBUG___bc_memset

#undef  _bc_memset
#define _bc_memset DEBUG___bc_memset

#undef  Voutportb
#define Voutportb DEBUG_Voutportb

#undef  Voutportw
#define Voutportw DEBUG_Voutportw

#undef  Voutportd
#define Voutportd DEBUG_Voutportd

#undef  Vinportb
#define Vinportb DEBUG_Vinportb

#undef  Vinportw
#define Vinportw DEBUG_Vinportw

#undef  Vinportd
#define Vinportd DEBUG_Vinportd

#undef  i_close
#define i_close DEBUG_i_close

#undef  i_remove
#define i_remove DEBUG_i_remove

#undef  i_read
#define i_read DEBUG_i_read

#undef  i_write
#define i_write DEBUG_i_write

#undef  __write_bin
#define __write_bin DEBUG___write_bin

#undef  _write_bin
#define _write_bin DEBUG___write_bin

#undef  __write_text
#define __write_text DEBUG___write_text

#undef  _write_text
#define _write_text DEBUG___write_text

#undef  i_lseek
#define i_lseek DEBUG_i_lseek

#undef  i_open
#define i_open DEBUG_i_open

#undef  vsprintf
#define vsprintf DEBUG_vsprintf

#undef  _vsnprintf
#define _vsnprintf DEBUG__vsnprintf

#undef  vsnprintf
#define vsnprintf DEBUG__vsnprintf

#undef  _snprintf
#define _snprintf DEBUG__snprintf

#undef  snprintf
#define snprintf DEBUG__snprintf

#undef  realloc
#define realloc DEBUG_realloc

#undef  calloc
#define calloc DEBUG_calloc

#undef  malloc
#define malloc DEBUG_malloc

#undef  dprintf
#define dprintf DEBUG_dprintf

#undef  nprintf
#define nprintf DEBUG_nprintf

#undef  sprintf
#define sprintf DEBUG_sprintf

#undef  _setjmp
#define _setjmp DEBUG__setjmp

#undef  setjmp
#define setjmp DEBUG__setjmp

#undef  longjmp
#define longjmp DEBUG_longjmp

#undef  strchr
#define strchr DEBUG_strchr

#undef  strcspn
#define strcspn DEBUG_strcspn

#undef  _strdup
#define _strdup DEBUG__strdup

#undef  strdup
#define strdup DEBUG__strdup

#undef  _stricmp
#define _stricmp DEBUG__stricmp

#undef  stricmp
#define stricmp DEBUG__stricmp

#undef  __bc_strlen
#define __bc_strlen DEBUG___bc_strlen

#undef  _bc_strlen
#define _bc_strlen DEBUG___bc_strlen

#undef  strncat
#define strncat DEBUG_strncat

#undef  strncmp
#define strncmp DEBUG_strncmp

#undef  strncpy
#define strncpy DEBUG_strncpy

#undef  _strnicmp
#define _strnicmp DEBUG__strnicmp

#undef  strnicmp
#define strnicmp DEBUG__strnicmp

#undef  _strnset
#define _strnset DEBUG__strnset

#undef  strnset
#define strnset DEBUG__strnset

#undef  _strupr
#define _strupr DEBUG__strupr

#undef  strupr
#define strupr DEBUG__strupr

#undef  _strlwr
#define _strlwr DEBUG__strlwr

#undef  strlwr
#define strlwr DEBUG__strlwr

#undef  _strrev
#define _strrev DEBUG__strrev

#undef  strrev
#define strrev DEBUG__strrev

#undef  strrchr
#define strrchr DEBUG_strrchr

#undef  strspn
#define strspn DEBUG_strspn

#undef  strpbrk
#define strpbrk DEBUG_strpbrk

#undef  strstr
#define strstr DEBUG_strstr

#undef  atol
#define atol DEBUG_atol

#undef  _ltoa
#define _ltoa DEBUG__ltoa

#undef  ltoa
#define ltoa DEBUG__ltoa

#undef  strtol
#define strtol DEBUG_strtol

#undef  _ultoa
#define _ultoa DEBUG__ultoa

#undef  ultoa
#define ultoa DEBUG__ultoa

#undef  strtoul
#define strtoul DEBUG_strtoul

#undef  sscanf
#define sscanf DEBUG_sscanf

#undef  _swab
#define _swab DEBUG__swab

#undef  swab
#define swab DEBUG__swab

#undef  memmove
#define memmove DEBUG_memmove

#undef  _memicmp
#define _memicmp DEBUG__memicmp

#undef  memicmp
#define memicmp DEBUG__memicmp

#undef  memchr
#define memchr DEBUG_memchr

#undef  _memccpy
#define _memccpy DEBUG__memccpy

#undef  memccpy
#define memccpy DEBUG__memccpy

#undef  _CTAB
#define _CTAB DEBUG__CTAB

#undef  CTAB
#define CTAB DEBUG__CTAB

extern char _CTAB[256];
#undef  free
#define free DEBUG_free

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
