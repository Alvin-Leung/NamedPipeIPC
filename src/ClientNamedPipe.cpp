#include "ClientNamedPipe.h"
#include <tchar.h>
#include <atlstr.h>
#include <iostream>

#define SYNCHRONOUS_IO NULL
#define BUFSIZE 1024

ClientNamedPipe::ClientNamedPipe(LPCTCH pPipename)
{
	DWORD dwMode = PIPE_READMODE_MESSAGE;

	while (TRUE)
	{
		this->hPipe = CreateFile(
			pPipename,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if (this->hPipe != INVALID_HANDLE_VALUE)
			break;

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			throw std::runtime_error("Could not open pipe with error code " + std::to_string(GetLastError()));
		}

		if (!WaitNamedPipe(pPipename, 10000))
		{
			throw std::runtime_error("Waiting for pipe timed out after 10 seconds. Error code " + std::to_string(GetLastError()));
		}
	}

	std::cout << "Opened pipe: " + std::to_string(GetLastError()) << std::endl;

	if (!SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL))
	{
		throw std::runtime_error("Setting named pipe handle state failed. Error code " + std::to_string(GetLastError()));
	}
}

BOOL ClientNamedPipe::WriteToServer(String stringToServer)
{
	TCHAR pWriteBuffer[BUFSIZE];
	DWORD numberOfBytesWritten;
	DWORD numberOfBytesToWrite;

	_tcscpy_s(pWriteBuffer, CA2T(stringToServer.c_str()));
	numberOfBytesToWrite = (lstrlen(pWriteBuffer) + 1) * sizeof(TCHAR);

	BOOL isWriteSuccessful = WriteFile(
		hPipe,
		pWriteBuffer,
		numberOfBytesToWrite,
		&numberOfBytesWritten,
		SYNCHRONOUS_IO
	);

	if (isWriteSuccessful == FALSE)
	{
		std::cout << "Write to server failed with error code " << GetLastError() << std::endl;
	}
	
	return isWriteSuccessful;
}

BOOL ClientNamedPipe::ReadFromServer(String &stringFromServer)
{
	DWORD numberOfBytesRead;
	BOOL isReadSuccessful;
	TCHAR pReadBuffer[BUFSIZE];
	DWORD numberOfBytesToRead = BUFSIZE * sizeof(TCHAR);

	do
	{
		isReadSuccessful = ReadFile(
			hPipe,
			pReadBuffer,
			numberOfBytesToRead,
			&numberOfBytesRead,
			SYNCHRONOUS_IO
		);

		if (!isReadSuccessful && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!isReadSuccessful);

	if (isReadSuccessful == FALSE)
	{
		std::cout << "Read from server failed with error code " << GetLastError() << std::endl;
	}

	stringFromServer = String(pReadBuffer);

	return isReadSuccessful;
}

ClientNamedPipe::~ClientNamedPipe()
{	
	CloseHandle(this->hPipe);
}
