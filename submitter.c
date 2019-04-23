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
// #include <sys/sendfile.h>

#define PORT_USER 8017

#define LENGTH 512

void help(){
	printf("no good\n");
}

int
main(int argc, char const *argv[])
{
	struct sockaddr_in serv_addr;

	int sock_fd ;
	int s, len ;
	char buffer[1024] = {0};
	char * data ;
	char ip_port[100];
	char id[10];
	char ip[20];
	int port;
	char pw[8];
	char filename[30];
	 
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
                memcpy(id, optarg, 8);
				opt_ok ++;
                break;
            case 'k':
				memcpy(pw, optarg, 8);
                opt_ok ++;
                break;
			
        }
    } 

	for (int index = optind; index < argc; index++) {
		opt_ok ++;
		memcpy(filename, argv[index], 30);
	}

	printf("IP : %s, port : %d, id : %s, pw : %s filename : %s\n", ip, port, id, pw, filename);


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
	printf("[Client] Sending %s to the Server... ", filename);
	FILE *fs = fopen(filename, "r");
	if(fs == NULL)
	{
		printf("ERROR: File %s not found.\n", filename);
		exit(1);
	}

	bzero(sdbuf, LENGTH);
	int fs_block_sz;
	while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0)
	{
	    if(send(sock_fd, sdbuf, fs_block_sz, 0) < 0)
	    {
	        fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", filename, errno);
	        break;
	    }
	    bzero(sdbuf, LENGTH);
	}
	printf("Ok File %s from Client was Sent!\n", filename);
	close(sock_fd);

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
		//물어볼 때 쓰는 질문을 만들어

		//아직이라고 하면 그냥 진행

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
		printf("client> %s\n", data);
		// 받았어! 그러면 show 하고 너는 죽어
		
		close(sock_fd);
	}
	
	

}
