// TailLightInst.h : Class declaration TailLightInstaller.
//


#ifndef __TailLightInstaller_h__
#define __TailLightInstaller_h__


// Function prototypes
DWORD AddService(LPCTSTR pszFilterName, LPCTSTR pszServiceBinary);
DWORD RemoveService(LPCTSTR pszFilterName);


//////////////////////////////////////////////////////////////////////////////
// class TailLightInstaller
//
// This class derives from CDeviceFilterInstaller which does most of the work
// of installing the filter driver.  Provide the Filter name, Device Class
// GUID, and flags to the base class.  Implement doLower and/or
// doUpper to provide a "policy".  Specifically, determine if the given
// device should be filtered as upper and/or lower filter.  To customize its
// behavior further, override virtual methods.
//
// The class will retrieve a "set" of devices based on the GUID and flags
// specified.  Then, it will iterate over all the device instances, calling
// doLower and doUpper for the derived class to choose whether or not to
// filter the specified device.
//
// By default, the base class will attempt to restart the filtered devices
// in order to have the filter driver loaded.  This behavior can be modified
// by overriding method RestartDevice().
//
// By default, the base class will prepend the filter name to the
// "UpperFilters"/"LowerFilters" registry property.  This determines the
// load order of the filter driver.  To alter this behavior, simply override
// AddFilterName() and implement a custom strategy.
//
// This implementation uses "Device Description" as the criteria for
// comparing devices to install.  Choose an alternate criteria for
// comparison in the doLower/doUpper methods.  Also, only one device is
// filterd.  Choose to filter more than one device in the
// doLower/doUpper methods.
//
class TailLightInstaller : public CDeviceFilterInstaller
{
public:
	TailLightInstaller();

	virtual ~TailLightInstaller(){}

// Helper methods
	// update Service Control Manager (SCM) database
	DWORD AddService() { return ::AddService(m_szFilterName,m_szFilterBinary); }
	DWORD RemoveService() { return ::RemoveService(m_szFilterName); }

protected:

// pure virtual method implementations required by base class
	// the job is to determine if the device represented by pDevInfo should be filtered
	// return TRUE to filter the given device, FALSE to not filter it
	virtual BOOL doLower(CDeviceInfo* pDevInfo);
	virtual BOOL doUpper(CDeviceInfo* pDevInfo) { return FALSE; }

	// virtual override to guard against installing the filter more than once
	virtual DWORD AddLowerFilter(CDeviceInfo* pDevInfo);

// Data members
public:
	// Filename for the driver binary - includes partial path
	static LPCTSTR	m_szFilterBinary;

protected:
	// Filter Name = must match the service registry key for the filter driver
	static LPCTSTR	m_szFilterName;
	// Filename for the driver binary
	static LPCTSTR	m_szFilterSys;
	// Device Description of the device to be filtered on Windows 2000
	static LPCTSTR	m_szDeviceDesc2k;
	// Device Description of the device to be filtered on Windows XP
	static LPCTSTR	m_szDeviceDescXp;
	// Device Description of the device to be filtered
	LPCTSTR			m_szDeviceDesc;
	// Class GUID to use in selecting a set of devices that are potentially filtered
	static GUID		m_Guid;
	// Flags to help in selecting the set of devices
	static DWORD	m_Flags;
};


#endif  //__TailLightInstaller_h__