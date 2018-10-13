#include "ServerNamedPipe.h"
#include <atlstr.h>
#include <iostream>

#define SYNCHRONOUS_IO NULL
#define BUFSIZE 1024

ServerNamedPipe::ServerNamedPipe(LPCTSTR pPipename)
{
	this->hPipe = CreateNamedPipe(
		pPipename,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 
		BUFSIZE,
		BUFSIZE,
		0,
		NULL);

	if (this->hPipe == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error("Could not open pipe. Error code " + std::to_string(GetLastError()));
	}
}

BOOL ServerNamedPipe::WaitForConnectingClients()
{
	std::cout << "Server named pipe waiting for connecting client..." << std::endl;
	
	BOOL isSuccessful = ConnectNamedPipe(this->hPipe, NULL);

	if (isSuccessful)
	{
		return TRUE;
	}
	else
	{
		return GetLastError() == ERROR_PIPE_CONNECTED;
	}
}

BOOL ServerNamedPipe::ReadFromClient(String& stringFromClient)
{
	TCHAR pReadBuffer[BUFSIZE];
	DWORD numberOfBytesToRead = BUFSIZE * sizeof(TCHAR);
	DWORD numberOfBytesRead;
	BOOL isReadSuccessful;
	
	BOOL fSuccess = ReadFile(
		this->hPipe,
		pReadBuffer,
		numberOfBytesToRead,
		&numberOfBytesRead,
		SYNCHRONOUS_IO);

	if (!fSuccess || numberOfBytesRead == 0)
	{
		if (GetLastError() == ERROR_BROKEN_PIPE)
		{
			std::cout << "Client has disconnected" << std::endl;
		}
		else
		{
			std::cout << "ERROR: ReadFromClient failed with error code " << GetLastError() << std::endl;
		}
	}

	stringFromClient = String(pReadBuffer);

	return fSuccess;
}

BOOL ServerNamedPipe::WriteToClient(String stringToClient)
{
	TCHAR pWriteBuffer[BUFSIZE];
	DWORD numberOfBytesWritten;
	DWORD numberOfBytesToWrite;
	
	_tcscpy_s(pWriteBuffer, CA2T(stringToClient.c_str()));
	numberOfBytesToWrite = (lstrlen(pWriteBuffer) + 1) * sizeof(TCHAR);

	BOOL fSuccess = WriteFile(
		this->hPipe,
		pWriteBuffer,
		numberOfBytesToWrite,
		&numberOfBytesWritten,
		SYNCHRONOUS_IO);

	if (!fSuccess || numberOfBytesToWrite != numberOfBytesWritten)
	{
		std::cout << "ERROR: WriteToClient failed with error code " << GetLastError() << std::endl;
	}

	return fSuccess;
}

ServerNamedPipe::~ServerNamedPipe()
{
	FlushFileBuffers(this->hPipe);
	DisconnectNamedPipe(this->hPipe);
	CloseHandle(this->hPipe);
}
