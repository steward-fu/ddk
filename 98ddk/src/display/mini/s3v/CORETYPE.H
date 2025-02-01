#ifndef CORETYPE_H
#define CORETYPE_H

#ifdef LINEARPM
#define _WIN32
#endif

#ifdef _WIN32
   #define SBYTE     char
   #define SWORD     short
   #define SDWORD    int
#else
   #define SBYTE     char
   #define SWORD     int
   #define SDWORD    long
#endif

#ifdef LINEARPM

#ifndef WORD
   #define WORD   unsigned SWORD
#endif

#ifndef DWORD
   #define DWORD  unsigned SDWORD
#endif

#ifndef BYTE
   #define BYTE   unsigned SBYTE
#endif

#endif /* LINEARPM */

#endif
