//---------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation 1993-1994
//
// File: string.c
//
//  This files contains common string routines
//
// History:
//  1-10-93 ScottH     Created
//
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////  INCLUDES

#include "suiprv.h"         // common headers


#pragma data_seg(DATASEG_PERINSTANCE)

// Static pointer to store the next token
//
LPSTR s_pszNextToken = NULL;

#pragma data_seg()


// Some of these are replacements for the C runtime routines.
//  This is so we don't have to link to the CRT libs.
//

/*----------------------------------------------------------
Purpose: strtok

         Swiped from the C 7.0 runtime sources.

Returns: 
Cond:    
*/
LPSTR PUBLIC SzStrTok(
    LPSTR string,
    LPCSTR control)
    {
    unsigned char map[32];
    int count;
    LPSTR token;
    
    for (count = 0; count < 32; count++)
        map[count] = 0;
    do 
        {
        map[*control >> 3] |= (1 << (*control & 7));
        } while (*control++);

    if (!string)
        {
        ENTEREXCLUSIVE()
            {
            string = s_pszNextToken;
            }
        LEAVEEXCLUSIVE()
        }

    while (map[*string >> 3] & (1 << (*string & 7)) && *string)
        string++;
    token = string;
    for (;; string++)
        {
        if (map[*string >> 3] & (1 << (*string & 7)))
            {
            if (!*string && string == token)
                return(NULL);
            if (*string)
                *string++ = '\0';

            ENTEREXCLUSIVE()
                {
                s_pszNextToken = string;
                }
            LEAVEEXCLUSIVE()
            return(token);
            }
        }
    }


/*----------------------------------------------------------
Purpose: strchr

         Swiped from the C 7.0 runtime sources.

Returns: 
Cond:    
*/
LPCSTR PUBLIC SzStrCh(
    LPCSTR string,
    char ch)
    {
    while (*string && *string != ch)
        string++;
    if (*string == ch)
        return(string);

    return((char *)0);
    }


/*----------------------------------------------------------
Purpose: strnicmp

         Swiped from the C 7.0 runtime sources.

Returns: 
Cond:    
*/
int PUBLIC lstrnicmp(
    LPCSTR first,
    LPCSTR last,
    UINT count)
    {
    int f,l;
    int result = 0;
    
    if (count) 
        {
        do      
            {
            f = (int)(DWORD)AnsiLower((LPSTR)MAKELONG(*first, 0));
            l = (int)(DWORD)AnsiLower((LPSTR)MAKELONG(*last, 0));
            first = AnsiNext(first);
            last = AnsiNext(last);
            } while (--count && f && l && f == l);
        result = f - l;
        }
    return(result);
    }


/*----------------------------------------------------------
Purpose: Get a string from the resource string table.  
Returns: Ptr to string
Cond:    --
*/
LPSTR PUBLIC SzFromIDS(
    UINT ids,               // resource ID
    LPSTR pszBuf,           // buffer
    int cbBuf)              // size of buffer
    {
    ASSERT(pszBuf);
    ASSERT(cbBuf > 0);

    *pszBuf = NULL_CHAR;
    LoadString(g_hinst, ids, pszBuf, cbBuf);
    return pszBuf;
    }



