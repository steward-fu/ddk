//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
// file generated by the wizard

#ifndef __CHARACTERISTICS_H__
#define __CHARACTERISTICS_H__

///////////////////////////////////////////////////////////////
// Driver Build Options


// Device Creation Option: set to 0 for W98 target, 1 for W2K/XP 
#ifndef VDEV_WIN2K
# define VDEV_WIN2K	1
#endif

// C++ Exception Support Option: set to 1 to include EH code.
// NOTE:
//	1. Use "Win32 NDIS 5 EH Demo" Configuration to build the demo
//	2  This is a contrived demo with a sole goal of demonstrating C++ EH syntax
//	   used in a driver. 
#ifndef VDEV_EH_DEMO
# define VDEV_EH_DEMO	0
#endif




// Declare Optional NDIS MiniportXxx handlers this driver supports

KNDIS_MINIPORT_HANDLER(VdevAdapter, CheckForHang)
KNDIS_MINIPORT_HANDLER(VdevAdapter, Send)
KNDIS_MINIPORT_HANDLER(VdevAdapter, ReturnPacket)
	

// Driver version
#define	VDEV_VERSION_MAJOR			1
#define	VDEV_VERSION_MINOR			0

// Medium Specific

#define KNDIS_MEDIUM_INCLUDE_802_3		/* forces OID handlers inclusion */
#define	VDEV_MEDIUM_TYPE			NdisMedium802_3 
#define	VDEV_802_3_MAX_LIST_SIZE	32 /* NDIS Test requirement (>=) */				


// OID Constants
#define	VDEV_MAX_LOOKAHEAD		256
#define	VDEV_MAX_FRAMESIZE		1500		// w/o header
#define	VDEV_MAX_TOTAL_SIZE		1514		// incl. header
#define	VDEV_MAX_LINKSPEED		10000000L
#define	VDEV_TRANSMIT_BLOCK		256
#define	VDEV_RECEIVE_BLOCK		256

#define VDEV_TX_BUFFER_SPACE	16000				
#define VDEV_RX_BUFFER_SPACE	16000

#define	VDEV_MAX_SEND_PACKETS	4

#define	VDEV_VENDOR_DESCRIPTION	"VDEV NDIS Miniport Driver"


#define	VDEV_TX_IRPPOOL_SIZE	8
#define	VDEV_RX_IRPPOOL_SIZE	8



// TODO: 
// Add other #defines and/or KNdisAdapterTraits<VdevAdapter> specializations as needed

#endif // __CHARACTERISTICS_H__
 