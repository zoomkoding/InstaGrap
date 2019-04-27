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
#include <unistd.h>

#define LENGTH 512
#define PORT_USER 8017
#define PORT_WORKER 8018

#define MAX 10

int id_check(char* id, char* pw){
	return 1;
}

int worker_fd;
int listen_fd;

char dir[40]= {0x0,};
struct sockaddr_in address, address2;
int opt;
int opt_ok = 0;
int addrlen = sizeof(address);
int port;
char ip[40] = {0x0,};
int wport;
char ip_port[100] = {0x0,};
int data_ready = 0;


void child_proc(int conn){
	char buf[1024];
	char line[1024];
	char type[10];
	char id[20];
	char pw[20];
	char code[1024];
	char data_from_server[1024] = {0x0,};
	char * data = 0x0, * orig = 0x0 ;
	int len = 0 ;
	int s ;
	int req_type = 0;
	

	//submitter 제출 내용 받아오기
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
	// printf(">%s\n", data) ;

	//request type 정해주기 1:send, 2:check
	if(strncmp(data, "send", 4) == 0) req_type = 1;
	if(strncmp(data, "check", 5) == 0) req_type = 2;
	
	//request type따라 진행
	if(req_type == 1){
		//파씽이 필요해
		char *token = NULL;
		int parse_count = 0;
		
		token = strtok( data, "@" );
		
		while( token != NULL )
		{	
			if(parse_count == 0)strcpy(type, token);
			else if(parse_count == 1)strcpy(id, token);
			else if(parse_count == 2)strcpy(pw, token);
			else if(parse_count == 3)strcpy(code, token);

			token = strtok( NULL, "@" );
			parse_count++;
		}

		printf("type_num : %d\ntype : %s\nid : %s\npw : %s\ncode :\n %s\n", req_type, type, id, pw, code);
		
		FILE *record = fopen(id, "w");

		//worker한테 일거리 주기
		for(int i = 1; i < 4; i ++){
			struct sockaddr_in serv_addr; 

			//worker랑 연결합시다 이제
			worker_fd = socket(AF_INET, SOCK_STREAM, 0) ;
			if (worker_fd <= 0) {
				perror("socket failed : ") ;
				exit(EXIT_FAILURE) ;
			} 
			printf("%d, wport\n", wport);
			memset(&serv_addr, '0', sizeof(serv_addr)); 
			serv_addr.sin_family = AF_INET; 
			serv_addr.sin_port = htons(wport); 
			if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
				perror("inet_pton failed : ") ; 
				exit(EXIT_FAILURE) ;
			} 

			if (connect(worker_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
				perror("connect failed : ") ;
				exit(EXIT_FAILURE) ;
			}

			char input[1024] = {0x0,};
			char where_in[100] = {0x0,};
			char where_out[100] = {0x0,};
			char tosend[1024] = {0x0,};
			char tocheck[1024] = {0x0,};
			strcpy(where_in, dir);
			strcat(where_in, "/");
			char str[5];
			sprintf(str, "%d", i);
			strcat(where_in, str);
			strcat(where_in, ".in");
			strcpy(where_out, dir);
			strcat(where_out, "/");
			strcat(where_out, str);
			strcat(where_out, ".out");
			// printf("주소: %s\n", where_in);
			FILE *fp;
			fp = fopen(where_in, "r");
			fgets(tosend, sizeof(tosend), fp); 
			fclose(fp);
			strcat(tosend, "@");
			strcat(tosend, code);
			// printf("워커한테 보내는 내용\n%s", tosend);
			send(worker_fd, tosend, strlen(tosend), 0);
			shutdown(worker_fd, SHUT_WR) ;

			char buf[1024] ;
			data = 0x0 ;
			len = 0 ;
			while ( (s = recv(worker_fd, buf, 1023, 0)) > 0 ) {
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
			
			printf("%d번째 Result %s\n", i, data);

			FILE *fp2;
			fp2 = fopen(where_out, "r");
			fgets(tocheck, sizeof(tocheck), fp); 
			fclose(fp2);

			printf("결과값 : %s\n정답값 : %s\n", data, tocheck);
			if(strcmp(data, tocheck) == 0) fprintf(record, "%d번째 테스트 케이스 : 정답입니다!\n", i);
			else fprintf(record, "%d번째 테스트 케이스 : 틀렸습니다!\n", i);
			close(worker_fd);
			fflush(record);
		}
		data_ready = 1;
		fclose(record);
	}
	else if(req_type == 2){
		//파씽이 필요해
		char *token = NULL;
		int parse_count = 0;
		
		token = strtok( data, "@" );

		while( token != NULL )
		{	
			if(parse_count == 0)strcpy(type, token);
			else if(parse_count == 1)strcpy(id, token);
			else if(parse_count == 2)strcpy(pw, token);

			
			token = strtok( NULL, "@" );
			parse_count++;
		}

		//데이터가 준비되면 파일에 써놨던 내용을 다 읽어서 보내줘
		if(data_ready && id_check(id, pw)) {
			FILE *myrecord = fopen(id, "r");
			char record[1024] = {0x0,};
			fread(record, sizeof(record), 1, myrecord);
			send(conn, record, 1024, 0);
		}
		else send(conn, "no", 10, 0);
		close(conn);
		printf("type_num : %d\ntype : %s\nid : %s\npw : %s\n", req_type, type, id, pw);

	}
	
}

int
main(int argc, char const *argv[])
{
	int new_socket ;

	
	
	while((opt = getopt(argc, argv, "p:w:")) != -1) 
    {
        switch(opt) 
        { 
            case 'p':
                port = atoi(optarg);
				printf("Port %d starts listening...\n", port);
				opt_ok ++;
                break; 
            case 'w':
			    memcpy(ip_port, optarg, 100);
				// printf("ip_port : %s\n");
				char *token = NULL;
				token = strtok( ip_port, ":" );
				int i = 0;
				while( token != NULL )
				{	
					printf("token : %s", token);
					if(i == 0) memcpy(ip, token, 20);
					else if(i == 1) wport = atoi(token);
					token = strtok( NULL, ":" );
					i++;
				}
				opt_ok ++;
                break;
        }
    } 


	for (int index = optind; index < argc; index++) {
		opt_ok ++;
		memcpy(dir, argv[index], 30);
	}

	printf("port : %d, ip : %s, wp : %d, dir : %s\n", port, ip, wport, dir);


//	if (opt_ok != 3)
//    { 
//		printf("Please give a right command with an available port number.\n");
//		printf("usage : ./instagrapd -p [port_num] -w <IP>:<WPort> <Dir>\n");
//		return 0;
//    } 

	listen_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
	if (listen_fd == 0)  {
		perror("socket failed : ");
		exit(EXIT_FAILURE);
	}

	memset(&address, '0', sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY /* the localhost*/ ;
	address.sin_port = htons(port);
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
