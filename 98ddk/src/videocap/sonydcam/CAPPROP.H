
NTSTATUS
SonyDCamReadRegisterAbsolute(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension,
	ULONG ulLowAddress,
	ULONG * pulValue
);

NTSTATUS
SonyDCamReadRegister(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension,
	ULONG ulFieldOffset,
	ULONG * pulValue
);  

NTSTATUS
SonyDCamWriteRegister(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension,
	ULONG ulFieldOffset,
	ULONG ulValue
);

BOOL
GetPropertyValuesFromRegistry(
	PSONYDCAM_EXTENSION DeviceExtension
	);

BOOL
SetPropertyValuesToRegistry(
	PSONYDCAM_EXTENSION DeviceExtension
	);

void
InitilizePropertyArray(
    IN PHW_STREAM_REQUEST_BLOCK Srb
	);

VOID
STREAMAPI 
AdapterGetProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    );

VOID
STREAMAPI 
AdapterSetProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    );

NTSTATUS
SonyDCamGetProperty(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension, 
	ULONG ulFieldOffset,
	LONG * plValue, 
	ULONG * pulCapability,	
	ULONG * pulFlags
);

NTSTATUS
SonyDCamSetProperty(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension, 
	ULONG ulFieldOffset,
	ULONG ulFlags,
	LONG  lValue
);

NTSTATUS
SonyDCamGetRange(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension,
	ULONG ulFieldOffset,
	LONG * pMinValue,
	LONG * pMaxValue
);

NTSTATUS
SonyDCamSetAutoMode(
    IN PIRB Irb,
    PSONYDCAM_EXTENSION DeviceExtension, 
	ULONG ulFieldOffset,
	BOOL bAutoMode
);

VOID
SetCurrentDevicePropertyValues(
    PHW_STREAM_REQUEST_BLOCK pSrb,
	PSONYDCAM_EXTENSION DeviceExtension
);