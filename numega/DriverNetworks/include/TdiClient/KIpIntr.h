// KIpIntr.h
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
//=============================================================================
// KEnumIPInterface
//
// Enumerates available IP interafaces.
// Allows the user to query all local IP addresses.

class KEnumIPInterface : public KTdiClient
{
public:

	KEnumIPInterface();

	~KEnumIPInterface()	{InvalidateIPTable();}

	//Get the number of IP Interfaces currently stored
	ULONG Count(){return m_AddressCount;}

	//Retrieve an IP Address in net order from the 0-based index specified
	ULONG operator[](ULONG index){return GetAddress(index);}
	ULONG GetAddress(ULONG index=0);

	//Retrieve the subnet mask in net order from the 0-based index specified
	ULONG GetSubnetMask(ULONG index=0);

	//Query TCP for changes in the IP interfaces
	TDI_STATUS Refresh();

protected:
	//Some internal routines
	VOID InvalidateIPTable();
	TDI_STATUS AddIPAddress(IPAddrEntry* NewAddress, ULONG NewAddressCount = 1);

	IPAddrEntry *m_AddressArray;
	ULONG        m_AddressCount;
};

//////////////////////////////////////////////////////////////////////////
inline VOID KEnumIPInterface::InvalidateIPTable()
{
	if(m_AddressCount != 0)
		delete [] m_AddressArray;
	m_AddressArray = NULL;
	m_AddressCount = 0;
}

/////////////////////////////////////////////////////////////////////////////
inline ULONG KEnumIPInterface::GetAddress(ULONG index)
{
	return (index < Count()) ? m_AddressArray[index].iae_addr : 0;
}
/////////////////////////////////////////////////////////////////////////////
inline ULONG KEnumIPInterface::GetSubnetMask(ULONG index)
{
	if(index < Count())
		return m_AddressArray[index].iae_mask;
	else
		return 0;
}
