// vregkey.cpp - implementation file for class VRegistryKey
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in 
// Compuware Corporation license agreement and as provided in 
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995), 
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995), 
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.  
// Compuware Corporation.
// 
// This product contains confidential information and trade secrets 
// of Compuware Corporation. Use, disclosure, or reproduction is 
// prohibited without the prior express written permission of Compuware 
// Corporation.
//
//=============================================================================
//

#ifdef WIN40

#include <vtoolscp.h>

#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// Function
//	VRegistryKey::VRegistryKey
//
// Syntax
//	VRegistryKey::VRegistryKey(PREDEFINEDKEY pdrk, PCHAR pSubkeyName,
//		BOOL bCreate)
//
// Parameters
//	prk		Predefined registry key. One of:
//			PRK_LOCAL_MACHINE
//			PRK_CLASSES_ROOT
//			PRK_CURRENT_USER
//			PRK_USERS
//	pSubkeyName	Name of the subkey to open or create, or NULL.
//	bCreate		(optional) If TRUE, the constructor will attempt to 
//			create a new key in the Registry if the specified key 
//			does not exist. Default is FALSE.
//
// Remarks
//	Constructs an instance of class VRegistryKey.  Call member function
//	lastError to determine if the construction was successful.
//
VRegistryKey::VRegistryKey(PREDEFINEDKEY prk, PCHAR pSubkeyName,
		BOOL bCreate)
{
	HKEY hKey = (HKEY)prk;

	if (bCreate)
		m_lastError = _RegCreateKey(hKey, pSubkeyName, &m_handle);
	else
		m_lastError = _RegOpenKey(hKey, pSubkeyName, &m_handle);
}

// Function
//	VRegistryKey::VRegistryKey
//
// Syntax
//	VRegistryKey::VRegistryKey(VRegistryKey& vrk, PCHAR pSubkeyName, 
//		BOOL bCreate)
//
// Parameters
//	vrk		Previously created VRegistryKey instance
//	pSubkeyName	Name of the subkey to open or create, or NULL.
//	bCreate		(optional) If TRUE, the constructor will attempt to 
//			create a new key in the Registry if the specified key 
//			does not exist. Default is FALSE.
// Remarks
//	Constructs an instance of class VRegistryKey.  Call member function
//	lastError to determine if the construction was successful.
//
VRegistryKey::VRegistryKey(VRegistryKey& vrk, PCHAR pSubkeyName, BOOL bCreate)
{
	HKEY hKey = vrk.m_handle;

	if (bCreate)
		m_lastError = _RegCreateKey(hKey, pSubkeyName, &m_handle);
	else
		m_lastError = _RegOpenKey(hKey, pSubkeyName, &m_handle);
}

// Function
//	VRegistryKey::~VRegistryKey
//
// Syntax
//	VRegistryKey::~VRegistryKey()
//
// Remarks
//	Destroys an instance of class VRegistryKey.  Closes the key.
//
VRegistryKey::~VRegistryKey()
{
	_RegCloseKey(m_handle);
}

// Function
//	VRegistryKey::reconstruct
//
// Syntax
//	BOOL VRegistryKey::reconstruct(PCHAR pSubkeyName, BOOL bCreate)
//
// Parameters
//	pSubkeyName	name of subkey to open or create
//	bCreate		(optional) If TRUE, the constructor will attempt to 
//			create a new key in the Registry if the specified key 
//			does not exist. Default is FALSE.
// Remarks
//	This member function "moves" the VRegistryKey object to one of its
//	subkeys.  If the call is successful, the parent key is no longer 
//	accessible.  This call is useful for descending a hierarchy of
//	keys without creating a large number of VRegistryKey objects.
//
// Return Value
//	Returns non-zero if successful.
//
BOOL VRegistryKey::reconstruct(PCHAR pSubkeyName, BOOL bCreate)
{
	HKEY newHandle;

	if (bCreate)
		m_lastError = _RegCreateKey(m_handle, pSubkeyName, &newHandle);
	else
		m_lastError = _RegOpenKey(m_handle, pSubkeyName, &newHandle);

	if (m_lastError == ERROR_SUCCESS)
	{
		_RegCloseKey(m_handle);
		m_handle = newHandle;
	}

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::firstSubkey
//
// Syntax
//	BOOL VRegistryKey::firstSubkey(PCHAR pSubkeyName, DWORD maxChars)
//
// Parameters
//	pSubkeyName	buffer to receive subkey name
//	maxChars	size of buffer pSubkeyName, in bytes
//
// Remarks
//	Begins an enumeration of subkeys. Call member function nextSubkey
//	to get additional subkeys.
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::firstSubkey(PCHAR pSubkeyName, DWORD maxChars)
{
	m_lastError = _RegEnumKey(m_handle, 0, pSubkeyName, maxChars);
	m_enumSubkeyIndex = 1;

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::nextSubkey
//
// Syntax
//	BOOL VRegistryKey::nextSubkey(PCHAR pSubkeyName, DWORD maxChars)
//
// Parameters
//	pSubkeyName	buffer to receive subkey name
//	maxChars	size of buffer pSubkeyName, in bytes
//
// Remarks
//	Fetches the name of the next subkey in an enumeration of subkeys. You
//	must first call member function firstSubkey to initialize the 
//	enumeration of subkeys.
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::nextSubkey(PCHAR pSubkeyName, DWORD maxChars)
{
	m_lastError =
		_RegEnumKey(m_handle, m_enumSubkeyIndex, pSubkeyName, maxChars);
	m_enumSubkeyIndex++;

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::firstValue
//
// Syntax
//	BOOL firstValue(PCHAR pValueName, DWORD* pcbValueName,  
//	      REGISTRYVALUETYPE* pType,PVOID pValueData, DWORD* pcbValueData)
//
// Parameters
//	pValueName	buffer to receive name of the value
//	pcbValueName	pointer to DWORD that holds size of pValueName 
//			buffer on input, and receives number of bytes written
//			to pValueName on output
//	pType		address of variable to receive value type
//	pValueData	buffer to receive value data
//	pcbValueData	pointer to DWORD that holds size of pValueData 
//			buffer on input, and receives number of bytes written
//			to pValueData on output
// Remarks
//	Begin an enumeration of the key's values.  This function retrieves
//	the information for the first value.  Each call to member function
//	nextValue retrieves information for a value until there are no
//	more.
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::firstValue(PCHAR pValueName, DWORD* pcbValueName,  
	      REGISTRYVALUETYPE* pType, PVOID pValueData, DWORD* pcbValueData)

{
	DWORD reserved;

	m_lastError = _RegEnumValue(m_handle, 0, pValueName, pcbValueName,
		&reserved, (DWORD*)pType, (PBYTE)pValueData, pcbValueData);

	m_enumValueIndex = 1;

	return (m_lastError == ERROR_SUCCESS);
}


// Function
//	VRegistryKey::nextValue
//
// Syntax
//	BOOL nextValue(PCHAR pValueName, DWORD* pcbValueName,  
//	      REGISTRYVALUETYPE* pType, PVOID pValueData, DWORD* pcbValueData)
//
// Parameters
//	pValueName	buffer to receive name of the value
//	pcbValueName	pointer to DWORD that holds size of pValueName 
//			buffer on input, and receives number of bytes written
//			to pValueName on output
//	pType		address of variable to receive value type
//	pValueData	buffer to receive value data
//	pcbValueData	pointer to DWORD that holds size of pValueData 
//			buffer on input, and receives number of bytes written
//			to pValueData on output
// Remarks
//	Continues an enumeration of the key's values.  You must first call
//	member function firstValue to initialize the enumeration.  This
//	function retrieves information for the next value of the key.
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::nextValue(PCHAR pValueName, DWORD* pcbValueName,  
	      REGISTRYVALUETYPE* pType,PVOID pValueData, DWORD* pcbValueData)

{
	DWORD reserved;

	m_lastError = _RegEnumValue(m_handle, m_enumValueIndex, pValueName, 
		pcbValueName, &reserved, (DWORD*)pType, (PBYTE)pValueData,
		pcbValueData);

	m_enumValueIndex++;

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::setValue
//
// Syntax
//	BOOL VRegistryKey::setValue(PCHAR pValueString)
//
// Parameters
//	pValueString	value data string
//
// Remarks
//	Sets the anonymous value of the key to the supplied string.
//	If the key previously had no anonymous value, then it is added
//	to the key.
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::setValue(PCHAR pValueString)
{
	return setSubkeyValue(0, pValueString);
}

// Function
//	VRegistryKey::setValue
//
// Syntax
//	BOOL VRegistryKey::setValue(PCHAR pValueName, PVOID pValueData, 
//		DWORD cbData, REGISTRYVALUETYPE rvType)
//
// Parameters
//	pValueName	name of value whose value is to be set
//	pValueData	value data (any binary data)
//	cbData		number of bytes of value data
//	rvType		one of: RVT_SZ (string) or RVT_BINARY (binary data)
//
// Remarks
//	Sets the data for a value of the key.  If the value is not already
//	present on the key, then it is added to the key.
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::setValue(PCHAR pValueName, PVOID pValueData, 
		DWORD cbData, REGISTRYVALUETYPE rvType)
{
	DWORD reserved=0;

	m_lastError = _RegSetValueEx(m_handle, pValueName, reserved, 
			(DWORD)rvType, (PBYTE)pValueData, cbData);

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::setSubkeyValue
//
// Syntax
//	BOOL VRegistryKey::setSubkeyValue(PCHAR pSubkeyName, PCHAR pValueString)
//
// Parameters
//	pSubkeyName	name of the subkey whose value is to be set
//	pValueString	value data string
//
// Remarks
//	Sets the anonymous value of the specified subkey to the supplied 
//	string.  If the subkey previously had no anonymous value, then it is 
//	added to the subkey.
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::setSubkeyValue(PCHAR pSubkeyName, PCHAR pValueString)
{
	m_lastError = _RegSetValue(m_handle, pSubkeyName, REG_SZ,
			pValueString, strlen(pValueString)+1);

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::getValue
//
// Syntax
//	BOOL VRegistryKey::getValue(PCHAR pValueString, DWORD* pcbValue)
//
// Parameters
//	pValueString	buffer to receive anonymous value string
//	pcbValue	pointer to DWORD that holds size of value
//			string buffer on input, and receives count
//			of bytes written to buffer on output.
//
// Remarks
//	Retrieves the anonymous value for the key, if any.  The value
//	is a null-terminated string.  
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::getValue(PCHAR pValueString, DWORD* pcbValue)
{
	m_lastError = _RegQueryValue(m_handle, 0, pValueString, (PLONG)pcbValue);

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::getValue
//
// Syntax
//	BOOL VRegistryKey::getValue(PCHAR pValueName, PVOID pValueData, 
//		DWORD* pcbData,	REGISTRYVALUETYPE* pValueType);
//
// Parameters
//	pValueName	value name string specifies the value that 
//			is to be retrieved.
//	pValueData	pointer to buffer to receive value data
//	pcbData		pointer to DWORD that holds size of pValueData
//			buffer on input, and receives actual size of
//			of value data on output
//	pValueType	(optional) pointer to location to receive value type;
//			the default is NULL, in which case the function
//			does not attempt to provide type information
//
// Remarks
//	Retrieves value information for the key.  pValueName specifies the
//	name of the value to be retrieved.  
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::getValue(PCHAR pValueName, PVOID pValueData, DWORD* pcbData,
		REGISTRYVALUETYPE* pValueType)
{
	DWORD reserved;

	m_lastError = _RegQueryValueEx(m_handle, pValueName, &reserved,
		(DWORD*)pValueType, (PBYTE)pValueData, pcbData);

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::getSubkeyValue
//
// Syntax
//	BOOL VRegistryKey::getSubkeyValue(PCHAR pSubkeyName, PCHAR pValueString, 
//		DWORD* pcbValue)
//
// Parameters
//	pSubkeyName	name of subkey for which the anonymous value is to
//			be retrieved
//	pValueString	buffer to receive anonymous value string
//	pcbValue	pointer to DWORD that holds size of value
//			string buffer on input, and receives count
//			of bytes written to buffer on output.
//
// Remarks
//	Retrieves the anonymous value for the specified subkey, if present.  
//	The value is a null-terminated string.  
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::getSubkeyValue(PCHAR pSubkeyName, PCHAR pValueString,
	DWORD* pcbValue)
{
	m_lastError = _RegQueryValue(m_handle, pSubkeyName, pValueString,
				(PLONG)pcbValue);

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::flush
//
// Syntax
//	BOOL VRegistryKey::flush()
//
// Remarks
//	Writes changes to the key out to the registry file. 
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::flush()
{
	m_lastError = _RegFlushKey(m_handle);

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::removeSubkey
//
// Syntax
//	BOOL VRegistryKey::removeSubkey(PCHAR pSubkeyName)
//
// Parameters
//	pSubkeyName	name of the subkey to remove; may not be NULL
//
// Remarks
//	Deletes the specified subkey from the Registry.  All subkeys
//	of the specified subkey are also removed.
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::removeSubkey(PCHAR pSubkeyName)
{
	m_lastError = _RegDeleteKey(m_handle, pSubkeyName);

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::removeValue
//
// Syntax
//	BOOL VRegistryKey::removeValue(PCHAR pValueName)
//
// Parameters
//	pValueName	name of the value to remove
//
// Remarks
//	Removes the specified value from the key.
//
// Return Value
//	Returns FALSE if any error occurs. See member function lastError 
//	for more information.
//
BOOL VRegistryKey::removeValue(PCHAR pValueName)
{
	m_lastError = _RegDeleteValue(m_handle, pValueName);

	return (m_lastError == ERROR_SUCCESS);
}

// Function
//	VRegistryKey::operator=(char*)
//
// Syntax
//	char* VRegistryKey::operator=(char* pValue)
//
// Parameters
//	pValue		string pointer assigned to *this
//
// Remarks
//	This function allows simple syntax for setting the anonymous
//	value of the key.  Specifically, when an expression of type
//	char* is assigned to an lvalue of type VRegistryKey, this
//	function sets the anonymous value of the key to the null
//	terminated string pointed to by the right side of the assignment.
//
//	For example:
//
//		VRegistryKey vkey(PRK_LOCAL_MACHINE,"");
//
//		...	
//
//		vkey = "Version 6.2";
//
//	sets the value of vkey to the string "Version 6.2".
//
// Return Value
//	Returns *this
//
char* VRegistryKey::operator=(char* pValue)
{
	setValue(pValue);
	return pValue;
}

#endif //WIN40

