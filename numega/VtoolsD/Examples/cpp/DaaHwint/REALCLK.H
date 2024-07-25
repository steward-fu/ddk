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
//
// Copyright (c) 1998, Compuware Corporation

#include <vdw.h>

///////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////// 
//
//  Definitions

// Define a callback function prototype for RTCInterrupt
typedef 
VOID
(*PRTC_CALLBACK) (						// We will call this routine
    PVOID pContext						// at DISPATCH_LEVEL
    );

// Interrupt for Real Time Clock
#define RTC_IRQ 8

// Required CMOS port definitions 
#define RTC_SECONDS		0x0
#define RTC_MINUTES		0x2
#define RTC_HOURS		0x4

#define RTC_STATREG_A	0xA
#define RTC_STATREG_B	0xB
#define RTC_STATREG_C	0xC

// Status Register A flags
#define RTC_UPDATE_IN_PROGRESS 0x80

// Status Register B flags
#define RTC_ENABLE_INTERRUPT	0x40	
#define RTC_HOURMODE24			0x2

// Macro to convert a byte of binary coded decimal (BCD) to int
#define BCDtoInt(x) (((x) & 0xf) + ((((x) & 0xf0) >> 4) * 10))

///////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////// 
//
//  Class

class RTCInterrupt : public KInterrupt
{
	public:
	
	// Constructors (2 forms)
	
	RTCInterrupt(PRTC_CALLBACK pCallback, PVOID pContext);
	RTCInterrupt();
	VOID Initialize(PRTC_CALLBACK pCallback, PVOID pContext);
	~RTCInterrupt();

	// Methods
	
	MEMBER_ISR(RTCInterrupt, OnHardwareInt)

	VOID Enable(BOOL bEnable=TRUE);

	BYTE ReadRegister(BYTE reg);
	VOID WriteRegister(BYTE reg, BYTE value);

	// Data
	
	protected:
	
	KIoRange 		m_Ports;
	KIoRegister		m_AddressPort;
	KIoRegister		m_DataPort;
	
	PRTC_CALLBACK	m_pCallback;
	PVOID			m_pContext;
	BYTE			m_originalA;
	BYTE			m_originalB;

#if 0
	public:
	// The following member function is actually defined by the
	// MEMBER_ISR macro.  This comment-only definition allows
	// easy navigation to the function within the Developer Studio.
	COMMENT_ONLY BOOLEAN OnHardwareInt(void);
#endif
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// In-lines

////////////////////////////////////////////////////////////////////////
// RTCInterrupt::RTCInterrupt
//
// Constructor for the DaahwintDevice class
//
// Input
//		None
//
// Output	
//		None
//
// Notes	
//  This form of the constructor is used in conjunction with the Initialize()
//		member function.
//
inline
RTCInterrupt::RTCInterrupt() 
			: KInterrupt()
{
}



