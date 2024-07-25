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

// RFSDIFS.C - File System Interface for Registry File System
// Copyright (c) 1996, Compuware Corporation

// This module contains the functions that are called by the IFSMgr.

#include  "regfsd.h"
#include PAGEABLE_CODE_SEGMENT
#include PAGEABLE_DATA_SEGMENT

// This is the table of function addresses that RFSD returns to 
// IFSMgr from RegMountVolume.

volfunc VolumeFunctions = {
	IFS_VERSION,
	IFS_REVISION,
	NUM_VOLFUNC,

	VolDelete, 	
	VolDir,		
	VolAttrib,	
	Vol_Flush,	
	VolInfo,	
	VolOpen,	
	VolRename,	
	VolSearch,	
	VolQuery,	
	VolDisconnect,	
	VolUNCPipe,	
	VolIoctl16,	
	VolParams, 	
	VolFindOpen,	
	VolDasdio	
};

// This is the table of function addresses that RFSD returns to 
// IFSMgr when a file is opened (VolOpen) or when a FindFirst/FindNext
// operation is initiated (VolFindOpen).

hndlmisc HandleFunctions = {
	IFS_VERSION,
	IFS_REVISION,
	NUM_HNDLMISC,

	HandleSeek,
	HandleClose,
	HandleCommit,	 
	HandleFilelocks,
	HandleFiletimes,
	HandlePiperequest,
	HandleHandleinfo,
	HandleEnumhandle
};

// Declare storage for the VolumeHandle. The VolumeHandle is supplied
// by the IFSMgr when the volume is mounted.

VRP* VolumeHandle=0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	RegMountVolume
//
// Purpose
//	Provides an entry point for the IFSMgr to notify RFSD of
//	volume events
//
// Parameters
//	pir		pointer to io_req 
//	  ir_flags	subfunction code
//	 For Mount call:
//	  ir_volh	VRP pointer
//	  ir_mntdrv	drive letter code
//	  
// Return Value
//	Returns zero on success.
//
// Remarks
//	There are several subfunctions; ir_flags specifies which
//	subfunction is requested. RFSD supports only the MOUNT
//	function. 
//
//	If RFSD determines that the volume being mounted is on the
//	virtual drive that it created, it sets the fields in the
//	request structure as follows:
//
//		ir_error = 0
//		ir_vfunc = address of volume function table
//
//	In addition, this routine sets field VRP_fsd_entry of the 
//	supplied VRP to point back to the mount function.
//
INT RegMountVolume(pioreq pir)
{
// Branch on subfunction:

	switch (pir->ir_flags)
	{
	case IR_FSD_MOUNT:
		DEBUGTRACE("IR_FSD_MOUNT");

// If the drive being mounted is found to be the virtual drive 
// created by the port driver component of RFSD, then set the
// fields of the request structure to indicate success. 

		if (IsThisOurDrive(pir))
		{
			VolumeHandle = (VRP*)pir->ir_volh;
			VolumeHandle->VRP_fsd_entry = 
				(ULONG)RegMountVolume;
			Drive = pir->ir_mntdrv;
			pir->ir_vfunc = &VolumeFunctions;
			return (pir->ir_error = 0);
		}	
		break;

// Subfunctions other than IF_FSD_MOUNT are not implemented

	case IR_FSD_VERIFY:
		DEBUGTRACE("IR_FSD_VERIFY");
		break;

	case IR_FSD_UNLOAD:
		DEBUGTRACE("IR_FSD_UNLOAD");
		break;

	case IR_FSD_MOUNT_CHILD:
		DEBUGTRACE("IR_FSD_MOUNT_CHILD");
		break;

	case IR_FSD_MAP_DRIVE:
		DEBUGTRACE("IR_FSD_MAP_DRIVE");
		break;

	case IR_FSD_UNMAP_DRIVE:
		DEBUGTRACE("IR_FSD_UNMAP_DRIVE");
		break;
	}

	return (pir->ir_error = -1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	VolAttrib
//
// Purpose
//	Several subfunctions related to file attributes. RFSD
//	implements only GET_ATTRIBUTES.
//
// Parameters
//	pir		pointer to io_req 
//	  ir_flags	subfunction code
//	  ir_ppath	unicode path 
//	  ir_attr	return attribute flags
//
// Return Value
//	Returns zero on success.
//
// Remarks
//	Only the GET_ATTRIBUTES subfunction is implemented. The 
//	function calls utility routines that access the Registry
//	to determine if the given path corresponds to a key
//	(directory),  a value (file), or is not found.
//
INT VolAttrib(pioreq pir)
{
// Branch on subfunction

	switch (pir->ir_flags)
	{
	case GET_ATTRIBUTES:
		pir->ir_error = 0;

		if  (CheckFileExists(pir->ir_ppath))
			pir->ir_attr = FILE_ATTRIBUTE_READONLY;

		else if (CheckDirectoryExists(pir->ir_ppath) && 
// A small kludge here is an artifact of the behavior of the 
// VMM's Registry services. The services indicate the existence
// of a key in cases where the path contains a valid key but a 
// non-existent value. This adds a layer of complexity which could
// have been dealt with more precisely, but the check for a dot in
// the filename is expedient for this demonstration driver.
			 !(pir->ir_attr & FILE_FLAG_HAS_DOT))

			pir->ir_attr = FILE_ATTRIBUTE_DIRECTORY;
		else
		{
			pir->ir_error = ERROR_FILE_NOT_FOUND;
			pir->ir_attr = 0;
		}
		break;

// Other subfunctions are not implemented.

	case SET_ATTRIBUTES:
	case GET_ATTRIB_COMP_FILESIZE:
	case SET_ATTRIB_MODIFY_DATETIME:
	case GET_ATTRIB_MODIFY_DATETIME:
	case SET_ATTRIB_LAST_ACCESS_DATETIME:
	case GET_ATTRIB_LAST_ACCESS_DATETIME: 
	case SET_ATTRIB_CREATION_DATETIME:
	case GET_ATTRIB_CREATION_DATETIME:
	default:
		pir->ir_error = ERROR_BAD_COMMAND;
		DEBUGTRACE("Unsupported attribute command");
		break;
	}

	return pir->ir_error;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	VolDir
//
// Purpose
//	Several subfunctions related to directories. 
//
// Parameters
//	pir		pointer to io_req 
//	  ir_flags	subfunction code
//	  ir_ppath	unicode path 
//	  ir_attr	parsing flags
//
// Return Value
//	Returns zero on success.
//
// Remarks
//	Only the GET_ATTRIBUTES subfunction is implemented. The 
//	function calls utility routines that access the Registry
//	to determine if the given path corresponds to a key
//	(directory),  a value (file), or is not found.
//
INT VolDir(pioreq pir)
{
// Branch on subfunction

	switch (pir->ir_flags)
	{

	case CHECK_DIR:

// Set ir_error to zero if the specified directory exists, otherwise
// set it to error code.

		DEBUGTRACE("VolDir: CHECK_DIR");
		pir->ir_error = CheckDirectoryExists(pir->ir_ppath) ?
			0 : ERROR_FILE_NOT_FOUND;
		break;

	case CREATE_DIR:
		pir->ir_error = ERROR_ACCESS_DENIED; // read-only
		DEBUGTRACE("VolDir: CREATE_DIR");	
		break;

	case DELETE_DIR:
		pir->ir_error = ERROR_ACCESS_DENIED; // read-only
		DEBUGTRACE("VolDir: DELETE_DIR");
		break;

	case QUERY83_DIR:
		DEBUGTRACE("VolDir: QUERY83_DIR"); 
		pir->ir_error = -1;	// not implemented		
		break;

	case QUERYLONG_DIR:
		DEBUGTRACE("VolDir: QUERYLONG_DIR");

// This function is supposed to "canonicalize" the path. We simply
// copy the supplied path to the output if the key exists.

		if (CheckDirectoryExists(pir->ir_ppath))
			CopyParsedPath(pir->ir_ppath2, pir->ir_ppath);
		else
			pir->ir_error = ERROR_FILE_NOT_FOUND;
		break;

	default:
		DEBUGERROR("unknown directory function request");
		pir->ir_error = ERROR_BAD_COMMAND;
	}

	return pir->ir_error;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	VolInfo
//
// Purpose
//	Retrieves volume parameters
//
// Parameters
//	pir		pointer to io_req 
//
// Return Value
//	Returns zero on success.
//
// Remarks
//	See the code for the fields of the request structure
//	that are set by this service.
//
//	For the virtual drive provided by RFSD, the numbers should
//	make sense, but have no real meaning.
//
INT VolInfo(pioreq pir)
{
	DEBUGTRACE("Volume Info");

	pir->ir_length = 0x200;		// bytes per sector
	pir->ir_size = 1;		// number of clusters
	pir->ir_data = 0;		// pointer to FAT ID byte
	pir->ir_sectors = 2;		// sectors per cluster
	pir->ir_numfree = 1;		// number of free clusters

	return (pir->ir_error = 0);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	VolOpen
//
// Purpose
//	Open a file
//
// Parameters
//	pir		pointer to io_req 
//
// Return Value
//	Returns zero on success.
//
// Remarks
//	This routine calls Registry services to see if the supplied
//	filename maps to a value in the Registry. If so, it creates
//	a handle structure and initializes it. It then sets ir_fh to
//	to point to the handle structure, and sets up ir_hfunc to 
//	point to the handle functions.
//
INT VolOpen(pioreq pir)
{
	char RegPath[256];
	char ValueName[64];
	_QWORD qResult;
	HKEY RootKey, k;
	PCHAR pLastElem;
	PREGFSDHANDLE pHndl;
	ULONG ValueSize=0;
	BOOL bParseStatus;

// Uncomment this code display the path being opened to debug console
//   memset(RegPath, 0, 256);
//   UniToBCSPath(RegPath, pir->ir_ppath->pp_elements, 256, BCS_WANSI, 
//     &qResult);
//   dprintf("Open File: flags=%x, path=|%s|\n", pir->ir_attr, RegPath);


	pir->ir_error = ERROR_FILE_NOT_FOUND;	// assume error

// Parse the supplied unicode path into a root key, subkey, and value.

	bParseStatus =  ParseOpenPath(pir->ir_ppath, &RootKey, RegPath, 
			ValueName, sizeof(ValueName));

// If a valid root key was found, open the key and subkey.

	if (bParseStatus)		
		if (RegOpenKey(RootKey, RegPath, &k)==ERROR_SUCCESS)

// If the open of the key is successful, see if the value exists

			if (RegQueryValueEx(k, ValueName,0, 0, 0, &ValueSize) 
				== ERROR_SUCCESS)
			{

// If the value exists, alloc and init a handle structure

				pHndl = (PREGFSDHANDLE)malloc(sizeof(*pHndl));
				pHndl->h_sig = SIGFILE;
				pHndl->h_key = k;
				pHndl->h_value = strdup(ValueName);
				pHndl->h_size = ValueSize;
				pir->ir_error = 0;
			}

// If successful, return a pointer to the handle struct in ir_fh, and set
// up the handle function pointers.

	if (pir->ir_error == 0)
	{
		pir->ir_fh = pHndl;
		pir->ir_hfunc->hf_read = HandleRead;
		pir->ir_hfunc->hf_write = HandleWrite;	
		pir->ir_hfunc->hf_misc = &HandleFunctions;
	}	

	return pir->ir_error;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	VolQuery
//
// Purpose
//	Returns information about the file system
//
// Parameters
//	pir		pointer to io_req 
//
// Return Value
//	Returns zero on success.
//
// Remarks
//	Refer to the code for meanings of returned fields in the
//	request structure.
//
INT VolQuery(pioreq pir)
{
	DEBUGTRACE("Volume Query");

// Local FSDs (as opposed to network) need respond only when ir_options
// is equal to two.

	if (pir->ir_options == 2)
	{
	// the low word of ir_length is set to the max size of a single path component.
	// the high word of ir_length is set to the max size of an cntire path.

		pir->ir_length = 64 | (260 << 16);	
		pir->ir_options = FS_VOL_SUPPORTS_LONG_NAMES | 
				  FS_CASE_IS_PRESERVED;
		pir->ir_pos = 0;
		pir->ir_error = 0;
	}
	else
		pir->ir_error = ERROR_INVALID_FUNCTION;

	return pir->ir_error;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	VolIoctl16
//
// Purpose
//	Process IOCTL requests
//
// Parameters
//	pir		pointer to io_req 
//
// Return Value
//	Returns zero on success.
//
// Remarks
//	The IOCTLs correspond to INT 21h AH=44h. It's not known which of
//	these are critical to the operation of RFSD, but this minimum
//	set seems to be sufficient.
//
INT VolIoctl16(pioreq pir)
{
	PDEVICEPARAMS pDP;
	PCLIENT_STRUCT pRegs;
	static MEDIAID Mid = {
		0, 		// info level
		1, 		// serial number
		{'R','e','g','i','s','t','r','y','F','S','D'},	// volume label
		{'F','A','T','1','6',' ',' ',' '}
	};
	PMEDIAID pMid;

// name "pRegs" enables us to use register abbreviation macros, e.g. _clientCL
	pRegs = (PCLIENT_STRUCT)pir->ir_cregptr;
		

// The buffer is either in ir_data or in client ds:dx. An options bit is
// queried to make this determination (IOCTL_PKT_LINEAR_ADDRESS).

	pir->ir_error = -1;		// assume error

	switch (pir->ir_flags)	
	{
	case 0:				// AX=4400h File or device
		_clientDX = Drive;	// say we are a device
		_clientFlags &= ~1;	// clear carry
		pir->ir_error = 0;	// success
		break;
	case 8:				// AX=4408h Check removable
		_clientAX = 1;		// not removable
		_clientFlags &= ~1;	// clear carry
		pir->ir_error = 0;	// success
		break;
	case 0xd:			// AX=440Dh

		switch ( _clientCL )	// branch on subfunction
		{
		case 0x60: // get device params
			pDP = (pir->ir_options & IOCTL_PKT_LINEAR_ADDRESS) ?
				(PDEVICEPARAMS)pir->ir_data :
				(PDEVICEPARAMS)Map_Flat(CLIENT_DS, CLIENT_DX);
			IoctlGetDeviceParams(pDP);
			_clientFlags &= ~1;
			pir->ir_error = 0;
			break;

		case 0x66: // get media id
			pMid = (pir->ir_options & IOCTL_PKT_LINEAR_ADDRESS) ?
				(PMEDIAID)pir->ir_data :
				(PMEDIAID)Map_Flat(CLIENT_DS, CLIENT_DX);
				
			memcpy(pMid, &Mid, sizeof(Mid));
			_clientFlags &= ~1;
			pir->ir_error = 0;
			break;	

		default:	
			DEBUGWARN("Unhandled 440d ioctl16 subfunction");
		}
		break;

	default:
			DEBUGWARN("Unhandled ioctl16 function");
	}

	return pir->ir_error;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	VolFindOpen
//
// Purpose
//	Initiates a find first/ find next operation
//
// Parameters
//	pir		pointer to io_req 
//	  ir_ppath	path name
//	  ir_attr	parsing flags, match attributes
//
// Return Value
//	Returns zero on success.
//
// Remarks
//	The IFSMgr calls this routine to retrieve file/directory information
//	for a particular path. The supplied path may include wild cards. It
//	is strongly recommended to rely on the IFSMgr's wild card matching
//	routines so that all FSDs behave the same.
//	
//	The first call in returns the first file or directory found, and also
//	returns a find handle in ir_fh. The IFSMgr passes this back to the
//	FindNext routine. When this call succeeds, it must supply the address
//	of the FindNext procedure in the ir_hfunc structure.
//
//	Match attributes are not fully implemented in RFSD.
//
INT VolFindOpen(pioreq pir)
{
	char Subkey[256];
	PFINDCONTEXT pCtx = NULL;
	HKEY RootKey = 0;
	PCHAR p;
	INT MatchElem, i;
	PathElement* pMatch;
	_QWORD qResult;
	struct _WIN32_FIND_DATA* pFile;
	short e;

// Uncomment this code to dump the path and other params to the debug console
//    memset(Subkey, 0, 256);
//    UniToBCSPath(Subkey, pir->ir_ppath->pp_elements, 256, 
//		BCS_WANSI, &qResult);
//    dprintf("Find Open: flags=%x, path=|%s|\n", pir->ir_attr, Subkey);
//
	pir->ir_error = 0;		// assume success
	pir->ir_fh = 0;

// pFile points to the structure that  receives the file information
	pFile = (struct _WIN32_FIND_DATA*)pir->ir_data;

// Special case: IFSMgr wants the volue label
	if (pir->ir_attr & FILE_ATTRIBUTE_LABEL)
	{
		BCSToUni(pFile->cFileName,"Registry", 9,BCS_WANSI, &qResult);
		pir->ir_fh = 0;
		pFile->dwFileAttributes = FILE_ATTRIBUTE_LABEL; 
		ZeroTime(&pFile->ftCreationTime);
		ZeroTime(&pFile->ftLastAccessTime);
		ZeroTime(&pFile->ftLastWriteTime);
		PutShortName(pFile->cAlternateFileName, 0, 0);
		pir->ir_hfunc->hf_read = FindNext;
		pir->ir_hfunc->hf_write = HandleWrite;	
		pir->ir_hfunc->hf_misc = &HandleFunctions;

		return 0;
	}

// Parse the supplied path. There must be a valid root key (e.g.
//  HKEY_LOCAL_MACHINE). 

	if (ParseFindPath(pir->ir_ppath, &RootKey, Subkey, &pMatch))
	{
// If parse is successful, allocate memory for a find handle

		pCtx = (PFINDCONTEXT)malloc(sizeof(FINDCONTEXT));
		if (pCtx == 0)
			pir->ir_error = ERROR_FILE_NOT_FOUND;

// Special case: input string has no root key (e.g. "*.*")
		else if (RootKey == REGISTRY_ROOT)
			pCtx->fc_hkey = REGISTRY_ROOT;

// Verify that the leading part of the path is a valid key (directory)
		else if (_RegOpenKey(RootKey, Subkey, &pCtx->fc_hkey) 
				!= ERROR_SUCCESS)
			pir->ir_error = -1;
	}
	else
		pir->ir_error = ERROR_FILE_NOT_FOUND;	// parse failed

// If ir_error is still OK, we have a valid key. Now we can begin the 
// matching proces.

	if (pir->ir_error == 0)
	{

// initialize the find handle

		pCtx->fc_sig = SIGFIND;	
		pCtx->fc_index = 0;
		pCtx->fc_bKeysDone = FALSE;

// copy the metamatch info to the find handle
		if (pMatch->pe_length != 0)
		{
			pCtx->fc_match = malloc(pMatch->pe_length);
			memcpy(pCtx->fc_match, pMatch->pe_unichars, pMatch->pe_length);
			pCtx->fc_match[pMatch->pe_length/sizeof(WORD)-1] = 0;	
		}
		else
			pCtx->fc_match = NULL;

// set ir_fh to the find handle and point to the handle functions

		pir->ir_fh = pCtx;
		pir->ir_hfunc->hf_read = FindNext; // this is the key one now
		pir->ir_hfunc->hf_write = HandleWrite;	
		pir->ir_hfunc->hf_misc = &HandleFunctions;

// The FILE_FLAG_IS_LFN seems to be a hint that the IFSMgr just wants to
// verify that the supplied path is a directory. We return the supplied
// name as the first find.

		if (pir->ir_attr & FILE_FLAG_IS_LFN)
		{
			if (pMatch->pe_length != 0)
				memcpy(pFile->cFileName, 
				  pMatch->pe_unichars, 
				  pMatch->pe_length);
			else
				memcpy(pFile->cFileName, 
	   			  pir->ir_ppath->pp_elements[0].pe_unichars, 
				  pir->ir_ppath->pp_elements[0].pe_length);

			pFile->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;

			PutShortName(pFile->cAlternateFileName, 
				pCtx->fc_hkey, pCtx->fc_index);
		}

		else
// Normal case: get first file by calling find next routine
			FindNext(pir);

		if (pir->ir_error != 0)
		{
		// save error code because HandleClose clears it
			e = pir->ir_error; 
			HandleClose(pir);
			pir->ir_error = e;
			pir->ir_fh = 0;
		}
	}
	else if (pCtx)
		free(pCtx);

	return pir->ir_error;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	FindNext
//
// Purpose
//	Continues a find first/ find next operation
//
// Parameters
//	pir		pointer to io_req 
//	  ir_ppath	path name
//	  ir_attr	parsing flags, match attributes
//	  ir_fh		find handle (context)
//
// Return Value
//	Returns zero on success.
//
// Remarks
//	This call is always preceded by a call to VolFindOpen which sets up
// 	ir_fh to point to a find handle (FINDCONTEXT). This data structure
//	is a private data structure for RFSD, not a standard IFSMgr struct.
//	Each FSD implements whatever it deems necessary to find the next
//	file. The Registry calls make this very easy, because they are
//	accessed by index. The FINDCONTEXT tracks whether the keys have
//	all been enumerated. After the keys are enumerated, it then
//	begin enumerating the values.
//
int __cdecl FindNext(pioreq pir)
{
	PFINDCONTEXT pCtx;
	struct _WIN32_FIND_DATA* pFile;
	_QWORD qResult;
	CHAR KeyName[128];
	DWORD ValueLength, ValueType;
	DWORD DataLength;

	pir->ir_error = ERROR_FILE_NOT_FOUND;	// assume no more files

// Get the context from the previous call and validate it
	pCtx = (PFINDCONTEXT)pir->ir_fh;
	if (pCtx == 0)	
		return pir->ir_error;
	Assert(pCtx->fc_sig == SIGFIND);

// get a pointer to the structure to hold the retured file information
	pFile= (struct _WIN32_FIND_DATA*)pir->ir_data;

// When the key is the root, we enumerate the predefined keys against the 
// matching specification

	if (pCtx->fc_hkey == REGISTRY_ROOT)
	{
		while (pCtx->fc_index < NPREDEFKEYS)
		{
			BCSToUni(pFile->cFileName, 
			  PredefinedRegKeyStrings[pCtx->fc_index],
			  strlen(PredefinedRegKeyStrings[pCtx->fc_index])+1, 
			  BCS_WANSI, &qResult);

			pCtx->fc_index++;

			if ((pCtx->fc_match == NULL) || 
			     IFSMgr_MetaMatch(pCtx->fc_match, 
			       pFile->cFileName, UFLG_NT_DOS | UFLG_META))  
			{
				pFile->dwFileAttributes = 
					FILE_ATTRIBUTE_DIRECTORY;
				PutShortName(pFile->cAlternateFileName, 
						pCtx->fc_hkey, pCtx->fc_index);
				pir->ir_error = 0;
				break;
			}
		}
	}
	else
	{

// This clause handles the normal case, when valid root key (HKEY_xxx) was
// supplied. Enumerate the keys and test them against the matching criteria.

		while (!pCtx->fc_bKeysDone && 
			(RegEnumKey(pCtx->fc_hkey,
				    pCtx->fc_index,
				    KeyName,
				    sizeof(KeyName)-1) == ERROR_SUCCESS))
		{
			BCSToUni(pFile->cFileName, KeyName, 
				  strlen(KeyName)+1, BCS_WANSI, &qResult);

			pCtx->fc_index++;

			if ((pCtx->fc_match == NULL) || 
			     IFSMgr_MetaMatch(pCtx->fc_match, 
			                      pFile->cFileName, 
			                      UFLG_NT_DOS|UFLG_META))  
			{
				pFile->dwFileAttributes = 
					FILE_ATTRIBUTE_DIRECTORY;
				PutShortName(pFile->cAlternateFileName, 
					pCtx->fc_hkey, pCtx->fc_index);

				pir->ir_error = 0;
				break;
			}	
		}

// If all of the keys have been enumerated, then start enumerating the values.

		if (pir->ir_error != 0)
		{
			if (!pCtx->fc_bKeysDone)
			{
				pCtx->fc_bKeysDone = TRUE;
				pCtx->fc_index = 0;
			}

			ValueLength = sizeof(KeyName);

			while (RegEnumValue(pCtx->fc_hkey, 
				            pCtx->fc_index, 
				            KeyName, 
				            &ValueLength,
					    NULL, 
					    &ValueType, 
					    NULL, 
					    &DataLength) == ERROR_SUCCESS)	
			{
				BCSToUni(pFile->cFileName, KeyName, 
				  strlen(KeyName)+1, BCS_WANSI, &qResult);

				pCtx->fc_index++;

				if ((pCtx->fc_match == NULL) || 
				     IFSMgr_MetaMatch(pCtx->fc_match, 
				                      pFile->cFileName, 
				                      UFLG_NT_DOS | UFLG_META))  
				{
					pFile->dwFileAttributes = 
						FILE_ATTRIBUTE_READONLY;
					pFile->nFileSizeLow = DataLength;	
					pFile->nFileSizeHigh = 0;
					PutShortName(pFile->cAlternateFileName,
						pCtx->fc_hkey, pCtx->fc_index);
					ZeroTime(&pFile->ftCreationTime);
					ZeroTime(&pFile->ftLastAccessTime);
					ZeroTime(&pFile->ftLastWriteTime);
					pir->ir_pos = (ULONG)pCtx;
					pir->ir_error = 0;
					break;
				}	
				ValueLength = sizeof(KeyName);
			}
		}		
	}
	return pir->ir_error;	
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	HandleRead
//
// Purpose
//	Handles read requests to a previously opened file
//
// Parameters
//	pir		pointer to io_req 
//	  ir_fh		file handle
//
// Return Value
//	Returns zero on success.
//
// Remarks
//	Although we could just call the Registry services, we act more like
// 	a real FSD and call the IOS to do the read.
//
int __cdecl HandleRead(pioreq pir)
{
	PREGFSDHANDLE h = (PREGFSDHANDLE)pir->ir_fh;
	WORD result;
	IOP* pIop; 
	IOR* pIor;
	VRP* pVrp;

	pir->ir_error = -1;		// assume error
	Assert(h->h_sig == SIGFILE);	// paranoid check
	if (h->h_sig != SIGFILE)
		return (pir->ir_error = ERROR_INVALID_HANDLE);

// Allocate an IOP to send the read request on
	pIop = GetIop(REGFSD_Ilb.ILB_service_rtn, sizeof(IOP), IORDELTA, 
			0, &result);
	if ( (pIop == NULL) || (result != 0) )
		return pir->ir_error;

	pIor = &pIop->IOP_ior;	// point to IOR portion of IOP

// Set up the IOR

	pIor->IOR_func 		= IOR_READ;
	pIor->IOR_flags 	= IORF_SYNC_COMMAND | IORF_VERSION_002;
	pIor->IOR_xfer_count 	=  pir->ir_length;
	pIor->IOR_start_addr[0] = (ULONG)h->h_key;
	pIor->IOR_start_addr[1] = (ULONG)h->h_value;
	pIor->IOR_private_port 	= (ULONG)h->h_pos;
	pIor->IOR_buffer_ptr 	= (ULONG)pir->ir_data;
	pIor->IOR_vol_designtr 	= Drive;
	pIor->IOR_req_vol_handle= (ULONG)VolumeHandle;
	pIor->IOR_num_sgds 	= 0;

// Send the IOP to the IOS. This is a synchronous request.

	IOS_SendCommand(pIor, pDcbOfThisDevice);

	if (pIor->IOR_status == IORS_SUCCESS) // Set error status
		pir->ir_error = 0;

// Free the IOP

	ReleaseIop(REGFSD_Ilb.ILB_service_rtn, pIop);

	return pir->ir_error;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	HandleSeek
//
// Purpose
//	Handles seek requests to a previously opened file
//
// Parameters
//	pir		pointer to io_req 
//	  ir_fh		file handle  
//	  ir_flags	indicates seek from start or end
//	  ir_pos	signed offset
//
// Return Value
//	Returns zero on success.
//
// Remarks
//	This function only affects the h_pos field of the file
// 	handle.
//
int __cdecl HandleSeek(pioreq pir)
{
	PREGFSDHANDLE h = (PREGFSDHANDLE)pir->ir_fh;

	Assert(h->h_sig == SIGFILE);	// paranoid check
	if (h->h_sig != SIGFILE)
		return (pir->ir_error = ERROR_INVALID_HANDLE);

	switch (pir->ir_flags)
	{
	case FILE_BEGIN:	// seek from start
		pir->ir_pos = min(pir->ir_pos, h->h_size);
		break;

	case FILE_END:		// seek from end
		if (pir->ir_pos > 0)	
			pir->ir_pos = h->h_size;
		else
			pir->ir_pos = h->h_size + pir->ir_pos;
	}

	h->h_pos = pir->ir_pos;
		
	return (pir->ir_error=0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	HandleClose
//
// Purpose
//	Closes a previously opened file
//
// Parameters
//	pir		pointer to io_req 
//	  ir_fh		file handle  
//
// Return Value
//	Returns zero on success.
//
// Remarks
//	This function closes both FindFirst handles and OpenFile handles
//
int __cdecl HandleClose(pioreq pir)
{
// Close a find first handle

	if ( ((PFINDCONTEXT)pir->ir_fh)->fc_sig == SIGFIND)
	{	
		((PFINDCONTEXT)pir->ir_fh)->fc_sig = 0;
		if (((PFINDCONTEXT)pir->ir_fh)->fc_match != NULL)
			free( ((PFINDCONTEXT)pir->ir_fh)->fc_match);
		free(pir->ir_fh);
	}	

// Close an open file handle

	else if ( ((PREGFSDHANDLE)pir->ir_fh)->h_sig == SIGFILE)
	{
		((PFINDCONTEXT)pir->ir_fh)->fc_sig = 0;
		if ( ((PREGFSDHANDLE)pir->ir_fh)->h_value )
			free( ((PREGFSDHANDLE)pir->ir_fh)->h_value);
		free(pir->ir_fh);
	}
	else
		DEBUGWARN("HandleClose: bad handle or already closed");

	return (pir->ir_error=0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
// The remaining functions in this module have no effect or are 
// unimplemented in the Registry File System:

INT VolRename(pioreq pir)
{
	return (pir->ir_error = ERROR_ACCESS_DENIED);
}

INT VolSearch(pioreq pir)
{
	return (pir->ir_error = ERROR_BAD_COMMAND);
}

INT VolDelete(pioreq pir)
{
	DEBUGTRACE("Volume Delete");
	return (pir->ir_error = 0);
}

INT Vol_Flush(pioreq pir)
{
	DEBUGTRACE("Volume Flush");
	return 0;
}

INT VolDisconnect(pioreq pir)
{
	DEBUGTRACE("Volume Disconnect");
	return (pir->ir_error = 0);
}

INT VolUNCPipe(pioreq pir)
{
	DEBUGTRACE("Volume UNC Pipe");
	return (pir->ir_error = 0);
}

INT VolParams(pioreq pir)
{
	DEBUGTRACE("Volume Params");
	return (pir->ir_error = 0);
}

INT VolDasdio(pioreq pir)
{
	DEBUGTRACE("Volume dasdio");
	return (pir->ir_error = 0);
}

INT HandleWrite(pioreq pir)
{
	DEBUGTRACE("Handle Write");
	return (pir->ir_error = ERROR_ACCESS_DENIED);
}

INT HandleCommit(pioreq pir)
{
	DEBUGTRACE("Handle Commit");
	return (pir->ir_error=ERROR_ACCESS_DENIED);
}

INT HandleFilelocks(pioreq pir)
{
	DEBUGTRACE("Handle Filelocks");
	return (pir->ir_error=ERROR_ACCESS_DENIED);
}

INT HandleFiletimes(pioreq pir)
{
	DEBUGTRACE("Handle Filetimes");
	return (pir->ir_error=ERROR_ACCESS_DENIED);
}

INT HandlePiperequest(pioreq pir)
{
	DEBUGTRACE("Handle Piperequest");
	return (pir->ir_error=ERROR_ACCESS_DENIED);
}

INT HandleHandleinfo(pioreq pir)
{
	DEBUGTRACE("Handle info");
	return (pir->ir_error=ERROR_ACCESS_DENIED);
}

INT HandleEnumhandle(pioreq pir)
{
	DEBUGTRACE("Handle Enumhandle");
	return (pir->ir_error=ERROR_ACCESS_DENIED);
}

