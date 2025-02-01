
--------------------------

Microsoft Windows 98 DDK

--------------------------

SUMMARY

In this sample we demonstrate how to build a simple NDIS 3.1 protocol driver.
The packet driver also demonstrates the use of the upper edge NDIS 3.1
services to communicate with NDIS 3.0 Miniports.

MORE INFORMATION

All entry points of a Protocol are included in the sample. This is to illustrate
the entry points that are for a complete Protocol to bind to a net card.

The driver expects that it will be bound an Ethernet card as it makes
assumptions about frame size.

The driver must be opened using the vxd path name because Win98 does not 
support symbolic links.  The Console sample application uses createfile in a 
manner consistent with Win32c.


BUILDING NETWORK PROTOCOL

Tools that you need to build VPacket.VxD
	Microsoft MASM 6.11c(d)
	Microsoft Visual C++
	Platform SDK
	Windows 98 DDK

Binaries that you will build
	VPacket.VxD
	Packet32.Dll
	Conpack.Exe


VPacket.VxD is a Windows 98 NDIS 3.1 Protocol Driver.  It should be installed using 
	vpacket.inf just like any protocol and bound to an appropriate net card using 
	the network control panel application.

Packet32.Dll is installed by vpacket.inf and provides as API to interface with 
	vpacket.vxd.  The dll uses DeviceIoControl to communicate with the protocol
	driver.

Conpack.Exe is a console application that utilizes Packet32.Dll to send and receive
	ethernet packets over the packet protocol.

INSTALLATION

To install the network provider use Control Panel -- Network and choose Add and Protocol.  
Select "Have Disk" and browse to a target directory that contains at least:
	vpacket.inf
	vpacket.vxd
	packet32.dll
	vpacket.sym ( optional with debug builds )

Select the Sample Packet Driver.

To install the protocol programmatically you can use the IPINST sample.

TESTING AND DEBUGGING

The Protocol includes debug statements in every entry point to study the flow of control.  
A Kernel level debugger is required to debug the network protocol.  Microsoft provides 
WDEB386.EXE and this tools is documented in the SDK.  Third party kernel level debuggers
are also available.
