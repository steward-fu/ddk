//  Copyright (c) 1994-1999 by Compuware Corporation.
//  All rights reserved

PCHAR __cdecl LOCK__ultoa64 (unsigned _int64 u, char* s, int radix) ;
PCHAR __cdecl INIT__ultoa64 (unsigned _int64 u, char* s, int radix) ;
PCHAR __cdecl DEBUG__ultoa64 (unsigned _int64 u, char* s, int radix) ;
PCHAR __cdecl STATIC__ultoa64 (unsigned _int64 u, char* s, int radix) ;
PCHAR __cdecl PAGEABLE__ultoa64 (unsigned _int64 u, char* s, int radix) ;

PCHAR __cdecl LOCK__ltoa64 (_int64 value, char* s, int radix) ;
PCHAR __cdecl INIT__ltoa64 (_int64 value, char* s, int radix) ;
PCHAR __cdecl DEBUG__ltoa64 (_int64 value, char* s, int radix) ;
PCHAR __cdecl STATIC__ltoa64 (_int64 value, char* s, int radix) ;
PCHAR __cdecl PAGEABLE__ltoa64 (_int64 value, char* s, int radix) ;

PVOID __cdecl LOCK_MapPointerToFlat (WORD sel, DWORD dwOff, VMHANDLE hVM) ;
PVOID __cdecl INIT_MapPointerToFlat (WORD sel, DWORD dwOff, VMHANDLE hVM) ;
PVOID __cdecl DEBUG_MapPointerToFlat (WORD sel, DWORD dwOff, VMHANDLE hVM) ;
PVOID __cdecl STATIC_MapPointerToFlat (WORD sel, DWORD dwOff, VMHANDLE hVM) ;
PVOID __cdecl PAGEABLE_MapPointerToFlat (WORD sel, DWORD dwOff, VMHANDLE hVM) ;

PWORD __cdecl LOCK_ustrcpy (WORD* dest, WORD* src) ;
PWORD __cdecl INIT_ustrcpy (WORD* dest, WORD* src) ;
PWORD __cdecl DEBUG_ustrcpy (WORD* dest, WORD* src) ;
PWORD __cdecl STATIC_ustrcpy (WORD* dest, WORD* src) ;
PWORD __cdecl PAGEABLE_ustrcpy (WORD* dest, WORD* src) ;

UINT __cdecl LOCK_ustrlen (WORD* pUniString) ;
UINT __cdecl INIT_ustrlen (WORD* pUniString) ;
UINT __cdecl DEBUG_ustrlen (WORD* pUniString) ;
UINT __cdecl STATIC_ustrlen (WORD* pUniString) ;
UINT __cdecl PAGEABLE_ustrlen (WORD* pUniString) ;

int __cdecl LOCK___bc_memcmp (const char* s1, const char* s2, size_t c) ;
int __cdecl INIT___bc_memcmp (const char* s1, const char* s2, size_t c) ;
int __cdecl DEBUG___bc_memcmp (const char* s1, const char* s2, size_t c) ;
int __cdecl STATIC___bc_memcmp (const char* s1, const char* s2, size_t c) ;
int __cdecl PAGEABLE___bc_memcmp (const char* s1, const char* s2, size_t c) ;

PVOID __cdecl LOCK___bc_memcpy ( void *dest, const void *src, size_t count) ;
PVOID __cdecl INIT___bc_memcpy ( void *dest, const void *src, size_t count) ;
PVOID __cdecl DEBUG___bc_memcpy ( void *dest, const void *src, size_t count) ;
PVOID __cdecl STATIC___bc_memcpy ( void *dest, const void *src, size_t count) ;
PVOID __cdecl PAGEABLE___bc_memcpy ( void *dest, const void *src, size_t count) ;

PVOID __cdecl LOCK___bc_memset ( void *dest, int c, size_t count) ;
PVOID __cdecl INIT___bc_memset ( void *dest, int c, size_t count) ;
PVOID __cdecl DEBUG___bc_memset ( void *dest, int c, size_t count) ;
PVOID __cdecl STATIC___bc_memset ( void *dest, int c, size_t count) ;
PVOID __cdecl PAGEABLE___bc_memset ( void *dest, int c, size_t count) ;

UCHAR __cdecl LOCK_Voutportb (USHORT port, UCHAR b) ;
UCHAR __cdecl INIT_Voutportb (USHORT port, UCHAR b) ;
UCHAR __cdecl DEBUG_Voutportb (USHORT port, UCHAR b) ;
UCHAR __cdecl STATIC_Voutportb (USHORT port, UCHAR b) ;
UCHAR __cdecl PAGEABLE_Voutportb (USHORT port, UCHAR b) ;

USHORT __cdecl LOCK_Voutportw (USHORT port, USHORT w) ;
USHORT __cdecl INIT_Voutportw (USHORT port, USHORT w) ;
USHORT __cdecl DEBUG_Voutportw (USHORT port, USHORT w) ;
USHORT __cdecl STATIC_Voutportw (USHORT port, USHORT w) ;
USHORT __cdecl PAGEABLE_Voutportw (USHORT port, USHORT w) ;

ULONG __cdecl LOCK_Voutportd (USHORT port, ULONG d) ;
ULONG __cdecl INIT_Voutportd (USHORT port, ULONG d) ;
ULONG __cdecl DEBUG_Voutportd (USHORT port, ULONG d) ;
ULONG __cdecl STATIC_Voutportd (USHORT port, ULONG d) ;
ULONG __cdecl PAGEABLE_Voutportd (USHORT port, ULONG d) ;

UCHAR __cdecl LOCK_Vinportb (USHORT port) ;
UCHAR __cdecl INIT_Vinportb (USHORT port) ;
UCHAR __cdecl DEBUG_Vinportb (USHORT port) ;
UCHAR __cdecl STATIC_Vinportb (USHORT port) ;
UCHAR __cdecl PAGEABLE_Vinportb (USHORT port) ;

USHORT __cdecl LOCK_Vinportw (USHORT port) ;
USHORT __cdecl INIT_Vinportw (USHORT port) ;
USHORT __cdecl DEBUG_Vinportw (USHORT port) ;
USHORT __cdecl STATIC_Vinportw (USHORT port) ;
USHORT __cdecl PAGEABLE_Vinportw (USHORT port) ;

ULONG __cdecl LOCK_Vinportd (USHORT port) ;
ULONG __cdecl INIT_Vinportd (USHORT port) ;
ULONG __cdecl DEBUG_Vinportd (USHORT port) ;
ULONG __cdecl STATIC_Vinportd (USHORT port) ;
ULONG __cdecl PAGEABLE_Vinportd (USHORT port) ;

int __cdecl LOCK_i_close (int handle) ;
int __cdecl INIT_i_close (int handle) ;
int __cdecl DEBUG_i_close (int handle) ;
int __cdecl STATIC_i_close (int handle) ;
int __cdecl PAGEABLE_i_close (int handle) ;

int __cdecl LOCK_i_remove (const char* filename) ;
int __cdecl INIT_i_remove (const char* filename) ;
int __cdecl DEBUG_i_remove (const char* filename) ;
int __cdecl STATIC_i_remove (const char* filename) ;
int __cdecl PAGEABLE_i_remove (const char* filename) ;

int __cdecl LOCK_i_read (int handle, void *buffer, unsigned int count) ;
int __cdecl INIT_i_read (int handle, void *buffer, unsigned int count) ;
int __cdecl DEBUG_i_read (int handle, void *buffer, unsigned int count) ;
int __cdecl STATIC_i_read (int handle, void *buffer, unsigned int count) ;
int __cdecl PAGEABLE_i_read (int handle, void *buffer, unsigned int count) ;

int __cdecl LOCK_i_write (int handle, void *buffer, unsigned int count) ;
int __cdecl INIT_i_write (int handle, void *buffer, unsigned int count) ;
int __cdecl DEBUG_i_write (int handle, void *buffer, unsigned int count) ;
int __cdecl STATIC_i_write (int handle, void *buffer, unsigned int count) ;
int __cdecl PAGEABLE_i_write (int handle, void *buffer, unsigned int count) ;

int __cdecl LOCK___write_bin (int handle, char *buffer, unsigned int count, CLIENT_STRUCT* pRegsa) ;
int __cdecl INIT___write_bin (int handle, char *buffer, unsigned int count, CLIENT_STRUCT* pRegsa) ;
int __cdecl DEBUG___write_bin (int handle, char *buffer, unsigned int count, CLIENT_STRUCT* pRegsa) ;
int __cdecl STATIC___write_bin (int handle, char *buffer, unsigned int count, CLIENT_STRUCT* pRegsa) ;
int __cdecl PAGEABLE___write_bin (int handle, char *buffer, unsigned int count, CLIENT_STRUCT* pRegsa) ;

int __cdecl LOCK___write_text (int handle, char *buf, unsigned int count, CLIENT_STRUCT* pRegs) ;
int __cdecl INIT___write_text (int handle, char *buf, unsigned int count, CLIENT_STRUCT* pRegs) ;
int __cdecl DEBUG___write_text (int handle, char *buf, unsigned int count, CLIENT_STRUCT* pRegs) ;
int __cdecl STATIC___write_text (int handle, char *buf, unsigned int count, CLIENT_STRUCT* pRegs) ;
int __cdecl PAGEABLE___write_text (int handle, char *buf, unsigned int count, CLIENT_STRUCT* pRegs) ;

long __cdecl LOCK_i_lseek (int handle, long offset, int origin) ;
long __cdecl INIT_i_lseek (int handle, long offset, int origin) ;
long __cdecl DEBUG_i_lseek (int handle, long offset, int origin) ;
long __cdecl STATIC_i_lseek (int handle, long offset, int origin) ;
long __cdecl PAGEABLE_i_lseek (int handle, long offset, int origin) ;

int __cdecl LOCK_i_open (const char* filename, int oflag, int pmode) ;
int __cdecl INIT_i_open (const char* filename, int oflag, int pmode) ;
int __cdecl DEBUG_i_open (const char* filename, int oflag, int pmode) ;
int __cdecl STATIC_i_open (const char* filename, int oflag, int pmode) ;
int __cdecl PAGEABLE_i_open (const char* filename, int oflag, int pmode) ;

int __cdecl LOCK_vsprintf (char *buffer, const char *format, va_list Next) ;
int __cdecl INIT_vsprintf (char *buffer, const char *format, va_list Next) ;
int __cdecl DEBUG_vsprintf (char *buffer, const char *format, va_list Next) ;
int __cdecl STATIC_vsprintf (char *buffer, const char *format, va_list Next) ;
int __cdecl PAGEABLE_vsprintf (char *buffer, const char *format, va_list Next) ;

int __cdecl LOCK__vsnprintf (char *buffer, size_t Limit, const char *format, va_list Next) ;
int __cdecl INIT__vsnprintf (char *buffer, size_t Limit, const char *format, va_list Next) ;
int __cdecl DEBUG__vsnprintf (char *buffer, size_t Limit, const char *format, va_list Next) ;
int __cdecl STATIC__vsnprintf (char *buffer, size_t Limit, const char *format, va_list Next) ;
int __cdecl PAGEABLE__vsnprintf (char *buffer, size_t Limit, const char *format, va_list Next) ;

int __cdecl LOCK__snprintf (char *buffer, size_t Limit, const char *format, ...) ;
int __cdecl INIT__snprintf (char *buffer, size_t Limit, const char *format, ...) ;
int __cdecl DEBUG__snprintf (char *buffer, size_t Limit, const char *format, ...) ;
int __cdecl STATIC__snprintf (char *buffer, size_t Limit, const char *format, ...) ;
int __cdecl PAGEABLE__snprintf (char *buffer, size_t Limit, const char *format, ...) ;

PVOID __cdecl LOCK_realloc (void *memblock, size_t size) ;
PVOID __cdecl INIT_realloc (void *memblock, size_t size) ;
PVOID __cdecl DEBUG_realloc (void *memblock, size_t size) ;
PVOID __cdecl STATIC_realloc (void *memblock, size_t size) ;
PVOID __cdecl PAGEABLE_realloc (void *memblock, size_t size) ;

PVOID __cdecl LOCK_calloc (size_t num, size_t size) ;
PVOID __cdecl INIT_calloc (size_t num, size_t size) ;
PVOID __cdecl DEBUG_calloc (size_t num, size_t size) ;
PVOID __cdecl STATIC_calloc (size_t num, size_t size) ;
PVOID __cdecl PAGEABLE_calloc (size_t num, size_t size) ;

PVOID __cdecl LOCK_malloc (size_t size) ;
PVOID __cdecl INIT_malloc (size_t size) ;
PVOID __cdecl DEBUG_malloc (size_t size) ;
PVOID __cdecl STATIC_malloc (size_t size) ;
PVOID __cdecl PAGEABLE_malloc (size_t size) ;

VOID __cdecl LOCK_dprintf (const char *format, ...) ;
VOID __cdecl INIT_dprintf (const char *format, ...) ;
VOID __cdecl DEBUG_dprintf (const char *format, ...) ;
VOID __cdecl STATIC_dprintf (const char *format, ...) ;
VOID __cdecl PAGEABLE_dprintf (const char *format, ...) ;

VOID __cdecl LOCK_nprintf (const char *format, ...) ;
VOID __cdecl INIT_nprintf (const char *format, ...) ;
VOID __cdecl DEBUG_nprintf (const char *format, ...) ;
VOID __cdecl STATIC_nprintf (const char *format, ...) ;
VOID __cdecl PAGEABLE_nprintf (const char *format, ...) ;

int __cdecl LOCK_sprintf (char *buffer, const char *format, ...) ;
int __cdecl INIT_sprintf (char *buffer, const char *format, ...) ;
int __cdecl DEBUG_sprintf (char *buffer, const char *format, ...) ;
int __cdecl STATIC_sprintf (char *buffer, const char *format, ...) ;
int __cdecl PAGEABLE_sprintf (char *buffer, const char *format, ...) ;

int __cdecl LOCK__setjmp (jmp_buf jbuf) ;
int __cdecl INIT__setjmp (jmp_buf jbuf) ;
int __cdecl DEBUG__setjmp (jmp_buf jbuf) ;
int __cdecl STATIC__setjmp (jmp_buf jbuf) ;
int __cdecl PAGEABLE__setjmp (jmp_buf jbuf) ;

int __cdecl LOCK_longjmp (jmp_buf jbuf, int value) ;
int __cdecl INIT_longjmp (jmp_buf jbuf, int value) ;
int __cdecl DEBUG_longjmp (jmp_buf jbuf, int value) ;
int __cdecl STATIC_longjmp (jmp_buf jbuf, int value) ;
int __cdecl PAGEABLE_longjmp (jmp_buf jbuf, int value) ;

PCHAR __cdecl LOCK_strchr ( const char* s, int c) ;
PCHAR __cdecl INIT_strchr ( const char* s, int c) ;
PCHAR __cdecl DEBUG_strchr ( const char* s, int c) ;
PCHAR __cdecl STATIC_strchr ( const char* s, int c) ;
PCHAR __cdecl PAGEABLE_strchr ( const char* s, int c) ;

size_t __cdecl LOCK_strcspn ( const char* s1, const char* s2) ;
size_t __cdecl INIT_strcspn ( const char* s1, const char* s2) ;
size_t __cdecl DEBUG_strcspn ( const char* s1, const char* s2) ;
size_t __cdecl STATIC_strcspn ( const char* s1, const char* s2) ;
size_t __cdecl PAGEABLE_strcspn ( const char* s1, const char* s2) ;

PCHAR __cdecl LOCK__strdup ( const char* s) ;
PCHAR __cdecl INIT__strdup ( const char* s) ;
PCHAR __cdecl DEBUG__strdup ( const char* s) ;
PCHAR __cdecl STATIC__strdup ( const char* s) ;
PCHAR __cdecl PAGEABLE__strdup ( const char* s) ;

int __cdecl LOCK__stricmp ( const char* s1, const char* s2) ;
int __cdecl INIT__stricmp ( const char* s1, const char* s2) ;
int __cdecl DEBUG__stricmp ( const char* s1, const char* s2) ;
int __cdecl STATIC__stricmp ( const char* s1, const char* s2) ;
int __cdecl PAGEABLE__stricmp ( const char* s1, const char* s2) ;

size_t __cdecl LOCK___bc_strlen ( const char* s) ;
size_t __cdecl INIT___bc_strlen ( const char* s) ;
size_t __cdecl DEBUG___bc_strlen ( const char* s) ;
size_t __cdecl STATIC___bc_strlen ( const char* s) ;
size_t __cdecl PAGEABLE___bc_strlen ( const char* s) ;

PCHAR __cdecl LOCK_strncat ( char* s1, const char* s2, size_t c) ;
PCHAR __cdecl INIT_strncat ( char* s1, const char* s2, size_t c) ;
PCHAR __cdecl DEBUG_strncat ( char* s1, const char* s2, size_t c) ;
PCHAR __cdecl STATIC_strncat ( char* s1, const char* s2, size_t c) ;
PCHAR __cdecl PAGEABLE_strncat ( char* s1, const char* s2, size_t c) ;

int __cdecl LOCK_strncmp (const char* s1, const char* s2, size_t c) ;
int __cdecl INIT_strncmp (const char* s1, const char* s2, size_t c) ;
int __cdecl DEBUG_strncmp (const char* s1, const char* s2, size_t c) ;
int __cdecl STATIC_strncmp (const char* s1, const char* s2, size_t c) ;
int __cdecl PAGEABLE_strncmp (const char* s1, const char* s2, size_t c) ;

PCHAR __cdecl LOCK_strncpy ( char* s1, const char* s2, size_t c) ;
PCHAR __cdecl INIT_strncpy ( char* s1, const char* s2, size_t c) ;
PCHAR __cdecl DEBUG_strncpy ( char* s1, const char* s2, size_t c) ;
PCHAR __cdecl STATIC_strncpy ( char* s1, const char* s2, size_t c) ;
PCHAR __cdecl PAGEABLE_strncpy ( char* s1, const char* s2, size_t c) ;

int __cdecl LOCK__strnicmp ( const char* s1, const char* s2, size_t c) ;
int __cdecl INIT__strnicmp ( const char* s1, const char* s2, size_t c) ;
int __cdecl DEBUG__strnicmp ( const char* s1, const char* s2, size_t c) ;
int __cdecl STATIC__strnicmp ( const char* s1, const char* s2, size_t c) ;
int __cdecl PAGEABLE__strnicmp ( const char* s1, const char* s2, size_t c) ;

PCHAR __cdecl LOCK__strnset (char* s, int c, size_t n) ;
PCHAR __cdecl INIT__strnset (char* s, int c, size_t n) ;
PCHAR __cdecl DEBUG__strnset (char* s, int c, size_t n) ;
PCHAR __cdecl STATIC__strnset (char* s, int c, size_t n) ;
PCHAR __cdecl PAGEABLE__strnset (char* s, int c, size_t n) ;

PCHAR __cdecl LOCK__strupr ( char* s) ;
PCHAR __cdecl INIT__strupr ( char* s) ;
PCHAR __cdecl DEBUG__strupr ( char* s) ;
PCHAR __cdecl STATIC__strupr ( char* s) ;
PCHAR __cdecl PAGEABLE__strupr ( char* s) ;

PCHAR __cdecl LOCK__strlwr (char* s) ;
PCHAR __cdecl INIT__strlwr (char* s) ;
PCHAR __cdecl DEBUG__strlwr (char* s) ;
PCHAR __cdecl STATIC__strlwr (char* s) ;
PCHAR __cdecl PAGEABLE__strlwr (char* s) ;

PCHAR __cdecl LOCK__strrev ( char* s) ;
PCHAR __cdecl INIT__strrev ( char* s) ;
PCHAR __cdecl DEBUG__strrev ( char* s) ;
PCHAR __cdecl STATIC__strrev ( char* s) ;
PCHAR __cdecl PAGEABLE__strrev ( char* s) ;

PCHAR __cdecl LOCK_strrchr ( const char* s, int c) ;
PCHAR __cdecl INIT_strrchr ( const char* s, int c) ;
PCHAR __cdecl DEBUG_strrchr ( const char* s, int c) ;
PCHAR __cdecl STATIC_strrchr ( const char* s, int c) ;
PCHAR __cdecl PAGEABLE_strrchr ( const char* s, int c) ;

size_t __cdecl LOCK_strspn ( const char* s1, const char* s2) ;
size_t __cdecl INIT_strspn ( const char* s1, const char* s2) ;
size_t __cdecl DEBUG_strspn ( const char* s1, const char* s2) ;
size_t __cdecl STATIC_strspn ( const char* s1, const char* s2) ;
size_t __cdecl PAGEABLE_strspn ( const char* s1, const char* s2) ;

PCHAR __cdecl LOCK_strpbrk ( const char* s1, const char* s2) ;
PCHAR __cdecl INIT_strpbrk ( const char* s1, const char* s2) ;
PCHAR __cdecl DEBUG_strpbrk ( const char* s1, const char* s2) ;
PCHAR __cdecl STATIC_strpbrk ( const char* s1, const char* s2) ;
PCHAR __cdecl PAGEABLE_strpbrk ( const char* s1, const char* s2) ;

PCHAR __cdecl LOCK_strstr ( const char* s1, const char* s2) ;
PCHAR __cdecl INIT_strstr ( const char* s1, const char* s2) ;
PCHAR __cdecl DEBUG_strstr ( const char* s1, const char* s2) ;
PCHAR __cdecl STATIC_strstr ( const char* s1, const char* s2) ;
PCHAR __cdecl PAGEABLE_strstr ( const char* s1, const char* s2) ;

long __cdecl LOCK_atol (const char* s) ;
long __cdecl INIT_atol (const char* s) ;
long __cdecl DEBUG_atol (const char* s) ;
long __cdecl STATIC_atol (const char* s) ;
long __cdecl PAGEABLE_atol (const char* s) ;

PCHAR __cdecl LOCK__ltoa (long value, char* s, int radix) ;
PCHAR __cdecl INIT__ltoa (long value, char* s, int radix) ;
PCHAR __cdecl DEBUG__ltoa (long value, char* s, int radix) ;
PCHAR __cdecl STATIC__ltoa (long value, char* s, int radix) ;
PCHAR __cdecl PAGEABLE__ltoa (long value, char* s, int radix) ;

long __cdecl LOCK_strtol (const char* s, char** pend, int radix) ;
long __cdecl INIT_strtol (const char* s, char** pend, int radix) ;
long __cdecl DEBUG_strtol (const char* s, char** pend, int radix) ;
long __cdecl STATIC_strtol (const char* s, char** pend, int radix) ;
long __cdecl PAGEABLE_strtol (const char* s, char** pend, int radix) ;

PCHAR __cdecl LOCK__ultoa (unsigned long u, char* s, int radix) ;
PCHAR __cdecl INIT__ultoa (unsigned long u, char* s, int radix) ;
PCHAR __cdecl DEBUG__ultoa (unsigned long u, char* s, int radix) ;
PCHAR __cdecl STATIC__ultoa (unsigned long u, char* s, int radix) ;
PCHAR __cdecl PAGEABLE__ultoa (unsigned long u, char* s, int radix) ;

DWORD __cdecl LOCK_strtoul (const char* s, char** pend, int radix) ;
DWORD __cdecl INIT_strtoul (const char* s, char** pend, int radix) ;
DWORD __cdecl DEBUG_strtoul (const char* s, char** pend, int radix) ;
DWORD __cdecl STATIC_strtoul (const char* s, char** pend, int radix) ;
DWORD __cdecl PAGEABLE_strtoul (const char* s, char** pend, int radix) ;

int __cdecl LOCK_sscanf (const char* buf, const char* fmt, ...) ;
int __cdecl INIT_sscanf (const char* buf, const char* fmt, ...) ;
int __cdecl DEBUG_sscanf (const char* buf, const char* fmt, ...) ;
int __cdecl STATIC_sscanf (const char* buf, const char* fmt, ...) ;
int __cdecl PAGEABLE_sscanf (const char* buf, const char* fmt, ...) ;

VOID __cdecl LOCK__swab (char *src, char *dest, int n) ;
VOID __cdecl INIT__swab (char *src, char *dest, int n) ;
VOID __cdecl DEBUG__swab (char *src, char *dest, int n) ;
VOID __cdecl STATIC__swab (char *src, char *dest, int n) ;
VOID __cdecl PAGEABLE__swab (char *src, char *dest, int n) ;

PVOID __cdecl LOCK_memmove (void *dest, void *src, UINT count) ;
PVOID __cdecl INIT_memmove (void *dest, void *src, UINT count) ;
PVOID __cdecl DEBUG_memmove (void *dest, void *src, UINT count) ;
PVOID __cdecl STATIC_memmove (void *dest, void *src, UINT count) ;
PVOID __cdecl PAGEABLE_memmove (void *dest, void *src, UINT count) ;

int __cdecl LOCK__memicmp (void *buf1, void *buf2, UINT count) ;
int __cdecl INIT__memicmp (void *buf1, void *buf2, UINT count) ;
int __cdecl DEBUG__memicmp (void *buf1, void *buf2, UINT count) ;
int __cdecl STATIC__memicmp (void *buf1, void *buf2, UINT count) ;
int __cdecl PAGEABLE__memicmp (void *buf1, void *buf2, UINT count) ;

PVOID __cdecl LOCK_memchr (CONST VOID *buf, INT c, size_t count) ;
PVOID __cdecl INIT_memchr (CONST VOID *buf, INT c, size_t count) ;
PVOID __cdecl DEBUG_memchr (CONST VOID *buf, INT c, size_t count) ;
PVOID __cdecl STATIC_memchr (CONST VOID *buf, INT c, size_t count) ;
PVOID __cdecl PAGEABLE_memchr (CONST VOID *buf, INT c, size_t count) ;

PVOID __cdecl LOCK__memccpy (PVOID dest, CONST VOID *src, INT c, UINT count) ;
PVOID __cdecl INIT__memccpy (PVOID dest, CONST VOID *src, INT c, UINT count) ;
PVOID __cdecl DEBUG__memccpy (PVOID dest, CONST VOID *src, INT c, UINT count) ;
PVOID __cdecl STATIC__memccpy (PVOID dest, CONST VOID *src, INT c, UINT count) ;
PVOID __cdecl PAGEABLE__memccpy (PVOID dest, CONST VOID *src, INT c, UINT count) ;

VOID __cdecl LOCK_free (CONST PVOID pMemblock) ;
VOID __cdecl INIT_free (CONST PVOID pMemblock) ;
VOID __cdecl DEBUG_free (CONST PVOID pMemblock) ;
VOID __cdecl STATIC_free (CONST PVOID pMemblock) ;
VOID __cdecl PAGEABLE_free (CONST PVOID pMemblock) ;

