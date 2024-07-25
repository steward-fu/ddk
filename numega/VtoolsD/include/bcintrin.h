// bcintrin.h - intrinsic function definitions for Borland C/C++ 4.x
// Copyright (c) 1994, Compuware Corporation

int	__cdecl __abs__(int);
#define abs __abs__

void * __cdecl alloca(size_t);
void * __cdecl __alloca__(size_t);

//#pragma intrinsic alloca
#define alloca __alloca__


unsigned long	__cdecl _lrotl(unsigned long, int);
unsigned long	__cdecl ___lrotl__(unsigned long, int);
//#pragma intrinsic _lrotl
#define _lrotl ___lrotl__

unsigned long	__cdecl _lrotr(unsigned long, int);
unsigned long	__cdecl ___lrotr__(unsigned long, int);
//#pragma intrinsic _lrotr
#define _lrotr ___lrotr__

unsigned short __cdecl _rotl(unsigned short, int);
unsigned short __cdecl ___rotl__(unsigned short, int);
#define _rotl ___rotl__

//#pragma intrinsic _rotl
unsigned short __cdecl _rotr(unsigned short, int);
unsigned short __cdecl ___rotr__(unsigned short, int);
//#pragma intrinsic _rotr
#define _rotr ___rotr__

char *	__cdecl strset(char *, int);
char *	__cdecl __strset__(char *, int);
//#pragma intrinsic strset
#define strset __strset__

void *	__cdecl memcpy(void *, const void *, size_t);
void *	__cdecl __memcpy__(void *, const void *, size_t);
//#pragma intrinsic memcpy
#define memcpy __memcpy__

void *	__cdecl memset(void *, int, size_t);
void *	__cdecl __memset__(void *, int, size_t);
//#pragma intrinsic memset
#define memset __memset__

char *	__cdecl strcat(char *, const char *);
char *	__cdecl __strcat__(char *, const char *);
//#pragma intrinsic strcat
#define strcat __strcat__

int	__cdecl strcmp(const char *, const char *);
int	__cdecl __strcmp__(const char *, const char *);
//#pragma intrinsic strcmp
#define strcmp __strcmp__

char *	__cdecl strcpy(char *, const char *);
char *	__cdecl __strcpy__(char *, const char *);
//#pragma intrinsic strcpy
#define strcpy __strcpy__

/*	
 *	BC 4.5 does not appear to provide a workable copy of
 *	STRLEN or MEMCMP.  So, we provide a callable version instead.
 *	See v?code.h for the definitions.
 *
 * int	__cdecl memcmp(const void *, const void *, size_t);
 * int	__cdecl __memcmp__(const void *, const void *, size_t);
 * //#pragma intrinsic memcmp
 * #define memcmp __memcmp__
 *
 * size_t	__cdecl strlen(const char *);
 * size_t	__cdecl __strlen__(const char *);
 * //#pragma intrinsic strlen
 * #define strlen __strlen__
 *
 */

char strncpy(char *dest, const char *src, size_t maxlen);
char *__strncpy__(char *dest, const char *src, size_t maxlen);
//#pragma intrinsic strncpy
#define  strncpy __strncpy__

void memchr(const void *s, int c, size_t n); 
void *__memchr__(const void *s, int c, size_t n); 
//#pragma intrinsic memchr
#define memchr __memchr__

char strrchr(const char *s, int c);
char *__strrchr__(const char *s, int c);
//#pragma intrinsic strrchr
#define strrchr __strrchr__

char strchr(const char *s, int c); 
char *__strchr__(const char *s, int c); 
//#pragma intrinsic strchr
#define strchr __strchr__

char strnset(char *s, int ch, size_t n);
char *__strnset__(char *s, int ch, size_t n);
//#pragma intrinsic strnset
#define strnset __strnset__

char stpcpy(char *dest, const char *src);
char *__stpcpy__(char *dest, const char *src);
//#pragma intrinsic stpcpy
#define stpcpy __stpcpy__

int strncmp(const char *s1, const char *s2, size_t  maxlen);
int __strncmp__(const char *s1, const char *s2, size_t  maxlen);
//#pragma intrinsic strncmp
#define strncmp __strncmp__

char strncat(char *dest, const char *src, size_t maxlen);
char *__strncat__(char *dest, const char *src, size_t maxlen);
//#pragma intrinsic strncat
#define strncat __strncat__

unsigned char _crotl(unsigned char val, int count);
unsigned char ___crotl__(unsigned char val, int count);
//#pragma intrinsic _crotl
#define _crotl ___crotl__

unsigned char _crotr(unsigned char val, int count);
unsigned char ___crotr__(unsigned char val, int count);
//#pragma intrinsic _crotr
#define _crotr ___crotr__

#define _disable()      __emit__((unsigned char)(0xfa))
#define disable()       __emit__((unsigned char)(0xfa))
#define _enable()       __emit__((unsigned char)(0xfb))
#define enable()        __emit__((unsigned char)(0xfb))

// As of version 4.02, all the inline port I/O functions generated bad code.
// Force them to use the C run time library substitutes.

#define _inp(a)		Vinportb(a)
#define  inp(a)		Vinportb(a)
#define  inportb(a)	Vinportb(a)
#define _inpw(a)	Vinportw(a)
#define  inpw(a) 	Vinportw(a)
#define  inportw(a)	Vinportw(a)
#define _inpd(a) 	Vinportd(a)
#define  inpd(a) 	Vinportd(a)

#define _outp(a,b)	Voutportb(a,b)
#define  outp(a,b)	Voutportb(a,b)
#define  outportb(a,b)	Voutportb(a,b)
#define _outpw(a,b)	Voutportw(a,b)
#define  outpw(a,b) 	Voutportw(a,b)
#define  outportw(a,b)	Voutportw(a,b)
#define _outpd(a,b) 	Voutportd(a,b)
#define  outpd(a,b) 	Voutportd(a,b)
