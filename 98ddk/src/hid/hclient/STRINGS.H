#ifndef __STRINGS_H__
#define __STRINGS_H__

VOID
Strings_CreateDataBufferString(
    IN  PCHAR    DataBuffer,
    IN  ULONG    DataBufferLength,
    IN  ULONG    NumBytesToDisplay,
    IN  ULONG    DisplayBlockSize,
    OUT PCHAR    *BufferString
);

VOID
Strings_StringToUnsigned(
    IN  PCHAR   InString,
    IN  ULONG   Base,
    OUT PCHAR   *endp,
    OUT PULONG  pValue
);

BOOL
Strings_StringToUnsignedList(
    IN  PCHAR   InString,
    IN  ULONG   UnsignedSize,
    IN  ULONG   Base,
    OUT PCHAR   *UnsignedList,
    OUT PULONG  nUnsigneds
);

#endif
