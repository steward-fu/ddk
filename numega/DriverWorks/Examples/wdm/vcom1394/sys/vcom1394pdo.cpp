// VCom1394Pdo.cpp
// Implementation of VCom1394Pdo device class
//
// Requires Compuware's DriverWorks classes
//

#include <vdw.h>
#include <initguid.h>
#include "..\VCom1394BusInterface.h"
#include "..\VCom1394PdoInterface.h"
#include "..\VCom1394Common.h"
#include "VCom1394Driver.h"
#include "VCom1394Bus.h"
#include "VCom1394Pdo.h"
#include "stdio.h"

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::VCom1394Pdo
//
//	Routine Description:
//		Constructor for VCom1394Pdo class
//
//	Parameters:
//		[in] pParent
//			pointer to a parent FDO
//
//		[in] pBus
//			pointer to a pdo collection
//
//		[in] Instance
//			instance value
//
//	Return Value:
//		None   
//
//	Comments:
//		None
//

VCom1394Pdo::VCom1394Pdo(VCom1394Bus* pParent, KBus* pBus, ULONG Instance) :
	KVirtualComPdo(pBus),
	m_pParent(pParent),
	m_Instance(Instance),
	m_HardwareIDs(NULL),
	m_DeviceIDs(NULL)
{
    NTSTATUS        status;
    ULONG           nameLength = 0;
    KRegistryKey    reg;

	t << "Entering VCom1394Pdo::VCom1394Pdo\n";

	// Check constructor status
    if (NT_SUCCESS(m_ConstructorStatus))
	{
		SetPnpPolicy();

		m_Policies.m_HoldPolicy.m_HoldIfRemovePending = FALSE;
		m_Policies.m_HoldPolicy.m_HoldIfStopPending = FALSE;
		m_Policies.m_HoldPolicy.m_HoldIfStopped = FALSE;

		SetPowerPolicy();

		m_HardwareIDs = new WCHAR[sizeof(COMPORT_IDS)/sizeof(WCHAR)];
		if ( m_HardwareIDs )
			RtlCopyMemory(m_HardwareIDs, COMPORT_IDS, sizeof (COMPORT_IDS));
		else
			m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;

		m_DeviceIDs = new WCHAR[sizeof(COMPORT_IDS)/sizeof(WCHAR)];
		if ( m_DeviceIDs )
			RtlCopyMemory(m_DeviceIDs, COMPORT_IDS, sizeof (COMPORT_IDS));
		else
        {
			m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
            return;
        }
	}

    // Determine whether or not we are on XP.  If so, then our device is a virtual device.
    // Otherwise we will have to send asynchronous requests to the 1394 port driver in order
    // to target nodes of interest.
    if(IoIsWdmVersionAvailable(1, 0x20)) {
        m_bIsVirtual = TRUE;
    } else {
        m_bIsVirtual = FALSE;
    }

    m_ConnectionInfoClient.Initialize(&pParent->m_Lower);

    m_DataRegisterClient.Initialize(&pParent->m_Lower);

    m_bOpenHandle = FALSE;

    // Get the machine name    
    status = reg.Initialize(REGISTRY_CONTROL, L"ComputerName\\ComputerName");

    if(NT_SUCCESS(status))
    {
        reg.QueryValue(L"ComputerName", 
	                   m_MachineName, 
	                   nameLength);
    }

    // initialize our linked list for the response context
    ExInitializeSListHead(&m_ResponseContextList);

    // initialize the SpinLock for the linked list
    KeInitializeSpinLock(&m_ResponseContextListLock);

    RtlInitUnicodeString(&m_InterfaceReferenceString, L"PrivateInterface");

    // initialize MDL pointer
    m_pConnectionInfoMdl = NULL;

    // initialize state to show not connected
    m_bConnected = FALSE;

    // set the parent pointer for our async clients
    m_ConnectionInfoClient.SetParent(this);
    m_DataRegisterClient.SetParent(this);

    // initialize reg name string pointer
    m_RegistryName = NULL;

    // initialize connection flags
    m_bContactedTarget = FALSE;

    // initialize the total transfer amount
    m_TotalTransferred = 0;
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::~VCom1394Pdo
//
//	Routine Description:
//		Destructor for VCom1394Pdo class
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//

VCom1394Pdo::~VCom1394Pdo()
{
	t << "Entering VCom1394Pdo::~VCom1394Pdo\n";

	if (m_HardwareIDs)
    {
		delete[] m_HardwareIDs;
    }

	if (m_DeviceIDs)
    {
		delete[] m_DeviceIDs;
    }

    // Remove our symbolic link if still valid
    if(m_LinkName != NULL) 
    {
        DestroyLink(m_LinkName);
        delete [] m_LinkName;
        m_LinkName = NULL;
    }

    if(m_MachineName != NULL) 
    {
        delete [] m_MachineName;
        m_MachineName = NULL;
    }

    RtlFreeUnicodeString(&m_InterfaceSymbolicLinkName);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::OnQueryCapabilities
//
//	Routine Description:
//		IRP_MN_QUERY_CAPABILITIES handler.  Allows our bus 
//      driver to create PDOs in Raw-mode.
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - status code
//
//	Comments:
//		None
//

NTSTATUS VCom1394Pdo::OnQueryCapabilities(KIrp I)
{
	t << "Entering VCom1394Pdo::OnQueryCapabilities\n";

    // Set the raw device flag for this PDO
    I.DeviceCapabilities()->RawDeviceOK = TRUE;

    I.PnpComplete(this, STATUS_SUCCESS);

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::OnQueryDeviceText
//
//	Routine Description:
//		IRP_MN_QUERY_DEVICE_TEXT handler.  This routine sets the 
//      device description text (available in device manager) for 
//      this PDO device.  The text is the description used until a 
//      a driver is loaded for the PDO.
//
//	Parameters:
//		[in] I
//			IRP_MJ_PNP IRP
//
//	Return Value:
//		NTSTATUS - Status code
//
//	Comments:
//		None
//

NTSTATUS VCom1394Pdo::OnQueryDeviceText(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering VCom1394Pdo::OnQueryDeviceText\n";

	switch (I.CurrentStackLocation()->Parameters.QueryDeviceText.DeviceTextType)
	{
	case DeviceTextDescription:
		switch (I.CurrentStackLocation()->Parameters.QueryDeviceText.LocaleId) 
		{
        case 0x00000409: // English
        default:
            if (!I.Information()) 
			{
                ULONG length = (ULONG)(wcslen(VENDORNAME) + wcslen(MODELNAME) + 8 + 2)*sizeof(WCHAR);

                PWCHAR pBuffer = (PWCHAR)ExAllocatePoolWithTag(PagedPool, length, DefaultPoolTag);
                if (pBuffer != NULL) 
				{
					swprintf(pBuffer, L"%ws%ws%08x", VENDORNAME, MODELNAME, m_Instance);
					I.Information() = (ULONG_PTR)pBuffer;
				}
				else
				{
                    status = STATUS_INSUFFICIENT_RESOURCES;
                }
            }
            break;           
		}
		break;
	case DeviceTextLocationInformation:
		break;
	default:
		status = I.Status();
		break;
	}

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::OnQueryId
//
//	Routine Description:
//		IRP_MN_QUERY_ID handler.
//
//	Parameters:
//		[in] I
//			IRP_MJ_PNP IRP
//
//	Return Value:
//		status
//
//	Comments:
//		None
//

NTSTATUS VCom1394Pdo::OnQueryId(KIrp I)
{
	NTSTATUS	status = STATUS_SUCCESS;
    PWCHAR		pBuffer;
    ULONG		length;

	t << "Entering VCom1394Pdo::OnQueryId\n";

    switch ( I.QueryIdType() )
	{
	case BusQueryDeviceID:
		pBuffer = m_DeviceIDs;

        while ( *(pBuffer++) ) 
		{
            while ( *(pBuffer++) );
        }

        length = (ULONG)(pBuffer - m_DeviceIDs)*sizeof(WCHAR);

        pBuffer = (PWCHAR)ExAllocatePoolWithTag(PagedPool, length, DefaultPoolTag);

        if (pBuffer != NULL) 
		{
			RtlCopyMemory(pBuffer, m_DeviceIDs, length);
			I.Information() = (ULONG_PTR)pBuffer;
		}
		else
		{
           status = STATUS_INSUFFICIENT_RESOURCES;
        }
		break;

    case BusQueryInstanceID:
        pBuffer = (PWCHAR)ExAllocatePoolWithTag(PagedPool, 12*sizeof(WCHAR), DefaultPoolTag);
        if (pBuffer != NULL) 
		{
			swprintf(pBuffer, L"%08x", m_Instance);
			I.Information() = (ULONG_PTR)pBuffer;
		}
		else
		{
           status = STATUS_INSUFFICIENT_RESOURCES;
        }
        break;

    case BusQueryHardwareIDs:
        pBuffer = m_HardwareIDs;

        while (*(pBuffer++)) 
		{
            while (*(pBuffer++));
        }

        length = (ULONG)(pBuffer - m_HardwareIDs)*sizeof(WCHAR);
        pBuffer = (PWCHAR)ExAllocatePoolWithTag(PagedPool, length, DefaultPoolTag);

        if (pBuffer != NULL) 
		{
			RtlCopyMemory(pBuffer, m_HardwareIDs, length);
			I.Information() = (ULONG_PTR)pBuffer;
		}
		else
		{
           status = STATUS_INSUFFICIENT_RESOURCES;
        }
        break;

    default:
        status = I.Status();
		break;
	}

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::OnQueryBusInformation
//
//	Routine Description:
//		IRP_MN_QUERY_BUS_INFORMATION handler
//
//	Parameters:
//		[in] I
//			IRP_MJ_PNP IRP
//
//	Return Value:
//		status
//
//	Comments:
//		None
//
NTSTATUS VCom1394Pdo::OnQueryBusInformation(KIrp I)
{
	t << "VCom1394Pdo::OnQueryBusInformation " << m_Instance << EOL;

	NTSTATUS status = STATUS_SUCCESS;

    PPNP_BUS_INFORMATION busInfo = (PPNP_BUS_INFORMATION)ExAllocatePoolWithTag(PagedPool, sizeof(PNP_BUS_INFORMATION), DefaultPoolTag);

    if ( busInfo ) 
	{
		busInfo->BusTypeGuid = VCOM1394BUS_GUID;

		busInfo->LegacyBusType = PNPBus;
		busInfo->BusNumber = 0;

	    I.Information() = (ULONG_PTR)busInfo;
	}
	else
	{
      status = STATUS_INSUFFICIENT_RESOURCES;
    }

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::OnDeviceUsageNotification
//
//	Routine Description:
//		IRP_MN_DEVICE_USAGE_NOTIFICATION handler
//
//	Parameters:
//		[in] I
//			IRP_MJ_PNP IRP
//
//	Return Value:
//		status
//
//	Comments:
//		None
//
NTSTATUS VCom1394Pdo::OnDeviceUsageNotification(KIrp I)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	t << "VCom1394Pdo::OnDeviceUsageNotification " << m_Instance << EOL;

	return I.PnpComplete(status);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::OnQueryResources
//
//	Routine Description:
//		IRP_MN_QUERY_RESOURCES handler
//
//	Parameters:
//		[in] I
//			IRP_MJ_PNP IRP
//
//	Return Value:
//		status
//
//	Comments:
//		None
//
NTSTATUS VCom1394Pdo::OnQueryResources(KIrp I)
{
	t << "VCom1394Pdo::OnQueryResources " << m_Instance << EOL;

	return DefaultPnp(I);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::OnQueryResourceRequirements
//
//	Routine Description:
//		IRP_MN_QUERY_RESOURCES handler
//
//	Parameters:
//		[in] I
//			IRP_MJ_PNP IRP
//
//	Return Value:
//		status
//
//	Comments:
//		None
//
NTSTATUS VCom1394Pdo::OnQueryResourceRequirements(KIrp I)
{
	t << "VCom1394Pdo::OnQueryResourceRequirements " << m_Instance << EOL;

	return DefaultPnp(I);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::OnStartDevice
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

NTSTATUS VCom1394Pdo::OnStartDevice(KIrp I)
{
    t << "Entering VCom1394Pdo::OnStartDevice\n";

    KRegistryKey    reg;
    ULONG           nameLength = 0;
    NTSTATUS        status;
    UNICODE_STRING  path;
    
    // Get the generation count
    KIrb<REQUEST_GET_GENERATION_COUNT> GenerationIrb;
    GenerationIrb.Initialize();
    status = m_pParent->m_Lower.SubmitIrb(GenerationIrb);

    if(NT_SUCCESS(status))
    {
        m_GenerationCount = GenerationIrb.GenerationCount();
    }

    status = reg.Initialize(m_pDeviceObject, PLUGPLAY_REGKEY_DEVICE, KEY_ALL_ACCESS);

    status = reg.QueryValue(L"PortName", 
	                        m_LinkName, 
	                        nameLength);

    if(!NT_SUCCESS(status) || (wcslen(m_LinkName) == 0)) {
        t << "  Failed to get COM port name\n";
        return I.PnpComplete(STATUS_UNSUCCESSFUL);
    }

	status = CreateLink(m_LinkName);

    if(!NT_SUCCESS(status)) 
    {
        t << "  Symbolic link creation failed: " << status << "\n";

        return I.PnpComplete(STATUS_UNSUCCESSFUL);
	}

    RtlInitUnicodeString(&path, L"SERIALCOMM");

	status = reg.Initialize(REGISTRY_DEVICEMAP, &path);

    if(!NT_SUCCESS(status) || !NT_SUCCESS (reg.LastError())) 
    {
        status = reg.Reconstruct(REGISTRY_DEVICEMAP, 
                                 &path, 
                                 KEY_DEFAULT_ACCESS, 
                                 KEY_DEFAULT_ATTRIBUTES, 
                                 TRUE);
        if(!NT_SUCCESS(status) || !NT_SUCCESS (reg.LastError())) 
        {
            t << "  Failed to create COM port reg value: " << status << "  " << reg.LastError() << EOL;

            return I.PnpComplete(STATUS_UNSUCCESSFUL);
        }
    } 

    m_RegistryName = new WCHAR[500];

    swprintf((PWCHAR)m_RegistryName, L"\\VCOM\\COMPORT%d", m_Instance);

    status = reg.WriteValue(m_RegistryName, m_LinkName);

    if(!NT_SUCCESS(status) || !NT_SUCCESS (reg.LastError())) 
    {
        t << "  Failed to write COM port name to registry: " << status << reg.LastError() << EOL;

        return I.PnpComplete(STATUS_UNSUCCESSFUL);
	}

    SetupPortRegisters();

    // setup the device interface for the coinstaller DLL to use
    if(m_InterfaceSymbolicLinkName.Length == 0)
    {
        status = IoRegisterDeviceInterface((PDEVICE_OBJECT)*this,
                                            &GUID_DEVINTERFACE_VCOM1394PDO,
                                            &m_InterfaceReferenceString,
                                            &m_InterfaceSymbolicLinkName);

        if(!NT_SUCCESS(status))
        {
            t << "  IoRegisterDeviceInterface failed with status: " << status << EOL;

            return I.PnpComplete(STATUS_UNSUCCESSFUL);
        }
    }

    IoSetDeviceInterfaceState(&m_InterfaceSymbolicLinkName,
                                TRUE);

    return I.PnpComplete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::OnStopDevice
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

NTSTATUS VCom1394Pdo::OnStopDevice(KIrp I)
{
    NTSTATUS status;

    t << "Entering VCom1394Pdo::OnStopDevice\n";

    if(m_InterfaceSymbolicLinkName.Length != 0)
    {
        IoSetDeviceInterfaceState(&m_InterfaceSymbolicLinkName,
                                  FALSE);
    }

    // Remove our symbolic link so that nobody tries to talk to us
    if(m_LinkName != NULL) {
        DestroyLink(m_LinkName);
        delete  [] m_LinkName;
        m_LinkName = NULL;
    }

    if(m_ConnectionInfoRange.Count() != 0)
    {
        m_ConnectionInfoClient.Free(m_ConnectionInfoRange);
    }

    if(m_DataRegisterRange.Count() != 0)
    {
        m_DataRegisterClient.Free(m_DataRegisterRange);
    }

    if(NULL != m_pConnectionInfoMdl)
    {
        IoFreeMdl(m_pConnectionInfoMdl);

        m_pConnectionInfoMdl = NULL;
    }

    // remove the registry keys that we created so that our COM ports disappear
    if(m_RegistryName != NULL)
    {
        KRegistryKey reg;

        PCWSTR  path = L"SERIALCOMM";

	    status = reg.Initialize(REGISTRY_DEVICEMAP, path);

        if(!NT_SUCCESS(status) || !NT_SUCCESS (reg.LastError())) 
        {
            t << "  Failed to delete registry key, status: " << status << "  " << reg.LastError() << "\n";
        } 
        else 
        {
            status = reg.DeleteValue(m_RegistryName);
        }

        delete [] m_RegistryName;

        m_RegistryName = NULL;
    }

    return I.PnpComplete(STATUS_SUCCESS);
}


////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::OnRemoveDevice
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

NTSTATUS VCom1394Pdo::OnRemoveDevice(KIrp I)
{
    NTSTATUS status;

    t << "Entering VCom1394Pdo::OnRemoveDevice\n";

    if(m_InterfaceSymbolicLinkName.Length != 0)
    {
        IoSetDeviceInterfaceState(&m_InterfaceSymbolicLinkName,
                                  FALSE);
    }

    // Remove our symbolic link so that nobody tries to talk to us
    if(m_LinkName != NULL) {
        DestroyLink(m_LinkName);
        delete  [] m_LinkName;
        m_LinkName = NULL;
    }

    if(m_ConnectionInfoRange.Count() != 0)
    {
        m_ConnectionInfoClient.Free(m_ConnectionInfoRange);
    }

    if(m_DataRegisterRange.Count() != 0)
    {
        m_DataRegisterClient.Free(m_DataRegisterRange);
    }

    if(NULL != m_pConnectionInfoMdl)
    {
        IoFreeMdl(m_pConnectionInfoMdl);

        m_pConnectionInfoMdl = NULL;
    }

    if(m_RegistryName != NULL)
    {
        KRegistryKey reg;

        PCWSTR  path = L"SERIALCOMM";

        t << "  Initializing reg\n";

	    status = reg.Initialize(REGISTRY_DEVICEMAP, path);

        if(!NT_SUCCESS(status) || !NT_SUCCESS (reg.LastError())) 
        {
            t << "  Failed to delete registry key, status: " << status << "  " << reg.LastError() << "\n";
        } else {
            t << "  reg initialized\n";

            status = reg.DeleteValue(m_RegistryName);

            t << "reg.DeleteValue returning status " << status << EOL;
        }

        delete [] m_RegistryName;

        m_RegistryName = NULL;
    }

    return I.PnpComplete(STATUS_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
// DeviceControl
//
NTSTATUS VCom1394Pdo::DeviceControl(KIrp I)
{
    PVOID           pSystemBuffer;
    NTSTATUS        status = STATUS_UNSUCCESSFUL;
    UNICODE_STRING  interfaceName;

    t << "VCom1394Pdo::DeviceControl " << I.m_Irp << EOL;

    RtlInitUnicodeString(&interfaceName, L"\\privateinterface");

    pSystemBuffer = I.IoctlBuffer();

	I.Information() = 0;

    // If this device is being opened as a COM port let
    // the virtual serial code handle it
    if ((I.FileObject() == NULL) || 
        (I.FileObject()->FileName.Length == 0) ||
        (0 != RtlCompareUnicodeString(&I.FileObject()->FileName, 
                                      &interfaceName, 
                                      TRUE))
        )
    {
        return KVirtualComPdo::DeviceControl(I);
    }

    t << "VCom1394Pdo::DeviceControl: FileName = " << &I.FileObject()->FileName << EOL;

    // Our registered interface is being used to open the device
    // so we allow these IOCTLs only
	switch (I.IoctlCode())
    {
        case VCOM1394PDO_IOCTL_GET_CONNECTED_PORT:
            if (I.IoctlOutputBufferSize() < sizeof(VCOM_PORT) )
            {
                t << "  VCOM1394PDO_IOCTL_GET_CONNECTED_PORT -- buffer too small\n";
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            status = GetCurrentConnectedPortHandler((PVCOM_PORT)pSystemBuffer);
            if(NT_SUCCESS(status))
            {
		        I.Information() = sizeof(VCOM_PORT);
            }
            break;

	    case VCOM1394PDO_IOCTL_GET_PORT_LIST:
	        if (I.IoctlOutputBufferSize() < sizeof(VCOM_PORT_LIST) )
            {
                t << "  VCOM1394PDO_IOCTL_GET_PORT_LIST -- buffer too small\n";
		        status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

		    status = GetPortListHandler((PVCOM_PORT_LIST) pSystemBuffer);
            if(NT_SUCCESS(status))
            {
		        I.Information() = sizeof(VCOM_PORT_LIST);
            }
		    break;

        case VCOM1394PDO_IOCTL_SELECT_PORT:
            if (I.IoctlInputBufferSize() < sizeof(VCOM_PORT) )
            {
                t << "  VCOM1394PDO_IOCTL_SELECT_PORT -- buffer too small\n";
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            status = SelectPortHandler((PVCOM_PORT)pSystemBuffer);
            break;

        default:
            // not supported
            status = STATUS_NOT_SUPPORTED;
    }

	if (status != STATUS_PENDING)
    {
		return I.Complete(status);
    }
	else
    {
		return STATUS_PENDING;
    }
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::GetPortListHandler - 
//
NTSTATUS VCom1394Pdo::GetCurrentConnectedPortHandler(PVCOM_PORT  pPort)
{
    NTSTATUS    status = STATUS_SUCCESS;

    t << "VCom1394Pdo::GetCurrentConnectedPortHandler\n";

    t << "  ****************** MachineName - " << (PWSTR)m_PortRegisters.PortConnection.MachineName << " PortName - " << (PWSTR)m_PortRegisters.PortConnection.PortName << " Node Number - " << m_PortRegisters.PortConnection.NodeNumber << EOL;

    RtlCopyMemory(pPort, 
                  &m_PortRegisters.PortConnection, 
                  sizeof(VCOM_PORT));
    
    t << "  ****************** MachineName - " << (PWSTR)pPort->MachineName << " PortName - " << (PWSTR)pPort->PortName << " Node Number - " << pPort->NodeNumber << EOL;

    return status;
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::GetPortListHandler - 
//
NTSTATUS VCom1394Pdo::GetPortListHandler(PVCOM_PORT_LIST    pPortList)
{
    return CreatePortList(pPortList);
}

////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::SelectPortHandler - 
//
NTSTATUS VCom1394Pdo::SelectPortHandler(PVCOM_PORT  pPort)
{
    NTSTATUS    status = STATUS_SUCCESS;
    VCOM_PORT   portInfo;

    t << "VCom1394Pdo::SelectPortHandler\n";

    RtlZeroMemory(&portInfo, sizeof(VCOM_PORT));

    RtlCopyMemory((PCHAR)portInfo.MachineName, 
                  (PCHAR)m_pParent->GetMachineName(), 
                  wcslen(m_pParent->GetMachineName()) * sizeof(WCHAR));
    portInfo.PortAddress = m_PortInfo.PortAddress;
    RtlCopyMemory((PCHAR)portInfo.PortName, 
                  (PCHAR)m_PortInfo.PortName, 
                  wcslen((PWSTR)m_PortInfo.PortName) * sizeof(WCHAR));

    //portInfo.NodeNumber = m_PortInfo.NodeNumber;

    t << "  Attempting connection to node " << pPort->NodeNumber << EOL;
    t << "  Attempting connection to OffsetHigh " << pPort->PortAddress.Off_High << EOL;
    t << "  Attempting connection to OffsetLow " << pPort->PortAddress.Off_Low << EOL;
    t << "  Sending connection node " << portInfo.NodeNumber << EOL;
    t << "  Sending connection OffsetHigh " << portInfo.PortAddress.Off_High << EOL;
    t << "  Sending connection OffsetLow " << portInfo.PortAddress.Off_Low << EOL;

    status = AsyncWrite(
                    &portInfo,
                    pPort->NodeNumber,
                    sizeof(VCOM_PORT),
                    pPort->PortAddress.Off_High,
                    pPort->PortAddress.Off_Low);

    if(NT_SUCCESS(status))
    {
        m_bContactedTarget = TRUE;

        InterlockedCompareExchange(
            (PLONG)&m_bConnected, 
            TRUE, 
            FALSE
            );

        // copy the connection info for this node
        RtlCopyMemory((PCHAR)&m_PortRegisters.PortConnection,
                      (PCHAR)pPort,
                      sizeof(VCOM_PORT));

        t << "  Connected to node: " << pPort->NodeNumber << EOL;
        t << "  Connected node Offset_High: " << pPort->PortAddress.Off_High << EOL;
        t << "  Connected node Offset_Low: " << pPort->PortAddress.Off_Low << EOL;
        t << "  Connected node Name: " << (PWSTR)pPort->MachineName << "\\" << (PWSTR)pPort->PortName << EOL;
    }
    else
    {
        t << "  AsyncWrite failed with status " << status << EOL;
    }

    return status;
}


////////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::SetupPortRegisters - sets up the registers for the 
//                                      1394 virtual device
//
NTSTATUS VCom1394Pdo::SetupPortRegisters()
{
    NTSTATUS        status = STATUS_SUCCESS;
    ULONG           nameLength = 0;
    ADDRESS_OFFSET  dataOffset;

    t << "VCom1394Pdo::SetupPortRegisters \n";

    // zero the connection information
    RtlZeroMemory(&m_PortRegisters.PortConnection, 
                  sizeof(VCOM_PORT));

    //t << " Setting connection name to " << (PWSTR)m_PortRegisters.PortConnection.MachineName << EOL;


    t << "  Allocate Connection Info Address Range\n";

    // request our addresses for the connection info lock-only registers

    // setup our address offsets for the address ranges to be allocated
    m_ConnectionInfoOffset.Off_High = INITIAL_REGISTER_SPACE_HI;
    m_ConnectionInfoOffset.Off_Low  = VCOM1394_SPACE_OFFSET_LO + 
                                        sizeof(VCOM1394_NODE_REGISTERS) + 
                                            (m_Instance * sizeof(VCOM1394_PORT_REGISTERS));
                                            

    if(m_bIsVirtual) 
    {
        status = m_ConnectionInfoClient.Allocate(
                        m_ConnectionInfoRange,
                        sizeof(VCOM_PORT),
                        m_ConnectionInfoOffset,
                        ACCESS_FLAGS_TYPE_WRITE | ACCESS_FLAGS_TYPE_READ | ACCESS_FLAGS_TYPE_BROADCAST);
    } 
    else 
    {
        status = m_ConnectionInfoClient.Allocate(
                        m_ConnectionInfoRange,
                        sizeof(VCOM_PORT),
                        m_ConnectionInfoOffset,
                        ACCESS_FLAGS_TYPE_WRITE | ACCESS_FLAGS_TYPE_READ);
    }

/*
    // setup the MDL for the read only registers
    m_pConnectionInfoMdl = IoAllocateMdl(&m_PortRegisters.PortConnection, 
                                         sizeof(VCOM_PORT), 
                                         FALSE, 
                                         FALSE, 
                                         NULL);
	if(NULL == m_pConnectionInfoMdl)
	{
		t << "  Cannot create storage buffer MDL\n";
		
        return STATUS_INSUFFICIENT_RESOURCES;
	}

    t << "  Setup MDL\n";

    // lock the pages in memory
    MmBuildMdlForNonPagedPool(m_pConnectionInfoMdl);

    if(m_bIsVirtual) 
    {
        status = m_ConnectionInfoClient.Allocate(
                        m_ConnectionInfoRange,
                        m_pConnectionInfoMdl,
                        ACCESS_FLAGS_TYPE_READ | ACCESS_FLAGS_TYPE_WRITE | ACCESS_FLAGS_TYPE_BROADCAST,
                        NOTIFY_FLAGS_AFTER_WRITE);
    } 
    else 
    {
        status = m_ConnectionInfoClient.Allocate(
                        m_ConnectionInfoRange,
                        m_pConnectionInfoMdl,
                        ACCESS_FLAGS_TYPE_READ | ACCESS_FLAGS_TYPE_WRITE,
                        NOTIFY_FLAGS_AFTER_WRITE);
    }
*/
    if (!NT_SUCCESS(status)) 
    {
        t << "  Allocate For Connection Info Range"
             << " failed - status (" << status << ")\n";
        return status;
    }

    t << "  Allocate Data Address Range\n";

    // setup our data register range starting offset
    dataOffset.Off_High = m_ConnectionInfoRange.Array()[0].AR_Off_High;
        dataOffset.Off_Low = m_ConnectionInfoRange.Array()[0].AR_Off_Low + 
                                m_ConnectionInfoRange.Array()[0].AR_Length;
                                //(m_ConnectionInfoRange.Array()[0].AR_Length / sizeof(ULONG));

        DbgPrint("  Allocated Data Address Range - Offset Low = 0x%X\n", dataOffset.Off_Low);
        DbgPrint("  Allocated Data Address Range - Offset Low = 0x%X\n", VCOM1394_SPACE_OFFSET_LO | 
                                                                            FIELD_OFFSET(VCOM1394_NODE_REGISTERS, Key));

        DbgPrint("  Allocated Data Address Range - Offset Hi = 0x%X\n", dataOffset.Off_High);
        DbgPrint("  Allocated Data Address Range - Offset Hi = 0x%X\n", INITIAL_REGISTER_SPACE_HI);
        

    // request our addresses for the data registers
    if(m_bIsVirtual) 
    {
	    status = m_DataRegisterClient.Allocate(
		                m_DataRegisterRange,
		                SERIAL_BUFFER_SIZE,
                        dataOffset,
                        ACCESS_FLAGS_TYPE_WRITE | ACCESS_FLAGS_TYPE_BROADCAST);
    } 
    else 
    {
	    status = m_DataRegisterClient.Allocate(
		                m_DataRegisterRange,
		                SERIAL_BUFFER_SIZE,
                        dataOffset,
                        ACCESS_FLAGS_TYPE_WRITE);
    }

    if (!NT_SUCCESS(status)) 
    {
        t << "  Allocate For Data Range"
             << " failed - status(" << status << ")\n";
        return status;
    }

    RtlCopyMemory((PWSTR)m_PortInfo.PortName, m_LinkName, sizeof(ULONG) * 4);
    m_PortInfo.PortAddress.Off_High = m_ConnectionInfoRange.Array()[0].AR_Off_High;
    m_PortInfo.PortAddress.Off_Low = m_ConnectionInfoRange.Array()[0].AR_Off_Low;

    m_pParent->UpdateNodeRegisters(TRUE, &m_PortInfo);

    return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////
//VCom1394Pdo::TopologyMap
//
// Parameters:
//
// Returns:
//
// Comments:
//

NTSTATUS VCom1394Pdo::TopologyMap(PTOPOLOGY_MAP pTopologyMap, PULONG pLength)
{
	GET_LOCAL_HOST_INFO6 topologyInfo;

    topologyInfo.CsrBaseAddress.Off_High = INITIAL_REGISTER_SPACE_HI;
    topologyInfo.CsrBaseAddress.Off_Low = TOPOLOGY_MAP_LOCATION;

	topologyInfo.CsrDataLength = *pLength;
    topologyInfo.CsrDataBuffer = (PVOID)pTopologyMap;

	KIrb<REQUEST_GET_LOCAL_HOST_INFO> Irb(GET_HOST_CSR_CONTENTS, &topologyInfo);

	NTSTATUS status = m_pParent->m_Lower.SubmitIrb(&Irb);

	if( status == STATUS_INVALID_BUFFER_SIZE )
		*pLength = topologyInfo.CsrDataLength;

	return status;
}

NTSTATUS VCom1394Pdo::NodeAddress(PNODE_ADDRESS pNodeAddress,
                                  ULONG         Flags)
{
    NTSTATUS    status;

	KIrb<REQUEST_GET_ADDR_FROM_DEVICE_OBJECT> Irb(Flags);

    ASSERT(pNodeAddress != NULL);

	status = m_pParent->m_Lower.SubmitIrb(&Irb);

    pNodeAddress->NA_Node_Number = Irb.NodeNumber();
    pNodeAddress->NA_Bus_Number = Irb.BusNumber();

    return status;
}

NTSTATUS VCom1394Pdo::AsyncWrite(PVOID  pBuffer,
                                 ULONG  Node,
                                 ULONG  Length,
                                 USHORT OffsetHigh,
                                 ULONG  OffsetLow)
{
	KIrb<REQUEST_ASYNC_WRITE> Irb;

	KMemory Mdl(pBuffer, Length);
	Mdl.SetPageArray();

	Irb.Initialize(m_GenerationCount, OffsetHigh, OffsetLow, Mdl);

    Irb.u.AsyncWrite.DestinationAddress.IA_Destination_ID.NA_Node_Number = Node;
    Irb.u.AsyncWrite.DestinationAddress.IA_Destination_ID.NA_Bus_Number  = LOCAL_BUS;

	return m_pParent->m_Lower.SubmitIrbDirect(Irb);
}

NTSTATUS VCom1394Pdo::AsyncWrite(PVOID                  pBuffer,
                                 ULONG                  Node,
                                 ULONG                  Length,
                                 USHORT                 OffsetHigh,
                                 ULONG                  OffsetLow,
                                 PCOMPLETION_ROUTINE    pRoutine,
                                 PVOID                  pContext)
{
    t << "VCom1394Pdo::AsyncWrite\n";

    KIrp I = KIrp::Allocate(m_pParent->m_Lower.StackRequirement());

	if( I.IsNull() )
    {
		return STATUS_INSUFFICIENT_RESOURCES;
	}

    PMDL pMdl = IoAllocateMdl(pBuffer, Length, FALSE, FALSE, NULL);

    if (pMdl == NULL)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    MmBuildMdlForNonPagedPool(pMdl);

	KIrbWithContext<REQUEST_ASYNC_WRITE, VCOM1394_ASYNC_TRANSFER_CONTEXT>* pIrb = 
	            new KIrbWithContext<REQUEST_ASYNC_WRITE, VCOM1394_ASYNC_TRANSFER_CONTEXT>;

	if ( NULL == pIrb )
	{
        return STATUS_INSUFFICIENT_RESOURCES;
	}

	pIrb->m_Context.pContext = this;
    pIrb->m_Context.pIrp = I;
    pIrb->m_Context.pMdl = pMdl;
    pIrb->m_Context.pRoutine = pRoutine;

	pIrb->Initialize(m_GenerationCount, OffsetHigh, OffsetLow, pMdl);

    pIrb->u.AsyncWrite.DestinationAddress.IA_Destination_ID.NA_Node_Number = Node;
    pIrb->u.AsyncWrite.DestinationAddress.IA_Destination_ID.NA_Bus_Number  = LOCAL_BUS;

	return m_pParent->m_Lower.SubmitIrbDirect(I, pIrb, AsyncWriteCompletion, pIrb);
}

NTSTATUS VCom1394Pdo::AsyncWriteCompletion(PDEVICE_OBJECT  pDeviceObject,
                                           PIRP            pIrp,
                                           PVOID           pContext)
{
    KIrbWithContext<REQUEST_ASYNC_WRITE, VCOM1394_ASYNC_TRANSFER_CONTEXT>* pIrb = (KIrbWithContext<REQUEST_ASYNC_WRITE, VCOM1394_ASYNC_TRANSFER_CONTEXT>*)pContext;

    t << "VCom1394Pdo::AsyncWriteCompletion\n";

    KIrp::Deallocate(pIrb->m_Context.pIrp);

    IoFreeMdl(pIrb->m_Context.pMdl);

    if( NULL != pIrb->m_Context.pRoutine )
    {
        pIrb->m_Context.pRoutine(pIrb->m_Context.pContext);
    }

    delete pIrb;

    return STATUS_SUCCESS;
}

NTSTATUS VCom1394Pdo::AsyncRead(PVOID   pBuffer,
                                ULONG   Node,
                                ULONG   Length,
                                USHORT  OffsetHigh,
                                ULONG   OffsetLow)
{
	KIrb<REQUEST_ASYNC_READ> Irb;

	KMemory Mdl(pBuffer, Length);
	Mdl.SetPageArray();

	Irb.Initialize(m_GenerationCount, OffsetHigh, OffsetLow, Mdl);

    Irb.u.AsyncRead.DestinationAddress.IA_Destination_ID.NA_Node_Number = Node;
    Irb.u.AsyncRead.DestinationAddress.IA_Destination_ID.NA_Bus_Number  = LOCAL_BUS;

	return m_pParent->m_Lower.SubmitIrbDirect(Irb);
}

////////////////////////////////////////////////////////////////////
//  VCom1394Pdo::CreatePortList - finds all peer devices which can be 
//           communicated with and fills in the peer list structure
//
NTSTATUS VCom1394Pdo::CreatePortList(PVCOM_PORT_LIST    pPortList)
{
    NTSTATUS                    status;                     // return status
    PTOPOLOGY_MAP               pTopologyMap = NULL;        // topology map
    ULONG                       topologyBufferLength = 0;   // topology map buffer length
    NODE_ADDRESS                nodeAddress;                // our node number
    VCOM1394_NODE_REGISTERS     vcomRegisters;
    VCOM_PORT                   portRegisters;

    t << "VCom1394Pdo::CreatePortList\n";

    // initialize to zero devices
    pPortList->NumberOfPorts = 0;

    // get the topology map from the bus
    status = TopologyMap(NULL, &topologyBufferLength);
    if(!NT_SUCCESS(status))
    {       
        if(status != STATUS_INVALID_BUFFER_SIZE)  
        {
            return STATUS_UNSUCCESSFUL;
        }

        // Allocate our topology map buffer
        pTopologyMap = (PTOPOLOGY_MAP) new CHAR[topologyBufferLength] ;

        status = TopologyMap(pTopologyMap, &topologyBufferLength);
        if(!NT_SUCCESS(status))
        {
            if(NULL != pTopologyMap)
            {
                delete [] pTopologyMap;
            }

            return STATUS_UNSUCCESSFUL;
        }
    }

    // get the node address
    NodeAddress(&nodeAddress);

    // go through the topology map

    t << "  Topology Map\n";
    t << "  ------------\n";
    t << "  " << pTopologyMap->TOP_Length << "  Number of DWORDS making up map.\n";
    t << "  " << pTopologyMap->TOP_CRC << "  16-bit CRC\n";
    t << "  " << pTopologyMap->TOP_Generation << "  Generation number\n";
    t << "  " << pTopologyMap->TOP_Node_Count << "  Number of nodes\n";
    t << "  " << pTopologyMap->TOP_Self_ID_Count << "  Number of Self IDs\n";

    for(ULONG ii = 0; ii < pTopologyMap->TOP_Self_ID_Count; ii++) 
    {
//        if(pTopologyMap->TOP_Self_ID_Array[ii].SID_Phys_ID == 
//            nodeAddress.NA_Node_Number) 
//        {
//            continue;
//        }

        t << "  Self ID #" << ii << EOL;
        t << "  ------------\n";
        t << "  " << pTopologyMap->TOP_Self_ID_Array[ii].SID_Phys_ID << "  physical ID\n";
        t << "  " << pTopologyMap->TOP_Self_ID_Array[ii].SID_Packet_ID << "  packet ID\n";
        t << "  " << pTopologyMap->TOP_Self_ID_Array[ii].SID_Port3 << "  Port3\n";
        t << "  " << pTopologyMap->TOP_Self_ID_Array[ii].SID_Port2 << "  Port2\n";
        t << "  " << pTopologyMap->TOP_Self_ID_Array[ii].SID_Port1 << "  Port1\n";


        t << "Reading from node #" << pTopologyMap->TOP_Self_ID_Array[ii].SID_Phys_ID << EOL;

        // try to read the key from the next node in the list
        status = AsyncRead(vcomRegisters.Key,
                           pTopologyMap->TOP_Self_ID_Array[ii].SID_Phys_ID,
                           sizeof(ULONG) * 36,
                           INITIAL_REGISTER_SPACE_HI,
                           (VCOM1394_SPACE_OFFSET_LO | 
                                FIELD_OFFSET(VCOM1394_NODE_REGISTERS, Key)));

        if(!NT_SUCCESS(status)) 
        {
            t << "  AsyncRead of Key register failed with status: " << status << "\n";
            continue;
        }
        
        // see if we got the right key
        if(sizeof("VCOM 2 1394") != 
            RtlCompareMemory((PSTR)vcomRegisters.Key, 
                                "VCOM 2 1394",
                                sizeof("VCOM 2 1394"))) 
        {
            // device is not one we can talk to
            continue;
        }

// 0x4D4F4356;  // ASCII "VCOM 2 1394" 
// 0x31203220;  
// 0x00343933;  

        t << (CHAR *)vcomRegisters.Key << "  Node's key\n";

        // Attempt to read and see if the port is already connected
        status = AsyncRead(
                    &vcomRegisters.NumberOfPorts,
                    pTopologyMap->TOP_Self_ID_Array[ii].SID_Phys_ID,
		            (sizeof(VCOM_PORT) * MAXIMUM_NUMBER_OF_PORTS) 
                        + sizeof(ULONG),
                    INITIAL_REGISTER_SPACE_HI,
                    (VCOM1394_SPACE_OFFSET_LO | 
                        FIELD_OFFSET(VCOM1394_NODE_REGISTERS, NumberOfPorts)));
        if(STATUS_SUCCESS != status) 
        {
            t << "  Failed to read remote node's port count, Status = " << status << EOL;
            continue;
        }

        t << "  Number of remote ports = " << vcomRegisters.NumberOfPorts << EOL;

        for(ULONG jj = 0; jj < vcomRegisters.NumberOfPorts; jj++)
        {
            // Attempt to read and see if the port is already connected
            status = AsyncRead(
                        &portRegisters,
                        pTopologyMap->TOP_Self_ID_Array[ii].SID_Phys_ID,
		                sizeof(VCOM_PORT),
                        vcomRegisters.PortInfo[jj].PortAddress.Off_High,
                        vcomRegisters.PortInfo[jj].PortAddress.Off_Low
                        );

            if(wcslen((PWSTR)portRegisters.PortName) && 
                    wcscmp((PWSTR)portRegisters.PortName, m_LinkName))
            {
                t << "Remote port offset " << vcomRegisters.PortInfo[jj].PortAddress.Off_Low << EOL;
                t << "  Remote port #" << jj << " already connected to " << (PWSTR)portRegisters.PortName << EOL;

                // remote port is already connected to someone else
//                continue;
            }

            // found a peer node we can talk to, save this node info

            t << (PWSTR)vcomRegisters.PortInfo[jj].PortName << "  Remote Port Name\n";

            t << (PWSTR)vcomRegisters.MachineName << "  Remote MachineName\n";

            // save the remote port info in our list
            wcscpy((PWSTR)pPortList->Port[pPortList->NumberOfPorts].PortName, (PWSTR)vcomRegisters.PortInfo[jj].PortName);

            t << (PWSTR)pPortList->Port[pPortList->NumberOfPorts].PortName << "  Remote Port Name\n";

            wcscpy((PWSTR)pPortList->Port[pPortList->NumberOfPorts].MachineName, (PWSTR)vcomRegisters.MachineName);

            pPortList->Port[pPortList->NumberOfPorts].NodeNumber = 
                            pTopologyMap->TOP_Self_ID_Array[ii].SID_Phys_ID;

            pPortList->Port[pPortList->NumberOfPorts].PortAddress.Off_Low = vcomRegisters.PortInfo[jj].PortAddress.Off_Low;
            pPortList->Port[pPortList->NumberOfPorts].PortAddress.Off_High = vcomRegisters.PortInfo[jj].PortAddress.Off_High;

            // update the port count
            pPortList->NumberOfPorts++;
        }
    }

    // free the topology map memory
    delete [] pTopologyMap;

    return STATUS_SUCCESS;
}

PVCOM1394_RESPONSE VCom1394Pdo::AllocateResponseContext()
{
    return m_pParent->AllocateResponseContext();
}

VOID VCom1394Pdo::QueueResponseContext(PVCOM1394_RESPONSE pContext)
{
    m_pParent->QueueResponseContext(pContext);
}


////////////////////////////////////////////////////////////////////
// VCom1394PdoAsyncConnectionClient::OnWrite - notification routine to 
//           notify when the ready register was accessed.
//
VOID VCom1394PdoAsyncConnectionClient::OnWrite(
                                PVOID       RequestPacket, 
                                PMDL        pResponseMdl,
                                PVOID      *ppResponsePacket,
                                PULONG      pResponseLength,
                                PKEVENT    *ppResponseEvent)
{   
    PVCOM1394_OHCI_ASYNC_PACKET pResponsePacket;
    PVCOM1394_OHCI_ASYNC_PACKET pRequestPacket = (PVCOM1394_OHCI_ASYNC_PACKET)RequestPacket;
    PVCOM1394_RESPONSE          pResponseContext;
    VCom1394Pdo                *pParent = (VCom1394Pdo *)m_pParent;
    BOOLEAN                     bAlreadyConnected;

    // Display where the request packet came from
//    t << "VCom1394PdoAsyncConnectionClient::OnWrite\n";

    t << "  Node - " << pRequestPacket->u1.Rx.OHCI_Source_ID.NA_Node_Number << EOL;

    // get current connection state and indicate connection if not already indicated
    bAlreadyConnected = (BOOLEAN)InterlockedCompareExchange((PLONG)&pParent->m_bConnected, 
                                                            TRUE, 
                                                            FALSE);

    // We have allocated our address range with no backing 
    // store, so we get the request packet and we have to 
    // create the response packet.

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
    RtlCopyMemory(pResponsePacket, 
                  pRequestPacket, 
                  sizeof(VCOM1394_OHCI_ASYNC_PACKET));

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

    // If we are not already connected, and this is a block write request 
    // and the request is of the proper size and to the proper offset then
    // copy the write data to our MDL.  Otherwise just drop it in the bit 
    // bucket.
//    if(!bAlreadyConnected &&
//        (pRequestPacket->OHCI_tCode == TCODE_WRITE_REQUEST_BLOCK) &&
//        ((pRequestPacket->OHCI_Offset_Low) == 0) &&
//        (pRequestPacket->u3.Block.OHCI_Data_Length == sizeof(VCOM_PORT)))
    if((pRequestPacket->OHCI_tCode == TCODE_WRITE_REQUEST_BLOCK) &&
//        ((pRequestPacket->OHCI_Offset_Low) == 0) &&
        (pRequestPacket->u3.Block.OHCI_Data_Length == sizeof(VCOM_PORT)))

    {
        // copy the connection info to our backing store for these registers
        RtlCopyMemory((PCHAR)&pParent->m_PortRegisters.PortConnection,
                      (PCHAR)pRequestPacket + sizeof(VCOM1394_OHCI_ASYNC_PACKET),
                      sizeof(VCOM_PORT));

        // save the node we are connecting too
        pParent->m_PortRegisters.PortConnection.NodeNumber = pRequestPacket->u1.Rx.OHCI_Source_ID.NA_Node_Number;

        t << "  Connected to node: " << pParent->m_PortRegisters.PortConnection.NodeNumber << EOL;
        t << "  Connected node Offset_High: " << pParent->m_PortRegisters.PortConnection.PortAddress.Off_High << EOL;
        t << "  Connected node Offset_Low: " << pParent->m_PortRegisters.PortConnection.PortAddress.Off_Low << EOL;
        t << "  Connected node Name: " << (PWSTR)pParent->m_PortRegisters.PortConnection.MachineName << " \\ " << (PWSTR)pParent->m_PortRegisters.PortConnection.PortName << EOL;
    }

    // indicate this is a write block response packet
    pResponsePacket->OHCI_tCode = TCODE_WRITE_RESPONSE;

    // initialize the info struct's response MDL
    // with our response buffer
    MmInitializeMdl(pResponseMdl,
                    pResponsePacket,
                    sizeof(VCOM1394_OHCI_ASYNC_PACKET));

    // map the pages
    MmBuildMdlForNonPagedPool(pResponseMdl);

    // set the response length
    *pResponseLength = MmGetMdlByteCount(pResponseMdl);

    // initialize our event
    KeInitializeEvent(&pResponseContext->ResponseEvent,
                      NotificationEvent, 
                      FALSE);

    // set our event as the response event in the info struct
    *ppResponseEvent = &pResponseContext->ResponseEvent;

    // put the response context entry on the used list
    pParent->QueueResponseContext(pResponseContext);
}

////////////////////////////////////////////////////////////////////
// VCom1394PdoAsyncConnectionClient::OnRead - notification routine to 
//           notify when the ready register was accessed.
//
VOID VCom1394PdoAsyncConnectionClient::OnRead(
                                PVOID       RequestPacket, 
                                PMDL        pResponseMdl,
                                PVOID      *ppResponsePacket,
                                PULONG      pResponseLength,
                                PKEVENT    *ppResponseEvent)
{   
    PVCOM1394_OHCI_ASYNC_PACKET pResponsePacket;
    PVCOM1394_OHCI_ASYNC_PACKET pRequestPacket = (PVCOM1394_OHCI_ASYNC_PACKET)RequestPacket;
    PVCOM1394_RESPONSE          pResponseContext;
    VCom1394Pdo                *pParent = (VCom1394Pdo *)m_pParent;
    BOOLEAN                     bAlreadyConnected;

    // Display where the request packet came from
//    t << "VCom1394PdoAsyncConnectionClient::OnRead\n";

    t << "  Node - " << pRequestPacket->u1.Rx.OHCI_Source_ID.NA_Node_Number << EOL;

    // get current connection state and indicate connection if not already indicated
    bAlreadyConnected = (BOOLEAN)InterlockedCompareExchange((PLONG)&pParent->m_bConnected, 
                                                            TRUE, 
                                                            FALSE);

    // We have allocated our address range with no backing 
    // store, so we get the request packet and we have to 
    // create the response packet.

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
    RtlCopyMemory(pResponsePacket, 
                  pRequestPacket, 
                  sizeof(VCOM1394_OHCI_ASYNC_PACKET));

    // setup the response packet

    // The source ID from the request packet
    // is actually used as the destination address 
    // for the response, which is weird.  The 
    // actual source ID for the response is filled 
    // in by the 1394 bus driver

    // calculate the speed for the response packet
    pResponsePacket->u.Tx.OHCI_spd = GetRequestPacketSpeed(pRequestPacket);

    // set an unsuccessful response code
    pResponsePacket->u2.Response.OHCI_Rcode = RCODE_TIMED_OUT;

    // If we are not already connected, and this is a block write request 
    // and the request is of the proper size and to the proper offset then
    // copy the write data to our MDL.  Otherwise just drop it in the bit 
    // bucket.
//    if(!bAlreadyConnected &&
//        (pRequestPacket->OHCI_tCode == TCODE_WRITE_REQUEST_BLOCK) &&
//        ((pRequestPacket->OHCI_Offset_Low) == 0) &&
//        (pRequestPacket->u3.Block.OHCI_Data_Length == sizeof(VCOM_PORT)))
    if(pRequestPacket->OHCI_tCode == TCODE_READ_REQUEST_QUADLET)
    {
    }

    // indicate this is a write block response packet
    pResponsePacket->OHCI_tCode = TCODE_READ_RESPONSE_QUADLET;

    // initialize the info struct's response MDL
    // with our response buffer
    MmInitializeMdl(pResponseMdl,
                    pResponsePacket,
                    sizeof(VCOM1394_OHCI_ASYNC_PACKET));

    // map the pages
    MmBuildMdlForNonPagedPool(pResponseMdl);

    // set the response length
    *pResponseLength = MmGetMdlByteCount(pResponseMdl);

    // initialize our event
    KeInitializeEvent(&pResponseContext->ResponseEvent,
                      NotificationEvent, 
                      FALSE);

    // set our event as the response event in the info struct
    *ppResponseEvent = &pResponseContext->ResponseEvent;

    // put the response context entry on the used list
    pParent->QueueResponseContext(pResponseContext);
}

/*
////////////////////////////////////////////////////////////////////
// VCom1394PdoAsyncConnectionClient::OnLock - notification routine to 
//           notify when the ready register was accessed.
//
VOID VCom1394PdoAsyncConnectionClient::OnLock(
                                PVOID       RequestPacket, 
                                PMDL        pResponseMdl,
                                PVOID      *ppResponsePacket,
                                PULONG      pResponseLength,
                                PKEVENT    *ppResponseEvent)
{   
//    PIRP                        pReadIrp;       // the current read IRP
//    KIRQL                       oldIrql;        // our current IRQL
//    PVCOM1394_OHCI_ASYNC_PACKET pResponsePacket = *ppResponsePacket;
    PVCOM1394_OHCI_ASYNC_PACKET pRequestPacket = (PVCOM1394_OHCI_ASYNC_PACKET)RequestPacket;


    // Show where the request packet came from

    t << "VCom1394PdoAsyncConnectionClient::OnLock: Node - " << pRequestPacket->u1.Rx.OHCI_Source_ID.NA_Node_Number << EOL;

    // we are not on XP so, in this case we have allocated our address
    // range with no backing store, so we get the request packet and 
    // we have to create the response packet.
    // get the write response context

    // copy the request packet over to the response 
    // packet buffer and modify
//    RtlCopyMemory(pResponsePacket, 
//                    pRequestPacket, 
//                    sizeof(VCOM1394_OHCI_ASYNC_PACKET));

    // setup the response packet

    // The source ID from the request packet
    // is actually used as the destination address 
    // for the response, which is weird.  The 
    // actual source ID for the response is filled 
    // in by the 1394 bus driver
/*
    // calculate the speed for the response packet
    pResponsePacket->u.Tx.OHCI_spd = 
            pDevice->GetRequestPacketSpeed(pRequestPacket);

        // indicate this is a write response packet
    pResponsePacket->OHCI_tCode = TCODE_WRITE_RESPONSE;
        // set a success response code
    pResponsePacket->u2.Response.OHCI_Rcode = RCODE_RESPONSE_COMPLETE;

            // get the address fifo entry
        pInfo->Fifo = CONTAINING_RECORD(pEntry,
                                        ADDRESS_FIFO,
                                        FifoList);
            // check for quadlet or block request.
        if(pRequestPacket->OHCI_tCode == TCODE_WRITE_REQUEST_QUADLET) 
        {

            // copy the quadlet data to the MDL
            RtlCopyMemory(GetSystemAddressForMdlSafe(pInfo->Fifo->FifoMdl, NormalPagePriority),
                            (PCHAR)&pRequestPacket->u3.OHCI_Quadlet_Data,
                            sizeof(ULONG));
        }
        else 
        {
            // copy the block data to the MDL
            RtlCopyMemory(GetSystemAddressForMdlSafe(pInfo->Fifo->FifoMdl, NormalPagePriority),
                            (PCHAR)pRequestPacket + sizeof(VCOM1394_OHCI_ASYNC_PACKET),
                            pRequestPacket->u3.Block.OHCI_Data_Length);
        }
    }

    // initialize the info struct's response MDL
    // with our response buffer
    MmInitializeMdl(pInfo->ResponseMdl,
                    pResponsePacket,
                    sizeof(VCOM1394_OHCI_ASYNC_PACKET));

    // map the pages
    MmBuildMdlForNonPagedPool(pInfo->ResponseMdl);

    // set the response length
    *pInfo->ResponseLength = MmGetMdlByteCount(pInfo->ResponseMdl);

    // set our event as the response event in the info struct
    *pInfo->ResponseEvent = &pResponseContext->ResponseEvent;

    // queue the work item 
    ExQueueWorkItem(&pResponseContext->ResponseWorkItem, 
                    DelayedWorkQueue);

    // determine whether to go on
    if(pResponsePacket->u2.Response.OHCI_Rcode != RCODE_RESPONSE_COMPLETE) 
    {
        // there was an error so just return
        return;
    }

    // get our list entry
    pListEntry = CONTAINING_RECORD(pInfo->Fifo,
                                   VCOM_READ_BUFFER,
                                   ListEntry);
    // get the read IRP
    pReadIrp = pListEntry->pIrp;

    // set the cancel routine to NULL in the IRP
    IoSetCancelRoutine(pReadIrp, NULL);

    // get the MDL list lock in order to synch
    // with the cancel routine
    KeAcquireSpinLock(&pDevice->m_DataMdlListLock, &oldIrql);

    // drop the lock
    KeReleaseSpinLock(&pDevice->m_DataMdlListLock, oldIrql);

    // because ReadFile uses Direct IO it doesn't matter what we set
    // pIrp->IoStatus.Information to here as long as its value does not exceed 
    // the size of the buffer.  So, we are going to cheat and put the node
    // number in this field.  As the buffer will always be 256 bytes, and
    // there can only be a max of 64 nodes, we are safe with this
    pReadIrp->IoStatus.Information = 
        pRequestPacket->u1.Rx.OHCI_Source_ID.NA_Node_Number;

    // complete the IRP
    pDevice->CompleteIrp(pReadIrp, STATUS_SUCCESS);

    // free the list entry 
    ExFreePool(pListEntry);

}
*/

////////////////////////////////////////////////////////////////////
// VCom1394PdoAsyncDataClient::OnWrite - notification routine to 
//           notify when the ready register was accessed.
//
VOID VCom1394PdoAsyncDataClient::OnWrite(
                                PVOID       RequestPacket, 
                                PMDL        pResponseMdl,
                                PVOID      *ppResponsePacket,
                                PULONG      pResponseLength,
                                PKEVENT    *ppResponseEvent)
{   
    PVCOM1394_OHCI_ASYNC_PACKET pResponsePacket;
    PVCOM1394_OHCI_ASYNC_PACKET pRequestPacket = (PVCOM1394_OHCI_ASYNC_PACKET)RequestPacket;
    PVCOM1394_RESPONSE          pResponseContext;
    VCom1394Pdo                *pParent = (VCom1394Pdo *)m_pParent;
    BOOLEAN                     bConnected;
    NTSTATUS                    status;

    // Display where the request packet came from
//    t << "VCom1394PdoAsyncDataClient::OnWrite\n";

    t << "  Node - " << pRequestPacket->u1.Rx.OHCI_Source_ID.NA_Node_Number << EOL;

    // get current connection state and indicate connection if not already indicated
    bConnected = (BOOLEAN)InterlockedCompareExchange((PLONG)&pParent->m_bConnected, 
                                                     TRUE, 
                                                     TRUE);

    // We have allocated our address range with no backing 
    // store, so we get the request packet and we have to 
    // create the response packet.

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
    RtlCopyMemory(pResponsePacket, 
                  pRequestPacket, 
                  sizeof(VCOM1394_OHCI_ASYNC_PACKET));

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

    // If we are connected, this is the node we are connected too,
    // and this is a write request to the beginning
    // of the data buffer then copy the data up to the write buffer.  
    // Otherwise just drop it in the bit bucket.
//    if((pParent->m_PortRegisters.PortConnection.NodeNumber == pRequestPacket->u1.Rx.OHCI_Source_ID.NA_Node_Number) &&
//        bConnected &&
//        ((pRequestPacket->OHCI_Offset_Low) == 0))
//    {
        if(pRequestPacket->OHCI_tCode == TCODE_WRITE_REQUEST_BLOCK)
        {
//            t << " Received data = " << *((PUCHAR)pRequestPacket + sizeof(VCOM1394_OHCI_ASYNC_PACKET)) << " Size " << pRequestPacket->u3.Block.OHCI_Data_Length << EOL;

            status = pParent->PostReceiveData((PUCHAR)pRequestPacket + sizeof(VCOM1394_OHCI_ASYNC_PACKET),
                                              pRequestPacket->u3.Block.OHCI_Data_Length);
        } 
        else 
        {
//            t << " Received Quadlet data = " << pRequestPacket->u3.OHCI_Quadlet_Data << " Size " << sizeof(ULONG) << EOL;

            status = pParent->PostReceiveData((PUCHAR)&pRequestPacket->u3.OHCI_Quadlet_Data,
                                              sizeof(ULONG));
        }

        if(!NT_SUCCESS(status))
        {
            pResponsePacket->u2.Response.OHCI_Rcode = RCODE_TIMED_OUT;
        }
//    }

    // indicate this is a write block response packet
    pResponsePacket->OHCI_tCode = TCODE_WRITE_RESPONSE;

    // initialize the info struct's response MDL
    // with our response buffer
    MmInitializeMdl(pResponseMdl,
                    pResponsePacket,
                    sizeof(VCOM1394_OHCI_ASYNC_PACKET));

    // map the pages
    MmBuildMdlForNonPagedPool(pResponseMdl);

    // set the response length
    *pResponseLength = MmGetMdlByteCount(pResponseMdl);

    // initialize our event
    KeInitializeEvent(&pResponseContext->ResponseEvent,
                      NotificationEvent, 
                      FALSE);

    // set our event as the response event in the info struct
    *ppResponseEvent = &pResponseContext->ResponseEvent;

    // put the response context entry on the used list
    pParent->QueueResponseContext(pResponseContext);
}

ULONG VCom1394Pdo::SendData(PUCHAR pBuffer, ULONG Count)
{
    NTSTATUS        status = STATUS_SUCCESS;
    static ULONG    value = SERIAL_BUFFER_SIZE;

    t << "VCom1394Pdo::SendData " << value << EOL;

    m_TotalTransferred += Count;

    if(!m_bConnected)
    {
        return 0;
    }
//        t << "  Sending " << Count << " bytes of data...\n";
//        t << "  Data = " << *pBuffer << EOL;

//        t << "  Node =  " << m_PortRegisters.PortConnection.NodeNumber << EOL;
//        t << "  Off_High =  " << m_PortRegisters.PortConnection.PortAddress.Off_High << EOL;
//        t << "  Off_Low =  " << m_PortRegisters.PortConnection.PortAddress.Off_Low + sizeof(VCOM_PORT) << EOL;


    while(Count > value) 
    {
        status = AsyncWrite(pBuffer,
                            m_PortRegisters.PortConnection.NodeNumber,
                            value,
                            m_PortRegisters.PortConnection.PortAddress.Off_High,
                            m_PortRegisters.PortConnection.PortAddress.Off_Low + sizeof(VCOM_PORT));
    
        if(!NT_SUCCESS(status))
        {
            t << "write error a! " << status << EOL;
            break;
        }

        pBuffer += value;

        Count -= value;
    }

    if (NT_SUCCESS(status))
    {

        // send remaining data
        status = AsyncWrite(pBuffer,
                            m_PortRegisters.PortConnection.NodeNumber,
                            Count,
                            m_PortRegisters.PortConnection.PortAddress.Off_High,
                            m_PortRegisters.PortConnection.PortAddress.Off_Low + sizeof(VCOM_PORT));
        
        if(!NT_SUCCESS(status))
        {
            t << "write error b! " << status << EOL;
        }
    }

    return Count;
}
