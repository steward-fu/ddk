#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "console.h"

////////////////////////////////////////////////////////////////////////////////
//Console::Initialize
//
// Initializes the console
//
// Returns:
//  TRUE if the operation succeeded, FALSE otherwise.
//
BOOLEAN Console::Initialize()
{
	if(!SetupInputConsole())
	{
		CloseHandle(m_StdIn);
		return FALSE;
	}

	return SetupOutputConsole();
}

///////////////////////////////////////////////////////////////////////////////
//SetupOutputConsole
//
// Creates an output console for our use, and saves the state of the standard
// output console so it can be restored in the destructor.
//
// Returns:
//  TRUE if the operation succeeded, FALSE otherwise.
//
BOOLEAN Console::SetupOutputConsole()
{
	// Get the current screen buffer handle
	m_StdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (m_StdOut == INVALID_HANDLE_VALUE)
			return FALSE;

	// Create a screen buffer for our use
	m_ConsoleHandle = CreateConsoleScreenBuffer(
			GENERIC_WRITE | GENERIC_READ, // access flag
			0,						// buffer share mode
			NULL,					// pointer to security attributes
			CONSOLE_TEXTMODE_BUFFER, // type of buffer to create
			NULL					// reserved
			);
	if (m_ConsoleHandle == INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_StdOut);
		return FALSE;
	}

	// Make our console active
	if (!SetConsoleActiveScreenBuffer(m_ConsoleHandle))
	{
		CloseHandle(m_StdOut);
		CloseHandle(m_ConsoleHandle);
		return FALSE;
	}
	
	//Turn off line wrapping
	if(!SetConsoleMode(m_ConsoleHandle, 0))
	{
		SetConsoleActiveScreenBuffer(m_StdOut);
		CloseHandle(m_StdOut);
		CloseHandle(m_ConsoleHandle);
		return FALSE;
	}

	CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
	if (!GetConsoleScreenBufferInfo(m_ConsoleHandle, &ScreenBufferInfo)) 
	{
		SetConsoleActiveScreenBuffer(m_StdOut);
		CloseHandle(m_StdOut);
		CloseHandle(m_ConsoleHandle);
		return FALSE;
	}

	// Draw a line across the screen to separate the input and
	// output and set the initial cursor position at the start of the input section
	COORD DestinationCoordinate;
	
	DestinationCoordinate.X = 0;
	DestinationCoordinate.Y = ScreenBufferInfo.dwSize.Y - 7;
	
	ULONG nWrittenConsole;
	FillConsoleOutputCharacter(m_ConsoleHandle,
		'=', ScreenBufferInfo.dwSize.X, 
		DestinationCoordinate,
		&nWrittenConsole);
	
	DestinationCoordinate.Y = ScreenBufferInfo.dwSize.Y - 6;
	
	//Set the cursor position
	SetConsoleCursorPosition(m_ConsoleHandle,DestinationCoordinate);

	m_IsInitialized = TRUE;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//Console::SetupInputConsole
//
// Gets a handle to StdIn, and sets the input mode.  Called from Initialize
//
// Returns:
//  TRUE if the operation succeeded, FALSE otherwise.
//

BOOLEAN Console::SetupInputConsole()
{
	//Get the standard input handle
	m_StdIn = GetStdHandle(STD_INPUT_HANDLE);
	if (m_StdIn == INVALID_HANDLE_VALUE)
			return FALSE;

	// Save old console input mode 
	DWORD NewConsoleMode;

	if (!GetConsoleMode(m_StdIn, &m_OldConsoleMode)) 
	{
		CloseHandle(m_StdIn);
		return FALSE;
	}
	
	//Turn off line input and echo input modes
	NewConsoleMode = m_OldConsoleMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT); 

	if (!SetConsoleMode(m_StdIn, NewConsoleMode)) 
	{
		CloseHandle(m_StdIn);
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//Console::~Console
//
// Destructor.
//
// Cleans up the handles allocated in Initialize()
//
Console::~Console()
{
	if(m_IsInitialized)
	{
	// Return to the previous active screen buffer
	SetConsoleActiveScreenBuffer(m_StdOut);

	// Restore the console input mode
	SetConsoleMode(m_StdIn, m_OldConsoleMode);

	CloseHandle(m_StdOut);
	CloseHandle(m_StdIn);
	CloseHandle(m_ConsoleHandle);
	}
}

////////////////////////////////////////////////////////////////////////////////
//ProcessInput
//
//  Writes an input character to the screen, and saves it to a buffer until the
//   buffer is ready to be sent.
//
//  Parameters
//		InputCharacter - The character read from the keyboard
//		Buffer - Buffer for storing input characters
//      BufferIndex - index at which to write in buffer
//      BufferSize - size of the buffer in characters
//
//  Returns:
//    TRUE if the input character was enter, false otherwise.  TRUE signifies that
//      the buffer is ready to be sent.
//
//  Comments:
//    This function takes care of processing function keys such as shift or
//      backspace, as well as normal characters.
//

BOOLEAN Console::ProcessInput(CHAR InputCharacter, 
		PCHAR Buffer, 
		PULONG BufferIndex,
		ULONG BufferSize)
{
	// Assume the buffer is not ready to send
	BOOLEAN ReadyToSend = FALSE;
	
	// Get the screen buffer info
	CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
	GetConsoleScreenBufferInfo(m_ConsoleHandle, &ScreenBufferInfo);

	// Check first for a function key and then for the backspace key.  A function
	// key comes as a pair of characters with the first character always a 0 or -32.
    // Process only the function key delete and backspace
	if (m_LastCharacter == 0 || m_LastCharacter == -32 || InputCharacter == 8)
	{
		// Handle delete and backspace unless the buffer is empty, in which case
		// ignore them
		if ((InputCharacter == 83 || InputCharacter == 8) && *BufferIndex > 0)
		{
			if (ScreenBufferInfo.dwCursorPosition.X == 0 && 
				ScreenBufferInfo.dwCursorPosition.Y > 
				ScreenBufferInfo.dwSize.Y - 6)
			{
				ScreenBufferInfo.dwCursorPosition.X = ScreenBufferInfo.dwSize.X - 1;
				ScreenBufferInfo.dwCursorPosition.Y -= 1;
			}
			else if (ScreenBufferInfo.dwCursorPosition.X != 0)
			{
				ScreenBufferInfo.dwCursorPosition.X -= 1;
			}
			SetConsoleCursorPosition(m_ConsoleHandle, ScreenBufferInfo.dwCursorPosition);
			ULONG nWritten;
			FillConsoleOutputCharacter(m_ConsoleHandle, ' ', 1, ScreenBufferInfo.dwCursorPosition, &nWritten);

			// Back up one character in the buffer
			(*BufferIndex)--;
		}
	}
	// Handle carrige return and reaching the maximum Y coordinate
    else if (InputCharacter == 13 || ScreenBufferInfo.dwCursorPosition.X == ScreenBufferInfo.dwSize.X - 2)
	{
		// If this is a carrige return, we will send the buffer.
		if (InputCharacter == 13)
		{
			ReadyToSend = TRUE;
		}

		// Set the x coordinate back to zero
		ScreenBufferInfo.dwCursorPosition.X = 0; 
 
		// If it is the last line in the screen buffer, scroll
		// the buffer up.  We only scroll the lower 5 lines of the screen
		// buffer
		if ((ScreenBufferInfo.dwSize.Y-1) == ScreenBufferInfo.dwCursorPosition.Y)
		{ 

			SMALL_RECT ScrollRect;

			// The scrolling rectangle is the bottom 5 rows of the
			// screen buffer.
			ScrollRect.Top = ScreenBufferInfo.dwSize.Y - 6;
			ScrollRect.Bottom = ScreenBufferInfo.dwSize.Y - 1;
			ScrollRect.Left = 0;
			ScrollRect.Right = ScreenBufferInfo.dwSize.X - 1;
			
			COORD Coordinate;

			// The destination for the scroll rectangle is one row up.
			Coordinate.X = 0;
			Coordinate.Y = ScreenBufferInfo.dwSize.Y - 7;

			// The clipping rectangle is the same as the scrolling rectangle.
			// The destination row is left unchanged.
			SMALL_RECT ClipRect = ScrollRect;
		
			CHAR_INFO Fill;
			// Fill the bottom row with blanks
			Fill.Attributes = ScreenBufferInfo.wAttributes; 
			Fill.Char.AsciiChar = ' ';
			
			// Scroll up one line.
			ScrollConsoleScreenBuffer(m_ConsoleHandle, &ScrollRect, &ClipRect,
				Coordinate, &Fill);
			SetConsoleCursorPosition(m_ConsoleHandle, ScreenBufferInfo.dwCursorPosition);
		} 
		// Otherwise, advance the cursor to the next line.
		else
		{
			ScreenBufferInfo.dwCursorPosition.Y += 1;
			SetConsoleCursorPosition(m_ConsoleHandle, ScreenBufferInfo.dwCursorPosition);
		}

	} 

	// Handle all "normal" characters
    if (InputCharacter >= 32 && InputCharacter < 127)
	{
		// If there's still room in the buffer put the character in
		if (*BufferIndex < BufferSize - 1)
		{
			Buffer[(*BufferIndex)++] = InputCharacter;
			// Echo character to screen buffer
			ULONG nWritten;
			WriteFile(m_ConsoleHandle, &InputCharacter, 1, &nWritten, NULL);
		}
	}

	// Save the character for reference next time since function keys come in pairs
	// of characters and we want to filter them out
	m_LastCharacter = InputCharacter;

	return ReadyToSend;

}

///////////////////////////////////////////////////////////////////////////////
//Console::LogOutput
//
// Displays a buffer of output to the console
//
// Parameters:
//		bLocal - Specifies rather the message came from the local or remote machine
//            A different color is used for remote messages
//		Buffer - buffer containing the message
//      BufferSize - Length of buffer in characters
//
VOID Console::LogOutput(BOOL bLocal, PCHAR Buffer, ULONG BufferSize)
{
	// Get the current screen buffer info
	CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
	GetConsoleScreenBufferInfo(m_ConsoleHandle, &ScreenBufferInfo);

	// Save the old colors
	WORD OldAttributes;
	OldAttributes = ScreenBufferInfo.wAttributes;

	// Set the text color based on the local flag
	if (!bLocal)
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_GREEN);

	// Save the current cursor position and setup the logging cursor
	COORD CursorCoordinate;
	CursorCoordinate = ScreenBufferInfo.dwCursorPosition;

	COORD LogCoordinate;
	LogCoordinate.X = 0;
	LogCoordinate.Y = ScreenBufferInfo.dwSize.Y - 8;

	ULONG BufferIndex = 0;
	// Print out each line of the log message and scroll the log buffer up by one
	while (BufferIndex < BufferSize)
	{
		// Set the cursor to the log position
		SetConsoleCursorPosition(m_ConsoleHandle, LogCoordinate);

		// Scroll the log buffer up one line.  The scrolling rectangle is all
		// but the the bottom 6 rows of the screen buffer.
		SMALL_RECT ScrollRect;
		ScrollRect.Top = 0;
		ScrollRect.Bottom = ScreenBufferInfo.dwSize.Y - 8;
		ScrollRect.Left = 0;
		ScrollRect.Right = ScreenBufferInfo.dwSize.X - 1;
		
		// The destination for the scroll rectangle is one row up.
		COORD Coordinate;
		Coordinate.X = 0;
		Coordinate.Y = -1;

		// The clipping rectangle is the same as the scrolling rectangle.
		// The destination row is left unchanged.
		SMALL_RECT ClipRect = ScrollRect;
				
		// Fill the bottom row with blanks
		CHAR_INFO Fill;
		Fill.Attributes = ScreenBufferInfo.wAttributes; 
		Fill.Char.AsciiChar = ' ';
			
		// Scroll up one line.
		ScrollConsoleScreenBuffer(m_ConsoleHandle, &ScrollRect, &ClipRect,
			Coordinate, &Fill);

		// Determine how many characters to write
		ULONG nWrite;
		nWrite = min((ULONG)ScreenBufferInfo.dwSize.X, BufferSize - BufferIndex);
		
		// Write characters 
		ULONG nWritten;
		WriteFile(m_ConsoleHandle, Buffer + BufferIndex, nWrite, &nWritten, NULL);

		// Update index
		BufferIndex += nWrite;
	}

	// Change the text color back and restore the cursor position
	SetConsoleTextAttribute(m_ConsoleHandle, OldAttributes);
	SetConsoleCursorPosition(m_ConsoleHandle, CursorCoordinate);
}