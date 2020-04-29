#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> 

void execute_echo(char **command) {
    int i = 1;
    while (command[i] != NULL) {
        write(STDOUT_FILENO, command[i], strlen(command[i]));
        write(STDOUT_FILENO, " ", 1);
        i++;
    }
    write(STDOUT_FILENO, "\n", 1);
}

void execute_pwd(char **command) {
    char *path = NULL;
    path = getcwd(NULL, 0);
    write(STDOUT_FILENO, path, strlen(path));
    write(STDOUT_FILENO, "\n", 1);
}

void execute_bye(char **command) {
    exit(0);
}

void execute_cd(char **command) {
    int i = 1;
    if (command[1] == NULL) {
        //  "cd  " back to the home path
        char *HOME = getenv("HOME");
        chdir(HOME);
    } else if (command[2] != NULL) {
        //error;
        return;
    } else {
        chdir(command[1]);
    }
    char *path = NULL;
    path = getcwd(NULL, 0);
    write(STDOUT_FILENO, path, strlen(path));
    write(STDOUT_FILENO, "\n", 1);
}

void execute_build_in_command(char **command) {
	if (strstr("echo", command[0]) != NULL) {
		execute_echo(command);
	}
    else if (strstr("pwd", command[0]) != NULL) {
        execute_pwd(command);
    }
    else if (strstr("cd", command[0]) != NULL) {
        execute_cd(command);
    }
    else if (strstr("bye", command[0]) != NULL) {
        execute_bye(command);
    }
    else{
        printf("invalid input");
        return;
    }
}