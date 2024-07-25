// KNdisResource.h: Hardware resource classes
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

#if !defined(AFX_KNDISRESOURCE_H__250916D6_3475_11D3_8FA2_00C04F7445B7__INCLUDED_)
#define AFX_KNDISRESOURCE_H__250916D6_3475_11D3_8FA2_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// About NDIS Miniport resources
//
// The h/w resource include:
//
// typedef enum _CM_RESOURCE_TYPE {
//     CmResourceTypeNull = 0,    // Reserved
//     CmResourceTypePort,
//     CmResourceTypeInterrupt,
//     CmResourceTypeMemory,
//     CmResourceTypeDma,
//     CmResourceTypeDeviceSpecific,
//     CmResourceTypeMaximum
// } CM_RESOURCE_TYPE;
//
// Bus Types:
//
// typedef enum _NDIS_INTERFACE_TYPE
// {
// 	NdisInterfaceInternal = Internal,
// 	NdisInterfaceIsa = Isa,
// 	NdisInterfaceEisa = Eisa,
// 	NdisInterfaceMca = MicroChannel,
// 	NdisInterfaceTurboChannel = TurboChannel,
// 	NdisInterfacePci = PCIBus,
// 	NdisInterfacePcMcia = PCMCIABus
// } NDIS_INTERFACE_TYPE, *PNDIS_INTERFACE_TYPE;
//

/*
NDIS Miniports adapters deal with h/w resources by

(1) querying what resources the system had assigned to the adapter,
(2) registering these resources for the adapter with NDIS

In pre-PNP NDIS, step (1) is implemented differently based on driver/bus type.
In NDIS 5, the PnP manager handles all h/w resources, including ones for
non-PnP devices. Thus, the preferred method for new (NDIS 5) miniports is using
non-bus-specific KNdisPnpResourceRequest, which based on NdisMQueryAdapterResources.

Step (2), however, is implemented uniformly for all drivers using NDIS services,
NdisMRegisterIoPortRange(), NdisMRegisterInterrupt(), NdisMMapIoSpace().

Non-PNP NDIS 4 miniports (such as NE2000 ISA)  simply read the "Parameters"
subkey data (I/O port, IRQ) using NdisReadConfiguration(). Conflicts at
step (2) result in miniport failure => installation problem.

Driver for adapters located at more intelligent buses (PCI, MCA, EISA)
use some sort of enumeration mechanism to find its device on the bus.

PCI miniports use NdisMPciAssignResources() that returns NDIS_RESOURCE_LIST
(which is an alias to NT's CM_RESOURCE_LIST) and NdisReadPciSlotInformation().

EISA-bus miniports use NdisReadEisaSlotInformation[Ex]() that returns
NDIS_EISA_FUNCTION_INFORMATION.

MCA-bus miniports do NdisReadMcaPosInformation() that returns
NDIS_MCA_POS_DATA.

PCMCIA miniports use NdisReadPcmciaAttributeMemory() that returns a BLOB.

DDK also states that "Alternatively, miniport drivers can call
NdisMGetDeviceProperty to retrieve pointers to "raw" or "translated' resources.
Raw resources have not been translated by HAL; translated resources have been."

Usage of the classes below:

  // read PCI res configuration:
  KNdisPciResourceRequest				 Req(this);

  // retrieve attributes of interrupt and port resource
  KNdisResource<CmResourceTypeInterrupt> Int(req);
  KNdisResource<CmResourceTypePort>		 Port(req);

  if (!Int.IsValid())
		FAIL("Can't retrieve interrupt resource");
  ...

  // Use the attributes:

  m_pInterrupt = new KNdisInterrupt(this, Int.Level(), Int.Affinity()...);


//
// This structure defines one type of resource used by a driver.
//

typedef struct _CM_PARTIAL_RESOURCE_DESCRIPTOR {
    UCHAR Type;
    UCHAR ShareDisposition;
    USHORT Flags;
    union {

        //
        // Range of port numbers, inclusive. These are physical, bus
        // relative. The value should be the same as the one passed to
        // HalTranslateBusAddress().
        //

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Port;

        //
        // IRQL and vector. Should be same values as were passed to
        // HalGetInterruptVector().
        //

        struct {
            ULONG Level;
            ULONG Vector;
            ULONG Affinity;
        } Interrupt;

        //
        // Range of memory addresses, inclusive. These are physical, bus
        // relative. The value should be the same as the one passed to
        // HalTranslateBusAddress().
        //

        struct {
            PHYSICAL_ADDRESS Start;    // 64 bit physical addresses.
            ULONG Length;
        } Memory;

        //
        // Physical DMA channel.
        //

        struct {
            ULONG Channel;
            ULONG Port;
            ULONG Reserved1;
        } Dma;

        //
        // Device Specific information defined by the driver.
        // The DataSize field indicates the size of the data in bytes. The
        // data is located immediately after the DeviceSpecificData field in
        // the structure.
        //

        struct {
            ULONG DataSize;
            ULONG Reserved1;
            ULONG Reserved2;
        } DeviceSpecificData;
    } u;
} CM_PARTIAL_RESOURCE_DESCRIPTOR, *PCM_PARTIAL_RESOURCE_DESCRIPTOR;


*/

//////////////////////////////////////////////////////////////////////
// KNdisResourceTraits
//
// Declares the rules of treating resources. This is an internally
// used class each specialization of which defines how to retrieve
// a resource of particular type (ports, interrupts, etc.) using
// a particular resource Query method (PCI, Pnp, etc). KNdisResourceTraits
// defines the requirements to the resources query classes. Each compliant
// class has to provide methods Port(), Interrupt(), Memory(), Dma() and
// IsValid(); e.g. see KNdisPnpResourceRequest. Those methods are declared
// protected since they are not of interest to the user of KNdisResource.
// The user simply (1) instantiates a resource query class and (2) uses
// a refernce to it to instantiate a KNdisResource.
// The resource query classes defined in DriverNetWorks follow the naming
// conventions of KNdisXxxResourceRequest,  where Xxx stands for Pci, Pnp, etc.
//
// The set of both resource types and/or resource request classes
// can be extended by specializing KNdisResourceTraits and providing
// some new KNdisXxxResourceRequest classes compliant to the resource
// request interface.
//
template <CM_RESOURCE_TYPE T>
struct KNdisResourceTraits {
	// Resource query signature
	template <class R>
	static void Query(R& Req, CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
};

template<> struct KNdisResourceTraits<CmResourceTypePort> {
	template <class R>
	static void Query(R& Req, CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0)
		{ Req.Port(desc, Ordinal);}
};
template<> struct KNdisResourceTraits<CmResourceTypeInterrupt> {
	template <class R>
	static void Query(R& Req, CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0)
		{ Req.Interrupt(desc, Ordinal);}
};
template<> struct KNdisResourceTraits<CmResourceTypeMemory> {
	template <class R>
	static void Query(R& Req, CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0)
		{ Req.Memory(desc, Ordinal);}
};
template<> struct KNdisResourceTraits<CmResourceTypeDma> {
	template <class R>
	static void Query(R& Req, CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0)
		{ Req.Dma(desc, Ordinal);}
};

// shorthand for declaring friends:
#define KNDIS_RESOURCE_FRIEND(r)		\
	friend  r<CmResourceTypePort>;		\
	friend  r<CmResourceTypeInterrupt>;	\
	friend  r<CmResourceTypeMemory>;	\
	friend  r<CmResourceTypeDma>;


//////////////////////////////////////////////////////////////////////
// KNdisResource
//
// KNdisResource provides access to attributes of various h/w resource types.
// The type T of resource is the template parameter, which could be one of
// CmResourceTypeXxxx NDIS constants. The constructor for
// KNdisResource does actual query for the resource, while the accessor methods
// retrieve the actual values of the resource's attributes.
//
// Before getting access to a resource's attributes, the driver must specify
// what resource query method will be used. The resource query method depends on
// the NIC's bus type and could be one of:
//
//	KNdisPnpResourceRequest		- preferred method for NDIS 5 drivers (n/a in NDIS 4)
//	KNdisPciResourceRequest		- based on PCI enumeration
//	KNdisParamResourceRequest	- based on custom config data in the Registry
//	KNdisEisaResourceRequest	- based on EISA enumeration
//
// Here's an example of accessing the "interrupt" resource using PCI query:
//
//	KNdisPciResourceRequest	req(*this);	// Declare PCI resource query
//	KNdisResource<CmResourceTypeInterrupt>	 Int(req); // Retrieve  interrupt resource
//	KNdisResource<CmResourceTypePort>		 Port(req);// Retrieve  port resource
//	. . .
//	if (!Int.IsValid() || !Port.IsValid()) // Make sure the resources are available
//		FAIL("Can't retrieve resource");
//	. . .
//	m_pInterrupt = new KNdisInterrupt		// use the attributes for the interrupt object
//		(this, Int.Level(), Int.Affinity());
//
// NOTE: Only accessors relevant to the template type are defined. An attempt
// to use an irrelevant accessor (e.g., an attempt to use 'Vector' with resource type
// "CmResourceTypePort") will result in build error.
//
template<CM_RESOURCE_TYPE T>
class KNdisResource
{
public:
	///////////////////////////////////////////////////////////////////////
	// Constructor
	//
	// Parameters:
	// 	Req                  - Reference to a resource query object, one of KNdisXxxResourceRequest
	// 	Ordinal              - Ordinal number of the resource of given type (default, 0)
	// Returns:
	// 	none
	// Comments:
	// 	The constructor calls an appropriate Query() request on the resource query object Req of type R.
	//  The results of the query are cached can be then accessed using one of the accessors methods.
	template <class R> KNdisResource(R& Req, ULONG Ordinal=0)
	{
		NdisZeroMemory(&m_desc, sizeof(m_desc));
		KNdisResourceTraits<T>::Query(Req, m_desc, Ordinal);
	}

// Accessors

	inline UCHAR Share(void);
	inline BOOLEAN IsValid(void);
	inline UCHAR Type(void);
	inline UCHAR Flags(void);
	inline ULONG Length(void);

	inline PHYSICAL_ADDRESS Start(void);
	inline ULONG_PTR Affinity(void);
	inline ULONG Level(void);
	inline ULONG Vector(void);
	inline ULONG Channel(void);

protected:
//	ULONG							m_uOrd;
	CM_PARTIAL_RESOURCE_DESCRIPTOR	m_desc;
};



//////////////////////////////////////////////////////////////////////
// KNdisPciResourceRequest
//
// PCI-bus specific resource query. Used for PCI NICs. NDIS 5 miniports
// are recommended to use KNdisPnpResource request.
//
class KNdisPciResourceRequest  {
public:
	KNdisPciResourceRequest(NDIS_HANDLE hAdapter, ULONG SlotNumber=0);

	// Tests the success of construction
	 BOOLEAN IsValid() { return m_pResList!=NULL; }

protected:
	KNDIS_RESOURCE_FRIEND(KNdisResourceTraits);

	void Port(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Memory(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Interrupt(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Dma(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);

	PNDIS_RESOURCE_LIST m_pResList;
	CM_PARTIAL_RESOURCE_DESCRIPTOR* Find(CM_RESOURCE_TYPE Type, ULONG Ordinal);
	PREVENT_COPY_CONSTRUCTORS(KNdisPciResourceRequest);
};

//////////////////////////////////////////////////////////////////////
// KNdisPnpResourceRequest
//
// Retrieves h/w resources assigned to the adapter by the Pnp Manager.
// This is the recommended method for all NDIS 5 miniports.
//
class KNdisPnpResourceRequest  {
public:
	KNdisPnpResourceRequest(KNdisConfig& Config);
	~KNdisPnpResourceRequest();

	// Tests the success of construction
	BOOLEAN IsValid() { return m_pResList!=NULL; }

protected:
	KNDIS_RESOURCE_FRIEND(KNdisResourceTraits);

	void Port(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Memory(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Interrupt(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Dma(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);

	PNDIS_RESOURCE_LIST m_pResList;
	CM_PARTIAL_RESOURCE_DESCRIPTOR* Find(CM_RESOURCE_TYPE Type, ULONG Ordinal);
	PREVENT_COPY_CONSTRUCTORS(KNdisPnpResourceRequest);
};


//////////////////////////////////////////////////////////////////////
// KNdisParamResourceRequest
//
// The resource query class that retrieves h/w resource assignments from the
// "Parameters" section of the registry. This class is used for "dumb"
// buses (e.g. NdisInterfaceIsa), where the resources have to be
// assigned parametrically, in the Registry.
//
// DriverNetworks uses the following predefined key strings for such parameters
// (to be placed in the "Parameter" key of the driver's registry node):
//
//	"IoBasePortAddress"		- base I/O port address in hex
//	"IoBaseMemoryAddress"	- base (shared) memory address in hex
//	"InterruptNumber"		- IRQ number in hex
//	"DmaChannel"			- DMA channel
//	"DmaPort"				- DMA port
//
// Note that NDIS 5 miniports are recommended to use KNdisPnpResourceRequest
// even for the "dumb" cards. The resource assignments are handled by the PnP
// manager.
//
// Limitation: A single resource object of each type is supported
//			   per request, i.e. the 'Ordinal' is ignored.
//
class KNdisParamResourceRequest  {
public:

	///////////////////////////////////////////////////////////////////////
	// Constructor
	//
	// Parameters:
	// 	Config               - NDIS Configuration Object (reference) passed into adapter's Initialize() handler
	// Returns:
	// 	none
	// Comments:
	// 	Always use IsValid() to check the result of the construction
	KNdisParamResourceRequest(KNdisConfig& Config) : m_Config(Config) {};

	// Tests the success of construction
	BOOLEAN IsValid() { return m_Config.IsValid(); }

protected:
	KNDIS_RESOURCE_FRIEND(KNdisResourceTraits);

	void Port(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Memory(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Interrupt(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Dma(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);

	KNdisConfig& m_Config;
	PREVENT_COPY_CONSTRUCTORS(KNdisParamResourceRequest);
};

//////////////////////////////////////////////////////////////////////
// KNdisEisaResourceRequest (TODO)
//
// EISA-specific resource query class. NDIS 5 miniports
// are recommended to use KNdisPnpResource request.
//
class KNdisEisaResourceRequest  {
public:
	KNdisEisaResourceRequest(KNdisConfig& Config, ULONG Slot=0);
	~KNdisEisaResourceRequest();

	// Tests the success of construction
	BOOLEAN IsValid() { return m_Config.IsValid(); }

protected:
	KNDIS_RESOURCE_FRIEND(KNdisResourceTraits);

	void Port(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Memory(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Interrupt(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);
	void Dma(OUT CM_PARTIAL_RESOURCE_DESCRIPTOR& desc, ULONG Ordinal=0);

	KNdisConfig& m_Config;

	// Eisa info
	PNDIS_EISA_FUNCTION_INFORMATION	m_pEisa;

	PREVENT_COPY_CONSTRUCTORS(KNdisEisaResourceRequest);
};


/////////////////////////////////////////////////////////////////////
// Implementations: KNdisResource
/////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
// Get share attribute accessor
//
// Parameters:
// 	void                 -
// Returns:
// 	none
// Comments:
// 	none
//
template<CM_RESOURCE_TYPE T>
UCHAR KNdisResource<T>::Share(void)
{
	return m_desc.ShareDisposition;
}

///////////////////////////////////////////////////////////////////////
// Test if the resource was successfully retrieved
//
// Parameters:
// 	none
// Returns:
// 	TRUE on success
// Comments:
// 	IsValid() should be called immediatelly after construction to make
//	sure the resource is available. The accessors results are undefined
//  is IsValid() is FALSE.
template<CM_RESOURCE_TYPE T>
BOOLEAN KNdisResource<T>::IsValid()
{
	// Happy only when contains the type of the class
	return Type() == UCHAR(T);
}

///////////////////////////////////////////////////////////////////////
//  Get type attribute accessor
//
// Parameters:
// 	none
// Returns:
// 	Resource type as uchar value (CmResourceTypeXxx enum)
// Comments:
// 	none
template<CM_RESOURCE_TYPE T>
UCHAR KNdisResource<T>::Type()
{
	return m_desc.Type;
}

///////////////////////////////////////////////////////////////////////
// Get flag attribute accessor
//
// Parameters:
// 	none
// Returns:
// 	Flags
// Comments:
// 	none
template<CM_RESOURCE_TYPE T>
UCHAR KNdisResource<T>::Flags()
{
	return m_desc.Flags;
}

// accessors: define only those relevant for the type

///////////////////////////////////////////////////////////////////////
// Get I/O range length attribute value
//
// Parameters:
// 	none
// Returns:
// 	Length of the i/o port range in bytes
// Comments:
// 	none
ULONG KNdisResource<CmResourceTypePort>::Length()
{
	return m_desc.u.Port.Length;
}

///////////////////////////////////////////////////////////////////////
// Get memory range length attribute value
//
// Parameters:
// 	none
// Returns:
// 	Length of the memory range in bytes
// Comments:
// 	none
ULONG KNdisResource<CmResourceTypeMemory>::Length()
{
	return m_desc.u.Memory.Length;
}


///////////////////////////////////////////////////////////////////////
// Get start physical address attribute
//
// Parameters:
// 	none
// Returns:
// 	The first physical address of the port range
// Comments:
// 	none
PHYSICAL_ADDRESS KNdisResource<CmResourceTypePort>::Start()
{
	return m_desc.u.Port.Start;
}

///////////////////////////////////////////////////////////////////////
// Get start physical address attribute
//
// Parameters:
// 	none
// Returns:
// 	The first physical address of the memory range
// Comments:
// 	none
PHYSICAL_ADDRESS KNdisResource<CmResourceTypeMemory>::Start()
{
	return m_desc.u.Port.Start;
}


///////////////////////////////////////////////////////////////////////
// Get Interrupt affinity attribute value
//
// Parameters:
// 	none
// Returns:
// 	Affinity for given interrupt resource
// Comments:
// 	none
ULONG_PTR KNdisResource<CmResourceTypeInterrupt>::Affinity()
{
	return m_desc.u.Interrupt.Affinity;
}


///////////////////////////////////////////////////////////////////////
// Get Interrupt level attribute
//
// Parameters:
// 	none
// Returns:
// 	DIRQL
// Comments:
// 	none
ULONG KNdisResource<CmResourceTypeInterrupt>::Level()
{
	return m_desc.u.Interrupt.Level;
}


///////////////////////////////////////////////////////////////////////
// Get Interrupt vector attribute
//
// Parameters:
// 	none
// Returns:
// 	Vector value
// Comments:
// 	none
ULONG KNdisResource<CmResourceTypeInterrupt>::Vector()
{
	return m_desc.u.Interrupt.Vector;
}


///////////////////////////////////////////////////////////////////////
// Get Dma channel attribute
//
// Parameters:
// 	none
// Returns:
// 	Channel number
// Comments:
// 	none
ULONG KNdisResource<CmResourceTypeDma>::Channel()
{
	return m_desc.u.Dma.Channel;
}

#endif // !defined(AFX_KNDISRESOURCE_H__250916D6_3475_11D3_8FA2_00C04F7445B7__INCLUDED_)
