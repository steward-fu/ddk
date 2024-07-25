
/*++

Copyright (c) 1990, 1991, 1992, 1993, 1994 - 1998  Microsoft Corporation

Module Name:

    kbdclass.c

Abstract:

    Keyboard class driver.

Environment:

    Kernel mode only.

Notes:

    NOTES:  (Future/outstanding issues)

    - Powerfail not implemented.

    - Consolidate common code into a function, where appropriate.

    - Unload not implemented.  We don't want to allow this driver
      to unload.

--*/


#include  "newkbd.h"

GLOBALS Globals;

//
// Use the alloc_text pragma to specify the driver initialization routines
// (they can be paged out).
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT,DriverEntry)
#pragma alloc_text(INIT,KbdConfiguration)
#pragma alloc_text(PAGE,KeyboardClassPassThrough)
#pragma alloc_text(PAGE,KeyboardQueryDeviceKey)
#pragma alloc_text(PAGE,KbdDeterminePortsServiced)
#pragma alloc_text(PAGE,KbdDeviceMapQueryCallback)
#pragma alloc_text(PAGE,KbdSendConnectRequest)
#pragma alloc_text(PAGE,KeyboardAddDevice)
#pragma alloc_text(PAGE,KeyboardAddDeviceEx)
#pragma alloc_text(PAGE,KeyboardClassDeviceControl)
#pragma alloc_text(PAGE,KeyboardSendIrpSynchronously)
#pragma alloc_text(PAGE,KbdCreateClassObject)
#pragma alloc_text(PAGE,KbdSetLights)
#pragma alloc_text(PAGE,KbdFlashLights)
#pragma alloc_text(PAGE,KeyboardClassFindMorePorts)
#pragma alloc_text(PAGE,KeyboardClassEnableGlobalPort)
#pragma alloc_text(PAGE,KeyboardClassPlugPlayNotification)
#pragma alloc_text(PAGE,KeyboardClassSystemControl)
#pragma alloc_text(PAGE,KeyboardClassSetWmiDataItem)
#pragma alloc_text(PAGE,KeyboardClassSetWmiDataBlock)
#pragma alloc_text(PAGE,KeyboardClassQueryWmiDataBlock)
#pragma alloc_text(PAGE,KeyboardClassQueryWmiRegInfo)

#pragma alloc_text(PAGE,KeyboardClassPower)
#pragma alloc_text(PAGE,KeyboardClassCreateWaitWakeIrpWorker)
#pragma alloc_text(PAGE,KeyboardClassCreateWaitWakeIrp)
// #pragma alloc_text(PAGE,KeyboardToggleWaitWakeWorker)
#pragma alloc_text(PAGE,KeyboardClassUnload)
#endif

#define WMI_CLASS_DRIVER_INFORMATION 0
#define WMI_WAIT_WAKE                1

GUID KeyboardClassGuid =  MSKeyboard_ClassInformationGuid;

WMIGUIDREGINFO KeyboardClassWmiGuidList[] =
{
    {
        &KeyboardClassGuid,
        1,
        0 // Keyboard class driver information
    },
    {
        &GUID_POWER_DEVICE_WAKE_ENABLE,
        1,
        0 // wait wake
    }
};

/*
***************************************************************************
**************** Modifications start here *********************************

  
***************************************************************************
***************** Initialization steps ************************************
*																		  *
*  The following steps are necessary modifications to allow the driver to *
*	be able to run DriverWorks classes									  *	
*	               														  *	
*@@@@@@@@@@@@@@@@@@@@@@**STEP_1**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*Replace #include <ntddk.h> with #include <vdw.h>						  *	
*See the top of newkbd.h.												  *
*																		  *
* VDW.h will include all the DriverWorks function prototypes as well as	  *	
* the DDK include files.												  *	  
*																		  * 	
*@@@@@@@@@@@@@@@@@@@@@@**STEP_2**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*Modify project settings												  * 
*Add NTVERSION to C/C++ Preprocessor, PreprocessorSettings				  * 	
*																		  *	
* To include ntddk.h, in step 2, NTVERSION='400'						  *	
* To include wdm.h, set NTVERSION='WDM'									  *	
*    In this case, NTVERSION is 400 because the DDK function originally	  *		
*    included was ntddk.h												  *
*																		  *	
*Add ".,$(DRIVERWORKS)\source,$(DRIVERWORKS)\include," to C/C++           *
*Preprocessor, additional include directories so that the DriverWorks	  *
* header files specified in vdw.h can be included.						  *
*																		  *	
*@@@@@@@@@@@@@@@@@@@@@@@**STEP_3**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*Create an empty function.h file in the source directory.				  *	
*This file will not be used in a non-framework driver, but it is		  *
* included by vdw.h, and will give an error if it doesn't exist.          *			
*@@@@@@@@@@@@@@@@@@@@@@**STEP_4**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*Add Boundschecker init code to DriverEntry								  *	
*Line 244																  *	
*																		  *	
*@@@@@@@@@@@@@@@@@@@@@@**STEP_5**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*Initialize the C++ Run time library in DriverEntry. This will call the	  *	
*constructors for any global variables.                                   *
*Line 252																  *	
*																		  *	 	
*Ordinarily, you would also need to call TerminateCppRunTime() in the	  * 
*unload function.  However, this driver cannot be unloaded making that	  * 
*step unnecessary.														  * 	
*																		  *	
*@@@@@@@@@@@@@@@@@@@@@@**STEP_6**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*  Put an Extern "C" {} around all the function prototypes				  * 	
*  kbdclass.h line 108													  *			
*@@@@@@@@@@@@@@@@@@@@@@**STEP_7**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
*  Fix all necessary typecasts ignored by C.							  *	
*    If a function returns a PVOID,in C, you can just use =.  In C++, you *
*    must use a specific typecast.										  * 
*																		  *
*  Also, in C, it is possible to have a function with a prototype that 	  *
*    has a parameter of type PVOID, while the actual function has the	  *	
*    same parameter defined with a type like PKEYBOARD_STRUCTURE. This	  *
*    is done because functions passed in as parameters must have a		  * 
*    specific parameter list, usually with a PVOID parameter that can	  *	
*    be used to pass in any parameter.  In C++, it is necessary that	  *
*    the function declaration match the prototype.  That is, PVOID		  *
*    parameters must appear as PVOID in the function declaration.		  *	
*																		  *			
***************************************************************************


***************************************************************************
***************** DriverWorks classes added *******************************
*																		  *			
*  DRIVERWORKS CLASS   VARIABLE NAME        FUNCTION MODIFIED			  *	
*  -----------------   -------------        -----------------			  *	
*																		  *			
*  KRegistryKey        ParametersKey        KeyboardAddDevice			  *
*  KRegistryKey        TheRegistryKey       KbdConfiguration			  *	
*																		  *			
*  KUstring            MyRegistryPath	    Global						  *
*  KUstring            MyFullPortName	    DriverEntry					  *
*  KUstring            MyBaseClassName      Global						  *
*  KUstring            MyBasePortName	    DriverEntry					  *	
*  KUstring            ValueName		    KeyboardQueryDeviceKey        *
*  KUstring            MyParametersPath     KbdConfiguration			  *
*  KUstring            MyDefaultUnicodeName KbdConfiguration			  * 	
*  KUstring            MyBasePortName		KeyboardClassFindMorePorts    *
*  KUstring            MyFullPortName		KeyboardClassFindMorePorts    *
*  KUstring            MyStrEnable		    KeyboardToggleWaitWakeWorker  *
*																		  *			
*																		  *			
***************************************************************************

*/

/**********************************************************************************************/
/* 
   Local DriverWorks parameters in DriverEntry
   
   KUstring MyFullPortName
   KUstring MyBasePortName
 
   Global DriverWorks parameters in DriverEntry
 
   KUstring Globals.MyRegistryPath
   KUstring Globals.MyBaseClassName
*/
/**********************************************************************************************/

NTSTATUS DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    This routine initializes the keyboard class driver.

Arguments:

    DriverObject - Pointer to driver object created by system.

    RegistryPath - Pointer to the Unicode name of the registry path
        for this driver.

Return Value:

    The function value is the final status from the initialization operation.

--*/

{
    NTSTATUS                errorCode = STATUS_SUCCESS;
    NTSTATUS                status = STATUS_SUCCESS;
    PDEVICE_EXTENSION       deviceExtension = NULL;
    PDEVICE_OBJECT          classDeviceObject = NULL;
    PIO_ERROR_LOG_PACKET    errorLogEntry;
    ULONG                   dumpCount = 0;
    ULONG                   dumpData[DUMP_COUNT];
    ULONG                   i;
    ULONG                   numPorts;
    ULONG                   uniqueErrorValue;
    KUstring                MyBasePortName;
	KUstring                MyFullPortName;
    PWCHAR                  fullClassName = NULL;
    PFILE_OBJECT            file;

/*	
@@@@@@@@@@@@@@@@@@@@@@@**STEP_4**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
Add BoundsChecker Initialization function
*/
  #if DBG
    // Initialize the connection to BoundsChecker
	BoundsChecker::Init(DriverObject);
    #endif
/*	
@@@@@@@@@@@@@@@@@@@@@@@**STEP_5**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
Initialize the CPP Run time library.  This will call the constructors for
any global variables.
*/
	status = InitializeCppRunTime();	
	if (STATUS_SUCCESS != status) 
	{
		TerminateCppRunTime();
		return status;
	}
/*	
@@@@@@@@@@@@@@@@@@@@@**END_STEPS_4_AND_5**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/



	KbdPrint((1,"\n\nKBDCLASS-KeyboardClassInitialize: enter\n"));

    //
    // Zero-initialize various structures.
    //
    RtlZeroMemory(&Globals, sizeof(GLOBALS));

    Globals.Debug = DEFAULT_DEBUG_LEVEL;


    ExInitializeFastMutex (&Globals.Mutex);
  
	/***********************************************************************
	Initialize KUstring MyBaseClassName.

    This form of Initialize allocates the unicode string's buffer and copies
	  the specified string into it.
	***********************************************************************/
	Globals.MyBaseClassName.Initialize(NAME_MAX*sizeof(WCHAR) , NonPagedPool);
	
	/***********************************************************************
	 Check that the allocation within MyBaseClassName succeeded
	***********************************************************************/
	
	if(!Globals.MyBaseClassName.IsValid())
	{
	   KbdPrint((
            1,
            "KBDCLASS-KeyboardClassInitialize: Couldn't allocate pool for base class name\n"
            ));

        status = STATUS_UNSUCCESSFUL;
        errorCode = KBDCLASS_INSUFFICIENT_RESOURCES;
        uniqueErrorValue = KEYBOARD_ERROR_VALUE_BASE + 2;
        dumpData[0] = (ULONG) RegistryPath->Length + sizeof(UNICODE_NULL);
        dumpCount = 1;
        goto KeyboardClassInitializeExit;

	}
	
	/***********************************************************************
	 Initialize KUstring MyBasePortName.(The same as MyBaseClassName, line 281)
	 ***********************************************************************/
	MyBasePortName.Initialize(NAME_MAX*sizeof(WCHAR), NonPagedPool);
	
  
	//
    // Need to ensure that the registry path is null-terminated.
    // Allocate pool to hold a null-terminated copy of the path.
    //
	/***********************************************************************
	 Initialize KUstring MyRegistryPath.  This form of KUstring::Initialize
	 allocates a buffer of the specified size from the pool specified.
	 The string is initialize as having length 0.
	 ***********************************************************************/
	Globals.MyRegistryPath.Initialize(RegistryPath->Length + sizeof(UNICODE_NULL), NonPagedPool);
    /***********************************************************************
	 KUstring::IsValid - Check that the string's buffer was allocated.
	 ***********************************************************************/
	if(!Globals.MyRegistryPath.IsValid()){
        KbdPrint((
            1,
            "KBDCLASS-KeyboardClassInitialize: Couldn't allocate pool for registry path\n"
            ));

        status = STATUS_UNSUCCESSFUL;
        errorCode = KBDCLASS_INSUFFICIENT_RESOURCES;
        uniqueErrorValue = KEYBOARD_ERROR_VALUE_BASE + 2;
        dumpData[0] = (ULONG) RegistryPath->Length + sizeof(UNICODE_NULL);
        dumpCount = 1;
        goto KeyboardClassInitializeExit;

    }
    /***********************************************************************
	KUstring::Assign copies the string provided into the unicode string's 
	 buffer and sets the length of the string appopriately.
	***********************************************************************/
	Globals.MyRegistryPath.Assign(RegistryPath->Buffer);
 
    //
    // Get the configuration information for this driver.
    //

    KbdConfiguration();

    //
    // If there is only one class device object then create it as the grand
    // master device object.  Otherwise let all the FDOs also double as the
    // class DO.
    //
    if (!Globals.ConnectOneClassToOnePort) {
        status = KbdCreateClassObject (DriverObject,
                                       &Globals.InitExtension,
                                       &classDeviceObject,
                                       &fullClassName,
                                       TRUE);
        if (!NT_SUCCESS (status)) {
            goto KeyboardClassInitializeExit;
        }
        deviceExtension = (PDEVICE_EXTENSION)classDeviceObject->DeviceExtension;
        Globals.GrandMaster = deviceExtension;
        deviceExtension->PnP = FALSE;
        KeyboardAddDeviceEx (deviceExtension, fullClassName, NULL);

        ASSERT (NULL != fullClassName);
        ExFreePool (fullClassName);
        fullClassName = NULL;

        classDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    }

    //
    // Set up the base device name for the associated port device.
    // It is the same as the base class name, with "Class" replaced
    // by "Port".
    //
    
    /***********************************************************************
	KUstring::Assign (same as line 341 above)
	***********************************************************************/
	MyBasePortName.Assign(Globals.MyBaseClassName);
    /***********************************************************************
	KUstring::ShortenBy shortens the length of the unicode string by the 
	  specified number of charecters.  In this case, it is chopping the 
	  word 'Class' off the end of the string.
	***********************************************************************/
	MyBasePortName.ShortenBy(5);     
    /***********************************************************************
	KUstring::Append copies the string specified into the end of the  
	  unicode string's buffer and adjusts the string's length accordingly. 
	***********************************************************************/
	MyBasePortName.Append(L"Port");
	
    //
    // Determine how many (static) ports this class driver is to service.
    //
    //
    // If this returns zero, then all ports will be dynamically PnP added later
    //
    
	/***********************************************************************
	KUstring::UnicodeString - Return a pointer to a Unicode string to be passed
	into DDK functions such as this one.
	***********************************************************************/
	KbdDeterminePortsServiced(&MyBasePortName.UnicodeString(), &numPorts);

    ASSERT (numPorts <= MAXIMUM_PORTS_SERVICED);

    KbdPrint((
        1,
        "KBDCLASS-KeyboardClassInitialize: Will service %d port devices\n",
        numPorts
        ));

    //
    // Set up space for the class's full device object name.
    //
    
	/***********************************************************************
	KUstring::Initialize (same as line 319)
	***********************************************************************/
	MyFullPortName.Initialize(sizeof(L"\\Device\\") + MyBasePortName.Size() + sizeof(UNICODE_NULL),
		                    NonPagedPool);
	/***********************************************************************
	check that the allocation succeeded
	***********************************************************************/
    if(!MyFullPortName.IsValid()){

        KbdPrint((
            1,
            "KBDCLASS-KeyboardClassInitialize: Couldn't allocate string for device object name\n"
            ));

        status = STATUS_UNSUCCESSFUL;
        errorCode = KBDCLASS_INSUFFICIENT_RESOURCES;
        uniqueErrorValue = KEYBOARD_ERROR_VALUE_BASE + 6;
        dumpData[0] = MyFullPortName.MaximumSize();
		dumpCount = 1;
        goto KeyboardClassInitializeExit;

    }

    
	/***********************************************************************
	KUstring::Assign (same as line 341 above)
	***********************************************************************/
	MyFullPortName.Assign(L"\\Device\\");
	/***********************************************************************
	KUstring::Append (same as line 395 above)
	***********************************************************************/
	MyFullPortName.Append(MyBasePortName);
    MyFullPortName.Append(L"0");

    //
    // Set up the class device object(s) to handle the associated
    // port devices.
    //
    for (i = 0; (i < Globals.PortsServiced) && (i < numPorts); i++) {

        //
        // Append the suffix to the device object name string.  E.g., turn
        // \Device\KeyboardClass into \Device\KeyboardClass0.  Then attempt
        // to create the device object.  If the device object already
        // exists increment the suffix and try again.
        //

		/***********************************************************************
		KUstring::Last returns a pointer to the last charector in the unicode 
		  string.  Here, we are setting the number at the end of the string to i.
		***********************************************************************/
		MyFullPortName.Last() = L'0' + (WCHAR) i;

        //
        // Create the class device object.
        //
        status = KbdCreateClassObject (DriverObject,
                                       &Globals.InitExtension,
                                       &classDeviceObject,
                                       &fullClassName,
                                       TRUE);

        if (!NT_SUCCESS(status)) {
            errorCode = KBDCLASS_INSUFFICIENT_RESOURCES;
            uniqueErrorValue = KEYBOARD_ERROR_VALUE_BASE + 8;
            dumpCount = 0;
            break;
        }

        deviceExtension = (PDEVICE_EXTENSION)classDeviceObject->DeviceExtension;
        deviceExtension->PnP = FALSE;

        classDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

        //
        // Connect to the port device.
        //
		/***********************************************************************
		KUstring::UnicodeString (same as line 407)
		***********************************************************************/
		status = IoGetDeviceObjectPointer (&MyFullPortName.UnicodeString(),
                                           FILE_READ_ATTRIBUTES,
                                           &file,
                                           &deviceExtension->TopPort);

        if (status != STATUS_SUCCESS) {
            // ISSUE: log error
            // ISSUE: get rid of the device object.
            break;
        }

        classDeviceObject->StackSize = 1 + deviceExtension->TopPort->StackSize;
        status = KeyboardAddDeviceEx (deviceExtension, fullClassName, file);

        if (fullClassName) {
            ExFreePool (fullClassName);
            fullClassName = NULL;
        }

        if (!NT_SUCCESS (status)) {
            // ISSUE: log error
            break;
        }
    } // for
    Globals.NumberLegacyPorts = i;

KeyboardClassInitializeExit:

    if (errorCode != STATUS_SUCCESS) {

        //
        // The initialization failed in some way.  Log an error.
        //

        errorLogEntry = (PIO_ERROR_LOG_PACKET)
            IoAllocateErrorLogEntry(
                (classDeviceObject == NULL) ?
                    (PVOID) DriverObject : (PVOID) classDeviceObject,
                (UCHAR) (sizeof(IO_ERROR_LOG_PACKET)
                         + (dumpCount * sizeof(ULONG)))
                );

        if (errorLogEntry != NULL) {

            errorLogEntry->ErrorCode = errorCode;
            errorLogEntry->DumpDataSize = (USHORT) (dumpCount * sizeof(ULONG));
            errorLogEntry->SequenceNumber = 0;
            errorLogEntry->MajorFunctionCode = 0;
            errorLogEntry->IoControlCode = 0;
            errorLogEntry->RetryCount = 0;
            errorLogEntry->UniqueErrorValue = uniqueErrorValue;
            errorLogEntry->FinalStatus = status;
            for (i = 0; i < dumpCount; i++)
                errorLogEntry->DumpData[i] = dumpData[i];

            IoWriteErrorLogEntry(errorLogEntry);
        }

        //
        // ISSUE: free any device objects and their associated memory
        //
    }

    //
    // Free the unicode strings.
    //

	/***********************************************************************
	KUstring::IsValid-Check that the unicode string was allocated
	KUstring::Invalidate-Free the memory allocated for the string
	This is not completely necessary as it will be done in the destructor
	as soon as the function exits.
	***********************************************************************/
	if(MyFullPortName.IsValid())
		MyFullPortName.Invalidate();

    if (fullClassName) {
        ExFreePool (fullClassName);
    }

    if (NT_SUCCESS (status)) {

        IoRegisterDriverReinitialization(DriverObject,
                                         KeyboardClassFindMorePorts,
                                         NULL);

        //
        // Set up the device driver entry points.
        //

        DriverObject->DriverStartIo = KeyboardClassStartIo;
        DriverObject->MajorFunction[IRP_MJ_CREATE]         = KeyboardClassCreate;
        DriverObject->MajorFunction[IRP_MJ_CLOSE]          = KeyboardClassClose;
        DriverObject->MajorFunction[IRP_MJ_READ]           = KeyboardClassRead;
        DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS]  = KeyboardClassFlush;
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KeyboardClassDeviceControl;
        DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] =
                                                             KeyboardClassPassThrough;
        DriverObject->MajorFunction[IRP_MJ_CLEANUP]        = KeyboardClassCleanup;
        DriverObject->MajorFunction[IRP_MJ_PNP]            = KeyboardPnP;
        DriverObject->MajorFunction[IRP_MJ_POWER]          = KeyboardClassPower;
        DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = KeyboardClassSystemControl;
        DriverObject->DriverExtension->AddDevice           = KeyboardAddDevice;

        //
        // NOTE: Don't allow this driver to unload.  Otherwise, we would set
        // DriverObject->DriverUnload = KeyboardClassUnload.
        //
        status = STATUS_SUCCESS;

    } else {
        //
        // ISSUE: memory leaks
        //
        if (Globals.MyRegistryPath.IsValid())
	         Globals.MyRegistryPath.Invalidate();
        
    }

    KbdPrint((1,"KBDCLASS-KeyboardClassInitialize: exit\n"));
    return(status);

}

/**********************************************************************************************/
/* 
   Local DriverWorks parameters in KeyboardQueryDeviceKey
   
   KUstring valueName
   
   Global DriverWorks parameters in DriverEntry
 
   --none--
*/
/**********************************************************************************************/


NTSTATUS
KeyboardQueryDeviceKey (
    IN  HANDLE  Handle,
    IN  PWCHAR  ValueNameString,
    OUT PVOID   Data,
    IN  ULONG   DataLength
    )
{
    NTSTATUS        status;
    ULONG           length;
    PKEY_VALUE_FULL_INFORMATION fullInfo;

    PAGED_CODE();

    /***********************************************************************
	KUstring::KUstring This function allocates space for the buffer, and 
	copies the supplied string into it.  It sets the length of the string
	appropriately.
	***********************************************************************/
	KUstring valueName(ValueNameString, PagedPool);
    /***********************************************************************
	KUstring::IsValid (same as line 322 above)
	***********************************************************************/
	if(valueName.IsValid())
	{
		length = sizeof (KEY_VALUE_FULL_INFORMATION)
           + valueName.Size()
           + DataLength;

		fullInfo = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool (PagedPool, length);

		/***********************************************************************
		KUstring::UnicodeString (same as line 407)
		***********************************************************************/
		if (fullInfo) {
			status = ZwQueryValueKey (Handle,
                                  &valueName.UnicodeString(),
                                  KeyValueFullInformation,
                                  fullInfo,
                                  length,
                                  &length);

			if (NT_SUCCESS (status)) {
				ASSERT (DataLength == fullInfo->DataLength);
				RtlCopyMemory (Data,
                           ((PUCHAR) fullInfo) + fullInfo->DataOffset,
                           fullInfo->DataLength);
			}


			ExFreePool (fullInfo);
		} else {
			status = STATUS_NO_MEMORY;
		}
	/***********************************************************************
	KUstring::Invalidate -free the memory used for the string buffer
	***********************************************************************/
	valueName.Invalidate();
	}

    return status;
}

/**********************************************************************************************/
/* 
   Local DriverWorks parameters in KbdConfiuration
   
   KRegistryKey TheRegistryKey;
   KUstring MyParametersPath
   KUstring MyDefaultUnicodeName
 
   Global DriverWorks parameters in KbdConfiguration
 
   KUstring Globals.MyRegistryPath
   KUstring Globals.MyBaseClassName
   
*/
/**********************************************************************************************/

VOID
KbdConfiguration()

/*++

Routine Description:

    This routine stores the configuration information for this device.

Return Value:

    None.  As a side-effect, sets fields in
    DeviceExtension->KeyboardAttributes.

--*/

{
    PRTL_QUERY_REGISTRY_TABLE parameters = NULL;
    ULONG defaultDataQueueSize = DATA_QUEUE_SIZE;
    ULONG defaultMaximumPortsServiced = 1;
    ULONG defaultConnectMultiplePorts = 1;
    ULONG defaultSendOutputToAllPorts = 0;
    NTSTATUS status = STATUS_SUCCESS;
    KUstring MyParametersPath;
	KUstring MyDefaultUnicodeName(DD_KEYBOARD_CLASS_BASE_NAME_U, NonPagedPool); 
    USHORT queriesPlusOne = 6;
	
	ULONG length, temp;

    PAGED_CODE ();

    //
    // Allocate the Rtl query table.
    //

    parameters = (PRTL_QUERY_REGISTRY_TABLE)ExAllocatePool(
                     PagedPool,
                     sizeof(RTL_QUERY_REGISTRY_TABLE) * queriesPlusOne
                     );

    if (!parameters) {

        KbdPrint((
            1,
            "KBDCLASS-KbdConfiguration: Couldn't allocate table for Rtl query to parameters \n"));

        status = STATUS_UNSUCCESSFUL;

    } else {

        RtlZeroMemory(
            parameters,
            sizeof(RTL_QUERY_REGISTRY_TABLE) * queriesPlusOne
            );

        //
        // Form a path to this driver's Parameters subkey.
        //

		/***********************************************************************
		KUstring::Initialize (same as line 407)
		***********************************************************************/
		MyParametersPath.Initialize(Globals.MyRegistryPath.Size() + sizeof(L"\\Parameters"),
			                        PagedPool);
		/***********************************************************************
		KUstring::IsValid (same as line 322)
		***********************************************************************/
		if(!MyParametersPath.IsValid()){

            KbdPrint((
                1,
                "KBDCLASS-KbdConfiguration: Couldn't allocate string for path to parameters\n"));

            status = STATUS_UNSUCCESSFUL;
		}

    }

    if (NT_SUCCESS(status)) {

        //
        // Form the parameters path.
        //
        /***********************************************************************
		KUstring::Assign (same as line 340)
		***********************************************************************/
		MyParametersPath.Assign(Globals.MyRegistryPath);
     	/***********************************************************************
		KUstring::Append (same as line 394)
		***********************************************************************/
		MyParametersPath.Append(L"\\Parameters");

        //
        // Gather all of the "user specified" information from
        // the registry.
        //
        /***********************************************************************
		KRegistryKey::KRegistryKey -Opens the registry key with the path specified
		   by the unicode string passed in.
		***********************************************************************/
		KRegistryKey  TheRegistryKey(&MyParametersPath.UnicodeString());		                          
        if(TheRegistryKey.IsValid())
		{
		/***********************************************************************
		KRegistryKey::QueryValue - This form is specific to DWORD values.  It
		  returns the value of the key specified in the buffer specified in the
		  second parameter.
		***********************************************************************/
		status = TheRegistryKey.QueryValue(L"KeyboardDataQueueSize", 
			    &Globals.InitExtension.KeyboardAttributes.InputDataQueueLength);
		if(!NT_SUCCESS(status))
			Globals.InitExtension.KeyboardAttributes.InputDataQueueLength =
				defaultDataQueueSize;
        /***********************************************************************
		KRegistryKey::QueryValue (same as line 826 above) 
		***********************************************************************/
		status = TheRegistryKey.QueryValue(L"MaximumPortsServiced", &Globals.PortsServiced);
		if(!NT_SUCCESS(status))
			Globals.PortsServiced = defaultMaximumPortsServiced;
        
		/***********************************************************************
		KUstring::MaximumSize - returns the Maximum size of the unicode string's
		  buffer.  That is the number of characters multiplied by the size of a 
		  character.
		***********************************************************************/
		length = MyDefaultUnicodeName.MaximumSize();
		/***********************************************************************
		KRegistryKey::QueryValue  - This is the general form.  The type of key
		  is specifed.  The value of the key is returned in the buffer provided if
		  the types match and the buffer is long enough.  In this case, we are 
		  retrieving a string value.
		KUstring::UnicodeString (Same as line 407)
		***********************************************************************/
		status = TheRegistryKey.QueryValue(L"KeyboardDeviceBaseName",
			                                 &temp, //type
											 length,                    
											 Globals.MyBaseClassName.UnicodeString().Buffer);
		/***********************************************************************
		KUstring::Assign (same as line 340)
		***********************************************************************/
		if(!NT_SUCCESS(status))
			Globals.MyBaseClassName.Assign(MyDefaultUnicodeName);
		
		/***********************************************************************
		KRegistryKey::QueryValue (same as line 826)
		***********************************************************************/
		status = TheRegistryKey.QueryValue(L"ConnectMultiplePorts", &Globals.ConnectOneClassToOnePort);
		if(!NT_SUCCESS(status))
			Globals.ConnectOneClassToOnePort = defaultConnectMultiplePorts;
        
		/***********************************************************************
		KRegistryKey::QueryValue (same as line 826)
		***********************************************************************/
		status = TheRegistryKey.QueryValue(L"SendOutputToAllPorts", &Globals.SendOutputToAllPorts);
		if(!NT_SUCCESS(status))
		   Globals.SendOutputToAllPorts = defaultSendOutputToAllPorts;
        
		}
		else //Couldn't open registry key
			status = STATUS_UNSUCCESSFUL;
	}

    if (!NT_SUCCESS(status)) {

        //
        // Go ahead and assign driver defaults.
        //

        Globals.InitExtension.KeyboardAttributes.InputDataQueueLength =
            defaultDataQueueSize;
        Globals.PortsServiced = defaultMaximumPortsServiced;
        Globals.ConnectOneClassToOnePort = defaultConnectMultiplePorts;
        Globals.SendOutputToAllPorts = defaultSendOutputToAllPorts;
        /***********************************************************************
		KUstring::Assign (same as line 340)
		***********************************************************************/
		Globals.MyBaseClassName.Assign(MyDefaultUnicodeName);
    }

	/***********************************************************************
	KUstring::UnicodeString (same as line 407)
	***********************************************************************/
	KbdPrint((
        1,
        "KBDCLASS-KbdConfiguration: Keyboard class base name = %ws\n",
        Globals.MyBaseClassName.UnicodeString().Buffer
        ));

    if (Globals.InitExtension.KeyboardAttributes.InputDataQueueLength == 0) {

        KbdPrint((
            1,
            "KBDCLASS-KbdConfiguration: overriding KeyboardInputDataQueueLength = 0x%x\n",
            Globals.InitExtension.KeyboardAttributes.InputDataQueueLength
            ));

        Globals.InitExtension.KeyboardAttributes.InputDataQueueLength =
            defaultDataQueueSize;
    }

    Globals.InitExtension.KeyboardAttributes.InputDataQueueLength *=
        sizeof(KEYBOARD_INPUT_DATA);

    KbdPrint((
        1,
        "KBDCLASS-KbdConfiguration: KeyboardInputDataQueueLength = 0x%x\n",
        Globals.InitExtension.KeyboardAttributes.InputDataQueueLength
        ));

    KbdPrint((
        1,
        "KBDCLASS-KbdConfiguration: MaximumPortsServiced = %d\n",
        Globals.PortsServiced
        ));

    //
    // Invert the flag that specifies the type of class/port connections.
    // We used it in the RtlQuery call in an inverted fashion.
    //

    Globals.ConnectOneClassToOnePort = !Globals.ConnectOneClassToOnePort;

    KbdPrint((
        1,
        "KBDCLASS-KbdConfiguration: Connection Type = %d\n",
        Globals.ConnectOneClassToOnePort
        ));

    //
    // Free the allocated memory before returning.
    //

	/***********************************************************************
	KUstring::IsValid/Invalidate (same as line 575)
	***********************************************************************/
	if(MyParametersPath.IsValid())
		MyParametersPath.Invalidate();
    if (parameters)
        ExFreePool(parameters);

}


/**********************************************************************************************/
/* 
   Local DriverWorks parameters in KeyboardClassFindMorePorts
   
   KUstring MyBasePortName
   KUstring MyFullPortName
 
   Global DriverWorks parameters in KeyboardClassFindMorePorts
 
   KUstring Globals.MyBaseClassName
   
*/
/************************************************************************************************/

VOID
KeyboardClassFindMorePorts (
    PDRIVER_OBJECT  DriverObject,
    PVOID           Context,
    ULONG           Count
    )
/*++

Routine Description:

    This routine is called from
    serviced by the boot device drivers and then called again by the
    IO system to find disk devices serviced by nonboot device drivers.

Arguments:

    DriverObject
    Context -
    Count - Used to determine if this is the first or second time called.

Return Value:

    None

--*/
{
    NTSTATUS                errorCode = STATUS_SUCCESS;
    NTSTATUS                status;
    PDEVICE_EXTENSION       deviceExtension = NULL;
    PDEVICE_OBJECT          classDeviceObject = NULL;
    PIO_ERROR_LOG_PACKET    errorLogEntry;
    ULONG                   dumpCount = 0;
    ULONG                   dumpData[DUMP_COUNT];
    ULONG                   i;
    ULONG                   uniqueErrorValue;
    ULONG                   numPorts;
    KUstring                MyBasePortName(NAME_MAX *sizeof(WCHAR), PagedPool);
	KUstring                MyFullPortName;
    PWCHAR                  fullClassName = NULL;
    PFILE_OBJECT            file;

    PAGED_CODE ();
    //
    // Set up the base device name for the associated port device.
    // It is the same as the base class name, with "Class" replaced
    // by "Port".
    //
  
    /***********************************************************************
	KUstring::Assign (same as line 340)
	***********************************************************************/
	MyBasePortName.Assign(Globals.MyBaseClassName);
	/***********************************************************************
	KUstring::ShortenBy (same as line 389)
	***********************************************************************/
	MyBasePortName.ShortenBy(5);
	/***********************************************************************
	KUstring::Append (same as line 394)
	***********************************************************************/
	MyBasePortName.Append(L"Port");

    //
    // Set up space for the full device object name for the ports.
    //
    
	/***********************************************************************
	KUstring::Initialize (same as line 318)
	***********************************************************************/
	MyFullPortName.Initialize( sizeof(L"\\Device\\")
                               + MyBasePortName.Size()
                               + sizeof (UNICODE_NULL),
							   PagedPool);

    /***********************************************************************
	KUstring::IsValid (same as line 322)
	***********************************************************************/
	if(!MyFullPortName.IsValid()){

        KbdPrint((
            1,
            "KBDCLASS-KeyboardClassInitialize: Couldn't allocate string for port device object name\n"
            ));

        status = STATUS_UNSUCCESSFUL;
        errorCode = KBDCLASS_INSUFFICIENT_RESOURCES;
        uniqueErrorValue = KEYBOARD_ERROR_VALUE_BASE + 8;
        dumpData[0] = MyFullPortName.MaximumSize();
        dumpCount = 1;
        goto KeyboardFindMorePortsExit;

    }
	/***********************************************************************
	KUstring::Assign (same as line 340)
	***********************************************************************/
	MyFullPortName.Assign(L"\\Device\\");
    /***********************************************************************
	KUstring::Append (same as line 394)
	***********************************************************************/
	MyFullPortName.Append(MyBasePortName);
	MyFullPortName.Append(L"0");
	/***********************************************************************
	KUstring::UnicodeString (same as line 407)
	***********************************************************************/
	KbdDeterminePortsServiced(&MyBasePortName.UnicodeString(), &numPorts);

    //
    // Set up the class device object(s) to handle the associated
    // port devices.
    //

    for (i = Globals.NumberLegacyPorts;
         ((i < Globals.PortsServiced) && (i < numPorts));
         i++) {

        //
        // Append the suffix to the device object name string.  E.g., turn
        // \Device\PointerClass into \Device\PointerClass0.  Then attempt
        // to create the device object.  If the device object already
        // exists increment the suffix and try again.
        //

      

	   /***********************************************************************
 	   KUstring::Last (same as line 473)
	   ***********************************************************************/
	   MyFullPortName.Last() = L'0' + (WCHAR) i; 
        //
        // Create the class device object.
        //
        status = KbdCreateClassObject (DriverObject,
                                       &Globals.InitExtension,
                                       &classDeviceObject,
                                       &fullClassName,
                                       TRUE);

        if (!NT_SUCCESS(status)) {
            errorCode = KBDCLASS_INSUFFICIENT_RESOURCES;
            uniqueErrorValue = KEYBOARD_ERROR_VALUE_BASE + 8;
            dumpCount = 0;
            break;
        }

        deviceExtension = (PDEVICE_EXTENSION)classDeviceObject->DeviceExtension;
        deviceExtension->PnP = FALSE;

        //
        // Connect to the port device.
        //
		/***********************************************************************
		KUstring::UnicodeString (same as line 407)
		***********************************************************************/
	    status = IoGetDeviceObjectPointer (&MyFullPortName.UnicodeString(),
                                           FILE_READ_ATTRIBUTES,
                                           &file,
                                           &deviceExtension->TopPort);

        if (status != STATUS_SUCCESS) {
            // ISSUE: Delete Device object
            // ISSUE: log error
            break;
        }

        classDeviceObject->StackSize = 1 + deviceExtension->TopPort->StackSize;
        status = KeyboardAddDeviceEx (deviceExtension, fullClassName, file);
        classDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

        if (fullClassName) {
            ExFreePool (fullClassName);
            fullClassName = NULL;
        }

        if (!NT_SUCCESS (status)) {
            // ISSUE: log error
            break;
        }
    } // for
    Globals.NumberLegacyPorts = i;

KeyboardFindMorePortsExit:

    if (errorCode != STATUS_SUCCESS) {

        //
        // The initialization failed in some way.  Log an error.
        //

        errorLogEntry = (PIO_ERROR_LOG_PACKET)
            IoAllocateErrorLogEntry(
                (classDeviceObject == NULL) ?
                    (PVOID) DriverObject : (PVOID) classDeviceObject,
                (UCHAR) (sizeof(IO_ERROR_LOG_PACKET)
                         + (dumpCount * sizeof(ULONG)))
                );

        if (errorLogEntry != NULL) {

            errorLogEntry->ErrorCode = errorCode;
            errorLogEntry->DumpDataSize = (USHORT) (dumpCount * sizeof(ULONG));
            errorLogEntry->SequenceNumber = 0;
            errorLogEntry->MajorFunctionCode = 0;
            errorLogEntry->IoControlCode = 0;
            errorLogEntry->RetryCount = 0;
            errorLogEntry->UniqueErrorValue = uniqueErrorValue;
            errorLogEntry->FinalStatus = status;
            for (i = 0; i < dumpCount; i++)
                errorLogEntry->DumpData[i] = dumpData[i];

            IoWriteErrorLogEntry(errorLogEntry);
        }

        //
        // ISSUE: free any device objects and their associated memory
        //
    }

    //
    // Free the unicode strings.
    //
    /***********************************************************************
	KUstring::IsValid/Invalidate (same as line 575)
	***********************************************************************/
	if(MyFullPortName.IsValid())
		MyFullPortName.Invalidate();
	if(MyBasePortName.IsValid())
		MyBasePortName.Invalidate();

    if (fullClassName) {
        ExFreePool (fullClassName);
    }
}

/**********************************************************************************************/
/* 
   Local DriverWorks parameters in KeyboardToggleWaitWakeWorker
   
   KUstring MyStrEnable
  
   Global DriverWorks parameters in KeyboardToggleWaitWakeWorker
 
   --none--
   
*/
/************************************************************************************************/

VOID
KeyboardToggleWaitWakeWorker(
    PKEYBOARD_WORK_ITEM Item
    )
/*++

Routine Description:

--*/
{
    PDEVICE_EXTENSION   data;
    PIRP                waitWakeIrp = NULL;
    KIRQL               irql;
    BOOLEAN             wwState = Item->WaitWakeState ? TRUE : FALSE;
    BOOLEAN             toggled = FALSE;


    //
    // Can't be paged b/c we are using spin locks
    //
    // PAGED_CODE ();

    data = Item->Data;

    KeAcquireSpinLock (&data->WaitWakeSpinLock, &irql);

    if (wwState != data->WaitWakeEnabled) {
        toggled = TRUE;
        if (data->WaitWakeEnabled) {
            waitWakeIrp = (PIRP)
                InterlockedExchangePointer ((PVOID*)&data->WaitWakeIrp, NULL);
        }
        
        data->WaitWakeEnabled = wwState;
    }

    KeReleaseSpinLock (&data->WaitWakeSpinLock, irql);

    if (toggled) {
        /***********************************************************************
		KUstring::KUstring (same as line 654)
		***********************************************************************/
		KUstring       MyStrEnable(KEYBOARD_WAIT_WAKE_ENABLE);
        HANDLE         devInstRegKey;
        ULONG          tmp = wwState;
        //
        // write the value out to the registry
        //
        if ((NT_SUCCESS(IoOpenDeviceRegistryKey (data->PDO,
                                                 PLUGPLAY_REGKEY_DEVICE,
                                                 STANDARD_RIGHTS_ALL,
                                                 &devInstRegKey)))) {

            /***********************************************************************
			KUstring::UnicodeString (same as line 407)
			***********************************************************************/
			ZwSetValueKey (devInstRegKey,
                           &MyStrEnable.UnicodeString(),
                           0,
                           REG_DWORD,
                           &tmp,
                           sizeof(tmp));

            ZwClose (devInstRegKey);
        }
    }

    if (toggled && wwState) {
        //
        // wwState is our new state, so WW was just turned on
        //
        KeyboardClassCreateWaitWakeIrp (data);
    }

    //
    // If we have an IRP, then WW has been toggled off, otherwise, if toggled is
    // TRUE, we need to save this in the reg and, perhaps, send down a new WW irp
    //
    if (waitWakeIrp) {
        IoCancelIrp (waitWakeIrp);
    }

    IoReleaseRemoveLock (&data->RemoveLock, KeyboardToggleWaitWakeWorker);
    ExFreePool(Item);
}

/**********************************************************************************************/
/* 
   Local DriverWorks parameters in KeyboardAddDevice
   
   KRegistryKey ParametersKey
   KUstring     MyParameters
  
   Global DriverWorks parameters in KeyboardAddDevice
 
   KUstring Globals.MyRegistryPath
   
*/
/************************************************************************************************/



NTSTATUS
KeyboardAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++
Description:
    The plug play entry point "AddDevice"

--*/
{
    NTSTATUS            status;
    PDEVICE_OBJECT      fdo;
    PDEVICE_EXTENSION   port;
    PWCHAR              fullClassName = NULL;
    POWER_STATE         state;
    HANDLE              devInstRegKey;
    ULONG               tmp;

    PAGED_CODE ();

    status = KbdCreateClassObject (DriverObject,
                                   &Globals.InitExtension,
                                   &fdo,
                                   &fullClassName,
                                   FALSE);

    if (!NT_SUCCESS (status)) {
        return status;
    }

    port = (PDEVICE_EXTENSION) fdo->DeviceExtension;
    port->TopPort = IoAttachDeviceToDeviceStack (fdo, PhysicalDeviceObject);
    port->PDO = PhysicalDeviceObject;
    port->PnP = TRUE;
    port->Started = FALSE;
    port->DeviceState = PowerDeviceD0;
    port->SystemState = PowerSystemWorking;

    state.DeviceState = PowerDeviceD0;
    PoSetPowerState (fdo, DevicePowerState, state);

    port->MinDeviceWakeState = PowerDeviceUnspecified;
    port->MinSystemWakeState = PowerSystemUnspecified;
    port->PowerLightTime = 0;
    port->WaitWakeEnabled = FALSE;

    status = IoOpenDeviceRegistryKey (PhysicalDeviceObject,
                                      PLUGPLAY_REGKEY_DEVICE,
                                      STANDARD_RIGHTS_ALL,
                                      &devInstRegKey);

    if (NT_SUCCESS (status)) {
        status = KeyboardQueryDeviceKey (devInstRegKey,
                                         KEYBOARD_POWER_LIGHT_TIME,
                                         &tmp,
                                         sizeof (tmp));
        if (NT_SUCCESS (status)) {
            port->PowerLightTime = tmp;
        }
        status = KeyboardQueryDeviceKey (devInstRegKey,
                                         KEYBOARD_WAIT_WAKE_ENABLE,
                                         &tmp,
                                         sizeof (tmp));
        if (NT_SUCCESS (status)) {
            port->WaitWakeEnabled = (tmp ? TRUE : FALSE);
        }
        ZwClose(devInstRegKey);
    }

    port->AllowDisable  = FALSE;
    
	/***********************************************************************
	KUstring::KUstring (same as line 654)
	***********************************************************************/
	KUstring     MyParameters(L"Parameters");
	/***********************************************************************
	KRegistryKey::KRegistryKey - This form of the constructor opens a key 
	  given a root directory and a path.  To find this key, navigate to the 
	  root directory and from there, navigate through any keys specified in
	  the path.
	***********************************************************************/
	KRegistryKey ParametersKey(&Globals.MyRegistryPath.UnicodeString(), //Root directory
		                       &MyParameters.UnicodeString()            //path
		                       );
	/***********************************************************************
	KRegistryKey::IsValid - Was the registry key opened
	***********************************************************************/
	if(ParametersKey.IsValid())
		{
		/***********************************************************************
		KRegistryKey::QueryValue (same as line 826)
		***********************************************************************/
		status = ParametersKey.QueryValue(KEYBOARD_ALLOW_DISABLE, &tmp);
		    if (NT_SUCCESS (status)) 
                port->AllowDisable = (tmp ? TRUE : FALSE);
        }
	

    fdo->Flags |= DO_POWER_PAGABLE;
    fdo->Flags &= ~DO_DEVICE_INITIALIZING;

    status = IoRegisterDeviceInterface (PhysicalDeviceObject,
                                        (LPGUID)&GUID_CLASS_KEYBOARD,
                                        NULL,
                                        &port->SymbolicLinkName );

    if (!NT_SUCCESS (status)) {
        IoDeleteDevice (fdo);
    } else {
        status = KeyboardAddDeviceEx (port, fullClassName, NULL);
    }

    if (fullClassName) {
        ExFreePool(fullClassName);
    }

    return status;
}

/*************************************************************************************************/
/* End of modified functions
 
   Note: Some of the global parameters were used outside these function

   Globals.MyRegistryPath was also used in KeyboardClassUnload and KeyboardClassQueryWmiRegInfo
   Globals.MyBaseClassName was also used in KeyboardAddDeviceEx and KbdCreateClassObject

*/
/*************************************************************************************************/




NTSTATUS
KeyboardClassPassThrough(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
        )
/*++
Routine Description:

        Passes a request on to the lower driver.

--*/
{
        //
        // Pass the IRP to the target
        //
    IoSkipCurrentIrpStackLocation (Irp);
        return IoCallDriver (
        ((PDEVICE_EXTENSION) DeviceObject->DeviceExtension)->TopPort,
                Irp);
}


NTSTATUS
KeyboardAddDeviceEx(
    IN PDEVICE_EXTENSION    ClassData,
    IN PWCHAR               FullClassName,
    IN PFILE_OBJECT         File
    )
 /*++ Description:
  *
  * Called whenever the Keyboard Class driver is loaded to control a device.
  *
  * Two possible reasons.
  * 1) Plug and Play found a PNP enumerated keyboard port.
  * 2) Driver Entry found this device via old crusty legacy reasons.
  *
  * Arguments:
  *
  *
  * Return:
  *
  * STATUS_SUCCESS - if successful STATUS_UNSUCCESSFUL - otherwise
  *
  * --*/
{
    NTSTATUS                errorCode = STATUS_SUCCESS;
    NTSTATUS                status = STATUS_SUCCESS;
    PDEVICE_EXTENSION       trueClassData;
    PPORT                   classDataList;
    ULONG                   uniqueErrorValue = 0;
    PIO_ERROR_LOG_PACKET    errorLogEntry;
    ULONG                   dumpCount = 0;
    ULONG                   dumpData[DUMP_COUNT];
    ULONG                   i;

    PAGED_CODE ();

    ExInitializeFastMutex (&ClassData->CleanupMutex);
    KeInitializeSpinLock (&ClassData->WaitWakeSpinLock);

    if (Globals.ConnectOneClassToOnePort) {

        ASSERT (NULL == Globals.GrandMaster);
        trueClassData = ClassData;

    } else {
        trueClassData = Globals.GrandMaster;
    }
    ClassData->TrueClassDevice = trueClassData->Self;

    if ((Globals.GrandMaster != ClassData) &&
        (Globals.GrandMaster == trueClassData)) {
        //
        // We have a grand master, and are adding a port device object.
        //

        //
        // Connect to port device.
        //
        status = KbdSendConnectRequest(ClassData, KeyboardClassServiceCallback);

        //
        // Link this class device object in the list of class devices object
        // associated with the true class device object
        //
        ExAcquireFastMutex (&Globals.Mutex);

        for (i=0; i < Globals.NumAssocClass; i++) {
            if (Globals.AssocClassList[i].Free) {
                Globals.AssocClassList[i].Free = FALSE;
                break;
            }
        }

        if (i == Globals.NumAssocClass) {
            classDataList = (PPORT)ExAllocatePool (
                                NonPagedPool,
                                (Globals.NumAssocClass + 1) * sizeof (PORT));

            if (NULL == classDataList) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                // ISSUE: log error

                ExReleaseFastMutex (&Globals.Mutex);

                goto KeyboardAddDeviceExReject;
            }

            RtlZeroMemory (classDataList,
                           (Globals.NumAssocClass + 1) * sizeof (PORT));

            if (0 != Globals.NumAssocClass) {
                RtlCopyMemory (classDataList,
                               Globals.AssocClassList,
                               Globals.NumAssocClass * sizeof (PORT));

                ExFreePool (Globals.AssocClassList);
            }
            Globals.AssocClassList = classDataList;
            Globals.NumAssocClass++;
        }

        ClassData->UnitId = i;
        Globals.AssocClassList [i].Port = ClassData;
        Globals.AssocClassList [i].File = File;

        trueClassData->Self->StackSize =
            MAX (trueClassData->Self->StackSize, ClassData->Self->StackSize);

        ExReleaseFastMutex (&Globals.Mutex);

    } else if ((Globals.GrandMaster != ClassData) &&
               (ClassData == trueClassData)) {

        //
        // Connect to port device.
        //
        status = KbdSendConnectRequest(ClassData, KeyboardClassServiceCallback);
        ASSERT (STATUS_SUCCESS == status);
    }

    if (ClassData == trueClassData) {

        ASSERT (NULL != FullClassName);

        //
        // Load the device map information into the registry so
        // that setup can determine which keyboard class driver is active.
        //

        status = RtlWriteRegistryValue(
                     RTL_REGISTRY_DEVICEMAP,
                     Globals.MyBaseClassName.UnicodeString().Buffer, // key name
                     FullClassName, // value name
                     REG_SZ,
                     Globals.MyRegistryPath.UnicodeString().Buffer,
					 Globals.MyRegistryPath.Size());
					 

        if (!NT_SUCCESS(status)) {

            KbdPrint((
                1,
                "KBDCLASS-KeyboardClassInitialize: Could not store %ws in DeviceMap\n",
                FullClassName));


            errorCode = KBDCLASS_NO_DEVICEMAP_CREATED;
            uniqueErrorValue = KEYBOARD_ERROR_VALUE_BASE + 14;
            dumpCount = 0;

        } else {

            KbdPrint((
                1,
                "KBDCLASS-KeyboardClassInitialize: Stored %ws in DeviceMap\n",
                FullClassName));

        }
    }

    return status;

KeyboardAddDeviceExReject:

    //
    // Some part of the initialization failed.  Log an error, and
    // clean up the resources for the failed part of the initialization.
    //
    if (errorCode != STATUS_SUCCESS) {

        errorLogEntry = (PIO_ERROR_LOG_PACKET)
            IoAllocateErrorLogEntry(
                trueClassData->Self,
                (UCHAR) (sizeof(IO_ERROR_LOG_PACKET)
                         + (dumpCount * sizeof(ULONG)))
                );

        if (errorLogEntry != NULL) {

            errorLogEntry->ErrorCode = errorCode;
            errorLogEntry->DumpDataSize = (USHORT) (dumpCount * sizeof (ULONG));
            errorLogEntry->SequenceNumber = 0;
            errorLogEntry->MajorFunctionCode = 0;
            errorLogEntry->IoControlCode = 0;
            errorLogEntry->RetryCount = 0;
            errorLogEntry->UniqueErrorValue = uniqueErrorValue;
            errorLogEntry->FinalStatus = status;
            for (i = 0; i < dumpCount; i++)
                errorLogEntry->DumpData[i] = dumpData[i];

            IoWriteErrorLogEntry(errorLogEntry);
        }

    }

    return status;
}

VOID
KeyboardClassCancel(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the class cancellation routine.  It is
    called from the I/O system when a request is cancelled.  Read requests
    are currently the only cancellable requests.

    N.B.  The cancel spinlock is already held upon entry to this routine.
          Also, there is no ISR to synchronize with.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet to be cancelled.

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION deviceExtension;
    KIRQL currentIrql;
    KIRQL cancelIrql;
    BOOLEAN completeIt = FALSE, current = FALSE, startIt = FALSE;

    KbdPrint((2,"KBDCLASS-KeyboardClassCancel: enter\n"));

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;


    //
    // Release the cancel spinlock and grab the keyboard class spinlock (it
    // protects the RequestIsPending flag).
    //
    // Before doing that, though, look at CurrentIrp.  It is protected by the
    // cancel spinlock, so we must look at it before releasing the lock
    //
    if (Irp == DeviceObject->CurrentIrp) {
        current = TRUE;
    }

    IoReleaseCancelSpinLock(Irp->CancelIrql);
    KeAcquireSpinLock(&deviceExtension->SpinLock, &currentIrql);

    if (current) {
        if (deviceExtension->RequestIsPending) {
            //
            // The current request is being cancelled.  Set the CurrentIrp to
            // null, clear the RequestIsPending flag, and release the keyboard
            // class spinlock before starting the next packet.
            //
            completeIt = TRUE;
            deviceExtension->RequestIsPending = FALSE;
            DeviceObject->CurrentIrp = NULL;

            //
            // Must call IoStartNextPacket outside of holding the spin lock
            //
            startIt = TRUE;
        }
        else {
            //
            // If the request isn't pending, then StartIo will take of the IRP
            //
            ;
        }
        KeReleaseSpinLock(&deviceExtension->SpinLock, currentIrql);
    }
    else {

        //
        // Cancel a request in the device queue.  Reacquire the cancel
        // spinlock, remove the request from the queue, and release the
        // cancel spinlock.  Release the keyboard class spinlock.
        //

        IoAcquireCancelSpinLock(&cancelIrql);
        if (TRUE != KeRemoveEntryDeviceQueue(
                        &DeviceObject->DeviceQueue,
                        &Irp->Tail.Overlay.DeviceQueueEntry
                        )) {
            KbdPrint((
                1,
                "KBDCLASS-KeyboardClassCancel: Irp 0x%p not in device queue?!?\n",
                Irp
                ));
        }
        else {
            completeIt = TRUE;
        }
        IoReleaseCancelSpinLock(cancelIrql);
        KeReleaseSpinLock(&deviceExtension->SpinLock, currentIrql);
    }

    //
    // Complete the request with STATUS_CANCELLED.
    //
    if (completeIt) {
        Irp->IoStatus.Status = STATUS_CANCELLED;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest (Irp, IO_KEYBOARD_INCREMENT);

        //
        // Release the ref count we took out in MouseClassRead
        //
        IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);
    }

    if (startIt) {
        IoStartNextPacket(DeviceObject, TRUE);
    }

    KbdPrint((2,"KBDCLASS-KeyboardClassCancel: exit\n"));

    return;
}

VOID
KeyboardClassCleanupQueue (
    IN PDEVICE_OBJECT       DeviceObject,
    IN PDEVICE_EXTENSION    DeviceExtension,
    IN PFILE_OBJECT         FileObject
    )
/*++
Routine Description:

    This does the work of MouseClassCleanup so that we can also do that work
    during remove device for when the grand master isn't enabled.


--*/
{
    KIRQL spinlockIrql;
    KIRQL cancelIrql;
    PKDEVICE_QUEUE_ENTRY packet;
    PIRP  currentIrp = NULL;
    LIST_ENTRY queueHead;
    PLIST_ENTRY entry;
    PDRIVER_CANCEL cancelRoutine = NULL;

    InitializeListHead (&queueHead);

    ExAcquireFastMutex (&DeviceExtension->CleanupMutex);

    //
    // Acquire the mouse class spinlock and the cancel spinlock.
    //
    KeAcquireSpinLock(&DeviceExtension->SpinLock, &spinlockIrql);
    IoAcquireCancelSpinLock(&cancelIrql);

    //
    // Test to see if this file object is the one sending reads.
    // if it is not then we skip all of this stuff.
    // (We need only clean out the RIT's irps.
    //
    currentIrp = DeviceObject->CurrentIrp;

    if (currentIrp != NULL) {
        if ((FileObject) &&
            (FileObject !=
                (IoGetCurrentIrpStackLocation (currentIrp))->FileObject)) {

            currentIrp = NULL;

        } else {
#if 0
            //
            // Indicate that the cleanup routine has been called (StartIo
            // cares about this).
            //

            DeviceExtension->CleanupWasInitiated = TRUE;
#endif

            //
            // Complete all requests queued by this thread with
            // STATUS_CANCELLED. Start with the real CurrentIrp, and run
            // down the list of requests in the device queue.  Be sure to
            // set the real CurrentIrp to NULL and the RequestIsPending
            // flag to FALSE, so that the classservice callback routine
            // won't attempt to complete CurrentIrp.
            // Note that we can really only trust CurrentIrp when
            // RequestIsPending.
            //

            currentIrp = DeviceObject->CurrentIrp;
            DeviceExtension->RequestIsPending = FALSE;

            //
            // Set CurrentIrp to NULL manually because we are not using the
            // IoStartNextPacket routine which would usually set this value
            //
            DeviceObject->CurrentIrp = NULL;
        }
    }

    while (currentIrp != NULL) {

        //
        // Remove the CurrentIrp from the cancellable state.
        //
        //

        cancelRoutine = IoSetCancelRoutine(currentIrp, NULL);
        ASSERT(KeyboardClassCancel == cancelRoutine);

        //
        // Set Status to CANCELLED, release the spinlocks,
        // and complete the request.  Note that the IRQL is reset to
        // DISPATCH_LEVEL when we release the spinlocks.
        //

        currentIrp->IoStatus.Status = STATUS_CANCELLED;
        currentIrp->IoStatus.Information = 0;

        InsertTailList (&queueHead, &currentIrp->Tail.Overlay.ListEntry);

        //
        // Dequeue the next packet (IRP) from the device work queue.
        //

        packet = KeRemoveDeviceQueue(&DeviceObject->DeviceQueue);
        if (packet != NULL) {
            currentIrp =
                CONTAINING_RECORD(packet, IRP, Tail.Overlay.DeviceQueueEntry);
        } else {
            currentIrp = (PIRP) NULL;
        }

    } // end while

    //
    // Release the spinlocks and lower IRQL.
    //
    IoReleaseCancelSpinLock(cancelIrql);
    KeReleaseSpinLock(&DeviceExtension->SpinLock, spinlockIrql);

    //
    // Complete the irps
    //
    while (! IsListEmpty (&queueHead)) {

        entry = RemoveHeadList (&queueHead);
        currentIrp = CONTAINING_RECORD(entry, IRP, Tail.Overlay.ListEntry);

        IoCompleteRequest(currentIrp, IO_NO_INCREMENT);
        IoReleaseRemoveLock (&DeviceExtension->RemoveLock, currentIrp);
    }

    ExReleaseFastMutex (&DeviceExtension->CleanupMutex);
}



NTSTATUS
KeyboardClassCleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for cleanup requests.
    All requests queued to the keyboard class device (on behalf of
    the thread for whom the cleanup request was generated) are
    completed with STATUS_CANCELLED.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    PDEVICE_EXTENSION deviceExtension;
    PIO_STACK_LOCATION irpSp;

    KbdPrint((2,"KBDCLASS-KeyboardClassCleanup: enter\n"));

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Get a pointer to the current stack location for this request.
    //

    irpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // If the file object is the FileTrustedForRead, then the cleanup
    // request is being executed by the trusted subsystem.  Since the
    // trusted subsystem is the only one with sufficient privilege to make
    // Read requests to the driver, and since only Read requests get queued
    // to the device queue, a cleanup request from the trusted subsystem is
    // handled by cancelling all queued requests.
    //
    // If not, there is no cleanup work to perform
    // (only read requests can be cancelled).
    //

    if (IS_TRUSTED_FILE_FOR_READ (irpSp->FileObject)) {
        KeyboardClassCleanupQueue (DeviceObject, deviceExtension, irpSp->FileObject);
    }

    //
    // Complete the cleanup request with STATUS_SUCCESS.
    //

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    KbdPrint((2,"KBDCLASS-KeyboardClassCleanup: exit\n"));

    return(STATUS_SUCCESS);

}


NTSTATUS
KeyboardClassDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for device control requests.
    All device control subfunctions are passed, asynchronously, to the
    connected port driver for processing and completion.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    PIO_STACK_LOCATION stack;
    PDEVICE_EXTENSION deviceExtension;
    PDEVICE_EXTENSION port;
    BOOLEAN  loopit = FALSE;
    NTSTATUS status = STATUS_SUCCESS;
//    PKEYBOARD_INDICATOR_PARAMETERS param;
    ULONG    unitId;
    ULONG    ioctl;
    ULONG    i;
    PKBD_CALL_ALL_PORTS callAll;

    PAGED_CODE ();

    KbdPrint((2,"KBDCLASS-KeyboardClassDeviceControl: enter\n"));

    //
    // Get a pointer to the device extension.
    //

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Get a pointer to the current parameters for this request.  The
    // information is contained in the current stack location.
    //

    stack = IoGetCurrentIrpStackLocation(Irp);

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    //
    // Check for adequate input buffer length.  The input buffer
    // should, at a minimum, contain the unit ID specifying one of
    // the connected port devices.  If there is no input buffer (i.e.,
    // the input buffer length is zero), then we assume the unit ID
    // is zero (for backwards compatibility).
    //

    unitId = 0;
    switch (ioctl = stack->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_KEYBOARD_SET_INDICATORS:
        if (stack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof (KEYBOARD_INDICATOR_PARAMETERS)) {

            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Status = status;
            Irp->IoStatus.Information = 0;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            goto KeyboardClassDeviceControlReject;
        }

        deviceExtension->IndicatorParameters
            = *(PKEYBOARD_INDICATOR_PARAMETERS)Irp->AssociatedIrp.SystemBuffer;
        // Fall through
    case IOCTL_KEYBOARD_SET_TYPEMATIC:
        if (Globals.SendOutputToAllPorts) {
            loopit = TRUE;
        }
        // Fall through
    case IOCTL_KEYBOARD_QUERY_ATTRIBUTES:
    case IOCTL_KEYBOARD_QUERY_INDICATOR_TRANSLATION:
    case IOCTL_KEYBOARD_QUERY_INDICATORS:
    case IOCTL_KEYBOARD_QUERY_TYPEMATIC:
    case IOCTL_KEYBOARD_QUERY_IME_STATUS:
    case IOCTL_KEYBOARD_SET_IME_STATUS:

        if (stack->Parameters.DeviceIoControl.InputBufferLength == 0) {
            unitId = 0;
        } else if (stack->Parameters.DeviceIoControl.InputBufferLength <
                      sizeof(KEYBOARD_UNIT_ID_PARAMETER)) {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Status = status;
            Irp->IoStatus.Information = 0;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            goto KeyboardClassDeviceControlReject;

        } else {
            unitId = ((PKEYBOARD_UNIT_ID_PARAMETER)
                         Irp->AssociatedIrp.SystemBuffer)->UnitId;
        }

        if (deviceExtension->Self != deviceExtension->TrueClassDevice) {
            status = STATUS_NOT_SUPPORTED;
            Irp->IoStatus.Status = status;
            Irp->IoStatus.Information = 0;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            goto KeyboardClassDeviceControlReject;

        } else if (deviceExtension == Globals.GrandMaster) {
            ExAcquireFastMutex (&Globals.Mutex);
            if (Globals.NumAssocClass <= unitId) {

                ExReleaseFastMutex (&Globals.Mutex);
                status = STATUS_INVALID_PARAMETER;
                Irp->IoStatus.Status = status;
                Irp->IoStatus.Information = 0;
                IoCompleteRequest(Irp, IO_NO_INCREMENT);
                goto KeyboardClassDeviceControlReject;
            }
            if (0 < Globals.NumAssocClass) {
                if (!PORT_WORKING (&Globals.AssocClassList [unitId])) {
                    unitId = 0;
                }
                while (!PORT_WORKING (&Globals.AssocClassList [unitId])) {
                    if (Globals.NumAssocClass <= unitId) {
                        break;
                    }
                    unitId++;
                }
            }
            if (Globals.NumAssocClass <= unitId) {
                ExReleaseFastMutex (&Globals.Mutex);
                status = STATUS_INVALID_PARAMETER;
                Irp->IoStatus.Status = status;
                Irp->IoStatus.Information = 0;
                IoCompleteRequest(Irp, IO_NO_INCREMENT);
                goto KeyboardClassDeviceControlReject;
            }
            port = Globals.AssocClassList [unitId].Port;
            stack->FileObject = Globals.AssocClassList[unitId].File;

            ExReleaseFastMutex (&Globals.Mutex);
        } else {
            loopit = FALSE;
            port = deviceExtension;
        }

        //
        // Pass the device control request on to the port driver,
        // asynchronously.  Get the next IRP stack location and copy the
        // input parameters to the next stack location.  Change the major
        // function to internal device control.
        //

        IoCopyCurrentIrpStackLocationToNext (Irp);
        (IoGetNextIrpStackLocation (Irp))->MajorFunction =
            IRP_MJ_INTERNAL_DEVICE_CONTROL;

        if (loopit) {
            //
            // Inc the lock one more time until this looping is done.
            // Since we are allready holding this semiphore, it should not
            // have triggered on us.
            //
            status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
            ASSERT (NT_SUCCESS (status));

            //
            // Prepare to call multiple ports
            // Make a copy of the port array.
            //
            // If someone yanks the keyboard, while the caps lock is
            // going we could be in trouble.
            //
            // We should therefore take out remove locks on each and every
            // port device object so that it won't.
            //

            ExAcquireFastMutex (&Globals.Mutex);
            callAll = (PKBD_CALL_ALL_PORTS)ExAllocatePool (NonPagedPool,
                                      sizeof (KBD_CALL_ALL_PORTS) +
                                      (sizeof (PORT) * Globals.NumAssocClass));

            if (callAll) {
                callAll->Len = Globals.NumAssocClass;
                callAll->Current = 0;
                for (i = 0; i < Globals.NumAssocClass; i++) {

                    callAll->Port[i] = Globals.AssocClassList[i];

                    if (PORT_WORKING (&callAll->Port[i])) {
                        status = IoAcquireRemoveLock (
                                     &(callAll->Port[i].Port)->RemoveLock,
                                    Irp);
                        ASSERT (NT_SUCCESS (status));
                    }
                }
                status = KeyboardCallAllPorts (DeviceObject, Irp, callAll);

            } else {
                status = STATUS_INSUFFICIENT_RESOURCES;
                Irp->IoStatus.Status = status;
                Irp->IoStatus.Information = 0;
                IoCompleteRequest (Irp, IO_NO_INCREMENT);
            }
            ExReleaseFastMutex (&Globals.Mutex);


        } else {
            status = IoCallDriver(port->TopPort, Irp);
        }
        break;

    case IOCTL_GET_SYS_BUTTON_CAPS:
    case IOCTL_GET_SYS_BUTTON_EVENT:
    case IOCTL_HID_GET_DRIVER_CONFIG:
    case IOCTL_HID_SET_DRIVER_CONFIG:
    case IOCTL_HID_GET_POLL_FREQUENCY_MSEC:
    case IOCTL_HID_SET_POLL_FREQUENCY_MSEC:
    case IOCTL_GET_NUM_DEVICE_INPUT_BUFFERS:
    case IOCTL_SET_NUM_DEVICE_INPUT_BUFFERS:
    case IOCTL_HID_GET_COLLECTION_INFORMATION:
    case IOCTL_HID_GET_COLLECTION_DESCRIPTOR:
    case IOCTL_HID_FLUSH_QUEUE:
    case IOCTL_HID_SET_FEATURE:
    case IOCTL_HID_GET_FEATURE:
    case IOCTL_GET_PHYSICAL_DESCRIPTOR:
    case IOCTL_HID_GET_HARDWARE_ID:
    case IOCTL_HID_GET_MANUFACTURER_STRING:
    case IOCTL_HID_GET_PRODUCT_STRING:
    case IOCTL_HID_GET_SERIALNUMBER_STRING:
    case IOCTL_HID_GET_INDEXED_STRING:
        if (deviceExtension->PnP && (deviceExtension != Globals.GrandMaster)) {
            IoSkipCurrentIrpStackLocation (Irp);
            status = IoCallDriver (deviceExtension->TopPort, Irp);
            break;
        }
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        break;
    }

KeyboardClassDeviceControlReject:

    IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);

    KbdPrint((2,"KBDCLASS-KeyboardClassDeviceControl: exit\n"));

    return(status);

}

NTSTATUS
KeyboardCallAllPorts (
   PDEVICE_OBJECT       Device,
   PIRP                 Irp,
   PVOID                CallAll2 //PKBD_CALL_ALL_PORTS
   )
/*++
Routine Description:
    Bounce this Irp to all the ports associated with the given device extension.

--*/
{
    PIO_STACK_LOCATION  nextSp;
    NTSTATUS            status;
    PDEVICE_EXTENSION   port;
    BOOLEAN             firstTime;
    PKBD_CALL_ALL_PORTS  CallAll = (PKBD_CALL_ALL_PORTS) CallAll2;

    firstTime = CallAll->Current == 0;

    ASSERT (Globals.GrandMaster->Self == Device);

    nextSp = IoGetNextIrpStackLocation (Irp);
    IoCopyCurrentIrpStackLocationToNext (Irp);
    nextSp->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

    while ((CallAll->Current < CallAll->Len) &&
           (!PORT_WORKING (&CallAll->Port[CallAll->Current]))) {
        CallAll->Current++;
    }
    if (CallAll->Current < CallAll->Len) {

        port = CallAll->Port [CallAll->Current].Port;
        nextSp->FileObject = CallAll->Port [CallAll->Current].File;

        CallAll->Current++;

        IoSetCompletionRoutine (Irp,
                                (PIO_COMPLETION_ROUTINE)&KeyboardCallAllPorts,
                                CallAll,
                                TRUE,
                                TRUE,
                                TRUE);

        status = IoCallDriver (port->TopPort, Irp);
        IoReleaseRemoveLock (&port->RemoveLock, Irp);

    } else {
        //
        // We are done so let this Irp complete normally
        //
        ASSERT (Globals.GrandMaster == Device->DeviceExtension);

        if (Irp->PendingReturned) {
            IoMarkIrpPending (Irp);
        }

        IoReleaseRemoveLock (&Globals.GrandMaster->RemoveLock, Irp);
        ExFreePool (CallAll);
        return STATUS_SUCCESS;
    }

    if (firstTime) {
        //
        // Here we are not completing an IRP but sending it down for the first
        // time.
        //
        return status;
    }

    //
    // Since we bounced the Irp another time we must stop the completion on
    // this particular trip.
    //
    return STATUS_MORE_PROCESSING_REQUIRED;
}


NTSTATUS
KeyboardClassFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for flush requests.  The class
    input data queue is reinitialized.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION irpSp;

    KbdPrint((2,"KBDCLASS-KeyboardClassFlush: enter\n"));

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    irpSp = IoGetCurrentIrpStackLocation(Irp);

    if (deviceExtension->Self != deviceExtension->TrueClassDevice) {
        status = STATUS_NOT_SUPPORTED;

    } else if (!IS_TRUSTED_FILE_FOR_READ (irpSp->FileObject)) {
        status = STATUS_PRIVILEGE_NOT_HELD;
    }

    if (NT_SUCCESS (status)) {
        //
        // Initialize keyboard class input data queue.
        //
        KbdInitializeDataQueue((PVOID)deviceExtension);
    }

    //
    // Complete the request and return status.
    //
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    KbdPrint((2,"KBDCLASS-KeyboardClassFlush: exit\n"));

    return(status);

}

NTSTATUS
KbdSyncComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:
    The pnp IRP is in the process of completing.
    signal

Arguments:
    Context set to the device object in question.

--*/
{
    PIO_STACK_LOCATION  stack;

    UNREFERENCED_PARAMETER (DeviceObject);

    stack = IoGetCurrentIrpStackLocation (Irp);

    //
    // Since this completion routines sole purpose in life is to synchronize
    // Irp, we know that unless something else happens that the IoCallDriver
    // will unwind AFTER the we have complete this Irp.  Therefore we should
    // NOT buble up the pending bit.
    //
    //    if (Irp->PendingReturned) {
    //        IoMarkIrpPending( Irp );
    //    }
    //

    KeSetEvent ((PKEVENT) Context, 0, FALSE);

    return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS
KeyboardSendIrpSynchronously (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN BOOLEAN          CopyToNext
    )
{
    KEVENT      event;
    NTSTATUS    status;

    PAGED_CODE ();

    KeInitializeEvent(&event, SynchronizationEvent, FALSE);

    if (CopyToNext) {
        IoCopyCurrentIrpStackLocationToNext(Irp);
    }

    IoSetCompletionRoutine(Irp,
                           KbdSyncComplete,
                           &event,
                           TRUE,                // on success
                           TRUE,                // on error
                           TRUE                 // on cancel
                           );

    IoCallDriver(DeviceObject, Irp);

    //
    // Wait for lower drivers to be done with the Irp
    //
    KeWaitForSingleObject(&event,
                         Executive,
                         KernelMode,
                         FALSE,
                         NULL
                         );
    status = Irp->IoStatus.Status;

    return status;
}


NTSTATUS
KeyboardClassCreate (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for create/open and close requests.
    Open/close requests are completed here.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    PIO_ERROR_LOG_PACKET errorLogEntry;
    PIO_STACK_LOCATION   irpSp;
    PDEVICE_EXTENSION    deviceExtension;
//  PKEYBOARD_WORK_ITEM  item;
    PPORT        port;
    KIRQL        oldIrql;
    NTSTATUS     status = STATUS_SUCCESS;
    ULONG        i;
    LUID         priv;
//  KEVENT       event;
    BOOLEAN      SomeEnableDisableSucceeded = FALSE;
    BOOLEAN      enabled;

    KbdPrint((2,"KBDCLASS-KeyboardClassCreate: enter\n"));

    //
    // Get a pointer to the device extension.
    //

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Get a pointer to the current parameters for this request.  The
    // information is contained in the current stack location.
    //

    irpSp = IoGetCurrentIrpStackLocation(Irp);
    ASSERT (IRP_MJ_CREATE == irpSp->MajorFunction);

    //
    // We do not allow user mode opens for read.  This includes services (who
    // have the TCB privilege).
    //
    if (Irp->RequestorMode == UserMode &&
        (irpSp->Parameters.Create.SecurityContext->DesiredAccess & FILE_READ_DATA)
        ) {
        status = STATUS_ACCESS_DENIED;
        goto KeyboardClassCreateEnd;
    }

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);

    if (!NT_SUCCESS (status)) {
        goto KeyboardClassCreateEnd;
    }

    if ((deviceExtension->PnP) && (!deviceExtension->Started)) {
        KbdPrint((
            1,
            "KBDCLASS-Create: failed create because PnP and Not started\n"
             ));

        status = STATUS_UNSUCCESSFUL;
        IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);
        goto KeyboardClassCreateEnd;
    }


    //
    // For the create/open operation, send a KEYBOARD_ENABLE internal
    // device control request to the port driver to enable interrupts.
    //

    if (deviceExtension->Self == deviceExtension->TrueClassDevice) {
        //
        // The real keyboard is being opened.  This either represents the
        // Grand Master, if one exists, or the individual keyboards objects,
        // if all for one is not set.  (IE "KeyboardClassX")
        //
        // First, if the requestor is the trusted subsystem (the single
        // reader), reset the the cleanup indicator and place a pointer
        // to the file object which this class driver uses
        // to determine if the requestor has sufficient
        // privilege to perform the read operation).
        //

        priv = RtlConvertLongToLuid(SE_TCB_PRIVILEGE);

        if (SeSinglePrivilegeCheck(priv, Irp->RequestorMode)) {

            KeAcquireSpinLock(&deviceExtension->SpinLock, &oldIrql);

            ASSERT (!IS_TRUSTED_FILE_FOR_READ (irpSp->FileObject));
            SET_TRUSTED_FILE_FOR_READ (irpSp->FileObject);
            deviceExtension->TrustedSubsystemCount++;

            if (1 == deviceExtension->TrustedSubsystemCount) {
                deviceExtension->CleanupWasInitiated = FALSE;
            }

            KeReleaseSpinLock(&deviceExtension->SpinLock, oldIrql);
        }

    }

    //
    // Pass on enables for opens to the true class device
    //
    ExAcquireFastMutex (&Globals.Mutex);
    if ((Globals.GrandMaster == deviceExtension) && (1 == ++Globals.Opens)) {

        for (i = 0; i < Globals.NumAssocClass; i++) {
            port = &Globals.AssocClassList[i];

            if (port->Free) {
                continue;
            }

            enabled = port->Enabled;
            port->Enabled = TRUE;
            ExReleaseFastMutex (&Globals.Mutex);

            if (!enabled) {
                status = KbdEnableDisablePort(TRUE,
                                              Irp,
                                              port->Port,
                                              &port->File);

            }

            if (status != STATUS_SUCCESS) {

                KbdPrint((0,
                          "KBDCLASS-KeyboardClassOpenClose: Could not enable/disable interrupts for port device object @ 0x%p\n",
                          port->Port->TopPort));

                //
                // Log an error.
                //

                errorLogEntry = (PIO_ERROR_LOG_PACKET) IoAllocateErrorLogEntry(
                                         DeviceObject,
                                         sizeof(IO_ERROR_LOG_PACKET)
                                         );

                if (errorLogEntry != NULL) {

                    errorLogEntry->ErrorCode
                        = KBDCLASS_PORT_INTERRUPTS_NOT_ENABLED;
                    errorLogEntry->SequenceNumber = 0;
                    errorLogEntry->MajorFunctionCode = irpSp->MajorFunction;
                    errorLogEntry->IoControlCode = 0;
                    errorLogEntry->RetryCount = 0;
                    errorLogEntry->UniqueErrorValue
                        = KEYBOARD_ERROR_VALUE_BASE + 120;
                    errorLogEntry->FinalStatus = status;

                    IoWriteErrorLogEntry(errorLogEntry);
                }
                port->Enabled = FALSE;
                // ASSERT (port->Enabled);

            } else {
                SomeEnableDisableSucceeded = TRUE;
            }
            ExAcquireFastMutex (&Globals.Mutex);
        }
        ExReleaseFastMutex (&Globals.Mutex);

    } else if (Globals.GrandMaster != deviceExtension) {
        ExReleaseFastMutex (&Globals.Mutex);

        if (deviceExtension->TrueClassDevice == DeviceObject) {
            //
            // An open to the true class Device => enable the one and only port
            //

            status = KbdEnableDisablePort (TRUE,
                                           Irp,
                                           deviceExtension,
                                           &irpSp->FileObject);

        } else {
            //
            // A subordinant FDO.  They are not their own TrueClassDeviceObject.
            // Therefore pass the create straight on through.
            //
            IoSkipCurrentIrpStackLocation (Irp);
            status = IoCallDriver (deviceExtension->TopPort, Irp);
            IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);
            return status;
        }

        if (status != STATUS_SUCCESS) {

            KbdPrint((0,
                      "KBDCLASS-KeyboardClassOpenClose: Could not enable/disable interrupts for port device object @ 0x%p\n",
                      deviceExtension->TopPort));

            //
            // Log an error.
            //

            errorLogEntry = (PIO_ERROR_LOG_PACKET) IoAllocateErrorLogEntry(
                                                    DeviceObject,
                                                    sizeof(IO_ERROR_LOG_PACKET)
                                                    );
            if (errorLogEntry != NULL) {

                errorLogEntry->ErrorCode
                    = KBDCLASS_PORT_INTERRUPTS_NOT_ENABLED;
                errorLogEntry->SequenceNumber = 0;
                errorLogEntry->MajorFunctionCode = irpSp->MajorFunction;
                errorLogEntry->IoControlCode = 0;
                errorLogEntry->RetryCount = 0;
                errorLogEntry->UniqueErrorValue
                    = KEYBOARD_ERROR_VALUE_BASE + 120;
                errorLogEntry->FinalStatus = status;

                IoWriteErrorLogEntry(errorLogEntry);
            }

        } else {
            SomeEnableDisableSucceeded = TRUE;
        }
    } else {
        ExReleaseFastMutex (&Globals.Mutex);
    }

    //
    // Complete the request and return status.
    //
    // NOTE: We complete the request successfully if any one of the
    //       connected port devices successfully handled the request.
    //       The RIT only knows about one pointing device.
    //

    if (SomeEnableDisableSucceeded) {
        status = STATUS_SUCCESS;
    }

    IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);

KeyboardClassCreateEnd:
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    KbdPrint((2,"KBDCLASS-KeyboardClassOpenClose: exit\n"));
    return(status);
}

NTSTATUS
KeyboardClassClose (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for create/open and close requests.
    Open/close requests are completed here.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    PIO_ERROR_LOG_PACKET errorLogEntry;
    PIO_STACK_LOCATION   irpSp;
    PDEVICE_EXTENSION    deviceExtension;
    PPORT        port;
    KIRQL        oldIrql;
    NTSTATUS     status = STATUS_SUCCESS;
    ULONG        i;
//  LUID         priv;
//  KEVENT       event;
//  PFILE_OBJECT file;
    BOOLEAN SomeEnableDisableSucceeded;
    BOOLEAN      enabled;
    PVOID        notifyHandle;

    KbdPrint((2,"KBDCLASS-KeyboardClassClose: enter\n"));

    //
    // Get a pointer to the device extension.
    //

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Get a pointer to the current parameters for this request.  The
    // information is contained in the current stack location.
    //

    irpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Let the close go through even if the device is removed
    // AKA do not call KbdIncIoCount
    //

    //
    // For the create/open operation, send a KEYBOARD_ENABLE internal
    // device control request to the port driver to enable interrupts.
    //

    ASSERT (IRP_MJ_CLOSE == irpSp->MajorFunction);

    if (deviceExtension->Self == deviceExtension->TrueClassDevice) {

        KeAcquireSpinLock(&deviceExtension->SpinLock, &oldIrql);
        if (IS_TRUSTED_FILE_FOR_READ (irpSp->FileObject)) {
            ASSERT(0 < deviceExtension->TrustedSubsystemCount);
            deviceExtension->TrustedSubsystemCount--;
            CLEAR_TRUSTED_FILE_FOR_READ (irpSp->FileObject);
        }
        KeReleaseSpinLock(&deviceExtension->SpinLock, oldIrql);
    }

    //
    // Pass on enables for closes to the true class device
    //
    ExAcquireFastMutex (&Globals.Mutex);
    if ((Globals.GrandMaster == deviceExtension) && (0 == --Globals.Opens)) {

        for (i = 0; i < Globals.NumAssocClass; i++) {
            port = &Globals.AssocClassList[i];

            if (port->Free) {
                continue;
            }

            enabled = port->Enabled;
            port->Enabled = FALSE;
            ExReleaseFastMutex (&Globals.Mutex);

            if (enabled) {
                notifyHandle = InterlockedExchangePointer (
                                    &port->Port->TargetNotifyHandle,
                                    NULL);

                if (NULL != notifyHandle) {
                    IoUnregisterPlugPlayNotification (notifyHandle);
                }
                status = KbdEnableDisablePort(FALSE,
                                              Irp,
                                              port->Port,
                                              &port->File);
            } else {
                ASSERT (NULL == port->Port->TargetNotifyHandle);
            }

            if (status != STATUS_SUCCESS) {

                KbdPrint((0,
                          "KBDCLASS-KeyboardClassOpenClose: Could not enable/disable interrupts for port device object @ 0x%p\n",
                          port->Port->TopPort));

                //
                // Log an error.
                //

                errorLogEntry = (PIO_ERROR_LOG_PACKET)
                                    IoAllocateErrorLogEntry(
                                             DeviceObject,
                                             sizeof(IO_ERROR_LOG_PACKET)
                                             );

                if (errorLogEntry != NULL) {

                    errorLogEntry->ErrorCode
                        = KBDCLASS_PORT_INTERRUPTS_NOT_DISABLED;
                    errorLogEntry->SequenceNumber = 0;
                    errorLogEntry->MajorFunctionCode = irpSp->MajorFunction;
                    errorLogEntry->IoControlCode = 0;
                    errorLogEntry->RetryCount = 0;
                    errorLogEntry->UniqueErrorValue
                        = KEYBOARD_ERROR_VALUE_BASE + 120;
                    errorLogEntry->FinalStatus = status;

                    IoWriteErrorLogEntry(errorLogEntry);

                    ASSERTMSG ("Could not close open port!", FALSE);
                }

            } else {
                SomeEnableDisableSucceeded = TRUE;
            }
            ExAcquireFastMutex (&Globals.Mutex);
        }
        ExReleaseFastMutex (&Globals.Mutex);

    } else if (Globals.GrandMaster != deviceExtension) {
        ExReleaseFastMutex (&Globals.Mutex);

        if (deviceExtension->TrueClassDevice == DeviceObject) {
            //
            // A close to the true class Device => disable the one and only port
            //

            status = KbdEnableDisablePort (FALSE,
                                           Irp,
                                           deviceExtension,
                                           &irpSp->FileObject);

        } else {
            IoSkipCurrentIrpStackLocation (Irp);
            status = IoCallDriver (deviceExtension->TopPort, Irp);
            return status;
        }

        if (status != STATUS_SUCCESS) {

            KbdPrint((0,
                      "KBDCLASS-KeyboardClassOpenClose: Could not enable/disable interrupts for port device object @ 0x%p\n",
                      deviceExtension->TopPort));

            //
            // Log an error.
            //

            errorLogEntry = (PIO_ERROR_LOG_PACKET) IoAllocateErrorLogEntry(
                                                    DeviceObject,
                                                    sizeof(IO_ERROR_LOG_PACKET)
                                                    );
            if (errorLogEntry != NULL) {

                errorLogEntry->ErrorCode
                    = KBDCLASS_PORT_INTERRUPTS_NOT_DISABLED;
                errorLogEntry->SequenceNumber = 0;
                errorLogEntry->MajorFunctionCode = irpSp->MajorFunction;
                errorLogEntry->IoControlCode = 0;
                errorLogEntry->RetryCount = 0;
                errorLogEntry->UniqueErrorValue
                    = KEYBOARD_ERROR_VALUE_BASE + 120;
                errorLogEntry->FinalStatus = status;

                IoWriteErrorLogEntry(errorLogEntry);
            }

        } else {
            SomeEnableDisableSucceeded = TRUE;
        }
    } else {
        ExReleaseFastMutex (&Globals.Mutex);
    }

    //
    // Complete the request and return status.
    //
    // NOTE: We complete the request successfully if any one of the
    //       connected port devices successfully handled the request.
    //       The RIT only knows about one pointing device.
    //

    if (SomeEnableDisableSucceeded) {
        status = STATUS_SUCCESS;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    KbdPrint((2,"KBDCLASS-KeyboardClassOpenClose: exit\n"));
    return(status);
}

NTSTATUS
KeyboardClassRead(
    IN PDEVICE_OBJECT Device,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for read requests.  Valid read
    requests are marked pending, and started via IoStartPacket.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    NTSTATUS status;
    PIO_STACK_LOCATION irpSp;
    PDEVICE_EXTENSION  deviceExtension;

    KbdPrint((2,"KBDCLASS-KeyboardClassRead: enter\n"));

    irpSp = IoGetCurrentIrpStackLocation(Irp);

    //
    // Validate the read request parameters.  The read length should be an
    // integral number of KEYBOARD_INPUT_DATA structures.
    //

    deviceExtension = (PDEVICE_EXTENSION) Device->DeviceExtension;
    if (irpSp->Parameters.Read.Length == 0) {
        status = STATUS_SUCCESS;
    } else if (irpSp->Parameters.Read.Length % sizeof(KEYBOARD_INPUT_DATA)) {
        status = STATUS_BUFFER_TOO_SMALL;
    } else if (IS_TRUSTED_FILE_FOR_READ (irpSp->FileObject)) {
        //
        // If the file object's FsContext is non-null, then we've already
        // done the Read privilege check once before for this thread.  Skip
        // the privilege check.
        //

        status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);

        if (NT_SUCCESS (status)) {
            status = STATUS_PENDING;
        }
    } else {
        //
        // We only allow a trusted subsystem with the appropriate privilege
        // level to execute a Read call.
        //

        status = STATUS_PRIVILEGE_NOT_HELD;
    }

    //
    // If status is pending, mark the packet pending and start the packet
    // in a cancellable state.  Otherwise, complete the request.
    //

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    if (status == STATUS_PENDING) {
        IoMarkIrpPending(Irp);
        IoStartPacket(Device, Irp, (PULONG)NULL, KeyboardClassCancel);
    } else {
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }

    KbdPrint((2,"KBDCLASS-KeyboardClassRead: exit\n"));

    return(status);
}

VOID
KeyboardClassServiceCallback(
    IN PDEVICE_OBJECT DeviceObject,
    IN PKEYBOARD_INPUT_DATA InputDataStart,
    IN PKEYBOARD_INPUT_DATA InputDataEnd,
    IN OUT PULONG InputDataConsumed
    )

/*++

Routine Description:

    This routine is the class service callback routine.  It is
    called from the port driver's interrupt service DPC.  If there is an
    outstanding read request, the request is satisfied from the port input
    data queue.  Unsolicited keyboard input is moved from the port input

    data queue to the class input data queue.

    N.B.  This routine is entered at DISPATCH_LEVEL IRQL from the port
          driver's ISR DPC routine.

Arguments:

    DeviceObject - Pointer to class device object.

    InputDataStart - Pointer to the start of the data in the port input
        data queue.

    InputDataEnd - Points one input data structure past the end of the
        valid port input data.

    InputDataConsumed - Pointer to storage in which the number of input
        data structures consumed by this call is returned.

    NOTE:  Could pull the duplicate code out into a called procedure.

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION deviceExtension;
    PIO_STACK_LOCATION irpSp;
    PIRP  irp;
    KIRQL cancelIrql;
    ULONG bytesInQueue;
    ULONG bytesToMove;
    ULONG moveSize;
    BOOLEAN satisfiedPendingReadRequest = FALSE;
//  PIO_ERROR_LOG_PACKET errorLogEntry;

    KbdPrint((2,"KBDCLASS-KeyboardClassServiceCallback: enter\n"));

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    bytesInQueue = (ULONG)((PCHAR) InputDataEnd - (PCHAR) InputDataStart);
    moveSize = 0;
    *InputDataConsumed = 0;

    //
    // Notify system that human input has occured
    //

    PoSetSystemState (ES_USER_PRESENT);

    //
    // Acquire the spinlock that  protects the class device extension
    // (so we can look at RequestIsPending synchronously).  If there is
    // a pending read request, satisfy it.
    //
    // N.B. We can use KeAcquireSpinLockAtDpcLevel, instead of
    //      KeAcquireSpinLock, because this routine is already running
    //      at DISPATCH_IRQL.
    //

    KeAcquireSpinLockAtDpcLevel(&deviceExtension->SpinLock);

    if (deviceExtension->RequestIsPending) {

        //
        // Acquire the cancel spinlock, remove the request from the
        // cancellable state, and free the cancel spinlock.
        //

        IoAcquireCancelSpinLock(&cancelIrql);
        irp = DeviceObject->CurrentIrp;
        IoSetCancelRoutine(irp, NULL);
        // DeviceObject->CurrentIrp = NULL;
        IoReleaseCancelSpinLock(cancelIrql);

        //
        // An outstanding read request exists.   Clear the RequestIsPending
        // flag to indicate there is no longer an outstanding read request
        // pending.
        //

        deviceExtension->RequestIsPending = FALSE;

        //
        // Copy as much of the input data possible from the port input
        // data queue to the SystemBuffer to satisfy the read.
        //

        irpSp = IoGetCurrentIrpStackLocation(irp);
        bytesToMove = irpSp->Parameters.Read.Length;
        moveSize = (bytesInQueue < bytesToMove) ?
                                   bytesInQueue:bytesToMove;
        *InputDataConsumed += (moveSize / sizeof(KEYBOARD_INPUT_DATA));

        KbdPrint((
            3,
            "KBDCLASS-KeyboardClassServiceCallback: port queue length 0x%lx, read length 0x%lx\n",
            bytesInQueue,
            bytesToMove
            ));
        KbdPrint((
            3,
            "KBDCLASS-KeyboardClassServiceCallback: number of bytes to move from port to SystemBuffer 0x%lx\n",
            moveSize
            ));
        KbdPrint((
            3,
            "KBDCLASS-KeyboardClassServiceCallback: move bytes from 0x%lx to 0x%lx\n",
            (PCHAR) InputDataStart,
            irp->AssociatedIrp.SystemBuffer
            ));

        RtlMoveMemory(
            irp->AssociatedIrp.SystemBuffer,
            (PCHAR) InputDataStart,
            moveSize
            );

        //
        // Set the flag so that we start the next packet and complete
        // this read request (with STATUS_SUCCESS) prior to return.
        //

        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = moveSize;
        irpSp->Parameters.Read.Length = moveSize;
        satisfiedPendingReadRequest = TRUE;
    }

    //
    // If there is still data in the port input data queue, move it to the class
    // input data queue.
    //

    InputDataStart = (PKEYBOARD_INPUT_DATA) ((PCHAR) InputDataStart + moveSize);
    moveSize = bytesInQueue - moveSize;
    KbdPrint((
        3,
        "KBDCLASS-KeyboardClassServiceCallback: bytes remaining after move to SystemBuffer 0x%lx\n",
        moveSize
        ));

    if (moveSize > 0) {

        //
        // Move the remaining data from the port input data queue to
        // the class input data queue.  The move will happen in two
        // parts in the case where the class input data buffer wraps.
        //

        bytesInQueue =
            deviceExtension->KeyboardAttributes.InputDataQueueLength -
            (deviceExtension->InputCount * sizeof(KEYBOARD_INPUT_DATA));
        bytesToMove = moveSize;

        KbdPrint((
            3,
            "KBDCLASS-KeyboardClassServiceCallback: unused bytes in class queue 0x%lx, remaining bytes in port queue 0x%lx\n",
            bytesInQueue,
            bytesToMove
            ));

#if ALLOW_OVERFLOW
#else
        if (bytesInQueue == 0) {

            //
            // Refuse to move any bytes that would cause a class input data
            // queue overflow.  Just drop the bytes on the floor, and
            // log an overrun error.
            //

            KbdPrint((
                1,
                "KBDCLASS-KeyboardClassServiceCallback: Class input data queue OVERRUN\n"
                ));

            if (deviceExtension->OkayToLogOverflow) {

                //
                // Log an error.
                //

                errorLogEntry = (PIO_ERROR_LOG_PACKET)IoAllocateErrorLogEntry(
                                                         DeviceObject,
                                                         sizeof(IO_ERROR_LOG_PACKET)
                                                         + (2 * sizeof(ULONG))
                                                         );

                if (errorLogEntry != NULL) {

                    errorLogEntry->ErrorCode = KBDCLASS_KBD_BUFFER_OVERFLOW;
                    errorLogEntry->DumpDataSize = 2 * sizeof(ULONG);
                    errorLogEntry->SequenceNumber = 0;
                    errorLogEntry->MajorFunctionCode = 0;
                    errorLogEntry->IoControlCode = 0;
                    errorLogEntry->RetryCount = 0;
                    errorLogEntry->UniqueErrorValue =
                        KEYBOARD_ERROR_VALUE_BASE + 210;
                    errorLogEntry->FinalStatus = 0;
                    errorLogEntry->DumpData[0] = bytesToMove;
                    errorLogEntry->DumpData[1] =
                        deviceExtension->KeyboardAttributes.InputDataQueueLength;

                    IoWriteErrorLogEntry(errorLogEntry);
                }

                deviceExtension->OkayToLogOverflow = FALSE;
            }

        } else {
#endif

            //
            // There is room in the class input data queue, so move
            // the remaining port input data to it.
            //
            // BytesToMove <- MIN(Number of unused bytes in class input data
            //                    queue, Number of bytes remaining in port
            //                    input queue).
            // This is the total number of bytes that actually will move from
            // the port input data queue to the class input data queue.
            //

#if ALLOW_OVERFLOW
            bytesInQueue = deviceExtension->KeyboardAttributes.InputDataQueueLength;
#endif
            bytesToMove = (bytesInQueue < bytesToMove) ?
                                          bytesInQueue:bytesToMove;

            //
            // BytesInQueue <- Number of unused bytes from insertion pointer to
            // the end of the class input data queue (i.e., until the buffer
            // wraps).
            //

            bytesInQueue = (ULONG)(((PCHAR) deviceExtension->InputData +
                    deviceExtension->KeyboardAttributes.InputDataQueueLength) -
                    (PCHAR) deviceExtension->DataIn);

            KbdPrint((
                3,
                "KBDCLASS-KeyboardClassServiceCallback: total number of bytes to move to class queue 0x%lx\n",
                bytesToMove
                ));

            KbdPrint((
                3,
                "KBDCLASS-KeyboardClassServiceCallback: number of bytes to end of class buffer 0x%lx\n",
                bytesInQueue
                ));

            //
            // MoveSize <- Number of bytes to handle in the first move.
            //

            moveSize = (bytesToMove < bytesInQueue) ?
                                      bytesToMove:bytesInQueue;
            KbdPrint((
                3,
                "KBDCLASS-KeyboardClassServiceCallback: number of bytes in first move to class 0x%lx\n",
                moveSize
                ));

            //
            // Do the move from the port data queue to the class data queue.
            //

            KbdPrint((
                3,
                "KBDCLASS-KeyboardClassServiceCallback: move bytes from 0x%lx to 0x%lx\n",
                (PCHAR) InputDataStart,
                (PCHAR) deviceExtension->DataIn
                ));

            RtlMoveMemory(
                (PCHAR) deviceExtension->DataIn,
                (PCHAR) InputDataStart,
                moveSize
                );

            //
            // Increment the port data queue pointer and the class input
            // data queue insertion pointer.  Wrap the insertion pointer,
            // if necessary.
            //

            InputDataStart = (PKEYBOARD_INPUT_DATA)
                             (((PCHAR) InputDataStart) + moveSize);
            deviceExtension->DataIn = (PKEYBOARD_INPUT_DATA)
                                 (((PCHAR) deviceExtension->DataIn) + moveSize);
            if ((PCHAR) deviceExtension->DataIn >=
                ((PCHAR) deviceExtension->InputData +
                 deviceExtension->KeyboardAttributes.InputDataQueueLength)) {
                deviceExtension->DataIn = deviceExtension->InputData;
            }

            if ((bytesToMove - moveSize) > 0) {

                //
                // Special case.  The data must wrap in the class input data buffer.
                // Copy the rest of the port input data into the beginning of the
                // class input data queue.
                //

                //
                // MoveSize <- Number of bytes to handle in the second move.
                //

                moveSize = bytesToMove - moveSize;

                //
                // Do the move from the port data queue to the class data queue.
                //

                KbdPrint((
                    3,
                    "KBDCLASS-KeyboardClassServiceCallback: number of bytes in second move to class 0x%lx\n",
                    moveSize
                    ));
                KbdPrint((
                    3,
                    "KBDCLASS-KeyboardClassServiceCallback: move bytes from 0x%lx to 0x%lx\n",
                    (PCHAR) InputDataStart,
                    (PCHAR) deviceExtension->DataIn
                    ));

                RtlMoveMemory(
                    (PCHAR) deviceExtension->DataIn,
                    (PCHAR) InputDataStart,
                    moveSize
                    );

                //
                // Update the class input data queue insertion pointer.
                //

                deviceExtension->DataIn = (PKEYBOARD_INPUT_DATA)
                                 (((PCHAR) deviceExtension->DataIn) + moveSize);
            }

            //
            // Update the input data queue counter.
            //

            deviceExtension->InputCount +=
                    (bytesToMove / sizeof(KEYBOARD_INPUT_DATA));
            *InputDataConsumed += (bytesToMove / sizeof(KEYBOARD_INPUT_DATA));

            KbdPrint((
                3,
                "KBDCLASS-KeyboardClassServiceCallback: changed InputCount to %ld entries in the class queue\n",
                deviceExtension->InputCount
                ));
            KbdPrint((
                3,
                "KBDCLASS-KeyboardClassServiceCallback: DataIn 0x%lx, DataOut 0x%lx\n",
                deviceExtension->DataIn,
                deviceExtension->DataOut
                ));
            KbdPrint((
                3,
                "KBDCLASS-KeyboardClassServiceCallback: Input data items consumed = %d\n",
                *InputDataConsumed
                ));
#if ALLOW_OVERFLOW
#else
        }
#endif

    }

    //
    // Release the class input data queue spinlock.
    //

    KeReleaseSpinLockFromDpcLevel(&deviceExtension->SpinLock);

    //
    // If we satisfied an outstanding read request, start the next
    // packet and complete the request.
    //

    if (satisfiedPendingReadRequest) {
        IoStartNextPacket(DeviceObject, TRUE);
        IoCompleteRequest(irp, IO_KEYBOARD_INCREMENT);
        IoReleaseRemoveLock (&deviceExtension->RemoveLock, irp);
    }

    KbdPrint((2,"KBDCLASS-KeyboardClassServiceCallback: exit\n"));

    return;

}

VOID
KeyboardClassStartIo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the StartIo routine.  It is invoked to start a Read
    request.  If the class input data queue contains input data, the input
    data is copied to the SystemBuffer to satisfy the read.

    N.B.  Requests enter KeyboardClassStartIo in a cancellable state.  Also,
          there is an implicit assumption that only read requests are
          queued to the device queue (and handled by StartIo).  If this
          changes in the future, the KeyboardClassCleanup routine will
          be impacted.

    NOTE:  Could pull the duplicate code out into a called procedure.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION deviceExtension;
    PIO_STACK_LOCATION irpSp;
    KIRQL cancelIrql;
    PCHAR destination;
    ULONG bytesInQueue;
    ULONG bytesToMove;
    ULONG moveSize;
    NTSTATUS status;

    KbdPrint((2,"KBDCLASS-KeyboardClassStartIo: enter\n"));

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // Bump the error log sequence number.
    //

    deviceExtension->SequenceNumber += 1;

    //
    // Acquire the class spinlock to protect the input data queue and associated
    // pointers.  Note that StartIo is already running at DISPATCH_LEVEL
    // IRQL, so we can use KeAcquireSpinLockAtDpcLevel instead of
    // KeAcquireSpinLock.
    //

    KeAcquireSpinLockAtDpcLevel(&deviceExtension->SpinLock);

    //
    // Acquire the cancel spinlock and verify that the Irp has not been
    // cancelled and that cleanup is not in progress.
    //

    IoAcquireCancelSpinLock(&cancelIrql);
    if (deviceExtension->CleanupWasInitiated || Irp->Cancel) {

        IoReleaseCancelSpinLock(cancelIrql);
        KeReleaseSpinLockFromDpcLevel(&deviceExtension->SpinLock);
        IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);

        //
        // If cleanup was initiated, that means that the entire queue was
        // emptied and all the irps were completed
        //
        if (!deviceExtension->CleanupWasInitiated) {
            Irp->IoStatus.Status = STATUS_CANCELLED;
            Irp->IoStatus.Information = 0;
            IoCompleteRequest (Irp, IO_KEYBOARD_INCREMENT);
        }

        return;
    }

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        //
        // We are removing here.
        // Throw away this read.
        //

        IoReleaseCancelSpinLock(cancelIrql);
        KeReleaseSpinLockFromDpcLevel(&deviceExtension->SpinLock);

        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoStartNextPacket(DeviceObject, TRUE);
        IoCompleteRequest(Irp, IO_KEYBOARD_INCREMENT);
        // release the lock we took out in KeyboardClassRead
        IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);
        return;
    }
    IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);

    KbdPrint((
        3,
        "KBDCLASS-KeyboardClassStartIo: DataIn 0x%lx, DataOut 0x%lx\n",
        deviceExtension->DataIn,
        deviceExtension->DataOut
        ));

    KbdPrint((
        3,
        "KBDCLASS-KeyboardClassStartIo: entries in queue %ld\n",
        deviceExtension->InputCount
        ));

    //
    // If the input data queue is non-empty, satisfy the read request.
    // Otherwise, hold the request pending.
    //

    if (deviceExtension->InputCount != 0) {

        //
        // Copy as much of the input data as possible from the class input
        // data queue to the SystemBuffer to satisfy the read.  It may be
        // necessary to copy the data in two chunks (i.e., if the circular
        // queue wraps).
        // First, remove the request from the cancellable state, and free the
        // cancel spinlock.
        //

        IoSetCancelRoutine(Irp, NULL);
        // DeviceObject->CurrentIrp = NULL;
        IoReleaseCancelSpinLock(cancelIrql);

        irpSp = IoGetCurrentIrpStackLocation(Irp);

        //
        // BytesToMove <- MIN(Number of filled bytes in class input data queue,
        //                    Requested read length).
        //

        bytesInQueue = deviceExtension->InputCount *
                           sizeof(KEYBOARD_INPUT_DATA);
        bytesToMove = irpSp->Parameters.Read.Length;
        KbdPrint((
            3,
            "KBDCLASS-KeyboardClassStartIo: queue size 0x%lx, read length 0x%lx\n",
            bytesInQueue,
            bytesToMove
            ));
        bytesToMove = (bytesInQueue < bytesToMove) ?
                                      bytesInQueue:bytesToMove;

        //
        // MoveSize <- MIN(Number of bytes to be moved from the class queue,
        //                 Number of bytes to end of class input data queue).
        //

        bytesInQueue = (ULONG)(((PCHAR) deviceExtension->InputData +
                    deviceExtension->KeyboardAttributes.InputDataQueueLength) -
                    (PCHAR) deviceExtension->DataOut);
        moveSize = (bytesToMove < bytesInQueue) ?
                                  bytesToMove:bytesInQueue;
        KbdPrint((
            3,
            "KBDCLASS-KeyboardClassStartIo: bytes to end of queue 0x%lx\n",
            bytesInQueue
            ));

        //
        // Move bytes from the class input data queue to SystemBuffer, until
        // the request is satisfied or we wrap the class input data buffer.
        //

        destination = (PCHAR)Irp->AssociatedIrp.SystemBuffer;
        KbdPrint((
            3,
            "KBDCLASS-KeyboardClassStartIo: number of bytes in first move 0x%lx\n",
            moveSize
            ));
        KbdPrint((
            3,
            "KBDCLASS-KeyboardClassStartIo: move bytes from 0x%lx to 0x%lx\n",
            (PCHAR) deviceExtension->DataOut,
            destination
            ));
        RtlMoveMemory(
            destination,
            (PCHAR) deviceExtension->DataOut,
            moveSize
            );
        destination += moveSize;

        //
        // If the data wraps in the class input data buffer, copy the rest
        // of the data from the start of the input data queue
        // buffer through the end of the queued data.
        //

        if ((bytesToMove - moveSize) > 0) {

            //
            // MoveSize <- Remaining number bytes to move.
            //

            moveSize = bytesToMove - moveSize;

            //
            // Move the bytes from the class input data queue to SystemBuffer.
            //

            KbdPrint((
                3,
                "KBDCLASS-KeyboardClassStartIo: number of bytes in second move 0x%lx\n",
                moveSize
                ));
            KbdPrint((
                3,
                "KBDCLASS-KeyboardClassStartIo: move bytes from 0x%lx to 0x%lx\n",
                (PCHAR) deviceExtension->InputData,
                destination
                ));

            RtlMoveMemory(
                destination,
                (PCHAR) deviceExtension->InputData,
                moveSize
                );

            //
            // Update the class input data queue removal pointer.
            //

            deviceExtension->DataOut = (PKEYBOARD_INPUT_DATA)
                             (((PCHAR) deviceExtension->InputData) + moveSize);
        } else {

            //
            // Update the input data queue removal pointer.
            //

            deviceExtension->DataOut = (PKEYBOARD_INPUT_DATA)
                             (((PCHAR) deviceExtension->DataOut) + moveSize);
        }

        //
        // Update the class input data queue InputCount.
        //

        deviceExtension->InputCount -=
            (bytesToMove / sizeof(KEYBOARD_INPUT_DATA));

        if (deviceExtension->InputCount == 0) {

            //
            // Reset the flag that determines whether it is time to log
            // queue overflow errors.  We don't want to log errors too often.
            // Instead, log an error on the first overflow that occurs after
            // the ring buffer has been emptied, and then stop logging errors
            // until it gets cleared out and overflows again.
            //

            KbdPrint((
                1,
                "KBDCLASS-KeyboardClassStartIo: Okay to log overflow\n"
                ));
            deviceExtension->OkayToLogOverflow = TRUE;
        }

        KbdPrint((
            3,
            "KBDCLASS-KeyboardClassStartIo: new DataIn 0x%lx, DataOut 0x%lx\n",
            deviceExtension->DataIn,
            deviceExtension->DataOut
            ));
        KbdPrint((
            3,
            "KBDCLASS-KeyboardClassStartIo: new InputCount %ld\n",
            deviceExtension->InputCount
            ));

        //
        // Clear the RequestIsPending flag to indicate this request is
        // not held pending.
        //

        deviceExtension->RequestIsPending = FALSE;

        //
        // Release the class input data queue spinlock.
        //

        KeReleaseSpinLockFromDpcLevel(&deviceExtension->SpinLock);

        //
        // Start the next packet, and complete this read request
        // with STATUS_SUCCESS.
        //

        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = bytesToMove;
        irpSp->Parameters.Read.Length = bytesToMove;
        IoStartNextPacket(DeviceObject, TRUE);
        IoCompleteRequest(Irp, IO_KEYBOARD_INCREMENT);
        IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);

    } else {

        //
        // Set the RequestIsPending flag to indicate this request is
        // held pending for the callback routine to complete.
        //

        deviceExtension->RequestIsPending = TRUE;

        //
        // Hold the read request pending.  It remains in the cancellable
        // state.  When new input is received, the class service
        // callback routine will eventually complete the request.  For now,
        // merely free the cancel spinlock and the class input data queue
        // spinlock.
        //

        IoReleaseCancelSpinLock(cancelIrql);
        KeReleaseSpinLockFromDpcLevel(&deviceExtension->SpinLock);
    }

    KbdPrint((2,"KBDCLASS-KeyboardClassStartIo: exit\n"));

    return;

}

VOID
KeyboardClassUnload(
    IN PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:

    This routine is the class driver unload routine.

    NOTE:  Not currently implemented.

Arguments:

    DeviceObject - Pointer to class device object.

Return Value:

    None.

--*/

{
    PAGED_CODE ();

    UNREFERENCED_PARAMETER(DriverObject);

 	Globals.MyRegistryPath.Invalidate();
    if (Globals.AssocClassList) {
        ExFreePool(Globals.AssocClassList);
    }

    KbdPrint((2,"KBDCLASS-KeyboardClassUnload: enter\n"));
    KbdPrint((2,"KBDCLASS-KeyboardClassUnload: exit\n"));

    return;
}


NTSTATUS
KbdCreateClassObject(
    IN  PDRIVER_OBJECT      DriverObject,
    IN  PDEVICE_EXTENSION   TmpDeviceExtension,
    OUT PDEVICE_OBJECT    * ClassDeviceObject,
    OUT PWCHAR            * FullDeviceName,
    IN  BOOLEAN             Legacy
    )

/*++

Routine Description:

    This routine creates the keyboard class device object.


Arguments:

    DriverObject - Pointer to driver object created by system.

    TmpDeviceExtension - Pointer to the template device extension.

    FullDeviceName - Pointer to the Unicode string that is the full path name
        for the class device object.

    ClassDeviceObject - Pointer to a pointer to the class device object.

Return Value:

    The function value is the final status from the operation.

--*/

{
    NTSTATUS            status;
    PIO_ERROR_LOG_PACKET errorLogEntry;
    ULONG               uniqueErrorValue;
    PDEVICE_EXTENSION   deviceExtension = NULL;
    NTSTATUS            errorCode = STATUS_SUCCESS;
    UNICODE_STRING      fullClassName = {0,0,0};
    ULONG               dumpCount = 0;
    ULONG               dumpData[DUMP_COUNT];
    ULONG               i;
    WCHAR               nameIndex;

    PAGED_CODE ();

    KbdPrint((1,"\n\nKBDCLASS-KbdCreateClassObject: enter\n"));

    //
    // Create a non-exclusive device object for the keyboard class device.
    //

    ExAcquireFastMutex (&Globals.Mutex);

    //
    // Make sure ClassDeviceObject isn't pointing to a random pointer value
    //
    *ClassDeviceObject = NULL;

    if (NULL == Globals.GrandMaster) {
        //
        // Create a legacy name for this DO.
        //
        ExReleaseFastMutex (&Globals.Mutex);

        //
        // Set up space for the class's full device object name.
        //
        fullClassName.MaximumLength = sizeof(L"\\Device\\") +
                                    + Globals.MyBaseClassName.Size()
                                    + sizeof(L"0");

        if (Globals.ConnectOneClassToOnePort && Legacy) {
            fullClassName.MaximumLength += sizeof(L"Legacy");
        }

        fullClassName.Buffer = (PWCHAR) ExAllocatePool(PagedPool,
                                              fullClassName.MaximumLength);

        if (!fullClassName.Buffer) {

            KbdPrint((
                1,
                "KbdCLASS-KeyboardClassInitialize: Couldn't allocate string for device object name\n"
                ));

            status = STATUS_UNSUCCESSFUL;
            errorCode = KBDCLASS_INSUFFICIENT_RESOURCES;
            uniqueErrorValue = KEYBOARD_ERROR_VALUE_BASE + 6;
            dumpData[0] = (ULONG) fullClassName.MaximumLength;
            dumpCount = 1;
            goto KbdCreateClassObjectExit;
        }

        RtlZeroMemory(fullClassName.Buffer, fullClassName.MaximumLength);
        RtlAppendUnicodeToString(&fullClassName, L"\\Device\\");
		/***********************************************************************
		KUstring::UnicodeString (same as line 407)
		***********************************************************************/
	    RtlAppendUnicodeToString(&fullClassName, Globals.MyBaseClassName.UnicodeString().Buffer);

        if (Globals.ConnectOneClassToOnePort && Legacy) {
            RtlAppendUnicodeToString(&fullClassName, L"Legacy");
        }

        RtlAppendUnicodeToString(&fullClassName, L"0");

        //
        // Using the base name start trying to create device names until
        // one succeeds.  Everytime start over at 0 to eliminate gaps.
        //
        nameIndex = 0;

        do {
            fullClassName.Buffer [ (fullClassName.Length / sizeof (WCHAR)) - 1]
                = L'0' + nameIndex++;

            KbdPrint((
                1,
                "KBDCLASS-KbdCreateClassObject: Creating device object named %ws\n",
                fullClassName.Buffer
                ));

            status = IoCreateDevice(DriverObject,
                                    sizeof (DEVICE_EXTENSION),
                                    &fullClassName,
                                    FILE_DEVICE_KEYBOARD,
                                    0,
                                    FALSE,
                                    ClassDeviceObject);

        } while (STATUS_OBJECT_NAME_COLLISION == status);

        *FullDeviceName = fullClassName.Buffer;

    } else {
        ExReleaseFastMutex (&Globals.Mutex);
        status = IoCreateDevice(DriverObject,
                                sizeof(DEVICE_EXTENSION),
                                NULL, // no name for this FDO
                                FILE_DEVICE_KEYBOARD,
                                0,
                                FALSE,
                                ClassDeviceObject);
        *FullDeviceName = NULL;
    }

    if (!NT_SUCCESS(status)) {
        KbdPrint((
            1,
            "KBDCLASS-KbdCreateClassObject: Could not create class device object = %ws\n",
            fullClassName.Buffer
            ));

        errorCode = KBDCLASS_COULD_NOT_CREATE_DEVICE;
        uniqueErrorValue = KEYBOARD_ERROR_VALUE_BASE + 6;
        dumpData[0] = (ULONG) fullClassName.MaximumLength;
        dumpCount = 1;
        goto KbdCreateClassObjectExit;
    }

    //
    // Do buffered I/O.  I.e., the I/O system will copy to/from user data
    // from/to a system buffer.
    //

    (*ClassDeviceObject)->Flags |= DO_BUFFERED_IO;
    deviceExtension =
        (PDEVICE_EXTENSION)(*ClassDeviceObject)->DeviceExtension;
    *deviceExtension = *TmpDeviceExtension;

    deviceExtension->Self = *ClassDeviceObject;
    IoInitializeRemoveLock (&deviceExtension->RemoveLock, KEYBOARD_POOL_TAG, 0, 0);

    //
    // Initialize spin lock for critical sections.
    //

    KeInitializeSpinLock(&deviceExtension->SpinLock);

    //
    // Initialize keyboard class flags to indicate there is no outstanding
    // read request pending and cleanup has not been initiated.
    //

    deviceExtension->RequestIsPending = FALSE;
    deviceExtension->CleanupWasInitiated = FALSE;

    //
    // No trusted subsystem has sent us an open yet.
    //

    deviceExtension->TrustedSubsystemCount = 0;

    //
    // Allocate the ring buffer for the keyboard class input data.
    //

    deviceExtension->InputData = (PKEYBOARD_INPUT_DATA)
        ExAllocatePool(
            NonPagedPool,
            deviceExtension->KeyboardAttributes.InputDataQueueLength
            );

    if (!deviceExtension->InputData) {

        //
        // Could not allocate memory for the keyboard class data queue.
        //

        KbdPrint((
            1,
            "KBDCLASS-KbdCreateClassObject: Could not allocate input data queue for %ws\n",
            FullDeviceName
            ));

        status = STATUS_INSUFFICIENT_RESOURCES;

        //
        // Log an error.
        //

        errorCode = KBDCLASS_NO_BUFFER_ALLOCATED;
        uniqueErrorValue = KEYBOARD_ERROR_VALUE_BASE + 20;
        goto KbdCreateClassObjectExit;
    }

    //
    // Initialize keyboard class input data queue.
    //

    KbdInitializeDataQueue((PVOID)deviceExtension);

    //
    // Create storage for the WaitWake Error Logging
    //
    deviceExtension->WWErrorLog = (PIO_ERROR_LOG_PACKET)
        IoAllocateErrorLogEntry (*ClassDeviceObject, sizeof (IO_ERROR_LOG_PACKET));

KbdCreateClassObjectExit:

    if (status != STATUS_SUCCESS) {

        //
        // Some part of the initialization failed.  Log an error, and
        // clean up the resources for the failed part of the initialization.
        //
        RtlFreeUnicodeString (&fullClassName);
        *FullDeviceName = NULL;

        if (errorCode != STATUS_SUCCESS) {
            errorLogEntry = (PIO_ERROR_LOG_PACKET)
                IoAllocateErrorLogEntry(
                    (*ClassDeviceObject == NULL) ?
                        (PVOID) DriverObject : (PVOID) *ClassDeviceObject,
                    (UCHAR) (sizeof(IO_ERROR_LOG_PACKET)
                             + (dumpCount * sizeof(ULONG))));

            if (errorLogEntry != NULL) {

                errorLogEntry->ErrorCode = errorCode;
                errorLogEntry->DumpDataSize = (USHORT) (dumpCount * sizeof (ULONG));
                errorLogEntry->SequenceNumber = 0;
                errorLogEntry->MajorFunctionCode = 0;
                errorLogEntry->IoControlCode = 0;
                errorLogEntry->RetryCount = 0;
                errorLogEntry->UniqueErrorValue = uniqueErrorValue;
                errorLogEntry->FinalStatus = status;
                for (i = 0; i < dumpCount; i++)
                    errorLogEntry->DumpData[i] = dumpData[i];

                IoWriteErrorLogEntry(errorLogEntry);
            }
        }

        if ((deviceExtension) && (deviceExtension->InputData)) {
            ExFreePool(deviceExtension->InputData);
        }
        if ((deviceExtension) && (deviceExtension->WWErrorLog)) {
            ExFreePool(deviceExtension->WWErrorLog);
        }
        if (*ClassDeviceObject) {
            IoDeleteDevice(*ClassDeviceObject);
            *ClassDeviceObject = NULL;
        }
    }

    KbdPrint((1,"KBDCLASS-KbdCreateClassObject: exit\n"));

    return(status);
}
#if DBG
VOID
KbdDebugPrint(
    ULONG DebugPrintLevel,
    PCCHAR DebugMessage,
    ...
    )

/*++

Routine Description:

    Debug print routine.

Arguments:

    Debug print level between 0 and 3, with 3 being the most verbose.

Return Value:

    None.

--*/

{
    va_list ap;

    va_start(ap, DebugMessage);

    if (DebugPrintLevel <= Globals.Debug) {

        char buffer[256];

        (VOID) vsprintf(buffer, DebugMessage, ap);

        DbgPrint(buffer);
    }

    va_end(ap);

}
#endif

NTSTATUS
KbdDeterminePortsServiced(
    IN PUNICODE_STRING BasePortName,
    IN OUT PULONG NumberPortsServiced
    )

/*++

Routine Description:

    This routine reads the DEVICEMAP portion of the registry to determine
    how many ports the class driver is to service.  Depending on the
    value of DeviceExtension->ConnectOneClassToOnePort, the class driver
    will eventually create one device object per port device serviced, or
    one class device object that connects to multiple port device objects.

    Assumptions:

        1.  If the base device name for the class driver is "KeyboardClass",
                                                                     ^^^^^
            then the port drivers it can service are found under the
            "KeyboardPort" subkey in the DEVICEMAP portion of the registry.
                     ^^^^

        2.  The port device objects are created with suffixes in strictly
            ascending order, starting with suffix 0.  E.g.,
            \Device\KeyboardPort0 indicates the first keyboard port device,
            \Device\KeyboardPort1 the second, and so on.  There are no gaps
            in the list.

        3.  If ConnectOneClassToOnePort is non-zero, there is a 1:1
            correspondence between class device objects and port device
            objects.  I.e., \Device\KeyboardClass0 will connect to
            \Device\KeyboardPort0, \Device\KeyboardClass1 to
            \Device\KeyboardPort1, and so on.

        4.  If ConnectOneClassToOnePort is zero, there is a 1:many
            correspondence between class device objects and port device
            objects.  I.e., \Device\KeyboardClass0 will connect to
            \Device\KeyboardPort0, and \Device\KeyboardPort1, and so on.


    Note that for Product 1, the Raw Input Thread (Windows USER) will
    only deign to open and read from one keyboard device.  Hence, it is
    safe to make simplifying assumptions because the driver is basically
    providing  much more functionality than the RIT will use.

Arguments:

    BasePortName - Pointer to the Unicode string that is the base path name
        for the port device.

    NumberPortsServiced - Pointer to storage that will receive the
        number of ports this class driver should service.

Return Value:

    The function value is the final status from the operation.

--*/

{

    NTSTATUS status;
    PRTL_QUERY_REGISTRY_TABLE registryTable = NULL;
    USHORT queriesPlusOne = 2;

    PAGED_CODE ();

    //
    // Initialize the result.
    //

    *NumberPortsServiced = 0;

    //
    // Allocate the Rtl query table.
    //

    registryTable = (PRTL_QUERY_REGISTRY_TABLE)ExAllocatePool(
										PagedPool,
										sizeof(RTL_QUERY_REGISTRY_TABLE) * queriesPlusOne
										);

    if (!registryTable) {

        KbdPrint((
            1,
            "KBDCLASS-KbdDeterminePortsServiced: Couldn't allocate table for Rtl query\n"
            ));

        status = STATUS_UNSUCCESSFUL;

    } else {

        RtlZeroMemory(
            registryTable,
            sizeof(RTL_QUERY_REGISTRY_TABLE) * queriesPlusOne
            );

        //
        // Set things up so that KbdDeviceMapQueryCallback will be
        // called once for every value in the keyboard port section
        // of the registry's hardware devicemap.
        //

        registryTable[0].QueryRoutine = KbdDeviceMapQueryCallback;
        registryTable[0].Name = NULL;

        status = RtlQueryRegistryValues(
                     RTL_REGISTRY_DEVICEMAP | RTL_REGISTRY_OPTIONAL,
                     BasePortName->Buffer,
                     registryTable,
                     NumberPortsServiced,
                     NULL
                     );

        if (!NT_SUCCESS(status)) {
            KbdPrint((
                1,
                "KBDCLASS-KbdDeterminePortsServiced: RtlQueryRegistryValues failed with 0x%x\n",
                status
                ));
        }

        ExFreePool(registryTable);
    }

    return(status);
}

NTSTATUS
KbdDeviceMapQueryCallback(
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength,
    IN PVOID Context,
    IN PVOID EntryContext
    )

/*++

Routine Description:

    This is the callout routine specified in a call to
    RtlQueryRegistryValues.  It increments the value pointed
    to by the Context parameter.

Arguments:

    ValueName - Unused.

    ValueType - Unused.

    ValueData - Unused.

    ValueLength - Unused.

    Context - Pointer to a count of the number of times this
        routine has been called.  This is the number of ports
        the class driver needs to service.

    EntryContext - Unused.

Return Value:

    The function value is the final status from the operation.

--*/

{
    PAGED_CODE ();

    *(PULONG)Context += 1;

    return(STATUS_SUCCESS);
}

NTSTATUS
KbdEnableDisablePort(
    IN BOOLEAN EnableFlag,
    IN PIRP    Irp,
    IN PDEVICE_EXTENSION Port,
    IN PFILE_OBJECT * File
    )

/*++

Routine Description:

    This routine sends an enable or a disable request to the port driver.
    The legacy port drivers require an enable or disable ioctl, while the
    plug and play drivers require merely a create.

Arguments:

    DeviceObject - Pointer to class device object.

    EnableFlag - If TRUE, send an ENABLE request; otherwise, send DISABLE.

    PortIndex - Index into the PortDeviceObjectList[] for the current
        enable/disable request.

Return Value:

    Status is returned.

--*/

{
//  IO_STATUS_BLOCK ioStatus;
    UNICODE_STRING  name = {0,0,0};
    PDEVICE_OBJECT  device = NULL;
    NTSTATUS    status = STATUS_SUCCESS;
    PWCHAR      buffer = NULL;
    ULONG       bufferLength = 0;
    PIO_STACK_LOCATION stack;

    PAGED_CODE ();

    KbdPrint((2,"KBDCLASS-KbdEnableDisablePort: enter\n"));

    //
    // Create notification event object to be used to signal the
    // request completion.
    //

    if ((Port->TrueClassDevice == Port->Self) && (Port->PnP)) {

        IoCopyCurrentIrpStackLocationToNext (Irp);
        stack = IoGetNextIrpStackLocation (Irp);

        if (EnableFlag) {
            //
            // Since there is no grand master there could not have been a
            // create file against the FDO before it was started.  Therefore
            // the only time we would enable is during a create and not a
            // start as we might with another FDO attached to an already open
            // grand master.
            //
            ASSERT (IRP_MJ_CREATE == stack->MajorFunction);

        } else {
            if (IRP_MJ_CLOSE != stack->MajorFunction) {
                //
                // We are disabling.  This could be because the device was
                // closed, or because the device was removed out from
                // underneath us.
                //
                ASSERT (IRP_MJ_PNP == stack->MajorFunction);
                ASSERT ((IRP_MN_REMOVE_DEVICE == stack->MinorFunction) ||
                        (IRP_MN_STOP_DEVICE == stack->MinorFunction));
                stack->MajorFunction = IRP_MJ_CLOSE;
            }
        }

        //
        // Either way we need only pass the Irp down without mucking with the
        // file object.
        //
        status = KeyboardSendIrpSynchronously (Port->TopPort, Irp, FALSE);

    } else if (!Port->PnP) {
        //
        // We have here an old style Port Object.  Therefore we send it the
        // old style internal IOCTLs of ENABLE and DISABLE, and not the new
        // style of passing on a create and close.
        //
        IoCopyCurrentIrpStackLocationToNext (Irp);
        stack = IoGetNextIrpStackLocation (Irp);

        stack->Parameters.DeviceIoControl.OutputBufferLength = 0;
        stack->Parameters.DeviceIoControl.InputBufferLength = 0;
        stack->Parameters.DeviceIoControl.IoControlCode
            = EnableFlag ? IOCTL_INTERNAL_KEYBOARD_ENABLE
                         : IOCTL_INTERNAL_KEYBOARD_DISABLE;
        stack->Parameters.DeviceIoControl.Type3InputBuffer = NULL;
        stack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

        status = KeyboardSendIrpSynchronously (Port->TopPort, Irp, FALSE);

    } else {
        //
        // We are dealing with a plug and play port and we have a Grand
        // Master.
        //
        ASSERT (Port->TrueClassDevice == Globals.GrandMaster->Self);

        //
        // Therefore we need to substitute the given file object for a new
        // one for use with each individual ports.
        // For enable, we need to create this file object against the given
        // port and then hand it back in the File parameter, or for disable,
        // deref the File parameter and free that file object.
        //
        // Of course, there must be storage for a file pointer pointed to by
        // the File parameter.
        //
        ASSERT (NULL != File);

        if (EnableFlag) {

            ASSERT (NULL == *File);

            //
            // The following long list of rigamaroll translates into
            // sending the lower driver a create file IRP and creating a
            // NEW file object disjoint from the one given us in our create
            // file routine.
            //
            // Normally we would just pass down the Create IRP we were
            // given, but since we do not have a one to one correspondance of
            // top device objects and port device objects.
            // This means we need more file objects: one for each of the
            // miriad of lower DOs.
            //

            bufferLength = 0;
            status = IoGetDeviceProperty (
                             Port->PDO,
                             DevicePropertyPhysicalDeviceObjectName,
                             bufferLength,
                             buffer,
                             &bufferLength);
            ASSERT (STATUS_BUFFER_TOO_SMALL == status);

            buffer = (PWCHAR)ExAllocatePool (PagedPool, bufferLength);

            if (NULL == buffer) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            status =  IoGetDeviceProperty (
                          Port->PDO,
                          DevicePropertyPhysicalDeviceObjectName,
                          bufferLength,
                          buffer,
                          &bufferLength);


			name.MaximumLength = (USHORT) bufferLength;
            name.Length = (USHORT) bufferLength - sizeof (UNICODE_NULL);
            name.Buffer = buffer;

            status = IoGetDeviceObjectPointer (&name,
                                               FILE_ALL_ACCESS,
                                               File,
                                               &device);
            ExFreePool (buffer);
            //
            // Note, that this create will first go to ourselves since we
            // are attached to this PDO stack.  Therefore two things are
            // noteworthy.  This driver will receive another Create IRP
            // (with a different file object) (not to the grand master but
            // to one of the subordenant FDO's).  The device object returned
            // will be the subordenant FDO, which in this case is the "self"
            // device object of this Port.
            //
            if (NT_SUCCESS (status)) {
                PVOID   buffer;

                ASSERT (device == Port->Self);

                if (NULL != Irp) {
                    //
                    // Set the indicators for this port device object.
                    // NB: The Grandmaster's device extension is initialized to
                    // zero, and the flags for indicator lights are flags, so
                    // this means that unless the RIUT has set the flags that
                    // IndicatorParameters will have no lights set.
                    //

                    IoCopyCurrentIrpStackLocationToNext (Irp);
                    stack = IoGetNextIrpStackLocation (Irp);

                    stack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
                    stack->Parameters.DeviceIoControl.OutputBufferLength = 0;
                    stack->Parameters.DeviceIoControl.InputBufferLength =
                        sizeof (KEYBOARD_INDICATOR_PARAMETERS);
                    stack->Parameters.DeviceIoControl.IoControlCode =
                        IOCTL_KEYBOARD_SET_INDICATORS;
                    stack->FileObject = *File;

                    buffer = Irp->AssociatedIrp.SystemBuffer;

                    Irp->AssociatedIrp.SystemBuffer =
                        & Globals.GrandMaster->IndicatorParameters;

                    status = KeyboardSendIrpSynchronously (Port->TopPort, Irp, FALSE);

                    Irp->AssociatedIrp.SystemBuffer = buffer;
                }

                //
                // Register for Target device removal events
                //
                ASSERT (NULL == Port->TargetNotifyHandle);
                status = IoRegisterPlugPlayNotification (
                             EventCategoryTargetDeviceChange,
                             0, // No flags
                             *File,
                             Port->Self->DriverObject,
                             (PDRIVER_NOTIFICATION_CALLBACK_ROUTINE) KeyboardClassPlugPlayNotification,
                             Port,
                             &Port->TargetNotifyHandle);
            }

        } else {
            //
            // Getting rid of the handle is easy.  Just deref the file.
            //
            ObDereferenceObject (*File);
            *File = NULL;
        }

    }
    KbdPrint((2,"KBDCLASS-KbdEnableDisablePort: exit\n"));

    return (status);
}

VOID
KbdInitializeDataQueue (
    IN PVOID Context
    )

/*++

Routine Description:

    This routine initializes the input data queue.  IRQL is raised to
    DISPATCH_LEVEL to synchronize with StartIo, and the device object
    spinlock is acquired.

Arguments:

    Context - Supplies a pointer to the device extension.

Return Value:

    None.

--*/

{
    KIRQL oldIrql;
    PDEVICE_EXTENSION deviceExtension;

    KbdPrint((3,"KBDCLASS-KbdInitializeDataQueue: enter\n"));

    //
    // Get address of device extension.
    //

    deviceExtension = (PDEVICE_EXTENSION)Context;

    //
    // Acquire the spinlock to protect the input data
    // queue and associated pointers.
    //

    KeAcquireSpinLock(&deviceExtension->SpinLock, &oldIrql);

    //
    // Initialize the input data queue.
    //

    deviceExtension->InputCount = 0;
    deviceExtension->DataIn = deviceExtension->InputData;
    deviceExtension->DataOut = deviceExtension->InputData;

    deviceExtension->OkayToLogOverflow = TRUE;

    //
    // Release the spinlock and lower IRQL.
    //

    KeReleaseSpinLock(&deviceExtension->SpinLock, oldIrql);

    KbdPrint((3,"KBDCLASS-KbdInitializeDataQueue: exit\n"));

}

NTSTATUS
KbdSendConnectRequest(
    IN PDEVICE_EXTENSION ClassData,
    IN PVOID ServiceCallback
    )

/*++

Routine Description:

    This routine sends a connect request to the port driver.

Arguments:

    DeviceObject - Pointer to class device object.

    ServiceCallback - Pointer to the class service callback routine.

    PortIndex - The index into the PortDeviceObjectList[] for the current
        connect request.

Return Value:

    Status is returned.

--*/

{
    PIRP irp;
    IO_STATUS_BLOCK ioStatus;
    NTSTATUS status;
    KEVENT event;
    CONNECT_DATA connectData;

    PAGED_CODE ();

    KbdPrint((2,"KBDCLASS-KbdSendConnectRequest: enter\n"));

    //
    // Create notification event object to be used to signal the
    // request completion.
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    //
    // Build the synchronous request to be sent to the port driver
    // to perform the request.  Allocate an IRP to issue the port internal
    // device control connect call.  The connect parameters are passed in
    // the input buffer, and the keyboard attributes are copied back
    // from the port driver directly into the class device extension.
    //

    connectData.ClassDeviceObject = ClassData->TrueClassDevice;
    connectData.ClassService = ServiceCallback;

    irp = IoBuildDeviceIoControlRequest(
            IOCTL_INTERNAL_KEYBOARD_CONNECT,
            ClassData->TopPort,
            &connectData,
            sizeof(CONNECT_DATA),
            NULL,
            0,
            TRUE,
            &event,
            &ioStatus
            );

    //
    // Call the port driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    status = IoCallDriver(ClassData->TopPort, irp);

    if (status == STATUS_PENDING) {
        (VOID) KeWaitForSingleObject(
                   &event,
                   Suspended,
                   KernelMode,
                   FALSE,
                   NULL
                   );

        status = irp->IoStatus.Status;
    } else {

        //
        // Ensure that the proper status value gets picked up.
        //

        ioStatus.Status = status;
    }

    KbdPrint((2,"KBDCLASS-KbdSendConnectRequest: exit\n"));

    return(ioStatus.Status);

} // end KbdSendConnectRequest()

NTSTATUS
KeyboardPnP (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    The plug and play dispatch routines.

    Most of these this filter driver will completely ignore.
    In all cases it must pass on the IRP to the lower driver.

Arguments:

   DeviceObject - pointer to a device object.

   Irp - pointer to an I/O Request Packet.

Return Value:

      NT status code

--*/
{
    PDEVICE_EXTENSION   data;
    PDEVICE_EXTENSION   trueClassData;
    PIO_STACK_LOCATION  stack;
    NTSTATUS            status, startStatus;
//  ULONG               i;
//  PFILE_OBJECT        filePointer;
    PFILE_OBJECT      * file;
    UINT_PTR            startInformation;
    DEVICE_CAPABILITIES devCaps;
    BOOLEAN             enabled;
    PIRP                waitWakeIrp;
//  PDRIVER_CANCEL      cancelRoutine;
    PPORT               port;
    PVOID               notifyHandle;
//  KIRQL               oldIrql;
//  KIRQL               cancelIrql;

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    stack = IoGetCurrentIrpStackLocation (Irp);

    if(!data->PnP) {
        //
        // This irp was sent to the control device object, which knows not
        // how to deal with this IRP.  It is therefore an error.
        //
        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return STATUS_NOT_SUPPORTED;

    }

    status = IoAcquireRemoveLock (&data->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        //
        // Someone gave us a pnp irp after a remove.  Unthinkable!
        //
        ASSERT (FALSE);
        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    trueClassData = (PDEVICE_EXTENSION) data->TrueClassDevice->DeviceExtension;
    switch (stack->MinorFunction) {
    case IRP_MN_START_DEVICE:

        //
        // The device is starting.
        //
        // We cannot touch the device (send it any non pnp irps) until a
        // start device has been passed down to the lower drivers.
        //
        status = KeyboardSendIrpSynchronously (data->TopPort, Irp, TRUE);

        if (NT_SUCCESS (status) && NT_SUCCESS (Irp->IoStatus.Status)) {
            //
            // As we are successfully now back from our start device
            // we can do work.
            //
            // Get the caps of the device.  Save off pertinent information
            // before mucking about w/the irp
            //
            startStatus = Irp->IoStatus.Status;
            startInformation = Irp->IoStatus.Information;

            Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
            Irp->IoStatus.Information = 0;

            RtlZeroMemory(&devCaps, sizeof (DEVICE_CAPABILITIES));
            devCaps.Size = sizeof (DEVICE_CAPABILITIES);
            devCaps.Version = 1;
            devCaps.Address = devCaps.UINumber = (ULONG)-1;

            stack = IoGetNextIrpStackLocation (Irp);
            stack->MinorFunction = IRP_MN_QUERY_CAPABILITIES;
            stack->Parameters.DeviceCapabilities.Capabilities = &devCaps;

            status = KeyboardSendIrpSynchronously (data->TopPort, Irp, FALSE);

            if (NT_SUCCESS (status) && NT_SUCCESS (Irp->IoStatus.Status)) {
                data->MinDeviceWakeState = devCaps.DeviceWake;
                data->MinSystemWakeState = devCaps.SystemWake;

                RtlCopyMemory (data->SystemToDeviceState,
                               devCaps.DeviceState,
                               sizeof(DEVICE_POWER_STATE) * PowerSystemHibernate);
            } else {
                ASSERTMSG ("Get Device caps Failed!\n", status);
            }

            //
            // Set everything back to the way it was and continue with the start
            //
            status = STATUS_SUCCESS;
            Irp->IoStatus.Status = startStatus;
            Irp->IoStatus.Information = startInformation;

            data->Started = TRUE;

            if (WAITWAKE_SUPPORTED (data)) {
                //
                // register for the wait wake guid as well
                //
                data->WmiLibInfo.GuidCount = sizeof (KeyboardClassWmiGuidList) /
                                             sizeof (WMIGUIDREGINFO);
                ASSERT (2 == data->WmiLibInfo.GuidCount);
            }
            else {
                data->WmiLibInfo.GuidCount = (sizeof (KeyboardClassWmiGuidList) /
                                              sizeof (WMIGUIDREGINFO)) - 1;
                ASSERT (1 == data->WmiLibInfo.GuidCount);
            }
            data->WmiLibInfo.GuidList = KeyboardClassWmiGuidList;
            data->WmiLibInfo.QueryWmiRegInfo = KeyboardClassQueryWmiRegInfo;
            data->WmiLibInfo.QueryWmiDataBlock = KeyboardClassQueryWmiDataBlock;
            data->WmiLibInfo.SetWmiDataBlock = KeyboardClassSetWmiDataBlock;
            data->WmiLibInfo.SetWmiDataItem = KeyboardClassSetWmiDataItem;
            data->WmiLibInfo.ExecuteWmiMethod = NULL;
            data->WmiLibInfo.WmiFunctionControl = NULL;

            IoWMIRegistrationControl(data->Self,
                                     WMIREG_ACTION_REGISTER
                                     );

            ExAcquireFastMutex (&Globals.Mutex);
            if (Globals.GrandMaster) {
                if (0 < Globals.Opens) {
                    port = &Globals.AssocClassList[data->UnitId];
                    ASSERT (port->Port == data);
                    file = &port->File;
                    enabled = port->Enabled;
                    port->Enabled = TRUE;
                    ExReleaseFastMutex (&Globals.Mutex);

                    if (!enabled) {
                        status = KbdEnableDisablePort (TRUE, Irp, data, file);
                        if (!NT_SUCCESS (status)) {
                            port->Enabled = FALSE;
                            // ASSERT (Globals.AssocClassList[data->UnitId].Enabled);
                        } else {
                            //
                            // This is not the first kb to start, make sure its
                            // lights are set according to the indicators on the
                            // other kbs
                            //
                            PVOID startBuffer;

                            stack = IoGetNextIrpStackLocation (Irp);
                            stack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
                            stack->Parameters.DeviceIoControl.IoControlCode =
                                IOCTL_KEYBOARD_SET_INDICATORS;

                            stack->FileObject = *file;
                            stack->Parameters.DeviceIoControl.OutputBufferLength = 0;
                            stack->Parameters.DeviceIoControl.InputBufferLength =
                                sizeof (KEYBOARD_INDICATOR_PARAMETERS);

                            startStatus = Irp->IoStatus.Status;
                            startInformation = Irp->IoStatus.Information;
                            startBuffer = Irp->AssociatedIrp.SystemBuffer;

                            Irp->IoStatus.Information = 0;
                            Irp->AssociatedIrp.SystemBuffer =
                                &Globals.GrandMaster->IndicatorParameters;

                            status =
                                KeyboardSendIrpSynchronously (data->TopPort,
                                                              Irp,
                                                              FALSE);

                            //
                            // We don't care if we succeeded or not...
                            // set everything back to the way it was and
                            // continue with the start
                            //
                            status = STATUS_SUCCESS;
                            Irp->IoStatus.Status = startStatus;
                            Irp->IoStatus.Information = startInformation;
                            Irp->AssociatedIrp.SystemBuffer = startBuffer;
                        }
                    }
                } else {
                    ASSERT (!Globals.AssocClassList[data->UnitId].Enabled);
                    ExReleaseFastMutex (&Globals.Mutex);
                }
            } else {
                ExReleaseFastMutex (&Globals.Mutex);
                ASSERT (data->Self == data->TrueClassDevice);
                status=IoSetDeviceInterfaceState(&data->SymbolicLinkName, TRUE);
            }

            //
            // Start up the Wait Wake Engine if required.
            //
            if (SHOULD_SEND_WAITWAKE (data)) {
                KeyboardClassCreateWaitWakeIrp (data);
            }

        }

        //
        // We must now complete the IRP, since we stopped it in the
        // completetion routine with MORE_PROCESSING_REQUIRED.
        //
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        break;

    case IRP_MN_STOP_DEVICE:
        //
        // After the start IRP has been sent to the lower driver object, the
        // bus may NOT send any more IRPS down ``touch'' until another START
        // has occured.
        // What ever access is required must be done before the Irp is passed
        // on.
        //

        //
        // Do what ever
        //

        //
        // Stop Device touching the hardware KbdStopDevice(data, TRUE);
        //
        if (data->Started) {
            ExAcquireFastMutex (&Globals.Mutex);
            if (Globals.GrandMaster) {
                if (0 < Globals.Opens) {
                    port = &Globals.AssocClassList[data->UnitId];
                    ASSERT (port->Port == data);
                    file = &(port->File);
                    enabled = port->Enabled;
                    port->Enabled = FALSE;
                    ExReleaseFastMutex (&Globals.Mutex);

                    if (enabled) {
                        notifyHandle = InterlockedExchangePointer (
                                           &data->TargetNotifyHandle,
                                           NULL);

                        if (NULL != notifyHandle) {
                            IoUnregisterPlugPlayNotification (notifyHandle);
                        }

                        status = KbdEnableDisablePort (FALSE, Irp, data, file);
                        ASSERTMSG ("Could not close open port", NT_SUCCESS(status));
                    } else {
                        ASSERT (NULL == data->TargetNotifyHandle);
                    }
                } else {
                    ASSERT (!Globals.AssocClassList[data->UnitId].Enabled);
                    ExReleaseFastMutex (&Globals.Mutex);
                }
            } else {
                ExReleaseFastMutex (&Globals.Mutex);
            }
        }

        data->Started = FALSE;

        //
        // We don't need a completion routine so fire and forget.
        //
        // Set the current stack location to the next stack location and
        // call the next device object.
        //
        IoSkipCurrentIrpStackLocation (Irp);
        status = IoCallDriver (data->TopPort, Irp);
        break;

    case IRP_MN_REMOVE_DEVICE:
        //
        // The PlugPlay system has dictacted the removal of this device.  We
        // have no choise but to detach and delete the device objecct.
        // (If we wanted to express and interest in preventing this removal,
        // we should have filtered the query remove and query stop routines.)
        //

        waitWakeIrp = (PIRP)
            InterlockedExchangePointer ((PVOID*)&data->WaitWakeIrp, NULL);

        if (waitWakeIrp) {
            IoCancelIrp (waitWakeIrp);
        }

        IoWMIRegistrationControl (data->Self, WMIREG_ACTION_DEREGISTER);

        if (data->Started) {
            //
            // Stop the device without touching the hardware.
            // KbdStopDevice(data, FALSE);
            //
            // NB sending down the enable disable does NOT touch the hardware
            // it instead sends down a close file.
            //
            ExAcquireFastMutex (&Globals.Mutex);
            if (Globals.GrandMaster) {
                if (0 < Globals.Opens) {
                    port = &Globals.AssocClassList[data->UnitId];
                    ASSERT (port->Port == data);
                    file = &(port->File);
                    enabled = port->Enabled;
                    port->Enabled = FALSE;
                    ExReleaseFastMutex (&Globals.Mutex);

                    //
                    // ASSERT (NULL == data->notifyHandle);
                    //
                    // If we have a grand master, that means we did the
                    // creation locally and registered for notification.
                    // we should have closed the file during
                    // TARGET_DEVICE_QUERY_REMOVE, but we will have not
                    // gotten rid of the notify handle.
                    //
                    // Of course if we receive a surprise removal then
                    // we should not have received the query cancel.
                    // In which case we should have received a
                    // TARGET_DEVICE_REMOVE_COMPLETE where we would have
                    // both closed the file and removed cleared the
                    // notify handle
                    //
                    // Either way the file should be closed by now.
                    //
                    ASSERT (!enabled);
                    // if (enabled) {
                    //     status = KbdEnableDisablePort (FALSE, Irp, data, file);
                    //     ASSERTMSG ("Could not close open port", NT_SUCCESS(status));
                    // }

                    notifyHandle = InterlockedExchangePointer (
                                       &data->TargetNotifyHandle,
                                       NULL);

                    if (NULL != notifyHandle) {
                        IoUnregisterPlugPlayNotification (notifyHandle);
                    }

                } else {
                    ASSERT (!Globals.AssocClassList[data->UnitId].Enabled);
                    ExReleaseFastMutex (&Globals.Mutex);
                }
            } else {
                ExReleaseFastMutex (&Globals.Mutex);
            }
         }

        //
        // Here if we had any outstanding requests in a personal queue we should
        // complete them all now.
        //
        // Note, the device is guarenteed stopped, so we cannot send it any non-
        // PNP IRPS.
        //

        //
        // Send on the remove IRP
        //
        IoCopyCurrentIrpStackLocationToNext (Irp);
        status = IoCallDriver (data->TopPort, Irp);

        ExAcquireFastMutex (&Globals.Mutex);
        if (Globals.GrandMaster) {
            ASSERT (Globals.GrandMaster->Self == data->TrueClassDevice);
            //
            // We must remove ourself from the Assoc List
            //

            if (1 < Globals.NumAssocClass) {
                ASSERT (Globals.AssocClassList[data->UnitId].Port == data);

                Globals.AssocClassList[data->UnitId].Free = TRUE;
                Globals.AssocClassList[data->UnitId].File = NULL;
                Globals.AssocClassList[data->UnitId].Port = NULL;

            } else {
                ASSERT (1 == Globals.NumAssocClass);
                Globals.NumAssocClass = 0;
                ExFreePool (Globals.AssocClassList);
                Globals.AssocClassList = NULL;
            }
            ExReleaseFastMutex (&Globals.Mutex);

        } else {
            //
            // We are removing the one and only port associated with this class
            // device object.
            //
            ExReleaseFastMutex (&Globals.Mutex);
            ASSERT (data->TrueClassDevice == data->Self);
            ASSERT (Globals.ConnectOneClassToOnePort);
            status = IoSetDeviceInterfaceState (&data->SymbolicLinkName, FALSE);

            KeyboardClassCleanupQueue (data->Self, data, NULL);
        }

        IoReleaseRemoveLockAndWait (&data->RemoveLock, Irp);

        IoDetachDevice (data->TopPort);

        //
        // Clean up memory
        //

        RtlFreeUnicodeString (&data->SymbolicLinkName);
        ExFreePool (data->InputData);
        IoDeleteDevice (data->Self);
        return STATUS_SUCCESS;

    case IRP_MN_QUERY_PNP_DEVICE_STATE:

        if (data->AllowDisable == FALSE) {
            status = KeyboardSendIrpSynchronously (data->TopPort, Irp, TRUE);

            if (status == STATUS_NOT_SUPPORTED ||
                status == STATUS_INVALID_DEVICE_REQUEST) {
                status = STATUS_SUCCESS;
            }

            #define BAD_PNP_DEVICE_STATE (PNP_DEVICE_DISABLED           |  \
                                          PNP_DEVICE_DONT_DISPLAY_IN_UI |   \
                                          PNP_DEVICE_FAILED             |   \
                                          PNP_DEVICE_REMOVED)

            if (NT_SUCCESS (status) &&
                (((PNP_DEVICE_STATE) Irp->IoStatus.Information) & BAD_PNP_DEVICE_STATE) == 0) 
			{
                Irp->IoStatus.Information |= PNP_DEVICE_NOT_DISABLEABLE;
            }

            Irp->IoStatus.Status = status;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);

            break;
        }
        else {
            // ||   fall  ||
            // \/ through \/
            ;
        }

    case IRP_MN_QUERY_REMOVE_DEVICE:
    case IRP_MN_CANCEL_REMOVE_DEVICE:
    case IRP_MN_QUERY_STOP_DEVICE:
    case IRP_MN_CANCEL_STOP_DEVICE:
    case IRP_MN_QUERY_DEVICE_RELATIONS:
    case IRP_MN_QUERY_INTERFACE:
    case IRP_MN_QUERY_CAPABILITIES:
    case IRP_MN_QUERY_RESOURCES:
    case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
    case IRP_MN_READ_CONFIG:
    case IRP_MN_WRITE_CONFIG:
    case IRP_MN_EJECT:
    case IRP_MN_SET_LOCK:
    case IRP_MN_QUERY_ID:
    default:
        //
        // Here the filter driver might modify the behavior of these IRPS
        // Please see PlugPlay documentation for use of these IRPs.
        //

        IoCopyCurrentIrpStackLocationToNext (Irp);
        status = IoCallDriver (data->TopPort, Irp);
        break;
    }

    IoReleaseRemoveLock (&data->RemoveLock, Irp);

    return status;
}





NTSTATUS
KeyboardClassEnableGlobalPort(
    IN PDEVICE_EXTENSION Port,
    IN BOOLEAN Enabled
    )
{
    NTSTATUS    status = STATUS_SUCCESS;
    PPORT       globalPort = NULL;
    BOOLEAN     enabled; //, testVal;
    ULONG       i;

    PAGED_CODE ();

    ExAcquireFastMutex (&Globals.Mutex);
    if (0 < Globals.Opens) {
        for (i = 0; i < Globals.NumAssocClass; i++) {
            if (! Globals.AssocClassList [i].Free) {
                if (Globals.AssocClassList[i].Port == Port) {
                    globalPort = &Globals.AssocClassList [i];
                    break;
                }
            }
        }
        ASSERTMSG ("What shall I do now?\n", (NULL != globalPort));

        enabled = globalPort->Enabled;
        globalPort->Enabled = Enabled;
        ExReleaseFastMutex (&Globals.Mutex);

        //
        // Check to see if the port should change state. If so, send the new state
        // down the stack
        //
        if (Enabled != enabled) {
            status = KbdEnableDisablePort (Enabled,
                                           NULL,
                                           Port,
                                           &globalPort->File);
        }
    } else {
        ExReleaseFastMutex (&Globals.Mutex);
    }

    return status;
}

NTSTATUS
KeyboardClassPlugPlayNotification(
    IN PVOID NotificationStructure2, //PTARGET_DEVICE_REMOVAL_NOTIFICATION
    IN PDEVICE_EXTENSION Port
    )
/*++

Routine Description:

    This routine is called as the result of recieving PlugPlay Notifications
    as registered by the previous call to IoRegisterPlugPlayNotification.

    Currently this should only occur for Target Device Notifications

Arguments:

    NotificationStructure - what happened.
    Port - The Fdo to which things happened.

Return Value:



--*/
{
    NTSTATUS    status = STATUS_SUCCESS;
    PVOID       notify = NULL;
    PTARGET_DEVICE_REMOVAL_NOTIFICATION NotificationStructure =
		(PTARGET_DEVICE_REMOVAL_NOTIFICATION) NotificationStructure2;

    PAGED_CODE ();

    ASSERT (Globals.GrandMaster->Self == Port->TrueClassDevice);

    if (IsEqualGUID ((REFGUID) (NotificationStructure->Event),
                     (REFGUID) GUID_TARGET_DEVICE_QUERY_REMOVE)) {

        //
        // Our port device object will soon be receiving a query remove.
        // Before that query remove will actually be sent to the device object
        // stack itself the plug and play subsystem will send those registered
        // for target device notification the message first.
        //

        //
        // What we should do is now close the handle.
        // Because if we do not the query remove will fail before it ever
        // gets to the IRP_MJ_PNP IRP_MN_QUERY_REMOVE stage, as the PlugPlay
        // system fails before it is sent based on there being an open handle
        // to the device.
        //
        // DbgPrint ("Keyboard QUERY Remove\n");
        // DbgBreakPoint();

        status = KeyboardClassEnableGlobalPort (Port, FALSE);

    } else if(IsEqualGUID ((REFGUID)(NotificationStructure->Event),
                           (REFGUID)GUID_TARGET_DEVICE_REMOVE_COMPLETE)) {

        //
        // Here the device has disappeared.
        //
        // DbgPrint ("Keyboard Remove Complete\n");
        // DbgBreakPoint();

        notify = InterlockedExchangePointer (&Port->TargetNotifyHandle,
                                             NULL);

        if (NULL != notify) {
            //
            // Deregister
            //
            IoUnregisterPlugPlayNotification (notify);

            status = KeyboardClassEnableGlobalPort (Port, FALSE);
        }

    } else if(IsEqualGUID ((REFGUID)(NotificationStructure->Event),
                           (REFGUID)GUID_TARGET_DEVICE_REMOVE_CANCELLED)) {

        //
        // The query remove has been revoked.
        // Reopen the device.
        //
        // DbgPrint ("Keyboard Remove Complete\n");
        // DbgBreakPoint();

        notify = InterlockedExchangePointer (&Port->TargetNotifyHandle,
                                             NULL);

        if (NULL != notify) {
            //
            // Deregister
            //
            IoUnregisterPlugPlayNotification (notify);

            //
            // If the notify handle is no longer around then this device must
            // have been removed, so there is no point trying to create again.
            //
            status = KeyboardClassEnableGlobalPort (Port, TRUE);
        }
    }

    return status;
}




VOID
KeyboardClassPoRequestComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus
    );

NTSTATUS
KeyboardClassPowerComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

VOID
KeyboardClassWaitWakeComplete(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus
    )
/*++

Routine Description:
    Catch the Wait wake Irp on its way back.

Return Value:

--*/
{
    PDEVICE_EXTENSION       data = (PDEVICE_EXTENSION)Context;
    POWER_STATE             powerState;
    NTSTATUS                status;
    PKEYBOARD_WORK_ITEM     item;

    ASSERT (MinorFunction == IRP_MN_WAIT_WAKE);
    //
    // PowerState.SystemState is undefined when the WW irp has been completed
    //
    // ASSERT (PowerState.SystemState == PowerSystemWorking);

    InterlockedExchangePointer((PVOID*)&data->WaitWakeIrp, NULL);

    switch (IoStatus->Status) {
    case STATUS_SUCCESS:
        KbdPrint((1, "KbdClass: Wake irp was completed succefully.\n"));

        //
        //      We need to request a set power to power up the device.
        //
        powerState.DeviceState = PowerDeviceD0;
        status = PoRequestPowerIrp(
                    data->PDO,
                    IRP_MN_SET_POWER,
                    powerState,
                    NULL,
                    NULL,
                    NULL);

        //
        // We do not notify the system that a user is present because:
        // 1  Win9x doesn't do this and we must maintain compatibility with it
        // 2  The USB PIX4 motherboards sends a wait wake event every time the
        //    machine wakes up, no matter if this device woke the machine or not
        // 
        // If we incorrectly notify the system a user is present, the following
        // will occur:
        // 1  The monitor will be turned on
        // 2  We will prevent the machine from transitioning from standby 
        //    (to PowerSystemWorking) to hibernate
        //
        // If a user is truly present, we will receive input in the service
        // callback and we will notify the system at that time.
        //
        // PoSetSystemState (ES_USER_PRESENT);

        if (data->WaitWakeEnabled) {
            //
            // We cannot call CreateWaitWake from this completion routine,
            // as it is a paged function.
            //
            item = (PKEYBOARD_WORK_ITEM)
                    ExAllocatePool (NonPagedPool, sizeof (KEYBOARD_WORK_ITEM));

            if (NULL != item) {
                item->Data = data;
                item->Irp = NULL;
                status = IoAcquireRemoveLock (&data->RemoveLock, item);
                if (NT_SUCCESS(status)) {
                    ExInitializeWorkItem (&item->Item,
                                          (PWORKER_THREAD_ROUTINE)KeyboardClassCreateWaitWakeIrpWorker,
                                          item);
                    ExQueueWorkItem (&item->Item, DelayedWorkQueue);
                }
                else {
                    //
                    // The device has been removed
                    //
                    ExFreePool(item);
                }
            } else {
                //
                // Well, we dropped the WaitWake.
                //
                // Print a warning to the debugger, and log an error.
                //
                DbgPrint ("KbdClass: WARNING: Failed alloc pool -> no WW Irp\n");

                ASSERT (NULL != data->WWErrorLog);

                data->WWErrorLog->ErrorCode = KBDCLASS_NO_RESOURCES_FOR_WAITWAKE;
                data->WWErrorLog->DumpDataSize = 0;
                data->WWErrorLog->SequenceNumber = 0;
                data->WWErrorLog->MajorFunctionCode = 0;
                data->WWErrorLog->IoControlCode = 0;
                data->WWErrorLog->RetryCount = 0;
                data->WWErrorLog->UniqueErrorValue = 1;
                data->WWErrorLog->FinalStatus = STATUS_INSUFFICIENT_RESOURCES;

                IoWriteErrorLogEntry (data->WWErrorLog);
            }
        }

        // fall through to the break

    //
    // We get a remove.  We will not (obviously) send another wait wake
    //
    case STATUS_CANCELLED:

    //
    // This status code will be returned if the device is put into a power state
    // in which we cannot wake the machine (hibernate is a good example).  When
    // the device power state is returned to D0, we will attempt to rearm wait wake
    //
    case STATUS_POWER_STATE_INVALID:
    case STATUS_ACPI_POWER_REQUEST_FAILED:

    //
    // We failed the Irp because we already had one queued, or a lower driver in
    // the stack failed it.  Either way, don't do anything.
    //
    case STATUS_INVALID_DEVICE_STATE:

    //
    // Somehow someway we got two WWs down to the lower stack.
    // Let's just don't worry about it.
    //
    case STATUS_DEVICE_BUSY:
        break;

    default:
        //
        // Something went wrong, disable the wait wake.
        //
        KdPrint(("KBDCLASS:  wait wake irp failed with %x\n", IoStatus->Status));
        KeyboardToggleWaitWake (data, FALSE);
    }

}

BOOLEAN
KeyboardClassCheckWaitWakeEnabled(
    IN PDEVICE_EXTENSION Data
    )
{
    KIRQL irql;
    BOOLEAN enabled;

    KeAcquireSpinLock (&Data->WaitWakeSpinLock, &irql);
    enabled = Data->WaitWakeEnabled;
    KeReleaseSpinLock (&Data->WaitWakeSpinLock, irql);

    return enabled;
}

void
KeyboardClassCreateWaitWakeIrpWorker (
    IN PKEYBOARD_WORK_ITEM  Item
    )
{
    PAGED_CODE ();

    KeyboardClassCreateWaitWakeIrp (Item->Data);
    IoReleaseRemoveLock (&Item->Data->RemoveLock, Item);
    ExFreePool (Item);
}

BOOLEAN
KeyboardClassCreateWaitWakeIrp (
    IN PDEVICE_EXTENSION Data
    )
/*++

Routine Description:
    Catch the Wait wake Irp on its way back.

Return Value:

--*/
{
    POWER_STATE powerState;
    BOOLEAN     success = TRUE;
    NTSTATUS    status;
    PIRP        waitWakeIrp;

    PAGED_CODE ();

    powerState.SystemState = Data->MinSystemWakeState;
    status = PoRequestPowerIrp (Data->PDO,
                                IRP_MN_WAIT_WAKE,
                                powerState,
                                KeyboardClassWaitWakeComplete,
                                Data,
                                NULL);

    if (status != STATUS_PENDING) {
        //
        // If we actually got a WW irp, cancel it
        //
        waitWakeIrp = (PIRP)
            InterlockedExchangePointer ((PVOID*)&Data->WaitWakeIrp, NULL);

        if (waitWakeIrp) {
            IoCancelIrp (waitWakeIrp);
        }

        success = FALSE;
    }

    return success;
}


NTSTATUS
KeyboardToggleWaitWake(
    PDEVICE_EXTENSION Data,
    BOOLEAN           WaitWakeState
    )
{
    NTSTATUS       status;
    PKEYBOARD_WORK_ITEM item;

    status = IoAcquireRemoveLock (&Data->RemoveLock, KeyboardToggleWaitWakeWorker);
    if (!NT_SUCCESS (status)) {
        //
        // Device has gone away, just silently exit
        //
        return status;
    }

    item = (PKEYBOARD_WORK_ITEM) ExAllocatePool(NonPagedPool, sizeof(KEYBOARD_WORK_ITEM));
    if (item) {
        item->Data = Data;
        item->WaitWakeState = WaitWakeState;

        if (KeGetCurrentIrql() == PASSIVE_LEVEL) {
            //
            // We are safely at PASSIVE_LEVEL, call callback directly to perform
            // this operation immediately.
            //
            KeyboardToggleWaitWakeWorker (item);

        } else {
            //
            // We are not at PASSIVE_LEVEL, so queue a workitem to handle this
            // at a later time.
            //
            ExInitializeWorkItem(&item->Item,
                                 (PWORKER_THREAD_ROUTINE)KeyboardToggleWaitWakeWorker,
                                 item);
            ExQueueWorkItem(&item->Item, DelayedWorkQueue);
        }
    }
    else {
        IoReleaseRemoveLock (&Data->RemoveLock, KeyboardToggleWaitWakeWorker);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
KeyboardClassPower (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    The power dispatch routine.

    In all cases it must call PoStartNextPowerIrp
    In all cases (except failure) it must pass on the IRP to the lower driver.

Arguments:

   DeviceObject - pointer to a device object.

   Irp - pointer to an I/O Request Packet.

Return Value:

      NT status code

--*/
{
    POWER_STATE_TYPE        powerType;
    PIO_STACK_LOCATION      stack;
    PDEVICE_EXTENSION       data;
    PKEYBOARD_WORK_ITEM     item;
    PKEYBOARD_POWER_CONTEXT context;
    NTSTATUS        status;
    POWER_STATE     powerState;
    BOOLEAN         hookit = FALSE;
    BOOLEAN         skipit = FALSE;

    PAGED_CODE ();

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    stack = IoGetCurrentIrpStackLocation (Irp);
    powerType = stack->Parameters.Power.Type;
    powerState = stack->Parameters.Power.State;

    if (data == Globals.GrandMaster) {
        //
        // We should never get a power irp to the grand master.
        //
        ASSERT (data != Globals.GrandMaster);
        PoStartNextPowerIrp (Irp);
        Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return STATUS_NOT_SUPPORTED; 

    } else if (!data->PnP) {
        //
        // We should never get a power irp to a non PnP device object.
        //
        ASSERT (data->PnP);
        PoStartNextPowerIrp (Irp);
        Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return STATUS_NOT_SUPPORTED;
    }

    status = IoAcquireRemoveLock (&data->RemoveLock, Irp);

    if (!NT_SUCCESS (status)) {
        PoStartNextPowerIrp (Irp);
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    switch (stack->MinorFunction) {
    case IRP_MN_SET_POWER:
        KbdPrint((2,"KBDCLASS-PnP Setting %s state to %d\n",
                  ((powerType == SystemPowerState) ?  "System" : "Device"),
                  powerState.SystemState));

        switch (powerType) {
        case DevicePowerState:
            // ASSERT (data->DeviceState != powerState.DeviceState);

            if (data->DeviceState < powerState.DeviceState) {
                //
                // Powering down
                //
    
                PoSetPowerState (data->Self, powerType, powerState);
                if (PowerDeviceD0 == data->DeviceState) {

                    item = (PKEYBOARD_WORK_ITEM) ExAllocatePool (
                                                  NonPagedPool,
                                                  sizeof (KEYBOARD_WORK_ITEM));

                    if (NULL == item) {
                        status = STATUS_INSUFFICIENT_RESOURCES;
                        break;
                    }

                    item->Data = data;
                    item->Irp = Irp;
                    status = IoAcquireRemoveLock (&data->RemoveLock, Irp);
#if KEYBOARD_USE_WORK_THREAD
                    ASSERT (STATUS_SUCCESS == status);
                    ExInitializeWorkItem (&item->Item, KbdSetLights, item);
                    ExQueueWorkItem (&item->Item, DelayedWorkQueue);
#else
                    KbdSetLights (item);
#endif
                    skipit = TRUE;
                }
                data->DeviceState = powerState.DeviceState;

            } else if (powerState.DeviceState < data->DeviceState) {
                //
                // Powering Up
                //
                hookit = TRUE;
            } // else { no change }.
            break;

        case SystemPowerState:

            data->PowerQueryLock = FALSE;

            if (data->SystemState < powerState.SystemState) {
                //
                // Powering down
                //
                
                if (PowerSystemWorking == data->SystemState) {

                    item = (PKEYBOARD_WORK_ITEM) ExAllocatePool (
                                                  NonPagedPool,
                                                  sizeof (KEYBOARD_WORK_ITEM));

                    if (NULL == item) {
                        goto TransitionToLowSystemPower; 
                    } else {
                        item->Data = data;
                        item->Irp = Irp;
                        status = IoAcquireRemoveLock (&data->RemoveLock, Irp);
                        ASSERT (STATUS_SUCCESS == status);
#if KEYBOARD_USE_WORK_THREAD
                        ExInitializeWorkItem (&item->Item, KbdFlashLights, item);
                        ExQueueWorkItem (&item->Item, DelayedWorkQueue);
#else
                        KbdFlashLights (item);
#endif
                        skipit = TRUE;
                    }

                } else {
TransitionToLowSystemPower:
                    status = IoAcquireRemoveLock (&data->RemoveLock, Irp);
                    ASSERT (STATUS_SUCCESS == status);

                    if (WAITWAKE_ON (data) &&
                        powerState.SystemState < PowerSystemHibernate) {
                        ASSERT (powerState.SystemState >= PowerSystemWorking &&
                                powerState.SystemState < PowerSystemHibernate);
                
                        powerState.DeviceState =
                            data->SystemToDeviceState[powerState.SystemState];
                    } else {
                        powerState.DeviceState = PowerDeviceD3;
                    }
                
                    status  = PoRequestPowerIrp (data->Self,
                                                 IRP_MN_SET_POWER,
                                                 powerState,
                                                 KeyboardClassPoRequestComplete,
                                                 Irp,
                                                 NULL);

                    if (status != STATUS_PENDING) {
                        //
                        // Failure...release the reference we just took
                        //
                        IoReleaseRemoveLock (&data->RemoveLock, Irp);
                        ASSERT (!NT_SUCCESS (status));
                    }

                    skipit = TRUE;
                }

            } else if (powerState.SystemState < data->SystemState) {
                //
                // Powering Up
                //
                hookit = TRUE;
            } else {
                //
                // No change, but we want to make sure a wait wake irp is sent.
                //
                if (powerState.SystemState == PowerSystemWorking &&
                    SHOULD_SEND_WAITWAKE (data)) {
                    KeyboardClassCreateWaitWakeIrp (data);
                }
            }
            break;
        }

        break;

    case IRP_MN_QUERY_POWER:
        ASSERT (SystemPowerState == powerType);

        //
        // Fail the query if we can't wake the machine.  We do, however, want to
        // let hibernate succeed no matter what (besides, it is doubtful that a 
        // keyboard can wait wake the machine out of S4).
        //
        if (powerState.SystemState < PowerSystemHibernate       &&
            powerState.SystemState > data->MinSystemWakeState   &&
            WAITWAKE_ON(data)) {
            status = STATUS_POWER_STATE_INVALID;
        }
        else {
            data->PowerQueryLock = TRUE;
            status = STATUS_SUCCESS;
        }

        Irp->IoStatus.Status = status;
        break;

    case IRP_MN_WAIT_WAKE:
        if (InterlockedCompareExchangePointer((PVOID*)&data->WaitWakeIrp,
                                              Irp, 
                                              NULL) != NULL) {
            //
            // More than one WW irp?  Unthinkable!
            //
            status = STATUS_INVALID_DEVICE_STATE;
        }
        else {
            status = STATUS_SUCCESS;
        }
        break;

    default:
        break;
    }

    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        PoStartNextPowerIrp (Irp);
        IoCompleteRequest (Irp, IO_NO_INCREMENT);

    } else if (hookit) {
        status = IoAcquireRemoveLock (&data->RemoveLock, Irp);
        ASSERT (STATUS_SUCCESS == status);
        IoCopyCurrentIrpStackLocationToNext (Irp);

        context = (PKEYBOARD_POWER_CONTEXT)ExAllocatePool(NonPagedPool, sizeof (KEYBOARD_POWER_CONTEXT));
        if (NULL == context) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            PoStartNextPowerIrp (Irp);
            Irp->IoStatus.Status = status;
            IoCompleteRequest (Irp, IO_KEYBOARD_INCREMENT);
            IoReleaseRemoveLock (&data->RemoveLock, Irp);
        } else {
            context->State = KeyboardPowerStateStart;

            IoSetCompletionRoutine (Irp,
                                    KeyboardClassPowerComplete,
                                    context,
                                    TRUE,
                                    TRUE,
                                    TRUE);

            PoCallDriver (data->TopPort, Irp);

            //
            // We are returning pending instead of the result from PoCallDriver becuase:
            // 1  we are changing the status in the completion routine
            // 2  we will not be completing this irp in the completion routine
            //
            status = STATUS_PENDING;
        }

    } else if (skipit) {
        status = STATUS_PENDING;
    } else {
        IoCopyCurrentIrpStackLocationToNext (Irp);

        PoStartNextPowerIrp (Irp);
        status = PoCallDriver (data->TopPort, Irp);
    }

    IoReleaseRemoveLock (&data->RemoveLock, Irp);
    return status;
}

VOID
KeyboardClassPoRequestComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE D_PowerState,
    IN PVOID S_Irp2, // The S irp that caused us to request the power.
    IN PIO_STATUS_BLOCK IoStatus
    )
{
    POWER_STATE         S_powerState;
    POWER_STATE_TYPE    S_powerType;
    PIO_STACK_LOCATION  stack;
    PDEVICE_EXTENSION   data;
    PKEYBOARD_WORK_ITEM item;
    PIRP S_Irp = (PIRP)S_Irp2;

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    stack = IoGetCurrentIrpStackLocation (S_Irp);
    S_powerType = stack->Parameters.Power.Type;
    S_powerState = stack->Parameters.Power.State;

    if (data->SystemState < S_powerState.SystemState) {
        //
        // Powering Down
        //
        data->SystemState = S_powerState.SystemState;

        PoStartNextPowerIrp (S_Irp);
        IoCopyCurrentIrpStackLocationToNext (S_Irp);
        PoCallDriver (data->TopPort, S_Irp);

    } else if (S_powerState.SystemState < data->SystemState) {
        //
        // Powering Up
        //
        data->SystemState = S_powerState.SystemState;

        //
        // Set the S irp status to the D irp status 
        //
        S_Irp->IoStatus.Status = IoStatus->Status;
        PoStartNextPowerIrp (S_Irp);
        IoCompleteRequest (S_Irp, IO_NO_INCREMENT);

        //
        // We have come back to the PowerSystemWorking state and the device is
        // fully powered.  If we can (and should), send a wait wake irp down
        // the stack.  This is necessary because we might have gone into a power
        // state where the wait wake irp was invalid.
        //
        ASSERT(data->SystemState == PowerSystemWorking);

        if (SHOULD_SEND_WAITWAKE (data)) {
            //
            // We cannot call CreateWaitWake from this completion routine,
            // as it is a paged function.
            //
            item = (PKEYBOARD_WORK_ITEM)
                    ExAllocatePool (NonPagedPool, sizeof (KEYBOARD_WORK_ITEM));

            if (NULL != item) {
                item->Data = data;
                item->Irp = NULL;
                IoAcquireRemoveLock (&data->RemoveLock, item);
                //
                // We are in the context of an already acrired remlock, for this
                // current Irp, so this should succeed.
                //
                ExInitializeWorkItem (&item->Item,
                                      (PWORKER_THREAD_ROUTINE)KeyboardClassCreateWaitWakeIrpWorker,
                                      item);
                ExQueueWorkItem (&item->Item, DelayedWorkQueue);
            } else {
                //
                // Well, we dropped the WaitWake.
                //
                // Print a warning to the debugger, and log an error.
                //
                DbgPrint ("KbdClass: WARNING: Failed alloc pool -> no WW Irp\n");

                ASSERT (NULL != data->WWErrorLog);

                data->WWErrorLog->ErrorCode = KBDCLASS_NO_RESOURCES_FOR_WAITWAKE;
                data->WWErrorLog->DumpDataSize = 0;
                data->WWErrorLog->SequenceNumber = 0;
                data->WWErrorLog->MajorFunctionCode = 0;
                data->WWErrorLog->IoControlCode = 0;
                data->WWErrorLog->RetryCount = 0;
                data->WWErrorLog->UniqueErrorValue = 1;
                data->WWErrorLog->FinalStatus = STATUS_INSUFFICIENT_RESOURCES;

                IoWriteErrorLogEntry (data->WWErrorLog);
            }
        }

    } else {
        ASSERTMSG ("KeyboardClass: Invalid power state\n", FALSE);
    }

    IoReleaseRemoveLock (&data->RemoveLock, S_Irp);
}

NTSTATUS
KeyboardClassPowerComplete (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context2  //PKEYBOARD_POWER_CONTEXT
    )
{
    NTSTATUS            status;
    POWER_STATE         powerState;
    POWER_STATE_TYPE    powerType;
    PIO_STACK_LOCATION  stack, next;
//  PIRP                setLEDs;
    PDEVICE_EXTENSION   data;
//  IO_STATUS_BLOCK     block;
    PFILE_OBJECT        file;
    PKEYBOARD_INDICATOR_PARAMETERS params;
    PKEYBOARD_POWER_CONTEXT Context = (PKEYBOARD_POWER_CONTEXT)Context2;

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    stack = IoGetCurrentIrpStackLocation (Irp);
    next = IoGetNextIrpStackLocation (Irp);
    powerType = stack->Parameters.Power.Type;
    powerState = stack->Parameters.Power.State;

    ASSERT (data != Globals.GrandMaster);
    ASSERT (data->PnP);
    ASSERT (NULL != Context);

    switch (stack->MinorFunction) {
    case IRP_MN_SET_POWER:
        switch (powerType) {
        case DevicePowerState:
            ASSERT (powerState.DeviceState < data->DeviceState);
            //
            // Powering up
            //

            switch (Context->State) {
            case KeyboardPowerStateStart:
                //
                // Reset the keyboard Indicators.
                //
                if (Globals.GrandMaster) {
                    params = &Globals.GrandMaster->IndicatorParameters;
                    file = Globals.AssocClassList[data->UnitId].File;
                } else {
                    params = &data->IndicatorParameters;
                    file = stack->FileObject;
                }

                //
                // This is a completion routine.  We could be at DISPATCH_LEVEL
                // Therefore we must bounce the IRP
                //

                next->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
                next->Parameters.DeviceIoControl.IoControlCode =
                    IOCTL_KEYBOARD_SET_INDICATORS;
                next->Parameters.DeviceIoControl.InputBufferLength =
                    sizeof (KEYBOARD_INDICATOR_PARAMETERS);
                next->Parameters.DeviceIoControl.OutputBufferLength = 0;
                next->FileObject = file;

                //
                // Temp swap out the system buffer to send the IOCTL
                // Shove it into the context of the next completion routine
                // Place it back.
                //

                Context->SystemBuffer = Irp->AssociatedIrp.SystemBuffer;
                Context->State = KeyboardPowerStateHaveSetLights;
                Context->Status = Irp->IoStatus.Status;

                IoSetCompletionRoutine (Irp,
                                        (PIO_COMPLETION_ROUTINE)KeyboardClassPowerComplete,
                                        Context,
                                        TRUE,
                                        TRUE,
                                        TRUE);

                Irp->AssociatedIrp.SystemBuffer = params;

                IoCallDriver (data->TopPort, Irp);

                return STATUS_MORE_PROCESSING_REQUIRED;

            case KeyboardPowerStateHaveSetLights:

                Irp->AssociatedIrp.SystemBuffer = Context->SystemBuffer;

                PoSetPowerState (data->Self, powerType, powerState);
                data->DeviceState = powerState.DeviceState;
                if (STATUS_DEVICE_DATA_ERROR == Irp->IoStatus.Status ||
                    STATUS_IO_TIMEOUT == Irp->IoStatus.Status) {
                    Irp->IoStatus.Status = Context->Status;
                }
                break;

            default:
                ASSERTMSG ("Bad power state", FALSE);
            }
            break;

        case SystemPowerState:
            ASSERT (powerState.SystemState < data->SystemState);
            //
            // Powering up
            //
            ASSERT (KeyboardPowerStateStart == Context->State);

            powerState.DeviceState = PowerDeviceD0;

            status = PoRequestPowerIrp (data->Self,
                                        IRP_MN_SET_POWER,
                                        powerState,
                                        KeyboardClassPoRequestComplete,
                                        Irp,
                                        NULL);

            if (status != STATUS_PENDING) {
                ASSERT (!NT_SUCCESS (status));

                //
                // Failure.  Complete the irp and remove the ref taken out in 
                // the power routine
                //
                Irp->IoStatus.Status = status;
                PoStartNextPowerIrp (Irp);
    
                IoReleaseRemoveLock (&data->RemoveLock, Irp);
            }
            else {
                //
                // We need to:
                // Start next power irp, release the removelock, and complete
                // the irp in the PoRequestComplete routine.
                //
                status = STATUS_MORE_PROCESSING_REQUIRED;
            }

            ExFreePool (Context);
            return status;
        }
        break;

    default:
        ASSERT (0xBADBAD == stack->MinorFunction);
        break;
    }

    ExFreePool (Context);
    PoStartNextPowerIrp (Irp);
    IoReleaseRemoveLock (&data->RemoveLock, Irp);

    return STATUS_SUCCESS;
}

void
KbdSetLights (
    PKEYBOARD_WORK_ITEM    Item
    )
/*++

Routine Description:
    Prepare to goto a non D0 state

--*/
{
    NTSTATUS            status, origStatus;
    PVOID               buffer;
    PIRP                irp;
    PDEVICE_EXTENSION   data;
    PIO_STACK_LOCATION  next, current;
    LARGE_INTEGER       time;
  //  KEVENT              event;
    KEYBOARD_INDICATOR_PARAMETERS indicators;
    USHORT leds [] =
    {
        KEYBOARD_NUM_LOCK_ON,
                               KEYBOARD_CAPS_LOCK_ON,
        KEYBOARD_NUM_LOCK_ON | KEYBOARD_CAPS_LOCK_ON,
                                                       KEYBOARD_SCROLL_LOCK_ON,
        KEYBOARD_NUM_LOCK_ON |                         KEYBOARD_SCROLL_LOCK_ON,
                               KEYBOARD_CAPS_LOCK_ON | KEYBOARD_SCROLL_LOCK_ON,
        KEYBOARD_NUM_LOCK_ON | KEYBOARD_CAPS_LOCK_ON | KEYBOARD_SCROLL_LOCK_ON,
    };

    PAGED_CODE ();

    data = Item->Data;
    irp = Item->Irp;

    //
    // Save off the original status so when we complete the irp, we will have 
    // keep the orig status and not a random error code that setting the lights
    // may produce.
    //
    origStatus = irp->IoStatus.Status;

    if (data->PowerLightTime) {
        next = IoGetNextIrpStackLocation (irp);
        current = IoGetCurrentIrpStackLocation (irp);
        buffer = irp->AssociatedIrp.SystemBuffer;
        irp->AssociatedIrp.SystemBuffer = (PVOID) &indicators;
        indicators.UnitId = 0;
        indicators.LedFlags = leds [current->Parameters.Power.State.DeviceState];

        time.QuadPart = -1 * 10000 * 1000 * data->PowerLightTime;

        next->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        next->Parameters.DeviceIoControl.InputBufferLength
            = next->Parameters.DeviceIoControl.OutputBufferLength
            = sizeof (KEYBOARD_INDICATOR_PARAMETERS);
        next->Parameters.DeviceIoControl.IoControlCode
            = IOCTL_KEYBOARD_SET_INDICATORS;

        next->FileObject = (Globals.GrandMaster)
                         ? Globals.AssocClassList[data->UnitId].File
                         : current->FileObject;


        status = KeyboardSendIrpSynchronously (data->TopPort,
                                               irp,
                                               FALSE);

        if (NT_SUCCESS (status)) {
            status = KeDelayExecutionThread (KernelMode, TRUE, &time);
        }
        irp->AssociatedIrp.SystemBuffer = buffer;
    }

    irp->IoStatus.Status = origStatus;

    PoStartNextPowerIrp (irp);
    IoSkipCurrentIrpStackLocation (irp);
    PoCallDriver (data->TopPort, irp);
    ExFreePool (Item);
    IoReleaseRemoveLock (&data->RemoveLock, irp);
}

void
KbdFlashLights (
    PKEYBOARD_WORK_ITEM Item
    )
{
    NTSTATUS status;
    ULONG    i;
//    KEVENT   event;
    PVOID    buffer;
    PIRP     irp;
    POWER_STATE         powerState;
    PDEVICE_EXTENSION   data;
    PIO_STACK_LOCATION  next, current;
    LARGE_INTEGER       time;
    KEYBOARD_INDICATOR_PARAMETERS indicators;
    USHORT leds [] =
    {
        KEYBOARD_NUM_LOCK_ON,
        KEYBOARD_NUM_LOCK_ON | KEYBOARD_CAPS_LOCK_ON,
        KEYBOARD_NUM_LOCK_ON | KEYBOARD_CAPS_LOCK_ON | KEYBOARD_SCROLL_LOCK_ON,
        KEYBOARD_CAPS_LOCK_ON | KEYBOARD_SCROLL_LOCK_ON,
        KEYBOARD_SCROLL_LOCK_ON,
        0
    };

    PAGED_CODE ();

    data = Item->Data;
    irp = Item->Irp;
    next = IoGetNextIrpStackLocation (irp);
    current = IoGetCurrentIrpStackLocation (irp);
    powerState = current->Parameters.Power.State;

    if (data->PowerLightTime) {
        buffer = irp->AssociatedIrp.SystemBuffer;
        irp->AssociatedIrp.SystemBuffer = &indicators;
        indicators.UnitId = 0;
        time.QuadPart = -1 * 10000 * 50 * data->PowerLightTime;

        for (i = 0; i < 6; i++) {

            next->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
            next->Parameters.DeviceIoControl.InputBufferLength
                = next->Parameters.DeviceIoControl.OutputBufferLength
                = sizeof (KEYBOARD_INDICATOR_PARAMETERS);
            next->Parameters.DeviceIoControl.IoControlCode
                = IOCTL_KEYBOARD_SET_INDICATORS;

            next->FileObject = (Globals.GrandMaster)
                             ? Globals.AssocClassList[data->UnitId].File
                             : current->FileObject;

            indicators.LedFlags = leds [i];

            status = KeyboardSendIrpSynchronously (data->TopPort,
                                                   irp,
                                                   FALSE);

            if (NT_SUCCESS (status)) {
                status = KeDelayExecutionThread (KernelMode, TRUE, &time);
            }

            if (!NT_SUCCESS (status)) {
                break;
            }
        }
        irp->AssociatedIrp.SystemBuffer = buffer;
    }

    if (WAITWAKE_ON (data) && powerState.SystemState < PowerSystemHibernate) {
        ASSERT(powerState.SystemState >= PowerSystemWorking &&
               powerState.SystemState < PowerSystemHibernate);

        powerState.DeviceState = data->SystemToDeviceState[powerState.SystemState];
    } else {
        powerState.DeviceState = PowerDeviceD3;
    }

    status = PoRequestPowerIrp (data->Self,
                                IRP_MN_SET_POWER,
                                powerState,
                                KeyboardClassPoRequestComplete,
                                irp,
                                NULL);

    if (status != STATUS_PENDING) {
        ASSERT (!NT_SUCCESS (status));

        //
        // Failure, complete the S irp now with the failure code
        //
        irp->IoStatus.Status = status;
        PoStartNextPowerIrp (irp);
        IoCompleteRequest (irp, IO_NO_INCREMENT);
        
        //
        // Release the lock taken out in the power routine
        //
        IoReleaseRemoveLock (&data->RemoveLock, irp);
    }

    ExFreePool (Item);
}

//
// WMI System Call back functions
//
NTSTATUS
KeyboardClassSystemControl (
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp
    )
/*++
Routine Description

    We have just received a System Control IRP.

    Assume that this is a WMI IRP and
    call into the WMI system library and let it handle this IRP for us.

--*/
{
    PDEVICE_EXTENSION   deviceExtension;
    NTSTATUS            status;
    SYSCTL_IRP_DISPOSITION disposition;

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    status = WmiSystemControl(&deviceExtension->WmiLibInfo,
                                 DeviceObject,
                                 Irp,
                                 &disposition);
    switch(disposition) {
    case IrpProcessed:
        //
        // This irp has been processed and may be completed or pending.
        //
        break;

    case IrpNotCompleted:
        //
        // This irp has not been completed, but has been fully processed.
        // we will complete it now
        //
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        break;

    case IrpForward:
    case IrpNotWmi:
        //
        // This irp is either not a WMI irp or is a WMI irp targetted
        // at a device lower in the stack.
        //
        status = KeyboardClassPassThrough(DeviceObject, Irp);
        break;

    default:
        //
        // We really should never get here, but if we do just forward....
        //
        ASSERT(FALSE);
        status = KeyboardClassPassThrough(DeviceObject, Irp);
        break;
    }

    IoReleaseRemoveLock (&deviceExtension->RemoveLock, Irp);
    return status;
}

NTSTATUS
KeyboardClassSetWmiDataItem(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG DataItemId,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    )
/*++

Routine Description:

    This routine is a callback into the driver to set for the contents of
    a data block. When the driver has finished filling the data block it
    must call ClassWmiCompleteRequest to complete the irp. The driver can
    return STATUS_PENDING if the irp cannot be completed immediately.

Arguments:

    DeviceObject is the device whose data block is being queried

    Irp is the Irp that makes this request

    GuidIndex is the index into the list of guids provided when the
        device registered

    InstanceIndex is the index that denotes which instance of the data block
        is being queried.

    DataItemId has the id of the data item being set

    BufferSize has the size of the data item passed

    Buffer has the new values for the data item


Return Value:

    status

--*/
{
    PDEVICE_EXTENSION   data;
    NTSTATUS            status;
    ULONG               size = 0;

    PAGED_CODE ();

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    switch(GuidIndex) {
    case WMI_CLASS_DRIVER_INFORMATION:
        status = STATUS_WMI_READ_ONLY;
        break;

    case WMI_WAIT_WAKE:

        //
        // Don't allow a toggle of wait wake when we are in the middle of a 
        // power transition
        //
        if (data->PowerQueryLock                    ||
            data->SystemState != PowerSystemWorking ||
            data->DeviceState !=  PowerDeviceD0) {
            status = STATUS_POWER_STATE_INVALID;
            break;
        }

        size = sizeof(BOOLEAN);

        if (BufferSize < size) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        } else if ((1 != DataItemId) || (0 != InstanceIndex)) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

        status = KeyboardToggleWaitWake (data, *(PBOOLEAN) Buffer);
        break;

    default:
        status = STATUS_WMI_GUID_NOT_FOUND;
    }

    status = WmiCompleteRequest (DeviceObject,
                                 Irp,
                                 status,
                                 size,
                                 IO_NO_INCREMENT);

    return status;
}

NTSTATUS
KeyboardClassSetWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    )
/*++

Routine Description:

    This routine is a callback into the driver to set the contents of
    a data block. When the driver has finished filling the data block it
    must call ClassWmiCompleteRequest to complete the irp. The driver can
    return STATUS_PENDING if the irp cannot be completed immediately.

Arguments:

    DeviceObject is the device whose data block is being queried

    Irp is the Irp that makes this request

    GuidIndex is the index into the list of guids provided when the
        device registered

    InstanceIndex is the index that denotes which instance of the data block
        is being queried.

    BufferSize has the size of the data block passed

    Buffer has the new values for the data block


Return Value:

    status

--*/
{
    PDEVICE_EXTENSION data;
    NTSTATUS          status;
    ULONG             size;

    PAGED_CODE ();

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    switch(GuidIndex) {
    case WMI_CLASS_DRIVER_INFORMATION:
        status = STATUS_WMI_READ_ONLY;
        break;

    case WMI_WAIT_WAKE: {
        size = sizeof(BOOLEAN);

        if (BufferSize < size) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        } else if (0 != InstanceIndex) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

        status = KeyboardToggleWaitWake (data, * (PBOOLEAN) Buffer);
        break;
    }

    default:
        status = STATUS_WMI_GUID_NOT_FOUND;
    }

    status = WmiCompleteRequest (DeviceObject,
                                 Irp,
                                 status,
                                 size,
                                 IO_NO_INCREMENT);

    return status;
}

NTSTATUS
KeyboardClassQueryWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG InstanceCount,
    IN OUT PULONG InstanceLengthArray,
    IN ULONG OutBufferSize,
    OUT PUCHAR Buffer
    )
/*++

Routine Description:

    This routine is a callback into the driver to query for the contents of
    a data block. When the driver has finished filling the data block it
    must call ClassWmiCompleteRequest to complete the irp. The driver can
    return STATUS_PENDING if the irp cannot be completed immediately.

Arguments:

    DeviceObject is the device whose data block is being queried

    Irp is the Irp that makes this request

    GuidIndex is the index into the list of guids provided when the
        device registered

    InstanceIndex is the index that denotes which instance of the data block
        is being queried.

    InstanceCount is the number of instnaces expected to be returned for
        the data block.

    InstanceLengthArray is a pointer to an array of ULONG that returns the
        lengths of each instance of the data block. If this is NULL then
        there was not enough space in the output buffer to fufill the request
        so the irp should be completed with the buffer needed.

    BufferAvail on has the maximum size available to write the data
        block.

    Buffer on return is filled with the returned data block


Return Value:

    status

--*/
{
    PDEVICE_EXTENSION   data;
    NTSTATUS    status;
    ULONG       size;
    PMSKeyboard_ClassInformation classInformation;

    PAGED_CODE ();

    data = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    switch (GuidIndex) {
    case WMI_CLASS_DRIVER_INFORMATION:
        //
        // Only registers 1 instance for this guid
        //
        if ((0 != InstanceIndex) || (1 != InstanceCount)) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }
        size = sizeof (MSKeyboard_ClassInformation);

        if (OutBufferSize < size) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        classInformation = (PMSKeyboard_ClassInformation)Buffer;
        classInformation->DeviceId = (ULONGLONG) DeviceObject;
        *InstanceLengthArray = size;
        status = STATUS_SUCCESS;

        break;

    case WMI_WAIT_WAKE:
        //
        // Only registers 1 instance for this guid
        //
        if ((0 != InstanceIndex) || (1 != InstanceCount)) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }
        size = sizeof(BOOLEAN);

        if (OutBufferSize < size) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        *(PBOOLEAN) Buffer = (WAITWAKE_ON (data) ? TRUE : FALSE );
        *InstanceLengthArray = size;
        status = STATUS_SUCCESS;
        break;

    default:
        status = STATUS_WMI_GUID_NOT_FOUND;
    }

    status = WmiCompleteRequest (DeviceObject,
                                 Irp,
                                 status,
                                 size,
                                 IO_NO_INCREMENT);

    return status;
}

NTSTATUS
KeyboardClassQueryWmiRegInfo(
    IN PDEVICE_OBJECT DeviceObject,
    OUT ULONG *RegFlags,
    OUT PUNICODE_STRING InstanceName,
    OUT PUNICODE_STRING *RegistryPath,
    OUT PUNICODE_STRING MofResourceName,
    OUT PDEVICE_OBJECT  *Pdo
    )
/*++

Routine Description:

    This routine is a callback into the driver to retrieve information about
    the guids being registered.

    Implementations of this routine may be in paged memory

Arguments:

    DeviceObject is the device whose registration information is needed

    *RegFlags returns with a set of flags that describe all of the guids being
        registered for this device. If the device wants enable and disable
        collection callbacks before receiving queries for the registered
        guids then it should return the WMIREG_FLAG_EXPENSIVE flag. Also the
        returned flags may specify WMIREG_FLAG_INSTANCE_PDO in which case
        the instance name is determined from the PDO associated with the
        device object. Note that the PDO must have an associated devnode. If
        WMIREG_FLAG_INSTANCE_PDO is not set then Name must return a unique
        name for the device. These flags are ORed into the flags specified
        by the GUIDREGINFO for each guid.

    InstanceName returns with the instance name for the guids if
        WMIREG_FLAG_INSTANCE_PDO is not set in the returned *RegFlags. The
        caller will call ExFreePool with the buffer returned.

    *RegistryPath returns with the registry path of the driver. This is
        required

    *MofResourceName returns with the name of the MOF resource attached to
        the binary file. If the driver does not have a mof resource attached
        then this can be returned as NULL.

    *Pdo returns with the device object for the PDO associated with this
        device if the WMIREG_FLAG_INSTANCE_PDO flag is retured in
        *RegFlags.

Return Value:

    status

--*/
{
    PDEVICE_EXTENSION deviceExtension;

    PAGED_CODE ();

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    *RegFlags = WMIREG_FLAG_INSTANCE_PDO;
	/***********************************************************************
	KUstring::UnicodeString (same as line 407)
	***********************************************************************/
	*RegistryPath = &Globals.MyRegistryPath.UnicodeString();
    *Pdo = deviceExtension->PDO;
    return STATUS_SUCCESS;
}


