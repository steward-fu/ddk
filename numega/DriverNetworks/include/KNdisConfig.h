// KNdisConfig.h: interface for the KNdisConfig class.
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

#if !defined(AFX_KNDISCONFIG_H__CCD29DA6_2F19_11D3_8F9F_00C04F7445B7__INCLUDED_)
#define AFX_KNDISCONFIG_H__CCD29DA6_2F19_11D3_8F9F_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>

//////////////////////////////////////////////////////////////////////
// KNdisConfig
//
// Provides access to the NDIS driver's portion of the Registry.
//
// Allows to read and write values from/to the Registry, usually
// during driver initialization. The class provides a number of
// overloaded Read/Write methods to read and write 32-bit values
// (ULONGs) and text strings (UNICODE strings). The registry key
// names are UNICODE strings, which can be conveniently specified
// in the Read/Write methods using the KNDIS_CONST_STRING macro.
//
// KNdisConfig also provides read access to some predefined
// NDIS values, such as Network Address, NDIS version, etc.
//
// In addition to that, for NDIS miniport drivers, KNdisConfig provides
// low-level "immediate" i/o methods (inp/out). These methods could
// be used by NIC miniport drivers to access a configuration EEPROM on the
// card before the driver claims the h/w resources during initialization.
//
// DriverNetworks framework usually instantiates an instance of KNdisConfig during
// driver initialization and passes a reference to it to miniport (intermediate)
// driver's Initialize() handler (see KNdisMiniAdapter and KNdisFilterAdapter).
//
class KNdisConfig
{
	SAFE_DESTRUCTORS;
public:
// ctors
// used by miniports:
	inline KNdisConfig(NDIS_HANDLE WrapperConfigurationContext);

// used by protocols:
	inline KNdisConfig(PNDIS_STRING ProtocolSection);

// used to browse subkeys:
	inline KNdisConfig(KNdisConfig& Config, IN PNDIS_STRING SubKeyName);
	inline KNdisConfig(KNdisConfig& Config, IN ULONG Index=0, OUT PNDIS_STRING SubKeyName=NULL);

// dtor
	inline ~KNdisConfig();

// Check the open status
	inline BOOLEAN IsValid() const;

// Cast to configuration handle (conversion)
	inline operator NDIS_HANDLE() const { return m_hConfig; }
// Cast to wrapper context handle (conversion)
	inline NDIS_HANDLE WrapperContext() const { return m_hWrapperContext; }

// Accessing the Registry

	// Reading Registry keys
	inline NDIS_STATUS	Read(IN PNDIS_STRING Keyword, OUT PULONG pValue,
		NDIS_PARAMETER_TYPE Type=NdisParameterInteger);
	inline NDIS_STATUS	Read(IN PNDIS_STRING Keyword, OUT PULONG pValue,
		IN ULONG const uMin, IN ULONG const uMax,
		NDIS_PARAMETER_TYPE Type=NdisParameterInteger);
	inline ULONG Read(IN PNDIS_STRING Keyword, IN ULONG DefaultValue,
		NDIS_PARAMETER_TYPE Type=NdisParameterInteger);
	inline ULONG Read(IN PNDIS_STRING Keyword, IN ULONG DefaultValue,
		IN ULONG const uMin, IN ULONG const uMax,
		NDIS_PARAMETER_TYPE Type=NdisParameterInteger);

	inline NDIS_STATUS	Read(IN PNDIS_STRING Keyword, OUT PNDIS_STRING pValue,
		NDIS_PARAMETER_TYPE Type=NdisParameterString);

// Special values
	inline NDIS_STATUS	ReadNetworkAddress(OUT ETHERNET_ADDRESS& Addr);
	inline BOOLEAN IsNT();
	inline ULONG NdisVersion();

	// Writing Registry keys
	inline NDIS_STATUS	Write(IN PNDIS_STRING Keyword, ULONG Value,
		NDIS_PARAMETER_TYPE Type=NdisParameterInteger);
	inline NDIS_STATUS	Write(IN PNDIS_STRING Keyword, PNDIS_STRING pValue,
		NDIS_PARAMETER_TYPE Type=NdisParameterString);

#if KNDIS_DDK_VERSION <= 5
	// Immediate i/o (implement explicitely for each NdisImmediateXXXYYY().
	// NOTE: n/a in NDIS 5.1+
	inline UCHAR inb(ULONG Port);
	inline VOID	outb(ULONG Port, UCHAR data);

	inline USHORT inw(ULONG Port);
	inline VOID	outw(ULONG Port, USHORT data);

	inline ULONG ind(ULONG Port);
	inline VOID	outd(ULONG Port, ULONG data);

	inline VOID inm(ULONG  SharedMemoryAddress, PUCHAR  Buffer, ULONG Length);
	inline VOID outm(ULONG  SharedMemoryAddress, PUCHAR  Buffer, ULONG Length);
#endif

protected:
	NDIS_HANDLE		m_hConfig;
	NDIS_HANDLE		m_hWrapperContext; // cache it as some APIs want it

};


//////////////////////////////////////////////////////////////////////
// Implementations
//////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
// Constructor (NDIS miniports, IM)
//
// Parameters:
// 	WrapperConfigurationContext - the handle input to MiniportInitialize
// Returns:
// 	none
// Comments:
// 	DriverNetworks framework constructs an instance of KNdisConfig
//	automatically and passes a reference to it to adapter's initialization
//  method.
//
//	This form of constructor is used by MP and IM drivers. Protocol drivers
//	use the 2nd form.
KNdisConfig::KNdisConfig(NDIS_HANDLE WrapperConfigurationContext)
	: m_hConfig(NULL), m_hWrapperContext(WrapperConfigurationContext)
{
	NDIS_STATUS Status;
    NdisOpenConfiguration(&Status, &m_hConfig,
		WrapperConfigurationContext);
	ASSERT(Status==NDIS_STATUS_SUCCESS);
}



///////////////////////////////////////////////////////////////////////
// Constructor (NDIS protocol, IM drivers)
//
// Parameters:
// 	ProtocolSection      - the adapter, for which the per-adapter parameters
//						   key in the registry protocol section should be opened
// Returns:
// 	none
// Comments:
// 	ProtocolSection is the SystemSpecific1 input parameter to the caller's
//  ProtocolBindAdapter function. DriverNetworks's filter framework automatically
//	instantiates an instance of this class and passes it to KNdisFilterAdapter-derived
//	classes.
KNdisConfig::KNdisConfig(PNDIS_STRING ProtocolSection)
	: m_hConfig(NULL)
{
	NDIS_STATUS Status;
	NdisOpenProtocolConfiguration(&Status, &m_hConfig, ProtocolSection);
	ASSERT(Status==NDIS_STATUS_SUCCESS);
}


///////////////////////////////////////////////////////////////////////
// Constructor for a subkey object by its name (NDIS 5 only)
//
// Parameters:
// 	Config               - the root KNdisConfig object for which a subkey
//	                       object is to be created
// 	SubKeyName           - the subkey string name
// Returns:
// 	none
// Comments:
// 	Always use IsValid() to check the successful creation of the
//	subkey object. The IsValid() returns FALSE if the subkey does not exist.
KNdisConfig::KNdisConfig(KNdisConfig& Config, IN PNDIS_STRING SubKeyName)
	: m_hConfig(NULL)
{
#if (KNDIS_DDK_VERSION >= 5)
	NDIS_STATUS Status;
	NdisOpenConfigurationKeyByName(
		&Status, Config.m_hConfig, SubKeyName, &m_hConfig);
#else
	ASSERT(!"Supported in NDIS 4");
	UNREFERENCED_PARAMETERS2(Config, SubKeyName);
#endif
}

///////////////////////////////////////////////////////////////////////
// Constructor for a subkey object by its index (NDIS 5 only)
//
// Parameters:
// 	Config               - the root KNdisConfig object for which a subkey
//	                       object is to be created
// 	Index                - Zero-based index of the subkey (0 by default)
// 	SubKeyName           - Pointer to string to receive the name of the opened subkey (NULL default)
// Returns:
// 	none
// Comments:
// 	The caller does not have to know the name of the subkey. This way, one can
//	enumerate all the configuration subkeys for the given KNdisConfig object.
// 	Always use IsValid() to check the successful creation of the
//	subkey object. The IsValid() returns FALSE if no subkey with given index exists.
KNdisConfig::KNdisConfig(KNdisConfig& Config, IN ULONG Index, OUT PNDIS_STRING SubKeyName)
	: m_hConfig(NULL)
{
#if (KNDIS_DDK_VERSION >= 5)
	NDIS_STATUS Status;
	PNDIS_STRING subkeyname;
	NdisOpenConfigurationKeyByIndex(
		&Status, Config.m_hConfig, Index, SubKeyName ? SubKeyName : subkeyname, &m_hConfig);
#else
	ASSERT(!"Supported in NDIS 4");
	UNREFERENCED_PARAMETERS3(Config, Index, SubKeyName);
#endif
}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisConfig::~KNdisConfig()
{
	if (m_hConfig)
		NdisCloseConfiguration(m_hConfig);
}


///////////////////////////////////////////////////////////////////////
// Test the status of the object
//
// Parameters:
// 	none
// Returns:
// 	TRUE if the configuration object has been successfully created
// Comments:
// 	none
BOOLEAN KNdisConfig::IsValid() const
{
	return m_hConfig!=NULL;
}


///////////////////////////////////////////////////////////////////////
// Read ULONG value
//
// Parameters:
// 	Keyword              - entry name in the Registry
// 	pValue               - pointer to ULONG to receive the value read
// 	Type                 - format: NdisParameterInteger (default) or NdisParameterHexInteger
// Returns:
// 	NDIS_STATUS_SUCCESS if the value was read, an error code otherwise
// Comments:
// 	none
NDIS_STATUS	KNdisConfig::Read
(IN PNDIS_STRING Keyword, OUT PULONG pValue, NDIS_PARAMETER_TYPE Type)
{
	NDIS_STATUS Status;
	PNDIS_CONFIGURATION_PARAMETER KeywordValue;

	NdisReadConfiguration(
		&Status, &KeywordValue, m_hConfig, Keyword, Type);
	if (Status == NDIS_STATUS_SUCCESS)
		*pValue = KeywordValue->ParameterData.IntegerData;
	return Status;
}


///////////////////////////////////////////////////////////////////////
// Read ULONG value with constraints
//
// Parameters:
// 	Keyword              - entry name in the Registry
// 	pValue               - pointer to ULONG to receive the value read
// 	uMin                 - minimum value accepted
// 	uMax                 - minimum value accepted
// 	Type                 - format: NdisParameterInteger (default) or NdisParameterHexInteger
// Returns:
// 	NDIS_STATUS_SUCCESS if the value was read, an error code otherwise.
//  A special error code, NDIS_STATUS_INVALID_DATA, is returned when the entry
//	is present in the Registry, but the value is out of[uMin, uMax] range specified.
// Comments:
// 	none
NDIS_STATUS	KNdisConfig::Read
(IN PNDIS_STRING Keyword, OUT PULONG pValue, IN ULONG const uMin, IN ULONG const uMax, NDIS_PARAMETER_TYPE Type)
{
	ULONG u;
	NDIS_STATUS Status = Read(Keyword, &u, Type);
	if (Status == NDIS_STATUS_SUCCESS) {
		if (u < uMin || u > uMax)
			return NDIS_STATUS_INVALID_DATA; // violation of boundaries
		else
			*pValue = u;
	}
	return Status;
}


///////////////////////////////////////////////////////////////////////
// Read ULONG value with default
//
// Parameters:
// 	Keyword              - entry name in the Registry
// 	DefaultValue         - default value; used when the keyword is absent
// 	Type                 - format: NdisParameterInteger (default) or NdisParameterHexInteger
// Returns:
// 	Value read or DefaultValue
// Comments:
// 	none
ULONG KNdisConfig::Read
(IN PNDIS_STRING Keyword, IN ULONG DefaultValue, NDIS_PARAMETER_TYPE Type)
{
	ULONG val = DefaultValue;
	Read(Keyword, &val, Type);
	return val;
}


///////////////////////////////////////////////////////////////////////
// Read ULONG value with constraints and default
//
// Parameters:
// 	Keyword              - entry name in the Registry
// 	DefaultValue         - default value; used when the keyword is absent or its value is out of range
// 	uMin                 - minimum value accepted
// 	uMax                 - minimum value accepted
// 	Type                 - format: NdisParameterInteger (default) or NdisParameterHexInteger
// Returns:
// 	Value read or DefaultValue
// Comments:
// 	none
ULONG
KNdisConfig::Read
(IN PNDIS_STRING Keyword, IN ULONG DefaultValue, IN ULONG const uMin, IN ULONG const uMax, NDIS_PARAMETER_TYPE Type)
{
	ULONG val;
	return (Read(Keyword, &val, uMin, uMax, Type) == NDIS_STATUS_SUCCESS) ?
		val : DefaultValue;
}



///////////////////////////////////////////////////////////////////////
// Read a string value
//
// Parameters:
// 	Keyword              - entry name in the Registry
// 	pValue               - pointer to PNDIS_STRING to receive the string value
// 	Type                 - format: NdisParameterString (default) or NdisParameterMultiString
// Returns:
// 	NDIS_STATUS_SUCCESS if the value was read, an error code otherwise.
//  A special error code, NDIS_STATUS_INVALID_DATA, is returned when the entry
// Comments:
//	none
NDIS_STATUS
KNdisConfig::Read(IN PNDIS_STRING Keyword, OUT PNDIS_STRING pValue, NDIS_PARAMETER_TYPE Type)
{
	NDIS_STATUS Status;
	PNDIS_CONFIGURATION_PARAMETER KeywordValue;

	NdisReadConfiguration(
		&Status, &KeywordValue, m_hConfig, Keyword, Type);
	if (Status == NDIS_STATUS_SUCCESS)
		*pValue = KeywordValue->ParameterData.StringData;
	return Status;
}


///////////////////////////////////////////////////////////////////////
// Write a ULONG value
//
// Parameters:
// 	Keyword              - entry name in the Registry
// 	Value                - value to write
// 	Type                 - format: NdisParameterInteger (default) or NdisParameterHexInteger
// Returns:
// 	NDIS_STATUS_SUCCESS if the value was written, an error code otherwise
// Comments:
// 	none
NDIS_STATUS	KNdisConfig::Write
(IN PNDIS_STRING Keyword, ULONG Value, NDIS_PARAMETER_TYPE Type)
{
#if !defined(VTOOLSD)
	NDIS_STATUS Status;
    NDIS_CONFIGURATION_PARAMETER ConfigParam;

    ConfigParam.ParameterType = Type;
    ConfigParam.ParameterData.IntegerData = Value;

    NdisWriteConfiguration(
		&Status, m_hConfig, Keyword, &ConfigParam );
	return Status;
#else
	UNREFERENCED_PARAMETERS3(Keyword, Value, Type);
	ASSERT(!"Not supported in W9X");
	return NDIS_STATUS_FAILURE;
#endif
}

///////////////////////////////////////////////////////////////////////
// Write a string
//
// Parameters:
// 	Keyword              - entry name in the Registry
// 	pValue               - string to write
// 	Type                 - format: NdisParameterString (default) or NdisParameterMultiString
// Returns:
// 	NDIS_STATUS_SUCCESS if the str was written, an error code otherwise
// Comments:
// 	none
NDIS_STATUS KNdisConfig::Write
(IN PNDIS_STRING Keyword, PNDIS_STRING pValue, NDIS_PARAMETER_TYPE Type)
{
#if !defined(VTOOLSD)
	NDIS_STATUS Status;
    NDIS_CONFIGURATION_PARAMETER ConfigParam;

    ConfigParam.ParameterType = Type;
    ConfigParam.ParameterData.StringData = *pValue;

    NdisWriteConfiguration(
		&Status, m_hConfig, Keyword, &ConfigParam );
	return Status;
#else
	UNREFERENCED_PARAMETERS2(Keyword, pValue);
	ASSERT(!"Not supported in W9X");
	return NDIS_STATUS_FAILURE;
#endif
}


///////////////////////////////////////////////////////////////////////
// Read Network Address (MAC)
//
// Parameters:
// 	Addr                 - pointer to an ETHERNET_ADDRESS structure to receive the value
// Returns:
// 	NDIS_STATUS_SUCCESS if the address was read, an error code otherwise
// Comments:
// 	NDIS uses a predefined keyword to store the network address value (if any)
//	for NICs with software-settable MAC address.
//	Note that this method simply returns a 6-byte binary value stored in the Registry.
//	It has no relevance to the physical MAC address "burned" on the card.
NDIS_STATUS
KNdisConfig::ReadNetworkAddress(OUT ETHERNET_ADDRESS& Addr)
{
	NDIS_STATUS Status;
	PETHERNET_ADDRESS pa;	// system-buffered area for the address
	UINT len;
	NdisReadNetworkAddress(&Status, (void**)&pa, &len, m_hConfig);
	if (Status == NDIS_STATUS_SUCCESS && len==sizeof(ETHERNET_ADDRESS))
		Addr = *pa;
	else return NDIS_STATUS_FAILURE;
	return Status;
}


///////////////////////////////////////////////////////////////////////
// Query operating system type
//
// Parameters:
// 	none
// Returns:
// 	TRUE if running under NT or W2K; otherwise Win9X.
// Comments:
// 	none
BOOLEAN KNdisConfig::IsNT()
{
#if !defined (VTOOLSD)
	NDIS_ENVIRONMENT_TYPE t=NdisEnvironmentWindowsNt;
	Read(KNDIS_STRING_CONST("Environment"), (PULONG)&t);
	return t==NdisEnvironmentWindowsNt;
#else
	return TRUE;
#endif
}


///////////////////////////////////////////////////////////////////////
// Query NDIS version the driver has been registered for
//
// Parameters:
// 	none
// Returns:
// 	A value in the form 0xMMMMmmmm, where MMMM is the major version and
//	mmmm the minor version number, or 0 if an error occured.
// Comments:
//	For example, 0x00050000 indicates the the driver is connection-oriented
//  (current version of NDIS library), while 0x00040000 indicates the driver
//  uses underlying connectionless media.
ULONG KNdisConfig::NdisVersion()
{
#if !defined(VTOOLSD)
	return Read(KNDIS_STRING_CONST("NdisVersion"), ULONG(0));
#else
	return 0x00040000; // BUGBUG: Fix it later
#endif
}

// immediate i/o register access

#ifdef VTOOLSD
#pragma warning (disable:4244)		// shut off the conversion warning on DDK98 ndis.h
#endif

#if KNDIS_DDK_VERSION <= 5			// Intermediate i/o is n/a in NDIS 5.1+

///////////////////////////////////////////////////////////////////////
// Read a byte from a bus-relative I/O port  (immediate)
//
// Parameters:
// 	Port               - the bus-relative I/O port address
// Returns:
// 	Byte read from the port
// Comments:
//	Windows 2000 DDK states that miniport drivers should not call the
//  "immediate" i/o functions "in a Plug and Play environment.
//	Future releases might not support" these functions.
UCHAR KNdisConfig::inb(ULONG Port)
{
	UCHAR c;
	NdisImmediateReadPortUchar(m_hConfig, Port, &c);
	return c;
}

///////////////////////////////////////////////////////////////////////
// Write a byte to a bus-relative I/O port  (immediate)
//
// Parameters:
// 	Port               - the bus-relative I/O port address
//	Data               - the byte to write
// Returns:
// 	none
// Comments:
//	Windows 2000 DDK states that miniport drivers should not call the
//  "immediate" i/o functions "in a Plug and Play environment.
//	Future releases might not support" these functions.
 VOID KNdisConfig::outb(ULONG Port, UCHAR data)
{
	NdisImmediateWritePortUchar(m_hConfig, Port, data);
}


///////////////////////////////////////////////////////////////////////
// Read a word from a bus-relative I/O port  (immediate)
//
// Parameters:
// 	Port               - the bus-relative I/O port address
// Returns:
// 	Word read from the port
// Comments:
//	Windows 2000 DDK states that miniport drivers should not call the
//  "immediate" i/o functions "in a Plug and Play environment.
//	Future releases might not support" these functions.
USHORT KNdisConfig::inw(ULONG Port)
{
	USHORT c;
	NdisImmediateReadPortUshort(m_hConfig, Port, &c);
	return c;
}

///////////////////////////////////////////////////////////////////////
// Write a word to a bus-relative I/O port  (immediate)
//
// Parameters:
// 	Port               - the bus-relative I/O port address
//	Data               - the word to write
// Returns:
// 	none
// Comments:
//	Windows 2000 DDK states that miniport drivers should not call the
//  "immediate" i/o functions "in a Plug and Play environment.
//	Future releases might not support" these functions.
 VOID KNdisConfig::outw(ULONG Port, USHORT data)
{
	NdisImmediateWritePortUshort(m_hConfig, Port, data);
}


///////////////////////////////////////////////////////////////////////
// Read a dword from a bus-relative I/O port  (immediate)
//
// Parameters:
// 	Port               - the bus-relative I/O port address
// Returns:
// 	DWORD read from the port
// Comments:
//	Windows 2000 DDK states that miniport drivers should not call the
//  "immediate" i/o functions "in a Plug and Play environment.
//	Future releases might not support" these functions.
 ULONG KNdisConfig::ind(ULONG Port)
{
	ULONG c;
	NdisImmediateReadPortUlong(m_hConfig, Port, &c);
	return c;
}


///////////////////////////////////////////////////////////////////////
// Write a dword to a bus-relative I/O port  (immediate)
//
// Parameters:
// 	Port               - the bus-relative I/O port address
//	Data               - the dword to write
// Returns:
// 	none
// Comments:
//	Windows 2000 DDK states that miniport drivers should not call the
//  "immediate" i/o functions "in a Plug and Play environment.
//	Future releases might not support" these functions.
 VOID KNdisConfig::outd(ULONG Port, ULONG data)
{
	NdisImmediateWritePortUlong(m_hConfig, Port, data);
}

#if defined (NDIS_MINIPORT_DRIVER) // those 2 are not avail in VtoolsD either
///////////////////////////////////////////////////////////////////////
// Read Shared Memory (immediate)
//
// Parameters:
// 	SharedMemoryAddress  - shared memory address to read from
// 	Buffer               - buffer to read into
// 	Length               - number of bytes to read
// Returns:
// 	none
// Comments:
//	Windows 2000 DDK states that miniport drivers should not call the
//  "immediate" i/o functions "in a Plug and Play environment.
//	Future releases might not support" these functions.
VOID KNdisConfig::inm(ULONG  SharedMemoryAddress, PUCHAR  Buffer, ULONG Length)
{
	NdisImmediateReadSharedMemory(m_hConfig,
		SharedMemoryAddress, Buffer, Length);
}
///////////////////////////////////////////////////////////////////////
// Write Shared Memory (immediate)
//
// Parameters:
// 	SharedMemoryAddress  - shared memory address to write to
// 	Buffer               - buffer to write from
// 	Length               - number of bytes to write
// Returns:
// 	none
// Comments:
//	Windows 2000 DDK states that miniport drivers should not call the
//  "immediate" i/o functions "in a Plug and Play environment.
//	Future releases might not support" these functions.
VOID KNdisConfig::outm(ULONG  SharedMemoryAddress, PUCHAR  Buffer, ULONG Length)
{
	NdisImmediateWriteSharedMemory(m_hConfig,
		SharedMemoryAddress, Buffer, Length);
}
#endif // NDIS_MINIPORT_DRIVER
#endif // KNDIS_DDK_VERSION <= 5

#ifdef VTOOLSD
#pragma warning (default:4244)
#endif


// TODO

//////////////////////////////////////////////////////////////////////
// NdisReadEisaSlotInformation

//////////////////////////////////////////////////////////////////////
// NdisReadMcaPosInformation

#endif // !defined(AFX_KNDISCONFIG_H__CCD29DA6_2F19_11D3_8F9F_00C04F7445B7__INCLUDED_)
