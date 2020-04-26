#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define COMMAND_LEN 50
#define ARGV_LEN 150
#define COMMAND_NUM 20

/*

Divide the input line into multiple commands
 return the relationship between multiple command.
 0 means that the commands should run consecutively  (";")
 1 means that the commands should tun concurrently   ("&")

*/
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

/* 

 Divide the redirection parts in a single commands
 return the property of the command.
 0 means that no file redirection or pipeline
 1 means that exists file redirection ('>')
 2 means that exists pipeline ('|')

*/
int sub_commands(char* command, char** all_sub_cmds){

	/* find pipelined commands*/
	const char delim_pipe[] = ">|\n";
	int redirection;
	if(strchr(input, '>') != NULL){
		redirection = 1;
	} else if (strchr(input, '|') != NULL){
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

}

int main(){
	char input_line[COMMAND_LEN];
 
    char *all_commands[COMMAND_NUM];    

    fgets(input_line, COMMAND_LEN, stdin); 
	
	int concurrent = extract_all_commands(input_line, all_commands);
	
	pid_t child_pid;
	int i=0;
	
	// a |b|g|h ; c>d; e>f
	while(all_commands[i]!=NULL){

		
			// a>b 
			char *sub_cmds[COMMAND_NUM] = sub_commands(all_commands[i],all_sub_cmds);
			// [a, b, g, h]
			while(){
				execute();
			}

		
		
	}


	


}



