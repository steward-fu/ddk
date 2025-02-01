/*** pcicss.c - PCIC Socket Services
 *
 *  This module contains Socket Services functions.
 *
 *  Copyright (c) 1995,1996 Microsoft Corporation
 *
 *  MODIFICATION HISTORY
 */


#include "pcicss.h"

#pragma CM_PAGEABLE_DATA
#pragma CM_PAGEABLE_CODE

/***EP  SSGetSSInfo - Get Socket Services Info
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

VOID SSAPI SSGetSSInfo(PADAPTER pa, PPARAMFRAME ppf)
{
    P_ADP(ppf) = 0;             //for backward compatibility with release 1.01
    P_COMPLIANCE(ppf) = PCCARD_COMPLIANCE;
    P_NUMADPS(ppf) = 1;
    P_FIRSTADP(ppf) = 0;
    P_RETCODE(ppf) = SSERR_SUCCESS;
    P_EFLAGS(ppf) &= ~EFLAGS_CARRY;
}       //SSGetSSInfo


/***EP  SSInqAdapter - Inquire adapter
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

VOID SSAPI SSInqAdapter(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Fill in adapter information structure.
    //>>Set P_NUMEDCS(ppf) to the number of EDCs supported.
    //>>Set P_NUMSKTS(ppf) to the number of sockets provided by the adapter.
    //>>Set P_NUMWINS(ppf) to the number of windows total for all sockets.
    //>>Set P_NUMBWINS(ppf) to 0 (this is cardbus specific).
    //
    P_RETCODE(ppf) = SSERR_SUCCESS;
    P_EFLAGS(ppf) &= ~EFLAGS_CARRY;
}       //SSInqAdapter


/***EP  SSGetAdapter - Get Adapter
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *
 *  EXIT
 *      DH - State
 *      DI - SCRouting
 */

VOID SSAPI SSGetAdapter(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Set P_STATE(ppf) to the adapter state.
    //>>Set P_SCROUTING(ppf) to status change IRQ status.
    //
    P_RETCODE(ppf) = SSERR_SUCCESS;
    P_EFLAGS(ppf) &= ~EFLAGS_CARRY;
}       //SSGetAdapter


/***EP  SSSetAdapter - Set Adapter
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

VOID SSAPI SSSetAdapter(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate all incoming parameters such as:
    //>>  Check for valid IRQ level and make sure IRQ number matches currently
    //>>  set IRQ.
    //>>Enable or disable IRQ for the sockets.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //SSSetAdapter


/***EP  SSInqWindow - Inquire Window
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

VOID SSAPI SSInqWindow(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the windo number for the adapter.
    //>>Set P_WINCAPS(ppf) to the window capability.
    //>>Set P_SOCKETS(ppf) to the bit mask of sockets.
    //>>Fill in the window information structure.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //SSInqWindow


/***EP  SSGetWindow - Get Window
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

VOID SSAPI SSGetWindow(PADAPTER pa, PPARAMFRAME ppf)
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
}       //SSGetWindow


/***EP  SSSetWindow - Set Window
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

VOID SSAPI SSSetWindow(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate all paramters.
    //>>Set the window based on the paramters.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //SSSetWindow


/***EP  SSGetPage - Get Page
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

VOID SSAPI SSGetPage(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the window and page numbers.
    //>>Set P_PAGESTATE(ppf) to the page state.
    //>>Set P_OFFSET(ppf) to the memory card offset.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //SSGetPage


/***EP  SSSetPage - Set Page
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

VOID SSAPI SSSetPage(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the paramters.
    //>>Set the page based on the paramters.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //SSSetPage


/***EP  SSInqSocket - Inquire Socket
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

VOID SSAPI SSInqSocket(PADAPTER pa, PPARAMFRAME ppf)
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
}       //SSInqSocket


/***EP  SSGetSocket - Get Socket
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

VOID SSAPI SSGetSocket(PADAPTER pa, PPARAMFRAME ppf)
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
}       //SSGetSocket


/***EP  SSSetSocket - Set Socket
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

VOID SSAPI SSSetSocket(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the paramters.
    //>>Set the socket based on the parameters.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //SSSetSocket


/***EP  SSGetStatus - Get Status
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

VOID SSAPI SSGetStatus(PADAPTER pa, PPARAMFRAME ppf)
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
}       //SSGetStatus


/***EP  SSResetSocket - Reset Socket
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *      BL - Socket
 *
 *  EXIT
 *      None
 */

VOID SSAPI SSResetSocket(PADAPTER pa, PPARAMFRAME ppf)
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
}       //SSResetSocket


/***EP  SSGetVendorInfo - Get Vendor Info.
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

VOID SSAPI SSGetVendorInfo(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Validate the function code (must be 0 for now).
    //>>Set P_RELEASE(ppf) to the vendor release number.
    //>>Fill in the vendor information structure.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //SSGetVendorInfo

#pragma VxD_LOCKED_DATA_SEG
#pragma VxD_LOCKED_CODE_SEG

/***EP  SSAckInt - Acknowledge Interrupt
 *
 *  ENTRY
 *      pa -> ADAPTER structure
 *      AL - adapter number
 *
 *  EXIT
 *      CX - Sockets
 */

VOID SSAPI SSAckInt(PADAPTER pa, PPARAMFRAME ppf)
{
    //
    //>>Make sure the adapter signature is correct.
    //>>Set P_SOCKETS(ppf) to the mask of sockets reporting a status change.
    //>>Set P_RETCODE(ppf) accordingly.
    //
    SETUPCARRY(ppf);
}       //SSAckInt

#pragma CM_PAGEABLE_DATA
#pragma CM_PAGEABLE_CODE

