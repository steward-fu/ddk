/*

     (C) Copyright MICROSOFT Corp., 1990
     (c) Copyright Compuware Corporation, 1994

*/

#ifndef __blockdev_h_
#define __blockdev_h_


//
//   Block Device Descriptor data structrue
//

#define BD_PRIV_DATA_SIZE	0x30

#define BD_MAJOR_VER		3
#define BD_MINOR_VER		0x0A


typedef struct BlockDev_Device_Descriptor{
	DWORD BDD_Next;
	BYTE  BDD_Major_Version; // INIT <BD_Major_Ver>
	BYTE  BDD_Minor_Version; // INIT <BD_Minor_Ver>
	BYTE  BDD_Device_Type;
	BYTE  BDD_Int_13h_Number;
	DWORD BDD_Flags;
	DWORD BDD_Name_Ptr;
	DWORD BDD_Max_Sector[2];
	DWORD BDD_Sector_Size;
	DWORD BDD_Num_Heads;
	DWORD BDD_Num_Cylinders;
	DWORD BDD_Num_Sec_Per_Track;
	DWORD BDD_Sync_Cmd_Proc;
	DWORD BDD_Command_Proc;
	DWORD BDD_Hw_Int_Proc; // INIT <0>
	BYTE  BDD_Reserved_BlockDev[BD_PRIV_DATA_SIZE];
}  _BlockDev_Device_Descriptor, BDD, *PBDD;


typedef struct BlockDev_Command_Block{
	DWORD BD_CB_Next;
	WORD  BD_CB_Command;
	WORD  BD_CB_Cmd_Status;
	DWORD BD_CB_Flags;
	DWORD BD_CB_Cmd_Cplt_Proc;
	DWORD BD_CB_Sector[2];
	DWORD BD_CB_Count;
	DWORD BD_CB_Buffer_Ptr;
	DWORD BD_CB_Reserved_Client;
	DWORD BD_CB_Reserved_BlockDev;
	DWORD BD_CB_Reserved_FastDisk;
#ifdef WIN40
	DWORD BD_CB_Spare[5 ];

	DWORD BD_CB_Req_Req_Handle;	/* requestor provided request	    */
					/* handle. often is a pointer to    */
					/* to this ior or its containing    */
					/* iop.				    */

	DWORD	 BD_CB_Req_Vol_Handle;	/* requestor provided media handle  */
					/* designating the media to perform */
					/* the function on.		    */

	DWORD	 BD_CD_SGD_Lin_Phys;	/* pointer to first sgd. note that  */
					/* this is either a linear or	    */
					/* address, depending on the needs  */
					/* of the drivers, as indicated     */
					/* via the demand bits.		    */

	BYTE	 BD_CB_Num_SGDs;	/* number of sgds to process	    */
	BYTE	 BD_CB_Vol_Designtr;	/* numeric representation of the    */
					/* drive letter designating the     */
					/* volume to perform the function   */
					/* on.				    */
	WORD	 BD_CB_reserved_1;	/* reserved to force alignment - must be zero */
#endif
}  _BlockDev_Command_Block, BCB, *PBCB;

//
//   Flags for BDD_Flags field
//
#define BDF_INT13_DRIVE		0x01
#define BDF_INT13_DRIVE_BIT	0

#define BDF_WRITEABLE		0x02
#define BDF_WRITEABLE_BIT	1

#define BDF_REMOVABLE		0x04
#define BDF_REMOVABLE_BIT	2

#define BDF_REMOTE		0x08
#define BDF_REMOTE_BIT		3

#define BDF_SERIAL_CMD		0x10
#define BDF_SERIAL_CMD_BIT	4

#define BDF_CACHE		0x20
#define BDF_CACHE_BIT		5

#ifdef WIN40

//
// New flags from IOR follow.
//

#define BDF_BYPASS_QUEUE	0x800
#define BDF_BYPASS_QUEUE_BIT	11

#define BDF_VERSION_002		0x0400
#define BDF_VERSION_002_BIT	10

#endif

//
//   Device types for BDD_Device_Type field
//
#define BDT_360K_5_INCH_FLOPPY		0x00
#define BDT_1200K_5_INCH_FLOPPY 	0x01
#define BDT_720K_3_INCH_FLOPPY		0x02
#define BDT_SINGLE_DENS_8_INCH		0x03
#define BDT_DOUBLE_DENS_8_INCH		0x04
#define BDT_FIXED_DISK			0x05
#define BDT_TAPE_DRIVE			0x06
#define BDT_OTHER			0x07


#ifdef WIN40
typedef struct BlockDev_Scatter_Gather{
	ULONG BD_SG_Count;
	ULONG BD_SG_Buffer_Ptr;
}  _BlockDev_Scatter_Gather	;
#endif


//
//   Commands
//   NOTE:  Commands 0x8000h-FFFFh are reserved for device specific command
//
#define	BDC_Read 0
#define	BDC_Write 1
#define BDC_Verify 2
#define BDC_Cancel 3

#ifdef WIN40
//
// VERSION 2 Commands follow
//

#define  BDC_Lock_Volume	0x09
#define  BDC_Unlock_Volume	0x0a
#endif

//
//   Equates for command status. All codes below 10h imply successful completion
//   Error code value are > 10h
//
#define  BDS_SUCCESS			0		// Completed successfully
#define  BDS_SUCCESS_WITH_RETRIES	1		// Completed successfully after retries
#define  BDS_SUCCESS_WITH_ECC		2		// Successful after error correction

#define  BDS_FIRST_ERROR_CODE		0x10		// first error code value
#define  BDS_INVALID_SECTOR_NUMBER	0x10		// Invalid sector number
#define  BDS_CANCELED			0x11		// Command was canceled
#define  BDS_CANCELLED			0x11		// Command was canceled
#define  BDS_CMD_IN_PROGRESS		0x12		// Can't cancel cmd in progress
#define  BDS_INVALID_CMD_PTR		0x13		// Cancel of invalid cmd pointer
#define  BDS_MEDIA_ERROR		0x14		// Read/Write failed
#define  BDS_DEVICE_ERROR		0x15		// Device/Adapter failed
#define  BDS_INVALID_COMMAND		0x16

#ifdef WIN40
#define	 BDS_NO_DEVICE		0x17  /* physical or logical device nonexistant */
#define  BDS_NO_MEDIA		0x18  /* media removed from device */
#define  BDS_UNCERTAIN_MEDIA	0x19  /* media may have changed */
#define  BDS_UNREC_ERROR 	0x1a  /* un-recoverable error */
#define  BDS_HW_FAILURE		0x1b  /* general hardware failure */
#define  BDS_UNFORMATTED_MEDIA	0x1c  /* unformatted media */
#define  BDS_MEMORY_ERROR	0x1d  /* error allocating dma buffer space for i/o*/
#define  BDS_TIME_OUT		0x1e  /* drive time-out */
#define  BDS_WRITE_PROTECT	0x1f  /* write protect error */
#define  BDS_NOT_READY		0x20  /* device not ready */
#define  BDS_BUSY		0x21  /* device is busy */
#endif


//
//   Flags for commands
//
#define  BDCF_HIGH_PRIORITY		0x01
#define  BDCF_HIGH_PRIORITY_BIT		0

#define  BDCF_SCATTER_GATHER		0x02
#define  BDCF_SCATTER_GATHER_BIT	1

#define  BDCF_DONT_CACHE		0x04
#define  BDCF_DONT_CACHE_BIT		2

#ifdef WIN40
#define BDCF_BYPASS_VOLTRK_BIT		3
#define BDCF_BYPASS_VOLTRK		(1 << BDCF_BYPASS_VOLTRK_BIT)

#define	BDCF_SWAPPER_IO	0x0020	/* request is from the swapper */

#define  BDCF_DOUBLE_BUFFER	  0x0040  /* indicates that the request is to be completed */
/* via double buffering.  client must */
/* ensure that both the sgd buffer */
/* pointers and IOR_sgd_lin_phys are */
/* virtual pointers. */

#define  BDCF_DOUBLE_BUFFER_BIT   6

#define	BDCF_BYPASS_QUEUE_BIT	11

#define 	BDCF_BYPASS_QUEUE	  (1 << BDCF_BYPASS_QUEUE_BIT)  
/* request should bypass IOS queuing */
/* should be used only dragon drivers */
#define	BDCF_LOGICAL_START_SECTOR_BIT	19
#define	BDCF_LOGICAL_START_SECTOR	  (1 << BDCF_LOGICAL_START_SECTOR_BIT)  
/* request contains the logical starting */
/* sector, rather than the physical  */

#endif

//
//   Equates for synchronous commands
//
#define  BD_SC_GET_VERSION		0x0000


//
//   Equates for error returns from synchronous command
//
#define  BD_SC_ERR_INVALID_CMD		0x0001


/*
   Value specified in CX register when API call-out Int 0x2F executed
   The Hw_Detect_Start and End APIs are used by block devices to notify
   TSR and DOS device driver software that they are performing hardware
   detection. This may, for example, disable a write-behind cache.
*/
#define  BLOCKDEV_API_HW_DETECT_START	0x0001
#define  BLOCKDEV_API_HW_DETECT_END	0x0002

/*
   The Int13_Chain_Check API is executed by Int 13h block devices to check
   to see if the software that has hooked DOS's ROM BIOS int 13h address
   is aware of BlockDev and it is OK to load block device drivers.  If it
   is OK for block devices such as WDCTRL to load even though the DOS Int 13h
   chain has been modified, the hook code should zero the CX register and
   iret from this call.
*/
#define	 BLOCKDEV_API_INT13_CHAIN_CHECK	0x0003

#endif		 // __blockdev_h_
