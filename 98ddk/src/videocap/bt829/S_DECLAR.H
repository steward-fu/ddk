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

#ifndef __S_DECLAR_H
#define __S_DECLAR_H

//===========================================================================
// Scaler registers
//===========================================================================
RegisterB regCROP;
RegField  fieldVDELAY_MSB;
RegField  fieldVACTIVE_MSB;
RegField  fieldHDELAY_MSB;
RegField  fieldHACTIVE_MSB;
RegisterB regVDELAY_LO;
RegisterB regVACTIVE_LO;
RegisterB regHDELAY_LO;
RegisterB regHACTIVE_LO;
RegisterB regHSCALE_HI;
RegField  fieldHSCALE_MSB;
RegisterB regHSCALE_LO;
RegisterB regSCLOOP;
RegField  fieldHFILT;
RegisterB regVSCALE_HI;
RegField  fieldVSCALE_MSB;
RegisterB regVSCALE_LO;
RegisterB regVTC;
RegField  fieldVBIEN;
RegField  fieldVBIFMT;
RegField  fieldVFILT;
CompositeReg regVDelay;
CompositeReg regVActive;
CompositeReg regVScale;
CompositeReg regHDelay;
CompositeReg regHActive;
CompositeReg regHScale;

// Since VDelay register in hardware is reversed;
// i.e. odd reg is really even field and vice versa, need an extra cropping reg
// for the opposite field
RegisterB regReverse_CROP;

#endif   // __S_DECLAR_H
