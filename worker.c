// Partly taken from https://www.geeksforgeeks.org/socket-programming-cc/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>

int time_check = 1; //3초 초과여부를 확인하는 변수

void
alarm_handler(int sig)
{
    FILE *fp = fopen("time_out.txt", "w");
    fputs("TIME OUT", fp);
    close(fp);
    
//    printf("...takes more than 3s. Quit...\n");
}

void
execute_test()
{
    signal(SIGALRM, alarm_handler) ;
    alarm(3);
    
    execl("test", (char *) NULL) ;
    
    while(1);
}

int cfileexists(const char * filename){ 
    /* try to open file to read */
    FILE *file;
    if (file = fopen(filename, "r")){
        fclose(file);
        return 1;
    }
    return 0;
}


void remove_all_files(){
	remove("output.out");
	remove("input.in");
	remove("error.out");
	remove("test");
	remove("test.c");
	remove("time_out.txt");

	// remove("test.cpp");
}

void child_proc(int conn){
	printf("worker starts working!\n");
	char buf[200000] ;
	char input[4000000] ;
	char code[100000] ;
	char * data = 0x0, * orig = 0x0 ;
	int len = 0 ;
	int s ;

	while ( (s = recv(conn, buf, 200000, 0)) > 0 ) {
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

	//파씽이 필요해
	char *token = NULL;
	int parse_count = 0;
	
	token = strtok( data, "@" );

	while( token != NULL )
	{	
		if(parse_count == 0)strcpy(input, token);
		else if(parse_count == 1)strcpy(code, token);

		
		token = strtok( NULL, "@" );
		parse_count++;
	}
	// printf("input : %s\n", input) ;

	FILE *fp = fopen("input.in", "w");
	fputs(input, fp);
	fclose(fp);

	FILE *fp2 = fopen("test.c", "w");
	// FILE *fp2 = fopen("test.cpp", "w");
	fputs(code, fp2);
	fclose(fp2);
	printf("compile : ");
	// orig = data ;
	pid_t child_pid, child_pid1 ;
	int exit_code ;
	child_pid = fork() ;
	if (child_pid == 0) {
		execl("/usr/bin/gcc", "gcc", "-o", "test", "test.c", (char *) NULL);
		// execl("/usr/bin/g++", "g++", "-o", "test", "test.cpp", (char *) NULL);
		
	}
	else {
		wait(0);
		int buildcheck = cfileexists("./test");
		// printf("build check : %d\n", buildcheck);
		if(!buildcheck) {
			printf("failed.\n");
			send(conn, "build fail", 20, 0);
			remove_all_files();
			close(conn);
		}
		else{
			printf("succeeded.\n");
			child_pid1 = fork();
			if(child_pid1 == 0){
				printf("output : ");
				freopen("input.in", "r", stdin);
				freopen("output.out", "w", stdout);
				freopen("error.out", "a+", stderr);
                
                signal(SIGALRM, alarm_handler) ;
                alarm(3);
                
                if(fork() == 0){
                    execute_test(); //자식 프로세스가 test를 실행한다.
                }else{
                    wait(0);
                    
                }
                
				exit(0);
			}
			else{
				wait(child_pid1);
                FILE *time_check_fp  = fopen("time_out.txt", "r");
                char time_check_read[20];
                
                if(time_check_fp == NULL){
                    time_check = 1;
                }else{
                    fgets(time_check_read, 20, time_check_fp);
                    if(strncmp("TIME OUT", time_check_read, 9) == 0){
                        time_check = 0;
                    }
                }
                
                if(time_check == 0){
                    printf("TIME OUT\n");
					remove_all_files();
                    send(conn, "TIME OUT", 9, 0);
                }else{
                    char output[1024];
                    FILE *fp = fopen("output.out", "r");
                    fread(output, sizeof(output), 1, fp);
                    if(output[0] == '\0') {
                        char error[1024] = "error - ";
                        char error_file[1000];
                        FILE *fp3 = fopen("error.out", "r");
                        fread(error_file, sizeof(error_file), 1, fp3);
                        printf("%s\n", error_file);
                        strcat(error, error_file);
                        send(conn, error, 1024, 0);
                    }
                    else{
                        printf("%s\nsend : ", output);
                        
                        send(conn, output, 1024, 0);
                        printf("succeeded.\n");
                    }
                }
				
				remove_all_files();
				close(conn);
				printf("worker's work done:)\n\n");

			}
		}
	}
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
		printf("Port %d starts listening...\n\n", port_num);
	}

	else {
		printf("Please give a right command with an available port number.\n");
		printf("usage : ./worker -p [port_num]\n");
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