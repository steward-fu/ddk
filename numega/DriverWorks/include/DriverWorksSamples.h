// DriverWorksSamples.h - default resource compiler definitions for samples
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#define VER_FILEFLAGSMASK			VS_FFI_FILEFLAGSMASK
#define VER_FILEOS          		VOS_NT_WINDOWS32
#define VER_FILEFLAGS				0

#define VER_COMPANYNAME_STR 		"Compuware Corporation"
#define VER_PRODUCTNAME_STR 		"DriverStudio"
#define VER_LEGALCOPYRIGHT_YEARS    "2004"
#define VER_LEGALCOPYRIGHT_STR		"Copyright (C) " VER_LEGALCOPYRIGHT_YEARS " Compuware Corporation"
#define VER_LEGALTRADEMARKS_STR     ""
#define VER_PRODUCTVERSION			3,20,000,000
#define VER_PRODUCTVERSION_STR		"3.20"
#define VER_FILEVERSION				VER_PRODUCTVERSION
#define VER_FILEVERSION_STR			VER_PRODUCTVERSION_STR
#define VER_PRODUCTVERSION_DW       (0x01000000 | VER_PRODUCTBUILD)

#define VER_FILETYPE				VFT_DRV
#define VER_FILESUBTYPE				VFT2_DRV_SYSTEM
