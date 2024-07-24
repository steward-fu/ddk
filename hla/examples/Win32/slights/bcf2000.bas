rem BCF2000.bas

' Important global pseudo-variables used to communicate data between
' the HLA main program and this BASIC code:
'
'	@midiDevice		- The device code for this particular device
'	@midiMsg		- The MIDI message code from Windows
'	@midiStatus		- The actual MIDI command ($B0 -- change control)
'	@midiChannel	- The MIDI channel number
'	@midiParm		- The control code data byte (fader/knob/btn # on the device)
'	@midiValue		- The MIDI value for the particular device.
'	@midiTimeStamp	- MIDI time stamp.
'
'	@numFaders		- # of faders consumed so far by the HLA program
'	@faders[4096]	- Array of fader values across all devices
'	@numKnobs		- # of knobs consumed so far by the HLA program
'	@knobs[4096]	- Array of knob values across all devices
'	@numButtons		- # of buttons consumed so far by the HLA program
'	@buttons[4096]	- Array of button values across all devices
'
'	@deviceChanged	- Set to non-zero value to tell HLA program that some
'						device has changed its value.
'	@changedIndex	- Set to the index into the @faders, @knobs, or @buttons
'						array to tell the HLA program which device has
'						changed.



' The following initialization code runs when the S'Lights program
' loads the BASIC code into memory. This is important initialization
' that must occur before we accept any MIDI messages from the BCF2000.


noChange       = 0
faderChanged   = 1
knobChanged    = 2
buttonChanged  = 3

nrpnLSB        = 98
nrpnMSB        = 99

bcfNumFaders  =  8	: ' BCF2000 has eight faders
bcfNumKnobs   = 32	: ' BCF2000 has four groups of eight knobs

' The BCF2000 has 32 buttons on the rotary knobs (four groups of eight buttons)
' plus an additional 20 buttons. This code does not use the edit/function
' buttons or the present buttons.

bcfFirstBtn   = 64
bcfLastBtn    = 115
bcfNumButtons = bcfLastBtn - bcfFirstBtn + 1

' On entry, @numFaders, @numKnobs, and @numButtons contain
' the starting indexes for the faders, knobs, and buttons
' that this program will supply to the HLA code. Because
' there could be multiple control surfaces attached to the
' HLA program, we cannot blindly assume a starting index
' of zero.

faderStart = @numFaders
faderEnd   = @numFaders+bcfNumFaders-1
knobStart  = @numKnobs
knobEnd    = @numKnobs+bcfNumKnobs-1
btnStart   = @numButtons
btnEnd     = @numButtons+bcfNumButtons-1

@numFaders  = @numFaders  + bcfNumFaders
@numKnobs   = @numKnobs   + bcfNumKnobs
@numButtons = @numButtons + bcfNumButtons


for i = faderStart to faderEnd

	@faders(i) = 0
	
next i
print "Behringer BCF2000 #"; @midiDevice; "; faders: "; faderStart; " to "; faderEnd

for i = knobStart to knobEnd

	@knobs(i) = 0
	
next i
print "Behringer BCF2000 #"; @midiDevice; "; knobs: "; knobStart; " to "; knobEnd

for i = btnStart to btnEnd

	@buttons(i) = 0
	
next i
print "Behringer BCF2000 #"; @midiDevice; "; buttons: "; btnStart; " to "; btnEnd



' Initialization is complete. Now this program suspends and waits for the
' HLA program to call us when a MIDI message is received from the BCF2000.
' The following loop executes once for each such command.

forever:

	' Wait for a MIDI command to arrive from the HLA program.
	
	suspend
	
	' Initialize the message values that the HLA program will check
	' upon our return:
	
	@deviceChanged = noChange
	@changedIndex = 0
	
	' At this point, the following pseudo-variables are available for use:
	'
	'	@midiStatus - 	Will contain a control change command ($B0)
	'	@midiChannel-	Will contain the channel number.
	'	@midiParm-		Will contain the change control code (btn/knob/fader #)
	'	@midiValue-		Will contain the value read from the device.
	
	' All commands come in as Control Change commands.
	
	if( @midiStatus = $b0 ) then
	
		' The faders come in as NRPN control change commands.
		' This is a four-message transmission:
		'	1)	Tells us this is an NRPM command (fader value)
		'	2)	@midiValue contains the fader # (0-7)
		'	3)	@midiValue contains the H.O. bit (bit #7) of the value.
		'	4)	@midiValue contains the L.O. seven bits of the value.
		
		if( @midiParm = nrpnMSB ) then
		
			' First byte (which we just received) contains the
			' H.O. byte of the fader address. It's always zero,
			' so just ignore it and wait for the next byte to arrive.
			
			suspend	: ' Get message #2
			
			' Okay, this byte is the fader index (0-7)
			
			
			index = @midiValue + faderStart
			
			' The next byte we receive is the most significant 7 bits
			' of the 14-bit value (we're only using eight bits, though).
			
			suspend : ' Get message #3
			
			MSB = @midiValue
			
			' The next byte is the least signficant 7 bits of the
			' 14-bit value.
			
			suspend	: ' Get message #4
			
			' Tell the HLA program that a fader has changed:
			
			@deviceChanged   = faderChanged
			
			' Tell the HLA program which fader has changed:
			
			@changedIndex    =  index + 4096 * @midiChannel
			
			' Set the 8-bit fader value in the global location for
			' use by the HLA program:
			
			@faders( index ) = @midiValue + (MSB * 128)
			 
			 
			 
		' The knobs come in as control change commands 0..31.
		' Because we're reading a full 8 bits (0..255) we have
		' to read two bytes: the first contains bit #7, the
		' second byte contains bits 0..6.
		'
		'	Knob readings require two midi messages:
		'		(1)	Message data payload contains H.O. bit (#7) of knob value.
		'		(2) Message data payload contains L.O. seven bits of knob value. 
		
		elseif( @midiParm >= 0 and @midiParm <= 31 ) then
		
			' Save the MSbit and the knob index:
			
			index = knobStart + @midiParm
			MSB = @midiValue
			
			' Get the second message:
			
			suspend
			
			
			' Tell the HLA program that a knob has changed:
			
			@deviceChanged = knobChanged
			
			' Tell the HLA program which knob has changed:
			
			@changedIndex  = index + 4096 * @midiChannel
			
			' Merge the bits and place them in the global location for
			' use by the HLA program:
			
			@knobs( index ) = @midiValue + (MSB * 128)
			
		
		' The buttons come in as control change commands 64..115:
		
		elseif( @midiParm >= bcfFirstBtn and @midiParm <= bcfLastBtn ) then
		
			' No need to read an extra byte, we only need one bit!
			
			index = btnStart + @midiParm - bcfFirstBtn
			
			' Tell the HLA program that a button has changed:
			
			@deviceChanged = buttonChanged
			
			' Tell the HLA program which button has changed:
			
			@changedIndex = index + 4096 * @midiChannel
			
			' Set the global button value for use by the HLA program:
			
			@buttons( index ) = @midiValue 
			
		endif
	
	endif
	
	
goto forever

