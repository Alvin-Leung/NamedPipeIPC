#include "ClientNamedPipe.h"
#include <tchar.h>
#include <iostream>

int _tmain(void)
{
	std::cout << "Press enter to start the client" << std::endl;
	getchar();

	String stringToWrite = "Hello from the client";
	ClientNamedPipe* pNamedPipe;
	String stringFromServer = "HI";

	try
	{
		pNamedPipe = new ClientNamedPipe(TEXT("\\\\.\\pipe\\mynamedpipe"));
	}
	catch (const std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
		std::cout << "Press enter to exit the client." << std::endl;
		getchar();
		return 0;
	}
	
	std::cout << "Successfully opened named pipe" << std::endl;

	for (int i = 0; i < 1000; i++)
	{
		if (!pNamedPipe->WriteToServer(stringToWrite))
		{
			return 0;
		}

		if (!pNamedPipe->ReadFromServer(stringFromServer))
		{
			return 0;
		}

		std::cout << "[SERVER RESPONSE " << i << "] " << stringFromServer << std::endl;
	}

	std::cout << "Press enter to exit the client." << std::endl;
	getchar();

	delete pNamedPipe;

	return 0;
}