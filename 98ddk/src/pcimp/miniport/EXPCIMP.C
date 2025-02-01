/*
 *  PCIMP.C - PCI Miniport functions.
 *
 *  Notes:
 *
 */

#include "pch.h"

/****************************************************************************
 *
 *	DriverEntry - Driver Entry miniport function
 *
 *	Exported.
 *
 *	ENTRY:	DriverObject is the NT object (NULL for our hack).
 *
 *		RegistryPath is the NT registry path (NULL for our hack).
 *
 *	EXIT:	Return the success status.
 *
 ***************************************************************************/
NTSTATUS
DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	return(STATUS_SUCCESS);
}

