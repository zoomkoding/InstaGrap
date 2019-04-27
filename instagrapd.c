#include <unistd.h> //getopt 등
#include <stdio.h>
#include <sys/socket.h> //accept 등
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h> //strdup, strncpy 등
#include <stdio.h> //getdelim

//submitter로부터 받는 과제물!
typedef struct Assignment{
  char *std_id;
  char *std_pwd;
  // FILE *f;이건 나중에
}assignment;

/**
TODO
1. sys/sendfile.h 에 있는 sendfile()이라는 함수에 대해 알아보고 적합하다고 판단되면 적용해서 코드짜기

**/

/*
submitter와 통신하는 부분
submitter로부터 받아야 하는 것들
1. student id
2. password
3. file (이 부분은 submitter가 file을 어떤 형식으로 보내는가에 따라 달라질 것 같은데...생각을 해보자)

일단 student_id와 password 받는 것부터!

param : sumitter_sd (submitter의 socket descritper)
*/
void
child_proc(int submitter_sd)
{
  assignment buf[1024];
	char * std_id = 0x0, * std_pwd = 0x0 ;

	// char * data = 0x0, * orig = 0x0 ;
	int len_id = 0 ;
  int len_pwd = 0 ;
	int s_id ;
  int s_pwd ;

	while ( (s = recv(submitter_sd, buf, 1023 * sizeof(assignment), 0)) > 0 ) { //만약 socket이 assignment 라는 structure를 보낸다면...너무 큰 거 같기도 하고 이걸 어떻게 처리하지...
		buf[s] = 0x0 ;
		if (std_id == 0x0) {
			std_id = strdup(buf) ;
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
  //
	// orig = data ;
	// while (len > 0 && (s = send(submitter_sd, data, len, 0)) > 0) {
	// 	data += s ;
	// 	len -= s ;
	// }
	shutdown(submitter_sd, SHUT_WR) ; //write를 shutdown한다?

	if (orig != 0x0)
		free(orig) ;
}

void Usage(const char *pname)
{
    printf("\nUsage: %s [options]\n",pname);
    printf("option: [p] | [w] \n");
    printf("option 'p' and 'w' need Add parameter\n");
    printf("ex) %s -p <Port> -w <IP>:<WPort> <Dir>\n", pname);
		printf(
					"\t<Port> : port for listening of instagrapd\n"
					"\t<IP> IP address of worker\n"
					"\t<WPort> port of worker\n"
					"\t<Dir> a path to a testcase directory\n"
				);
}

int
main(int argc, char **argv)
{
	int submitter_listen_fd, submitter_socket, listen_port ;
	struct sockaddr_in submitter_address; //submitter와 통신할 주소를 저장하는 구조체
	int submitter_addrlen = sizeof(submitter_address);
	char buffer[1024] = {0}; //이게 굳이 필요한 이유는??

	submitter_listen_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
	if (submitter_listen_fd == 0)  {
		perror("socket failed : ");
		exit(EXIT_FAILURE);
	}

	memset(&submitter_address, '0', sizeof(submitter_address));

	int opt; //option
	/*
	option을 받아서 처리하는 부분
	*/
	// while((opt = getopt(argc, argv, "p:w:")) != -1) { //일단 worker에 관련된 부분은 나중에 코딩하자
	while((opt = getopt(argc, argv, "p:")) != -1) {
		switch (opt) {
			case 'p':
			listen_port = (unsigned short int)(atoi(optarg)); //이 부분이 약간 걸린다...
			printf("%hu \n", listen_port);

			break;

			// case 'w':
			// /*
			// 이 부분은 worker와 관련한 부분이니 후에 코딩하자
			// */
			// printf("-w %s\n", optarg);
			//strdup 사용하기!!
			//param = strdup(optarg);
			//free(param);
			// break;

			default:
			Usage(argv[0]);
			break;
		}
	}

	submitter_address.sin_family = AF_INET;
	submitter_address.sin_addr.s_addr = INADDR_ANY /* 일단 the localhost 나중에 server 복구되면 peace server로 입력*/ ;
	submitter_address.sin_port = htons(listen_port); //option p에서 설정한 port에서 listen
	if (bind(submitter_listen_fd, (struct sockaddr *)&submitter_address, sizeof(submitter_address)) < 0) {
		perror("bind failed : ");
		exit(EXIT_FAILURE);
	}

	while (1) {
		if (listen(submitter_listen_fd, 16 /*the size of waiting queue*/) < 0) {
			perror("listen failed : ");
			exit(EXIT_FAILURE);
		}

		submitter_socket = accept(submitter_listen_fd, (struct sockaddr *) &submitter_address, (socklen_t*)&submitter_addrlen) ;
		if (submitter_socket < 0) {
			perror("accept failed : ");
			exit(EXIT_FAILURE);
		}

		if (fork() > 0) {
			child_proc(submitter_socket) ;
		}
		else {
			close(submitter_socket) ;
		}
	}
}
