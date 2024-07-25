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
//=============================================================================

// RFSDUTIL.C - Utility routines for Registry File System
// Copyright (c) 1996, Compuware Corporation

#include  "regfsd.h"
#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

PCHAR PredefinedRegKeyStrings[NPREDEFKEYS] = {
	"HKEY_CLASSES_ROOT",
	"HKEY_CURRENT_USER",
	"HKEY_LOCAL_MACHINE",
	"HKEY_USERS",
	"HKEY_CURRENT_CONFIG",
	"HKEY_DYN_DATA"
};

INT PredefinedRegKeyValues[NPREDEFKEYS] = {
	HKEY_CLASSES_ROOT,
	HKEY_CURRENT_USER,
	HKEY_LOCAL_MACHINE,
	HKEY_USERS,
	HKEY_CURRENT_CONFIG,
	HKEY_DYN_DATA
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	SetupDCB
//
// Purpose
//	Initializes fields of a DCB passed to the port driver on an
//	AEP_INQUIRY call.
//
// Parameters
//	pDcb	address of the DCB to initialize
//
// Return Value
//	nonen
//
// Remarks
//	The upper IOS layers are fairly sensitive the values inserted 
//	into the DCB by this routine. The fields must be consistent with
//	values supplied in the INF file and the DRP, or the upper layers
//	may ignore or unconfig the DCB. What's not clear is exactly which
//	parameters have a critical role.

VOID SetupDCB(DCB* pDcb)
{
	strcpy(pDcb->DCB_vendor_id, "VIREO");
	strcpy(pDcb->DCB_product_id, "VREGFSD");
	pDcb->DCB_cmn.DCB_unit_number = 0;
	pDcb->DCB_cmn.DCB_TSD_Flags = 0;
	pDcb->DCB_cmn.DCB_device_type = DCB_type_disk;
	pDcb->DCB_cmn.DCB_device_flags = DCB_DEV_PHYSICAL|
					 DCB_DEV_MUST_CONFIGURE;
	pDcb->DCB_cmn.DCB_device_flags2 = 0;
	pDcb->DCB_cmn.DCB_user_drvlet = 0xffff;
	pDcb->DCB_cmn.DCB_Partition_Start = 1;
	pDcb->DCB_cmn.DCB_track_table_ptr = 0;
	pDcb->DCB_cmn.DCB_disk_bpb_flags = DCBF_DISK_BPB_USEFAKE;
	pDcb->DCB_cmn.DCB_bds_ptr = 0;
	pDcb->DCB_cmn.DCB_partition_type = 0;
	pDcb->DCB_cmn.DCB_cAssoc = 0;

	pDcb->DCB_actual_sector_cnt[0] = 2;
	pDcb->DCB_actual_sector_cnt[1] = 0;
	pDcb->DCB_actual_blk_size = 512;
	pDcb->DCB_actual_head_cnt = 1;
	pDcb->DCB_actual_cyl_cnt = 1;
	pDcb->DCB_actual_spt = 2;
	pDcb->DCB_bus_type = DCB_BUS_ESDI;

	strcpy(pDcb->DCB_port_name, "VREGFSD");
	pDcb->DCB_bdd.DCB_apparent_blk_size = 512;
	pDcb->DCB_bdd.DCB_apparent_head_cnt = 1;
	pDcb->DCB_bdd.DCB_apparent_sector_cnt[0] = 2;
	pDcb->DCB_bdd.DCB_apparent_sector_cnt[1] = 0;
	pDcb->DCB_bdd.DCB_apparent_head_cnt = 1;
	pDcb->DCB_bdd.DCB_apparent_cyl_cnt = 1;
	pDcb->DCB_bdd.DCB_apparent_spt = 2;
	pDcb->DCB_bdd.DCB_BDD_flags = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	AssociateDriveLetter
//
// Purpose
//	Associate a drive letter with a DCB
//
// Parameters
//	pDcb	pointer to the DCB
//
// Return Value
//	Returns the assigned drive letter code (A=0).
//
// Remarks
//	To do this requires two calls to the IOS service routine in 
//	the first field of the ILB. The first call grabs the next
//	free drive letter, and the second call binds the DCB to that
//	drive letter.
//
//	It has been observed that previously configured network shares
//	may end up conflicting with the RFSD upon booting after the 
//	initial installation. This is not serious - simply redefine the
//	shares to the next letter.
//
//	A port driver should not really have to do this - the DISKTSD
//	(or voltrack?) should take care of it automatically. Again, 
//	it is not clear exactly what the requirements are for having the
//	IOS automatically assign a drive letter, but this approach seems
//	to work fine.
//
BYTE AssociateDriveLetter(DCB_COMMON* pDcb)
{
	ISP_pick_drive_letter PDisp;
	ISP_dcb_associate ADisp;	

// Set up an ISP to grab the next free drive letter and send it.

	PDisp.ISP_p_d_l_hdr.ISP_func = ISP_DRIVE_LETTER_PICK;
	PDisp.ISP_p_d_l_pdcb = pDcb;
	PDisp.ISP_p_d_l_flags = 0;
	REGFSD_Ilb.ILB_service_rtn((PISP)&PDisp);

// Now bind the allocated drive letter to the DCB.

	ADisp.ISP_d_a_hdr.ISP_func = ISP_ASSOCIATE_DCB;
	ADisp.ISP_d_a_dcb = pDcb;
	ADisp.ISP_d_a_drive = PDisp.ISP_p_d_l_letter[0];
	REGFSD_Ilb.ILB_service_rtn((PISP)&ADisp);

	return (pDcb->DCB_drive_lttr_equiv = PDisp.ISP_p_d_l_letter[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	ParseOpenFilePath
//
// Purpose
//	Parse the unicode filepath provided by IFSMgr on the call to open a
//	file
//
// Parameters
//	path		the unicode path
//	pRootKey	location to receive root key (HKEY_xxx)
//	pRegPath	location to receive subkey string
//	pValue		location to receive value string
//	ValueSize	size of the buffer to receive value
//
// Return Value
//	Returns TRUE if the path is a valid Registry path for a file open
//
BOOL ParseOpenPath(path_t path, PHKEY pRootKey, PCHAR pRegPath, PCHAR pValue,
	DWORD ValueSize)
{
	CHAR ElemName[64];
	PathElement* pElem;
	PathElement* pLastElem;
	_QWORD qResult;
	int ElemIndex;

// Initialize the parse

	pElem = &path->pp_elements[0];
	pLastElem = IFSLastElement(path);
	*pRegPath = '\0';
	*pValue = '\0';

// Scan the first element of the path name. It must be one of the predefined
// keys.
	UniToBCS(ElemName, pElem->pe_unichars, pElem->pe_length-2, 
		sizeof(ElemName)-1, BCS_WANSI, &qResult);
	ElemName[qResult.ddLower] = '\0';	// null terminate

	if ( (*pRootKey = IsPredefinedKey(ElemName)) == 0)
		return FALSE;

// Walk through the remaining elements one by one until we reach the last

	pElem = IFSNextElement(pElem);	
	for (ElemIndex=1; pElem < pLastElem; ElemIndex++)	
	{
		if (ElemIndex != 1)
			*pRegPath++ = '\\';

// Append the current element to the subkey

		UniToBCS(pRegPath, pElem->pe_unichars, pElem->pe_length-2, 
			sizeof(ElemName)-1, BCS_WANSI, &qResult);
		pRegPath[qResult.ddLower] = '\0';                
		pRegPath += qResult.ddLower;

// Advance to next element. IFSNextElement is a macro defined in ifs.h.

		pElem=IFSNextElement(pElem);
	}

// The value is the last element 

	UniToBCS(pValue, pElem->pe_unichars, pElem->pe_length-2, 
		ValueSize, BCS_WANSI, &qResult);

	pValue[qResult.ddLower] = '\0';

	return TRUE;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	ParseFindPath
//
// Purpose
//	Parse the unicode filepath provided by IFSMgr on a find first call
//
// Parameters
//	path		the unicode path to parse
//	pRootKey	location to receive root key
//	pSubKey		location to receive subkey
//	ppMatch		location to receive pointer to metamatch element
//
// Return Value
//	Returns TRUE if the path is a valid Registry path for a find first
//
BOOL ParseFindPath(path_t path, PHKEY pRootKey, PCHAR pSubKey, 
	PathElement** ppMatch)
{
	CHAR ElemName[32];
	PathElement* pElem;
	PathElement* pLastElem;
	_QWORD qResult;
	int ElemIndex;

// Initialize the parse

	pElem = &path->pp_elements[0];
	pLastElem = IFSLastElement(path);
	*pSubKey = '\0';

// Scan off the first element of the path

	UniToBCS(ElemName, pElem->pe_unichars, pElem->pe_length-2, sizeof(ElemName)-1, 
		BCS_WANSI, &qResult);
	ElemName[qResult.ddLower] = '\0';

// If first element has metachar, it's a find first in the root

	if (HasMetaChar(ElemName))
	{
		*pRootKey = REGISTRY_ROOT;
		*ppMatch = pElem;
		return TRUE;
	}	

// If the first element does not have a metachar, it must be one of the
// predefined keys.

	else if ( (*pRootKey = IsPredefinedKey(ElemName)) == 0)
		return FALSE;

// Walk the remaining elements

	pElem = IFSNextElement(pElem);	
	for (ElemIndex=1; pElem < pLastElem; ElemIndex++)	
	{
		if (ElemIndex != 1)
			*pSubKey++ = '\\';

// Append the current element to the subkey

		UniToBCS(pSubKey, pElem->pe_unichars, pElem->pe_length-2, 
			sizeof(ElemName)-1, BCS_WANSI, &qResult);
		pSubKey[qResult.ddLower] = '\0';
		pSubKey += qResult.ddLower;

		pElem=IFSNextElement(pElem);
	}

// The pointer to the element containing the metamatch string is the
// a pointer to the last element.

	*ppMatch = pElem;

	return TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	ZeroTime
//
// Purpose
//	Clears the fields of a _FILETIME struct
//
// Parameters
//	pft	points to the _FILETIME struct
//
// Return Value
//	none
//
// Remarks
//	RFSD does not implement file times. Most applications recognize
//	an zeroed out time as empty and do not attempt to convert it.
//
VOID ZeroTime(_FILETIME* pft)
{
	pft->dwLowDateTime = 0;
	pft->dwHighDateTime = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	PutShortName
//
// Purpose
//	Fill in a short name based on key values
//
// Parameters
//	pUniShortName	location to receive short name
//	h		key to build name with
//	e		int to use for .3 extension
//
// Return Value
//	none
//
// Remarks
//	Registry keys and values do not really map on to 8.3 names.
//	FSDs are not required to support 8.3. RFSD half (third?) supports
//	short names. We make the name equal to the hex representation of
//	the key and subkey or value index.
//
VOID PutShortName(PWORD pUniShortName, HKEY h, INT e)
{
	char buf[13];
	_QWORD q;

	sprintf(buf, "%08x.%03x", h, e);
	BCSToUni(pUniShortName, buf, 13, BCS_WANSI, &q); 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	IsThisOurDrive
//
// Purpose
//	Determine if an the request structure on a mount request
//	is referencing the virtual drive created by RFSD.
//
// Parameters
//	pir	points to fsd request sent to mount routine
//
// Return Value
//	Returns TRUE if this is our drive; otherwise FALSE.
//
// Remarks
//	This functions issues an ISP to get the DCB corresponding to
//	the mounted drive. It the DCB address matches, then this is our
//	drive
//
BOOL IsThisOurDrive(pioreq pir)
{
	ISP_dcb_get GetDcbIsp;
	
	GetDcbIsp.ISP_g_d_hdr.ISP_func = ISP_GET_DCB;	
	GetDcbIsp.ISP_g_d_drive = pir->ir_mntdrv;
	REGFSD_Ilb.ILB_service_rtn((PISP)&GetDcbIsp);

	return (GetDcbIsp.ISP_g_d_dcb->DCB_physical_dcb == 
		(ULONG)pDcbOfThisDevice);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	HasMetaChar
//
// Purpose
//	Determine if a string contains a metachar
//
// Parameters
//	s	pointer to the string
//
// Return Value
//	Returns TRUE if the string contains a metachar
//
// Remarks
//	Sometimes the IFSMgr passes this information in the
//	parsing flags. 
//
BOOL HasMetaChar(PCHAR s)
{	
	while (*s)
		if ( (*s == '*') || (*s == '?') )
			return TRUE;
		else
			s++;
	return FALSE;		
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	CheckFileExists
//
// Purpose
//	Determine if a file (Registry value) exists
//
// Parameters
//	path	unicode path of file (value)
//
// Return Value
//	Returns TRUE if the file exists
//
BOOL CheckFileExists(path_t path)
{
	HKEY RootKey, k;
	CHAR subkey[256];
	CHAR value[128];
	DWORD ValueSize = sizeof(value);
	_QWORD qResult;

	if (IFSIsRoot(path))	// macro
		return FALSE;	// the root is a directory, not a file

	return ParseOpenPath(path,&RootKey,subkey,value,sizeof(value)) &&
	       (_RegOpenKey(RootKey, subkey, &k) == ERROR_SUCCESS) &&
	       (RegQueryValueEx(k,value, 0,0,0,&ValueSize)==ERROR_SUCCESS);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	CheckDirectoryExists
//
// Purpose
//	Determine if a path (Registry key) exists
//
// Parameters
//	path	unicode path of directory (key)
//
// Return Value
//	Returns TRUE if the file exists
//
// Remarks
//	This will return TRUE if passed a valid key with an invalid value.

BOOL CheckDirectoryExists(path_t path)
{
	HKEY RootKey, k;
	CHAR subkey[256];
	PathElement* pLast;
	_QWORD qResult;

	if (IFSIsRoot(path))
		return TRUE;

	return ParseFindPath(path, &RootKey, subkey, &pLast) &&
	       (_RegOpenKey(RootKey, subkey, &k) == ERROR_SUCCESS);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	CopyParsedPath
//
// Purpose
//	Copy a ParsedPath structure
//
// Parameters
//	dest	location to receive copied path
//	source	location to copy path from
//
// Return Value
//	Returns number of bytes copied
//
WORD CopyParsedPath(path_t dest, path_t source)
{
	memcpy(dest, source,  source->pp_totalLength+2);
	return dest->pp_totalLength;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	ustrlen
//
// Purpose
//	Unicode strlen
//
// Parameters
//	pUniString	string to take length of
//
// Return Value
//	Returns number of unicode characters in the supplied string
//
unsigned int ustrlen(WORD* pUniString)
{
	unsigned int i = 0;

	while (*pUniString++)
		i++;

	return i;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	ustrcpy
//
// Purpose
//	Unicode strcpy
//
// Parameters
//	dest	location to receive string
//	src	location to copy from
//
// Return Value
//	Returns dest
//
WORD* ustrcpy(WORD* dest, WORD* src)
{
	_asm {
		mov	esi, [src]
		mov	edi, [dest]
		cld
	}
next:
	_asm {
		lodsw
		stosw
		or	ax, ax
		jnz	next
	}

	return dest;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	IsPredefinedKey
//
// Purpose
//	Determine if the supplied string matches the name of one of
//	six predefined Registry root keys
//
// Parameters
//	AscPath		pointer to string
//
// Return Value
//	If success, returns the constant value associated with the
//	key (as defined by the system). Otherwise returns zero.
//
HKEY IsPredefinedKey(PCHAR AscPath)
{
	INT i;

	for (i=0; i <  NPREDEFKEYS; i++)
		if (strcmp(AscPath, PredefinedRegKeyStrings[i]) == 0)
			return PredefinedRegKeyValues[i];

	return 0;		
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	GetIop
//
// Purpose
//	Allocate an IOP
//
// Parameters
//	pSvc		address of the ILB service routine
//	IopSize		size of the IOP to allocate
//	DeltaIor	byte offset from start of IOP to start of IOR
//	flags		flags to provide on allocation call
//	pResult		location to receive result
//
// Return Value
//	Returns a pointer to the allocated IOP
//
IOP* GetIop(PFNISP pSvc, USHORT IopSize, ULONG DeltaIor, 
	UCHAR flags, WORD* pResult)
{
	ISP_IOP_alloc AllocIsp;

	AllocIsp.ISP_i_c_hdr.ISP_func = ISP_CREATE_IOP;
	AllocIsp.ISP_IOP_size = IopSize;
	AllocIsp.ISP_delta_to_ior = DeltaIor;
	AllocIsp.ISP_i_c_flags = flags;

	pSvc((ISP*)&AllocIsp);
	if (pResult)
		*pResult = AllocIsp.ISP_i_c_hdr.ISP_result;

	return (IOP*)AllocIsp.ISP_IOP_ptr;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	ReleaseIop
//
// Purpose
//	Releases a previously allocate IOP
//
// Parameters
//	pSvc	address of the ILB service routine
//	pIop	address of the IOP to free
//
// Return Value
//	Returns the result from the dealloc call
//
WORD ReleaseIop(PFNISP pSvc, IOP* pIop)
{
	ISP_mem_dealloc DeallocIsp;

	DeallocIsp.ISP_mem_da_hdr.ISP_func = ISP_DEALLOC_MEM;
	DeallocIsp.ISP_mem_ptr_da = (ULONG)pIop;

	pSvc((ISP*)&DeallocIsp);
	
	return DeallocIsp.ISP_mem_da_hdr.ISP_result;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	IoctlGetDeviceParams	
//
// Purpose
//	Get (fake) IOCTL parameters
//
// Parameters
//	pDP	address of struct to receive params
//
// Return Value
//	none
//
// Remarks
//	The struct is defined by DOS
//
VOID IoctlGetDeviceParams(PDEVICEPARAMS pDP)
{
	pDP->dpSpecFunc = 0;
	pDP->dpDevType = 5; // hard disk
	pDP->dpDevAttr = 1; // not removable
	pDP->dpCylinders = 1;
	pDP->dpMediaType = 0;
	pDP->dpBytesPerSector = 512;
	pDP->dpSectorsPerCluster = 1;
	pDP->dpReservedSectors = 0;
	pDP->dpFATs = 0;
	pDP->dpRootDirectoryEntries = 1024;
	pDP->dpSectors = 2;
	pDP->dpMedia = 0;
	pDP->dpSectorsPerFAT = 0;
	pDP->dpSectorsPerTrack = 2;
	pDP->dpHeads = 2;
	pDP->dpHiddenSectors = 0;
	pDP->dpHugeSectors = 0;
}

