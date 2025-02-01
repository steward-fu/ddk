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

#ifndef __S_CONSTR_H
#define __S_CONSTR_H

#define REGALIGNMENT 1

#define CONSTRUCT_SCALER_REGISTERS( offset ) \
regCROP ( (0x03 * REGALIGNMENT) + (offset), RW, deviceExtension ) ,\
fieldVACTIVE_MSB( regCROP, 4, 2, RW) ,\
fieldHDELAY_MSB( regCROP, 2, 2, RW) ,\
fieldHACTIVE_MSB( regCROP, 0, 2, RW) ,\
regVACTIVE_LO ( (0x05 * REGALIGNMENT) + (offset), RW, deviceExtension ) ,\
regHDELAY_LO ( (0x06 * REGALIGNMENT) + (offset), RW, deviceExtension ) ,\
regHACTIVE_LO ( (0x07 * REGALIGNMENT) + (offset), RW, deviceExtension ) ,\
regHSCALE_HI ( (0x08 * REGALIGNMENT) + (offset), RW, deviceExtension ) ,\
fieldHSCALE_MSB( regHSCALE_HI, 0, 8, RW) ,\
regHSCALE_LO ( (0x09 * REGALIGNMENT) + (offset), RW, deviceExtension ) ,\
regSCLOOP ( (0x10 * REGALIGNMENT) + (offset), RW, deviceExtension ) ,\
fieldHFILT( regSCLOOP, 3, 2, RW) ,\
regVSCALE_HI ( (0x13 * REGALIGNMENT) + (offset), RW, deviceExtension ) ,\
fieldVSCALE_MSB( regVSCALE_HI, 0, 5, RW) ,\
regVSCALE_LO ( (0x14 * REGALIGNMENT) + (offset), RW, deviceExtension ) ,\
regVActive( regVACTIVE_LO, 8, fieldVACTIVE_MSB, RW ),\
regVScale( regVSCALE_LO, 8, fieldVSCALE_MSB, RW ),\
regHDelay( regHDELAY_LO, 8, fieldHDELAY_MSB, RW ),\
regHActive( regHACTIVE_LO, 8, fieldHACTIVE_MSB, RW ),\
regHScale( regHSCALE_LO, 8, fieldHSCALE_MSB, RW ),\
regVTC ( (0x1B * REGALIGNMENT) + (offset), RW, deviceExtension ) ,\
fieldVBIEN  ( regVTC, 4, 1, RW), \
fieldVBIFMT ( regVTC, 3, 1, RW), \
fieldVFILT  ( regVTC, 0, 2, RW)

#endif   // __S_CONSTR_H
