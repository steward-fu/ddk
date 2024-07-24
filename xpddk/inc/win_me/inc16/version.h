#define OFFICIAL   1
#define FINAL      1


/****************************************************************************
 *                                                                          *
 *      VERSION.H        -- Version information for internal builds         *
 *                                                                          *
 *      This file is only modified by the official builder to update the    *
 *      VERSION, VER_PRODUCTVERSION and VER_PRODUCTVERSION_STR values       *
 *                                                                          *
 ****************************************************************************/

/*XLATOFF*/
#ifndef VS_FF_DEBUG
/* ver.h defines constants needed by the VS_VERSION_INFO structure */
#include "ver.h"
#endif
/*XLATON*/

/*--------------------------------------------------------------*/
/*                                                              */
/*                    CHANGING VERSION?                         */
/*                                                              */
/*                      PLEASE READ!                            */
/*                                                              */
/* The version has BOTH hex and string representations.  Take   */
/* care that the string version components are PROPERLY         */
/* CONVERTED TO HEX and that the hex values are INSERTED INTO   */
/* THE CORRECT POSITION in the hex versions.                    */
/*                                                              */
/* Suppose the version was being defined as:                    */
/*                                                              */
/*           #define VERSION  "9.99.1234"                       */
/*                                                              */
/* The other string preresentations of the version would be:    */
/*                                                              */
/*           #define VER_PRODUCTVERSION_STR  "9.99.1234\0"      */
/*           #define VER_PRODUCTVERSION       9,99,0,1234       */
/*                                                              */
/* The hex versions would NOT be 0x0999????.  The correct       */
/* definitions are:                                             */
/*                                                              */
/*    #define VER_PRODUCTVERSION_BUILD 1234                     */
/*    #define VER_PRODUCTVERSION_DW    (0x09630000 | 1234)      */
/*    #define VER_PRODUCTVERSION_W     (0x0963)                 */
/*                                                              */
/* The last four digits of the build number should be modified  */
/* by the official builder for each build.                      */
/*                                                              */
/*--------------------------------------------------------------*/

#ifdef PLUS98

/*--------------------------------------------------------------*/
/* Version numbers for Windows 98 Plus Pack                                */
/*--------------------------------------------------------------*/

#ifndef WIN32
#define VERSION                     "4.90.3000"
#endif
#define VER_PRODUCTVERSION_STR      "4.90.3000\0"
#define VER_PRODUCTVERSION          4,90,0,3000
#define VER_PRODUCTVERSION_BUILD    3000
#define VER_PRODUCTVERSION_DW       (0x045A0000 | 3000>
#define VER_PRODUCTVERSION_W        (0x045A)

#elif defined(NASH) || defined(FROSTING)

/*--------------------------------------------------------------*/
/* Version numbers for NASHVILLE                                */
/*--------------------------------------------------------------*/

#ifndef WIN32
#define VERSION                     "4.90.3000"
#endif
#define VER_PRODUCTVERSION_STR      "4.90.3000\0"
#define VER_PRODUCTVERSION          4,90,0,3000
#define VER_PRODUCTVERSION_BUILD    3000
#define VER_PRODUCTVERSION_DW       (0x045A0000 | 3000>
#define VER_PRODUCTVERSION_W        (0x045A)

#else

/*--------------------------------------------------------------*/
/* Version numbers for OPK3/MEMPHIS                             */
/*--------------------------------------------------------------*/

#ifndef WIN32
#define VERSION                     "4.90.3000"
#endif
#define VER_PRODUCTVERSION_STR      "4.90.3000\0"
#define VER_PRODUCTVERSION          4,90,0,3000
#define VER_PRODUCTVERSION_BUILD    3000
#define VER_PRODUCTVERSION_DW       (0x045A0000 | 3000)
#define VER_PRODUCTVERSION_W        (0x045A)

#endif

/*--------------------------------------------------------------*/
/* the following section defines values used in the version     */
/* data structure for all files, and which do not change.       */
/*--------------------------------------------------------------*/

/* This is for .ASM guys who only include VERSION.INC */
#ifndef VS_FF_DEBUG
#define VS_FF_DEBUG                 0
#endif

/* default is nodebug */
#ifndef DEBUG
#define VER_DEBUG                   0
#else
#define VER_DEBUG                   VS_FF_DEBUG
#endif

/* This is for .ASM guys who only include VERSION.INC */
#ifndef VS_FF_PRIVATEBUILD                            
#define VS_FF_PRIVATEBUILD          0
#endif

/* default is privatebuild */
#ifndef OFFICIAL
#define VER_PRIVATEBUILD            VS_FF_PRIVATEBUILD
#else
#define VER_PRIVATEBUILD            0
#endif

/* This is for .ASM guys who only include VERSION.INC */
#ifndef VS_FF_PRERELEASE
#define VS_FF_PRERELEASE            0
#endif

/* default is prerelease */
#ifndef FINAL
#define VER_PRERELEASE              VS_FF_PRERELEASE
#else
#define VER_PRERELEASE              0
#endif

#define VER_COPYRIGHT_CURRENT_YEAR  "2000"
#define VER_FILEFLAGSMASK           VS_FFI_FILEFLAGSMASK
#define VER_FILEOS                  VOS_DOS_WINDOWS16
#define VER_FILEFLAGS               (VER_PRIVATEBUILD|VER_PRERELEASE|VER_DEBUG)

#define VER_COMPANYNAME_STR         "Microsoft Corporation\0"

#ifndef NASH
#define VER_PRODUCTNAME_STR         "Microsoft(R) Windows(R) Millennium Operating System\0"
#else
#define VER_PRODUCTNAME_STR         "Microsoft(R) Windows(R) Millennium Operating System\0"
#endif

#define VER_LEGALTRADEMARKS_STR     \
"Microsoft(R) is a registered trademark of Microsoft Corporation. Windows(R) is a trademark of Microsoft Corporation.\0"





