//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// main.cpp : main source to insert a TV crossover link.
//
#include <afx.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <conio.h>
#include <assert.h>

#include "ccbeacon.h"
#include "getopt.h"


BOOL ReadCrossLinks(LPTSTR szFileName, CStringList& list)
{
#define BUF_SIZE 512
	FILE* f;
	char szBuffer[BUF_SIZE];

	list.RemoveAll();
	f = fopen(szFileName, "r");

	if (f == NULL)
	{
		fprintf(stderr, "unable to open file %s\n", szFileName);
		return(FALSE);
	}

	while(!feof(f) && !ferror(f))
	{
		int len;
		fgets(szBuffer, BUF_SIZE, f);
		len = strlen(szBuffer);

		if (len && (szBuffer[len-1] == '\n'))
		{
			szBuffer[len-1] = 0;
			len--;
		}

		if ((len > 2) && (szBuffer[0]=='/') && (szBuffer[1]=='/'))
		{
			// looks like a comment
			fprintf(stderr, "comment: %s\n", &szBuffer[2]);
			// set buffer to zero so we reject it below
			szBuffer[0] = 0;
		}

		if (strlen(szBuffer))
		{
			CString strTemp;
			//
			// enclose TVCOL in TEXT-2 codes
			// we add a space before the ending _2CR_ if odd length
			// to make sure we have an even length string
			//
			// this way all control codes are aligned
			//
			strTemp.Format("%s%s%s%s", 
				_2RTD_, 
				szBuffer,
				((strlen(szBuffer) % 2) ? " " : ""),
				_2CR_);
			list.AddTail(strTemp);
		}

	}
	return(TRUE);
}


void main(int argc, char *argv[])
{
	CString strPattern, strCrossLink;
	CStringList listCrossLinks;
	unsigned int uDelay = 0;
	unsigned int uCrossLinkFreq = 100;
	char* szFile = NULL;
	char chSwitch;

#define OPTARGS "d:i:f:h"
	//
	// set defaults
	//
	while ((chSwitch = getopt(argc, argv, OPTARGS)) != EOF)
	{
		switch (chSwitch)
		{
		case 'd' :
			uDelay = atoi(optarg);
			break;
		case 'h' :
			fprintf(stderr, "ccbeacon -d {uDelay per line} -i {input file for xlinks} -f {frequency of xlinks}\n");
			return;
		case 'i' :
			szFile = optarg;
			break;
		case 'f' :
			uCrossLinkFreq = atoi(optarg);
			break;
		default :
			;
		}
	}

	int result = _setmode(_fileno(stdout), _O_BINARY);
	if (result == -1)
	{
		fprintf(stderr, "Can't set stdout to binary\n");
		exit(-1);
	}

	if (szFile)
	{
		if (!ReadCrossLinks(szFile, listCrossLinks))
		{
			szFile = NULL;
		}
	}
	if (listCrossLinks.IsEmpty())
	{
		BuildCrosslinkString(strCrossLink);
		listCrossLinks.AddTail(strCrossLink);
	}

	fprintf(stderr, "CC Beacon\n");
	fprintf(stderr, "- delay per line (ms) = %d\n", uDelay);
	if (szFile)
	{
		fprintf(stderr, "- crosslinks filename = %s\n", szFile);
	}
	fprintf(stderr, "- number of crosslinks = %d\n", listCrossLinks.GetCount());

	POSITION pos;
	for (pos = listCrossLinks.GetHeadPosition(); pos != NULL;)
	{
		strCrossLink = listCrossLinks.GetNext(pos);
		fprintf(stderr, "- (%02d) %s...\n", strCrossLink.GetLength(), (LPCTSTR) (strCrossLink.Left(50)));
	}
	fprintf(stderr, "- crosslinks frequency = %d\n", uCrossLinkFreq);

	printf("\006\006");		// reset encoder (Ctrl-F Ctrl-F)
	printf("\003\r");		// (Ctrl-C)
	printf("\0013 2\r");	// enter pass-thru mode (take data from encoder's serial port)
							// (Ctrl-A) 3 2
							// 3 = pass-thru mode
							// 2 = align valid controls

	BuildPatternString(strPattern);

	unsigned int i = 0;
	pos = listCrossLinks.GetHeadPosition();

	while (!kbhit()) //(1)
	{
		if (i++ == uCrossLinkFreq)
		{
			strCrossLink = listCrossLinks.GetNext(pos);
			if (pos == NULL)
			{
				pos = listCrossLinks.GetHeadPosition();
			}
			fprintf(stdout, (LPCTSTR) strCrossLink);
			i = 0;
		}
		else
		{
			fprintf(stdout, (LPCTSTR) strPattern);
		}
		if (uDelay)
			Sleep(uDelay);
	}
}