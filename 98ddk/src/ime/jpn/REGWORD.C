/**********************************************************************/
/*                                                                    */
/*      REGWORD.C - Windows 95 FAKEIME                                */
/*                                                                    */
/*      Copyright (c) 1994-1995  Microsoft Corporation                */
/*                                                                    */
/**********************************************************************/
#include "windows.h"
#include "imm.h"
#include "fakeime.h"


#define FAKEWORD_NOUN  IME_REGWORD_STYLE_USER_FIRST
#define FAKEWORD_VERB  (IME_REGWORD_STYLE_USER_FIRST+1)

BOOL    WINAPI ImeRegisterWord(LPCSTR lpRead, DWORD dw, LPCSTR lpStr)
{
    if ((dw == FAKEWORD_NOUN) || (dw== FAKEWORD_VERB))
        return WritePrivateProfileString(lpRead,lpStr,lpStr,szDicFileName);

    return FALSE;
}
BOOL    WINAPI ImeUnregisterWord(LPCSTR lpRead, DWORD dw, LPCSTR lpStr)
{
    if ((dw == FAKEWORD_NOUN) || (dw== FAKEWORD_VERB))
        return WritePrivateProfileString(lpRead,lpStr,NULL,szDicFileName);

    return FALSE;
}
UINT    WINAPI ImeGetRegisterWordStyle(UINT u, LPSTYLEBUF lp)
{
    UINT uRet = 0;

    if (u > 0 && lp)
    {
        lp->dwStyle = FAKEWORD_NOUN; 
        lstrcpy(lp->szDescription,"NOUN");
   
        if (u > 1)
        {
            lp++;
            lp->dwStyle = FAKEWORD_VERB; 
            lstrcpy(lp->szDescription,"VERB");
        }
    }
    else
        uRet = 2;

    return uRet;
}
UINT    WINAPI ImeEnumRegisterWord(REGISTERWORDENUMPROC lpfn, LPCSTR lpRead, DWORD dw, LPCSTR lpStr, LPVOID lpData)
{
    UINT uRet = 0;
    char szBuf[256];
    int nBufLen;
    LPSTR lpBuf;

    if (*lpfn)
        return 0;

    lpBuf = (LPSTR)szBuf;

    if (!dw || (dw == FAKEWORD_NOUN))
    {
        if (lpRead)
        {
            nBufLen = GetPrivateProfileString(lpRead, NULL,(LPSTR)"",
                            (LPSTR)szBuf,sizeof(szBuf),(LPSTR)szDicFileName );

            if (nBufLen)
            {
                while (*lpBuf)
                {
                    if (lpStr && lstrcmp(lpStr, lpBuf))
                        continue;

                    uRet = (*lpfn)(lpRead, dw, lpBuf, lpData);
                    lpBuf += (lstrlen(lpBuf) + 1);

                    if (!uRet)
                        break;
                }
            }
        }
        else
        {
        }
    }

    return uRet;
}
