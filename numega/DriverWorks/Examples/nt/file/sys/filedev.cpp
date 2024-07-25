// filedev.cpp - implementation of device class for KFile example
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

#include <vdw.h>
#include <filedev.h>
#include <common.h>

// Class FileTestDevice demonstrates usage of class KFile. Member function
// DeviceControl provides services that allow an application to open, read,
// and close a file using DeviceIoControl calls.

//////////////////////////////////////////////////////////////////////////
// Begin INIT time  code
//
#pragma code_seg("INIT")
//////////////////////////////////////////////////////////////////////////
//
//
FileTestDevice::FileTestDevice() :
	KDevice(L"FileTest", FILE_DEVICE_UNKNOWN, L"FileTest")
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
#if DBG
		DbgPrint("FileTest: Failed to create device: %x\n", m_ConstructorStatus);
#endif
		return;
	}
}
#pragma code_seg()
//
// End INIT time  code
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CreateClose - enable the device to be opened and closed by an app. 
//
//
// This demonstration assumes that there is one instance of the test app,
// and that the test app is well behaved, i.e. it will not read the file
// until it has been opened, and will always close the file before exiting.
// In a more rigorous implementation, member function CreateClose would 
// check the status of data member m_file.
//
NTSTATUS FileTestDevice::CreateClose(KIrp I)
{
	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
// DeviceControl
//
// This is the application interface for manipulating the KFile object that
// is embedded in the device object.
// 
NTSTATUS FileTestDevice::DeviceControl(KIrp I)
{
	FILE_STANDARD_INFORMATION StandardInfo;
	NTSTATUS status;

	switch (I.IoctlCode())
	{
	case FILETEST_OPEN:
	{

		// The name of the file to open is passed in the input buffer
		KUstring name( (PCWSTR)I.IoctlBuffer() );

		status = m_File.OpenCreate(
				name,
				NULL,
				FILE_GENERIC_READ | SYNCHRONIZE,
				OBJ_CASE_INSENSITIVE,
				0,
				FILE_SHARE_READ,
				FILE_OPEN,
				FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);

		// Return the status in the output buffer
		*(NTSTATUS*)I.IoctlBuffer() = status;
		I.Information() = sizeof(NTSTATUS);				

		m_File.Query(&StandardInfo);
		DbgPrint("File allocation size is %x\n", StandardInfo.AllocationSize.LowPart);		
		DbgPrint("File size is %x\n", StandardInfo.EndOfFile.LowPart);		

		return I.Complete(STATUS_SUCCESS);
		break;
	}
	case FILETEST_READ:

		ULONG BytesRead;
		// Read from the file. 
		m_File.Read((PUCHAR)I.IoctlBuffer(), I.IoctlOutputBufferSize(), &BytesRead);
		I.Information() = (ULONG_PTR)BytesRead;
		return I.Complete(m_File.Status());

		break;
	case FILETEST_CLOSE:

		// Close the file and return the status.
		*(NTSTATUS*)I.IoctlBuffer() = m_File.Close();	

		I.Information() = sizeof(NTSTATUS);				
		return I.Complete(STATUS_SUCCESS);

		break;

	default:
		I.Information()=0;
		return I.Complete(STATUS_INVALID_PARAMETER);
	}
}
