    page    , 132
;---------------------------------------------------------------------------;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1998  Microsoft Corporation.  All Rights Reserved.
;
;   File:   poll.asm
;   Content:    Polls any combination of the 4 axes at the same time
;
;---------------------------------------------------------------------------;

;
; We poll the analog ports that we are interested in twice, once to
; make sure that the monostable vibrators have stabilized since the
; last time they were polled and once to actually read the positions.
; The first poll is almost always a single pass (ie, things are stable)
; but this one pass also helps ensure that the entire loop is in cache.
;

;
; When reading joystick port, the bit for a pot starts at 1, and when
; the resistance value for a counter is greater than or equal to the
; pot's, the bit goes to zero, and stays there.
;
    
    .386p

    .xlist
    include vmm.inc
    include poll.inc
    .list

extern _dwTSCScale:DWORD
extern _dwTickTimeout:DWORD
extern _dwReadLimit:DWORD
extern _dwReadAvg:DWORD
extern _fpPoll:DWORD

VxD_PAGEABLE_DATA_SEG

IFNDEF RETAIL
    dwDbg1              dd 0
    dwDbg2              dd 0

    TimeoutMsg          db "DIGIJOY: Timed out",13,10,0
    AverageReadMsg      db "DIGIJOY: using averaged read delta of #ax",13,10,0
    CorrectedReadMsg    db "DIGIJOY: using corrected read delta due to average of #ax",13,10,0
ENDIF
VxD_PAGEABLE_DATA_ENDS

VxD_PAGEABLE_CODE_SEG
public _jsPoll@20
_jsPoll@20 PROC

; The stack structure used in this file is like so:
;
;   <params>    timeout             36                      172
;               ptr axis data       32                      168
;               ptr button data     28                      164
;               port                24                      160
;               mask                20                      156
;   <calling>   return address      16                      152
;               ebp                 12                      148
;               ebx                  8                      144
;               esi                  4                      140
;               edi                 esp after reg store     136
;   <workspace> Buffer[1][15]                               132
;                   .
;                   .
;                   .
;               Buffer[1][00]                                72
;               Buffer[0][15]                                68
;                   .
;                   .
;                   .
;               Buffer[0][00]       ebp during poll           8
;   <int status>flags                                         4
;   <calling>   return address                              esp during poll

;
; get the button state
;
    
    push    ebp                 ; Could use pushad/popad but the extra 
    push    ebx                 ; work to keep the return code in eax
    push    esi                 ; makes it less appealling
    push    edi

    mov     edx,[esp+24]        ; port address
    mov     ebx,[esp+28]        ; pdwButtons
    in      al, dx              ; get button bits
    and     eax,0f0h            ; isolate bits we want
    xor     eax,0f0h            ; invert sense (port gives reverse logic)
    shr     al, 4               ; shift into place
    mov     [ebx],eax           ; save it

    sub     esp,128
    mov     ebp,esp             ; create a 32 DWORD non-paged buffer

    ; The following is done twice
    ;   Clear the buffer
    ;   Set up the polling parameters
    ;   Sample the axes until the ones we care about are zero
    ;
    ; The first pass makes sure that the axes are settled from any previous activity
    ; and makes sure everything is paged in and (supposedly) cached
    mov     esi,ebp
    mov     eax,[esp+164]       ; the scaled timeout value
    mov     ecx,31              ; wipe whole buffer
    lea     edi,[esi+4]
    mov     [esi],eax

    rep movsd

    mov     edi,eax             ; timeout

    mov     esi,[esp+148]       ; axis mask
    test    esi,0fh             ; see if any axes were requested
    jz      jsPoll_exit         ; if not leave, assume timeout non-zero

    pushfd                      ; Want to be able to restore interrupt
                                ; status ASAP so pushed last

    call    [_fpPoll]
    jnz     timeout_exit

    ; Clear the buffer again in case we trashed stuff
    mov     ebx,[esp+168]       ; timeout value
    mov     esi,ebp
    mov     ecx,31              ; wipe whole buffer
    lea     edi,[esi+4]
    mov     [esi],ebx

    rep movsd

    mov     esi,[esp+152]       ; axis mask
    mov     edi,ebx

    ; Start the pots
    out     dx,al
    call    [_fpPoll]
    jnz     timeout_exit

    popfd                       ; get the interrupt status back

;   Walk through the data filled in by the poll routines, picking out the
;   before and after transition times for each axis.
;   For polls which did not collect after transition times, all times
;   result in a zero.
;
;   Registers are used as follows:
;   Constants:
;       EBP =  pointer to 32 DWORD buffer
;       EDI =  timeout value
;       ESI =  axis mask
;       EBX =  output ptr
;
;   Variables:
;       EAX,smallest so far
;       EDX,temp bit mask
;       ECX,index

    mov     ebx,[esp+160]       ; pointer to axis array
    mov     edi,[esp+164]       ; the scaled timeout value

    xor     edx,edx
    mov     ecx,7
@@:
    mov     [ebx+ecx*4],edx
    dec cl
    jns @b

; for each axis bit, find the smallest value and subtract it from _dwTickTimeout

; X, bit 0
    mov     edx,1
    test    esi,edx
    jz      find_Y
    
    mov     ecx,esi             ; Only indices we were watching could be set
    mov     eax,edi
find_X_loop:
    cmp     eax,[ebp+ecx*4]
    jl      @f
    mov     eax,[ebp+ecx*4+64]  ; copy the after transition value
    mov     [ebx+16],eax        ; to the output buffer
    mov     eax,[ebp+ecx*4]     ; take copy of smallest
@@:
    sub     cl,2
    jns     find_X_loop

    sub     eax,edi
    neg     eax
    mov     [ebx],eax           ; set actual time taken

    mov     eax,[ebx+16]        ; get after transition back
    sub     eax,edi             ; convert it to a time taken
    neg     eax
    mov     [ebx+16],eax        ; set actual time taken
        
find_Y:
    shl     dl,1
    test    esi,edx
    jz      find_R
    
    xor     ecx,ecx
    mov     eax,edi
find_Y_loop:
    or      cl,dl
    cmp     eax,[ebp+ecx*4]
    jl      @f
    mov     eax,[ebp+ecx*4+64]  ; copy the after transition value
    mov     [ebx+20],eax        ; to the output buffer
    mov     eax,[ebp+ecx*4]     ; take copy of smallest
@@:
    inc     cl
    test    cl,10h
    jz      find_Y_loop

    sub     eax,edi
    neg     eax
    mov     [ebx+4],eax         ; set actual time taken

    mov     eax,[ebx+20]        ; get after transition back
    sub     eax,edi             ; convert it to a time taken
    neg     eax
    mov     [ebx+20],eax        ; set actual time taken

        
find_R:
    shl     dl,1
    test    esi,edx
    jz      find_Z
    
    xor     ecx,ecx
    mov     eax,edi
find_R_loop:
    or      cl,dl
    cmp     eax,[ebp+ecx*4]
    jl      @f
    mov     eax,[ebp+ecx*4+64]  ; copy the after transition value
    mov     [ebx+24],eax        ; to the output buffer
    mov     eax,[ebp+ecx*4]     ; take copy of smallest
@@:
    inc     cl
    test    cl,10h
    jz      find_R_loop

    sub     eax,edi
    neg     eax
    mov     [ebx+8],eax         ; set actual time taken

    mov     eax,[ebx+24]        ; get after transition back
    sub     eax,edi             ; convert it to a time taken
    neg     eax
    mov     [ebx+24],eax        ; set actual time taken

        
find_Z:
    shl     dl,1
    test    esi,edx
    jz      found_all
    
    ;last one, so all the possible bits are contiguous
    mov     eax,edi
find_Z_loop:
    cmp     eax,[ebp+edx*4]
    jl      @f
    mov     eax,[ebp+edx*4]
    mov     ecx,[ebp+edx*4+64]
@@:
    inc     dl
    test    dl,10h
    jz      find_Z_loop

    sub     eax,edi
    sub     ecx,edi
    neg     eax
    neg     ecx
    mov     [ebx+12],eax
    mov     [ebx+28],ecx
    
found_all:
    mov     eax,ebx             ; ebx still points to the axes so not zero

jsPoll_exit:
    add     esp,128

    pop     edi
    pop     esi
    pop     ebx
    pop     ebp
    ret     20

timeout_exit:
    DPF     TimeoutMsg
    xor     eax,eax
    popfd
    jmp jsPoll_exit

_jsPoll@20 ENDP
VxD_PAGEABLE_CODE_ENDS

VxD_LOCKED_CODE_SEG
;******************************************************************************
;
;   _(D/E)SWPoll
;
;   This is the absolute fallback polling.
;   It uses the same software loop count method as the original pre-DX3 VJoyD 
;   so the the values it returns are dependant on the CPU speed, it will slow 
;   down during DMA and if interrupts are not disabled, the result will be
;   inaccurate but it will not be detectable at this level.
;   There are two entry points, DSWPoll and ESWPoll, the former disables
;   interrupts before dropping into the latter.  
;   It is used for sub-pentium class processors when the system clock polling
;   initialization fails.  Since the faalback is silent, nobody knows if
;   this is ever actually used in any end-customer systems.
;
;   Entry:
;       EBP =  pointer to 32 DWORD buffer (only first 16 used) (not modified)
;       ESI =  axis mask (not modified)
;       EDI =  timeout
;
;   Returns:
;       NZ & EAX = non-zero for error
;
;   Uses:
;       EAX,general
;       EDX,port
;       EBX,unused
;       ECX,unused
;       EDI,counter
;
;------------------------------------------------------------------------------
public _DSWPoll
_DSWPoll PROC

    cli

public _ESWPoll
_ESWPoll:
    mov     edx,[esp+160]

dsw_poll_loop:
    dec     edi
    js      dsw_timeout

    in      al,dx
    and     eax,esi
    mov     [ebp+eax*4],edi
    jnz     dsw_poll_loop

    ;eax == zero, ZR == true
    ret
dsw_timeout:
    mov     eax,esi
    ret

_DSWPoll ENDP
VxD_LOCKED_CODE_ENDS

VxD_PAGEABLE_CODE_SEG
;******************************************************************************
;
;   _ERTCPoll
;
;   This is the interrupt enabled version of the sub-pentium polling.
;   Two sets of values are produced such that an a reading is available at 
;   all touched indices for the before transition time in on set and the after
;   time in the other such that the time taken to change can be found and thus
;   untimely interrupts can be detected.
;
;   Entry:
;       EBP =  pointer to 32 DWORD buffer (not modified)
;       ESI =  axis mask (not modified)
;       EDI =  timeout
;
;   Returns:
;       NZ & EAX = non-zero for error
;
;   Uses:
;       EAX,general
;       EDX,general
;       EBX,previous index
;       ECX,last timer
;       EDI,counter
;
;------------------------------------------------------------------------------
public _ERTCPoll
_ERTCPoll PROC

    pushfd
    cli
    Read_Sys_Clock
    popfd
    mov     ecx,eax             ;Init last to now
    mov     ebx,esi             ;Init last index to all interesting bits
    
    mov eax,esi
    shl eax,24
    or eax,_dwReadAvg           ; initialize a 3 DWORD history
    push eax                    ; to all interesting bits
    push eax                    ; with a default delta
    push eax                    ; (last one is lost immediately)

ertc_poll_loop:
    mov     edx,[esp+4]
    mov     eax,[esp]
    mov     [esp+8],edx
    mov     [esp+4],eax         ; move the history

    pushfd
    cli
    Read_Sys_Clock
    popfd

    xchg    eax,ecx             ; Save latest to 
    sub     eax,ecx             ; get delta
    
    mov     [esp],eax           ; Add to history

    mov     edx,[esp+172]       ; gameport address
    in      al,dx               ; get position info
    and     eax,esi             ; only watch bits of interest

    mov     [esp+3],al          ; store in hi byte of current item

    mov     eax,[esp]
    mov     ebx,[esp+4]

    and     eax,03ffh
    and     ebx,03ffh

    cmp     ebx,_dwReadLimit    ; More error than a DMA hit?
    jbe     ertc_use_delta      ; N: OK

    cmp     eax,_dwReadLimit    ; More error than a DMA hit?
    jbe     ertc_use_delta      ; N: We probably just took an interrupt
                                ;    hit, so we must live with it

    add     edi,ebx             ; remove previous delta
    add     ebx,eax
    and     ebx,03ffh           ; get combined delta
    shr     ebx,1               ; get average delta

    cmp     ebx,_dwReadLimit    ; Still more error than a DMA hit?
    jbe     ertc_use_corrected_delta    ; N: use this

IFDEF DEBUG
    mov     dwDbg1,ebx
    mov     dwDbg2,1
ENDIF
    mov     ebx,_dwReadAvg      ; Give up trying to be accurate
                                ; If we've really taken interrupt hits
                                ; on successize iterations, we're going
                                ; to be toasted anyway.
                                ; More likely the timer is lying to us
                                ; so use an average
IFDEF DEBUG
    mov     dwDbg2,2
ENDIF

ertc_use_corrected_delta:
    xor     eax,eax

    mov     al,[esp+11]         ; get index for previous before transition
    mov     [ebp+eax*4+64],edi  ; repair

    sub     edi,ebx             ; get average value
    mov     al,[esp+7]          ; get index for current before transition
                                ; and previous after transition
    mov     [ebp+eax*4+64],edi  ; set
    mov     [ebp+eax*4],edi     ; repair

    sub     edi,ebx             ; get to the final value
    js      ertc_timeout        ; If the counter went negative we timed out
    mov     bl,al               ; save the axes to decide whether or not to quit
    mov     al,[esp+3]          ; get index for current after transition
    mov     [ebp+eax*4],edi     ; set

    test    bl,bl
    jz      ertc_poll_done      ; counters hit zero, exit
    jmp     ertc_poll_loop

ertc_use_delta:
    sub     edi,ebx
    js      ertc_timeout        ; If the counter went negative we timed out
    xor     eax,eax
    xor     ebx,ebx
    mov     al,[esp+3]
    mov     bl,[esp+7]
    mov     [ebp+eax*4+64],edi  ; set before transition
    mov     [ebp+ebx*4],edi     ; set after transition
    
    test    bl,bl
    jnz     ertc_poll_loop      ; counters hit zero, exit

ertc_timeout:
ertc_poll_done:
IFDEF DEBUG
    mov     eax,dwDbg1
    test    dwDbg2,1
    jz @f
    DPF     CorrectedReadMsg
    jmp     remove_history
@@:
    test    dwDbg2,2
    jz remove_history
    DPF     AverageReadMsg

remove_history:
    mov     dwDbg2,0
ENDIF

    add     esp,12              ; get rid of history buffer
    xor     eax,eax
    mov     al,bl
    test    eax,eax             ; set flags and eax for result
    ;if OK, eax == zero, ZR == true
    ;else,  eax == delta, NZ == true
    ret

_ERTCPoll ENDP
VxD_PAGEABLE_CODE_ENDS

VxD_LOCKED_CODE_SEG
;******************************************************************************
;
;   _DRTCPoll
;
;   This is the interrupt disabled version of the sub-pentium polling.
;   It is essentially the same as the DX3 sub-pentium polling.
;
;   The system timer counts from its initial value down to ONE. At the point
;   it would reach zero the initial value is reloaded.  The VTD always uses
;   exact powers of two ranging from 1024 to 65536 (the later being set
;   as 0).  The bottom 10 bits are therefore always in a contiguous cycle.
;   There appear to be two problems reading the timer (this may be only on
;   some machines):
;   i)    On random occations, the lowest bits (usually four) are set to
;         zero and then on the following read, the count is back to normal.
;   ii)   On random occations, the counter will start counting at a random
;         value, in the range of the initial value.
;
;   Entry:
;       EBP =  pointer to 32 DWORD buffer (only first 16 used) (not modified)
;       ESI =  axis mask (not modified)
;       EDI =  timeout
;
;   Returns:
;       NZ & EAX = non-zero for error
;
;   Uses:
;       EAX,general
;       EDX,general
;       EBX,previous index
;       ECX,last timer
;       EDI,counter
;
;------------------------------------------------------------------------------
public _DRTCPoll
_DRTCPoll PROC

    Read_Sys_Clock
    mov     ecx,eax             ;Init last to now
    
drtc_poll_loop:
    mov     edx,[esp+160]
    in      al,dx               ; get position info
    and     eax,esi             ; only watch bits of interest

    mov     ebx,eax             ; save index in case we have to undo it
    mov     [ebp+eax*4],edi

    jz      drtc_poll_done      ; counters hit zero, exit

    Read_Sys_Clock

    xchg    eax,ecx             ; Save latest to 
    sub     eax,ecx
    
    and     eax,03ffh           ; Mask out bits which could be overflow
    cmp     eax,_dwReadLimit    ; More error than a DMA hit?
    jbe     drtc_use_delta      ; N: OK

    ; if the error is small and negative modulo 1024 we probably just mislaid some
    ; of the lower bits on the PREVIOUS loop
    sub eax,400h                ; get 32 bit negative
    neg eax                     ; get absolute delta
    cmp eax,_dwReadLimit        ; is the error small?
    ja drtc_random_clock        ; no, assume random

    ; assume the latest value is correct add the negative modulo 1024 delta to get
    ; the real elapsed ticks then go back and correct the stored value.
    add     edi,eax             ; Apply negative delta
 
    mov     [ebp+ebx*4],edi     ; repair previous value with current

    jns     drtc_poll_loop      ; If the counter went negative we timed out
    jmp     drtc_timeout

drtc_random_clock:
    ; if the error is large, the clock has probably just randomized so add an
    ; average poll duration to the count and assume the current tick is good
    mov     eax,_dwReadAvg

drtc_use_delta:
    sub     edi,eax

    jns     drtc_poll_loop      ; If the counter went negative we timed out

drtc_timeout:
drtc_poll_done:
    ;if OK, eax == zero, ZR == true
    ;else,  eax == delta, NZ == true
    ret

_DRTCPoll ENDP
VxD_LOCKED_CODE_ENDS

    .586p
VxD_PAGEABLE_CODE_SEG
;******************************************************************************
;
;   _ETSCPoll
;
;   This is the pentium polling with interrupts enabled.
;   It is used for pentium class processors unless the interrupts are enabled
;   with the registry flag.
;   Two sets of values are produced such that an a reading is available at 
;   all touched indices for the before transition time in on set and the after
;   time in the other such that the time taken to change can be found and thus
;   untimely interrupts can be detected.
;
;   Entry:
;       EBP =  pointer to 32 DWORD buffer (unmodified)
;       EDI =  timeout value
;       ESI =  axis mask (unmodified)
;
;   Returns:
;       NZ & EAX = non-zero for error
;
;   Uses:
;       EAX,general
;       EDX,general
;       EBX,previous index
;       ECX,last timer
;       EDI,counter

;------------------------------------------------------------------------------

; Pentium time stamp counter poll
public _ETSCPoll
_ETSCPoll PROC
    
    mov     ebx,esi             ;First previous index has all interesting bits set
    rdtsc
    mov     ecx,eax             ;init last to now

etsc_poll_loop:
    rdtsc
    mov     edx,[esp+160]       ;load port value
    xchg    eax,ecx             ;save last timer
    sub     eax,ecx             ;
    add     edi,eax
    js      etsc_timeout

    in      al,dx
    mov     [ebp+ebx*4+64],edi
    and     eax,esi
    mov     [ebp+eax*4],edi
    mov     ebx,eax
    jnz     etsc_poll_loop

    ;if OK, eax == zero, ZR == true
    ;else,  eax == delta, NZ == true
etsc_timeout:
    ret

_ETSCPoll ENDP
VxD_PAGEABLE_CODE_ENDS

VxD_LOCKED_CODE_SEG
;******************************************************************************
;
;   _DTSCPoll
;
;   This is the pentium polling with interrupts disabled.
;   It is used for pentium class processors only if the interrupts are disabled
;   with the registry flag.
;   The values produced are the post-transition counters.
;
;   Entry:
;       EBP =  pointer to 32 DWORD buffer (only first 16 are used) (unmodified)
;       EDI =  timeout value
;       ESI =  axis mask (unmodified)
;
;   Returns:
;       NZ & EAX = non-zero for error
;
;   Uses:
;       EAX,general
;       EDX,general
;       EBX,port
;       ECX,last timer
;       EDI,counter

;------------------------------------------------------------------------------
public _DTSCPoll
_DTSCPoll PROC
    
    mov     ebx,[esp+160]       ;fetch port value
    rdtsc
    mov     ecx,eax             ;init last to now

    cli
dtsc_poll_loop:
    rdtsc
    mov     edx,ebx             ;load port value into edx
    xchg    eax,ecx             ;save last
    sub     eax,ecx             ;find delta
    add     edi,eax             ;apply delta to total
    js      dtsc_timeout        ;timed out?

    in      al,dx               ;get port status
    and     eax,esi             ;only watch bits of interest
    mov     [ebp+eax*4],edi     ;store total at position relative to remain axes positions
    jnz     dtsc_poll_loop      ;still going?

    ;if OK, eax == zero, ZR == true
    ;else,  eax == delta, NZ == true
dtsc_timeout:
    ret

_DTSCPoll ENDP
VxD_LOCKED_CODE_ENDS

end
