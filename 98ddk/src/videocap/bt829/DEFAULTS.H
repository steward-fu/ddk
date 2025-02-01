#ifndef __DEFAULTS_H
#define __DEFAULTS_H

const int NumBuffers = 8;

// BUGBUG. Hardcoded for NTSC for now
const int SamplingFrequency = 28636363;
const int NTSCFrameDuration = 333667;
const int NTSCFieldDuration = NTSCFrameDuration/2;

const int DefWidth = 320;
const int DefHeight = 240;

const int MaxInWidth = 720;
const int MinInWidth = 80;

const int MaxInHeight = 480;
const int MinInHeight = 60;

const int MaxOutWidth = 320;
const int MinOutWidth = 80;

const int MaxOutHeight = 240;
const int MinOutHeight = 60;

const int VBISamples  = 800 * 2;

/// BUGBUG. hack
const int VREFDiscard = 8;

const int VBIStart    =  10; // inclusive
const int VBIEnd      =  21; // inclusive
const int VBILines    = (VBIEnd - VBIStart) + 1;

#endif
