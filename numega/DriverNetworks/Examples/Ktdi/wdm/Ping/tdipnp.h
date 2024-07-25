//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
#ifdef TDI_WINDOWS_NT

class TdiPnpCallbacks : public KTdiPnpClient
{
public:
	VOID SetDeviceObject(PingDevice* theDeviceObject){m_DeviceObject = theDeviceObject;}
	TdiPnpCallbacks():m_ProviderReady(FALSE){}
	
	//This is the only callback we are interested in.  
	//It will be called when the network is ready to take traffic so that
	//we can start our web server.
	VOID OnNetworkReady()
	{
		if(m_DeviceObject && m_ProviderReady)
			m_DeviceObject->InitializeTDI(); //Start our web server
	}
	
	VOID OnProviderReady(KNdisString& DeviceName)
	{
		m_ProviderReady = TRUE;
	}
	
protected:
	PingDevice* m_DeviceObject; //A pointer to our device object
	BOOLEAN m_ProviderReady;
};

#endif