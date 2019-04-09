#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
  pid_t child_pid ;
	int exit_code ;

	child_pid = fork() ;
	if (child_pid == 0) {
    
		execl("/usr/bin/gcc", "gcc", "-o", "child", "child.c", (char *) NULL);
	}
	else {
    wait(0);
    freopen("../uppercase.txt", "r", stdin);
    freopen("lowercase.txt", "w", stdout);
    execl("./child", 0);
	}
	wait(&exit_code) ;

	exit(0) ;
  
}


