#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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
		// printf("extract all: %s input %s\n", command,input);
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
		all_sub_cmds[i] = (char *) malloc(sizeof(char)*COMMAND_LEN);
		all_sub_cmds[i++] =sub_cmd;
	  	sub_cmd = strtok(NULL, delim_pipe);
	}
	all_sub_cmds[i] = NULL;
	return redirection;
}	


int parse(char *command, char **argv)
{
	const char delim[] = " \n";
	char *arg = strtok(command, delim);   	
	int i=0;

	while(arg != NULL && strlen(arg)>0) {
		argv[i] = (char *) malloc(sizeof(char)*COMMAND_LEN);

		argv[i++] =arg;
	  	arg = strtok(NULL, delim);
	}
	for(int j=i;j<ARGV_LEN;j++){
		argv[j] = NULL;
	}
	return i;
}

void pipe_line(char** all_sub_cmds, int redirection){
	pid_t pid;
	int i=0, nbytes=0, argv_cnt, status;
	int dir_output[2];
	

	while(all_sub_cmds[i] != NULL){
		// printf("inside: cmd %s \n", all_sub_cmds[i]);
	
		if(pipe(dir_output) == -1){
			fprintf("%s\n", stderr); 
			exit(EXIT_FAILURE);
		}

		if((pid = fork())==0){

			char *argvs[ARGV_LEN];
			parse(all_sub_cmds[i], argvs);
			close(dir_output[0]);
			if (all_sub_cmds[i+1] != NULL && dir_output[1]!=STDOUT_FILENO){
				// printf("need connection\n");
				dup2 (dir_output[1], STDOUT_FILENO);
				close(dir_output[1]);
			}

			if (execvp(*argvs, argvs) < 0) {
				printf("pipe line sub_commands %s\n", all_sub_cmds[i]);
            	printf("*** ERROR: exec failed\n");
            	exit(1);
        	} 

        	exit(0);
        	

		} else {
			
			close(dir_output[1]);
			if (dup2(dir_output[0], STDIN_FILENO) == -1) {
				exit(EXIT_FAILURE);
			}
			waitpid(pid,NULL,0);
			close(dir_output[0]);

			
			// if(all_sub_cmds[i+1] == NULL){
			// 	char out_put[OUT_PUT_LEN];
				
			// 	printf("miao\n");
			// 	nbytes = read(dir_output[0], out_put, sizeof(out_put));
			// 	printf("nbytes:%d\n", nbytes);
			// 	printf("out_put: (%.*s)  \n", nbytes,out_put);
			// }
							
			
		} 

		i++;
	}
	// printf("finish");
}

void redirect(char** all_sub_cmds, int redirection) {
    char *command[ARGV_LEN];
    char *filename[ARGV_LEN];
    int file_desc;
    FILE *fp;

    parse(all_sub_cmds[0], command);
    parse(all_sub_cmds[1], filename);
    if (filename[0] == NULL || filename[1] != NULL ) {
        // invalid filename
        printf("Invalid filename.\n");
        return;
    }

    fp = fopen(filename[0], "w");
    fclose(fp);
    file_desc = open(filename[0] ,O_WRONLY | O_APPEND);
    dup2(file_desc, STDOUT_FILENO);
    if (execvp(*command, command) < 0) {
        printf("*** ERROR: exec failed\n");
        exit(1);
 }

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
        // sleep(3);
        char *all_sub_cmds[COMMAND_NUM];
        // printf("here");
        redirection = sub_commands(command, all_sub_cmds);
        // printf("redirection: %d, %s\n", redirection, command);
        if (redirection == 0){
        	char *argvs[ARGV_LEN];
        	argv_num = parse(command, argvs);
        	if (execvp(*argvs, argvs) < 0) {
        		printf("sub_commands %s\n", command);

            	printf("*** ERROR: exec failed\n");
            	exit(1);
        	} 
        	exit(0);
        } else if(redirection == 1){
        	redirect(all_sub_cmds, 1);
        } else {
        	
        	pipe_line(all_sub_cmds, 2);
        }
        exit(0);
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
		printf("execute %s \n", all_commands[k]);
		pids[count++] = execute(all_commands[k], status);
		k++;
	}

	// if (status == 1) {
	
		for (int i = 0; i < count; i++) {
			waitpid(pids[i], &st[i], 0);
		}
	printf("execute finish %d\n", getpid());
}




void batch_mode(char * file){
	FILE *fp = fopen(file,"r");

	if (fp == NULL) {
		perror("Unable to open file!");
		exit(1);
	}

	char *line[ARGV_LEN];
	char input[ARGV_LEN];
	int i=0, j=0;
	while(fgets(input, sizeof(input), fp) != NULL){
		// printf("batchmode here\n");
		line[i] = (char *) malloc(sizeof(char)*COMMAND_LEN);
		// printf("line%s\n",line[i]);
		strcpy(line[i], input); 
		printf("[] %s\n", line[i]);
		// printf("miao%s\n",line);
		// printf("\ni: %d\n\n\n", i);
		
		// printf("\ni: %d\n\n\n", i);
		i++;

	}
	fclose(fp);
	for(j=0;j<i;j++){
		write(STDOUT_FILENO, ">>", 2);
		write(STDOUT_FILENO, line[j], strlen(line[j]));
		char *all_commands[COMMAND_NUM];    
		int status = extract_all_commands(line[j], all_commands);
		execute_all_commands(all_commands, status);
	}

	
}


/*
 Check if the the command is to enter batch mode
 if it is, then direct to the batch mode
*/
// int check_batch_mode(char * input_line){
// 	char *argvs[ARGV_LEN];
// 	int num = parse(input_line, argvs);
// 	if(num==2 && strcmp(argvs[0], "myshel")==0) {
// 		batch_mode(argvs[1]);
// 		return 1;
// 	} else {
// 		return 0;
// 	}
// }

int main(int argc, char *argv[]){
	if (argc == 2){
		// printf(argv[1]);
		// printf(argv[0]);
		batch_mode(argv[1]);
	} else {
		while(1){
			char input_line[COMMAND_LEN];
	   		char *all_commands[COMMAND_NUM];    

	   		// read in the stdin
	    	fgets(input_line, COMMAND_LEN, stdin); 

	    	// check batch mode
	    	// if(check_batch_mode(input_line)){
	    	// 	printf("finish batch_mode\n\n\n");
	    	// 	continue;
	    	// }
	    	// printf("batch finish\n");
	 
			// status = 1 means concurrent, 0 means serial
			int status = extract_all_commands(input_line, all_commands);
			execute_all_commands(all_commands, status);
		}
	}
	
	

}

