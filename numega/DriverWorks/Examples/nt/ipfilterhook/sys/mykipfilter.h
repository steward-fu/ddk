// myKIPFilter.h - definition of class
//////////////////////////////////////////////////////////////////////////////

#ifndef	__MYKIPFILTER_H__
#define	__MYKIPFILTER_H__

#include <vdw.h>
#include <KIPFilterHook.h>

//////////////////////////////////////////////////////////////////////////////
//
//	Class myKIPFilter illustrates the usage of the KIPFilterHook class.
//
//	The member XxxHandler() is mandatory and will be called by th Framework
//	when ever a dispatch to the filter hook occurs by the IP Filter Driver.
//
//////////////////////////////////////////////////////////////////////////////
class myKIPFilter : public KIPFilterHook
{

public:
	
	// Construct object
	myKIPFilter()
	{};

	// Destruct object
	~myKIPFilter()
	{};

	// Method to be called by framework 
	PF_FORWARD_ACTION SendHandler (
						IPHeader   *PacketHeader,
						PUCHAR	Packet,
						ULONG   PacketLength,
						ULONG   SendInterfaceIndex,
						IPAddr	SendLinkNextHop
						);

	PF_FORWARD_ACTION ReceiveHandler (
						IPHeader   *PacketHeader,
						PUCHAR	Packet,
						ULONG   PacketLength,
						ULONG   RecvInterfaceIndex,
						IPAddr	RecvLinkNextHop
						);

protected:
private:

};

#endif	__MYKIPFILTER_H__


