////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//              resource.c
//
//	Abstract:
//
//	Author:
//		William Ingle
//
//	Environment:
//		Windows 95 Sample 16-bit custom install dll
//
//	Notes:
//
//	Future:
//
//	Revision History:
//
////////////////////////////////////////////////////////////////////////////////////

#define IDS_NOMEM						50 
#define IDS_ERR_ASSERT					51

#define IDS_CAPTION						100

#define IDS_DIF_BASE					200
#define IDS_DIF_SELECTDEVICE			201
#define IDS_DIF_INSTALLDEVICE			202
#define IDS_DIF_ASSIGNRESOURCES			203
#define IDS_DIF_PROPERTIES				204
#define IDS_DIF_REMOVE					205
#define IDS_DIF_FIRSTTIMESETUP			206
#define IDS_DIF_FOUNDDEVICE				207
#define IDS_DIF_SELECTCLASSDRIVERS		208
#define IDS_DIF_VALIDATECLASSDRIVERS	209
#define IDS_DIF_INSTALLCLASSDRIVERS		210
#define IDS_DIF_CALCDISKSPACE			211
#define IDS_DIF_DESTROYPRIVATEDATA		212
#define IDS_DIF_VALIDATEDATA			213
#define IDS_DIF_MOVEDEVICE				214
#define IDS_DIF_DETECT					215
#define IDS_DIF_INSTALLWIZARD			216
#define IDS_DIF_DESTROYWIZARDDATA		217
#define IDS_DIF_PROPERTYCHANGE			218
#define IDS_DIF_ENABLECLASS				219
#define IDS_DIF_DETECTVERIFY			220
#define IDS_DIF_INSTALLDEVICEFILES		221


// Note that this must be synched up with the value defined for DI_ERROR in Setupx.h
#define DI_ERROR (500)


#define IDS_ERR_DI_INVALID_DEVICE_ID			   DI_ERROR		// Incorrectly formed device IDF
#define IDS_ERR_DI_INVALID_COMPATIBLE_DEVICE_LIST  DI_ERROR+1	// Invalid compatible device list
#define IDS_ERR_DI_REG_API                         DI_ERROR+2	// Error returned by Reg API.
#define IDS_ERR_DI_LOW_MEM                         DI_ERROR+3	// Insufficient memory to complete
#define IDS_ERR_DI_BAD_DEV_INFO                    DI_ERROR+4	// Device Info struct invalid
#define IDS_ERR_DI_INVALID_CLASS_INSTALLER         DI_ERROR+5	// Registry entry / DLL invalid
#define IDS_ERR_DI_DO_DEFAULT                      DI_ERROR+6	// Take default action
#define IDS_ERR_DI_USER_CANCEL                     DI_ERROR+7	// the user cancelled the operation
#define IDS_ERR_DI_NOFILECOPY                      DI_ERROR+8	// No need to copy files (in install)
#define IDS_ERR_DI_BAD_CLASS_INFO                  DI_ERROR+9	// Class Info Struct invalid
#define IDS_ERR_DI_BAD_INF                         DI_ERROR+10	// Bad INF file encountered
#define IDS_ERR_DI_BAD_MOVEDEV_PARAMS	           DI_ERROR+11	// Bad Move Device Params struct
#define IDS_ERR_DI_NO_INF                          DI_ERROR+12	// No INF found on OEM disk
#define IDS_ERR_DI_BAD_PROPCHANGE_PARAMS           DI_ERROR+13	// Bad property change param struct
#define IDS_ERR_DI_BAD_SELECTDEVICE_PARAMS         DI_ERROR+14	// Bad Select Device Parameters
#define IDS_ERR_DI_BAD_REMOVEDEVICE_PARAMS         DI_ERROR+15	// Bad Remove Device Parameters
#define IDS_ERR_DI_BAD_ENABLECLASS_PARAMS          DI_ERROR+16	// Bad Enable Class Parameters
#define IDS_ERR_DI_FAIL_QUERY                      DI_ERROR+17	// Fail the Enable Class query
#define IDS_ERR_DI_API_ERROR                       DI_ERROR+18	// DI API called incorrectly
#define IDS_ERR_DI_BAD_PATH                        DI_ERROR+19	// An OEM path was specified incorrectly
