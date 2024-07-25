readme.txt - kbfiltr Sample
Copyright (c) 2000 Compuware Corporation

This is a demonstration of STL code in a driver that uses none of the
classes of DriverWorks. It is based on the kbfiltr sample from the
Windows 2000 DDK (src\input\kbfiltr).

After the driver is built and instlled, whenever the Escape key is
pressed, this module will emit to the debugger (or to the output
window of DriverWorkbench) a sorted list of all the strings that
have been typed since boot up. The display will also show the
number of times that each of the strings has been typed. For
purposes of this demonstration, strings are considered to be
consecutive letters, and the driver does not monitor the
shift/control/alt states.
