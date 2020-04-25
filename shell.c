#include <stdio.h>
#include <string.h>
#define COMMAND_LEN 32
#define ARGV_LEN 150
#define COMMAND_NUM 10

int extract_all_commands(char* input, char** all_commands){
	int concurrent;
	const char delim[2]= " &;";

	if(strchr(input, '&') != NULL){
		concurrent = 1;
	} else if (strchr(input, ';') != NULL){
		concurrent = 0;	
	} 


	// divide multiple commands
	char *command = strtok(input, delim);   	
	int i=0;
	while(command != NULL ) {
		all_commands[i++] = command;
	  	command = strtok(NULL, delim);
	}
	return concurrent;
}

void sub_commands(char* command){

	/* find pipelined commands*/
	const char delim_pipe[] = " >|";
	char *all_sub_cmds[COMMAND_NUM];
	// parse arguements
	char *sub_cmd = strtok(command, delim_pipe);   	
	int i=0;
	while(sub_cmd != NULL ) {
		printf( "%s\n", sub_cmd );
		all_sub_cmds[i++] = sub_cmd;
	  	sub_cmd = strtok(NULL, delim_pipe);
	}

}

int main(){
	char input_line[COMMAND_LEN];
	const char delim[] = " \t"; 
  
    char *all_commands[COMMAND_NUM];
    char *command;
    
    // scanf("s", command)

    fgets(input_line, COMMAND_LEN, stdin); 
	// printf(input_line);
	
	int concurrent = extract_all_commands(input_line, all_commands);
	
	int i=0;

	while(all_commands[i]!=NULL){
		sub_commands(all_commands[i++]);
	}

	


}