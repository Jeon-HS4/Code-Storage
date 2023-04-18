#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 30
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	FILE* fp;

	char buf[BUF_SIZE];
	int readCnt, totalSize, ret, curentRecv = 0;
	float downloadRate = 0.0;
	SOCKADDR_IN servAdr;

	if (argc != 3) {
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// �Է¹��� �����͸� �ۼ��� ���� ����
	fp = fopen("receive.dat", "wb");
	hSocket = socket(PF_INET, SOCK_STREAM, 0);

	// ���� ���� �ʱ�ȭ
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr(argv[1]);
	servAdr.sin_port = htons(atoi(argv[2]));

	// ������ ����
	ret = connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr));
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> connect() error\n");
	}

	// ���� ������ �ޱ�
	ret = recv(hSocket, &totalSize, sizeof(int), 0);
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> recv filesize error\n");
	}
	printf("Client > ���� ũ�� = %d\n", totalSize);

	while (totalSize > curentRecv)  // ó�� ���� ����ũ�⸸ŭ ���������� �ݺ�
	{
		readCnt = recv(hSocket, buf, BUF_SIZE, 0);  // BUF_SIZE��ŭ �����͸� recv()�Լ��� ���� �޾ƿ�.
		if (readCnt != 0)
		{
			curentRecv += readCnt;	// ���� ������ ũ�⸦ �������� ����
			downloadRate = (float)curentRecv/totalSize*100;  // �ٿ�ε� ������ ������ ���������� ���
			fwrite((void*)buf, 1, readCnt, fp);
			printf("Clent> recieve %d bytes(total: %d bytes).\n", readCnt, totalSize);
			printf("> ���ŷ�: %0.2f %%...[%d / %d]\n\n",downloadRate,curentRecv,totalSize);
		}
		else {  // �о�� ������ ���� ��� ����
			printf("Client> server execute shutdown.\n");
			break;
		}
	}

	//������ ��� ���� �� ������ �޼��� ����
	puts("Received file data");
	ret = send(hSocket, "Thank you", 10, 0);
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> send Thanks error\n");
	}

	// closesocket()�Լ��� ����Ͽ� ������ �����Ѵ�.
	fclose(fp);
	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}