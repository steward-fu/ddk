// S5933Isa.h
//

#ifndef __S5933Isa_h__
#define __S5933Isa_h__
   
#define EOL "\n"

class S5933Isa : public KDriver
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
	

#if _COMMENT_ONLY
    The following member functions are actually defined by macros.
    These comment-only definitions are for easier Studio navigation.
   ;
#endif
};



#endif			// __S5933Isa_h__
