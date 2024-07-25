// kiocparm.h - include file class KIoctlParameters
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

#ifndef __KIOCPARAM__
#define __KIOCPARAM__

#if defined VXD_COMPATLIB
typedef enum {
	CURRENT=0
} EStackLocation;
#define IO_NO_INCREMENT 0
#endif

class KIoctlParameters
{
public:
#if !defined VXD_COMPATLIB
	KIoctlParameters(KIrp I);

	KIrp		m_Irp;

#else
	KIoctlParameters(PIOCTLPARAMS p);

	PIOCTLPARAMS	m_pIoctl;
	NTSTATUS		m_Status;

#endif

	BOOLEAN		IsNull(void);

	NTSTATUS	Complete(NTSTATUS status,CCHAR Boost=IO_NO_INCREMENT);
	NTSTATUS& 	Status(void);
	ULONG_PTR&	Information(void);
	PVOID& 		UserBuffer(void);

	ULONG&		IoctlCode(EStackLocation s=CURRENT);
	ULONG& 		IoctlOutputBufferSize(EStackLocation s=CURRENT);
	ULONG& 		IoctlInputBufferSize(EStackLocation s=CURRENT);
	PVOID&		IoctlType3InputBuffer(EStackLocation s=CURRENT);
};


#if !defined VXD_COMPATLIB
inline KIoctlParameters::KIoctlParameters(KIrp I) {m_Irp = I;}
#else
inline KIoctlParameters::KIoctlParameters(PIOCTLPARAMS p) {m_pIoctl = p;}
#endif

inline BOOLEAN KIoctlParameters::IsNull(void)
{
#if !defined VXD_COMPATLIB
	return m_Irp.IsNull();
#else
	return (m_pIoctl == NULL);
#endif
}

inline NTSTATUS	KIoctlParameters::Complete(NTSTATUS status,CCHAR Boost)
{
#if !defined VXD_COMPATLIB
	return m_Irp.Complete(status, Boost);
#else
	m_Status = status;
	return status;
#endif
}

inline NTSTATUS& KIoctlParameters::Status(void)
{
#if !defined VXD_COMPATLIB
	return m_Irp.Status();
#else
	return m_Status;
#endif
}

inline ULONG_PTR& KIoctlParameters::Information(void)
{
#if !defined VXD_COMPATLIB
	return m_Irp.Information();
#else
	return *m_pIoctl->dioc_bytesret;
#endif
}

inline PVOID& KIoctlParameters::UserBuffer(void)
{
#if !defined VXD_COMPATLIB
	return m_Irp.UserBuffer();
#else
	return m_pIoctl->dioc_OutBuf;
#endif
}

inline ULONG& KIoctlParameters::IoctlCode(EStackLocation s)
{
#if !defined VXD_COMPATLIB
	return m_Irp.IoctlCode(s);
#else
	return m_pIoctl->dioc_IOCtlCode;
#endif
}

inline ULONG& KIoctlParameters::IoctlOutputBufferSize(EStackLocation s)
{
#if !defined VXD_COMPATLIB
	return m_Irp.IoctlOutputBufferSize(s);
#else
	return m_pIoctl->dioc_cbOutBuf;
#endif
}

inline ULONG& KIoctlParameters::IoctlInputBufferSize(EStackLocation s)
{
#if !defined VXD_COMPATLIB
	return m_Irp.IoctlInputBufferSize(s);
#else
	return m_pIoctl->dioc_cbInBuf;
#endif
}

inline PVOID& KIoctlParameters::IoctlType3InputBuffer(EStackLocation s)
{
#if !defined VXD_COMPATLIB
	return m_Irp.IoctlType3InputBuffer(s);
#else
	return m_pIoctl->dioc_InBuf;
#endif
}

#endif // __KIOCPARAM__
