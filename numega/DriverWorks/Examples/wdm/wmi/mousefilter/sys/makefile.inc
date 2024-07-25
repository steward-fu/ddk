# makefile.inc - MOF compiler lines for MouseFilter sample
# Copyright (c) 1999 Compuware Corporation

mofcomp: MOUFILT.BMF

MOUFILT.BMF: mousefilter.mof
        mofcomp -B:MOUFILT.BMF mousefilter.mof
        wmimofck MOUFILT.BMF


