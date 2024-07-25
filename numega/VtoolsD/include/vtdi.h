/*
 *
 *   (C) Copyright MICROSOFT Corp., 1989-1992
 *   (C) Copyright Compuware Corporation, 1994
 */

#ifndef __vtdi_h_
#define __vtdi_h_

#ifndef VTDI_Device_ID
#define VTDI_Device_ID   0x0488
#endif

#ifndef VTDI_DEVICE_ID
#define VTDI_DEVICE_ID   VTDI_Device_ID
#endif

#define	VTDI_Service	Declare_Service

Begin_Service_Table( VTDI )
	VTDI_Service(VTDI_Get_Version)
	VTDI_Service(VTDI_Start_Timer)
	VTDI_Service(VTDI_Stop_Timer)
	VTDI_Service(VTDI_Schedule_Event)
	VTDI_Service(VTDI_Cancel_Event)
	VTDI_Service(VTDI_Block)
	VTDI_Service(VTDI_Signal)
	VTDI_Service(VTDI_Register)
	VTDI_Service(VTDI_Get_Info)

#ifdef WIN40
	VTDI_Service(VTDI_Unload)
	VTDI_Service(VTDI_Initialize)
	VTDI_Service(VTDI_Register_UnloadProc)
	VTDI_Service(VTDI_Register_LoadProc)
#endif

End_Service_Table(VTDI)

#endif		// __vtdi_h_
