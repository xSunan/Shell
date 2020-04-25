
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