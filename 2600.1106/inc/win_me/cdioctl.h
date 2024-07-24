
/***	CDIOCTL.H
 *
 *	WARNING: PLEASE READ AND UNDERSTAND THE IMPLEMENTATION WARNINGS !!!
 *
 *	This file contains the definitions for the CDROM IOCTL commands.
 *	These commands are used to utilize and control the various
 *	hardware features which are unique to CD-ROM drives.
 *
 *	WARNING: USE AT YOUR OWN RISK.
 *
 *	IMPLEMENTATION WARNINGS:
 *	    1) These interfaces are for use by VxD drivers only.
 *		That is, the client of these interfaces must be a
 *		VxD driver.
 *
 *	    2) These interfaces are only used to communicate with
 *		CD-ROM devices controlled by Protect-Mode CD-ROM
 *		device drivers.
 *
 *	    3) These interfaces are not functional for real-mode
 *		CD-ROM drivers.
 *
 *	    4) These interfaces are currently not supported via
 *		Win32 APIs.
 *
 *	    5) Several of these functions are empty placeholder
 *		specifications with no working code to make them
 *		actually work.
 *
 *	       Those unimplemented functions are merely placeholder
 *		specifications for features which may or may not be
 *		implemented in the future.
 *
 *	       For example, none of the the commands for controlling
 *		the playback speed of the CD-ROM drive are actually
 *		implemented.
 *
 *	    6) Microsoft reserves all rights to these functions.
 *		Microsoft reserves the right to add, alter or remove
 *		the specification or implementation of any or all of
 *		these functions at any time, without notice.
 *
 *	    7) Microsoft makes no warranties with respect to the proper
 *		or reliable functioning of any of the functions specified
 *		herein.  Microsoft will not be held liable for any
 *		circumstances arising from the use of these interfaces.
 *
 *
 *	Microsoft Confidential
 *	Copyright (c) Microsoft Corporation 1995
 *	All Rights Reserved
 *
 *	Standards for Recording:
 *	    Redbook: ISO/IEC 908.
 *		- Defines CD-Digital Audio
 *	    Yellowbook: ISO/IEC 10149
 *		- Defines CD-ROM.
 *		- CD-XA is an extension of the Yellowbook
 *	    Orangebook:
 *		- Defines CD-MO and CD-WO recordable media technologies.
 *
 *	    The RSTUVW subchannels are not defined by these standards.
 *	    Some common formats are CD+G and CD+MIDI.
 *
 *	Notes:
 *	  - CDROM_IOCTL_GET_VOLUME_SIZE replaced by CDROM_IOCTL_DISC_INFO
 *	  - CDROM_IOCTL_GET_SESSION_INFO replaced by CDROM_IOCTL_DISC_INFO
 *	  - Structures DWORD aligned
 *	  - Commands which take structures all given a reserved zero DWORD.
 *
 */


/***	Definitions for CD-ROM Disk Addresses
 *
 *	Two standards are available for addressing the disk, known as
 *	the Redbook (MSF) or the High Sierra Group (HSG) addresses.
 *	Redbook addresses are given as Minute, Second, and Frame numbers.
 *	HSG addresses are just logical block addresses, as an unsigned long.
 *	The term "LBA", for "logical block address", is preferred over HSG.
 *	The standard conversion formula between these two formats is
 *	Sector = (((Minute*60) + Second)*75) + Frame;
 */


/***	The MSF_ADDR structure describes a disk address in Redbook format.
 *	The Minute, Second, and Frame are given as binary numbers.
 *	BEWARE: In some cases, the MSF is given as 3 pairs of BCD digits
 *	on disk.   This format only specifies the binary number version.
 */

typedef struct	MSF_ADDR { /* */
    UCHAR   msf_Frame;
    UCHAR   msf_Second;
    UCHAR   msf_Minute;
    UCHAR   msf_Filler;
} MSF_ADDR, *PMSF_ADDR;


/***	The LBA_ADDR typedef describes a disk address in HSG format.
 */

typedef ULONG	LBA_ADDR;


/***	The MSF_LBA union combines both Redbook and HSG formats
 */

typedef union	MSF_LBA { /* */
    MSF_ADDR	msf;
    LBA_ADDR	lba;
} MSF_LBA;


/***	The Addressing Mode Indicator indicates whether LBA or MSF is used.
 */

#define CDROM_ADDR_LBA	0
#define CDROM_ADDR_MSF	1


/***	The CDROM_ADDR structure is used to store addresses of either format.
 */

typedef struct	CDROM_ADDR { /* */
    ULONG   Mode;
    MSF_LBA Address;
} CDROM_ADDR, *PCDROM_ADDR;


/***	CDROM_REGION
 *
 *	The CDROM_REGION structure is used to store addresses for
 *	instructions that require a start and finish address.  The
 *	mode of both addresses must be indicated by the mode flag.
 */

typedef struct	CDROM_REGION { /* */
    ULONG   Mode;
    MSF_LBA StartAddr;
    MSF_LBA EndAddr;
} CDROM_REGION, *PCDROM_REGION;


/***	These macros convert either format of CDROM address to the other.
 *	They are written as expressions rather than functions to simplify
 *	callers.  Converting LBA to MSF format is more difficult, and
 *	requires a temporary variable of type MSF_ADDR.
 */

#define CDROM_MSF_TO_LBA(pcdra) 		\
    (((pcdra)-> Mode = CDROM_ADDR_LBA), 	\
     (((pcdra)-> Address.lba) = (LBA_ADDR) (	   \
       (((pcdra)-> Address.msf.msf_Minute) * 60 +  \
	((pcdra)-> Address.msf.msf_Second)) * 75 + \
       ((pcdra)-> Address.msf.msf_Frame)) - 150))


#define CDROM_LBA_TO_MSF(pcdra,msfTemp) 			\
    (								\
	((pcdra)-> Mode = CDROM_ADDR_MSF),			\
	(((pcdra)-> Address.lba) += 150),			\
	(msfTemp.msf_Frame  = ((pcdra)-> Address.lba) % 75),	\
	(((pcdra)-> Address.lba) /= 75),			\
	(msfTemp.msf_Second = ((pcdra)-> Address.lba) % 60),	\
	(((pcdra)-> Address.lba) /= 60),			\
	(msfTemp.msf_Minute = ((pcdra)-> Address.lba)), 	\
	(msfTemp.msf_Filler = 0),				\
	((pcdra)-> Address.msf.msf_Filler = msfTemp.msf_Filler),\
	((pcdra)-> Address.msf.msf_Minute = msfTemp.msf_Minute),\
	((pcdra)-> Address.msf.msf_Second = msfTemp.msf_Second),\
	((pcdra)-> Address.msf.msf_Frame  = msfTemp.msf_Frame)	\
    )


/***	These macros convert either format of CDROM address to the desired
 *	format.  These are typically used on driver entry to simplify
 *	the driver code so the driver can work with a consistent format.
 */

#define CDROM_CDADDR_TO_LBA(pcdra)					    \
    (((pcdra)-> Mode == CDROM_ADDR_LBA) || CDROM_MSF_TO_LBA(pcdra))


#define CDROM_CDADDR_TO_MSF(pcdra, msfTemp)				    \
    (((pcdra)-> Mode == CDROM_ADDR_MSF) || CDROM_LBA_TO_MSF(pcdra,msfTemp))


/***	IOCTL Command Definitions
 *	IOCTL Command Codes
 *
 *	CDROM_IOCTL_GET_VOLUME_SIZE replaced by CDROM_IOCTL_DISC_INFO
 *	CDROM_IOCTL_GET_SESSION_INFO replaced by CDROM_IOCTL_DISC_INFO
 */

#define CDROM_IOCTL_RESET_DRIVE 	0xCD00	// Reset CD-ROM Drive
#define CDROM_IOCTL_SEEK		0xCD01	// Seek the Read Head
#define CDROM_IOCTL_GET_DEVICE_STATUS	0xCD02	// Get Device Status Info
#define CDROM_IOCTL_GET_SECTOR_SIZE	0xCD03	// Get Current Sector Size
#define CDROM_IOCTL_GET_HEAD_LOCATION	0xCD04	// Get Head Location
#define CDROM_IOCTL_QCHAN_UPC		0xCD05	// Get Media Catalog Number
#define CDROM_IOCTL_QCHAN_ISRC		0xCD06	// Get ISRC for Track
#define CDROM_IOCTL_QCHAN_CURRENT_INFO	0xCD07	// Get Current Q Channel Info
#define CDROM_IOCTL_GET_AUDIO_STATE	0xCD08	// Get Audio Pause State
#define CDROM_IOCTL_GET_AUDIO_CHAN_CTRL 0XCD09	// Get Audio Volume Controls
#define CDROM_IOCTL_SET_AUDIO_CHAN_CTRL 0xCD0A	// Set Audio Volume Controls
#define CDROM_IOCTL_DISC_INFO		0XCD0B	// Get Disk Information
#define CDROM_IOCTL_TRACK_INFO		0XCD0C	// Get Track Parameters
#define CDROM_IOCTL_AUDIO_SUB_CHAN_INFO 0XCD0D	// Get P-W Subchannel Vector
#define CDROM_IOCTL_AUDIO_PLAY		0XCD0E	// Play Audio Selection
#define CDROM_IOCTL_AUDIO_STOP		0XCD0F	// Pause or Stop Audio Play
#define CDROM_IOCTL_AUDIO_RESUME	0XCD10	// Resume Audio Play
#define CDROM_IOCTL_SECTOR_INFO 	0XCD11	// Get Sector Information
#define CDROM_IOCTL_QUERY_SPEED 	0XCD12	// Get Current Device Speed
#define CDROM_IOCTL_QUERY_SPEED_CAPS	0XCD13	// Get Speed Capabilities
#define CDROM_IOCTL_SET_SPEED		0XCD14	// Set New Device Speed
#define CDROM_IOCTL_GENERIC		0XCD15	// Generic Vendor specific IOCTL
#define CDROM_IOCTL_READ_DRIVE_INFO	0XCD16	// Reads driver specific info
#define CDROM_IOCTL_READ_ERROR_STATS	0xCD17	// Reads error statistic info
						// specific to each IOCTL
#define CDROM_IOCTL_WRITE_DRIVE_INFO	0XCD18	// Writes driver specific info
#define CDROM_IOCTL_QUERY_IOCTL_FUNC	0xCF00	// Queries IOCTL support caps


/***	CDROM_IOCTL_RESET_DRIVE - Reset CD-ROM Drive
 *
 *	CDROM_IOCTL_RESET_DRIVE resets and reinitializes the CD-ROM drive.
 *
 *	ENTRY
 *	    Request Packet Address = NULL.
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		The CD-ROM Drive is reset
 *
 *	    Failure
 *		Returns an extended error code.
 *
 */


/***	CDROM_IOCTL_SEEK - Seek the Read Head
 *
 *	CDROM_IOCTL_SEEK relocates the CD-ROM read head to the
 *	position passed in.
 *
 *	Control returns immediately to the caller without waiting for
 *	the seek to be completed.
 *
 *	Further requests for disk activity will wait until the seek
 *	operation is completed.
 *
 *	If the address is not within the range of the disk, a Seek Error
 *	is returned immediately.
 *
 *	This function may be treated as advisory.  On devices which do
 *	not support plain head seeks, CDROM_IOCTL_SEEK may return
 *	success without acutally moving the read head.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_SEEK structure.
 *	    Reserved	- Reserved Zero
 *	    SeekAddr	- Set to sector number to locate the head at.
 *			    This may be in a MSF or LBA format.
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *
 *	    Failure
 *		Returns an extended error code.
 *		    ERROR_UNKNOWN_COMMAND - The command is not supported
 *		    ERROR_INVALID_PARAMETER - The sector number was invalid
 *		    Other Extended Errors may also be returned
 *
 */

typedef struct	CDROM_SEEK { /* */
    ULONG	Reserved;		// Reserved - must be zero
    CDROM_ADDR	SeekAddr;		// Sector number to locate head at.
} CDROM_SEEK, *PCDROM_SEEK;


/***	CDROM_IOCTL_GET_DEVICE_STATUS - Get Device Status Info
 *
 *	CDROM_IOCTL_GET_DEVICE_STATUS returns the current status of the
 *	CD-ROM drive.
 *
 *	This command can be issued at any time to get information about
 *	the current settings on the drive.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_DEVSTAT structure.
 *	    Reserved - Reserved Zero
 *	    DeviceStatus - Don't care
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		Bits of DeviceStatus set according to CDDEVSTAT definitions
 *
 *		Protect Mode Drivers must obey the following:
 *		    The CDDEVSTAT_INTERLEAVE bit must be clear.
 *		    The CDDEVSTAT_REDBOOK_TOO bit must be set.
 *
 *	    Failure
 *		Returns an extended error code.
 *
 *	DeviceStatus Bit Definitions
 *	Bit	Definition
 *	0	0 - Door Closed
 *		1 - Door Open
 *	1	0 - Door Locked
 *		1 - Door Unlocked
 *	2	0 - Supports Only Cooked Reading
 *		1 - Supports Cooked and Raw Reading
 *	3	0 - Read Only
 *		1 - Read/Write
 *	4	0 - Data Read Only
 *		1 - Data Read and Plays Audio and/or Video Tracks
 *	5	0 - No Interleaving Supported
 *		1 - Supports ISO-9660 Interleaving Factors
 *	6	0 - Reserved (Must Be Zero)
 *	7	0 - No Prefetching Supported
 *		1 - Supports Prefetching
 *	8	0 - No Audio Channel Manipulation
 *		1 - Supports Audio Channel Manipulation
 *	9	0 - Supports HSG Addressing Mode
 *		1 - Supports HSG and Redbook Addressing Modes
 *	10	0 - Can Not Read Mode 2 Form 1 Sectors
 *		1 - Can Read Mode 2 Form 1 Sectors
 *	11	0 - Disk Present in Drive
 *		1 - Disk Not Present in Drive
 *	12	0 - Does Not Support R-W Subchannels
 *		1 - Supports R-W Subchannels
 *	13	0 - Drive Not Playing Audio
 *		1 - Drive Playing Audio
 *	14	0 - Drive Does Not Support Adjustable Drive Speeds
 *		1 - Drive Supports Adjustable Drive Speeds
 *	15	0 - Current Data Rate is Standard = 150K/sec
 *		1 - Nonstandard Data Rate in Operation
 *	16-31	Reserved
 */

typedef struct	CDROM_DEVSTAT { /* */
    ULONG	Reserved;		// Reserved - must be zero
    ULONG	DeviceStatus;		// CD-ROM Device Status Indicator
} CDROM_DEVSTAT, *PCDROM_DEVSTAT;


#define CDDEVSTAT_DOOR_OPEN	    0x00000001
#define CDDEVSTAT_DOOR_UNLOCKED     0x00000002
#define CDDEVSTAT_READ_RAW_TOO	    0x00000004
#define CDDEVSTAT_WRITE_TOO	    0x00000008
#define CDDEVSTAT_PLAY_AUDIO_TOO    0x00000010
#define CDDEVSTAT_INTERLEAVE	    0x00000020
#define CDDEVSTAT_RESERVED_6	    0x00000040
#define CDDEVSTAT_PREFETCHING	    0x00000080
#define CDDEVSTAT_AUDIO_MANIPULATE  0x00000100
#define CDDEVSTAT_REDBOOK_TOO	    0x00000200
#define CDDEVSTAT_CDXA		    0x00000400
#define CDDEVSTAT_NO_DISK_IN_DRIVE  0x00000800
#define CDDEVSTAT_RW_CHANNELS_OK    0x00001000
#define CDDEVSTAT_RESERVED_13	    0x00002000
#define CDDEVSTAT_PLAYING_AUDIO     0x00004000
#define CDDEVSTAT_SPEED_ADJUSTABLE  0x00008000
#define CDDEVSTAT_SPEED_NONSTANDARD 0x00010000


/***	CDROM_IOCTL_GET_SECTOR_SIZE - Get Current Sector Size
 *
 *	CDROM_IOCTL_GET_SECTOR_SIZE returns the sector size of
 *	a cooked or raw mode sector.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by the CDROM_GETSECSIZE structure.
 *	    Reserved - Reserved Zero
 *	    ReadMode	- Set to desired read mode
 *			    = CDROM_COOKED_MODE
 *			    = CDROM_RAW_MODE
 *
 *	    SectorSize	- Don't care
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		ReadMode    - Unchanged
 *		SectorSize  - Set to Sector Size Corresponding to Read Mode
 *		    If ReadMode = CDROM_COOKED_MODE - SectorSize = 2048
 *		    If ReadMode = CDROM_RAW_MODE    - SectorSize = 2352
 *
 *	    Failure
 *		Returns an extended error code.
 *
 */

typedef struct	CDROM_GETSECSIZE { /* */
    ULONG	Reserved;		// Reserved - must be zero
    ULONG	ReadMode;		// Cooked/Raw Read Mode Indicator
    ULONG	SectorSize;		// Sector Size
} CDROM_GETSECSIZE, *PCDROM_GETSECSIZE;

#define CDROM_COOKED_MODE   0
#define CDROM_RAW_MODE	    1


/***	CDROM_IOCTL_GET_HEAD_LOCATION - Get Head Location
 *
 *	CDROM_IOCTL_GET_HEAD_LOCATION returns the address of the
 *	current head position
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_GETHEADLOC structure.
 *	    Reserved - Reserved Zero
 *	    HeadAddr.Mode set to desired addressing mode
 *	    HeadAddr.Address = Don't Care
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		HeadAddr.Mode unchanged
 *		HeadAddr.Address set to current head location using the
 *		indicated addressing mode
 *
 *	    Failure
 *		Returns an extended error code.
 *
 */

typedef struct	CDROM_GETHEADLOC   { /* */
    ULONG	Reserved;		// Reserved - must be zero
    CDROM_ADDR	HeadAddr;		// Current CD-ROM Head Location
} CDROM_GETHEADLOC, *PCDROM_GETHEADLOC;


/***	CDROM_IOCTL_QCHAN_UPC - Get Media Catalog Number
 *
 *	CDROM_IOCTL_QCHAN_UPC returns the Media Catalog Number for
 *	the disk.  The Media Catalog number is the UPC/EAN number
 *	for the disk.
 *
 *	This information is stored as a q-mode-2 (ADR=2) Q-channel entry,
 *	as defined by the Yellowbook.
 *
 *	The UPC Code is returned as follows:
 *	    - Q-Control recorded in nibble 0
 *	    - ADR Mode recorded in nibble 1.  Set to 2.
 *	    - 13 digits recorded in nibbles 2-14, in BCD notation
 *	    - Zero recorded in nibbles 15-17
 *	    - A-FRAC field recorded in nibbles 18 and 19
 *
 *	If the Control/ADR byte is zero or if the 13 digits of the UPC
 *	code are all zero, then no catalog number was encoded on the disc.
 *
 *	The two synchronization bits are not included.	The trailing
 *	16 CRC bits are returned as zero.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_GETUPC structure.
 *	    Reserved - Reserved Zero
 *	    The CDROM_GETUPC packet contains no input parameters.
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		All CDROM_GETUPC packet parameters filled in.
 *
 *		The returned Q-channel information is as defined in the
 *		standards for recording.  The 2 leading synchronization
 *		bits are not included in the Q-channel information.
 *
 *		The CRC bytes (the last 2 bytes) are zero, as the CRC
 *		has already been applied in the hardware.
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		Other Extended Errors may also be returned
 *
 */

typedef struct	CDROM_GETUPC   { /* */
    ULONG	Reserved;		// Reserved - must be zero
    UCHAR	QChan[12];		// Q-channel UPC information
} CDROM_GETUPC, *PCDROM_GETUPC;


/***	CDROM_IOCTL_QCHAN_ISRC - Get ISRC for a Track
 *
 *	CDROM_IOCTL_QCHAN_ISRC retrieves the ISRC for a track.
 *
 *	The ISRC gives a unique number to an audio track.  It gives
 *	information such as the country, owner, year of recording, and
 *	the serial number of the recording.  This information is stored
 *	as a q-mode-3 (ADR=3) Q-channel entry, as defined by the Redbook.
 *
 *	The Q-channel ISRC information will not necessarily be available
 *	on the disk.  If it is not available, the Q-channel ISRC
 *	information will be returned as all zeros.
 *
 *	The two synchronization bits are not included.	The trailing
 *	16 CRC bits are returned as zero.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_GETISRC structure.
 *	    Reserved - Reserved Zero
 *	    Track   - Set to track number of the desired ISRC.
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		Track number unchanged
 *		Remainder of CDROM_GETISRC structure filled in
 *
 *		The returned Q-channel information is as defined in the
 *		standards for recording.  The 2 leading synchronization
 *		bits are not included in the Q-channel information.
 *
 *		The CRC bytes (the last 2 bytes) are zero, as the CRC
 *		has already been applied in the hardware.
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		Other Extended Errors may also be returned
 */

typedef struct	CDROM_GETISRC  { /* */
    ULONG	Reserved;		// Reserved - must be zero
    ULONG	Track;			// Desired Track Number
    UCHAR	ISRC_QChan[12]; 	// Q-channel ISRC information
} CDROM_GETISRC, *PCDROM_GETISRC;


/***	CDROM_IOCTL_QCHAN_CURRENT_INFO - Get Current Q Channel Info
 *
 *	CDROM_IOCTL_QCHAN_CURRENT_INFO returns information about the
 *	current position of the media being played.
 *
 *	This function returns the most up-to-date Q-channel address
 *	information available.	The current status of the drive must
 *	not be altered as a result of this operation.  Its intended
 *	purpose is to monitor the location of the read head while
 *	playing audio tracks.
 *
 *	This function should return valid information even when no
 *	audio tracks are playing and the head is stationary.  The
 *	addresses are returned in the Redbook format with binary values.
 *	The track number and Index number are returned as BCD values.
 *
 *	Consult the standards for recording for the format of the
 *	Q-channel frames.
 *
 *	The two synchronization bits are not included.	The trailing
 *	16 CRC bits are returned as zero.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_GETQINFO structure.
 *	    Reserved - Reserved Zero
 *	    CDROM_GETQINFO contents = don't care
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		CDROM_GETQINFO structure filled in for current address
 *
 *		The returned Q-channel information is as defined in the
 *		standards for recording.  The 2 leading synchronization
 *		bits are not included in the Q-channel information.
 *
 *		The CRC bytes (the last 2 bytes) are zero, as the CRC
 *		has already been applied in the hardware.
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		Other Extended Errors may also be returned
 *
 */

typedef struct	CDROM_GETQINFO { /* */
    ULONG	Reserved;		// Reserved - must be zero
    UCHAR	QChan[12];		// Q-channel information
} CDROM_GETQINFO, *PCDROM_GETQINFO;


/***	CDROM_IOCTL_GET_AUDIO_STATE - Get Audio Pause State
 *
 *	CDROM_IOCTL_GET_AUDIO_STATE returns inforation about the current
 *	audio state.
 *
 *	This function returns information about whether audio is playing
 *	or paused on the drive.
 *
 *	If the audio is paused, the Redbook starting and ending addresses
 *	for the resume audio operation are returned.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_AUDSTATE structure.
 *	    Reserved - Reserved Zero
 *	    CDROM_AUDSTATE contents = don't care
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		CDROM_AUDSTATE structure filled in
 *		Bits of AudioFlags set according to CDAUDSTAT definitions
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		Other Extended Errors may also be returned
 *
 *	AudioFlags Bit Definitions
 *
 *	Bit	Definition
 *	0	0 - Audio is not paused
 *		1 - Audio is paused
 *	1	0 - Audio play request is in progress
 *		1 - There are no play requests in progress
 *	1-15	Reserved (must be zero)
 *
 */

typedef struct	CDROM_AUDSTATE { /* */
    ULONG	Reserved;		// Reserved - must be zero
    ULONG	AudioFlags;		// Audio Pause Flags
    MSF_ADDR	StartAddress;		// Starting Address for Audio Resume
    MSF_ADDR	EndAddress;		// Ending Address for Audio Resume
} CDROM_AUDSTATE, *PCDROM_AUDSTATE;

#define CDAUDSTAT_PAUSED    0x0001
#define CDAUDSTAT_PLAYING   0x0002


/***	CDROM_IOCTL_GET_AUDIO_CHAN_CTRL - Get Audio Volume Controls
 *
 *	This function returns the current settins on audio channels.
 *
 *	The default settins are for each output channel to play from
 *	its corresponding input channel, and the volume control to
 *	be set to 0xFF for all channels.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_AUDCTRL structure.
 *	    CDROM_AUDCTRL contents = don't care
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		CDROM_AUDCTRL structure filled in
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		Other Extended Errors may also be returned
 *
 */

typedef struct	CDROM_AUDCTRL  { /* */
    ULONG	Reserved;		// Reserved - must be zero
    UCHAR	AudioCh0Input;		// Input channel for output channel 0
    UCHAR	AudioCh0Volume; 	// Volume control for output channel 0
    UCHAR	AudioCh1Input;		// Input channel for output channel 1
    UCHAR	AudioCh1Volume; 	// Volume control for output channel 1
    UCHAR	AudioCh2Input;		// Input channel for output channel 2
    UCHAR	AudioCh2Volume; 	// Volume control for output channel 2
    UCHAR	AudioCh3Input;		// Input channel for output channel 3
    UCHAR	AudioCh3Volume; 	// Volume control for output channel 3
} CDROM_AUDCTRL, *PCDROM_AUDCTRL;


/***	CDROM_IOCTL_SET_AUDIO_CHAN_CTRL - Set Audio Volume Controls
 *
 *	This function is intended to provide playback control of audio
 *	information on the disk.  It allows input channels on the CD-ROM
 *	drive to be assigned to specific output speaker connections.
 *
 *	The purpose of this function is to allow two independent
 *	channels to be recorded and to manipulate an audio signal so
 *	that the source appears to move.
 *
 *	Output channel 0 is the left channel.
 *	Output channel 1 is the right channel.
 *	Output channel 2 is the left prime channel.
 *	Output channel 3 is the right prime channel.
 *
 *	The two prime channels extend stereo to quadrophonic stereo.
 *
 *	A volume setting of 0 means off.  Drives that do not support
 *	variable audio control will treat a setting of 0 as off and
 *	nonzero as on.	Drives that support less than 256 settings will
 *	break up the 256 settings among settings they can support.
 *
 *	The channels may be cross-mapped by altering the input
 *	channel (0, 1, 2, or 3) for each output channel for special
 *	effects.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_AUDCTRL structure, above.
 *	    Reserved - Reserved Zero
 *	    CDROM_AUDCTRL contents set up for all 4 channels.
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		CDROM_AUDCTRL structure unchanged
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		Other Extended Errors may also be returned
 */


/***	CDROM_IOCTL_DISC_INFO - Get Disk Information
 *
 *	This function returns the TOC (table of contents) information
 *	from the Q-channel in the lead-in track of the disc, which
 *	indicates the starting and ending tracks and the start of
 *	the lead-out track.
 *
 *	The first and last track number are binary values, not BCD.
 *
 *	It is recommended that the information from the TOC be read
 *	in and cached on drive initialization so that when this function
 *	is called, there is not need to interrupt drive play to get this
 *	information.  Note that the first and last track numbers do not
 *	include the lead-in and lead-out tracks of the session.
 *
 *	The SessionIndex is used to get TOC information from disks
 *	with more than one recorded session.  Sessions are the same
 *	as OrangeBook Volumes.	The first session has a SessionIndex
 *	of zero, and the second session has a SessionIndex of one.
 * A SessionIndex of DISC_INFO_LAST_SESSION (-1) requests the disc
 * info for the last session recorded on the disc.
 *
 *	The LogicStartAddr is the logical sector address of the first
 *	data sector of the first track in this session.
 *
 *	For standard Redbook and Yellowbook CD-ROMs, zero (0) is the
 *	only valid SessionIndex value.	In this case, LogicStartAddr
 *	should be returned as zero (0).
 *
 *	Note: The LogicStartAddr is normally used to locate the first
 *	sector of the Volume Recognition Sequence for the indicated
 *	session. The Volume Recognition Sequence for the session is
 *	expected to start at Sector (LogicStartAddr + 16).
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_DISCINFO structure.
 *	    Reserved - Reserved Zero
 *	    SessionIndex - Set to desired session number to query.
 *		SessionIndex = 0 indicates a query for the first session
 *		SessionIndex = 0xFFFFFFFF indicates a query for the last
 *		    session on the disk
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		CDROM_DISCINFO structure filled in for indicated session
 *
 *		If SessionIndex was passed in as 0xFFFFFFFF, it may be
 *		modified to contain the index of the last session on the
 *		disk.  Because not all device drivers must scan through
 *		all the sessions on the disk, this update may or may not
 *		actually take place.
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		ERROR_INVALID_PARAMETER - The session number is invalid
 *		Other Extended Errors may also be returned
 */

typedef struct	CDROM_DISCINFO { /* */
    ULONG	Reserved;		// Reserved - must be zero
    ULONG	FirstTrack;		// First Track of Session
    ULONG	LastTrack;		// Last Track of Session
    MSF_ADDR	LeadOutTrackAddr;	// Address of Lead-out Track of Session
    UCHAR	FirstSession;
    UCHAR	LastSession;
    UCHAR	ReqSession;
    UCHAR	RetSession;
    ULONG	LogicStartAddr; 	// Session Logical Start Acdress
} CDROM_DISCINFO, *PCDROM_DISCINFO;

#define DISC_INFO_LAST_SESSION 0xFF

/***	CDROM_IOCTL_TRACK_INFO - Get Track Parameters
 *
 *	This function takes a binary track number in the range
 *	specified by the first and last track number for the disk
 *	and returns the Redbook address for the starting point of
 *	the track and control information for the track.  The control
 *	information is contained in the 4 most significant bits of the
 *	control information byte.  The definitions of the bit fields
 *	are given below.
 *
 *	It is recommended that the information from the TOC be read
 *	in and cached on drive initialization so that when this function
 *	is called, there is not need to interrupt drive play to get this
 *	information.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_TRACKINFO structure.
 *	    Reserved - Reserved Zero
 *	    TrackNumber - Set to desired track number to query.
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		CDROM_TRACKINFO structure filled in for indicated track
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		ERROR_INVALID_PARAMETER - The track number is invalid
 *		Other Extended Errors may also be returned
 *
 *	TrackControl Bit Definitions
 *
 *	Bit	Definition
 *	0-3	ADR information
 *	4	0 - No preemphasis on audio channels
 *		1 - Preemphasis on audio channels
 *	5	0 - Digital copy prohibited
 *		1 - Digital copy allowed
 *	6	0 - Audio Track
 *		1 - Data Track
 *	7	0 - 2 Audio channels
 *		1 - 4 Audio channels
 */

typedef struct	CDROM_TRACKINFO    { /* */
    ULONG	Reserved;		// Reserved - must be zero
    ULONG	TrackNumber;		// Desired Track Number
    MSF_ADDR	TrackAddr;		// Track Starting Address
    ULONG	TrackControl;		// Track Control Flags (CDTRACK_)
} CDROM_TRACKINFO, *PCDROM_TRACKINFO;

#define CDTRACK_ADRMASK     0x0F	// ADR Information
#define CDTRACK_PREEMPHASIS 0x10	// Audio Preemphasis Indicator
#define CDTRACK_COPYOK	    0x20	// Digital Copy Allowed
#define CDTRACK_DATA	    0x40	// Data Track
#define CDTRACK_4CHANNEL    0x80	// 4 Audio Channels


/***	CDROM_IOCTL_AUDIO_SUB_CHAN_INFO - Get P-W Subchannel Vector
 *
 *	CDROM_IOCTL_AUDIO_SUB_CHAN_INFO retrieves the subchannel data
 *	from the indicated sectors.  If multiple sectors are requested,
 *	the data is copied sequentially.
 *
 *	Each sector contains 96 bytes of subchannle information.  These
 *	bytes do not include the two sync patterns that head a subcoding
 *	block.
 *
 *	Each byte contains 1 bit of information from each of the eight
 *	different subcoding channels.  In this byte, the two most
 *	significant bits (representing channels P and Q) are undefined.
 *
 *	For the subchannel data to be useful, it must be provided during
 *	an AUDIO PLAY operation without interrupting it.  Therefore, when
 *	the requested sectors lie within the current play request, they
 *	must be provided in real-time.	This requires buffering for at
 *	least 1 sector of sub-channel information.  It is recommended
 *	that at least 32 sectors be buffered to give an application time
 *	to process the subchannel data between requests.
 *
 *	Implementation of this command is optional.  An UNKNOWN_COMMAND
 *	error will be returned if this command is not supported by the
 *	driver.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_SUBINFO structure.
 *	    Reserved	- Reserved Zero
 *	    StartAddr	- Set to desired starting starting sector number
 *	    FrameCount	- Set to desired number of subchannel frames to read
 *	    SubChanBuffer - Address of subchannel buffer to fill in
 *	    ValidChannels - don't care
 *
 *	    The size of the SubChanBuffer is 96*FrameCount bytes.
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		StartAddr, FrameCount, SubChanBuffer fields unchanged
 *		ValidChannels - Set to indicate which subchannels are
 *		    were returned as valid through the SubChanBuffer.
 *
 *			ValidChannels Bit Definition
 *			0   0 - W Channel bits Invalid
 *			    1 - W Channel bits Valid
 *			1   0 - V Channel bits Invalid
 *			    1 - V Channel bits Valid
 *			2   0 - U Channel bits Invalid
 *			    1 - U Channel bits Valid
 *			3   0 - T Channel bits Invalid
 *			    1 - T Channel bits Valid
 *			4   0 - S Channel bits Invalid
 *			    1 - S Channel bits Valid
 *			5   0 - R Channel bits Invalid
 *			    1 - R Channel bits Valid
 *			6   0 - Q Channel bits Invalid
 *			    1 - Q Channel bits Valid
 *			7   0 - P Channel bits Invalid
 *			    1 - P Channel bits Valid
 *
 *		SubChanBuffer filled in.
 *
 *		Each of the returned subchannel frames in the SubChanBuffer
 *		is 96 bytes in length.	This does not include the two sync
 *		patterns that head a subcoding block.
 *
 *		Each byte in the returned SubChanBuffer contains a single
 *		bit from each of the PQRSTUVW subchannels.  The most
 *		significant bit (Bit 7) represents channel P, and the
 *		least significant bit (Bit 0) represents channel W.
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		ERROR_INVALID_PARAMETER - The indicated sectors are not valid
 *		Other Extended Errors may also be returned
 *
 */

typedef struct	CDROM_SUBINFO  { /* */
    ULONG	Reserved;		// Reserved - must be zero
    MSF_ADDR	StartAddr;		// Starting Subchannel Info Address
    ULONG	FrameCount;		// Number of Subchannel Frames
    PVOID	SubChanBuffer;		// Subchannel Buffer Pointer
    UCHAR	ValidChannels;		// Valid Channels bit mask
    UCHAR	Filler[3];		// Alignment filler
} CDROM_SUBINFO, *PCDROM_SUBINFO;


/***	CDROM_IOCTL_AUDIO_PLAY - Play Audio Selection
 *
 *	CDROM_IOCTL_AUDIO_PLAY will cause the driver to play the selected
 *	audio tracks until the requested sectors have been exhausted or
 *	until the play is interrupted by a CDROM_IOCTL_AUDIO_STOP request.
 *
 *	Control returns immediately to the caller after the command is
 *	received.  The paused bit and the starting and ending play
 *	addresses in the CDROM_IOCTL_GET_AUDIO_STATE function will also
 *	be updated by this call.
 *
 *	The bit in the device status will also be set indicating that an
 *	audio play operation is in progress on this drive.
 *
 *	If the drive does not support playing Audio, ERROR_UNKNOWN_COMMAND
 *	will be returned.
 *
 *	If the starting or ending addresses are outside the range of
 *	tracks that can be played, ERROR_INVALID_PARAMETER will be returned.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_AUDPLAY structure.
 *	    Reserved	    - Reserved Zero
 *	    PlayRegion	    - Start/stop addresses for play
 *	    PlayFlags	    - Reserved, must be zero
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		CDROM_AUDPLAY structure filled in for indicated track
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		ERROR_INVALID_PARAMETER - The selected addresses are invalid
 *		    for audio play.
 *		Other Extended Errors may also be returned.
 *
 */

typedef struct	CDROM_AUDPLAY  { /* */
    ULONG	Reserved;		// Reserved - must be zero
    CDROM_REGION PlayRegion;		// Start/end play addresses
    ULONG	PlayFlags;		// Reserved - must be zero
} CDROM_AUDPLAY, *PCDROM_AUDPLAY;


/***	CDROM_IOCTL_AUDIO_STOP - Pause or Stop Audio Play
 *
 *	CDROM_IOCTL_AUDIO_STOP either pauses or stops the drive unit.
 *
 *	If the drive unit is in play mode, the function pauses the
 *	drive unit and updates starting and ending addresses for the
 *	CDROM_IOCTL_AUDIO_RESUME function.
 *
 *	If the drive unit is in paused mode, the function resets the
 *	starting and ending addresses for the CDROM_IOCTL_AUDIO_RESUME
 *	function.
 *
 *	The CDAUDSTAT_PAUSED bit returned by CDROM_IOCTL_GET_AUDIO_STATE
 *	function will be set.  The CDDEVSTAT_PLAYING_AUDIO bit returned
 *	by the CDROM_IOCTL_GET_DEVICE_STATUS function will be cleared.
 *
 *	ENTRY
 *	    Request Packet Address = NULL.
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		Other Extended Errors may also be returned.
 *
 */


/***	CDROM_IOCTL_AUDIO_RESUME - Resume Audio Play
 *
 *	CDROM_IOCTL_AUDIO_RESUME causes the drive to resume play from
 *	where it was previously stopped.
 *
 *	The drive unit will resume playing from the starting address
 *	indicated by the CDROM_IOCTL_GET_AUDIO_STATE function.
 *
 *	The CDAUDSTAT_PAUSED bit returned by CDROM_IOCTL_GET_AUDIO_STATE
 *	function will be cleared.  The CDDEVSTAT_PLAYING_AUDIO bit returned
 *	by the CDROM_IOCTL_GET_DEVICE_STATUS function will be set.
 *
 *	ENTRY
 *	    Request Packet Address = NULL.
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		Other Extended Errors may also be returned.
 *
 */


/***	CDROM_IOCTL_SECTOR_INFO - Get Sector Information
 *
 *	CDROM_IOCTL_SECTOR_INFO indicates recording information
 *	about the sector.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_SECINFO structure.
 *	    Reserved - Reserved Zero
 *	    SectorAddr - Set to the address of the sector to query
 *	    SectorState = don't care
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		SectorAddr unchanged
 *		Bits of SectorState set according to CDSECINFO definitions
 *
 *		Bit	Definition
 *		0	0 - Sector is recorded
 *			1 - Sector is not recorded
 *		1-7	Reserved (must be zero)
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		ERROR_INVALID_PARAMETER - The sector number is invalid
 *		Other Extended Errors may also be returned
 *
 */

typedef struct	CDROM_SECINFO  { /* */
    ULONG	Reserved;		// Reserved - must be zero
    CDROM_ADDR	SectorAddr;		// Sector Address to Query
    ULONG	SectorState;		// Sector Recording State
} CDROM_SECINFO, *PCDROM_SECINFO;

#define CDSECINFO_UNRECORDED	0x01


/***	CDROM_IOCTL_QUERY_SPEED - Get Current Device Speed
 *
 *	CDROM_IOCTL_QUERY_SPEED determines the current device speed
 *	for the drive.
 *
 *	This is meant to be an approximation.  For devices which do not
 *	support variable device speeds, or tend to have speeds which
 *	vary over time, a best-guess at the speed should be made.
 *	The standard data transfer rate for CD-ROMs is 150 kilobytes
 *	per second, which is a common best-guess.
 *
 *	(For the purposes of this command, devices should assume they
 *	are reading Mode 1 or Mode 2 Form 1 sectors, of 2048 bytes each.)
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_SPEED structure.
 *	    Reserved - Reserved Zero
 *	    CurrentDeviceSpeed = don't care
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		CDROM_SPEED structure filled in
 *		DevSpeed is set to the current data transfer rate for
 *		    the CD-ROM device, in kilobytes/second.
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		ERROR_INVALID_PARAMETER - The sector number is invalid
 *		Other Extended Errors may also be returned
 *
 */

typedef struct	CDROM_SPEED { /* */
    ULONG	Reserved;		// Reserved - must be zero
    ULONG	DevSpeed;		// Current Device Speed
} CDROM_SPEED, *PCDROM_SPEED;


/***	CDROM_IOCTL_QUERY_SPEED_CAPS - Get Speed Capabilities
 *
 *	CDROM_IOCTL_QUERY_SPEED_CAPS determines the device speed
 *	capabilities for the drive which best fits the device speed
 *	which is desired by the application.
 *
 *	For CD-ROM devices which do not support variable device speeds,
 *	a single speed is chosen which is a best-guess approximation
 *	of the device speed.
 *
 *	For CD-ROM devices which support infinitely variable device
 *	speeds, the device should choose a reasonable subset of the
 *	supported device speeds so that applications do not waste
 *	time enumerating all the possible device speeds.
 *
 *	If the requested device speed can be supported, then
 *	SpeedLowerBound will be set to SpeedDesired upon return.
 *
 *	To change the current device speed, use CDROM_IOCTL_SET_SPEED.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_SPEEDCAP structure.
 *	    Reserved - Reserved Zero
 *	    SpeedDesired = Desired device speed, in Kilobytes/second
 *	    SpeedLowerBound = don't care
 *	    SpeedUpperBound = don't care
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		CDROM_SPEEDCAP structure filled in
 *
 *		SpeedDesired unchanged
 *		SpeedLowerBound set to closest speed <= desired speed
 *		    If the SpeedDesired is at or below the minimum
 *		    device speed, then SpeedLowerBound will be set to
 *		    the minimum supported device speed.
 *
 *		SpeedUpperBound set to closest speed > desired speed
 *		    If the SpeedDesired is at or above the maximum
 *		    device speed, then SpeedUpperBound will be set
 *		    to maximum supported device speed.
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		Other Extended Errors may also be returned
 *
 */

typedef struct	CDROM_SPEEDCAP	{ /* */
    ULONG	Reserved;		// Reserved - must be zero
    ULONG	SpeedDesired;		// Desired Speed Setting
    ULONG	SpeedLowerBound;	// Lower Speed Boundary
    ULONG	SpeedUpperBound;	// Upper Speed Boundary
} CDROM_SPEEDCAP, *PCDROM_SPEEDCAP;


/***	CDROM_IOCTL_SET_SPEED - Set New Device Speed
 *
 *	CDROM_IOCTL_SET_SPEED sets the desired CD-ROM device speed.
 *
 *	ENTRY
 *	    IOCTL Packet format specified by CDROM_SPEEDSET structure.
 *	    Reserved - Reserved Zero
 *	    SpeedMode set to one of the following:
 *		CDSPEED_VARY - Ignore previous speed setting requests.
 *		    The CD-ROM device may choose the speed at will.
 *
 *		CDSPEED_STANDARD - Set the device speed to the standard
 *		    speed of 150 K/second.  (Ignore SpeedDesired)
 *
 *		CDSPEED_MAXIMUM - Set the device speed to the maximum
 *		    device speed for the drive.  (Ignore SpeedDesired)
 *
 *		CDSPEED_GIVEN - Set the device speed to the speed
 *		    given by SpeedDesired.
 *
 *	    SpeedDesired is set to desired device speed
 *
 *	EXIT
 *	    Success
 *		Returns NO_ERROR
 *		CDROM_SPEEDSET unchanged
 *
 *	    Failure
 *		Returns an extended error code.
 *		ERROR_UNKNOWN_COMMAND - The command is not supported
 *		ERROR_INVALID_PARAMETER - The desired speed is not supported
 *		Other Extended Errors may also be returned
 *
 */

typedef struct	CDROM_SPEEDSET { /* */
    ULONG	Reserved;		// Reserved - must be zero
    ULONG	SpeedMode;		// Speed Setting Mode
    ULONG	SpeedDesired;		// Set to desired speed
} CDROM_SPEEDSET, *PCDROM_SPEEDSET;

#define CDSPEED_VARY	    0		// Set to any variable speed
#define CDSPEED_STANDARD    1		// Set to standard 150K/sec
#define CDSPEED_MAXIMUM     2		// Set to maximum speed
#define CDSPEED_GIVEN	    3		// Set to given speed

