#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	pid_t child_pid;
	for (int i=0;i<3;i++){

		if ( (child_pid=fork()) == 0){
			// a>b 
			printf("here id: %d\n", getpid());

		} else{
			printf("parent miao\n");
		}
		
	}
}