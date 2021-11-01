// 12150981 박중규

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;

    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;
    int str_len, state;
    char buf[BUF_SIZE];
    if (argc != 2) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    // signal handler 함수 정보 전달 및 멤버변수 초기화
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    // sinal 등록
    state = sigaction(SIGCHLD, &act, 0);

    // 서버소켓 생성 및 멤버변수 초기화
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // bind 함수 호출, 소켓에 주소할당
    if (bind(serv_sock, (struct sockaddr*) & serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    // listen 함수 호출, 연결요청 대기 큐 크기 전달
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    // 다수의 클라이언트와 연결을 위해 while문을 통해 accept 함수 반복
    while (1) {
        // accept 함수를 통해 client와 연결, clinet 정보 획득
        adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &adr_sz);
        if (clnt_sock == -1)
            continue;
        else {
            // 클라이언트의 ip주소를 전해줄 임시 char형 포인터
            char* tmp = inet_ntoa(clnt_adr.sin_addr);
            
            // 클라이언트의 ip주소와 port번호 출력
            printf("new client connected... \n");
            printf("connected client IP : % s   port : %d \n", tmp, ntohs(clnt_adr.sin_port));
        }

        // 자식 프로세스 생성
        pid = fork();
        
        // 자식 프로세스 생성 실패 시, 소켓을 close후 다시 while문으로
        if (pid == -1) {
            close(clnt_sock);
            continue;
        }
        
        // 자식 프로세스 실행영역
        if (pid == 0) {
            // 자식 프로세스와 관련없는 서버소켓 파일디스크립터 정리
            close(serv_sock);

            // echo 서비스
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
                write(clnt_sock, buf, str_len);

            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        }
        // 부모 프로세스 실행영역
        else
            // 부모 프로세스와 관련없는 입출력소켓 파일디스크립터 정리
            close(clnt_sock);
    }
    close(serv_sock);
    return 0;

}

// 자식프로세스가 종료시 호출되는 함수
void read_childproc(int sig) {
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}

// 에러 핸들링 함수
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}