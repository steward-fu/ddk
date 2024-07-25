/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/****************************************************************************\
*
* SGD - scatter/gather descriptor
*
\****************************************************************************/

/* equivalent names:                                                        */
/*                                                                          */
/* old rlh.h/strat2.h   old rlx.h               new sgd.h                   */
/* ==================   ==================      ==================          */
/* sg_bufferptr                                 sg_buffer_ptr               */
/* sg_buffersize                                sg_buffer_size              */

/* H2INCSWITCHES -t */

// NOTE: The scatter/gather descriptor may be in one of two forms, depending on
//       usage. The orders of fields in the two forms differ.

typedef union _SGD {

// The BlockDev style struct is used if IORF_SCATTER_GATHER is set. IOR_buffer_ptr is
// the linear address of an array of s/g descriptors, whose end is marked by a struct
// whose first field (SG_buff_size) is zero. If IORF_CHAR_COMMAND is set, the size is
// in bytes. Otherwise, it is in sectors.

	struct {			// blockdev style
		ULONG SG_buff_size;
		ULONG SG_buff_ptr;
	} bd;

// If IORF_DOUBLE_BUFFER is set, IOR_sgd_lin_phys is the address of an array of 
// the following structures (the addresss may be linear or physical depending on
// demand bit DCB_dmd_phys_sgd_ptr at the time of calldown insertion). The number
// of structs in the array is contained in IOR_num_sgds.

	struct {			// double buffer style
		ULONG SG_buff_ptr;
		ULONG SG_buff_size;
	} sg;

} SGD, *PSGD;
