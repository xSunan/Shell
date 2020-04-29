
#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>  


int main(int argc, char **argv) {
	char s[100];
	printf("PATH : %s\n", getenv("PATH"));
	printf("HOME : %s\n", getenv("HOME"));
 	printf("ROOT : %s\n", getenv("ROOT"));
 	printf("%s\n", getcwd(s, 100)); 
 	// using the command 
    chdir(".."); 
  
    // printing current working directory 
    printf("%s\n", getcwd(s, 100)); 
   	return(0);
}

void execute_cd(char **command) {
    int i = 1;
    if (command[1] == NULL) {
        //  "cd  " back to the home path
        char *HOME = getenv("HOME");
        chdir(HOME);
    } else if (command[2] ！= NULL{
        //error
    } else {
        chdir(command[1])
    }
}
