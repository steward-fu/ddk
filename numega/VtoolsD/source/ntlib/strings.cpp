// strings.cpp - implementation of NT string functions for VtoolsD
// Copyright (c) 1997 Compuware Corporation

#include <vdw.h>

VOID NTAPI RtlCopyString(
    PSTRING DestinationString,
    PSTRING SourceString
    )
{
	ULONG copyLength = (DestinationString->MaximumLength < SourceString->Length) ?
				 DestinationString->MaximumLength : SourceString->Length ;
	RtlCopyMemory(DestinationString->Buffer,SourceString->Buffer,copyLength ) ;
}

VOID NTAPI RtlCopyUnicodeString(
    PUNICODE_STRING DestinationString,
    PUNICODE_STRING SourceString
    )
{
	ULONG copyLength = (DestinationString->MaximumLength < SourceString->Length) ?
				 DestinationString->MaximumLength : SourceString->Length ;
	RtlCopyMemory(DestinationString->Buffer,SourceString->Buffer,copyLength ) ;
}


VOID NTAPI RtlInitUnicodeString(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    )
{
	DestinationString->Length = ustrsiz(SourceString);	
	DestinationString->MaximumLength = DestinationString->Length;
	DestinationString->Buffer = (PWSTR)SourceString;
}


NTSTATUS NTAPI RtlAppendUnicodeStringToString (
    PUNICODE_STRING Destination,
    PUNICODE_STRING Source
    )
{
	USHORT TotalSize = Destination->Length + Source->Length;

	if (TotalSize > Destination->MaximumLength)
		return STATUS_BUFFER_TOO_SMALL;

	memcpy(
		(PCHAR)Destination->Buffer + Destination->Length,
		Source->Buffer,
		Source->Length
		);

	Destination->Length = TotalSize;

	return STATUS_SUCCESS;
}

NTSTATUS NTAPI RtlAnsiStringToUnicodeString(
    PUNICODE_STRING Destination,
    PANSI_STRING Source,
    BOOLEAN AllocateDestination
    )
{
	_QWORD result;

	if (AllocateDestination)
	{
		Destination->Buffer = new (PagedPool) WCHAR[Source->Length];
		if ( Destination->Buffer == NULL )
			return STATUS_INSUFFICIENT_RESOURCES;
		else
			Destination->MaximumLength = Source->Length*sizeof(WCHAR);
	}	
	else if (Source->Length*sizeof(WCHAR) > ULONG(Destination->MaximumLength))
		return STATUS_BUFFER_TOO_SMALL;		

	BCSToUni(
		Destination->Buffer,
		(PUCHAR)Source->Buffer,
		Source->Length,
		BCS_WANSI,
		&result
		);

	Destination->Length = result.ddLower;

	return STATUS_SUCCESS;
}

NTSTATUS NTAPI RtlUnicodeStringToAnsiString(
    PANSI_STRING Destination,
    PUNICODE_STRING Source,
    BOOLEAN AllocateDestination
    )
{
	_QWORD result;
	ULONG Length = Source->Length/sizeof(WCHAR);

	if (AllocateDestination)
	{
		Destination->Buffer = new (PagedPool) CHAR[Length+1];
		if ( Destination->Buffer == NULL )
			return STATUS_INSUFFICIENT_RESOURCES;
		else
			Destination->MaximumLength = Length+1;
	}
	else if (Length > ULONG(Destination->MaximumLength))
		return STATUS_BUFFER_TOO_SMALL;		

	UniToBCS(
		(PUCHAR)Destination->Buffer,
		Source->Buffer,
		Source->Length,
		Destination->MaximumLength,
		BCS_WANSI,
		&result
		);

	Destination->Length = result.ddLower;

	return STATUS_SUCCESS;
}


VOID NTAPI RtlInitAnsiString(PANSI_STRING DestinationString, PCSZ SourceString)
{
	DestinationString->Length = strlen(SourceString);	
	DestinationString->MaximumLength = DestinationString->Length;
	DestinationString->Buffer = PSZ(SourceString);
}

VOID NTAPI RtlFreeAnsiString(PANSI_STRING AnsiString)
{
	delete AnsiString->Buffer;
}

VOID NTAPI RtlFreeUnicodeString(PUNICODE_STRING String)
{
	delete String->Buffer;
}

LONG NTAPI RtlCompareUnicodeString(
    PUNICODE_STRING String1,
    PUNICODE_STRING String2,
    BOOLEAN CaseInsensitive
    )
{
	LONG result=0;
	int i = 0;

	int nChar1 = String1->Length/sizeof(WCHAR);
	int nChar2 = String2->Length/sizeof(WCHAR);
	int nChar = min(nChar1, nChar2);

	while (i < nChar)
	{
		WCHAR c1, c2;

		c1 = String1->Buffer[i];
		c2 = String2->Buffer[i];

		if (CaseInsensitive)
		{
			if ( ('A' <= c1) && (c1 <= 'Z')  )
				c1 += ('a'-'A');

			if ( ('A' <= c2) && (c2 <= 'Z')  )
				c2 += ('a'-'A');
		}

		if ( c1 < c2 )
		{
			result = -1;
			break;
		}
		else if ( c1 > c2 )
		{
			result = 1;
			break;
		}
		else
			i++;
	}

	if ( result == 0 )
		return String1->Length - String2->Length;

	else
		return result;
}

LONG NTAPI RtlCompareString(
    PANSI_STRING String1,
    PANSI_STRING String2,
    BOOLEAN CaseInsensitive
    )
{
	LONG result=0;
	int i = 0;

	int nChar1 = String1->Length;
	int nChar2 = String2->Length;
	int nChar = min(nChar1, nChar2);

	while (i < nChar)
	{
		CHAR c1, c2;

		c1 = String1->Buffer[i];
		c2 = String2->Buffer[i];

		if (CaseInsensitive)
		{
			if ( ('A' <= c1) && (c1 <= 'Z')  )
				c1 += ('a'-'A');

			if ( ('A' <= c2) && (c2 <= 'Z')  )
				c2 += ('a'-'A');
		}

		if ( c1 < c2 )
		{
			result = -1;
			break;
		}
		else if ( c1 > c2 )
		{
			result = 1;
			break;
		}
		else
			i++;
	}

	if ( result == 0 )
		return String1->Length - String2->Length;

	else
		return result;
}

NTSTATUS NTAPI RtlIntegerToUnicodeString(
	IN ULONG  Value,
	IN ULONG  Base,					/* optional */
	IN OUT PUNICODE_STRING  String
	)
{
	char abuf[32];

	ANSI_STRING astr; astr.Length=0; astr.MaximumLength=sizeof(abuf); astr.Buffer=abuf;

	memset(abuf, 0, sizeof(abuf));

	ultoa(Value, abuf, Base);

	RtlInitAnsiString(&astr, abuf);

	return RtlAnsiStringToUnicodeString(String, &astr, FALSE);
}
