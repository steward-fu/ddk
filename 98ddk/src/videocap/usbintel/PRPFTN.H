
#ifndef __PRPFTN_H__
#define __PRPFTN_H__

#include "prpobj.h"

//
// Function declarations (PRPOBJ.C).
//
PVOID   
INTELCAM_GetAdapterPropertyTable(
    PULONG NumberOfArrayEntries
    ); 

BOOLEAN
GetPropertyCurrent(
    IN PINTELCAM_DEVICE_CONTEXT pDC,
    OUT PVOID pProperty,
    IN REQUEST ReqID
    );

BOOLEAN
GetPropertyDefault(
    IN PINTELCAM_DEVICE_CONTEXT pDC,
    OUT PVOID pProperty,
    IN REQUEST ReqID
    );


BOOLEAN
SetPropertyCurrent(
    IN PINTELCAM_DEVICE_CONTEXT pDC,
    IN PVOID pProperty,
    IN REQUEST ReqID
    );

VOID
SetPropertyDefaults(
    IN PINTELCAM_DEVICE_CONTEXT pDC
    );

NTSTATUS
FormPropertyData(
    IN OUT PHW_STREAM_REQUEST_BLOCK pSrb,
    IN PVOID pData,
	IN REQUEST ReqID
    );


//
// Function declarations (PRPGET.C).
//
NTSTATUS
INTELCAM_GetCameraProperty(
    PINTELCAM_DEVICE_CONTEXT DeviceContext,
    PHW_STREAM_REQUEST_BLOCK pSrb
    );

NTSTATUS
GetPropertyCtrl(
    IN REQUEST ReqID,
    IN PINTELCAM_DEVICE_CONTEXT pDC,
    PHW_STREAM_REQUEST_BLOCK pSrb
    );


NTSTATUS
INTELCAM_GetVideoControlProperty(
    PINTELCAM_DEVICE_CONTEXT pDeviceContext,
    PHW_STREAM_REQUEST_BLOCK pSrb
    );


//
// Function declarations (PRPSET.C).
//
NTSTATUS
INTELCAM_SetCameraProperty(
    PINTELCAM_DEVICE_CONTEXT DeviceContext,
    PHW_STREAM_REQUEST_BLOCK pSrb
    );


NTSTATUS
SetPropertyCtrl(
    IN REQUEST ReqID,
    IN PINTELCAM_DEVICE_CONTEXT pDC,
    IN OUT PHW_STREAM_REQUEST_BLOCK pSrb
    );


NTSTATUS
INTELCAM_SetVideoControlProperty(
    PINTELCAM_DEVICE_CONTEXT pDeviceContext,
    PHW_STREAM_REQUEST_BLOCK pSrb
    );


//
// Function declarations (PRPMANF.C).
//
NTSTATUS
INTELCAM_GetCustomProperty(
    PINTELCAM_DEVICE_CONTEXT DeviceContext,
    PHW_STREAM_REQUEST_BLOCK pSrb
    );


NTSTATUS
INTELCAM_SetCustomProperty(
    PINTELCAM_DEVICE_CONTEXT DeviceContext,
    IN OUT PHW_STREAM_REQUEST_BLOCK pSrb
    );

NTSTATUS
FormCustomPropertyData(
    IN OUT PHW_STREAM_REQUEST_BLOCK pSrb,
    IN PVOID pData,
	IN REQUEST_CUSTOM ReqID
    );

#endif
