Readme.txt - DriverWorks AMCC S5933 example
Copyright (c) 1999 Compuware Corporation

AMCC S5933 Exmaple Drivers for Windows NT 4.0 and WDM

	This is an example of a driver that uses the DriverWorks built in
	KS59XXPci classes to create a driver for a device built using the
	AMCC S5933  Matchmaker PCI Controller.  This sample is designed to
	be used with the AMCC DK1 Development Kit, which is available from
	AMCC.

	The DK1 Development Kit contains two boards.  One is a PCI board
	containing the AMCC S5933, and the other is an ISA board that
	attaches to the first using an over the top ribbon cable.  This
	second board allows access to the ADDON side of the PCI Controller
	chipset.  This example therefore contains two drivers, one for the
	PCI device and one for the ISA device.  The PCI example driver is
	found in the subdirectory S5933.  This example consists of a device
	driver and a console test application program to exercise the AMCC
	S5933 device. The driver is written to compile either as a NT 4.0
	style driver, or a WDM style driver. The ISA example driver is
	contained in the subdirectory S5933Isa, and also contains a driver
	and a console test application.  This driver is written in NT 4.0
	style.  The two drivers are designed to work together.  Each can
	also perform a subset of their funcitons when used independently.

	To install the drivers follow the methods normally employed for the
	style driver you want to test.
	
	For NT 4.0 style drivers:
	1. Use regini from the DDK to install the .ini
		settings in the registry
	2. Start the driver using NET START DriverName, or
		use the Monitor program

	For WDM style drivers
	1. Invoke a search for Plug and Play devices after removing any
		nonfunctioning placeholder in the DeviceManager
	2. Navigate to the .inf file when propmted and then the .sys file when
		prompted
	
	The console test applications provide several functions to exercise
	the AMCC S5933 device.  For a list of available functions, run the
	test application with no parameters or with -?.  The drivers must be
	started for the test programs to run.

	The ISA driver as stated above is written from the ADDON perspective
	of the S5933 chip, and uses polling to perform such operations as
	reading and writing to service DMA transactions from the PCI side.
	Because of this, when performing a test, it is necessary to first
	invoke the operation from the PCI side and then perform the
	corresponding action on the ISA side, at which time the action will
	occur. Failure to do this will result in the ISA drivers timing out
	while waiting for data to become available.


	Notes:

	There are several registry parameters that control the operation of
	the driver. These can be found at
	HKLM->CurrentControlSet->Services->S5933->Parameters

		BreakOnEntry - Causes the driver to halt in driver entry by
			calling INT3
		VendorID - The vendor ID the board is using
		DeviceID - The device ID the board is using
		MaxDmaReadLength - The max read block size set on the DMA
			Adapter object
		MaxDmaWriteLength - The max write block size set on the DMA
			Adapter	object
		NvramIsSerial - 1 if using serial NVRAM, 0 if using parallel
			NVRAM
		NvramPageSize - Page size of parallel NVRAM

	The S5933DK1 kit comes setup with the BARs all requesting some form
	of I/O or memory range. One BAR is setup to request memory below
	1MB.  This may cause a Pnp configuration error on some machines,
	resulting in the  S5933 driver not being able to load.  One failure
	code observed is STATUS_CONFLICTING_ADDRESSES 0xC0000018.  To fix
	this the NVRAM must be reprogrammed using the NVBUILD utility
	provided with the AMCC kit.  It may not even be possible to run this
	program on a PC that exhibits the error, and an alternate PC must be
	used.

	The S5933DK1 kit may come with the serial NVRAM write protected.
	This can be verified if writing to the serial NVRAM fails using the
	NVBUILD utility provided by AMCC or using the S5933 driver.  To
	unprotect the NVRAM, lift pin 7, labeled TEST on the S5933 PCI Card
	schematic.

	The ISA card provided with the S5933DK1 is fixed at I/O address
	300-33f, which often conflicts with built in soundcards.  Disabling
	on-board sound or re-configuring a board that conflicts with this
	address may be necessary to get this card functioning.

	Don't forget that to write to the NVRAM, the registry key
	NvramIsSerial must be set to 1 to program a the 24C16 2Kx8 Serial
	EEPROM or to 0 to program the 29C512 64Kx8 Parallel flash memory.
	Additionally, the NvramPageSize key must be 0x80 for the 29C512 (it
	is a don't care for the 24C16).
	        
