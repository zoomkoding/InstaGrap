#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

typedef struct Assignment{
  char *std_id;
  char *std_pwd;
  // FILE *f;이건 나중에
}assignment;
/**
TODO
1. getopt에 맞춰서 base code 짜기 (아직 client.c 그대로 복붙한 형태임)
2. sys/sendfile.h 에 있는 sendfile()이라는 함수에 대해 알아보고 적합하다고 판단되면 적용해서 코드짜기
3. instagrapd에서 답이 올때까지 돌고 있어야 하는데 While(1) 안에 listen부터 쭉 있어야 하는 건가? <-- 이것도 고민이 필요함.

**/
int
main(int argc, char const *argv[])
{
	struct sockaddr_in serv_addr;
	int sock_fd ;
	int s, len ;
	char buffer[1024] = {0};
	char * data ;
 //1. socket
	sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	if (sock_fd <= 0) {
		perror("socket failed : ") ;
		exit(EXIT_FAILURE) ;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8080);
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		perror("inet_pton failed : ") ;
		exit(EXIT_FAILURE) ;
	}
  //2. connect (여기까지는 client.c와 유사)
	if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed : ") ;
		exit(EXIT_FAILURE) ;
	}

	scanf("%s", buffer) ;
  /**
  3 - 1. send
  이전에 open()을 이용해서 param으로 넘겨준 파일을 open하고 read()를 통해 읽어들인 후 그 char * 를 넘긴다.
  **/
	data = buffer ;
	len = strlen(buffer) ;
	s = 0 ;
	while (len > 0 && (s = send(sock_fd, data, len, 0)) > 0) {
		data += s ;
		len -= s ;
	}

	shutdown(sock_fd, SHUT_WR) ;
  // 3-2. recv
	char buf[1024] ;
	data = 0x0 ;
	len = 0 ;
	while ( (s = recv(sock_fd, buf, 1023, 0)) > 0 ) {
		buf[s] = 0x0 ;
		if (data == 0x0) {
			data = strdup(buf) ;
			len = s ;
		}
		else {
			data = realloc(data, len + s + 1) ;
			strncpy(data + len, buf, s) ;
			data[len + s] = 0x0 ;
			len += s ;
		}

	}
	printf(">%s\n", data);

}
