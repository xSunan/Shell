#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define COMMAND_LEN 50
#define ARGV_LEN 150
#define COMMAND_NUM 20

int extract_all_commands(char* input, char** all_commands){
	int concurrent;
	const char delim[3]= "&;\n";

	if(strchr(input, '&') != NULL){
		concurrent = 1;
	} else if (strchr(input, ';') != NULL){
		concurrent = 0;	
	} 

	// divide multiple commands
	char *command = strtok(input, delim);   	
	int i=0;
	while(command != NULL ) {
		all_commands[i] = (char *) malloc(sizeof(char)*COMMAND_LEN);
		all_commands[i++] = command;
	  	command = strtok(NULL, delim);
	}
	all_commands[i] = NULL;
	return concurrent;
}

void sub_commands(char* command, char** all_sub_cmds){

	/* find pipelined commands*/
	const char delim_pipe[] = ">|\n";

	// parse arguements
	char *sub_cmd = strtok(command, delim_pipe);   	
	int i=0;
	while(sub_cmd != NULL && strlen(sub_cmd)>0) {
		all_sub_cmds[i] = (char *) malloc(sizeof(char)*COMMAND_LEN);
		all_sub_cmds[i++] =sub_cmd;
	  	sub_cmd = strtok(NULL, delim_pipe);
	}

}

void  parse(char *line, char **argv)
{
	while (*line != '\0') {
		while (*line == ' ' || *line == '\t' || *line == '\n')
			*line++ = '\0';
		*argv++ = line;
		while (*line != '\0' && *line != ' ' && 
				*line != '\t' && *line != '\n') 
			line++;
	}
	*argv = '\0';
}

pid_t execute(char **command, int concurrent)
{
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    }
    else if (pid == 0) {
        // child process
        sleep(2);
        if (execvp(*command, command) < 0) {
            printf("*** ERROR: exec failed\n");
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

void execute_all_commands(char **all_commands, int status)
{
	char *single_command[64];
	pid_t pids[COMMAND_NUM];
	int st[COMMAND_NUM], count=0;
	while(*all_commands!=NULL){
		parse(*all_commands++, single_command);
		pids[count++] = execute(single_command, status);
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
    char *command;

    fgets(input_line, COMMAND_LEN, stdin); 
	
	// status = 1 means concurrent, 0 means serial
	int status = extract_all_commands(input_line, all_commands);
	char *all_sub_cmds[COMMAND_NUM];
	
	execute_all_commands(all_commands, status);
}



