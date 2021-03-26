#include <afx.h>
#include <iostream>
#include <winsock.h>
#pragma comment (lib,"ws2_32")

#define  CHUNK_SIZE (1024*64)

using namespace std;

char FilePath[MAX_PATH];



int main()
{
	cout << "\t\tClient-[RESUME BROKEN TRANSFER]" << "\t  author: ColdWind\n\n" << "Please enter the path to save the file eg: C:\\Save.RAR \n\n" << "Path:  ";

	cin >> FilePath;

	WSADATA wsadata;
	SOCKET client;
	SOCKADDR_IN serveraddr;
	int port = 5555;

	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &wsadata);

	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if (connect(client, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == INVALID_SOCKET)
	{
		cout << "Connect Server Error" << endl;
		return 0;
	}
	int FileLen = 0;

	int nCurrentPos = 0;

	UINT OpenFlags;

	/*CFile PosFile;
	if (PosFile.Open("D:\\PosFile", CFile::modeRead | CFile::typeBinary))
	{
		PosFile.Read((char*)&nCurrentPos, sizeof(nCurrentPos));
		cout << "The File Pos is " << nCurrentPos << "\n";
		nCurrentPos = nCurrentPos + 1;
		PosFile.Close();
		send(client, (char*)&nCurrentPos, sizeof(nCurrentPos), 0);
		OpenFlags = CFile::modeWrite | CFile::typeBinary;
	}
	else
	{
		send(client, (char*)&nCurrentPos, sizeof(nCurrentPos), 0);
		OpenFlags = CFile::modeWrite | CFile::typeBinary | CFile::modeCreate;
	}*/

	CFile file;
	DWORD dwAttrib = GetFileAttributes(FilePath);
	int ret = 0;
	if (INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		ret = file.Open(FilePath, CFile::modeWrite | CFile::typeBinary);
		int FileLen = file.GetLength();
		nCurrentPos = FileLen / CHUNK_SIZE + 1;
		cout << "The File Pos is " << nCurrentPos << "\n";
		send(client, (char*)&nCurrentPos, sizeof(nCurrentPos), 0);
	}
	else
	{
		ret = file.Open(FilePath, CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
		send(client, (char*)&nCurrentPos, sizeof(nCurrentPos), 0);
	}

	if (recv(client, (char*)&FileLen, sizeof(FileLen), 0) != 0)
	{
		int		nChunkCount;
		/*CFile	file;*/
		nChunkCount = FileLen / CHUNK_SIZE;
		if (nChunkCount == 0 || FileLen%nChunkCount != 0)
		{
			nChunkCount++;
		}

		if (ret)
		{
			file.Seek(nCurrentPos*CHUNK_SIZE, CFile::begin);

			cout << "file seek is " << nCurrentPos*CHUNK_SIZE << "\n";

			char *date = new char[CHUNK_SIZE];

			for (int i = nCurrentPos; i<nChunkCount; i++)
			{

				cout << "Recv The Chunk is " << i << "\n";

				int nLeft;

				if (i + 1 == nChunkCount)
					nLeft = FileLen - CHUNK_SIZE*(nChunkCount - 1);
				else
					nLeft = CHUNK_SIZE;

				int idx = 0;

				while (nLeft>0)
				{
					int ret = recv(client, &date[idx], nLeft, 0);

					if (ret == SOCKET_ERROR)
					{
						cout << "Recv Date Error" << endl;
						return 0;
					}
					idx += ret;
					nLeft -= ret;
				}
				// file.Write(date, CHUNK_SIZE);
				file.Write(date, idx);

				/*CFile	PosFile;
				int seekpos = i + 1;
				if (PosFile.Open("D:\\PosFile", CFile::modeWrite | CFile::typeBinary | CFile::modeCreate));
				{
					PosFile.Write((char*)&seekpos, sizeof(seekpos));
					PosFile.Close();
				}*/
			}
			file.Close();
			delete[] date;
		}
		/*if (DeleteFile("d:\\PosFile") != 0)
		{
			cout << "File Transfer Complete" << endl;
			getchar();
		}*/
	}

	return 0;
}
