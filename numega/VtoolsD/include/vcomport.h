// Copyright (c) 1994, Compuware Corporation All rights reserved
//
// vcomport.h - include file for class VCommPort and class VCommPortDriver

#ifdef WIN40
#define NPORTFUNCS  0x17
#else
#define NPORTFUNCS  0x13
#endif
#define PORTFUNCTHUNKSIZE 0x10
#define PDTHUNKSIZE 0x10
#define COMMREQUEST BOOL _cdecl

class VCommPortDriver;

class VCommPort 
{
public:
	VCommPort(PCHAR name);
	virtual ~VCommPort(void)
	{
		if (m_name) free(m_name);
	}
protected:
	virtual COMMREQUEST Initialize(DEVNODE handle, DWORD baseIO, 
					DWORD irq);
	virtual COMMREQUEST Open(VMHANDLE hVM, PDWORD pError);
	virtual COMMREQUEST Close(); 

	virtual COMMREQUEST GetCommState(P_DCB pDCB);
	virtual COMMREQUEST GetProperties(_COMMPROP* pCommProp);
	virtual COMMREQUEST SetCommState(P_DCB pDCB, DWORD ActionMask);
	virtual COMMREQUEST Setup(PCHAR RxQueue, DWORD cbRxQueue, 
					PCHAR TxQueue, DWORD cbTxQueue);

	virtual COMMREQUEST ClearError(_COMSTAT* pComstat, PDWORD pError);
	virtual COMMREQUEST GetQueueStatus(_COMSTAT* pComstat);
	virtual COMMREQUEST Purge(DWORD qType);
	virtual COMMREQUEST Read(PCHAR buf, DWORD cbRequest, PDWORD pRxCount);
	virtual COMMREQUEST TransmitChar(CHAR ch);
	virtual COMMREQUEST Write(PCHAR buf, DWORD cbRequest,PDWORD pTxCount);

	virtual COMMREQUEST EnableNotification(PCOMMNOTIFYPROC pCallback, 
							PVOID refData);
	virtual COMMREQUEST EscapeFunction(DWORD lFunc, DWORD InData, 
							PVOID pOutData);
	virtual COMMREQUEST GetModemStatus(PDWORD pModemStatus);
	virtual COMMREQUEST GetEventMask(DWORD mask, PDWORD pEvents);
	virtual COMMREQUEST SetEventMask(DWORD mask, PDWORD pEvents);
	virtual COMMREQUEST SetModemStatusShadow(PBYTE pMSRShadow);

// The following two member functions replace SetReadCallback and 
// SetWriteCallback. Note the upper case 'B'. The change was made so that
// the types of refData would be consistent across all members functions.
// Two simple steps make the conversion for existing drivers:
// 1. change the name ('b' -> 'B')
// 2. change the type of the last parameter to PVOID

	virtual COMMREQUEST SetReadCallBack(DWORD RxTrigger, 
				PCOMMNOTIFYPROC pCallback, PVOID refData);
	virtual COMMREQUEST SetWriteCallBack(DWORD TxTrigger, 
				PCOMMNOTIFYPROC pCallback, PVOID refData);

// The following two member functions were changed for VtoolsD 2.02 (and 1.15)
// The only difference is the type of the last parameter, which has been
// changed to PVOID to conform to EnableNotification and PCOMMNOTIFYPROC.
// Note that the return type has been changed to force a compiler diagnostic
// when rebuilding drivers that used the old prototype.

#define OBSOLETE void	
	virtual OBSOLETE    SetReadCallback(DWORD RxTrigger, 
				PCOMMNOTIFYPROC pCallback, DWORD refData);
	virtual OBSOLETE    SetWriteCallback(DWORD TxTrigger, 
				PCOMMNOTIFYPROC pCallback, DWORD refData);

#ifdef WIN40
	virtual COMMREQUEST GetCommConfig(PCOMMCONFIG pCC, PDWORD pSize);
	virtual COMMREQUEST SetCommConfig(PCOMMCONFIG pCC, DWORD dwSize);
	virtual COMMREQUEST GetError(PDWORD pError);
	virtual COMMREQUEST CtnNotifyHandler(DWORD Notification);
	virtual DWORD __cdecl DeviceIOCtl(DWORD IoControlCode, PVOID pInBuf, DWORD nInBufferSize, 
							PVOID pOutBuf,DWORD  nOutBufferSize,
							PIOCTLNOTIFICATIONFUNC pNotify, IOCtlBlock*  pIOCtlBlock);
#endif

	PORTDATA 	m_data;
	_DCB		m_dcb;	

	PCHAR 		m_name;
	DWORD 		m_PortNumber;
	VMHANDLE 	m_ownerVM;
	VCommPort* 	m_chain;
	COMMPORTHANDLE	m_handle;
	DWORD		m_PortFlags;
#ifdef WIN40
	PCTNHANDLER	m_ContentionHandler;
	HANDLE		m_ResourceHandle;
	BOOLEAN		m_StealFlag;
#endif

private:
	VCommPort(void) {m_name = NULL;}

	VOID 		InitPortData();
	VOID 		InitPortFunctions();
	BYTE 		m_thunks[PORTFUNCTHUNKSIZE*NPORTFUNCS];
	PortFunctions	m_Functions;
#ifdef WIN40
	BYTE		m_CtnNotifyThunk[PORTFUNCTHUNKSIZE];
#endif
	friend		VCommPortDriver;
};

// VCommPort::m_PortFlags values
#define PF_RXQINTERNAL		0x00000001
#define PF_TXQINTERNAL		0x00000002
#define PF_PORTADDED		0x00000004


class VCommPortDriver : public VDevice
{

public:
	virtual BOOL OnSysDynamicDeviceExit(void);

	VCommPort* GetNextPort(VCommPort* pPort)
	{
		if (pPort == NULL)
			return m_PortList;
		else
			return pPort->m_chain;
	}

protected:
	BOOL Register();
	VOID DoAcquireRelease(BOOL b);
	VOID DeletePort(VCommPort* pPort);
	VOID DeleteAllPorts(void);

	// -- creates a new port when needed by DriverControl
	//		in Win95.  Default implementation doesn't do anything.
	//		Your method should construct one of your VCommPort
	//		derived objects	and return a pointer to it! --
	virtual VCommPort* CreatePort(PCHAR szPortName)	{ return NULL; }

private:
	VOID _cdecl DriverControl(DWORD thunkRet, DWORD function, 
	    		DEVNODE handle, PVOID refData, PVOID args, ...);

	PPORTDATA _cdecl PortOpen(DWORD thunkRet, PCHAR szPortName, 
			VMHANDLE hVM, PDWORD pError);
	BOOL _cdecl PortClose(DWORD thunkRet, PPORTDATA pPortData);

	VCommPort* LookupPort(PCHAR szPortName);
	VCommPort* LookupPort(PPORTDATA pPortData);

	static BOOL m_bDoAcquireRelease;
	static VCommPort* m_PortList;

 	BYTE m_dcthunk[PDTHUNKSIZE];
 	BYTE m_openthunk[PDTHUNKSIZE];
 	BYTE m_closethunk[PDTHUNKSIZE];

	friend VCommPort;
};
