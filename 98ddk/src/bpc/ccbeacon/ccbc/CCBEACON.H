//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// ccbeacon.h : Defines close caption display codes.
//
#define _1EDM_	"\x014\x02c"
#define _1RU4_	"\x014\x027"
#define _1WHT_	"\x011\x020"
#define _1GRN_	"\x011\x022"
#define _ZERO_	"\x080\x080"
#define _1CR_	"\x014\x02d"
#define _2RTD_	"\x01c\x02b"
#define	_2TR_	"\x01c\x02a"
#define _2CR_	"\x01c\x02d"

#if 1
#define PATTERN_LEN 32
#else
#define PATTERN_LEN	28
#endif

#define XLINK_LEN 76

void BuildPatternString(CString &);
void BuildCrosslinkString(CString &);