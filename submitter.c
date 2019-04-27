#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

// #include <sys/sendfile.h>

#define PORT_USER 8017

#define LENGTH 1024

void help(){
	printf("no good\n");
}

void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}

int
main(int argc, char const *argv[])
{
	struct sockaddr_in serv_addr;

	int sock_fd ;
	int s, len ;
	char buffer[LENGTH] = "send@";
	char * data ;
	char ip_port[100];
	char id[20];
	char ip[20];
	int port;
	char pw[20];
	char filename[30];
	char recved[100];
	 
	int opt;
	int opt_ok = 0;
	while((opt = getopt(argc, argv, "n:u:k:")) != -1) 
    {
        switch(opt) 
        { 
            case 'n':
                memcpy(ip_port, optarg, 100);
				char *token = NULL;
				token = strtok( ip_port, ":" );
				int i = 0;
				while( token != NULL )
				{
					if(i == 0) memcpy(ip, token, 20);
					else if(i == 1) port = atoi(token);
					token = strtok( NULL, ":" );
					i++;
				}
				opt_ok ++;
                break; 
            case 'u':
                memcpy(id, optarg, 20);
				opt_ok ++;
                break;
            case 'k':
				memcpy(pw, optarg, 20);
                opt_ok ++;
                break;
			
        }
    } 

	for (int index = optind; index < argc; index++) {
		opt_ok ++;
		memcpy(filename, argv[index], 30);
	}

	printf("IP : %s, port : %d, id : %s, pw : %s filename : %s\n", ip, port, id, pw, filename);

	//알규먼트가 제대로 안왔으면 help 보여주고 종료
	if (opt_ok != 4)
    { 
        help();
        exit(0);
    } 

	sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	if (sock_fd <= 0) {
		perror("socket failed : ") ;
		exit(EXIT_FAILURE) ;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
		perror("inet_pton failed : ") ;
		exit(EXIT_FAILURE) ;
	}

	if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed : ") ;
		exit(EXIT_FAILURE) ;
	}

	char sdbuf[LENGTH];
	FILE *fs = fopen(filename, "r");
	if(fs == NULL)
	{
		printf("ERROR: File %s not found.\n", filename);
		exit(1);
	}

	bzero(sdbuf, LENGTH);
	int fs_block_sz;
	
	//파일 읽고 읽기 다 끝내면 내용을 중간에 @을 넣어서 instagrapd한테 보냅시다.
	while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0)
	{
		strcat(buffer, id);
		strcat(buffer, "@");
		strcat(buffer, pw);
		strcat(buffer, "@");
		strcat(buffer, sdbuf);
		data = buffer ;
		len = strlen(buffer) ;
		s = 0 ;
		printf("%s", buffer);
		while (len > 0 && (s = send(sock_fd, data, len, 0)) > 0) {
			data += s ;
			len -= s ;
		}
	    bzero(sdbuf, LENGTH);
		bzero(buffer, LENGTH);
	}
	close(sock_fd);
	

	char request[100] = "check@";
	strcat(request, id);
	strcat(request, "@");
	strcat(request, pw);

	while(1){
		sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
		if (sock_fd <= 0) {
			perror("socket failed : ") ;
			exit(EXIT_FAILURE) ;
		}

		memset(&serv_addr, '0', sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(port);
		if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
			perror("inet_pton failed : ") ;
			exit(EXIT_FAILURE) ;
		}

		if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
			perror("connect failed : ") ;
			exit(EXIT_FAILURE) ;
		}
		data = request ;
		len = strlen(request) ;
		s = 0 ;
		while (len > 0 && (s = send(sock_fd, data, len, 0)) > 0) {
			data += s ;
			len -= s ;
		}
		shutdown(sock_fd, SHUT_WR) ;

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
		close(sock_fd);
		if(strncmp(data, "no", 2) != 0) {
			printf("%s", data);
			break;
		}
		printf("request result : %s\n", data);
		waitFor(1);
	}
}
