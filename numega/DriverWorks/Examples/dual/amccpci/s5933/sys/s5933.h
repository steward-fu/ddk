// S5933.h
//

#ifndef __S5933_h__
#define __S5933_h__
   
#define EOL "\n"

class S5933Device; // Forward

class S5933 : public KDriver
{
	SAFE_DESTRUCTORS
public:
	// Member functions

# if !_WDM_
	virtual  VOID Unload(VOID);
#endif // _WDM_

	virtual  NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
	void	 LoadRegistryParameters(KRegistryKey &Params);

#if  _WDM_
	virtual NTSTATUS AddDevice(PDEVICE_OBJECT Pdo);
#endif // _WDM_

	// Member data
	// The following data members are loaded from the registry during DriverEntry
	ULONG m_bBreakOnEntry;

#if  _WDM_
	int	m_Unit;
#else // _WDM_
	S5933Device* m_pS5933Device;	
#endif // _WDM_

};



#endif			// __S5933_h__
