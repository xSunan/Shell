#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

int main() {
	  int link[2];
	  pid_t pid;
	  char foo[4096];

	  if (pipe(link)==-1)
	    die("pipe");

	  if ((pid = fork()) == -1)
	    die("fork");

	  if(pid == 0) {
	    // dup2 (link[1], STDOUT_FILENO);
	    // // close(link[0]);
	    // // close(link[1]);
	    char  *command1[10], *command2[10];
	    int st1, st2;
	    for (int i = 0; i < 10; i++) {
	        command1[i] = NULL;
	        command2[i] = NULL;
	    }

	    command1[0] = "ls";

	    command2[0] = "ls";
	    command2[1] = "..";

	   

	    // command[1] = "-l";
	    dup2 (link[1], STDOUT_FILENO);
	    execvp(*command1, command1);
	     
	    // close(link[0]);
    	// close(link[1]);
	    die("execvp");

	  } 
	  else {

	    // close(link[1]);
	    int nbytes = read(link[0], foo, sizeof(foo));
	    printf("Output: (%.*s)\n", nbytes, foo);
	    wait(NULL);

	  }
	  return 0;
 }