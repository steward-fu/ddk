#To be included into a|w\makefile.inc
copyfile: $(TARGET)
	copy ..\fakeime.inf $(**D)
	copy ..\*.dic $(**D)

