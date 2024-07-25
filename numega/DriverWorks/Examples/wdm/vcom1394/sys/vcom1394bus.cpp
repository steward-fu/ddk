// VCom1394Bus.cpp
// Implementation of VCom1394Bus device class
//
// Requires Compuware's DriverWorks classes
//

#pragma warning(disable:4065) // Allow switch statement with no cases
          
#include <vdw.h>
#include <initguid.h>
#include "..\VCom1394BusInterface.h"
#include "..\VCom1394PdoInterface.h"
#include "..\VCom1394Common.h"
#include "VCom1394Driver.h"
#include "VCom1394Bus.h"
#include "VCom1394Pdo.h"
#include "stdio.h"

#pragma hdrstop("VCom1394.pch")

// Global PDO instance value, used for uniquely naming PDOs created by our
// bus driver
ULONG   g_PdoInstance = 0;

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::VCom1394Bus
//
//  Routine Description:
//      This is the constructor for the Functional Device Object, or FDO.
//      It is derived from KPnpDevice, which builds in automatic
//      dispatching of subfunctions of IRP_MJ_POWER and IRP_MJ_PNP to
//      virtual member functions.
//
//  Parameters:
//      Pdo - Physical Device Object - this is a pointer to a system
//          device object that represents the physical device.
//
//      Unit - Unit number. This is a number to append to the device's
//          base device name to form the Logical Device Object's name
//
//  Return Value:
//      None   
//
//  Comments:
//      The object being constructed contains a data member (m_Lower) of type
//      KPnpLowerDevice. By initializing it, the driver binds the FDO to the
//      PDO
//

VCom1394Bus::VCom1394Bus(PDEVICE_OBJECT Pdo, ULONG Unit) :
    KPnpDevice(Pdo, (CONST LPGUID)&GUID_DEVINTERFACE_VCOM1394BUS)
{
    NTSTATUS            status;
    ULONG               nameLength = 0;
    KRegistryKey        reg;
    OBJECT_ATTRIBUTES   objectAttributes;

    t << "Entering VCom1394Bus::VCom1394Bus (constructor)\n";


    // Check constructor status
    if ( ! NT_SUCCESS(m_ConstructorStatus) )
    {
        return;
    }

    // Determine whether or not we are on XP.  If so, then our device is a virtual device.
    // Otherwise we will have to send asynchronous requests to the 1394 port driver in order
    // to target nodes of interest.
    if(IoIsWdmVersionAvailable(1, 0x20)) 
    {
        m_bIsVirtual = TRUE;
    } else {
        m_bIsVirtual = FALSE;
    }

    // Remember our unit number
    m_Unit = Unit;

    m_Lower.Initialize(this, Pdo);

    // Initialize the PnP Policy settings to the "standard" policy
    SetPnpPolicy();

    // Inform the base class of the lower edge device object
    SetLowerDevice(&m_Lower);

    // Initialize our async clients
    m_IdRegisterClient.Initialize(&m_Lower);

    m_PortInfoClient.Initialize(&m_Lower);

    // setup our address offsets for the address ranges to be allocated
    m_IdRegisterOffset.Off_High = INITIAL_REGISTER_SPACE_HI;
    m_IdRegisterOffset.Off_Low  = VCOM1394_SPACE_OFFSET_LO | 
                                            FIELD_OFFSET(VCOM1394_NODE_REGISTERS, Key);

    m_PortInfoOffset.Off_High = INITIAL_REGISTER_SPACE_HI;
    m_PortInfoOffset.Off_Low = VCOM1394_SPACE_OFFSET_LO | 
                                            FIELD_OFFSET(VCOM1394_NODE_REGISTERS, NumberOfPorts);

    // Don't let the library handle IRP_MN_QUERY_PNP_DEVICE_STATE
    // we need that to make the bus device hidden in DeviceManager
    m_Policies.m_CompletedByDriverWorks.m_QueryPnpState = FALSE;
    m_Policies.m_ForwardToPdoPolicy.m_CallAfterQueryPnpState = FALSE;

    // Initialize the Power Policy settings to the "standard" policy
    SetPowerPolicy();

	// inform the bus about its parent
	m_VComBus.SetParent(this);

    // Initialize the number of ports
    m_NodeRegisters.NumberOfPorts = 0;

    // Get the machine name    
    status = reg.Initialize(REGISTRY_CONTROL, L"ComputerName\\ComputerName");

    m_MachineName = NULL;

    if(NT_SUCCESS(status))
    {
        reg.QueryValue(L"ComputerName", 
	                   m_MachineName, 
	                   nameLength);
    }

    KeInitializeEvent(&m_PacketCleanupTerminationEvent, SynchronizationEvent, FALSE); 

    // setup the object attributes for our driver created thread
    InitializeObjectAttributes(&objectAttributes, 
                               NULL, 
                               OBJ_KERNEL_HANDLE, 
                               NULL, 
                               NULL);

    // create our thread to handle 1394 response packet cleanup
    status = PsCreateSystemThread(
                            &m_hPacketCleanupThread,
                            0L,
                            &objectAttributes,
                            NULL,
                            NULL,
                            PacketCleanupThread,
                            this);
    if(!NT_SUCCESS(status))
    {
        m_ConstructorStatus = status;
    }
    else
    {
        // Take out a reference on the thread so we can get the object pointer
        // We need this so we can wait on the thread termination in the destructor
        ObReferenceObjectByHandle(
            m_hPacketCleanupThread,
            THREAD_ALL_ACCESS,
            NULL,
            KernelMode,
            (PVOID *)&m_PacketCleanupThreadObject,
            NULL);
    }

    // initialize our used packet list
    InitializeListHead(&m_ResponsePacketList);

    // initialize our used packet list spinlock
    KeInitializeSpinLock(&m_ResponsePacketListLock);

    // initialize MDL pointers
    m_pPortInfoMdl = NULL;
    m_pIdRegistersMdl = NULL;

    // initialize our response context lookaside list
    m_ResponsePacketLookaside.Initialize(0, DefaultPoolTag);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::~VCom1394Bus
//
//  Routine Description:
//      This is the destructor for the Functional Device Object, or FDO.
//
//  Parameters:
//      None
//
//  Return Value:
//      None
//
//  Comments:
//      None
//
VCom1394Bus::~VCom1394Bus()
{
    t << "Entering VCom1394Bus::~VCom1394Bus() (destructor)\n";

    if(m_MachineName != NULL) {
        delete [] m_MachineName;
        m_MachineName = NULL;
    }

    // Let the packet cleanup thread know it's time to go away
    KeSetEvent(&m_PacketCleanupTerminationEvent,
               IO_NO_INCREMENT,
               FALSE);

    // Make sure our thread terminates
    KeWaitForSingleObject(m_PacketCleanupThreadObject,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    // dereference the thread object so it can go away
    ObDereferenceObject(m_PacketCleanupThreadObject);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::PacketCleanupThread
//
//  Routine Description:
//      Thread routine used to cleanup 1394 async transfer response 
//      packet related structures.  The bus driver uses an allocated 
//      address range with no backing store, requiring that the bus
//      driver handle its own response packets for the range.  
//
//  Parameters:
//      pContext - Pointer to the bus driver object instance.
//
//  Return Value:
//      None   
//
//  Comments:
//

VOID VCom1394Bus::PacketCleanupThread(PVOID pContext)
{
    VCom1394Bus        *pBus = (VCom1394Bus *)pContext;
    LARGE_INTEGER       interval;
    LARGE_INTEGER       timeOut;
    NTSTATUS            status = STATUS_TIMEOUT;
    KIRQL               oldIrql;
    PLIST_ENTRY         pEntry;
    PLIST_ENTRY         pNextEntry;
    PVCOM1394_RESPONSE  pResponseContext;

    t << "VCom1394Bus::PacketCleanupThread\n";

    // Delay between cycles of this thread, we use one second arbitrarily
    interval.QuadPart = -10000 * 1;

    // specify a zero timeout, so events are just checked
    // not waited on
    timeOut.QuadPart = 0;
    

    while(status != STATUS_SUCCESS)
    {
        // Any outstanding response packets?
        if( !IsListEmpty(&pBus->m_ResponsePacketList) )
        {
            // Acquire synchronization to the used packet list
            KeAcquireSpinLock(&pBus->m_ResponsePacketListLock, &oldIrql);

            // Get the first entry in the list
            pNextEntry = pBus->m_ResponsePacketList.Flink;

            // Walk our response context list looking for completed packets
            while(pNextEntry != &pBus->m_ResponsePacketList)
            {
                pEntry = pNextEntry;

                // get the next entry from the list
                pNextEntry = pNextEntry->Flink;

                // Get the response context from the entry
                pResponseContext = CONTAINING_RECORD(pEntry, 
                                                     VCOM1394_RESPONSE, 
                                                     ListEntry);
                if( NULL == pResponseContext )
                {
                    // get the next entry from the list
                    pEntry = pEntry->Flink;
                    continue;
                }

                // see if the event for this context has been signalled
                status = KeWaitForSingleObject(
                                    &pResponseContext->ResponseEvent,
                                    Executive,
                                    KernelMode,
                                    FALSE,
                                    &timeOut);
                if(STATUS_SUCCESS == status)
                {
                    // Remove this entry from the list
                    RemoveEntryList(pEntry);

                    // event was signalled, release the memory
                    pBus->m_ResponsePacketLookaside.free((PVOID)pResponseContext);
                }

                continue;
            }

            // Release synchronization to the used packet list
            KeReleaseSpinLock(&pBus->m_ResponsePacketListLock, oldIrql);
        }

        // Check if the thread has been terminated
        status = KeWaitForSingleObject(
                            &pBus->m_PacketCleanupTerminationEvent,
                            Executive,
                            KernelMode,
                            FALSE,
                            &interval);
    }

    // Exit the thread
    PsTerminateSystemThread(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  PNPMinorFunctionName
//
//  Routine Description:
//      Return a string describing the Plug and Play minor function 
//
//  Parameters:
//      mn - Minor function code
//
//  Return Value:
//      char * - Ascii name of minor function
//
//  Comments:
//      This function is used for tracing the IRPs.  Remove the function,
//      or conditionalize it for debug-only builds, if you want to save
//      space in the driver image.
//
    
char *PNPMinorFunctionName(ULONG mn)
{
    static char* minors[] = {
        "IRP_MN_START_DEVICE",
        "IRP_MN_QUERY_REMOVE_DEVICE",
        "IRP_MN_REMOVE_DEVICE",
        "IRP_MN_CANCEL_REMOVE_DEVICE",
        "IRP_MN_STOP_DEVICE",
        "IRP_MN_QUERY_STOP_DEVICE",
        "IRP_MN_CANCEL_STOP_DEVICE",
        "IRP_MN_QUERY_DEVICE_RELATIONS",
        "IRP_MN_QUERY_INTERFACE",
        "IRP_MN_QUERY_CAPABILITIES",
        "IRP_MN_QUERY_RESOURCES",
        "IRP_MN_QUERY_RESOURCE_REQUIREMENTS",
        "IRP_MN_QUERY_DEVICE_TEXT",
        "IRP_MN_FILTER_RESOURCE_REQUIREMENTS",
        "<unknown minor function>",
        "IRP_MN_READ_CONFIG",
        "IRP_MN_WRITE_CONFIG",
        "IRP_MN_EJECT",
        "IRP_MN_SET_LOCK",
        "IRP_MN_QUERY_ID",
        "IRP_MN_QUERY_PNP_DEVICE_STATE",
        "IRP_MN_QUERY_BUS_INFORMATION",
        "IRP_MN_DEVICE_USAGE_NOTIFICATION",
        "IRP_MN_SURPRISE_REMOVAL",
        "IRP_MN_QUERY_LEGACY_BUS_INFORMATION"
    };

    if (mn > 0x18) // IRP_MN_QUERY_LEGACY_BUS_INFORMATION
        return "<unknown minor function>";
    else
        return minors[mn];
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::DefaultPnp
//
//  Routine Description:
//      Default handler for IRP_MJ_PNP
//
//  Parameters:
//      I - Current PnP IRP
//
//  Return Value:
//      NTSTATUS - Result returned from lower device
//
//  Comments:
//      This routine just passes the IRP through to the lower device. It is 
//      the default handler for IRP_MJ_PNP. IRPs that correspond to
//      any virtual members of KpnpDevice that handle minor functions of
//      IRP_MJ_PNP and that are not overridden get passed to this routine.
//

NTSTATUS VCom1394Bus::DefaultPnp(KIrp I) 
{
    t << "Entering VCom1394Bus::DefaultPnp with IRP minor function="
      << PNPMinorFunctionName(I.MinorFunction()) << EOL;

    I.ForceReuseOfCurrentStackLocationInCalldown();
    return m_Lower.PnpCall(this, I);
}


////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::DefaultPower
//
//  Routine Description:
//      Default handler for IRP_MJ_POWER 
//
//  Parameters:
//      I - Current IRP
//
//  Return Value:
//      NTSTATUS - Result returned from lower device
//
//  Comments:
//      This routine just passes the IRP through to the lower device. It is 
//      the default handler for IRP_MJ_POWER.
//

NTSTATUS VCom1394Bus::DefaultPower(KIrp I) 
{
    t << "Entering VCom1394Bus::DefaultPower\n";

    I.IndicatePowerIrpProcessed();
    I.CopyParametersDown();
    return m_Lower.PnpPowerCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::SystemControl
//
//  Routine Description:
//      Default handler for IRP_MJ_SYSTEM_CONTROL
//
//  Parameters:
//      I - Current IRP
//
//  Return Value:
//      NTSTATUS - Result returned from lower device
//
//  Comments:
//      This routine just passes the IRP through to the next device since this driver
//      is not a WMI provider.
//

NTSTATUS VCom1394Bus::SystemControl(KIrp I) 
{
    t << "Entering VCom1394Bus::SystemControl\n";

    I.ForceReuseOfCurrentStackLocationInCalldown();
    return m_Lower.PnpCall(this, I);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::OnStartDevice
//
//  Routine Description:
//      Handler for IRP_MJ_PNP subfcn IRP_MN_START_DEVICE
//
//  Parameters:
//      I - Current IRP
//
//  Return Value:
//      NTSTATUS - Result code
//
//  Comments:
//      Initialize the 1394 device.
//

NTSTATUS VCom1394Bus::OnStartDevice(KIrp I)
{
    NTSTATUS        status = STATUS_SUCCESS;
    PCWSTR          deviceName = new WCHAR[500];
    ULONG           pdoInstance;
    KRegistryKey    reg;
    ULONG           numberOfPorts;

    t << "Entering VCom1394Bus::OnStartDevice\n";

    I.Information() = 0;

    // The base class will handle completion

	// we will create our comport devices here.

    // Look in the registry to see how many virtual ports to create
    status = reg.Initialize(PDO(), PLUGPLAY_REGKEY_DRIVER, KEY_ALL_ACCESS);

    if (!NT_SUCCESS(status)) 
    {
        t << "  Failed to initialize reg " << status << " " << m_pDeviceObject << EOL;
    }

    status = reg.QueryValue(
                        L"NumberOfPorts", 
	                    &numberOfPorts
                        );

    if (!NT_SUCCESS(status)) 
    {
        t << "  Failed to get number of COM ports to create, defaulting to one\n";

        numberOfPorts = 1;
    }
    

    for(ULONG ii = 0; ii < numberOfPorts; ii++)
    {
        pdoInstance = g_PdoInstance++;

        // First setup the device name, we need to name the devices so we
        // can create a symbolic link
        swprintf((PWCHAR)deviceName, L"VCOM1394%d\0", pdoInstance);

	    VCom1394Pdo* pCom = new (
		    deviceName,
		    FILE_DEVICE_SERIAL_PORT,
		    NULL,
		    FILE_DEVICE_SECURE_OPEN,
		    DO_BUFFERED_IO | DO_POWER_PAGABLE
		    )

	    VCom1394Pdo(this, &m_VComBus, pdoInstance);

	    if ( pCom )
	    {
		    status = pCom->ConstructorStatus();

		    if ( NT_SUCCESS(status) )
		    {
			    m_VComBus.AddChild(pCom);
			    ((PDEVICE_OBJECT)*pCom)->Flags &= ~DO_DEVICE_INITIALIZING;
			    IoInvalidateDeviceRelations(m_Lower.PDO(), BusRelations);
		    }
		    else
            {
			    delete pCom;
            }
	    }
    }

    delete (PVOID)deviceName;

    // set the parent pointer for our async clients
    m_IdRegisterClient.SetParent(this);
    m_PortInfoClient.SetParent(this);

    status = SetupNodeRegisters();

    return status;
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::OnStopDevice
//
//  Routine Description:
//      Handler for IRP_MJ_PNP subfcn IRP_MN_STOP_DEVICE
//
//  Parameters:
//      I - Current IRP
//
//  Return Value:
//      NTSTATUS - Result code
//
//  Comments:
//      The system calls this when the device is stopped.
//      The driver should release any hardware resources
//      in this routine.
//
//      The base class passes the irp to the lower device.
//

NTSTATUS VCom1394Bus::OnStopDevice(KIrp I)
{
    NTSTATUS status = STATUS_SUCCESS;

    t << "Entering VCom1394Bus::OnStopDevice\n";

    // Cleanup up our 1394 allocated address ranges
    if(m_IdRegisterRange.Count() != 0)
    {
        m_IdRegisterClient.Free(m_IdRegisterRange);
    }

    if(m_PortInfoRange.Count() != 0)
    {
        m_PortInfoClient.Free(m_PortInfoRange);
    }

    if(NULL != m_pIdRegistersMdl)
    {
        IoFreeMdl(m_pIdRegistersMdl);

        m_pIdRegistersMdl = NULL;
    }

    if(NULL != m_pPortInfoMdl)
    {
        IoFreeMdl(m_pPortInfoMdl);

        m_pPortInfoMdl = NULL;
    }
    
    return status;
    
    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::OnRemoveDevice
//
//  Routine Description:
//      Handler for IRP_MJ_PNP subfcn IRP_MN_REMOVE_DEVICE
//
//  Parameters:
//      I - Current IRP
//
//  Return Value:
//      NTSTATUS - Result code
//
//  Comments:
//      The system calls this when the device is removed.
//      Our PnP policy will take care of 
//          (1) giving the IRP to the lower device
//          (2) detaching the PDO
//          (3) deleting the device object
//

NTSTATUS VCom1394Bus::OnRemoveDevice(KIrp I)
{
    t << "Entering VCom1394Bus::OnRemoveDevice\n";

	// make sure that all children objects are deleted
	m_VComBus.RemoveChildren();

    // Cleanup up our 1394 allocated address ranges
    if(m_IdRegisterRange.Count() != 0)
    {
        m_IdRegisterClient.Free(m_IdRegisterRange);
    }

    if(m_PortInfoRange.Count() != 0)
    {
        m_PortInfoClient.Free(m_PortInfoRange);
    }

    if(NULL != m_pIdRegistersMdl)
    {
        IoFreeMdl(m_pIdRegistersMdl);

        m_pIdRegistersMdl = NULL;
    }

    if(NULL != m_pPortInfoMdl)
    {
        IoFreeMdl(m_pPortInfoMdl);

        m_pPortInfoMdl = NULL;
    }

    return STATUS_SUCCESS;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::OnSurpriseRemoval
//
//	Routine Description:
//		Handler for IRP_MJ_PNP subfcn IRP_MN_SURPRISE_REMOVAL
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		The system calls this when the device is removed.
//		Our PnP policy will take care of 
//			(1) giving the IRP to the lower device
//			(2) detaching the PDO
//			(3) deleting the device object
//

NTSTATUS VCom1394Bus::OnSurpriseRemoval(KIrp I)
{
	t << "Entering VCom1394Bus::OnSurpriseRemoval\n";

	// allow children objects to be deleted in IRP_MN_REMOVE_DEVICE handler
	m_VComBus.RemoveParent();

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::OnQueryDeviceState
//
//	Routine Description:
//		Handler for IRP_MJ_PNP subfcn IRP_MN_QUERY_PNP_DEVICE_STATE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		We handle this IRP on its way up the driver stack so as to 
//      hide this device in DeviceManager.  This device will now only
//      show up in the device manager if "show hidden devices" is 
//      selected.  The bus driver works behind the scenes, so there
//      is no reason to have it be visible.
//

NTSTATUS VCom1394Bus::OnQueryDeviceState(KIrp I)
{
    PPNP_DEVICE_STATE   deviceState;
    ULONG_PTR           information;
    NTSTATUS            status;

    t << "Entering VCom1394Bus::OnQueryDeviceState\n";

    // Get the device state buffer
    deviceState = (PPNP_DEVICE_STATE) &(I.Information());

    // Make sure we got a buffer
    ASSERT(deviceState != NULL);

    if (deviceState != NULL)
    {
        // Indicate that we want to hide this device
        *deviceState |= PNP_DEVICE_DONT_DISPLAY_IN_UI;
    }

    // Send the request down the stack first,
    // we handle on the way up
    I.CopyParametersDown();

    m_Lower.CallWaitComplete(I, TRUE, &information);

    // Get the status of the request
    status = I.Status();

    // If the request was successful in the rest of the stack
    // We set our information and complete the IRP
    if (NT_SUCCESS(status))
    {
        // Get the device state buffer
        deviceState = (PPNP_DEVICE_STATE) &(I.Information());

        // Make sure we got a buffer
        ASSERT(deviceState != NULL);

        if (deviceState != NULL)
        {
            // Indicate that we want to hide this device
            *deviceState |= PNP_DEVICE_DONT_DISPLAY_IN_UI;
        }
    }

    I.PnpComplete(status);

    return status;
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::OnQueryDeviceRelations
//
//	Routine Description:
//		Handler for IRP_MJ_PNP subfcn IRP_MN_QUERY_DEVICE_RELATIONS
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//      Bus drivers have to handle BusRelations requests to report 
//      the addition and removal of child devices.
//

NTSTATUS VCom1394Bus::OnQueryDeviceRelations(KIrp I)
{
    NTSTATUS    status;

	t << "Entering VCom1394Bus::OnQueryDeviceRelations\n";

    // Let the bus class handle the relations requests
	status = m_VComBus.QueryDeviceRelations(I);

	if (NT_SUCCESS(status))
    {
		return m_Lower.PnpCall(this, I);
    }
	else
    {
		return I.PnpComplete(status);
    }
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::OnDevicePowerUp
//
//  Routine Description:
//      Handler for IRP_MJ_POWER with minor function IRP_MN_SET_POWER
//      for a request to go to power on state from low power state
//
//  Parameters:
//      I - IRP containing POWER request
//
//  Return Value:
//      NTSTATUS - Status code indicating success or failure
//
//  Comments:
//      This routine implements the OnDevicePowerUp function.
//      This function was called by the framework from the completion
//      routine of the IRP_MJ_POWER dispatch handler in KPnpDevice.
//      The bus driver has completed the IRP and this driver can now
//      access the hardware device.  
//      This routine runs at dispatch level.
//  

NTSTATUS VCom1394Bus::OnDevicePowerUp(KIrp I)
{
    NTSTATUS status = STATUS_SUCCESS;

    t << "Entering VCom1394Bus::OnDevicePowerUp\n";

    // Nothing to do here, we don't do anything for power in this device
    return status;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove 
    // the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::OnDeviceSleep
//
//  Routine Description:
//      Handler for IRP_MJ_POWER with minor function IRP_MN_SET_POWER
//      for a request to go to a low power state from a high power state
//
//  Parameters:
//      I - IRP containing POWER request
//
//  Return Value:
//      NTSTATUS - Status code indicating success or failure
//
//  Comments:
//      This routine implements the OnDeviceSleep function.
//      This function was called by the framework from the IRP_MJ_POWER 
//      dispatch handler in KPnpDevice prior to forwarding to the PDO.
//      The hardware has yet to be powered down and this driver can now
//      access the hardware device.  
//      This routine runs at passive level.
//  

NTSTATUS VCom1394Bus::OnDeviceSleep(KIrp I)
{
    NTSTATUS status = STATUS_SUCCESS;

    t << "Entering VCom1394Bus::OnDeviceSleep\n";

    // Nothing to do here, this device doesn't do 
    // anything special for sleep
    return status;

    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(I);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::Create
//
//  Routine Description:
//      Handler for IRP_MJ_CREATE
//
//  Parameters:
//      I - Current IRP
//
//  Return Value:
//      NTSTATUS - Result code
//
//  Comments:
//

NTSTATUS VCom1394Bus::Create(KIrp I)
{
    NTSTATUS status;

    t << "Entering VCom1394Bus::Create\n";

    // Just complete the IRP
    status = I.PnpComplete(this, STATUS_SUCCESS, IO_NO_INCREMENT);
    
    t << "VCom1394Bus::Create Status " << (ULONG)status << EOL;

    return status;
}


////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::Close
//
//  Routine Description:
//      Handler for IRP_MJ_CLOSE
//
//  Parameters:
//      I - Current IRP
//
//  Return Value:
//      NTSTATUS - Result code
//
//  Comments:
//

NTSTATUS VCom1394Bus::Close(KIrp I)
{
    NTSTATUS status;

    t << "Entering VCom1394Bus::Close, " << I << EOL;

    // Just complete the request
    status = I.PnpComplete(this, STATUS_SUCCESS, IO_NO_INCREMENT);

    t << "VCom1394Bus::Close Status " << (ULONG)status << EOL;

    return status;
}


////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::Read
//
//  Routine Description:
//      Handler for IRP_MJ_READ
//
//  Parameters:
//      I - Current IRP
//
//  Return Value:
//      NTSTATUS - Result code
//
//  Comments:
//

NTSTATUS VCom1394Bus::Read(KIrp I)
{
    NTSTATUS status;

    t << "Entering VCom1394Bus::Read\n";

    // Just complete the request
    status = I.PnpComplete(this, STATUS_SUCCESS);

	return status;
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::Write
//
//  Routine Description:
//      Handler for IRP_MJ_WRITE
//
//  Parameters:
//      I - Current IRP
//
//  Return Value:
//      NTSTATUS - Result code
//
//  Comments:
//

NTSTATUS VCom1394Bus::Write(KIrp I)
{
    NTSTATUS status;

	t << "Entering VCom1394Bus::Write\n";

    // Just complete the request
    status = I.PnpComplete(this, STATUS_SUCCESS);

	return status;
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::DeviceControl
//
//  Routine Description:
//      Handler for IRP_MJ_DEVICE_CONTROL
//
//  Parameters:
//      I - Current IRP
// 
//  Return Value:
//      None
//
//  Comments:
//      This routine is the first handler for Device Control requests.
//      The KPnpDevice class handles restricting IRP flow
//      if the device is stopping or being removed.
//

NTSTATUS VCom1394Bus::DeviceControl(KIrp I) 
{
    NTSTATUS status;

    t << "Entering VCom1394Bus::Device Control\n";

    I.Information() = 0;

    switch (I.IoctlCode())
    {
        case VCOM1394BUS_IOCTL_GET_PORTS:
            status = VCom1394IoctlGetPortsHandler(I);
            break;

        default:
            // Unrecognized IOCTL request
            status = STATUS_INVALID_PARAMETER;
            break;
    }

    // If the IRP's IOCTL handler deferred processing using some driver
    // specific scheme, the status variable is set to STATUS_PENDING.
    // In this case we simply return that status, and the IRP will be
    // completed later.  Otherwise, complete the IRP using the status
    // returned by the IOCTL handler.
    if (status == STATUS_PENDING)
    {
        return status;
    }
    else
    {
        return I.PnpComplete(this, status);
    }
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::VCom1394IoctlGetPortsHandler
//
//  Routine Description:
//      Handler for IO Control Code VCOM1394BUS_IOCTL_GET_PORTS
//
//  Parameters:
//      I - IRP containing IOCTL request
//
//  Return Value:
//      NTSTATUS - Status code indicating success or failure
//
//  Comments:
//      This routine implements the VCOM1394BUS_IOCTL_GET_PORTS function.
//      This routine runs at passive level.
//

NTSTATUS VCom1394Bus::VCom1394IoctlGetPortsHandler(KIrp I)
{
    NTSTATUS                    status = STATUS_SUCCESS;
    PVCOM1394_PORT_INFORMATION  pUserBuffer;


    t << "Entering VCom1394Bus::VCom1394IoctlGetPortsHandler, " << I << EOL;

    if(I.IoctlOutputBufferSize() < sizeof(VCOM1394_PORT_INFORMATION))
    {
        return STATUS_BUFFER_TOO_SMALL;
    }

    pUserBuffer = (PVCOM1394_PORT_INFORMATION)I.IoctlBuffer();
    
    pUserBuffer->MaximumNumberOfPorts = MAXIMUM_NUMBER_OF_PORTS;

    pUserBuffer->MinimumNumberOfPorts = MINIMUM_NUMBER_OF_PORTS;

    pUserBuffer->CurrentNumberOfPorts = m_NodeRegisters.NumberOfPorts;

    I.Information() = sizeof(VCOM1394_PORT_INFORMATION);

    return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::SetupNodeRegisters
//
//  Routine Description:
//      Sets up registers in 1394 address space that allow other 
//      nodes to query this node for VCOM port information
//
//  Parameters:
//
//  Return Value:
//      NTSTATUS - Status code indicating success or failure
//
//  Comments:
//      This routine implements the VCOM1394BUS_IOCTL_GET_PORTS function.
//      This routine runs at passive level.
//

NTSTATUS VCom1394Bus::SetupNodeRegisters()
{
    NTSTATUS        status = STATUS_SUCCESS;
    LARGE_INTEGER   hostID;
    ULONG           nameLength = 0;

    t << "Entering VCom1394Pdo::SetupNodeRegisters \n";

    RtlZeroMemory(&m_NodeRegisters, sizeof(VCOM1394_NODE_REGISTERS));

    // setup the key registers
    m_NodeRegisters.Key[0] = 0x4D4F4356;  // ASCII "VCOM 2 1394" 
    m_NodeRegisters.Key[1] = 0x31203220;  
    m_NodeRegisters.Key[2] = 0x00343933;  

    // get our host controller's ID
    hostID = m_Lower.HostUniqueID();

    // Save the machine name in our registers
    if(m_MachineName != NULL)
    {
        RtlCopyMemory(&m_NodeRegisters.MachineName, m_MachineName, wcslen(m_MachineName) * sizeof(WCHAR));
    }

    // setup the MDL for the read only registers
    m_pIdRegistersMdl = IoAllocateMdl(&m_NodeRegisters.Key, 
                                      sizeof(ULONG) * 36, 
                                      FALSE, 
                                      FALSE, 
                                      NULL);
	if(NULL == m_pIdRegistersMdl)
	{
        t << "VCom1394Bus::SetupNodeRegisters:  Failed to create storage buffer MDL\n";
		
        return STATUS_INSUFFICIENT_RESOURCES;
	}

    // lock the pages in memory
    MmBuildMdlForNonPagedPool(m_pIdRegistersMdl);

    // request our addresses for the read only registers
    if(m_bIsVirtual) 
    {
        status = m_IdRegisterClient.Allocate(
                        m_IdRegisterRange,
                        m_pIdRegistersMdl,
                        m_IdRegisterOffset,
                        ACCESS_FLAGS_TYPE_READ | ACCESS_FLAGS_TYPE_BROADCAST,
                        NOTIFY_FLAGS_NEVER);
    } 
    else 
    {
        status = m_IdRegisterClient.Allocate(
                        m_IdRegisterRange,
                        m_pIdRegistersMdl,
                        m_IdRegisterOffset,
                        ACCESS_FLAGS_TYPE_READ,
                        NOTIFY_FLAGS_NEVER);
    }

    if (!NT_SUCCESS(status)) 
    {
        t << "VCom1394Bus::SetupNodeRegisters:  RequestAllocateAddressRange failed - status (" << status << ")\n";
        return status;
    }

    // setup the MDL for the read only registers
    m_pPortInfoMdl = IoAllocateMdl(&m_NodeRegisters.NumberOfPorts, 
                                   (sizeof(VCOM_PORT) * MAXIMUM_NUMBER_OF_PORTS) 
                                        + sizeof(ULONG),
                                   FALSE, 
                                   FALSE, 
                                   NULL);
	if(NULL == m_pPortInfoMdl)
	{
		t << "VCom1394Bus::SetupNodeRegisters:  Failed to create storage buffer MDL\n";
		
        return STATUS_INSUFFICIENT_RESOURCES;
	}

    // lock the pages in memory
    MmBuildMdlForNonPagedPool(m_pPortInfoMdl);


    // request our addresses for the data registers
    if(m_bIsVirtual) 
    {
	    status = m_PortInfoClient.Allocate(
                        m_PortInfoRange,
                        m_pPortInfoMdl,
                        m_PortInfoOffset,
		                ACCESS_FLAGS_TYPE_READ | ACCESS_FLAGS_TYPE_BROADCAST,
                        NOTIFY_FLAGS_NEVER
                        );
    } 
    else 
    {
	    status = m_PortInfoClient.Allocate(
                        m_PortInfoRange,
		                m_pPortInfoMdl,
		                m_PortInfoOffset,
		                ACCESS_FLAGS_TYPE_READ,
                        NOTIFY_FLAGS_NEVER
                        );
    }

    if (!NT_SUCCESS(status)) 
    {
        t << "VCom1394Pdo::SetupDeviceRegisters: RequestAllocateAddressRange failed - status(" << status << ")\n";
        return status;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Bus::UpdateNodeRegisters
//
//  Routine Description:
//      Sets up registers in 1394 address space that allow other 
//      nodes to query this node for VCOM port information
//
//  Parameters:
//
//  Return Value:
//      BOOLEAN - Returns TRUE if port information was added
//
//  Comments:
//      This routine runs at passive level.
//

BOOLEAN VCom1394Bus::UpdateNodeRegisters(BOOLEAN bAddPort, PVCOM_PORT pPortInfo)
{
    KIRQL   oldIrql;

    KeAcquireSpinLock(&m_PortInfoClient.m_PortInfoLock, &oldIrql);

    if(FALSE != bAddPort)
    {
        // Make sure we can add a port
        if(m_NodeRegisters.NumberOfPorts >= MAXIMUM_NUMBER_OF_PORTS)
        {
            return FALSE;
        }

        // copy in the new info
        RtlCopyMemory(&m_NodeRegisters.PortInfo[m_NodeRegisters.NumberOfPorts], 
                      pPortInfo, 
                      sizeof(VCOM_PORT));

        // update the count
        m_NodeRegisters.NumberOfPorts++;
    }
    else
    {
        // Make sure we have port(s) to remove
        if(m_NodeRegisters.NumberOfPorts <= 0)
        {
            return FALSE;
        }

        for(ULONG ii = 0; ii < m_NodeRegisters.NumberOfPorts; ii++)
        {
            if((sizeof(ULONG) * 4) == RtlCompareMemory(pPortInfo->PortName, 
                                                       m_NodeRegisters.PortInfo[ii].PortName, 
                                                       sizeof(ULONG) * 4))
            {
                // move the list up a notch and copy over this entry
                RtlCopyMemory(&m_NodeRegisters.PortInfo[ii],
                              &m_NodeRegisters.PortInfo[ii + 1],
                              sizeof(VCOM_PORT) * (m_NodeRegisters.NumberOfPorts - (ii + 1)));

                // adjust the number of ports
                m_NodeRegisters.NumberOfPorts--;

                break;
            }
        }
    }

    KeReleaseSpinLock(&m_PortInfoClient.m_PortInfoLock, oldIrql);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////
//  BusAsyncClient::OnRead
//
//  Routine Description:
//
//  Parameters:
//
//  Return Value:
//
//  Comments:
//

VOID BusAsyncClient::OnRead(
        PVOID       RequestPacket, 
        PMDL        pResponseMdl,
        PVOID      *ppResponsePacket,
        PULONG      pResponseLength,
        PKEVENT    *ppResponseEvent
        )
{   
    KIRQL                       oldIrql;        // our current IRQL
    PVCOM1394_OHCI_ASYNC_PACKET pResponsePacket;
    PVCOM1394_OHCI_ASYNC_PACKET pRequestPacket = (PVCOM1394_OHCI_ASYNC_PACKET)RequestPacket;
    PVCOM1394_RESPONSE          pResponseContext;
    ULONG                       offset;
    ULONG                       mdlSize;
    VCom1394Bus                *pParent = (VCom1394Bus *)m_pParent;

    // Display where the request packet came from
    t << "Entering BusAsyncClient::OnRead\n";
    
    // We have allocated our address range with no backing 
    // store, so we get the request packet and we have to 
    // create the response packet.

    // Figure out the offset being accessed relative to our port info storage
    offset = pRequestPacket->OHCI_Offset_Low & ~VCOM1394_SPACE_OFFSET_LO;

    // Get a new response context 
    pResponseContext = pParent->AllocateResponseContext();

    if(pResponseContext == NULL)
    {
        // just return, the request will timeout 
        // and signal an error automatically
        return;
    }

    // Get a pointer to our new response packet
    pResponsePacket = &pResponseContext->ResponsePacket;

    // set this pointer in the NOTIFICATION_INFO struct
     *ppResponsePacket = pResponsePacket;

    // copy the request packet over to the response 
    // packet buffer and modify
    RtlCopyMemory(
        pResponsePacket, 
        pRequestPacket, 
        sizeof(VCOM1394_OHCI_ASYNC_PACKET)
        );

    // setup the response packet

    // The source ID from the request packet
    // is actually used as the destination address 
    // for the response, which is weird.  The 
    // actual source ID for the response is filled 
    // in by the 1394 bus driver

    // calculate the speed for the response packet
    pResponsePacket->u.Tx.OHCI_spd = GetRequestPacketSpeed(pRequestPacket);

    // set a success response code
    pResponsePacket->u2.Response.OHCI_Rcode = RCODE_RESPONSE_COMPLETE;

    // Acquire synchronization to our port info register data
    KeAcquireSpinLock(&m_PortInfoLock, &oldIrql);

    mdlSize = sizeof(VCOM1394_OHCI_ASYNC_PACKET);

    // check for quadlet or block request.
    if(pRequestPacket->OHCI_tCode == TCODE_READ_REQUEST_QUADLET) 
    {
        // copy the quadlet data to the response packet
        RtlCopyMemory(
            (PCHAR)&pResponsePacket->u3.OHCI_Quadlet_Data,
            (PCHAR)&pParent->m_NodeRegisters + offset,
            sizeof(ULONG)
            );

        // indicate this is a read response packet
        pResponsePacket->OHCI_tCode = TCODE_READ_RESPONSE_QUADLET;
    }
    else 
    {
        mdlSize += pRequestPacket->u3.Block.OHCI_Data_Length;

        // copy the block data to the response packet
        RtlCopyMemory(
            (PCHAR)pResponsePacket + sizeof(VCOM1394_OHCI_ASYNC_PACKET),
            (PCHAR)&pParent->m_NodeRegisters + offset,
            pRequestPacket->u3.Block.OHCI_Data_Length
            );

        if(offset == 0)
            DbgPrint("Number Of Ports being returned = 0x%X\n" ,*(PULONG)((PCHAR)pResponsePacket + sizeof(VCOM1394_OHCI_ASYNC_PACKET)));

        // indicate this is a read response packet
        pResponsePacket->OHCI_tCode = TCODE_READ_RESPONSE_BLOCK;
    }

    // Release synchronization to our port info register data
    KeReleaseSpinLock(&m_PortInfoLock, oldIrql);

    // initialize the info struct's response MDL
    // with our response buffer
    MmInitializeMdl(pResponseMdl, pResponsePacket, mdlSize);

    // map the pages
    MmBuildMdlForNonPagedPool(pResponseMdl);

    // set the response length
    *pResponseLength = MmGetMdlByteCount(pResponseMdl);

    // initialize our event
    KeInitializeEvent(
        &pResponseContext->ResponseEvent,
        NotificationEvent, 
        FALSE
        );

    // set our event as the response event in the info struct
    *ppResponseEvent = &pResponseContext->ResponseEvent;

    // put the response context entry on the used list
    ExInterlockedInsertHeadList(
        &pParent->m_ResponsePacketList,
        &pResponseContext->ListEntry,
        &pParent->m_ResponsePacketListLock
        );
}
