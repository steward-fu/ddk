/****************************************************************************
 *
 *  HPSJUSD.CPP
 *
 *  Copyright (c) Microsoft Corporation 1996-1997
 *  All rights reserved
 *
 ***************************************************************************/

#define INITGUID

#include "hpsjusd.h"
#include "resource.h"

#if !defined(dllexp)
#define DLLEXPORT __declspec( dllexport )
#endif

/*****************************************************************************
 *
 *      Globals
 *
 *****************************************************************************/

// Reference counter for the whole library
DWORD       g_cRef;

// DLL module instance
HINSTANCE   g_hInst;

// Critical section for low level syncronization
CRITICAL_SECTION g_crstDll;

// Can we use UNICODE APIs
BOOL    g_NoUnicodePlatform = TRUE;

// Is COM initialized
BOOL    g_COMInitialized = FALSE;

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   void | DllEnterCrit |
 *
 *          Take the DLL critical section.
 *
 *          The DLL critical section is the lowest level critical section.
 *          You may not attempt to acquire any other critical sections or
 *          yield while the DLL critical section is held.
 *
 *****************************************************************************/

void
DllEnterCrit(void)
{
    EnterCriticalSection(&g_crstDll);
}

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   void | DllLeaveCrit |
 *
 *          Leave the DLL critical section.
 *
 *****************************************************************************/

void
DllLeaveCrit(void)
{
    LeaveCriticalSection(&g_crstDll);
}

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   void | DllAddRef |
 *
 *          Increment the reference count on the DLL.
 *
 *****************************************************************************/

void
DllAddRef(void)
{
    InterlockedIncrement((LPLONG)&g_cRef);
}

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   void | DllRelease |
 *
 *          Decrement the reference count on the DLL.
 *
 *****************************************************************************/

void
DllRelease(void)
{
    InterlockedDecrement((LPLONG)&g_cRef);
}


/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   HRESULT | DllInitializeCOM |
 *
 *          Initialize COM libraries
 *
 *  @parm   IN  |  |
 *
 *  @returns
 *
 *          Returns a boolean error code.
 *
 *****************************************************************************/

BOOL
DllInitializeCOM(
    void
    )
{
    DllEnterCrit();

    if(!g_COMInitialized) {
        if(SUCCEEDED(CoInitialize(NULL))) {
            g_COMInitialized = TRUE;
        }
    }

    DllLeaveCrit();

    return g_COMInitialized;
}

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   HRESULT | DllUnInitializeCOM |
 *
 *          UnInitialize COM libraries
 *
 *  @parm   IN  |  |
 *
 *  @returns
 *
 *          Returns a boolean error code.
 *
 *****************************************************************************/
BOOL
DllUnInitializeCOM(
    void
    )
{
    DllEnterCrit();

    if(g_COMInitialized) {
        CoUninitialize();
        g_COMInitialized = FALSE;
    }

    DllLeaveCrit();

    return TRUE;
}

/*****************************************************************************
 *
 *  @class UsdSclFactory |
 *
 *****************************************************************************/

class UsdHPSJClassFactory : public IClassFactory
{
private:
    ULONG   m_cRef;

public:
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    STDMETHODIMP CreateInstance(
            /* [unique][in] */ IUnknown __RPC_FAR *pUnkOuter,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

    STDMETHODIMP LockServer(
            /* [in] */ BOOL fLock);

    UsdHPSJClassFactory();
};

UsdHPSJClassFactory::UsdHPSJClassFactory()
{
    //DEBUGPRINTF((DBG_LVL_DEBUG, TEXT("UsdHPSJClassFactory: Constructor")));

    // Constructor logic
    m_cRef = 0;
}


STDMETHODIMP UsdHPSJClassFactory::QueryInterface(
    /* [in] */ REFIID riid,
    /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
    // DEBUGPRINTF((DBG_LVL_DEBUG, TEXT("UsdHPSJClassFactory: QueryInterface")));

    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_IClassFactory)) {
        *ppvObject = (LPVOID)this;
        AddRef();
        return NOERROR;
    }
    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) UsdHPSJClassFactory::AddRef(void)
{
    DllAddRef();
    return ++m_cRef;
}


STDMETHODIMP_(ULONG) UsdHPSJClassFactory::Release(void)
{
    DllRelease();
    if (--m_cRef == 0) {
        delete this;
        return 0;
    }
    return m_cRef;
}


STDMETHODIMP UsdHPSJClassFactory::CreateInstance(
    /* [unique][in] */ IUnknown __RPC_FAR *punkOuter,
    /* [in] */ REFIID riid,
    /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{

    if (!IsEqualIID(riid, IID_IStiUSD) &&
        !IsEqualIID(riid, IID_IUnknown)) {
        return STIERR_NOINTERFACE;
    }

    // When created for aggregation, only IUnknown can be requested, so fail
    // call if not.
    if (punkOuter && !IsEqualIID(riid, IID_IUnknown)) {
        return CLASS_E_NOAGGREGATION;
    }

    UsdHPSJDevice    *pDev = NULL;
    HRESULT         hres;

    pDev = new UsdHPSJDevice(punkOuter);
    if (!pDev) {
        return STIERR_OUTOFMEMORY;
    }

    //
    //  Move to the requested interface if we aren't aggregated.
    //  Don't do this if aggregated! or we will lose the private
    //  IUnknown and then the caller will be hosed.
    //
    hres = pDev->NonDelegatingQueryInterface(riid,ppvObject);
    pDev->NonDelegatingRelease();

    return hres;
}


STDMETHODIMP UsdHPSJClassFactory::LockServer(
    /* [in] */ BOOL fLock)
{
    if (fLock) {
        DllAddRef();
    } else {
        DllRelease();
    }

    return NOERROR;
}

/*****************************************************************************
 *
 *  @class UsdHPSJDevice | INonDelegatingUnknown
 *
 *****************************************************************************/
STDMETHODIMP UsdHPSJDevice::NonDelegatingQueryInterface( REFIID riid, LPVOID* ppvObj )
{
    HRESULT hres;

    if( !IsValid() || !ppvObj )
    {
        return STIERR_INVALID_PARAM;
    }

    *ppvObj = NULL;

    if( IsEqualIID( riid, IID_IUnknown ))
    {
        *ppvObj = static_cast<INonDelegatingUnknown*>(this);
        hres = S_OK;
    }
    else if( IsEqualIID( riid, IID_IStiUSD ))
    {
        *ppvObj = static_cast<IStiUSD*>(this);
        hres = S_OK;
    }

    else
    {
        hres =  STIERR_NOINTERFACE;
    }

    if (SUCCEEDED(hres)) {
//        NonDelegatingAddRef();
        (reinterpret_cast<IUnknown*>(*ppvObj))->AddRef();
    }

    return hres;
}


STDMETHODIMP_(ULONG) UsdHPSJDevice::NonDelegatingAddRef( VOID )
{
    ULONG ulRef;
    ulRef = InterlockedIncrement((LPLONG)&m_cRef);
    return ulRef;
}

STDMETHODIMP_(ULONG) UsdHPSJDevice::NonDelegatingRelease( VOID )
{
    ULONG ulRef;
    ulRef = InterlockedDecrement((LPLONG)&m_cRef);

    if(!ulRef)
    {
        delete this;
    }

    return ulRef;
}


/*****************************************************************************
 *
 *  @class UsdHPSJDevice | IUnknown (Delegating)
 *
 *         Delegating unknown methods.
 *
 *****************************************************************************/

STDMETHODIMP UsdHPSJDevice::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
    return m_punkOuter->QueryInterface(riid,ppvObj);
}


STDMETHODIMP_(ULONG) UsdHPSJDevice::AddRef( VOID )
{
    return m_punkOuter->AddRef();
}

STDMETHODIMP_(ULONG) UsdHPSJDevice::Release( VOID )
{
    return m_punkOuter->Release();
}

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   BOOL | DllEntryPoint |
 *
 *          Called to notify the DLL about various things that can happen.
 *
 *          We are not interested in thread attaches and detaches,
 *          so we disable thread notifications for performance reasons.
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
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:

        g_hInst = hinst;

        // Disable thread library calls to avoid
        // deadlock when we spin up the worker thread

        DisableThreadLibraryCalls(hinst);
        InitializeCriticalSection(&g_crstDll);

        // Set global flags
        // g_NoUnicodePlatform = !OSUtil_IsPlatformUnicode();

        break;

    case DLL_PROCESS_DETACH:
        if (g_cRef) {
            // DPRINTF("hpsjusd: Unloaded before all objects Release()d! Crash soon!\r\n");
        }

        // Free COM libraries if connected to them
        // DllUnInitializeCOM();

        break;
    }

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
 *  @func   HRESULT | DllCanUnloadNow |
 *
 *          Determine whether the DLL has any outstanding interfaces.
 *
 *  @returns
 *
 *          Returns <c S_OK> if the DLL can unload, <c S_FALSE> if
 *          it is not safe to unload.
 *
 *****************************************************************************/
extern "C"
STDMETHODIMP
DllCanUnloadNow(void)
{
    HRESULT hres;

    hres = g_cRef ? S_FALSE : S_OK;

    return hres;
}

/*****************************************************************************
 *
 *  @doc    INTERNAL
 *
 *  @func   HRESULT | DllGetClassObject |
 *
 *          Create an <i IClassFactory> instance for this DLL.
 *
 *  @parm   REFCLSID | rclsid |
 *
 *          The object being requested.
 *
 *  @parm   RIID | riid |
 *
 *          The desired interface on the object.
 *
 *  @parm   PPV | ppvOut |
 *
 *          Output pointer.
 *
 *  @notes
 *          We support only one class of objects , so this function does not need
 *          to go through table of supported classes , looking for proper constructor
 *
 *
 *****************************************************************************/
extern "C"
STDAPI DllGetClassObject(
    REFCLSID    rclsid,
    REFIID      riid,
    LPVOID      *ppv)
{

    if (!ppv) {
        return ResultFromScode(E_FAIL);
    }

    if (!IsEqualCLSID(rclsid, CLSID_SclUsdObj) ) {
        return ResultFromScode(E_FAIL);
    }

    if (!IsEqualIID(riid, IID_IUnknown) &&
        !IsEqualIID(riid, IID_IClassFactory)) {
        return ResultFromScode(E_NOINTERFACE);
    }

    if (IsEqualCLSID(rclsid, CLSID_SclUsdObj)) {
        UsdHPSJClassFactory *pcf = new UsdHPSJClassFactory;
        if (pcf) {
            *ppv = (LPVOID)pcf;
        }
    }

    return NOERROR;
}


