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
#include <sys/sendfile.h>

int 
main(int argc, char const *argv[]) 
{ 
	struct sockaddr_in serv_addr; 
	int sock_fd ;
	int s, len ;
	char buffer[1024] = {0}; 
	char * data ;
    int fd;
    int sent_bytes = 0;
    char file_size[256];
    struct stat file_stat;
    int offset;
    int remain_data;
	
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	if (sock_fd <= 0) {
		perror("socket failed : ") ;
		exit(EXIT_FAILURE) ;
	} 

	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(8090); 
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		perror("inet_pton failed : ") ; 
		exit(EXIT_FAILURE) ;
	} 

	if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed : ") ;
		exit(EXIT_FAILURE) ;
	}

    fd = open("./hello.c", O_RDONLY);
    if (fd == -1)
    {
            fprintf(stderr, "Error opening file --> %s", strerror(errno));

            exit(EXIT_FAILURE);
    }

    /* Get file stats */
    if (fstat(fd, &file_stat) < 0)
    {
            fprintf(stderr, "Error fstat --> %s", strerror(errno));

            exit(EXIT_FAILURE);
    }

    fprintf(stdout, "File Size: \n%d bytes\n", file_stat.st_size);

    fprintf(stdout, "Submitter sent %d bytes for the size\n", len);

    offset = 0;
    remain_data = file_stat.st_size;
    /* Sending file data */
     while (((sent_bytes = sendfile(sock_fd, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0))
    {
            fprintf(stdout, "1. Submitter sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
            remain_data -= sent_bytes;
            fprintf(stdout, "2. Submitter sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
    }


	shutdown(sock_fd, SHUT_WR) ;

	// char buf[1024] ;
	// data = 0x0 ;
	// len = 0 ;
	// while ( (s = recv(sock_fd, buf, 1023, 0)) > 0 ) {
	// 	buf[s] = 0x0 ;
	// 	if (data == 0x0) {
	// 		data = strdup(buf) ;
	// 		len = s ;
	// 	}
	// 	else {
	// 		data = realloc(data, len + s + 1) ;
	// 		strncpy(data + len, buf, s) ;
	// 		data[len + s] = 0x0 ;
	// 		len += s ;
	// 	}

	// }
	// printf("client> %s\n", data); 

} 

