/*** cbss.c - CardBus Socket Services
 *
 *  This module contains Socket Services functions.
 *
 *  Copyright (c) 1995,1996 Microsoft Corporation
 *
 *  MODIFICATION HISTORY
 */


#include "cbss.h"

#pragma CM_PAGEABLE_DATA
#pragma CM_PAGEABLE_CODE

/***EP  CBSSGetSSInfo - Get Socket Services Info (PC16/PC32)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *
 *  EXIT
 *      AL = 0
 *      BX = Compliance
 *      CH = NumAdapters
 *      CL = FirstAdapter
 */

VOID SSAPI CBSSGetSSInfo(PADAPTER pa, PPARAMFRAME ppf)
{
    P_ADP(ppf) = 0;             //for backward compatibility with release 1.01
    P_COMPLIANCE(ppf) = PCCARD_COMPLIANCE;
    P_NUMADPS(ppf) = 1;
    P_FIRSTADP(ppf) = 0;
    P_RETCODE(ppf) = SSERR_SUCCESS;
    P_EFLAGS(ppf) &= ~EFLAGS_CARRY;
}       //CBSSGetSSInfo


/***EP  CBSSInqAdapter - Inquire adapter (PC16/PC32)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      EDI -> buffer
 *
 *  EXIT
 *      BH - NumWindows
 *      BL - NumSockets
 *      CX - NumEDCs
 *      DX - NumBridgeWindows
 */

VOID SSAPI CBSSInqAdapter(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Fill in adapter information structure.
    //>>Set P_NUMEDCS(ppf) to the number of EDCs supported.
    //>>Set P_NUMSKTS(ppf) to 1 (separate skts are treated as different adpt)
    //>>Set P_NUMWINS(ppf) to the number of R2 windows for this socket.
    //>>Set P_NUMBWINS(ppf) to 0 (we don't handle bridge windows, PCI does).
    //
    P_RETCODE(ppf) = SSERR_SUCCESS;
    P_EFLAGS(ppf) &= ~EFLAGS_CARRY;
}       //CBSSInqAdapter


/***EP  CBSSGetAdapter - Get Adapter (PC16/PC32)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *
 *  EXIT
 *      DH - State
 *      DI - SCRouting
 */

VOID SSAPI CBSSGetAdapter(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Set P_STATE(ppf) to the adapter state.
    //>>Set P_SCROUTING(ppf) to status change IRQ status.
    //
    P_RETCODE(ppf) = SSERR_SUCCESS;
    P_EFLAGS(ppf) &= ~EFLAGS_CARRY;
}       //CBSSGetAdapter


/***EP  CBSSSetAdapter - Set Adapter (PC16/PC32)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      DH - State
 *      DI - SCRouting
 *
 *  EXIT
 *      None
 */

VOID SSAPI CBSSSetAdapter(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate all incoming parameters such as:
    //>>  Check for valid IRQ level and make sure IRQ number matches currently
    //>>  set IRQ.
    //>>Enable or disable IRQ for the sockets.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSSetAdapter


/***EP  CBSSInqWindow - Inquire Window (PC16)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BH - window number
 *      EDI -> buffer
 *
 *  EXIT
 *      BL - WndCaps
 *      CX - Sockets
 */

VOID SSAPI CBSSInqWindow(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the windo number for the adapter.
    //>>Set P_WINCAPS(ppf) to the window capability.
    //>>Set P_SOCKETS(ppf) to the bit mask of sockets.
    //>>Fill in the window information structure.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSInqWindow


/***EP  CBSSGetWindow - Get Window (PC16)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BH - window number
 *
 *  EXIT
 *      BL - Socket
 *      CX - Size
 *      DH - State
 *      DL - Speed
 *      DI - Base
 */

VOID SSAPI CBSSGetWindow(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the window number for the adapter.
    //>>Set P_SOCKET(ppf) to the socket number window is assigned to.
    //>>Set P_WINSIZE(ppf) to the window size.
    //>>Set P_STATE(ppf) to the window state.
    //>>Set P_SPEED(ppf) to the window speed.
    //>>Set P_BASE(ppf) to the window base address.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSGetWindow


/***EP  CBSSSetWindow - Set Window (PC16)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BH - window number
 *      BL - socket number
 *      CX - Size
 *      DH - State
 *      DL - Speed
 *      DI - Base
 *
 *  EXIT
 *      None
 */

VOID SSAPI CBSSSetWindow(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate all paramters.
    //>>Set the window based on the paramters.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSSetWindow


/***EP  CBSSGetPage - Get Page (PC16)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BH - Window
 *      BL - Page
 *
 *  EXIT
 *      DL - State
 *      DI - Offset
 */

VOID SSAPI CBSSGetPage(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the window and page numbers.
    //>>Set P_PAGESTATE(ppf) to the page state.
    //>>Set P_OFFSET(ppf) to the memory card offset.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSGetPage


/***EP  CBSSSetPage - Set Page (PC16)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BH - Window
 *      BL - Page
 *      DL - State //BUGBUG: Spec say DH
 *      DI - Offset
 *
 *  EXIT
 *      None
 */

VOID SSAPI CBSSSetPage(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the paramters.
    //>>Set the page based on the paramters.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSSetPage


/***EP  CBSSInqSocket - Inquire Socket (PC16/PC32)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BL - Socket
 *      EDI - pBuffer
 *
 *  EXIT
 *      BH - SCIntCaps
 *      DH - SCRptCaps
 *      DL - CtlIndCaps
 */

VOID SSAPI CBSSInqSocket(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the socket number.
    //>>Set P_SCINTCAPS(ppf) to the status change interrupt capability.
    //>>Set P_SCRPTCAPS(ppf) to the socket event capability.
    //>>Set P_CLTINDCAPS(ppf) to the socket control or indicator capability.
    //>>Fill in the socket information structure.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSInqSocket


/***EP  CBSSGetSocket - Get Socket (PC16/PC32)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BL - Socket
 *
 *  EXIT
 *      BH - SCIntMask
 *      CH - Vcontrol
 *      CL - VppLevels
 *      DH - State
 *      DL - CtlInd
 *      DI - IFType,IREQRouting
 *      BP - IFIndex
 */

VOID SSAPI CBSSGetSocket(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the socket number.
    //>>Set P_SCINTMASK(ppf) to the status change interrupt event mask.
    //>>Set P_VCONTROL(ppf) to the socket Vcc level.
    //>>Set P_VPPLEVELS(ppf) to the socket Vpp1 and Vpp2 levels.
    //>>Set P_STATE(ppf) to the socket state.
    //>>Set P_CTLIND(ppf) to the socket control or indicator state.
    //>>Set P_IF_ROUTING(ppf) to the IRQ steering level and interface type.
    //>>Set P_IFINDEX(ppf) to the custom interface index.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSGetSocket


/***EP  CBSSSetSocket - Set Socket (PC16/PC32)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BL - Socket
 *      BH - SCIntMask
 *      CH - Vcontrol
 *      CL - VppLevels
 *      DH - State
 *      DL - CtlInd
 *      DI - IFType,IREQRouting
 *      BP - IFIndex
 *
 *  EXIT
 *      None
 */

VOID SSAPI CBSSSetSocket(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the paramters.
    //>>Set the socket based on the parameters.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSSetSocket


/***EP  CBSSGetStatus - Get Status (PC16/PC32)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BL - Socket
 *
 *  EXIT
 *      BH - CardState
 *      DH - State
 *      DL - CtlInd
 *      DI - IFType,IREQRouting
 */

VOID SSAPI CBSSGetStatus(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the socket number.
    //>>Set P_CARDSTATE(ppf) to the card state.
    //>>Set P_STATE(ppf) to the socket state.
    //>>Set P_CTLIND(ppf) to the socket control or indicator state.
    //>>Set P_IF_ROUTING(ppf) to the IRQ steering level and interface type.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSGetStatus


/***EP  CBSSResetSocket - Reset Socket (PC16/PC32)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BL - Socket
 *
 *  EXIT
 *      None
 */

VOID SSAPI CBSSResetSocket(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the socket number.
    //>>Make sure there is a card in the socket.
    //>>Disable all windows for the socket.
    //>>Disable IREQ routing.
    //>>Reset custom interface.
    //>>Set all power levels to CIS read level.
    //>>Reset any card in the sockets.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSResetSocket


/***EP  CBSSGetVendorInfo - Get Vendor Info. (PC16/PC32)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BL - Type
 *      EDI - pBuffer
 *
 *  EXIT
 *      DX - Release
 */

VOID SSAPI CBSSGetVendorInfo(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the function code (must be 0 for now).
    //>>Set P_RELEASE(ppf) to the vendor release number.
    //>>Fill in the vendor information structure.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSGetVendorInfo

#pragma VxD_LOCKED_DATA_SEG
#pragma VxD_LOCKED_CODE_SEG

/***EP  CBSSAckInt - Acknowledge Interrupt (PC16/PC32)
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *
 *  EXIT
 *      CX - Sockets
 */

VOID SSAPI CBSSAckInt(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Make sure the adapter signature is correct.
    //>>Set P_SOCKETS(ppf) to the mask of sockets reporting a status change.
    //>>Clear the interrupt.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //CBSSAckInt

#pragma CM_PAGEABLE_DATA
#pragma CM_PAGEABLE_CODE

