//////////////////////////////////////////////////////////////////////
// StlFilterDrv.h 
//

#ifndef _StlFilterDrv_h	
#define _StlFilterDrv_h

#include	<kndis.h>



//////////////////////////////////////////////////////////////////////
// StlFilterDriver	Declaration
class StlFilterDriver : public KNdisIMiniDriver
{
protected:
	// must implement the entry point
	virtual NTSTATUS DriverEntry(IN PVOID RegistryPath);

};

#endif