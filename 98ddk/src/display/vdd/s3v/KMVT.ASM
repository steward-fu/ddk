page            ,132
title           S3 Mini VDD Kernel Mode Video Transport (VPE ring 0) Functions
.386p
;
;Created 1-20-97 Bernie Thompson
;
.xlist
include     VMM.INC
include     VMMREG.INC
include     VWIN32.INC
include     MINIVDD.INC
include     DDKMMINI.INC
include     S3MINI.INC
include     CONFIGMG.INC
include     LPB.INC
.list

;
; remove these when Microsoft exports them in the DDK
;
DDOVER_BOB         EQU 00200000H    ; DDRAW.H
DDOVER_INTERLEAVED EQU 00800000H    ; DDRAW.H

VxD_LOCKED_DATA_SEG
;
; Many of these functions are called at interrupt time
; So all data must be allocated in a locked (non-swappable) segment
;

;
; Performance and Quality Metrics
;
METRICS_TIME            EQU 0   ; time elapsed in milliseconds
METRICS_IRQ             EQU 1   ; a call to IsOurIRQ happened
METRICS_VPVSYNC         EQU 2   ; a video port VSYNC happened
METRICS_OVSYNC          EQU 3   ; an overlay (display) VSYNC happened
METRICS_TEAR            EQU 4   ; an overlay flipped to a buffer being filled
METRICS_SKIP            EQU 5   ; a field was skipped with SkipNextField
METRICS_REPEAT          EQU 6   ; an overlay buffer was displayed twice
METRICS_OBADBUFFER      EQU 7   ; flip to overlay buffer not in vpflip list
METRICS_POLARITYGUESS   EQU 8   ; polarity report from hardware not available in time
METRICS_FLIPLATE        EQU 9   ; FlipVP() happened late
METRICS_MAX             EQU 9

IFDEF DEBUG
public metrics
metrics                 dd METRICS_MAX+1 dup(0)
public metrics_old
metrics_old             dd METRICS_MAX+1 dup(0)
public metrics_persec
metrics_persec          dd METRICS_MAX+1 dup(0)

p1                  dd  0               ; parameter stack to debug printf
p2                  dd  0
p3                  dd  0
SetStateString      db  'SetState called with overlay flags %lx\n',0
public DisabledString
DisabledString      db  'KMVT disabled\n',0
MetricsString       db  'Metrics:\n',0
OVSYNCString        db  '  display vsyncs',0
VPVSYNCString       db  '  video vsyncs',0
TOTALIRQDebugString db  '  all interrupts',0
SkippedFieldsString db  '  skipped fields',0
TearingString       db  '  tears',0
PolarityGuessString db  '  polarity guesses',0
BadOFlipString      db  '  bad overlay address',0
FlipLateString      db  '  flip >2ms late',0
ValuesString        db  '%-24s: %8lu per second, %12lu total\n',0
QualityString       db  'Playback Quality Features:\n',0
InputSizeString     db  '  %lux%lu\n',0
SkippingString      db  '  Field Skipping Mode\n',0
BobString           db  '  Bob Mode\n',0
WeaveString         db  '  Weave Mode\n',0
InterleavedString   db  '  Interleaved\n',0
NonInterleavedString db '  NonInterleaved\n',0
VInterpolationString db '  Vertical Interpolation Enabled\n',0
BuffersString       db  '  %lu autoflip buffers\n',0
VDecimationString   db  '  Vertical Decimation value %lx\n',0
HDecimationString   db  '  Horizontal Decimation value %lx\n',0
vvsynctime          dd 0                ; timer of last vvsync
ENDIF

;
; shadowed registers
;
public MM81D0
MM81D0              dd  0
public MM81E8
MM81E8              dd  0

;
; interrupts enabled status
;
public EnabledIRQs
EnabledIRQs         dd  0
public RequestedIRQs
RequestedIRQs       dd  0

;
; User-configurable options
;

RegString           db      'Display\Settings',0
FlagsString         db      'KMVT_FLAGS',0
RegHandle           dd      0
RegSize             dd      4
dwType              dd      0

; On all products prior to GLH:
;   81E8 gets loaded 5 lines above the overlay window
;   81D0 gets loaded at the beginning of any line not in the overlay
DISABLE_81D0DELAY   EQU 00000001H
DISABLE_VDDABOB     EQU 00000002H
DISABLE_81E8DELAY   EQU 00000004H
public flags
flags               dd  0

;
; Buffer management.  Primarily to track field polarity for interlaced
; content.  Also used for detecting quality problems and other metrics.
; buffer is an array of offsets from the start of the frame buffer
; the top bits of every buffer offset are used for flags
BUFFER_POLARITY_KNOWN      EQU 080000000h
BUFFER_POLARITY_EVEN       EQU 040000000h
BUFFER_MASK                EQU 03FFFFFFFh

public buffer
buffer              dd 15 dup (0)       ; array of buffers referenced by FlipVP
bufferend           dd 0                ; last element in array
buffers             dd 0                ; # buffers to flip (as told by VPE)
buffersize          dd 0                ; size of buffers in bytes (as told by VPE)
vpindex             dd 0                ; current byte offset of vp in blist (index*4)
oindex              dd 0                ; current byte offset of o  in blist (index*4)
polarity            dd 0                ; polarity of last buffer

extrn MMIOLinearAddrBase:dword
extrn ChipID:dword

VxD_LOCKED_DATA_ENDS

VxD_DATA_SEG
extrn DisplayInfoStructure:byte
extrn ConfigStructure:byte
VxD_DATA_ENDS

;
; Debugging Macro Support
;
DPF macro string
IFDEF DEBUG
    mov  p1, ebx
    mov  p2, ecx
    mov  p3, edx
    push OFFSET32 p1
    push OFFSET32 string
    VMMcall _Debug_Printf_Service
    add esp, 8
ENDIF
endm

TRIGGER macro string
IFDEF DEBUG
    inc     metrics+string*4
ENDIF
endm

bufferdec macro reg
local end
    sub     reg, 4
    cmp     reg, offset32 buffer
    jae     end
    mov     reg, offset32 bufferend     ; roll over to top
end:
endm

bufferinc macro reg
local end
    add     reg, 4
    cmp     reg, offset32 bufferend
    jbe     end
    mov     reg, offset32 buffer        ; roll over to bottom
end:
endm

VxD_LOCKED_CODE_SEG

;****************************************************************************
;
;   GetDDHAL
;
;   DESCRIPTION: This function fills in the function table supplied
;       by DDRAW with functions that their VxD can call fro kernel mode
;       DirectDraw support.  The MiniVDD can fill in the dwContext field
;       with a value that will always be passed back when called.
;
;   ENTRY:
;          ESI  Ptr to DIOCParams
;               ESI+18h  Ptr to output buffer
;               ESI+1Ch  Size of output buffer
;               ESI+20h  Amount of data we write into output buffer
;
;   EXIT:
;          EAX  0 = success
;
;****************************************************************************

BeginProc GetDDHAL, RARE

        mov     ebx, [esi.DIOCParams.cbOutBuffer]
        cmp     ebx, SIZE _DDMINIVDDTABLE
        jl      GetDDHAL_Error

        mov     ebx, [esi.DIOCParams.lpvOutBuffer]
        mov     eax, MMIOLinearAddrBase ; Context information
        mov     [ebx._DDMINIVDDTABLE.dwMiniVDDContext], eax
        mov     eax, OFFSET32 DDGetIRQInfo
        mov     [ebx._DDMINIVDDTABLE.vddGetIRQInfo], eax
        mov     eax, OFFSET32 DDIsOurIRQ
        mov     [ebx._DDMINIVDDTABLE.vddIsOurIRQ], eax
        mov     eax, OFFSET32 DDEnableIRQ
        mov     [ebx._DDMINIVDDTABLE.vddEnableIRQ], eax
        mov     eax, OFFSET32 DDSkipNextField
        mov     [ebx._DDMINIVDDTABLE.vddSkipNextField], eax
        mov     eax, OFFSET32 DDBobNextField
        mov     [ebx._DDMINIVDDTABLE.vddBobNextField], eax
        mov     eax, OFFSET32 DDSetState
        mov     [ebx._DDMINIVDDTABLE.vddSetState], eax
        mov     eax, OFFSET32 DDLock
        mov     [ebx._DDMINIVDDTABLE.vddLock], eax
        mov     eax, OFFSET32 DDFlipOverlay
        mov     [ebx._DDMINIVDDTABLE.vddFlipOverlay], eax
        mov     eax, OFFSET32 DDFlipVideoPort
        mov     [ebx._DDMINIVDDTABLE.vddFlipVideoPort], eax
        mov     eax, OFFSET32 DDGetFieldPolarity
        mov     [ebx._DDMINIVDDTABLE.vddGetPolarity], eax
        mov     eax, OFFSET32 DDGetCurrentAutoflipSurface
        mov     [ebx._DDMINIVDDTABLE.vddGetCurrentAutoflip], eax
        mov     eax, OFFSET32 DDGetPreviousAutoflipSurface
        mov     [ebx._DDMINIVDDTABLE.vddGetPreviousAutoflip], eax
        mov     ebx, [esi.DIOCParams.lpcbBytesReturned]
        mov     eax, SIZE _DDMINIVDDTABLE
        mov     [ebx], eax

        ;
        ; Read user/debug options from registry
        ;
        mov     eax,OFFSET32 RegString
        mov     edx,OFFSET32 RegHandle
        VMMCall _RegOpenKey,<HKEY_CURRENT_CONFIG,eax,edx>
        cmp     eax,ERROR_SUCCESS       ;we're we successful.
        jne     short GetDDHAL_Exit     ;no, so leave everything alone.
        mov     eax,OFFSET32 FlagsString
        mov     ebx,OFFSET32 dwType
        mov     ecx,OFFSET32 flags
        mov     edx,OFFSET32 RegSize
        VMMCall _RegQueryValueEx,<RegHandle,eax,0,ebx,ecx,edx>
        cmp     eax,ERROR_SUCCESS
        je      @f
        mov     eax,0
        mov     flags,0
@@:
        VMMCall _RegCloseKey,<RegHandle>
GetDDHAL_Exit:
        sub     eax, eax
        ret

GetDDHAL_Error:
        mov     eax, 1
        ret

EndProc GetDDHAL

;****************************************************************************
;
;   DDGetIRQInfo
;
;   DESCRIPTION: If the Mini VDD is already managing the IRQ, this
;          function returns that information; otherwise, it returns the
;          IRQ number assigned to the device so DDraw can manage the IRQ.
;
;          The returning the IRQ number, it is important that it get the
;          value assigned by the Config Manager rather than simply get
;          the value from the hardware (since it can be remapped by PCI).
;
;   ENTRY:
;          ESI  NULL
;          EDI  LPDDGETIRQINFO
;                   DWORD dwSize;
;                   DWORD dwFlags;
;                   DWORD dwIRQNum;
;
;   EXIT:
;          EAX  0 = success, 1 = error
;
;****************************************************************************
BeginProc DDGetIRQInfo, RARE

    push        ecx
    push        ebx
    push        edi

    ; First we need to get the DEVNODE so we can then get the IRQ number
    mov         eax,OFFSET32 DisplayInfoStructure
    mov         ecx,SIZE DISPLAYINFO    ;pass size in ECX, address in EAX
    mov         [eax].diHdrSize,cx      ;fill in this entry
    push        eax                     ;save EAX --> DisplayInfoStructure
    VxDCall     VDD_Get_DISPLAYINFO     ;get information from the VDD
    pop         eax
    mov         ebx, [eax].diDevNodeHandle

    ; Now call Get_Alloc_Log_Conf to get the IRQ number assigned by
    ; the Config Manager.
    mov         eax, CM_GET_ALLOC_LOG_CONF_ALLOC
    push        eax
    push        ebx
    mov         eax,OFFSET32 ConfigStructure
    push        eax
    VxDCall     _CONFIGMG_Get_Alloc_Log_Conf
    pop         ebx
    pop         ebx
    pop         ebx
    cmp         eax, CR_SUCCESS
    jnz         GII_ConfigError
    mov         eax,OFFSET32 ConfigStructure
    mov         ebx, 1
    cmp         [eax].wNumIRQs, bx
    jl          GII_ConfigError
    mov         ebx, IRQINFO_NOTHANDLED
    pop         edi
    mov         [edi._DDGETIRQINFO.dwFlags], ebx
    xor         ebx, ebx
    mov         bl, [eax].bIRQRegisters
    test        ebx, not 0Fh                ; IRQNum should be 0..15
    jnz         GII_IRQError
    mov         [edi._DDGETIRQINFO.dwIRQNum], ebx
    pop         ebx
    pop         ecx
    xor         eax, eax
    ret

public GII_ConfigError
GII_ConfigError:
    pop     edi

public GII_IRQError
GII_IRQError:
    pop         ebx
    pop         ecx
    mov         eax, 1
    ret

EndProc DDGetIRQInfo

;****************************************************************************
;
;   DDEnableIRQ
;
;   DESCRIPTION: Notifies the Mini VDD which IRQs should be enabled.  If
;          a previously enabled IRQ is not specified in this call,
;          it should be disabled.
;
;   ENTRY:
;          ESI  LPDDENABLEIRQINFO
;                   DWORD dwSize
;                   DWORD dwIRQSources
;                   DWORD dwLine
;                   DWORD IRQCallback
;                   DWORD dwContext
;          EDI  NULL
;
;   EXIT:
;          EAX  0 = success, 1 = error
;
;****************************************************************************
BeginProc DDEnableIRQ, RARE

    mov     MM81D0, 0
    mov     MM81E8, 0
    mov     vpindex, offset32 buffer
    mov     oindex, offset32 buffer
IFDEF DEBUG
    mov     eax, METRICS_MAX           ; zero metrics
    mov     ecx, eax
    add     ecx, eax
    add     ecx, eax
@@:
    mov     [metrics+ecx*4], 0
    dec     ecx
    cmp     ecx, 0
    jge     @b
ENDIF
    mov     edi, [esi]._DDENABLEIRQINFO.dwIRQSources
    mov     eax, RequestedIRQs
    cmp     eax, edi
    je      DDEIRet

    mov     EnabledIRQs, edi
    mov     RequestedIRQs, edi

    mov     eax, MMIOLinearAddrBase
    add     eax, 083d4h                 ; INDEX
    mov     cl, [eax]                   ; save
    push    ecx

    mov     eax, MMIOLinearAddrBase
    add     eax, LPB_INTERRUPT_FLAGS
    mov     ebx, [eax]
    and     ebx, 0FFF0FFFFh             ; default don't enable LPB ints
    or      ebx, 0Fh                    ; clear any existing interrupts
    test    edi, DDIRQ_VPORT0_VSYNC
    jz      @f
    or      ebx, LPB_IENABLE_VSYNC      ; enable video VSYNC
@@:
    mov     [eax], ebx

    cmp     ChipID, S3765               ; 8504 accesses will hang Trio64V+
    je      DDEIMasterSwitch
    cmp     ChipID, S3M65
    je      DDEIMasterSwitch
    cmp     ChipID, S3767
    je      DDEIMasterSwitch

    mov     eax, MMIOLinearAddrBase
    add     eax, 08504h
    mov     ebx, 00000EFh              ; Default: Disable Display VSYNC

    cmp     ChipID, S3775              ; 8504 can't be read back on Trio64V2
    je      DDEISetDisplayVSync
    test    edi, DDIRQ_DISPLAY_VSYNC   ; enable Display VSYNC if asked to
    jnz     DDEIEnableDisplayVSync
    test    edi, DDIRQ_VPORT0_VSYNC    ; if Video VSYNC is not enabled
    jz      DDEISetDisplayVSync        ; don't enable display VSYNC workarounds
    test    flags, DISABLE_81D0DELAY
    jz      DDEIEnableDisplayVSync     ; enable for 81D0 Workaround
    test    flags, DISABLE_81E8DELAY
    jz      DDEIEnableDisplayVSync     ; enable for 81E8 Workaround
    jmp     DDEISetDisplayVSync

DDEIEnableDisplayVSync:
    or      EnabledIRQs, DDIRQ_DISPLAY_VSYNC
    or      ebx, 00000100h              ; Enable Display VSYNC

DDEISetDisplayVSync:
    mov     [eax], ebx

DDEIMasterSwitch:                       ; enable or disable interrupt generation in general
    mov     eax, MMIOLinearAddrBase
    add     eax, 083d4h
    mov     bl, 032h
    mov     [eax], bl
    inc     eax                         ; DATA
    mov     bl, [eax]
    and     bl, not 010h                ; default: disable interrupts
    cmp     edi, 0                      ; were any ints enabled?
    je      @f
    or      bl, 010h                    ; enable VGA+GE+LPB interrupts
@@:
    mov     [eax], bl

DDEIDone:
    pop     ecx
    mov     eax, MMIOLinearAddrBase     ; restore CRTC index
    add     eax, 083d4h
    mov     [eax], cl

DDEIRet:
    xor     eax, eax
    ret

EndProc DDEnableIRQ

;****************************************************************************
;
;   DDIsOurIRQ
;
;   DESCRIPTION: Called when the VDD's IRQ handler is triggered.  This
;          determines if the IRQ was caused by our VGA and if so, it
;          clears the IRQ and returns which event(s) generated the IRQ.
;
;   ENTRY:
;          ESI  NULL
;
;   EXIT:
;          EDI  IRQ source flags
;          EAX  0 = success, 1 = error
;
;   NOTE that if ANYTHING weird is going on, like a new version of the
;   VDD resulting in traps in other people's code, it's most likely
;   because edi returned by this function isn't accurately reflecting
;   the interrupt that was generated.  It's crucial that EDI be accurate,
;   so that the system components acknowledge the interrupt correctly.
;
;****************************************************************************
BeginProc DDIsOurIRQ, High_Freq

    xor     edi, edi                    ; assume no interrupt
    TRIGGER METRICS_IRQ

    cmp     EnabledIRQs, 0
    je      IOIEnd

IOIGE:
    test    EnabledIRQs, DDIRQ_DISPLAY_VSYNC
    jz      IOILPB

    mov     eax, MMIOLinearAddrBase
    add     eax, 08504h
    mov     ebx, [eax]
    test    ebx, 1                     ; Did we get a display VSYNC int?
    jz      IOILPB

IOIGotDisplayVSync:
    mov     eax, MMIOLinearAddrBase
    add     eax, 08504h
    mov     ebx, 000001EFh
    mov     [eax], ebx

    mov     edi, DDIRQ_DISPLAY_VSYNC
    TRIGGER METRICS_OVSYNC

    mov     ebx, MM81D0
    cmp     ebx, 0
    je      IOI81E8

    mov     eax, MMIOLinearAddrBase
    add     eax, 081d0h
    call    SP_Set
    mov     MM81D0, 0

IOI81E8:
    mov     ebx, MM81E8
    cmp     ebx, 0
    je      IOIEnd

    mov     eax, MMIOLinearAddrBase
    add     eax, 081e8h
    call    SP_Set
    mov     MM81E8, 0
    jmp     IOIEnd

IOILPB:
    test    EnabledIRQs, DDIRQ_VPORT0_VSYNC
    jz      IOIEnd

    mov     eax, MMIOLinearAddrBase
    add     eax, LPB_INTERRUPT_FLAGS
    mov     ebx, [eax]
    test    ebx, LPB_INT_VSYNC
    jz      IOILPBSET
    ; LPB Video Port VSYNC interrupt
    mov     edi, DDIRQ_VPORT0_VSYNC
    TRIGGER METRICS_VPVSYNC

IFDEF DEBUG
    push    eax
    VMMCall Get_System_Time
    mov     vvsynctime, eax
    pop     eax
ENDIF

IOILPBSET:
    or      ebx, 0Fh
    mov     [eax], ebx                 ; clear any LPB ints that were set

IOIEnd:
IFDEF DEBUG
    VMMCall Get_System_Time
    cmp     eax, metrics+METRICS_TIME*4
    jbe     IOIExit

public IOIMetricsPerSec
IOIMetricsPerSec:
    push    edi
    add     eax, 1000
    mov     metrics+METRICS_TIME*4, eax

    mov     esi, OFFSET32 metrics
    mov     edi, OFFSET32 metrics_old
    mov     edx, OFFSET32 metrics_persec
    xor     ecx, ecx
@@:
    mov     eax, [ecx*4+esi]            ; metrics
    mov     ebx, [ecx*4+edi]            ; metrics_old
    sub     eax, ebx                    ; mertrics_persec = metrics-metrics_old
    mov     [ecx*4+edx], eax
    inc     ecx
    cmp     ecx, METRICS_MAX
    ja      IOIMetricExit
    jmp     @b

IOIMetricExit:
    rep     movsd                       ; copy metrics to metrics_old
    pop     edi
ENDIF

IOIExit:
    xor     eax, eax
    ret

EndProc DDIsOurIRQ

;****************************************************************************
;
;   DDFlipVideoPort
;
;   DESCRIPTION: Flips the video port to the target surface.
;
;   ENTRY:
;          ESI  LPDDFLIPVIDEOPORTINFO
;                   DWORD dwSize
;                   LPDDVIDEOPORTDATA video port info
;                   LPDDSURFACEDATA current surface
;                   LPDDSURFACEDATA target surface
;                   DWORD dwFlipVPFlags
;          EDI  NULL
;
;   EXIT:
;          EAX  0 = success, 1 = error
;
;****************************************************************************

BeginProc DDFlipVideoPort, High_Freq

IFDEF DEBUG
    VMMCall Get_System_Time
    mov     ebx, vvsynctime
    add     ebx, 2                     ; half a field
    cmp     eax, ebx
    jbe     @f
    inc     metrics+METRICS_FLIPLATE*4 ; flip late
@@:
ENDIF
    ; Get the new offset
    mov     edi, [esi]._DDFLIPVIDEOPORTINFO.lpTargetSurface  ; Point edi to target surface
    mov     ebx, [edi]._DDSURFACEDATA.dwSurfaceOffset        ; get offset (from framebuffer)
    mov     edi, [esi._DDFLIPVIDEOPORTINFO.lpVideoPortData]  ; Point edi to video port info
    add     ebx, [edi].DDVIDEOPORTDATA.dwOriginOffset        ; add offset from surface
    and     ebx, BUFFER_MASK       ; mask out top bits

    mov     edx, [edi.DDVIDEOPORTDATA.dwDriverReserved3]     ; tuck away buffer size
    mov     buffersize, edx

    mov     edx, vpindex           ; vpindex (pre-flip)

    mov     eax, [edx]
    and     eax, BUFFER_POLARITY_EVEN
    mov     polarity, eax          ; save previous polarity

    bufferinc edx                  ; increment vpindex
    mov     vpindex, edx

; The Philips 7110 method of communicating odd/even means that we don't know
; for certain what the polarity of the field is until after the first HSYNC
; after the start of V Active.
; So while we tuck away the buffer address now, we have to allocate
; a callback which will be called to read the field polarity from the
; hardware and update the list with the value in hardware at that time.

    mov     [edx], ebx             ; *vpindex = address;
    mov     eax, 4                 ; wait 4ms (about 1/4 of a 60Hz field rate)
    mov     esi, DDUpdatePolarity
    VMMcall Set_Async_Time_Out     ; refdata (vpindex) in edx

FVP_SetBase:

    mov     eax, MMIOLinearAddrBase
    add     eax, LPB_BASE0
    call    LPB_Set               ; address in ebx

FVP_Exit:
    xor     eax, eax
    ret

EndProc DDFlipVideoPort

;****************************************************************************
;
;   DDFlipOverlay
;
;   DESCRIPTION: Flips the overlay to the target surface.
;
;   if (!Weave)
;       if (Interleaved)
;           double SP stride
;           if (EvenField)
;               adjust start down one line
;       else
;           if (EvenField)
;               shift VDDA left 1
;
;   ENTRY:
;          ESI  LPDDFLIPOVERLAYINFO
;                   DWORD dwSize
;                   LPDDSURFACEDATA current surface
;                   LPDDSURFACEDATA target surface
;                   DWORD flags
;          EDI  NULL
;
;   EXIT:
;          EAX  0 = success, 1 = error
;
;****************************************************************************
BeginProc DDFlipOverlay, High_Freq

    ; Get the new offset
    mov     edi, [esi]._DDFLIPOVERLAYINFO.lpTargetSurface    ; Point edi to target offset
    mov     ebx, [edi]._DDSURFACEDATA.dwOverlayOffset        ; Get the overlay offset

    test    flags, DISABLE_81D0DELAY
    jnz     DDFO81D0Immediate

    mov     MM81D0, ebx
    mov     eax, EnabledIRQs
    test    eax, DDIRQ_DISPLAY_VSYNC
    jnz     DDFO81E8

DDFO81D0Immediate:

    ; Program 81D0 immediately.  Assume hardware will handle
    ; having this SP write only take effect next display VSYNC
    mov     eax, MMIOLinearAddrBase
    add     eax, 081D0h
    call    SP_Set                  ; address in ebx

DDFO81E8:
    ; Get VideoPort flags (see if source data is interleaved or not)
    mov     eax, [edi]._DDSURFACEDATA.dwOverlayFlags

    test    eax, DDOVER_INTERLEAVED ; don't adjust VDDA if interleaved
    jnz     DDFOEnd

    test    eax, DDOVER_BOB         ; don't adjust VDDA if we're in weave mode
    jz      DDFOEnd

    test    flags, DISABLE_VDDABOB
    jnz     DDFOEnd                 ; don't adjust VDDA if disabled

DDFOBobNonInterleaved:

    ;
    ; Bob NonInterleaved
    ;
    ; - double VDDA
    ;

    mov     edx, [edi]._DDSURFACEDATA.dwOverlayDestHeight
    mov     esi, 1                      ; Calculate VDDA
    sub     esi, edx

    call    GetBufferPolarity           ; assumes address in ebx, result in eax
    test    eax, BUFFER_POLARITY_EVEN   ; polarity in bit 31
    jz      @f                          ; don't adjust VDDA if last field is odd
    shl     esi, 1                      ; even field -- adjust VDDA
@@:

    mov     eax, MMIOLinearAddrBase
    add     eax, 081E8h
    mov     ebx, [eax]
    and     ebx, 0FFFFE000h
    and     esi, 000001FFFh
    or      ebx, esi

    test    flags, DISABLE_81E8DELAY
    jnz     DDFOProgram81E8Immediate

    mov     MM81E8, ebx
    mov     eax, EnabledIRQs
    test    eax, DDIRQ_DISPLAY_VSYNC
    jnz     DDFOEnd

DDFOProgram81E8Immediate:

    ; Program 81E8 immediately.  Assume hardware will handle
    ; having this SP write only take effect next display VSYNC
    mov     eax, MMIOLinearAddrBase
    add     eax, 081E8h
    call    SP_Set

DDFOEnd:
    xor     eax, eax
    ret

EndProc DDFlipOverlay


;****************************************************************************
;
;   DDBobNextInterleavedEvenOverlayField
;
;   DESCRIPTION: Called when "bob" is used and a VPORT VSYNC occurs that does
;       not cause a flip to occur (e.g. bobbing while interleaved).  When
;       bobbing, the overlay must adjust itself on every VSYNC, so this
;       function notifies it of the VSYNCs that it doesn't already know
;       about (e.g. VSYNCs that trigger a flip to occur).
;
;   ENTRY:
;          ESI  LPDDBOBINFO
;                   DWORD dwSize
;                   LPDDSURFACE lpSurface
;          EDI  NULL
;
;   EXIT:
;          EAX  0 = success, 1 = error
;
;****************************************************************************
BeginProc DDBobNextField

    ;
    ; Bob Interleaved Even Field
    ;
    ; - Adjust starting line down by 1
    ;
    ; Get the new offset

    mov     edi, [esi]._DDBOBINFO.lpSurface
    mov     ebx, [edi]._DDSURFACEDATA.dwOverlayOffset       ; Get the overlay offset
    add     ebx, [edi]._DDSURFACEDATA.lPitch

    mov     MM81D0, ebx
    mov     eax, EnabledIRQs
    test    eax, DDIRQ_DISPLAY_VSYNC
    jnz     @f

    mov     eax, MMIOLinearAddrBase
    add     eax, 081D0h
    call    SP_Set

@@:
    sub     eax, eax
    ret
EndProc DDBobNextField

;****************************************************************************
;
;   DDSetState
;
;   DESCRIPTION: Called when the client wants to switch from bob to weave.
;       The overlay flags indicate which state to use. Only called for interleaved
;   surfaces.
;
;       NOTE: When this is called, the specified surface may not be
;       displaying the overlay (due to a flip).  Instead of failing
;       the call, change the bob/weave state for the overlay that would
;       be used if the overlay was flipped again to the specified surface.
;
;   ENTRY:
;          ESI  LPDDSTATEININFO
;                   DWORD dwSize
;                   LPDDSURFACEDATA overlay surface
;          EDI  LPDDSTATEOUTINFO
;                   DWORD dwSize
;                   DWORD dwSoftwareAutoflip
;                   DWORD dwSurfaceIndex        ; Return Current hardware autoflip
;
;   DURING:
;           EDI OverlaySuface
;           EBX Overlay Flags
;           ECX Overlay Pitch
;           EDX Overlay Src Height
;
;   EXIT:
;          EAX  0 = success, 1 = error
;
;****************************************************************************
BeginProc DDSetState

    ;
    ; We cannot hardware autoflip interleaved surfaces,
    ; so tell VPE to revert to software autoflipping
    ;
    mov     [edi]._DDSTATEOUTINFO.dwSoftwareAutoflip, 0
    mov     [edi]._DDSTATEOUTINFO.dwSurfaceIndex, 0

    ; Get the new overlay flags
    mov     edi, [esi._DDSTATEININFO.lpSurfaceData]  ; Point edi to overlay surface
    mov     ebx, [edi._DDSURFACEDATA.dwOverlayFlags] ; Get the new flags

    DPF     SetStateString

    mov     ecx, [edi]._DDSURFACEDATA.lPitch
    mov     edx, [edi]._DDSURFACEDATA.dwOverlaySrcHeight

    test    ebx, DDOVER_BOB
    jz      DDSSProgram                  ; Weave

DDSSBob:

    shl     ecx, 1                       ; Double Stride
    shr     edx, 1                       ; Halve Height
    jmp     DDSSProgram

DDSSProgram:

    ; Secondary Stream Stride
    mov     eax, MMIOLinearAddrBase
    add     eax, 081D8h
    mov     ebx, [eax]
    and     ebx, 0FFFFF000h
    and     ecx, not 0FFFFF000h
    or      ebx, ecx                     ; ecx==Pitch no longer needed
    call    SP_Set

    ; K1 Vertical Scale Factor (h0 - 1)
    mov     eax, MMIOLinearAddrBase
    add     eax, 081E0h
    mov     ebx, [eax]
    and     ebx, 0FFFFF800h
    mov     ecx, 1
    sub     ecx, edx
    and     ecx, not 0FFFFF800h
    or      ebx, ecx
    call    SP_Set

    ; K2 Vertical Scale Factor (h0 - h1)
    mov     eax, MMIOLinearAddrBase
    add     eax, 081E4h
    mov     ebx, [eax]
    and     ebx, 0FFFFF800h
    sub     edx, [edi]._DDSURFACEDATA.dwOverlayDestHeight
    and     edx, not 0FFFFF800h
    or      ebx, edx
    call    SP_Set

    sub         eax, eax
    ret

EndProc DDSetState

;****************************************************************************
;
;   DDLock
;
;   DESCRIPTION: Called when the client wants to lock the surface to
;       access the frame buffer. The driver doens't have to do anything,
;       but it can if it needs to.
;
;   ENTRY:
;          ESI  LPDDLOCKININFO
;                   DWORD dwSize
;                   LPDDSURFACEDATA surface
;          EDI  LPDDLOCKOUTINFO
;                   DWORD dwSize
;                   DWORD Pointer to a pointer to the surface
;
;   EXIT:
;          EAX  0 = success, 1 = error
;
;****************************************************************************

BeginProc DDLock

    sub         eax, eax
    ret

EndProc DDLock

;****************************************************************************
;
;   DDSkipNextVideoPortField
;
;   DESCRIPTION: Called when they want to skip the next field, usually
;       to undo a 3:2 pulldown but also for decreasing the frame rate.
;       The driver should not lose the VBI lines if dwVBIHeight contains
;       a valid value.
;
;   ENTRY:
;          ESI  LPDDSKIPINFO
;                   DWORD dwSize
;                   LPDDVIDEOPORTDATA video port
;          EDI  NULL
;
;   EXIT:
;          EAX  0 = success, 1 = error
;
;****************************************************************************
BeginProc DDSkipNextField

    mov     edi, [esi._DDSKIPINFO.lpVideoPortData]

    mov     ecx, [esi._DDSKIPINFO.dwSkipFlags]
    test    ecx, DDSKIP_SKIPNEXT
    jz      @f
    ;DDSKIP_SKIPNEXT
    ;
    ; Use V Decimation to prevent field from being written to surface
    ;
    mov     ebx, 0FFFFFFFFh
    jmp     SNF_Set
@@:
    ;DDSKIP_ENABLENEXT
    ;
    ; Enable next field: restore actual V decimation pattern
    ;
    mov     ebx, [edi.DDVIDEOPORTDATA.dwDriverReserved1]

SNF_Set:
    mov     eax, MMIOLinearAddrBase
    add     eax, LPB_VDECIMATION_PATTERN
    call    LPB_Set

    sub         eax, eax
    ret

EndProc DDSkipNextField

;****************************************************************************
;
;   DDGetFieldPolarity
;
;   DESCRIPTION: Returns the polarity of the current field being written
;       to the specified video port.
;
;   ENTRY:
;          ESI  LPDDPOLARITYININFO
;                   DWORD dwSize
;                   LPDDVIDEOPORTDATA
;          EDI  LPDDPOLARITYOUTINFO
;                   DWORD dwSize
;                   DWORD bPolority (even field = TRUE, odd field = FALSE)
;
;   EXIT:
;          EAX  0 = success, 1 = error
;          ECX  0 = odd,     1 = even
;
;
;   MODIFIES:
;          EAX, EBX, ECX
;****************************************************************************

BeginProc DDGetFieldPolarity

    ; Read if current field will be even or odd
    ; ecx = 1 if current field is EVEN

    mov     eax, MMIOLinearAddrBase
    add     eax, LPB_MODE
    mov     ebx, [eax]
    xor     ecx, ecx                    ; assume odd field
    test    ebx, LPB_EVENFIELD_V2
    jz      @f
    mov     ecx, 1                      ; even field
@@:
    cmp     edi, 0
    je      @f
    mov     [edi._DDPOLARITYOUTINFO.bPolarity], ecx
@@:
    sub     eax, eax
    ret

EndProc DDGetFieldPolarity
;****************************************************************************
;
;   DDGetCurrentAutoflipSurface
;
;   DESCRIPTION: Returns the current surface receiving data from the
;       video port while autoflipping is taking palce.  Only called when
;   hardware autoflipping.
;
;   ENTRY:
;          ESI  LPDDGETAUTOFLIPINFO
;                   DWORD               dwSize
;          EDI  LPDDGETAUTOFLIPINFO
;                   DWORD               dwSize
;                   DWORD               dwSurfaceIndex
;
;   EXIT:
;          EAX  0 = success, 1 = error
;
;****************************************************************************

BeginProc DDGetCurrentAutoflipSurface

    ; S3 has 2 autoflippable hardware buffers

    ; assume Surface 0
    mov     [edi]._DDGETAUTOFLIPOUTINFO.dwSurfaceIndex, 0

    ; See if it is, in fact, Surface 1
    mov     eax, MMIOLinearAddrBase
    add     eax, LPB_MODE
    mov     ebx, [eax]
    test    ebx, LPB_BASE1ACTIVE_V3
    jz      @f

    mov     [edi]._DDGETAUTOFLIPOUTINFO.dwSurfaceIndex, 1

@@:
    mov     eax, 0
    ret

EndProc DDGetCurrentAutoflipSurface

;****************************************************************************
;
;   DDGetPreviousAutoflipSurface
;
;   DESCRIPTION: Returns the surface that received the data from the
;       previous field of video port while autoflipping is taking palce. Only
;   called for hardware autoflipping.
;
;   ENTRY:
;          ESI  LPDDGETAUTOFLIPINFO
;                   DWORD               dwSize
;          EDI  LPDDGETAUTOFLIPINFO
;                   DWORD               dwSize
;                   DWORD               dwSurfaceIndex
;
;   EXIT:
;          EAX  0 = success, 1 = error
;
;****************************************************************************

BeginProc DDGetPreviousAutoflipSurface

    ; S3 has 2 hardware autoflippable buffers

    call    DDGetCurrentAutoflipSurface

    mov     eax, [edi]._DDGETAUTOFLIPOUTINFO.dwSurfaceIndex
    xor     eax, 1
    mov     [edi]._DDGETAUTOFLIPOUTINFO.dwSurfaceIndex, eax

    mov     eax, 0
    ret

EndProc DDGetPreviousAutoflipSurface

BeginProc DDUpdatePolarity
;
; Timer Callback, set by FlipVideoPort.  Set to call back 1/4 through the
; field (4ms), when the field polarity is accurately known by the hardware
; If ECX > 12, we're going to read the wrong polarity information, since
; it's about 16ms total per field at 60Hz
;
; INPUTS
;   ECX = # of milliseconds Win9X was tardy
;   EDX = address of buffer array entry to update with polarity
; MODIFIES
;   EAX, EBX, ECX, EDI

    mov     ebx, [edx]
    and     ebx, BUFFER_MASK

    xor     edi, edi
    push    ebx
    call    DDGetFieldPolarity ; result in ecx
    pop     ebx
    test    ecx,ecx
    jz      @f
    or      ebx, BUFFER_POLARITY_EVEN
@@:
    or      ebx, BUFFER_POLARITY_KNOWN
    mov     [edx], ebx

    ret
EndProc DDUpdatePolarity

BeginProc GetBufferPolarity
; given a buffer address that the overlay is flipping to,
; this function scans the internal buffer list to figure
; out the odd/even polarity of this buffer
; INPUTS
;   ebx = buffer address
; OUTPUTS
;   eax = buffer polarity

    push    edx
    mov     edx, vpindex

GBPScan:
    mov     eax, [edx]
    and     eax, BUFFER_MASK
    cmp     ebx, eax
    jb      GBPNot                      ; out of range low
    add     eax, buffersize
    cmp     ebx, eax
    jae     GBPNot                      ; out of range high
    mov     oindex, edx                 ; match found
    mov     eax, [edx]                  ; get upper bits back
    test    eax, BUFFER_POLARITY_KNOWN
    jnz     GBPEnd
    ; uh-oh UpdatePolarity has not yet been called for this buffer
    bufferdec edx                       ; so grab polarity of one buffer older
    TRIGGER METRICS_POLARITYGUESS
    mov     eax, [edx]
    xor     eax, BUFFER_POLARITY_EVEN   ; and invert
    jmp     GBPEnd
GBPNot:
    bufferdec edx
    cmp     edx, vpindex
    jne     GBPScan
    TRIGGER METRICS_OBADBUFFER

GBPEnd:
    mov     edx, oindex
    cmp     edx, vpindex
    jne     @f
    TRIGGER METRICS_TEAR
@@:
    pop     edx
    ret
EndProc GetBufferPolarity

BeginProc LPB_Set
    push    ecx
    mov     [eax], ebx
    mov     ecx, [eax]
    pop     ecx
    ret
EndProc LPB_Set

BeginProc SP_Set
    push    ecx
    mov     [eax], ebx
    mov     ecx, [eax]
    pop     ecx
    ret
EndProc SP_Set

public  MiniVDD_Debug_Query
BeginProc MiniVDD_Debug_Query
IFDEF DEBUG
    mov     eax, EnabledIRQs
    test    eax, eax
    jnz     @f
    DPF     DisabledString
    jmp     QueryExit
@@:
    DPF     MetricsString
    mov     ebx, offset32 OVSYNCString
    mov     ecx, metrics_persec+METRICS_OVSYNC*4
    mov     edx, metrics+METRICS_OVSYNC*4
    DPF     ValuesString
    mov     ebx, offset32 VPVSYNCString
    mov     ecx, metrics_persec+METRICS_VPVSYNC*4
    mov     edx, metrics+METRICS_VPVSYNC*4
    DPF     ValuesString
    mov     ebx, offset32 TOTALIRQDebugString
    mov     ecx, metrics_persec+METRICS_IRQ*4
    mov     edx, metrics+METRICS_IRQ*4
    DPF     ValuesString
    mov     ebx, offset32 PolarityGuessString
    mov     ecx, metrics_persec+METRICS_POLARITYGUESS*4
    mov     edx, metrics+METRICS_POLARITYGUESS*4
    DPF     ValuesString
    mov     ebx, offset32 SkippedFieldsString
    mov     ecx, metrics_persec+METRICS_SKIP*4
    mov     edx, metrics+METRICS_SKIP*4
    DPF     ValuesString
    mov     ebx, offset32 TearingString
    mov     ecx, metrics_persec+METRICS_TEAR*4
    mov     edx, metrics+METRICS_TEAR*4
    DPF     ValuesString
    mov     ebx, offset32 FlipLateString
    mov     ecx, metrics_persec+METRICS_FLIPLATE*4
    mov     edx, metrics+METRICS_FLIPLATE*4
    DPF     ValuesString

    DPF     QualityString

    mov     eax, MMIOLinearAddrBase

    mov     ebx, [eax + LPB_INPUT_SIZE]
    mov     ecx, ebx
    and     ebx, 0FFFh
    add     ebx, 2
    shr     ebx, 1                      ; only works for 8 bit port width
    shr     ecx, 16
    and     ecx, 01FFh
    DPF     InputSizeString

    mov     ebx, [eax + LPB_MODE]
    test    ebx, LPB_SKIPFIELDS
    jz      @f
    DPF     SkippingString
    jmp     QueryModeEnd
@@:

    mov     ebx, [eax+081D0h]           ; height of overlay source
    mov     eax, MMIOLinearAddrBase
    mov     ecx, [eax+LPB_INPUT_SIZE]
    add     ecx, LPB_INPUT_HEIGHT_MASK
    shl     ecx, LPB_INPUT_HEIGHT_SHIFT

    cmp     ecx, ebx
    jb      QueryWeave
    DPF     BobString
    jmp     QueryModeEnd
QueryWeave:
    DPF     WeaveString
QueryModeEnd:

    mov     ebx, [eax + LPB_MODE]
    test    ebx, LPB_DEINTERLACE_V2
    jnz     QueryDeinterlacing
    DPF     NonInterleavedString
    jmp     @f
QueryDeinterlacing:
    DPF     InterleavedString
@@:

    mov     ebx, [eax + 81E8h]
    test    ebx, 08000h
    jz      @f
    DPF     VInterpolationString
@@:

    sub     ebx, ebx
    mov     edx, vpindex
    mov     ecx, [edx]
    and     ecx, BUFFER_MASK
@@:
    inc     ebx
    cmp     ebx,10
    jae     error1
    bufferdec edx
    mov     esi, [edx]
    and     esi, BUFFER_MASK
    cmp     ecx, esi
    jne     @b
    DPF     BuffersString

error1:

    mov     eax, MMIOLinearAddrBase
    mov     ebx, [eax+LPB_VDECIMATION_PATTERN]
    DPF     VDecimationString

    mov     ebx, [eax+LPB_HDECIMATION_PATTERN]
    DPF     HDecimationString

QueryExit:
    clc                                        ; valid return code
ENDIF
    ret
EndProc   MiniVDD_Debug_Query

VxD_LOCKED_CODE_ENDS
end
