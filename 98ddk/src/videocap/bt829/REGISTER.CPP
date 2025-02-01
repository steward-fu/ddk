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

#include "register.h"
#include "i2script.h"
#include "capdebug.h"
#include "capmain.h"

/* Method: Register::operator DWORD()
 * Purpose: a dummy function. Always returns -1
*/
Register::operator DWORD()
{
   return ReturnAllFs();
}


/* Method: Register::operator=
 * Purpose: a dummy function. Does not perform an assignment. Always returns -1
*/
DWORD Register::operator=( DWORD )
{
   return ReturnAllFs();
}

/* Method: RegisterB::operator DWORD()
 * Purpose: Performs the read from a byte register
*/
RegisterB::operator DWORD()
{
   // if write-only return the shadow
   if ( GetRegisterType() == WO )
      return GetShadow();

   // for RO and RW do the actual read
   LPBYTE pRegAddr = GetBaseAddress() + GetOffset();
   // Not really an address; just a number indicating which reg
   return ReadReg((BYTE)pRegAddr);
}


/* Method: RegisterB::operator=
 * Purpose: performs the assignment to a byte register
*/
DWORD RegisterB::operator=( DWORD dwValue )
{
// if a register is read-only nothing is done. This is an error
   if ( GetRegisterType() == RO )
      return ReturnAllFs();

   // keep a shadow around
   SetShadow( dwValue );

   LPBYTE pRegAddr = GetBaseAddress() + GetOffset();

   // Not really an address; just a number indicating which reg
   WriteReg((BYTE) pRegAddr, (BYTE)dwValue);

   return dwValue;
}

DWORD RegisterB::ReadReg(BYTE reg)
{
    I2CPacket   i2cPacket;
    BYTE outBuf = reg;
    BYTE inBuf;

    CI2CScript *pI2cScript = m_deviceExtension->pI2cScript;

    i2cPacket.uchChipAddress = m_deviceExtension->chipAddr;
    i2cPacket.cbReadCount = sizeof(inBuf);
    i2cPacket.cbWriteCount = sizeof(outBuf);
    i2cPacket.puchReadBuffer = &inBuf;
    i2cPacket.puchWriteBuffer = &outBuf;
    i2cPacket.usFlags = I2COPERATION_READ;
    i2cPacket.uchORValue = 0;
    i2cPacket.uchANDValue = 0;

    if (!pI2cScript->LockI2CProviderEx())
    {
        DBGERROR(("Bt829: Couldn't get I2CProvider.\n"));
        TRAP();
        return ReturnAllFs();
    }
    
    // Now  I know I have I2c services.

    pI2cScript->ExecuteI2CPacket(&i2cPacket);

    pI2cScript->ReleaseI2CProvider();
    if (i2cPacket.uchI2CResult == I2C_STATUS_NOERROR)
    {
        return (DWORD) inBuf;
    }
    else
    {
        TRAP();

        return ReturnAllFs();
    }
}

DWORD RegisterB::WriteReg(BYTE reg, BYTE value)
{
    I2CPacket   i2cPacket;
    BYTE outBuf[2];

    CI2CScript *pI2cScript = m_deviceExtension->pI2cScript;

    outBuf[0] = reg;
    outBuf[1] = value;
    i2cPacket.uchChipAddress = m_deviceExtension->chipAddr;
    i2cPacket.cbReadCount = 0;
    i2cPacket.cbWriteCount = sizeof(outBuf);
    i2cPacket.puchReadBuffer = NULL;
    i2cPacket.puchWriteBuffer = &outBuf[0];
    i2cPacket.usFlags = I2COPERATION_WRITE;
    i2cPacket.uchORValue = 0;
    i2cPacket.uchANDValue = 0;

    if (!pI2cScript->LockI2CProviderEx())
    {
        DBGERROR(("Bt829: Couldn't get I2CProvider.\n"));
        TRAP();
        return ReturnAllFs();
    }
    // Now  I know I have I2c services.

    pI2cScript->ExecuteI2CPacket(&i2cPacket);

    pI2cScript->ReleaseI2CProvider();

    if (i2cPacket.uchI2CResult == I2C_STATUS_NOERROR)
    {
        return value;
    }
    else
    {
        TRAP();

        return ReturnAllFs();
    }
}

/* Method: RegisterW::operator DWORD()
 * Purpose: Performs the read from a word register
*/
RegisterW::operator DWORD()
{
   // Should never get here. This driver only supports byte registers
   TRAP();

   // if write-only return the shadow
   if ( GetRegisterType() == WO )
      return GetShadow();

   // for RO and RW do the actual read
   LPWORD pRegAddr = (LPWORD)( GetBaseAddress() + GetOffset() );

   return READ_REGISTER_USHORT( pRegAddr );
}

/* Method: RegisterW::operator=
 * Purpose: performs the assignment to a word register
*/
DWORD RegisterW::operator=( DWORD dwValue )
{
   // Should never get here. This driver only supports byte registers
   TRAP();

   // if a register is read-only nothing is done. This is an error
   if ( GetRegisterType() == RO )
      return ReturnAllFs();

   // keep a shadow around
   SetShadow( dwValue );

   LPWORD pRegAddr = (LPWORD)( GetBaseAddress() + GetOffset() );
   *pRegAddr = (WORD)dwValue;

   WRITE_REGISTER_USHORT( pRegAddr, (WORD)dwValue );

   return dwValue;
}

/* Method: RegisterDW::operator DWORD()
 * Purpose: Performs the read from a dword register
*/
RegisterDW::operator DWORD()
{
   // Should never get here. This driver only supports byte registers
   TRAP();

   // if write-only return the shadow
   if ( GetRegisterType() == WO )
      return GetShadow();

   // for RO and RW do the actual read
   LPDWORD pRegAddr = (LPDWORD)( GetBaseAddress() + GetOffset() );

   return READ_REGISTER_ULONG( pRegAddr );
}


/* Method: RegisterDW::operator=
 * Purpose: performs the assignment to a dword register
*/
DWORD RegisterDW::operator=( DWORD dwValue )
{
   // Should never get here. This driver only supports byte registers
   TRAP();

   // if a register is read-only nothing is done. This is an error
   if ( GetRegisterType() == RO )
      return ReturnAllFs();

   // keep a shadow around
   SetShadow( dwValue );

   LPDWORD pRegAddr = (LPDWORD)( GetBaseAddress() + GetOffset() );

   WRITE_REGISTER_ULONG( pRegAddr, dwValue );
   return dwValue;
}
