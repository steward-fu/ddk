/* Copyright (c) 1995, Microsoft Corporation, all rights reserved
**
** rnasmm.h  (to be merged with ras.h)
**
** Remote Access external API
** Public header for external API clients
*/

#ifndef _RNASMM_H_
#define _RNASMM_H_

#ifdef __cplusplus
extern "C" {
#endif


DWORD WINAPI 
RasEnumSmmTypes(
    LPTSTR  szDeviceName,       // Device name on which to enumerate SMM types
    LPTSTR  *lppSmmTypes,       // Pointer to buffer that receives SMM type names
    LPDWORD lpdwcbSmmTypes,     // Pointer to size, in bytes, of buffer
    LPDWORD lpdwcSmmTypes);     // Pointer to number of SMM types returned
    
DWORD WINAPI 
RasSetEntrySmmType(
    LPTSTR szEntry,             // Phonebook entry name
    LPTSTR szSmmType);          // SMM Type to set entry to
   
DWORD WINAPI 
RasGetEntrySmmType(
    LPTSTR szEntry,             // Phoneboodk entry name
    LPTSTR szSmmType,           // Pointer to string that will receive SMM type
    DWORD  cSmmType);           // number of bytes in buffer

#ifdef __cplusplus
}
#endif

#endif 
