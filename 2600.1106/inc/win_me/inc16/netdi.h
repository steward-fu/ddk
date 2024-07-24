//**********************************************************************
//
//  NETDI.H
//
//  Copyright (c) 1995 - Microsoft Corp.
//  All rights reserved.
//  Microsoft Confidential
//
// Public include file for Chicago Network Device Installer services.
//
//**********************************************************************

#ifndef _INC_NETDI
#define _INC_NETDI          // NETDI.H signature
#include <prsht.h>          // Property sheet API
#include <setupx.h>         // Device Installer API

// Return error codes for NDI_ messages.
#define NDI_ERROR           (1200)  // BUGBUG see setupx.h for error #
enum _ERR_NET_DEVICE_INSTALL
{
    ERR_NDI_ERROR               = NDI_ERROR,  // generic failure
    ERR_NDI_INVALID_HNDI,
    ERR_NDI_INVALID_DEVICE_INFO,
    ERR_NDI_INVALID_DRIVER_PROC,
    ERR_NDI_LOW_MEM,
    ERR_NDI_REG_API,
    ERR_NDI_NOTBOUND,
    ERR_NDI_NO_MATCH,
    ERR_NDI_INVALID_NETCLASS,
    ERR_NDI_INSTANCE_ONCE,
    ERR_NDI_CANCEL,
    ERR_NDI_NO_DEFAULT,
	ERR_NDI_ADD_RECURSE,
	ERR_NDI_MSG_NOT_SUPPORTED
};

// Network Driver Info Handle
DECLARE_HANDLE(HNDI);

/*******************************************************************************
*   AUTODOC
*   @doc	EXTERNAL NETDI DEVICE_INSTALLER
*
*   @types	SELECTBESTCOMPATDRV_PARAMS | DIF_SELECTBESTCOMPATDRV class install parameters
*
*   @field	UINT | cbSize | Size of the SELECTBESTCOMPATDRV_PARAMS struct.
*
*	@field	LPDEVICE_INFO | lpdiParent | Pointer to DEVICE_INFO struct of the parent 
*			devnode so that NetDi can check the interfaces of the candidates against 
*			the parent.  If list is empty or DI_CLASSINSTALLPARAMS is clear then all 
*			pre-existing rules and routines are still used.  The parent interfaces 
*			are used to further restrict candidates to a possibly NULL lpSelectedDriver.
*
*******************************************************************************/

typedef struct _SELECTBESTCOMPATDRV_PARAMS
{
	UINT           cbSize;
	LPDEVICE_INFO  lpdiParent;
} 
SELECTBESTCOMPATDRV_PARAMS, FAR * LPSELECTBESTCOMPATDRV_PARAMS;

#define ASSERT_SELECTBESTCOMPATDRVPARAMS_STRUC(lpsbc) if (lpsbc->cbSize != sizeof(SELECTBESTCOMPATDRV_PARAMS)) return (ERR_DI_BAD_DEV_INFO)

/*******************************************************************************
*   AUTODOC
*   @doc	EXTERNAL NETDI DEVICE_INSTALLER
*
*   @types	INSTALLDEVICE_PARAMS | DIF_INSTALLDEVICE class install parameters
*
*   @field	UINT | cbSize | Size of the INSTALLDEVICE_PARAMS struct.
*
*	@field	LPDEVICE_INFO | lpdiParent | Pointer to DEVICE_INFO struct of the parent 
*			devnode so that NetDi knows what parent object gets bound to the new device.
*			If the list is empty or DI_CLASSINSTALLPARAMS is clear then a new device
*			will be installed for every lower edge device with a compatible interface.
*			This means that a single call to DiCallClassInstaller with DIF_INSTALLDEVICE
*			can result in the installation of several instances of the target net
*			object.  Physical net cards are exempt from this behavior.  The use of
*			INSTALLDEVICE_PARAMS for devices of class NET is however reserved for future use.
*
*******************************************************************************/

typedef struct _INSTALLDEVICE_PARAMS
{
	UINT           cbSize;
	LPDEVICE_INFO  lpdiParent;
} 
INSTALLDEVICE_PARAMS, FAR * LPINSTALLDEVICE_PARAMS;

#define ASSERT_INSTALLDEVICEPARAMS_STRUC(lpsbc) if (lpsbc->cbSize != sizeof(INSTALLDEVICE_PARAMS)) return (ERR_DI_BAD_DEV_INFO)

// Network Driver Installer Callback
typedef RETERR (CALLBACK* NDIPROC)(HNDI, UINT, WPARAM, LPARAM);
RETERR WINAPI DefNdiProc(HNDI,UINT,WPARAM,LPARAM);

// Network Driver Installer Messages
#define NDI_NULL                0x0000
#define NDI_CREATE              0x0001
#define NDI_DESTROY             0x0002
#define NDI_VALIDATE            0x0003
#define NDI_INSTALL             0x0004

// johnri 3/8/84 removed-using property sheets only
//#define NDI_ASSIGNRESOURCES     0x0005
#define NDI_HASPROPPAGES        0x0005
#define NDI_ADDPROPPAGES        0x0006

// lpapp = (LPNDIADDPROPPAGES)lParam;
typedef BOOL (CALLBACK* LPFNADDNDIPROPPAGE)(LPCPROPSHEETPAGE,LPARAM,BOOL);
typedef struct tagNDIADDPROPPAGES
{
    LPFNADDNDIPROPPAGE      lpfnAddNdiPropPage;
    LPARAM                  lParam;
} NDIADDPROPPAGES, FAR* LPNDIADDPROPPAGES;

#define NDI_REMOVE              0x0007
#define NDI_FIRSTTIMESETUP      0x0008
#define NDI_QUERY_BIND          0x0009
#define NDI_NOTIFY_BIND         0x000A
#define NDI_NOTIFY_UNBIND       0x000B
#define NDI_GETTEXT             0x000C
#define NDI_SETTEXT             0x000D
#define NDI_SYSTEMCHANGE        0x000E

// Only sent if the INF indicates the NDI proc is capable of handling the message
#define NDI_MILL_MSG_START                  0x000F
#define NDI_MILL_MSG_UPGRADE                0x000F  // Sent when a device is upgrade in place
#define NDI_MILL_MSG_UPGRADE_BROADCAST      0x0010  // Sent to all devices when a device is upgraded in place

// Sent to all devices when all devices have been enumerate from the registry 
// (Warning: not availabe until after a device has been fully upgraded)
#define NDI_MILL_MSG_REGISTRY_ENUMERATED    0x0011
#define NDI_MILL_MSG_ENUM_COMPLETE          0x0012
#define NDI_MILL_MSG_END                    0x0012

#define NDI_NDICREATE           0x0040
#define NDI_NDIDESTROY          0x0041

// Messages above NDI_INSTALLER are reserved for installer dlls
#define NDI_INSTALLER           0x8000

// Ndi_Create: lParam flags:
#define NDI_CREATE_BATCHMODE    0x00000001
#define NDI_CREATE_NODISPLAY    0x00000002
#define NDI_CREATE_WORKSTATION  0x00000004

typedef enum _BusType
{
	ISA=1,
	EISA, 
	MicroChannel,
	TurboChannel,
	PCIBus,
	VMEbus,
	NuBus,
	PCMCIABus,
	Cbus,
	MPIBus,
	MPSABus,
	PNPISABus,
	PNPBus,
	MaxBusType
} 
BUSTYPE;

// General NDI management
HNDI   WINAPI NdiGetFirst(VOID);
HNDI   WINAPI NdiGetNext(HNDI hndi);
HNDI   WINAPI NdiFindNdi(HNDI ndiRelation, WORD wNetClass, LPCSTR lpszDeviceId);
RETERR WINAPI NdiIsNdi(HNDI hndi);
RETERR WINAPI NdiCallInstaller(HNDI hndi,UINT,WPARAM,LPARAM);
RETERR WINAPI NdiAddNewDriver(HNDI FAR* lphndi, LPDEVICE_INFO lpdi, LPCSTR lpszDeviceID, UINT uFlags);
    #define NDI_ADD_NO_DISELECT 0x0001

HNDI   WINAPI NdiFindDefNdi( WORD wNetClass );
RETERR WINAPI NdiSetDefNdi( HNDI ndi );
RETERR WINAPI NdiClrDefNdi( HNDI ndi );
HNDI   WINAPI NdiChooseCandidateDefNdi( HNDI ndi, WORD wClass );
RETERR WINAPI NdiNeedReboot ( HNDI ndi, DWORD wRebootType );
RETERR WINAPI CreateNdiFromDeviceId (LPCSTR szId);

// Device Manager
RETERR WINAPI NdiValidate(HNDI hndi, HWND hwndParent);
RETERR WINAPI NdiInstall(HNDI hndi);
RETERR WINAPI NdiRemove(HNDI hndi);
RETERR WINAPI NdiProperties(HNDI hndi, HWND hwndParent);

// Bindings
RETERR WINAPI NdiBind(HNDI hndiLower, HNDI hndiUpper);
RETERR WINAPI NdiUnbind(HNDI hndiLower, HNDI hndiUpper);
RETERR WINAPI NdiQueryBind(HNDI hndiLower, HNDI hndiUpper, UINT uBindType);
RETERR WINAPI NdiIsBound(HNDI hndiLower, HNDI hndiUpper);
RETERR WINAPI NdiGetBinding(HNDI hndi, HNDI FAR* lphndi, UINT uBindType);
enum _NDIBIND {
    NDIBIND_UPPER       = 1,
    NDIBIND_UPPER_FIRST = NDIBIND_UPPER,
    NDIBIND_UPPER_NEXT,
    NDIBIND_LOWER,
    NDIBIND_LOWER_FIRST = NDIBIND_LOWER,
    NDIBIND_LOWER_NEXT};

// Ndi_SystemChange object properties
RETERR WINAPI NdiSystemChange(UINT uTypeOfChange, HNDI hndi);
enum _NDISYTEMCHANGE {  //lpwClass
    NDISYSTEMCHANGE_ADD,
    NDISYSTEMCHANGE_REMOVE};

// General NDI Object Properties
RETERR WINAPI NdiGetText(HNDI hndi, LPSTR, UINT);
RETERR WINAPI NdiSetText(HNDI hndi, LPSTR);
RETERR WINAPI NdiGetDeviceInfo(HNDI hndi, LPLPDEVICE_INFO);
RETERR WINAPI NdiGetClass(HNDI hndi, LPWORD lpwClass);
enum _NDICLASS {    // lpwClass
    NDI_CLASS_NET,
    NDI_CLASS_TRANS,
    NDI_CLASS_CLIENT,
    NDI_CLASS_SERVICE};
RETERR WINAPI NdiGetProperties(HNDI hndi, LPVOID FAR* lplpvProperties);
RETERR WINAPI NdiSetProperties(HNDI hndi, LPVOID lpvProperties);
RETERR WINAPI NdiGetOwnerWindow(HNDI hndi, HWND FAR* lphwnd);
RETERR WINAPI NdiGetDeviceId(HNDI hndi, LPSTR, UINT);
RETERR WINAPI NdiGetFlags(HNDI hndi, LPDWORD lpdwFlags);
// hiword is reserved for internal use
    #define NDIF_ADDED                  0x00000001
    #define NDIF_REMOVED                0x00000002
    #define NDIF_MODIFIED_BINDINGS      0x00000004
    #define NDIF_MODIFIED_PROPERTIES    0x00000008
    #define NDIF_SAVE_MASK              0x0000000F
    #define NDIF_DEFAULT                0x00000010
    #define NDIF_INVISIBLE              0x00000020
    #define NDIF_HAS_PARAMS             0x00000040

// Interfaces
RETERR WINAPI NdiCompareInterface(HNDI ndi, UINT uRelation, HNDI ndi2, UINT uRelation2);
RETERR WINAPI NdiGetInterface(HNDI ndi, UINT uRelation, UINT index, LPSTR lpsz, UINT cbSizeOflpsz);
RETERR WINAPI NdiAddInterface(HNDI ndi, UINT uRelation, LPCSTR lpsz);
RETERR WINAPI NdiRemoveInterface(HNDI ndi, UINT uRelation, LPCSTR lpsz);
enum _NDIEDGERELATION {
    NDI_EDGE_ALL=100,               // used to free all edges and marker for first in edge class
    NDI_EDGE_UPPER,
    NDI_EDGE_LOWER,
    NDI_EDGE_UPPERRANGE,
    NDI_EDGE_LOWERRANGE,
    NDI_EDGE_REQUIRELOWER,
    NDI_EDGE_REQUIREANY,
    NDI_EDGE_EXCLUDELOWER,
    NDI_EDGE_EXCLUDEANY,
    NDI_EDGE_ORGUPPER,
    NDI_EDGE_ORGLOWER,
    NDI_EDGE_END,                   // marker only for end of edges
    NDI_COMATIBLE_ALL=200,          // used to free all edges and marker for first in compatible class
    NDI_COMPATIBLE_REQUIREDUPPER,
    NDI_COMPATIBLE_REQUIREDLOWER,
    NDI_COMPATIBLE_REQUIREDALL,
    NDI_COMPATIBLE_REQUIREDANY,
    NDI_COMPATIBLE_EXCLUDEUPPER,
    NDI_COMPATIBLE_EXCLUDELOWER,
    NDI_COMPATIBLE_EXCLUDEALL,
    NDI_COMPATIBLE_END };           // marker only for end of edges

// Driver Registry Access
RETERR WINAPI NdiRegOpenKey(HNDI hndi, LPCSTR lpszSubKey, LPHKEY lphk);
RETERR WINAPI NdiRegCreateKey(HNDI hndi, LPCSTR lpszSubKey, LPHKEY lphk);
RETERR WINAPI NdiRegCloseKey(HKEY hkey);
RETERR WINAPI NdiRegQueryValue(HNDI hndi, LPCSTR lpszSubKey, LPCSTR lpszValueName, LPSTR lpValue, DWORD cbValue);
RETERR WINAPI NdiRegSetValue(HNDI hndi, LPCSTR lpszSubKey, LPCSTR lpszValueName, DWORD dwType, LPCSTR lpValue, DWORD cbValue);
RETERR WINAPI NdiRegDeleteValue(HNDI hndi,LPCSTR lpszSubKey, LPCSTR lpszValueName);

DWORD  WINAPI IsWSock2Installed(void);
RETERR WINAPI ForceWSock2Install( DWORD Reserved );

BOOL WINAPI IsFirstBootUpgrade(void); // Returns TRUE if a first boot upgrade is being done

// Entry point called by NETCPL.
RETERR WINAPI NdiCplProperties(HWND hwndCpl);

#endif // _INC_NETDI
