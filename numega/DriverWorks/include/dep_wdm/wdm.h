// DUMMY wdm.h
// This file is intentionally empty.
//
#ifdef NTVERSION
#if NTVERSION == 'WDM'
#error Invalid wdm.h! Check include path.  wdm.h should be in the DDK directory defined by $(WDM_INC_PATH).
#endif
