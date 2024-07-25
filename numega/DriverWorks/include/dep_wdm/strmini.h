// DUMMY strmini.h
// This file is intentionally empty.
//
#ifdef NTVERSION
#if NTVERSION == 'WDM'
#error Wrong INCLUDE path! strmini.h is in the XP DDK (and later) in $(BASEDIR)\inc\ddk\wdm\wxp
#endif
