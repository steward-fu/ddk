// TTYDLG.C
// TTY dialog functions.

#pragma code_seg("_CODESEG")
#define DB_UNKNOWN 0xFFFF  

#include "tty.h"
#include "ttyhelp.h"
BOOL bHelpWasCalled = FALSE;

// Making these global implies that only one properties page can be
// displayed at a time. If these pages are later changed to instanced
// data, then these variables should be converted to window data.
BOOL bApplyEnabled=FALSE;
BOOL bDontEnableApply=FALSE;

BOOL CALLBACK TTYDeviceOptionsSheet(hDlg, uMessage, wParam, lParam)
HWND hDlg;
UINT uMessage;
WPARAM wParam;
LPARAM lParam; 
{
  static int iNumprn, i, ii, j, iLastprn;
  short n, ntemp;
  static char cBuffer[CHAR_BUFSZ];
  BOOL bReturn=TRUE;

  switch(uMessage)
  {
   case WM_INITDIALOG:
        if (!(LoadTTYDataFile())) 
        {                            
         UnregisterClass(szName, hTTYInst);
         return FALSE;
        }
        
        // Make a copy of the custom code page
        
        // Prevent the Apply button from changing
        bDontEnableApply=TRUE;

        iNumprn = GetNumDevices();
        for (i = 1; i <= iNumprn; i++)
        {
         GetDevice(i);
         SendDlgItemMessage(hDlg, IDD_MODELNAME, CB_ADDSTRING, 0,
                            (LONG)(LPSTR) Device.DeviceName);
        }
        
        SendDlgItemMessage(hDlg, IDD_MODELNAME, CB_SETCURSEL, GetCurDevice() - 1,
                           0L);
        GetDevice(GetCurDevice());
        
        SendDlgItemMessage(hDlg, IDD_SCREENCHAR, WM_SETREDRAW, FALSE, 0L);
        for (ii = HI_ANSISTART; ii <= HI_ANSIEND; ii++)
        {
         wsprintf(cBuffer, " %3d = %c", ii, ii);
         SendDlgItemMessage(hDlg, IDD_SCREENCHAR, LB_ADDSTRING, 0, 
                            ((LONG)(LPSTR)cBuffer));
        }
        DisplayTTYModelData(hDlg, uMessage, wParam, DEVICESHEET);

        // Re-enable changes
        bDontEnableApply=FALSE;
   break;
        
   case WM_COMMAND:
   {
        BOOL bHasChanged=FALSE;

        switch(wParam)
        {
         case IDD_ADD:
              if (DialogBox(hTTYInst, MAKEINTRESOURCE(TTY_ADDMODEL_DLG), hDlg,
                            TTYAddNewModel)) 
              {
               SendDlgItemMessage(hDlg, IDD_MODELNAME, CB_ADDSTRING, NULL, 
                                  (LONG)(LPSTR)Device.DeviceName);
               SetDevice(n = GetNumDevices() + 1);
               SetCurDevice(n); 
               SendDlgItemMessage(hDlg, IDD_MODELNAME, CB_SETCURSEL, n - 1, 0L);
               DisplayTTYModelData(hDlg, uMessage, wParam, DEVICESHEET);
               bHasChanged=TRUE;
              }
              else
               GetDevice(GetCurDevice());
         break;

         case IDD_DELETE:
              ntemp = (int)SendMessage(GetDlgItem(hDlg, IDD_MODELNAME),
                                                  CB_GETCURSEL, 0, 0L);
              GetDevice(ntemp + 1);           
              // Write a routine in case we can't delete it.
              // -1 Device not found
              DeleteDevice(GetCurDevice());  // Delete current device
              SetCurDevice(ntemp);            // Set current = what we see
              GetDevice(ntemp);               // Now show data for this.
              iNumprn = (int)SendDlgItemMessage(hDlg, IDD_MODELNAME,
                                                CB_GETCURSEL, 0, 0L);
              SendDlgItemMessage(hDlg, IDD_MODELNAME, CB_DELETESTRING,
                                 iNumprn, 0L);
              SendDlgItemMessage(hDlg, IDD_MODELNAME, CB_SETCURSEL, 
                                 GetCurDevice() - 1 , 0L);
              DisplayTTYModelData(hDlg, uMessage, wParam, DEVICESHEET);
              bHasChanged=TRUE;
         break;
         
         case IDD_MODELNAME:
              if (HIWORD(lParam) == CBN_SELCHANGE)
              {
               ntemp = (int)SendMessage(GetDlgItem(hDlg, IDD_MODELNAME),
                                               CB_GETCURSEL, 0, 0L);
               GetDevice(ntemp+1);
               SetCurDevice(ntemp+1); // Get ready to show this model's data
               DisplayTTYModelData(hDlg, uMessage, wParam, DEVICESHEET);
               bHasChanged=TRUE;
              }
         break;
         
         case IDD_PRINTCHAR: 
              if (HIWORD(lParam) == EN_KILLFOCUS)
              {
                GetDlgItemText(hDlg, IDD_PRINTCHAR, Device.CTTCode[
                               SendDlgItemMessage(hDlg, IDD_SCREENCHAR, 
                               LB_GETCURSEL, 0, 0L)], CTTESCLEN + 1);
              }
              else if(HIWORD(lParam)==EN_CHANGE)
                bHasChanged=TRUE;
         break;

         case IDD_SCREENCHAR:
         {     
              switch (HIWORD(lParam))
              {
               case LBN_SELCHANGE:
                    j = (int)SendDlgItemMessage(hDlg, IDD_SCREENCHAR, 
                                                LB_GETCURSEL, 0, 0L);
                    if (j >= 0)
                    {
                     EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR), TRUE);
                     SetDlgItemText(hDlg, IDD_PRINTCHAR, Device.CTTCode[j]);
                    }
                    else
                     EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR), FALSE);
               break;
              }
         }
         break;

         case IDD_CUSTOMCHARSET:
              j = (int)SendDlgItemMessage(hDlg, IDD_SCREENCHAR, 
                                          LB_GETCURSEL, 0, 0L);
              if (j >= 0)
              {
               SetCodePage(hDlg, RES_DB_CUSTOM);
               EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR), TRUE);
               EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR_LABEL), TRUE);
               EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR), TRUE);
               EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR_LABEL), TRUE);
               SetDlgItemText(hDlg, IDD_PRINTCHAR, Device.CTTCode[j]);
              }
              else
               EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR), FALSE);

              bHasChanged=TRUE;
         break;


         case IDD_437CHARSET: // We don't allow changes to 437 or 850 tables!
              SetCodePage(hDlg, RES_DB_CP437);
              EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR_LABEL), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR_LABEL), FALSE);
              bHasChanged=TRUE;
         break;

         case IDD_850CHARSET:
              SetCodePage(hDlg, RES_DB_CP850);
              EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR_LABEL), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR_LABEL), FALSE);
              bHasChanged=TRUE;
         break;

         // Controls that we watch only to enable the apply button
         case IDD_BEGINJOB_ESC:
         case IDD_ENDJOB_ESC:
         case IDD_PAPERSIZE_ESC:
         case IDD_PAPERSRC_ESC:
              if(EN_CHANGE==HIWORD(lParam))
                  bHasChanged=TRUE;
              bReturn=FALSE;
              break;

         default: // switch (wParam)
               return FALSE;
        }

        if(!bDontEnableApply && !bApplyEnabled && bHasChanged)
        {
            PropSheet_Changed(GetParent(hDlg),0);
            bApplyEnabled=TRUE;
        }
   }
   break; // WM_COMMAND

   case WM_NOTIFY:
   {
        NMHDR FAR *lpnmhdr = (NMHDR FAR *)lParam;
        
        switch (lpnmhdr->code)
        {
         case PSN_SETACTIVE:
         break;
         
         case PSN_KILLACTIVE:  
              if (!SaveTTYModelData(hDlg, DEVICESHEET))
              {
               SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
               return TRUE;
              }
              return FALSE;
         break;
         
         case PSN_APPLY:
              SaveTTYModelData(hDlg, DEVICESHEET);
              bApplyEnabled=FALSE;
              break;
              
         case PSN_RESET: 
              WinHelp(hDlg, (LPCSTR)HELPFILENAME, HELP_QUIT, NULL);
              break;
        }
   }
   break;
   
   case WM_CONTEXTMENU:
        WinHelp(hDlg ,(LPCSTR)HELPFILENAME, HELP_CONTEXTMENU,
                (DWORD)(LPSTR)aKeyWordIds);
        break;
   
   case WM_HELP:
        WinHelp(((LPHELPINFO)lParam)->hItemHandle, (LPCSTR)HELPFILENAME,
                HELP_WM_HELP, (DWORD)(LPSTR)aKeyWordIds);
        break;
   
   default: 
         return FALSE;
  }
 return bReturn;
}


BOOL CALLBACK TTYFontOptionsSheet(hDlg, uMessage, wParam, lParam)
HWND hDlg;
UINT uMessage;
WPARAM wParam;
LPARAM lParam; 
{
  switch(uMessage)
  {
   case WM_INITDIALOG:
        if (!(LoadTTYDataFile())) 
        {                            
         UnregisterClass(szName, hTTYInst);
         return FALSE;
        }
        GetDevice(GetCurDevice());
   break;
   
   case WM_COMMAND:
        switch(wParam)
        {
            // We only need to catch these to enable the apply button
            case IDD_10CPI_ESC:
            case IDD_12CPI_ESC:
            case IDD_17CPI_ESC:
            case IDD_DBLON_ESC:
            case IDD_DBLOFF_ESC:
            case IDD_BOLDON_ESC:
            case IDD_BOLDOFF_ESC:
            case IDD_ULINEON_ESC:
            case IDD_ULINEOFF_ESC:
                if((EN_CHANGE==HIWORD(lParam)) && !bDontEnableApply &&
                    !bApplyEnabled)
                {
                    PropSheet_Changed(GetParent(hDlg),0);
                    bApplyEnabled=TRUE;
                }
        }
        return FALSE;

   case WM_NOTIFY:
   {
        NMHDR FAR *lpnmhdr = (NMHDR FAR *)lParam;
        
        switch (lpnmhdr->code)
        {
         case PSN_SETACTIVE:
              bDontEnableApply=TRUE;
              DisplayTTYModelData(hDlg, uMessage, wParam, FONTSHEET);
              bDontEnableApply=FALSE;
         break;
         
         case PSN_KILLACTIVE:
              if (!SaveTTYModelData(hDlg, FONTSHEET))
              {
               SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
               return TRUE;
              }
              return FALSE;
         break;
         
         case PSN_APPLY:
              SaveTTYModelData(hDlg, FONTSHEET);
              bApplyEnabled=FALSE;
              break;
   
         case PSN_RESET: 
              WinHelp(hDlg, (LPCSTR)HELPFILENAME, HELP_QUIT, NULL);
              break;
        }
   }
   break;
   
   case WM_CONTEXTMENU:
        WinHelp(hDlg ,(LPCSTR)HELPFILENAME, HELP_CONTEXTMENU,
                (DWORD)(LPSTR)aKeyWordIds);
        break;
   
   case WM_HELP:
        WinHelp(((LPHELPINFO)lParam)->hItemHandle, (LPCSTR)HELPFILENAME,
                HELP_WM_HELP, (DWORD)(LPSTR)aKeyWordIds);
        break;
   
   
   default: 
         return FALSE;
  }
 return TRUE;
}

VOID NEAR PASCAL DisplayTTYModelData(
HWND hDlg,
UINT uMessage,
WPARAM wParam,
BYTE PropSheetType)
{
  int CodePageValue;
  
  if ((PropSheetType == DEVICESHEET) &&
     ((uMessage == WM_INITDIALOG) || (wParam == IDD_MODELNAME) ||
      (wParam == IDD_ADD) || (wParam == IDD_DELETE)))
  { 
   CodePageValue = Device.DeviceData.CodePage;
   switch (CodePageValue)
   {
    case RES_DB_CP437: // We don't allow user to modify 437/850 Tables.
         SetCodePage(hDlg, RES_DB_CP437);
         EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR), FALSE);
         EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR_LABEL), FALSE);
         EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR), FALSE);
         EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR_LABEL), FALSE);
    break;                                                      
   
    case RES_DB_CP850:
         SetCodePage(hDlg, RES_DB_CP850);
         EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR), FALSE);
         EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR_LABEL), FALSE);
         EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR), FALSE);
         EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR_LABEL), FALSE);
    break;

    case RES_DB_CUSTOM:
         SetCodePage(hDlg, RES_DB_CUSTOM);
         EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR), TRUE);
         EnableWindow(GetDlgItem(hDlg, IDD_PRINTCHAR_LABEL), TRUE);
         EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR), TRUE);
         EnableWindow(GetDlgItem(hDlg, IDD_SCREENCHAR_LABEL), TRUE);
    break;
   }
   
   SendDlgItemMessage(hDlg, IDD_SCREENCHAR, WM_SETREDRAW, TRUE, 0L);
   SendDlgItemMessage(hDlg, IDD_PRINTCHAR, EM_LIMITTEXT, CTTESCLEN+1, 0L); 
   SendDlgItemMessage(hDlg, IDD_SCREENCHAR, LB_SETCURSEL, HILIGHTCHARA, 0L);
   SendDlgItemMessage(hDlg, IDD_SCREENCHAR, LB_SETCURSEL, HILIGHTCHARB, 0L);
   SetDlgItemText(hDlg, IDD_PRINTCHAR, Device.CTTCode[HILIGHTCHARB]);
   SetDlgItemText(hDlg, IDD_BEGINJOB_ESC, Device.DeviceData.BeginJob);  
   SetDlgItemText(hDlg, IDD_ENDJOB_ESC, Device.DeviceData.EndJob);  
   SetDlgItemText(hDlg, IDD_PAPERSIZE_ESC, Device.DeviceData.PaperSize);  
   SetDlgItemText(hDlg, IDD_PAPERSRC_ESC, Device.DeviceData.PaperSource);  
   SetFocus(GetDlgItem(hDlg, IDD_MODELNAME));
  } 

  // Grey out Delete button if we have only one model's data left to display
  
  if ((PropSheetType == DEVICESHEET) && 
     ((uMessage == WM_INITDIALOG || IDD_DELETE) && GetCurDevice() == ONLY_MODEL))
    EnableWindow(GetDlgItem(hDlg, IDD_DELETE), FALSE);
  else
    EnableWindow(GetDlgItem(hDlg, IDD_DELETE), TRUE);
  
  if (PropSheetType == FONTSHEET)
  {   
   SetDlgItemText(hDlg, IDD_10CPI_ESC, Device.DeviceData.f10Pitch);
   SetDlgItemText(hDlg, IDD_12CPI_ESC, Device.DeviceData.f12Pitch);
   SetDlgItemText(hDlg, IDD_17CPI_ESC, Device.DeviceData.f17Pitch);
   SetDlgItemText(hDlg, IDD_DBLON_ESC, Device.DeviceData.DoubleOn);
   SetDlgItemText(hDlg, IDD_DBLOFF_ESC, Device.DeviceData.DoubleOff); 
   SetDlgItemText(hDlg, IDD_BOLDON_ESC, Device.DeviceData.BoldOn);
   SetDlgItemText(hDlg, IDD_BOLDOFF_ESC, Device.DeviceData.BoldOff); 
   SetDlgItemText(hDlg, IDD_ULINEON_ESC, Device.DeviceData.UnderlineOn);
   SetDlgItemText(hDlg, IDD_ULINEOFF_ESC, Device.DeviceData.UnderlineOff); 
  }
}  
  

BOOL CALLBACK TTYAddNewModel(hDlg, uMessage, wParam, lParam)
HWND hDlg;
UINT uMessage;
WPARAM wParam;
LPARAM lParam; 
{
  switch (uMessage)
  {
   case WM_INITDIALOG: // Null all data.
        InitialiseNewTTYData(); // From TTYFILE.C
        LoadString(hTTYInst, IDS_MSG_NEWMODELID, (LPSTR)Device.DeviceName, MODELNAME);
        SetDlgItemText(hDlg, IDD_NEW_MODELNAME, Device.DeviceName);
        SendDlgItemMessage(hDlg, IDD_NEW_MODELNAME, EM_LIMITTEXT, GEN_TXT_LEN, 0L);
        SetFocus(GetDlgItem(hDlg, IDD_NEW_MODELNAME));
        SendDlgItemMessage(hDlg, IDD_NEW_MODELNAME, EM_SETSEL, 0, 0x7fff0000L);
        if (!lstrlen(Device.DeviceName))
          EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
        return FALSE;
   
   case WM_COMMAND:
   {
        switch(wParam)
        {
         case IDD_NEW_MODELNAME:
              if (SendDlgItemMessage(hDlg, IDD_NEW_MODELNAME, WM_GETTEXTLENGTH, 0, 0L))
               EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
              else
               EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
         break;

         case IDOK:
              GetDlgItemText(hDlg, IDD_NEW_MODELNAME, Device.DeviceName, 
                             GEN_TXT_LEN + 1);
              EndDialog(hDlg, IDOK);
         break;

         case IDCANCEL:
              EndDialog(hDlg, FALSE);
         break;
        }
   }
   break;  // WM_COMMAND
   
   case WM_CONTEXTMENU:
        WinHelp(hDlg ,(LPCSTR)HELPFILENAME, HELP_CONTEXTMENU,
                (DWORD)(LPSTR)aKeyWordIds);
        break;
   
   case WM_HELP:
        WinHelp(((LPHELPINFO)lParam)->hItemHandle, (LPCSTR)HELPFILENAME,
                HELP_WM_HELP, (DWORD)(LPSTR)aKeyWordIds);
        break;
   
   default: // switch (wParam)
          return FALSE;
  } 
  return TRUE;
}


BOOL NEAR PASCAL SaveTTYModelData(hDlg, PropertySheetPage)
HWND hDlg;
UINT PropertySheetPage;
{
  BOOL bOK = TRUE;
  
  if (PropertySheetPage == DEVICESHEET)
  {
   GetDlgItemText(hDlg, IDD_BEGINJOB_ESC, Device.DeviceData.BeginJob, DEVICEESCLEN + 1);
   GetDlgItemText(hDlg, IDD_ENDJOB_ESC, Device.DeviceData.EndJob, DEVICEESCLEN + 1);
   GetDlgItemText(hDlg, IDD_PAPERSIZE_ESC, Device.DeviceData.PaperSize, DEVICEESCLEN + 1);
   GetDlgItemText(hDlg, IDD_PAPERSRC_ESC, Device.DeviceData.PaperSource, DEVICEESCLEN + 1);
   SetDevice(GetCurDevice());
   return bOK;
  }
  else
  if (PropertySheetPage == FONTSHEET)
  {
   GetDlgItemText(hDlg, IDD_10CPI_ESC, Device.DeviceData.f10Pitch, FONTESCLEN + 1);
   GetDlgItemText(hDlg, IDD_12CPI_ESC, Device.DeviceData.f12Pitch, FONTESCLEN + 1); 
   GetDlgItemText(hDlg, IDD_17CPI_ESC, Device.DeviceData.f17Pitch, FONTESCLEN + 1);
   GetDlgItemText(hDlg, IDD_DBLON_ESC, Device.DeviceData.DoubleOn, FONTESCLEN + 1);
   GetDlgItemText(hDlg, IDD_DBLOFF_ESC, Device.DeviceData.DoubleOff, FONTESCLEN + 1);
   GetDlgItemText(hDlg, IDD_BOLDON_ESC, Device.DeviceData.BoldOn, FONTESCLEN + 1);
   GetDlgItemText(hDlg, IDD_BOLDOFF_ESC, Device.DeviceData.BoldOff, FONTESCLEN + 1);
   GetDlgItemText(hDlg, IDD_ULINEON_ESC, Device.DeviceData.UnderlineOn, FONTESCLEN + 1);
   GetDlgItemText(hDlg, IDD_ULINEOFF_ESC, Device.DeviceData.UnderlineOff, FONTESCLEN + 1);
   SetDevice(GetCurDevice());
   return bOK;
  }
}

VOID NEAR PASCAL SetCodePage(HWND hDlg, UINT iCodePageOption)
{
  if (iCodePageOption == DB_UNKNOWN)
  {
   if (IsDlgButtonChecked(hDlg,IDD_CUSTOMCHARSET))
     iCodePageOption=RES_DB_CUSTOM;
   else 
   if (IsDlgButtonChecked(hDlg,IDD_437CHARSET))
     iCodePageOption=RES_DB_CP437;
   else 
   if (IsDlgButtonChecked(hDlg,IDD_850CHARSET))
     iCodePageOption=RES_DB_CP850;
  }
  
  CheckRadioButton(hDlg, IDD_CUSTOMCHARSET, IDD_850CHARSET, 
                   IDD_CUSTOMCHARSET + iCodePageOption);
  EnableWindow(GetDlgItem(hDlg, IDD_CUSTOMCHARSET + iCodePageOption), TRUE); 
  Device.DeviceData.CodePage = RES_DB_CUSTOM + iCodePageOption;
}

// Chicago TTY dialog, edit control functions.
// Each of the edit controls in the UI it's own independent class, pedit.
// The functions below handle keyboard hits such as the <esc> & <ctrl> keys.
// Code skeleton from Win 3.1 TTY driver, dials.c.
// Cleaned up buffer, memory handling, everything.


#define TEXTBUFF        0
#define CHARPOS         2
#define FIRSTCHAR       4
#define BACKSPACE       '\x08'

#define SetEditBuff(hWnd,bf)     SetWindowWord(hWnd,TEXTBUFF, (WORD)(bf))
#define SetEditPos(hWnd,h)       SetWindowWord(hWnd,CHARPOS, (WORD)(h))
#define SetEditLeft(hWnd,f)      SetWindowWord(hWnd,FIRSTCHAR, (WORD)(f))

#define EditBuff(hWnd)           (HGLOBAL)GetWindowWord(hWnd, TEXTBUFF)
#define EditPos(hWnd)            GetWindowWord(hWnd, CHARPOS)
#define EditLeft(hWnd)           GetWindowWord(hWnd, FIRSTCHAR)

PAINTSTRUCT ps;
TEXTMETRIC  tm;

BYTE            byText[7*DEVICEESCLEN];
BYTE            byPos[DEVICEESCLEN+2];
int             iShift;
int             iControl;
int             iReturn;
int             iTab;
int             iBackSpace;
int             iNumberCapture;
UINT            uNumberCaptured;


VOID NEAR ecInsert(LPSTR lpTextString, BYTE byCharToInsert)
{
    BYTE tBuff[DEVICEESCLEN];
    
    lstrcpy((LPBYTE)tBuff+1, lpTextString);
    tBuff[0] = byCharToInsert;
    lstrcpy(lpTextString, (LPBYTE)tBuff);
}


VOID NEAR ecDelete(LPSTR pChar)
{
  lstrcpy(pChar, pChar+1);
}


VOID NEAR ecReplace(LPSTR lpControlChar, LPSTR lpExpandedString)
{
  BYTE tBuff[7*DEVICEESCLEN];

  lstrcpy((LPBYTE)tBuff, lpExpandedString);
  lstrcat((LPBYTE)tBuff, lpControlChar+1);
  lstrcpy(lpControlChar, (LPBYTE)tBuff);
}


int NEAR Printable(LPSTR lpCharA, LPSTR lpCharB)
{            
    int n1 ,n2 , n3;
    char c, HexVal[5];

    *lpCharB = '\0';
    n3 = 0;
    
    for (n1=0; lpCharA[n1];)
    {
     n3++;
     *lpCharB++ = (char)n1;
     if (lpCharA[n1] == '\x1b')
     {
      ecReplace(lpCharA + n1, "<ESC>");
      n1 += 5;
     }
     else 
     if ((BYTE)lpCharA[n1] == (BYTE)'\xFF')
     {
      ecReplace(lpCharA + n1, "<FF>");
      n1 += 4;
     }
     else 
     if (iscntrl((BYTE)lpCharA[n1]))
     {
      c = lpCharA[n1];
      ecReplace(lpCharA + n1, "^ ");
      n1 += 2;
      lpCharA[n1-1] = (char)'@' + c;
     }
     else 
     if (isprint((BYTE)lpCharA[n1]))
         n1++;
     else
     {
      for (c = 3, n2= (BYTE)lpCharA[n1]; c; c--, n2 /= 10)
         HexVal[c] = (char)(n2%10)+(char)'0';
         HexVal[0] = '<';
         HexVal[4] = '>';
         HexVal[5] = '\0';
         ecReplace(lpCharA + n1, HexVal);
         n1 += 5;
     }
    }
    *lpCharB = (char)n1;
 return n3;
}


int NEAR RealPos(HWND hWnd, int i, char *pBuff, BYTE byPos[])
{
    int j;
    HDC hDc;

    hDc = GetDC(hWnd);
    
    for (j = 0; i > (int)LOWORD(GetTextExtent(hDc, pBuff, byPos[j])); j++)
       if (*(pBuff+byPos[j]) == '\0')
       {
         j++;
         break;
       }
    j--;
    
    ReleaseDC(hWnd, hDc);
    return j;
}

VOID NEAR PutCaret(HWND hWnd, int nCharNum)
{
    RECT    rect;
    HDC     hDc;
    int     pos, relPos;  //   position of caret relative to EditLeft.
    HGLOBAL hGlb;
    LPSTR  lpTextBuff;
    
    GetClientRect(hWnd,(LPRECT)&rect);
    hDc = GetDC(hWnd);
    
    hGlb = EditBuff(hWnd);
    lpTextBuff = (LPSTR)GlobalLock(hGlb);
    lstrcpy((LPSTR)byText, lpTextBuff);
    GlobalUnlock(hGlb);
    
    Printable((LPBYTE)byText, (LPBYTE)byPos);
    
    while(TRUE)
    {
     if ((int)(relPos = nCharNum-EditLeft(hWnd)) < 0)
       SetEditLeft(hWnd, EditLeft(hWnd)-1);
     else 
     {
      pos = LOWORD(GetTextExtent(hDc, byText+EditLeft(hWnd), relPos)) + 1;
      if (pos >= rect.right - rect.left)
        SetEditLeft(hWnd, EditLeft(hWnd)+1);
      else
      break;
     }
     InvalidateRect(hWnd, NULL, TRUE);
    }

    ReleaseDC(hWnd, hDc);
    SetCaretPos(pos,1);
}

VOID NEAR ecCopy(HWND hWnd)
{
    HGLOBAL hClp,hGlb;

    OpenClipboard(hWnd);
    EmptyClipboard();
    
    hClp = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, (DWORD)(DEVICEESCLEN+2));
    hGlb = EditBuff(hWnd);
    lstrcpy(GlobalLock(hClp), GlobalLock(hGlb));
    GlobalUnlock(hGlb);
    GlobalUnlock(hClp);

    SetClipboardData(CF_TEXT,hClp);
    
    CloseClipboard();
}

VOID NEAR ecClear(HWND hWnd)
{
    HGLOBAL hGlb; 
   
    hGlb = EditBuff(hWnd);
    *(GlobalLock(hGlb)) = '\0';
    GlobalUnlock(hGlb);
    
    SetEditPos(hWnd,0);
    PutCaret(hWnd, 0);
    InvalidateRect(hWnd, NULL, TRUE);
}

VOID NEAR ecCut(HWND hWnd)
{
    ecCopy(hWnd);
    ecClear(hWnd);
}

VOID NEAR ecPaste(HWND hWnd)
{
    HGLOBAL hClp, hGlb;
    LPSTR   lpAuxStr, lpTextBuff;
    register int i;

    OpenClipboard(hWnd);

    if (hClp = GetClipboardData(CF_TEXT))
    {
     lpAuxStr = GlobalLock(hClp);
     hGlb = EditBuff(hWnd);
     lpTextBuff = (LPSTR)GlobalLock(hGlb);
     
     if (lstrlen(lpAuxStr)+lstrlen(lpTextBuff) > DEVICEESCLEN)
       MessageBeep(MB_OK);
     else
     {
      for (i = 0; *(lpAuxStr+i); i++)
      {
       ecInsert(lpTextBuff+EditPos(hWnd), *(lpAuxStr+i));
       SetEditPos(hWnd, EditPos(hWnd)+1);
      }
      lstrcpy(byText, lpTextBuff);
      Printable((LPBYTE)byText, (LPBYTE)byPos);
      PutCaret(hWnd, byPos[EditPos(hWnd)]);
      InvalidateRect(hWnd, NULL, TRUE);
      UpdateWindow(hWnd);
     }
     GlobalUnlock(hGlb);
     GlobalUnlock(hClp);
    }
    CloseClipboard();
}

VOID NEAR ecSysVirtual(HWND hWnd, WORD wCheckKeyPressed)
{
    static int i;
    static WORD SysKeys[] = {VK_INSERT,VK_END,VK_DOWN,VK_NEXT, VK_LEFT,
                             VK_CLEAR,VK_RIGHT,VK_HOME,VK_UP,VK_PRIOR};

    for (i=0; i<=9; i++)
       if (SysKeys[i] == wCheckKeyPressed)
         break;
       if (i == 10)
         return;
    
    iNumberCapture = TRUE;
    
    if (uNumberCaptured < 0xff)
      uNumberCaptured = uNumberCaptured * 10 + i;
    if (uNumberCaptured > 0xff)
      uNumberCaptured = uNumberCaptured % 0xFF;
}

VOID NEAR ecVirtual(HWND hWnd, WORD wCheckKeyPressed)
{
    UINT i;
    HWND hFocus;
    HGLOBAL hGlb;
    LPSTR lpTextBuff;

    hGlb = EditBuff(hWnd);
    lpTextBuff = (LPSTR)GlobalLock(hGlb);

    switch(wCheckKeyPressed)
    {
      case VK_SHIFT:
           iShift = 1;
           break;

      case VK_CONTROL:
           iControl = 1;
           break;

      case VK_LEFT:
           if (EditPos(hWnd) <= 0)
           {
            SetEditPos(hWnd, 0);
            PutCaret(hWnd, 0);
            MessageBeep(MB_OK);
            break;
           }
           lstrcpy((LPSTR)byText, lpTextBuff);
           Printable((LPBYTE)byText, (LPBYTE)byPos);
           SetEditPos(hWnd, EditPos(hWnd)-1);
           PutCaret(hWnd, byPos[EditPos(hWnd)]);
           break;

      case VK_RIGHT:
           
           lstrcpy((LPSTR)byText, lpTextBuff);
           if (EditPos(hWnd) >= (i = Printable((LPBYTE)byText, (LPBYTE)byPos)))
           {
            SetEditPos(hWnd, i);
            PutCaret(hWnd, byPos[i]);
            MessageBeep(MB_OK);
            break;
           }
           SetEditPos(hWnd, EditPos(hWnd)+1);
           PutCaret(hWnd, byPos[EditPos(hWnd)]);
           break;

      case VK_HOME:
           lstrcpy((LPSTR)byText, lpTextBuff);
           Printable((LPBYTE)byText, (LPBYTE)byPos);
           SetEditPos(hWnd, 0);
           PutCaret(hWnd, 0);
           break;

      case VK_END:
           lstrcpy((LPSTR)byText, lpTextBuff);
           i = Printable((LPBYTE)byText, (LPBYTE)byPos);
           SetEditPos(hWnd, i);
           PutCaret(hWnd, byPos[i]);
           break;

      case VK_TAB:
           hFocus = GetNextDlgTabItem(GetParent(hWnd), hWnd, iShift);
           iTab = 1;
           if (hFocus)
             SetFocus(hFocus);
           break;

      case VK_BACK:
           iBackSpace = 1;
           break;

      case VK_RETURN:
           iReturn = 1;
           SendMessage(GetParent(hWnd), WM_COMMAND, IDOK, 0L);
           break;

      case VK_INSERT:
           if (iShift)
             ecPaste(hWnd);
           else 
           if (iControl)
             ecCopy(hWnd);
           break;

      case VK_DELETE:
           if (iShift)
             ecCut(hWnd);
           else
             ecClear(hWnd);
           break;
    }
    GlobalUnlock(hGlb);
}


VOID NEAR ecInput(HWND hWnd, UINT uExtKeyPress)
{
  HGLOBAL hGlb;
  LPSTR lpTextBuff;
  HWND hWndParent;
  BOOL bChange=TRUE;

   hGlb = EditBuff(hWnd);
   lpTextBuff = (LPSTR)GlobalLock(hGlb);
    
    switch(uExtKeyPress)
    {
      case BACKSPACE:
           if (iBackSpace)
           {
            if (EditPos(hWnd) <= 0)
            {
              MessageBeep(MB_OK);
              bChange=FALSE;
            }
            else
            {
             ecDelete(lpTextBuff + EditPos(hWnd)-1);
             SetEditPos(hWnd, EditPos(hWnd)-1);
             lstrcpy((LPSTR)byText, lpTextBuff);
             Printable((LPBYTE)byText, (LPBYTE)byPos);
             PutCaret(hWnd, byPos[EditPos(hWnd)]);
            }
            InvalidateRect(hWnd, NULL, TRUE);
            iBackSpace = 0;
            break;
           }

      default:
           if (uExtKeyPress == '\011' && iTab)
           {
            iTab = 0;
            break;
           }
           if (uExtKeyPress == '\015' && iReturn)
           {
            iReturn = 0;
           }
           if (lstrlen(lpTextBuff) >= DEVICEESCLEN)
           {
            MessageBeep(MB_OK);
            break;
           }
           if (iNumberCapture)
           {
            iNumberCapture = FALSE;
            uExtKeyPress = uNumberCaptured;
           }
           if (uExtKeyPress == 0)
             uExtKeyPress = (BYTE)'\xFF';
           
           ecInsert(lpTextBuff+EditPos(hWnd), (BYTE)uExtKeyPress);
           SetEditPos(hWnd, EditPos(hWnd)+1);
           lstrcpy((LPSTR)byText, lpTextBuff);
           Printable((LPBYTE)byText, (LPBYTE)byPos);
           PutCaret(hWnd,byPos[EditPos(hWnd)]);
           InvalidateRect(hWnd, NULL, TRUE);
           break;
    }
    GlobalUnlock(hGlb);

    // Look like an edit control to the parent
    if(bChange && (hWndParent=GetParent(hWnd)))
    {
        SendMessage(hWndParent,WM_COMMAND,GetDlgCtrlID(hWnd),
            MAKELPARAM(hWnd,EN_CHANGE));
    }
}

// This function traps all keybord input from the extended keys 
// ESC, CTRL and ALT XXX operations and displays the actual characters as
// <ESC>, ^ and <XXX> respectively.
// Initially called from TTY.C - PsuedoInit().


LONG FAR PASCAL TTYPseudoEdit(HWND hWnd, UINT uMessage, WORD wParam, 
                              DWORD lParam)
{
    HDC hDc;
    HGLOBAL hGlb;
    LPSTR lpTextBuff;
    int i;

    SetBkMode(ps.hdc, TRANSPARENT);

    switch(uMessage)
    {
      case WM_KEYDOWN:
           ecVirtual(hWnd, wParam);
           break;

      case WM_KEYUP:
           switch(wParam)
           {
             case VK_SHIFT:
                  iShift = 0;
                  break;
            }
            break;

      case WM_SYSKEYDOWN:
           if (wParam == VK_MENU)
           {
             uNumberCaptured = 0;
             iNumberCapture = 0;
           }
           else
            ecSysVirtual(hWnd,wParam);
           break;

      case WM_SYSKEYUP:
           if ((wParam == VK_MENU) && iNumberCapture && !uNumberCaptured)
             ecInput(hWnd, wParam);
           break;

      case WM_CHAR:
           ecInput(hWnd, wParam);
           break;

      case WM_CREATE:
           hGlb = GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, DEVICEESCLEN + 1);
           SetEditBuff(hWnd, (WORD)hGlb);
           SetEditPos(hWnd, 0);
           SetEditLeft(hWnd, 0); 
           iShift = iTab = iReturn = iBackSpace = 0;
           break;

      case WM_DESTROY:
           hGlb = EditBuff(hWnd);
           GlobalFree(hGlb);
           break;

      case WM_LBUTTONDOWN:
           if (GetFocus() != hWnd)
             SetFocus(hWnd);

           hDc = GetDC(hWnd);
           GetTextMetrics(hDc, (LPTEXTMETRIC)&tm);
           ReleaseDC(hWnd, hDc);
           
           hGlb = EditBuff(hWnd);
           lpTextBuff = (LPSTR)GlobalLock(hGlb);
           lstrcpy((LPSTR)byText, lpTextBuff);
           GlobalUnlock(hGlb);           
           
           Printable((LPBYTE)byText, (LPBYTE)byPos);
           i = RealPos(hWnd, LOWORD(lParam), byText, byPos);
           PutCaret(hWnd, byPos[i]);
           SetEditPos(hWnd, i);
           break;

      case WM_SETFOCUS:
           hDc = GetDC(hWnd);
           GetTextMetrics(hDc, (LPTEXTMETRIC)&tm);
           ReleaseDC(hWnd, hDc);
           
           i = tm.tmHeight + tm.tmExternalLeading;
           CreateCaret(hWnd, NULL, 1, i);
           lstrcpy((LPSTR)byText, lpTextBuff);
           Printable((LPBYTE)byText, (LPBYTE)byPos);
           PutCaret(hWnd, byPos[EditPos(hWnd)]);
           ShowCaret(hWnd);
           SendMessage(GetParent(hWnd),WM_COMMAND,
                       GetWindowWord(hWnd,GWW_ID),MAKELONG(hWnd, EN_SETFOCUS));
           break;

      case WM_KILLFOCUS:
           HideCaret(hWnd);
           DestroyCaret();
           iBackSpace = 0;
           SendMessage(GetParent(hWnd), WM_COMMAND,
                       GetWindowWord(hWnd,GWW_ID),MAKELONG(hWnd, EN_KILLFOCUS));
           break;

      case WM_PAINT:
           BeginPaint(hWnd ,(LPPAINTSTRUCT)&ps);
           
           hGlb = EditBuff(hWnd);
           lpTextBuff = (LPSTR)GlobalLock(hGlb);
           lstrcpy((LPSTR)byText, lpTextBuff);
           GlobalUnlock(hGlb);

           Printable((LPBYTE)byText, (LPBYTE)byPos);
           TextOut(ps.hdc, 1, 1, (LPSTR)byText+EditLeft(hWnd),
           lstrlen((LPSTR)(byText+EditLeft(hWnd))));
           EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
           break;

      case WM_GETDLGCODE:
           return DLGC_WANTALLKEYS;

      case WM_GETTEXT:
           hGlb = EditBuff(hWnd);
           lpTextBuff = (LPSTR)GlobalLock(hGlb);
           
           for (i = 0; *(lpTextBuff + i); i++)
           {
            if (i >= ((int)wParam-1))
            {
             *((LPSTR)lParam + wParam) = '\0';
             return i;
            }
            *((LPSTR)lParam + i) = *(lpTextBuff + i);
           }
           *((LPSTR)lParam + i) = '\0';
           
           GlobalUnlock(hGlb);
           
           return i;
           
      case WM_SETTEXT:
           hGlb = EditBuff(hWnd);
           lpTextBuff = (LPSTR)GlobalLock(hGlb);
           
           for (i = 0; i < DEVICEESCLEN && *((LPSTR)lParam + i); i++)
              *(lpTextBuff + i) = *((LPSTR)lParam + i);
           
           *(lpTextBuff + i) = '\0';

           if (EditPos(hWnd) > (WORD)lstrlen(lpTextBuff))
             SetEditPos(hWnd, (WORD)lstrlen(lpTextBuff));
           
           GlobalUnlock(hGlb);
           InvalidateRect(hWnd, NULL, TRUE);
           return i;

      default:
           return DefWindowProc(hWnd, uMessage, wParam, lParam);
    }
    SetBkMode(ps.hdc, OPAQUE);
    return 0;
}

