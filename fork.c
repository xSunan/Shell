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


while(all_sub_cmds[i] != NULL){
	if((pid = fork())==0){
		nbytes = read(dir_input[0], readin, sizeof(readin));
		if (dir_output[1]!=STDOUT_FILENO){
			dup2 (dir_output[1], STDOUT_FILENO);
			close(dir_output[1]);
		}

		execvp(cmd, argvs)
	}

    waitpid(pid,&status,0);
	char out_put[OUT_PUT_LEN];
	nbytes = read(dir_output[0], out_put, sizeof(out_put));
	write(dir_input[1], out_put, sizeof(out_put)); 	
	
}