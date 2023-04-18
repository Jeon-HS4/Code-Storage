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

	// 전송할 파일 불러오기
	fp = fopen("file_server_win.c", "rb");
	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	// 서버 소켓 메모리 초기화
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(argv[1]));

	// 클라이언트 연결 대기
	ret = bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> bind() error\n");
	}
	ret = listen(hServSock, 5);
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> listen() error\n");
	}
	printf("> Server : client 연결 대기중..\n");

	// 클라이언트 연결
	clntAdrSz = sizeof(clntAdr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);

	// 클라이언트 접속 확인
	printf("> Server : Client가 연결됨....\n");

	// 파일 크기 전송
	//int totlaTileSize = sizeof(fp);
	//send(hClntSock, (char*)buf, sizeof(totlaTileSize), 0);
	
	// 파일 크기 전송
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	ret = send(hClntSock, (char*)&fileSize, sizeof(fileSize), 0);
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> send fileSize error\n");
	}
	printf("전송할 파일 총 크기 : %d bytes\n",fileSize);

	// 파일 30byte씩 모두 전송할 때까지 반복
	while (totalSend < fileSize)
	{
		readCnt = fread((void*)buf, 1, BUF_SIZE, fp);  // BUF_SIZE 만큼 파일을 읽어서 전송
		totalSend += readCnt;
		if (readCnt <= 0) { // 보낼 파일이 없을 경우 반복문 탈출
			break;
		}
		ret = send(hClntSock, (char*)&buf, readCnt, 0);
		if (ret == SOCKET_ERROR) {
			ErrorHandling("<ERROR> send() error\n");
		}
		printf("Server> send %d bytes... %d bytes left\n", readCnt, (fileSize - totalSend));
	}

	// Client에서 마지막 Thank you를 수신
	ret = recv(hClntSock, (char*)buf, BUF_SIZE, 0);
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> recv Thanks error\n");
	}
	printf("Message from client: %s \n", buf);

	// shutdown() 함수를 사용하는 대신 closesocket()함수를 사용하여 소켓을 종료
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