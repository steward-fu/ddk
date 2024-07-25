// VCom1394Driver.cpp
//
// Requires Compuware's DriverWorks classes
//

#define VDW_MAIN
#include <vdw.h>
#include <initguid.h>
#include "..\VCom1394BusInterface.h"
#include "..\VCom1394PdoInterface.h"
#include "..\VCom1394Common.h"
#include "VCom1394Driver.h"
#include "VCom1394Pdo.h"
#include "VCom1394Bus.h"

#pragma hdrstop("VCom1394.pch")

// Set a default 32-bit tag value to be stored with each heap block
// allocated by operator new. Use BoundsChecker to view the memory pool.
// This value can be overridden using the global function SetPoolTag().
POOLTAG DefaultPoolTag('MOCV');



// Create the global driver trace object
// TODO:    Use KDebugOnlyTrace if you want trace messages
//          to appear only in debug builds.  Use KTrace if
//          you want trace messages to always appear.   
KTrace t("VCom1394");

/////////////////////////////////////////////////////////////////////
// Begin INIT section
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(VCom1394, NULL)

/////////////////////////////////////////////////////////////////////
//  VCom1394::DriverEntry
//
//  Routine Description:
//      This is the first entry point called by the system when the
//      driver is loaded.
// 
//  Parameters:
//      RegistryPath - String used to find driver parameters in the
//          registry.  To locate VCom1394 look for:
//          HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\VCom1394
//
//  Return Value:
//      NTSTATUS - Return STATUS_SUCCESS if no errors are encountered.
//          Any other indicates to the system that an error has occured.
//
//  Comments:
//

NTSTATUS VCom1394::DriverEntry(PUNICODE_STRING RegistryPath)
{
    t << "In DriverEntry Compiled at " __TIME__ " on " __DATE__ "\n";


    // The following macro simply allows compilation at Warning Level 4
    // If you reference this parameter in the function simply remove the macro.
    UNREFERENCED_PARAMETER(RegistryPath);

    m_Unit = 0;

    return STATUS_SUCCESS;
}

// End INIT section
/////////////////////////////////////////////////////////////////////
#pragma code_seg()

/////////////////////////////////////////////////////////////////////
//  VCom1394::AddDevice
//
//  Routine Description:
//      Called when the system detects a device for which this
//      driver is responsible.
//
//  Parameters:
//      Pdo - Physical Device Object. This is a pointer to a system device
//          object that represents the physical device.
//
//  Return Value:
//      NTSTATUS - Success or failure code.
//
//  Comments:
//      This function creates the Functional Device Object, or FDO. The FDO
//      enables this driver to handle requests for the physical device. 
//

NTSTATUS VCom1394::AddDevice(PDEVICE_OBJECT Pdo)
{
    t << "AddDevice called\n";

    // Create the device object. Note that we used a form of "placement" new,
    // that is a member operator of KDevice.  This form will use storage
    // allocated by the system in the device object's device to store our
    // class instance.
    VCom1394Bus * pDevice = new (
            static_cast<PCWSTR>(KUnitizedName(L"VCom1394Bus", m_Unit)),
            FILE_DEVICE_UNKNOWN,
            NULL,
            0,
            DO_POWER_PAGABLE
            )
        VCom1394Bus(Pdo, m_Unit);

    if (pDevice == NULL)
    {
        t << "Error creating device VCom1394Bus"
               << (ULONG) m_Unit << EOL;
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    NTSTATUS status = pDevice->ConstructorStatus();

    if ( !NT_SUCCESS(status) )
    {
        t << "Error constructing device VCom1394Bus"
          << (ULONG) m_Unit << " status " << (ULONG) status << EOL;
        delete pDevice;
    }
    else
    {
        m_Unit++;

        pDevice->ReportNewDevicePowerState(PowerDeviceD0);
    }

    return status;
}
