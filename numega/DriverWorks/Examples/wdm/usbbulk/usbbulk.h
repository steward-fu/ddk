// UsbBulk.h - include file for sample Usb Bulk Device
//
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


class UsbBulkDriver : public KDriver
{
    SAFE_DESTRUCTORS
public:
    virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
    virtual NTSTATUS AddDevice(PDEVICE_OBJECT Pdo);

	int m_Unit;
};

