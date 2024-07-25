VCOM1394.rc: msglog.rc

msglog.h msglog.rc msg00001.bin: msglog.mc
    mc -v msglog.mc
	copy msglog.mc MC_RAN.###
