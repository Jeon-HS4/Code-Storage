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

	// 입력받을 데이터를 작성할 파일 오픈
	fp = fopen("receive.dat", "wb");
	hSocket = socket(PF_INET, SOCK_STREAM, 0);

	// 서버 소켓 초기화
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr(argv[1]);
	servAdr.sin_port = htons(atoi(argv[2]));

	// 서버와 연결
	ret = connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr));
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> connect() error\n");
	}

	// 파일 사이즈 받기
	ret = recv(hSocket, &totalSize, sizeof(int), 0);
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> recv filesize error\n");
	}
	printf("Client > 파일 크기 = %d\n", totalSize);

	while (totalSize > curentRecv)  // 처음 받은 파일크기만큼 받을때까지 반복
	{
		readCnt = recv(hSocket, buf, BUF_SIZE, 0);  // BUF_SIZE만큼 데이터를 recv()함수를 통해 받아옴.
		if (readCnt != 0)
		{
			curentRecv += readCnt;	// 받은 파일의 크기를 누적시켜 저장
			downloadRate = (float)curentRecv/totalSize*100;  // 다운로드 비율을 파일을 받을때마다 계산
			fwrite((void*)buf, 1, readCnt, fp);
			printf("Clent> recieve %d bytes(total: %d bytes).\n", readCnt, totalSize);
			printf("> 수신률: %0.2f %%...[%d / %d]\n\n",downloadRate,curentRecv,totalSize);
		}
		else {  // 읽어올 파일이 없을 경우 종료
			printf("Client> server execute shutdown.\n");
			break;
		}
	}

	//파일을 모두 받은 후 마무리 메세지 전송
	puts("Received file data");
	ret = send(hSocket, "Thank you", 10, 0);
	if (ret == SOCKET_ERROR) {
		ErrorHandling("<ERROR> send Thanks error\n");
	}

	// closesocket()함수를 사용하여 소켓을 종료한다.
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