//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// ccbeacon.cpp : Defines functions that insert TV crossover links.
//
#include <afx.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <assert.h>

#include "ccbeacon.h"

void BuildPatternString(CString &s)
{
	//
	// s is the string for the beacon
	//
	unsigned char c;

	s.Format("%s%s", _1RU4_, _1WHT_);
	for (c = 'A'; c <= 'Z'; c++)
	{
		s += c;
	}
	s += _1CR_;
}


void BuildCrosslinkString(CString &s)
{
	//
	// s is the string for the crosslink
	//
	s.Format("%s<http://webtv.microsoft.com>[type:SPONSOR][name:WebTV for Windows][3592]%s", _2TR_, _2CR_);
}

