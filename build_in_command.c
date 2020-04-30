#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> 
#include <dirent.h>
#include <errno.h>


void raise_error() {
	char error_message[30] = "An ERROR has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}

void execute_echo(char **command) {
    if (command[1] == NULL) {
        raise_error();
        exit(0);
    }
    int count = 0, i = 1;
    char *token, *delimit = " ";

    if (strchr(command[i], '\"') != NULL) {
        delimit = "\"";
    }
    else if (strchr(command[i], '\'') != NULL) {
        delimit = "\'";
    }
    else {
        delimit = " ";
    }

    if (strcmp(delimit, " ") == 0) {
        token = strtok(command[i], " ");
        write(STDOUT_FILENO, token, strlen(token));
    }
    else {
        while (command[i] != NULL) {
            if (strchr(command[i], delimit) != NULL) {
                count++;
            }

            token = strtok(command[i], delimit);
            write(STDOUT_FILENO, token, strlen(token));
            write(STDOUT_FILENO, " ", 1);

            if (count == 2) {
                break;
            }
            i++;
        }
    }
    write(STDOUT_FILENO, "\n", 1);
}

void execute_pwd(char **command) {
    if(command[1] != NULL){
        raise_error();
        exit(0);
    }
    char *path = NULL;
    path = getcwd(NULL, 0);
    write(STDOUT_FILENO, path, strlen(path));
    write(STDOUT_FILENO, "\n", 1);
}

void execute_bye(char **command) {
    exit(0);
}

void execute_cd(char **command) {
    if (command[1] == NULL) {
        //  "cd  " back to the home path
        char *HOME = getenv("HOME");
        chdir(HOME);
    } else if (command[2] != NULL) {
        //error;
        return;
    } else {
        // check if the directory exists
        DIR* dir = opendir(command[1]);
        if (dir) {
            /* Directory exists. */
            closedir(dir);
        } else if (ENOENT == errno) {
            char error[] = "Error: the directory doesn't exists\n";
            write(STDERR_FILENO, error, strlen(error));
        }
        chdir(command[1]);
    }
    
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
        return;
    }
}
