This directory contains source code for a skeleton PCI miniport driver, EXPCIMP.SYS. As a skeleton, the driver does not control any physical hardware, but serves as a framework on which a working driver can be built.

EXPCIMP.SYS is actually an aggregate driver made up of GART, IRQ, and IDE miniport drivers. Only the GART portion is documented in the DDK, since the other components are not generally implemented by IHVs.

The driver can be built using the standard BUILD utility supplied with the DDK. To install the GART portion of the driver, copy the EXPCIMP.SYS file in the system directory, and insert the following entries into the registry:

HKLM,System\CurrentControlSet\Services\VxD\PCI\GARTMiniports\EXPCIMP\Name = "Example PCI Miniport Driver"

HKLM,System\CurrentControlSet\Services\VxD\PCI\GARTMiniports\EXPCIMP\Path = <path to EXPCIMP.SYS>

HKLM,System\CurrentControlSet\Services\VxD\PCI\GARTMiniports\EXPCIMP\Instance = 1


