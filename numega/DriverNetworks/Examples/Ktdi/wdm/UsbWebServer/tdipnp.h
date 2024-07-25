// tdipnp.h - TdiPnpCallbacks class 
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001-2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

class TdiPnpCallbacks : public KTdiPnpClient
{
public:
	TdiPnpCallbacks() : m_ProviderReady(FALSE), m_DeviceObject(NULL) {}
	~TdiPnpCallbacks() 
	{
		// Guard against OnNetworkReady() accessing the device
		eUsbThermometer* pDevice = (eUsbThermometer*)InterlockedExchangePointer(
			(PVOID*)&m_DeviceObject, NULL);

		if (pDevice)
		{
			// there still exists a reference on the device object.
			// release the pointer to the device object
			pDevice->DecrementOutstandingRequestCount();
		}
	}

	VOID SetDeviceObject(eUsbThermometer* pDevice) 
	{ 
		// This must be called from AddDevice to avoid race conditions
		// with Pnp handlers.
		ASSERT(pDevice);

		ASSERT(!m_DeviceObject);
		// allow only the first device object to get notified of
		// network ready.  
		if (!m_DeviceObject) 
		{
			// Guard against the device going away by referencing the object
			pDevice->IncrementOutstandingRequestCount();

			m_DeviceObject = pDevice; 
		}
	}

	// It is called when the network is ready to take traffic so that
	// the web server can be started.
	VOID OnNetworkReady()
	{
		if (m_DeviceObject && m_ProviderReady)
		{
			// Guard against the destructor accessing the device 
			eUsbThermometer* pDevice = (eUsbThermometer*)InterlockedExchangePointer(
				(PVOID*)&m_DeviceObject, NULL);

			if (pDevice)
			{
				// notify the device that the network is ready
				pDevice->InitializeTDI(); 

				// release the pointer to the device object
				m_DeviceObject->DecrementOutstandingRequestCount();
			}
		}
	}

	VOID OnProviderReady(KNdisString& DeviceName)
	{
		m_ProviderReady = TRUE;
	}

protected:
	eUsbThermometer* m_DeviceObject;	// A pointer to our device object
	BOOLEAN			 m_ProviderReady;
};
