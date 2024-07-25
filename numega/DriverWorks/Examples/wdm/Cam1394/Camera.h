#ifndef __CAMERA_H__
#define __CAMERA_H__

// Defines the bit fields for Feature Registers of the Camera
typedef struct _FEATURE_REGISTER {
	ULONG       MaxValue:12;           // Bits 20-31
	ULONG       MinValue:12;           // Bits 8-19
	ULONG       ManualMode:1;            // Bit 7
	ULONG       AutoMode:1;             // Bit 6
	ULONG       OnOff:1;                // Bit 5
	ULONG       ReadOutInq:1;          // Bit 4
	ULONG       OnePush:1;              // Bit 3
	ULONG       Reserved:2;             // Bits 1-2
	ULONG       PresenceInq:1;          // Bit 0

//	operator ULONG() { return MaxValue; }
	operator PULONG() { return PULONG(this); }


	_FEATURE_REGISTER ByteSwap() { 
		PULONG pR = (PULONG) this;
		ULONG R = *pR;
		::bswap(R);
		return *this;
	}
} FEATURE_REGISTER, *PFEATURE_REGISTER;


/*
typedef struct DEVICE_REGISTER 
{
	FEATURE_REGISTER Reg;

	operator FEATURE_REGISTER() { return Reg; }
//	operator ULONG() { return (ULONG) Reg; }
	operator ULONG&() { return (ULONG&) Reg; }

} DEVICE_REGISTER, *PDEVICE_REGISTER;
*/

inline KTrace& operator << (KTrace& T, const FEATURE_REGISTER& R)
{
	T.Trace(TraceInfo,"MaxValue:%x, MinValue:%x, ManualMode:%x, AutoMode:%x, "
		"OnOff:%x, ReadOutInq:%x, OnePush:%x, PresenceIng:%x\n",
		R.MaxValue, R.MinValue, R.ManualMode, R.AutoMode, R.OnOff,
		R.ReadOutInq, R.OnePush, R.PresenceInq
		);

	return T;
}


// Digital Camera Register definitions
typedef struct _CAMERA_REGISTERS
{
	ULONG   Initialize;         // 0x000 - Initialize Camera to default 
	ULONG   Reserved[63];       // 0x004 - 0x0FC unused
	ULONG   VFormatInq;         // 0x100 - video format inquiry
	ULONG   Reserved2[31];      // 0x104 - 0x17C unused
	ULONG   VModeInq[8];        // 0x180 - video mode inquiry 
	ULONG   Reserved3[24];      // 0x1A0 - 0x1FC unused

	ULONG   VRateInq[128];      // 0x200 - video frame rate inquiry

	ULONG   BasicInq;           // 0x400 - basic function inquiry
	ULONG   HiFeatureInq;       // 0x404 - High feature inquiry
	ULONG   LoFeatureInq;       // 0x408 - Low feature inquiry
	ULONG   Reserved4[61];      // 0x40C - 0x4FC unused

    FEATURE_REGISTER	BrightnessInq;     // 0x500
    FEATURE_REGISTER	ExposureInq;       // 0x504
    FEATURE_REGISTER	SharpnessInq;      // 0x508
    FEATURE_REGISTER	WhiteBalanceInq;   // 0x50c
    FEATURE_REGISTER	HueInq;            // 0x510
    FEATURE_REGISTER	SaturationInq;     // 0x514
    FEATURE_REGISTER	GammaInq;          // 0x518
    FEATURE_REGISTER	ShutterInq;        // 0x51c
    FEATURE_REGISTER	GainInq;           // 0x520
    FEATURE_REGISTER	IrisInq;           // 0x524
    FEATURE_REGISTER	FocusInq;          // 0x528
	ULONG   Reserved5[53];      // 0x500 - Inquiry Registers

	ULONG   VideoFrameRate;     // 0x600 - current video frame rate
	ULONG   VideoMode;          // 0x604 - current video frame
	ULONG   VideoFormat;        // 0x608 - current video mode
	ULONG   IsoChannel;         // 0x60C
	ULONG   CameraPower;        // 0x610
	ULONG   IsoEnable;          // 0x614
	ULONG   MemorySave;         // 0x618
	ULONG   OneShot;            // 0x61C
	ULONG   MemorySaveChannel;  // 0x620
	ULONG   MemoryChannel;      // 0x624
	ULONG   Reserved6[118];     // 0x628 - 0x7FC
	ULONG   FeatureControStatus[34];// 0x800 - feature control/status registers

} CAMERA_REGISTERS, *PCAMERA_REGISTERS;


#define CAMERA_OFFSET(x) FIELD_OFFSET(CAMERA_REGISTERS, x)


// Definitions of the Frame Rate register 
const ULONG		FRAME_RATE_1		=	0x1;	// 1 : 3.75
const ULONG		FRAME_RATE_2		=	0x2;	// 2 : 7.5
const ULONG		FRAME_RATE_3		=	0x3;	// 3 : 15 (DEFAULT_FRAME_RATE)
const ULONG		FRAME_RATE_4		=	0x4;	// 4 : 30 
const ULONG		FRAME_RATE_5		=	0x5;	// 5 : 60 
const ULONG		FRAME_RATE_SHIFT	=	5;

const ULONG		FORMAT_VGA_NON_COMPRESSED = 0;

// Information for Isochronous Transfers
typedef struct _CAMERA_ISOCH_INFO {

    ULONG	m_BytesPerPacket;	// number of quadlets in each Isochronous packet
    ULONG	m_Speed;			// speed required to receive this mode
    ULONG	m_Size;				// picture size at this mode

} CAMERA_ISOCH_INFO, *PCAMERA_ISOCH_INFO;


const ULONG		RATES_PER_MODE		=	6;

const ULONG		START_ISOCH_TRANSMISSION = 0x80;
const ULONG		STOP_ISOCH_TRANSMISSION	 = 0x0;
const ULONG		START_OF_IMAGE		=	1;
const ULONG		MAX_ISO_FRAME		=	640;
const ULONG		INITIALIZE_BIT		= 0x80;


// Definitions of Video Mode
const ULONG		VMODE0_YUV444	= 0x0;
const ULONG		VMODE1_YUV422	= 0x1;
const ULONG		VMODE2_YUV411	= 0x2;
const ULONG		VMODE3_YUV422	= 0x3;
const ULONG		VMODE4_RGB24	= 0x4;
const ULONG		VMODE5_YUV800	= 0x5;


// Information for Isochronous Transfers
// based on Video Mode
const CAMERA_ISOCH_INFO IsochInfoTable[] = {

// Video Mode 0
	0,      SPEED_FLAGS_100,        0,          // Reserved 
	0,      SPEED_FLAGS_100,        0,          // 3.75 fps 
	15,     SPEED_FLAGS_100,        57600,      // 7.5  fps 
	30,     SPEED_FLAGS_100,        57600,      // 15   fps 
	60,     SPEED_FLAGS_100,        57600,      // 30   fps 
	0,      SPEED_FLAGS_100,        0,          // 60   fps 

// Video Mode 1
	0,      SPEED_FLAGS_100,        0,          // Reserved 
	20,     SPEED_FLAGS_100,        153600,     // 3.75 fps 
	40,     SPEED_FLAGS_100,        153600,     // 7.5  fps 
	80,     SPEED_FLAGS_100,        153600,     // 15   fps 
	160,    SPEED_FLAGS_100,        153600,     // 30   fps 
	0,      SPEED_FLAGS_100,        0,          // 60   fps 

// Video Mode 2
	0,      SPEED_FLAGS_100,        0,          // Reserved 
	60,     SPEED_FLAGS_100,        460800,     // 3.75 fps 
	120,    SPEED_FLAGS_100,        460800,     // 7.5  fps 
	240,    SPEED_FLAGS_100,        460800,     // 15   fps 
	480,    SPEED_FLAGS_200,        460800,     // 30   fps 
	0,      SPEED_FLAGS_100,        0,          // 60   fps 

// Video Mode 3
	0,      SPEED_FLAGS_100,        0,          // Reserved 
	80,     SPEED_FLAGS_100,        614400,     // 3.75 fps 
	160,    SPEED_FLAGS_100,        614400,     // 7.5  fps 
	320,    SPEED_FLAGS_200,        614400,     // 15   fps 
	640,    SPEED_FLAGS_400,        614400,     // 30   fps 
	0,      SPEED_FLAGS_100,        0,          // 60   fps 

// Video Mode 4
	0,      SPEED_FLAGS_100,        0,          // Reserved 
	120,    SPEED_FLAGS_100,        921600,     // 3.75 fps 
	240,    SPEED_FLAGS_100,        921600,     // 7.5  fps 
	480,    SPEED_FLAGS_200,        921600,     // 15   fps 
	960,    SPEED_FLAGS_400,        921600,     // 30   fps 
	0,      SPEED_FLAGS_100,        0,          // 60   fps 

// Video Mode 5
	0,      SPEED_FLAGS_100,        0,          // Reserved 
	40,     SPEED_FLAGS_100,        307200,     // 3.75 fps 
	80,     SPEED_FLAGS_100,        307200,     // 7.5  fps 
	160,    SPEED_FLAGS_100,        307200,     // 15   fps 
	320,    SPEED_FLAGS_200,        307200,     // 30   fps 
	640,    SPEED_FLAGS_400,        307200,     // 60   fps 
        
};


#endif // __CAMERA_H__