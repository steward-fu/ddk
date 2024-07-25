// BMDmaTst.h
//

#ifndef __BMDmaTst_h__
#define __BMDmaTst_h__
   
#define EOL "\n"

class BMDmaTst : public KDriver
{
	SAFE_DESTRUCTORS
public:
	// Member functions
	virtual  VOID Unload(VOID);
	virtual  NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);
	void	 LoadRegistryParameters(KRegistryKey &Params);

	// Member data
	// The following data members are loaded from the registry during DriverEntry
	ULONG m_bBreakOnEntry;
	

#if 0
    The following member functions are actually defined by macros.
    These comment-only definitions are for easier Studio navigation.
   ;
#endif
};



#endif			// __BMDmaTst_h__
