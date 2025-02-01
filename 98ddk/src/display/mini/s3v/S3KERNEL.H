/******************************************************************************\
*                                                                              *
*    S3KERNEL.H  -  Header file for the S3 KERNEL Module                       *
*                                                                              *
*    Copyright (c) S3 Incorporated, 1995                                       *
*    All Rights Reserved.                                                      *
*                                                                              *
*    Use of S3 Inc. code is governed by terms and conditions                   *
*    stated in the accompanying licensing statement.                           *
*                                                                              *
\******************************************************************************/

#ifndef __S3KERNEL__
#define __S3KERNEL__

#ifdef WIN32
#define _loadds
#endif

#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************\
*                                                                              *
*                                 DEFINITIONS                                  *
*                                                                              *
\******************************************************************************/

// Error codes
typedef enum
{
  S3KN_ERR_SUCCESS              = 0x0000,
  S3KN_ERR_HWBUSY               = 0x0001,
  S3KN_ERR_VXDNOTFOUND          = 0x0002,
  S3KN_ERR_INCORRECTADDRESS     = 0x0003,
  S3KN_ERR_TIMEOUTEXPIRED       = 0x0004,
  S3KN_ERR_NOTSUPPORTEDAPI      = 0x0005,
  S3KN_ERR_CANTLOCK             = 0x0006
} S3KN_ERR;

// ENGINE Flags
#define S3KN_ENG_2D           	0x00100000
#define S3KN_ENG_3D           	0x00200000
#define S3KN_ENG_SP           	0x00400000
#define S3KN_ENG_LPB           	0x00800000

// WAIT Flags
#define S3KN_WAIT               0x00000001
#define S3KN_NOTWAIT            0x00000002

// ACCESS Flags
#define S3KN_ACC_PIO            0x00000010
#define S3KN_ACC_MMIO           0x00000020
#define S3KN_ACC_DMA           	0x00000040

// REQUEST Flags
#define S3KN_REQ_PRECISE        0x00000100
#define S3KN_REQ_FULL           0x00000200


/******************************************************************************\
*                                                                              *
*                               COMMON TYPES                                   *
*                                                                              *
\******************************************************************************/


typedef unsigned long S3KN_HCRITSECT;
typedef S3KN_HCRITSECT FAR * S3KN_LPHCRITSECT;

//typedef unsigned long S3KN_ERR;

typedef struct tag_S3KN_COMMONDATA
{
  unsigned long ulFlags;
  VOID FAR *    lpVideoBuffer;
  VOID FAR *    lpMMIOSpace;
  VOID FAR *    lpDMABuffer;
  unsigned long ulDMALength;
  unsigned long ulS3Internal; // driver internal do not touch
} S3KN_COMMONDATA, FAR * S3KN_LPCOMMONDATA;

typedef struct tag_S3KN_ACCESSTRUCT
{
  unsigned long ulHandle;
  unsigned long ulRequestFlags;
  unsigned long ulRequestCount;
  unsigned long ulAvailableCount;
  unsigned long ulBufferOffset;
  unsigned long ulActualCount;

} S3KN_ACCESSTRUCT, FAR * S3KN_LPACCESSTRUCT;


typedef struct tag_S3KN_DBGEVENT
{
  unsigned long dwEvent;
} S3KN_DBGEVENT, FAR * S3KN_LPDBGEVENT;


/******************************************************************************\
*                                                                              *
*                          GENERAL HIGH LEVEL FUNCTIONS                        *
*                                                                              *
\******************************************************************************/

// Retrieves pointer to the common data structure for the given client
S3KN_ERR WINAPI _loadds S3KN_GetCommonDataStruct( S3KN_LPCOMMONDATA FAR * lplpCommonData );

// Starts unconditional critical section. Function willtry to establish a critical
// section for dwTimeout number of  micro seconds.
// Certain fields from the common data structure may be accessed only when
// critical section is set
S3KN_ERR WINAPI _loadds S3KN_BeginCriticalSection( );

// Stops unconditional critical section
S3KN_ERR WINAPI _loadds S3KN_EndCriticalSection();

// Requests an exclusive access to the hardware resources for a client.  Critical
// section will be set automatically by this function
// To specify request the following groups of flags should be used:
//	 	engine which will be influenced - ENGINE
//		type of access                  - ACCESS
//		wait or not wait		  - WAIT
//		precise or estimated request    - REQUEST
// For the DMA access all four flags should be defined; counters and pointers are defined
// For MMIO or PIO first ENGINE, ACCESS, and WAIT flags should be specified,  counters and
// pointers are undefined
//
// If S3KN_ERR_SUCCESS is returned, handle will be initialised with critical section
// unique number. Function may return S3KN_ERR_HWBUSY as a result
S3KN_ERR WINAPI _loadds S3KN_RequestAccess( S3KN_LPACCESSTRUCT lpAccessStruct );

// Releases the access and critical section for the specified handle.  Handle will be
// undefined after the successfull completion  of the function
S3KN_ERR WINAPI _loadds S3KN_ReleaseAccess( S3KN_LPACCESSTRUCT lpAccessStruct );

// Returns last error description
S3KN_ERR WINAPI _loadds S3KN_GetErrorText( S3KN_ERR eError, char FAR * lpErrorText );



/******************************************************************************\
*                                                                              *
*                             DMA SPECIFIC FUNCTIONS                           *
*                                                                              *
\******************************************************************************/

S3KN_ERR WINAPI _loadds S3KN_DMA_Sleep( S3KN_LPACCESSTRUCT lpAccessStruct );
S3KN_ERR WINAPI _loadds S3KN_DMA_Wakeup( S3KN_LPACCESSTRUCT lpAccessStruct );


/******************************************************************************\
*                                                                              *
*                          DEBUGGING SPECIFIC FUNCTIONS                        *
*                                                                              *
\******************************************************************************/

S3KN_ERR WINAPI _loadds S3KN_DBG_TraceEvents( S3KN_LPDBGEVENT lpEventData, unsigned short FAR * lpwEventsCount );

/******************************************************************************\
*                                                                              *
*                          SERVICE  FUNCTIONS                                  *
*                                                                              *
\******************************************************************************/
                                          
#ifndef WIN32                                          
DWORD WINAPI _loadds S3KN_GetProt16FromLin( DWORD dwLinAddr, DWORD dwSize );
#endif                              

#ifdef __cplusplus
}
#endif

#endif
