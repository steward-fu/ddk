// Copyright (c) 1994 Compuware Corporation
//
// vregkey.h - include file for class VRegistryKey
//

#ifdef WIN40

#ifndef __vregkey_h_
#define __vregkey_h_

typedef enum PredefinedRegistryKey {
	PRK_CLASSES_ROOT = HKEY_CLASSES_ROOT,
	PRK_CURRENT_USER = HKEY_CURRENT_USER,
	PRK_LOCAL_MACHINE = HKEY_LOCAL_MACHINE,
	PRK_USERS = HKEY_USERS
} PREDEFINEDKEY;

typedef enum RegistryValueType {
	RVT_BINARY = REG_BINARY,
	RVT_STRING = REG_SZ
} REGISTRYVALUETYPE;

class VRegistryKey
{
public:
	VRegistryKey(PREDEFINEDKEY prk, PCHAR pSubkeyName,
		BOOL bCreate=FALSE);
	VRegistryKey(VRegistryKey& vrk, PCHAR pSubkeyName, BOOL bCreate=FALSE);

	~VRegistryKey();

	BOOL reconstruct(PCHAR pSubkeyName, BOOL bCreate=FALSE);

	BOOL firstSubkey(PCHAR pSubkeyName, DWORD maxChars);
	BOOL nextSubkey(PCHAR pSubkeyName, DWORD maxChars);

	BOOL firstValue(PCHAR pValueName, DWORD* pcbValueName,  
	      REGISTRYVALUETYPE* pType,PVOID pValueData, DWORD* pcbValueData);
	BOOL nextValue(PCHAR pValueName, DWORD* pcbValueName,  
	      REGISTRYVALUETYPE* pType,PVOID pValueData, DWORD* pcbValueData);

	BOOL setValue(PCHAR pValueString);
	BOOL setSubkeyValue(PCHAR pSubkeyName, PCHAR pValueString);
	BOOL setValue(PCHAR pValueName, PVOID pValueData, DWORD cbData,
		REGISTRYVALUETYPE rvType);

	BOOL getValue(PCHAR pValueString, DWORD* maxChars);
	BOOL getValue(PCHAR pValueName, PVOID pValueData, DWORD* pcbData,
		REGISTRYVALUETYPE* pValueType);
	BOOL getSubkeyValue(PCHAR pSubkeyName, PCHAR pValueString,DWORD* pcbValue);

	BOOL removeSubkey(PCHAR pSubkeyName);
	BOOL removeValue(PCHAR pValueName);

	BOOL flush();

	DWORD lastError() {return m_lastError;};

	char* operator=(char* psz);

protected:
	HKEY m_handle;
	DWORD m_enumSubkeyIndex;
	DWORD m_enumValueIndex;
	DWORD m_lastError;

};

#endif
#endif

