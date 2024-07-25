// Characteristics.h - file generated by the wizard
//====================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2002 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//====================================================================

#ifndef __CHARACTERISTICS_H__
#define __CHARACTERISTICS_H__

// Declare Optional NDIS MiniportXxx handlers this driver supports

KNDIS_MINIPORT_HANDLER(USBNICAdapter, CheckForHang)
KNDIS_MINIPORT_HANDLER(USBNICAdapter, Send)
KNDIS_MINIPORT_HANDLER(USBNICAdapter, ReturnPacket)
	

// Driver version
#define	USBNIC_VERSION_MAJOR			1
#define	USBNIC_VERSION_MINOR			0

// Medium Specific

#define KNDIS_MEDIUM_INCLUDE_802_3		/* forces OID handlers inclusion */
#define	USBNIC_MEDIUM_TYPE			NdisMedium802_3 
#define	USBNIC_802_3_MAX_LIST_SIZE	32 /* NDIS Test requirement (>=) */				


// OID Constants
#define	USBNIC_MAX_LOOKAHEAD		256
#define	USBNIC_MAX_FRAMESIZE		1500		// w/o header
#define	USBNIC_MAX_TOTAL_SIZE		1514		// incl. header
#define	USBNIC_MAX_LINKSPEED		10000000L	// 10 Mbps
#define	USBNIC_TRANSMIT_BLOCK		4096
#define	USBNIC_RECEIVE_BLOCK		4096

#define USBNIC_TX_BUFFER_SPACE	4096				
#define USBNIC_RX_BUFFER_SPACE	4096

#define	USBNIC_MAX_SEND_PACKETS	1

#define	USBNIC_VENDOR_DESCRIPTION	"USBNIC NDIS Miniport Driver"


#define	USBNIC_TX_IRPPOOL_SIZE	32
#define	USBNIC_RX_IRPPOOL_SIZE	32
#define USBNIC_CNTRL_IRPPOOL_SIZE 8

#endif // __CHARACTERISTICS_H__
 