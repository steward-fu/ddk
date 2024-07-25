//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

// realclk.h - real time clock interrupt class definition
// Copyright (c) Compuware Corporation

class RTCInterrupt : public VHardwareInt
{
public:
	RTCInterrupt(VOID (*pCallback)());
	~RTCInterrupt();

	virtual VOID OnHardwareInt(VMHANDLE);
	VOID Enable(BOOL b=TRUE);

	static BYTE ReadRegister(BYTE reg);
	static VOID WriteRegister(BYTE reg, BYTE value);

protected:
	VOID (*m_callback)();
	BYTE m_originalA;
	BYTE m_originalB;
};

#define RTC_IRQ 8

// Required CMOS port definitions 

#define MINUTES		0x2
#define HOUR		0x4

#define STATREG_A	0xA
#define STATREG_B	0xB
#define STATREG_C	0xC

// Status Register B flags

#define ENABLE_INTERRUPT 0x40	
#define HOURMODE24	 0x2
