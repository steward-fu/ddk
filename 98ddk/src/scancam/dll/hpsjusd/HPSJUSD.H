/****************************g************************************************
 *
 *  HPSJUSD.H
 *
 *  Copyright (c) Microsoft Corporation 1996-1997
 *  All rights reserved
 *
 ***************************************************************************/

#include <windows.h>

#pragma intrinsic(memcmp,memset)

#include <objbase.h>

#include "sti.h"
#include "stierr.h"
#include "stiusd.h"

/*
 * Class IID's
 */
#if defined( _WIN32 ) && !defined( _NO_COM)

//  {E9FA3320-7F3F-11D0-90EA-00AA0060F86C}
DEFINE_GUID(CLSID_SclUsdObj, 0xE9FA3320L, 0x7F3F, 0x11D0, 0x90, 0xEA, 0x00, 0xAA, 0x00, 0x60, 0xF8, 0x6C);

// {AD879F40-0982-11d1-A43B-080009EEBDF6}
DEFINE_GUID( guidButton1, 0xad879f40, 0x982, 0x11d1, 0xa4, 0x3b, 0x8, 0x0, 0x9, 0xee, 0xbd, 0xf6 );

#endif


#define DATASEG_PERINSTANCE     ".instance"
#define DATASEG_SHARED          ".shared"
#define DATASEG_READONLY        ".code"

#define DATASEG_DEFAULT         DATASEG_SHARED

#pragma data_seg(DATASEG_PERINSTANCE)

// Set the default data segment
#pragma data_seg(DATASEG_DEFAULT)

//
// Module ref counting
//
extern UINT g_cRefThisDll;
extern UINT g_cLocks;

extern BOOL DllInitializeCOM(void);
extern BOOL DllUnInitializeCOM(void);

extern void DllAddRef(void);
extern void DllRelease(void);

//
// Base class for supporting non-delegating IUnknown for contained objects
//
struct INonDelegatingUnknown
{
    // *** IUnknown-like methods ***
    STDMETHOD(NonDelegatingQueryInterface)( THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,NonDelegatingAddRef)(THIS)  PURE;
    STDMETHOD_(ULONG,NonDelegatingRelease)( THIS) PURE;
};


//
// Class definition for object
//

class UsdHPSJDevice : public IStiUSD, public INonDelegatingUnknown
{
private:
    ULONG       m_cRef;
    BOOL        m_fValid;
    LPTYPEINFO  m_ptinfo;
    BOOL        m_bRaiseException;
    EXCEPINFO   m_excepinfo;
    LPUNKNOWN   m_punkOuter;
    PSTIDEVICECONTROL   m_pDcb;
    WCHAR       m_szDeviceName[MAX_PATH];
    CHAR        *m_pszDeviceNameA;
    HANDLE      m_DeviceDataHandle;
    DWORD       m_dwLastOperationError;
    DWORD       m_dwAsync ;
    HANDLE      m_hSignalEvent;
    HANDLE      m_hShutdownEvent;
    HANDLE      m_hThread;

    GUID        m_guidLastEvent;

    BOOL inline IsValid(VOID) {
        return m_fValid;
    }

public:
    // *** IUnknown-like methods ***
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, LPVOID * ppvObj);
    STDMETHODIMP_(ULONG) NonDelegatingAddRef();
    STDMETHODIMP_(ULONG) NonDelegatingRelease();

    // *** IUnknown methods ***
    STDMETHODIMP QueryInterface( REFIID riid, LPVOID * ppvObj);
    STDMETHODIMP_(ULONG) AddRef( void);
    STDMETHODIMP_(ULONG) Release( void);

    /*** IStiUSD methods ***/
    STDMETHOD(Initialize) (THIS_ PSTIDEVICECONTROL pHelDcb,DWORD dwStiVersion,HKEY hParametersKey)  ;
    STDMETHOD(GetCapabilities) (THIS_ PSTI_USD_CAPS pDevCaps)  ;
    STDMETHOD(GetStatus) (THIS_ PSTI_DEVICE_STATUS pDevStatus)  ;
    STDMETHOD(DeviceReset)(THIS )  ;
    STDMETHOD(Diagnostic)(THIS_ LPDIAG pBuffer)  ;
    STDMETHOD(Escape)(THIS_ STI_RAW_CONTROL_CODE    EscapeFunction,LPVOID  lpInData,DWORD   cbInDataSize,LPVOID pOutData,DWORD dwOutDataSize,LPDWORD pdwActualData)   ;
    STDMETHOD(GetLastError) (THIS_ LPDWORD pdwLastDeviceError)  ;
    STDMETHOD(LockDevice) (THIS )  ;
    STDMETHOD(UnLockDevice) (THIS )  ;
    STDMETHOD(RawReadData)(THIS_ LPVOID lpBuffer,LPDWORD lpdwNumberOfBytes,LPOVERLAPPED lpOverlapped)  ;
    STDMETHOD(RawWriteData)(THIS_ LPVOID lpBuffer,DWORD nNumberOfBytes,LPOVERLAPPED lpOverlapped)  ;
    STDMETHOD(RawReadCommand)(THIS_ LPVOID lpBuffer,LPDWORD lpdwNumberOfBytes,LPOVERLAPPED lpOverlapped)  ;
    STDMETHOD(RawWriteCommand)(THIS_ LPVOID lpBuffer,DWORD nNumberOfBytes,LPOVERLAPPED lpOverlapped)  ;
    STDMETHOD(SetNotificationHandle)(THIS_ HANDLE hEvent)  ;
    STDMETHOD(GetNotificationData)(THIS_ LPSTINOTIFY   lpNotify)  ;
    STDMETHOD(GetLastErrorInfo) (THIS_ STI_ERROR_INFO *pLastErrorInfo);

    UsdHPSJDevice(LPUNKNOWN punkOuter);
    ~UsdHPSJDevice();

    VOID RunNotifications(VOID);
};

typedef UsdHPSJDevice *PUsdHPSJDevice;

//
// Syncronization mechanisms
//
#define ENTERCRITICAL   DllEnterCrit(void);
#define LEAVECRITICAL   DllLeaveCrit(void);


