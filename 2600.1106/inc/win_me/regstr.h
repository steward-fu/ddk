/*** regstr.h - Registry string definitions
 *
 *  This module contains public registry string definitions.
 *
 *  Copyright (c) 1992-1995 Microsoft Corporation
 *  Created 12/10/92
 *
 *  MODIFICATION HISTORY
 */


#ifndef _INC_REGSTR
#define _INC_REGSTR


/*** Public registry key names
 */

#define REGSTR_KEY_CLASS        "Class"     //child of LOCAL_MACHINE
#define REGSTR_KEY_CONFIG       "Config"    //child of LOCAL_MACHINE
#define REGSTR_KEY_ENUM         "Enum"      //child of LOCAL_MACHINE
#define REGSTR_KEY_ROOTENUM     "Root"      //child of ENUM
#define REGSTR_KEY_BIOSENUM     "BIOS"      //child of ENUM
#define REGSTR_KEY_ACPIENUM     "ACPI"      //child of ENUM
#define REGSTR_KEY_PCMCIAENUM   "PCMCIA"    // child of ENUM
#define REGSTR_KEY_PCIENUM      "PCI"       // child of ENUM
#define REGSTR_KEY_VPOWERDENUM  "VPOWERD"       // child of ENUM
#ifndef NEC_98
#define REGSTR_KEY_ISAENUM  "ISAPnP"    //child of ENUM
#define REGSTR_KEY_EISAENUM "EISA"      //child of ENUM
#else // ifdef NEC_98
#define REGSTR_KEY_ISAENUM  "C98PnP"    //child of ENUM
#define REGSTR_KEY_EISAENUM "NESA"      //child of ENUM
#endif // ifdef NEC_98
#define REGSTR_KEY_LOGCONFIG    "LogConfig" //child of enum\root\dev\inst
#define REGSTR_KEY_SYSTEMBOARD  "*PNP0C01"  //child of enum\root
#define REGSTR_KEY_APM      "*PNP0C05"  //child of enum\root

#define REGSTR_KEY_INIUPDATE    "IniUpdate"
#define REG_KEY_INSTDEV     "Installed" //Child of hklm\class\classname

#define REGSTR_KEY_DOSOPTCDROM  "CD-ROM"
#define REGSTR_KEY_DOSOPTMOUSE  "MOUSE"


/*** Public registry paths
 */

#define REGSTR_DEFAULT_INSTANCE "0000"
#define REGSTR_PATH_MOTHERBOARD REGSTR_KEY_SYSTEMBOARD "\\" REGSTR_DEFAULT_INSTANCE
#define REGSTR_PATH_SETUP   "Software\\Microsoft\\Windows\\CurrentVersion"
#define REGSTR_PATH_SETUP_SETUP "Software\\Microsoft\\Windows\\CurrentVersion\\Setup"
#define REGSTR_PATH_SETUP_MIGRATION "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\Migration"
#define REGSTR_PATH_SETUP_OC "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\OptionalComponents"
#define REGSTR_PATH_SETUP_OC_MULTILANGUAGE "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\OptionalComponents\\MultilanguageTop"
#define REGSTR_PATH_SETUP_OC_BALTIC "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\OptionalComponents\\Baltic"
#define REGSTR_PATH_SETUP_OC_CE "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\OptionalComponents\\CE"
#define REGSTR_PATH_SETUP_OC_CYRILLIC "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\OptionalComponents\\Cyrillic"
#define REGSTR_PATH_SETUP_OC_GREEK "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\OptionalComponents\\Greek"
#define REGSTR_PATH_SETUP_OC_TURKISH "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\OptionalComponents\\Turkish"
#define REGSTR_PATH_SETUPX  "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\SetupX"
#define REGSTR_PATH_SETUPX_CERT "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\SetupX\\Cert"
#define REGSTR_PATH_INF_INTERNET "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\SetupX\\INF\\Internet"
#define REGSTR_PATH_INF_OEMNAME  "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\SetupX\\INF\\OEM Name"
#define REGSTR_PATH_CATALOGS  "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\SetupX\\Catalogs"
#define REGSTR_PATH_DRIVERSIGN  "Software\\Microsoft\\Driver Signing"
#define REGSTR_PATH_DRIVERSIGN_CLASSES  "Software\\Microsoft\\Driver Signing\\Classes"
#define REGSTR_PATH_DRIVERSIGN_CLASSES_HWID  "Software\\Microsoft\\Driver Signing\\Classes\\HWID"
#define REGSTR_PATH_REG1STBOOTCLEAN "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\Reg1stBootClean"
#define REGSTR_PATH_SRCPATHS    "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\SourcePaths"
#define REGSTR_PATH_VERSION_CONFLICT "Software\\Microsoft\\VersionConflictManager"
#define REGSTR_PATH_VERSION_CONFLICT_FILES "Software\\Microsoft\\VersionConflictManager\\Files"
#define REGSTR_PATH_PIFCONVERT  "Software\\Microsoft\\Windows\\CurrentVersion\\PIFConvert"
#define REGSTR_PATH_MSDOSOPTS   "Software\\Microsoft\\Windows\\CurrentVersion\\MS-DOSOptions"
#define REGSTR_PATH_MSDOSEMU    "Software\\Microsoft\\Windows\\CurrentVersion\\MS-DOS Emulation"
#define REGSTR_PATH_NEWDOSBOX   "Software\\Microsoft\\Windows\\CurrentVersion\\MS-DOS Emulation\\AppCompat"
#define REGSTR_PATH_RUNONCE "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
#define REGSTR_PATH_RUNONCE_SETUP "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce\\Setup"
#define REGSTR_PATH_RUN     "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define REGSTR_PATH_RUNSERVICESONCE "Software\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce"
#define REGSTR_PATH_RUNSERVICES     "Software\\Microsoft\\Windows\\CurrentVersion\\RunServices"
#define REGSTR_PATH_EXPLORER    "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"
#define REGSTR_PATH_DETECT  "Software\\Microsoft\\Windows\\CurrentVersion\\Detect"
#define REGSTR_PATH_APPPATHS    "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths"
#define REGSTR_PATH_UNINSTALL   "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
#define REGSTR_PATH_REALMODENET "Software\\Microsoft\\Windows\\CurrentVersion\\Network\\Real Mode Net"
#define REGSTR_PATH_NETEQUIV    "Software\\Microsoft\\Windows\\CurrentVersion\\Network\\Equivalent"
#define REGSTR_PATH_CVNETWORK "Software\\Microsoft\\Windows\\CurrentVersion\\Network"
#define REGSTR_PATH_REINSTALL   "Software\\Microsoft\\Windows\\CurrentVersion\\Reinstall"
#define REGSTR_PATH_SETUP_PRESERVE "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\Preserve"

//
// Attempting something dangerous.
//
#define REGSTR_PATH_ASD         "System\\CurrentControlSet\\Control\\ASD"
#define REGSTR_PATH_ASD_PROB    "System\\CurrentControlSet\\Control\\ASD\\Prob"
#define REGSTR_PATH_ASD_LIST    "System\\CurrentControlSet\\Control\\ASD\\List"

// Product ID
#define REGSTR_VAL_PRODUCTKEY  "ProductKey"
#define REGSTR_VAL_DIGITALPRODUCTID "DigitalProductId"

//
//Disk Cleanup applet and Cleanup COM objects
//
#define REGSTR_PATH_VOLUMECACHE     "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VolumeCaches"
#define REGSTR_VAL_DISPLAY              "display"
#define REGSTR_VAL_ADVANCEDBUTTONTEXT   "AdvancedButtonText"
#define REGSTR_VAL_BITMAPDISPLAY        "BitmapDisplay"
#define REGSTR_VAL_URL                  "URL"
#define REGSTR_VAL_FOLDER               "folder"
#define REGSTR_VAL_FILELIST             "FileList"
#define REGSTR_VAL_LASTACCESS           "LastAccess"
#define REGSTR_VAL_FLAGS                "Flags"
#define REGSTR_VAL_CLEANUPSTRING        "CleanupString"
#define REGSTR_VAL_FAILIFPROCESSRUNNING "FailIfProcessRunning"

#define REGSTR_PATH_IDCONFIGDB  "System\\CurrentControlSet\\Control\\IDConfigDB"
#define REGSTR_PATH_CLASS   "System\\CurrentControlSet\\Services\\Class"
#define REGSTR_PATH_DEVICECLASSES   "System\\CurrentControlSet\\Control\\DeviceClasses"
#define REGSTR_PATH_DISPLAYSETTINGS "Display\\Settings"
#define REGSTR_PATH_FONTS       "Display\\Fonts"
#define REGSTR_PATH_ENUM    "Enum"
#define REGSTR_PATH_ROOT    "Enum\\Root"
#define REGSTR_PATH_SERVICES    "System\\CurrentControlSet\\Services"
#define REGSTR_PATH_VXD     "System\\CurrentControlSet\\Services\\VxD"
#define REGSTR_PATH_NTKERN  "System\\CurrentControlSet\\Services\\NTKern"
#define REGSTR_PATH_CONFIGMG    "System\\CurrentControlSet\\Services\\VxD\\ConfigMg"
#define	REGSTR_VAL_PRELOAD_SUPPORT	"PreloadSupport"
#define	REGSTR_VAL_DEEP_SLEEP		"DeepSleep"
#define REGSTR_PATH_IOS         "System\\CurrentControlSet\\Services\\VxD\\IOS"
#define REGSTR_PATH_VMM         "System\\CurrentControlSet\\Services\\VxD\\VMM"
#define REGSTR_PATH_PCI         "System\\CurrentControlSet\\Services\\VxD\\PCI"
#define REGSTR_PATH_BIOS        "System\\CurrentControlSet\\Services\\VxD\\BIOS"
#define REGSTR_PATH_VPOWERD     "System\\CurrentControlSet\\Services\\VxD\\VPOWERD"
#define REGSTR_PATH_VNETSUP     "System\\CurrentControlSet\\Services\\VxD\\VNETSUP"
#define REGSTR_PATH_NWREDIR     "System\\CurrentControlSet\\Services\\VxD\\NWREDIR"
#define REGSTR_PATH_NCPSERVER   "System\\CurrentControlSet\\Services\\NcpServer\\Parameters"
#define REGSTR_PATH_SPANNABLEBUS "System\\CurrentControlSet\\Services\\VxD\\ConfigMg\\SpannableBus"
#define REGSTR_PATH_PNPBUS "System\\CurrentControlSet\\Services\\VxD\\ConfigMg\\PnPBus"
#define REGSTR_PATH_PREINSTALL     "System\\CurrentControlSet\\Services\\VxD\\ConfigMg\\PreInstall"
#define REGSTR_PATH_PREINSTALL_SOFTWARE "SoftWare"
#define REGSTR_PATH_PREINSTALL_HARDWARE "HardWare"
#define REGSTR_PATH_VCOMM       "System\\CurrentControlSet\\Services\\VxD\\VCOMM"

#define REGSTR_PATH_IOARB   "System\\CurrentControlSet\\Services\\Arbitrators\\IOArb"
#define REGSTR_PATH_ADDRARB "System\\CurrentControlSet\\Services\\Arbitrators\\AddrArb"
#define REGSTR_PATH_DMAARB  "System\\CurrentControlSet\\Services\\Arbitrators\\DMAArb"
#define REGSTR_PATH_IRQARB  "System\\CurrentControlSet\\Services\\Arbitrators\\IRQArb"

//
//  Used by address arbitrator
//
#define REGSTR_VAL_DONTUSEMEM   "DontAllocLastMem"

//
// Used by address and io arbitrator
//
#define REGSTR_PATH_RESERVED_RANGELIST  "ReservedRangeList"

#define REGSTR_PATH_CODEPAGE                "System\\CurrentControlSet\\Control\\Nls\\Codepage"
#define REGSTR_PATH_LOCALE                  "System\\CurrentControlSet\\Control\\Nls\\Locale"
#define REGSTR_PATH_FILESYSTEM              "System\\CurrentControlSet\\Control\\FileSystem"
#define REGSTR_PATH_FILESYSTEM_CACHEDIRS    "System\\CurrentControlSet\\Control\\FileSystem\\CacheDirectories"
#define REGSTR_RELPATH_FILESYSTEM_CACHEDIRS "CacheDirectories"
#define REGSTR_PATH_FILESYSTEM_NOVOLTRACK   "System\\CurrentControlSet\\Control\\FileSystem\\NoVolTrack"
#define REGSTR_PATH_CDFS                    "System\\CurrentControlSet\\Control\\FileSystem\\CDFS"
#define REGSTR_PATH_UDF                     "System\\CurrentControlSet\\Control\\FileSystem\\UDF"
#define REGSTR_PATH_WINBOOT                 "System\\CurrentControlSet\\Control\\WinBoot"
#define REGSTR_PATH_INSTALLEDFILES          "System\\CurrentControlSet\\Control\\InstalledFiles"
#define REGSTR_PATH_INSTALLEDFILES_RENAME   "System\\CurrentControlSet\\Control\\InstalledFiles\\Rename"
#define REGSTR_PATH_VMM32FILES              "System\\CurrentControlSet\\Control\\VMM32Files"
#define REGSTR_PATH_SETUPFLAGS              "Software\\Microsoft\\Windows\\CurrentVersion"

//
// Reasonable Limit for Values Names
//
#define REGSTR_MAX_VALUE_LENGTH     256

//
// Values under REGSTR_PATH_DISPLAYSETTINGS
//

#define REGSTR_VAL_BITSPERPIXEL  "BitsPerPixel"
#define REGSTR_VAL_RESOLUTION    "Resolution"
#define REGSTR_VAL_DPILOGICALX   "DPILogicalX"
#define REGSTR_VAL_DPILOGICALY   "DPILogicalY"
#define REGSTR_VAL_DPIPHYSICALX  "DPIPhysicalX"
#define REGSTR_VAL_DPIPHYSICALY  "DPIPhysicalY"
#define REGSTR_VAL_REFRESHRATE   "RefreshRate"
#define REGSTR_VAL_DISPLAYFLAGS  "DisplayFlags"
#define REGSTR_VAL_SCREENRES     "ScreenResolution"
#define REGSTR_VAL_DESKTOPSIZE   "DesktopSize"
#define REGSTR_VAL_DESKTOPPOS    "DesktopPos"


// under REGSTR_PATH_REINSTALL
#define REGSTR_VAL_BACKUPDIR                    "BackupDirectory"
#define REGSTR_VAL_LASTINSTALLSTATUS            "LastInstallStatus"
#define REGSTR_VAL_REBOOTSTATUS                 "RebootStatus"
#define REGSTR_VAL_REINSTALL_DISPLAYNAME        "DisplayName"
#define REGSTR_VAL_REINSTALL_HARDWAREID         "HardwareID"
#define REGSTR_VAL_REINSTALL_REINSTALLSTRING    "ReinstallString"


// under HKEY_CURRENT_USER
#define REGSTR_PATH_CONTROLPANEL    "Control Panel"

// under HKEY_LOCAL_MACHINE

#define REGSTR_PATH_CONTROLSFOLDER  "Software\\Microsoft\\Windows\\CurrentVersion\\Controls Folder"

//
// Entries under REGSTR_PATH_CODEPAGE
//

#define REGSTR_VAL_DOSCP        "OEMCP"
#define REGSTR_VAL_WINCP        "ACP"
#define REGSTR_VAL_DOSVMCP      "DOSCP"
#define REGSTR_VAL_DOSCC		"OEMCC"
#define REGSTR_VAL_DOSKT		"KYBTYPE"
#define REGSTR_VAL_DOSKL		"KYBLAYOUT"
#define REGSTR_VAL_DOSLID		"LANGID"
#define REGSTR_VAL_DOSCFN		"COUNTRYFNAME"
#define REGSTR_VAL_DOSCPFN		"CODEPAGEFNAME"
#define REGSTR_VAL_DOSKFN		"KYBFNAME"

#define REGSTR_VAL_INSTALLED    "Installed"
#define REGSTR_PATH_DYNA_ENUM   "Config Manager\\Enum"
#define REGSTR_PATH_DYNA_GLOBAL "Config Manager\\Global"

//
// Entries under REGSTR_PATH_DYNA_ENUM
//
#define REGSTR_VAL_HARDWARE_KEY "HardWareKey"
#define REGSTR_VAL_ALLOCATION   "Allocation"
#define REGSTR_VAL_PROBLEM  "Problem"
#define REGSTR_VAL_STATUS   "Status"
#define REGSTR_VAL_CHILD        "Child"
#define REGSTR_VAL_SIBLING  "Sibling"
#define REGSTR_VAL_PARENT   "Parent"

//
// Entries under REGSTR_PATH_DYNA_GLOBAL
//
#define	REGSTR_VAL_PAGER_TYPE	"Pager"
#define	REGSTR_VAL_DEVNODES_CHANGED	"Changed"

//
//  Entries under REGSTR_PATH_SETUP
//
#define REGSTR_VAL_SYSTEMROOT       "SystemRoot"
#define REGSTR_VAL_BOOTCOUNT        "BootCount"
#define REGSTR_VAL_REALNETSTART     "RealNetStart"
#define REGSTR_VAL_MEDIA        "MediaPath"
#define REGSTR_VAL_CONFIG       "ConfigPath"
#define REGSTR_VAL_DEVICEPATH       "DevicePath"    //default search path for .INFs
#define REGSTR_VAL_OTHERDEVICEPATH  "OtherDevicePath"   //alternate search path for .INFs
#define REGSTR_VAL_DRIVERUPDATEPATH "DriverUpdatePath"  // List of places to search for driver during an update
#define REGSTR_VAL_REMMEDIA_RECURSE "RemovableMediaSearchRoot"  // List of Removable Media root dirs to be recursivly searched for drivers
#define REGSTR_VAL_OLDWINDIR        "OldWinDir" //old windows location
#define REGSTR_VAL_OLDWINVER          "OldWinVer" // old windows version
#define REGSTR_VAL_SETUPFLAGS       "SetupFlags"    //flags that setup passes on after install.
#define REGSTR_VAL_REGOWNER             "RegisteredOwner"
#define REGSTR_VAL_REGORGANIZATION      "RegisteredOrganization"
#define REGSTR_VAL_LICENSINGINFO        "LicensingInfo"
#define REGSTR_VAL_OLDMSDOSVER          "OldMSDOSVer" // will be DOS ver < 7 (when Setup run)
#define REGSTR_VAL_FIRSTINSTALLDATETIME "FirstInstallDateTime" // will Win 95 install date-time
#define REGSTR_VAL_INSTALLTYPE      "InstallType"
#define REGSTR_VAL_WRAPPER      "Wrapper"
#define REGSTR_VAL_HWSETUP      "HWSetup"
#define REGSTR_VAL_VERSION      "Version"
#define REGSTR_VAL_VERSIONNUMBER    "VersionNumber"
#define REGSTR_VAL_SUBVERSIONNUMBER    "SubVersionNumber"
#define REGSTR_VAL_KEYBOARDID    "KeyboardID"
#define REGSTR_VAL_PORTABLE      "Portable"
#define REGSTR_VAL_LEGACYFREE    "LegacyFree"
#define REGSTR_VAL_AUDITINPROGRESS	"AuditInProgress"


//  Values for InstallType
#define IT_COMPACT          0x0000
#define IT_TYPICAL          0x0001
#define IT_PORTABLE         0x0002
#define IT_CUSTOM           0x0003

#define REGSTR_KEY_SETUP        "\\Setup"
#define REGSTR_KEY_VAR_LDID           "VarLDID"
#define REGSTR_VAL_BOOTDIR              "BootDir"
#define REGSTR_VAL_WINBOOTDIR           "WinbootDir"
#define REGSTR_VAL_WINDIR       "WinDir"
#define REGSTR_VAL_SHOWEBDPAGE  "EBDPage"

// Values for setup - Reg1stBootClean
#define REGSTR_VAL_REGCLEAN_ROOTKEY "RootKey"
#define REGSTR_VAL_REGCLEAN_SUBKEY  "SubKey"
#define REGSTR_VAL_REGCLEAN_VALUE   "ValueName"
#define REGSTR_VAL_REGCLEAN_DATA    "DataString"

// Values for version conflict manager
#define REGSTR_VAL_BACKUP_DIRTY "Dirty"
#define REGSTR_VAL_BACKUP_DATE "BackupDate"
#define REGSTR_VAL_BACKUP_CONFLICT_TYPE "ConflictType"
#define REGSTR_VAL_BACKUP_RESTORE "Restore"

// Value for install media classes
#define REGSTR_VAL_INSTALL_CLASSES      "InstallClasses"

// Source Path Types
#define SPT_OTHER       0x00000000
#define SPT_CDROM       0x00000001
#define SPT_FLOPPY      0x00000002
#define SPT_NETWORK     0x00000004
#define SPT_FIXEDDISK   0x00000008

#define REGSTR_VAL_SRCPATH            "SourcePath"              //last source files path during setup.
#define REGSTR_VAL_SRCPATHTYPE        "InstallPathType"         // type of source path
#define REGSTR_VAL_PRECOPY_SRCPATH    "PostDetectSrcPath"
#define REGSTR_VAL_SUWIN_CMDLINE      "CommandLine"
#define REGSTR_VAL_SETUPBINARY        "SetupBinary"

#define REGSTR_VAL_APPINSTPATH      "AppInstallPath"    // Used by install wizard
#define REGSTR_VAL_OLSSRCPATH       "OLSSrcPath"

// Values for international startup disk
#define REGSTR_PATH_EBD          REGSTR_PATH_SETUP REGSTR_KEY_SETUP "\\EBD"
// Keys under REGSTR_KEY_EBD
#define REGSTR_KEY_EBDFILESLOCAL     "EBDFilesLocale"
#define REGSTR_KEY_EBDFILESKEYBOARD     "EBDFilesKeyboard"
#define REGSTR_KEY_EBDAUTOEXECBATLOCAL "EBDAutoexecBatLocale"
#define REGSTR_KEY_EBDAUTOEXECBATKEYBOARD "EBDAutoexecBatKeyboard"
#define REGSTR_KEY_EBDCONFIGSYSLOCAL   "EBDConfigSysLocale"
#define REGSTR_KEY_EBDCONFIGSYSKEYBOARD   "EBDConfigSysKeyboard"

#define REGSTR_PATH_ACTIVE_SETUP       "Software\\Microsoft\\Active Setup"
#define REGSTR_PATH_AS_INSTALLED       REGSTR_PATH_ACTIVE_SETUP "\\Installed Components"
// Values under REGSTR_PATH_AS_INSTALLED
#define REGSTR_VAL_AS_ISINSTALLED      "IsInstalled"
#define REGSTR_VAL_AS_VERSION          "Version"
#define REGSTR_VAL_AS_LOCALE           "Locale"
#define REGSTR_VAL_AS_STUBPATH         "StubPath"
#define REGSTR_VAL_AS_COMPONENTID      "ComponentID"
#define REGSTR_VAL_AS_DONTASK          "DontAsk"

// Values under REGSTR_PATH_DRIVERSIGN
#define REGSTR_VAL_POLICY           "Policy"

// Types of driver signing policies
#define DRIVERSIGN_NONE             0x00000000
#define DRIVERSIGN_WARNING          0x00000001
#define DRIVERSIGN_BLOCKING         0x00000002

//
// Entries for REGSTR_PATH_SRCPATHS
//
#define REGSTR_VAL_PATH     "Path"

//
//  Entries under REGSTR_PATH_PIFCONVERT
//
#define REGSTR_VAL_MSDOSMODE            "MSDOSMode"
#define REGSTR_VAL_MSDOSMODEDISCARD     "Discard"

//
//  Entries under REGSTR_PATH_MSDOSOPTS (global settings)
//
#define REGSTR_VAL_DOSOPTGLOBALFLAGS    "GlobalFlags"
//  Flags for GlobalFlags
#define DOSOPTGF_DEFCLEAN   0x00000001L // Default action is clean config

//
//  Entries under REGSTR_PATH_MSDOSOPTS \ OptionSubkey
//
#define REGSTR_VAL_DOSOPTFLAGS      "Flags"
#define REGSTR_VAL_OPTORDER     "Order"
#define REGSTR_VAL_CONFIGSYS        "Config.Sys"
#define REGSTR_VAL_AUTOEXEC     "Autoexec.Bat"
#define REGSTR_VAL_STDDOSOPTION     "StdOption"
#define REGSTR_VAL_DOSOPTTIP        "TipText"

//  Flags for DOSOPTFLAGS
#define DOSOPTF_DEFAULT     0x00000001L // Default enabled for clean config
#define DOSOPTF_SUPPORTED   0x00000002L // Option actually supported
#define DOSOPTF_ALWAYSUSE   0x00000004L // Always use this option
#define DOSOPTF_USESPMODE   0x00000008L // Option puts machine in Prot Mode
#define DOSOPTF_PROVIDESUMB 0x00000010L // Can load drivers high
#define DOSOPTF_NEEDSETUP   0x00000020L // Need to configure option
#define DOSOPTF_INDOSSTART  0x00000040L // Suppored by DOSSTART.BAT
#define DOSOPTF_MULTIPLE    0x00000080L // Load multiple configuration lines

//
//  Flags returned by SUGetSetSetupFlags and in the registry
//
#define SUF_FIRSTTIME    0x00000001L // First boot into Win95.
#define SUF_EXPRESS      0x00000002L // User Setup via express mode (vs customize).
#define SUF_BATCHINF     0x00000004L // Setup using batch file (MSBATCH.INF).
#define SUF_CLEAN        0x00000008L // Setup was done to a clean directory.
#define SUF_INSETUP      0x00000010L // You're in Setup.
#define SUF_NETSETUP     0x00000020L // Doing a net (workstation) setup.
#define SUF_NETHDBOOT    0x00000040L // Workstation boots from local harddrive
#define SUF_NETRPLBOOT   0x00000080L // Workstation boots via RPL (vs floppy)
#define SUF_SBSCOPYOK    0x00000100L // Can copy to LDID_SHARED (SBS)
#define SUF_NODEVINSTALL 0x00000200L // Device Installers can devices.
#define SUF_MIGRATEDLLS  0x00000400L // Call the Migration DLLs
#define SUF_CUSTOMINSTALL 0x00000800L // Tells If setup is in custom Mode.

//
//  Entries under REGSTR_PATH_VMM
//
#define REGSTR_VAL_DOSPAGER "DOSPager"
#define REGSTR_VAL_VXDGROUPS    "VXDGroups"

//
// Entries under REGSTR_PATH_NTKERN or NTKERN as devloader
//
#define REGSTR_VAL_NTMPDRIVER   "NTMPDriver"

//
//  Entries under REGSTR_PATH_VPOWERD
//
#define REGSTR_VAL_VPOWERDFLAGS "Flags"         // Stupid machine workarounds
#define VPDF_DISABLEPWRMGMT         0x00000001  // Don't load device
#define VPDF_FORCEAPM10MODE         0x00000002  // Always go into 1.0 mode
#define VPDF_SKIPINTELSLCHECK       0x00000004  // Don't detect Intel SL chipset
#define VPDF_DISABLEPWRSTATUSPOLL   0x00000008  // Don't poll power status
#define VPDF_DISABLERINGRESUME      0x00000010  // Don't let the modem wake the machine (APM 1.2 only)
#define VPDF_SHOWMULTIBATT          0x00000020  // Show all batteries checkbox in power control panel
#define VPDF_DONTDISABLEDEVPM       0x00000100  // Indicates a bios that hangs when we disable device PM
#define VPDF_SUSPENDWORKSWRONG      0x00000200  // BIOS whose suspend is completely broken but otherwise OK.
#define VPDF_PERMSUSPENDWORKSWRONG  0x00000400  // Same as VPDF_SUSPENDWORKSWRONG but cannot be cleared by device manager
#define VPDF_HIBERNATEWORKSWRONG    0x00000800  // Hibernate is completely broken but otherwise OK.
#define VPDF_PERMHIBERNATEWORKSWRONG 0x00001000  // Same as VPDF_HIBERNATEWORKSWRONG but cannot be cleared by device manager

#define REGSTR_VAL_SUSPENDFLAG "SuspendFlag"    // Set when suspending, cleared when resuming
#define REGSTR_VAL_SUSPENDWORKEDFLAG "SuspendHasWorkedFlag"    // We have successfully suspended/resumed on this machine
#define REGSTR_VAL_LASTBIOSDATE "BiosDate"  // Current Bios Date
#define REGSTR_VAL_HIBERFLAG "HiberFlag"    // Set when Hibernating, Cleared when Resuming from Hibernate.
#define REGSTR_VAL_HIBERNATEWORKEDFLAG "HibernateHasWorkedFlag"    // We have successfully Hibernated/resumed on this machine
#define	REGSTR_VAL_DELAYOFFONHIBERNATE	"HiberDelayOff"
#define	REGSTR_VAL_REBOOTONHIBERNATE	"HiberReboot"
#define	REGSTR_VAL_PERFHIBERNATE	"HiberPerf"
#define REGSTR_VAL_BAD32BITAPMS5 "Skip32BitAPMS5"    // Set when there is a bad AMP s5 implementation in 32 bit prot mode.
#define REGSTR_VAL_USES4OS		"UseS4OS"	// Set via inf based on SKU
#define	REGSTR_VAL_DISABLE_LEGACY_WAKEUP	"DisableLegacyWakeUp"

//
// Power Policy 
//
#define REGSTR_PATH_POWERCFG            "Control Panel\\PowerCfg"
#define REGSTR_VAL_CURRENTPOWERPOLICY   "CurrentPowerPolicy"

//
//  Entries under REGSTR_PATH_VNETSUP
//
#define REGSTR_VAL_WORKGROUP "Workgroup"
#define REGSTR_VAL_DIRECTHOST "DirectHost"
#define REGSTR_VAL_FILESHARING      "FileSharing"
#define REGSTR_VAL_PRINTSHARING     "PrintSharing"

//
//  Entries under REGSTR_PATH_NWREDIR
//
#define REGSTR_VAL_FIRSTNETDRIVE    "FirstNetworkDrive"
#define REGSTR_VAL_MAXCONNECTIONS   "MaxConnections"
#define REGSTR_VAL_APISUPPORT       "APISupport"
#define REGSTR_VAL_MAXRETRY     "MaxRetry"
#define REGSTR_VAL_MINRETRY     "MinRetry"
#define REGSTR_VAL_SUPPORTLFN       "SupportLFN"
#define REGSTR_VAL_SUPPORTBURST     "SupportBurst"
#define REGSTR_VAL_SUPPORTTUNNELLING    "SupportTunnelling"
#define REGSTR_VAL_FULLTRACE        "FullTrace"
#define REGSTR_VAL_READCACHING      "ReadCaching"
#define REGSTR_VAL_SHOWDOTS     "ShowDots"
#define REGSTR_VAL_GAPTIME      "GapTime"
#define REGSTR_VAL_SEARCHMODE       "SearchMode"
#define REGSTR_VAL_SHELLVERSION     "ShellVersion"
#define REGSTR_VAL_MAXLIP           "MaxLIP"
#define REGSTR_VAL_PRESERVECASE     "PreserveCase"
#define REGSTR_VAL_OPTIMIZESFN      "OptimizeSFN"


//
//  Entries under REGSTR_PATH_NCPSERVER
//
#define REGSTR_VAL_NCP_BROWSEMASTER     "BrowseMaster"
#define REGSTR_VAL_NCP_USEPEERBROWSING  "Use_PeerBrowsing"
#define REGSTR_VAL_NCP_USESAP           "Use_Sap"

//
// Entries under REGSTR_PATH_VCOMM
//

#define REGSTR_VAL_VCOMM_POWER			"EnablePowerManagement"
#define REGSTR_VAL_PCCARD_WAKEUP        "EnableWakeUp"
#define REGSTR_VAL_DISABLE_WAKEUP       "DisableWakeUp"

//
//  Entries under REGSTR_PATH_FILESYSTEM
//
#define REGSTR_VAL_AUTOMOUNT        "AutoMountDrives"
#define REGSTR_VAL_COMPRESSIONMETHOD    "CompressionAlgorithm"
#define REGSTR_VAL_COMPRESSIONTHRESHOLD "CompressionThreshold"
#define REGSTR_VAL_DOUBLEBUFFER     "DoubleBuffer"
#define REGSTR_VAL_FORCEPMIO        "ForcePMIO"
#define REGSTR_VAL_NAMENUMERICTAIL  "NameNumericTail"
#define REGSTR_VAL_READAHEADTHRESHOLD   "ReadAheadThreshold"
#define REGSTR_VAL_SOFTCOMPATMODE   "SoftCompatMode"
#define REGSTR_VAL_WIN31FILESYSTEM  "Win31FileSystem"
#define REGSTR_VAL_ACDRIVESPINDOWN  "ACDriveSpinDown"
#define REGSTR_VAL_ASYNCFILECOMMIT  "AsyncFileCommit"
#define REGSTR_VAL_BATDRIVESPINDOWN "BatDriveSpinDown"
#define REGSTR_VAL_BUFFAGETIMEOUT   "BufferAgeTimeout"
#define REGSTR_VAL_BUFFIDLETIMEOUT  "BufferIdleTimeout"
#define REGSTR_VAL_CONTIGFILEALLOC  "ContigFileAllocSize"
#define REGSTR_VAL_FLOPPYFASTBOOT   "FloppyFastBoot"	// Obsolete: use REGSTR_VAL_FLOPPYENUM
#define REGSTR_VAL_DRIVESPINDOWN    "DriveSpinDown"
#define REGSTR_VAL_DRIVEWRITEBEHIND "DriveWriteBehind"
#define REGSTR_VAL_FORCERMIO        "ForceRMIO"
#define REGSTR_VAL_LASTBOOTPMDRVS   "LastBootPMDrvs"
#define REGSTR_VAL_ACSPINDOWNPREVIOUS   "ACSpinDownPrevious"
#define REGSTR_VAL_BATSPINDOWNPREVIOUS  "BatSpinDownPrevious"
#define REGSTR_VAL_MAXLOADREAD      "MaxLoadRead"
#define REGSTR_VAL_NAMECACHECOUNT   "NameCache"
#define REGSTR_VAL_NETWAREPATHSEMANTICS "NetwarePathSemantics"
#define REGSTR_VAL_NOGANGLOAD       "NoGangLoad"
#define REGSTR_VAL_PATHCACHECOUNT   "PathCache"
#define REGSTR_VAL_PRESERVELONGNAMES    "PreserveLongNames"
#define REGSTR_VAL_SHUTDOWNSTATUS   "ShutdownStatus"
#define REGSTR_VAL_SRVNAMECACHE     "ServerNameCache"
#define REGSTR_VAL_SRVNAMECACHECOUNT    "ServerNameCacheMax"
#define REGSTR_VAL_SRVNAMECACHENETPROV  "ServerNameCacheNumNets"
#define REGSTR_VAL_VIRTUALHDIRQ     "VirtualHDIRQ"
#define REGSTR_VAL_VOLIDLETIMEOUT   "VolumeIdleTimeout"
#define REGSTR_VAL_GETDEVPARMSIOCTLAPI  "GetDevParmsIoctl"
#define REGSTR_VAL_MAX_FAT32_TOTAL_SPACE "MaxFAT32TotalSpace"
#define REGSTR_VAL_MAX_FAT32_FREE_SPACE  "MaxFAT32FreeSpace"
#define REGSTR_VAL_DRIVE_LDS_BDCAST_DISABLE  "DisableLowDiskSpaceBroadcast"
#define REGSTR_VAL_DISABLE_SCANDISK_ON_BOOT	 "DisableScandiskOnBoot"
#define REGSTR_VAL_WINDRIVE_LOW_DISK_SPACE	 "WindowsDriveLowDiskSpace"

//
//  Entries under REGSTR_PATH_FILESYSTEM_NOVOLTRACK
//
//  A sub-key under which a variable number of variable length structures are stored.
//
//  Each structure contains an offset followed by a number of pattern bytes.
//  The pattern in each structure is compared at the specified offset within
//  the boot record at the time a volume is mounted.  If any pattern in this
//  set of patterns matches a pattern already in the boot record, VFAT will not
//  write a volume tracking serial number in the OEM_SerialNum field of the
//  boot record on the volume being mounted.
//

//
//  Entries under REGSTR_PATH_CDFS
//
#define REGSTR_VAL_CDCACHESIZE  "CacheSize" // Number of 2K cache sectors
#define REGSTR_VAL_CDPREFETCH   "Prefetch"  // Number of 2K cache sectors for prefetching
#define REGSTR_VAL_CDPREFETCHTAIL "PrefetchTail"// Number of LRU1 prefetch sectors
#define REGSTR_VAL_CDRAWCACHE   "RawCache"  // Number of 2352-byte cache sectors
#define REGSTR_VAL_CDEXTERRORS  "ExtendedErrors"// Return extended error codes
#define REGSTR_VAL_CDSVDSENSE   "SVDSense"  // 0=PVD, 1=Kanji, 2=Unicode
#define REGSTR_VAL_CDSHOWVERSIONS "ShowVersions"// Show file version numbers
#define REGSTR_VAL_CDCOMPATNAMES "MSCDEXCompatNames"// Disable Numeric Tails on long file names
#define REGSTR_VAL_CDNOREADAHEAD "NoReadAhead"  // Disable Read Ahead if set to 1

//
//  Entry under REGSTR_PATH_UDF
//
#define REGSTR_VAL_UDFCONTROLFLAG  "ControlFlag" // UDF control flag

//
//  define values for IOS devices
//
#define REGSTR_VAL_SCSI "SCSI\\"
#define REGSTR_VAL_ESDI "ESDI\\"
#define REGSTR_VAL_FLOP "FLOP\\"

//
// define defs for IOS device types and values for IOS devices
//

#define REGSTR_VAL_DISK "GenDisk"
#define REGSTR_VAL_CDROM    "GenCD"
#define REGSTR_VAL_TAPE "TAPE"
#define REGSTR_VAL_SCANNER "SCANNER"
#define REGSTR_VAL_FLOPPY   "FLOPPY"

#define REGSTR_VAL_SCSITID "SCSITargetID"
#define REGSTR_VAL_SCSILUN "SCSILUN"
#define REGSTR_VAL_REVLEVEL "RevisionLevel"
#define REGSTR_VAL_PRODUCTID "ProductId"
#define REGSTR_VAL_PRODUCTTYPE "ProductType"
#define REGSTR_VAL_DEVTYPE "DeviceType"
#define REGSTR_VAL_REMOVABLE "Removable"
#define  REGSTR_VAL_CURDRVLET "CurrentDriveLetterAssignment"
#define REGSTR_VAL_USRDRVLET "UserDriveLetterAssignment"
#define REGSTR_VAL_SYNCDATAXFER "SyncDataXfer"
#define REGSTR_VAL_AUTOINSNOTE  "AutoInsertNotification"
#define REGSTR_VAL_AUTOINSERT_CAPABLE   "AutoInsertNotificationCapable"
#define REGSTR_VAL_AUTOINSNOTE_DRIVE "AutoInsertNotificationDrive"
#define REGSTR_VAL_DISCONNECT "Disconnect"
#define REGSTR_VAL_INT13 "Int13"
#define REGSTR_VAL_PMODE_INT13 "PModeInt13"
#define REGSTR_VAL_USERSETTINGS "AdapterSettings"
#define REGSTR_VAL_NOIDE "NoIDE"
#define REGSTR_VAL_NODMA "NoDMA"

#define REGSTR_VAL_IDE_DMA "IDEDMADRIVE"

#define REGSTR_VAL_GOOD_OEM1_DMA "GOODOEM1DMA"

#define REGSTR_VAL_DMA_IN_USE "DMACurrentlyUsed"

#define REGSTR_VAL_DMA_UNSUPPORTED "DMAUnsupported"

#define REGSTR_VAL_IDE_MASTER_DRIVE "IDEMaster"

#define REGSTR_VAL_NO_EXT_INT13 "NoExtInt13"

#ifdef NEC_98
#define REGSTR_VAL_IDE_PRIMARY_CHANNEL "IDEPrimary"
#define REGSTR_VAL_NO_DWORD_PIO "No32bitPIO"
#endif //#ifdef NEC_98

// The foll. clase name definitions should be the same as in dirkdrv.inx and
// cdrom.inx
#define REGSTR_VAL_DISKCLASSNAME    "DiskDrive"
#define REGSTR_VAL_CDROMCLASSNAME   "CDROM"

// The foll. value determines whether a port driver should be force loaded
// or not.

#define REGSTR_VAL_FORCELOAD    "ForceLoadPD"

//
// The following values are used for HSFLOP .pdr
//

#define REGSTR_VAL_FORCEFIFO    "ForceFIFO"
#define REGSTR_VAL_FORCECL      "ForceChangeLine"
#define REGSTR_VAL_LEAVERM      "LeaveRealMode"
#define REGSTR_VAL_FASTBOOT     "FloppyFastBoot"	// Obsolete: use REGSTR_VAL_FLOPPYENUM
#define REGSTR_VAL_MAXSCAN      "FloppyMaxScan"
#define REGSTR_VAL_FLOPPYENUM   "FloppyEnumerate"

//
// Generic CLASS Entries
//
#define REGSTR_VAL_NOUSECLASS       "NoUseClass"            // Don't include this class in PnP functions
#define REGSTR_VAL_NOINSTALLCLASS   "NoInstallClass"        // Don't include this class in New Device Wizard
#define REGSTR_VAL_NODISPLAYCLASS   "NoDisplayClass"        // Don't include this class in Device Manager
#define REGSTR_VAL_SILENTINSTALL    "SilentInstall"         // Always Silent Install devices of this class.
#define REGSTR_VAL_SILENTNOTIFY     "SilentInstallNotify"   // This silent class wants the New Hardware Found dialog.
#define REGSTR_VAL_CLASSORDER       "ClassProcessingOrder"  // Class processing ordering
//
//  Class Names
//
#define REGSTR_KEY_PCMCIA_CLASS     "PCMCIA"            //child of PATH_CLASS
#define REGSTR_KEY_SCSI_CLASS       "SCSIAdapter"
#define REGSTR_KEY_PORTS_CLASS      "ports"
#define REGSTR_KEY_MEDIA_CLASS      "MEDIA"
#define REGSTR_KEY_DISPLAY_CLASS    "Display"
#define REGSTR_KEY_KEYBOARD_CLASS   "Keyboard"
#define REGSTR_KEY_MOUSE_CLASS      "Mouse"
#define REGSTR_KEY_MONITOR_CLASS    "Monitor"
#define REGSTR_KEY_MODEM_CLASS      "Modem"
#define REGSTR_KEY_HDC_CLASS        "HDC"
#define REGSTR_KEY_DISKDRIVE_CLASS  "DiskDrive"

//
//  Values under PATH_CLASS\PCMCIA
//
#define REGSTR_VAL_SKIPWIZARD   "SkipWizardForBatchSetup"

#define REGSTR_VAL_PCMCIA_OPT   "Options"
#define PCMCIA_OPT_HAVE_SOCKET  0x00000001l
//#define PCMCIA_OPT_ENABLED    0x00000002l
#define PCMCIA_OPT_AUTOMEM	0x00000004l
#define PCMCIA_OPT_NO_SOUND	0x00000008l
#define PCMCIA_OPT_NO_AUDIO	0x00000010l
#define PCMCIA_OPT_NO_APMREMOVE 0x00000020l
#define PCMCIA_OPT_LEAVE_PWR_ON 0x00000040l
#define PCMCIA_OPT_NO_MODEM_WAKE 0x00000100l	// Enable power state mapping for modems
#define PCMCIA_OPT_DEVICE_WAKEUP 0x00000200l	// Disable power state mapping for non-modems

#define REGSTR_VAL_PCMCIA_MEM   "Memory"    // Card services shared mem range
#define PCMCIA_DEF_MEMBEGIN 0x000C0000  // default 0xC0000 - 0x00FFFFFF
#define PCMCIA_DEF_MEMEND   0x00FFFFFF  // (0 - 16meg)
#define PCMCIA_DEF_MEMLEN   0x00001000  // default 4k window

#define REGSTR_VAL_PCMCIA_ALLOC "AllocMemWin"   // PCCard alloced memory Window
#define REGSTR_VAL_PCMCIA_ATAD  "ATADelay"  // ATA device config start delay
#define REGSTR_VAL_PCMCIA_CNFGD "ConfigDelay"   // Device specific config start delay
#define REGSTR_VAL_PCMCIA_SHARE "EnableIRQSharing"  // Keep shared IRQ configs

#define REGSTR_VAL_PCMCIA_SIZ   "MinRegionSize" // Minimum region size
#define PCMCIA_DEF_MIN_REGION   0x00010000  // 64K minimum region size

#define REGSTR_VAL_PCMCIA_TFFS  "TFFSOption"
#define PCMCIA_OPT_TFFS_DEFAULT 0x00000001l

// Values in LPTENUM keys
#define REGSTR_VAL_P1284MDL     "Model"
#define REGSTR_VAL_P1284MFG     "Manufacturer"

//
//  Values under PATH_CLASS\ISAPNP
//
#ifndef NEC_98
#define REGSTR_VAL_ISAPNP       "ISAPNP"    // ISAPNP VxD name
#else
#define REGSTR_VAL_ISAPNP       "C98PNP"    // C98PNP VxD name
#endif
#define REGSTR_VAL_ISAPNP_RDP_OVERRIDE  "RDPOverRide"   // ReadDataPort OverRide

//
//  Values under PATH_CLASS\ISAPNP
//
#define REGSTR_ISAPNP_OPTIONS       "Options"   // Possible ISAPNP options
#define ISAPNP_FLAG_NO_ENUM     0x00000001l
#define ISAPNP_FLAG_ALWAYS_ON       0x00000002l
#define ISAPNP_FLAG_NO_BOOT_CONFIG  0x00000004l
#define ISAPNP_FLAG_DONT_STOP    0x00000008l

//
//  Values under PATH_CLASS\PCI
//
#define REGSTR_VAL_PCI          "PCI"       // PCI VxD name
#define REGSTR_PCI_OPTIONS      "Options"   // Possible PCI options
#define REGSTR_VAL_BIOS_ATTRIBUTES  "Attributes"    // Possible PCI BIOS attributes
#define REGSTR_VAL_REMOVE_DOCK_ON_SUSPEND	"RemoveDockOnSuspend"	// Force DOCK removal during suspend
#define REGSTR_PCI_DUAL_IDE     "PCIDualIDE"    // Dual IDE flag
#define REGSTR_KEY_IDE_MINIPORTS    "IDEMiniports"
#define REGSTR_KEY_IRQ_ROUTING_TABLE    "IRQRoutingTable"
#define REGSTR_VAL_CURRENT_ROUTING  "CurrentRouting"
#define REGSTR_KEY_IRQ_MINIPORTS    "IRQMiniports"
#define REGSTR_VAL_IRQTABLESTATUS       "IRQTableStatus"
#define REGSTR_VAL_IRQMINIPORTDATASTATUS    "IRQMiniportDataStatus"
#define REGSTR_VAL_IRQMINIPORTSTATUS        "IRQMINIPORTStatus"
#define REGSTR_VAL_IRQ_ROUTING_TABLE_PREFIX "IRQORT"
#define REGSTR_KEY_GART_MINIPORTS   "GARTMiniports"
#define	REGSTR_VAL_PCIDEVICE	"PCIDevice"
#define	PCI_DEVICE_ROOT_BUS	0x00000000
#define	PCI_DEVICE_IRQ_HOLDER	0x00000001

#define REGSTR_VAL_MINIPORT_NAME    "Name"
#define REGSTR_VAL_MINIPORT_PATH    "Path"
#define REGSTR_VAL_MINIPORT_INSTANCE    "Instance"

#define PCI_BIOS_BAD_ROUTING_TABLE          0x00000001  // This BIOS has bad PCI IRQ routing table
#define PCI_BIOS_FAILS_PROTECTED_MODE_CALL      0x00000002  // This BIOS hangs on PM call for routing table
#define PCI_BIOS_FAILS_REAL_MODE_CALL      0x00000004  // This BIOS hangs on RM call for routing table
#define PCI_BIOS_TURN_OFF_PARITY		0x00000008 // Turn of parity on this machine

#define PCI_OPTION_USE_BIOS             0x00000001l
#define PCI_OPTION_USE_IRQ_STEERING         0x00000002l
#define PCI_OPTION_OVERRIDE_BRIDGES         0x00000004l
#define PCI_OPTION_DONT_OVERRIDE_GAT            0x00000008l
#define PCI_OPTION_GAT_ON               0x00000010l
#define PCI_OPTION_GET_IRQ_TABLE_FROM_BIOS      0x00000020l
#define PCI_OPTION_IGNORE_PREFETCH          0x00000040l
#define PCI_OPTION_DONT_GET_IRQ_TABLE_FROM_REALMODE_BIOS    0x00000080l
#define PCI_OPTION_DONT_GET_IRQ_TABLE_FROM_SPEC_BIOS        0x00000100l
#define PCI_OPTION_CANNOT_USE_PM_INTERFACE            0x00000200l
#define PCI_OPTION_CANNOT_USE_RM_INTERFACE            0x00000400l
#define PCI_OPTION_DONT_GET_IRQ_TABLE_FROM_ACPI        0x00000800l

#define PCI_IRQ_OPTIONS_MASK    (PCI_OPTION_DONT_GET_IRQ_TABLE_FROM_ACPI|PCI_OPTION_DONT_GET_IRQ_TABLE_FROM_SPEC_BIOS|PCI_OPTION_DONT_GET_IRQ_TABLE_FROM_REALMODE_BIOS|PCI_OPTION_GET_IRQ_TABLE_FROM_BIOS|PCI_OPTION_USE_IRQ_STEERING)
#define PCI_OPTIONS_REBOOT_MASK (PCI_IRQ_OPTIONS_MASK|PCI_OPTION_USE_BIOS|PCI_OPTION_OVERRIDE_BRIDGES)

#define PCI_FLAG_NO_VIDEO_IRQ           0x00000001l
#define PCI_FLAG_PCMCIA_WANT_IRQ        0x00000002l
#define PCI_FLAG_DUAL_IDE           0x00000004l
#define PCI_FLAG_NO_ENUM_AT_ALL         0x00000008l
#define PCI_FLAG_ENUM_NO_RESOURCE       0x00000010l
#define PCI_FLAG_NEED_DWORD_ACCESS      0x00000020l
#define PCI_FLAG_SINGLE_FUNCTION        0x00000040l
#define PCI_FLAG_ALWAYS_ENABLED         0x00000080l
#define PCI_FLAG_IS_IDE             0x00000100l
#define PCI_FLAG_IS_VIDEO           0x00000200l
#define PCI_FLAG_FAIL_START         0x00000400l
#define PCI_FLAG_GHOST              0x00000800l
#define PCI_FLAG_DOUBLE_DECKER          0x00001000l
#define PCI_FLAG_ONE_CHILD          0x00002000l
#define PCI_FLAG_PRESERVE_COMMAND       0x00004000l
#define PCI_FLAG_IS_VGA             0x00008000l
#define PCI_FLAG_CB_SHARE_CMD_BITS      0x00010000l
#define PCI_FLAG_STRAIGHT_IRQ_ROUTING       0x00020000l
#define PCI_FLAG_SUBTRACTIVE_DECODE     0x00040000l
#define PCI_FLAG_FDMA_ISA                       0x00080000l
#define PCI_FLAG_EXCLUSIVE          0x00100000l
#define PCI_FLAG_EDGE               0x00200000l
#define PCI_FLAG_NO_SUBSYSTEM           0x00400000l
#define PCI_FLAG_NO_WPE                 0x00800000l
#define PCI_FLAG_OLD_ID                 0x01000000l
#define PCI_FLAG_DONT_SHRINK_BRIDGE	0x02000000l
#define PCI_FLAG_TURN_OFF_PARITY	0x04000000l
#define PCI_FLAG_NO_NON_PCI_CHILD_BAR	0x08000000l
#define PCI_FLAG_NO_ENUM_WITH_DISABLE	0x10000000l
#define PCI_FLAG_NO_PM_CAPS		0x20000000l
#define	PCI_FLAG_NO_BRIDGE_RESET	0x40000000l

// Error values.
#define PCI_TABLE_STATUS_NOT_FOUND          0x00000000l
#define PCI_TABLE_STATUS_FAILED_SANITY_CHECK        0x00000001l
#define PCI_TABLE_STATUS_BAD_ROUTING_BIOS       0x00000002l
#define PCI_TABLE_STATUS_FAILS_PROTECTED_MODE_CALL  0x00000003l

#define PCI_TABLE_STATUS_ERROR_MAX          0x00000020l

// Status values.
#define PCI_TABLE_STATUS_REGISTRY           0x00000020l
#define PCI_TABLE_STATUS_MS_ROUTING_TABLE       0x00000021l
#define PCI_TABLE_STATUS_PCIBIOS            0x00000022l
#define PCI_TABLE_STATUS_REALMODE_PCIBIOS           0x00000023l
#define PCI_TABLE_STATUS_ACPI			0x00000024l

#define PCI_MINIPORTDATA_STATUS_BAD_DATA        0x00000000l
#define PCI_MINIPORTDATA_STATUS_NO_KEY          0x00000001l
#define PCI_MINIPORTDATA_STATUS_NOT_FOUND       0x00000002l
#define PCI_MINIPORTDATA_STATUS_ERROR_MAX       0x00000020l
#define PCI_MINIPORTDATA_STATUS_SUCCESS         0x00000020l

#define PCI_MINIPORT_STATUS_LOAD_ERROR          0x00000000l
#define PCI_MINIPORT_STATUS_INVALID_MINIPORT        0x00000001l
#define PCI_MINIPORT_STATUS_ERROR_INITIALIZING      0x00000002l
#define PCI_MINIPORT_STATUS_ERROR_VALIDATING        0x00000003l
#define PCI_MINIPORT_STATUS_ERROR_MAX           0x00000020l
#define PCI_MINIPORT_STATUS_SUCCESS         0x00000020l
#define PCI_MINIPORT_STATUS_REGISTRY_ERROR          0x00000100l
#define PCI_MINIPORT_STATUS_ALREADY_INITIALIZED     0x00000200l

//
// Registry entry for devices using F-DMA
//
#define REGSTR_VAL_USE_FDMA                     "UseFDMA"

//
// Values under a PCI Dual IDE controller.
//
#define REGSTR_VAL_CHANNEL_OPTIONS      "ChannelOptions"

#define PCIIDE_CHANNELS_NONE            0x00
#define PCIIDE_CHANNELS_PRI_ONLY        0x01
#define PCIIDE_CHANNELS_SEC_ONLY        0x02
#define PCIIDE_CHANNELS_PRI_SEC         0x03

//
//  Values under PATH_CLASS\BIOS
//
#define REGSTR_VAL_BIOS         "BIOS"      // BIOS VxD name
#define REGSTR_VAL_BIOS_OPTIONS     "Options"   // Possible BIOS options
#define REGSTR_VAL_BIOS_DONT_REMOVE_PCIC	"DontRemovePCIC"


#define BIOS_OPTION_NO_NVRAM        0x00000001l

//
// Detection related values
//
#define REGSTR_KEY_CRASHES  "Crashes"   // key of REGSTR_PATH_DETECT
#define REGSTR_KEY_DANGERS  "Dangers"   // key of REGSTR_PATH_DETECT
#define REGSTR_KEY_DETMODVARS   "DetModVars"    // key of REGSTR_PATH_DETECT
#define REGSTR_KEY_NDISINFO "NDISInfo"  // key of netcard hw entry
#define REGSTR_VAL_PROTINIPATH  "ProtIniPath"   // protocol.ini path
#define REGSTR_VAL_RESOURCES    "Resources" // resources of crash func.
#define REGSTR_VAL_CRASHFUNCS   "CrashFuncs"    // detfunc caused the crash
#define REGSTR_VAL_CLASS    "Class"     // device class
#define REGSTR_VAL_DEVDESC  "DeviceDesc"    // device description
#define REGSTR_VAL_BOOTCONFIG   "BootConfig"    // detected configuration
#define REGSTR_VAL_DETFUNC  "DetFunc"   // specifies detect mod/func.
#define REGSTR_VAL_DETFLAGS "DetFlags"  // detection flags
#define REGSTR_VAL_COMPATIBLEIDS "CompatibleIDs" //value of enum\dev\inst
#define REGSTR_VAL_DETCONFIG    "DetConfig" // detected configuration
#define REGSTR_VAL_VERIFYKEY    "VerifyKey" // key used in verify mode
#define REGSTR_VAL_COMINFO  "ComInfo"   // com info. for serial mouse
#define REGSTR_VAL_INFNAME  "InfName"   // INF filename
#define REGSTR_VAL_CARDSPECIFIC "CardSpecific"  // Netcard specific info (WORD)
#define REGSTR_VAL_NETOSTYPE    "NetOSType" // NetOS type associate w/ card
#define REGSTR_DATA_NETOS_NDIS  "NDIS"      // Data of REGSTR_VAL_NETOSTYPE
#define REGSTR_DATA_NETOS_ODI   "ODI"       // Data of REGSTR_VAL_NETOSTYPE
#define REGSTR_DATA_NETOS_IPX   "IPX"       // Data of REGSTR_VAL_NETOSTYPE
#define REGSTR_VAL_MFG              "Mfg"
#define REGSTR_VAL_DRIVERVER_STRING "DriverVersion"
#define REGSTR_VAL_DRIVERDATE_STRING "DriverDate"
#define REGSTR_VAL_DRIVERSIGNER_STRING "SignedBy"
#define REGSTR_VAL_OEMID    "OEMID"     // Value name of *PNP0A05
#define REGSTR_VAL_RSDPTRADDR   "RSDPTRAddr"    // Value name of *PNP0A05
#define REGSTR_VAL_RSDTADDR "RSDTAddr"  // Value name of *PNP0A05
#define REGSTR_VAL_MATCHINGDEVID    "MatchingDeviceId"
#define REGSTR_VAL_FAILREASONSTRING    "FailReasonString"
#define REGSTR_VAL_FAILREASONID        "FailReasonID"

#define REGSTR_VAL_SCAN_ONLY_FIRST  "ScanOnlyFirstDrive"    // used with IDE driver
#define REGSTR_VAL_SHARE_IRQ    "ForceIRQSharing"   // used with IDE driver
#define REGSTR_VAL_NONSTANDARD_ATAPI    "NonStandardATAPI"  // used with IDE driver
#define REGSTR_VAL_IDE_FORCE_SERIALIZE  "ForceSerialization"    // used with IDE driver
#define REGSTR_VAL_MAX_HCID_LEN 1024        // Maximum hardware/compat ID len
#define REGSTR_VAL_HWREV            "HWRevision"
#define REGSTR_VAL_ENABLEINTS  "EnableInts"
#define REGSTR_VAL_ACPIOPTION	"ACPIOption"
#define REGSTR_VAL_ACPICHECKDATE     "ACPICheckDate"
//
// Bit values of REGSTR_VAL_DETFLAGS
//
#define REGDF_NOTDETIO      0x00000001  //cannot detect I/O resource
#define REGDF_NOTDETMEM     0x00000002  //cannot detect mem resource
#define REGDF_NOTDETIRQ     0x00000004  //cannot detect IRQ resource
#define REGDF_NOTDETDMA     0x00000008  //cannot detect DMA resource
#define REGDF_NOTDETALL     (REGDF_NOTDETIO | REGDF_NOTDETMEM | REGDF_NOTDETIRQ | REGDF_NOTDETDMA)
#define REGDF_NEEDFULLCONFIG    0x00000010  //stop devnode if lack resource
#define REGDF_GENFORCEDCONFIG   0x00000020  //also generate forceconfig
#define REGDF_TOPLEVEL_BUS  0x00000040  //devnode is top level bus
#define REGDF_NODETCONFIG   0x00008000  //don't write detconfig to reg.
#define REGDF_CONFLICTIO    0x00010000  //I/O res. in conflict
#define REGDF_CONFLICTMEM   0x00020000  //mem res. in conflict
#define REGDF_CONFLICTIRQ   0x00040000  //IRQ res. in conflict
#define REGDF_CONFLICTDMA   0x00080000  //DMA res. in conflict
#define REGDF_CONFLICTALL   (REGDF_CONFLICTIO | REGDF_CONFLICTMEM | REGDF_CONFLICTIRQ | REGDF_CONFLICTDMA)
#define REGDF_MAPIRQ2TO9    0x00100000  //IRQ2 has been mapped to 9
#define REGDF_NOTVERIFIED   0x80000000  //previous device unverified

#define REGSTR_VAL_UNIQUE_ID	"UniqueID"

//
//  Values in REGSTR_KEY_SYSTEMBOARD
//
#define REGSTR_VAL_APMBIOSVER       "APMBiosVer"
#define REGSTR_VAL_APMFLAGS     "APMFlags"
#define REGSTR_VAL_SLSUPPORT        "SLSupport"
#define REGSTR_VAL_MACHINETYPE      "MachineType"
#define REGSTR_VAL_SETUPMACHINETYPE "SetupMachineType"
#define REGSTR_MACHTYPE_UNKNOWN     "Unknown"
#define REGSTR_MACHTYPE_IBMPC       "IBM PC"
#define REGSTR_MACHTYPE_IBMPCJR     "IBM PCjr"
#define REGSTR_MACHTYPE_IBMPCCONV   "IBM PC Convertible"
#define REGSTR_MACHTYPE_IBMPCXT     "IBM PC/XT"
#define REGSTR_MACHTYPE_IBMPCXT_286 "IBM PC/XT 286"
#define REGSTR_MACHTYPE_IBMPCAT     "IBM PC/AT"
#define REGSTR_MACHTYPE_IBMPS2_25   "IBM PS/2-25"
#define REGSTR_MACHTYPE_IBMPS2_30_286   "IBM PS/2-30 286"
#define REGSTR_MACHTYPE_IBMPS2_30   "IBM PS/2-30"
#define REGSTR_MACHTYPE_IBMPS2_50   "IBM PS/2-50"
#define REGSTR_MACHTYPE_IBMPS2_50Z  "IBM PS/2-50Z"
#define REGSTR_MACHTYPE_IBMPS2_55SX "IBM PS/2-55SX"
#define REGSTR_MACHTYPE_IBMPS2_60   "IBM PS/2-60"
#define REGSTR_MACHTYPE_IBMPS2_65SX "IBM PS/2-65SX"
#define REGSTR_MACHTYPE_IBMPS2_70   "IBM PS/2-70"
#define REGSTR_MACHTYPE_IBMPS2_P70  "IBM PS/2-P70"
#define REGSTR_MACHTYPE_IBMPS2_70_80    "IBM PS/2-70/80"
#define REGSTR_MACHTYPE_IBMPS2_80   "IBM PS/2-80"
#define REGSTR_MACHTYPE_IBMPS2_90   "IBM PS/2-90"
#define REGSTR_MACHTYPE_IBMPS1      "IBM PS/1"
#define REGSTR_MACHTYPE_PHOENIX_PCAT    "Phoenix PC/AT Compatible"
#define REGSTR_MACHTYPE_HP_VECTRA   "HP Vectra"
#define REGSTR_MACHTYPE_ATT_PC      "AT&T PC"
#define REGSTR_MACHTYPE_ZENITH_PC   "Zenith PC"

#define REGSTR_VAL_APMMENUSUSPEND   "APMMenuSuspend"
#define APMMENUSUSPEND_DISABLED     0           // always disabled
#define APMMENUSUSPEND_ENABLED      1           // always enabled
#define APMMENUSUSPEND_UNDOCKED     2           // enabled undocked
#define APMMENUSUSPEND_NOCHANGE     0x80        // bitflag - cannot change setting via UI

#define REGSTR_VAL_APMACTIMEOUT     "APMACTimeout"
#define REGSTR_VAL_APMBATTIMEOUT    "APMBatTimeout"
#define APMTIMEOUT_DISABLED         0

#define REGSTR_VAL_APMSHUTDOWNPOWER "APMShutDownPower"

#define REGSTR_VAL_BUSTYPE          "BusType"
#define REGSTR_VAL_CPU              "CPU"
#define REGSTR_VAL_NDP              "NDP"
#define REGSTR_VAL_PNPBIOSVER       "PnPBIOSVer"
#define REGSTR_VAL_PNPSTRUCOFFSET   "PnPStrucOffset"
#define REGSTR_VAL_PCIBIOSVER       "PCIBIOSVer"
#define REGSTR_VAL_HWMECHANISM      "HWMechanism"
#define REGSTR_VAL_LASTPCIBUSNUM    "LastPCIBusNum"
#define REGSTR_VAL_CONVMEM          "ConvMem"
#define REGSTR_VAL_EXTMEM           "ExtMem"
#define REGSTR_VAL_COMPUTERNAME     "ComputerName"
#define REGSTR_VAL_BIOSNAME         "BIOSName"
#define REGSTR_VAL_BIOSVERSION      "BIOSVersion"
#define REGSTR_VAL_BIOSDATE         "BIOSDate"
#define REGSTR_VAL_MODEL            "Model"
#define REGSTR_VAL_SUBMODEL         "Submodel"
#define REGSTR_VAL_REVISION         "Revision"

//
//  Values used in the LPT(ECP) device entry
//
#define REGSTR_VAL_FIFODEPTH        "FIFODepth"
#define REGSTR_VAL_RDINTTHRESHOLD   "RDIntThreshold"
#define REGSTR_VAL_WRINTTHRESHOLD   "WRIntThreshold"

//used in enum\xxx\<devname>\<instname>
#define REGSTR_VAL_PRIORITY "Priority"      // WHAT IS THIS FOR??
#define REGSTR_VAL_DRIVER   "Driver"        //
#define REGSTR_VAL_FUNCDESC "FunctionDesc"      //
#define REGSTR_VAL_FORCEDCONFIG "ForcedConfig"      //
#define REGSTR_VAL_CONFIGFLAGS  "ConfigFlags"       // (binary ULONG)
#define REGSTR_VAL_CSCONFIGFLAGS "CSConfigFlags"    // (binary ULONG)
#define REGSTR_VAL_DEVICEVXDS       "DeviceVxDs"    // Used by ENUMFILE.DLL
#define REGSTR_KEY_BINDINGS         "Bindings"      // Used by ENUMFILE.DLL
#define REGSTR_VAL_PORTDRIVER       "PortDriver"    // Used by ENUMFILE.DLL
#define REGSTR_VAL_USERCHANGEDDRV   "UserChangedDriver" //Set to a 1 if user changed driver during setup.
#define REGSTR_VAL_CLASSGUID            "ClassGUID"
#define REGSTR_VAL_PARENT_UNIQUE_ID     "ParentUniqueID"
#define REGSTR_VAL_CONFIGMG_UNIQUE_ID   "ConfigMGUniqueID"
#define REGSTR_VAL_UPPER_FILTERS        "UpperFilters"
#define REGSTR_VAL_LOWER_FILTERS        "LowerFilters"
#define REGSTR_VAL_PRELOAD              "PreloadDrivers"
#define REGSTR_VAL_NTDEVICEPATHS        "NTDevicePaths"
#define REGSTR_VAL_SERVICE              "Service"
#define REGSTR_VAL_CONFIGURATION        "Configuration"
#define REGSTR_VAL_CONFIGURATIONVECTOR  "ConfigurationVector"
#define REGSTR_VAL_LOCATIONINFORMATION  "LocationInformation"
#define REGSTR_VAL_DEVICEOBJECTNAME     "DeviceObjectName"
#define REGSTR_VAL_CAPABILITIES         "Capabilities"
#define REGSTR_VAL_UINUMBER             "UINumber"
#define REGSTR_VAL_POWERENABLE          "PowerEnable"
#define REGSTR_VAL_DISABLEHIBERNATE "DisHiber"    // Set via inf if we want to disable the auto disable of hibernate for audio

#define CONFIGFLAG_DISABLED     0x00000001L  // Set if disabled
#define CONFIGFLAG_REMOVED      0x00000002L  // Set if a present hardware enum device deleted
#define CONFIGFLAG_MANUAL_INSTALL   0x00000004L  // Set if the devnode was manually installed
#define CONFIGFLAG_IGNORE_BOOT_LC   0x00000008L  // Set if skip the boot config
#define CONFIGFLAG_NET_BOOT     0x00000010L  // Load this devnode when in net boot
#define CONFIGFLAG_REINSTALL        0x00000020L  // Redo install
#define CONFIGFLAG_FAILEDINSTALL    0x00000040L  // Failed the install
#define CONFIGFLAG_CANTSTOPACHILD   0x00000080L  // Can't stop/remove a single child
#define CONFIGFLAG_OKREMOVEROM      0x00000100L  // Can remove even if rom.
#define CONFIGFLAG_NOREMOVEEXIT     0x00000200L  // Don't remove at exit.
#define CONFIGFLAG_CANGOAWAY        0x00000400L  // Device not fixed to portable.
#define CONFIGFLAG_MANUAL_DRIVER    0x00000800L  // Driver selected manually by user.
#define CONFIGFLAG_NOUPDATECANGOAWAY    0x00001000L  // Do not update CANGOAWAY flag automatically.
#define CONFIGFLAG_NODISCARDBOOT    0x00002000L  // Don't discard boot_log_conf even if told to.
#define CONFIGFLAG_NOBROADCAST		0x00004000L  // Don't do shell messages
#define CONFIGFLAG_NOPOWERMGMT      0x00008000L  // No system power management when this device is present.
#define CONFIGFLAG_NOWAKEUP         0x00010000L  // No wake up
#define CONFIGFLAG_USING_COMPAT_ID    0x00020000L  // Driver rank is >= FIRST_CID_RANK_FROM_INF

#define CSCONFIGFLAG_BITS				0x0000000FL  // OR of below bits
#define CSCONFIGFLAG_DISABLED			0x00000001L  // Set if
#define CSCONFIGFLAG_DO_NOT_CREATE		0x00000002L  // Set if
#define CSCONFIGFLAG_DO_NOT_START		0x00000004L  // Set if
#define CSCONFIGFLAG_DO_NOT_SHOW_IN_DM	0x00000008L  // Do not show in device manager

#define DMSTATEFLAG_APPLYTOALL      0x00000001  // Set if Apply To All check box is checked

//
// Special devnodes name
//
#define REGSTR_VAL_ROOT_DEVNODE     "HTREE\\ROOT\\0"
#define REGSTR_VAL_RESERVED_DEVNODE "HTREE\\RESERVED\\0"
#define REGSTR_PATH_READDATAPORT    REGSTR_KEY_ISAENUM "\\ReadDataPort\\0"
#define REGSTR_PATH_PCIIRQHOLDER    REGSTR_KEY_PCIENUM "\\IRQHolder\\"
#define REGSTR_PATH_ACPIIRQHOLDER    REGSTR_KEY_ACPIENUM "\\*PNP0C0F\\"

//
// Multifunction definitions
//
#define REGSTR_PATH_MULTI_FUNCTION      "MF"
#define REGSTR_VAL_RESOURCE_MAP         "ResourceMap"
#define REGSTR_VAL_VARYING_RESOURCE_MAP     "VaryingResourceMap"
#define REGSTR_PATH_CHILD_PREFIX        "Child"
#define REGSTR_PATH_SUBCHILD_PREFIX     "SubChild"
#define NUM_RESOURCE_MAP            256
#define REGSTR_VAL_MF_FLAGS         "MFFlags"
#define MF_FLAGS_EVEN_IF_NO_RESOURCE        0x00000001
#define MF_FLAGS_NO_CREATE_IF_NO_RESOURCE   0x00000002
#define MF_FLAGS_FILL_IN_UNKNOWN_RESOURCE   0x00000004
#define MF_FLAGS_CREATE_BUT_NO_SHOW_DISABLED    0x00000008
#define MF_FLAGS_ADD_VRM_EVEN_IF_NO_RESOURCE	0x00000010
#define MF_FLAGS_NO_FILL_IN_UNKNOWN_VRM		0x00000020

//
// EISA multi functions add-on
//
#ifndef NEC_98
#define REGSTR_VAL_EISA_RANGES      "EISARanges"
#define REGSTR_VAL_EISA_FUNCTIONS   "EISAFunctions"
#define REGSTR_VAL_EISA_FUNCTIONS_MASK  "EISAFunctionsMask"
#define REGSTR_VAL_EISA_FLAGS       "EISAFlags"
#define REGSTR_VAL_EISA_SIMULATE_INT15  "EISASimulateInt15"
#else // ifdef NEC_98
#define REGSTR_VAL_EISA_RANGES      "NESARanges"
#define REGSTR_VAL_EISA_FUNCTIONS   "NESAFunctions"
#define REGSTR_VAL_EISA_FUNCTIONS_MASK  "NESAFunctionsMask"
#define REGSTR_VAL_EISA_FLAGS       "NESAFlags"
#define REGSTR_VAL_EISA_SIMULATE_INT15  "NESASimulateInt15"
#endif // ifdef NEC_98
#define EISAFLAG_NO_IO_MERGE        0x00000001
#define EISAFLAG_SLOT_IO_FIRST      0x00000002
#define EISA_NO_MAX_FUNCTION        0xFF
#define NUM_EISA_RANGES         4


//
//  Driver entries
//
#define REGSTR_VAL_DRVDESC  "DriverDesc"    // value of enum\dev\inst\DRV
#define REGSTR_VAL_DEVLOADER    "DevLoader" // value of DRV
#define REGSTR_VAL_STATICVXD    "StaticVxD" // value of DRV
#define REGSTR_VAL_PROPERTIES   "Properties"    // value of DRV
#define REGSTR_VAL_MANUFACTURER "Manufacturer"
#define REGSTR_VAL_EXISTS   "Exists"    // value of HCC\HW\ENUM\ROOT\dev\inst
#define REGSTR_VAL_CMENUMFLAGS  "CMEnumFlags"   // (binary ULONG)
#define REGSTR_VAL_CMDRIVFLAGS  "CMDrivFlags"   // (binary ULONG)
#define REGSTR_VAL_ENUMERATOR   "Enumerator"    // value of DRV
#define REGSTR_VAL_DEVICEDRIVER "DeviceDriver"  // value of DRV
#define REGSTR_VAL_PORTNAME "PortName"  // VCOMM uses this for it's port names
#define REGSTR_VAL_INFPATH      "InfPath"
#define REGSTR_VAL_INFSECTION   "InfSection"
#define REGSTR_VAL_POLLING  "Polling"           // SCSI specific
#define REGSTR_VAL_DONTLOADIFCONFLICT "DontLoadIfConflict"  // SCSI specific
#define REGSTR_VAL_PORTSUBCLASS "PortSubClass"
#define REGSTR_VAL_NETCLEAN "NetClean" // Driver required for NetClean boot
#define REGSTR_VAL_IDE_NO_SERIALIZE "IDENoSerialize" // IDE specific
#define REGSTR_VAL_NOCMOSORFDPT "NoCMOSorFDPT"       // IDE specific
#define REGSTR_VAL_CD_MSN "TurnOnCDMsn"          // IDE specific
#define REGSTR_VAL_COMVERIFYBASE "COMVerifyBase"     // VCD specific
#define REGSTR_VAL_CERTIFICATE_FILE     "CertificationFile"     // Name of file containing Compat Drv Certificate
#define REGSTR_VAL_LINK                 "Link"
#define REGSTR_VAL_REINSTALLCLASS   "ReinstallClass"    // set if class should be reinstalled.

//
//  Driver keys
//
#define REGSTR_KEY_OVERRIDE "Override"  // key under the software section

//used by CONFIGMG
#define REGSTR_VAL_CONFIGMG "CONFIGMG"  // Config Manager VxD name
#define REGSTR_VAL_SYSDM    "SysDM"     // The device installer DLL
#define REGSTR_VAL_PRIVATE  "Private"   // The private library
#define REGSTR_VAL_DETECT   "Detect"    // The detection library
#define REGSTR_VAL_ASKFORCONFIG "AskForConfig"  // The AskForConfig library
#define REGSTR_VAL_WAITFORUNDOCK "WaitForUndock"    // The WaitForUndock library
#define REGSTR_VAL_REMOVEROMOKAY "RemoveRomOkay"    // The RemoveRomOkay library
#define REGSTR_VAL_ADD_FUNC "Func"      // The function extension
#define REGSTR_VAL_DONTSHAREISAIRQS "DontShareISAIRQs"
#define REGSTR_VAL_DONTFORCED3ONROOTDEVICES	"NoForcedD3"

//used in IDCONFIGDB
#define REGSTR_VAL_CURCONFIG    "CurrentConfig"     //value of idconfigdb
#define REGSTR_VAL_FRIENDLYNAME "FriendlyName"      //value of idconfigdb
#define REGSTR_VAL_CURRENTCONFIG "CurrentConfig"    //value of idconfigdb
#define REGSTR_VAL_MAP      "Map"           //value of idconfigdb
#define REGSTR_VAL_ID       "CurrentID"     //value of idconfigdb
#define REGSTR_VAL_DOCKED   "CurrentDockedState"    //value of idconfigdb
#define REGSTR_VAL_CHECKSUM "CurrentChecksum"   //value of idconfigdb
#define REGSTR_VAL_HWDETECT "HardwareDetect"    //value of idconfigdb
#define REGSTR_VAL_INHIBITRESULTS "InhibitResults"  //value of idconfigdb

//used in HKEY_CURRENT_CONFIG
#define REGSTR_VAL_PROFILEFLAGS "ProfileFlags"  // value of HKEY_CURRENT_CONFIG

//used in PCMCIA
#define REGSTR_KEY_PCMCIA   "PCMCIA\\"  //PCMCIA dev ID prefix
#define REGSTR_KEY_PCUNKNOWN    "UNKNOWN_MANUFACTURER"  //PCMCIA dev ID manuf
#define REGSTR_KEY_PCMDEV   "-DEV"      //MFC dev ID function component
#define REGSTR_VAL_PCSSDRIVER   "Driver"    //value of DRV
#define REGSTR_KEY_PCMTD    "MTD-"      //MTD dev ID component
#define REGSTR_VAL_PCMTDRIVER   "MTD"       //value of Mem Tech DRV

//used in hardware\enum\dev\inst by Device Installer
#define REGSTR_VAL_HARDWAREID    "HardwareID"    //value of enum\dev\inst
#define REGSTR_VAL_HARDWARE_LOCATION    "Location"  //value of enum\dev\inst

//value names under class brach REGSTR_KEY_CLASS + class name
// and for the drivers REGSTR_KEY_CLASS\classname\xxxx
#define REGSTR_VAL_INSTALLER    "Installer" //value of class\name
#define REGSTR_VAL_INSICON  "Icon"      //value of class\name
#define REGSTR_VAL_ENUMPROPPAGES    "EnumPropPages" // For Class/Device Properties
#define REGSTR_VAL_BASICPROPERTIES  "BasicProperties"   // For CPL basic Properties
#define REGSTR_VAL_PRIVATEPROBLEM   "PrivateProblem"    // For Handling Private Problems
#define REGSTR_VAL_ENUMDRVSTACK     "EnumDriverStack"   // Enumerate a devices driver files into a tree control

// names used for display driver set information
#define REGSTR_KEY_CURRENT  "Current"   // current mode information
#define REGSTR_KEY_DEFAULT  "Default"   // default configuration
#define REGSTR_KEY_MODES    "Modes"     // modes subtree
#define REGSTR_KEY_INFO         "Info"          // adapter information

#define REGSTR_VAL_MODE     "Mode"      // default mode
#define REGSTR_VAL_BPP      "BPP"       // bits per pixel
#define REGSTR_VAL_HRES     "HRes"      // horizontal resolution
#define REGSTR_VAL_VRES     "VRes"      // vertical resolution
#define REGSTR_VAL_FONTSIZE "FontSize"  // used in default or override
#define REGSTR_VAL_DRV      "drv"       // the driver file
#define REGSTR_VAL_GRB      "grb"       // the grabber file
#define REGSTR_VAL_VDD      "vdd"       // vdds used here
#define REGSTR_VAL_VER      "Ver"
#define REGSTR_VAL_MAXRES   "MaxResolution" // max res for monitors
#define REGSTR_VAL_DPMS     "DPMS"      // DPMS enabled
#define REGSTR_VAL_RESUMERESET  "ResumeReset"   // need reset on resume
#define REGSTR_VAL_MINIVDD  "minivdd"

#define REGSTR_VAL_DESCRIPTION "Description"

// keys in fontsize tree
#define REGSTR_KEY_SYSTEM   "System"    // entries for system.ini
#define REGSTR_KEY_USER     "User"      // entries for win.ini
#define REGSTR_VAL_DPI      "dpi"       // dpi of fontsize

//
// Used by PCIC socket services
//
#define REGSTR_VAL_PCICOPTIONS  "PCICOptions"   // Binary DWORD.  IRQ mask in
			// low word.
#define REGSTR_VAL_PCICFLAGS    "PCICFlags" // Binary DWORD flags
#define PCICFLAGS_TRUST_VSPINS  0x00000001  // Trust the voltage sense pins
#define PCICFLAGS_CL_RESET_BFS  0x00000002  // Reset Cirrus Logic BFS bit
#define REGSTR_VAL_PCIC_EXCASAVE "PCICEXCAContextRanges"
#define REGSTR_VAL_PCIC_CLEXTSAVE "PCICCLExtContextRanges"

#ifndef NEC_98
#define PCIC_DEFAULT_IRQMASK    0x4EB8      // Default IRQ masks
#else // ifdef NEC_98
#define PCIC_DEFAULT_IRQMASK    0x1468      // Default IRQ masks
#endif // ifdef NEC_98
#define PCIC_DEFAULT_NUMSOCKETS 0       // 0 = Automatic detection
#define REGSTR_VAL_PCICIRQMAP   "PCICIRQMap"    // Binary 16 byte IRQ map table

//
// Used by CBSS socket services
//
#define REGSTR_VAL_CBSSOPTIONS  "CBSSOptions"   // Binary DWORD.  IRQ mask in
			// low word.
#define REGSTR_VAL_CBSSIRQMAP   "CBSSIRQMap"    // Binary 16 byte IRQ map table
#define REGSTR_VAL_CBSSFLAGS    "CBSSFlags" // Binary DWORD flags
#define REGSTR_VAL_CBSS_COMPATID "CBSSCompatID" // Binary DWORD PCI ID
#define REGSTR_VAL_CBSS_ISAIRQS "CBSSISAIRQs"   // Binary DWORD ISA IRQ mask
#define REGSTR_VAL_CBSS_VENDOR_CTXT "CBSSVendorContext"
#define REGSTR_VAL_CBSS_DEVINIT	"CBSSDevInit"
#define REGSTR_VAL_CBSS_ZVENABLE "CBSSZVEnable"
#define REGSTR_VAL_CBSS_ZVDISABLE "CBSSZVDisable"
#define REGSTR_VAL_CBSS_ZVCAPABLE "CBSSZVCapable"

// names used for control panel entries
#define REGSTR_PATH_APPEARANCE  "Control Panel\\Appearance"
#define REGSTR_PATH_LOOKSCHEMES "Control Panel\\Appearance\\Schemes"
#define REGSTR_VAL_CUSTOMCOLORS "CustomColors"

#define REGSTR_PATH_SCREENSAVE      "Control Panel\\Desktop"
#define REGSTR_VALUE_USESCRPASSWORD "ScreenSaveUsePassword"
#define REGSTR_VALUE_SCRPASSWORD    "ScreenSave_Data"

#define REGSTR_VALUE_LOWPOWERTIMEOUT    "ScreenSaveLowPowerTimeout"
#define REGSTR_VALUE_POWEROFFTIMEOUT    "ScreenSavePowerOffTimeout"
#define REGSTR_VALUE_LOWPOWERACTIVE "ScreenSaveLowPowerActive"
#define REGSTR_VALUE_POWEROFFACTIVE "ScreenSavePowerOffActive"

// used for Windows applets
#define REGSTR_PATH_WINDOWSAPPLETS "Software\\Microsoft\\Windows\\CurrentVersion\\Applets"

//
// system tray.  Flag values defined in systrap.h
//
#define REGSTR_PATH_SYSTRAY "Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\SysTray"
#define REGSTR_VAL_SYSTRAYSVCS "Services"
#define REGSTR_VAL_SYSTRAYBATFLAGS "PowerFlags"
#define REGSTR_VAL_SYSTRAYPCCARDFLAGS "PCMCIAFlags"

//
//International floppy 3mode keys
//
#define REGSTR_PATH_3MODE   "System\\CurrentControlSet\\Control\\3mode"
#define REGSTR_VAL_3MODE    "3modeDrv"

//
// Used by system networking components to store per-user values.
// All keys here are under HKCU.
//
#define REGSTR_PATH_NETWORK_USERSETTINGS    "Network"

#define REGSTR_KEY_NETWORK_PERSISTENT       "\\Persistent"
#define REGSTR_KEY_NETWORK_RECENT       "\\Recent"
#define REGSTR_VAL_REMOTE_PATH          "RemotePath"
#define REGSTR_VAL_USER_NAME            "UserName"
#define REGSTR_VAL_PROVIDER_NAME        "ProviderName"
#define REGSTR_VAL_CONNECTION_TYPE      "ConnectionType"
#define REGSTR_VAL_UPGRADE          "Upgrade"

#define REGSTR_KEY_LOGON "\\Logon"
#define REGSTR_VAL_MUSTBEVALIDATED  "MustBeValidated"
#define REGSTR_VAL_RUNLOGINSCRIPT   "ProcessLoginScript"

//
// NetworkProvider entries. These entries are under
// REGSTR_PATH_SERVICES\\xxx\\NetworkProvider
//
#define REGSTR_KEY_NETWORKPROVIDER "\\NetworkProvider"
#define REGSTR_PATH_NW32NETPROVIDER REGSTR_PATH_SERVICES "\\NWNP32" REGSTR_KEY_NETWORKPROVIDER
#define REGSTR_PATH_MS32NETPROVIDER REGSTR_PATH_SERVICES "\\MSNP32" REGSTR_KEY_NETWORKPROVIDER
#define REGSTR_VAL_AUTHENT_AGENT "AuthenticatingAgent"

//
// Entries under REGSTR_PATH_REALMODENET
//
#define REGSTR_VAL_PREFREDIR "PreferredRedir"
#define REGSTR_VAL_AUTOSTART "AutoStart"
#define REGSTR_VAL_AUTOLOGON "AutoLogon"
#define REGSTR_VAL_OLDAUTOLOGON "OldAutoLogon"
#define REGSTR_VAL_SETBYSETUP "SetBySetup"
#define REGSTR_VAL_NETCARD "Netcard"
#define REGSTR_VAL_TRANSPORT "Transport"
#define REGSTR_VAL_DYNAMIC "Dynamic"
#define REGSTR_VAL_TRANSITION "Transition"
#define REGSTR_VAL_STATICDRIVE "StaticDrive"
#define REGSTR_VAL_LOADHI "LoadHi"
#define REGSTR_VAL_LOADRMDRIVERS "LoadRMDrivers"
#define REGSTR_VAL_SETUPN "SetupN"
#define REGSTR_VAL_SETUPNPATH "SetupNPath"

//
// Entries under REGSTR_PATH_CVNETWORK
//
#define REGSTR_VAL_WRKGRP_FORCEMAPPING "WrkgrpForceMapping"
#define REGSTR_VAL_WRKGRP_REQUIRED "WrkgrpRequired"

//
// NT-compatible place where the name of the currently logged-on user is stored.
//
#define REGSTR_PATH_CURRENT_CONTROL_SET "System\\CurrentControlSet\\Control"
#define REGSTR_VAL_CURRENT_USER         "Current User"

// section where password providers are installed (each provider has subkey under this key)
#define REGSTR_PATH_PWDPROVIDER     "System\\CurrentControlSet\\Control\\PwdProvider"
#define REGSTR_VAL_PWDPROVIDER_PATH "ProviderPath"
#define REGSTR_VAL_PWDPROVIDER_DESC "Description"
#define REGSTR_VAL_PWDPROVIDER_CHANGEPWD "ChangePassword"
#define REGSTR_VAL_PWDPROVIDER_CHANGEPWDHWND "ChangePasswordHwnd"
#define REGSTR_VAL_PWDPROVIDER_GETPWDSTATUS "GetPasswordStatus"
#define REGSTR_VAL_PWDPROVIDER_ISNP "NetworkProvider"
#define REGSTR_VAL_PWDPROVIDER_CHANGEORDER "ChangeOrder"

//
// Used by administrator configuration tool and various components who enforce
//
//
#define REGSTR_PATH_POLICIES    "Software\\Microsoft\\Windows\\CurrentVersion\\Policies"

// used to control remote update of administrator policies
#define REGSTR_PATH_UPDATE      "System\\CurrentControlSet\\Control\\Update"
#define REGSTR_VALUE_ENABLE     "Enable"
#define REGSTR_VALUE_VERBOSE    "Verbose"
#define REGSTR_VALUE_NETPATH    "NetworkPath"
#define REGSTR_VALUE_DEFAULTLOC "UseDefaultNetLocation"

//
//  Entries under REGSTR_PATH_POLICIES
//
#define REGSTR_KEY_NETWORK      "Network"
#define REGSTR_KEY_SYSTEM       "System"
#define REGSTR_KEY_PRINTERS     "Printers"
#define REGSTR_KEY_WINOLDAPP        "WinOldApp"

// (following are values REG_DWORD, legal values 0 or 1, treat as "0" if value not present)
// policies under NETWORK key
#define REGSTR_VAL_NOFILESHARING        "NoFileSharing" // "1" prevents server from loading
#define REGSTR_VAL_NOPRINTSHARING       "NoPrintSharing"
#define REGSTR_VAL_NOFILESHARINGCTRL    "NoFileSharingControl" // "1" removes sharing ui
#define REGSTR_VAL_NOPRINTSHARINGCTRL   "NoPrintSharingControl"
#define REGSTR_VAL_HIDESHAREPWDS        "HideSharePwds" // "1" hides share passwords with asterisks
#define REGSTR_VAL_DISABLEPWDCACHING    "DisablePwdCaching" // "1" disables caching
#define REGSTR_VAL_ALPHANUMPWDS         "AlphanumPwds" // "1" forces alphanumeric passwords
#define REGSTR_VAL_NETSETUP_DISABLE         "NoNetSetup"
#define REGSTR_VAL_NETSETUP_NOCONFIGPAGE    "NoNetSetupConfigPage"
#define REGSTR_VAL_NETSETUP_NOIDPAGE        "NoNetSetupIDPage"
#define REGSTR_VAL_NETSETUP_NOSECURITYPAGE  "NoNetSetupSecurityPage"
#define REGSTR_VAL_SYSTEMCPL_NOVIRTMEMPAGE  "NoVirtMemPage"
#define REGSTR_VAL_SYSTEMCPL_NODEVMGRPAGE   "NoDevMgrPage"
#define REGSTR_VAL_SYSTEMCPL_NOCONFIGPAGE   "NoConfigPage"
#define REGSTR_VAL_SYSTEMCPL_NOFILESYSPAGE  "NoFileSysPage"
#define REGSTR_VAL_DISPCPL_NODISPCPL        "NoDispCPL"
#define REGSTR_VAL_DISPCPL_NOBACKGROUNDPAGE "NoDispBackgroundPage"
#define REGSTR_VAL_DISPCPL_NOSCRSAVPAGE "NoDispScrSavPage"
#define REGSTR_VAL_DISPCPL_NOAPPEARANCEPAGE "NoDispAppearancePage"
#define REGSTR_VAL_DISPCPL_NOSETTINGSPAGE "NoDispSettingsPage"
#define REGSTR_VAL_SECCPL_NOSECCPL          "NoSecCPL"
#define REGSTR_VAL_SECCPL_NOPWDPAGE         "NoPwdPage"
#define REGSTR_VAL_SECCPL_NOADMINPAGE       "NoAdminPage"
#define REGSTR_VAL_SECCPL_NOPROFILEPAGE     "NoProfilePage"
#define REGSTR_VAL_PRINTERS_HIDETABS        "NoPrinterTabs"
#define REGSTR_VAL_PRINTERS_NODELETE        "NoDeletePrinter"
#define REGSTR_VAL_PRINTERS_NOADD           "NoAddPrinter"
#define REGSTR_VAL_WINOLDAPP_DISABLED       "Disabled"
#define REGSTR_VAL_WINOLDAPP_NOREALMODE     "NoRealMode"
#define REGSTR_VAL_NOENTIRENETWORK          "NoEntireNetwork"
#define REGSTR_VAL_NOWORKGROUPCONTENTS      "NoWorkgroupContents"
#define REGSTR_VAL_NO_CLEARTEXT_NET_PWD     "NoClearTextNetPassword"

// REG_DWORD, 0=off, otherwise value is minimum # of chars to allow in password
#define REGSTR_VAL_MINPWDLEN            "MinPwdLen"
// REG_DWORD, 0=off, otherwise value is # of days for pwd to expire
#define REGSTR_VAL_PWDEXPIRATION        "PwdExpiration"

#define REGSTR_VAL_WIN31PROVIDER        "Win31Provider" // REG_SZ

// policies under SYSTEM key
#define REGSTR_VAL_DISABLEREGTOOLS      "DisableRegistryTools"

#define REGSTR_VAL_DEVICEMANAGER	"sysdm.cpl"
#define REGSTR_PATH_SYSDM   "Software\\Microsoft\\Windows\\CurrentVersion\\SysDM"
#define REGSTR_VAL_REBOOTFLAGS           "RebootFlags"
#define REGSTR_VAL_SEARCHFLAGS           "SearchFlags"
#define REGSTR_VAL_SEARCHLOCATION        "SearchLocation"

#define BY_TYPE 0
#define BY_CONNECTION 1

#define REGSTR_PATH_WINLOGON    "Software\\Microsoft\\Windows\\CurrentVersion\\Winlogon"
#define REGSTR_VAL_LEGALNOTICECAPTION   "LegalNoticeCaption"    // REG_SZ
#define REGSTR_VAL_LEGALNOTICETEXT      "LegalNoticeText"       // REG_SZ
#define REGSTR_VAL_DRIVE_SPINDOWN       "NoDispSpinDown"

#define REGSTR_VAL_RESTRICTRUN  "RestrictRun"
//
//  Entries in policy file.  (Won't be in local registry, only policy hive)
#define REGSTR_KEY_POL_USERS        "Users"
#define REGSTR_KEY_POL_COMPUTERS    "Computers"
#define REGSTR_KEY_POL_USERGROUPS   "UserGroups"
#define REGSTR_KEY_POL_DEFAULT      ".default"
#define REGSTR_KEY_POL_USERGROUPDATA "GroupData\\UserGroups\\Priority"

//
//  Entries for time zone information under LOCAL_MACHINE
//
#define REGSTR_PATH_TIMEZONE        "System\\CurrentControlSet\\Control\\TimeZoneInformation"
#define REGSTR_VAL_TZBIAS       "Bias"
#define REGSTR_VAL_TZDLTBIAS        "DaylightBias"
#define REGSTR_VAL_TZSTDBIAS        "StandardBias"
#define REGSTR_VAL_TZACTBIAS        "ActiveTimeBias"
#define REGSTR_VAL_TZDLTFLAG        "DaylightFlag"
#define REGSTR_VAL_TZSTDSTART       "StandardStart"
#define REGSTR_VAL_TZDLTSTART       "DaylightStart"
#define REGSTR_VAL_TZDLTNAME        "DaylightName"
#define REGSTR_VAL_TZSTDNAME        "StandardName"
#define REGSTR_VAL_TZNOCHANGESTART  "NoChangeStart"
#define REGSTR_VAL_TZNOCHANGEEND    "NoChangeEnd"
#define REGSTR_VAL_TZNOAUTOTIME     "DisableAutoDaylightTimeSet"

//
//  Entries for floating point processor existence under LOCAL_MACHINE
//
#define REGSTR_PATH_FLOATINGPOINTPROCESSOR  "HARDWARE\\DESCRIPTION\\System\\FloatingPointProcessor"
#define REGSTR_PATH_FLOATINGPOINTPROCESSOR0 "HARDWARE\\DESCRIPTION\\System\\FloatingPointProcessor\\0"


//
//  Entries for computer name under LOCAL_MACHINE
//
#define REGSTR_PATH_COMPUTRNAME "System\\CurrentControlSet\\Control\\ComputerName\\ComputerName"
#define REGSTR_VAL_COMPUTRNAME "ComputerName"

//  Entry so that we force a reboot on shutdown / single instance dos app
#define REGSTR_PATH_SHUTDOWN "System\\CurrentControlSet\\Control\\Shutdown"
#define REGSTR_VAL_FORCEREBOOT     "ForceReboot"
#define REGSTR_VAL_SETUPPROGRAMRAN "SetupProgramRan"
#define REGSTR_VAL_DOES_POLLING    "PollingSupportNeeded"
#define REGSTR_VAL_FAST_REBOOT     "FastReboot"

//
//  Entries for known system DLLs under LOCAL_MACHINE
//
//  The VAL keys here are the actual DLL names (FOO.DLL)
//
#define REGSTR_PATH_KNOWNDLLS   "System\\CurrentControlSet\\Control\\SessionManager\\KnownDLLs"
#define REGSTR_PATH_KNOWN16DLLS "System\\CurrentControlSet\\Control\\SessionManager\\Known16DLLs"

//      Entries here for system dlls we need to version check in case overwritten
#define REGSTR_PATH_CHECKVERDLLS "System\\CurrentControlSet\\Control\\SessionManager\\CheckVerDLLs"
#define REGSTR_PATH_WARNVERDLLS  "System\\CurrentControlSet\\Control\\SessionManager\\WarnVerDLLs"

//  Entries here for app ini files we (msgsrv32) need to hack
#define REGSTR_PATH_HACKINIFILE  "System\\CurrentControlSet\\Control\\SessionManager\\HackIniFiles"

//  Keys here for bad applications we want to warn the user about before running
#define REGSTR_PATH_CHECKBADAPPS "System\\CurrentControlSet\\Control\\SessionManager\\CheckBadApps"
//  For apps == 4.00
#define REGSTR_PATH_CHECKBADAPPS400 "System\\CurrentControlSet\\Control\\SessionManager\\CheckBadApps400"

//  Keys here for applications we need to patch
#define REGSTR_PATH_APPPATCH "System\\CurrentControlSet\\Control\\SessionManager\\AppPatches"

//
//  Entries for known system VxDs under LOCAL_MACHINE
//
//  The VAL keys here are the full path names of VxDs (c:\app\vapp.vxd)
//  It is suggested that the keynames be the same as the module name of
//  the VxD.
//  This section is used to dyna-load VxDs with
//  CreateFile(\\.\vxd_regstr_keyname).
//

#define REGSTR_PATH_KNOWNVXDS   "System\\CurrentControlSet\\Control\\SessionManager\\KnownVxDs"

//
//  Entry for global environment variables under LOCAL_MACHINE
//
#define REGSTR_PATH_GLOBAL_ENV	"System\\CurrentControlSet\\Control\\SessionManager\\Environment"

//
//  Entry for values stored under REGSTR_GLOBAL_ENV
//
#define REGSTR_VAL_GLOBAL_ENV_ACT				"EnvAction"
#define REGSTR_VAL_GLOBAL_ENV_ACT_NOP			0x00000000
#define REGSTR_VAL_GLOBAL_ENV_ACT_IMP			0x00000001
#define REGSTR_VAL_GLOBAL_ENV_ACT_EXP			0x00000002
#define REGSTR_VAL_GLOBAL_ENV_ACT_SYNC			0x00000003
#define REGSTR_VAL_GLOBAL_ENV_ACT_RECON			0x00000004
#define REGSTR_VAL_GLOBAL_ENV_ACT_DEL			0x00000005

#define REGSTR_VAL_REG_ENV_CRC					"RegEnvCRC"
#define REGSTR_VAL_REG_ENV_SIZE					"RegEnvSize"

#define REGSTR_VAL_CONFIG_ENV_CRC				"ConfigEnvCRC"
#define REGSTR_VAL_CONFIG_ENV_SIZE				"ConfigEnvSize"
#define REGSTR_VAL_CONFIG_ENV_MOD				"ConfigEnvMod"

#define REGSTR_VAL_AUTOEXEC_ENV_CRC				"AutoexecEnvCRC"
#define REGSTR_VAL_AUTOEXEC_ENV_SIZE			"AutoexecEnvSize"
#define REGSTR_VAL_AUTOEXEC_ENV_MOD				"AutoexecEnvMod"

#define REGSTR_VAL_SPEC_OPS_FLAGS				  "SpecOpsFlags"
#define REGSTR_VAL_SPEC_OPS_RETAIN_VCOMM_PWR_MGMT 0x00000001

//
// Entries for values in uninstaller keys under REGSTR_PATH_UNINSTALL \ appname
//
#define REGSTR_VAL_UNINSTALLER_DISPLAYNAME     "DisplayName"
#define REGSTR_VAL_UNINSTALLER_COMMANDLINE     "UninstallString"

//
//  Entries for known per user settings: Under HKEY_CURRENT_USER
//
#define REGSTR_PATH_DESKTOP REGSTR_PATH_SCREENSAVE
#define REGSTR_PATH_MOUSE       "Control Panel\\Mouse"
#define REGSTR_PATH_KEYBOARD    "Control Panel\\Keyboard"
#define REGSTR_PATH_COLORS      "Control Panel\\Colors"
#define REGSTR_PATH_SOUND       "Control Panel\\Sound"
#define REGSTR_PATH_METRICS     "Control Panel\\Desktop\\WindowMetrics"
#define REGSTR_PATH_ICONS       "Control Panel\\Icons"
#define REGSTR_PATH_CURSORS     "Control Panel\\Cursors"
#define REGSTR_PATH_CHECKDISK   "Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Check Drive"
#define REGSTR_PATH_CHECKDISKSET    "Settings"
#define REGSTR_PATH_CHECKDISKUDRVS  "NoUnknownDDErrDrvs"
#define REGSTR_PATH_DEFRAG      "Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Defrag"
#define REGSTR_PATH_DEFRAG_WIZARD   "Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Defrag\\Wizard"
#define REGSTR_PATH_DEFRAG_SET      "Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Defrag\\Settings"
#define REGSTR_PATH_DEFRAG_SET_METH "Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Defrag\\Settings\\method"
#define REGSTR_PATH_DEFRAG_SET_SCANDSK "Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Defrag\\Settings\\scandisk";

//
//  Entries for known Default user settings: Under HKEY_USERS
//
#define REGSTR_PATH_DEFAULT_DESKTOP      ".Default\\Control Panel\\Desktop"

//
//  Entries under REGSTR_PATH_FAULT
//
#define REGSTR_PATH_FAULT       "Software\\Microsoft\\Windows\\CurrentVersion\\Fault"
#define REGSTR_VAL_FAULT_LOGFILE    "LogFile"

//
//  Entries under REGSTR_PATH_AEDEBUG
//
#define REGSTR_PATH_AEDEBUG     "Software\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug"
#define REGSTR_VAL_AEDEBUG_DEBUGGER "Debugger"
#define REGSTR_VAL_AEDEBUG_AUTO     "Auto"

//
//  Entries under REGSTR_PATH_GRPCONV
//
#define REGSTR_PATH_GRPCONV "Software\\Microsoft\\Windows\\CurrentVersion\\GrpConv"

//
//  Entries under the RegItem key in a shell namespace
//
#define REGSTR_VAL_REGITEMDELETEMESSAGE "Removal Message"

//
//  Entries for the Drives Tools page
//
//  NOTE that these items are not recorded for removable drives. These
//  keys record X=DSKTLSYSTEMTIME where X is the drive letter. Since
//  these tools actually work on the disk in the drive, as opposed to
//  the drive itself, it is pointless to record them on a removable media
//  since if a different disk is inserted in the drive, the data is
//  meaningless.
//
#define REGSTR_PATH_LASTCHECK       "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\LastCheck"
#define REGSTR_PATH_LASTOPTIMIZE    "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\LastOptimize"
#define REGSTR_PATH_LASTBACKUP      "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\LastBackup"
//
// The above 3 keys record with the registry value of the drive letter
// a SYSTEMTIME structure
//

//
// Entries under HKEY_LOCAL_MACHINE for Check Drive specific stuff
//
#define REGSTR_PATH_CHKLASTCHECK    "Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Check Drive\\LastCheck"
#define REGSTR_PATH_CHKLASTSURFAN   "Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Check Drive\\LastSurfaceAnalysis"

//
// Entries under HKEY_LOCAL_MACHINE for Defrag App Start Optimization Region Info
//
#define REGSTR_PATH_DEFRAG_APPSTART "Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Defrag\\AppStartRegions"

//
// Entries under HKEY_LOCAL_MACHINE\REGSTR_PATH_DEFRAG
//
// REGSTR_VAL_DEFRAG_APPLOGFILTER values, any combination of the following:
//   (0x0001)   Filters files in Windows system directory
//   (0x0002)   Filters data files
//
#define REGSTR_VAL_DEFRAG_APPLOGFILTER  "ApplogFilter"

//
// The above 2 keys record the following binary structure which is
// a system time structure with the addition of a result code field.
// Note that the time part of REGSTR_PATH_CHKLASTCHECK is effectively
// identical to REGSTR_PATH_LASTCHECK under the explorer key
//
typedef struct _DSKTLSYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
    WORD wResult;
} DSKTLSYSTEMTIME, *PDSKTLSYSTEMTIME, *LPDSKTLSYSTEMTIME;
//
// The following are defines for the wResult field
//
#define DTRESULTOK  0   // Operation was successful, no errors
#define DTRESULTFIX 1   // Operation was successful, errors were found
		//   but all were fixed.
#define DTRESULTPROB    2   // Operation was not successful or errors
		//   were found and some or all were not fixed.
#define DTRESULTPART    3   // Operation was partially completed but was
		//   terminated either by the user or an error.

//
//  Entries for persistent shares
//
#define REGSTR_KEY_SHARES         "Software\\Microsoft\\Windows\\CurrentVersion\\Network\\LanMan"
#define REGSTR_VAL_SHARES_FLAGS   "Flags"
#define REGSTR_VAL_SHARES_TYPE    "Type"
#define REGSTR_VAL_SHARES_PATH    "Path"
#define REGSTR_VAL_SHARES_REMARK  "Remark"
#define REGSTR_VAL_SHARES_RW_PASS "Parm1"
#define REGSTR_VAL_SHARES_RO_PASS "Parm2"

//
//  Entries for printer settings under LOCAL_MACHINE
//
#define REGSTR_PATH_PRINT           "System\\CurrentControlSet\\Control\\Print"
#define REGSTR_PATH_PRINTERS        "System\\CurrentControlSet\\Control\\Print\\Printers"
#define REGSTR_PATH_PROVIDERS       "System\\CurrentControlSet\\Control\\Print\\Providers"
#define REGSTR_PATH_MONITORS        "System\\CurrentControlSet\\Control\\Print\\Monitors"
#define REGSTR_PATH_ENVIRONMENTS    "System\\CurrentControlSet\\Control\\Print\\Environments"
#define REGSTR_VAL_START_ON_BOOT    "StartOnBoot"
#define REGSTR_VAL_PRINTERS_MASK    "PrintersMask"
#define REGSTR_VAL_DOS_SPOOL_MASK   "DOSSpoolMask"
#define REGSTR_KEY_CURRENT_ENV      "\\Windows 4.0"
#define REGSTR_KEY_DRIVERS          "\\Drivers"
#define REGSTR_KEY_PRINT_PROC       "\\Print Processors"

//
// Entries for MultiMedia under HKEY_CURRENT_USER
//
#define REGSTR_PATH_EVENTLABELS     "AppEvents\\EventLabels"
#define REGSTR_PATH_SCHEMES         "AppEvents\\Schemes"
#define REGSTR_PATH_APPS            REGSTR_PATH_SCHEMES "\\Apps"
#define REGSTR_PATH_APPS_DEFAULT    REGSTR_PATH_SCHEMES "\\Apps\\.Default"
#define REGSTR_PATH_NAMES           REGSTR_PATH_SCHEMES "\\Names"
#define REGSTR_PATH_MULTIMEDIA      REGSTR_PATH_SETUP "\\Multimedia"
#define REGSTR_PATH_MULTIMEDIA_AUDIO "Software\\Microsoft\\Multimedia\\Audio"

//
// Entries for MultiMedia under HKEY_LOCAL_MACHINE
//
#define REGSTR_PATH_MEDIARESOURCES  REGSTR_PATH_CURRENT_CONTROL_SET "\\MediaResources"
#define REGSTR_PATH_MEDIAPROPERTIES REGSTR_PATH_CURRENT_CONTROL_SET "\\MediaProperties"
#define REGSTR_PATH_PRIVATEPROPERTIES REGSTR_PATH_MEDIAPROPERTIES "\\PrivateProperties"
#define REGSTR_PATH_PUBLICPROPERTIES REGSTR_PATH_MEDIAPROPERTIES "\\PublicProperties"

// joysticks
#define REGSTR_PATH_JOYOEM           REGSTR_PATH_PRIVATEPROPERTIES "\\Joystick\\OEM"
#define REGSTR_PATH_JOYCONFIG        REGSTR_PATH_MEDIARESOURCES "\\Joystick"
#define REGSTR_KEY_JOYCURR           "CurrentJoystickSettings"
#define REGSTR_KEY_JOYSETTINGS       "JoystickSettings"

// joystick values found under REGSTR_PATH_JOYCONFIG
#define REGSTR_VAL_JOYUSERVALUES     "JoystickUserValues"
#define REGSTR_VAL_JOYCALLOUT        "JoystickCallout"

// joystick values found under REGSTR_KEY_JOYCURR and REGSTR_KEY_JOYSETTINGS
#define REGSTR_VAL_JOYNCONFIG        "Joystick%dConfiguration"
#define REGSTR_VAL_JOYNOEMNAME       "Joystick%dOEMName"
#define REGSTR_VAL_JOYNOEMCALLOUT    "Joystick%dOEMCallout"

// joystick values found under keys under REGSTR_PATH_JOYOEM
#define REGSTR_VAL_JOYOEMCALLOUT    "OEMCallout"
#define REGSTR_VAL_JOYOEMNAME       "OEMName"
#define REGSTR_VAL_JOYOEMDATA       "OEMData"
#define REGSTR_VAL_JOYOEMXYLABEL    "OEMXYLabel"
#define REGSTR_VAL_JOYOEMZLABEL     "OEMZLabel"
#define REGSTR_VAL_JOYOEMRLABEL     "OEMRLabel"
#define REGSTR_VAL_JOYOEMPOVLABEL   "OEMPOVLabel"
#define REGSTR_VAL_JOYOEMULABEL     "OEMULabel"
#define REGSTR_VAL_JOYOEMVLABEL     "OEMVLabel"
#define REGSTR_VAL_JOYOEMTESTMOVEDESC   "OEMTestMoveDesc"
#define REGSTR_VAL_JOYOEMTESTBUTTONDESC "OEMTestButtonDesc"
#define REGSTR_VAL_JOYOEMTESTMOVECAP    "OEMTestMoveCap"
#define REGSTR_VAL_JOYOEMTESTBUTTONCAP  "OEMTestButtonCap"
#define REGSTR_VAL_JOYOEMTESTWINCAP "OEMTestWinCap"
#define REGSTR_VAL_JOYOEMCALCAP     "OEMCalCap"
#define REGSTR_VAL_JOYOEMCALWINCAP  "OEMCalWinCap"
#define REGSTR_VAL_JOYOEMCAL1       "OEMCal1"
#define REGSTR_VAL_JOYOEMCAL2       "OEMCal2"
#define REGSTR_VAL_JOYOEMCAL3       "OEMCal3"
#define REGSTR_VAL_JOYOEMCAL4       "OEMCal4"
#define REGSTR_VAL_JOYOEMCAL5       "OEMCal5"
#define REGSTR_VAL_JOYOEMCAL6       "OEMCal6"
#define REGSTR_VAL_JOYOEMCAL7       "OEMCal7"
#define REGSTR_VAL_JOYOEMCAL8       "OEMCal8"
#define REGSTR_VAL_JOYOEMCAL9       "OEMCal9"
#define REGSTR_VAL_JOYOEMCAL10      "OEMCal10"
#define REGSTR_VAL_JOYOEMCAL11      "OEMCal11"
#define REGSTR_VAL_JOYOEMCAL12      "OEMCal12"

//
// DDS object management
//

#define REGSTR_PATH_DeviceObjects "System\\CurrentControlSet\\Services\\Device Objects"
#define REGSTR_KEY_DeviceObjects_CLSID "CLSID"

#define REGSTR_VAL_KernelObject "Kernel Object"
#define REGSTR_VAL_UserObject "User Object"
#define REGSTR_VAL_UserProxyKernelObject "User Proxy Kernel Object"
#define REGSTR_VAL_UserProxyRemoteObject "User Proxy Remote Object"
#define REGSTR_VAL_KernelStubKernelObject "Kernel Stub Kernel Object"

#define REGSTR_KEY_KernelServer "Kernel Server"
#define REGSTR_VAL_ObjectServer "Object Server"

//
// Keys for OEM CPU ID
//

#define REGSTR_PATH_CPUID "Hardware\\Description\\System\\CentralProcessor\\0"
#define REGSTR_CPUID_KEY            "VendorIdentifier"
#define REGSTR_VAL_CPUTYPE          "Identifier"
#define REGSTR_VAL_CPUMMX           "MMXIdentifier"
#define REGSTR_NX_GENERIC           "NexGenDriven"
#define REGSTR_CYRIX                "CyrixInstead"
#define REGSTR_INTEL                "GenuineIntel"
#define REGSTR_AMD                  "AuthenticAMD"

// DICE certificate management

#define REGSTR_PATH_CERTMGR         "Software\\Microsoft\\Certificate Mgr"
#define REGSTR_PATH_CERTMGR_KEYS    REGSTR_PATH_CERTMGR "\\Keys"
#define REGSTR_PATH_CERTMGR_CERTS   REGSTR_PATH_CERTMGR "\\Certificates"
#define REGSTR_VAL_SERIALNO         "SerialNo"
#define REGSTR_VAL_CANSIGN          "CanSign"
#define REGSTR_VAL_CRYPTOTYPE       "CryptoType"
#define REGSTR_VAL_PUBLICKEY        "PublicKey"
#define REGSTR_VAL_CERT             "Certificate"
#define REGSTR_VAL_COUNT            "Count"

//
//DeviceClasses values
//
#define REGSTR_VAL_SYMBOLICLINK     "SymbolicLink"

//
//Registration wizard values
//
#define REGSTR_VAL_REGDONE          "Regdone"

#endif  //_INC_REGSTR
