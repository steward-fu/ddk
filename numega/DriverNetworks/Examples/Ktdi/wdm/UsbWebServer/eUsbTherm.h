// eUsbTherm.h - interface for eUsbThermometer class 
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2000 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

class eUsbThermometer : public KPnpDevice
{

// methods

	SAFE_DESTRUCTORS
public:
	eUsbThermometer(PDEVICE_OBJECT Pdo, ULONG unit);
	DEVMEMBER_DISPATCHERS

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by the
		// a DEVMEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	virtual NTSTATUS DeviceControlComplete(KIrp I);	// COMMENT_ONLY
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS Write(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
#endif

	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);
	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	NTSTATUS PassThru(KIrp I);

	NTSTATUS			InitializeTDI();
	NTSTATUS			GetTemperature();
	ULONG				CelsiusTemperature() { return m_dwCelsiusTemp; }
	inline const char * GetHostAddr();
	inline void			SetHostAddr(const char * p);

// data

public:
	KUsbLowerDevice m_Usb;
	ULONG m_Unit;

protected:
	ULONG									m_dwCelsiusTemp;	// Temperature in Celsius
    KStreamServer<KHttpServerSession>*		m_pListener;		// http socket
	PCHAR									m_pszHostIpAddr;	// pointer to Host IP address
	KInterlockedList<KHttpServerSession>	m_ActiveSessionList;// list of active sessions

private:

	//Prevent the compiler from generating default copy 
	//constructor and assignment operator
	eUsbThermometer(eUsbThermometer&);
	operator=(eUsbThermometer&);

	friend class KHttpServerSession;
};


const char * eUsbThermometer::GetHostAddr()
{
	return (const char *)m_pszHostIpAddr;
}

void eUsbThermometer::SetHostAddr(const char * p)
{
	if( NULL == p ) return;

	// only allow one time set operation....
	if( m_pszHostIpAddr ) return;

	size_t len = strlen(p);
	m_pszHostIpAddr = new CHAR[len+1];
	if( m_pszHostIpAddr )
	{
		RtlZeroMemory( m_pszHostIpAddr, len+1);
		strcpy(m_pszHostIpAddr,p);
	}
}