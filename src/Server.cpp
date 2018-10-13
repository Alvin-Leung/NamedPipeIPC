#include "ServerNamedPipe.h"
#include <windows.h> 
#include <tchar.h>
#include <iostream>
#include <thread>
#include <vector>

void CommunicateWithClient(ServerNamedPipe*);
String GetReplyFromRequest(String);

int _tmain(void)
{
	ServerNamedPipe* pNamedPipe;
	BOOL isConnected = FALSE;
	std::vector<std::thread*> pipeThreads;

	for (;;)
	{
		try
		{
			pNamedPipe = new ServerNamedPipe(TEXT("\\\\.\\pipe\\mynamedpipe"));
		}
		catch (const std::runtime_error e)
		{
			std::cout << e.what() << std::endl;
			break;
		}
		
		isConnected = pNamedPipe->WaitForConnectingClients();

		if (isConnected)
		{
			std::cout << "Client successfully connected. Creating new thread for IPC." << std::endl;

			pipeThreads.push_back(new std::thread(CommunicateWithClient, pNamedPipe));
		}
		else
		{
			delete pNamedPipe;
		}
	}

	for (std::thread* pipeThread : pipeThreads)
	{
		pipeThread->join();
		delete pipeThread;
	}

	return 0;
}

void CommunicateWithClient(ServerNamedPipe* pNamedPipe)
{
	String requestFromClient;
	String replyToClient;

	while (1)
	{
		if (!pNamedPipe->ReadFromClient(requestFromClient)) break;

		replyToClient = GetReplyFromRequest(requestFromClient);

		if (!pNamedPipe->WriteToClient(replyToClient)) break;
	}

	delete pNamedPipe;

	std::cout << "Thread exiting..." << std::endl;
	return;
}

String GetReplyFromRequest(String request)
{
	std::cout << "Client Request String: " << request << std::endl;

	return String("Hello from the server");
}