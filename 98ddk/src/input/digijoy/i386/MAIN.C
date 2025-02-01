/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1998  Microsoft Corporation.  All Rights Reserved.
 * 
 *  Notes:
 *
 *  File:       main.c
 *  Content:    This file handles all callbacks from VJOYD.VXD
 **************************************************************************/

/*
 *  configmg.h will be included but fail without these macros being defined
 */
#define CAT_HELPER(x, y)    x##y
#define CAT(x, y)       CAT_HELPER(x, y)
#define MAKE_HEADER(RetType, DecType, Function, Parameters)

#include <basedef.h>
#include <vmm.h>
#include <vmmreg.h>
#include <debug.h>
#include <regstr.h>

#include "digijoy.h"
#include "wraps.h"

#pragma VxD_LOCKED_DATA_SEG

#pragma VxD_PAGEABLE_CODE_SEG

char szComboMapping[] = "OEMComboMapping";
char szRegPollFlags[] = "PollFlags";
char szTypeKey[]="Standard Gameport";
extern DWORD dwTSCScale;
DWORD dwTickTimeout;
DWORD dwPollFlags;
DWORD dwPollType;

void (__cdecl *fpPoll)( void );
void (__stdcall *NewTimeout)( DWORD ); /*  Set to NULL during init */

/*
 *  These are only hit during config manager callbacks (or fake ones)
 *  we should be safe to touch these without synchronization
 */
PDEVNODEDESC    pDevnodes;
int             NumDevnodes;
int             MaxDevnodes;

/****************************************************************************
 *
 *  Function: GetComboInfo
 *      Read the POV mapping from the registry and process into an easy to 
 *      handle form for run time usage.
 *
 ***************************************************************************/
void GetComboInfo( PDEVNODEDESC pDevnodeDesc )
{
    DWORD       type;
    DWORD       cb;
    VMMHKEY     hkSettings;
    VMMREGRET   reg_rc;
    DWORD       id;
    int         idx;
    char        regval[ MAX_JOYSTICKOEMVXDNAME ];
    DWORD       reg_mapping[16];
    DWORD       dwButtons = 1;

    /*
     *  Reset the map to a four button no combo map, all POVs centered
     *
     *  (Use the fact that JOY_POVCENTERED is -1)
     */
    memset( pDevnodeDesc->rgwPOVMap, (BYTE)-1, sizeof( pDevnodeDesc->rgwPOVMap ) );
    for( idx=15; idx>=0; idx-- )
    {
        pDevnodeDesc->rgwBtnMap[idx]=(WORD)idx;
    }
    
    pDevnodeDesc->dwPollMask &= ~( JOYPD_POV0 | JOYPD_POV1 | JOYPD_POV2 | JOYPD_POV3 );

    /*
     *  Look up our settings key by:
     *      opening the current configuration key
     *      reading the type name for our id
     *      closing the current configuration key
     *      opening our type key
     *      reading the combo mappings
     *      closing the type key
     *      processing the combo mappings
     */

    id = pDevnodeDesc->id;

    __asm mov eax,id
    __asm lea ecx,hkSettings
    VxDCall( VJOYD_OpenConfigKey_Service )
    Touch_Register( edx );
    __asm mov reg_rc,eax

    if( reg_rc != ERROR_SUCCESS )
    {
        EPF( "Failed to open curr config key for combo mappings, code %08x", reg_rc );
    }
    else
    {
        SprintfOne( regval, REGSTR_VAL_JOYNOEMNAME, id+1 );
        cb = sizeof( regval );
        reg_rc = VMM_RegQueryValueEx( hkSettings, regval, NULL, &type, (PBYTE)&regval, &cb );
        VMM_RegCloseKey( hkSettings );

        if( ( reg_rc != ERROR_SUCCESS ) || ( type != REG_SZ ) )
        {
            EPF( "Failed to read type of id %l with code %08x", id, reg_rc );
        }
        else
        {
            __asm lea eax,regval
            __asm lea ecx,hkSettings
            VxDCall( VJOYD_OpenTypeKey_Service )
            Touch_Register( edx );
            __asm mov reg_rc,eax

            if( reg_rc != ERROR_SUCCESS )
            {
                EPF( "Failed to open type key %s for combo mappings, code %08x", regval, reg_rc );
            }
            else
            {
                cb = sizeof( reg_mapping );
                reg_rc = VMM_RegQueryValueEx( hkSettings, szComboMapping, NULL, &type, (PBYTE)&reg_mapping, &cb );

                VMM_RegCloseKey( hkSettings );

                if( ( reg_rc != ERROR_SUCCESS ) 
                  ||( type != REG_BINARY )
                  ||( cb != sizeof( reg_mapping ) ) )
                {
                    EPF( "Failed to read combo mappings flags code %08x, defaulting non-combo", reg_rc );
                }
                else
                {
                    /*
                     *  In the registry, the combo mappings are packed such 
                     *  that each combination is represented by a DWORD, with
                     *  the index of the DWORD being the button combination.
                     *  The packing is as follows:
                     *  aaaa aaaa aaaa aaap pbbb bbbb bbbb bbbb
                     *  Where a is a bit from the POV angle, p is a bit from 
                     *  the POV index and b is a bit from the button bitmask.
                     *  In this format 15 buttons and 4 15 bit POVs can be 
                     *  stored with only one POV being non-centered for any 
                     *  combination but potentially any button map allowed.
                     *
                     *  To reduce complexity in the poll routine these are 
                     *  unpacked here.
                     */
                    dwButtons = 0;

                    for( idx=15; idx>=0; idx-- )
                    {
                        dwButtons |= reg_mapping[idx];
                        pDevnodeDesc->rgwBtnMap[idx] = (WORD)( LOWORD(reg_mapping[idx]) & (WORD)0x7fff );
                        if( (HIWORD(reg_mapping[idx]) & 0xfffe) < 36000 ) 
                        {
                            pDevnodeDesc->rgwPOVMap[idx][(reg_mapping[idx] >> 15) & 3] 
                                = (WORD)( (HIWORD(reg_mapping[idx]) & (WORD)0xfffe) );
                            pDevnodeDesc->dwPollMask 
                                |= ( JOYPD_POV0 << ( (reg_mapping[idx] >> 15) & 3 ) );
                        }
                    }
                }
        
            }
        }
    }

    /*
     *  If buttons were anywhere, we have between 1 and 32 buttons.
     */
    if( dwButtons & 0x7fff )
    {
        pDevnodeDesc->dwPollMask |= JOYPD_BTN0;
    }

} /* GetComboInfo */

/****************************************************************************
 *
 *  Function: GetAxisInfo
 *      Update the axes to be supported from the flags in the registry
 *
 ***************************************************************************/
void GetAxisInfo( DWORD dwId, PDWORD pdwPollMask )
{
    DWORD           type;
    DWORD           cb;
    VMMHKEY         hkSettings;
    VMMREGRET       reg_rc;
    char            regval[ MAX_JOYSTICKOEMVXDNAME ];
    JOYREGHWCONFIG  hwcfg;

    /*
     *  All these devices have at least X and Y.
     */
    *pdwPollMask = JOYPD_POSITION | JOYPD_X | JOYPD_Y;

    __asm mov eax,dwId
    __asm lea ecx,hkSettings
    VxDCall( VJOYD_OpenConfigKey_Service )
    Touch_Register( edx );
    __asm mov reg_rc,eax

    if( reg_rc != ERROR_SUCCESS )
    {
        EPF( "Failed to open curr config key for axis config, code %08x", reg_rc );
    }
    else
    {
        SprintfOne( regval, REGSTR_VAL_JOYNCONFIG, dwId+1 );
        cb = sizeof( hwcfg );
        reg_rc = VMM_RegQueryValueEx( hkSettings, regval, NULL, &type, (PBYTE)&hwcfg, &cb );
        VMM_RegCloseKey( hkSettings );

        if( ( reg_rc != ERROR_SUCCESS ) 
          ||( type != REG_BINARY )
          ||( cb != sizeof( hwcfg ) ) )
        {
            EPF( "Failed to read axis config code %08x, defaulting to 2 axis", reg_rc );
        }
        else
        {
            if( hwcfg.hws.dwFlags & JOY_HWS_HASZ )
            {
                *pdwPollMask |= JOYPD_Z;
            }

            if( ( hwcfg.hws.dwFlags & JOY_HWS_HASR ) 
             || ( hwcfg.dwUsageSettings & JOY_US_HASRUDDER ) )
            {
                *pdwPollMask |= JOYPD_R;
            }
        }
    }
} /* GetAxisInfo */

/****************************************************************************
 *
 *  Function: GetPortDataPtr
 *      Find the stored data for this id.
 *
 ***************************************************************************/
PDEVNODEDESC GetPortDataPtr( DWORD dwId )
{
    int Idx;

    for( Idx=0; Idx<NumDevnodes; Idx++ )
    {
        if( pDevnodes[Idx].id == dwId ) 
        {
            return( &pDevnodes[Idx] );
        }
    }

    return( (PDEVNODEDESC)NULL );
} /* GetPortDataPtr */

/****************************************************************************
 *
 *  CtrlMsg: Handle control messages from VJoyD.
 *      This procedure handles joystick mini-driver control messages
 *      generated by VJoyD.  It was originally intended that there would
 *      be several different types of messages but only two are currently
 *      defined.
 *
 *      VJCM_PASSDRIVERDATA is sent when an application calls joyGetPosEx
 *      with the flag JOY_PASSDRIVERDATA set.  This allows applications to 
 *      pass a single DWORD of data (passed in dwParam) to a mini-driver.
 *
 *      VJCM_CONFIGCHANGED is sent when a configuration change has occured.
 *      Normally this is as a result of an application, typically a control
 *      panel, calling joyConfigChanged but it will also happen when the 
 *      16-bit joystick driver (MSJSTICK.DRV) is initialized. *      
 *
 *  ENTRY:      DWORD   dwId    the joystick id being referenced
 *              DWORD   dwMsg   the message number (see above)
 *              DWORD   dwParam message specific data
 *
 *  RETURNS:    HRESULT Standard HRESULT, depending on the result of 
 *                  attempting to process the message.  Typically one of:
 *                  VJ_OK       successfully processed
 *                  VJ_DEFAULT  ignored
 *                  VJERR_FAIL  processing failed on a recognized message
 *
 ***************************************************************************/
HRESULT __stdcall CtrlMsg( DWORD dwId, DWORD dwMsg, DWORD dwParam )
{
    HRESULT     rc = VJ_DEFAULT;
    VMMHKEY     hkSettings;
    VMMREGRET   reg_rc;
    DWORD       type;
    DWORD       cb;

    if( dwMsg == VJCM_CONFIGCHANGED )
    {
        /*
         *  The only change type currently implemented is VJCMCT_GENERAL so 
         *  ( ((LPVJCFGCHG)dwParam)->dwChangeType == VJCMCT_GENERAL ) can be 
         *  assumed to be true.  If any future version of VJoyD implement 
         *  other change types, a new registration flag will be defined to 
         *  allow mini-drivers report support for them.
         */
        
        /*
         *  Most real digital joystick drivers do not have to worry about
         *  the timeout value changing but since we're faking it...
         */ 
        if( NewTimeout )
        {
            ZPF( "Setting new timeout of %d", ((LPVJCFGCHG)dwParam)->dwTimeOut );
            if( ((LPVJCFGCHG)dwParam)->dwSize != sizeof(VJCFGCHG) )
            {
                WPF( "ConfigChanged structure size is %d, was expecting %d",
                    ((LPVJCFGCHG)dwParam)->dwSize, sizeof(VJCFGCHG) );
            }
            NewTimeout( ((LPVJCFGCHG)dwParam)->dwTimeOut );
            rc = VJ_OK;
        }
        else
        {
            EPF( "No timeout converter initialized" );
            rc = VJERR_FAIL;
        }

        /*
         *  Look in the registry to see if our settings have been changed.
         *
         *  First update our button combo mappings
         *  Then update the interrupt enable status by looking at the standard 
         *  global gameport type.  If the standard global gameport is in use, 
         *  the CPL will allow this flag to be changed via a check box on the 
         *  Advanced page.  Of course if any other global gameport driver is 
         *  being used, this flag only reflects either the last selection 
         *  made before another driver was selected, or the default but that's
         *  as good as we can get in that case.  Besides, a real digital 
         *  driver won't care about the flag and this sample won't work if 
         *  the standard gameport driver won't function since they are based 
         *  on the same polling code.
         */

        GetComboInfo( GetPortDataPtr( dwId ) );

        __asm lea eax,szTypeKey
        __asm lea ecx,hkSettings
        VxDCall( VJOYD_OpenTypeKey_Service )
        Touch_Register( edx );
        __asm mov reg_rc,eax

        if( reg_rc != ERROR_SUCCESS )
        {
            EPF( "Failed to open type key %s with code %08x", szTypeKey, reg_rc );
            dwPollFlags = 0;
        }
        else
        {
            cb = sizeof( dwPollFlags );
            reg_rc = VMM_RegQueryValueEx( hkSettings, szRegPollFlags, NULL, &type, (PBYTE)&dwPollFlags, &cb );
            if( reg_rc != ERROR_SUCCESS )
            {
                WPF( "Failed to read poll flags code %08x, defaulting to zero", reg_rc );
                /*
                 *  Don't rely on VMM_RegQueryValueEx not to trash output on 
                 *  failure so zero the result here just in case
                 */
                dwPollFlags = 0;
            }
            else
            {
                IPF( "Read poll flags of %08x", dwPollFlags );
            }
            VMM_RegCloseKey( hkSettings );
        
        }

        /*
         *  Change the polling type 
         *  Unless no devnodes have been started
         */
        if( fpPoll )
        {
            /* Only change the interrupt flag */
            dwPollType = ( dwPollFlags & PF_INTMASK ) | ( dwPollType & ~PF_INTMASK );

            if( dwPollType > MAX_POLL )
            {
                EPF( "Never expected no polling set up here!  Defaulting to SW\r\n" );
                dwPollType = DSW_POLL | ( dwPollFlags & PF_INTMASK );
                rc = VJERR_FAIL;
            }
            else
            {
                rc = VJ_OK;
            }
                
            switch( dwPollType )
            {
            case DSW_POLL:
                fpPoll = DSWPoll;
                IPF( "Interrupt disabled software loop count polling\r\n" );
                break;

            case ESW_POLL:
                fpPoll = ESWPoll;
                IPF( "Interrupt enabled software loop count polling\r\n" );
                break;

            case DRTC_POLL:
                fpPoll = DRTCPoll;
                IPF( "Interrupt disabled RealTime Clock polling\r\n" );
                break;

            case ERTC_POLL:
                fpPoll = ERTCPoll;
                IPF( "Interrupt enabled RealTime Clock polling\r\n" );
                break;
    
            case DTSC_POLL:
                fpPoll = DTSCPoll;
                IPF( "Interrupt disabled Time Stamp Counter polling\r\n" );
                break;

            case ETSC_POLL:
                fpPoll = ETSCPoll;
                IPF( "Interrupt enabled Time Stamp Counter polling\r\n" );
                break;
            }
        }
        else
        {
            EPF( "No polling core initialzed" );
            rc = VJERR_FAIL;
        }
    }
    else if( dwMsg == VJCM_PASSDRIVERDATA )
    {
        IPF( "Who's been passing data (%08x) to this driver?", dwParam );
    }

    return rc;
} /* CtrlMsg */

/****************************************************************************
 *
 *  Function: ScaleTSC
 *      Scale the TSC values in the passed four DWORD array
 *      The passed array is both the input and output.
 *      This scaling uses the dwTSCScale value calculated in InitTSC to take 
 *      account of the difference in speed between different CPU clocks.
 *
 ***************************************************************************/
void ScaleTSC( PDWORD pdwTSC )
{
    __asm push esi
    __asm mov esi,pdwTSC
    __asm add esi,12
    __asm mov ecx,dwTSCScale

ScaleTSCLoop:
    __asm mov eax,[esi]
    __asm mul ecx
    __asm shr eax,20
    __asm mov [esi],eax
    __asm sub esi,4
    __asm cmp esi,pdwTSC
    __asm jae ScaleTSCLoop

    __asm pop esi
    return;
} /* ScaleTSC */

/****************************************************************************
 *
 *  Function: ScaleRTC
 *      Scale the RTC values in the passed four DWORD array
 *      The passed array is both the input and output.
 *      Since the RTC is by definition the same speed on all machines, the 
 *      scale is a simple shift.
 *
 ***************************************************************************/
void ScaleRTC( PDWORD pdwRTC )
{
    *pdwRTC >>= 1;
    *(pdwRTC+1) >>= 1;
    *(pdwRTC+2) >>= 1;
    *(pdwRTC+3) >>= 1;
    return;
} /* ScaleRTC */

/****************************************************************************
 *
 *  Function: CopyAxes
 *      Selectively copy the axis values conditional upon the dwMask bitmask.
 *
 ***************************************************************************/
void CopyAxes( PDWORD pdwDest, PDWORD pdwSrc, DWORD dwMask )
{
    DWORD   CopyMask;

    for( CopyMask = 1; CopyMask < 32; CopyMask <<= 1 )
    {
        if( CopyMask & dwMask )
        {
            *pdwDest = *pdwSrc;
        }
        pdwDest++;
        pdwSrc++;
    }

    return;
} /* CopyAxes */

/****************************************************************************
 *
 *  Function: RTCFilterAxes
 *      Try to filter out the RTC timing anomilies and any poll results which 
 *      have been invalidated by interrupt hits.
 *
 ***************************************************************************/
HRESULT INLINE_RETAIL RTCFilterAxes( PDWORD pdwDest, PDWORD pdwLimits, DWORD dwMask, PDWORD pdwGlitches )
{
    DWORD   CopyMask;
    int     Axis;
    HRESULT rc = VJ_OK;

    for( CopyMask = 8, Axis = 3; Axis >= 0; CopyMask >>= 1, Axis-- )
    {
        if( CopyMask & dwMask )
        {
            DWORD dwNew;
            DWORD dwNext;
            DWORD dwLast;

            if( *(pdwLimits+4+Axis) - *(pdwLimits+Axis) > dwIntLimit )
            {
                /* 
                 *  We hit an interrupt, so let caller know results are not
                 *  perfect.
                 */
                rc = VJ_FALSE;

                /*
                 *  Using a delta here tends to exaggerate the normal jitter
                 *  so use the last value as an approximation but check 
                 *  against the known limits
                 */
                IPF( "%d   %04x,   %04x  and  %04x", 
                    Axis, *(pdwDest+Axis), *(pdwLimits+Axis), *(pdwLimits+4+Axis) );

                if( *(pdwDest+Axis) > *(pdwLimits+4+Axis) )
                {
                    dwNew = *(pdwLimits+4+Axis); /* Above, use upper limit */
                }
                else if( *(pdwDest+Axis) < *(pdwLimits+Axis) )
                {
                    dwNew = *(pdwLimits+Axis); /* Below, use lower limit */
                }
                else
                {
                    /*
                     *  This guarantees that it will be considered a 
                     *  non-glitch which is a flaw
                     */
                    dwNew = *(pdwDest+Axis);
                }
            }
            else
            {
                /*
                 *  No interrupt, get the value
                 */
                dwNew = ( *(pdwLimits+Axis) + *(pdwLimits+4+Axis) ) >> 1;
            }

            /*
             *  OK we now have a candidate value
             *  So call it good and use it to check the previous value for potential glitch
             *  new  in dwGuess
             *  last in *(pdwDest+Axis)
             *  next in *(pdwDest+Axis+4)
             *  if all is well, next -> last; new -> next; (last is discarded)
             *  if next looks bad, new -> next; (next is discarded)
             *  this relies on not getting two glitches in a row
             */
            dwNext = *(pdwDest+Axis+4);
            dwLast = *(pdwDest+Axis);
            if( ( ( dwNext >= dwLast - dwIntLimit ) && ( dwNext <= dwNew + dwIntLimit ) )
              ||( ( dwNext <= dwLast + dwIntLimit ) && ( dwNext >= dwNew - dwIntLimit ) ) )
            {
                /*
                 *  All is well (enough)
                 */
                *(pdwDest+Axis) = dwNext; 
                *pdwGlitches &= ~CopyMask;
            }
            else
            {
                if( *pdwGlitches & CopyMask )
                {
                    /*
                     *  A double glitch is very unlikely, so assume the stick 
                     *  is being moved fast.  Without this, fast movement 
                     *  causes the values to stick
                     */
                    *pdwGlitches &= ~CopyMask;
                    *(pdwDest+Axis) = dwNext;
                }
                else
                {
                    *pdwGlitches |= CopyMask;
                }
                IPF( "Last %04x, Next %04x, New  %04x", dwLast, dwNext, dwNew );
            }
            *(pdwDest+Axis+4) = dwNew;
        }
    }

    return rc;
} /* RTCFilterAxes */

/****************************************************************************
 *
 *  Function: TSCFilterAxes
 *      The TSC is perfectly accurate (unless the CPU clock gets changed by
 *      some power management but that can be ignored) so the only thing to 
 *      deal with is interrupt hits between the last high pol on an axis and 
 *      the first low poll.  If this happens, it is fairly rare so just use 
 *      the last known good value.
 *
 ***************************************************************************/
HRESULT INLINE_RETAIL TSCFilterAxes( PDWORD pdwDest, PDWORD pdwLimits, DWORD dwMask )
{
    DWORD   CopyMask;
    int     Axis;
    HRESULT rc = VJ_OK;

    for( CopyMask = 8, Axis = 3; Axis >= 0; CopyMask >>= 1, Axis-- )
    {
        if( CopyMask & dwMask )
        {
            /*
             *  each iteration is one so 3 would be a hit dwIntLimit )
             */
            if( *(pdwLimits+4+Axis) - *(pdwLimits+Axis) > 2 ) 
            {
                /* 
                 *  We hit an interrupt, so let caller know results are not
                 *  perfect.
                 */
                rc = VJ_FALSE;

                /*
                 *  Using a delta here tends to exaggerate the normal jitter
                 *  so use the last value as an approximation but check 
                 *  against the known limits
                 */
                IPF( "%d   %04x,   %04x  and  %04x", 
                    Axis, *(pdwDest+Axis), *(pdwLimits+Axis), *(pdwLimits+4+Axis) );

                if( *(pdwDest+Axis) > *(pdwLimits+4+Axis) )
                {
                    *(pdwDest+Axis) = *(pdwLimits+4+Axis); /* Above, use upper limit */
                }
                else if( *(pdwDest+Axis) < *(pdwLimits+Axis) )
                {
                    *(pdwDest+Axis) = *(pdwLimits+Axis); /* Below, use lower limit */
                }
            }
            else
            {
                /*
                 *  No interrupt, get the value
                 */
                *(pdwDest+Axis) = ( *(pdwLimits+Axis) + *(pdwLimits+4+Axis) ) >> 1;
            }
        }
    }

    return rc;
} /* TSCFilterAxes */

/****************************************************************************
 *
 *  Function: AnalogPoll
 *      Call the poll function and the appropriate axis copying and filtering 
 *      functions.
 *
 ***************************************************************************/
HRESULT __stdcall AnalogPoll( PDEVNODEDESC pdnDesc, DWORD dwMask, PDWORD pdwButtons )
{
    DWORD   rgdwAxis[2][4];
    HRESULT rc;

    if( fpPoll == NULL )
    {
        EPF( "Polling not initialized" );
        return VJERR_FAIL;
    }

    rc = jsPoll( dwMask, pdnDesc->io, pdwButtons, &rgdwAxis[0][0], dwTickTimeout );
    /*
     *  rc is zero for error, non-zero otherwise
     */

    if( rc )
    {
        /* 
         *  Polled OK but this may be modified to VJ_FALSE if an interrupt 
         *  is detected during filtering.
         */ 
        rc = VJ_OK;

        switch( dwPollType )
        {
        case DSW_POLL:
        case ESW_POLL:
            CopyAxes( &pdnDesc->rgdwLastValues[0], &rgdwAxis[0][0], dwMask );
            break;

        case DRTC_POLL:
            ScaleRTC( &rgdwAxis[0][0] );
            CopyAxes( &pdnDesc->rgdwLastValues[0], &rgdwAxis[0][0], dwMask );
            break;

        case ERTC_POLL:
            ScaleRTC( &rgdwAxis[0][0] );
            ScaleRTC( &rgdwAxis[1][0] );
            rc = RTCFilterAxes( &pdnDesc->rgdwLastValues[0], &rgdwAxis[0][0], dwMask, &pdnDesc->dwGlitches );
            break;
        
        case DTSC_POLL:
            ScaleTSC( &rgdwAxis[0][0] );
            CopyAxes( &pdnDesc->rgdwLastValues[0], &rgdwAxis[0][0], dwMask );
            break;

        case ETSC_POLL:
            ScaleTSC( &rgdwAxis[0][0] );
            ScaleTSC( &rgdwAxis[1][0] );
            rc = TSCFilterAxes( &pdnDesc->rgdwLastValues[0], &rgdwAxis[0][0], dwMask );
            break;

        default:
            EPF( "Bad poll function @%08x",fpPoll );
            break;
        }
    }
    else
    {
        rc = VJERR_FAIL;
    }

    return rc;
} /* AnalogPoll */

/****************************************************************************
 *
 *  PollRoutine: The poll callback called by VJOYD to get device data
 *      
 *  ENTRY:      DWORD           dwId    the joystick id being referenced
 *              PDWORD          pdwMask flags to indicate what data should 
 *                                      be returned
 *              LPVJPOLLDATA    pd      address of array to hold results
 *
 *  RETURNS:    HRESULT Standard HRESULT, depending on the result of 
 *                  attempting to poll the device.  Typically one of:
 *                  VJ_OK       successfully polled
 *                  VJ_DEFAULT  some data is dubious (data may be stale)
 *                  VJERR_FAIL  device unplugged or failed
 *  NOTES:
 *      1. Since this interface supports differnt types of polls (currently 
 *      position, velocity, acceleration and force) it is critical that the 
 *      driver check which type of poll is being requested in the high WORD 
 *      of the flags pointed to by pdwFlags.  
 *      2. The low word points out which elements (X, third DWORD of buttons, 
 *      POV2) is being requested.  There are some problems with the VJoyD 
 *      handling of this in certain situations which causes VJoyD not to 
 *      request an element (POV0) which it has already established is 
 *      available but then to process the result anyway.  The way to handle 
 *      this is, once you have established that you should respond to this 
 *      type of poll, simply AND the bitmask passed with the mask of what 
 *      your driver is responding with.  This way, your driver will function 
 *      with the bad current behavior and any future corrected behavior.  
 *      3. In many circumstances, the flags passed will indicate that all 
 *      possible elements be returned; this is not an error you should just 
 *      return those elements that your device supports and set the flags as 
 *      described above.  
 *      4. You should not set any bits in the passed mask for output that 
 *      were not set on input.  Doing so will cause polls to be failed.
 *      5. It is not necessary to assign zero or any other values to elements 
 *      which your driver does not support, nor should you rely on the 
 *      initial values of elements that your driver does support.  
 *      6. If your driver supports poll types other than position, please 
 *      note that the poll for each type is always stored at the same offset 
 *      relative to pd regardless of the other types that are being 
 *      requested.  
 *      7. Also if two types of data are requested simultaneously,
 *      the element mask used to and with the requested mask should be the 
 *      OR of the element mask for each type.  For instance if a driver is 
 *      requested to return X, Y, Z, R, U, V for position and velocity but 
 *      the device supports only X and Y for position and R and U for 
 *      velocity then the driver should or the passed mask with 
 *      (JOYPD_POSITION|JOYPD_VELOCITY|JOYPD_X|JOYPD_Y|JOYPD_R|JOYPD_U).
 *      8. If it is not possible to return data due to some short term 
 *      problem, it is usually better to return stale data than to fail the 
 *      poll.  This is particularly important for the first poll by some 
 *      applications as they use this to determine the presence of the 
 *      device so failing the first poll will cause the device to be unused. 
 *      
 ***************************************************************************/
HRESULT __stdcall PollRoutine( DWORD dwId, PDWORD pdwMask, LPVJPOLLDATA pd )
{
    DWORD           dwMask;
    DWORD           dwButtons;
    PDEVNODEDESC    pPortData;

    
    if( ( *pdwMask & JOYPD_POSITION ) == 0 )
    {
        /*
         *  We only support position data, so set the return mask to zero
         *  to indicate that nothing is returned and leave.
         */
        *pdwMask = 0 ;
        return( VJ_INCOMPLETE );
    }

    pPortData = GetPortDataPtr( dwId );

    if( pPortData )
    {
        if( pPortData->io )
        {
            /*
             *  Set the returned axis mask
             *  Then get a bitmask for the port poll
             */
            *pdwMask &= pPortData->dwPollMask;
            dwMask = *pdwMask & ( JOYPD_X | JOYPD_Y | JOYPD_Z | JOYPD_R );

            /*
             *  Forgot to swap Z and R in the mask.
             *  Don't do this at home
             */
            if( !( dwMask & JOYPD_Z ) != !( dwMask & JOYPD_R ) )
            {
                dwMask ^= (JOYPD_Z | JOYPD_R );
            }
            
            if( SUCCEEDED( AnalogPoll( pPortData, dwMask, &dwButtons ) ) )
            {
                /*
                 *  For this generic example, it would take longer to 
                 *  selectively copy those axes we really care about, so 
                 *  just copy the lot.
                 *  This is also where we swap the R/Z order
                 */
                pd->dwX = pPortData->rgdwLastValues[0];
                pd->dwY = pPortData->rgdwLastValues[1];
                pd->dwR = pPortData->rgdwLastValues[2];
                pd->dwZ = pPortData->rgdwLastValues[3];

                ZPF( "Polled: %d, X:%x  Y:%x  Z:%x  R:%x  B:%x",dwId, pd->dwX, pd->dwY, pd->dwZ, pd->dwR, dwButtons );

                pd->dwBTN0 = (DWORD)pPortData->rgwBtnMap[dwButtons];
                pd->dwPOV0 = (DWORD)pPortData->rgwPOVMap[dwButtons][0];
                pd->dwPOV1 = (DWORD)pPortData->rgwPOVMap[dwButtons][1];
                pd->dwPOV2 = (DWORD)pPortData->rgwPOVMap[dwButtons][2];
                pd->dwPOV3 = (DWORD)pPortData->rgwPOVMap[dwButtons][3];
            }
            else
            {
                ZPF( "Poll for %08x failed", dwMask );
                return( VJERR_FAIL );
            }
        }
        else
        {
            EPF( "ID %d on devnode %08x has no IO port", dwId, pPortData->dn );
            return( VJERR_FAIL );
        }
    }
    else
    {
        EPF( "ID %d not matched in poll", dwId );
        return( VJERR_INVALIDPARAM );
    }

    return( VJ_OK );

} /* PollRoutine */

/****************************************************************************
 *
 *  Function: StartDevnode
 *      We've been allocated a devnode so find out what we need to to set 
 *      up our internal structures.
 *
 ***************************************************************************/
CONFIGRET INLINE_RETAIL StartDevnode( DEVNODE dnDevnode )
{
    CMCONFIG    ccb;        /* config buffer */
    CONFIGRET   cr;
    DWORD       dwLength;
    PCHAR       pszDeviceId;
    int         Idx;
    int         DNIdx;

    /*
     * Get our resource allocation from CM.  if this fails,
     * we have no choice but to fail the CONFIG_START.
     * We'll return the same error that the CM_Get_Alloc_Conf
     * returned.
     */

    cr = CM_Get_Alloc_Log_Conf( &ccb, dnDevnode, 0 );
    if( cr != CR_SUCCESS ) 
    {
        /*
         *  It is possible that this is a replacement devnode for one we had 
         *  already, but there's nothing we can do if CM is not cooperating.
         */
        return cr;
    }

    /*
     *  Get the unique device ID for this devnode
     */
    cr = CM_Get_Device_ID_Size( &dwLength, dnDevnode, 0 );
    if( cr == CR_SUCCESS )
    {
        pszDeviceId = Malloc( dwLength+1, 0 );
        if( pszDeviceId )
        {
            cr = CM_Get_Device_ID( dnDevnode, pszDeviceId, dwLength+1, 0 );
            if( cr != CR_SUCCESS )
            {
                FPF( "CM_Get_Device_ID failed for devnode %08x with error %d",
                    dnDevnode, cr );
                Free( pszDeviceId );
                cr = CR_FAILURE;
            }
            else
            {
                IPF( "dnDevnode %08x has device id %s", dnDevnode, pszDeviceId );
            }
        }
        else
        {
            MPF( "copying device id for devnode %08x", dnDevnode );
            cr = CR_OUT_OF_MEMORY;
        }
    }
    else
    {
        FPF( "CM_Get_Device_ID_Size failed for devnode %08x with error %d",
            dnDevnode, cr );
        cr = CR_FAILURE;
        pszDeviceId = NULL;
    }
            
    /*
     * copy devnode io port info, report abnormality under debug
     */

    /*
     * each i/o port allocation is added as though it were a separate devnode
     * this is a hack to maintain backwards compatibility with multiport
     * gamecards.  Sadly it means that a mini-driver owns a whole card rather
     * than a port so a dual gameport card cannot have a different driver for 
     * each port.
     * Note, this means that the DeviceId is copied twice if this is a dual
     * port gamecard.
     * Because of the danger of polling a SCSI card, only let the first port
     * be a non-standard one.  In particular though, this allows us to accept 
     * a devnode from a device which cannot split its resources which is in
     * fact pretty much all the PnP devices we know of.
     */
    IPF( "ccb.wNumIOPorts = %x", ccb.wNumIOPorts );

    for( Idx = 0; Idx<ccb.wNumIOPorts; Idx++ )
    {
        IPF( "IO range @%04x, %02x", ccb.wIOPortBase[Idx], ccb.wIOPortLength[Idx] );
    
        if( ( ccb.wIOPortBase[0] < MIN_JOY_PORT ) 
          ||( ccb.wIOPortBase[0] + ccb.wIOPortLength[0] > MAX_JOY_PORT ) )
        {
            if( Idx == 0 )
            {
                WPF( "Unusual gameport %4x", ccb.wIOPortBase[Idx] );
            }
            else
            {
                /*
                 *  Never heard of a multiport PnP gamecard so ignore extra 
                 *  i/o resources.
                 */
                WPF( "Only using first IO range due to non-standard IO" );
                break;
            }
        }

        /*
         *  Take IO port validity on trust from VJoyD.
         *  VJoyD orders the unallocated gameports it finds at allocation time 
         *  by IO address so we should keep this list ordered
         *
         *
         *  See if this devnode is a replacement for one we had before
         *  if so, update that devnode,
         *  if not, add on the end of the current devnode list
         */

        if( pszDeviceId )
        {

            /*
             *   See if this is a replacement devnode for one which has been stopped.  
             *   If so we update the previous devnode
             *   Note if this one already has the new devnode, this is the first half
             *   of a dual port gamecard, so carry on looking for the second.
             *   Note, it is possible that the number of ports could have changed.
             */
            for( DNIdx = 0; DNIdx < NumDevnodes; DNIdx++ )
            {
                if( ( pDevnodes[DNIdx].pszDeviceId )
                  &&( pDevnodes[DNIdx].dn != dnDevnode ) 
                  &&( !strcmp( pszDeviceId, pDevnodes[DNIdx].pszDeviceId ) ) )
                {
                    /*
                     *   Update the current devnode link to replacement
                     */
                    IPF( "dnDevnode %08x is a replacement for device ID = %s", dnDevnode, pszDeviceId );
                    pDevnodes[DNIdx].dn = dnDevnode;
                    break;
                }
            }
        }
        else
        {
            /*
             *  This is a catastrophic failure but we have to handle it somehow.
             */
            EPF( "Failed to get DeviceId for devnode %08x", dnDevnode );
            DNIdx = NumDevnodes;
            Free( pszDeviceId );
            pszDeviceId = NULL;

            /*
             *  Since we cannot do any further recovery, call it a success
             */
            cr = CR_SUCCESS;
        }

        if( DNIdx < NumDevnodes )
        {
            Free( pszDeviceId );
        }
        else
        {
            /*
             *  New devnode (or additional port on existing one)
             */
            if( NumDevnodes == MaxDevnodes )
            {
                PDEVNODEDESC pTemp;
                pTemp = ReAlloc( pDevnodes, ++MaxDevnodes * sizeof( *pDevnodes ), HEAPSWAP );
                if( pTemp )
                {
                    pDevnodes = pTemp;
                }
                else
                {
                    MPF( "ReAllocating devnode array" );
                    MaxDevnodes--;
                    return( CR_OUT_OF_MEMORY );
                }
            }

            pDevnodes[NumDevnodes].id = UNUSED;
            pDevnodes[NumDevnodes].dwPollMask = 0;
            pDevnodes[NumDevnodes].pszDeviceId = pszDeviceId;

            DNIdx = NumDevnodes;
            NumDevnodes++;
        }

        pDevnodes[DNIdx].dn = dnDevnode;
        pDevnodes[DNIdx].io = ccb.wIOPortBase[Idx];
    }

    /*
     *  Now that we have at least one gameport, setup the polling core
     */
    if( !fpPoll )
    {
        /*
         *  Mask out anything except the interrupt flag
         */
        dwPollType = dwPollFlags & PF_INTMASK;
        if( dwTSCScale )
        {
            InitTSC( ccb.wIOPortBase[0] );
            NewTimeout = CalcTSCTickTimeOut;
            fpPoll = ( dwPollFlags ) ? ETSCPoll : DTSCPoll;
            dwPollType |= TSC_POLL;
        }
        else if( InitRTC( ccb.wIOPortBase[0] ) )
        {
            NewTimeout = CalcRTCTickTimeOut;
            fpPoll = ( dwPollFlags ) ? ERTCPoll : DRTCPoll;
            dwPollType |= RTC_POLL;
        }
        else
        {
            InitSW();
            NewTimeout = CalcSWTickTimeOut;
            fpPoll = ( dwPollFlags ) ? ESWPoll : DSWPoll;
            dwPollType |= SW_POLL;
        }
    }

    return( cr );
} /* StartDevnode */

/****************************************************************************
 *
 *  Function: EliminateDevnode
 *      Remove a devnode from the array, keeping the active values contiguous.
 *
 ***************************************************************************/
void EliminateDevnode( int Idx )
{
    if( pDevnodes[Idx].pszDeviceId )
    {
        IPF( "Freeing DeviceId %s for devnode %08x", pDevnodes[Idx].pszDeviceId, pDevnodes[Idx].dn );
        Free( pDevnodes[Idx].pszDeviceId );
    }
    else
    {
        WPF( "DeviceId missing for devnode %08x", pDevnodes[Idx].dn );
    }

    if( Idx+1 != NumDevnodes )
    {
        _lmemcpy( &pDevnodes[Idx], &pDevnodes[Idx+1], 
            sizeof( *pDevnodes ) * ( NumDevnodes - (Idx+1) ) );
    }
    NumDevnodes--;
} /* EliminateDevnode */

/****************************************************************************
 *
 *  Function: StopDevnode
 *      Mark a devnode's resources as unusable.
 *      If no ids are using this devnode then forget about it, otherwise,
 *      make sure any future polls are prevented but keep the information
 *      around in case the device id is reactivated.  This is basically so 
 *      that when power management stops a devnode for a suspend, it can be 
 *      matched when a new devnode is started on resume and the new devnode's
 *      resources can be used.
 *
 ***************************************************************************/
void INLINE_RETAIL StopDevnode( DEVNODE Devnode )
{
    int         Idx;

    /*
     *  Find all ports associated with the devnode and cancel the port use
     */
    for( Idx = NumDevnodes-1; Idx >= 0; Idx-- )
    {
        if( pDevnodes[Idx].dn == Devnode )
        {
            if( pDevnodes[Idx].id != UNUSED )
            {
                WPF( "Devnode %08x killed with id %d still active on port %04x", Devnode, pDevnodes[Idx].id, pDevnodes[Idx].io );
                /*
                 *  Keep devnode data so that we can match a devnode with 
                 *  the same DeviceId string.
                 */
                
                /*
                 *  Set io to zero to stop polling
                 */
                pDevnodes[Idx].io = 0;
                
                /*
                 *  Set devnode to zero to make a start get recognized for 
                 *  the DeviceId
                 */
                pDevnodes[Idx].dn = 0;
            }
            else
            {
                EliminateDevnode( Idx );
            }
        }
    }
} /* StopDevnode */

/****************************************************************************
 *
 *  CfgRoutine: This procedure handles all the mmdevldr CfgMgr messages
 *                  Note, these have been cached and filtered by VJoyD
 *
 *  ENTRY:  Same entry parameters as a standard config handler
 *
 *  EXIT:   configmg return code.
 *
 ***************************************************************************/
CONFIGRET _cdecl CfgRoutine( CONFIGFUNC cfFuncName, SUBCONFIGFUNC scfSubFuncName,
                      DEVNODE dnToDevnode, ULONG dwRefData, ULONG ulFlags)
{
    CONFIGRET cr = CR_DEFAULT;         /* config man return code */

    UNREFERENCED_PARAMETER( ulFlags );
    UNREFERENCED_PARAMETER( dwRefData );

    switch( cfFuncName ) 
    {
    case CONFIG_FILTER:
        IPF("CONFIG_FILTER");
        /*
         *  Don't use
         */
        break;

    case CONFIG_START:
        /*
         *  Add START to list
         */
        switch( scfSubFuncName ) 
        {
        case CONFIG_START_FIRST_START:
            IPF( "CONFIG_START_FIRST_START for devnode %8x", dnToDevnode );
            break;
        case CONFIG_START_DYNAMIC_START:
            IPF( "CONFIG_START_DYNAMIC_START for devnode %8x", dnToDevnode );
            break;
        case CONFIG_START_SHUTDOWN_START:
            IPF( "CONFIG_START_SHUTDOWN_START for devnode %8x", dnToDevnode );
            break;
        default:
            IPF( "CONFIG_START with unknown sub-function: %x", (DWORD)scfSubFuncName );
        }

        cr = StartDevnode( dnToDevnode );
        break;
                

    case CONFIG_STOP:
    case CONFIG_REMOVE:
    case CONFIG_SHUTDOWN:
#if defined DEBUG || defined DEBUG_RETAIL
        switch( cfFuncName )
        {
        case CONFIG_STOP:
            IPF("CONFIG_STOP");
            break;

        case CONFIG_REMOVE:
            IPF("CONFIG_REMOVE");
            break;

        case CONFIG_SHUTDOWN:
            IPF("CONFIG_SHUTDOWN");
            break;

        }
#endif
        StopDevnode( dnToDevnode );
        return( CR_SUCCESS );

    case CONFIG_PREREMOVE:
        /*
         *  Don't use
         */
        IPF("CONFIG_PREREMOVE");
        break;

    case CONFIG_PREREMOVE2:
        /*
         *  Don't use
         */
        IPF("CONFIG_PREREMOVE2");
        break;

    case CONFIG_PRESHUTDOWN:
        /*
         *  Don't use
         */
        IPF("CONFIG_PRESHUTDOWN");
        break;

    case CONFIG_TEST:
        /*
         *  Don't use
         */
        IPF("CONFIG_TEST");
        break;

    case CONFIG_TEST_FAILED:
        /*
         *  Don't use
         */
        IPF("CONFIG_TEST_FAILED");
        break;

    case CONFIG_TEST_SUCCEEDED:
        /*
         *  Don't use
         */
        IPF("CONFIG_TEST_SUCCEEDED");
        break;

    case CONFIG_ENUMERATE:
        /*
         *  Don't use
         */
        IPF("CONFIG_ENUMERATE");
        break;

    case CONFIG_SETUP:
        /*
         *  Don't use
         */
        IPF("CONFIG_SETUP");
        break;

    case CONFIG_READY:
        /*
         *  Don't use
         */
        IPF("CONFIG_READY");
        break;

    case CONFIG_CALLBACK:
        /*
         *  Don't use
         */
        IPF("CONFIG_CALLBACK");
        break;

    case CONFIG_PRIVATE:
        /*
         *  Don't use
         */
        IPF("CONFIG_PRIVATE");
        break;

    case CONFIG_VERIFY_DEVICE:
        /*
         *  Don't use
         */
        IPF("CONFIG_VERIFY_DEVICE");
        break;

    case CONFIG_PROP_CHANGE:
        /*
         *  Don't use
         */
        IPF("CONFIG_PROP_CHANGE");
        break;

    case CONFIG_APM:
        /*
         *  Don't use
         */
        IPF("CONFIG_APM");
        break;

    case CONFIG_BEGIN_PNP_MODE:
        /*
         *  Don't use
         */
        IPF("CONFIG_BEGIN_PNP_MODE");
        break;

    default:
        IPF("unhandled cfFuncName: %x", cfFuncName );
    }

    return CR_DEFAULT;

} /* CfgRoutine */

/****************************************************************************
 *
 *  Function: Initialize
 *      Maps or unmaps joystick ids to resources.
 *      For VJIF_BEGIN_ACCESS, try to map the assigned joystick type and id 
 *      onto the gameport resources available.  If this is OK, set up 
 *      internal structures to support axis mapping and intial values.
 *      For VJIF_END_ACCESS mark the resources as unused.
 *
 ***************************************************************************/
HRESULT __stdcall Initialize( DWORD dwId, LPDID_INITPARAMS lpInitParams )
{
    int Idx;
    DWORD dwFlags;
    dwFlags = lpInitParams->hws.dwFlags;

    IPF( "DevId: %d for Btns %d Flgs %x on dn %08x", dwId, lpInitParams->hws.dwNumButtons, dwFlags, lpInitParams->dwDevnode );

    if( lpInitParams->dwFlags & VJIF_BEGIN_ACCESS )  
    {
        for( Idx = 0; Idx<NumDevnodes; Idx++ )
        {
            /*
             *  DigiJoy can use any number of ports per devnode so make sure 
             *  we find the right devnode, or we'll get out of sync.
             */
            if( pDevnodes[Idx].dn == lpInitParams->dwDevnode )
            {
                if( pDevnodes[Idx].id == UNUSED )
                {
                    pDevnodes[Idx].id = dwId;
                    IPF( "Id %d set for port %x", pDevnodes[Idx].id, pDevnodes[Idx].io );

                    /*
                     *  Initialize the axes last values to the default center
                     */
                    pDevnodes[Idx].rgdwLastValues[0] = 328;
                    pDevnodes[Idx].rgdwLastValues[1] = 328;
                    pDevnodes[Idx].rgdwLastValues[2] = 328;
                    pDevnodes[Idx].rgdwLastValues[3] = 328;
                    pDevnodes[Idx].rgdwLastValues[4] = 328;
                    pDevnodes[Idx].rgdwLastValues[5] = 328;
                    pDevnodes[Idx].rgdwLastValues[6] = 328;
                    pDevnodes[Idx].rgdwLastValues[7] = 328;
                    pDevnodes[Idx].dwGlitches = 0;

                    GetAxisInfo( pDevnodes[Idx].id, &pDevnodes[Idx].dwPollMask );
                    GetComboInfo( &pDevnodes[Idx] );

                    break;
                }
            }
        }
        if( Idx >= NumDevnodes )
        {
            IPF( "no free full ports, need more" );
            return( VJERR_NEED_DEVNODE );
        }
    }
    else if( lpInitParams->dwFlags & VJIF_END_ACCESS )
    {
        /*
         *  It does no harm to leave the dwPollMasks set to stale values
         *  as it is the ids that are used for availability testing
         */
        
        for( Idx = 0; Idx<NumDevnodes; Idx++ )
        {
            if( pDevnodes[Idx].id == dwId )
            {
                pDevnodes[Idx].id = UNUSED;
                IPF( "Freed port used by id %d", dwId );
                if( pDevnodes[Idx].io == 0 )
                {
                    /*
                     *  This dead devnode is now unused so eliminate it
                     */
                    EliminateDevnode( Idx );
                }
                break;
            }
        }
        if( Idx == NumDevnodes )
        {
            WPF( "Can't unuse id that's not in use" );
            return( VJERR_INVALIDPARAM );
        }
    }
    else
    {
        EPF( "Failing unknown flags %08x on Initialize", lpInitParams->dwFlags );
        return( VJERR_INVALIDPARAM );
    }
    return( VJ_OK );

} /* Initialize */

/****************************************************************************
 *
 *  Function: RegisterLoad
 *      Register callbacks and other attributes with VJoyD.
 *
 ***************************************************************************/
HRESULT __stdcall RegisterLoad( void )
{
    VJREGDRVINFO        Reg;
    VJPOLLREG           PollReg;
    HRESULT             rc = VJERR_FAIL;

    Reg.dwSize = sizeof( Reg );
    Reg.dwFlags = VJDF_ISANALOGPORTDRIVER;
    Reg.dwFunction = VJRT_LOADED;
    Reg.lpszOEMCallout = "DIGIJOY.VXD";
    Reg.dwFirmwareRevision = 0;
    Reg.dwHardwareRevision = 0;
    Reg.dwDriverVersion = 0x00010000;

    /*
     *  Unfortunately the VJoyD/mini-driver architecture is backwards so we 
     *  don't know what we're supposed to be driving yet so we can't register 
     *  a device description.  By the time we can see what we're driving 
     *  there's no way to tell VJoyD, so register nothing and VJoyD and 
     *  DInput will try to work out what is there through a combination of 
     *  the registry settings and test polls.
     */
    Reg.lpDeviceDesc = NULL;
    Reg.lpPollReg = &PollReg;
    Reg.lpForceReg = NULL;
    Reg.dwReserved = NULL;

    PollReg.dwSize = sizeof( PollReg );
    PollReg.fpCfg = &CfgRoutine;
    PollReg.fpEscape = NULL;
    PollReg.fpCtrlMsg = &CtrlMsg;
    PollReg.fpInitialize = &Initialize;
    PollReg.fpPoll = &PollRoutine;

    rc = VJoyDRegister( (DWORD)(&Reg) );

    return( 0 );
} /* RegisterLoad */

/****************************************************************************
 *
 *  Function: ExitRoutine
 *      The VxD is being unloaded so free up all resources.
 *
 ***************************************************************************/
void __cdecl ExitRoutine( void )
{
    /*
     *  Since the only time we have nothing to free is when INIT fails, 
     *  there should always be something to free here
     */
    if( !pDevnodes )
    {
        while( NumDevnodes )
        {
            NumDevnodes--;
            if( pDevnodes[NumDevnodes].pszDeviceId )
            {
                WPF( "Freeing DeviceId %s at exit", pDevnodes[NumDevnodes].pszDeviceId );
                Free( pDevnodes[NumDevnodes].pszDeviceId );
            }
        }

        FPF( "pDevnodes NULL on DeviceExit" );
        Free( pDevnodes );
    }
    IPF( "Exit" );
    return;
} /* ExitRoutine */

#pragma VxD_IDATA_SEG
#pragma VxD_ICODE_SEG

/****************************************************************************
 *
 *  Function: InitRoutine
 *      The VxD has been loaded so do once only initialzation.
 *
 ***************************************************************************/
DWORD __cdecl InitRoutine( void )
{
    pDevnodes = Malloc( sizeof( *pDevnodes ), HEAPSWAP );
    if( pDevnodes )
    {
        MaxDevnodes = 1;
        NumDevnodes = 0;

        IsTSCPresent();
        fpPoll = NULL;
        dwPollType = (DWORD)(-1);
        dwPollFlags = 0;
        NewTimeout = NULL;

        return( RegisterLoad() );
    }
    else
    {
        MPF( "in Init for first devnode description" );
        return( 1 );
    }
} /* InitRoutine */
