CPLSVR1 - Sample Game Controllers Property Sheet
================================================

This is a sample Game Controllers device property sheet extension.  It is
launched by the new Game Controllers control panel applet when the user clicks
the "Properties..." button for a selected device.

The code provided with this sample demonstrates how to create a multi-page
property sheet dialog using the IDIGameCntrlPropSheet interface.  Additionally,
each page provides information about the configured device obtained via the
IDirectInputJoyConfig and IDirectInputDevice2 interfaces.

To install the sample extension, right click on the provided .INF file
(CPLSVR1.INF) and select install.  This will register the DLL's CLSID and make
it available to the Game Controllers applet and also register a new joystick
type.

Select either "CPLSVR1 (2-axis, 4-button)" or "CPLSVR1 Sample (2-axis,
4-button, has effects)" as your joystick to view the extension DLL.  NOTE: be
sure that you only select one of these "joysticks" when you are viewing the
extension DLL, and NOT when you are using the joystick for normal operation.

Before you can run the DLL, you will need to either 1) build the sample code or
2) copy the file CPLSVR1.DLL to your %WINDIR%\SYSTEM directory.

The sample code files are listed below with a brief description of what they
contain:

CPLSVR1.CPP     - implementation of the interface classes.  contains a derived
                  class CDIGameCntrlPropSheet_X that extends the property sheet
                  interface to provide additional services to the extension
                  DLL.
PAGES.CPP       - implementation of the property sheet page dialog proceedures.
                  also contains functions that interact with the dialog box
                  controls.
DICPUTIL.CPP    - a handful of functions that get information from DirectInput.
