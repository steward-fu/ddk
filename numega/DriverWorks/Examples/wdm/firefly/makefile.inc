# makefile.inc - MOF compiler lines for Firefly sample
# for use with the BUILD utility
# Copyright (c) 2003 Compuware Corporation

mofcomp: Firefly.BMF

Firefly.BMF: Firefly.mof
        mofcomp -B:Firefly.BMF Firefly.mof
        wmimofck Firefly.BMF


