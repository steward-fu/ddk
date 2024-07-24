rem "nanoKONTROL 1 CTRL.bas"

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
' that must occur before we accept any MIDI messages from the nanoKontrol.


noChange       = 0
faderChanged   = 1
knobChanged    = 2
buttonChanged  = 3


nkNumFaders  =  9	: ' nk1 has nine faders
nkNumKnobs   =  9	: ' nk1 has nine knobs
nkNumBtns	 = 24	: ' nk1 has 24 buttons


' On entry, @numFaders, @numKnobs, and @numButtons contain
' the starting indexes for the faders, knobs, and buttons
' that this program will supply to the HLA code. Because
' there could be multiple control surfaces attached to the
' HLA program, we cannot blindly assume a starting index
' of zero.

faderStart = @numFaders
faderEnd   = @numFaders+nkNumFaders-1
knobStart  = @numKnobs
knobEnd    = @numKnobs+nkNumKnobs-1
btnStart   = @numButtons
btnEnd     = @numButtons+nkNumBtns-1

@numFaders  = @numFaders  + nkNumFaders
@numKnobs   = @numKnobs   + nkNumKnobs
@numButtons = @numButtons + nkNumBtns


for i = faderStart to faderEnd

	@faders(i) = 0
	
next i
print "KORG nanoKontrol 1 CTRL #"; @midiDevice; "; faders: "; faderStart; " to "; faderEnd

for i = knobStart to knobEnd

	@knobs(i) = 0
	
next i
print "KORG nanoKontrol 1 CTRL #"; @midiDevice; "; knobs: "; knobStart; " to "; knobEnd

for i = btnStart to btnEnd

	@buttons(i) = 0
	
next i
print "KORG nanoKontrol 1 CTRL #"; @midiDevice; "; buttons: "; btnStart; " to "; btnEnd



' Initialization is complete. Now this program suspends and waits for the
' HLA program to call us when a MIDI message is received from the nanoKontrol.
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
	
		' The nanoKontrol returns 7-bit values. We really want 8-bit values.
		' We will multiply the values we get by two (and add one if it's $7f)
		' so that we get the full range.
		
		if( @midiParm >= 0 and @midiParm <= 8 ) then
		
			index = knobStart + @midiParm			
			
			' Tell the HLA program that a knob has changed:
			
			@deviceChanged = knobChanged
			
			' Tell the HLA program which knob has changed:
			
			@changedIndex  = index + 4096 * @midiChannel
			
			' Merge the bits and place them in the global location for
			' use by the HLA program:
			
			value = @midiValue * 2
			if( value = 254 ) then
			
				value = 255
				
			endif
			@knobs( index ) = value
			
		elseif( @midiParm >= 9 and @midiParm <= 17 ) then
		
			' Okay, this byte is the fader index (0-7)
			
			
			index = faderStart + @midiParm - 9
			
			' Tell the HLA program that a fader has changed:
			
			@deviceChanged   = faderChanged
			
			' Tell the HLA program which fader has changed:
			
			@changedIndex    =  index + 4096 * @midiChannel
			
			' Set the 8-bit fader value in the global location for
			' use by the HLA program:
			
			value = @midiValue * 2
			if( value = 254 ) then
			
				value = 255
				
			endif
			@faders( index ) = value
			 
			
		
		' The buttons come in as control change commands 18..41:
		
		elseif( @midiParm >= 18 and @midiParm <= 41 ) then
		
			index = btnStart + @midiParm - 18
			
			' Tell the HLA program that a button has changed:
			
			@deviceChanged = buttonChanged
			
			' Tell the HLA program which button has changed:
			
			@changedIndex = index + 4096 * @midiChannel
			
			' Set the global button value for use by the HLA program:
			
			@buttons( index ) = @midiValue 
			
		endif
	
	endif
	
	
goto forever

