/****************************************************************************
 *
 *  DEVICE.CPP
 *
 *  Copyright (c) Microsoft Corporation 1996-1997
 *  All rights reserved
 *
 ***************************************************************************/

#ifndef SCLBUTTON_H
#define SCLBUTTON_H

const LEN_INQUIRE_BUTTON = 8;
const BYTE INQUIRE_BUTTON[LEN_INQUIRE_BUTTON + 1] = "\x1B*s1044E";

const LEN_INQUIRE_BUTTON_READ = 10;

const LEN_CLEAR_BUTTON = 5;
const BYTE CLEAR_BUTTON[LEN_CLEAR_BUTTON + 1] = "\x1B*a0A";

const LEN_CURRENT_ERROR = 7;
const BYTE CURRENT_ERROR[LEN_CURRENT_ERROR + 1] = "\x1B*s259E";

const LEN_DIAGS = 5;
const BYTE TURN_ON_LAMP[LEN_DIAGS + 1] = "\x1B*f1L";
const BYTE TURN_OFF_LAMP[LEN_DIAGS + 1] = "\x1B*f0L";
const BYTE SELF_TEST[LEN_DIAGS + 1] = "\x1B*f2T";
const BYTE STATUS_STRING[LEN_DIAGS + 1] = "\x1B*s5E";

#endif
