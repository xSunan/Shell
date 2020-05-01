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
        return;
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
            int len = strlen( command[i] );
            for (int k = 0; k < len; k++){
                if(command[i][k] == delimit[0])
                    count++;
            }
            if (count > 2) {
                break;
            }

            token = strtok(command[i], delimit);
            if(token!=NULL) {
                write(STDOUT_FILENO, token, strlen(token));
                write(STDOUT_FILENO, " ", 1);
            }
            i++;
        }
    }
    write(STDOUT_FILENO, "\n", 1);
}

void execute_pwd(char **command) {
    if(command[1] != NULL){
        raise_error();
        return;
    }
    char *path = NULL;
    path = getcwd(NULL, 0);
    write(STDOUT_FILENO, path, strlen(path));
    write(STDOUT_FILENO, "\n", 1);
}

void execute_bye(char **command) {
    if (command[1] != NULL){
        raise_error();
        return;
    }
    exit(0);
}

void execute_cd(char **command) {
    if (command[1] == NULL) {
        char *HOME = getenv("HOME");
        chdir(HOME);
    } else if (command[2] != NULL) {
        raise_error();
        return;
    } else {
        DIR* dir = opendir(command[1]);
        if (dir) {
            closedir(dir);
        } else if (ENOENT == errno) {
            raise_error();
            return;
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
        raise_error();
        return;
    }
}
