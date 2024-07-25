// ktrace.cpp - Trace debug class implementation
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

#ifndef VXD_COMPATLIB
#include <stdarg.h>
#include <stdio.h>
#endif

#include <vdw.h>
#include <dmioctl.h>

#if !defined VXD_COMPATLIB
#define vsnprintf _vsnprintf
#endif

#pragma warning(disable:4702)

//
// addresses of entry points in DbgMsg
//
HANDLE 	(__stdcall *KTrace::m_Open)(PUNICODE_STRING, PKINTERRUPT, ULONG) = 0;
VOID	(__stdcall *KTrace::m_Post)(HANDLE h, PCHAR sz) = 0;
VOID	(__stdcall *KTrace::m_Close)(HANDLE h) = 0;

///////////////////////////////////////////////////////////////////

#define N_SCRATCH_BUFFERS 4
#define SCRATCH_BUF_SIZE  256
typedef struct
{
	char b[SCRATCH_BUF_SIZE];
} KTRACESCRATCHBUFFER;

KTRACESCRATCHBUFFER ScratchBuffers[N_SCRATCH_BUFFERS];
LONG ScratchBufferIndex = N_SCRATCH_BUFFERS;

///////////////////////////////////////////////////////////////////
// Constructors
//
KTrace::KTrace(
	PCHAR prefix,
	ULONG targetmask,
	TRACE_LEVEL tracelevel,
	BREAK_LEVEL breaklevel,
	PUNICODE_STRING ChannelName,
	PKINTERRUPT intobject
	) :
	m_TargetMask(targetmask),
	m_TraceLevel(tracelevel),
	m_BreakLevel(breaklevel),
	m_Prefix(NULL),
	m_PrefixLength(0),
	m_Channel(NULL),
	m_NeedPrefix(TRUE),
	m_StreamTraceLevel(TraceAlways),
	m_Radix(TraceHex),
	m_FreeOnDestroy(FALSE)
{
	ANSI_STRING asPrefix;
	RtlInitAnsiString(&asPrefix, prefix);

	__construct(&asPrefix, ChannelName, intobject);
}

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

KTrace::KTrace(
	PUNICODE_STRING prefix,
	ULONG targetmask,
	TRACE_LEVEL tracelevel,
	BREAK_LEVEL breaklevel,
	PUNICODE_STRING ChannelName,
	PKINTERRUPT intobject
	) :
	m_TargetMask(targetmask),
	m_TraceLevel(tracelevel),
	m_BreakLevel(breaklevel),
	m_Prefix(NULL),
	m_PrefixLength(0),
	m_Channel(NULL),
	m_NeedPrefix(TRUE),
	m_StreamTraceLevel(TraceAlways),
	m_Radix(TraceHex),
	m_FreeOnDestroy(FALSE)
{
	ANSI_STRING asPrefix;
	RtlInitAnsiString(&asPrefix, NULL);
	RtlUnicodeStringToAnsiString(&asPrefix, prefix, TRUE);

	__construct(&asPrefix, ChannelName, intobject);

	RtlFreeAnsiString(&asPrefix);
}

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
/*	Note: Needs reference counting for safe use
KTrace::KTrace(KTrace& src) :
	m_TargetMask(src.m_TargetMask),
	m_TraceLevel(src.m_TraceLevel),
	m_BreakLevel(src.m_BreakLevel),
	m_Prefix(src.m_Prefix),
	m_PrefixLength(src.m_PrefixLength),
	m_Channel(src.m_Channel),
	m_NeedPrefix(src.m_NeedPrefix),
	m_StreamTraceLevel(src.m_StreamTraceLevel),
	m_Radix(src.m_Radix),
	m_FreeOnDestroy(FALSE)
{
}
*/
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

VOID KTrace::__construct(
	PANSI_STRING prefix,
	PUNICODE_STRING ChannelName,
	PKINTERRUPT intobject
	)
{

	// If this is the first time in, resolve the entry points into DBGMSG
	if ( (m_Open == 0) && (m_TargetMask & TRACE_MONITOR))
		Connect();

	// Copy the prefix string
	if ( (prefix != NULL) && (prefix->Length != 0) && (prefix->Buffer != NULL) )
	{
		m_PrefixLength = prefix->Length;
		m_Prefix = new (NonPagedPool) char[m_PrefixLength+4];
		if (m_Prefix != NULL)
		{
			memcpy(m_Prefix, prefix->Buffer, prefix->Length);
			m_Prefix[m_PrefixLength] = ':';
			m_PrefixLength++;
			m_Prefix[m_PrefixLength] = ' ';
			m_PrefixLength++;
			m_Prefix[m_PrefixLength] = 0;
			m_FreeOnDestroy = TRUE;
		}
		else
		{
			m_PrefixLength = 0;
		}
	}

	// Open the DBGMSG channel
	if (m_Open)
	{
		if (ChannelName != NULL)
		{
	// Open a named channel
			m_Channel = m_Open(ChannelName, intobject, m_TargetMask);
			ASSERT(m_Channel != NULL);
		}
		else
		{
	// Not named channel, use default channel
			m_Channel = NULL;
	// Can't have int object without named channel
			ASSERT(intobject == NULL);
		}
	}
}


///////////////////////////////////////////////////////////////////
// Connect
//
// Establish a link to driver DBGMSG and obtain its entry points,
// which are stored in static data members.
//
BOOLEAN KTrace::Connect(void)
{
	DbgMsgEntryPoints dmep;
	ULONG_PTR nBytes;
	NTSTATUS status;

#ifndef VXD_COMPATLIB // NT/WDM code

	KLowerDevice DbgMsg(L"\\Device\\DebugMessageDevice", FILE_ALL_ACCESS);
	if ( NT_SUCCESS(DbgMsg.ConstructorStatus()) )
	{
		status = DbgMsg.DeviceIoControl(
			IOCTL_DBGMSG_GETENTRYPOINTS,
			NULL,
			0,
			&dmep,
			sizeof(dmep),
			TRUE,
			&nBytes
			);

	}
	else
	{
		return FALSE;
	}

#else // VxD code

	PDDB pDdb; 		// pointer to DDB (VxD system record)
	ALLREGS regs;	// register struct passed to Directed_Sys_Ctrl
	IOCTLPARAMS io;	// Ioctl param struct passed to target device
	ULONG scratch;

	pDdb = VMM_GetDDBList();

	CHAR Name[9] = "NDBGMSG ";

	while (pDdb)
	{
		if (memcmp(Name, pDdb->DDB_Name, 8) == 0)
		{
			break;
		}
		else
		{
			pDdb = (PDDB)pDdb->DDB_Next;
		}
	}

	if ( pDdb  == NULL )
	{
		return FALSE;
	}

	else
	{
	// set up the ioctl params
		io.dioc_IOCtlCode = IOCTL_DBGMSG_GETENTRYPOINTS;
		io.dioc_InBuf = 0;
		io.dioc_cbInBuf = 0;
		io.dioc_OutBuf = &dmep;
		io.dioc_cbOutBuf = sizeof(dmep);
		io.dioc_bytesret = &scratch;

	// put a pointer to the ioctl param struct in the register struct
		memset(&regs, 0, sizeof(regs));
		regs.RESI = (DWORD)&io;

	// send the message to the target device
		Directed_Sys_Control(pDdb, W32_DEVICEIOCONTROL, &regs);

		status = ( (regs.REAX == 0) ? STATUS_SUCCESS : -1 );
	}
#endif

// common code

	if (NT_SUCCESS(status))
	{
		m_Open = dmep.m_Open;
		m_Close = dmep.m_Close;
		m_Post = dmep.m_Post;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


///////////////////////////////////////////////////////////////////
// Trace output
//
VOID KTrace::Trace(TRACE_LEVEL Level, PCHAR fmt, ...)
{
	int outLen;

	LONG bufindex = InterlockedDecrement(&ScratchBufferIndex);

	if (bufindex < 0)
	{
		InterlockedIncrement(&ScratchBufferIndex);
		DbgPrint("KTrace: Dropping a trace message due to lack of buffer space\n");
		return;
	}

	if (((Level == TraceAlways)|| (Level >= m_TraceLevel))
		 && (Level != TraceNever))
	{
	// Send the message
		va_list ap;
		va_start(ap, fmt);
		char* buf = ScratchBuffers[bufindex].b;

	// format string to buffer
		outLen = vsnprintf(buf+m_PrefixLength, SCRATCH_BUF_SIZE-m_PrefixLength, fmt, ap);

		if(outLen == -1)
		{
			//We overran the buffer.  Write in the '\0', and set the outlen appropriately
			buf[SCRATCH_BUF_SIZE-1] = '\0';
			outLen = SCRATCH_BUF_SIZE-m_PrefixLength;
		}

	// Copy prefix string to buffer
		if (m_Prefix != NULL)
			memcpy(buf, m_Prefix, m_PrefixLength);

	// output to debugger if requested
		if (m_TargetMask & TRACE_DEBUGGER)
		{

			if(m_TargetMask & TRACE_CHECK_FORMAT)
			{
				//Make sure the string is 'safe' to print.  (No %'s)
				PCHAR StringPosition = strchr(buf, '%');
				while(StringPosition)
				{
					size_t length = strlen(StringPosition);

					//Check for buffer overflow
					size_t LengthToShift = length + 1;
					if((size_t)(StringPosition - buf) + LengthToShift > SCRATCH_BUF_SIZE)
					{
						LengthToShift = SCRATCH_BUF_SIZE - (size_t)(StringPosition - buf) - 1;
						buf[SCRATCH_BUF_SIZE-1] = '\0';
					}

					//Insert an extra %
					RtlMoveMemory(StringPosition+1, StringPosition, LengthToShift);
					StringPosition = strchr(StringPosition+2, '%');
				}
			}

			DbgPrint(buf + (m_NeedPrefix ? 0 : m_PrefixLength));
		}
	// output to monitor if requested

	// On x86/IA64/AMD64 platforms, DBGMSG will hook the output to the debugger,
	// so we shouldn't send to BOTH debugger and monitor.

	// On other platforms, debugger output is not hooked, so
	// send to monitor if requested
#if defined(i386) || defined(_IA64_) || defined(_AMD64_)
		else
#endif
		if ((m_Post != 0) && (m_TargetMask & TRACE_MONITOR))
			m_Post(m_Channel, buf + (m_NeedPrefix ? 0 : m_PrefixLength));

	// if the last char was a newline, need prefix next time
		m_NeedPrefix = (buf[m_PrefixLength+outLen-1] == '\n');
	}

	// break if requested

	if ((((BREAK_LEVEL)Level == BreakAlways) || ((BREAK_LEVEL) Level >= m_BreakLevel))
		 && (m_BreakLevel != BreakNever))
	{
		DbgBreakPoint();
	}

	InterlockedIncrement(&ScratchBufferIndex);
}

///////////////////////////////////////////////////////////////////
// Destructor
//
KTrace::~KTrace(void)
{
	if (m_Close && (m_Channel != NULL))
	{
		m_Close(m_Channel);
	}
	if (m_FreeOnDestroy && (m_Prefix != NULL))
	{
		delete m_Prefix;
	}
}


#if !defined VXD_COMPATLIB
#if DBG

static CHAR* GetIrpName(ULONG Major)
{

	static char* Names[]= {
		"IRP_MJ_CREATE",
		"IRP_MJ_CREATE_NAMED_PIPE",
		"IRP_MJ_CLOSE",
		"IRP_MJ_READ",
		"IRP_MJ_WRITE",
		"IRP_MJ_QUERY_INFORMATION",
		"IRP_MJ_SET_INFORMATION",
		"IRP_MJ_QUERY_EA",
		"IRP_MJ_SET_EA",
		"IRP_MJ_FLUSH_BUFFERS",
		"IRP_MJ_QUERY_VOLUME_INFORMATION",
		"IRP_MJ_SET_VOLUME_INFORMATION",
		"IRP_MJ_DIRECTORY_CONTROL",
		"IRP_MJ_FILE_SYSTEM_CONTROL",
		"IRP_MJ_DEVICE_CONTROL",
		"IRP_MJ_INTERNAL_DEVICE_CONTROL",
		"IRP_MJ_SHUTDOWN",
		"IRP_MJ_LOCK_CONTROL",
		"IRP_MJ_CLEANUP",
		"IRP_MJ_CREATE_MAILSLOT",
		"IRP_MJ_QUERY_SECURITY",
		"IRP_MJ_SET_SECURITY",
		"IRP_MJ_POWER",
		"IRP_MJ_SYSTEM_CONTROL",
		"IRP_MJ_DEVICE_CHANGE",
		"IRP_MJ_QUERY_QUOTA",
		"IRP_MJ_SET_QUOTA",
		"IRP_MJ_PNP"
		};


	if ( Major > IRP_MJ_MAXIMUM_FUNCTION)
	{
		return "<unknown major function>";
	}
	else
	{
		return Names[Major];
	}
}

#endif

#if _WDM_
static CHAR* PNPMinorFunctionName(ULONG mn)
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
		"IRP_MN_SURPRISE_REMOVAL"
	};

	if (mn > IRP_MN_SURPRISE_REMOVAL)
		return "<unknown minor function>";
	else
		return minors[mn];
}

static CHAR* PowerMinorFunctionName(ULONG mn)
{
	static char* minors[] = {
		"IRP_MN_WAIT_WAKE",
		"IRP_MN_POWER_SEQUENCE",
		"IRP_MN_SET_POWER",
		"IRP_MN_QUERY_POWER"
	};

	if (mn > IRP_MN_QUERY_POWER)
		return "<unknown minor function>";
	else
		return minors[mn];
}

static CHAR* SystemPowerString(SYSTEM_POWER_STATE Type)
{
    switch ( Type )
    {
        case PowerSystemUnspecified:
            return "PowerSystemUnspecified";
        case PowerSystemWorking:
            return "PowerSystemWorking";
        case PowerSystemSleeping1:
            return "PowerSystemSleeping1";
        case PowerSystemSleeping2:
            return "PowerSystemSleeping2";
        case PowerSystemSleeping3:
            return "PowerSystemSleeping3";
        case PowerSystemHibernate:
            return "PowerSystemHibernate";
        case PowerSystemShutdown:
            return "PowerSystemShutdown";
        case PowerSystemMaximum:
            return "PowerSystemMaximum";
        default:
            return "<unknown system power state>";
    }
}

static CHAR* DevicePowerString(DEVICE_POWER_STATE Type)
{
    switch ( Type )
    {
        case PowerDeviceUnspecified:
            return "PowerDeviceUnspecified";
        case PowerDeviceD0:
            return "PowerDeviceD0";
        case PowerDeviceD1:
            return "PowerDeviceD1";
        case PowerDeviceD2:
            return "PowerDeviceD2";
        case PowerDeviceD3:
            return "PowerDeviceD3";
        case PowerDeviceMaximum:
            return "PowerDeviceMaximum";
        default:
            return "<unknown device power state>";
    }
}

static CHAR* WMIMinorFunctionName(ULONG mn)
{
	static char* minors[] = {
		"IRP_MN_QUERY_ALL_DATA",
		"IRP_MN_QUERY_SINGLE_INSTANCE",
		"IRP_MN_CHANGE_SINGLE_INSTANCE",
		"IRP_MN_CHANGE_SINGLE_ITEM",
		"IRP_MN_ENABLE_EVENTS",
		"IRP_MN_DISABLE_EVENTS",
		"IRP_MN_ENABLE_COLLECTION",
		"IRP_MN_DISABLE_COLLECTION",
		"IRP_MN_REGINFO",
		"IRP_MN_EXECUTE_METHOD",
		"<unknown minor function>",
		"IRP_MN_REGINFO_EX"
	};

#if (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20)))
	if (mn > IRP_MN_REGINFO_EX)
#else
	if (mn > IRP_MN_EXECUTE_METHOD)
#endif
		return "<unknown minor function>";
	else
		return minors[mn];
}

#endif

///////////////////////////////////////////////////////////////////
// IRP Dump
//
KTrace& KTrace::operator << (const KIrp J)
{
	KIrp I = const_cast<KIrp&>(J);

	ULONG Major = I.MajorFunction();
	ULONG Minor = I.MinorFunction();

#if DBG
	Trace(m_StreamTraceLevel, "IRP: Major=%s", GetIrpName(Major));
#else
	Trace(m_StreamTraceLevel, "IRP: Major=0x%x", Major);
#endif

#if _WDM_
	if ( Major == IRP_MJ_PNP )
		Trace(m_StreamTraceLevel, " Minor=%s (0x%x) Flags=0x%x\n", PNPMinorFunctionName(Minor), Minor, I->Flags);
	else if ( Major == IRP_MJ_POWER )
	{
		Trace(m_StreamTraceLevel, " Minor=%s (0x%x) Flags=0x%x  ", PowerMinorFunctionName(Minor), Minor, I->Flags);
		if ( (Minor == IRP_MN_QUERY_POWER) || (Minor == IRP_MN_SET_POWER) )
		{
			if ( I.PowerStateType() == DevicePowerState )
				Trace(m_StreamTraceLevel, "%s\n", DevicePowerString(I.PowerStateSetting().DeviceState));
			else
				Trace(m_StreamTraceLevel, "%s\n", SystemPowerString(I.PowerStateSetting().SystemState));
		}
	}
	else if ( Major == IRP_MJ_SYSTEM_CONTROL )
		Trace(m_StreamTraceLevel, " Minor=%s (0x%x) Flags=0x%x\n", WMIMinorFunctionName(Minor), Minor, I->Flags);
	else
#endif
 		Trace(m_StreamTraceLevel,"  Minor=0x%x Flags=0x%x\n", Minor, I->Flags);

	switch (Major)
	{
	case IRP_MJ_READ:
	case IRP_MJ_WRITE:
		Trace(m_StreamTraceLevel, " Length=0x%x\n", I.ReadSize());
		break;
	case IRP_MJ_DEVICE_CONTROL:
	case IRP_MJ_INTERNAL_DEVICE_CONTROL:
		Trace(m_StreamTraceLevel, " Code=0x%x InputSize=0x%x OutputSize=0x%x\n",
			I.IoctlCode(),
			I.IoctlInputBufferSize(),
			I.IoctlOutputBufferSize()
			);
		break;
	}

	return *this;
}
#endif // Vxd_Compatlib

#pragma warning(default:4702)
