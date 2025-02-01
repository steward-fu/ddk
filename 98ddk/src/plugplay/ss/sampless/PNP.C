/*** pnp.c - Plug and Play Handlers
 *
 *  This module contains functions to handle pnp events.
 *
 *  Copyright (c) 1995,1996 Microsoft Corporation
 *
 *  MODIFICATION HISTORY
 */


#include "pcicss.h"


//Function Prototypes
int CM_EXTERNAL RegWithCS(PADAPTER pa);
CONFIGRET CM_HANDLER SSPnPHandler(CONFIGFUNC cfFunc, SUBCONFIGFUNC scfSubFunc,
	                          DEVNODE dn, DWORD dwRefData, ULONG ulFlags);
BOOL LOCAL InitAdapterData(DEVNODE dn, PADAPTER pa);
BOOL LOCAL InitAdapter(PADAPTER pa);

#pragma CM_PAGEABLE_DATA
#pragma CM_PAGEABLE_CODE

/***EP  SSInit - Initialization
 *
 *  ENTRY
 *      None
 *
 *  EXIT-SUCCESS
 *      returns VXD_SUCCESS
 *  EXIT-FAILURE
 *      returns VXD_FAILURE
 */

//shut up inline expansion warning on CM_Get_Version
#pragma warning(disable: 4710)
CM_VXD_RESULT CM_SYSCTRL SSInit(VOID)
{
    CM_VXD_RESULT rc;

    if (CM_Get_Version() >= CONFIGMG_VERSION)
    {
        rc = VXD_SUCCESS;
    }
    else
    {
        rc = VXD_FAILURE;
        CM_ERROR(("SSInit: incorrect ConfigMG version"));
    }

    return rc;
}       //SSInit
#pragma warning(default: 4710)


/***EP  SSNewDevnode - New devnode handler
 *
 *  ENTRY
 *      dn - new devnode
 *      LoadType - type of functionality CM wants this VxD to handle
 *
 *  EXIT
 *      returns standard CM return code
 */

CONFIGRET CM_SYSCTRL SSNewDevnode(DEVNODE dn, LOAD_TYPE LoadType)
{
    CONFIGRET rc;

    if (LoadType == DLVXD_LOAD_DRIVER)
    {
        PADAPTER pa;

        rc = CR_FAILURE;
        if ((pa = (PADAPTER)_HeapAllocate(sizeof(ADAPTER), HEAPZEROINIT)) ==
            NULL)
        {
            CM_ERROR(("SSNewDevnode: failed to allocate instance data"));
        }
        else if (InitAdapterData(dn, pa) &&
                 ((rc = CM_Register_Device_Driver(dn, SSPnPHandler, (DWORD)pa,
                                     CM_REGISTER_DEVICE_DRIVER_DISABLEABLE |
                                     CM_REGISTER_DEVICE_DRIVER_REMOVABLE)) !=
                  CR_SUCCESS))
        {
            CM_ERROR(("SSNewDevnode: CM_RegDeviceDrv failed (rc=%lx)", rc));
        }

        if ((rc == CR_FAILURE) && (pa != NULL))
        {
            if (!_LinRegionLock((DWORD)pa, (DWORD)pa + sizeof(ADAPTER), 0))
            {
                CM_ERROR(("SSNewDevnode: failed to unlock adapter data"));
            }
            _HeapFree(pa, 0);
        }
    }
    else
    {
        rc = CR_DEFAULT;
    }

    return rc;
}       //SSNewDevnode


/***EP  SSPnPHandler - PnP handler
 *
 *  ENTRY
 *      cfFunc - function code
 *      scfSubFunc - sub-function code
 *      dn - devnode
 *      dwRefData -> instance data
 *      ulFlags - flags
 *
 *  EXIT
 *      returns standard CM return code
 */

CONFIGRET CM_HANDLER SSPnPHandler(CONFIGFUNC cfFunc, SUBCONFIGFUNC scfSubFunc,
	                          DEVNODE dn, DWORD dwRefData, ULONG ulFlags)
{
    CONFIGRET rc = CR_SUCCESS;
    PADAPTER pa;

    switch (cfFunc)
    {
        case CONFIG_FILTER:
            ASSERT(dwRefData != 0);
            //
            //>>Filter logical configurations for controller.
            //>>This is called before CONFIG_START and may not be required.
            //
            break;

        case CONFIG_START:
            ASSERT(dwRefData != 0);
	    pa = (PADAPTER)dwRefData;
            if (!InitAdapter(pa))
                rc = CR_FAILURE;
            else if (scfSubFunc == CONFIG_START_FIRST_START)
            {
                if (RegWithCS(pa) != 0)
                {
                    CM_ERROR(("SSPnPHandler: failed to register with card services"));
                    rc = CR_FAILURE;
                }
                else
                {   //save as RefData
                    CM_Set_Device_Driver_Private_DWord(dn, dwRefData, 0);
                }
            }
            break;

        case CONFIG_REMOVE:
            ASSERT(dwRefData != NULL);
	    pa = (PADAPTER)dwRefData;
            if (!_LinRegionUnLock(dwRefData, dwRefData + sizeof(ADAPTER), 0))
                CM_ERROR(("SSPnPHandler: failed to unlock adapter data"));

            _HeapFree(pa, 0);
            break;

        case CONFIG_APM:
            ASSERT(dwRefData != NULL);
            //
            //>>Respond to power management requests passed in scfSubFunc.
            //
            break;

        default:
            rc = CR_DEFAULT;
    }

    return rc;
}       //SSPnPHandler


/***LP  InitAdapterData - Initialize adapter instance data
 *
 *  ENTRY
 *      dn - devnode
 *      pa -> ADAPTER structure
 *
 *  EXIT-SUCCESS
 *      returns TRUE
 *  EXIT-FAILURE
 *      returns FALSE
 */

BOOL LOCAL InitAdapterData(DEVNODE dn, PADAPTER pa)
{
    BOOL rc;

    if (!_LinRegionLock((DWORD)pa, (DWORD)pa + sizeof(ADAPTER), 0))
    {
        CM_ERROR(("InitAdapterData: failed to lock adapter data"));
        rc = FALSE;
    }
    else
    {
        pa->devnode = dn;
        pa->dwSig = SIG_MYSS;

        rc = TRUE;
    }

    return rc;
}       //InitAdapterData


/***LP  InitAdapter - Initialize adapter
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *
 *  EXIT-SUCCESS
 *      returns TRUE
 *  EXIT-FAILURE
 *      returns FALSE
 */

BOOL LOCAL InitAdapter(PADAPTER pa)
{
    BOOL rc = FALSE;
    CONFIGRET cr;
    LOG_CONF lc;
    RES_DES rd;

    if (CM_Get_First_Log_Conf(&lc, pa->devnode, ALLOC_LOG_CONF) != CR_SUCCESS)
    {
        CM_ERROR(("InitAdapter: failed to get Alloc. LogConf"));
    }
    else if (((cr = CM_Get_Next_Res_Des(&rd, (RES_DES)lc, ResType_IRQ, NULL, 0))
              != CR_SUCCESS) &&
             (cr != CR_NO_MORE_RES_DES))
    {
        CM_ERROR(("InitAdapter: failed to get resource IRQ"));
    }
    else
    {
        if (cr == CR_SUCCESS)
            pa->bStatusIRQ = (BYTE)((IRQ_DES *)rd)->IRQD_Alloc_Num;
        else
        {
            pa->bStatusIRQ = 0;
            CM_WARN1(("InitAdapter: no allocated IRQ!"));
        }

        if (CM_Get_Next_Res_Des(&rd, (RES_DES)lc, ResType_IO, NULL, 0) !=
            CR_SUCCESS)
        {
            CM_ERROR(("InitAdapter: failed to get resource IO"));
        }
        else
        {
            WORD wIOBase;
            BYTE bRegBase;

            //Each I/O range defined in the AllocLogConf is checked
            //for a valid socket at the offset 0x00, 0x40, 0x80, 0xc0.
            pa->bNumSkts = 0;
            do
            {
                wIOBase = ((IO_DES *)rd)->IOD_Alloc_Base;
                bRegBase = 0;
                //
                //>>Determine if a socket exists at this I/O and this RegBase.
                //>>If exists, increment the number of sockets and remember
                //>>the IOBase and RegBase.  Set to next RegBase and repeat
                //>>the process.
                //
            } while ((pa->bNumSkts < MAX_SOCKETS) &&
                     (CM_Get_Next_Res_Des(&rd, rd, ResType_IO, NULL, 0)
                      == CR_SUCCESS));

            if (pa->bNumSkts == 0)
            {
                CM_ERROR(("InitAdapter: failed to find any socket"));
            }
            else
            {
                //
                //>>Initialize the sockets on the controller.
                //
                rc = TRUE;
            }
        }
    }

    return rc;
}       //InitAdapter
