# makefile.inc - message processing for HotII
# Copyright (c) 1998 Compuware Corporation

Hotpci.rc: Hotpcilog.rc

Hotpcilog.h Hotpcilog.rc msg00001.bin: Hotpcilog.mc
    mc -v Hotpcilog.mc
