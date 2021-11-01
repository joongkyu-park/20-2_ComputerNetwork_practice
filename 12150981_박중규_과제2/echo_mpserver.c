// 12150981 ���߱�

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

    // signal handler �Լ� ���� ���� �� ������� �ʱ�ȭ
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    // sinal ���
    state = sigaction(SIGCHLD, &act, 0);

    // �������� ���� �� ������� �ʱ�ȭ
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // bind �Լ� ȣ��, ���Ͽ� �ּ��Ҵ�
    if (bind(serv_sock, (struct sockaddr*) & serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    // listen �Լ� ȣ��, �����û ��� ť ũ�� ����
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    // �ټ��� Ŭ���̾�Ʈ�� ������ ���� while���� ���� accept �Լ� �ݺ�
    while (1) {
        // accept �Լ��� ���� client�� ����, clinet ���� ȹ��
        adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &adr_sz);
        if (clnt_sock == -1)
            continue;
        else {
            // Ŭ���̾�Ʈ�� ip�ּҸ� ������ �ӽ� char�� ������
            char* tmp = inet_ntoa(clnt_adr.sin_addr);
            
            // Ŭ���̾�Ʈ�� ip�ּҿ� port��ȣ ���
            printf("new client connected... \n");
            printf("connected client IP : % s   port : %d \n", tmp, ntohs(clnt_adr.sin_port));
        }

        // �ڽ� ���μ��� ����
        pid = fork();
        
        // �ڽ� ���μ��� ���� ���� ��, ������ close�� �ٽ� while������
        if (pid == -1) {
            close(clnt_sock);
            continue;
        }
        
        // �ڽ� ���μ��� ���࿵��
        if (pid == 0) {
            // �ڽ� ���μ����� ���þ��� �������� ���ϵ�ũ���� ����
            close(serv_sock);

            // echo ����
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
                write(clnt_sock, buf, str_len);

            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        }
        // �θ� ���μ��� ���࿵��
        else
            // �θ� ���μ����� ���þ��� ����¼��� ���ϵ�ũ���� ����
            close(clnt_sock);
    }
    close(serv_sock);
    return 0;

}

// �ڽ����μ����� ����� ȣ��Ǵ� �Լ�
void read_childproc(int sig) {
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}

// ���� �ڵ鸵 �Լ�
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}