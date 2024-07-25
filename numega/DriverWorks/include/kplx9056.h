// plx9056.h - KPlx9056 class declaration and implementation
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#ifndef __KPLX9056_H__
#define __KPLX9056_H__

///////////////////////////////////////////////////////////////////////////////////
// Local Bus Access Mode
//
typedef enum _PLX9056_ACCESS_MODE
{
	Plx9056BitSize8 = 0,
	Plx9056BitSize16 = 1,
	Plx9056BitSize32 = 3
} PLX9056_ACCESS_MODE;

///////////////////////////////////////////////////////////////////////////////////
// Reason for ISR
//
typedef enum _PLX9056_ISR_REASON
{
	Plx9056ReasonNone = 0,
	Plx9056ReasonDoorbell = 1,
	Plx9056ReasonPciAbort = 2,
	Plx9056ReasonLocalInput = 4,
	Plx9056ReasonDma0 = 8,
	Plx9056ReasonDma1 = 16,
	Plx9056ReasonOutboundPost = 32

} PLX9056_ISR_REASON;

///////////////////////////////////////////////////////////////////////////////////
// There is no |= operator for enum type
//
inline PLX9056_ISR_REASON operator |= (PLX9056_ISR_REASON& a, PLX9056_ISR_REASON b)
{
	return (a = (PLX9056_ISR_REASON)(ULONG(a) | ULONG(b)));
}

///////////////////////////////////////////////////////////////////////////////////
// KPlx9056 class
//		This class abstracts PLX9056 Chipset
//
class KPlx9056
{
	// Constructors
public:
	SAFE_DESTRUCTORS;
	KPlx9056() : m_bValid(false), m_Pci(NULL) {}
	~KPlx9056() { Invalidate(); }

	// init/deinit/validate methods
#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
	NTSTATUS Initialize(PDEVICE_OBJECT TopOfStack, PCM_RESOURCE_LIST pTranslated, PCM_RESOURCE_LIST pRaw, PKSERVICE_ROUTINE pIsr, PVOID pContext);
#else
	NDIS_STATUS Initialize(KNdisMiniAdapter* pAdapter, KNdisConfig& Config);
#endif

	VOID Invalidate();
	bool IsValid() { return m_bValid; }

public:
	////////////////////////////////////////
	// Interrupts

	// this context is used to synchronize
	// access to registers with ISR
	typedef struct _SET_CLEAR_CONTEXT
	{
		KPlx9056*		m_pPlx;
		ULONG			m_reg;
		ULONG			m_set;
		ULONG			m_clear;

		_SET_CLEAR_CONTEXT(KPlx9056* pPlx, ULONG reg, ULONG set, ULONG clear) :
		m_pPlx(pPlx), m_reg(reg), m_set(set), m_clear(clear) {}
	} SET_CLEAR_CONTEXT, *PSET_CLEAR_CONTEXT;

	void EnableInterrupts(bool bIsr = false);
	void DisableInterrupts(bool bIsr = true);
	PLX9056_ISR_REASON GetIsrReason();
	VOID ClearIsrReason(PLX9056_ISR_REASON Reason);
	ULONG Doorbell() { return m_Doorbell; }

	/////////////////////////////////
	// Direct Slave Mode
	void SetAccessMode(ULONG Space, PLX9056_ACCESS_MODE Mode);
	void SetRange(ULONG Space, ULONG Size);
	ULONG GetRange(ULONG Space);
	void SetRemap(ULONG Space, ULONG Address);

	NTSTATUS BusIopRead(ULONG Space, ULONG LocalAddress, bool bRemap, PVOID pDest, ULONG Size, PLX9056_ACCESS_MODE Mode = Plx9056BitSize32);
	NTSTATUS BusIopWrite(ULONG Space, ULONG LocalAddress, bool bRemap, PVOID pDest, ULONG Size, PLX9056_ACCESS_MODE Mode = Plx9056BitSize32);


	////////////////////////////////////////
	// Dma
	void SetBlockedDma(ULONG Channel, PHYSICAL_ADDRESS PciAddr, ULONG LocalAddr, ULONG Length, bool bRead);
	void SetSgDma(ULONG Channel, PHYSICAL_ADDRESS PciAddr, ULONG LocalAddr, ULONG Length, bool bRead, bool bValid, ULONG DescPtr);
	static void SetSgDesc(PPLX9056_DESCRIPTOR pDesc, PHYSICAL_ADDRESS PciAddr, ULONG LocalAddr, ULONG Length, bool bRead, bool bValid, bool bEnd, ULONG DescPtr);
	void StartDma(ULONG Channel);
	void StopDma(ULONG Channel);

	////////////////////////////////////////
	// Eeprom
	VOID EepromClock();
	VOID EepromSendCommand(ULONG cmd, ULONG Length);
	VOID EepromWrite(ULONG Offset, USHORT value);
	USHORT EepromRead(ULONG Offset);

	////////////////////////////////////////
	// Reset the chip
	VOID Reset();

	////////////////////////////////////////
	// Hardware registers lock
	void Lock(BOOLEAN bDpcLevel = FALSE) { m_HwLock.Lock(bDpcLevel); }
	void Unlock() { m_HwLock.Unlock(); }

protected:
	static BOOLEAN _set_clear_isr(PSET_CLEAR_CONTEXT p)
	{
		ULONG v = p->m_pPlx->m_MemConfig[p->m_reg];
		v |= p->m_set;
		v &= ~p->m_clear;
		p->m_pPlx->m_MemConfig[p->m_reg] = v;

		return TRUE;
	}

	void set_bit(ULONG reg, ULONG set)
	{
		SET_CLEAR_CONTEXT context(this, reg, set, 0);
		m_Interrupt.Synchronize((PKSYNCHRONIZE_ROUTINE)_set_clear_isr, &context);
	}

	void clear_bit(ULONG reg, ULONG clear)
	{
		SET_CLEAR_CONTEXT context(this, reg, 0, clear);
		m_Interrupt.Synchronize((PKSYNCHRONIZE_ROUTINE)_set_clear_isr, &context);
	}

public:
#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
	KMemoryRange		m_MemConfig;		// Memory mapped Configuration Registers
	KIoRange			m_IoConfig;			// I/O Mapped Configuration Registers
	KMemoryRange		m_LocalAddrSpace0;	// Local Address Space 0
	KMemoryRange		m_LocalAddrSpace1;	// Local Address Space 1
	KInterrupt			m_Interrupt;		// Interrupt

	KPciConfiguration*	m_Pci;				// PCI Configuration Registers

protected:
	KSpinLock			m_HwLock;			// Lock to synchronize register access
#else
	KNdisMemoryRange	m_MemConfig;		// Memory mapped Configuration Registers
	KNdisIoRange		m_IoConfig;			// I/O Mapped Configuration Registers
	KNdisMemoryRange	m_LocalAddrSpace0;	// Local Address Space 0
	KNdisMemoryRange	m_LocalAddrSpace1;	// Local Address Space 1
	KNdisInterrupt		m_Interrupt;		// Interrupt

	KNdisPciInformation* m_Pci;				// PCI Configuration Registers

protected:
	KNdisSpinLock		m_HwLock;			// Lock to synchronize register access
#endif

protected:
	bool				m_bValid;			// track valid state
	ULONG				m_Doorbell;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// implementation
//

////////////////////////////////////////////////////////////////////////
//  KPlx9056::Initialize
//
//	Routine Description:
//		This routine is initialized the class
//
//	Parameters:
//		[in] pTranslated
//			pointer to translated resources
//
//		[in] pRaw
//			pointer to raw resources
//
//		[in] pIsr
//			pointer to ISR routine
//
//		[in] pContext
//			pointer to context for ISR
//
//	Return Value:
//		status
//
//	Comments:
//		This function should be called from OnStartDevice
//
#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
inline NTSTATUS KPlx9056::Initialize(PDEVICE_OBJECT TopOfStack, PCM_RESOURCE_LIST pTranslated, PCM_RESOURCE_LIST pRaw, PKSERVICE_ROUTINE pIsr, PVOID pContext)
{
	NTSTATUS status;

	do
	{
		// Initialize PCI Configuration
		m_Pci = new KPciConfiguration(TopOfStack);
		if ( !m_Pci )
		{
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

		// Initialize configuration register memory space
		status = m_MemConfig.Initialize(pTranslated, pRaw, 0);
		if ( !NT_SUCCESS(status) )
			break;

		// Initialize configuration register i/o space
		status = m_IoConfig.Initialize(pTranslated, pRaw, 0);
		if ( !NT_SUCCESS(status) )
			break;

		// Initialize Local Space 0
		status = m_LocalAddrSpace0.Initialize(pTranslated, pRaw, 1);
		if ( !NT_SUCCESS(status) )
			break;

		// Initialize Local Space 1
		status = m_LocalAddrSpace1.Initialize(pTranslated, pRaw, 2);
		if ( !NT_SUCCESS(status) )
			break;

		// make sure that interrupts are disabled
		DisableInterrupts(true);

		// Initialize Interrupt
		status = m_Interrupt.InitializeAndConnect(pTranslated, pIsr, pContext);
		if ( !NT_SUCCESS(status) )
			break;

		m_bValid = true;

	} while ( false );

	return status;
}
#else
inline NDIS_STATUS KPlx9056::Initialize(KNdisMiniAdapter* pAdapter, KNdisConfig& Config)
{
	NDIS_STATUS status;

	KNdisPnpResourceRequest request(Config);

	do
	{
		// Initialize PCI Configuration
		m_Pci = new KNdisPciInformation(pAdapter);
		if ( !m_Pci )
		{
			status = NDIS_STATUS_RESOURCES;
			break;
		}

		// Initialize configuration register memory space
		KNdisResource<CmResourceTypeMemory>	Memory0(request, 0);
		if ( !Memory0.IsValid() )
		{
			status = NDIS_STATUS_NOT_ACCEPTED;
			break;
		}

		status = m_MemConfig.Initialize(pAdapter, Memory0);
		if ( status != NDIS_STATUS_SUCCESS )
			break;

		// Initialize configuration register i/o space
		KNdisResource<CmResourceTypePort> Port(request);
		if ( !Port.IsValid() )
		{
			status = NDIS_STATUS_NOT_ACCEPTED;
			break;
		}

		status = m_IoConfig.Initialize(pAdapter, Port);
		if ( status != NDIS_STATUS_SUCCESS )
			break;

		// Initialize Local Space 0
		KNdisResource<CmResourceTypeMemory>	Memory1(request, 1);
		if ( !Memory1.IsValid() )
		{
			status = NDIS_STATUS_NOT_ACCEPTED;
			break;
		}

		status = m_LocalAddrSpace0.Initialize(pAdapter, Memory1);
		if ( status != NDIS_STATUS_SUCCESS )
			break;

		// Initialize Local Space 1
		KNdisResource<CmResourceTypeMemory>	Memory2(request, 2);
		if ( !Memory2.IsValid() )
		{
			status = NDIS_STATUS_NOT_ACCEPTED;
			break;
		}

		status = m_LocalAddrSpace1.Initialize(pAdapter, Memory2);
		if ( status != NDIS_STATUS_SUCCESS )
			break;

		// Initialize Interrupt
		KNdisResource<CmResourceTypeInterrupt> Int(request);
		if ( !Int.IsValid() )
		{
			status = NDIS_STATUS_NOT_ACCEPTED;
			break;
		}

		// make sure that interrupts are disabled
		DisableInterrupts(true);

		status = m_Interrupt.Initialize(pAdapter, Int, NdisInterruptLevelSensitive);
		if ( !NT_SUCCESS(status) )
			break;

		m_bValid = true;

	} while ( false );

	return status;
}
#endif

////////////////////////////////////////////////////////////////////////
//  KPlx9056::Invalidate
//
//	Routine Description:
//		This routine frees allocated resources
//
//	Parameters:
//		[in] bPresent
//			true if hardware is still present
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KPlx9056::Invalidate()
{
	m_bValid = false;

	m_Interrupt.Invalidate();
	m_LocalAddrSpace0.Invalidate();
	m_LocalAddrSpace1.Invalidate();
	m_IoConfig.Invalidate();
	m_MemConfig.Invalidate();

	if ( m_Pci )
	{
		delete m_Pci;
		m_Pci = NULL;
	}
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::EnableInterrupts
//
//	Routine Description:
//		enables interrupts
//
//	Parameters:
//		[in] bIsr
//			true if routine is called from ISR
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline void KPlx9056::EnableInterrupts(bool bIsr)
{
	if ( bIsr )
		m_MemConfig[PLX9056_INTCSR] |= PLX9056_INTCSR_IE;
	else
		set_bit(PLX9056_INTCSR, PLX9056_INTCSR_IE);
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::DisableInterrupts
//
//	Routine Description:
//		disables interrupts
//
//	Parameters:
//		[in] bIsr
//			true if routine is called from ISR
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline void KPlx9056::DisableInterrupts(bool bIsr)
{
	if ( bIsr )
		m_MemConfig[PLX9056_INTCSR] &= ~PLX9056_INTCSR_IE;
	else
		clear_bit(PLX9056_INTCSR, PLX9056_INTCSR_IE);
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::GetIsrReason
//
//	Routine Description:
//		Examines the reason of ISR
//
//	Parameters:
//		None
//
//	Return Value:
//		ISR Reason
//
//	Comments:
//		None
//
inline PLX9056_ISR_REASON KPlx9056::GetIsrReason()
{
	PLX9056_ISR_REASON Reason = Plx9056ReasonNone;

	ULONG IntCS = m_MemConfig[PLX9056_INTCSR];

	if ( IntCS & PLX9056_INTCSR_IE )
	{
		// check pci abort bit
		if ( (IntCS & PLX9056_INTCSR_AIA) && (IntCS & PLX9056_INTCSR_AIE) )
			Reason |= Plx9056ReasonPciAbort;

		// check pci doorbell bit
		if ( (IntCS & PLX9056_INTCSR_DIA) && (IntCS & PLX9056_INTCSR_DIE) )
			Reason |= Plx9056ReasonDoorbell;

		// check local input bit
		if ( (IntCS & PLX9056_INTCSR_LIIA) && (IntCS & PLX9056_INTCSR_LIIE) )
			Reason |= Plx9056ReasonLocalInput;

		// check dma0
		if ( (IntCS & PLX9056_INTCSR_DMA0IA) && (IntCS & PLX9056_INTCSR_DMA0IE) && ((ULONG)m_MemConfig[PLX9056_DMAMODE0] & PLX9056_DMAMODE0_IS) )
			Reason |= Plx9056ReasonDma0;

		// check dma1
		if ( (IntCS & PLX9056_INTCSR_DMA1IA) && (IntCS & PLX9056_INTCSR_DMA1IE) && ((ULONG)m_MemConfig[PLX9056_DMAMODE1] & PLX9056_DMAMODE1_IS) )
			Reason |= Plx9056ReasonDma1;

		// check outbound post queue
		ULONG reg = m_MemConfig[PLX9056_OPQIS];
		if ( reg & PLX9056_OPQIS_I )
		{
			reg = m_MemConfig[PLX9056_OPQIM];
			if ( (reg & PLX9056_OPQIM_M) == 0 )
			{
				Reason |= Plx9056ReasonOutboundPost;
			}
		}
	}

	return Reason;
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::ClearIsrReason
//
//	Routine Description:
//		Clears the ISR reason
//
//	Parameters:
//		[in] Reason
//			ISR Reason
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KPlx9056::ClearIsrReason(PLX9056_ISR_REASON Reason)
{
	ULONG reg;

	// disable local input interrupt
	if ( Reason & Plx9056ReasonLocalInput )
		set_bit(PLX9056_INTCSR, PLX9056_INTCSR_LIIE);

	m_HwLock.Lock();

	if ( Reason & Plx9056ReasonDoorbell )
	{
		// read doorbell register and write it back
		// to clear interrupt
		m_Doorbell = m_MemConfig[PLX9056_L2PDBELL];
		m_MemConfig[PLX9056_L2PDBELL] = m_Doorbell;
	}

	if ( Reason & Plx9056ReasonPciAbort )
	{
		// to clear pci abort interrupt
		// read command register and write it back
#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
		m_Pci->ReadHeader(&reg, PLX9056_PCICR, 4);
		m_Pci->WriteHeader(&reg, PLX9056_PCICR, 4);
#else
		m_Pci->Read(PLX9056_PCICR, &reg);
		m_Pci->Write(PLX9056_PCICR, reg);
#endif
	}

	// mask outbound queue interrupt
	if ( Reason & Plx9056ReasonOutboundPost )
		m_MemConfig[PLX9056_OPQIM] |= PLX9056_OPQIM_M;

	// clear dma0 interrupt
	if ( Reason & Plx9056ReasonDma0 )
		m_MemConfig[PLX9056_DMACSR0] |= PLX9056_DMACSR0_CI;

	// clear dma1 interrupt
	if ( Reason & Plx9056ReasonDma1 )
		m_MemConfig[PLX9056_DMACSR1] |= PLX9056_DMACSR1_CI;

	m_HwLock.Unlock();
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::SetAccessMode
//
//	Routine Description:
//		Set Local Bus Access Mode
//
//	Parameters:
//		[in] Space
//			Local Address Space (0, 1)
//
//		[in] Mode
//			New Access Mode
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline void KPlx9056::SetAccessMode(ULONG Space, PLX9056_ACCESS_MODE Mode)
{
	ULONG reg;

	if ( Space == 0 )
		reg = PLX9056_LBRD0;
	else
		reg = PLX9056_LBRD1;

	m_MemConfig[reg] = ((ULONG)m_MemConfig[reg] & 0xFFFFFFFC) | Mode;
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::SetRange
//
//	Routine Description:
//		Set Local Address Mapping Range
//
//	Parameters:
//		[in] Space
//			Local Address Space (0, 1)
//
//		[in] Size
//			New mapping size
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline void KPlx9056::SetRange(ULONG Space, ULONG Size)
{
	ULONG reg;

	if ( Space == 0 )
		reg = PLX9056_LAS0RR;
	else
		reg = PLX9056_LAS1RR;

	m_MemConfig[reg] = ~(Size - 1) | 0x8;
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::GetRange
//
//	Routine Description:
//		Get Local Address Mapping Range
//
//	Parameters:
//		[in] Space
//			Local Address Space (0, 1)
//
//	Return Value:
//		Current Mapping Size
//
//	Comments:
//		None
//
inline ULONG KPlx9056::GetRange(ULONG  Space)
{
	ULONG reg;

	if ( Space == 0 )
		reg = PLX9056_LAS0RR;
	else
		reg = PLX9056_LAS1RR;

	ULONG Size = m_MemConfig[reg];

	Size = ~(Size & 0xFFFFFFF0) + 1;

	return Size;
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::SetRemap
//
//	Routine Description:
//		Set Local Address Mapping Address
//
//	Parameters:
//		[in] Space
//			Local Address Space (0, 1)
//
//		[in] Address
//			New local mapping address
//
//	Return Value:
//		Current Mapping Size
//
//	Comments:
//		None
//
inline void KPlx9056::SetRemap(ULONG Space, ULONG Address)
{
	ULONG reg;

	if ( Space == 0 )
		reg = PLX9056_LAS0BA;
	else
		reg = PLX9056_LAS1BA;

	m_MemConfig[reg] = (Address & 0xFFFFFFF0) | 0x1;
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::BusIopRead
//
//	Routine Description:
//		Reads Local Bus in Direct Slave Mode
//
//	Parameters:
//		[in] Space
//			Local Address Space (0, 1)
//
//		[in] LocalAddress
//			Address to read from. If bRemap is true, then it is an
//			absolute offset on local bus, otherwise it is a relative
//			offset to the current mapping
//
//		[in] bRemap
//			if true, then we need to remap the PCI to Local mapping
//
//		[out] pDest
//			buffer to hold the data
//
//		[in] Size
//			size of the buffer
//
//		[in] Mode
//			local bus access mode
//
//	Return Value:
//		status
//
//	Comments:
//		None
//
inline NTSTATUS KPlx9056::BusIopRead(ULONG Space, ULONG LocalAddress, bool bRemap, PVOID pDest, ULONG Size, PLX9056_ACCESS_MODE Mode)
{
	NTSTATUS status = STATUS_SUCCESS;

	m_HwLock.Lock();

	// set local bus width
	SetAccessMode(Space, Mode);

#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
	KMemoryRange& MemoryRange = (Space == 0) ? m_LocalAddrSpace0 : m_LocalAddrSpace1;
#else
	KNdisMemoryRange& MemoryRange = (Space == 0) ? m_LocalAddrSpace0 : m_LocalAddrSpace1;
#endif

	ULONG Offset = bRemap ? 0 : LocalAddress;

	// remap local space if required
	if ( bRemap )
		SetRemap(Space, LocalAddress);

	if ( Offset + Size <= GetRange(Space) )
	{
		if ( ((ULONG_PTR)pDest & 0x1) || (Size & 0x1) || (Mode == Plx9056BitSize8) )
			MemoryRange.inb(Offset, (PUCHAR)pDest, Size);
		else if ( ((ULONG_PTR)pDest & 0x2) || (Size & 0x2) || (Mode == Plx9056BitSize16) )
			MemoryRange.inw(Offset, (PUSHORT)pDest, Size/sizeof(USHORT));
		else
			MemoryRange.ind(Offset, (PULONG)pDest, Size/sizeof(ULONG));
	}
	else
		status = STATUS_INVALID_PARAMETER;

	m_HwLock.Unlock();

	return status;
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::BusIopWrite
//
//	Routine Description:
//		Writes Local Bus in Direct Slave Mode
//
//	Parameters:
//		[in] Space
//			Local Address Space (0, 1)
//
//		[in] LocalAddress
//			Address to read from. If bRemap is true, then it is an
//			absolute offset on local bus, otherwise it is a relative
//			offset to the current mapping
//
//		[in] bRemap
//			if true, then we need to remap the PCI to Local mapping
//
//		[out] pDest
//			buffer to hold the data
//
//		[in] Size
//			size of the buffer
//
//		[in] Mode
//			local bus access mode
//
//	Return Value:
//		status
//
//	Comments:
//		None
//
inline NTSTATUS KPlx9056::BusIopWrite(ULONG Space, ULONG LocalAddress, bool bRemap, PVOID pDest, ULONG Size, PLX9056_ACCESS_MODE Mode)
{
	NTSTATUS status = STATUS_SUCCESS;

	m_HwLock.Lock();

	// set local bus width
	SetAccessMode(Space, Mode);

#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
	KMemoryRange& MemoryRange = (Space == 0) ? m_LocalAddrSpace0 : m_LocalAddrSpace1;
#else
	KNdisMemoryRange& MemoryRange = (Space == 0) ? m_LocalAddrSpace0 : m_LocalAddrSpace1;
#endif

	ULONG Offset = bRemap ? 0 : LocalAddress;

	// remap local space if required
	if ( bRemap )
		SetRemap(Space, LocalAddress);

	if ( Offset + Size <= GetRange(Space) )
	{
		if ( ((ULONG_PTR)pDest & 0x1) || (Size & 0x1) || (Mode == Plx9056BitSize8) )
			MemoryRange.outb(Offset, (PUCHAR)pDest, Size);
		else if ( ((ULONG_PTR)pDest & 0x2) || (Size & 0x2) || (Mode == Plx9056BitSize16) )
			MemoryRange.outw(Offset, (PUSHORT)pDest, Size/sizeof(USHORT));
		else
			MemoryRange.outd(Offset, (PULONG)pDest, Size/sizeof(ULONG));
	}
	else
		status = STATUS_INVALID_PARAMETER;

	m_HwLock.Unlock();

	return status;
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::StartDma
//
//	Routine Description:
//		Starts DMA
//
//	Parameters:
//		[in] Channel
//			Dma Channel
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KPlx9056::StartDma(ULONG Channel)
{
	if ( Channel == 0 )
		m_MemConfig[PLX9056_DMACSR0] |= (UCHAR)(PLX9056_DMACSR0_E | PLX9056_DMACSR0_S);
	else
		m_MemConfig[PLX9056_DMACSR1] |= (UCHAR)(PLX9056_DMACSR1_E | PLX9056_DMACSR1_S);
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::StopDma
//
//	Routine Description:
//		Stops DMA
//
//	Parameters:
//		[in] Channel
//			Dma Channel
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KPlx9056::StopDma(ULONG Channel)
{
	if ( Channel == 0 )
		m_MemConfig[PLX9056_DMACSR0] |= PLX9056_DMACSR0_A;
	else
		m_MemConfig[PLX9056_DMACSR1] |= PLX9056_DMACSR1_A;
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::SetBlockedDma
//
//	Routine Description:
//		Sets Dma registers for blocked Dma
//
//	Parameters:
//		[in] Channel
//			Dma Channel
//
//		[in] PciAddr
//			Address on PCI bus
//
//		[in] LocalAddr
//			Address on Local bus
//
//		[in] Length
//			Length of transfer
//
//		[in] bRead
//			true if it is read from local bus
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline void KPlx9056::SetBlockedDma(ULONG Channel, PHYSICAL_ADDRESS PciAddr, ULONG LocalAddr, ULONG Length, bool bRead)
{
	if ( Channel == 0 )
	{
		m_MemConfig[PLX9056_DMAMODE0] = PLX9056_DMAMODE0_IS | PLX9056_DMAMODE0_DIE | PLX9056_DMAMODE0_LBE | PLX9056_DMAMODE0_TARIE | Plx9056BitSize32;
		m_MemConfig[PLX9056_DMAPADR0] = PciAddr.LowPart;
		m_MemConfig[PLX9056_DMADAC0] = (ULONG)PciAddr.HighPart;
		m_MemConfig[PLX9056_DMALADR0] = LocalAddr;
		m_MemConfig[PLX9056_DMASIZ0] = Length;
		m_MemConfig[PLX9056_DMADPR0] = (ULONG)(bRead ? PLX9056_DMADPR0_DT : 0);
	}
	else
	{
		m_MemConfig[PLX9056_DMAMODE1] = PLX9056_DMAMODE1_IS | PLX9056_DMAMODE1_DIE | PLX9056_DMAMODE1_LBE | PLX9056_DMAMODE1_TARIE | Plx9056BitSize32;
		m_MemConfig[PLX9056_DMAPADR1] = PciAddr.LowPart;
		m_MemConfig[PLX9056_DMADAC1] = (ULONG)PciAddr.HighPart;
		m_MemConfig[PLX9056_DMALADR1] = LocalAddr;
		m_MemConfig[PLX9056_DMASIZ1] = Length;
		m_MemConfig[PLX9056_DMADPR1] = (ULONG)(bRead ? PLX9056_DMADPR1_DT : 0);
	}
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::SetSgDma
//
//	Routine Description:
//		Sets Dma registers for scatter-gather dma
//
//	Parameters:
//		[in] Channel
//			Dma Channel
//
//		[in] PciAddr
//			Address on PCI bus
//
//		[in] LocalAddr
//			Address on Local bus
//
//		[in] Length
//			Length of transfer
//
//		[in] bRead
//			true if it is read from local bus
//
//		[in] bValid
//			if true, then set valid bit
//
//		[in] bEnd
//			if true, then set end of chain bit
//
//		[in] DescPtr
//			pointer to next descriptor
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline void KPlx9056::SetSgDma(ULONG Channel, PHYSICAL_ADDRESS PciAddr, ULONG LocalAddr, ULONG Length, bool bRead, bool bValid, ULONG DescPtr)
{
	if ( Channel == 0 )
	{
		m_MemConfig[PLX9056_DMAPADR0] = PciAddr.LowPart;
		m_MemConfig[PLX9056_DMADAC0] = (ULONG)PciAddr.HighPart;
		m_MemConfig[PLX9056_DMALADR0] = LocalAddr;
		m_MemConfig[PLX9056_DMASIZ0] = Length | (bValid ? PLX9056_DMASIZ0_V : 0);
		m_MemConfig[PLX9056_DMADPR0] = DescPtr | (bRead ? PLX9056_DMADPR0_DT : 0) | PLX9056_DMADPR0_DL;

		m_MemConfig[PLX9056_DMAMODE0] =
			PLX9056_DMAMODE0_IS |
			PLX9056_DMAMODE0_EOTEL |
			PLX9056_DMAMODE0_CCM |
			PLX9056_DMAMODE0_DIE |
			PLX9056_DMAMODE0_SGM |
			PLX9056_DMAMODE0_LBE |
			PLX9056_DMAMODE0_TARIE |
			Plx9056BitSize32;
	}
	else
	{
		m_MemConfig[PLX9056_DMAPADR1] = PciAddr.LowPart;
		m_MemConfig[PLX9056_DMADAC1] = (ULONG)PciAddr.HighPart;
		m_MemConfig[PLX9056_DMALADR1] = LocalAddr;
		m_MemConfig[PLX9056_DMASIZ1] = Length | (bValid ? PLX9056_DMASIZ1_V : 0);
		m_MemConfig[PLX9056_DMADPR1] = DescPtr | (bRead ? PLX9056_DMADPR1_DT : 0) | PLX9056_DMADPR1_DL;

		m_MemConfig[PLX9056_DMAMODE1] =
			PLX9056_DMAMODE1_IS |
			PLX9056_DMAMODE1_EOTEL |
			PLX9056_DMAMODE1_CCM |
			PLX9056_DMAMODE1_DIE |
			PLX9056_DMAMODE1_SGM |
			PLX9056_DMAMODE1_LBE |
			PLX9056_DMAMODE1_TARIE |
			Plx9056BitSize32;
	}
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::SetSgDesc
//
//	Routine Description:
//		Sets scatter-gather descriptor
//
//	Parameters:
//		[in] pDesc
//			pointer to the descriptor
//
//		[in] PciAddr
//			Address on PCI bus
//
//		[in] LocalAddr
//			Address on Local bus
//
//		[in] Length
//			Length of transfer
//
//		[in] bRead
//			true if it is read from local bus
//
//		[in] bValid
//			if true, then set valid bit
//
//		[in] bEnd
//			if true, then set end of chain bit
//
//		[in] DescPtr
//			pointer to next descriptor
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline void KPlx9056::SetSgDesc(PPLX9056_DESCRIPTOR pDesc, PHYSICAL_ADDRESS PciAddr, ULONG LocalAddr, ULONG Length, bool bRead, bool bValid, bool bEnd, ULONG DescPtr)
{
	pDesc->PciAddrLow = PciAddr.LowPart;
	pDesc->PciAddrHigh = PciAddr.HighPart;
	pDesc->LocalAddr = LocalAddr;
	pDesc->Size = Length | (bValid ? PLX9056_DMASIZ0_V : 0);

	pDesc->NextDescPtr = DescPtr | (bRead ? PLX9056_DMADPR0_DT : 0) | PLX9056_DMADPR0_DL | (bEnd ? PLX9056_DMADPR0_EC : 0);
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::EepromClock
//
//	Routine Description:
//		Triggers eeprom clock
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KPlx9056::EepromClock()
{
	ULONG reg = m_MemConfig[PLX9056_CNTRL];

	m_MemConfig[PLX9056_CNTRL] = reg | PLX9056_CNTRL_CLOCK;
	m_MemConfig[PLX9056_CNTRL] = reg & ~PLX9056_CNTRL_CLOCK;
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::EepromSendCommand
//
//	Routine Description:
//		Send command to eeprom
//
//	Parameters:
//		[in] cmd
//			command to send
//
//		[in] length
//			command length
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KPlx9056::EepromSendCommand(ULONG cmd, ULONG length)
{
	ULONG reg = m_MemConfig[PLX9056_CNTRL];

	reg &= ~(0xF << 24);
	m_MemConfig[PLX9056_CNTRL] = reg;

	reg |= PLX9056_CNTRL_CHIP_SELECT;
	m_MemConfig[PLX9056_CNTRL] = reg;

	for (int i = (int)(length - 1); i >= 0; i--)
	{
		if ( cmd & (1 << i) )
		{
			m_MemConfig[PLX9056_CNTRL] = reg | PLX9056_CNTRL_WRITE_BIT;
		}
		else
		{
			m_MemConfig[PLX9056_CNTRL] = reg;
		}

		EepromClock();
	}
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::EepromWrite
//
//	Routine Description:
//		Write eeprom
//
//	Parameters:
//		[in] Offset
//			Offset into eeprom
//
//		[in] value
//			value to write
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KPlx9056::EepromWrite(ULONG Offset, USHORT value)
{
	ULONG Shift = 2;
	ULONG Length = EE66_CMD_LEN;

	EepromSendCommand(EE_WREN << Shift, Length);

	EepromSendCommand((EE_WRITE << Shift) | (Offset >> 1), Length);

	ULONG reg = m_MemConfig[PLX9056_CNTRL];

	reg &= ~(0xF << 24);
	reg &= ~PLX9056_CNTRL_EEDO_ENABLE;
	reg |= PLX9056_CNTRL_CHIP_SELECT;

	for (int i = 15; i >= 0; i--)
	{
		if ( value & (1 << i) )
		{
			m_MemConfig[PLX9056_CNTRL] = reg | PLX9056_CNTRL_WRITE_BIT;
		}
		else
		{
			m_MemConfig[PLX9056_CNTRL] = reg;
		}

		EepromClock();
	}

	m_MemConfig[PLX9056_CNTRL] = reg & ~PLX9056_CNTRL_CHIP_SELECT;
	m_MemConfig[PLX9056_CNTRL] = reg | PLX9056_CNTRL_CHIP_SELECT;

	ULONG Timeout = 0;
	do
    {
        reg = m_MemConfig[PLX9056_CNTRL];
        Timeout++;

    } while ( !(reg & PLX9056_CNTRL_READ_BIT) && (Timeout < 10000) );

	EepromSendCommand(EE_WDS << Shift, Length);

	m_MemConfig[PLX9056_CNTRL] = reg & ~(0xF << 24);
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::EepromRead
//
//	Routine Description:
//		Read eeprom
//
//	Parameters:
//		[in] Offset
//			Offset into eeprom
//
//	Return Value:
//		read value
//
//	Comments:
//		None
//
inline USHORT KPlx9056::EepromRead(ULONG Offset)
{
	ULONG Shift = 2;
	ULONG Length = EE66_CMD_LEN;

	USHORT value = 0;

	EepromSendCommand((EE_READ << Shift) | (Offset >> 1), Length);

	ULONG reg = m_MemConfig[PLX9056_CNTRL];
	reg |= PLX9056_CNTRL_EEDO_ENABLE;
	m_MemConfig[PLX9056_CNTRL] = reg;

	for (int i = 0; i < 16; i++)
	{
		EepromClock();

        for (int count = 0; count < 100; count++)
        {
			reg = m_MemConfig[PLX9056_CNTRL];
        }

        if ( reg & PLX9056_CNTRL_READ_BIT )
        {
            value = (value << 1) | 1;
        }
        else
        {
            value = value << 1;
        }
	}

	reg &= ~PLX9056_CNTRL_EEDO_ENABLE;
	m_MemConfig[PLX9056_CNTRL] = reg & ~(0xF << 24);

	return value;
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::Reset
//
//	Routine Description:
//		Reset chip
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KPlx9056::Reset()
{
	ULONG Interrupt;
	ULONG Power;
	ULONG HotSwap;

	ULONG reg;

	// clear PCI errors
#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
	m_Pci->ReadHeader(&reg, PLX9056_PCICR, 4);
	if ( reg & 0xF8000000 )
	{
		m_Pci->WriteHeader(&reg, PLX9056_PCICR, 4);
	}
#else
	m_Pci->Read(PLX9056_PCICR, &reg);
	if ( reg & 0xF8000000 )
	{
		m_Pci->Write(PLX9056_PCICR, reg);
	}
#endif

	// Save state of I2O Decode Enable
	reg = m_MemConfig[PLX9056_QSR];
	bool bMuEnabled = (reg & 1);

	// Determine if an EEPROM is present
	reg = m_MemConfig[PLX9056_CNTRL];

	// Make sure S/W Reset & EEPROM reload bits are clear
	reg &= ~(PLX9056_CNTRL_RESET | PLX9056_CNTRL_RELOAD_CONFIG);

	ULONG Mailbox0;
	ULONG Mailbox1;

	// Save some registers if EEPROM present
	if ( reg & PLX9056_CNTRL_PRESENT )
	{
		Mailbox0 = m_MemConfig[PLX9056_MBOX0];
		Mailbox1 = m_MemConfig[PLX9056_MBOX1];

#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
		m_Pci->ReadHeader(&Interrupt, PLX9056_PCIILR, 4);
		m_Pci->ReadHeader(&Power, PLX9056_PMCSR, 4);
		m_Pci->ReadHeader(&HotSwap, PLX9056_HS_CNTL, 4);
#else
		m_Pci->Read(PLX9056_PCIILR, &Interrupt);
		m_Pci->Read(PLX9056_PMCSR, &Power);
		m_Pci->Read(PLX9056_HS_CNTL, &HotSwap);
#endif

		// Mask out HS bits that can be cleared
		Power &= ~PLX9056_PMCSR_PMES;
		HotSwap &= ~((PLX9056_HS_CSR_BIESI | PLX9056_HS_CSR_BRESI | PLX9056_HS_CSR_ENUMIC) << 16);
	}

	    // Issue Software Reset to hold PLX chip in reset
	m_MemConfig[PLX9056_CNTRL] = reg | PLX9056_CNTRL_RESET;

	// Delay for a bit
#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
	{
		LARGE_INTEGER time = { -100 * 10000, -1 }; // 100 ms
		KeDelayExecutionThread(KernelMode, FALSE, &time);
	}
#else
		NdisMSleep(100 * 1000);
#endif

	// Bring chip out of reset
	m_MemConfig[PLX9056_CNTRL] = reg;

	// Restore I2O Decode Enable state
	if ( bMuEnabled )
	{
		ULONG reg1 = m_MemConfig[PLX9056_QSR];
		m_MemConfig[PLX9056_QSR] = reg1 | 1;
	}

	// If EEPROM present, issue reload & restore registers
	if ( reg & PLX9056_CNTRL_PRESENT )
	{
		// Issuse EEPROM reload
		m_MemConfig[PLX9056_CNTRL] = reg | PLX9056_CNTRL_RELOAD_CONFIG;

		// Delay for a bit
#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
		{
			LARGE_INTEGER time = { -10 * 10000, -1 }; // 10 ms
			KeDelayExecutionThread(KernelMode, FALSE, &time);
		}
#else
		NdisMSleep(10 * 1000);
#endif

		// Clear EEPROM reload
		m_MemConfig[PLX9056_CNTRL] = reg;

		// Restore saved registers
		m_MemConfig[PLX9056_MBOX0] = Mailbox0;
		m_MemConfig[PLX9056_MBOX1] = Mailbox1;

#if !defined(KNDIS_WDM_LIBBLD) && !defined(__KNDIS_VERSION__)
		m_Pci->WriteHeader(&Interrupt, PLX9056_PCIILR, 4);
		m_Pci->WriteHeader(&Power, PLX9056_PMCSR, 4);
		m_Pci->WriteHeader(&HotSwap, PLX9056_HS_CNTL, 4);
#else
		m_Pci->Write(PLX9056_PCIILR, Interrupt);
		m_Pci->Write(PLX9056_PMCSR, Power);
		m_Pci->Write(PLX9056_HS_CNTL, HotSwap);
#endif
	}
}

#endif // __KPLX9056_H__
