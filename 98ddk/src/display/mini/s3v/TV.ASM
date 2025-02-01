;----------------------------------------------------------------------------
;	Copyright (C) 1997 S3 Incorporated.
;	All rights reserved.
; 
;	Contents: Virge MX (M3/M5/GX2) TV Specific Code
;
;	Modification History:
;	$Log:   Q:/softdev/vcs/WIN9x/display/mini/s3_3/tv.asv  $
;  
;     Rev 1.22   10 Jul 1997 13:46:36   hlu
;  Combined all two consecutive byte-sized writes to MMIO index/data
;  pair to a single word write to get around h/w problem with byte merge.
;  
;     Rev 1.21   07 Jul 1997 18:05:40   hlu
;  Aligned data in data segment to DWORD for improving performance,
;  especially for Direct3D polygon throughput test (a mere 7%).
;  
;     Rev 1.20   03 Jul 1997 19:00:18   hlu
;  Changed TV_Y_POS_MAX from 24 to 12.
;  
;     Rev 1.19   03 Jul 1997 10:25:42   hlu
;  Fixed an error in TVWriteVsync where CR07 bit 8 may be corrupted.
;  
;     Rev 1.18   02 Jul 1997 11:35:18   hlu
;  Removed unused code TVLoadPosition and TVSavePosition since
;  one table TVPos is enough for both NTSC and PAL.
;  
;     Rev 1.17   02 Jul 1997 10:57:44   hlu
;  Added mobile family check in TVWriteVSync and added CR35 unlock
;  sequence before writing v.sync.  Also added check for zero changes
;  in x, y position and skip code if no changes in x, y position.
;  
;     Rev 1.16   02 Jul 1997 09:38:48   hlu
;  Optimized some code and added additional comments.
;  
;     Rev 1.15   01 Jul 1997 21:59:22   hlu
;  Modified TVSetXPosition to take care of extension bits of various
;  TV parameters. Modified TVReadVSync to differentiate GX2 from M5
;  when TV is assigned to IGA1 - GX2 read v.sync from CR and M5 read
;  v.sync from SR.
;  
;     Rev 1.14   01 Jul 1997 18:14:06   hlu
;  Redesigned the TV code from the ground up.  Everything is re-written.
;  Nothing is what it was before.  Improved the coding style dramatically.
;----------------------------------------------------------------------------
        .XLIST
        INCLUDE cmacros.inc
        INCLUDE dibeng.inc
	INCLUDE s3.inc
        .LIST
        .386


TVPosS  STRUC   1
  XPos  DB      ?                       ; X position
  YPos  DB      ?                       ; Y position
  XMax  DB      ?                       ; X position range
  YMax  DB      ?                       ; Y position range
TVPosS  ENDS

TV_X_POS_MAX    EQU     32
TV_Y_POS_MAX    EQU     12

;============================================================================
createSeg _INIT,InitSeg,word,public,CODE


;=============================== NEW TV STUFF ===============================
sBegin	Data

        externB S3MobileData
        externB bFamilyFlags
        externW wMMIOSelector

        ALIGN   4
                
        globalB TV_SR70,19h
        globalB TV_SR71,03h
        
TVPos           TVPosS <0FFh,0FFh,TV_X_POS_MAX,TV_Y_POS_MAX>

TVParam0        DW      0
TVParam1        DW      0
TVParam2        DW      0
TVParam3        DW      0
TVParam4        DW      0
TVParam5        DW      0
TVParam6        DW      0
TVParam7        DW      0
TVParam8        DW      0
TVParam9        DW      0

sEnd

;============================================================================
sBegin	InitSeg
        assumes cs,InitSeg
        assumes ds,Data
	.386
        
;----------------------------------------------------------------------------
;
;       TVGetXPosition - Get current X position of TV
;
;       Entry:  Assume SR is unlocked and MMIO is enabled
;
;       Exit:   AL - Current TV X position
;               AH - Max TV X position
;               CF - C (Success), NC (Failure)
;
TVGetXPosition PROC FAR PUBLIC uses si
        test    S3MobileData.wSPStyle,SP_M5
        stc
        jz      TVGetXExit              ; If not Virge MX, bail out at once!

        mov     ah,TV_X_POS_MAX         ; Get X position range
        mov     si,OFFSET TVPos         ;
        mov     al,[si].TVPosS.XPos     ; Read current TV X position
        cmp     al,0FFh                 ; Has X position been initialized?
        jne     TVGetXExit              ; Yes- exit and AL has X position

        mov     al,TV_X_POS_MAX/2       ; Assume middle pos after mode set
        mov     [si].TVPosS.XPos,al     ; Initial Y position
        clc
        
TVGetXExit:        
        ret
TVGetXPosition ENDP

;----------------------------------------------------------------------------
;
;       TVSetXPosition - Set current X position of TV
;
;       Entry:  Assume SR is unlocked and MMIO is enabled
;               AL - TV X position to be set
;
;       Exit:   AX destroyed
;
TVSetXPosition PROC FAR PUBLIC uses es si bx
        test    S3MobileData.wSPStyle,SP_M5
        jz      TVSetXExit              ; If not Virge MX, bail out at once!

        mov     si,OFFSET TVPos
        sub     al,[si].TVPosS.XPos
        jz      TVSetXExit              ; If no change in X pos, exit
        
        add     [si].TVPosS.XPos,al     ; Update current TV X position
        push    ax                      ; Save new X position
        mov     ax,wMMIOSelector        ;
        mov     es,ax                   ; Setup MMIO selector

        ;
        ; Read TV Parameters
        ;
        mov     BYTE PTR es:[83C4h],90h ; SR90 = TV param 0
        mov     al,es:[83C5h]
        mov     BYTE PTR es:[83C4h],9Bh
        mov     ah,es:[83C5h]
        and     ah,07h
        mov     TVParam0,ax

        mov     BYTE PTR es:[83C4h],91h ; SR91 = TV param 1
        mov     al,es:[83C5h]
        mov     BYTE PTR es:[83C4h],9Bh
        mov     ah,es:[83C5h]
        and     ah,70h
        shr     ah,4
        mov     TVParam1,ax

        mov     BYTE PTR es:[83C4h],92h ; SR92 = TV param 2
        mov     al,es:[83C5h]
        mov     BYTE PTR es:[83C4h],9Ch
        mov     ah,es:[83C5h]
        and     ah,07h
        mov     TVParam2,ax

        mov     BYTE PTR es:[83C4h],93h ; SR93 = TV param 3
        mov     al,es:[83C5h]
        mov     BYTE PTR es:[83C4h],9Ch
        mov     ah,es:[83C5h]
        and     ah,70h
        shr     ah,4
        mov     TVParam3,ax

        mov     BYTE PTR es:[83C4h],94h ; SR94 = TV param 4
        mov     al,es:[83C5h]
        mov     BYTE PTR es:[83C4h],9Dh
        mov     ah,es:[83C5h]
        and     ah,07h
        mov     TVParam4,ax

        mov     BYTE PTR es:[83C4h],95h ; SR95 = TV param 5
        mov     al,es:[83C5h]
        mov     BYTE PTR es:[83C4h],9Dh
        mov     ah,es:[83C5h]
        and     ah,70h
        shr     ah,4
        mov     TVParam5,ax

        mov     BYTE PTR es:[83C4h],96h ; SR96 = TV param 6
        mov     al,es:[83C5h]
        mov     BYTE PTR es:[83C4h],9Eh
        mov     ah,es:[83C5h]
        and     ah,03h
        mov     TVParam6,ax

        mov     BYTE PTR es:[83C4h],97h ; SR97 = TV param 7
        mov     al,es:[83C5h]
        mov     BYTE PTR es:[83C4h],9Eh
        mov     ah,es:[83C5h]
        and     ah,0Ch
        shr     ah,2
        mov     TVParam7,ax

        mov     BYTE PTR es:[83C4h],98h ; SR98 = TV param 8
        mov     al,es:[83C5h]
        mov     BYTE PTR es:[83C4h],9Eh
        mov     ah,es:[83C5h]
        and     ah,30h
        shr     ah,4
        mov     TVParam8,ax

        mov     BYTE PTR es:[83C4h],99h ; SR99 = TV param 9
        mov     al,es:[83C5h]
        mov     BYTE PTR es:[83C4h],9Fh
        mov     ah,es:[83C5h]
        and     ah,07h
        mov     TVParam9,ax

        ;
        ; Re-calculate TV Parameters
        ;
        pop     ax
        movsx   ax,al
        sub     TVParam0,ax
        sub     TVParam1,ax
        sub     TVParam2,ax
        sub     TVParam3,ax
        sub     TVParam4,ax
        sub     TVParam5,ax
        sub     TVParam6,ax
        sub     TVParam7,ax
        sub     TVParam8,ax
        sub     TVParam9,ax

        ;
        ; Write TV Parameters
        ;
        mov     ax,TVParam0             
        mov     bl,90h                  ; SR90 = TV param 0
        mov     bh,al
        mov     es:[83C4h],bx
        and     ah,07h
        mov     BYTE PTR es:[83C4h],9Bh
        and     BYTE PTR es:[83C5h],0F8h
        or      es:[83C5h],ah

        mov     ax,TVParam1             
        mov     bl,91h                  ; SR91 = TV param 1
        mov     bh,al
        mov     es:[83C4h],bx
        and     ah,07h
        shl     ah,4
        mov     BYTE PTR es:[83C4h],9Bh
        and     BYTE PTR es:[83C5h],08Fh
        or      es:[83C5h],ah

        mov     ax,TVParam2
        mov     bl,92h                  ; SR92 = TV param 2
        mov     bh,al
        mov     es:[83C4h],bx
        and     ah,07h
        mov     BYTE PTR es:[83C4h],9Ch
        and     BYTE PTR es:[83C5h],0F8h
        or      es:[83C5h],ah

        mov     ax,TVParam3
        mov     bl,93h                  ; SR93 = TV param 3
        mov     bh,al
        mov     es:[83C4h],bx
        and     ah,07h
        shl     ah,4
        mov     BYTE PTR es:[83C4h],9Ch
        and     BYTE PTR es:[83C5h],08Fh
        or      es:[83C5h],ah

        mov     ax,TVParam4
        mov     bl,94h                  ; SR94 = TV param 4
        mov     bh,al
        mov     es:[83C4h],bx
        and     ah,07h
        mov     BYTE PTR es:[83C4h],9Dh
        and     BYTE PTR es:[83C5h],0F8h
        or      es:[83C5h],ah

        mov     ax,TVParam5
        mov     bl,95h                  ; SR95 = TV param 5
        mov     bh,al
        mov     es:[83C4h],bx
        and     ah,07h
        shl     ah,4
        mov     BYTE PTR es:[83C4h],9Dh
        and     BYTE PTR es:[83C5h],08Fh
        or      es:[83C5h],ah

        mov     ax,TVParam6
        mov     bl,96h                  ; SR96 = TV param 6
        mov     bh,al
        mov     es:[83C4h],bx
        and     ah,03h
        mov     BYTE PTR es:[83C4h],9Eh
        and     BYTE PTR es:[83C5h],0FCh
        or      es:[83C5h],ah

        mov     ax,TVParam7
        mov     bl,97h                  ; SR97 = TV param 7
        mov     bh,al
        mov     es:[83C4h],bx
        and     ah,03h
        shl     ah,2
        mov     BYTE PTR es:[83C4h],9Eh
        and     BYTE PTR es:[83C5h],0F3h
        or      es:[83C5h],ah

        mov     ax,TVParam8
        mov     bl,98h                  ; SR98 = TV param 8
        mov     bh,al
        mov     es:[83C4h],bx
        and     ah,03h
        shl     ah,4
        mov     BYTE PTR es:[83C4h],9Eh
        and     BYTE PTR es:[83C5h],0CFh
        or      es:[83C5h],ah

        mov     ax,TVParam9
        mov     bl,99h                  ; SR99 = TV param 9
        mov     bh,al
        mov     es:[83C4h],bx
        and     ah,07h
        mov     BYTE PTR es:[83C4h],9Fh
        and     BYTE PTR es:[83C5h],0F8h
        or      es:[83C5h],ah

TVSetXExit:                
        ret
TVSetXPosition ENDP

;----------------------------------------------------------------------------
;
;       TVGetYPosition - Get current Y position of TV
;
;       Entry:  Assume SR is unlocked and MMIO is enabled
;
;       Exit:   AL - Current TV Y position
;               AH - Max TV Y position
;               CF - C (Success), NC (Failure)
;
TVGetYPosition PROC FAR PUBLIC uses si
        test    S3MobileData.wSPStyle,SP_M5
        stc
        jz      TVGetYExit              ; If not Virge MX, bail out at once!

        mov     ah,TV_Y_POS_MAX         ; Get Y position range
        mov     si,OFFSET TVPos         ;
        mov     al,[si].TVPosS.YPos     ; Read current TV Y position
        cmp     al,0FFh                 ; Has Y position been initialized?
        jne     TVGetYExit              ; Yes- exit and AL has Y position

        mov     al,TV_Y_POS_MAX/2       ; Assume middle pos after mode set
        mov     [si].TVPosS.YPos,al     ; Initial Y position
        clc
        
TVGetYExit:
        ret
TVGetYPosition ENDP

;----------------------------------------------------------------------------
;
;       TVSetYPosition - Set current Y position of TV
;
;       Entry:  Assume SR is unlocked and MMIO is enabled
;               AL - TV X position to be set
;
;       Exit:   AX destroyed
;
TVSetYPosition PROC FAR PUBLIC uses si bx
        test    S3MobileData.wSPStyle,SP_M5
        jz      TVSetYExit              ; If not Virge MX, bail out at once!

        mov     si,OFFSET TVPos
        sub     al,[si].TVPosS.YPos     ; Calculate incremental Y pos change
        jz      TVSetYExit              ; If no change in TV pos, exit
        
        add     [si].TVPosS.YPos,al     ; Save new position
        movsx   bx,al                   ;
        call    TVReadVSync             ; Get current v.sync
        add     ax,bx                   ; Add the incremental change
        call    TVWriteVSync            ; Write it out
        
TVSetYExit:
        ret
TVSetYPosition ENDP

;----------------------------------------------------------------------------
;
;       TVReadVSync - Read TV vertical sync start position
;
;       Entry:  AssumeV SR is unlocked and MMIO is enabled
;
;       Exit:   AX - Vertical Retrace Start Position on TV
;
TVReadVSync PROC NEAR uses es bx
        mov     ax,wMMIOSelector
        mov     es,ax                   ; Setup MMIO selector

        xor     bx,bx
        mov     BYTE PTR es:[83C4h],30h ; SR30
        test    BYTE PTR es:[83C5h],04h ; TV on IGA2?
        jnz     ReadVSyncIGA2           ; Yes- read controller 2

        mov     WORD PTR es:[83C4h],4026h ; Read/write IGA1
        
        test    bFamilyFlags,MOBILE_FAMILY
        jz      ReadVSyncCR             ; Non-mobile family always uses CR
        
        mov     BYTE PTR es:[83C4h],6Ch ; SR6C
        mov     al,es:[83C5h]           ; Read vertical retrace start
        mov     bl,al                   ;
        
        mov     BYTE PTR es:[83C4h],6Fh ; SR6F
        mov     al,es:[83C5h]           ; Read extended bits
        and     al,07h                  ; Mask out needed bits
        mov     bh,al                   ;
        jmp     TVReadExit
        
ReadVSyncIGA2:
        mov     WORD PTR es:[83C4h],4F26h ; Read/write IGA2
        
ReadVSyncCR:
        mov     BYTE PTR es:[83D4h],10h ; CR10
        mov     al,es:[83D5h]           ; Read vertical retrace start
        mov     bl,al
        
        mov     BYTE PTR es:[83D4h],07h ; CR07
        mov     al,es:[83D5h]           ; Read extended bits
        mov     ah,al                   ;
        and     al,04h                  ; Mask out bit 8
        shr     al,2                    ; Shift bit 8 in place
        or      bh,al                   ;
        and     ah,80h                  ; Mask out bit 8
        shr     ah,6                    ; Shift bit 8 in place
        or      bh,ah                   ;

        mov     BYTE PTR es:[83D4h],5Eh ; CR5E
        mov     al,es:[83D5h]           ; Read extended bits
        and     al,10h                  ; Mask out VRS bit
        shr     al,2                    ;
        or      bh,al                   ;

        mov     WORD PTR es:[83C4h],4026h ; Read/write IGA1
        
TVReadExit:
        mov     ax,bx
        ret
TVReadVSync ENDP

;----------------------------------------------------------------------------
;
;       TVWriteVSync - Write TV vertical sync start position
;
;       Entry:  AssumeV SR is unlocked and MMIO is enabled
;               AX - Vertical Retrace Start Position on TV
;
;       Exit:   AX destoryed
;
TVWriteVSync PROC NEAR uses es bx
        mov     bx,ax                   ; Save v.sync in BX
        mov     ax,wMMIOSelector        ;
        mov     es,ax                   ; Setup MMIO selector

        mov     BYTE PTR es:[83C4h],30h ; SR30
        test    BYTE PTR es:[83C5h],04h ; TV on IGA2?
        jnz     WriteVSyncIGA2

        mov     WORD PTR es:[83C4h],4026h ; Read/write IGA1
        
        test    bFamilyFlags,MOBILE_FAMILY
        jz      WriteVSyncCR             ; Non-mobile family always uses CR

        mov     al,6Ch                  ; SR6C
        mov     ah,bl                   ; Loads lower byte of v.sync
        mov     es:[83C4h],ax           ; Write vertical retrace start
        
        mov     BYTE PTR es:[83C4h],6Fh ; SR6F
        and     BYTE PTR es:[83C5h],0F8h; Write extended bits
        or      es:[83C5h],bh           ; Mask out needed bits
        jmp     TVWriteExit
        
WriteVSyncIGA2:
        mov     WORD PTR es:[83C4h],4F26h ; Read/write IGA2
        
WriteVSyncCR:
        mov     al,35h                  ; CR35
        mov     ah,es:[83D5h]           ;
        push    ax                      ; Save current CR35 lock status
        mov     BYTE PTR es:[83D5h],00h ; Force unlock

        mov     al,10h                  ; CR10
        mov     ah,bl                   ; Loads lower byte of v.sync
        mov     es:[83D4h],ax           ; Write vertical retrace start

        mov     BYTE PTR es:[83D4h],07h ; CR07
        mov     ah,bh                   ;
        and     BYTE PTR es:[83D5h],0FBh;
        and     ah,01h                  ;
        shl     ah,2                    ;
        or      es:[83D5h],ah           ; Write extended bits
        mov     ah,bh                   ;
        and     BYTE PTR es:[83D5h],07Fh;
        and     ah,02h                  ;
        shl     ah,6                    ;
        or      es:[83D5h],ah           ; Write extended bits
        
        mov     ah,bh                   ;
        mov     BYTE PTR es:[83D4h],5Eh ; CR5E
        and     BYTE PTR es:[83D5h],0EFh;
        and     ah,04h                  ;
        shl     ah,2                    ;
        or      es:[83D5h],ah           ; Write extended bits

        pop     ax                      ; Restore CR35 lock status
        mov     es:[83D4h],ax           ; Restore CR35

        mov     WORD PTR es:[83C4h],4026h ; Read/write IGA1
        
TVWriteExit:
        ret
TVWriteVSync ENDP

;----------------------------------------------------------------------------
;
;       TVInitPosition - Initialize TV position after each set mode and
;                        display switch using the TV position set before.
;
;       Entry:  Assume SR is unlocked and MMIO is enabled
;
;       Exit:   AX destroyed
;
TVInitPosition PROC FAR PUBLIC uses si
        test    S3MobileData.wSPStyle,SP_M5
        jz      TVInitExit              ; If not Virge MX, bail out at once!

        push    ax
        mov     ax,wMMIOSelector
        mov     es,ax                   ; Setup MMIO selector
        pop     ax

        mov     BYTE PTR es:[83D4h],6Bh ;
        test    BYTE PTR es:[83D5h],TV_ONLY
        jz      TVInitExit              ; If TV not active, skip code below

        mov     si,OFFSET TVPos
        call    TVGetXPosition          ; Get previously set X position
        push    ax                      ; Save it
        call    TVGetYPosition          ; Get previously set Y position
        push    ax                      ; Save it
        
        mov     [si].TVPosS.XPos,0FFh   ; Force X position re-init
        call    TVGetXPosition          ; Re-initialize X position
        mov     [si].TVPosS.YPos,0FFh   ; Force Y position re-init
        call    TVGetYPosition          ; Re-initialize Y position

        pop     ax                      ; Restore previously set X position
        call    TVSetYPosition          ; Update new X position
        pop     ax                      ; Restore previously set Y position
        call    TVSetXPosition          ; Update new X position

TVInitExit:
        ret
TVInitPosition ENDP

sEnd

        END
