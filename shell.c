#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define COMMAND_LEN 50
#define ARGV_LEN 150
#define COMMAND_NUM 20
#define OUT_PUT_LEN 4090

/*

Divide the input line into multiple commands
 return the relationship between multiple command.
 0 means that the commands should run consecutively  (";")
 1 means that the commands should tun concurrently   ("&")

*/
int extract_all_commands(char* input, char** all_commands){
	int concurrent;
	const char delim[3]= "&;";

	if(strchr(input, '&') != NULL){
		concurrent = 1;
	} else if (strchr(input, ';') != NULL){
		concurrent = 0;	
	} 

	// divide multiple commands
	char *command = strtok(input, delim);   	
	int i=0;
	while(command != NULL ) {
		printf("command: %s\n", command);
		all_commands[i] = (char *) malloc(sizeof(char)*COMMAND_LEN);
		all_commands[i++] = command;
	  	command = strtok(NULL, delim);
	}
	all_commands[i] = NULL;
	return concurrent;
}

/* 

 Divide the redirection parts in a single commands
 return the property of the command.
 0 means that no file redirection or pipeline
 1 means that exists file redirection ('>')
 2 means that exists pipeline ('|')

*/
int sub_commands(char* command, char** all_sub_cmds){

	/* find pipelined commands*/
	const char delim_pipe[] = ">|";
	int redirection;

	if(strchr(command, '>') != NULL){
		redirection = 1;
	} else if (strchr(command, '|') != NULL){
		redirection = 2;	
	}  else {
		redirection = 0;
	}
	// parse arguements
	char *sub_cmd = strtok(command, delim_pipe);   	
	int i=0;
	while(sub_cmd != NULL && strlen(sub_cmd)>0) {
		printf("sub: %s\n", sub_cmd);
		all_sub_cmds[i] = (char *) malloc(sizeof(char)*COMMAND_LEN);
		all_sub_cmds[i++] =sub_cmd;
	  	sub_cmd = strtok(NULL, delim_pipe);
	}
	all_sub_cmds[i] = NULL;
	printf("miao end sub\n");
	return redirection;
}	

int parse(char *command, char **argv)
{
	const char delim[] = " \n";
	char *arg = strtok(command, delim);   	
	int i=0;

	while(arg != NULL && strlen(arg)>0) {
		printf("arg: %s\n", arg);
		argv[i] = (char *) malloc(sizeof(char)*COMMAND_LEN);
		argv[i++] =arg;
	  	arg = strtok(NULL, delim);
	}
	arg[i] = NULL;
	printf("return\n");
	return i;
}

void redirect(char** all_sub_cmds, int redirection){
	pid_t pid;
	int i=0, nbytes=0, argv_cnt, status;
	int dir_input[2], dir_output[2];
	if (pipe(dir_input)==-1){
		fprintf("%s\n", stderr); 
		exit(EXIT_FAILURE);
	}

	if(pipe(dir_output) == -1){
		fprintf("%s\n", stderr); 
		exit(EXIT_FAILURE);
	}

	while(all_sub_cmds[i] != NULL){
		printf("inside: cmd %s \n", all_sub_cmds[i]);

		if((pid = fork())==0){
			char readin[OUT_PUT_LEN];
			if(i>=1)
				nbytes = read(dir_input[0], readin, sizeof(readin));
			printf("nbytes:%d\n", nbytes);

			char *argvs[ARGV_LEN];
			argv_cnt = parse(all_sub_cmds[i], argvs);

			printf("argv_cnt:%d\n", argv_cnt);

			if (nbytes !=0){
				argvs[argv_cnt] = readin;
			}

			// pass the file descriptor of stdout to pipe
			dup2 (dir_output[1], STDOUT_FILENO);

			if (execvp(*argvs, argvs) < 0) {
            	printf("*** ERROR: exec failed\n");
            	exit(1);
        	} else {
        		exit(0);
        	}

		} 
		waitpid(pid,&status,0);
		// if (status == pid){
			printf("succeed\n");
			char out_put[OUT_PUT_LEN];
			nbytes = read(dir_output[0], out_put, sizeof(out_put));
			printf("out_put: (%.*s)  \n", nbytes,out_put);
			write(dir_input[1], out_put, sizeof(out_put));
		// }
		i++;
	}
	printf("finish");
}

pid_t execute(char *command, int concurrent)
{
    pid_t pid;
    int status, redirection, argv_num;

    if ((pid = fork()) < 0) {
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    }
    else if (pid == 0) {
        // child process
        char *all_sub_cmds[COMMAND_NUM];
        printf("here");
        redirection = sub_commands(command, all_sub_cmds);
        printf("redirection: %d, %s\n", redirection, command);
        if (redirection == 0){
        	char ** argvs;
        	argv_num = parse(command, argvs);
        	if (execvp(*argvs, argvs) < 0) {
            	printf("*** ERROR: exec failed\n");
            	exit(1);
        	} else {
        		exit(0);
        	}
        } else if(redirection == 1){
        	redirect(all_sub_cmds, 1);
        } else {
        	//  |
        	int i=0;
        	while(all_sub_cmds[i] != NULL){
        		printf("%s \n", all_sub_cmds[i++]);
        	}
        	redirect(all_sub_cmds, 2);
        }
        // exit(0);
    }
    else  {
        if (concurrent == 0) {
            while (wait(&status) != pid) {
                continue;
            }
        }
    }
    return pid;
}

void execute_all_commands(char **all_commands, int status)
{
	char *single_command[64];
	pid_t pids[COMMAND_NUM];
	int st[COMMAND_NUM],count=0,k=0;

	while(all_commands[k]!=NULL){
		
		pids[count++] = execute(all_commands[k], status);
		k++;
	}

	if (status == 1) {
		for (int i = 0; i < count; i++) {
			waitpid(pids[i], &st[i], 0);
		}
	}
}

int main(){
	char input_line[COMMAND_LEN];
 
    char *all_commands[COMMAND_NUM];    

    fgets(input_line, COMMAND_LEN, stdin); 
	
	// status = 1 means concurrent, 0 means serial
	int status = extract_all_commands(input_line, all_commands);
	execute_all_commands(all_commands, status);
}



