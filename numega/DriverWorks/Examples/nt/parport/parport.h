// parport.h - include file for parallel port driver 
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


typedef struct _CONFIG_DATA 
{
    //
    // This list entry is used to link all of the "valid"
    // configuration entries together.
    //
    LIST_ENTRY m_ListEntry;

    //
    // The suffix to be used in the nt device name space for this
    // port.
    //
    KUnitizedName* DeviceName;

    //
    // The base address of the registry set for this device.
    //
    PHYSICAL_ADDRESS Controller;

    //
    // The number of contiguous bytes take up by the register
    // set for the device.
    //
    ULONG SpanOfController;

    //
    // The bus number (with respect to the bus type) of the bus
    // that this device occupies.
    //
    ULONG BusNumber;

    //
    // Denotes whether this devices physical addresses live in io space
    // or memory space.
    //
    ULONG AddressSpace;

    //
    // The kind of bus that this device lives on (e.g. Isa, Eisa, MCA, etc)
    //
    INTERFACE_TYPE InterfaceType;

    //
    // Registry information on the parallel port interrupt.
    //
    ULONG InterruptLevel;
    ULONG InterruptVector;
    KAFFINITY InterruptAffinity;
    KINTERRUPT_MODE InterruptMode;

    //
    // Denotes whether the device should be disabled after it has been
    // initialized.
    //
    ULONG DisablePort;

} CONFIG_DATA, *PCONFIG_DATA;

typedef KList<CONFIG_DATA> CONFIG_LIST;

typedef struct 
{
	CONFIG_LIST m_List;
	ULONG m_ControllersFound;
} QUERY_LIST;


class ParallelDriver : public KDriver
{
public:
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);

protected:
	VOID GetConfigurationData(CONFIG_LIST& CfgList);

	static NTSTATUS ConfigQueryCallback(
		QUERY_RESULT* bus,
		QUERY_RESULT* controller,
		QUERY_RESULT* peripheral,
		PVOID Context
		);
};






