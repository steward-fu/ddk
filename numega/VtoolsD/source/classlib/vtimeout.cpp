// vtimeout.h  - implementation file for class VTimeOut
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
//

#include <vtoolscp.h>

// VGlobalTimeOut::Set - request global time out callback from VMM

BOOL VGlobalTimeOut::Set() 
{
	return !((m_handle=Set_Global_Time_Out(m_mSec, NULL, 
			createTimeOutThunk(this, m_thunk, &VGlobalTimeOut::handler), 
			NULL)) == NULL);
}

// VVMTimeOut::Set - request VM time out callback from VMM

BOOL VVMTimeOut::Set()
{
	return !((m_handle=Set_VM_Time_Out(m_mSec, m_hVM, NULL, 
			createTimeOutThunk(this, m_thunk, &VVMTimeOut::handler), 
			NULL)) == NULL);
}

#ifdef WIN40

// VThreadTimeOut::Set - request thread time out callback from VMM


BOOL VThreadTimeOut::Set() 
{
	return !((m_handle=Set_Thread_Time_Out(m_mSec, m_hThread, NULL, 
			(PTHREADTIMEOUT_HANDLER) createTimeOutThunk(this, m_thunk, &VThreadTimeOut::handler), 
			NULL)) == NULL);
}

// VAsyncTimeOut::Set - request async time out callback from VMM

BOOL VAsyncTimeOut::Set()
{
	return !((m_handle=Set_Async_Time_Out(m_mSec, NULL, 
			(PASYNCTIMEOUT_HANDLER)createTimeOutThunk(this, m_thunk, &VAsyncTimeOut::handler), 
			NULL)) == NULL);
}
#endif
