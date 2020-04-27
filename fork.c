#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	pid_t pid;
	int link[2];

	for (int i=0;i<3;i++){
		
		pid = fork();
		
		if ( pid == 0){
			// a>b 
			printf("here child  execute id: %d\n", getpid());
			sleep(5);
			exit(0);

		} 
		// printf("wait \n");
		waitpid(pid,NULL,0);
		printf("parent  childfinish\n");

		
		
	}
}