;----------------------------------------------------------------------------
;	Copyright (C) 1997 S3 Incorporated.
;	All rights reserved.
; 
;	Contents: Virge MX (M3/M5/GX2) Specific Code
;
;	Modification History:
;	$Log:   Q:/softdev/vcs/WIN9x/display/mini/s3_3/virgemx.asv  $
;  
;     Rev 1.7   10 Jul 1997 13:48:46   hlu
;  Combined all two consecutive byte-sized write to MMIO index/data
;  pair to a single word write to get around h/w problem with byte merge.
;  
;     Rev 1.6   03 Jul 1997 19:08:38   hlu
;  SPR #16226 - Never assign SP to IGA2 if DuoView is not enabled.
;  Otherwise, wait for IGA2 v.sync will loop for ever.
;  
;     Rev 1.5   18 Jun 1997 13:17:40   hlu
;  Improve VirgeMXSyncVCount to eliminate endless loop.
;  Modified VirgeMXCfgSPSrc to leave USER_SP_IGA1 intact.
;  
;     Rev 1.4   18 Jun 1997 11:30:10   hlu
;  Added VirgeMXCfgSPSrcFar.
;  
;     Rev 1.3   17 Jun 1997 18:50:20   hlu
;  Added VirgeMXCfgSPSrc to replace old AssignSPSource macro.
;  
;     Rev 1.2   13 Jun 1997 15:08:42   hlu
;  Removed IGA select routines and optimized some code
;  Also force SR26 bit 6 to 1 for ViRGE MX part.
;  
;     Rev 1.1   12 Jun 1997 11:23:28   hlu
;  Added VirgeMXSaveRegIndices and VirgeMXRestRegIndices.
;  
;     Rev 1.0   11 Jun 1997 19:34:06   hlu
;  Initial revision.
;----------------------------------------------------------------------------
        .XLIST
        INCLUDE cmacros.inc
        INCLUDE dibeng.inc
	INCLUDE s3.inc
        .LIST
        .386

;============================================================================
createSeg _INIT,InitSeg,word,public,CODE

;============================================================================
sBegin  Data
        externB S3MobileData
        externW wMMIOSelector
        globalB SavedSRIndex,0
        globalB SavedSR26,0
        globalB SavedCRIndex1,0
        globalB SavedCRIndex2,0
sEnd

;============================================================================
        externFP GetDCLK2               ; in k2v.c
        
;============================================================================
sBegin  InitSeg
        assumes ds,Data
        assumes es,NOTHING
        .386

;----------------------------------------------------------------------------
;
;       VirgeMXSyncVCount - Re-Synchronize vertical counter after
;                           reassigning SP to an IGA
;
;       Entry:  Assume SR unlocked and MMIO is enabled
;
;       Exit:   AL destroyed
;
VirgeMXSyncVCount PROC NEAR PUBLIC uses es
        test    S3MobileData.wSPStyle,SP_M5
        jz      Exit                    ; If not Virge MX, bail out at once!

        mov     ax,wMMIOSelector
        mov     es,ax                   ; Setup MMIO selector
;;
;; Setup special CR39 lock 0A5h for accessing CTR26 (synchronization)
;;
        mov     WORD PTR es:[83d4h],0A539h
;;
;; First find out which IGA our SP has been assigned to by reading SR30_1
;;         
        mov     WORD PTR es:[83c4h],4026h ; Read/write IGA1
        
        mov     al,30h                  ; SR30
        mov     es:[83c4h],al           ; Out index
        mov     al,es:[83c5h]           ; In data
        and     al,02h                  ; Check SP source in SR30_1
        jz      SyncVCWaitLoop1         ; If SP is on IGA1, continue

        mov     WORD PTR es:[83c4h],4F26h ; Read/write IGA2

SyncVCWaitLoop1:
        mov     BYTE PTR es:[83d4h],33h ; CR33
        test    BYTE PTR es:[83d5h],04h ; Are we in vertical retrace period?
        jz      SyncVCWaitLoop1         ; No, keep waiting

SyncVCWaitLoop2:
        mov     BYTE PTR es:[83d4h],33h ; CR33
        test    BYTE PTR es:[83d5h],01h ; Are we in active display period?
        jnz     SyncVCWaitLoop2         ; No, keep waiting
;;
;; We are now at the beginning of the active display start.  It's time
;; to synchronize the vertical counter now.  So let's hit it.
;;
        mov     WORD PTR es:[83d4h],0026h
        mov     WORD PTR es:[83c4h],4026h ; Read/write IGA1
;;
;; Restore to normal CR39 lock 0A0h
;;
        mov     WORD PTR es:[83d4h],0A039h

Exit:
        ret
VirgeMXSyncVCount ENDP

VirgeMXSyncVCountFar PROC FAR PUBLIC
        call    VirgeMXSyncVCount       ; A FAR version of orig. procedure
        ret
VirgeMXSyncVCountFar ENDP

;----------------------------------------------------------------------------
;
;       VirgeMXCfgSPSrc - Configure SP source control bit (SR30_1) and flags
;
;       Entry:  Assume SR unlocked and MMIO is enabled
;
;       Exit:   AL destroyed
;
VirgeMXCfgSPSrc PROC NEAR PUBLIC uses es
        test    S3MobileData.wSPStyle,SP_M5
        jz      CfgSPSrcExit            ; If not Virge MX, bail out at once!
        
        mov     ax,wMMIOSelector
        mov     es,ax                   ; Setup MMIO selector

	mov     BYTE PTR es:[83c4h],30h ; SR30
	mov     al,es:[83c5h]           ; Read org. SR30

	and	al,not SPSRC_IGA2       ; Make IGA1 the default SP source
	or	S3MobileData.dwMobileCaps,SP_IGA1
	and	S3MobileData.dwMobileCaps,not VIDEO_ON_TV
        
	test	S3MobileData.dwMobileCaps,USER_SP_IGA1
	jnz	short CfgSPSource       ; User chose IGA1 for SP, so skip now

        test    S3MobileData.bCurrentDisplay,DUOVIEW
        jz      CfgSPSource
        
        push    ax
        cCall   GetDCLK2                ; Get IGA2 dot clock in AX
        cmp     ax,135                  ; Dot clock >= 135MHz?
        pop     ax                      ;
        jae     CfgSPSource             ; If true, then force SP to IGA1 

	or	al,SPSRC_IGA2           ; User chose IGA2, so set SR30_1=1
	and	S3MobileData.dwMobileCaps,not SP_IGA1
	or	S3MobileData.dwMobileCaps,VIDEO_ON_TV

CfgSPSource:
        mov     ah,al
        mov     al,30h
	mov     WORD PTR es:[83c4h],ax

CfgSPSrcExit:
        ret
VirgeMXCfgSPSrc ENDP

VirgeMXCfgSPSrcFar PROC FAR PUBLIC
        call    VirgeMXCfgSPSrc       ; A FAR version of orig. procedure
        ret
VirgeMXCfgSPSrcFar ENDP

sEnd    InitSeg

;============================================================================
sBegin  Code
        assumes ds,Data
        assumes es,NOTHING

;----------------------------------------------------------------------------
;
;       VirgeMXSaveRegIndices - Save CR, SR index registers and IGA state
;
;       Entry:  CR, SR are unlocked and MMIO is enabled
;
;       Exit:   
;
VirgeMXSaveRegIndices PROC NEAR PUBLIC uses es ds
        mov     ax,DGROUP
        mov     ds,ax                   ; Setup Data segment
        test    S3MobileData.wSPStyle,SP_M5
        jz      SaveExit                ; If not Virge MX, bail out at once!
        mov     ax,wMMIOSelector
        mov     es,ax                   ; Setup MMIO selector
        mov     al,es:[83c4h]           ; Read SR index
        mov     SavedSRIndex,al         ; Save SR index locally
        mov     al,26h                  ; SR26
        mov     es:[83c4h],al           ; 
        mov     al,es:[83c5h]           ; Read SR26
        mov     SavedSR26,al            ; Save SR26 locally
        and     al,0Fh                  ;
        cmp     al,0Fh                  ; Read/Write IGA2 only?
        jne     SaveCRIndices           ; Yes- SR index is correct
        mov     SavedSR26,26h           ; No- SR index must be 26h

SaveCRIndices:
        mov     WORD PTR es:[83c4h],4F26h ; Read/write IGA2
        
        mov     al,es:[83d4h]           ; Read IGA2 CR index
        mov     SavedCRIndex2,al        ; Save CR index locally

        mov     WORD PTR es:[83c4h],4026h ; Read/write IGA1

        mov     al,es:[83d4h]           ; Read IGA1 CR index
        mov     SavedCRIndex1,al        ; Save CR index locally

SaveExit:                               ; Single point of exit
        ret
VirgeMXSaveRegIndices ENDP

;----------------------------------------------------------------------------
;
;       VirgeMXRestRegIndices - Save CR, SR index registers and IGA state
;
;       Entry:  Assume SR are unlocked and MMIO is enabled
;
;       Exit:   AL destroyed
;
VirgeMXRestRegIndices PROC NEAR PUBLIC uses es ds
        mov     ax,DGROUP
        mov     ds,ax                   ; Setup Data segment
        test    S3MobileData.wSPStyle,SP_M5
        jz      RestExit                ; If not Virge MX, bail out at once!
        mov     ax,wMMIOSelector
        mov     es,ax                   ; Setup MMIO selector

        mov     WORD PTR es:[83c4h],4F26h ; Read/write IGA2

        mov     al,SavedCRIndex2        ; Get saved IGA2 CR index
        mov     es:[83d4h],al           ; Restore IGA2 CR index

        mov     WORD PTR es:[83c4h],4026h ; Read/write IGA1
    
        mov     al,SavedCRIndex1        ; Get saved IGA1 CR index
        mov     es:[83d4h],al           ; Restore IGA1 CR index
        
        mov     al,26h                  ; SR26 controls IGA read/write select
        mov     ah,SavedSR26            ; Get saved SR26
        mov     es:[83c4h],ax           ; Restore IGA read/write selection
        
        mov     al,SavedSRIndex         ; Get saved SR index
        mov     es:[83c4h],al           ; Restore SR index
        
RestExit:                               ; Single point of exit
        ret
VirgeMXRestRegIndices ENDP

sEnd    Code

        END



