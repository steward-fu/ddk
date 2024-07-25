// kregkey.h - Include file for class KRegistryKey
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

#ifndef __KREGKEY__
#define __KREGKEY__

typedef enum __registryRoot
{
	REGISTRY_ABSOLUTE = RTL_REGISTRY_ABSOLUTE,
	REGISTRY_SERVICES = RTL_REGISTRY_SERVICES,   	// \Registry\Machine\System\CurrentControlSet\Services
	REGISTRY_CONTROL = RTL_REGISTRY_CONTROL,     	// \Registry\Machine\System\CurrentControlSet\Control
	REGISTRY_WINDOWS_NT = RTL_REGISTRY_WINDOWS_NT,	// \Registry\Machine\Software\Microsoft\Windows NT\CurrentVersion
	REGISTRY_DEVICEMAP = RTL_REGISTRY_DEVICEMAP,  	// \Registry\Machine\Hardware\DeviceMap
	REGISTRY_USER = RTL_REGISTRY_USER,            	// \Registry\User\CurrentUser
} REGISTRY_ROOT;


#ifndef KEY_DEFAULT_ACCESS
#define KEY_DEFAULT_ACCESS (KEY_READ|KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE)
#endif

#ifndef KEY_DEFAULT_ATTRIBUTES
#define KEY_DEFAULT_ATTRIBUTES (OBJ_CASE_INSENSITIVE)
#endif

struct REGKEYPROPERTIES
{
	ULONG NameLength;
	ULONG ClassNameLength;
	ULONG NumberOfSubkeys;
	ULONG NumberOfValues;
	ULONG MaxSubkeyNameLen;
	ULONG MaxClassNameLen;
	ULONG MaxValueNameLen;
	ULONG MaxValueDataLen;
	LARGE_INTEGER LastWriteTime;
};

struct REGVALUEPROPERTIES
{
	ULONG NameLength;
	ULONG DataLength;
	ULONG Type;
};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

class KRegistryKey
{

// Construction
public:
	SAFE_DESTRUCTORS;

	KRegistryKey(void);

	KRegistryKey(
		REGISTRY_ROOT root,
		PCWSTR path,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES
		);

	KRegistryKey(
		REGISTRY_ROOT root,
		PUNICODE_STRING path,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES
		);

	KRegistryKey(
		PUNICODE_STRING RootDir,
		PCWSTR path=NULL,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES
		);

	KRegistryKey(
		PUNICODE_STRING RootDir,
		PUNICODE_STRING path,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES
		);

#if _WDM_ && !defined(VXD_COMPATLIB)
	KRegistryKey(
		PDEVICE_OBJECT pdo,
		PCWSTR path=NULL,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES
		);
	KRegistryKey(
		PDEVICE_OBJECT pdo,
		ULONG DevInstKeyType,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS
		);
#endif // _WDM_ && !defined(VXD_COMPATLIB)

	NTSTATUS Initialize(
		REGISTRY_ROOT root,
		PCWSTR path,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES
		);

	NTSTATUS Initialize(
		REGISTRY_ROOT root,
		PUNICODE_STRING path,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES
		);

	NTSTATUS Initialize(
		PUNICODE_STRING RootDir,
		PCWSTR path=NULL,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES
		);

	NTSTATUS Initialize(
		PUNICODE_STRING RootDir,
		PUNICODE_STRING path,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES
		);

#if _WDM_ && !defined(VXD_COMPATLIB)
	NTSTATUS Initialize(
		PDEVICE_OBJECT pdo,
		PCWSTR path=NULL,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES
		);

	NTSTATUS Initialize(
		PDEVICE_OBJECT pdo,
		ULONG DevInstKeyType,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS
		);
#endif // _WDM_ && !defined(VXD_COMPATLIB)

	NTSTATUS Reconstruct(
		PUNICODE_STRING RootDir,
		PUNICODE_STRING path,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0
		);

	NTSTATUS Reconstruct(
		REGISTRY_ROOT root,
		PUNICODE_STRING path,
		ACCESS_MASK access = KEY_DEFAULT_ACCESS,
		ULONG attributes = KEY_DEFAULT_ATTRIBUTES,
		BOOLEAN bCreateSubkey=FALSE,
		ULONG CreateOptions=0
		);

    NTSTATUS Reconstruct();

	~KRegistryKey(void)
    {
        if (IsValid()) Invalidate();

        if(m_RootDir.Buffer != NULL)
        {
            delete [] m_RootDir.Buffer;
            m_RootDir.Buffer = NULL;
            m_RootDir.Length = 0;
        }

        if(m_Path.Buffer != NULL)
        {
            delete [] m_Path.Buffer;
            m_Path.Buffer = NULL;
            m_Path.Length = 0;
        }
    }

	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return (m_KeyHandle != (HANDLE)0); }

// Methods
public:
	// current status
	NTSTATUS LastError(void);

	// access data for known value
	NTSTATUS QueryValue(
		PCWSTR Name,
		ULONG* pType,
		ULONG& Length,
		PVOID DataBuffer
		);
	NTSTATUS QueryValue(PCWSTR Name, ULONG* pDword);
	NTSTATUS QueryValue(
		PCWSTR Name,
		PVOID& pBinary,
		ULONG& Length,
		POOL_TYPE=PagedPool
		);
	NTSTATUS QueryValue(
		PCWSTR Name,
		PWSTR& pRegsz,
		ULONG& Length,
		POOL_TYPE=PagedPool
		);

	// subkey enumeration
	VOID InitializeSubkeyEnumeration(void);
	NTSTATUS EnumerateSubkey(
		PUNICODE_STRING Name,
		PUNICODE_STRING Class,
		REGKEYPROPERTIES* pKeyProps);

	// value enumeration
	VOID InitializeValueEnumeration(void);
	NTSTATUS EnumerateValue(PUNICODE_STRING Name);
	NTSTATUS EnumerateValue(
		PUNICODE_STRING Name,
		PVOID Data,
		ULONG DataBufferLen,
		REGVALUEPROPERTIES* pValueProps);

	// creation of values
	NTSTATUS WriteValue(PCWSTR ValueName, ULONG Type, PVOID Data, ULONG DataSize);
	NTSTATUS WriteValue(PCWSTR ValueName, ULONG ValueData);
	NTSTATUS WriteValue(PCWSTR ValueName, PCWSTR ValueData);
	NTSTATUS WriteValue(PUNICODE_STRING ValueName, ULONG Type, PVOID Data, ULONG DataSize);
	NTSTATUS WriteValue(PUNICODE_STRING ValueName, ULONG ValueData);
	NTSTATUS WriteValue(PUNICODE_STRING ValueName, PCWSTR ValueData);

	// delete the current registry key
	NTSTATUS Delete(void);

    // delete a value key
#if !defined(VXD_COMPATLIB)
	NTSTATUS DeleteValue(PCWSTR ValueName);
#endif // !defined(VXD_COMPATLIB)
	NTSTATUS DeleteValue(PUNICODE_STRING ValueName);

	static PUNICODE_STRING GetRootPath(PUNICODE_STRING pRoot, REGISTRY_ROOT r);

protected:
	ULONG GetValueDataSize(PCWSTR Name);

// Data
public:
	ULONG	            m_CreateDisposition;
protected:
	HANDLE              m_KeyHandle;        // returned by ZwOpenKey, ZwCreateKey
	NTSTATUS            m_LastError;
	ULONG               m_ValueEnumIndex;
	ULONG               m_KeyEnumIndex;

    // members for reconstruct
    UNICODE_STRING      m_RootDir;
	UNICODE_STRING      m_Path;
	ACCESS_MASK         m_Access;
	ULONG               m_Attributes;
	BOOLEAN             m_bCreateSubkey;
	ULONG               m_CreateOptions;
};

#if (!WDM_MAJORVERSION || ((WDM_MAJORVERSION == 0x01) && (WDM_MINORVERSION < 0x20)))
extern "C" NTSTATUS ZwDeleteValueKey(HANDLE h, PUNICODE_STRING s);
#endif

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

inline VOID KRegistryKey::Invalidate(void)
{
	HANDLE h = m_KeyHandle;
	m_KeyHandle = (HANDLE)0;
	if (h != (HANDLE)0) ZwClose(h);
}

inline NTSTATUS KRegistryKey::LastError(void)
{
	return m_LastError;
}

inline VOID KRegistryKey::InitializeSubkeyEnumeration(void)
{
	m_KeyEnumIndex = 0;
}

inline VOID KRegistryKey::InitializeValueEnumeration(void)
{
	m_ValueEnumIndex = 0;
}

#if !defined(VXD_COMPATLIB)
inline NTSTATUS KRegistryKey::DeleteValue(PCWSTR ValueName)
{
	UNICODE_STRING s;

	RtlInitUnicodeString(&s, ValueName);

	return DeleteValue(&s);
}
#endif // !defined(VXD_COMPATLIB)

#if !_WDM_
inline NTSTATUS KRegistryKey::DeleteValue(PUNICODE_STRING ValueName)
{
	return ZwDeleteValueKey(m_KeyHandle, ValueName);
}
#else // !_WDM_
#if !defined(VXD_COMPATLIB)
inline NTSTATUS KRegistryKey::DeleteValue(PUNICODE_STRING ValueName)
{
    NTSTATUS status;

    // Create a temporary string used to delete the value
    PWSTR    value = new WCHAR[(ValueName->Length/sizeof(WCHAR)) + 2];

    if(NULL == value)
    {
        return STATUS_UNSUCCESSFUL;
    }

    RtlCopyMemory(value, ValueName->Buffer, ValueName->Length);

    value[ValueName->Length/sizeof(WCHAR)] = L'\0';
    value[ValueName->Length/sizeof(WCHAR) + 1] = L'\0';

	status = RtlDeleteRegistryValue(RTL_REGISTRY_HANDLE,
                                    (PWSTR)m_KeyHandle,
                                    value);

    // Delete our temporary string
    delete [] value;

    if(!NT_SUCCESS(status))
    {
        return status;
    }

    // The call to RtlDeleteRegistryValue closes
    // our handle on some platforms, so we need to
    // reconstruct. This happens on all platforms
    // prior to and including 2000 despite what
    // some DDK documentation may claim about 2000
    if(!IoIsWdmVersionAvailable(1, 0x20))
    {
        m_KeyHandle = (HANDLE)0;

        Reconstruct();
    }

    return STATUS_SUCCESS;
}
#endif // !VXD_COMPATLIB
#endif // !_WDM_

inline NTSTATUS KRegistryKey::Delete(void)
{
    // Callers of ZwDeleteKey must be running at PASSIVE_LEVEL IRQL
	ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );

	return ZwDeleteKey(m_KeyHandle);
}

#endif // __KREGKEY__
