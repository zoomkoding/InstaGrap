// Partly taken from https://www.geeksforgeeks.org/socket-programming-cc/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int time_check = 1; //3초 초과여부를 확인하는 변수

void
alarm_handler(int sig)
{
	printf("...takes more than 3s. Quit...\n");
	time_check--;
}

void
execute_test()
{
  signal(SIGALRM, alarm_handler) ;
  alarm(3);

	execl("test", (char *) NULL) ;

	while(1);
}

void remove_all_files(){
	remove("output.out");
	remove("input.in");
	remove("test");
	remove("test.c");
}

void child_proc(int conn){
	char buf[1024] ;
	char input[200] ;
	char code[1024] ;
	char * data = 0x0, * orig = 0x0 ;
	int len = 0 ;
	int s ;

	while ( (s = recv(conn, buf, 1023, 0)) > 0 ) {
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
	printf("input : %s\n", input, code) ;

	FILE *fp = fopen("input.in", "w");
	fputs(input, fp);
	fclose(fp);

	FILE *fp2 = fopen("test.c", "w");
	fputs(code, fp2);
	fclose(fp2);

	// orig = data ;
	pid_t child_pid, child_pid1 ;
	int exit_code ;
	child_pid = fork() ;
	if (child_pid == 0) {
		printf("compile\n");
		execl("/usr/bin/gcc", "gcc", "-o", "test", "test.c", (char *) NULL);
	}
	else {
		wait(0);
		child_pid1 = fork();
		if(child_pid1 == 0){
			printf("test begins\n");
			freopen("input.in", "r", stdin);
			freopen("output.out", "w", stdout);

			signal(SIGALRM, alarm_handler) ;
		  alarm(3);

		  if(fork()== 0){
		    execute_test(); //자식 프로세스가 test를 실행한다.
		  }else{
		    wait(0);
		  }

		}
		else{
			wait(child_pid1);

			if (time_check <= 0) {
				printf("TIME OUT\n");
				send(conn, "TIME OUT", 9, 0);
				remove_all_files();
				close(conn);
			}else{
				char output[1024];
				FILE *fp = fopen("output.out", "r");
				fgets(output, sizeof(output), fp);
				printf("The result : %s\n", output);
				send(conn, output, 1024, 0);
				remove_all_files();
				close(conn);
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
		printf("Port %d starts listening...\n", port_num);
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
