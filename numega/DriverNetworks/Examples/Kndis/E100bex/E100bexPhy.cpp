// E100bexPhy.cpp: implementation of the E100bexPhy class.
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2000 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//
// Portions copied from Microsoft Windows 2000 DDK sample driver containing the
// following copyright
//
/****************************************************************************
** COPYRIGHT (C) 1994-1997 INTEL CORPORATION                               **
** DEVELOPED FOR MICROSOFT BY INTEL CORP., HILLSBORO, OREGON               **
** HTTP://WWW.INTEL.COM/                                                   **
** THIS FILE IS PART OF THE INTEL ETHEREXPRESS PRO/100B(TM) AND            **
** ETHEREXPRESS PRO/100+(TM) NDIS 5.0 MINIPORT SAMPLE DRIVER               **
****************************************************************************/


#include <kndis.h>
#include "E100bexInc.h"


////////////////////////////////////////////////////////////////////
// E100bexPhy::Detect
//
// This routine will detect what phy we are using, set the line
//              speed, FDX or HDX, and configure the phy if necessary.
//
//		        The following combinations are supported:
//              - TX or T4 PHY alone at PHY address 1
//              - T4 or TX PHY at address 1 and MII PHY at address 0
//              - 82503 alone (10Base-T mode, no full duplex support)
//              - 82503 and MII PHY (TX or T4) at address 0
//
//              The sequence / priority of detection is as follows:
//                  If there is a PHY Address override use that address.
//                  else scan based on the 'Connector' setting.
//                      Switch Connector
//                          0 = AutoScan
//                          1 = Onboard TPE only
//                          2 = MII connector only
//
//              Each of the above cases is explained below.
//
//              AutoScan means:
//                Look for link on addresses 1, 0, 2..31 (in that order).  Use the first
//                address found that has link.
//                If link is not found then use the first valid PHY found in the same scan
//                order 1,0,2..31.  NOTE: this means that NO LINK or Multi-link cases will
//                default to the onboard PHY (address 1).
//
//              Onboard TPE only:
//                Phy address is set to 1 (No Scanning).
//
//              MII connector only means:
//                Look for link on addresses 0, 2..31 (again in that order, Note address 1 is
//                NOT scanned).   Use the first address found that has link.
//                If link is not found then use the first valid Phy found in the same scan
//                order 0, 2..31.
//                In the AutoScan case above we should always find a valid PHY at address 1,
//                there is no such guarantee here, so, If NO Phy is found then the driver
//                should default to address 0 and continue to load.  Note: External
//                transceivers should be at address 0 but our early Nitro3 testing found
//                transceivers at several non-zero addresses (6,10,14).
//
//   NWAY
//              Additionally auto-negotiation capable (NWAY) and parallel
//              detection PHYs are supported. The flow-chart is described in
//              the 82557 software writer's manual.
//
//   NOTE:  1.  All PHY MDI registers are read in polled mode.
//          2.  The routines assume that the 82557 has been RESET and we have
//              obtained the virtual memory address of the CSR.
//          3.  PhyDetect will not RESET the PHY.
//          4.  If FORCEFDX is set, SPEED should also be set. The driver will
//              check the values for inconsistency with the detected PHY
//              technology.
//          5.  PHY 1 (the PHY on the adapter) MUST be at address 1.
//          6.  Driver ignores FORCEFDX and SPEED overrides if a 503 interface
//              is detected.
//
// Returns:
//      TRUE - If a Phy was detected, and configured correctly.
//      FALSE - If a valid phy could not be detected and configured.
//
BOOLEAN E100bexPhy::Detect(void)
{
    TRACE("E100bexPhy::Detect entered\n");

	USHORT MdiControlReg, MdiStatusReg;
    UCHAR FoundPhyAt = 0xff;

    // Check for a phy address over-ride of 32 which indicates a 503
    if (32 == m_PhyAddress)
    {
        // 503 interface over-ride
        TRACE("   503 serial component over-ride\n");

        m_PhyAddress = 32;

        // Record the current speed and duplex.  We will be in half duplex
        // mode unless the user used the force full duplex over-ride.
        m_AiLineSpeedCur = 10;
        m_AiDuplexCur = (USHORT) m_AiForceDpx;
        if (!m_AiDuplexCur)
		{
            m_AiDuplexCur = 1;
		}

        return TRUE;
    }

    // Check for other phy address over-rides.
    //   If the Phy Address is between 0-31 then there is an over-ride.
    //   Or the connector was set to 1
    if ( (m_PhyAddress < 32) || (m_Connector == CONNECTOR_TPE) )
    {   // User Override nothing to do but setup Phy and leave

        if ( (m_PhyAddress > 32) && (m_Connector == CONNECTOR_TPE) )
        {
            m_PhyAddress = 1;  // Connector was forced

            // Isolate all other PHYs and unisolate this one
            Select( m_PhyAddress, FALSE);
        }
        TRACE("   Phy address Override to address %d\n", m_PhyAddress);
#if DBG
        // Read the MDI control register at override address.
        MdiControlReg = MdiRead( MDI_CONTROL_REG, m_PhyAddress);
        // Read the status register at override address.
        MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);
        // Read the status register again because of sticky bits
        MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);
        // check if we found a valid phy
        if (!((MdiControlReg == 0xffff) || ((MdiStatusReg == 0) && (MdiControlReg == 0))))
        {
            // we have a valid phy1
            TRACE("   Over-ride address %d has a valid Phy.\n", m_PhyAddress);
            // Read the status register again
            MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);
            // If there is a valid link then use this Phy.
            if (MdiStatusReg & MDI_SR_LINK_STATUS)
            {
                TRACE("   Phy at address %d has link\n", m_PhyAddress);
            }
        }
        else
        {
            // no PHY at over-ride address
            TRACE("   Over-ride address %d has no Phy!!!!\n", m_PhyAddress);
        }
#endif
        return Setup();
    }
    else // Need to scan - No address over-ride and Connector is AUTO or MII
    {
        for (char LookForLink=1; LookForLink>=0 ;LookForLink-- )
        {
            // For each PhyAddress 0 - 31
            for (UCHAR i = 0; i < 32; i++)
            {
                if (LookForLink)
                {
                    // Phy Addresses must be tested in the order 1,0,2..31.
                    switch (i)
                    {
                    case 0:
                        m_PhyAddress = 1;
                        break;
                    case 1:
                        m_PhyAddress = 0;
                        break;
                    default:
                        m_PhyAddress = i;
                        break;
                    }

                    // Skip OnBoard for MII only case
                    if ( (m_PhyAddress == 1) && (m_Connector == CONNECTOR_MII) ) continue;

                    TRACE("   Scanning Phy address %d for link\n", m_PhyAddress);

                    // Read the MDI control register
                    MdiControlReg = MdiRead( MDI_CONTROL_REG, m_PhyAddress);
                    // Read the status register
                    MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);
                    MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);   // Sticky Bits
                }
                else
                {   // Not looking for link
                    if (FoundPhyAt < 32)
					{
                        m_PhyAddress = FoundPhyAt;
					}
                    else
					{
						// No valid PHYs were found last time so just default
                        if (m_Connector == CONNECTOR_MII)
						{
                            m_PhyAddress = 0;  // Default for MII
						}
                        else // assume a 503 interface
                        {
                            m_PhyAddress = 32;

                            // Record the current speed and duplex.  We will be in half duplex
                            // mode unless the user used the force full duplex over-ride.
                            m_AiLineSpeedCur = 10;
                            m_AiDuplexCur = (USHORT) m_AiForceDpx;
                            if (!m_AiDuplexCur)
							{
                                m_AiDuplexCur = 1;
							}

                            return TRUE;
                        }
                        TRACE("   No Links Found!!\n");
					}
                }

                // check if we found a valid phy or on !LookForLink pass
                if ( !( (MdiControlReg == 0xffff) || ((MdiStatusReg == 0) && (MdiControlReg == 0))
                    ) || (!LookForLink)
                    )
                {   // Valid phy or Not looking for Link

#if DBG
                    if (!( (MdiControlReg == 0xffff) || ((MdiStatusReg == 0) && (MdiControlReg == 0)) ) )
                    {
                        TRACE("      Found a Phy at address %d\n", m_PhyAddress);
                    }
#endif
                    // Store highest priority phy found for NO link case
                    if (i < FoundPhyAt && FoundPhyAt != 1)
                    {
                        // this phy is higher priority
                        FoundPhyAt = (UCHAR) m_PhyAddress;
                    }

                    // Select Phy before checking link status
                    // NOTE: may take up to 3.5 Sec if LookForLink == TRUE
                    Select( m_PhyAddress, (BOOLEAN)LookForLink);
                    // Read the MDI control register
                    MdiControlReg = MdiRead( MDI_CONTROL_REG, m_PhyAddress);
                    // Read the status register
                    MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);
                    MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);
                    // If there is a valid link or we alreadry tried once then use this Phy.
                    if ((MdiStatusReg & MDI_SR_LINK_STATUS) || (!LookForLink))
                    {
#if DBG
                        if (MdiStatusReg & MDI_SR_LINK_STATUS)
                        {
                            TRACE("   Using Phy at address %d with link\n", m_PhyAddress);
                        }
                        else
                        {
                            TRACE("   Using Phy at address %d WITHOUT link!!!\n", m_PhyAddress);
                        }
#endif
                        return Setup();    // Exit with Link Path
                    }
                } // End if valid PHY
            } // End for each PHY address
        } // End LookForLink
    } // End else scan

    return FALSE;
}

////////////////////////////////////////////////////////////////////
// E100bexPhy::Setup
//
// This routine will setup phy 1 or phy 0 so that it is configured
//              to match a speed and duplex over-ride option.  If speed or
//              duplex mode is not explicitly specified in the registry, the
//              driver will skip the speed and duplex over-ride code, and
//              assume the adapter is automatically setting the line speed, and
//              the duplex mode.  At the end of this routine, any truly Phy
//              specific code will be executed (each Phy has its own quirks,
//              and some require that certain special bits are set).
//
//   NOTE:  The driver assumes that SPEED and FORCEFDX are specified at the
//          same time. If FORCEDPX is set without speed being set, the driver
//          will encouter a fatal error.
//
// Parameters:
//		none
// IRQL: 
//		
// Return Mode:
//		Synchronous
// Returns:
//      TRUE - If the phy could be configured correctly
//      FALSE - If the phy couldn't be configured correctly, because an
//              unsupported over-ride option was used
//      	
BOOLEAN E100bexPhy::Setup(void)
{
    USHORT      MdiControlReg, MdiStatusReg, MdiIdLowReg, MdiIdHighReg;
    USHORT      MdiMiscReg;
    BOOLEAN     ForcePhySetting = FALSE;

    TRACE("E100bexPhy::Setup\n");

    // If we are NOT forcing a setting for line speed or full duplex, then
    // we won't force a link setting, and we'll jump down to the phy
    // specific code.
    if (m_AiTempSpeed || m_AiForceDpx)
    {
        // Find out what kind of technology this Phy is capable of.
        MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);
        // Read the MDI control register at our phy
        MdiControlReg = MdiRead( MDI_CONTROL_REG, m_PhyAddress);

        // Now check the validity of our forced option.  If the force option is
        // valid, then force the setting.  If the force option is not valid,
        // we'll set a flag indicating that we should error out.

        // If speed is forced to 10mb
        if ( m_AiTempSpeed == 10)
        {
            // If half duplex is forced
            if (m_AiForceDpx == 1)
            {
                if (MdiStatusReg & MDI_SR_10T_HALF_DPX)
                {
                    TRACE("Forcing 10mb 1/2 duplex\n");
                    MdiControlReg &= ~(MDI_CR_10_100 | MDI_CR_AUTO_SELECT | MDI_CR_FULL_HALF);
                    ForcePhySetting = TRUE;
                }
            }

            // If full duplex is forced
            else if (m_AiForceDpx == 2)
            {
                if (MdiStatusReg & MDI_SR_10T_FULL_DPX)
                {
                    TRACE("Forcing 10mb full duplex\n");
                    MdiControlReg &= ~(MDI_CR_10_100 | MDI_CR_AUTO_SELECT);
                    MdiControlReg |= MDI_CR_FULL_HALF;
                    ForcePhySetting = TRUE;
                }
            }

            // If auto duplex (we actually set phy to 1/2)
            else
            {
                if (MdiStatusReg & (MDI_SR_10T_FULL_DPX | MDI_SR_10T_HALF_DPX))
                {
                    TRACE("Forcing 10mb auto duplex\n");
                    MdiControlReg &= ~(MDI_CR_10_100 | MDI_CR_AUTO_SELECT | MDI_CR_FULL_HALF);
                    ForcePhySetting = TRUE;
                    m_AiForceDpx = 1;
                }
            }
        }

        // If speed is forced to 100mb
        else if (m_AiTempSpeed == 100)
        {
            // If half duplex is forced
            if (m_AiForceDpx == 1)
            {
                if (MdiStatusReg & (MDI_SR_TX_HALF_DPX | MDI_SR_T4_CAPABLE))
                {
                    TRACE("Forcing 100mb half duplex\n");
                    MdiControlReg &= ~(MDI_CR_AUTO_SELECT | MDI_CR_FULL_HALF);
                    MdiControlReg |= MDI_CR_10_100;
                    ForcePhySetting = TRUE;
                }
            }

            // If full duplex is forced
            else if (m_AiForceDpx == 2)
            {
                if (MdiStatusReg & MDI_SR_TX_FULL_DPX)
                {
                    TRACE("Forcing 100mb full duplex\n");
                    MdiControlReg &= ~MDI_CR_AUTO_SELECT;
                    MdiControlReg |= (MDI_CR_10_100 | MDI_CR_FULL_HALF);
                    ForcePhySetting = TRUE;
                }
            }

            // If auto duplex (we set phy to 1/2)
            else
            {
                if (MdiStatusReg & (MDI_SR_TX_HALF_DPX | MDI_SR_T4_CAPABLE))
                {
                    TRACE("Forcing 100mb auto duplex\n");
                    MdiControlReg &= ~(MDI_CR_AUTO_SELECT | MDI_CR_FULL_HALF);
                    MdiControlReg |= MDI_CR_10_100;
                    ForcePhySetting = TRUE;
                    m_AiForceDpx = 1;
                }
            }
        }

        if (ForcePhySetting == FALSE)
        {
            TRACE("Can't force speed=%d, duplex=%d\n",m_AiTempSpeed, m_AiForceDpx);

            return FALSE;
        }

        // Write the MDI control register with our new Phy configuration
        MdiWrite( MDI_CONTROL_REG, m_PhyAddress, MdiControlReg);
        // wait 100 milliseconds for auto-negotiation to complete
        StallExecution(100);
    }

    // Find out specifically what Phy this is.  We do this because for certain
    // phys there are specific bits that must be set so that the phy and the
    // 82557 work together properly.

    MdiIdLowReg = MdiRead( PHY_ID_REG_1, m_PhyAddress);
    MdiIdHighReg = MdiRead( PHY_ID_REG_2, m_PhyAddress);

    UINT PhyId = ((UINT) MdiIdLowReg | ((UINT) MdiIdHighReg << 16));
    m_PhyId = PhyId;

    TRACE("Phy ID is %x\n", PhyId);

    // And out the revsion field of the Phy ID so that we'll be able to detect
    // future revs of the same Phy.
    PhyId &= PHY_MODEL_REV_ID_MASK;

    // Handle the National TX
    if (PhyId == PHY_NSC_TX)
    {
        TRACE("Found a NSC TX Phy\n");

        MdiMiscReg = MdiRead( NSC_CONG_CONTROL_REG, m_PhyAddress);

        MdiMiscReg |= (NSC_TX_CONG_TXREADY | NSC_TX_CONG_F_CONNECT);

        // If we are configured to do congestion control, then enable the
        // congestion control bit in the National Phy
        if (m_Congest)
		{
            MdiMiscReg |= NSC_TX_CONG_ENABLE;
		}
        else
		{
            MdiMiscReg &= ~NSC_TX_CONG_ENABLE;
		}

        MdiWrite( NSC_CONG_CONTROL_REG, m_PhyAddress, MdiMiscReg);
    }

    FindSpeedAndDpx(PhyId);

    TRACE("Current speed=%d, Current Duplex=%d\n",m_AiLineSpeedCur, m_AiDuplexCur);

    return TRUE;
}

////////////////////////////////////////////////////////////////////
// E100bexPhy::FindSpeedAndDpx
//
// This routine will figure out what line speed and duplex mode
//              the PHY is currently using.
//
// Parameters:
//      PhyId - The ID of the PHY in question.
// IRQL: 
//		
// Return Mode:
//		Synchronous
// Returns:
//      TRUE - If the phy could be configured correctly
//      FALSE - If the phy couldn't be configured correctly, because an
//              unsupported over-ride option was used
//      	
VOID E100bexPhy::FindSpeedAndDpx(UINT PhyId)
{
    USHORT MdiMiscReg, MdiOwnAdReg;

    TRACE("E100bexPhy::FindSpeedAndDpx\n");

    // If there was a speed and/or duplex override, then set our current
    // value accordingly
    m_AiLineSpeedCur = m_AiTempSpeed;
    m_AiDuplexCur = (USHORT)  m_AiForceDpx;

    // If speed and duplex were forced, then we know our current settings, so
    // we'll just return.  Otherwise, we'll need to figure out what NWAY set
    // us to.
    if (m_AiLineSpeedCur && m_AiDuplexCur)
	{
        return;
	}

    // If we didn't have a valid link, then we'll assume that our current
    // speed is 10mb half-duplex.

    // Read the status register twice because of sticky bits
    USHORT MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);
    MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);

    // If there wasn't a valid link then use default speed & duplex
    if (!(MdiStatusReg & MDI_SR_LINK_STATUS))
    {
        TRACE("Link Not found for speed detection!!!  Using defaults.\n");

        m_AiLineSpeedCur = 10;
        m_AiDuplexCur = 1;

        return;
    }

    // If this is an Intel PHY (a T4 PHY_100 or a TX PHY_TX), then read bits
    // 1 and 0 of extended register 0, to get the current speed and duplex
    // settings.
    if ((PhyId == PHY_100_A) || (PhyId == PHY_100_C) || (PhyId == PHY_TX_ID))
    {
        TRACE("Detecting Speed/Dpx for an Intel PHY\n");

        // Read extended register 0
        MdiMiscReg = MdiRead( EXTENDED_REG_0, m_PhyAddress);
        // Get current speed setting
        if (MdiMiscReg & PHY_100_ER0_SPEED_INDIC)
		{
            m_AiLineSpeedCur = 100;
		}
        else
		{
            m_AiLineSpeedCur = 10;
		}

        // Get current duplex setting -- if bit is set then FDX is enabled
        if (MdiMiscReg & PHY_100_ER0_FDX_INDIC)
		{
            m_AiDuplexCur = 2;
		}
        else
		{
            m_AiDuplexCur = 1;
		}

        return;
    }

    // Read our link partner's advertisement register
    USHORT MdiLinkPartnerAdReg = MdiRead( AUTO_NEG_LINK_PARTNER_REG, m_PhyAddress);
    // See if Auto-Negotiation was complete (bit 5, reg 1)
    MdiStatusReg = MdiRead( MDI_STATUS_REG, m_PhyAddress);

    // If a True NWAY connection was made, then we can detect speed/duplex by
    // ANDing our adapter's advertised abilities with our link partner's
    // advertised ablilities, and then assuming that the highest common
    // denominator was chosed by NWAY.
    if ((MdiLinkPartnerAdReg & NWAY_LP_ABILITY) &&
        (MdiStatusReg & MDI_SR_AUTO_NEG_COMPLETE))
    {
        TRACE("Detecting Speed/Dpx from NWAY connection\n");

        // Read our advertisement register
        MdiOwnAdReg = MdiRead( AUTO_NEG_ADVERTISE_REG, m_PhyAddress);

        // AND the two advertisement registers together, and get rid of any
        // extraneous bits.
        MdiOwnAdReg &= (MdiLinkPartnerAdReg & NWAY_LP_ABILITY);

        // Get speed setting
        if (MdiOwnAdReg & (NWAY_AD_TX_HALF_DPX | NWAY_AD_TX_FULL_DPX | NWAY_AD_T4_CAPABLE))
		{
            m_AiLineSpeedCur = 100;
		}
        else
		{
            m_AiLineSpeedCur = 10;
		}

        // Get duplex setting -- use priority resolution algorithm
        if (MdiOwnAdReg & (NWAY_AD_T4_CAPABLE))
        {
            m_AiDuplexCur = 1;
            return;
        }
        else if (MdiOwnAdReg & (NWAY_AD_TX_FULL_DPX))
        {
            m_AiDuplexCur = 2;
            return;
        }
        else if (MdiOwnAdReg & (NWAY_AD_TX_HALF_DPX))
        {
            m_AiDuplexCur = 1;
            return;
        }
        else if (MdiOwnAdReg & (NWAY_AD_10T_FULL_DPX))
        {
            m_AiDuplexCur = 2;
            return;
        }
        else
        {
            m_AiDuplexCur = 1;
            return;
        }
    }

    // If we are connected to a non-NWAY repeater or hub, and the line
    // speed was determined automatically by parallel detection, then we have
    // no way of knowing exactly what speed the PHY is set to unless that PHY
    // has a propietary register which indicates speed in this situation.  The
    // NSC TX PHY does have such a register.  Also, since NWAY didn't establish
    // the connection, the duplex setting should HALF duplex.

    m_AiDuplexCur = 1;

    if (PhyId == PHY_NSC_TX)
    {
        TRACE("Detecting Speed/Dpx from non-NWAY NSC connection\n");

        // Read register 25 to get the SPEED_10 bit
        MdiMiscReg = MdiRead(NSC_SPEED_IND_REG, m_PhyAddress);

        // If bit 6 was set then we're at 10mb
        if (MdiMiscReg & NSC_TX_SPD_INDC_SPEED)
		{
            m_AiLineSpeedCur = 10;
		}
        else
		{
            m_AiLineSpeedCur = 100;
		}
    }
    else
	{
		// If we don't know what line speed we are set at, then we'll default to
		// 10mbs
        m_AiLineSpeedCur = 10;
	}
}

////////////////////////////////////////////////////////////////////
// E100bexPhy::Select 
//
// This routine will Isolate all Phy addresses on the MII
//                  Bus except for the one address to be 'selected'.  This
//                  Phy address will be un-isolated and auto-negotiation will
//                  be enabled, started, and completed.  The Phy will NOT be
//                  reset and the speed will NOT be set to any value (that is
//                  done in SetupPhy).
//
// Parameters:       
//		SelectPhyAddress - PhyAddress to select
//      WaitAutoNeg      - TRUE = Wait for Auto Negociation to complete.
//                         FALSE = don't wait. Good for 'No Link' case.
// IRQL: 
//		
// Return Mode:
//		Synchronous
//      
VOID E100bexPhy::Select( UINT SelectPhyAddress, BOOLEAN WaitAutoNeg)
{
    // Isolate all other phys and unisolate the one to query
    for (UCHAR i = 0; i < 32; i++ )
    {
        if (i != SelectPhyAddress)
        {
            // isolate this phy
            MdiWrite( MDI_CONTROL_REG, i, MDI_CR_ISOLATE);
            // wait 100 microseconds for the phy to isolate.
            NdisStallExecution(100);
        }
    }

    // unisolate the phy to query

    // Read the MDI control register
    USHORT MdiControlReg = MdiRead( MDI_CONTROL_REG, SelectPhyAddress);
    // Set/Clear bit unisolate this phy
    MdiControlReg &= ~MDI_CR_ISOLATE;             // Clear the Isolate Bit
    // issue the command to unisolate this Phy
    MdiWrite(MDI_CONTROL_REG, SelectPhyAddress, MdiControlReg);
    // sticky bits on link
    USHORT MdiStatusReg = MdiRead(MDI_STATUS_REG, SelectPhyAddress);
    MdiStatusReg = MdiRead(MDI_STATUS_REG, SelectPhyAddress);
    // if we have link, don't mess with the phy
    if (MdiStatusReg & MDI_SR_LINK_STATUS)
	{
        return;
	}

    // Read the MDI control register
    MdiControlReg = MdiRead(MDI_CONTROL_REG, SelectPhyAddress);
    // set Restart auto-negotiation
    MdiControlReg |= MDI_CR_AUTO_SELECT;          // Set Auto Neg Enable
    MdiControlReg |= MDI_CR_RESTART_AUTO_NEG;     // Restart Auto Neg
    // restart the auto-negotion process
    MdiWrite(MDI_CONTROL_REG, SelectPhyAddress, MdiControlReg);
    // wait 200 microseconds for the phy to unisolate.
    NdisStallExecution(200);

    if (WaitAutoNeg)
    {
        // wait for auto-negotiation to complete (up to 3.5 seconds)
        for (i = RENEGOTIATE_TIME; i != 0; i-- )
        {
            // Read the status register twice because of sticky bits
            MdiStatusReg = MdiRead(MDI_STATUS_REG, SelectPhyAddress);
            MdiStatusReg = MdiRead(MDI_STATUS_REG, SelectPhyAddress);

            if (MdiStatusReg & MDI_SR_AUTO_NEG_COMPLETE)
			{
                break;
			}

            StallExecution(100);
        }
    }
}

////////////////////////////////////////////////////////////////////
// E100bexPhy::MdiRead
//
// This routine will read a value from the specified MII register
//              of an external MDI compliant device (e.g. PHY 100), and return
//              it to the calling routine.  The command will execute in polled
//              mode.
//
// Parameters:       
//      MIIRegAddress - The MII register that we are reading from
//      PhyMdiAddress - The MDI address of the Phy component.
// IRQL: 
//		
// Return Mode:
//		Synchronous
// Returns:
//      The value read from the MII register.
//      
USHORT E100bexPhy::MdiRead(ULONG MIIRegAddress, ULONG PhyMdiAddress)
{
	// Issue the read command to the MDI control register.
	m_MDIControl = ((MIIRegAddress << 16) |
		(PhyMdiAddress << 21) |
		(MDI_READ << 26));

	// wait 20usec before checking status
	NdisStallExecution(20);

	// poll for the mdi read to complete
	for (UINT counter = 100000; counter != 0; counter--)
	{
		if ( (ULONG) m_MDIControl & MDI_PHY_READY)
		{
			break;
		}

		NdisStallExecution(20);
	}

	if (!counter)
	{
		TRACE("HARDWARE_NOT_RESPONDING\n");
		return 0;
	}

	return (USHORT) (ULONG) m_MDIControl;
}

////////////////////////////////////////////////////////////////////
// E100bexPhy::MdiWrite
//
// This routine will write a value to the specified MII register
//              of an external MDI compliant device (e.g. PHY 100).  The
//              command will execute in polled mode.
//
// Parameters:       
//      MIIRegAddress - The MII register that we are writing to
//      PhyMdiAddress - The MDI address of the Phy component.
//		DataValue	  - The value to write to the MII register.
// IRQL: 
//		
// Return Mode:
//		Synchronous
//      
VOID E100bexPhy::MdiWrite(
	ULONG MIIRegAddress,
	ULONG PhyMdiAddress,
	USHORT DataValue
	)
{
    // Issue the write command to the MDI control register.
    m_MDIControl = (((ULONG) DataValue) |
        (MIIRegAddress << 16) |
        (PhyMdiAddress << 21) |
        (MDI_WRITE << 26));

    // wait 20usec before checking status
    NdisStallExecution(20);

    // poll for the mdi write to complete
    for (UINT counter = 100000; counter != 0; counter--)
    {
        if ( (ULONG) m_MDIControl & MDI_PHY_READY)
		{
            break;
		}

        NdisStallExecution(20);
    }

    if (!counter)
    {
		TRACE("HARDWARE_NOT_RESPONDING\n");
    }
}

////////////////////////////////////////////////////////////////////
// E100bexPhy::Reset
//
// This routine will reset the Phy
//
// Parameters:       
//		None
// IRQL: 
//		
// Return Mode:
//		Synchronous
//      
VOID E100bexPhy::Reset(void)
{
    TRACE("E100bexPhy::Reset\n");

    // Reset the Phy, enable auto-negotiation, and restart auto-negotiation.
    USHORT MdiControlReg = MDI_CR_AUTO_SELECT | MDI_CR_RESTART_AUTO_NEG | MDI_CR_RESET;

    // Write the MDI control register with our new Phy configuration
    MdiWrite( MDI_CONTROL_REG, m_PhyAddress, MdiControlReg);
}
