#ifndef _SHSEMIP_H_
#define _SHSEMIP_H_

//
// Define API decoration for direct importing of DLL references.
//
#ifndef RC_INVOKED
#pragma pack(1)         /* Assume byte packing throughout */
#endif /* !RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */


#ifndef DONT_WANT_SHELLDEBUG
    
#ifndef DebugMsg                                                                /* ;Internal */
#define DM_TRACE    0x0001      // Trace messages                               /* ;Internal */
#define DM_WARNING  0x0002      // Warning                                      /* ;Internal */
#define DM_ERROR    0x0004      // Error                                        /* ;Internal */
#define DM_ASSERT   0x0008      // Assertions                                   /* ;Internal */
#define Assert(f)                                                               /* ;Internal */
#define AssertE(f)      (f)                                                     /* ;Internal */
#define AssertMsg   1 ? (void)0 : (void)                                        /* ;Internal */
#define DebugMsg    1 ? (void)0 : (void)                                        /* ;Internal */
#endif  // DebugMsg                                                             /* ;Internal */
                                                                                /* ;Internal */
#endif  // DONT_WANT_SHELLDEBUG
    
typedef void (WINAPI FAR* RUNDLLPROC)(HWND hwndStub,                      
        HINSTANCE hAppInstance,                                           
        LPSTR lpszCmdLine, int nCmdShow);                                 

//======================================================================= 
// String constants for                                                   
//  1. Registration database keywords       (prefix STRREG_)              
//  2. Exported functions from handler dlls (prefix STREXP_)              
//  3. .INI file keywords                   (prefix STRINI_)              
//  4. Others                               (prefix STR_)                 
//======================================================================= 
#define STRREG_SHELLUI          "ShellUIHandler"                          
#define STRREG_SHELL            "Shell"                                   
#define STRREG_DEFICON          "DefaultIcon"                             
#define STRREG_SHEX             "shellex"                                
#define STRREG_SHEX_PROPSHEET   STRREG_SHEX "\\PropertySheetHandlers"     
#define STRREG_SHEX_DDHANDLER   STRREG_SHEX "\\DragDropHandlers"              
#define STRREG_SHEX_MENUHANDLER STRREG_SHEX "\\ContextMenuHandlers"           
#define STRREG_SHEX_COPYHOOK    "Directory\\" STRREG_SHEX "\\CopyHookHandlers"
#define STRREG_SHEX_PRNCOPYHOOK "Printers\\" STRREG_SHEX "\\CopyHookHandlers" 
                                                                         
#define STREXP_CANUNLOAD        "DllCanUnloadNow"       // From OLE 2.0  
                                                                         
#define STRINI_CLASSINFO        ".ShellClassInfo"       // secton name   
#define STRINI_SHELLUI          "ShellUIHandler"                         
#define STRINI_OPENDIRICON      "OpenDirIcon"                            
#define STRINI_DIRICON          "DirIcon"                                
                                                                         
#define STR_DESKTOPINI          "desktop.ini"                            
                                                                         
// Maximum length of a path string
#define CCHPATHMAX      MAX_PATH
#define MAXSPECLEN      MAX_PATH
#define DRIVEID(path)   ((path[0] - 'A') & 31)
#define ARRAYSIZE(a)    (sizeof(a)/sizeof(a[0]))


#define PATH_CCH_EXT    64
// PathResolve flags							
#define PRF_VERIFYEXISTS	    0x0001				
#define PRF_TRYPROGRAMEXTENSIONS    (0x0002 | PRF_VERIFYEXISTS)		
#define PRF_FIRSTDIRDEF		    0x0004
#define PRF_DONTFINDLNK		    0x0008	// if PRF_TRYPROGRAMEXTENSIONS is specified




//
// For CallCPLEntry16
//
DECLARE_HANDLE(FARPROC16);

// Needed for RunFileDlg
#define RFD_NOBROWSE		0x00000001
#define RFD_NODEFFILE		0x00000002
#define RFD_USEFULLPATHDIR	0x00000004
#define RFD_NOSHOWOPEN          0x00000008

#ifdef RFN_FIRST
#define RFN_EXECUTE             (RFN_FIRST - 0)
typedef struct {
    NMHDR hdr;
    LPCSTR lpszCmd;
    LPCSTR lpszWorkingDir;
    int nShowCmd;
} NMRUNFILE, FAR *LPNMRUNFILE;
#endif

// RUN FILE RETURN values from notify message
#define RFR_NOTHANDLED 0
#define RFR_SUCCESS 1
#define RFR_FAILURE 2



 int WINAPI DriveType(int iDrive);
 int WINAPI RestartDialog(HWND hwnd, LPCSTR lpPrompt, DWORD dwReturn);
 int WINAPI PickIconDlg(HWND hwnd, LPSTR pszIconPath, UINT cbIconPath, int FAR *piIconIndex);


//===================================================================
// Shell_MergeMenu parameter
//
#define MM_ADDSEPARATOR		0x00000001L
#define MM_SUBMENUSHAVEIDS	0x00000002L

//-------- drive type identification --------------
// iDrive      drive index (0=A, 1=B, ...)
//
#define DRIVE_CDROM     5           // extended DriveType() types
#define DRIVE_RAMDRIVE  6
#define DRIVE_TYPE      0x000F      // type masek
#define DRIVE_SLOW      0x0010      // drive is on a slow link
#define DRIVE_LFN       0x0020      // drive supports LFNs
#define DRIVE_AUTORUN   0x0040      // drive has AutoRun.inf in root.
#define DRIVE_AUDIOCD   0x0080      // drive is a AudioCD
#define DRIVE_AUTOOPEN  0x0100      // should *always* auto open on insert
#define DRIVE_NETUNAVAIL 0x0200     // Network drive that is not available
#define DRIVE_SHELLOPEN  0x0400     // should auto open on insert, if shell has focus

#define DriveTypeFlags(iDrive)      DriveType('A' + (iDrive))
#define DriveIsSlow(iDrive)         (DriveTypeFlags(iDrive) & DRIVE_SLOW)
#define DriveIsLFN(iDrive)          (DriveTypeFlags(iDrive) & DRIVE_LFN)
#define DriveIsAutoRun(iDrive)      (DriveTypeFlags(iDrive) & DRIVE_AUTORUN)
#define DriveIsAutoOpen(iDrive)     (DriveTypeFlags(iDrive) & DRIVE_AUTOOPEN)
#define DriveIsShellOpen(iDrive)    (DriveTypeFlags(iDrive) & DRIVE_SHELLOPEN)
#define DriveIsAudioCD(iDrive)      (DriveTypeFlags(iDrive) & DRIVE_AUDIOCD)
#define DriveIsNetUnAvail(iDrive)   (DriveTypeFlags(iDrive) & DRIVE_NETUNAVAIL)

#define IsCDRomDrive(iDrive)        (DriveType(iDrive) == DRIVE_CDROM)
#define IsRamDrive(iDrive)          (DriveType(iDrive) == DRIVE_RAMDRIVE)
#define IsRemovableDrive(iDrive)    (DriveType(iDrive) == DRIVE_REMOVABLE)
#define IsRemoteDrive(iDrive)       (DriveType(iDrive) == DRIVE_REMOTE)


//-------- file engine stuff ----------

// "current directory" management routines.  used to set parameters
// that paths are qualfied against in MoveCopyDeleteRename()

 int  WINAPI GetDefaultDrive();
 int  WINAPI SetDefaultDrive(int iDrive);
 int  WINAPI SetDefaultDirectory(LPCSTR lpPath);
 void WINAPI GetDefaultDirectory(int iDrive, LPSTR lpPath);

typedef enum {
        PARSEFILENAME_QUALIFY = 1,
} PARSEFILENAME_FLAGS;

// this parses a list of file names (like you'd get from an edit control)
// into a double null terminated list appropriate for passing to the file
// engine or drag/drop.  this accounts for excape sequences and strings
// in quotes

 LPSTR WINAPI ParseFilenames(LPCSTR lpNames, PARSEFILENAME_FLAGS fFlags);

// this can be used to walk through a string of file names, dealing with
// seperators and escapes

 LPSTR WINAPI GetNextFile(LPCSTR pFrom, LPSTR pTo, int cbMax);

// NOTES: No reason to have this one here, but I don't want to break the build.
//
#ifndef WINCOMMCTRLAPI
int WINAPI StrToInt(LPCSTR lpSrc);  // atoi()
#endif

#define POSINVALID  32767       // values for invalid position

#define IDCMD_SYSTEMFIRST       0x8000
#define IDCMD_SYSTEMLAST        0xbfff
#define IDCMD_CANCELED          0xbfff
#define IDCMD_PROCESSED         0xbffe
#define IDCMD_DEFAULT           0xbffe


/*
 * The SHFormatDrive API provides access to the Shell
 *   format dialog. This allows apps which want to format disks
 *   to bring up the same dialog that the Shell does to do it.
 *
 *   This dialog is not sub-classable. You cannot put custom
 *   controls in it. If you want this ability, you will have
 *   to write your own front end for the DMaint_FormatDrive
 *   engine.
 *
 *   NOTE that the user can format as many diskettes in the specified
 *   drive, or as many times, as he/she wishes to. There is no way to
 *   force any specififc number of disks to format. If you want this
 *   ability, you will have to write your own front end for the
 *   DMaint_FormatDrive engine.
 *
 *   NOTE also that the format will not start till the user pushes the
 *   start button in the dialog. There is no way to do auto start. If
 *   you want this ability, you will have to write your own front end
 *   for the DMaint_FormatDrive engine.
 *
 *   PARAMETERS
 *
 *     hwnd    = The window handle of the window which will own the dialog
 *		 NOTE that unlike SHCheckDrive, hwnd == NULL does not cause
 *		 this dialog to come up as a "top level application" window.
 *		 This parameter should always be non-null, this dialog is
 *		 only designed to be the child of another window, not a
 *		 stand-alone application.
 *     drive   = The 0 based (A: == 0) drive number of the drive to format
 *     fmtID   = The ID of the physical format to format the disk with
 *		 NOTE: The special value SHFMT_ID_DEFAULT means "use the
 *		       default format specified by the DMaint_FormatDrive
 *		       engine". If you want to FORCE a particular format
 *		       ID "up front" you will have to call
 *		       DMaint_GetFormatOptions yourself before calling
 *		       this to obtain the valid list of phys format IDs
 *		       (contents of the PhysFmtIDList array in the
 *		       FMTINFOSTRUCT).
 *     options = There is currently only two option bits defined
 *
 *		  SHFMT_OPT_FULL
 *                SHFMT_OPT_SYSONLY
 *
 *		 The normal defualt in the Shell format dialog is
 *		 "Quick Format", setting this option bit indicates that
 *		 the caller wants to start with FULL format selected
 *		 (this is useful for folks detecting "unformatted" disks
 *		 and wanting to bring up the format dialog).
 *
 *               The SHFMT_OPT_SYSONLY initializes the dialog to
 *               default to just sys the disk.
 *
 *		 All other bits are reserved for future expansion and
 *		 must be 0.
 *
 *		 Please note that this is a bit field and not a value
 *		 and treat it accordingly.
 *
 *   RETURN
 *	The return is either one of the SHFMT_* values, or if the
 *	returned DWORD value is not == to one of these values, then
 *	the return is the physical format ID of the last succesful
 *	format. The LOWORD of this value can be passed on subsequent
 *	calls as the fmtID parameter to "format the same type you did
 *	last time".
 *
 */
DWORD WINAPI SHFormatDrive(HWND hwnd, UINT drive, UINT fmtID,
				 UINT options);

//
// Special value of fmtID which means "use the default format"
//
#define SHFMT_ID_DEFAULT    0xFFFF

//
// Option bits for options parameter
//
#define SHFMT_OPT_FULL     0x0001
#define SHFMT_OPT_SYSONLY  0x0002

//
// Special return values. PLEASE NOTE that these are DWORD values.
//
#define SHFMT_ERROR	0xFFFFFFFFL	// Error on last format, drive may be formatable
#define SHFMT_CANCEL	0xFFFFFFFEL	// Last format was canceled
#define SHFMT_NOFORMAT 0xFFFFFFFDL	// Drive is not formatable



/*
 * The SHCheckDrive API provides access to the Shell
 *   ScanDisk dialog. This allows apps which want to check disks
 *   to bring up the same dialog that Shell does to do it.
 *
 *   This dialog is not sub-classable. You cannot put custom
 *   controls in it. If you want this ability, you will have
 *   to write your own front end for the DMaint_FixDrive
 *   engine.
 *
 *   NOTE that the check will not start till the user pushes the
 *   start button in the dialog unless the SHCHK_OPT_AUTO option is set.
 *
 *   PARAMETERS
 *
 *     hwnd    = The window handle of the window which will own the dialog.
 *		 If this parameter is NULL it triggers the special case
 *		 "run as a top level window". The dialog will have a regular
 *		 top level application system menu, can be minimized, etc.
 *		 This is how SCANDSKW.EXE works, it just calls SHCheckDrive
 *		 with a NULL hwnd. If hwnd != NULL SHCheckDrive brings
 *		 up a regular application dialog box form.
 *     options = These options basically coorespond to the check boxes
 *		 in the Advanced Options dialog. See SHCHK_OPT_ defines
 *		 below.
 *     DrvList = This is a DWORD bit field which indicates the 0 based
 *		 drive numbers to check. Bit 0 = A, Bit 1 = B, ...
 *		 For use on this API at least one bit must be set (if
 *		 this argument is 0, the call will return SHCHK_NOCHK).
 *		 An exception: if the SHCHK_OPT_SAGESET or SHCHK_OPT_SAGERUN
 *		 bit is set in the 'options' parameter, then DrvList is
 *		 instead treated as an index into a collection of settings
 *		 which ScanDisk keeps in the registry; if the
 *		 SHCHK_OPT_SAGESET bit is set in 'options', ScanDskW will
 *		 allow the settings for the given index to be modified,
 *		 and if the SHCHK_OPT_SAGERUN bit is set, ScanDskW will
 *		 use the settings for the given index to check one or more
 *		 drives (as indicated by one of the settings for the given
 *		 index).
 *     lpHwnd  = An optional argument (can be NULL). If it is non-NULL,
 *		 SHCheckDrive will place the window handle of the
 *		 window it creates in this location.
 *
 *   RETURN
 *	The return is one of the SHCHK_* values.
 *
 */
DWORD WINAPI SHCheckDrive(HWND hwnd, DWORD options, DWORD DrvList, HWND FAR* lpHwnd);

//
// Special return values. PLEASE NOTE that these are DWORD values.
//
#define SHCHK_ERROR	0xFFFFFFFFL	// Fatal Error on check
#define SHCHK_CANCEL	0xFFFFFFFEL	// Check was canceled
#define SHCHK_NOCHK	0xFFFFFFFDL	// At least one Drive is not "checkable"
					//   or no drives were specified
#define SHCHK_SMNOTFIX	0xFFFFFFFCL	// Some errors were not fixed
#define SHCHK_ERRORMEM	0xFFFFFFFBL	// Couldn't alloc memory to start check
#define SHCHK_ERRORINIT 0xFFFFFFFAL	// Couldn't access DSKMAINT.DLL
#define SHCHK_RERUN     0xFFFFFFF9L	// Couldn't check all drives (for sage)
#define SHCHK_NOERROR	0x00000000L	// No errors on drive
#define SHCHK_ALLFIXED	0x00000001L	// Errors on drive, all were fixed
					// NOTE: This is not sensitive to
					//	 SHCHK_OPT_RO being set

//
// Option bits
//
// IMPORTANT NOTE: These are set up so that the default setting is 0
//		   for all bits WITH TWO EXCEPTIONS. Currently the default
//		   setting has the SHCHK_OPT_XLCPY and SHCHK_OPT_LSTMF
//		   bits set......
//
// Also note that specification of invalid combinations of bits (for example
// setting both SHCHK_OPT_XLCPY and SHCHK_OPT_XLDEL) will result in very random
// behavior.
//
#define SHCHK_OPT_REP		0x00000001L  // Generate detail report
#define SHCHK_OPT_RO		0x00000002L  // Run in preview mode
#define SHCHK_OPT_NOSYS 	0x00000004L  // Surf Anal don't check system area
#define SHCHK_OPT_NODATA	0x00000008L  // Surf Anal don't check data area
#define SHCHK_OPT_NOBAD 	0x00000010L  // Disable Surface Analysis
#define SHCHK_OPT_LSTMF 	0x00000020L  // Convert lost clusters to files
#define SHCHK_OPT_NOCHKNM	0x00000040L  // Don't check file names
#define SHCHK_OPT_NOCHKDT	0x00000080L  // DOn't check date/time fields
#define SHCHK_OPT_INTER 	0x00000100L  // Interactive mode
#define SHCHK_OPT_XLCPY 	0x00000200L  // Def cross link resolution is COPY
#define SHCHK_OPT_XLDEL 	0x00000400L  // Def cross link resolution is DELETE
#define SHCHK_OPT_ALLHIDSYS	0x00000800L  // All HID SYS files are unmovable
#define SHCHK_OPT_NOWRTTST	0x00001000L  // Surf Anal no write testing.
#define SHCHK_OPT_DEFOPTIONS	0x00002000L  // Get above options from registry
#define SHCHK_OPT_DRVLISTONLY	0x00004000L  // Normaly all drives in the system
					     // are shown in the drive list box
					     // and those on the DrvList are selected
					     // This option says put only the drives
					     // in DrvList in the list box and
					     // disable the control
#define SHCHK_OPT_AUTO		0x00008000L  // Auto push start button
#define SHCHK_OPT_EXCLULOCK	0x00010000L  // Exclusive lock drive
#define SHCHK_OPT_FLWRTLOCK	0x00020000L  // Allow RD fail write
#define SHCHK_OPT_MKOLDFS	0x00040000L  // Remove new FS stuff
					     // WARNING: This function is
					     // reserved for SETUP, specifying
					     // This will TURN OFF the Read
					     // Only (preview), Interactive
					     // and report settings, and turn
					     // off surface analysis
					     // automatically.....
#define SHCHK_OPT_PROGONLY	0x00080000L  // Put up front end dialog with
					     // the progress bar, but run
					     // "silently", no errors or
					     // warnings...
#define SHCHK_OPT_NOWND 	0x00100000L  // No window at all and run
					     // silently as above...
#define SHCHK_OPT_NOCHKHST	0x00200000L  // By default a check of the
					     // host drive of compresssed
					     // volumes is done first as
					     // part of the check of the
					     // compressed drive. This
					     // option disables this.
#define SHCHK_OPT_REPONLYERR	0x00400000L  // Report only if error
#define SHCHK_OPT_NOLOG 	0x00800000L  // No logging
#define SHCHK_OPT_LOGAPPEND	0x01000000L  // Append to log
#define SHCHK_OPT_MINIMIZED	0x02000000L  // Allows top level window
					     // to be started minimized
					     // this option is valid only
					     // when the hwnd parameter is
					     // NULL
#define SHCHK_OPT_SAGESET	0x04000000L  // Specifies that the given
					     // settings should be configured
#define SHCHK_OPT_SAGERUN	0x08000000L  // Specifies that the given
					     // settings should be used to
					     // check the appropriate drives

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#ifndef RC_INVOKED
#pragma pack()
#endif  /* !RC_INVOKED */

#endif // _SHSEMIP_H_
