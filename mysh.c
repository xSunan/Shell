#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "build_in_command.c"

#define COMMAND_LEN 1024
#define ARGV_LEN 1024
#define COMMAND_NUM 50

char *build_in_commands = "cd echo pwd bye";

/*

Divide the input line into multiple commands
 return the relationship between multiple command.
 0 means that the commands should run consecutively  (";")
 1 means that the commands should tun concurrently   ("&")

*/

int extract_all_commands(char* input, char** all_commands){
	int concurrent=0;
	const char delim[3]= "&;";
	if (strchr(input, '&') != NULL && strchr(input, ';') != NULL) {
		raise_error();
		all_commands[0] = NULL;
		return 0;
	} else if(strchr(input, '&') != NULL){
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
	// printf("extract_all finish\n");
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

	if (strchr(command, '>') != NULL && strchr(command, '|') != NULL){
		raise_error();
		all_sub_cmds[0] = NULL;
		return 0;
	}else if(strchr(command, '>') != NULL){
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
		all_sub_cmds[i] = (char *) malloc(sizeof(char)*ARGV_LEN);
		all_sub_cmds[i++] =sub_cmd;
	  	sub_cmd = strtok(NULL, delim_pipe);
	}
	all_sub_cmds[i] = NULL;
	return redirection;
}	


int parse(char *command, char **argv)
{
	const char delim[] = " \n\t";
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
	// printf("parse count: %d\n", i);
	return i;
}

pid_t pipe_line(char** all_sub_cmds, int concurrent){
	pid_t pid, total_pid;
	int i=0, status;
	int dir_output[2];
	total_pid = fork();

	if(total_pid == 0){
		while(all_sub_cmds[i] != NULL){
			// printf("inside: cmd %s \n", all_sub_cmds[i]);
			if(pipe(dir_output) == -1){
				exit(EXIT_FAILURE);
			}

			if((pid = fork())==0){
				char *argvs[ARGV_LEN];
				parse(all_sub_cmds[i], argvs);
				close(dir_output[0]);
				if (all_sub_cmds[i+1] != NULL && dir_output[1]!=STDOUT_FILENO){
					dup2 (dir_output[1], STDOUT_FILENO);
					close(dir_output[1]);
				} 
				if (execvp(*argvs, argvs) < 0) {
					exit(1);
				} 
				exit(0);
				

			} else {
				close(dir_output[1]);
				if (dup2(dir_output[0], STDIN_FILENO) == -1) {
					exit(EXIT_FAILURE);
				}
				close(dir_output[0]);
				waitpid(pid,NULL,0);
			} 

			i++;
		}
		exit(0);
	} else {
		if (concurrent == 0) {
			while (wait(&status) != total_pid) {
				continue;
			}
		}
	}
	
	// printf("finish");
	return total_pid;
}

pid_t redirect(char** all_sub_cmds, int concurrent) {
	pid_t pid;
    char *command[ARGV_LEN];
    char *filename[ARGV_LEN];
    int file_desc, status;

    parse(all_sub_cmds[0], command);
    parse(all_sub_cmds[1], filename);
    if (filename[0] == NULL || filename[1] != NULL ) {
        // invalid filename
        raise_error();
        return -1;
    }

	if (creat(filename[0], 0777) < 0) {
		raise_error();
		return -1;
	}

	if ((pid = fork()) < 0) {
		raise_error();
		return -1;
	}
	else if (pid == 0) {
		// child process
		file_desc = open(filename[0] ,O_WRONLY | O_APPEND);
		dup2(file_desc, STDOUT_FILENO);
		if (strstr(build_in_commands, command[0]) != NULL) {
			execute_build_in_command(command);
			exit(0);
		}
		else if (execvp(*command, command) < 0) {
			exit(1);
		}
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


pid_t execute(char *command, int concurrent)
{
    pid_t pid;
    int status, argv_num;
	char *argvs[ARGV_LEN];

	argv_num = parse(command, argvs);
	if(argv_num == 0){
		return -1;
	}
	if (strstr(build_in_commands, argvs[0]) != NULL) {
		if (strcmp(argvs[0],"bye")==0 && concurrent==1) {
			raise_error();
			return -1;
		}
		execute_build_in_command(argvs);
		return -1;
	}
	else {
		if ((pid = fork()) < 0) {
			exit(1);
		}
		else if (pid == 0) {
			if (execvp(*argvs, argvs) < 0) {
				exit(1);
			}
		}
		else  {
			if (concurrent == 0) {
				while (wait(&status) != pid) {
					continue;
				}
			}
		}
	}
    return pid;
}

int execute_all_commands(char **all_commands, int status)
{
	pid_t pids[COMMAND_NUM];
	int st[COMMAND_NUM],count=0,k=0, redirection;
	char *command;
	char *all_sub_cmds[COMMAND_NUM];

	if (status == 1){
		while(all_commands[k]!=NULL){
			if(strstr(build_in_commands, all_commands[k]) != NULL) {
				raise_error();
				return 0;
			}
			k++;
		}
		
	}
	k=0;
	while(all_commands[k]!=NULL){
		command = all_commands[k];
		if(strlen(command) >= 66){
			raise_error();
			k++;
			continue;
		}
        redirection = sub_commands(command, all_sub_cmds);
        if (redirection == 0){
			pids[count] = execute(all_sub_cmds[0], status);
        } else if(redirection == 1){
        	pids[count] = redirect(all_sub_cmds, status);
        } else {
			pids[count] = pipe_line(all_sub_cmds, status);
        }

		count++;
		k++;
	}
	
	for (int i = 0; i < count; i++) {
		waitpid(pids[i], &st[i], 0);
	}
	return 0;
}



int batch_mode(char * file){
	if( access( file, F_OK ) == -1 ){
		raise_error();
		return 1;
	}
	FILE *fp = fopen(file,"r");

	if (fp == NULL) {
		raise_error();
		exit(1);
	}

	char *line[ARGV_LEN];
	char input[ARGV_LEN];
	int i=0, j=0;
	while(fgets(input, sizeof(input), fp) != NULL){
		line[i] = (char *) malloc(sizeof(char)*ARGV_LEN);
		strcpy(line[i], input); 	
		i++;
	}
	fclose(fp);
	for(j=0;j<i;j++){
		write(STDOUT_FILENO, line[j], strlen(line[j]));
		char *all_commands[COMMAND_NUM];    
		int status = extract_all_commands(line[j], all_commands);
		execute_all_commands(all_commands, status);
	}
	return 0;
}


int main(int argc, char *argv[]){
	if(argc>2){
		raise_error();
		return 1;
	} else if (argc == 2){
		int status = batch_mode(argv[1]);
		return status;
	} else {
		char input_line[ARGV_LEN];
		char prompt[] = "520shell> ";
		write(STDOUT_FILENO, prompt, strlen(prompt));
		while(fgets(input_line, ARGV_LEN, stdin)!=NULL){
			char *all_commands[COMMAND_NUM];
			int status = extract_all_commands(input_line, all_commands);
			execute_all_commands(all_commands, status);
			write(STDOUT_FILENO, prompt, strlen(prompt));
		}
	}
}

