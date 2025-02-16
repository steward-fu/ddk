; ==================================================================================================
; Title:      DbgOutText_TX.inc
; Author:     G. Friedrich
; Version:    C.1.0
; Notes:      Version C.1.0, October 2017
;               - First release.
; ==================================================================================================


externdef DbgColorError:DWORD

; 覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧
; Procedure:  DbgOutTextA / DbgOutTextW
; Purpose:    Send a string to the debug output device.
; Arguments:  Arg1: -> Zero terminated string.
;             Arg2: Foreground RGB color value.
;             Arg3: Background RGB color value.
;             Arg4: Effect value (DBG_EFFECT_XXX)
;             Arg5: -> Destination window WIDE name.
; Return:     Nothing.

align ALIGN_CODE
ProcName proc uses xbx pString:POINTER, dForeColor:DWORD, dBackColor:DWORD, dEffects:DWORD, pDest:POINTER
  local CDS:COPYDATASTRUCT, dCharsWritten:DWORD
  local dInfo1:DWORD, dInfo2:DWORD, dInfo3:DWORD, dInfo4:DWORD

  .if pString == NULL
    c2m pString, $OfsCStr("NULL Pointer"), xax
    m2m dForeColor, DbgColorError, edx
  .endif

  mov eax, dDbgDev
  .if eax == DBG_DEV_WIN_LOG
    .if $invoke(DbgOpenLog)
      invoke StrSize, pString
      sub eax, sizeof(CHR)
      invoke WriteFile, hDbgDev, pString, eax, NULL, NULL
    .endif
    .ifBitSet dEffects, DBG_EFFECT_NEWLINE
      invoke WriteFile, hDbgDev, offset cCRLF, 2*sizeof(CHR), NULL, NULL
    .endif

  .elseif eax == DBG_DEV_WIN_CON
    .if $invoke(DbgOpenCon)
      invoke RGB24To16ColorIndex, dForeColor
      invoke SetConsoleTextAttribute, hDbgDev, ax
      invoke StrLength, pString
      lea xbx, dCharsWritten
      invoke WriteConsole, hDbgDev, pString, eax, xbx, NULL
      .ifBitSet dEffects, DBG_EFFECT_NEWLINE
        invoke WriteConsole, hDbgDev, offset cCRLF, 2*sizeof(CHR), xbx, NULL
      .endif
    .endif

  .elseif eax == DBG_DEV_WIN_DC
    .if $invoke(DbgOpenWnd)
      mov CDS.dwData, DGB_MSG_ID                        ;Set DebugCenter identifier
      invoke StrSize, pString                           ;String size
      mov dInfo1, eax
      add eax, sizeof(DBG_STR_INFO)                     ;This includes the ZTC
      mov dInfo2, eax
      mov CDS.cbData, eax
      invoke StrSizeW, pDest
      mov dInfo3, eax                                   ;String size
      add eax, sizeof(DBG_HEADER_INFO)
      mov dInfo4, eax
      add CDS.cbData, eax
      invoke GlobalAlloc, GPTR, CDS.cbData
      .if xax != NULL                                   ;Continue only if GlobalAlloc succeeded
        mov CDS.lpData, xax
        mov [xax].DBG_HEADER_INFO.bBlockID, DBG_MSG_HDR ;Set block type = header
        m2m [xax].DBG_HEADER_INFO.dBlockLen, dInfo4, ebx
        lea xcx, [xax + sizeof(DBG_HEADER_INFO)]
        invoke MemClone, xcx, pDest, dInfo3
        mov xax, CDS.lpData
        mov ecx, [xax].DBG_HEADER_INFO.dBlockLen
        add xax, xcx
        mov [xax].DBG_STR_INFO.bBlockID, DBG_MSG_STR    ;Set block type = string
        m2m [xax].DBG_STR_INFO.dBlockLen, dInfo2, ebx
        m2m [xax].DBG_STR_INFO.dForeColor, dForeColor, edx
        m2m [xax].DBG_STR_INFO.dBackColor, dBackColor, ebx
        mov edx, dEffects
        if TARGET_STR_TYPE eq STR_TYPE_WIDE
          BitSet edx, DBG_CHARTYPE_WIDE                 ;Clear if ANSI
        endif
        mov [xax].DBG_STR_INFO.dEffects, edx
        invoke MemClone, addr [xax + sizeof(DBG_STR_INFO)], pString, dInfo1
        invoke SendMessageTimeout, hDbgDev, WM_COPYDATA, -1, addr CDS, \
                                   SMTO_BLOCK, SMTO_TIMEOUT, NULL
        invoke GlobalFree, CDS.lpData
      .else
        invoke MessageBox, 0, offset szDbgComErr, offset szDbgErr, MB_OK or MB_ICONERROR
      .endif
    .endif
  .endif
  ret
ProcName endp
