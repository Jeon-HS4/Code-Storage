#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 30
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
	WSADATA		wsaData;
	SOCKET		hServSock, hClntSock;
	FILE* fp;
	char		buf[BUF_SIZE];
	int			readCnt, fileSize, ret, totalSend = 0;

	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSz;

	if (argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// ������ ���� �ҷ�����
	fp = fopen("file_server_win.c", "rb");
	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	// ���� ���� �޸� �ʱ�ȭ
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(argv[1]));

	// Ŭ���̾�Ʈ ���� ���
	ret = bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> bind() error\n");
	}
	ret = listen(hServSock, 5);
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> listen() error\n");
	}
	printf("> Server : client ���� �����..\n");

	// Ŭ���̾�Ʈ ����
	clntAdrSz = sizeof(clntAdr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);

	// Ŭ���̾�Ʈ ���� Ȯ��
	printf("> Server : Client�� �����....\n");

	// ���� ũ�� ����
	//int totlaTileSize = sizeof(fp);
	//send(hClntSock, (char*)buf, sizeof(totlaTileSize), 0);
	
	// ���� ũ�� ����
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	ret = send(hClntSock, (char*)&fileSize, sizeof(fileSize), 0);
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> send fileSize error\n");
	}
	printf("������ ���� �� ũ�� : %d bytes\n",fileSize);

	// ���� 30byte�� ��� ������ ������ �ݺ�
	while (totalSend < fileSize)
	{
		readCnt = fread((void*)buf, 1, BUF_SIZE, fp);  // BUF_SIZE ��ŭ ������ �о ����
		totalSend += readCnt;
		if (readCnt <= 0) { // ���� ������ ���� ��� �ݺ��� Ż��
			break;
		}
		ret = send(hClntSock, (char*)&buf, readCnt, 0);
		if (ret == SOCKET_ERROR) {
			ErrorHandling("<ERROR> send() error\n");
		}
		printf("Server> send %d bytes... %d bytes left\n", readCnt, (fileSize - totalSend));
	}

	// Client���� ������ Thank you�� ����
	ret = recv(hClntSock, (char*)buf, BUF_SIZE, 0);
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> recv Thanks error\n");
	}
	printf("Message from client: %s \n", buf);

	// shutdown() �Լ��� ����ϴ� ��� closesocket()�Լ��� ����Ͽ� ������ ����
	fclose(fp);
	closesocket(hClntSock);
	closesocket(hServSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}