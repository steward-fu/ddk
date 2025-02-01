//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

#ifndef __CAPMAIN_H__
#define __CAPMAIN_H__

#ifdef DEBUG
// Change this at compile time or in a debugger to change the amount
// of debug spew. Any strings that start with a single quote show up in
// the buffer maintained by ntkern. Type '.ntkern' in the debugger to 
// see its menu.
extern int DebugLevel;
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "ddkmapi.h"

#include "i2script.h"

#define FIELDOFFSET(type, field)        ((int)(&((type *)1)->field)-1)

class Device;

typedef enum {
    STREAM_VideoCapture,
    STREAM_VPVideo,
    STREAM_VBICapture,
    STREAM_VPVBI,
    STREAM_AnalogVideoInput
}; 

typedef enum {
    ChangeComplete,
    Starting,
    Closing,
    Running,
    Pausing,
    Stopping
};

/*****************************************************************************
*
* The following structures are samples of information that could be used in
* a device extension structure
*
*****************************************************************************/
// forward definition
typedef struct _STREAMEX *PSTREAMEX;

//
// definition of the full HW device extension structure This is the structure
// that will be allocated in HW_INITIALIZATION by the stream class driver
// Any information that is used in processing a device request (as opposed to
// a STREAM based request) should be in this structure.  A pointer to this
// structure will be passed in all requests to the minidriver. (See
// HW_STREAM_REQUEST_BLOCK in STRMINI.H)
//

typedef struct _HW_DEVICE_EXTENSION {
    // Misc
    PDEVICE_OBJECT          PDO;                    // Physical Device Object
    CI2CScript              CScript;
    CI2CScript              *pI2cScript;
    UINT                    i2cExpanderAddr;
    UINT                    chipAddr;
    UINT                    chipID;
    UINT                    chipRev;
    UINT                    outputEnablePolarity;
    Device                  *device;
    UINT                    openStreams;

    // Video port handles
    // Passed down from ring 3
    
    ULONG                   numRing3SurfaceHandles;
    ULONG                   *ring3SurfaceHandles;

    ULONG                   numRing3VBISurfaceHandles;
    ULONG                   *ring3VBISurfaceHandles;

    ULONG                   ring3VideoPortHandle;
    ULONG                   ring3DirectDrawHandle;

    HANDLE                  ring0VideoPortHandle;
    HANDLE                  ring0DirectDrawHandle;
    HANDLE                  ring0VBISurfaceHandle;
    HANDLE                  ring0SurfaceHandle;

    // shared between full-screen DOS and res changes
    BOOL                    preEventOccurred;
    BOOL                    postEventOccurred;

    // Channel Change information
    KS_TVTUNER_CHANGE_INFO  TVTunerChangeInfo;
    BOOL                    TVTunerChanged;
    PHW_STREAM_REQUEST_BLOCK    TVTunerChangedSrb;
    BOOL                    VBICaptureStreamConnected;

    // Boolean indicator for new VBI info
    BOOL                    VBIInfoChanged;

    LIST_ENTRY              adapterSrbQueue;
    KSPIN_LOCK              deviceExtensionLock;
    BOOL                    processingAdapterSrb;
    BOOL                    queueInitialized;
    
    // Need this to renegotiate VP params
    PSTREAMEX                   pVpStrmEx;
    PSTREAMEX                   pVideoCaptureStrmEx;
} HW_DEVICE_EXTENSION, *PHW_DEVICE_EXTENSION;

typedef HW_DEVICE_EXTENSION ADAPTER_DATA_EXTENSION, *PADAPTER_DATA_EXTENSION;

//
// this structure is our per stream extension structure.  This stores
// information that is relevant on a per stream basis.  Whenever a new stream
// is opened, the stream class driver will allocate whatever extension size
// is specified in the HwInitData.PerStreamExtensionSize.
//
 
typedef struct _STREAMEX {
    PHW_DEVICE_EXTENSION        pHwDevExt;          // For timer use
    PHW_STREAM_OBJECT           pStreamObject;      // For timer use
    union {
        KS_VIDEOINFOHEADER      *pVideoInfoHeader;  // format (variable size!)
        KS_VBIINFOHEADER        *pVBIInfoHeader;    //
    };
    union {
        KS_FRAME_INFO           FrameInfo;          // PictureNumber, etc.
        KS_VBI_FRAME_INFO       VBIFrameInfo;
    };
    KSSTATE                     KSState;            // Run, Stop, Pause

    HANDLE                      hMasterClock;       // 

    UINT EventCount;                                // for IVPNotify interface

    // We get this from Ddraw
    HANDLE                      hCapture;

    HANDLE                      threadHandle;
 
    // Incoming SRBs go here
    LIST_ENTRY                  incomingDataSrbQueue;

    // SRBs in DDraw-land are moved to this queue
    LIST_ENTRY                  waitQueue;

    // During some state transitions, we need to 
    // temporarily move SRBs here (purely for the
    // purpose of reordering them) before being
    // returned to the incomingDataSrbQueue.
    LIST_ENTRY                  reversalQueue;

    // Control SRBs go here
    LIST_ENTRY                  ctrlSrbQueue;

    // Flag to indicate whether or not we are currently
    // busy processing a control SRB
    BOOL                        processingCtrlSrb;

    // General purpose lock. We could use a separate one
    // for each queue, but this keeps things a little
    // more simple. Since it is never held for very long,
    // this shouldn't be a big performance hit.
    KSPIN_LOCK                  streamExtensionLock;
    
    // Our frame rate is determined by this
    ULONG                       everyNFields;

    // internal flag to indicate whether or not we
    // have registered for DirectDraw events
    BOOL                        registered;
    
    // for synchronizing state changes
    KEVENT                      specialEvent;

    UINT                        stateChange;
    KEVENT                      stateTransitionEvent;
    KEVENT                      SrbAvailableEvent;
} STREAMEX, *PSTREAMEX;

//
// this structure defines the per request extension.  It defines any storage
// space that the min driver may need in each request packet.
//

typedef struct _SRB_EXTENSION {
    KEVENT                      bufferDoneEvent;
    BOOL                        eventInitialized;
    DDCAPBUFFINFO               ddCapBuffInfo;
    LIST_ENTRY                  listEntry;
    PHW_STREAM_REQUEST_BLOCK    pSrb;
} SRB_EXTENSION, * PSRB_EXTENSION;

/*****************************************************************************
*
* the following section defines prototypes for the minidriver initialization
* routines
*
******************************************************************************/

//
// DriverEntry:
//
// This routine is called when the mini driver is first loaded.  The driver
// should then call the StreamClassRegisterAdapter function to register with
// the stream class driver
//

ULONG DriverEntry(IN PDRIVER_OBJECT, PUNICODE_STRING);

//
// This routine is called by the stream class driver with configuration
// information for an adapter that the mini driver should load on.  The mini
// driver should still perform a small verification to determine that the
// adapter is present at the specified addresses, but should not attempt to
// find an adapter as it would have with previous NT miniports.
//
// All initialization of the adapter should also be performed at this time.
//

VOID HwInitialize (IN OUT PHW_STREAM_REQUEST_BLOCK);

VOID STREAMAPI AdapterCompleteInitialization (PHW_STREAM_REQUEST_BLOCK);
//
// This routine is called when the system is going to remove or disable the
// device.
//
// The mini-driver should free any system resources that it allocated at this
// time.  Note that system resources allocated for the mini-driver by the
// stream class driver will be free'd by the stream driver, and should not be
// free'd in this routine.  (Such as the HW_DEVICE_EXTENSION)
//

VOID HwUnInitialize (PHW_STREAM_REQUEST_BLOCK);

//
// This is the prototype for the stream enumeration function.  This routine
// provides the stream class driver with the information on data stream types
// supported
//

VOID AdapterStreamInfo(PHW_STREAM_REQUEST_BLOCK);

//
// This is the prototype for the stream open function
//

VOID AdapterOpenStream(PHW_STREAM_REQUEST_BLOCK);

//
// This is the prototype for the stream close function
//

VOID AdapterCloseStream(PHW_STREAM_REQUEST_BLOCK);

//
// This is the prototype for the AdapterReceivePacket routine.  This is the
// entry point for command packets that are sent to the adapter (not to a
// specific open stream)
//

VOID STREAMAPI AdapterReceivePacket(IN PHW_STREAM_REQUEST_BLOCK);

//
// This is the protoype for the cancel packet routine.  This routine enables
// the stream class driver to cancel an outstanding packet.
//

VOID STREAMAPI AdapterCancelPacket(IN PHW_STREAM_REQUEST_BLOCK);

//
// This is the packet timeout function.  The adapter may choose to ignore a
// packet timeout, or rest the adapter and cancel the requests, as required.
//

VOID STREAMAPI AdapterTimeoutPacket(IN PHW_STREAM_REQUEST_BLOCK);

//
// For event handling on the VP stream
//
NTSTATUS STREAMAPI VPStreamEventProc (PHW_EVENT_DESCRIPTOR);

//
// For event handling on the VP VBI stream
//
NTSTATUS STREAMAPI VPVBIStreamEventProc (PHW_EVENT_DESCRIPTOR);

//
// prototypes for data handling routines
//

VOID STREAMAPI VideoReceiveDataPacket(IN PHW_STREAM_REQUEST_BLOCK);
VOID STREAMAPI VideoReceiveCtrlPacket(IN PHW_STREAM_REQUEST_BLOCK);

//
// prototypes for properties and states
//

// Adapter based
VOID STREAMAPI AdapterGetProperty(PHW_STREAM_REQUEST_BLOCK);
VOID STREAMAPI AdapterSetProperty(PHW_STREAM_REQUEST_BLOCK);

// Stream based
VOID SetVideoPortProperty(PHW_STREAM_REQUEST_BLOCK);
VOID GetVideoPortProperty(PHW_STREAM_REQUEST_BLOCK);

VOID SetVideoPortVBIProperty(PHW_STREAM_REQUEST_BLOCK);
VOID GetVideoPortVBIProperty(PHW_STREAM_REQUEST_BLOCK);

VOID VideoGetProperty(PHW_STREAM_REQUEST_BLOCK);
VOID VideoSetProperty(PHW_STREAM_REQUEST_BLOCK);

VOID VideoGetState(PHW_STREAM_REQUEST_BLOCK);
VOID VideoSetState(PHW_STREAM_REQUEST_BLOCK);

VOID VideoStreamGetConnectionProperty (PHW_STREAM_REQUEST_BLOCK);
VOID VideoStreamGetDroppedFramesProperty(PHW_STREAM_REQUEST_BLOCK);

// for verifying data formats
BOOL AdapterVerifyFormat(PKSDATAFORMAT, int);
VOID ProcessDataIntersection(PHW_STREAM_REQUEST_BLOCK);
BOOL AdapterFormatFromRange(IN PHW_STREAM_REQUEST_BLOCK);

// Creates system thread
BOOL CreateStreamThread(PSTREAMEX);

// System thread for handling buffers and events
VOID StreamThread(IN PVOID);

// Submits buffers to DirectDraw
void AddBuffersToDirectDraw(PSTREAMEX);

// Called by above function
BOOL AddBuffer(PHW_STREAM_REQUEST_BLOCK);

// Requests DirectDraw to return all buffers immediately
BOOL FlushBuffers(PSTREAMEX);

// Sets DDraw's notion of field number to zero
BOOL ResetFieldNumber(PSTREAMEX);

// Gets ring-0 DD, VP, and Capture handles
BOOL GetRing0DirectDrawHandle(PSTREAMEX);
BOOL GetRing0VideoPortHandle(PSTREAMEX);
BOOL GetRing0SurfaceHandle(PSTREAMEX);
BOOL GetCaptureHandle(PSTREAMEX);

// Releases them
BOOL ReleaseRing0DirectDrawHandle(PSTREAMEX);
BOOL ReleaseRing0VideoPortHandle(PSTREAMEX);
BOOL ReleaseRing0SurfaceHandle(PSTREAMEX);
BOOL ReleaseCaptureHandle(PSTREAMEX);

BOOL RegisterForDirectDrawEvents(PSTREAMEX);
BOOL UnregisterForDirectDrawEvents(PSTREAMEX);
VOID AttemptRenegotiation(PSTREAMEX);

// Does what they say
VOID TimeStampSrb(PHW_STREAM_REQUEST_BLOCK);
VOID SetFrameInfo(PHW_STREAM_REQUEST_BLOCK);
VOID HandleBusmasterCompletion(PHW_STREAM_REQUEST_BLOCK);
VOID EmptyIncomingDataSrbQueue(PSTREAMEX);
VOID HandleStateTransition(PSTREAMEX);

// 
// stream clock functions
//

ULONGLONG VideoGetSystemTime();
VOID STREAMAPI StreamClockRtn(IN PHW_TIME_CONTEXT);
VOID VideoOpenCloseMasterClock (PHW_STREAM_REQUEST_BLOCK);
VOID VideoIndicateMasterClock (PHW_STREAM_REQUEST_BLOCK);

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif //__CAPMAIN_H__

