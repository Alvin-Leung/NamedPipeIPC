#pragma once
#include <windows.h>
#include <string>

#ifndef UNICODE  
typedef std::string String;
#else
typedef std::wstring String;
#endif

class ServerNamedPipe
{
public:
	ServerNamedPipe(LPCTSTR);
	BOOL WaitForConnectingClients();
	BOOL ReadFromClient(String&);
	BOOL WriteToClient(String);
	~ServerNamedPipe();

private:
	HANDLE hPipe;
};

