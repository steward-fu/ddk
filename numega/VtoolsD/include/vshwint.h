// vshwint.h - Include file for class VSharedHardwareInt
// Copyright (c) 1996, Compuware Corporation

#define SHWINTTHUNKSIZE 0x58

class VSharedHardwareInt : public VHardwareInt
{
public:
	VSharedHardwareInt(int irq, DWORD flags, DWORD timeout, PVOID refdata);
	virtual BOOL hook();	
	virtual BOOL OnSharedHardwareInt(VMHANDLE);
protected:
	BYTE m_sthunk[SHWINTTHUNKSIZE];
private:
	VSharedHardwareInt(void);
};
