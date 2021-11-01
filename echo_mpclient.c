#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30
void error_handling(char* message);
void read_routine(int sock, char* buf);
void write_routine(int sock, char* buf);

int main(int argc, char* argv[])
{
	int sock;
	char buf[BUF_SIZE];
	struct sockaddr_in serv_adr;
	if (argc != 3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	fd_set reads, cpy_reads; // fd_set 변수 2개 선언
	struct timeval timeout; // timeval 변수 선언

	int fd_max, str_len, fd_num, i;	

	if (connect(sock, (struct sockaddr*) & serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	
	FD_ZERO(&reads); // fd_set 초기화
	FD_SET(sock, &reads); // 서버소켓을 관심대상으로 설정
	fd_max = sock;

	while (1)
	{
		write_routine(sock, buf);

		cpy_reads = reads; // 원본을 복사
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000; // 타임아웃 설정

		if ((fd_num = select(fd_max + 1; &cpy_reads, 0, 0, &timeout)) == -1)
			break;
		if (fd_num == 0)
			continue;

		for (i = 0;i < fd_max + 1;i++)
		{
			if (FD_ISSET(i, &cpy_reads))
			{
				if (i == sock)
				{
					read_routine(sock, buf);
				}
			}
		}

		/*if (pid == 0)
			write_routine(sock, buf);
		else
			read_routine(sock, buf);*/
	}

	close(sock);
	return 0;
}

void read_routine(int sock, char* buf)
{
	while (1)
	{
		int str_len = read(sock, buf, BUF_SIZE);
		if (str_len == 0)
			return;

		buf[str_len] = 0;
		printf("Message from server: %s", buf);
	}
}
void write_routine(int sock, char* buf)
{
	while (1)
	{
		fgets(buf, BUF_SIZE, stdin);
		if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
		{
			shutdown(sock, SHUT_WR);
			return;
		}
		write(sock, buf, strlen(buf));
	}
}
void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}