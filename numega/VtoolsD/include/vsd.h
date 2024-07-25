/*
 *
 *   (C) Copyright MICROSOFT Corp., 1988-1990
 *   (C) Copyright Compuware Corporation, 1994
 *
 */

#ifndef __vsd_h_
#define __vsd_h_

#define	VSD_Service	Declare_Service

Begin_Service_Table (VSD)

VSD_Service(VSD_Get_Version)
VSD_Service(VSD_Bell)
#ifdef WIN40
VSD_Service(VSD_SoundOn)
VSD_Service(VSD_TakeSoundPort)
#endif

End_Service_Table (VSD)

#endif			//  __vsd_h_
