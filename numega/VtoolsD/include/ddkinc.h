//		ddkinc.h
//
//	Include standard ddk header files
//
//	Copyright (c) 1994 by Compuware Corporation
//

#ifndef __ddkinc_h_
#define __ddkinc_h_


#include	<vmm.h>
#include	<blockdev.h>
#include	<debug.h>
#include	<pagefile.h>
#include	<shell.h>
#include	<v86mmgr.h>
#include	<vcd.h>
#include	<power.h>
#ifdef WIN31
#include	<vdd.h>
#endif
#include	<vddgrb.h>
#include	<vdmad.h>
#include	<vkd.h>
#include	<vmd.h>
#include	<vmpoll.h>
#include	<vpicd.h>
#include	<vtd.h>
#include	<vsd.h>
#include	<winerror.h>

#ifdef WFW311
#include	<vcomm.h>
#include	<vxdldr.h>
#endif

#ifdef WIN40
#include	<vcomm.h>
#include	<vxdldr.h>
#include	<vpowerd.h>
#include	<configmg.h>
#include	<ifs.h>
#include	<pccard.h>
#include	<drp.h>
#include	<irs.h>
#include	<ior.h>
#include	<ilb.h>
#include	<isp.h>
#include	<aep.h>
#include	<iop.h>
#include	<dcb.h>
#include	<vwin32.h>
#include	<vcache.h>
#include	<perf.h>
#include	<ios.h>
#include	<vrp.h>
#include	<sgd.h>
#include	<regstr.h>
#include	<vfbackup.h>
#include	<winioctl.h>
#include	<vmcpd.h>
#endif

#endif		// __ddkinc_h_
