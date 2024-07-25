/*
	Pagefile.h

   (C) Copyright MICROSOFT Corp., 1988-1990
   (c) Copyright Compuware Corporation, 1994

*/

#ifndef __pagefile_h_
#define __pagefile_h_

// Commands to PageFile_Read_Or_Write

#define	PF_READ_DATA	0		// Read a page
#define	PF_WRITE_DATA	1		// Write a page

// Flags to PageFile_Read_Or_Write

#define	PF_LOW_PRIORITY     	0
#define	PF_HIGH_PRIORITY    	1
#define	PF_HIGH_PRIORITY_BIT	0


// Paging Types

#define	PF_PAGING_NONE	1		// Paging not active
#define	PF_PAGING_DOS	2		// Paging I/O via DOS or Int 13h
#define	PF_PAGING_HW	3		// Paging direct to hardware


// Read/Write I/O Call Back Status

#define	PFS_SUCCESS	0
#define	PFS_FAILURE	1
#define	PFS_CANCELED	11h

// VxD Service Table

Begin_Service_Table (PAGEFILE)

Declare_Service ( PageFile_Get_Version )
Declare_Service ( PageFile_Init_File )
Declare_Service ( PageFile_Clean_Up )
Declare_Service ( PageFile_Grow_File )
Declare_Service ( PageFile_Read_Or_Write )
Declare_Service ( PageFile_Cancel )
Declare_Service ( PageFile_Test_IO_Valid )
Declare_Service ( PageFile_Get_Size_Info )
Declare_Service ( PageFile_Set_Async_Manager )
Declare_Service ( PageFile_Call_Async_Manager )

End_Service_Table ( PAGEFILE )

#endif		//  __pagefile_h_
