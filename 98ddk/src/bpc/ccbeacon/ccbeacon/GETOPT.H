//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// getopt.h : include file for other files USING getopt functions



#ifdef __cplusplus 
extern "C" {
#endif

char getopt (
          int nargc,
          char **nargv,
          char *ostr );

extern char *optarg;
extern int optind;
extern int opterr;
extern char optopt;


#ifdef __cplusplus 
}
#endif
