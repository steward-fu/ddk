/**********************************************************************/
/*  Copyright (C) 1995-1996 Microsoft Corporation.                    */
/*  Author: ChaoYang Wang & J J Guan                                  */
/**********************************************************************/
#include <windows.h>
#include "imm.h"
#include "imedefs.h"

/**********************************************************************/
/* IsUsedCode()                                                       */
/* Return Value:                                                      */
/*      TURE: is UsedCode;  FALSE: is'nt UsedCode;                    */
/**********************************************************************/
BOOL IsUsedCode(
    WORD wCharCode)
{
  	  WORD wFlg;

	  for(wFlg=0; wFlg<sImeG.wNumCodes; wFlg++)
		if (wCharCode == sImeG.UsedCodes[wFlg])
		    break;
	  if(wFlg < sImeG.wNumCodes)
	  	return (TRUE);
      return (FALSE);
}

/**********************************************************************/
/* GBProcessKey()                                                       */
/* Return Value:                                                      */
/*      different state which input key will change IME to (CST_      */
/**********************************************************************/
UINT PASCAL GBProcessKey(     // this key will cause the IME go to what state
    WORD           wCharCode,
    LPPRIVCONTEXT  lpImcP)
{
    if (!lpImcP) {
        return (CST_INVALID);
    }

    if (wCharCode == ' ') {
     	if (lpImcP->bSeq[0] && lpImcP->bSeq[1]) {
	 		return (CST_INPUT);
		} else if (!lpImcP->bSeq[0]) {
            return (CST_ALPHANUMERIC);
		} else {
            return (CST_INVALID_INPUT);
		}
	 }
    // check finalize char
      if ((wCharCode >= '0' && wCharCode <= '9')
       ||(wCharCode >= 'a' && wCharCode <= 'f')
       ||(wCharCode == '?')) {

      if (!lpImcP->bSeq[0]) {
        if (wCharCode == '?'){
            // 0x0??? - 0xF??? is OK
            return (CST_ALPHANUMERIC);
        } else {
            // there is no 0x0??? - 0x7???
            return (CST_INPUT);
        }

      } else if (!lpImcP->bSeq[1]) {

         if (lpImcP->bSeq[0] >='0' && lpImcP->bSeq[0] <= '9'){ //Area
           if ((lpImcP->bSeq[0] == '0' && wCharCode == '0')
               ||(lpImcP->bSeq[0] == '9' && wCharCode >= '5')
               ||(wCharCode >= 'a' && wCharCode <= 'f')
               ||(wCharCode == '?')) {
               // there is less than 95 area and bigger than 0 area
                  return (CST_INVALID_INPUT);
           }
           else {
                  return (CST_INPUT);
           }
         }

         if (lpImcP->bSeq[0] >= 'a' && lpImcP->bSeq[0] <= 'f') { //GB
           if ((lpImcP->bSeq[0] == 'a' && wCharCode == '0')
               ||(lpImcP->bSeq[0] == 'f' && wCharCode == 'f')
               ||(wCharCode == '?')) {
                  // there is less than 95 area and bigger than 0 area
                  return (CST_INVALID_INPUT);
           }
           else {
                  return (CST_INPUT);
           }
         }

      } else if (!lpImcP->bSeq[2]) {

         if (wCharCode == '?'){
                  return (CST_INPUT);
         }
         if (lpImcP->bSeq[0] >= '0' && lpImcP->bSeq[0] <= '9'){ //Area
           if (wCharCode >= '0' && wCharCode <= '9') {
              return (CST_INPUT);
           } else {
              return (CST_INVALID_INPUT);
           }

         }
     
         if (lpImcP->bSeq[0] >= 'a' && lpImcP->bSeq[0] <= 'f') { //GB
           if (wCharCode >= 'a' && wCharCode <= 'f') {
              return (CST_INPUT);
           } else {
              return (CST_INVALID_INPUT);
           }
         }
      } else if (!lpImcP->bSeq[3]) {

         if (lpImcP->bSeq[2] == '?') {
           if (wCharCode == '?') {
              return (CST_INPUT);
           }else{
              return (CST_INVALID_INPUT);
           }
         }
         if (lpImcP->bSeq[0] >= '0' && lpImcP->bSeq[0] <= '9') { //Area
           if ((lpImcP->bSeq[2] == '0' && wCharCode == '0')
               ||(lpImcP->bSeq[2] == '9' && wCharCode >= '5')
               ||(wCharCode >= 'a' && wCharCode <= 'f')
               ||(wCharCode == '?')) {
                  // there is less than 95 area and bigger than 0 area
                  return (CST_INVALID_INPUT);
           }
           else {
                  return (CST_INPUT);
           }
         }
         if (lpImcP->bSeq[0] >= 'a' && lpImcP->bSeq[0] <= 'f') { //GB
           if ((lpImcP->bSeq[2] == 'a' && wCharCode == '0')
               ||(lpImcP->bSeq[2] == 'f' && wCharCode == 'f')
               ||(wCharCode == '?')){
                  // there is less than 95 area and bigger than 0 area
                  return (CST_INVALID_INPUT);
           }
           else {
                  return (CST_INPUT);
           }
         }
      } else {
	      return (CST_INVALID_INPUT);
	  }

    } else if (wCharCode >= ' ' && wCharCode <= '~') {
        return (CST_ALPHANUMERIC);
	} else {
	}

}

/**********************************************************************/
/* UnicodeProcessKey()                                                */
/* Return Value:                                                      */
/*      different state which input key will change IME to (CST_      */
/**********************************************************************/
UINT PASCAL UnicodeProcessKey(     // this key will cause the IME go to what state
    WORD           wCharCode,
    LPPRIVCONTEXT  lpImcP)
{
    if (!lpImcP) {
        return (CST_INVALID);
    }

    if (wCharCode == ' ') {
     	if (lpImcP->bSeq[0] && lpImcP->bSeq[1]) {
	 		return (CST_INPUT);
		} else if (!lpImcP->bSeq[0]) {
            return (CST_ALPHANUMERIC);
		} else {
            return (CST_INVALID_INPUT);
		}
	}

     // check finalize char
	 // 0000 -- ffff

    if ((wCharCode >= '0' && wCharCode <= '9')
       ||(wCharCode >= 'a' && wCharCode <= 'f')
       ||(wCharCode == '?')) {

         if (wCharCode == '?'){
			if (!lpImcP->bSeq[2]) {
                  return (CST_INPUT);
	         }else
				 return(CST_INVALID_INPUT);
		 }else{
                  return (CST_INPUT);
         }
	} else if(lpImcP->bSeq[0]){
	      return (CST_INVALID_INPUT);
	} else 
		return (CST_ALPHANUMERIC);

}

/**********************************************************************/
/* XGBProcessKey()                                                       */
/* Return Value:                                                      */
/*      different state which input key will change IME to (CST_      */
/**********************************************************************/
UINT PASCAL XGBProcessKey(     // this key will cause the IME go to what state
    WORD           wCharCode,
    LPPRIVCONTEXT  lpImcP)
{
    if (!lpImcP) {
        return (CST_INVALID);
    }

     if (wCharCode == ' ') {
     	if (lpImcP->bSeq[0] && lpImcP->bSeq[1]) {
	 		return (CST_INPUT);
		} else if (!lpImcP->bSeq[0]) {
            return (CST_ALPHANUMERIC);
		} else {
            return (CST_INVALID_INPUT);
		}
	 }

     // check finalize char
     //lead  byte 81 - fe
     //trail byte 40 - 7e, 80 - fe

     if ((wCharCode >= '0' && wCharCode <= '9')
       ||(wCharCode >= 'a' && wCharCode <= 'f')
       ||(wCharCode == '?')) {

      if (!lpImcP->bSeq[0]) {
        if (wCharCode == '?') {
            // 0x0??? - 0xF??? is OK
            // : - @ was filted
            return (CST_ALPHANUMERIC);

        }else if (wCharCode >='8' && wCharCode <= 'f' ){
            // 0x0??? - 0xF??? is OK
            return (CST_INPUT);
            
        } else {
            // there is no 0x0??? - 0x7???
            return (CST_INVALID_INPUT);
        }

      } else if (!lpImcP->bSeq[1]) {

           if ((lpImcP->bSeq[0] == 'f' && wCharCode == 'f')
             ||(lpImcP->bSeq[0] == '8' && wCharCode == '0')
             ||(wCharCode == '?')) {
               //XGB is 81 - fe
                  return (CST_INVALID_INPUT);
           }
           else {
                  return (CST_INPUT);
           }

      } else if (!lpImcP->bSeq[2]) {

         if (wCharCode == '?'){
                  return (CST_INPUT);
         }

           if (wCharCode >= '4' && wCharCode <= 'f') {
              return (CST_INPUT);
           } else {
              return (CST_INVALID_INPUT);
           }

      } else if (!lpImcP->bSeq[3]) {

         if (lpImcP->bSeq[2] == '?') {
           if (wCharCode == '?') {
              return (CST_INPUT);
           }else{
              return (CST_INVALID_INPUT);
           }
         }
           if ((lpImcP->bSeq[2] == '7' && wCharCode == 'f')
             ||(lpImcP->bSeq[2] == 'f' && wCharCode == 'f')
             ||(wCharCode == '?')) {
                  //trail byte
                  //40 - 7e, 80 - fe

                  return (CST_INVALID_INPUT);
           }
           else {
                  return (CST_INPUT);
           }
      } else {
	      return (CST_INVALID_INPUT);
	  }

    } else if (wCharCode >= ' ' && wCharCode <= '~') {
        return (CST_ALPHANUMERIC);
	} else {
	}

}

/**********************************************************************/
/* ProcessKey()                                                       */
/* Return Value:                                                      */
/*      different state which input key will change IME to (CST_      */
/**********************************************************************/
UINT PASCAL ProcessKey(     // this key will cause the IME go to what state
    WORD           wCharCode,
    UINT           uVirtKey,
    UINT           uScanCode,
    LPBYTE         lpbKeyState,
    LPINPUTCONTEXT lpIMC,
    LPPRIVCONTEXT  lpImcP)
{

    if (!lpIMC) {
        return (CST_INVALID);
    }

    if (!lpImcP) {
        return (CST_INVALID);
    }


    // filter system key (alt,alt+,ctrl,shift)
    // and fOpen, IME_CMODE_NOCONVERSION
    if (uVirtKey == VK_MENU) {       		// ALT key
        return (CST_INVALID);
    } else if (uScanCode & KF_ALTDOWN) {    // ALT-xx key
        return (CST_INVALID);
    } else if (uVirtKey == VK_CONTROL) {    // CTRL key
        return (CST_INVALID);
    } else if (uVirtKey == VK_SHIFT) {      // SHIFT key
        return (CST_INVALID);
    } else if (!lpIMC->fOpen) {             // don't compose in 
                                            // close status
        return (CST_INVALID);
    } else if (lpIMC->fdwConversion & IME_CMODE_NOCONVERSION) {
		// Caps on/off
		if(uVirtKey == VK_CAPITAL) {
			return (CST_CAPITAL);
		}else		
			return (CST_INVALID);
        
    } else if (uVirtKey >= VK_NUMPAD0 && uVirtKey <= VK_DIVIDE) {
        return (CST_INVALID);
    } else {
    }

    // Caps on/off
    if(uVirtKey == VK_CAPITAL) {
        return (CST_CAPITAL);
	}

    // SoftKBD   
    if ((lpIMC->fdwConversion & IME_CMODE_SOFTKBD)
       && (lpImeL->dwSKWant != 0)){
        if (wCharCode >= ' ' && wCharCode <= '~') {
          return (CST_SOFTKB);
		} else {
          return (CST_INVALID);
		}
    }
	
    // candidate alaredy open,  <,>,pageup,pagedown,?,ECS,key
    if (lpImcP->fdwImeMsg & MSG_ALREADY_OPEN) {
        if (uVirtKey == VK_PRIOR) {			// PageUp
            return (CST_CHOOSE);
        } else if (uVirtKey == VK_NEXT) {	// PageDown
            return (CST_CHOOSE);
        } else if (wCharCode == '-') {
            return (CST_CHOOSE);
        } else if (wCharCode == '=') {
            return (CST_CHOOSE);
        } else if (uVirtKey == VK_HOME) {
            return (CST_CHOOSE);
        } else if (uVirtKey == VK_END) {
            return (CST_CHOOSE);
        } else if (uVirtKey == VK_ESCAPE) {	// Esc
            return (CST_CHOOSE);
        } else if (wCharCode == ' ') {
            return (CST_CHOOSE);
        } else {
        }
    }


    // candidate alaredy open, shift + num key
    if (lpImcP->fdwImeMsg & MSG_ALREADY_OPEN) {
            if ((wCharCode >= '0') && wCharCode <= '9') {
                return (CST_CHOOSE);
            }
    }

    // IME_CMODE_CHARCODE
    if (lpIMC->fdwConversion & IME_CMODE_CHARCODE) {	//Code Input Mode
            return (CST_INVALID);
    }

    if (!(lpIMC->fdwConversion & IME_CMODE_NATIVE)) {
        // alphanumeric mode
        if (wCharCode >= ' ' && wCharCode <= '~') {
            return (CST_ALPHANUMERIC);
        } else {
            return (CST_INVALID);
        }
    } else if (wCharCode == '?') {
    } else if (wCharCode == ' ') {
	} else if(wCharCode >= ' ' && wCharCode <= '~') {
		if(!IsUsedCode(wCharCode)
		&& lpImcP->iImeState != CST_INIT)
            return (CST_INVALID_INPUT);
    } else {
    }

    // Esc key
    if (uVirtKey == VK_ESCAPE) {
        register LPGUIDELINE lpGuideLine;
        register UINT        iImeState;

        lpGuideLine = ImmLockIMCC(lpIMC->hGuideLine);
		if(!lpGuideLine){
            return (CST_INVALID);
		}
        if (lpImcP->fdwImeMsg & MSG_ALREADY_START) {
            iImeState = CST_INPUT;
        } else if (!lpGuideLine) {
            iImeState = CST_INVALID;
        } else if (lpGuideLine->dwLevel == GL_LEVEL_NOGUIDELINE) {
            iImeState = CST_INVALID;
        } else {
            // need this key to clean information string or guideline state
            iImeState = CST_INPUT;
        }

        ImmUnlockIMCC(lpIMC->hGuideLine);

        return (iImeState);
    } 
    
    // BackSpace Key
    else if (uVirtKey == VK_BACK) {
        if (lpImcP->fdwImeMsg & MSG_ALREADY_START) {
            return (CST_INPUT);
        } else {
            return (CST_INVALID);
        }
    }
    
    // NumPad key and Other Input vailid key
    else if (uVirtKey >= VK_NUMPAD0 && uVirtKey <= VK_DIVIDE) {
        return (CST_ALPHANUMERIC);
    } else if (wCharCode > '~') {
        return (CST_INVALID);
    } else if (wCharCode < ' ') {
        return (CST_INVALID);
    } else if (lpIMC->fdwConversion & IME_CMODE_EUDC) {
    } 
    else {
    }
    if (lpIMC->fdwConversion & IME_CMODE_NATIVE) {      

		switch(sImeL.dwRegImeIndex){
		case INDEX_GB:
			return (GBProcessKey(wCharCode,lpImcP));
		case INDEX_GBK:
			return (XGBProcessKey (wCharCode,lpImcP));
		case INDEX_UNICODE:
			return(UnicodeProcessKey(wCharCode, lpImcP));
		}
     }

  return (CST_INVALID);
}

/**********************************************************************/
/* ImeProcessKey()                                                    */
/* Return Value:                                                      */
/*      TRUE - successful, FALSE - failure                            */
/**********************************************************************/
BOOL WINAPI ImeProcessKey(   // if this key is need by IME?
    HIMC   hIMC,
    UINT   uVirtKey,
    LPARAM lParam,
    CONST LPBYTE lpbKeyState)
{
    LPINPUTCONTEXT lpIMC;
    LPPRIVCONTEXT  lpImcP;
    BYTE           szAscii[4];
    int            nChars;
    int            iRet;
    BOOL           fRet;

    // can't compose in NULL hIMC
    if (!hIMC) {
        return (FALSE);
    }

    lpIMC = (LPINPUTCONTEXT)ImmLockIMC(hIMC);
    if (!lpIMC) {
        return (FALSE);
    }

    lpImcP = (LPPRIVCONTEXT)ImmLockIMCC(lpIMC->hPrivate);
    if (!lpImcP) {
        ImmUnlockIMC(hIMC);
        return (FALSE);
    }

    nChars = ToAscii(uVirtKey, HIWORD(lParam), lpbKeyState,
                (LPVOID)szAscii, 0);

    if (!nChars) {
        szAscii[0] = 0;
    }

    iRet = ProcessKey((WORD)szAscii[0], uVirtKey, HIWORD(lParam), lpbKeyState, lpIMC, lpImcP);
    if(iRet == CST_INVALID) {
        fRet = FALSE;
    } else if((iRet == CST_INPUT) && (uVirtKey == '\b')
             && (lpImcP->iImeState == CST_INIT)) {
        lpImcP->fdwImeMsg = ((lpImcP->fdwImeMsg | MSG_END_COMPOSITION)
                            & ~(MSG_START_COMPOSITION)) & ~(MSG_IN_IMETOASCIIEX);

      	if (lpImcP->fdwImeMsg & MSG_ALREADY_OPEN) {
            ClearCand(lpIMC);
            lpImcP->fdwImeMsg = (lpImcP->fdwImeMsg | MSG_CLOSE_CANDIDATE) &
                ~(MSG_OPEN_CANDIDATE);
        }

	    GenerateMessage(hIMC, lpIMC, lpImcP);
        fRet = FALSE;
    } else if(uVirtKey == VK_CAPITAL) {
        DWORD fdwConversion;
		if (!(lpbKeyState[VK_CAPITAL] & 0x01)) {
            fdwConversion = lpIMC->fdwConversion & ~(IME_CMODE_CHARCODE |
                IME_CMODE_NATIVE | IME_CMODE_EUDC);
			uCaps = 1;
		} else {
            // change to native mode
            fdwConversion = (lpIMC->fdwConversion | IME_CMODE_NATIVE)&
                ~(IME_CMODE_CHARCODE | IME_CMODE_EUDC); 
			uCaps = 0;
	}
        ImmSetConversionStatus(hIMC, fdwConversion, lpIMC->fdwSentence);
        fRet = FALSE;
    } else if((iRet == CST_ALPHANUMERIC)
              && !(lpIMC->fdwConversion & IME_CMODE_FULLSHAPE)
			  && (uVirtKey == VK_SPACE)) {
        fRet = FALSE;
    } else {
        fRet = TRUE;
    }

    ImmUnlockIMCC(lpIMC->hPrivate);
    ImmUnlockIMC(hIMC);

    return (fRet);
}

/**********************************************************************/
/* TranslateSymbolChar()                                              */
/* Return Value:                                                      */
/*      the number of translated chars                                */
/**********************************************************************/
UINT PASCAL TranslateSymbolChar(
    LPDWORD lpdwTransBuf,
    WORD    wSymbolCharCode,
    BOOL    SymbolMode)
{
    UINT uRet;
    // 3 DWORD (message, wParam, lParam)
    uRet = 0;
    lpdwTransBuf = lpdwTransBuf + 1;

    *lpdwTransBuf++ = WM_CHAR;
    *lpdwTransBuf++ = (DWORD)HIBYTE(wSymbolCharCode);
    *lpdwTransBuf++ = 1UL;
	uRet++;

    *lpdwTransBuf++ = WM_CHAR;
    *lpdwTransBuf++ = (DWORD)LOBYTE(wSymbolCharCode);
    *lpdwTransBuf = 1UL;
	uRet++;

    if(SymbolMode) {
	    lpdwTransBuf = lpdwTransBuf + 1;

	    *lpdwTransBuf++ = WM_CHAR;
	    *lpdwTransBuf++ = (DWORD)HIBYTE(wSymbolCharCode);
	    *lpdwTransBuf++ = 1UL;
		uRet++;

	    *lpdwTransBuf++ = WM_CHAR;
	    *lpdwTransBuf++ = (DWORD)LOBYTE(wSymbolCharCode);
	    *lpdwTransBuf = 1UL;
		uRet++;
	}

    return (uRet);         // generate two messages
}

/**********************************************************************/
/* TranslateFullChar()                                                */
/* Return Value:                                                      */
/*      the number of translated chars                                */
/**********************************************************************/
UINT PASCAL TranslateFullChar(
    LPDWORD lpdwTransBuf,
    WORD    wCharCode)
{
    // if your IME is possible to generate over ? messages,
    // you need to take care about it

     wCharCode = sImeG.wFullABC[wCharCode - ' '];

    // 3 DWORD (message, wParam, lParam)
    lpdwTransBuf = lpdwTransBuf + 1;

    *lpdwTransBuf++ = WM_CHAR;
    *lpdwTransBuf++ = (DWORD)HIBYTE(wCharCode);
    *lpdwTransBuf++ = 1UL;

    *lpdwTransBuf++ = WM_CHAR;
    *lpdwTransBuf++ = (DWORD)LOBYTE(wCharCode);
    *lpdwTransBuf = 1UL;
    return (2);         // generate two messages
}
 
/**********************************************************************/
/* TranslateToAscii()                                                 */
/* Return Value:                                                      */
/*      the number of translated chars                                */
/**********************************************************************/
UINT PASCAL TranslateToAscii(       // translate the key to WM_CHAR
                                    // as keyboard driver
    UINT    uVirtKey,
    UINT    uScanCode,
    LPDWORD lpdwTransBuf,
    WORD    wCharCode)
{
    // 3 DWORD (message, wParam, lParam)
    lpdwTransBuf = lpdwTransBuf + 1;

    if (wCharCode) {                    // one char code
        *lpdwTransBuf++ = WM_CHAR;
        *lpdwTransBuf++ = wCharCode;
        *lpdwTransBuf = (uScanCode << 16) | 1UL;
        return (1);
    }

    // no char code case
    return (0);
}

/**********************************************************************/
/* TranslateImeMessage()                                              */
/* Return Value:                                                      */
/*      the number of translated messages                             */
/**********************************************************************/
UINT PASCAL TranslateImeMessage(
    LPDWORD        lpdwTransBuf,
    LPINPUTCONTEXT lpIMC,
    LPPRIVCONTEXT  lpImcP)
{
    UINT uNumMsg;
    UINT i;
    BOOL bLockMsgBuf;

    uNumMsg = 0;
    bLockMsgBuf = FALSE;

    for (i = 0; i < 2; i++) {
        if (lpImcP->fdwImeMsg & MSG_CLOSE_CANDIDATE) {
            if (lpImcP->fdwImeMsg & MSG_ALREADY_OPEN) {
                if (!i) {
                    uNumMsg++;
                } else {
                    *lpdwTransBuf++ = WM_IME_NOTIFY;
                    *lpdwTransBuf++ = IMN_CLOSECANDIDATE;
                    *lpdwTransBuf++ = 0x0001;
                    lpImcP->fdwImeMsg &= ~(MSG_ALREADY_OPEN);
                }
            }
        }

        if (lpImcP->fdwImeMsg & MSG_END_COMPOSITION) {
            if (lpImcP->fdwImeMsg & MSG_ALREADY_START) {
                if (!i) {
                    uNumMsg++;
                } else {
                    *lpdwTransBuf++ = WM_IME_ENDCOMPOSITION;
                    *lpdwTransBuf++ = 0;
                    *lpdwTransBuf++ = 0;
                    lpImcP->fdwImeMsg &= ~(MSG_ALREADY_START);
                }
            }
        }

        if (lpImcP->fdwImeMsg & MSG_START_COMPOSITION) {
            if (!(lpImcP->fdwImeMsg & MSG_ALREADY_START)) {
                if (!i) {
                    uNumMsg++;
                } else {
                    *lpdwTransBuf++ = WM_IME_STARTCOMPOSITION;
                    *lpdwTransBuf++ = 0;
                    *lpdwTransBuf++ = 0;
                    lpImcP->fdwImeMsg |= MSG_ALREADY_START;
                }
            }
        }

        if (lpImcP->fdwImeMsg & MSG_IMN_COMPOSITIONPOS) {
            if (!i) {
                uNumMsg++;
            } else {
                *lpdwTransBuf++ = WM_IME_NOTIFY;
                *lpdwTransBuf++ = IMN_SETCOMPOSITIONWINDOW;
                *lpdwTransBuf++ = 0;
            }
        }

        if (lpImcP->fdwImeMsg & MSG_COMPOSITION) {
            if (!i) {
                uNumMsg++;
            } else {
                *lpdwTransBuf++ = WM_IME_COMPOSITION;
                *lpdwTransBuf++ = (DWORD)lpImcP->dwCompChar;
                *lpdwTransBuf++ = (DWORD)lpImcP->fdwGcsFlag;
            }
        }

        if (lpImcP->fdwImeMsg & MSG_GUIDELINE) {
            if (!i) {
                uNumMsg++;
            } else {
                *lpdwTransBuf++ = WM_IME_NOTIFY;
                *lpdwTransBuf++ = IMN_GUIDELINE;
                *lpdwTransBuf++ = 0;
            }
        }

        if (lpImcP->fdwImeMsg & MSG_OPEN_CANDIDATE) {
            if (!(lpImcP->fdwImeMsg & MSG_ALREADY_OPEN)) {
                if (!i) {
                    uNumMsg++;
                } else {
                    *lpdwTransBuf++ = WM_IME_NOTIFY;
                    *lpdwTransBuf++ = IMN_OPENCANDIDATE;
                    *lpdwTransBuf++ = 0x0001;
                    lpImcP->fdwImeMsg |= MSG_ALREADY_OPEN;
                }
            }
        }

        if (lpImcP->fdwImeMsg & MSG_CHANGE_CANDIDATE) {
            if (!i) {
                uNumMsg++;
            } else {
                *lpdwTransBuf++ = WM_IME_NOTIFY;
                *lpdwTransBuf++ = IMN_CHANGECANDIDATE;
                *lpdwTransBuf++ = 0x0001;
            }
        }

        if (lpImcP->fdwImeMsg & MSG_IMN_UPDATE_SOFTKBD) {
            if (!i) {
                uNumMsg++;
            } else {
                *lpdwTransBuf++ = WM_IME_NOTIFY;
                *lpdwTransBuf++ = IMN_PRIVATE;
                *lpdwTransBuf++ = IMN_PRIVATE_UPDATE_SOFTKBD;
            }
        }

        if (lpImcP->fdwImeMsg & MSG_IMN_UPDATE_STATUS) {
            if (!i) {
                uNumMsg++;
            } else {
                *lpdwTransBuf++ = WM_IME_NOTIFY;
                *lpdwTransBuf++ = IMN_PRIVATE;
                *lpdwTransBuf++ = IMN_PRIVATE_UPDATE_STATUS;
            }
        }

        if (lpImcP->fdwImeMsg & MSG_IMN_DESTROYCAND) {
            if (!i) {
                uNumMsg++;
            } else {
                *lpdwTransBuf++ = WM_IME_NOTIFY;
                *lpdwTransBuf++ = IMN_PRIVATE;
                *lpdwTransBuf++ = IMN_PRIVATE_DESTROYCANDWIN;
            }
        }

        if (lpImcP->fdwImeMsg & MSG_BACKSPACE) {
            if (!i) {
                uNumMsg++;
            } else {
		        *lpdwTransBuf++ = WM_CHAR;
		        *lpdwTransBuf++ = '\b';
		        *lpdwTransBuf = 0x000e;
            }
        }

        if (!i) {
            HIMCC hMem;

            if (!uNumMsg) {
                return (uNumMsg);
            }

            if (lpImcP->fdwImeMsg & MSG_IN_IMETOASCIIEX) {
                UINT uNumMsgLimit;

                uNumMsgLimit = *lpdwTransBuf;

                if (uNumMsg <= uNumMsgLimit) {
        			lpdwTransBuf++;
                    continue;
                }
            }

            // we need to use message buffer
            if (!lpIMC->hMsgBuf) {
                lpIMC->hMsgBuf = ImmCreateIMCC(uNumMsg * sizeof(DWORD) * 3);
                lpIMC->dwNumMsgBuf = 0;
            } else if (hMem = ImmReSizeIMCC(lpIMC->hMsgBuf,
                (lpIMC->dwNumMsgBuf + uNumMsg) * sizeof(DWORD) * 3)) {
                if (hMem != lpIMC->hMsgBuf) {
                    ImmDestroyIMCC(lpIMC->hMsgBuf);
                    lpIMC->hMsgBuf = hMem;
                }
            } else {
                return (0);
            }

            lpdwTransBuf = (LPDWORD)ImmLockIMCC(lpIMC->hMsgBuf);
            if (!lpdwTransBuf) {
                return (0);
            }

            lpdwTransBuf += lpIMC->dwNumMsgBuf * 3;

            bLockMsgBuf = TRUE;
        } else {
            if (bLockMsgBuf) {
                ImmUnlockIMCC(lpIMC->hMsgBuf);
            }
        }
    }

    return (uNumMsg);
}

/**********************************************************************/
/* ImeToAsciiEx()                                                     */
/* Return Value:                                                      */
/*      the number of translated message                              */
/**********************************************************************/
UINT WINAPI ImeToAsciiEx(
    UINT    uVirtKey,
    UINT    uScanCode,
    CONST LPBYTE  lpbKeyState,
    LPDWORD lpdwTransBuf,
    UINT    fuState,
    HIMC    hIMC)
{
    WORD                wCharCode;
    LPINPUTCONTEXT      lpIMC;
    LPCOMPOSITIONSTRING lpCompStr;
    LPPRIVCONTEXT       lpImcP;
    UINT                uNumMsg;
    int                 iRet;

    wCharCode = HIBYTE(uVirtKey);
    uVirtKey = LOBYTE(uVirtKey);

    // hIMC=NULL?
    if (!hIMC) {
        uNumMsg = TranslateToAscii(uVirtKey, uScanCode, lpdwTransBuf,
            wCharCode);
        return (uNumMsg);
    }

    // get lpIMC
    lpIMC = (LPINPUTCONTEXT)ImmLockIMC(hIMC);
    
    if (!lpIMC) {
        uNumMsg = TranslateToAscii(uVirtKey, uScanCode, lpdwTransBuf,
            wCharCode);
        return (uNumMsg);
    }

    // get lpImcP
    lpImcP = (LPPRIVCONTEXT)ImmLockIMCC(lpIMC->hPrivate);
    
    if (!lpImcP) {
        ImmUnlockIMC(hIMC);
        uNumMsg = TranslateToAscii(uVirtKey, uScanCode, lpdwTransBuf,
            wCharCode);
        return (uNumMsg);
    }

    // get lpCompStr and init
    if (lpImcP->fdwGcsFlag & (GCS_RESULTREAD|GCS_RESULT)) {
        lpCompStr = (LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);

        if (lpCompStr) {
            lpCompStr->dwResultStrLen = 0;
        }

        ImmUnlockIMCC(lpIMC->hCompStr);

        lpImcP->fdwGcsFlag = (DWORD)NULL;
    }

    // Now all composition realated information already pass to app
    // a brand new start

    // init lpImcP->fdwImeMsg
    lpImcP->fdwImeMsg = lpImcP->fdwImeMsg & (MSG_ALREADY_OPEN|
        MSG_ALREADY_START) | MSG_IN_IMETOASCIIEX;
    
    iRet = ProcessKey(wCharCode, uVirtKey, uScanCode, lpbKeyState, lpIMC,
        lpImcP);

    // CST_SOFTKB
    if (iRet == CST_SOFTKB) {
	    WORD wSymbolCharCode;
		WORD CHIByte, CLOByte;
		int  SKDataIndex;

		// Mapping VK
		if(uVirtKey == 0x20) {
			SKDataIndex = 0;
		} else if(uVirtKey >= 0x30 && uVirtKey <= 0x39) {
			SKDataIndex = uVirtKey - 0x30 + 1;
		} else if (uVirtKey >= 0x41 && uVirtKey <= 0x5a) {
			SKDataIndex = uVirtKey - 0x41 + 0x0b;
		} else if (uVirtKey >= 0xba && uVirtKey <= 0xbf) {
			SKDataIndex = uVirtKey - 0xba + 0x25;
		} else if (uVirtKey >= 0xdb && uVirtKey <= 0xde) {
			SKDataIndex = uVirtKey - 0xdb + 0x2c;
		} else if (uVirtKey == 0xc0) {
			SKDataIndex = 0x2b;
		} else {
			SKDataIndex = 0;
		}

		if (lpbKeyState[VK_SHIFT] & 0x80) {
	        CHIByte = SKLayoutS[lpImeL->dwSKWant][SKDataIndex*2] & 0x00ff;
			CLOByte = SKLayoutS[lpImeL->dwSKWant][SKDataIndex*2 + 1] & 0x00ff;
		} else {
	        CHIByte = SKLayout[lpImeL->dwSKWant][SKDataIndex*2] & 0x00ff;
			CLOByte = SKLayout[lpImeL->dwSKWant][SKDataIndex*2 + 1] & 0x00ff;
		}

	    wSymbolCharCode = (CHIByte << 8) | CLOByte;
		if(wSymbolCharCode == 0x2020) {
		    MessageBeep((UINT) -1);
			uNumMsg = 0;
		} else {
        	uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
		}
	} 

    // CST_ALPHANUMERIC
    else if (iRet == CST_ALPHANUMERIC) {
	    if (lpImcP->fdwImeMsg & MSG_ALREADY_OPEN) {
            lpImcP->fdwImeMsg = (lpImcP->fdwImeMsg | MSG_CLOSE_CANDIDATE) &
                ~(MSG_OPEN_CANDIDATE) & ~(MSG_IN_IMETOASCIIEX);
       	    GenerateMessage(hIMC, lpIMC, lpImcP);
		}

        if (lpIMC->fdwConversion & IME_CMODE_SYMBOL) {
		    WORD wSymbolCharCode;

			if(wCharCode == '.') {
				wSymbolCharCode = '¡£';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == ',') {
				wSymbolCharCode = '£¬';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == ';') {
				wSymbolCharCode = '£»';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == ':') {
				wSymbolCharCode = '£º';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == '?') {
				wSymbolCharCode = '£¿';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == '!') {
				wSymbolCharCode = '£¡';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == '(') {
				wSymbolCharCode = '£¨';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == ')') {
				wSymbolCharCode = '£©';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == '\\') {
				wSymbolCharCode = '¡¢';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == '@') {
				wSymbolCharCode = '¡¤';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == '&') {
				wSymbolCharCode = '¡ª';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == '$') {
				wSymbolCharCode = '£¤';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == '_') {
				wSymbolCharCode = '¡ª';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, TRUE);
			} else if(wCharCode == '^') {
				wSymbolCharCode = '¡­';
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, TRUE);
			} else if(wCharCode == '"') {
				if(lpImcP->uSYHFlg) {
					wSymbolCharCode = '¡±';
				} else {
					wSymbolCharCode = '¡°';
				}
		        lpImcP->uSYHFlg ^= 0x00000001;
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == '\'') {
				if(lpImcP->uDYHFlg) {
					wSymbolCharCode = '¡¯';
				} else {
					wSymbolCharCode = '¡®';
				}
        		lpImcP->uDYHFlg ^= 0x00000001;
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == '<') {
				if(lpImcP->uDSMHFlg) {
					wSymbolCharCode = '¡´';
					lpImcP->uDSMHCount++;
				} else {
					wSymbolCharCode = '¡¶';
	        		lpImcP->uDSMHFlg = 0x00000001;
				}
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else if(wCharCode == '>') {
				if((lpImcP->uDSMHFlg) && (lpImcP->uDSMHCount)) {
					wSymbolCharCode = '¡µ';
					lpImcP->uDSMHCount--;
				} else {
					wSymbolCharCode = '¡·';
	        		lpImcP->uDSMHFlg = 0x00000000;
				}
	            uNumMsg = TranslateSymbolChar(lpdwTransBuf, wSymbolCharCode, FALSE);
			} else {
		        if (lpIMC->fdwConversion & IME_CMODE_FULLSHAPE) {
		            // convert to DBCS
		            uNumMsg = TranslateFullChar(lpdwTransBuf, wCharCode);
		        } else {
		            uNumMsg = TranslateToAscii(uVirtKey, uScanCode, lpdwTransBuf,
		                wCharCode);
		        }
			}
        } else if (lpIMC->fdwConversion & IME_CMODE_FULLSHAPE) {
            // convert to DBCS
            uNumMsg = TranslateFullChar(lpdwTransBuf, wCharCode);
        } else {
            uNumMsg = TranslateToAscii(uVirtKey, uScanCode, lpdwTransBuf,
                wCharCode);
        }
    }
    // CST_CHOOSE
    else if (iRet == CST_CHOOSE) {
         LPCANDIDATEINFO lpCandInfo;

        lpCandInfo = (LPCANDIDATEINFO)ImmLockIMCC(lpIMC->hCandInfo);
		if(!lpCandInfo){
            return (CST_INVALID);
		}

        if (uVirtKey == VK_PRIOR) {
            wCharCode = '-';
        } else if (uVirtKey == VK_NEXT) {
            wCharCode = '=';
        } else if (uVirtKey == VK_SPACE) {
            wCharCode = '1';
        } else if (uVirtKey >= 0 && uVirtKey <= '9') {
            // convert shift-0 ... shift-9 to 0 ... 9
            wCharCode = uVirtKey;
        } else if (uVirtKey == VK_HOME) {
            wCharCode = 0x24;
        } else if (uVirtKey == VK_END) {
            wCharCode = 0x23;
        } else {
        }

	    lpImcP->iImeState = CST_CHOOSE;
        ChooseCand(wCharCode, lpIMC, lpCandInfo, lpImcP);

        ImmUnlockIMCC(lpIMC->hCandInfo);

        uNumMsg = TranslateImeMessage(lpdwTransBuf, lpIMC, lpImcP);
    }
    // CST_INPUT(IME_CMODE_CHARCODE)
    else if (iRet == CST_INPUT &&
        lpIMC->fdwConversion & IME_CMODE_CHARCODE) {
        uNumMsg = TranslateToAscii(uVirtKey, uScanCode, lpdwTransBuf,
            wCharCode);
    }
    // CST_INPUT 
    else if (iRet == CST_INPUT) {
        LPGUIDELINE         lpGuideLine;

        // get lpCompStr & lpGuideLine
        lpCompStr = (LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
		if(!lpCompStr){
            return (CST_INVALID);
		}

        lpGuideLine = (LPGUIDELINE)ImmLockIMCC(lpIMC->hGuideLine);
		if(!lpGuideLine){
	        ImmUnlockIMCC(lpIMC->hCompStr);
            return (CST_INVALID);
		}

        // composition
        CompWord(wCharCode, lpIMC, lpCompStr, lpImcP, lpGuideLine);

        ImmUnlockIMCC(lpIMC->hGuideLine);
        ImmUnlockIMCC(lpIMC->hCompStr);
        
        // generate message
        uNumMsg = TranslateImeMessage(lpdwTransBuf, lpIMC, lpImcP);
    }
    else if (iRet == CST_INVALID_INPUT) {
	    MessageBeep((UINT) -1);
		uNumMsg = 0;
    }else {
        uNumMsg = TranslateToAscii(uVirtKey, uScanCode, lpdwTransBuf,
            wCharCode);
    }

    // reset lpImcP->fdwImeMsg
    lpImcP->fdwImeMsg &= (MSG_ALREADY_OPEN|MSG_ALREADY_START);
    lpImcP->fdwGcsFlag &= (GCS_RESULTREAD|GCS_RESULT);

    ImmUnlockIMCC(lpIMC->hPrivate);
    ImmUnlockIMC(hIMC);

    return (uNumMsg);
}
