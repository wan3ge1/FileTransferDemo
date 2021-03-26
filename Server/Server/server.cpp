#include<afx.h>
#include <iostream>
#include <winsock.h>
#pragma comment (lib,"ws2_32")
#define  CHUNK_SIZE (1024*64)

using namespace std;

char FilePath[MAX_PATH];

DWORD  GetFileProc(int nCurrentPos, SOCKET client);

int main()
{
	cout << "\t\tServer-[RESUME BROKEN TRANSFER]" << "\t  author: ColdWind\n\n" << "Please enter the file path to be downloaded eg: C:\\File.rar \n\n" << "Path:  ";

	cin >> FilePath;

	WSADATA wsadata;
	SOCKET server;
	SOCKET client;
	SOCKADDR_IN serveraddr;
	SOCKADDR_IN clientaddr;
	int port = 5555;

	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &wsadata);

	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	bind(server, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	listen(server, 5);

	int len = sizeof(clientaddr);


	while (true)
	{
		if (client = accept(server, (sockaddr *)&clientaddr, &len))
		{
			cout << "have one connect\n";
			int nCurrentPos = 0;

			if (recv(client, (char*)&nCurrentPos, sizeof(nCurrentPos), 0) == SOCKET_ERROR)
			{
				cout << "The Clinet Socket is Closed\n";
				break;
			}
			else
			{
				cout << "the currentpos is the" << nCurrentPos << "\n";
				GetFileProc(nCurrentPos, client);
			}
		}
	}

	closesocket(server);
	closesocket(client);
	WSACleanup();
	return 0;
}

DWORD  GetFileProc(int nCurrentPos, SOCKET client)
{
	cout << "get file proc is ok\n";

	CFile	file;
	int		nChunkCount = 0;

	if (file.Open(FilePath, CFile::modeRead | CFile::typeBinary))
	{

		if (nCurrentPos != 0)
		{
			file.Seek(nCurrentPos*CHUNK_SIZE, CFile::begin);

			cout << "file seek is " << nCurrentPos*CHUNK_SIZE << "\n";
		}


		int FileLen = file.GetLength();

		nChunkCount = FileLen / CHUNK_SIZE;

		if (nChunkCount == 0 || FileLen%nChunkCount != 0)
			nChunkCount++;

		send(client, (char*)&FileLen, sizeof(FileLen), 0);

		char *data = new char[CHUNK_SIZE];



		for (int i = nCurrentPos; i<nChunkCount; i++)
		{

			cout << "send the count" << i << "\n";
			int nLeft;

			if (i + 1 == nChunkCount)
				nLeft = FileLen - CHUNK_SIZE*(nChunkCount - 1);
			else
				nLeft = CHUNK_SIZE;

			int idx = 0;
			file.Read(data, CHUNK_SIZE);

			while (nLeft>0)
			{
				int ret = send(client, &data[idx], nLeft, 0);
				if (ret == SOCKET_ERROR)
				{
					cout << "Send The Date Error \n";
					break;
				}
				nLeft -= ret;
				idx += ret;
			}
		}
		file.Close();
		delete[] data;

	}
	else
	{
		cout << "open the file error\n";
	}
	return 0;
}
