// Partly taken from https://www.geeksforgeeks.org/socket-programming-cc/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LENGTH 512

int sock_fd ;


void child_proc(int conn){
	char buf[1024] ;
	char * data = 0x0, * orig = 0x0 ;
	int len = 0 ;
	int s ;
  int file_size;
	FILE *received_file;
	int remain_data = 0;

	/*Receive File from Client */
	char* fr_name = "21500670.c";
	char* student_id = "21500670";
	FILE *fr = fopen(fr_name, "a");
	if(fr == NULL)
		printf("File %s Cannot be opened file on server.\n", fr_name);
	else
	{
		bzero(buf, LENGTH);
		int fr_block_sz = 0;
		while((fr_block_sz = recv(conn, buf, LENGTH, 0)) > 0)
		{
		  int write_sz = fwrite(buf, sizeof(char), fr_block_sz, fr);
			if(write_sz < fr_block_sz)
		    {
		        error("File write failed on server.\n");
		    }
			bzero(buf, LENGTH);
			if (fr_block_sz == 0 || fr_block_sz != 512)
			{
				break;
			}
		}
		if(fr_block_sz < 0)
	    {
	        if (errno == EAGAIN)
        	{
                printf("recv() timed out.\n");
            }
            else
            {
                fprintf(stderr, "recv() failed due to errno = %d\n", errno);
				exit(1);
            }
      	}
		printf("Ok received from client!\n");
		fclose(fr);
	}


	pid_t child_pid = fork() ;
	if (child_pid == 0) {
		execl("/usr/bin/gcc", "gcc", "-o", student_id, fr_name, (char *) NULL);
	}
	else {
		wait(0);
		printf("instagrapd> exe file created\n");
		data = student_id;
		len = 8;
		s = 0;
		while (len > 0 && (s = send(sock_fd, data, len, 0)) > 0) {
			data += s ;
			len -= s ;
		}
		shutdown(sock_fd, SHUT_WR) ;

	}


	char buf1[1024] ;
	data = 0x0 ;
	len = 0 ;
	while ( (s = recv(sock_fd, buf1, 1023, 0)) > 0 ) {
		buf1[s] = 0x0 ;
		if (data == 0x0) {
			data = strdup(buf1) ;
			len = s ;
		}
		else {
			data = realloc(data, len + s + 1) ;
			strncpy(data + len, buf1, s) ;
			data[len + s] = 0x0 ;
			len += s ;
		}

	}
	printf("instagrapd> %s\n", data);

}

int
main(int argc, char const *argv[])
{
	int listen_fd, new_socket ;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
  	int port_num;

	if(strcmp(argv[1], "-p") == 0){
		port_num = atoi(argv[2]);
		printf("Port %d starts listening...\n", port_num);
	}

	else {
		printf("Please give a right command with an available port number.\n");
		printf("usage : ./instagrapd -p [port_num] -w <IP>:<WPort> <Dir>\n");
		return 0;
	}

	listen_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
	if (listen_fd == 0)  {
		perror("socket failed : ");
		exit(EXIT_FAILURE);
	}

	memset(&address, '0', sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY /* the localhost*/ ;
	address.sin_port = htons(port_num);
	if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed : ");
		exit(EXIT_FAILURE);
	}


	struct sockaddr_in serv_addr;


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

	if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed : ") ;
		exit(EXIT_FAILURE) ;
	}

	while (1) {
		if (listen(listen_fd, 16 /* the size of waiting queue*/) < 0) {
			perror("listen failed : ");
			exit(EXIT_FAILURE);
		}

		new_socket = accept(listen_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;
		if (new_socket < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		if (fork() > 0) {
			child_proc(new_socket) ;
		}
		else {
			close(new_socket) ;
		}
	}
}
