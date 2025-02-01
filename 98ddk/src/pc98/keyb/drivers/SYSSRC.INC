;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

        PAGE
;
;       SYSTEM MACRO
;
DELAY   MACRO   DEV,ACT,MOD

        IFIDN   <DEV>,<8237>
                IFIDN   <ACT>,<O-O>
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<O-I>
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<>
        JMP     SHORT   $+2
                ENDIF
        ENDIF

        IFIDN   <DEV>,<8253>
                IFIDN   <ACT>,<O-O>
        REPT    2
        JMP     SHORT   $+2
        ENDM
                ENDIF
                IFIDN   <ACT>,<I-I>
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<O-I>
        REPT    2
        JMP     SHORT   $+2
        ENDM
                ENDIF
                IFIDN   <ACT>,<I-O>
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<>
        REPT    2
        JMP     SHORT   $+2
        ENDM
                ENDIF
        ENDIF

        IFIDN   <DEV>,<8255>
                IFIDN   <ACT>,<O-O>
        REPT    2
        JMP     SHORT   $+2
        ENDM
                ENDIF
                IFIDN   <ACT>,<I-I>
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<O-I>
        REPT    2
        JMP     SHORT   $+2
        ENDM
                ENDIF
                IFIDN   <ACT>,<I-O>
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<>
        REPT    2
        JMP     SHORT   $+2
        ENDM
                ENDIF
        ENDIF

        IFIDN   <DEV>,<8259>
                IFIDN   <ACT>,<O-O>
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<I-I>
                ENDIF
                IFIDN   <ACT>,<O-I>
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<I-O>
                ENDIF
                IFIDN   <ACT>,<>
        JMP     SHORT   $+2
                ENDIF
        ENDIF

        IFIDN   <DEV>,<8251>
                IFIDN   <ACT>,<O-O>
                        IFIDN   <MOD>,<INIT>
        REPT    4
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<ASYNC>
        REPT    5
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<SYNC>
        REPT    10
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                ENDIF
        ENDIF

        IFIDN   <DEV>,<765>
        ENDIF

        IFIDN   <DEV>,<7220>
                IFIDN   <ACT>,<O-O>
                        IFIDN   <MOD>,<GRAPH>
        REPT    4
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<TEXT>
        REPT    2
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                ENDIF
                IFIDN   <ACT>,<I-I>
                        IFIDN   <MOD>,<GRAPH>
        REPT    3
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<TEXT>
        JMP     SHORT   $+2
                        ENDIF
                ENDIF
                IFIDN   <ACT>,<O-I>
                        IFIDN   <MOD>,<GRAPH>
        REPT    4
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<TEXT>
        REPT    2
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                ENDIF
                IFIDN   <ACT>,<I-O>
                        IFIDN   <MOD>,<GRAPH>
        REPT    3
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<TEXT>
        JMP     SHORT   $+2
                        ENDIF
                ENDIF
        ENDIF

        IFIDN   <DEV>,<7201>
                IFIDN   <ACT>,<O-O>
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<O-I>
        JMP     SHORT   $+2
                ENDIF
        ENDIF

        IFIDN   <DEV>,<7210>
                IFIDN   <ACT>,<O-O>
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<O-I>
        JMP     SHORT   $+2
                ENDIF
        ENDIF

        ENDM


DELAYNP MACRO   DEV,ACT,MOD

        IFIDN   <DEV>,<8237>
                IFIDN   <ACT>,<O-O>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<O-I>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
        ENDIF

        IFIDN   <DEV>,<8253>
                IFIDN   <ACT>,<O-O>
        REPT    2
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                ENDIF
                IFIDN   <ACT>,<I-I>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<O-I>
        REPT    2
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                ENDIF
                IFIDN   <ACT>,<I-O>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<>
        REPT    2
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                ENDIF
        ENDIF

        IFIDN   <DEV>,<8255>
                IFIDN   <ACT>,<O-O>
        REPT    2
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                ENDIF
                IFIDN   <ACT>,<I-I>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<O-I>
        REPT    2
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                ENDIF
                IFIDN   <ACT>,<I-O>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<>
        REPT    2
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                ENDIF
        ENDIF

        IFIDN   <DEV>,<8259>
                IFIDN   <ACT>,<O-O>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<I-I>
                ENDIF
                IFIDN   <ACT>,<O-I>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<I-O>
                ENDIF
                IFIDN   <ACT>,<>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
        ENDIF

        IFIDN   <DEV>,<8251>
                IFIDN   <ACT>,<O-O>
                        IFIDN   <MOD>,<INIT>
        REPT    6               ;OLD = 4
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<ASYNC>
        REPT    7               ;OLD = 5
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<SYNC>
        REPT    14              ;OLD = 10
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                ENDIF
        ENDIF

        IFIDN   <DEV>,<765>
        ENDIF

        IFIDN   <DEV>,<7220>
                IFIDN   <ACT>,<O-O>
                        IFIDN   <MOD>,<GRAPH>
        REPT    6               ;OLD = 4
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<TEXT>
        REPT    2
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                ENDIF
                IFIDN   <ACT>,<I-I>
                        IFIDN   <MOD>,<GRAPH>
        REPT    4               ;OLD = 3
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<TEXT>
        OUT     5FH,AL
        JMP     SHORT   $+2
                        ENDIF
                ENDIF
                IFIDN   <ACT>,<O-I>
                        IFIDN   <MOD>,<GRAPH>
        REPT    6               ;OLD = 4
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<TEXT>
        REPT    2
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                ENDIF
                IFIDN   <ACT>,<I-O>
                        IFIDN   <MOD>,<GRAPH>
        REPT    4               ;OLD = 3
        OUT     5FH,AL
        JMP     SHORT   $+2
        ENDM
                        ENDIF
                        IFIDN   <MOD>,<TEXT>
        OUT     5FH,AL
        JMP     SHORT   $+2
                        ENDIF
                ENDIF
        ENDIF

        IFIDN   <DEV>,<7201>
                IFIDN   <ACT>,<O-O>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<O-I>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
        ENDIF

        IFIDN   <DEV>,<7210>
                IFIDN   <ACT>,<O-O>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
                IFIDN   <ACT>,<O-I>
        OUT     5FH,AL
        JMP     SHORT   $+2
                ENDIF
        ENDIF

        ENDM

