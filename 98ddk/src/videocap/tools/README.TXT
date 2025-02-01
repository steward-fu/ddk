This directory contains two binaies that can be used to test video
capture minidrivers. Both of these tools are part of larger SDKs where 
you can obtain the sources and more complete documentation. They are
included here simply to provide easy verification of minidrivers.

vidcap32 - this is a Video for Windows (VfW) application that uses the VfW 1.1
interfaces to control a video source. Using this binary the VfW to WDM mapper
will be used to control a WDM Video capture minidriver. Vidcap32 is
more fully described in the platform SDK which contains the VfW 1.1 SDK.

amcap - this is a DirectShow (ActiveMovie) application which uses the
WDM driver directly, that is, without using the VfW to WDM mapper. There is
a document that describes simple use of the amcap tool. It is more
fully described in the DirectMedia SDK.

