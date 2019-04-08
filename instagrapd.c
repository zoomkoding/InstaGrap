// Partly taken from https://www.geeksforgeeks.org/socket-programming-cc/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

void child_proc(int conn){
	char buf[1024] ;
	char * data = 0x0, * orig = 0x0 ;
	int len = 0 ;
	int s ;
    int file_size;
	FILE *received_file;
	int remain_data = 0;

    /* Receiving file size */
	recv(conn, buf, 1023, 0);
	file_size = atoi(buf);
	//fprintf(stdout, "\nFile size : %d\n", file_size);

	received_file = fopen("./hello.c", "w");
	if (received_file == NULL)
	{
			fprintf(stderr, "Failed to open file \"./hello.c\"\n");

			exit(EXIT_FAILURE);
	}

	remain_data = file_size;

	while ((remain_data > 0) && ((len = recv(conn, buf, 1023, 0)) > 0))
	{
			fwrite(buf, sizeof(char), len, received_file);
			remain_data -= len;
			fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
	}
    
	fclose(received_file);

	printf("worker> %s\n", data) ;

	orig = data ;
	while (len > 0 && (s = send(conn, data, len, 0)) > 0) {
		data += s ;
		len -= s ;
	}
	shutdown(conn, SHUT_WR) ;
	if (orig != 0x0)
		free(orig) ;
}

int
main(int argc, char const *argv[])
{
	int listen_fd, new_socket ;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
  	int port_num;
	char buffer[1024] = {0};

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
