#pragma once
#include <windows.h>
#include <string>

#ifndef UNICODE  
typedef std::string String;
#else
typedef std::wstring String;
#endif

class ClientNamedPipe
{
public:
	ClientNamedPipe(LPCTCH);
	BOOL WriteToServer(String);
	BOOL ReadFromServer(String&);
	~ClientNamedPipe();

private:
	HANDLE hPipe;
};

