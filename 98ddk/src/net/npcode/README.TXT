This information in this article applies to 
--------------------------

Microsoft Windows 9x DDK

--------------------------

SUMMARY

In this sample we have tried to demonstrate how you could use the network provider to 
perform authentication of users logging on to the network.

MORE INFORMATION

The sample network provider needs to be set as a primary network provider, this will result in 
Win 95 or Win 98 calling the network provider for user credentials ( username and password).  
If the user information is not valid you can return a failure and thus stop MPR from calling 
other network providers thus restricting the access of the user to Windows 9x desktop. 
Currently the sample assumes a successful LOGON and no failure codes are returned.

All entry points of a Network provider are included in the sample. This was to 
illustrate the entry points that are  getting called for developers who want to 
to write a complete network provider to log into proprietary networks.

For a simple logon provider which verifies user credentials only NPgetCaps, NPLogon 
and NPLogoff are needed.


BUILDING NETWORK PROVIDER

Tools that you need to build MYNP.DLL
  Microsoft Visual C++
  Platform SDK
  Windows 9x DDK
The sample network provider can be built using the Micrsoft Visual C++ ver 5.0 IDE.  
The resulting DLL is MYNP.DLL

INSTALLATION

To install the network provider use Control Panel -- Network and choose Add clients.  
It should use the oemsetup.inf file provided in the project.  
To make the Network provider primary you can use the drop down box in the Control Panel 
Network.
To install the client programmatically you can use the IPINST sample available on the 
support web site.  

TESTING AND DEBUGGING

The network provider includes debug statements in every entry point to study the flow 
of entry points that are called in the network provider.  To debug the network provider
one has to use Windows Kernel dubugger ( WDEB386.EXE ) which is documented in the SDK.  
The Visual C++ debugger will not serve the process because the DLL is loaded at boot time

