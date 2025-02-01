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

#ifndef __REGISTER_H
#define __REGISTER_H

#ifndef __REGBASE_H
#include "regbase.h"
#endif

#include "capmain.h"

/* Type: Register
 * Purpose: An intermediate class between the RegBase and actual usable classes.
 *   Actual classes are RegisterB, RegisterW, RegisterDW
 * Attributes:
 *   uOffset_: unsigned int - an offset of the register from the base
 * Operations:
 *   GetOffset(): returns the offset value. Protected
 *   operator DWORD(): data access method. Always returns -1
 *   DWORD operator=( DWORD ): assignment operator. Always returns -1
 * Note:
 *   The reason to have operators in this class is for the register field class
 *   to have a member of type 'reference to Register'. Otherwise RegField is not
 *   able to use access methods.
 */
class Register : public RegBase
{
   private:
      unsigned int uOffset_;
      Register();
    protected:

    public:
         unsigned int GetOffset() { return uOffset_; }

         virtual operator DWORD();
      virtual DWORD operator=( DWORD dwValue );

         Register( unsigned int uOff, RegisterType aType ) :
            RegBase( aType ), uOffset_( uOff ) {}
};

/* Type: RegisterB
 * Purpose: A register that performs the BYTE I/O
 * Note:
 *   This class has no additional data members, it just overloads operators
 */
class RegisterB : public Register
{
    private:
         RegisterB();
         DWORD ReadReg(BYTE);
         DWORD WriteReg(BYTE, BYTE);
         HW_DEVICE_EXTENSION *m_deviceExtension;
    public:
         virtual operator DWORD();
         virtual DWORD operator=( DWORD dwValue );
         RegisterB( unsigned int uOff, RegisterType aType, HW_DEVICE_EXTENSION *deviceExtension ) :
            Register( uOff, aType ) {m_deviceExtension = deviceExtension;};
};

/* Type: RegisterW
 * Purpose: A register that performs the WORD I/O
 * Note:
 *   This class has no additional data members, it just overloads operators
 */
class RegisterW : public Register
{
    private:
         RegisterW();
    public:
         virtual operator DWORD();
         virtual DWORD operator=( DWORD dwValue );
         RegisterW( unsigned int uOff, RegisterType aType ) :
            Register( uOff, aType ) {};

};


/* Type: RegisterDW
 * Purpose: A register that performs the DWORD I/O
 * Note:
 *   This class has no additional data members, it just overloads operators
 */
class RegisterDW : public Register
{
    private:
         RegisterDW();
    public:
         virtual operator DWORD();
         virtual DWORD    operator=( DWORD dwValue );

         RegisterDW( unsigned int uOff, RegisterType aType ) :
            Register( uOff, aType ) {};
};

#endif __REGISTER_H
