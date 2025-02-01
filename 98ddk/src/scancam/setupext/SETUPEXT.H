/****************************g************************************************
 *
 *  SetupExt.H
 *
 *  Copyright (c) Microsoft Corporation 1996-1998
 *  All rights reserved
 *
 ***************************************************************************/

#include <windows.h>

#include "sti.h"
#include "stierr.h"

#if !defined(DLLEXPORT)
#define DLLEXPORT __declspec( dllexport )
#endif

#define MAX_LEN 255
#define ENUM "Enum\\"
#define LPTENUM "Enum\\LptEnum\\"
#define PORTNAME "PORTNAME"
#define STI_KEY "System\\CurrentControlSet\\Services\\Class\\"
#define CREATEFN "CreateFileName"
#define DRIVER "Driver"
