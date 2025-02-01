/*++

    Copyright (c) 1997-1998 Microsoft Corporation

Module Name:

    ksproxy.h

Abstract:

    Interface definitions for WDM-CSA proxy filters.

--*/

#ifndef __KSPROXY__
#define __KSPROXY__

#ifdef __cplusplus
extern "C" {
#endif

#undef KSDDKAPI
#ifdef _KSDDK_
#define KSDDKAPI
#else // !_KSDDK_
#define KSDDKAPI DECLSPEC_IMPORT
#endif // _KSDDK_

#define STATIC_IID_IKsObject\
    0x423c13a2L, 0x2070, 0x11d0, 0x9e, 0xf7, 0x00, 0xaa, 0x00, 0xa2, 0x16, 0xa1

#define STATIC_IID_IKsPin\
    0xb61178d1L, 0xa2d9, 0x11cf, 0x9e, 0x53, 0x00, 0xaa, 0x00, 0xa2, 0x16, 0xa1

#define STATIC_IID_IKsDataTypeHandler\
    0x5ffbaa02L, 0x49a3, 0x11d0, 0x9f, 0x36, 0x00, 0xaa, 0x00, 0xa2, 0x16, 0xa1

#define STATIC_IID_IKsInterfaceHandler\
    0xD3ABC7E0L, 0x9A61, 0x11D0, 0xA4, 0x0D, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96

#define STATIC_IID_IKsClockPropertySet\
    0x5C5CBD84L, 0xE755, 0x11D0, 0xAC, 0x18, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96

#define STATIC_IID_IKsAllocator\
    0x8da64899L, 0xc0d9, 0x11d0, 0x84, 0x13, 0x00, 0x00, 0xf8, 0x22, 0xfe, 0x8a

#ifndef STATIC_IID_IKsPropertySet
#define STATIC_IID_IKsPropertySet\
    0x31EFAC30L, 0x515C, 0x11d0, 0xA9, 0xAA, 0x00, 0xAA, 0x00, 0x61, 0xBE, 0x93
#endif // STATIC_IID_IKsPropertySet

#define STATIC_CLSID_Proxy \
    0x17CCA71BL, 0xECD7, 0x11D0, 0xB9, 0x08, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96

#ifdef _KS_

#if !defined(__cplusplus) || _MSC_VER < 1100
DEFINE_GUIDEX(IID_IKsObject);

DEFINE_GUIDEX(IID_IKsPin);

DEFINE_GUIDEX(IID_IKsDataTypeHandler);

DEFINE_GUIDEX(IID_IKsInterfaceHandler);

DEFINE_GUIDEX(IID_IKsClockPropertySet);

DEFINE_GUIDEX(IID_IKsAllocator);

#define IID_IKsQualityForwarder KSCATEGORY_QUALITY
#endif // !defined(__cplusplus) || _MSC_VER < 1100

#define STATIC_IID_IKsQualityForwarder STATIC_KSCATEGORY_QUALITY

#ifdef __STREAMS__

struct DECLSPEC_UUID("5C5CBD84-E755-11D0-AC18-00A0C9223196") IKsClockPropertySet;
DECLARE_INTERFACE_(IKsClockPropertySet, IUnknown)
{
    STDMETHOD (KsGetTime)(LONGLONG* Time) PURE;
    STDMETHOD (KsSetTime)(LONGLONG Time) PURE;
    STDMETHOD (KsGetPhysicalTime)(LONGLONG* Time) PURE;
    STDMETHOD (KsSetPhysicalTime)(LONGLONG Time) PURE;
    STDMETHOD (KsGetCorrelatedTime)(KSCORRELATED_TIME* CorrelatedTime) PURE;
    STDMETHOD (KsSetCorrelatedTime)(KSCORRELATED_TIME* CorrelatedTime) PURE;
    STDMETHOD (KsGetCorrelatedPhysicalTime)(KSCORRELATED_TIME* CorrelatedTime) PURE;
    STDMETHOD (KsSetCorrelatedPhysicalTime)(KSCORRELATED_TIME* CorrelatedTime) PURE;
    STDMETHOD (KsGetResolution)(KSRESOLUTION* Resolution) PURE;
    STDMETHOD (KsGetState)(KSSTATE* State) PURE;
};

typedef enum {
    KsAllocatorMode_User,
    KsAllocatorMode_Kernel
} KSALLOCATORMODE;

interface DECLSPEC_UUID("8da64899-c0d9-11d0-8413-0000f822fe8a") IKsAllocator;
DECLARE_INTERFACE_(IKsAllocator, IUnknown)
{
    STDMETHOD_(HANDLE, KsGetAllocatorHandle)() PURE;
    STDMETHOD_(KSALLOCATORMODE, KsGetAllocatorMode)() PURE;
    STDMETHOD (KsGetAllocatorStatus)( PKSSTREAMALLOCATOR_STATUS AllocatorStatus ) PURE;
    STDMETHOD_(VOID, KsSetAllocatorMode)( KSALLOCATORMODE Mode ) PURE;
};  

typedef enum {
    KsPeekOperation_PeekOnly,
    KsPeekOperation_AddRef
} KSPEEKOPERATION;

typedef struct _KSSTREAM_SEGMENT *PKSSTREAM_SEGMENT;

interface DECLSPEC_UUID("b61178d1-a2d9-11cf-9e53-00aa00a216a1") IKsPin;
DECLARE_INTERFACE_(IKsPin, IUnknown)
{
    STDMETHOD (KsQueryMediums)(PKSMULTIPLE_ITEM* MediumList) PURE;
    STDMETHOD (KsQueryInterfaces)(PKSMULTIPLE_ITEM* InterfaceList) PURE;
    STDMETHOD (KsCreateSinkPinHandle)(KSPIN_INTERFACE& Interface, KSPIN_MEDIUM& Medium) PURE;
    STDMETHOD (KsGetCurrentCommunication)(
        KSPIN_COMMUNICATION *Communication,
        KSPIN_INTERFACE *Interface,
        KSPIN_MEDIUM *Medium) PURE;
    STDMETHOD (KsPropagateAcquire)() PURE;
    STDMETHOD (KsDeliver)(IMediaSample* Sample, ULONG Flags) PURE;
    STDMETHOD (KsMediaSamplesCompleted)( PKSSTREAM_SEGMENT StreamSegment ) PURE;
    
    STDMETHOD_(IMemAllocator *, KsPeekAllocator)(KSPEEKOPERATION Operation) PURE;
    STDMETHOD (KsReceiveAllocator)(IMemAllocator *MemAllocator) PURE;
    STDMETHOD (KsRenegotiateAllocator)() PURE;
    STDMETHOD_(LONG, KsIncrementPendingIoCount)() PURE;
    STDMETHOD_(LONG, KsDecrementPendingIoCount)() PURE;
    STDMETHOD (KsQualityNotify)(ULONG Proportion, REFERENCE_TIME TimeDelta) PURE;
};

interface DECLSPEC_UUID("CD5EBE6B-8B6E-11D1-8AE0-00A0C9223196") IKsPinFactory;
DECLARE_INTERFACE_(IKsPinFactory, IUnknown)
{
    STDMETHOD (KsPinFactory)(ULONG* PinFactory) PURE;
};

typedef enum {
    KsIoOperation_Write,
    KsIoOperation_Read
} KSIOOPERATION;

interface DECLSPEC_UUID("5ffbaa02-49a3-11d0-9f36-00aa00a216a1") IKsDataTypeHandler;
DECLARE_INTERFACE_(IKsDataTypeHandler, IUnknown)
{
    STDMETHOD (KsCompleteIoOperation)(IMediaSample *Sample, PVOID StreamHeader, KSIOOPERATION IoOperation, BOOL Cancelled) PURE;
    STDMETHOD (KsIsMediaTypeInRanges)(PVOID DataRanges) PURE;
    STDMETHOD (KsPrepareIoOperation)(IMediaSample *Sample, PVOID StreamHeader, KSIOOPERATION IoOperation) PURE;
    STDMETHOD (KsQueryExtendedSize)(ULONG* ExtendedSize) PURE;
    STDMETHOD (KsSetMediaType)(const CMediaType *MediaType) PURE;
};

interface DECLSPEC_UUID("D3ABC7E0-9A61-11d0-A40D-00A0C9223196") IKsInterfaceHandler;
DECLARE_INTERFACE_(IKsInterfaceHandler, IUnknown)
{
    STDMETHOD (KsSetPin)(IKsPin *KsPin ) PURE;
    STDMETHOD (KsProcessMediaSamples)(IKsDataTypeHandler *KsDataTypeHandler, IMediaSample** SampleList, PLONG SampleCount, KSIOOPERATION IoOperation, PKSSTREAM_SEGMENT *StreamSegment) PURE;
    STDMETHOD (KsCompleteIo)(PKSSTREAM_SEGMENT StreamSegment) PURE;
};


//
// This structure definition is the common header required by the proxy to 
// dispatch the stream segment to the interface handler.  Interface handlers 
// will create extended structures to include other information such as 
// media samples, extended header size and so on.
//

typedef struct _KSSTREAM_SEGMENT {
    IKsInterfaceHandler     *KsInterfaceHandler;
    IKsDataTypeHandler      *KsDataTypeHandler;
    KSIOOPERATION           IoOperation;
    HANDLE                  CompletionEvent;
    
} KSSTREAM_SEGMENT;

interface DECLSPEC_UUID("423c13a2-2070-11d0-9ef7-00aa00a216a1") IKsObject;
DECLARE_INTERFACE_(IKsObject, IUnknown)
{
    STDMETHOD_(HANDLE, KsGetObjectHandle)() PURE;
};

interface DECLSPEC_UUID("97ebaacb-95bd-11d0-a3ea-00a0c9223196") IKsQualityForwarder;
DECLARE_INTERFACE_(IKsQualityForwarder, IKsObject)
{
    STDMETHOD_(VOID, KsFlushClient)(IKsPin* Pin) PURE;
};

KSDDKAPI
HRESULT
WINAPI
KsOpenDefaultDevice(
    REFGUID Category,
    ACCESS_MASK Access,
    PHANDLE DeviceHandle
    );
KSDDKAPI
HRESULT
WINAPI
KsSynchronousDeviceControl(
    HANDLE      Handle,
    ULONG       IoControl,
    PVOID       InBuffer,
    ULONG       InLength,
    PVOID       OutBuffer,
    ULONG       OutLength,
    PULONG      BytesReturned
    );
KSDDKAPI
HRESULT
WINAPI
KsGetMultiplePinFactoryItems(
    HANDLE  FilterHandle,
    ULONG   PinFactoryId,
    ULONG   PropertyId,
    PVOID*  Items
    );
KSDDKAPI
HRESULT
WINAPI
KsGetMediaTypeCount(
    HANDLE      FilterHandle,
    ULONG       PinFactoryId,
    ULONG*      MediaTypeCount
    );
KSDDKAPI
HRESULT
WINAPI
KsGetMediaType(
    int         Position,
    CMediaType* MediaType,
    HANDLE      FilterHandle,
    ULONG       PinFactoryId
    );

#endif // __STREAMS__

#ifndef _IKsPropertySet_
#if !defined(__cplusplus) || _MSC_VER < 1100
DEFINE_GUIDEX(IID_IKsPropertySet);
#endif // !defined(__cplusplus) || _MSC_VER < 1100
#endif // _IKsPropertySet_

DEFINE_GUIDSTRUCT("17CCA71B-ECD7-11D0-B908-00A0C9223196", CLSID_Proxy);
#define CLSID_Proxy DEFINE_GUIDNAMED(CLSID_Proxy)

#else // !_KS_

#ifndef _IKsPropertySet_
#if !defined(__cplusplus) || _MSC_VER < 1100
DEFINE_GUID(IID_IKsPropertySet, STATIC_IID_IKsPropertySet);
#endif // !defined(__cplusplus) || _MSC_VER < 1100
#endif // _IKsPropertySet_

#if !defined(__cplusplus) || _MSC_VER < 1100
DEFINE_GUID(CLSID_Proxy, STATIC_CLSID_Proxy);
#else  // defined(__cplusplus) && _MSC_VER >= 1100
DECLSPEC_UUID("17CCA71B-ECD7-11D0-B908-00A0C9223196") CLSID_Proxy;
#endif  // defined(__cplusplus) && _MSC_VER >= 1100

#endif // !_KS_

#ifndef _IKsPropertySet_
#define _IKsPropertySet_

#define KSPROPERTY_SUPPORT_GET 1
#define KSPROPERTY_SUPPORT_SET 2

#ifdef DECLARE_INTERFACE_

interface DECLSPEC_UUID("31EFAC30-515C-11d0-A9AA-00aa0061be93") IKsPropertySet;
DECLARE_INTERFACE_(IKsPropertySet, IUnknown)
{
    STDMETHOD (Set)(
        IN REFGUID PropSet,
        IN ULONG Id,
        IN LPVOID InstanceData,
        IN ULONG InstanceLength,
        IN LPVOID PropertyData,
        IN ULONG DataLength) PURE;

    STDMETHOD (Get)(
        IN REFGUID PropSet,
        IN ULONG Id,
        IN LPVOID InstanceData,
        IN ULONG InstanceLength,
        OUT LPVOID PropertyData,
        IN ULONG DataLength,
        OUT ULONG* BytesReturned) PURE;

    STDMETHOD (QuerySupported)(
        IN REFGUID PropSet,
        IN ULONG Id,
        OUT ULONG* TypeSupport) PURE;
};

#endif // DECLARE_INTERFACE_

#endif // _IKsPropertySet_

#ifdef __cplusplus
}
#endif

#endif // __KSPROXY__
