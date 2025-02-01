/****************************************************************************
 *
 *  SetupExt.CPP
 *
 *  Copyright (c) Microsoft Corporation 1996-1998
 *  All rights reserved
 *
 ***************************************************************************/

#include "SetupExt.h"
#include "debug.h"


/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   BOOL | DllEntryPoint |
 *
 *          Called to notify the DLL about various things that can happen.
 *
 *
 *  @parm   HINSTANCE | hinst |
 *
 *          The instance handle of this DLL.
 *
 *  @parm   DWORD | dwReason |
 *
 *          Notification code.
 *
 *  @parm   LPVOID | lpReserved |
 *
 *          Not used.
 *
 *  @returns
 *
 *          Returns <c TRUE> to allow the DLL to load.
 *
 *****************************************************************************/


extern "C"
DLLEXPORT
BOOL APIENTRY
DllEntryPoint(HINSTANCE hinst, DWORD dwReason, LPVOID lpReserved)
{
   return 1;
}


extern "C"
DLLEXPORT
BOOL WINAPI
DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID lpReserved)
{
    return DllEntryPoint(hinst, dwReason, lpReserved);
}

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   VOID | SetupEntryPoint |
 *
 *          This DLL implements a vendor setup extension. This can be used to
 *          do extra steps at installation time. This DLL will get called right
 *          after the class installer has done all of its setup.
 *          This example specifically implements a fix to the STI class installer
 *          in Windows 98 where the port name is not set for parallel PnP devices.
 *          This function will get the port name out of Enum branch of the registry
 *          and store it in the STI branch.
 *          Note that it is normally not a good idea to directly change entries in
 *          the registry because their location may change in the future, but STI
 *          does not provide a mechanism for updating the port name.
 *
 *  @parm   LPSTR | lpszCmdLine |
 *
 *          For this example, this parameter is the PnP ID of the device.
 *
 *****************************************************************************/
extern "C"
DLLEXPORT
VOID FAR PASCAL
SetupEntryPoint(HWND hwnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow)
{
   Report(("Running vendor extension\n"));

   // Open the ENUM registry key for the device. It will be HKLM\Enum\LptEnum\XXXX
   // where XXXX is the PnP ID for the device which should be passed in the command line.
   CHAR lpszSubKey[MAX_LEN];
   strcpy(lpszSubKey, LPTENUM);
   strcat(lpszSubKey, lpszCmdLine);

   HKEY hkLptKey;
   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpszSubKey, 0, KEY_READ, &hkLptKey) != ERROR_SUCCESS) {
      Report(("Could not open %s\n", lpszSubKey));
      return;
   }

   // The first subkey of the opened key will be a pointer to the parallel port where
   // the device is attached.
   CHAR lpszBiosKey[MAX_LEN];
   DWORD dwLength = MAX_LEN;
   if (RegEnumKeyEx(hkLptKey, 0, lpszBiosKey, &dwLength, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) {
      Report(("Could not find key under %s\n", lpszSubKey));
      return;
   }

   // While we have this key open, grab the name of the STI key to use later when
   // storing the port name
   if (RegOpenKeyEx(hkLptKey, lpszBiosKey, 0, KEY_READ, &hkLptKey) != ERROR_SUCCESS) {
      Report(("Could not open %s\n", lpszBiosKey));
      return;
   }

   CHAR lpszStiKey[MAX_LEN];
   dwLength = MAX_LEN;
   if (RegQueryValueEx(hkLptKey, DRIVER, NULL, NULL, (LPBYTE) lpszStiKey, &dwLength) != ERROR_SUCCESS) {
      Report(("Could not read %s key\n", DRIVER));
      return;
   }

   RegCloseKey(hkLptKey);

   // The subkey name contains ampersands (&) where there should be backslashes (\),
   // so replace them with backslashes.
   PCHAR pcCurr;
   for (pcCurr = lpszBiosKey; *pcCurr; pcCurr++) {
      if (*pcCurr == '&') {
         *pcCurr = '\\';
      }
   }

   // Open the registry key of the parallel port where the device is attached.
   strcpy(lpszSubKey, ENUM);
   strcat(lpszSubKey, lpszBiosKey);

   Report(("Opening key %s\n", lpszSubKey));

   HKEY hkBiosKey;
   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpszSubKey, 0, KEY_READ, &hkBiosKey) != ERROR_SUCCESS) {
      Report(("Could not open key %s\n", lpszSubKey));
      return;
   }

   // Read the port name of the parallel port.
   CHAR lpszPortName[MAX_LEN];
   dwLength = MAX_LEN;
   if (RegQueryValueEx(hkBiosKey, PORTNAME, NULL, NULL, (LPBYTE) lpszPortName, &dwLength) != ERROR_SUCCESS) {
      Report(("Could not read key %s\n", PORTNAME));
      return;
   }

   Report(("Found port name %s\n", lpszPortName));

   // Now that we have the port name, store it in STI's section of the registry. First
   // open the correct STI key using the key we found earlier.
   CHAR lpszFullStiKey[MAX_LEN];
   strcpy(lpszFullStiKey, STI_KEY);
   strcat(lpszFullStiKey, lpszStiKey);

   HKEY hkSti;
   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpszFullStiKey, 0, KEY_WRITE, &hkSti) != ERROR_SUCCESS) {
      Report(("Could not open key %s\n", lpszFullStiKey));
      return;
   }

   // Store the port name under CreateFileName
   if (RegSetValueEx(hkSti, CREATEFN, 0, REG_SZ, (CONST BYTE *) lpszPortName, strlen(lpszPortName) + 1) != ERROR_SUCCESS) {
      Report(("Could not write key %s\n", CREATEFN));
      return;
   }

   RegCloseKey(hkSti);

   Report(("Port name successfully updated\n"));
}

