    page    , 132
;---------------------------------------------------------------------------;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1998 Microsoft Corporation.  All Rights Reserved.
;
;   File:   timing.asm
;   Content:    Code to support timing for joystick polling 
;               using rdtsc and the System Timer.
;
;---------------------------------------------------------------------------;

    .586p

;---------------------------------------------------------------------------;
;                I N C L U D E S
;---------------------------------------------------------------------------;

    .xlist
    include vmm.inc
    include vtd.inc
    include poll.inc
    .list

; equates for timeouts
;
MIN_TIMEOUT     EQU 50
MAX_TIMEOUT     EQU 500000
DEFAULT_TIMEOUT EQU 5000        ; should agree with same in msjstick.h

VxD_PAGEABLE_DATA_SEG
    _dwTSCScale             dd ?
    _dwTickTimeout          dd ?
    _dwReadLimit            dd ?
    _dwIntLimit             dd ?
    _dwReadAvg              dd ?
    PUBLIC _dwTSCScale
    PUBLIC _dwTickTimeout
    PUBLIC _dwReadLimit
    PUBLIC _dwIntLimit
    PUBLIC _dwReadAvg
    
IFNDEF RETAIL
    ; Timer polling messages
    TSCResolutionMsg    db "DIGIJOY: 2*TSC clock ticks per cycle = #eax"
                        db 0dh,0ah,0
    MadClockTicksMsg    db "DIGIJOY: Total clock ticks = #ebx",0dh,0ah,0
    RTCOKMsg            db "DIGIJOY: RTC polling calibrated with limit = "
                        db "#eax, avg = #ebx",0dh,0ah,0
ENDIF

VxD_PAGEABLE_DATA_ENDS

VxD_PAGEABLE_CODE_SEG
;---------------------------------------------------------------------------;
;
;   _InitSW@0
;
;   DESCRIPTION:
;       If both TSC counting and RTC counting have failed.  This function is 
;       called when a gameport devnode is received from the CFG_MGR and both 
;       this is a sub-pentium class machine and the RTC polling could not 
;       calibrate.
;       The units of calibration are the same regardless of whether or not
;       interrupts are disabled.
;
;   ENTRY:  None
;       
;   EXIT:   None
;   
;   USES:   None
;---------------------------------------------------------------------------;
public _InitSW@0
_InitSW@0 PROC

    push    1000h
    call    _CalcSWTickTimeOut@4
    ret

_InitSW@0 ENDP

;---------------------------------------------------------------------------;
;
;   _InitRTC@4
;
;   DESCRIPTION:
;       This function is called as soon as the gameport IO is known to check 
;       if the system timer can be used for timing joystick polls.
;       The time taken to perfom a large number of dummy joystick poll
;       cycles is measured and from the result two values are dervived,
;       a maximum poll cycle delta and an average poll cycle delta.
;       
;       During interrupt disabled polling, the maximum is used polling as 
;       the limit of valid poll deltas and the average is used to replace 
;       deltas which are deemed to be bogus.
;
;       For interrupt enabled polling, the maximum is used polling as the 
;       test of an interrupt occuring during the transition poll and the 
;       average is not used.
;
;       The units of calibration are the same regardless of whether or not
;       interrupts are disabled.
;
;   ENTRY:  None
;       
;   EXIT:   EAX = TRUE if TSC polling can be used
;                 FALSE if not
;   
;   USES:   FLAGS
;---------------------------------------------------------------------------;
public _InitRTC@4
_InitRTC@4 PROC

    push    ebx

    push    1000h
    call    _CalcRTCTickTimeOut@4

    pushfd
    cli

    ;Get an average time for a poll cycle
CLK_READS_BIT equ 12
    ;Power of two that the limit is larger than the average
LIMIT_SHIFT equ 2 
    ;Power of two that the result is scaled by must be less than LIMIT_SHIFT
SCALE_SHIFT equ 1

clock_cal:
    Read_Sys_Clock
    mov     edx,eax
    xor     ebx,ebx
    mov     ecx,1 SHL CLK_READS_BIT ;number of times to retry

clock_cal_loop:
    ;The point here is to do the same IO as in the real polls and 
    ;anything to take about the same time as the other poll logic.
    push edx
    mov edx,[esp+16]            ; gameport address
    in al,dx

    mov     eax,5
dummy_loop:
    dec     eax
    mov     edx,[esp+eax*4]
    jnz     dummy_loop

    pop edx

    Read_Sys_Clock
    xchg    eax,edx
    sub     eax,edx

    and     eax,0ffh            ; T0 min counter wrap = 400h
    add     ebx,eax

    dec     ecx
    jnz     clock_cal_loop

    popfd                       ; Restore callers interrupt state

    ;Sanity check total clock ticks
    cmp     ebx,1 SHL (CLK_READS_BIT-4) ; Can't read 1.2MHz clock 16 times/tick
    jle     MadClockTicks
    cmp     ebx,1 SHL (CLK_READS_BIT+7) ; Must read faster than 1 per 128 ticks
    jg      MadClockTicks

    shr     ebx,(CLK_READS_BIT-LIMIT_SHIFT)
    mov     eax,ebx
    adc     eax,0
    mov     _dwReadLimit,eax

    mov     eax,ebx
    shr     eax,(LIMIT_SHIFT-SCALE_SHIFT)
    adc     ebx,0
    mov     _dwIntLimit,ebx

    shr     ebx,LIMIT_SHIFT
    adc     ebx,0
    mov     _dwReadAvg,ebx

IFNDEF RETAIL
    mov     eax,_dwReadLimit
    mov     ebx,_dwReadAvg
    DPF RTCOKMsg
ENDIF
    mov     eax, 1              ; OK, use system timer

Init_RTC_Exit:
    pop     ebx
    ret 4

MadClockTicks:
    DPF     MadClockTicksMsg
    xor     eax,eax             ; Failed
    jmp     Init_RTC_Exit

_InitRTC@4 ENDP

;---------------------------------------------------------------------------;
;
;   _InitTSC@4
;
;   DESCRIPTION:
;       This function is called as soon as the gameport IO is known to 
;       calibrate the TSC polling loop.
;       A test poll loop is executed with interrupts disabled and the 
;       counter difference is found.  From this a maximum cycle is limit is 
;       guessed.  If the difference between the pre and post transition 
;       times during a poll is greater than this limit, it will be assumed 
;       that an interrupt occurred and so the actual transition time could 
;       have been anywhere between the two times.
;       The test poll is intended to take the same number of TSC ticks as
;       the real poll.  The limit is twice the test poll which leaves a
;       significant margin for error, but any real interrupt hit is certain 
;       to take more time than that due to context saving.
;       NOTE if somebody creates a Pentium compatible with a significantly
;       improved interrupt latency, this may need to be made more accurate.
;
;   ENTRY:  None
;       
;   EXIT:   None
;   
;   USES:   FLAGS
;---------------------------------------------------------------------------;
public _InitTSC@4
_InitTSC@4 PROC

    push    ebx
    push    esi
    push    edi
    push    ebp

    pushfd
    cli                         ; we need to know how long this takes
                                ; without any interrupt hits

    sub     esp,24              ; leave 6 DWORD buffer
    mov     ebp,esp

    mov     esi,ebp             ; read, write each DWORD to get it in cache
    mov     edi,ebp
    mov     ecx,6
    rep     movsd
    
    mov     esi,2
    mov     ebx,esi

test_tsc_poll:
    rdtsc
    mov     edx,[esp+48]
    xchg    eax,ecx 
    sub     eax,ecx
    dec     esi
    js      test_tsc_done

    in      al,dx
    mov     [ebp+ebx*4+8],ecx
    mov     eax,esi
    mov     [ebp+eax*4],ecx
    mov     ebx,eax
    jns     test_tsc_poll       ; mov and in do not alter the flags so ns is 
                                ; still true from previous not taken js
                                ; so this will always jump
test_tsc_done:
    ; [ebp] and [ebp+4] have consecutive timer values 
    ; so the difference is what we need
    mov     eax,[ebp]
    sub     eax,[ebp+4]
    shr     eax,1
    mov     _dwIntLimit,eax
    DPF     TSCResolutionMsg

    add     esp,24              ; remove 6 DWORD buffer

    popfd
    pop     ebp
    pop     edi
    pop     esi
    pop     ebx

    ret 4

_InitTSC@4 ENDP

;---------------------------------------------------------------------------;
;
;   _CalcTSCTickTimeOut@4
;
;   DESCRIPTION:
;       This function is called to calculate the dwTickTimeout from
;       dwTimeOut when time stamp counter timing is in use.
;
;   ENTRY:  None
;       
;   EXIT:   None
;   
;   USES:   EAX, EDX, FLAGS
;---------------------------------------------------------------------------;
public _CalcTSCTickTimeOut@4
BeginProc _CalcTSCTickTimeOut@4,SCALL,PUBLIC,ESP
    ArgVar NewTimeout, DWORD

    EnterProc
    ; Convert the scale back into a number of TSCs per joystick unit
    mov     ecx,_dwTSCScale
    mov     eax,(1 SHL BIN_POINT)
    xor     edx,edx
    div     ecx                 ; If a scale could not be calculated, 
                                ; TSC would have been failed

    ; The range limits for the divisor and MAX_TIMEOUT ensure the TickTimeOut
    ; will not max out a 32 bit value.
    mov     ecx,NewTimeout
    cmp     ecx,MAX_TIMEOUT
    jg      tsc_use_default_timeout
    cmp     ecx,MIN_TIMEOUT
    jge     @f
tsc_use_default_timeout:
    mov     ecx,DEFAULT_TIMEOUT
@@:

    ; With both parameters within limits, calculate the timeout in tsc ticks
    mul     ecx
    mov     _dwTickTimeout,eax
    
    LeaveProc
    return
EndProc _CalcTSCTickTimeOut@4

;---------------------------------------------------------------------------;
;
;   _CalcRTCTickTimeOut@4
;
;   DESCRIPTION:
;       This function is called to calculate the dwTickTimeout from
;       dwTimeOut when sytem clock timing is in use.
;
;   ENTRY:  None
;       
;   EXIT:   None
;   
;   USES:   EAX, EDX, FLAGS
;---------------------------------------------------------------------------;
public _CalcRTCTickTimeOut@4
BeginProc _CalcRTCTickTimeOut@4,SCALL,PUBLIC,ESP
    ArgVar NewTimeout, DWORD

    EnterProc
    ; A reasonable maximum poll duration is ~ 1.5ms which is 1790 system 
    ; timer clock ticks.
    ; The current control panel is happiest when the values returned are in 
    ; the range 0->655.  
    ; The timeout in clock ticks is NewTimeout * 1790 / 655.
    ; Use a precalculated full 32 bit value as the multiplier to get a good 
    ; timeout value.

    mov     eax,NewTimeout
    cmp     eax,MAX_TIMEOUT
    jg      rtc_use_default_timeout
    cmp     eax,MIN_TIMEOUT
    jge     @f
rtc_use_default_timeout:
    mov     eax,DEFAULT_TIMEOUT

@@:
    mov     edx,0aee0d79eh      ; (1789.77/655) * 2^30
    mul     edx
    shrd    eax,edx,30

    mov     _dwTickTimeout,eax

    LeaveProc
    return
EndProc _CalcRTCTickTimeOut@4

;---------------------------------------------------------------------------;
;
;   _CalcSWTickTimeOut@4
;
;   DESCRIPTION:
;       This function is called to calculate the dwTickTimeout from
;       dwTimeOut when software loop count timing is in use.
;
;   ENTRY:  None
;       
;   EXIT:   None
;   
;   USES:   EAX, EDX, FLAGS
;---------------------------------------------------------------------------;
public _CalcSWTickTimeOut@4
BeginProc _CalcSWTickTimeOut@4,SCALL,PUBLIC,ESP
    ArgVar NewTimeout, DWORD

    EnterProc

    ; A reasonable maximum poll duration is ~ 1.5ms which is 1790
    ; system timer clock ticks.
    ; The current control panel is happiest when the values
    ; returned are in the range 0->655.  

    ; Use pre-DX3 compatible timeout value and assume that given RDTSC
    ; detection failed and we couldn't find a usable system clock that this
    ; system is no hot-rod so we won't timeout under normal circumstances.
    mov     eax,NewTimeout
    mov     _dwTickTimeout,eax

    LeaveProc
    return
EndProc _CalcSWTickTimeOut@4

VxD_PAGEABLE_CODE_ENDS

VxD_IDATA_SEG

IFNDEF RETAIL
    ;_IsTSCPresent@0 messages
    NoCPUIDMsg          db "DIGIJOY: CPUID not supported",0dh,0ah,0
    CPUIDFailedMsg      db "DIGIJOY: CPUID failed!",0dh,0ah,0
    CPUIDResultsMsg     db "DIGIJOY: Family, model, step #ax Features #edx "
                        db "Vendor Str = <"
    VendorEBX           dd ?
    VendorEDX           dd ?
    VendorECX           dd ?
    VendorEOT           db '>',0dh,0ah,0
    NoRDTSCMsg          db "DIGIJOY: No rdtsc supported",0dh,0ah,0
    BadTSCScaleMsg      db "DIGIJOY: TSC scale out of range",0dh,0ah,0
    TSCOKMsg            db "DIGIJOY: TSC polling calibrated: Joy Unit = #edx"
                        db ", dwTickTimeout = #eax",0dh,0ah,0
ENDIF

VxD_IDATA_ENDS

VxD_INIT_CODE_SEG

;---------------------------------------------------------------------------;
;
;   _IsTSCPresent@0
;
;   DESCRIPTION:
;       This function is called during DigiJoy initialization to 
;       check if the rdtsc (read time stamp counter) instruction 
;       is available.  If it is, the CPU speed is calibrated 
;       against the system timer.
;       If available, this counter is incremented at CPU clock speed
;       regardless of what else is happening in the system (CPU power saving
;       excluded).
;
;   ENTRY:  None
;       
;   EXIT:   EAX = TRUE if TSC polling can be used
;                 FALSE if not
;   
;   USES:   FLAGS
;---------------------------------------------------------------------------;
public _IsTSCPresent@0
_IsTSCPresent@0 PROC

    pushfd
    push    ebx

    ; The CPUID instruction reports information about the processor, taking
    ; an implied parameter in eax for a function number.
    ; The CPU tests run during Windows boot check if the instruction is
    ; supported and set a bit in the machine info accordingly.
    VMMcall Get_Machine_Info

    test    ebx,GMIF_CPUID
    jz      NoCPUID
    

    ; CPUID function 0 returns the highest supported function number in
    ; eax and a 12 char string in EBX-EDX-ECX for manufacturer identification.
    xor     eax,eax
    cpuid

    ; eax == 0 ? function 1 is not supported (not any known case)
    test    eax,eax
    jz      CPUIDFailed

IFNDEF RETAIL
    ; Why not display the string.
    mov     VendorEBX,ebx
    mov     VendorEDX,edx
    mov     VendorECX,ecx
ENDIF

    ; CPUID function 1 returns a bunch of flags in EDX, bit 4 reports
    ; support of the rdtsc instruction.  Later 486s support CPUID but
    ; not RDTSC. (EAX has CPU version info).
    mov     eax,1
    cpuid

    DPF     CPUIDResultsMsg

    ; does it support RDTSC?
    test    edx,010h
    jz      NoRDTSC

    ; OK rdtsc is available, so calibrate a poll to take 
    ; account of the tsc running at CPU clock speed
    ;
    ; A reasonable maximum poll duration is ~ 1.5ms.
    ; The current control panel is happiest when the values
    ; returned are in the range 0->655.  To find ~ equivalent 
    ; tick values measure 1.5ms in tsc ticks and treat this
    ; as 655 sw poll units.

    ; Take start times, wait for at least 1.5ms then scale 
    ; the result by how long we realy took.
    ; Given that the time slicing works on a much larger scale 
    ; than we do, we'll probably either go massively over our 
    ; minimum time or we'll just waste cycles in our yield 
    ; loop.  The main thing is we won't upset VTD by asking it 
    ; to delay with interrupts disabled and we haven't stalled 
    ; any other system activity either.

    ; All this cooperativety does assume than no joker's going 
    ; to mess with either the RTC (since we use VTD we should
    ; be immune to its actions) or the TSC (that would be evil).

    ; Still cli/sti around the time fetches
    cli
    VxDCall VTD_Get_Real_Time
    mov ebx,eax                 ; low dword of 0.8uS since boot
    rdtsc
    sti
    push eax                    ; low dword of CPU clock count

    
YieldLoop:
    xor eax,eax
    VMMCall Time_Slice_Sleep
    
    VxDCall VTD_Get_Real_Time
    sub eax,ebx
    cmp eax,1790                ; 1.5ms in 0.8us System clock units
    jl YieldLoop

    cli
    VxDCall VTD_Get_Real_Time
    mov ecx,eax
    rdtsc
    sti

    pop edx
    sub ecx,ebx                 ; RTC difference
    jz BadScale                 ; Don't use a 0 divide if the RTC is broken
    sub eax,edx                 ; TSC difference

    mov edx,1790
    mul edx
    div ecx                     ; eax = TSC ticks normalized for 1.5ms

    xor     edx,edx
    mov     ecx,eax
    mov     eax,(655 SHL BIN_POINT) ; Nice big number (but less than 32 bits)
    div     ecx                 ; Assume if RTC value was OK, so is the divisor

    ; eax = sw poll units * 64K per time stamp count
    mov     _dwTSCScale,eax

    ; Do sanity check on scale.
    ; in 1.5ms a 50MHz pentium compatible would increment TSC    75,000 times
    ;          a  5GHz pentium compatible would increment TSC 7,500,000 times
    ; and dividing 655 * 2^BIN_POINT by the limits gives 9158 and 91
    ; assume these as reasonable limits for the divisor (and be cursed by
    ; someone with their new 6GHz ?ium when it reverts to RTC polling).
    
    cmp     eax,91              ; 5GHz
    jle     BadScale
    cmp     eax,9158            ; 50MHz
    jge     BadScale

    push    1000h
    call    _CalcTSCTickTimeOut@4

IFNDEF RETAIL
    mov     eax,_dwTickTimeout
    mov     edx,_dwTSCScale
    DPF     TSCOKMsg
ENDIF

    mov     eax,1               ; Everything OK

    pop     ebx
    popfd
    ret

NoCPUID:
IFNDEF RETAIL
    DPF     NoCPUIDMsg
    jmp     short NotPresentExit
ENDIF

CPUIDFailed:
IFNDEF RETAIL
    DPF     CPUIDFailedMsg
    jmp     short NotPresentExit
ENDIF

NoRDTSC:
IFNDEF RETAIL
    DPF     NoRDTSCMsg
    jmp     short NotPresentExit
ENDIF

BadScale:
    DPF     BadTSCScaleMsg

NotPresentExit:
    xor     eax,eax             ; Failed
    mov     _dwTSCScale,eax     ; used as flag later

    pop     ebx
    popfd
    ret

_IsTSCPresent@0 ENDP

VxD_INIT_CODE_ENDS

end
