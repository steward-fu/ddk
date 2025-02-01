/*****************************************************************/
/**               Microsoft Windows for Workgroups              **/
/**           Copyright (C) Microsoft Corp., 1991-1994          **/
/*****************************************************************/

/* PWDSPI.H -- Password provider interface definitions.
 *
 * History:
 *  10/7/94    jeremys   Created
 *
 */

#ifndef _INC_PWDSPI
#define _INC_PWDSPI

#ifndef RC_INVOKED
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
#endif

typedef struct _CHANGEPWDINFO {
    LPTSTR lpUsername;
    LPTSTR lpPassword;
	DWORD cbPassword;
} CHANGEPWDINFO, FAR *LPCHANGEPWDINFO;

#define PWDCHANGE_MASTERPWD_NOTIFY      1
#define PWDCHANGE_PROVIDERPWD_PENDING   2
#define PWDCHANGE_PROVIDERPWD_COMMIT    3
#define PWDCHANGE_PROVIDERPWD_CANCEL    4

//
//	PASSWORD SYNCHRONIZATION FUNCTIONS
//

DWORD APIENTRY
PwdChangePasswordA (
	LPCSTR			lpPassword,
	HWND			hwndOwner,
	DWORD			dwFlags,
	LPCHANGEPWDINFO	lpChangePwdInfo
    );
DWORD APIENTRY
PwdChangePasswordW (
	LPCWSTR 		lpPassword,
	HWND			hwndOwner,
	DWORD			dwFlags,
	LPCHANGEPWDINFO lpChangePwdInfo
    );
#ifdef UNICODE
#define PwdChangePassword  PwdChangePasswordW
#else
#define PwdChangePassword  PwdChangePasswordA
#endif // UNICODE

#define CHANGEPWD_OLDPWDONLY	0x01	// only prompt for old password, use
										// login password for new password

DWORD APIENTRY
PwdGetPasswordStatusA (
    LPCSTR  lpPassword,
    DWORD	dwIndex,
    LPDWORD	lpStatus
    );
DWORD APIENTRY
PwdGetPasswordStatusW (
    LPCWSTR  lpPassword,
    DWORD	dwIndex,
    LPDWORD	lpStatus
    );
#ifdef UNICODE
#define PwdGetPasswordStatus  PwdGetPasswordStatusW
#else
#define PwdGetPasswordStatus  PwdGetPasswordStatusA
#endif // UNICODE

// ordinals for index to WNetGetPasswordStatus
#define PS_ONOFF			0x01
#define PS_HASOLDPWD		0x02
#define PS_SYNCMASTERPWD	0x03
#define PS_STORAGETYPE		0x04

// bits for each index
#define PS_ONOFF_OFF				0x00
#define PS_ONOFF_ON					0x01
#define PS_HASOLDPWD_OFF			0x00
#define PS_HASOLDPWD_ON				0x01
#define PS_SYNCMASTERPWD_OFF		0x00
#define PS_SYNCMASTERPWD_ON			0x01
#define PS_SYNCMASTERPWD_NOTSUPP	0x02
#define PS_STORAGETYPE_LOCAL		0x01
#define PS_STORAGETYPE_NETWORK		0x02


DWORD APIENTRY
PwdSetPasswordStatusA (
    LPCSTR  lpPassword,
    DWORD	dwIndex,
    DWORD	dwStatus
    );
DWORD APIENTRY
PwdSetPasswordStatusW (
    LPCWSTR  lpPassword,
    DWORD	dwIndex,
    DWORD	dwStatus
    );


#ifdef UNICODE
#define PwdSetPasswordStatus  PwdSetPasswordStatusW
#else
#define PwdSetPasswordStatus  PwdSetPasswordStatusA
#endif // UNICODE

#ifndef RC_INVOKED
#ifdef __cplusplus
};	/* extern "C" */
#endif  /* __cplusplus */
#endif

#endif	// _INC_PWDSPI
