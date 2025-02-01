/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    GuidIso.h

Abstract:

 The below GUID is used to generate symbolic links to
  driver instances created from user mode

Environment:

    Kernel & user mode

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.

Revision History:

    2/11/98 : created

--*/
#ifndef GUIDISOH_INC
#define GUIDISOH_INC

#include <initguid.h>


// {A1155B78-A32C-11d1-9AED-00A0C98BA608} for IsoUsb.sys
DEFINE_GUID(GUID_CLASS_I82930_ISO, 
0xa1155b78, 0xa32c, 0x11d1, 0x9a, 0xed, 0x0, 0xa0, 0xc9, 0x8b, 0xa6, 0x8);



#endif // end, #ifndef GUIDISOH_INC
