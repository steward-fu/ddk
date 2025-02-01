/**********************************************************************/
/*                                                                    */
/*      TOASCII.C - Windows 95 FAKEIME                                */
/*                                                                    */
/*      Copyright (c) 1994-1995  Microsoft Corporation                */
/*                                                                    */
/**********************************************************************/
#include "windows.h"
#include "imm.h"
#include "fakeime.h"

/**********************************************************************/
/*      ImeToAsciiEx                                                  */
/*                                                                    */
/*    HIBYTE of uVirtKey is char code now.                            */
/**********************************************************************/
UINT WINAPI ImeToAsciiEx (UINT uVKey,UINT uScanCode,CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT fuState,HIMC hIMC)
{
    LPARAM lParam;
    LPINPUTCONTEXT lpIMC;
    BOOL fOpen;

    ImeLog(LOGF_KEY | LOGF_API, "ImeToAsciiEx");

    lpdwCurTransKey = lpdwTransKey;
    lParam = ((DWORD)uScanCode << 16) + 1L;
    
    // Init uNumTransKey here.
    uNumTransKey = 0;

    // if hIMC is NULL, this means DISABLE IME.
    if (!hIMC)
        return 0;

    if (!(lpIMC = ImmLockIMC(hIMC)))
        return 0;

    fOpen = lpIMC->fOpen;

    ImmUnlockIMC(hIMC);

    // The current status of IME is "closed".
    if (!fOpen)
        goto itae_exit;

    if (uScanCode & 0x8000)
        IMEKeyupHandler( hIMC, uVKey, lParam, lpbKeyState);
    else
        IMEKeydownHandler( hIMC, uVKey, lParam, lpbKeyState);


    // Clear static value, no more generated message!
    lpdwCurTransKey = NULL;

itae_exit:

    // If trans key buffer that is allocated by USER.EXE full up,
    // the return value is the negative number.
    if (fOverTransKey)
    {
#ifdef DEBUG
OutputDebugString((LPSTR)"***************************************\r\n");
OutputDebugString((LPSTR)"*   TransKey OVER FLOW Messages!!!    *\r\n");
OutputDebugString((LPSTR)"*                by FAKEIME.DLL       *\r\n");
OutputDebugString((LPSTR)"***************************************\r\n");
#endif
        return (int)uNumTransKey;
    }

    return (int)uNumTransKey;
}


/**********************************************************************/
/*      GenerateMessageToTransKey()                                   */
/*                                                                    */
/*      Update the transrate key buffer.                              */
/**********************************************************************/
BOOL PASCAL GenerateMessageToTransKey(LPDWORD lpdwTransKey,LPGENEMSG lpGeneMsg)
{
    LPGENEMSG lpgmT0;

    uNumTransKey++;
    if (uNumTransKey >= (UINT)*lpdwTransKey)
    {
        fOverTransKey = TRUE;
        return FALSE;
    }

    lpgmT0= (LPGENEMSG)(lpdwTransKey+1) + (uNumTransKey - 1);
    *lpgmT0= *lpGeneMsg;

    return TRUE;
}

