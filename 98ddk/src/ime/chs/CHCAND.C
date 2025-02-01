#include <windows.h>
#include "imm.h"
#include "imedefs.h"

//*******************************************************************
// The parameters are inherited from SelectOneCand
//	CrossReverseConv()
//*******************************************************************
void CrossReverseConv(
    LPINPUTCONTEXT      lpIMC,
    LPCOMPOSITIONSTRING lpCompStr,
    LPPRIVCONTEXT       lpImcP,
    LPCANDIDATELIST     lpCandList)
{
LPGUIDELINE lpGuideLine;

    if (!sImeG.hRevKL) {
        return;
    }

	lpGuideLine = ImmLockIMCC(lpIMC->hGuideLine);

    if (!lpGuideLine) {
        return;
    }

    if (lpCompStr->dwResultStrLen != sizeof(WORD)) {
        // we only can reverse convert one DBCS character for now
        lpGuideLine->dwLevel = GL_LEVEL_NOGUIDELINE;
        lpGuideLine->dwIndex = GL_ID_UNKNOWN;
    } else {
        BYTE szStrBuf[4];
        UINT uSize;
 		LPCANDIDATELIST     lpRevCandList;

		if(lpImcP->hRevCandList == (HIMCC)NULL){
			//we alloc memory in lpImcP->hRevCandList, for reverse convert
			//result codes; When finish reconvert, should read out this info.
REALLOC:
		    lpImcP->hRevCandList = (HIMCC)GlobalAlloc(GHND, sizeof(CANDIDATELIST)+1*sizeof(DWORD)+MAXCODE+1);
			if (lpImcP->hRevCandList == (HIMCC)NULL) {
				return ;
			}
   			lpRevCandList = (LPCANDIDATELIST)GlobalLock((HGLOBAL)lpImcP->hRevCandList);
			if (lpRevCandList == NULL) {
				return ;
			}
   		}else{
		   	lpRevCandList = (LPCANDIDATELIST)GlobalLock((HGLOBAL)lpImcP->hRevCandList);
   			if (lpRevCandList == NULL) {
				goto REALLOC;
   			}
		}

        *(LPDWORD)szStrBuf = 0;

        *(LPUNAWORD)szStrBuf = *(LPUNAWORD)((LPBYTE)lpCompStr +
            lpCompStr->dwResultStrOffset);

		memset(lpRevCandList, 0, sizeof(CANDIDATELIST)+1*sizeof(DWORD)+MAXCODE+1);
		lpRevCandList->dwSize = sizeof(CANDIDATELIST)+1*sizeof(DWORD)+MAXCODE+1;

        uSize = ImmGetConversionList(sImeG.hRevKL, (HIMC)NULL, szStrBuf,
            (LPCANDIDATELIST)lpRevCandList, 
            lpRevCandList->dwSize, GCL_REVERSECONVERSION);

	    GlobalUnlock((HGLOBAL)lpImcP->hRevCandList);
		

        if (uSize) {
            if (lpImcP->fdwImeMsg & MSG_ALREADY_START) {
                lpImcP->fdwImeMsg &= ~(MSG_END_COMPOSITION|
                    MSG_START_COMPOSITION);
            } else {
                lpImcP->fdwImeMsg = (lpImcP->fdwImeMsg|
                    MSG_START_COMPOSITION) & ~(MSG_END_COMPOSITION);
            }
        } else {
			GlobalFree((HGLOBAL)lpImcP->hRevCandList);
        }
    }

    ImmUnlockIMCC(lpIMC->hGuideLine);
}

/**********************************************************************/
/* SelectOneCand()                                                    */
/**********************************************************************/
void PASCAL SelectOneCand(
    LPINPUTCONTEXT      lpIMC,
    LPCOMPOSITIONSTRING lpCompStr,
    LPPRIVCONTEXT       lpImcP,
    LPCANDIDATELIST     lpCandList)
{
    DWORD dwCompStrLen;
    DWORD dwReadStrLen;

    if (!lpCompStr) {
        MessageBeep((UINT)-1);
        return;
    }

    if (!lpImcP) {
        MessageBeep((UINT)-1);
        return;
    }

    // backup the dwCompStrLen, this value decide whether
    // we go for phrase prediction
    dwCompStrLen = lpCompStr->dwCompStrLen;
    // backup the value, this value will be destroyed in InitCompStr
    dwReadStrLen = lpCompStr->dwCompReadStrLen;

    InitCompStr(lpCompStr);

    // calculate result string length
    lpCompStr->dwResultStrLen = lstrlen(
        (LPSTR)lpCandList + lpCandList->dwOffset[
        lpCandList->dwSelection]);

    // the result string = the selected candidate;
    lstrcpy((LPSTR)lpCompStr + lpCompStr->dwResultStrOffset,
        (LPSTR)lpCandList + lpCandList->dwOffset[
        lpCandList->dwSelection]);

    // tell application, there is a reslut string
    lpImcP->fdwImeMsg |= MSG_COMPOSITION;
    lpImcP->dwCompChar = (DWORD)NULL;
    lpImcP->fdwGcsFlag |= GCS_COMPREAD|GCS_COMP|GCS_CURSORPOS|
        GCS_DELTASTART|GCS_RESULTREAD|GCS_RESULT;

    if (lpImcP->fdwImeMsg & MSG_ALREADY_OPEN) {
        lpImcP->fdwImeMsg = (lpImcP->fdwImeMsg | MSG_CLOSE_CANDIDATE) &
            ~(MSG_OPEN_CANDIDATE);
    }

    // no candidate now, the right candidate string already be finalized
    lpCandList->dwCount = 0;

    lpImcP->iImeState = CST_INIT;
    
    
	// init Engine private data
    *(LPDWORD)lpImcP->bSeq = 0;

	CrossReverseConv(lpIMC, lpCompStr, lpImcP, lpCandList);

    return;
}

/**********************************************************************/
/* CandEscapeKey()                                                    */
/**********************************************************************/
void PASCAL CandEscapeKey(
    LPINPUTCONTEXT  lpIMC,
    LPPRIVCONTEXT   lpImcP)
{
    LPCOMPOSITIONSTRING lpCompStr;
    LPGUIDELINE         lpGuideLine;

    // clean all candidate information
    if (lpImcP->fdwImeMsg & MSG_ALREADY_OPEN) {
        ClearCand(lpIMC);
        lpImcP->fdwImeMsg = (lpImcP->fdwImeMsg | MSG_CLOSE_CANDIDATE) &
            ~(MSG_OPEN_CANDIDATE);
    }


    // if it start composition, we need to clean composition
    if (!(lpImcP->fdwImeMsg & MSG_ALREADY_START)) {
        return;
    }

    lpCompStr = (LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
    if (!lpCompStr) {          // Oh! Oh!
        return;
    }

    lpGuideLine = (LPGUIDELINE)ImmLockIMCC(lpIMC->hGuideLine);
    if (!lpGuideLine) {          // Oh! Oh!
        return;
    }

    CompEscapeKey(lpIMC, lpCompStr, lpGuideLine, lpImcP);

    ImmUnlockIMCC(lpIMC->hGuideLine);
    ImmUnlockIMCC(lpIMC->hCompStr);

    return;
}

/**********************************************************************/
/* ChooseCand()                                                       */
/**********************************************************************/
void PASCAL ChooseCand(         // choose one of candidate strings by
                                // input char
    WORD            wCharCode,
    LPINPUTCONTEXT  lpIMC,
    LPCANDIDATEINFO lpCandInfo,
    LPPRIVCONTEXT   lpImcP)
{
    LPCANDIDATELIST     lpCandList;
    LPCOMPOSITIONSTRING lpCompStr;

    if (wCharCode == VK_ESCAPE) {           // escape key
        CandEscapeKey(lpIMC, lpImcP);
        return;
    }

    if (!lpCandInfo) {
        MessageBeep((UINT)-1);
        return;
    }

    lpCandList = (LPCANDIDATELIST)
        ((LPBYTE)lpCandInfo + lpCandInfo->dwOffset[0]);

    if (wCharCode == ' ') {      // circle selection
        if ((lpCandList->dwSelection += lpCandList->dwPageSize) >=
            lpCandList->dwCount) {
            // no more candidates, restart it!
            lpCandList->dwSelection = 0;
            MessageBeep((UINT)-1);
        }
        // inform UI, dwSelectedCand is changed
        lpImcP->fdwImeMsg |= MSG_CHANGE_CANDIDATE;
        return;
    }

    if (wCharCode == '=') {      // next selection
		if(sImeL.dwRegImeIndex == INDEX_GB){
	        if (lpCandList->dwSelection >= (IME_MAXCAND/10)*lpCandList->dwPageSize) {
	            MessageBeep((UINT)-1);
		        return;
			}
       	}else if(sImeL.dwRegImeIndex == INDEX_GBK){
	        if (lpCandList->dwSelection >= (IME_XGB_MAXCAND/10 - 1)*lpCandList->dwPageSize) {
	            MessageBeep((UINT)-1);
		        return;
			}
       	}else if(sImeL.dwRegImeIndex == INDEX_UNICODE){
	        if (lpCandList->dwSelection >= (IME_UNICODE_MAXCAND/10)*lpCandList->dwPageSize) {
	            MessageBeep((UINT)-1);
		        return;
			}
		}
        lpCandList->dwSelection += lpCandList->dwPageSize;
        // inform UI, dwSelectedCand is changed
        lpImcP->fdwImeMsg |= MSG_CHANGE_CANDIDATE;
        return;
    }

    if (wCharCode == '-') {      // previous selection
        if (lpCandList->dwSelection < lpCandList->dwPageSize) {
            MessageBeep((UINT)-1);
            return;
        }
        lpCandList->dwSelection -= lpCandList->dwPageSize;
        // inform UI, dwSelectedCand is changed
        lpImcP->fdwImeMsg |= MSG_CHANGE_CANDIDATE;
        return;
    }

    if (wCharCode == 0x23) {      // previous selection
		if(sImeL.dwRegImeIndex == INDEX_GB){
	        if (lpCandList->dwSelection >= (IME_MAXCAND/10)*lpCandList->dwPageSize) {
	            MessageBeep((UINT)-1);
		        return;
			}else{
				lpCandList->dwSelection = (IME_MAXCAND/10)*lpCandList->dwPageSize;
			}
       	}else if(sImeL.dwRegImeIndex == INDEX_GBK){
	        if (lpCandList->dwSelection >= (IME_XGB_MAXCAND/10 - 1)*lpCandList->dwPageSize) {
	            MessageBeep((UINT)-1);
		        return;
			}else{
				lpCandList->dwSelection = (IME_XGB_MAXCAND/10 - 1)*lpCandList->dwPageSize;
			}
       	}else if(sImeL.dwRegImeIndex == INDEX_UNICODE){
	        if (lpCandList->dwSelection >= (IME_UNICODE_MAXCAND/10)*lpCandList->dwPageSize) {
	            MessageBeep((UINT)-1);
		        return;
			}else{
		        lpCandList->dwSelection = (IME_UNICODE_MAXCAND/10)*lpCandList->dwPageSize;
			}
		}
        // inform UI, dwSelectedCand is changed
        lpImcP->fdwImeMsg |= MSG_CHANGE_CANDIDATE;
        return;
    }

    if (wCharCode == 0x24) {
        if (lpCandList->dwSelection < lpCandList->dwPageSize) {
            MessageBeep((UINT)-1);
            return;
        }
        lpCandList->dwSelection = 0;
        // inform UI, dwSelectedCand is changed
        lpImcP->fdwImeMsg |= MSG_CHANGE_CANDIDATE;
        return;
    }

    if (wCharCode == '?') {      // home selection
        if (lpCandList->dwSelection == 0) {
            MessageBeep((UINT)-1);      // already home!
            return;
        }
        lpCandList->dwSelection = 0;
        // inform UI, dwSelectedCand is changed
        lpImcP->fdwImeMsg |= MSG_CHANGE_CANDIDATE;
        return;
    }

    if ((wCharCode >= '0') && wCharCode <= '9') {

	    DWORD dwSelCand;

        dwSelCand = wCharCode - '0' - CAND_START;
        if(wCharCode == '0') {
		    dwSelCand = 9;
		}

        if ((lpCandList->dwSelection + dwSelCand) >=
            lpCandList->dwCount) {
            // out of range
            MessageBeep((UINT)-1);
            return;
        }

        lpCandList->dwSelection = lpCandList->dwSelection + dwSelCand;

        lpCompStr = (LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
    	if (!lpCompStr) {          // Oh! Oh!
        	return;
    	}

        // translate into translate buffer
        SelectOneCand(lpIMC, lpCompStr, lpImcP, lpCandList);

        ImmUnlockIMCC(lpIMC->hCompStr);

        return;
    }


    return;
}
