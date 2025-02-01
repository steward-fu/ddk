/*++

Copyright (c) 1998 Bull Smart Cards & Terminals

Module Name:

    TLP3cb.c

Abstract:

    Implements the callback functions of the smart card library.

Environment:

    Win 9x

--*/

#include <stdio.h>
#include <TLP3Scr.h>

BOOL
_cdecl
TLP3ConfigureLigthSerialPort(
    PSMARTCARD_EXTENSION SmartcardExtension
    )

/*++

TLP3ConfigureSerialPort

    This routine will appropriately configure the serial port.

Arguments:

    SmartcardExtension      Pointer to smart card struct

Return Value:

    NTSTATUS

--*/

{
    DWORD actionmask;
    BOOL bRet;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3ConfigureLigthSerialPort: Enter\n",
        TLP3SCR_NAME)
        );

    actionmask =
        fBaudRate |
        fByteSize |
        fParity |
        fStopBits |
        fBitMask |
        fErrorChar;

    bRet = VCOMM_SetCommState(
        SmartcardExtension->ReaderExtension->hCommPort,
        &SmartcardExtension->ReaderExtension->SerialConfigData.theDCB,
        actionmask);

    ASSERT(bRet != FALSE);

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3ConfigureLigthSerialPort: Exit\n",
        TLP3SCR_NAME)
        );

    return bRet;
}

NTSTATUS
TLP3CardTracking(
    PSMARTCARD_EXTENSION SmartcardExtension
    )
/*++

TLP3CardTracking

    The smart card lib requires to have this function. It is called
    to setup event tracking for card insertion and removal events.

Arguments:

    SmartcardExtension      pointer to the smart card data struct.

Return Value:

    NTSTATUS

--*/
{
    SmartcardExtension->ReaderExtension->SerialConfigData.WaitMask = TRUE;
    return STATUS_PENDING;
}


NTSTATUS
TLP3ReaderPower(
    PSMARTCARD_EXTENSION SmartcardExtension
    )
/*++

TLP3ReaderPower

    The smart card lib requires to have this function. It is called
    for certain power requests to the card. We do nothing here, because
    this action is performed in the StartIo function.

Arguments:

    SmartcardExtension - Pointer to smart card data struct.

Return Value:

    NTSTATUS

--*/
{
    ULONG numTries, step, waitTime, minWaitTime, numTry = 0;
    BOOL bDone = FALSE, bRet;
    PREADER_EXTENSION readerExtension = SmartcardExtension->ReaderExtension;
    HPORT hCommPort = readerExtension->hCommPort;
    NTSTATUS status = STATUS_SUCCESS;
    DWORD dwNumBytesWritten, dwError, dwBytesRead;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3ReaderPower: Enter (%lx)\n",
        TLP3SCR_NAME,
        SmartcardExtension->MinorIoControlCode)
        );

    //
    // Set standard parameters for serial port
    //
    readerExtension->SerialConfigData.theDCB.Parity = EVENPARITY;
    readerExtension->SerialConfigData.theDCB.StopBits = TWOSTOPBITS;

    readerExtension->SerialConfigData.theDCB.BaudRate =
        SmartcardExtension->ReaderCapabilities.DataRate.Default;

    bRet = TLP3ConfigureLigthSerialPort(SmartcardExtension);
    ASSERT(bRet == TRUE);

    if (bRet == FALSE) {

        return STATUS_INVALID_DEVICE_STATE;         	
    }

    // We don't send data to the reader, so set Number of bytes to send = 0
    SmartcardExtension->SmartcardRequest.BufferLength = 0;

    // Default number of bytes we expect to get back
    SmartcardExtension->SmartcardReply.BufferLength = 0;

    //
    // Since power down triggers the UpdateSerialStatus function, we have
    // to inform it that we forced the change of the status and not the user
    // (who might have removed and inserted a card)
    //
    SmartcardExtension->ReaderExtension->PowerRequest = TRUE;

    // purge the serial buffer (it can contain the pnp id of the reader)
    bRet = VCOMM_PurgeComm(hCommPort, SERIAL_PURGE_RX);
    ASSERT(bRet == TRUE);

    bRet = VCOMM_PurgeComm(hCommPort, SERIAL_PURGE_TX);
    ASSERT(bRet == TRUE);

    SmartcardExtension->CardCapabilities.ATR.Length = 0;

    for (step = 0; NT_SUCCESS(status) && bDone == FALSE; step++) {

        if (SmartcardExtension->MinorIoControlCode ==
            SCARD_WARM_RESET &&
            step == 0) {

            step = 4;
        }

        switch (step) {

            case 0:

                // RTS = 0 means reader is in command mode
                bRet = VCOMM_EscapeCommFunction(
                    hCommPort,
                    CLRRTS,
                    0,
                    0
                    );
                ASSERT(bRet == TRUE);

                if (bRet != TRUE) {

                    // Clear the COMM break
                    VCOMM_ClearCommError(
                        hCommPort,
                        NULL,
                        &dwError
                        );
                }

                //
                // This is the minimum wait time we have to wait before
                // we can send commands to the microcontroller.
                //
                waitTime = 1000;
                break;

            case 1:
                // write power down command to the reader
                SmartcardExtension->SmartcardRequest.BufferLength = 1;

                SmartcardExtension->SmartcardRequest.Buffer[0] =
                    READER_CMD_POWER_DOWN;

                bRet = VCOMM_WriteComm(
                    hCommPort,
                    &SmartcardExtension->SmartcardRequest.Buffer[0],
                    SmartcardExtension->SmartcardRequest.BufferLength,
                    &dwNumBytesWritten
                    );

                ASSERT(bRet == TRUE);
                waitTime = 100;
                break;

            case 2:
                // Read back the echo of the reader
                SmartcardExtension->SmartcardReply.BufferLength = 1;

                for (numTries = 0, bRet = FALSE; numTries < 10; numTries++) {

                    DWORD dwBytesRead;
                    CHAR dummy;
                    
                    VCOMM_ReadComm(hCommPort, &dummy, 1, &dwBytesRead);

                    if(dwBytesRead != 0) {
                     
                        bRet = TRUE;
                        break;
                    }

                    CM_Yield(1000, CM_YIELD_RESUME_EXEC); 
                }
                ASSERT(bRet == TRUE);

                // Wait the recovery time for the microcontroller
                waitTime = 1000;
                break;

            case 3:
                // set RTS again so the microcontroller can execute the command
                bRet = VCOMM_EscapeCommFunction(
                    hCommPort,
                    SETRTS,
                    0, 
                    0
                    );
                ASSERT(bRet == TRUE);

                if (bRet != TRUE) {

                    // Clear the COMM break
                    VCOMM_ClearCommError(
                        hCommPort,
                        NULL,
                        &dwError
                        );
                }

                waitTime = 10000;
                break;

            case 4:
                if (SmartcardExtension->MinorIoControlCode == SCARD_POWER_DOWN) {

                    if (SmartcardExtension->ReaderCapabilities.CurrentState >
                        SCARD_PRESENT) {

                        SmartcardExtension->ReaderCapabilities.CurrentState =
                            SCARD_PRESENT;
                    }

                    SmartcardExtension->CardCapabilities.Protocol.Selected =
                        SCARD_PROTOCOL_UNDEFINED;

                    SmartcardExtension->ReaderExtension->PowerRequest = FALSE;

                    bDone = TRUE;
                    break;
                }

                // clear RTS to switch to command mode
                bRet = VCOMM_EscapeCommFunction(
                    hCommPort,
                    CLRRTS,
                    0, 
                    0
                    );
                ASSERT(bRet == TRUE);

                if (bRet != TRUE) {

                    // Clear the COMM break
                    VCOMM_ClearCommError(
                        hCommPort,
                        NULL,
                        &dwError
                        );
                }

                // Wait the recovery time for the microcontroller
                waitTime = 1000;
                break;

            case 5:
                // write the appropriate reset command to the reader
                SmartcardExtension->SmartcardRequest.BufferLength = 1;
                switch (SmartcardExtension->MinorIoControlCode) {

                    case SCARD_COLD_RESET:
                        SmartcardExtension->SmartcardRequest.Buffer[0] =
                            READER_CMD_COLD_RESET;
                        break;

                    case SCARD_WARM_RESET:
                        SmartcardExtension->SmartcardRequest.Buffer[0] =
                            READER_CMD_WARM_RESET;
                        break;
                }

                bRet = VCOMM_WriteComm(
                    hCommPort,
                    &SmartcardExtension->SmartcardRequest.Buffer[0],
                    SmartcardExtension->SmartcardRequest.BufferLength,
                    &dwNumBytesWritten
                    );

                ASSERT(bRet == TRUE);
                waitTime = 100;
                break;

            case 6:
                // Read back the echo of the reader
                SmartcardExtension->SmartcardReply.BufferLength = 1;

                for (numTries = 0, bRet = FALSE; numTries < 10; numTries++) {

                    DWORD dwBytesRead;
                    CHAR dummy;
                    
                    VCOMM_ReadComm(hCommPort, &dummy, 1, &dwBytesRead);

                    if(dwBytesRead != 0) {
                     
                        bRet = TRUE;
                        break;
                    }

                    CM_Yield(1000, CM_YIELD_RESUME_EXEC); 
                }
                ASSERT(bRet == TRUE);

                // Wait the recovery time for the microcontroller
                waitTime = 1000;
                break;

            case 7:
                // set RTS again so the microcontroller can execute the command
                bRet = VCOMM_EscapeCommFunction(
                    hCommPort,
                    SETRTS,
                    0, 
                    0
                    );
                ASSERT(bRet == TRUE);

                if (bRet != TRUE) {

                    // Clear the COMM break
                    VCOMM_ClearCommError(
                        hCommPort,
                        NULL,
                        &dwError
                        );
                }

                waitTime = 10000;
                break;

            case 8:
                //
                // We now try to get the ATR as fast as possible.
                // Therefor we prev. set a very short read timeout and
                // 'hope' that the card delivered its ATR within this
                // short time. To verify the correctness of the ATR we call
                // SmartcardUpdateCardCapabilities(). If this call returns
                // with STATUS_SUCCESS we know that the ATR is complete.
                // Otherwise we read again and append the new data to the
                // ATR buffer in the CardCapabilities and try again.
                //
                SmartcardExtension->SmartcardReply.BufferLength =
                    MAXIMUM_ATR_LENGTH -
                    SmartcardExtension->CardCapabilities.ATR.Length;

                bRet = VCOMM_ReadComm(
                    hCommPort,
                    SmartcardExtension->SmartcardReply.Buffer,
                    SmartcardExtension->SmartcardReply.BufferLength,
                    &dwBytesRead
                    );
                SmartcardExtension->SmartcardReply.BufferLength = dwBytesRead;

                if(SmartcardExtension->SmartcardReply.BufferLength == 0) {

                    VCOMM_ClearCommError(hCommPort, NULL, &dwError);
                    bRet = FALSE;
                }
                waitTime = 50000;
                break;

            case 9:
                if (SmartcardExtension->SmartcardReply.BufferLength != 0) {

                    ASSERT(
                        SmartcardExtension->CardCapabilities.ATR.Length +
                        SmartcardExtension->SmartcardReply.BufferLength <
                        MAXIMUM_ATR_LENGTH
                        );

                    if( SmartcardExtension->CardCapabilities.ATR.Length +
                        SmartcardExtension->SmartcardReply.BufferLength >=
                        MAXIMUM_ATR_LENGTH) {

                        status = STATUS_UNRECOGNIZED_MEDIA;
                        bDone = TRUE;
                        break;
                    }

                    // we got some ATR bytes.
                    SysCopyMemory(
                        SmartcardExtension->CardCapabilities.ATR.Buffer +
                            SmartcardExtension->CardCapabilities.ATR.Length,
                        SmartcardExtension->SmartcardReply.Buffer,
                        SmartcardExtension->SmartcardReply.BufferLength
                        );

                    SmartcardExtension->CardCapabilities.ATR.Length +=
                        (UCHAR) SmartcardExtension->SmartcardReply.BufferLength;

                    status = SmartcardUpdateCardCapabilities(
                        SmartcardExtension
                        );
                }
                else {
                    
                    status = STATUS_TIMEOUT;
                }

                if (status != STATUS_SUCCESS && numTry < 100) {
                    // ATR is incomplete. Try again to get ATR bytes.
                    numTry += 1;
                    // continue with step 8
                    step = 7;
                    //status = STATUS_TIMEOUT;
                    status = STATUS_SUCCESS;
                    continue;
                }

                if (status != STATUS_SUCCESS) {

                    bDone = TRUE;
                    break;
                }

                waitTime = 0;
                // No break

            case 10:
                if (SmartcardExtension->ReaderCapabilities.CurrentState <=
                    SCARD_ABSENT) {

                    status = STATUS_MEDIA_CHANGED;
                }

                if (status != STATUS_SUCCESS) {

                    bDone = TRUE;
                    break;
                }

                // Copy ATR to user space
                if (SmartcardExtension->IoRequest.ReplyBuffer) {

                    SysCopyMemory(
                        SmartcardExtension->IoRequest.ReplyBuffer,
                        SmartcardExtension->CardCapabilities.ATR.Buffer,
                        SmartcardExtension->CardCapabilities.ATR.Length
                        );

                    // Tell user length of ATR
                    *SmartcardExtension->IoRequest.Information =
                        SmartcardExtension->CardCapabilities.ATR.Length;
                }

                //
                // If the card uses invers convention we need to switch
                // the serial driver to odd paritiy
                //
                if (SmartcardExtension->CardCapabilities.InversConvention) {

                    readerExtension->SerialConfigData.theDCB.Parity = ODDPARITY;
                }

                //
                // If the extra guard time is 255 it means that our
                // frame with have to expect from the card has only
                // 1 instead of 2 stop bits
                // 1start bit + 8data bits + 1parity + 1stop == 11 etu
                // see iso 7816-3 6.1.4.4 Extra Guard Time N
                //
                if (SmartcardExtension->CardCapabilities.PtsData.StopBits == 1) {

                     readerExtension->SerialConfigData.theDCB.StopBits = ONESTOPBIT;
                }

                // depending on the protocol set the timeout values
                if (SmartcardExtension->CardCapabilities.Protocol.Selected &
                    SCARD_PROTOCOL_T1) {

                    // set timeouts
                    readerExtension->SerialConfigData.CommTimeouts.ReadTotalTimeoutConstant =
                        SmartcardExtension->CardCapabilities.T1.BWT / 1000;

                    readerExtension->SerialConfigData.CommTimeouts.ReadIntervalTimeout =
                        SmartcardExtension->CardCapabilities.T1.CWT / 1000;

                } else if (SmartcardExtension->CardCapabilities.Protocol.Selected &
                           SCARD_PROTOCOL_T0) {

                    // set timeouts
                    readerExtension->SerialConfigData.CommTimeouts.ReadTotalTimeoutConstant =
                        SmartcardExtension->CardCapabilities.T0.WT / 1000;
                    readerExtension->SerialConfigData.CommTimeouts.ReadIntervalTimeout =
                        SmartcardExtension->CardCapabilities.T0.WT / 1000;
                }

                // Now make some adjustments depending on the system speed
                minWaitTime = MIN_WAITING_TIME;

                if (readerExtension->SerialConfigData.CommTimeouts.ReadTotalTimeoutConstant <
                    minWaitTime) {

                    readerExtension->SerialConfigData.CommTimeouts.ReadTotalTimeoutConstant =
                        minWaitTime;
                }

                if (readerExtension->SerialConfigData.CommTimeouts.ReadIntervalTimeout <
                    minWaitTime) {

                    readerExtension->SerialConfigData.CommTimeouts.ReadIntervalTimeout =
                        minWaitTime;
                }

                bRet = TLP3ConfigureLigthSerialPort(SmartcardExtension);
                ASSERT(bRet == TRUE);

                // We're done anyway, so leave
                bDone = TRUE;
                break;
        }

        if (!NT_SUCCESS(status)) {
            
            break;
        }

        if (waitTime) {

            if(waitTime >= 1000) {
             	
                TLP3Delay(SmartcardExtension, waitTime / 1000);
            }
            else {

                CM_Yield(waitTime, CM_YIELD_RESUME_EXEC);
            }
        }
    }

    if (status == STATUS_TIMEOUT) {

        status = STATUS_UNRECOGNIZED_MEDIA;
    }

    SmartcardExtension->ReaderExtension->PowerRequest = FALSE;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3ReaderPower: Exit (%lx)\n",
        TLP3SCR_NAME,
        status)
        );

    return status;
}

NTSTATUS
TLP3SerialIo(
    PSMARTCARD_EXTENSION SmartcardExtension
    )
    /*++

Routine Description:

    This routine sends IOCTL's to the serial driver. It waits on for their
    completion, and then returns.

    Arguments:

Return Value:

    NTSTATUS

--*/
{
    ULONG currentByte = 0, numBytesRead, waitCounter, timeout;
    NTSTATUS status = STATUS_SUCCESS;
    PUCHAR requestBuffer = NULL;
    ULONG requestBufferLength = SmartcardExtension->SmartcardRequest.BufferLength;

    // maximum delay between 2 consecutive caracters
    ULONG timout =
        SmartcardExtension->ReaderExtension->SerialConfigData.CommTimeouts.ReadIntervalTimeout;

     //
     // Check if the buffers are large enough
     //
     ASSERT(SmartcardExtension->SmartcardReply.BufferLength <=
          SmartcardExtension->SmartcardReply.BufferSize);

     ASSERT(SmartcardExtension->SmartcardRequest.BufferLength <=
          SmartcardExtension->SmartcardRequest.BufferSize);

    if (SmartcardExtension->SmartcardReply.BufferLength >
          SmartcardExtension->SmartcardReply.BufferSize ||
          SmartcardExtension->SmartcardRequest.BufferLength >
          SmartcardExtension->SmartcardRequest.BufferSize)
    {
        SmartcardDebug(
            DEBUG_TRACE,
            ("%s!TLP3SerialIo: Exit BUFFER_TOO_SMALL = (%lx)\n",
            TLP3SCR_NAME,
            STATUS_BUFFER_TOO_SMALL)
            );

        return STATUS_BUFFER_TOO_SMALL;
    }

    switch (SmartcardExtension->ReaderExtension->SerialIoControlCode)
    {
        case SMARTCARD_WRITE:
            do {

                DWORD numBytesWritten;

                if (SmartcardExtension->CardCapabilities.GT != 0) {

                    //
                    // If the guardtime isn't 0 and we write data to the smart card
                    // we only write byte by byte, because we have to insert a delay
                    // between every sent byte
                    //
                    requestBufferLength = 1;
                }

                requestBuffer =
                    &SmartcardExtension->SmartcardRequest.Buffer[currentByte++];

                VCOMM_WriteComm(
                    SmartcardExtension->ReaderExtension->hCommPort,
                    requestBuffer,
                    requestBufferLength,
                    &numBytesWritten
                    );

                // Check if we have to write more bytes to the reader
                if (SmartcardExtension->CardCapabilities.GT != 0 &&
                    currentByte <
                    SmartcardExtension->SmartcardRequest.BufferLength)
                {
                    // Now wait the required guard time
                    CM_Yield(
                        (ULONG)((SmartcardExtension->CardCapabilities.GT) + GT_WAITING_TIME_ADJUSTEMENT),
                        CM_YIELD_RESUME_EXEC
                        );

                    status = STATUS_MORE_PROCESSING_REQUIRED;

                } else {

                    status = STATUS_SUCCESS;                 	
                }

            } while (status == STATUS_MORE_PROCESSING_REQUIRED);

            break;

        case SMARTCARD_READ:

            for (currentByte = 0;
                 currentByte < SmartcardExtension->SmartcardReply.BufferLength;) {

                if (currentByte == 0 || numBytesRead != 0) {
                 	
                    if (SmartcardExtension->CardCapabilities.Protocol.Selected &
                        SCARD_PROTOCOL_T0)
                    {
                        waitCounter =
                            SmartcardExtension->CardCapabilities.T0.WT / 10000;
                    }
                    else
                    {
                        waitCounter = timeout / 10;
                    }
                }

                VCOMM_ReadComm(
                    SmartcardExtension->ReaderExtension->hCommPort,
                    &SmartcardExtension->SmartcardReply.Buffer[currentByte],
                    1,
                    &numBytesRead
                    );

                if (numBytesRead == 0)
                {
                    //
                    // We didn't get any bytes, so wait part of the character timeout time.
                    //
                    DWORD dwError;
                    _COMSTAT Comstat;

                    VCOMM_ClearCommError(
                        SmartcardExtension->ReaderExtension->hCommPort,
                        &Comstat,
                        &dwError
                        );

                    TLP3Delay(SmartcardExtension, 10);

                    if (--waitCounter == 0) {

                        // No (more) characters available
                        status = STATUS_TIMEOUT;
                        break;
                    }

                } else {

                    currentByte++;
                }
            } 

            break;
    }
    return status;
}

NTSTATUS
TLP3TransmitT0(
    PSMARTCARD_EXTENSION SmartcardExtension
    )
/*++

Routine Description:

    This function performs a T=0 transmission.

Arguments:

    SmartcardExtension - Pointer to smart card data struct.

Return Value:

    NTSTATUS

--*/
{
    PUCHAR requestBuffer = SmartcardExtension->SmartcardRequest.Buffer;
    PUCHAR replyBuffer = SmartcardExtension->SmartcardReply.Buffer;
    PULONG requestLength = &SmartcardExtension->SmartcardRequest.BufferLength;
    PULONG replyLength = &SmartcardExtension->SmartcardReply.BufferLength;
    PULONG serialIoControlCode = &SmartcardExtension->ReaderExtension->SerialIoControlCode;
    ULONG bytesToSend, bytesToRead, currentByte = 0;
    BOOLEAN restartWorkWaitingTime = FALSE;
    NTSTATUS status;
    BOOL bLoopcontinue = TRUE;

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3TransmitT0: Enter\n",
        TLP3SCR_NAME)
        );

    // Let the lib build a T=0 packet
    status = SmartcardT0Request(SmartcardExtension);

    if (status == STATUS_SUCCESS)
    {
        //
        // The number of bytes we expect from the card
        // is Le + 2 status bytes
        //
        bytesToSend = *requestLength;
        bytesToRead = SmartcardExtension->T0.Le + 2;

        //
        // Send the first 5 bytes to the card
        //
        *requestLength = 5;

        do {

            UCHAR procByte;

            //
            // According to ISO 7816 a procedure byte of
            // 60 should be treated as a request for a one time wait.
            // In this case we do not write anything to the card
            //
            if (restartWorkWaitingTime == FALSE) {

                SmartcardDebug(
                    DEBUG_PROTOCOL,
                    ("%s!TLP3TransmitT0: -> Sending %s (%ld bytes)\n",
                    TLP3SCR_NAME,
                    (currentByte == 0 ? "header" : "data"),
                    *requestLength)
                    );
                //
                // Write to the card
                //
                *serialIoControlCode = SMARTCARD_WRITE;
                SmartcardExtension->SmartcardRequest.Buffer = &requestBuffer[currentByte];

                status = TLP3SerialIo(SmartcardExtension);

                if (status != STATUS_SUCCESS)
                {
                    break;
                }

                //
                // The TLP3 echos all sent bytes. We read the echo
                // back into our send buffer
                //
                *serialIoControlCode = SMARTCARD_READ;
                *replyLength = *requestLength;
                SmartcardExtension->SmartcardReply.Buffer = &requestBuffer[currentByte];

                status = TLP3SerialIo(SmartcardExtension);

                if (status != STATUS_SUCCESS)
                {
                    break;
                }

                currentByte += *requestLength;
                bytesToSend -= *requestLength;
            }

            // Read the 'Procedure byte'.
            SmartcardExtension->SmartcardReply.Buffer = &procByte;
            *serialIoControlCode = SMARTCARD_READ;
            *replyLength = 1;

            status = TLP3SerialIo(SmartcardExtension);

            if (status != STATUS_SUCCESS)
            {
                break;
            }

            restartWorkWaitingTime = FALSE;
            //
            // Check the procedure byte.
            // Please take a look at ISO 7816 Part 3 Section 8.2.2
            //
            if (procByte == requestBuffer[1] ||
                procByte == requestBuffer[1] + 1)
            {
                SmartcardDebug(
                    DEBUG_PROTOCOL,
                    ("%s!TLP3TransmitT0: <- ACK (send all)\n",
                    TLP3SCR_NAME)
                    );

                // All remaining data bytes can be sent at once
                *requestLength = bytesToSend;

            }
            else if (procByte == (UCHAR) ~requestBuffer[1] ||
                       procByte == (UCHAR) ~(requestBuffer[1] + 1))
            {

                SmartcardDebug(
                    DEBUG_PROTOCOL,
                    ("%s!TLP3TransmitT0: <- ACK (send single)\n",
                    TLP3SCR_NAME)
                    );

                // We can send only one byte
                *requestLength = 1;

            }
            else if (procByte == 0x60 ||
                       SmartcardExtension->CardCapabilities.InversConvention &&
                       procByte == 0xf9)
            {
                //
                // We have to reset the wait time and try again to read
                //

                SmartcardDebug(
                    DEBUG_PROTOCOL,
                    ("%s!TLP3TransmitT0: <- NULL (%ldms)\n",
                    TLP3SCR_NAME,
                    SmartcardExtension->CardCapabilities.T0.WT / 1000)
                    );

                TLP3Delay(SmartcardExtension,
                    SmartcardExtension->CardCapabilities.T0.WT / 1000
                    );

                //
                // Set flag that we only should read the proc byte
                // without writing data to the card
                //
                restartWorkWaitingTime = TRUE;
            }
            else
            {
                //
                // The card returned a status byte.
                // Status bytes are always two bytes long.
                // Store this byte first and then read the next
                //
                replyBuffer[0] = procByte;

                *serialIoControlCode = SMARTCARD_READ;
                *replyLength = 1;
                bytesToSend = 0;
                bytesToRead = 0;

                //
                // Read in the second status byte
                //
                SmartcardExtension->SmartcardReply.Buffer =
                    &replyBuffer[1];

                status = TLP3SerialIo(SmartcardExtension);

                SmartcardExtension->SmartcardReply.BufferLength = 2;

                SmartcardDebug(
                    DEBUG_PROTOCOL,
                    ("%s!TLP3TransmitT0: <- SW1=%02x SW2=%02x\n",
                    TLP3SCR_NAME,
                    replyBuffer[0],
                    replyBuffer[1])
                    );
            }

        } while( (bytesToSend || restartWorkWaitingTime) && (status == STATUS_SUCCESS));

        if (status == STATUS_SUCCESS)
        {
            if (bytesToRead != 0) {

                *serialIoControlCode = SMARTCARD_READ;
                *replyLength = bytesToRead;

                SmartcardExtension->SmartcardReply.Buffer =
                    replyBuffer;

                status = TLP3SerialIo(SmartcardExtension);


                SmartcardDebug(
                    DEBUG_PROTOCOL,
                    ("%s!TLP3TransmitT0: <- Data %ld bytes, SW1=%02x SW2=%02x\n",
                    TLP3SCR_NAME,
                    bytesToRead,
                    replyBuffer[bytesToRead - 2],
                    replyBuffer[bytesToRead - 1])
                    );
            }
        }
    }

    // Restore pointers to their original location
    SmartcardExtension->SmartcardRequest.Buffer = requestBuffer;

    SmartcardExtension->SmartcardReply.Buffer = replyBuffer;

    if (status == STATUS_TIMEOUT) {

        // STATUS_TIMEOUT is not mapped to a Win32 error code
        status = STATUS_IO_TIMEOUT;
    }

    if (status == STATUS_SUCCESS) {

        status = SmartcardT0Reply(SmartcardExtension);
    }

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3TransmitT0: Exit(%lx)\n",
        TLP3SCR_NAME,
        status)
        );

    return status;
}

NTSTATUS
TLP3Transmit(
    PSMARTCARD_EXTENSION SmartcardExtension
    )
/*++

Routine Description:

    This function is called by the smart card library whenever a transmission
    is required.

Arguments:

    SmartcardExtension - Pointer to smart card data struct.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;
    BOOL bLoopcontinue = TRUE;

//    PAGED_CODE();

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3Transmit: Enter\n",
        TLP3SCR_NAME)
        );


    do {

        PUCHAR requestBuffer = SmartcardExtension->SmartcardRequest.Buffer;
        PUCHAR replyBuffer = SmartcardExtension->SmartcardReply.Buffer;
        PULONG requestLength = &SmartcardExtension->SmartcardRequest.BufferLength;
        PULONG replyLength = &SmartcardExtension->SmartcardReply.BufferLength;
        PULONG serialIoControlCode = &SmartcardExtension->ReaderExtension->SerialIoControlCode;

        //
        // Tell the lib function how many bytes I need for the prologue
        //
        *requestLength = 0;

        switch (SmartcardExtension->CardCapabilities.Protocol.Selected)
        {
            case SCARD_PROTOCOL_RAW:
                status = SmartcardRawRequest(SmartcardExtension);
                break;

            case SCARD_PROTOCOL_T0:
                //
                // T=0 requires a bit more work.
                // So we do this in a seperate function.
                //
                status = TLP3TransmitT0(SmartcardExtension);
                bLoopcontinue = FALSE;
                break;

            case SCARD_PROTOCOL_T1:
                status = SmartcardT1Request(SmartcardExtension);
                break;

            default:
                status = STATUS_INVALID_DEVICE_REQUEST;
                bLoopcontinue = FALSE;
                break;
        }

        if (status != STATUS_SUCCESS)
        {
            bLoopcontinue = FALSE;
        }

        if (bLoopcontinue != FALSE)
        {
        //
        // Write the command to the card
        //

            *replyLength = 0;
            *serialIoControlCode = SMARTCARD_WRITE;

            status = TLP3SerialIo(SmartcardExtension);

            if (status != STATUS_SUCCESS)
            {
                bLoopcontinue = FALSE;
            }
        }


        if (bLoopcontinue != FALSE)
        {
        //
        // The Bull reader always echos the bytes sent, so read that echo back
        //

            *serialIoControlCode = SMARTCARD_READ;
            *replyLength = *requestLength;

            status = TLP3SerialIo(SmartcardExtension);

            if (status != STATUS_SUCCESS)
            {
                bLoopcontinue = FALSE;
            }
        }

        if (bLoopcontinue != FALSE)
        {

            switch (SmartcardExtension->CardCapabilities.Protocol.Selected) {

                case SCARD_PROTOCOL_RAW:
                    status = SmartcardRawReply(SmartcardExtension);
                    break;

                case SCARD_PROTOCOL_T1:
                    //
                    // Check if the card requested a waiting time extension
                    //
                    if (SmartcardExtension->T1.Wtx)
                    {
                        ULONG ulMicroseconds;


                        ulMicroseconds =
                            (SmartcardExtension->T1.Wtx *
                            SmartcardExtension->CardCapabilities.T1.BWT)
                            + WAITING_TIME_ADJUSTEMENT;

                        TLP3Delay(
                            SmartcardExtension,
                            ulMicroseconds / 1000
                            );
                    }

                    //
                    // Read NAD, PCB and LEN fields
                    //
                    *replyLength = 3;

                    status = TLP3SerialIo(SmartcardExtension);

                    //
                    // Check for timeout first. If the card did not reply
                    // we need to send a resend request
                    //
                    if (status != STATUS_TIMEOUT)
                    {

                        if (status != STATUS_SUCCESS)
                        {
                            bLoopcontinue = FALSE;
                            break;
                        }

                        //
                        // The third byte contains the length of the data in the packet
                        // and we additinally want to have the EDC bytes which
                        // is one for LRC and 2 for CRC
                        //
                        *replyLength =
                            replyBuffer[2] +
                            (SmartcardExtension->CardCapabilities.T1.EDC & 0x01 ? 2 : 1);

                        // We want to have the remaining bytes just after the first 3
                        SmartcardExtension->SmartcardReply.Buffer += 3;

                        status = TLP3SerialIo(SmartcardExtension);

                        SmartcardExtension->SmartcardReply.Buffer -= 3;
                        SmartcardExtension->SmartcardReply.BufferLength += 3;

                        if (status != STATUS_SUCCESS && status != STATUS_TIMEOUT)
                        {
                            bLoopcontinue = FALSE;
                            break;
                        }
                    }

                    if (status == STATUS_TIMEOUT)
                    {
                        //
                        // Since the card did not reply we set the number of
                        // bytes received to 0. This will trigger a resend
                        // request
                        //
                        SmartcardDebug(
                            DEBUG_PROTOCOL,
                            ("%s!TLP3TransmitT1: Timeout\n",
                            TLP3SCR_NAME)
                            );
                        SmartcardExtension->SmartcardReply.BufferLength = 0;
                    }


                    SmartcardDebug(
                        DEBUG_TRACE,
                        ("%s!TLP3Transmit: ReplyBufferLength = (%lx) BufferSize = (%lx) \n",
                        TLP3SCR_NAME,
                        SmartcardExtension->SmartcardReply.BufferLength,
                        SmartcardExtension->SmartcardReply.BufferSize)
                        );
                    {
                        int i;
                        for(i=0 ; i<SmartcardExtension->SmartcardReply.BufferLength ; i++)
                            SmartcardDebug(
                                DEBUG_TRACE,
                                (" (%lx)",
                                SmartcardExtension->SmartcardReply.Buffer[i])
                                );
                    }


                    status = SmartcardT1Reply(SmartcardExtension);
                    break;

                default:
                    status = STATUS_INVALID_DEVICE_REQUEST;
                    bLoopcontinue = FALSE;
                    break;
            }
        }

    } while ((status == STATUS_MORE_PROCESSING_REQUIRED)  && (bLoopcontinue != FALSE));


    if ((bLoopcontinue == FALSE) || (status != STATUS_SUCCESS))
    {

        if (status == STATUS_TIMEOUT)
        {
            // STATUS_TIMEOUT is not mapped to a Win32 error code
            status = STATUS_IO_TIMEOUT;
        }
    }

    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3Transmit: Exit(%lx)\n",
        TLP3SCR_NAME,
        status)
        );

    return status;
}

NTSTATUS
TLP3VendorIoctl(
    PSMARTCARD_EXTENSION SmartcardExtension
    )
{
    NTSTATUS status;
    static char answer[] = "Vendor IOCTL";


    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3VendorIoctl: Enter\n",
        TLP3SCR_NAME)
        );


    if (SmartcardExtension->IoRequest.ReplyBuffer != NULL &&
        SmartcardExtension->IoRequest.ReplyBufferLength >= _lstrlen(answer) + 1)
    {
        _lstrcpyn(
            SmartcardExtension->IoRequest.ReplyBuffer,
            answer,
            sizeof(SmartcardExtension->IoRequest.ReplyBuffer));

        *SmartcardExtension->IoRequest.Information = _lstrlen(answer);
        status = STATUS_SUCCESS;

    }
    else
    {

        status = STATUS_BUFFER_TOO_SMALL;
    }


    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3VendorIoctl: Exit(%lx)\n",
        TLP3SCR_NAME,
        status)
        );


    return status;
}

NTSTATUS
TLP3SetProtocol(
    PSMARTCARD_EXTENSION SmartcardExtension
    )
/*++

Routine Description:

    The smart card lib requires to have this function. It is called
    to set a the transmission protocol and parameters. If this function
    is called with a protocol mask (which means the caller doesn't card
    about a particular protocol to be set) we first look if we can
    set T=1 and the T=0

Arguments:

    SmartcardExtension - Pointer to smart card data struct.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;
    BOOL bLoopcontinue = TRUE;

//    PAGED_CODE();


        PUCHAR ptsRequest = SmartcardExtension->SmartcardRequest.Buffer;
        PUCHAR ptsReply = SmartcardExtension->SmartcardReply.Buffer;
        PSERIAL_READER_CONFIG serialConfigData =
            &SmartcardExtension->ReaderExtension->SerialConfigData;
        PREADER_EXTENSION readerExtension =
            SmartcardExtension->ReaderExtension;
        ULONG minWaitTime, newProtocol;

        SmartcardDebug(
            DEBUG_TRACE,
            ("%s!TLP3SetProtocol: Enter\n",
            TLP3SCR_NAME)
            );


        //
        // Check if the card is already in specific state
        // and if the caller wants to have the already selected protocol.
        // We return success if this is the case.
        //
        if (SmartcardExtension->ReaderCapabilities.CurrentState == SCARD_SPECIFIC &&
            (SmartcardExtension->CardCapabilities.Protocol.Selected &
             SmartcardExtension->MinorIoControlCode))
        {
            status = STATUS_SUCCESS;
            bLoopcontinue = FALSE;

            SmartcardDebug(
                DEBUG_TRACE,
                ("%s!TLP3SetProtocol: card is already in specific state\n",
                TLP3SCR_NAME)
                );
        }

//MA        if (bLoopcontinue != FALSE)
//MA        {
            // set normal timeout
//MA            readerExtension->SerialConfigData.CommTimeouts.ReadIntervalTimeout =
//MA                READ_INTERVAL_TIMEOUT_DEFAULT;
//MA            readerExtension->SerialConfigData.CommTimeouts.ReadTotalTimeoutConstant =
//MA                READ_TOTAL_TIMEOUT_CONSTANT_DEFAULT;

//MA            if (TLP3ConfigureSerialPort(SmartcardExtension) == TRUE)
//MA            {
//MA                status = STATUS_SUCCESS;
//MA            }
//MA            else
//MA            {
//MA                status = STATUS_UNSUCCESSFUL;
//MA            }


//MA            ASSERT(status == STATUS_SUCCESS);

//MA            if (status != STATUS_SUCCESS)
//MA            {
//MA                bLoopcontinue = FALSE;
//MA            }
//MA        }


        if (bLoopcontinue != FALSE)
        {
            //
            // Assemble and send a pts selection
            //

            newProtocol = SmartcardExtension->MinorIoControlCode;

            while(bLoopcontinue != FALSE)
            {
                // set initial character of PTS
                ptsRequest[0] = 0xff;

                // set the format character
                if (SmartcardExtension->CardCapabilities.Protocol.Supported &
                    newProtocol &
                    SCARD_PROTOCOL_T1)
                {
                    // select T=1 and indicate that pts1 follows
                    ptsRequest[1] = 0x11;
                    SmartcardExtension->CardCapabilities.Protocol.Selected = SCARD_PROTOCOL_T1;

                }
                else if (SmartcardExtension->CardCapabilities.Protocol.Supported &
                           newProtocol &
                           SCARD_PROTOCOL_T0)
                {

                    // select T=0 and indicate that pts1 follows
                    ptsRequest[1] = 0x10;
                    SmartcardExtension->CardCapabilities.Protocol.Selected = SCARD_PROTOCOL_T0;

                }
                else
                {

                    status = STATUS_INVALID_DEVICE_REQUEST;
                    bLoopcontinue = FALSE;
                }


                if (bLoopcontinue != FALSE)
                {
                    // set pts1 which codes Fl and Dl
                    ptsRequest[2] =
                        SmartcardExtension->CardCapabilities.PtsData.Fl << 4 |
                        SmartcardExtension->CardCapabilities.PtsData.Dl;

                    // set pck (check character)
                    ptsRequest[3] = ptsRequest[0] ^ ptsRequest[1] ^ ptsRequest[2];

                    SmartcardExtension->SmartcardRequest.BufferLength = 4;
                    SmartcardExtension->ReaderExtension->SerialIoControlCode = SMARTCARD_WRITE;

                    status = TLP3SerialIo(SmartcardExtension);

                    if (status != STATUS_SUCCESS)
                    {
                        bLoopcontinue = FALSE;
                    }
                }


                if (bLoopcontinue != FALSE)
                {
                    // read back the echo of the reader
                    SmartcardExtension->SmartcardReply.BufferLength = 4;
                    SmartcardExtension->ReaderExtension->SerialIoControlCode = SMARTCARD_READ;

                    status = TLP3SerialIo(SmartcardExtension);

                    if (status != STATUS_SUCCESS)
                    {
                        bLoopcontinue = FALSE;
                    }
                }


                if (bLoopcontinue != FALSE)
                {
                    // read back the pts data
                    status = TLP3SerialIo(SmartcardExtension);

                    if (status != STATUS_SUCCESS &&
                        status != STATUS_TIMEOUT)
                    {
                        bLoopcontinue = FALSE;
                    }
                }


                if (bLoopcontinue != FALSE)
                {
                    if (status != STATUS_TIMEOUT &&
                        SysCompareMemory(ptsRequest, ptsReply, 4) == 0)
                    {
                        // the card replied correctly to our pts-request
                        break;
                    }

                    if (SmartcardExtension->CardCapabilities.PtsData.Type !=
                        PTS_TYPE_DEFAULT)
                    {
                        SmartcardDebug(
                            DEBUG_TRACE,
                            ("%s!TLP3SetProtocol: PTS failed. Trying default parameters...\n",
                            TLP3SCR_NAME,
                            status)
                            );
                        //
                        // The card did either NOT reply or it replied incorrectly
                        // so try default values
                        //
                        SmartcardExtension->CardCapabilities.PtsData.Type =
                            PTS_TYPE_DEFAULT;

                        SmartcardExtension->MinorIoControlCode = SCARD_COLD_RESET;

                        status = TLP3ReaderPower(SmartcardExtension);

                        continue;
                    }
                }

                // the card failed the pts-request
                status = STATUS_DEVICE_PROTOCOL_ERROR;
                bLoopcontinue = FALSE;
            }
        }


        if (bLoopcontinue != FALSE)
        {
            //
            // The card replied correctly to the pts request
            // Set the appropriate parameters for the port
            //
            if (SmartcardExtension->CardCapabilities.Protocol.Selected &
                SCARD_PROTOCOL_T1)
            {
                // set timeouts
                readerExtension->SerialConfigData.CommTimeouts.ReadTotalTimeoutConstant =
                    SmartcardExtension->CardCapabilities.T1.BWT / 1000;

                readerExtension->SerialConfigData.CommTimeouts.ReadIntervalTimeout =
                    SmartcardExtension->CardCapabilities.T1.CWT / 1000;

            }
            else if (SmartcardExtension->CardCapabilities.Protocol.Selected &
                       SCARD_PROTOCOL_T0)
            {
                // set timeouts
                readerExtension->SerialConfigData.CommTimeouts.ReadTotalTimeoutConstant =
                readerExtension->SerialConfigData.CommTimeouts.ReadIntervalTimeout =
                    SmartcardExtension->CardCapabilities.T0.WT / 1000;
            }

            // Now make some adjustments depending on the system speed
            minWaitTime = MIN_WAITING_TIME;

            if (readerExtension->SerialConfigData.CommTimeouts.ReadTotalTimeoutConstant < minWaitTime)
            {
                readerExtension->SerialConfigData.CommTimeouts.ReadTotalTimeoutConstant = minWaitTime;
            }

            if (readerExtension->SerialConfigData.CommTimeouts.ReadIntervalTimeout < minWaitTime)
            {
                readerExtension->SerialConfigData.CommTimeouts.ReadIntervalTimeout = minWaitTime;
            }

            // Change data rate according to the new settings
            serialConfigData->theDCB.BaudRate =
                SmartcardExtension->CardCapabilities.PtsData.DataRate;

            if (TLP3ConfigureLigthSerialPort(SmartcardExtension) == TRUE)
            {
                status = STATUS_SUCCESS;
            }
            else
            {
                status = STATUS_UNSUCCESSFUL;
            }


            ASSERT(status == STATUS_SUCCESS);

            // now indicate that we're in specific mode
            SmartcardExtension->ReaderCapabilities.CurrentState = SCARD_SPECIFIC;

            // return the selected protocol to the caller
            *(PULONG) SmartcardExtension->IoRequest.ReplyBuffer =
                SmartcardExtension->CardCapabilities.Protocol.Selected;

            *SmartcardExtension->IoRequest.Information =
                sizeof(SmartcardExtension->CardCapabilities.Protocol.Selected);
        }


        if (status == STATUS_TIMEOUT)
        {
            // STATUS_TIMEOUT is not mapped to a Win32 error code
            status = STATUS_IO_TIMEOUT;

            *SmartcardExtension->IoRequest.Information = 0;

        }
        else if (status != STATUS_SUCCESS)
        {
            SmartcardExtension->CardCapabilities.Protocol.Selected =
                SCARD_PROTOCOL_UNDEFINED;

            *SmartcardExtension->IoRequest.Information = 0;
        }


    SmartcardDebug(
        DEBUG_TRACE,
        ("%s!TLP3SetProtocol: Exit(%lx)\n",
        TLP3SCR_NAME,
        status)
        );

   return status;
}