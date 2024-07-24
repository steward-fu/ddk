slights:

This example program demonstrates multimedia/MIDI programming
as well as incorporating HLA Basic into an HLA program.

This program allows the user to connect MIDI/USB "control surfaces"
to the computer. It reads the inputs on those control surfaces and
displays the data in a console window whenever one of the input
devices on these control surfaces changes.

Currently, this demo supports two different control surfaces:
1) Behringer BCF2000
2) KORG nanoControl 1 SLIDER/KNOB

You must have the appropriate drivers installed.
Also, this program expects these devices to return certain
values for each of the controls,  I've included the
sysex file for the BCF2000 and the nktrl_data file for
the KORG nanoKontrol. You'll have to use their respective
editor programs (or MIDI program) to send this data to those
devices.

Here's some sample output from the program running on
my machine:

G:\hla\slights>slights
Loading: 'midiIn/nanoKONTROL 1 SLIDER/KNOB.hbbc' as MIDI #0
KORG nanoKontrol 1 CTRL #0; faders: 0 to 8
KORG nanoKontrol 1 CTRL #0; knobs: 0 to 8
KORG nanoKontrol 1 CTRL #0; buttons: 0 to 23
Loading: 'midiIn/BCF2000.hbbc' as MIDI #1
Behringer BCF2000 #1; faders: 9 to 16
Behringer BCF2000 #1; knobs: 9 to 40
Behringer BCF2000 #1; buttons: 24 to 75
Fader[9] = 1
Fader[9] = 3
Fader[9] = 4
Fader[9] = 6
Fader[9] = 9
Fader[9] = 13
Fader[9] = 20

Note that I moved fader #1 on the Behringer (Fader[9] in the
program) to produce this output.

