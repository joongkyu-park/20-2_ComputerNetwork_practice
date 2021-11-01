#include <stdio.h>	//ǥ�� �����
#include <stdlib.h>	// ǥ�� ���̺귯��
#include <string.h>	// ���ڿ� ó�� �Լ�
#include <unistd.h>	// ���н� ǥ��
#include <arpa/inet.h>	// ���ͳ� ��������
#include <sys/socket.h>	// ���� ��� �Լ�

#define BUF_SIZE 1024	// 1024 ����Ʈ ���� ũ�� ���
void error_handling(char* message);	// ���� ó�� �Լ�( ��� ����)

int main(int argc, char* argv[])	// ������ �ԷµǴ� ���� ������ �迭
{
	int serv_sock, clnt_sock;	// ���� ���ϰ� Ŭ���̾�Ʈ ���� 2�� �ʿ�
		//���� ������ �޾Ƶ��̴� �ϸ��ϰ�, ���� ����� Ŭ���̾�Ʈ ������ �Ѵ�.
	char message[BUF_SIZE];	// �޽��� ���ſ� ���� 1024 ����Ʈ
	int str_len, i;	// ������ �������� ���̿� ���� ���� i (index, ����)

	struct sockaddr_in serv_adr;	// ������ ����(accept ��)
	struct sockaddr_in clnt_adr;		// Ŭ���̾�Ʈ ����( ���� ������ accept ���ָ� ����)
	socklen_t clnt_adr_sz;		// Ŭ���̾�Ʈ �ּ� ����ü�� ����Ʈ ������(ũ��)

	if (argc != 2) {	// �ƱԸ�Ʈ�� 2���� �ƴϸ�. shell ���� ��Ʈ ��ȣ�� �������� �ʾ�����
		printf("Usage : %s <port>\n", argv[0]);	// ����ڿ��� ���� �ȳ�
		exit(1);		// ������ ���� ó��
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);	// TCP ��ſ� ���� ���� ����
	if (serv_sock == -1)	// ���� �����ϸ�
		error_handling("socket() error");	// ���� ǥ��

	memset(&serv_adr, 0, sizeof(serv_adr));	// ���� ����ü�� 0���� ä���.
	serv_adr.sin_family = AF_INET;	// ���� �ּ� ü�踦 ���ͳ� �迭�� ����
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);	// ������ �ּ� �ʿ� ����(���ڸ� ����)
	serv_adr.sin_port = htons(atoi(argv[1]));	// ������ ����� ��Ʈ ��ȣ
					// ���α׷��� ��Ʈ 1��
	// bind : ���ϰ� �ּҸ� �����ش�.
	if (bind(serv_sock, (struct sockaddr*) & serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	// listen : ������ ���������� ����� �� �ְ� �Ѵ�. (Ŭ���̾�Ʈ�� ��´�?)
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	clnt_adr_sz = sizeof(clnt_adr);	// Ŭ���̾�Ʈ �ּ� ����ü�� ũ��

	for (i = 0; i < 5; i++)	// 5���� Ŭ���̾�Ʈ�� ������ �޾� ���δ�.
	{			// ���� ���α׷��� ���⼭ Block �ȴ�. �մ��� �Ë� ���� ��� �ִ´�.
		clnt_sock = accept(serv_sock, (struct sockaddr*) & clnt_adr, &clnt_adr_sz);
		if (clnt_sock == -1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n", i + 1);
		// accept�� �����ϸ� Ŭ���̾�Ʈ ���� clnt_sock �� ���������.
			// while���� ���� ������?
			// Ŭ���̾�Ʈ�� ������ �ݾ� ������ 0(NULL)�� ���ƿ´�.
		while ((str_len = read(clnt_sock, message, BUF_SIZE)) != 0)
			write(clnt_sock, message, str_len);

		close(clnt_sock);	// Ŭ���̾�Ʈ�� ������ �������� Ŭ���̾�Ʈ ������ �ݴ´�.
	}

	close(serv_sock);	// 5�� Ŭ���̾�Ʈ�� �޾��ְ� ���� ���� ������ �ݰ� ������.
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}