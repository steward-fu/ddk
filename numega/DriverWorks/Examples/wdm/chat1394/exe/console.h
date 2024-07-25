class Console
{

public:
	Console():m_IsInitialized(FALSE),
		m_LastCharacter(' '){}
	BOOLEAN Initialize();
	~Console();
	BOOLEAN ProcessInput(CHAR InputCharacter, 
		PCHAR Buffer, 
		PULONG BufferIndex,
		ULONG BufferSize);
	VOID LogOutput(BOOL bLocal, PCHAR Buffer, ULONG BufferSize);
protected:
	BOOLEAN SetupOutputConsole();
	BOOLEAN SetupInputConsole();
protected:
	HANDLE m_StdIn, m_StdOut, m_ConsoleHandle;
	DWORD  m_OldConsoleMode;
	BOOLEAN m_IsInitialized;
	CHAR   m_LastCharacter;
};