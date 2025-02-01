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

#ifndef __CONSTR_H
#define __CONSTR_H

#define REGALIGNMENT 1

#define CONSTRUCT_REGISTERS \
decRegSTATUS ( ( (0x00 + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decFieldPRES( decRegSTATUS, 7, 1, RW) ,\
decFieldHLOC( decRegSTATUS, 6, 1, RW) ,\
decFieldEVENFIELD( decRegSTATUS, 5, 1, RW) ,\
decFieldNUML( decRegSTATUS, 4, 1, RW) ,\
decFieldCSEL( decRegSTATUS, 3, 1, RW) ,\
decFieldSTATUS_RES( decRegSTATUS, 2, 1, RW) ,\
decFieldLOF( decRegSTATUS, 1, 1, RW) ,\
decFieldCOF( decRegSTATUS, 0, 1, RW) ,\
decRegIFORM ( ( (0x01 + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decFieldHACTIVE( decRegIFORM, 7, 1, RW) ,\
decFieldMUXSEL( decRegIFORM, 5, 2, RW) ,\
decFieldXTSEL( decRegIFORM, 3, 2, RW) ,\
decFieldFORMAT( decRegIFORM, 0, 3, RW) ,\
decRegTDEC ( ( (0x02 + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decFieldDEC_FIELD( decRegTDEC, 7, 1, RW) ,\
decFieldDEC_FIELDALIGN( decRegTDEC, 6, 1, RW) ,\
decFieldDEC_RAT( decRegTDEC, 0, 6, RW) ,\
decRegBRIGHT ( ( (0x0A + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decRegMISCCONTROL ( ( (0x0B + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decFieldLNOTCH( decRegMISCCONTROL, 7, 1, RW) ,\
decFieldCOMP( decRegMISCCONTROL, 6, 1, RW) ,\
decFieldLDEC( decRegMISCCONTROL, 5, 1, RW) ,\
decFieldCBSENSE( decRegMISCCONTROL, 4, 1, RW) ,\
decFieldMISCCONTROL_RES( decRegMISCCONTROL, 3, 1, RW) ,\
decFieldCON_MSB( decRegMISCCONTROL, 2, 1, RW) ,\
decFieldSAT_U_MSB( decRegMISCCONTROL, 1, 1, RW) ,\
decFieldSAT_V_MSB( decRegMISCCONTROL, 0, 1, RW) ,\
decRegCONTRAST_LO ( ( (0x0C + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decRegSAT_U_LO ( ( (0x0D + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decRegSAT_V_LO ( ( (0x0E + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decRegHUE ( ( (0x0F + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decRegSCLOOP ( ( (0x10 + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decFieldCAGC( decRegSCLOOP, 6, 1, RW) ,\
decFieldCKILL( decRegSCLOOP, 5, 1, RW) ,\
decFieldHFILT( decRegSCLOOP, 3, 2, RW) ,\
decRegWC_UP( ( (0x11 + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decRegOFORM ( ( (0x12 + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decFieldRANGE( decRegOFORM, 7, 1, RW) ,\
decFieldCORE( decRegOFORM, 5, 2, RW) ,\
decFieldVBI_FRAME( decRegOFORM, 4, 1, RW) ,\
decFieldCODE( decRegOFORM, 3, 1, RW) ,\
decFieldLEN( decRegOFORM, 2, 1, RW) ,\
decFieldOES( decRegOFORM, 0, 2, RW) ,\
decRegVSCALE_HI ( ( (0x13 + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decFieldYCOMB( decRegVSCALE_HI, 7, 1, RW) ,\
decFieldCOMB( decRegVSCALE_HI, 6, 1, RW) ,\
decFieldINT( decRegVSCALE_HI, 5, 1, RW) ,\
decRegTEST ( ( (0x15 + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decRegVPOLE ( ( (0x16 + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decFieldOUT_EN (decRegVPOLE, 7, 1, RW), \
decFieldDVALID (decRegVPOLE, 6, 1, RW), \
decFieldVACTIVE (decRegVPOLE, 5, 1, RW), \
decFieldCBFLAG (decRegVPOLE, 4, 1, RW), \
decFieldFIELD (decRegVPOLE, 3, 1, RW), \
decFieldACTIVE (decRegVPOLE, 2, 1, RW), \
decFieldHRESET (decRegVPOLE, 1, 1, RW), \
decFieldVRESET (decRegVPOLE, 0, 1, RW), \
decRegADELAY ( ( (0x18 + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decRegBDELAY ( ( (0x19 + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decRegADC ( ( (0x1A + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decFieldSYNC_T( decRegADC, 5, 1, RW) ,\
decFieldAGC_EN( decRegADC, 4, 1, RW) ,\
decFieldCLK_SLEEP( decRegADC, 3, 1, RW) ,\
decFieldY_SLEEP( decRegADC, 2, 1, RW) ,\
decFieldC_SLEEP( decRegADC, 1, 1, RW) ,\
decFieldCRUSH( decRegADC, 0, 1, RW), \
decRegVTC ( ( (0x1B + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decFieldHSFMT( decRegVTC, 7, 1, RW) ,\
decRegWC_DN( ( (0x1E + 0) * REGALIGNMENT) + 0, RW, deviceExtension ) ,\
decRegSRESET ( ( (0x1F + 0) * REGALIGNMENT) + 0, RW, deviceExtension ), \
decRegODD_MISCCONTROL ( ( (0x0B + -0x03) * REGALIGNMENT) + 0x8C, RW, deviceExtension ) ,\
decFieldODD_LNOTCH( decRegODD_MISCCONTROL, 7, 1, RW) ,\
decFieldODD_COMP( decRegODD_MISCCONTROL, 6, 1, RW) ,\
decFieldODD_LDEC( decRegODD_MISCCONTROL, 5, 1, RW) ,\
decFieldODD_CBSENSE( decRegODD_MISCCONTROL, 4, 1, RW) ,\
decFieldODD_MISCCONTROL_RES( decRegODD_MISCCONTROL, 3, 1, RW) ,\
decFieldODD_CON_MSB( decRegODD_MISCCONTROL, 2, 1, RW) ,\
decFieldODD_SAT_U_MSB( decRegODD_MISCCONTROL, 1, 1, RW) ,\
decFieldODD_SAT_V_MSB( decRegODD_MISCCONTROL, 0, 1, RW) ,\
decRegODD_SCLOOP ( ( (0x10 + -0x03) * REGALIGNMENT) + 0x8C, RW, deviceExtension ) ,\
decFieldODD_CAGC( decRegODD_SCLOOP, 6, 1, RW) ,\
decFieldODD_CKILL( decRegODD_SCLOOP, 5, 1, RW) ,\
decFieldODD_HFILT( decRegODD_SCLOOP, 3, 2, RW) ,\
decRegODD_VSCALE_HI ( ( (0x13 + -0x03) * REGALIGNMENT) + 0x8C, RW, deviceExtension ) ,\
decFieldODD_YCOMB( decRegODD_VSCALE_HI, 7, 1, RW) ,\
decFieldODD_COMB( decRegODD_VSCALE_HI, 6, 1, RW) ,\
decFieldODD_INT( decRegODD_VSCALE_HI, 5, 1, RW) ,\
decRegODD_VTC ( ( (0x1B + -0x03) * REGALIGNMENT) + 0x8C, RW, deviceExtension ) ,\
decFieldODD_HSFMT( decRegODD_VTC, 7, 1, RW)

#endif   // __CONSTR_H


