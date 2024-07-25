// E100bexPhy.h: interface for the E100bexPhy class.
//
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
//
// Portions copied from Microsoft Windows 2000 DDK sample driver containing the
// following copyright
//
/****************************************************************************
** COPYRIGHT (C) 1994-1997 INTEL CORPORATION                               **
** DEVELOPED FOR MICROSOFT BY INTEL CORP., HILLSBORO, OREGON               **
** HTTP://WWW.INTEL.COM/                                                   **
** THIS FILE IS PART OF THE INTEL ETHEREXPRESS PRO/100B(TM) AND            **
** ETHEREXPRESS PRO/100+(TM) NDIS 5.0 MINIPORT SAMPLE DRIVER               **
****************************************************************************/


#if !defined(AFX_E100BEXPHY_H__0E5098A9_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
#define AFX_E100BEXPHY_H__0E5098A9_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


///////////////////////////////////////////////////////////////////////
// E100bexPhy		 
//
// This class models the Phy component of the NIC.  
//
class E100bexPhy {

// construction
public:

	E100bexPhy(
		UINT PhyAddress, 
		UCHAR Connector, 
		UCHAR AiForceDpx,
		USHORT AiTempSpeed,
		UCHAR Congest,
#if _MSC_VER > 1200
		KNdisMemoryRegister& Reg
#else
		KNdisMemoryRegisterSafe<ULONG>& Reg
#endif
		);

// methods
public:

	VOID	Reset(void);
	BOOLEAN	Detect(void);
	NDIS_MEDIA_STATE GetConnectionStatus(void);

	// Accessors
	UINT	GetPhyAddress(void)		   { return m_PhyAddress; }
	UINT	GetCurrentDuplexMode(void) { return m_AiDuplexCur; }
	ULONG	GetCurrentLineSpeed(void)  { return m_AiLineSpeedCur; }

protected:

	BOOLEAN Setup(void);
	VOID	FindSpeedAndDpx(UINT PhyId);
	VOID	Select(UINT SelectPhyAddress, BOOLEAN WaitAutoNeg);
	USHORT	MdiRead(ULONG RegAddress, ULONG MdiAddress);
	VOID	MdiWrite(ULONG MIIRegAddress, ULONG PhyMdiAddress, USHORT DataValue);

// data
protected:

	UINT	m_PhyId;			// revision of the PHY
	USHORT	m_AiDuplexCur;		// Current duplex mode
	USHORT	m_AiLineSpeedCur;	// Current line speed
	UCHAR	m_AiForceDpx;		// duplex setting
	USHORT	m_AiTempSpeed;		// 'Speed', user over-ride of line speed
	UCHAR	m_Congest;			// Enables congestion control
	UINT	m_PhyAddress;		// Address of the phy component
	UCHAR	m_Connector;		// 0=Auto, 1=TPE, 2=MII
#if _MSC_VER > 1200
	KNdisMemoryRegister&		m_MDIControl;	// MDI Control Register
#else
	KNdisMemoryRegisterSafe<ULONG>&		m_MDIControl;	// MDI Control Register
#endif
};


///////////////////////////////////////////////////////////////////////
// E100bexPhy constructor		 
//
inline E100bexPhy::E100bexPhy(
	UINT PhyAddress, 
	UCHAR Connector, 
	UCHAR AiForceDpx,
	USHORT AiTempSpeed,
	UCHAR Congest,
#if _MSC_VER > 1200
	KNdisMemoryRegister& Reg
#else
	KNdisMemoryRegisterSafe<ULONG>& Reg
#endif
	) : m_MDIControl(Reg)
{ 
	m_PhyAddress = PhyAddress; 
	m_Connector = Connector; 
	m_AiForceDpx = AiForceDpx; 
	m_AiTempSpeed = AiTempSpeed; 
	m_Congest = Congest; 
}

///////////////////////////////////////////////////////////////////////
// E100bexPhy::GetConnectionStatus 		 
//
inline NDIS_MEDIA_STATE E100bexPhy::GetConnectionStatus()
{
    // Read the status register at phy 1
    USHORT MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);
		   MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);

    if (MdiStatusReg & MDI_SR_LINK_STATUS)
	{
        return NdisMediaStateConnected;
	}
    else
	{
        return NdisMediaStateDisconnected;
	}
}


#endif // !defined(AFX_E100BEXPHY_H__0E5098A9_1538_11D4_AFF3_00C04F6A5DA9__INCLUDED_)
