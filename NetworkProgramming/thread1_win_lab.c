#include <stdio.h>
#include <windows.h>
#include <process.h>    /* _beginthreadex, _endthreadex */
unsigned WINAPI ThreadFunc(void *arg);

int main(int argc, char *argv[]) 
{
	HANDLE		hThread;
	unsigned	threadID;
	int			param = 2;

	hThread =  (HANDLE)	_beginthreadex( 
							// 파라미터 추가
							NULL, 			// 보안 정보
							0,				// 스택 크기
							ThreadFunc, 	//(중요)실행될때 사용되는 함수의 포인터(이름)
							(void*)&param	//(중요)함수 파라미터, 주소를 넘겨야 하며, 자료형도 맞춰야함
							0, 				// 스레드 생성 후 즉시 실행, 0이 아닌경우 일정 시간 뒤 실행
							&threadID		// threadID를 담을 변수의 주소
						);

	Sleep(3000);
	puts("<main> end of main.");
	return 0;
}

unsigned WINAPI ThreadFunc(void *arg)
{
	int i;
	int cnt;
	// 스레드 동작 코딩.
	cnt = *((int*)arg);
	for(i = 0; i < cnt; i++){
		printf("<thread> run %d.\n",i);
		Sleep(1000);
	}
	return 0;
}

/* 출력결과
<thread> run 0
<thread> run 1
<thread> run 2
<main> end of main.

main스레드가 종료되면 main에서 파생된 스레드는 같이 종료된다.
*/