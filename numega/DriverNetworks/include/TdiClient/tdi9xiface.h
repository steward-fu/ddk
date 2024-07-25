// tdi9xiface.h		-		Low-level interface to TDI VxD
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#ifndef _tdi9xiface_h
#define _tdi9xiface_h

/////////////////////////////////////////////////////////////////////
// KTDInterface
//
// Provides system-specific interface to TDI service. It defines
// *how* do we talk to the TDI layer.
//
// For Win9X, this is done via fptr table interface exposed by
// VTDI.VxD. Provided by "MSTCP" registered transport.
//
class KTDInterface {
	static TDIDispatchTable* sm_pProviders[];
	static const char	szDefaultProvider[];
protected:
	TDIDispatchTable*	m_pTDI;
public:
	KTDInterface(const char* szProvider=NULL, UINT nREQs=1);
    KTDInterface(const KTDInterface& c) { m_pTDI=c.m_pTDI; }
    ~KTDInterface() {}

	static BOOLEAN __cdecl	Initialize(void);

	BOOLEAN IsPresent(void) const
    { return m_pTDI != NULL; }

	BOOLEAN WaitForCompletion(ULONG mSec=0) {return TRUE;}

    // TDI table methods, inline, sys-independent API

	TDI_STATUS TdiOpenAddress(PTDI_REQUEST pReq, PTRANSPORT_ADDRESS pTA,
                    uint uProto, PVOID pOpt=NULL)
        {
    	return	(m_pTDI->TdiOpenAddressEntry)(pReq, pTA, uProto, pOpt);
        }
	TDI_STATUS TdiCloseAddress(PTDI_REQUEST pReq)
        {
    	return	(m_pTDI->TdiCloseAddressEntry)(pReq);
        }
	TDI_STATUS		TdiOpenConnection(PTDI_REQUEST pReq, PVOID pCxt)
        {
    	return	(m_pTDI->TdiOpenConnectionEntry)(pReq, pCxt);
        }
	TDI_STATUS		TdiCloseConnection(PTDI_REQUEST pReq)
        {
    	return	(m_pTDI->TdiCloseConnectionEntry)(pReq);
        }
	TDI_STATUS		TdiAssociateAddress(PTDI_REQUEST pReq, HANDLE AddrHandle)
        {
    	return	(m_pTDI->TdiAssociateAddressEntry)(pReq, AddrHandle);
        }
	TDI_STATUS		TdiDisAssociateAddress(PTDI_REQUEST pReq)
        {
    	return	(m_pTDI->TdiDisAssociateAddressEntry)(pReq );
        }
	TDI_STATUS		TdiConnect(PTDI_REQUEST pReq, UINT* pTMO,
                	PTDI_CONNECTION_INFORMATION RequestAddr,
                    PTDI_CONNECTION_INFORMATION ReturnAddr)
        {
    	return	(m_pTDI->TdiConnectEntry)(pReq,pTMO,RequestAddr,ReturnAddr);
        }
	TDI_STATUS		TdiDisconnect(PTDI_REQUEST pReq, UINT* pTMO, ushort flags,
                	PTDI_CONNECTION_INFORMATION pConn, PTDI_CONNECTION_INFORMATION pRet)
        {
    	return	(m_pTDI->TdiDisconnectEntry)(pReq,pTMO,flags,pConn,pRet);
        }
	TDI_STATUS		TdiListen(PTDI_REQUEST pReq, ushort Flags,
                	PTDI_CONNECTION_INFORMATION AcceptableAddr,
                    PTDI_CONNECTION_INFORMATION ConnectedAddr)
        {
    	return	(m_pTDI->TdiListenEntry)(pReq, Flags, AcceptableAddr, ConnectedAddr);
        }
	TDI_STATUS		TdiAccept(PTDI_REQUEST pReq, PTDI_CONNECTION_INFORMATION AcceptInfo,
                    	PTDI_CONNECTION_INFORMATION ConnectedInfo)
        {
    	return	(m_pTDI->TdiAcceptEntry)(pReq,AcceptInfo, ConnectedInfo);
        }
	TDI_STATUS		TdiReceive(PTDI_REQUEST pReq, ushort *Flags, uint *RcvLength,
                    	PNDIS_BUFFER pBuf)
        {
    	return	(m_pTDI->TdiReceiveEntry)(pReq, Flags, RcvLength, pBuf);
        }
	TDI_STATUS		TdiSend(PTDI_REQUEST pReq, ushort Flags, uint SendLength,
                        PNDIS_BUFFER pBuf)
        {
    	return	(m_pTDI->TdiSendEntry)(pReq, Flags, SendLength, pBuf);
        }
	TDI_STATUS		TdiSendDatagram(PTDI_REQUEST pReq, PTDI_CONNECTION_INFORMATION pConn,
                    	uint Size, uint* pRetSize, PNDIS_BUFFER pBuf)
        {
    	return	(m_pTDI->TdiSendDatagramEntry)(pReq,pConn, Size, pRetSize, pBuf);
        }
	TDI_STATUS		TdiReceiveDatagram(PTDI_REQUEST pReq,
                    	PTDI_CONNECTION_INFORMATION pConn,
                    	PTDI_CONNECTION_INFORMATION pRet, uint Size,
                        uint* pSize, PNDIS_BUFFER pBuf)
        {
    	return	(m_pTDI->TdiReceiveDatagramEntry)(pReq, pConn, pRet, Size, pSize, pBuf);
        }
	TDI_STATUS		TdiSetEvent(PVOID pHandle, int Type, PVOID Handler, PVOID Context)
        {
    	return	(m_pTDI->TdiSetEventEntry)(pHandle, Type, Handler, Context);
        }
	TDI_STATUS		TdiQueryInformation(PTDI_REQUEST pReq, uint QueryType,
                    	PNDIS_BUFFER pBuf, uint *BufferSize, uint IsConn)
        {
    	return	(m_pTDI->TdiQueryInformationEntry)(pReq,QueryType,pBuf,BufferSize,IsConn);
        }
	TDI_STATUS		TdiSetInformation(PTDI_REQUEST pReq, uint QueryType,
                    	PNDIS_BUFFER pBuf, uint BufferSize, uint IsConn)
        {
    	return	(m_pTDI->TdiSetInformationEntry)(pReq,QueryType,pBuf,BufferSize,IsConn);
        }

    TDI_STATUS      TdiQueryInformationEx(PTDI_REQUEST pReq,
                    	struct TDIObjectID * pTDIo, PNDIS_BUFFER pBuf,
                            uint * pSize, void * pCxt)
        {
        return	(m_pTDI->TdiQueryInformationExEntry)(pReq, pTDIo, pBuf, pSize, pCxt);
        }
    TDI_STATUS      TdiSetInformationEx(PTDI_REQUEST pReq,
                    	struct TDIObjectID *pTDIo, void * pBuf, uint size)
        {
        return	(m_pTDI->TdiSetInformationExEntry)(pReq, pTDIo, pBuf, size);
        }

#if 0 // According to available doc, non-supported
    TDI_STATUS		TdiAction(PTDI_REQUEST pReq, uint ActionType,
                          PNDIS_BUFFER pBuf, uint BufferSize)
        {
    	return	(m_pTDI->TdiActionEntry)(pReq,ActionType,pBuf, BufferSize);
        }
#endif
};

class K9xSemaphore
{
public:
	K9xSemaphore(){m_Semaphore = CreateSemaphore();}
	~K9xSemaphore(){if(m_Semaphore) DestroySemaphore(m_Semaphore);}

	VOID Wait(){WaitSemaphore(m_Semaphore);}
	VOID Signal(){SignalSemaphore(m_Semaphore);}

	BOOLEAN IsValid(){return (m_Semaphore != 0);}
protected:
	static ULONG CreateSemaphore();
	static VOID DestroySemaphore(ULONG semaphore);
	static VOID WaitSemaphore(ULONG semaphore);
	static VOID SignalSemaphore(ULONG semaphore);

//Data
	ULONG m_Semaphore;
};

#endif // _tdi9xiface_h
