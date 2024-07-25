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
// RFSDDBG.C - Debug utilities for Registry File System
// Copyright (c) 1996, Compuware Corporation

#include  "regfsd.h"
#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

#ifdef DEBUG
const char* GetName(NAMETABLE nameTable[], int n, ULONG func)
{

	int i;

	for (i = 0; i < n; i++)
		if (func == nameTable[i]._func)
			return nameTable[i]._name;

	return "<unknown>";
}

const CHAR* GetAEFunctionName(ULONG func)
{
	static NAMETABLE nameTable[] = {
		{AEP_INITIALIZE, "AEP_INITIALIZE"},
		{AEP_SYSTEM_CRIT_SHUTDOWN, "AEP_SYSTEM_CRIT_SHUTDOWN"},
		{AEP_BOOT_COMPLETE, "AEP_BOOT_COMPLETE"},
		{AEP_CONFIG_DCB, "AEP_CONFIG_DCB"},
		{AEP_UNCONFIG_DCB, "AEP_UNCONFIG_DCB"},
		{AEP_IOP_TIMEOUT, "AEP_IOP_TIMEOUT"},
		{AEP_DEVICE_INQUIRY, "AEP_DEVICE_INQUIRY"},
		{AEP_HALF_SEC, "AEP_HALF_SEC"},
		{AEP_1_SEC, "AEP_1_SEC"},
		{AEP_2_SECS, "AEP_2_SECS"},
		{AEP_4_SECS, "AEP_4_SECS"},
		{AEP_DBG_DOT_CMD, "AEP_DBG_DOT_CMD"},
		{AEP_ASSOCIATE_DCB, "AEP_ASSOCIATE_DCB"},
		{AEP_REAL_MODE_HANDOFF, "AEP_REAL_MODE_HANDOFF"},
		{AEP_SYSTEM_SHUTDOWN, "AEP_SYSTEM_SHUTDOWN"},
		{AEP_UNINITIALIZE, "AEP_UNINITIALIZE"},
		{AEP_DCB_LOCK, "AEP_DCB_LOCK"},
		{AEP_MOUNT_NOTIFY, "AEP_MOUNT_NOTIFY"},
		{AEP_CREATE_VRP, "AEP_CREATE_VRP"},
		{AEP_DESTROY_VRP, "AEP_DESTROY_VRP"},
		{AEP_REFRESH_DRIVE, "AEP_REFRESH_DRIVE"},
		{AEP_PEND_UNCONFIG_DCB, "AEP_PEND_UNCONFIG_DCB"},
		{AEP_1E_VEC_UPDATE, "AEP_1E_VEC_UPDATE"},
		{AEP_CHANGE_RPM, "AEP_CHANGE_RPM"}
	};

#define NAEFUNCTIONS (sizeof(nameTable)/sizeof(NAMETABLE))

		return GetName(nameTable, NAEFUNCTIONS, func);
}

const CHAR* GetIOFunctionName(ULONG func)
{
	
	static NAMETABLE nameTable[] = {
		{IOR_READ, "IOR_READ"},
		{IOR_WRITE, "IOR_WRITE"},
		{IOR_VERIFY, "IOR_VERIFY"},
		{IOR_CANCEL, "IOR_CANCEL"},
		{IOR_WRITEV, "IOR_WRITEV"},
		{IOR_MEDIA_CHECK, "IOR_MEDIA_CHECK"},
		{IOR_MEDIA_CHECK_RESET, "IOR_MEDIA_CHECK_RESET"},
		{IOR_LOAD_MEDIA, "{IOR_IOR_LOAD_MEDIA"},
		{IOR_EJECT_MEDIA, "IOR_EJECT_MEDIA"},
		{IOR_LOCK_MEDIA, "IOR_LOCK_MEDIA"},
		{IOR_UNLOCK_MEDIA, "IOR_UNLOCK_MEDIA"},
		{IOR_REQUEST_SENSE, "IOR_REQUEST_SENSE"},
		{IOR_COMPUTE_GEOM, "IOR_COMPUTE_GEOM"},
		{IOR_GEN_IOCTL, "IOR_GEN_IOCTL"},
		{IOR_FORMAT, "IOR_FORMAT"},
		{IOR_SCSI_PASS_THROUGH, "IOR_SCSI_PASS_THROUGH"},
		{IOR_CLEAR_QUEUE, "IOR_CLEAR_QUEUE"},
		{IOR_DOS_RESET, "IOR_DOS_RESET"},
		{IOR_SCSI_REQUEST, "IOR_SCSI_REQUEST"},
		{IOR_SET_WRITE_STATUS, "IOR_SET_WRITE_STATUS"},
		{IOR_RESTART_QUEUE, "IOR_RESTART_QUEUE"},
		{IOR_ABORT_QUEUE, "IOR_ABORT_QUEUE"},
		{IOR_SPIN_DOWN, "IOR_SPIN_DOWN"},
		{IOR_SPIN_UP, "IOR_SPIN_UP"},
		{IOR_FLUSH_DRIVE, "IOR_FLUSH_DRIVE"},
		{IOR_FLUSH_DRIVE_AND_DISCARD, "IOR_FLUSH_DRIVE_AND_DISCARD"},
		{IOR_FSD_EXT, "IOR_FSD_EXT"}
	};

#define NIOCOMMANDS (sizeof(nameTable)/sizeof(NAMETABLE))

	return GetName(nameTable, NIOCOMMANDS, func);
}

#endif


