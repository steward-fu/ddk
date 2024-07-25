// msintrin.h - intrinsic function definitions for Microsoft C/C++
// Copyright (c) 1994, Compuware Corporation

void *		__cdecl _alloca(size_t);
unsigned long	__cdecl _lrotl(unsigned long, int);
unsigned long	__cdecl _lrotr(unsigned long, int);
unsigned int	__cdecl _rotl(unsigned int, int);
unsigned int	__cdecl _rotr(unsigned int, int);
char *		__cdecl _strset(char *, int);
int		__cdecl abs(int);
long		__cdecl labs(long);
int		__cdecl memcmp(const void *, const void *, size_t);
void *		__cdecl memcpy(void *, const void *, size_t);
void *		__cdecl memset(void *, int, size_t);
char *		__cdecl strcat(char *, const char *);
int		__cdecl strcmp(const char *, const char *);
char *		__cdecl strcpy(char *, const char *);
size_t		__cdecl strlen(const char *);
void		__cdecl _disable(void);
void		__cdecl _enable(void);

#if _MSC_VER >= 1000
int			__cdecl _inp(unsigned short);
unsigned short	__cdecl _inpw(unsigned short);
unsigned long	__cdecl _inpd(unsigned short);
#endif
int		__cdecl _outp(unsigned short, int);
unsigned short	__cdecl _outpw(unsigned short, unsigned short);
unsigned long	__cdecl _outpd(unsigned short, unsigned long);
double		__cdecl fabs(double);

#pragma intrinsic(_alloca)
#pragma intrinsic(_lrotl)
#pragma intrinsic(_lrotr)
#pragma intrinsic(_rotl)
#pragma intrinsic(_rotr)
#pragma intrinsic(_strset)
#pragma intrinsic(abs)
#pragma intrinsic(labs)
#pragma intrinsic(memcmp)
#pragma intrinsic(memcpy)
#pragma intrinsic(memset)
#pragma intrinsic(strcat)
#pragma intrinsic(strcmp)
#pragma intrinsic(strcpy)
#pragma intrinsic(strlen)
#pragma intrinsic(_disable)
#pragma intrinsic(_enable)
#pragma intrinsic(_outp)
#pragma intrinsic(_outpw)
#pragma intrinsic(_outpd)


// Compiler can make these intrinsic, but we will make them
// functions (and then NOT include the library) so the user
// will get an error.

#pragma function(fabs)
double		__cdecl __fabs__(double);

// The compiler generates bad code for _inp, inpw, and _inpd when the port
// argument is less than 0x100. Force these calls to use the run time lib
// substitutes for MSVC versions before 4.0

#if _MSC_VER >= 1000
#pragma intrinsic(_inp)
#pragma intrinsic(_inpw)
#pragma intrinsic(_inpd)
#else
#define _inp(a) 	Vinportb(a)
#define _inpw(a)	Vinportw(a)
#define _inpd(a)	Vinportd(a)
#endif

