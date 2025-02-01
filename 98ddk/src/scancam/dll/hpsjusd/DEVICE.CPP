/****************************************************************************
 *
 *  DEVICE.CPP
 *
 *  Copyright (c) Microsoft Corporation 1996-1997
 *  All rights reserved
 *
 ***************************************************************************/

#include "hpsjusd.h"
#include "sclbuton.h"

#include <winioctl.h>

UsdHPSJDevice::UsdHPSJDevice( LPUNKNOWN punkOuter ):
    m_cRef(1),
    m_punkOuter(NULL),
    m_fValid(FALSE),
    m_pDcb(NULL),
    m_DeviceDataHandle(INVALID_HANDLE_VALUE),
    m_hSignalEvent(INVALID_HANDLE_VALUE),
    m_hThread(NULL),
    m_guidLastEvent(GUID_NULL)
{
    *m_szDeviceName = L'\0';

    //
    // See if we are aggregated. If we are ( which will be almost always the case )
    // save pointer to controlling Unknown , so subsequent calls will be delegated
    // If not, set the same pointer to "this" .
    // N.b. cast below is important in order to point to right virtual table
    //
    if (punkOuter) {
        m_punkOuter = punkOuter;
    }
    else {
        m_punkOuter = reinterpret_cast<IUnknown*>
                      (static_cast<INonDelegatingUnknown*>
                      (this));
    }

    m_hShutdownEvent =  CreateEvent( NULL,  // Attributes
                                   FALSE,   // Auto reset
                                   FALSE,   // Initial state - not set
                                   NULL );  // Anonymous

    m_fValid = TRUE;
}

UsdHPSJDevice::~UsdHPSJDevice( VOID )
{
    // Kill notification thread if it exists
    SetNotificationHandle(NULL);

    if (m_hShutdownEvent && m_hShutdownEvent!=INVALID_HANDLE_VALUE) {
        CloseHandle(m_hShutdownEvent);
    }

    if( INVALID_HANDLE_VALUE != m_DeviceDataHandle )
    {
        CloseHandle( m_DeviceDataHandle );
    }

	if (m_pDcb) {
		m_pDcb->Release();
		m_pDcb = NULL;
	}

    if (m_pszDeviceNameA) {
        delete [] m_pszDeviceNameA;
        m_pszDeviceNameA = NULL;
    }
}

STDMETHODIMP UsdHPSJDevice::GetCapabilities( PSTI_USD_CAPS pUsdCaps )
{
    HRESULT hres = STI_OK;

    ZeroMemory(pUsdCaps,sizeof(*pUsdCaps));

    // Set that we are only pass-through
    pUsdCaps->dwVersion = STI_VERSION;

    pUsdCaps->dwGenericCaps = STI_GENCAP_NOTIFICATIONS |
                              STI_GENCAP_POLLING_NEEDED;

    return hres;
}

STDMETHODIMP UsdHPSJDevice::GetStatus( PSTI_DEVICE_STATUS pDevStatus )
{
    HRESULT hres = STI_OK;
    BYTE    ReadBuffer[100];
    DWORD   ReadLength;

    pDevStatus->dwOnlineState = 0L;

	// Assume check for online status always for now 
    if( pDevStatus->StatusMask & STI_DEVSTATUS_ONLINE_STATE ) 
	{
	    //
        // Attempt to get current error code from device
        //
        hres = RawWriteData( (void*)CURRENT_ERROR, LEN_CURRENT_ERROR, NULL );
        if( !SUCCEEDED( hres ))
        {
            pDevStatus->dwOnlineState &= ~STI_ONLINESTATE_OPERATIONAL;
            return hres;
        }
    
        ReadLength = 8;
        hres = RawReadData( ReadBuffer, &ReadLength, NULL );

        #if 0
        // BUGBUG On some SCSI cards read actual length is not right
        if( ReadLength < 8 || ReadLength > 20 )
        {
            pDevStatus->dwOnlineState &= ~STI_ONLINESTATE_OPERATIONAL;
            return hres;
        }
        #endif

        //
        // Mark scanner as online
        //
        pDevStatus->dwOnlineState |= STI_ONLINESTATE_OPERATIONAL;
    }

    //
    // If we are asked , verify state of the button
    //
    if( pDevStatus->StatusMask & STI_DEVSTATUS_EVENTS_STATE )
    {
        ReadLength = LEN_INQUIRE_BUTTON_READ;
        BYTE    Zero = 0;

        ZeroMemory( ReadBuffer, sizeof( ReadBuffer ));

        hres = RawWriteData( (void*)INQUIRE_BUTTON, LEN_INQUIRE_BUTTON, NULL );
        hres = RawReadData( ReadBuffer, &ReadLength, NULL );

        if( !SUCCEEDED( hres ) || ( ReadLength < 9 ))
        {
            return hres;
        }

        if ( ReadBuffer[8] == '1' )
        {
			m_guidLastEvent = guidButton1;
            //Button push detected
            pDevStatus->dwEventHandlingState |= STI_EVENTHANDLING_PENDING;
            //Clear button push
            hres = RawWriteData( (void*)CLEAR_BUTTON, LEN_CLEAR_BUTTON, NULL );
        }
    }

    return hres;
}

STDMETHODIMP UsdHPSJDevice::DeviceReset( VOID )
{
    HRESULT hres = STI_OK;

    // Reset current active device
    if (INVALID_HANDLE_VALUE != m_DeviceDataHandle)
    {
        // Reset scanner
        BYTE Buffer[3] = "\033E";
        m_dwLastOperationError = RawWriteData( Buffer, sizeof(Buffer), NULL );
    }

    hres = HRESULT_FROM_WIN32(m_dwLastOperationError);

    return hres;
}

STDMETHODIMP UsdHPSJDevice::Diagnostic( LPSTI_DIAG pBuffer )
{
    HRESULT hres = STI_OK;

    // Initialize response buffer
    pBuffer->dwStatusMask = 0;
    ZeroMemory(&pBuffer->sErrorInfo,sizeof(pBuffer->sErrorInfo));
    pBuffer->sErrorInfo.dwGenericError = NOERROR;
    pBuffer->sErrorInfo.dwVendorError = 0;

    // turn on lamp
    m_dwLastOperationError = RawWriteData( (void*)TURN_ON_LAMP, LEN_DIAGS, NULL );
    hres = HRESULT_FROM_WIN32(m_dwLastOperationError);
    if( !SUCCEEDED( hres )) {
        pBuffer->sErrorInfo.dwGenericError = m_dwLastOperationError;
        return hres;
    }

    SleepEx( 2000, FALSE );

    // turn off lamp
    m_dwLastOperationError = RawWriteData( (void*)TURN_OFF_LAMP, LEN_DIAGS, NULL );
    hres = HRESULT_FROM_WIN32(m_dwLastOperationError);
    if( !SUCCEEDED( hres )) {
        pBuffer->sErrorInfo.dwGenericError = m_dwLastOperationError;
        return hres;
    }

    // get status string
    m_dwLastOperationError = RawWriteData( (void*)SELF_TEST, LEN_DIAGS, NULL );
    hres = HRESULT_FROM_WIN32(m_dwLastOperationError);
    if( !SUCCEEDED( hres )) {
        pBuffer->sErrorInfo.dwGenericError = m_dwLastOperationError;
        return hres;
    }

    m_dwLastOperationError = RawWriteData( (void*)STATUS_STRING, LEN_DIAGS, NULL );
    hres = HRESULT_FROM_WIN32(m_dwLastOperationError);
    if( !SUCCEEDED( hres )) {
        pBuffer->sErrorInfo.dwGenericError = m_dwLastOperationError;
        return hres;
    }

    BYTE    ReadBuffer[30];
    DWORD   ReadLength = 30;

    ZeroMemory( ReadBuffer, sizeof( ReadBuffer ));

    m_dwLastOperationError = RawReadData( ReadBuffer, &ReadLength, NULL );
    if( !SUCCEEDED( hres ))
    {
        pBuffer->sErrorInfo.dwGenericError = STIERR_GENERIC;
        return hres;
    }

    hres = HRESULT_FROM_WIN32(m_dwLastOperationError);
    if( !SUCCEEDED( hres ))
    {
        pBuffer->sErrorInfo.dwGenericError = m_dwLastOperationError;
        return hres;
    }

    if( ReadBuffer[10] != 'P' )
    {
        pBuffer->sErrorInfo.dwGenericError = STIERR_GENERIC;
    }

    return hres;
}

STDMETHODIMP UsdHPSJDevice:: SetNotificationHandle( HANDLE hEvent )
{
	return STIERR_UNSUPPORTED;
}


STDMETHODIMP UsdHPSJDevice::GetNotificationData( LPSTINOTIFY pBuffer )
{
    HRESULT hres = STI_OK;

    //
    // If we have notification ready - return it's guid
    //
    if (!IsEqualIID(m_guidLastEvent,GUID_NULL)) {
        pBuffer->guidNotificationCode  = m_guidLastEvent;
        m_guidLastEvent = GUID_NULL;
        pBuffer->dwSize = sizeof(STINOTIFY);
		ZeroMemory( &pBuffer->abNotificationData, sizeof(pBuffer->abNotificationData));
    }
    else {
        hres = STIERR_NOEVENTS;
    }

    return hres;
}

STDMETHODIMP UsdHPSJDevice::Escape( STI_RAW_CONTROL_CODE    EscapeFunction,
                                    LPVOID                  pInData,
                                    DWORD                   cbInDataSize,
                                    LPVOID                  pOutData,
                                    DWORD                   cbOutDataSize,
                                    LPDWORD                 pcbActualData )
{
    HRESULT hres = STIERR_UNSUPPORTED;

    //
    // Write indata to device  if needed.
    //

    return hres;
}

STDMETHODIMP UsdHPSJDevice::GetLastError( LPDWORD pdwLastDeviceError )
{
    HRESULT hres = STI_OK;

    if ( IsBadWritePtr( pdwLastDeviceError,4 ))
    {
        hres = STIERR_INVALID_PARAM;
    }
    else
    {
        *pdwLastDeviceError = m_dwLastOperationError;
    }

    return hres;
}

STDMETHODIMP UsdHPSJDevice::GetLastErrorInfo(STI_ERROR_INFO *pLastErrorInfo)
{
    HRESULT hres = STI_OK;

    if ( IsBadWritePtr( pLastErrorInfo,4 ))
    {
        hres = STIERR_INVALID_PARAM;
    }
    else
    {
        pLastErrorInfo->dwGenericError = m_dwLastOperationError;
        pLastErrorInfo->szExtendedErrorText[0] = L'\0';
    }

    return hres;
}

STDMETHODIMP UsdHPSJDevice::LockDevice( VOID )
{
    HRESULT hres = STI_OK;

    return hres;
}

STDMETHODIMP UsdHPSJDevice::UnLockDevice( VOID )
{
    HRESULT hres = STI_OK;

    return hres;
}

STDMETHODIMP UsdHPSJDevice::RawReadData( LPVOID lpBuffer, LPDWORD lpdwNumberOfBytes,
                                        LPOVERLAPPED lpOverlapped )
{
    HRESULT hres = STI_OK;
    BOOL    fRet;
    DWORD   dwBytesReturned = 0;

    if (INVALID_HANDLE_VALUE != m_DeviceDataHandle)
    {
        fRet = ReadFile( m_DeviceDataHandle,
                         lpBuffer,
                         *lpdwNumberOfBytes,
                         &dwBytesReturned,
                         lpOverlapped );

        m_dwLastOperationError = ::GetLastError();
        hres = fRet ? STI_OK : HRESULT_FROM_WIN32(m_dwLastOperationError);

        *lpdwNumberOfBytes = (fRet) ? dwBytesReturned : 0;
    }
    else
    {
        hres = STIERR_NOT_INITIALIZED;
    }

    return hres;
}

STDMETHODIMP UsdHPSJDevice::RawWriteData( LPVOID lpBuffer, DWORD dwNumberOfBytes,
                                            LPOVERLAPPED lpOverlapped )
{
    HRESULT hres = STI_OK;
    BOOL    fRet;
    DWORD   dwBytesReturned = 0;

    if (INVALID_HANDLE_VALUE != m_DeviceDataHandle)
    {
        fRet = WriteFile(m_DeviceDataHandle,lpBuffer,dwNumberOfBytes,&dwBytesReturned,lpOverlapped);
        m_dwLastOperationError = ::GetLastError();
        hres = fRet ? STI_OK : HRESULT_FROM_WIN32(m_dwLastOperationError);
    }
    else
    {
        hres = STIERR_NOT_INITIALIZED;
    }

    return hres;
}

STDMETHODIMP UsdHPSJDevice::RawReadCommand( LPVOID lpBuffer, LPDWORD lpdwNumberOfBytes,
                                            LPOVERLAPPED lpOverlapped )
{
    HRESULT hres = STIERR_UNSUPPORTED;

    return hres;
}

STDMETHODIMP UsdHPSJDevice::RawWriteCommand( LPVOID lpBuffer, DWORD nNumberOfBytes,
                                            LPOVERLAPPED lpOverlapped )
{
    HRESULT hres = STIERR_UNSUPPORTED;

    return hres;
}


STDMETHODIMP UsdHPSJDevice::Initialize( PSTIDEVICECONTROL pDcb, DWORD dwStiVersion,
                                        HKEY hParametersKey )
{
    HRESULT hres = STI_OK;
    UINT    uiNameLen = 0;

    if (!pDcb)
    {
        hres = STIERR_INVALID_PARAM;
    }
    else
    {
        // Check STI specification version number
        m_pDcb = pDcb;
        m_pDcb->AddRef();

        // Get the name of the device port we need to open
        hres = m_pDcb->GetMyDevicePortName(m_szDeviceName,sizeof(m_szDeviceName)/sizeof(WCHAR));
        if (!SUCCEEDED(hres) || !*m_szDeviceName)
        {
            return hres;
        }

        // Convert name to SBCS for use in WIn95
        uiNameLen = WideCharToMultiByte(CP_ACP, 0, m_szDeviceName, -1, NULL, NULL, 0, 0);
        if (!uiNameLen)
        {
            return STIERR_INVALID_PARAM;
        }

        m_pszDeviceNameA = new CHAR[uiNameLen+1];
        if (!m_pszDeviceNameA)
        {
            return STIERR_INVALID_PARAM;
        }

        WideCharToMultiByte(CP_ACP, 0, m_szDeviceName, -1, m_pszDeviceNameA, uiNameLen, 0, 0);

        // Open device ourselves
        m_DeviceDataHandle = CreateFileA( m_pszDeviceNameA,
                                         GENERIC_READ | GENERIC_WRITE, // Access mask
                                         0,                            // Share mode
                                         NULL,                         // SA
                                         OPEN_EXISTING,                // Create disposition
                                         FILE_ATTRIBUTE_SYSTEM,        // Attributes
                                         NULL );
        m_dwLastOperationError = ::GetLastError();

        hres = (m_DeviceDataHandle != INVALID_HANDLE_VALUE) ?
                    S_OK : MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,m_dwLastOperationError);

    }

    return hres;
}

