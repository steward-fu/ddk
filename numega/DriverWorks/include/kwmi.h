// kwmi.h - definitions for Windows Management Instrumentation support
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

#if _WDM_	// not supported for non-WDM drivers

#if !defined(__KWMI__)
#define __KWMI__

extern "C" {

#include <wmistr.h>

typedef NTSTATUS (*PWMI_QUERY_REGINFO) (
	PDEVICE_OBJECT DeviceObject,
	PULONG RegFlags,
	PUNICODE_STRING InstanceName,
	PUNICODE_STRING *RegistryPath,
	PUNICODE_STRING MofResourceName,
	PDEVICE_OBJECT *Pdo
	);

typedef NTSTATUS (*PWMI_QUERY_DATABLOCK) (
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp,
	ULONG GuidIndex,
	ULONG InstanceIndex,
	ULONG InstanceCount,
	OUT PULONG InstanceLengthArray,
	ULONG BufferAvail,
	PUCHAR Buffer
	);

typedef NTSTATUS (*PWMI_SET_DATABLOCK) (
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp,
	ULONG GuidIndex,
	ULONG InstanceIndex,
	ULONG BufferSize,
	PUCHAR Buffer
	);

typedef NTSTATUS (*PWMI_SET_DATAITEM) (
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp,
	ULONG GuidIndex,
	ULONG InstanceIndex,
	ULONG DataItemId,
	ULONG BufferSize,
	PUCHAR Buffer
	);

typedef NTSTATUS (*PWMI_EXECUTE_METHOD) (
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp,
	ULONG GuidIndex,
	ULONG InstanceIndex,
	ULONG MethodId,
	ULONG InBufferSize,
	ULONG OutBufferSize,
	OUT PUCHAR Buffer
	);

typedef struct
{
	LPCGUID Guid;
	ULONG InstanceCount;
	ULONG Flags;
} WMIGUIDREGINFO, *PWMIGUIDREGINFO;

typedef enum
{
	WmiEventControl,
	WmiDataBlockControl
} WMIENABLEDISABLECONTROL;

typedef NTSTATUS (*PWMI_FUNCTION_CONTROL) (
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp,
	ULONG GuidIndex,
	WMIENABLEDISABLECONTROL Function,
	BOOLEAN Enable
	);

typedef struct _WMILIB_CONTEXT
{
    //
    // WMI data block guid registration info
	ULONG GuidCount;
	PWMIGUIDREGINFO GuidList;

	//
	// WMI functionality callbacks
	PWMI_QUERY_REGINFO       QueryWmiRegInfo;
	PWMI_QUERY_DATABLOCK     QueryWmiDataBlock;
	PWMI_SET_DATABLOCK       SetWmiDataBlock;
	PWMI_SET_DATAITEM        SetWmiDataItem;
	PWMI_EXECUTE_METHOD      ExecuteWmiMethod;
	PWMI_FUNCTION_CONTROL    WmiFunctionControl;
} WMILIB_CONTEXT, *PWMILIB_CONTEXT;



typedef enum
{
	IrpProcessed,    // Irp processed, and possibly completed
	IrpNotCompleted, // Irp processed, but NOT completed
	IrpNotWmi,       // Irp is not a WMI IRP
	IrpForward       // Irp is a WMI IRP, but not targeted at this device
} SYSCTL_IRP_DISPOSITION, *PSYSCTL_IRP_DISPOSITION;


NTSTATUS WmiSystemControl(
	PWMILIB_CONTEXT WmiLibInfo,
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp,
	PSYSCTL_IRP_DISPOSITION IrpDisposition
    );

NTSTATUS WmiCompleteRequest(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp,
	NTSTATUS Status,
	ULONG BufferUsed,
	CCHAR PriorityBoost
    );

}; // end extern "C"


template <class T> class KWmiBlock;			// forward
template <class T> class KWmiDispatcher;	// forward

//////////////////////////////////////////////////////////////////////////////
// Each device that is a WMI provider embeds an instance of class KWmiContext.
//
//  For each block type (identified by a GUID) that a device provides, there is a
//  KWmiDispatcher object associated with the KWmiContext object. Each
//  KWmiDispatcher object may be associated with multiple instances of classes
//  derived from KWmiBlock.

template <class F = KStdWmiInterface>
class KWmiContext : public F
{
public:
	SAFE_DESTRUCTORS

// constructor
	KWmiContext(void);

// destructor
	~KWmiContext(void);

// Add a dispatcher object to the list of dispatchers associated with this context

	NTSTATUS AddDispatcher(
		const GUID* pGuid,
		ULONG Flags,
		PULONG pGuidIndex,
		KWmiDispatcher<F>*& pDispatcher
		);

// Remove a dispatcher
	NTSTATUS RemoveDispatcher(KWmiDispatcher<F>* dispatcher);

// Add a block object to the dispatcher associated with the given GUID
	NTSTATUS AddBlock(
		KWmiBlock<F>* pBlock,
		const GUID* pGuid,
		ULONG Flags,
		PULONG pGuidIndex,
		PULONG pBlockIndex
		);

// Remove a block from the list for this context
	NTSTATUS RemoveBlock(KWmiBlock<F>* pBlock);

// Registration functions. These are wrappers around IoWMIRegistrationControl.
	NTSTATUS Register(
		PDEVICE_OBJECT pDevice,
		PCWSTR MofResourceName = NULL,
		PCWSTR InstanceNameBase = NULL
		);

	NTSTATUS Deregister(void);

// Internal use
	VOID HasMethodBlocks(void) { m_WmilibContext.ExecuteWmiMethod =_DpWmiExecuteMethod; }

// Get pointer to WMILIB_CONTEXT
	operator WMILIB_CONTEXT*(void) { return &m_WmilibContext; }

// Get default method object for context
	PVOID GetDefaultMethodObject(void);

//////////////////////////////////////////////////////////////////////////
// Static member functions
//////////////////////////////////////////////////////////////////////////
//
// Handlers for callbacks from WmiSystemControl
//
// These members field callbacks from WmiSystemControl, which is called
// in the driver's handler for IRP_MJ_SYSTEM_CONTROL. The correspondence
// between minor functions and the members is as follows:
//
// IRP_MN_QUERY_ALL_DATA			_DpWmiQueryDataBlock
// IRP_MN_QUERY_SINGLE_INSTANCE		_DpWmiQueryDataBlock
// IRP_MN_CHANGE_SINGLE_INSTANCE	_DpWmiSetDataBlock
// IRP_MN_CHANGE_SINGLE_ITEM		_DpWmiSetDataItem
// IRP_MN_ENABLE_EVENTS				_DpWmiFunctionControl
// IRP_MN_DISABLE_EVENTS			_DpWmiFunctionControl
// IRP_MN_ENABLE_COLLECTION			_DpWmiFunctionControl
// IRP_MN_DISABLE_COLLECTION		_DpWmiFunctionControl
// IRP_MN_REGINFO					_DpWmiQueryReginfo
// IRP_MN_EXECUTE_METHOD			_DpWmiExecuteMethod

	static NTSTATUS _DpWmiQueryReginfo(
       IN PDEVICE_OBJECT DeviceObject,
       OUT PULONG RegFlags,
       OUT PUNICODE_STRING InstanceName,
       OUT PUNICODE_STRING *RegistryPath,
       OUT PUNICODE_STRING MofResourceName,
       OUT PDEVICE_OBJECT *Pdo
       );

	static NTSTATUS _DpWmiQueryDataBlock(
       IN PDEVICE_OBJECT DeviceObject,
       IN PIRP Irp,
       IN ULONG GuidIndex,
       IN ULONG InstanceIndex,
       IN ULONG InstanceCount,
       IN OUT PULONG InstanceLengthArray,
       IN ULONG BufferAvail,
       OUT PUCHAR Buffer
       );

	static NTSTATUS _DpWmiSetDataItem(
       IN PDEVICE_OBJECT DeviceObject,
       IN PIRP Irp,
       IN ULONG GuidIndex,
       IN ULONG InstanceIndex,
       IN ULONG DataItemId,
       IN ULONG BufferSize,
       IN PUCHAR Buffer
       );

	static NTSTATUS _DpWmiSetDataBlock(
       IN PDEVICE_OBJECT DeviceObject,
       IN PIRP Irp,
       IN ULONG GuidIndex,
       IN ULONG InstanceIndex,
       IN ULONG BufferSize,
       IN PUCHAR Buffer
       );

	static NTSTATUS _DpWmiExecuteMethod(
       IN PDEVICE_OBJECT DeviceObject,
       IN PIRP Irp,
       IN ULONG GuidIndex,
       IN ULONG InstanceIndex,
       IN ULONG MethodId,
       IN ULONG InBufferSize,
       IN ULONG OutBufferSize,
       IN OUT PUCHAR Buffer
       );

	static NTSTATUS _DpWmiFunctionControl(
       IN PDEVICE_OBJECT DeviceObject,
       IN PIRP Irp,
       IN ULONG GuidIndex,
       IN WMIENABLEDISABLECONTROL Function,
       IN BOOLEAN Enable
       );

	static NTSTATUS GetBlock(
		PDEVICE_OBJECT pDevice,
		ULONG GuidIndex,
		ULONG InstanceIndex,
		KWmiBlock<F>*& pBlock
		);

/////////////////////////////////////////////////////////////////////////
// Data members
//////////////////////////////////////////////////////////////////////////

// m_Dispatchers is the array of dispatcher objects. Each dispatcher holds an array of blocks.
	KArray<KWmiDispatcher<F>*, KGenericLockMutex> m_Dispatchers;

// This is the array of WMIBUIDINFO structs. m_Dispatchers[i] corresponds to
// m_GuidInfo[i]. This is key to making things work correctly and efficiently.
	KArray<WMIGUIDREGINFO, KGenericLockMutex>  m_GuidInfo;

// m_state tracks the state of the context object.
	enum WmiState {
		state_preregistered, // Registration has not yet been done
		state_registered,    // Context has been registered
		state_deregistered 	 // Context has been deregistered
	} m_state;

// m_DeviceObject is a pointer to the system device object WMI provider that
// owns this context object. This gets initialized in member function Register.
public:
	PDEVICE_OBJECT	m_DeviceObject;

// Handle for logging/tracing, supplied by WMI when traced events enabled
	ULONG64 m_hTrace;

protected:
// This is a pointer to a string that may be used as the stem for block instance names.
	PWSTR m_InstanceName;

// This is the (optional) string that specifies the name of the device's MOF resource
	KUstring m_MofName;
	BOOLEAN m_MofNameSpecified;

// m_WmilibContext is the data structure whose address is passed to
// WmiSystemControl from the driver's IRP_MJ_SYSTEM_CONTROL handler.
	WMILIB_CONTEXT 	m_WmilibContext;

	friend KWmiBlock<F>;
	friend KWmiDispatcher<F>;

///////////////////////////////////////////////////////////////////////////////////////
//
//	SetMethods
//		Template function to set method handlers
//
//	Parameters:
//		MethodCount
//			Number of methods
//
//		Methods
//			Pointer to method array
//
//	Return Value:
//		Returns STATUS_INSUFFICIENT_RESOURCES if unable to allocate storage
//		for method pointers.
//
//	Comments:
//		Here is a sample to illustrate how this function is to be
//		called. Suppose the block class is MyBlock and the method
//		class is MyDevice:
//
//			KWmiDataBlock<MyBlock,MyDevice>::METHOD  MethodArray[] = {
//				MyDevice::MyBlockMethod1,
//				MyDevice::MyBlockMethod2,
//				MyDevice::MyBlockMethod3
//				);
//
//			SetMethods(3, MethodArray);
//
//		The above is repeated for each block type (if it has methods) that the
//		device uses.
//
//		In the above example, the caller must implement the methods with the
//		following prototypes:
//
//		NTSTATUS MyDevice::MyBlockMethod1(
//						MyBlock* pBlock,
//						ULONG InBufferSize,
//						ULONG OutBufferSize
//						PUCHAR Buffer
//						);
//
//		NTSTATUS MyDevice::MyBlockMethod2(
//						MyBlock* pBlock,
//						ULONG InBufferSize,
//						ULONG OutBufferSize
//						PUCHAR Buffer
//						);
//		etc.
public:
	template <class T, class U>
	NTSTATUS SetMethods(ULONG MethodCount,
		NTSTATUS (U::**Methods)(T*,ULONG,ULONG,PUCHAR,PULONG)
		)
	{
		HasMethodBlocks();
		return GetSetMethods(TRUE, &MethodCount, &Methods);
	}


///////////////////////////////////////////////////////////////////////////////////////
//
//	GetMethods
//		Template function to get method handlers
//
//	Parameters:
//		pMethodCount
//			pointer to number of methods
//
//		Methods
//			Pointer to pointer to method array
//
//	Return Value:
//		Returns STATUS_SUCCESS.
//
//	Comments:
//		This routine just retrieves the static data from GetSetMethods.
//
	template <class T, class U>
	NTSTATUS GetMethods(
		ULONG* pMethodCount,
		NTSTATUS (U::***Methods)(T*,ULONG,ULONG,PUCHAR,PULONG)
		)
	{
		return GetSetMethods(FALSE, pMethodCount, Methods);
	}


///////////////////////////////////////////////////////////////////////////////////////
//
//	GetSetMethods
//		Template function to get or set method handlers
//
//	Parameters:
//		DoSet
//			If TRUE, this is a set operation; else get
//
//		pMethodCount
//			pointer to number of methods
//
//		Methods
//			Pointer to pointer to method array
//
//	Return Value:
//		For get, always returns success. For set, it may return
//		STATUS_INSUFFICIENT_RESOURCES if it can't allocate the array.
//
//	Comments:
//		An instance of this function exists for each T,U pair. Static variables
//		local to the function provide storage of method pointers specific to
//		each T,U pair.
//
	template <class T, class U>
	NTSTATUS GetSetMethods(
		BOOLEAN DoSet,
		ULONG* pMethodCount,
		NTSTATUS (U::***Methods)(T*,ULONG,ULONG,PUCHAR,PULONG)
		)
	{
		static ULONG StaticMethodCount=0;
		static NTSTATUS (U::**StaticMethodsPointer)(T*,ULONG,ULONG,PUCHAR,PULONG) = NULL;

		if (DoSet) // Set operation
		{
			// discard previous settings
			delete [] StaticMethodsPointer;
			StaticMethodsPointer = NULL;
			StaticMethodCount = *pMethodCount;

			// setting method count to zero is done to release reources
			if (*pMethodCount == 0)
			{
				return STATUS_SUCCESS;
			}

			// Allocate new methods array

			StaticMethodsPointer = new (PagedPool) (NTSTATUS (U::*[*pMethodCount])(T*,ULONG,ULONG,PUCHAR,PULONG));

			// Initialize new array with provided data
			if ( StaticMethodsPointer != NULL )
			{
				for (ULONG i=0; i < *pMethodCount; i++)
				{
					StaticMethodsPointer[i] = (*Methods)[i];
				}
			}
			// Failed to allocate memory for methods array
			else
			{
				StaticMethodCount = 0;
				return STATUS_INSUFFICIENT_RESOURCES;
			}
		}
		else	// Get operation
		{
			*pMethodCount = StaticMethodCount;
			*Methods = StaticMethodsPointer;
		}

		return STATUS_SUCCESS;
	}
};

//////////////////////////////////////////////////////////////////////////
//  class KWmiDispatcher

template <typename F = KStdWmiInterface>
class KWmiDispatcher
{
public:
	SAFE_DESTRUCTORS

	KWmiDispatcher(KWmiContext<F>* pContext);
	~KWmiDispatcher(void);

	NTSTATUS InsertBlock(KWmiBlock<F>* pBlock, PULONG InstanceIndex);

	NTSTATUS FunctionControl(UCHAR action);
	BOOLEAN EventsEnabled(void) { return m_EventsEnabled; }
	BOOLEAN CollectionEnabled(void) { return m_CollectionEnabled; }

protected:
	KArray<KWmiBlock<F>*>	m_Blocks;
	KWmiContext<F>*			m_Context;
	ULONG					m_GuidIndex;
	BOOLEAN 				m_CollectionEnabled;
	BOOLEAN					m_EventsEnabled;

	friend KWmiContext<F>;
	friend KWmiBlock<F>;
};


//////////////////////////////////////////////////////////////////////////////////////////
// Members of KWmiContext
//////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiContext::KWmiContext
//		Constructor for KWmiContext
//
//	Comments:
//		Initializes members and sets up the WMILIB_CONTEXT struct to reference the
//		static dispatchers.
//
//		IRQL must be PASSIVE_LEVEL.
//
template <class F>
KWmiContext<F>::KWmiContext(void) :
	m_Dispatchers(16, 8, NonPagedPool),
	m_GuidInfo(16, 8, NonPagedPool),
	m_state( state_preregistered ),
	m_DeviceObject(NULL),
	m_InstanceName(NULL),
	m_MofName(),
	m_MofNameSpecified(FALSE)
{
// Initialize the WMILIB_CONTEXT member. It holds a pointer to the list of supported
// GUIDs (initially empty), and pointers to the functions that handle IRP_MJ_SYSTEM_CONTROL.

	m_WmilibContext.GuidCount = 0;
	m_WmilibContext.GuidList = m_GuidInfo.Data();

	m_WmilibContext.QueryWmiRegInfo= 	_DpWmiQueryReginfo;
	m_WmilibContext.QueryWmiDataBlock=	_DpWmiQueryDataBlock;
	m_WmilibContext.SetWmiDataBlock= 	_DpWmiSetDataBlock;
	m_WmilibContext.SetWmiDataItem= 	_DpWmiSetDataItem;
	m_WmilibContext.WmiFunctionControl= _DpWmiFunctionControl;

// The pointer to the ExecuteMethod routine will be initialized later if required.
	m_WmilibContext.ExecuteWmiMethod= 	NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiContext::~KWmiContext
//		Destructor for KWmiContext
//
//	Comments:
//		Currently has little or nothing to do.
//
template <class F>
KWmiContext<F>::~KWmiContext(void)
{
	// do not destroy while registered

	ASSERT ( m_state != state_registered );
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiContext::Register
//		Register with the system. This call causes the system to send an
//		IRP_MJ_SYSTEM_CONTROL with minor function IRP_MN_REGINFO.
//
//	Parameters:
//		pDevice
//			The FDO providing WMI
//
//		MofResourceName
//			If the provider creates instances of MOF classes that are not in
//			the standard set defined by WMI, then it must identify the MOF
//			instance containing its class definitions.
//
//			The MOF file is compiled (to .bmf). The resource file (.rc) must
//			include a line of the form:
//
//			resource_name MOFDATA pathname.bmf
//
//			The resource_name string in the MOF file must match this parameter.
//
//		InstanceNameBase
//			String to serve as the stem for block instance names for this
//			device. If NULL, then the PDO name will be used instead.
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		This routine causes an IRP_MJ_SYSTEM_CONTROL/IRP_MN_REGINFO to be sent
//		to the named device. This routine stores the information that is passed in,
//		in order to fully handle the processing of the IRP_MN_REGINFO when it
//		arrives.
//
//		IRQL must be PASSIVE_LEVEL.
//
template <class F>
NTSTATUS KWmiContext<F>::Register(
	PDEVICE_OBJECT pDevice,
	PCWSTR MofResourceName,
	PCWSTR InstanceNameBase
	)
{
	NTSTATUS status;
	ULONG action;
	ULONG n;

	switch ( m_state )
	{
	default:
		ASSERT(FALSE);				// unexpected, but just fall through

	case state_preregistered:		// Registration has not yet been done
	case state_deregistered:		// Context has been deregistered
		action = WMIREG_ACTION_REGISTER;
		break;

	case state_registered:			// Context has already been registered
		action = WMIREG_ACTION_REREGISTER;
		break;
	};

// If the caller supplies a name string (for naming block instances) then
// we must allocate memory and copy it for the callback. We use ExAllocatePool
// explicitly because the system will free the block using ExFreePool.

	if ( InstanceNameBase != NULL )
	{
		n = ustrsiz(InstanceNameBase);
		m_InstanceName = static_cast<PWSTR>(ExAllocatePool(PagedPool, n+sizeof(WCHAR)));

		if ( m_InstanceName == NULL )
		{
			BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR,(this, "KWmiContext instance name base", n+sizeof(WCHAR)));
			return STATUS_INSUFFICIENT_RESOURCES;
		}

		ustrcpy(m_InstanceName, InstanceNameBase);
	}

// Save the MOF resource name.

	if ( MofResourceName != NULL )
	{
		USHORT mofLen = USHORT(ustrlen(MofResourceName));

		if ( mofLen > m_MofName.MaximumLength() )
		{
			status = m_MofName.GrowMaxBy(USHORT(mofLen-m_MofName.MaximumLength()), PagedPool);
			if ( !NT_SUCCESS(status) )
			{
				BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR,(this, "KWmiContext MOF name", mofLen));
				return STATUS_INSUFFICIENT_RESOURCES;
			}
		}

		m_MofName.Assign(MofResourceName);
		m_MofNameSpecified = TRUE;
	}
	else
	{
		m_MofNameSpecified = FALSE;
	}


// Request a registration IRP from the system.

	status = IoWMIRegistrationControl(pDevice, action);

// If the request succeeded, update the object state to state_registered

	if ( NT_SUCCESS(status) )
	{
		m_DeviceObject = pDevice;
		m_state = state_registered;
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiContext::Deregister
//		Deregister the context from the WMI subsystem
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		This is not a request for an IRP_SYSTEM_CONTROL. The WMI manager simply
//		disassociates the device object belonging to this context.
//
//		IRQL must be PASSIVE_LEVEL.
//
template <class F>
NTSTATUS KWmiContext<F>::Deregister(void)
{
	NTSTATUS status = STATUS_SUCCESS;

	ASSERT ( m_DeviceObject != NULL );

	if ( m_state == state_registered )
	{
		status = IoWMIRegistrationControl(m_DeviceObject, WMIREG_ACTION_DEREGISTER);
		if ( NT_SUCCESS(status) )
		{
			m_state = state_deregistered;
		}
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiContext::AddDispatcher
//		This function is used to add a dispatcher to a WMI context.
//
//	Parameters:
//		pGuid
//			Pointer to the GUID associated with the dispatcher
//
//		Flags
//			WMI registration flags
//
//		pGuidIndex
//			Address of variable to receive the GUID's index in the context
//
//		pDispatcher
//			Variable to be set to a pointer to the new dispatcher object
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		If the GUID is already known to this context, it does not create a new
//		dispatcher. Rather, it returns the GUID index and dispatcher pointer of
//		the existing dispatcher. If the GUID is not know to this context,  this
//		routine allocates a new dispatcher and initializes it.
//
//		IRQL must be PASSIVE_LEVEL.
//
template <class F>
NTSTATUS KWmiContext<F>::AddDispatcher(
	const GUID* pGuid,
	ULONG Flags,
	PULONG pGuidIndex,
	KWmiDispatcher<F>*& pDispatcher
	)
{
	ULONG i;

// First, check if we already know about this GUID

	for (pDispatcher=NULL, i = 0; i < m_WmilibContext.GuidCount; i++)
	{
		if ( IsEqualGUID(*m_WmilibContext.GuidList[i].Guid,*pGuid) )
		{
		// Found it. Done.
			pDispatcher = m_Dispatchers[i];
			return STATUS_SUCCESS;
		}
	}

// Now we need to set up a new element in the GuidInfo array

	WMIGUIDREGINFO GuidInfo;

	GuidInfo.Guid = pGuid;
	GuidInfo.InstanceCount = 0;
	GuidInfo.Flags = Flags;

	m_GuidInfo.InsertNext(GuidInfo, pGuidIndex);

// Bump the Guid count for this context
	m_WmilibContext.GuidCount++;

// Because inserting can cause a re-allocation of the array, we must update the
// pointer to the WMIGUIDREGINFO array in the WMILIB_CONTEXT.

	m_WmilibContext.GuidList = m_GuidInfo.Data();

// Now allocate the new dispatcher object. This is basically an array of block
// pointers.

	pDispatcher = new (NonPagedPool) KWmiDispatcher<F>(this);

	if ( pDispatcher == NULL )
	{
		BOUNDS_CHECKER(OUT_OF_MEMORY_ERROR,(this,"KWmiContext::Dispatcher",sizeof KWmiDispatcher<F>));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

// We have created the WMIGUIDINFO object and inserted into our array.
// Now we need to insert the dispatcher object at the same index in
// the dispatcher array.

	pDispatcher->m_GuidIndex = *pGuidIndex;
	NTSTATUS status = m_Dispatchers.Insert(*pGuidIndex, pDispatcher);

// If the array insertion failed, then clean up the dispatcher. Otherwise, indicate success.

	if ( !NT_SUCCESS(status) )
	{
		delete pDispatcher;
	}
	else
	{
		status = STATUS_SUCCESS;
	}

// Set the collection enable boolean based on whether or not this block is expensive

	if ((Flags & WMIREG_FLAG_EXPENSIVE) == 0)
	{
		pDispatcher->m_CollectionEnabled = TRUE;
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiContext::AddBlock
//		Add a block to the context.
//
//	Parameters:
//		pBlock
//			pointer to the block to add
//
//		pGuid
//			GUID that identifies the block class
//
//		Flags
//			registration flags (see WMIGUIDREGINFO)
//
//		pGuidIndex
//			returns GUID index for the block
//
//		pBlockIndex
//			return block index in the dispatcher
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		This function is used to add a block to a WMI context. It creates a
//		KWmiDispatcher object if one does not already exist for the given GUID.
//		This function is called by the ctor for KWmiBlock.
//
template <class F>
NTSTATUS KWmiContext<F>::AddBlock(
	KWmiBlock<F>* pBlock,
	const GUID* pGuid,
	ULONG Flags,
	PULONG pGuidIndex,
	PULONG pBlockIndex
	)
{
	NTSTATUS status;
	KWmiDispatcher<F>* pDispatcher;

// Make sure we have a dispatcher object

	status = AddDispatcher(pGuid, Flags, pGuidIndex, pDispatcher);

	if ( !NT_SUCCESS(status) )
		return status;

// We have a dispatcher set up correctly. Now just insert the block in
// the dispatcher's block array.

	status = pDispatcher->m_Blocks.InsertNext(pBlock, pBlockIndex);

// Increment the count in the corresponding WMIREGINFO struct

	if ( NT_SUCCESS(status) )
		m_GuidInfo[*pGuidIndex].InstanceCount++;

	return status;
}


///////////////////////////////////////////////////////////////////////////////
//
//	KWmiContext::RemoveDispatcher
//		Remove an existing dispatcher from the context
//
//	Parameters:
//		dispatcher
//			Pointer to the dispatcher to be removed
//
//	Return Value:
//		Returns status.
//
//	Comments:
//
//
template <class F>
NTSTATUS KWmiContext<F>::RemoveDispatcher(KWmiDispatcher<F>* dispatcher)
{
	ULONG GuidIndex = dispatcher->m_GuidIndex;

	// Make sure blocks are all gone
	ASSERT ( m_GuidInfo[GuidIndex].InstanceCount == 0 );

	if ( m_GuidInfo[GuidIndex].InstanceCount == 0 )
	{
		delete m_Dispatchers[GuidIndex];
		m_Dispatchers[GuidIndex] = NULL;
		m_WmilibContext.GuidCount--;

		// if there are no more dispatchers, self destruct
		if ( (m_WmilibContext.GuidCount == 0) && (m_DeviceObject != NULL) )
		{
			PDEVICE_OBJECT pDevice = m_DeviceObject;
			m_DeviceObject = NULL;

			_self_delete();
			GetContextFromDevice(pDevice) = NULL;
		}

		return STATUS_SUCCESS;
	}
	else
	{
		return STATUS_UNSUCCESSFUL;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiContext::RemoveBlock
//		Remove an existing block from the context
//
//	Parameters:
//		pBlock
//			Pointer to the block to be removed
//
//	Return Value:
//		Returns status.
//
//	Comments:
//
//
template <class F>
NTSTATUS KWmiContext<F>::RemoveBlock(KWmiBlock<F>* pBlock)
{
	ULONG BlockIndex = pBlock->m_BlockIndex;
	ULONG GuidIndex = pBlock->m_GuidIndex;

	pBlock->m_BlockIndex = ULONG(-1);
	pBlock->m_GuidIndex = ULONG(-1);
	pBlock->m_Context = NULL;

	_try
	{
		m_Dispatchers[GuidIndex]->m_Blocks[BlockIndex] = NULL;
		m_GuidInfo[GuidIndex].InstanceCount--;

		if (m_GuidInfo[GuidIndex].InstanceCount == 0)
		{
			return RemoveDispatcher(m_Dispatchers[GuidIndex]);
		}
		else
		{
			return STATUS_SUCCESS;
		}

	}
	_except( EXCEPTION_EXECUTE_HANDLER )
	{
		ASSERT( !"Failed to remove block" );
		return STATUS_UNSUCCESSFUL;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	KWmiContext::_DpWmiQueryReginfo
//		Handles registration callback from WMILIB
//
//	Parameters:
//		DeviceObject
//			Pointer to system device object being registered as provider
//
//		RegFlags
//			Pointer to variable to receive registration flags (see wmi.h)
//
//		InstanceName
//			Pointer to UNICODE string to be initialized by this routine
//			with name stem for instances. If PDO naming is indicated by
//			the flags,
//
//		RegistryPath
//			Address of a pointer. This routine sets the pointer to point
//			to the driver's registry path string.
//
//		MofResourceName
//			If the provider has unique MOF classes, this routine must initialize
//			the UNICODE_STRING pointed to by this parameter to name the
//			MOF resource as it appears in the resource (.rc) file.
//
//		Pdo
//			Address of variable to receive the provider's PDO if the RegFlags
//			indicate that the block instance names are based on the PDO name.
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		This is a static function. Its prototype is dictated by WMILIB.
// 		This function is called by WMI lib in response to an IRP_MN_REGINFO.
//
//		See also: KWmiContext::Register.
//
// 		TBD: NOTE that this does not really handle the re-registration/update
//		case.
//
//		IRQL is PASSIVE_LEVEL.
//
template <class F>
NTSTATUS KWmiContext<F>::_DpWmiQueryReginfo(
   IN PDEVICE_OBJECT DeviceObject,
   OUT PULONG RegFlags,
   OUT PUNICODE_STRING InstanceName,
   OUT PUNICODE_STRING *RegistryPath,
   OUT PUNICODE_STRING MofResourceName,
   OUT PDEVICE_OBJECT *Pdo
   )
{
	KWmiContext<F>* pContext = GetContextFromDevice(DeviceObject);

// Set the registry path return value using the value saved in DriverEntry
	*RegistryPath = GetDriverRegistryPath();

// The name stem is based on either the PDO name or on the string passed
// to Register.

	if ( pContext->m_InstanceName != NULL )	// if string was passed
	{
		*RegFlags = WMIREG_FLAG_INSTANCE_BASENAME;
		RtlInitUnicodeString(InstanceName, pContext->m_InstanceName);
		pContext->m_InstanceName = NULL;
	}

// Otherwise, the instance name base is the PDO
	else
	{
		*Pdo = GetPdo(DeviceObject);
		*RegFlags = WMIREG_FLAG_INSTANCE_PDO;
	}

// If a MOF resource name was passed to member Register, set it up now

	if ( pContext->m_MofNameSpecified )
	{
		*MofResourceName = *PUNICODE_STRING(pContext->m_MofName);
	}

	// WmiCompleteSystemRequest is NOT to be called here.

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
//	_DpWmiQueryDataBlock
//		Called by WMILIB when client requests data block.
//
//	Parameters:
//		DeviceObject
//			Provider FDO
//
//		Irp
//			Requesting IRP_MJ_SYSTEM_CONTROL
//
//		GuidIndex
//			Index into context of block's GUID
//
//		InstanceIndex
//			Index into dispatcher's array of block instances
//
//		InstanceCount
//			Number of instances to get
//
//		pLengthArray
//			pLengthArray[i] on input holds the number of bytes provided
//			in Buffer for the ith instance. On output, it is set to the
//			number of bytes written by the provider.
//
//		BufferAvail
//			Total size in bytes of buffer (input).
//
//		Buffer
//			Buffer to receive data
//
//	Return Value:
//		Returns status
//
//	Comments:
//		The system calls this to query data in a provider's block.
//		This is a static function. Its prototype is dictated by WMILIB.
//
//		This routine calls virtual function KWmiBlock::_query to get the
//		data.
//
template <class F>
NTSTATUS KWmiContext<F>::_DpWmiQueryDataBlock(
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp,
   IN ULONG GuidIndex,
   IN ULONG InstanceIndex,
   IN ULONG InstanceCount,
   IN OUT PULONG pLengthArray,
   IN ULONG BufferAvail,
   OUT PUCHAR Buffer
   )
{
	KWmiBlock<F>* pBlock;
	NTSTATUS status=STATUS_SUCCESS;
	BOOLEAN AtLeastOneInstanceBufferTooSmall = FALSE;
	ULONG TotalBytesRequired = 0;

	// Loop over each requested instance.
	for (ULONG i=0; i < InstanceCount; i++, InstanceIndex++)
	{
		ULONG BufferUsed;

		// Retrieve the pointer to the block
		status = GetBlock(DeviceObject, GuidIndex,  InstanceIndex, pBlock);

		// If the block was found, then query it.
		if ( NT_SUCCESS(status) )
		{
			status =
				pBlock->_query(Irp, BufferAvail, Buffer, &BufferUsed, (i==(InstanceCount-1)));

			// If the provided storage was insufficient, set status accordingly
			if ( status == STATUS_BUFFER_TOO_SMALL )
			{
				AtLeastOneInstanceBufferTooSmall = TRUE;
				if(pLengthArray != NULL)
					pLengthArray[i] = 0;
			}

			// Normal case, tell client how much buffer used for this instance
			else
			{
				pLengthArray[i] = BufferUsed;
			}

			// Align instances on 8 byte boundary
			BufferUsed = (BufferUsed+7) & ~7;
			BufferAvail -= BufferUsed;

			Buffer += BufferUsed;
			TotalBytesRequired += BufferUsed;
		}
		else
			break;
	}

	if (AtLeastOneInstanceBufferTooSmall)
	{
		if(pLengthArray)
			*pLengthArray = NULL;
		status = STATUS_BUFFER_TOO_SMALL;
	}

	if ( status != STATUS_PENDING )
		return WmiCompleteRequest(DeviceObject, Irp, status, TotalBytesRequired, IO_NO_INCREMENT);
	else
	{
		IoMarkIrpPending(Irp);
		return STATUS_PENDING;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	_DpWmiSetDataBlock
//		Set the provider's data block
//
//	Parameters:
//		DeviceObject
//			Provider FDO
//
//		Irp
//			Requesting IRP_MJ_SYSTEM_CONTROL
//
//		GuidIndex
//			Index into context of block's GUID
//
//		InstanceIndex
//			Index into dispatcher's array of block instances
//
//		BufferSize
//			Total size in bytes of buffer (input).
//
//		Buffer
//			Buffer containing block data to store
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		This is a static function. Its prototype is dictated by WMILIB.
//		The system calls this to set data in a provider's block.
//
template <class F>
NTSTATUS KWmiContext<F>::_DpWmiSetDataBlock(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp,
	IN ULONG GuidIndex,
	IN ULONG InstanceIndex,
	IN ULONG BufferSize,
	IN PUCHAR Buffer
	)
{
	KWmiBlock<F>* pBlock;
	NTSTATUS status = GetBlock(DeviceObject, GuidIndex,  InstanceIndex, pBlock);

	if ( NT_SUCCESS(status) )
	{
	 	return (pBlock->_set(Irp, BufferSize, Buffer));
	}

	if ( status != STATUS_PENDING )
		return WmiCompleteRequest(DeviceObject, Irp, status, 0, IO_NO_INCREMENT);
	else
	{
		IoMarkIrpPending(Irp);
		return STATUS_PENDING;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	_DpWmiSetDataItem
//		description.
//
//	Parameters:
//		DeviceObject
//			Provider FDO
//
//		Irp
//			Requesting IRP_MJ_SYSTEM_CONTROL
//
//		GuidIndex
//			Index into context of block's GUID
//
//		InstanceIndex
//			Index into dispatcher's array of block instances
//
//		DataItemId
//			Id of item to get (from its MOF declaration)
//
//		BufferSize
//			Size in bytes of Buffer
//
//		Buffer
//			Buffer containing item data to store
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		The system calls this to set a specific item within a provider's block.
//		This is a static function. Its prototype is dictated by WMILIB.
//
template <class F>
NTSTATUS KWmiContext<F>::_DpWmiSetDataItem(
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp,
   IN ULONG GuidIndex,
   IN ULONG InstanceIndex,
   IN ULONG DataItemId,
   IN ULONG BufferSize,
   IN PUCHAR Buffer
   )
{
	KWmiBlock<F>* pBlock;
	NTSTATUS status = GetBlock(DeviceObject, GuidIndex,  InstanceIndex, pBlock);

	if ( NT_SUCCESS(status) )
	{
	 	status = pBlock->SetItem(Irp, DataItemId, BufferSize, Buffer);
	}

	if ( status != STATUS_PENDING )
		return WmiCompleteRequest(DeviceObject, Irp, status, 0, IO_NO_INCREMENT);
	else
	{
		IoMarkIrpPending(Irp);
		return STATUS_PENDING;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	_DpWmiFunctionControl
//
//	Parameters:
//		DeviceObject
//			Provider FDO
//
//		Irp
//			Requesting IRP_MJ_SYSTEM_CONTROL
//
//		GuidIndex
//			Index into context of block's GUID
//
//		Function
//			Not used
//
//		Enable
//			Not used
//
//	Return Value:
//		Returns status
//
//	Comments:
//		The system calls this to enable/disable collection/events.
//		This is a static function. Its prototype is dictated by WMILIB.
//
//		The IRP minor function determines the action.
//
template <class F>
NTSTATUS KWmiContext<F>::_DpWmiFunctionControl(
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp,
   IN ULONG GuidIndex,
   IN WMIENABLEDISABLECONTROL Function,
   IN BOOLEAN Enable
   )
{
	NTSTATUS status;

	PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);
	UCHAR MinorFunction = pIoStack->MinorFunction;

	KWmiContext<F>* pContext = GetContextFromDevice(DeviceObject);
	KWmiDispatcher<F>* pDispatcher = pContext->m_Dispatchers[GuidIndex];

	if ( ( MinorFunction == IRP_MN_ENABLE_EVENTS) &&
	     ( pContext->m_GuidInfo[GuidIndex].Flags & WMIREG_FLAG_TRACE_CONTROL_GUID )
	   )
	{
		pContext->m_hTrace = ((WNODE_HEADER*)pIoStack->Parameters.WMI.Buffer)->HistoricalContext;
// TBD: enable all events that have WMIREG_FLAG_TRACED_GUID

	}

	ASSERT (pDispatcher != NULL);

	status = pDispatcher->FunctionControl(MinorFunction);

	if ( status != STATUS_PENDING )
	{
		return (WmiCompleteRequest(DeviceObject, Irp, status, 0, IO_NO_INCREMENT));
	}
	else
	{
		IoMarkIrpPending(Irp);
		return STATUS_PENDING;
	}

	UNREFERENCED_PARAMETER(Enable);
	UNREFERENCED_PARAMETER(Function);
}

///////////////////////////////////////////////////////////////////////////////
//
//	_DpWmiExecuteMethod
//		description.
//
//	Parameters:
//		DeviceObject
//			Provider FDO
//
//		Irp
//			Requesting IRP_MJ_SYSTEM_CONTROL
//
//		GuidIndex
//			Index into context of block's GUID
//
//		InstanceIndex
//			Index into dispatcher's array of block instances
//
//		MethodId
//			Id of method to execute (per MOF declaration)
//
//		InBufferSize
//			Size in bytes of input data
//
//		OutBufferSize
//			Size in bytes of output data
//
//		Buffer
//			Buffer to receive/return data (passed to method)
//
//	Return Value:
//		Returns status of method.
//
//	Comments:
//		The system calls this to instruct a block to execute one of its methods.
//		This is a static function. Its prototype is dictated by WMILIB.
//
template <class F>
NTSTATUS KWmiContext<F>::_DpWmiExecuteMethod(
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp,
   IN ULONG GuidIndex,
   IN ULONG InstanceIndex,
   IN ULONG MethodId,
   IN ULONG InBufferSize,
   IN ULONG OutBufferSize,
   IN OUT PUCHAR Buffer
   )
{
	KWmiBlock<F>* pBlock;
	ULONG BufferUsed=0;

	NTSTATUS status = GetBlock(DeviceObject, GuidIndex,  InstanceIndex, pBlock);

	if ( NT_SUCCESS(status) )
	{
		status = pBlock->_execute_method(Irp, MethodId, InBufferSize, OutBufferSize, Buffer, &BufferUsed);

		if ( status == STATUS_PENDING )
		{
			IoMarkIrpPending(Irp);
			return STATUS_PENDING;
		}
	}

	return WmiCompleteRequest(DeviceObject, Irp, status, BufferUsed, IO_NO_INCREMENT);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiContext::GetBlock
//		Find a pointer to a KWmiBlock
//
//	Parameters:
//		pDevice
//			Provider FDO
//
//		GuidIndex
//			Index into context of block's GUID
//
//		InstanceIndex
//			Index into dispatcher's array of block instances
//
//		pBlock
//			Reference to the block pointer to be set by this function
//
//	Return Value:
//		Returns status
//
//	Comments:
//		The static WMILIB callback handlers use this utility function
//		to locate the block instance specified by the GUID index and
//		block instance parameters.
//
template <class F>
NTSTATUS KWmiContext<F>::GetBlock(
	PDEVICE_OBJECT pDevice,
	ULONG GuidIndex,
	ULONG InstanceIndex,
	KWmiBlock<F>*& pBlock
	)
{
	NTSTATUS status;

	KWmiContext<F>* pContext = GetContextFromDevice(pDevice);
	KWmiDispatcher<F>* pDispatcher = pContext->m_Dispatchers[GuidIndex];

	ASSERT (pDispatcher != NULL);

	if ( pDispatcher != NULL )
	{
		pBlock = pDispatcher->m_Blocks[InstanceIndex];

		ASSERT ( pBlock != NULL );

		if ( pBlock != NULL )
		{
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_WMI_INSTANCE_NOT_FOUND;
		}
	}
	else
	{
		status = STATUS_WMI_GUID_NOT_FOUND;
	}

	return status;
}


template <class F>
inline PVOID KWmiContext<F>::GetDefaultMethodObject(void)
{
	return DefaultMethodObject(m_DeviceObject);
}

///////////////// Implementation of class KWmiDispatcher ///////////////////////


///////////////////////////////////////////////////////////////////////////////
//
//	KWmiDispatcher::KWmiDispatcher
//		Ctor for KWmiDispatcher
//
//	Parameters:
//		pContext
//			Pointer to the context object that 'owns' this dispatcher
//
//	Comments:
//		Initializes the object.
//
template <class F>
KWmiDispatcher<F>::KWmiDispatcher(KWmiContext<F>* pContext) :
	m_Blocks(8,4,NonPagedPool),
	m_Context(pContext),
	m_GuidIndex( ULONG(-1) ),
	m_CollectionEnabled(FALSE),
	m_EventsEnabled(FALSE)
{
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiDispatcher::~KWmiDispatcher
//		Dtor for KWmiDispatcher
//
//	Comments:
//		The caller should remove the dispatcher from the owning context before
//		invoking the destructor.
//
template <class F>
KWmiDispatcher<F>::~KWmiDispatcher(void)
{
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiDispatcher::FunctionControl
//		Handle enable/disable collection/events
//
//	Parameters:
//		pIrp
//			Pointer to the requesting IRP
//	Return Value:
//		Returns STATUS_SUCCESS.
//
//	Comments:
//		Calls the appropriate function in each of the dispatcher's
//		block instances, based on the minor function in the IRP.
//
template <class F>
NTSTATUS KWmiDispatcher<F>::FunctionControl(UCHAR action)
{
	ULONG i;
	VOID (KWmiBlock<F>::*NotificationMember)(void);

	// First decide which function is to be called
	switch (action)
	{
	case IRP_MN_ENABLE_EVENTS:
		m_EventsEnabled = TRUE;
		NotificationMember = KWmiBlock<F>::OnEnableEvents;
		break;

	case IRP_MN_DISABLE_EVENTS:
		m_EventsEnabled = FALSE;
		NotificationMember = KWmiBlock<F>::OnDisableEvents;
		break;

	case IRP_MN_ENABLE_COLLECTION:
		m_CollectionEnabled = TRUE;
		NotificationMember = KWmiBlock<F>::OnEnableCollection;
		break;

	case IRP_MN_DISABLE_COLLECTION:
		m_CollectionEnabled = FALSE;
		NotificationMember = KWmiBlock<F>::OnDisableCollection;
		break;

	default:
		ASSERT(FALSE);
		return STATUS_NOT_IMPLEMENTED;
	}

	// Now call the chosen function for each block instance
	ULONG BlockCount = m_Context->m_GuidInfo[m_GuidIndex].InstanceCount;

	for (i=0; i < BlockCount; i++)
	{
		if ( m_Blocks[i] != NULL )
			(m_Blocks[i]->*NotificationMember)();
	}

	return STATUS_SUCCESS;
}


template <typename T>
inline VOID PutParameter(PUCHAR& buffer, T value)
{
//	buffer = PUCHAR( (ULONG_PTR(buffer) + sizeof(T)-1) & ~ULONG_PTR(sizeof(T)-1) );
	*reinterpret_cast<T*>(buffer) = value;
	buffer += sizeof(T);
}


template <typename T>
inline VOID GetParameter(PUCHAR& buffer, T& value)
{
//	buffer = PUCHAR( (ULONG_PTR(buffer) + sizeof(T)-1) & ~ULONG_PTR(sizeof(T)-1) );
	value = *reinterpret_cast<T*>(buffer);
	buffer += sizeof(T);
}


//////////////////////////////////////////////////////////////////////////////
//  class KWmiBlock
//
//	This is an abstract base class for KWmiDataBlock<T>

template <class F = KStdWmiInterface>
class KWmiBlock
{
public:
// Data
	KWmiContext<F>*		m_Context;			// controlling context object
	ULONG				m_GuidIndex;		// index to guid in context
	ULONG				m_BlockIndex;		// index of block for its guid

// Ctors
	SAFE_DESTRUCTORS

	KWmiBlock(void);

	KWmiBlock(KWmiContext<F>*& pContext, const GUID* pGuid, ULONG Flags);

	VOID Initialize(KWmiContext<F>*& pContext, const GUID* pGuid, ULONG Flags);

// Dtor
	~KWmiBlock(void);

	VOID Invalidate(void);

// Test state
	BOOLEAN IsValid(void)
	{
		return	( 	(m_Context!=NULL) &&
					(m_GuidIndex!=ULONG(-1)) &&
					(m_BlockIndex!=ULONG(-1))
				);
	}

	BOOLEAN TestEventsEnabled(void)
		{ return m_Context->m_Dispatchers[m_GuidIndex]->m_EventsEnabled; }
	BOOLEAN TestCollectionEnabled(void)
		{ return m_Context->m_Dispatchers[m_GuidIndex]->m_CollectionEnabled; }

// These pure virtuals are overridden by the derived templates. These are typically
// specialized if used.
	virtual NTSTATUS SetItem(PIRP pIrp, ULONG ItemId, ULONG BufferSize, PUCHAR ItemBuffer)=0;

	virtual VOID OnEnableEvents(void)=0;
	virtual VOID OnDisableEvents(void)=0;
	virtual VOID OnEnableCollection(void)=0;
	virtual VOID OnDisableCollection(void)=0;

// These pure virtuals should not require specialization.
	virtual NTSTATUS _query(PIRP Irp, ULONG BufferSize, PUCHAR Buffer, PULONG pBufferUsed, BOOLEAN Last)=0;
	virtual NTSTATUS _set(PIRP Irp, ULONG BufferSize, PUCHAR Buffer)=0;
	virtual
	NTSTATUS _execute_method(
		PIRP Irp,
		ULONG MethodId,
		ULONG InBufferSize,
		ULONG OutBufferSize,
		PUCHAR Buffer,
		PULONG pBufferUsed
		)=0;

	friend KWmiContext<F>;
	friend KWmiDispatcher<F>;
};

///////////////// Implementation of class KWmiBlock ///////////////////////


///////////////////////////////////////////////////////////////////////////////
//
//	KWmiBlock::KWmiBlock
//		Default ctor.
//
//	Comments:
//		Use Initialize to initialize the block if this ctor is used.
//
template <class F>
KWmiBlock<F>::KWmiBlock(void) :
	m_Context(NULL),
	m_GuidIndex(ULONG(-1)),
	m_BlockIndex(ULONG(-1))
{
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiBlock::KWmiBlock
//		Ctor for KWmiBlock
//
//	Parameters:
//		pContext
//			Ref to pointer to the WMI context. If null, allocate it.
//
//		pGuid
//			GUID for this block.
//
//		Flags
//			Registration flags
//
//	Comments:
//		If the context pointer is NULL, this function creates a new KWmiContext.

template <class F>
KWmiBlock<F>::KWmiBlock(KWmiContext<F>*& pContext, const GUID* pGuid, ULONG Flags) :
	m_Context(NULL),
	m_GuidIndex(ULONG(-1)),
	m_BlockIndex(ULONG(-1))
{
	Initialize(pContext, pGuid, Flags);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiBlock::Initialize
//		Initialize a KWmiBlock
//
//	Parameters:
//		pContext
//			Ref to a pointer to a KWmiContext that will own the block. Pointer
//			may be NULL.
//
//		pGuid
//			GUID for this block.
//
//		Flags
//			Registration flags
//
//	Comments:
//		This function attempts to add the block to the KWmiContext indicated by
//		parameter pContext. If that pointer is NULL, then it will create a new
//		KWmiContext, and set the pointer. Once the context object is created,
//		this function calls AddBlock, which may create the dispatcher object for
//		the GUID in the context.
//
template <class F>
VOID KWmiBlock<F>::Initialize(KWmiContext<F>*& pContext, const GUID* pGuid, ULONG Flags)
{
	if (pContext == NULL)
	{
		pContext = new (NonPagedPool) KWmiContext<F>();
	}

	m_Context = pContext;

	if ( m_Context != NULL)
	{
		m_Context->AddBlock(this, pGuid, Flags, &m_GuidIndex, &m_BlockIndex);
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiBlock::~KWmiBlock
//		Dtor for KWmiBlock
//
//	Comments:
//		Destroys the block.
//
template <class F>
KWmiBlock<F>::~KWmiBlock(void)
{
	if ( IsValid() )
	{
		Invalidate();
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiBlock::Invalidate
//		Dtor for KWmiBlock
//
//	Comments:
//		Removes the block from its context. Use Initialize to resuscitate.
//
template <class F>
VOID KWmiBlock<F>::Invalidate(void)
{
	if ( IsValid() )
	{
		m_Context->RemoveBlock(this);
		m_Context = NULL;
		m_GuidIndex = m_BlockIndex = ULONG(-1);
	}
}

//////////////////////////////////////////////////////////////////////////
// class _none_
//
// This class is the default value for the second parameter to template
// KWmiDataBlock. Use the default if your block has no methods.
//
class _none_
{
};

#endif // __KWMI__
#endif // _WDM_
