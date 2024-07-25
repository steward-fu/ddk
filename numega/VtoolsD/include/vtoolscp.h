// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vtoolscp.h - include file for VtoolsD C++ class library

#ifndef __vtoolscp_h_
#define __vtoolscp_h_

#ifdef ALT_INIT_SEG
// Force initializers to ICRTTEXT section
#pragma warning(disable:4075)
#pragma init_seg("ICRTTEXT")
#pragma warning(default:4075)
#endif

extern "C" {
#include <vtoolsd.h>
}
#ifndef __BORLANDC__
#include LOCKED_CODE_SEGMENT
#include LOCKED_DATA_SEGMENT
#else
#include <bcdefseg.h>
#endif

#include <vdispatc.h>
#include <vmachine.h>    
#include <vcallbak.h>    
#include <vd2wpipe.h>    
#include <vdebug.h>      
#include <vdevapi.h>     
#include <vdevice.h>     
#include <vdma.h>        
#include <vdpmient.h>    
#include <vevent.h>      
#include <vfault.h>      
#include <vhotkey.h>     
#include <vhwint.h>      
#include <vshwint.h>
#include <vintrs.h>      
#include <vipf.h>        
#include <vlist.h>       
#include <vmemory.h>     
#include <vpipe.h>       
#include <vport.h>       
#include <vsemapho.h>    
#include <pipevent.h>    
#include <vmutex.h>      
#ifdef WFW311_OR_LATER 
#include <vcomport.h>
#endif
#include <vtimeout.h>
#ifdef WIN40
#include <vthread.h>     
#include <vregkey.h>     
#include <vappy.h>
#include <vid.h>
#endif


typedef int (__cdecl * _PNH)( size_t );

/* function prototypes */

_PNH __cdecl _set_new_handler( _PNH );

#ifdef __BORLANDC__
inline char* __cdecl strcat(char* d, const char* s)
{
	return strcpy(d+strlen(d), s);
}
#endif

#endif
