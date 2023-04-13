#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
void ErrorHandling(char *message);
void ShowSocketBufSize(SOCKET sock);
void GetSocketBufSize(SOCKET sock, int *sendBuf, int *rcvBuf );

int main(int argc, char *argv[])
{
	WSADATA  wsaData;
	SOCKET hSock;
	int sndBuf, rcvBuf, state;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");
	
	hSock = socket(PF_INET, SOCK_STREAM, 0);
	ShowSocketBufSize(hSock);

	GetSocketBufSize(hSock, &sndBuf, &rcvBuf);
	
	sndBuf 	= sndBuf*3;
	rcvBuf 	= rcvBuf*3;
	
	// 새로운 버퍼 크기 설정하기
	
	state = setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char*)&sndBuf, &len);
	if(state == SOCKET_ERROR){
		ErrorHandling("<ERROR.1> setsockopt error");
	}

	state = setsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char*)&rcvBuf, &len);
	if(state == SOCKET_ERROR){
		ErrorHandling("<ERROR.2> setsockopt error");
	}


	ShowSocketBufSize( hSock );
	
	closesocket(hSock);
	WSACleanup();
	return 0;
}
 
void GetSocketBufSize(SOCKET sock, int *sendBuf, int *rcvBuf )
{
	int tmpSndBuf, tmpRcvBuf, state, len;

	// 새로운 버퍼 크기 보여주기

	// 버퍼 크기 보여주기
	// 송신 버퍼크기 확인
	len = sizeof(tmpSndBuf);
	state = getsockopt(sock, SOL_SOCKET, SO_SENDBUF, (char*)&tmpSndBuf, &len); // 소겟, 소켓레벨, 소켓 옵션, 저장할 변수, 받아올 변수
	if(state == SOCKET_ERROR){
		ErrorHandling("<ERROR.1> getsockopt() error");
	}

	// 수신 버퍼크기 확인
	len = sizeof(tmpRcvBuf);
	state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&tmpRcvBuf, &len); // 소겟, 소켓레벨, 소켓 옵션, 저장할 변수, 받아올 변수
	if(state == SOCKET_ERROR){
		ErrorHandling("<ERROR.2> getsockopt() error");
	}

	*sendBuf = tmpSndBuf;
	*rcvBuf = tmpRcvBuf;	
}

void ShowSocketBufSize(SOCKET sock)
{
	int sndBuf, rcvBuf, state, len;

	// 버퍼 크기 보여주기
	// 송신 버퍼크기 확인
	len = sizeof(sndBuf);
	state = getsockopt(sock, SOL_SOCKET, SO_SENDBUF, (char*)&sndBuf, &len); // 소겟, 소켓레벨, 소켓 옵션, 저장할 변수, 받아올 변수
	if(state == SOCKET_ERROR){
		ErrorHandling("<ERROR.1> getsockopt() error");
	}

	// 수신 버퍼크기 확인
	len = sizeof(rcvBuf);
	state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&rcvBuf, &len); // 소겟, 소켓레벨, 소켓 옵션, 저장할 변수, 받아올 변수
	if(state == SOCKET_ERROR){
		ErrorHandling("<ERROR.2> getsockopt() error");
	}

	printf("> Send buffer size = %d bytes\n",sndBuf);
	printf("> Recv buffer size = %d bytes\n",rcvBuf);

}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}