/*****************************************************************************
 *;++
 * Name           : $RCSfile$
 * Title          : Direct Draw Hook
 * C Author       : Jim Page
 * Created        : 03/02/96
 *
 * Copyright      : 1995 by VideoLogic Limited. All rights reserved.
 *                  No part of this software, either material or conceptual 
 *                  may be copied or distributed, transmitted, transcribed,
 *                  stored in a retrieval system or translated into any 
 *                  human or computer language in any form by any means,
 *                  electronic, mechanical, manual or other-wise, or 
 *                  disclosed to third parties without the express written
 *                  permission of VideoLogic Limited, Unit 8, HomePark
 *                  Industrial Estate, King's Langley, Hertfordshire,
 *                  WD4 8LZ, U.K.
 *
 * Description    : Pseudo stackable direct draw hook services
 *                   
 * Program Type   : C header file
 *
 * RCS info:
 *
 * $Date:   04 Mar 1997 14:49:08  $
 * $Revision:   1.0  $
 * $Locker$
 * $Log:   Q:/SOFTDEV/VCS/WIN9x/display/mini/s3_3/DDHOOK.H_v  $
//
//   Rev 1.0   04 Mar 1997 14:49:08   mbenso
//Initial revision.
//
//   Rev 1.0   01 Jul 1996 14:23:30   bchung
//Initial revision.
 *
 *;--
 *****************************************************************************/

/* 
 *	These functions are in the DDHOOK.C module. This module should be linked to your
 *	direct draw driver.
 */

// called on DirectDrawCreate (just before a successful return)
LPDDHALINFO DDHookOnDDCreateDriverObject (LPDDHALINFO lpddHALInfo, int bReset);

// This should be called at driver initialisation time. A FALSE return value
// indicates that the init failed; the direct draw driver can still run solo.
BOOL DDHookInit ();

// This should be called at driver deinit time
void DDHookDeinit ();

/* 
 *	hookable (externsion) drivers that wish to layer over a base Direct Draw driver
 *	must have this entry point:
 */
typedef LPDDHALINFO (FAR PASCAL * LPFNSTACKHALINFO)(LPDDHALINFO lpddHALInfo, BOOL bReset);

/* end of $RCSfile$ */
