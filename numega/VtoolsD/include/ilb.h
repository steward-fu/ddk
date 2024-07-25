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
*
* ILB (IOS Linkage Block)
*
*
\****************************************************************************/

struct ISP;
struct _IOP;
struct _IOR;

typedef void (*PFNISP) (struct ISP *pisp);  /* IOS Service Routine Prototype */
typedef void (__cdecl *PFNIOP) (struct _IOP *iop);
typedef ULONG (__cdecl *PFNULONGIOR) (struct _IOR *ior);
typedef ULONG (__cdecl *PFNULONGIOP) (struct _IOP *iop);
typedef void (*PFNVOID) (void);

typedef struct ILB { /* */

        PFNISP  ILB_service_rtn;        /* service rtn entry point into ios */
        PVOID   ILB_dprintf_rtn;        /* dprintf rtn entry point into ios */
        PVOID   ILB_Wait_10th_Sec;      /* cpu spin wait for 10th second    */
        PFNVOID	ILB_internal_request;   /* vector for issuing internal requests */
					/* called by dragon drivers (drivers*/
					/* in DCB calldown stack) 	*/
	PFNULONGIOR ILB_io_criteria_rtn;/* routine to satisfy IOR criteria  */
					/* called by dragon clients 		 */
					/* with IOS allocated IOR           */		
	PFNULONGIOP ILB_int_io_criteria_rtn;/* routine to satisfy IOP criteria  */
					/* called by dragon drivers (drivers*/
					/* in DCB calldown stack) 			 */
					/* with IOS allocated IOP           */		
        ULONG   ILB_dvt;                /* 32 bit offset of this drivers dvt*/
        ULONG   ILB_ios_mem_virt;       /* pointer to ios's memory pool     */
        ULONG   ILB_enqueue_iop;        /* iop enqueue routine			       */
        ULONG   ILB_dequeue_iop;        /* iop dequeue routine			       */
        ULONG   ILB_reserved_1;         /* reserved: MBZ							 */
        ULONG   ILB_reserved_2;         /* reserved: MBZ                    */
        USHORT  ILB_flags;              /* flags - see defines above        */
        CHAR    ILB_driver_numb;        /* the number of times the driver   */
                                        /* has been called with an          */
                                        /* initialize aep                   */
                                        /* used to be ILB_reserved_43       */
        CHAR    ILB_reserved_3;         /* reserved: MBZ							 */
			
} ILB, *PILB;

