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

int main(){
	char input_line[COMMAND_LEN];
 
    char *all_commands[COMMAND_NUM];
    char *all_sub_cmds[COMMAND_NUM];
    

    fgets(input_line, COMMAND_LEN, stdin); 
	
	int concurrent = extract_all_commands(input_line, all_commands);
	
	int i=0;
	

	while(all_commands[i]!=NULL){
		sub_commands(all_commands[i++],all_sub_cmds);
	}


	


}



