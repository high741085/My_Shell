#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

int strSplit(const char *, char **, char);
int findStringIndex(char **, char *, int);

int main(int argc, char** argv) {
    while (1) {
        //Declaration
        int exit_status;
        int fd[2];
        char buffer[BUFSIZ];
        char *arguments[BUFSIZ]; 

        pipe(fd);   //Need to pipe whenever a new child process is created.
        pid_t myPID = fork();

        if (myPID == -1) {
            perror("fork()");
            exit(-1);
        } else if (myPID == 0) {
            // printf("[Child] PID: %d\n", getpid());
            close(fd[0]);
            //Print prompt
            printf("> ");
            fgets(buffer, sizeof(buffer), stdin);
            argc = strSplit(buffer, arguments, ' ');
            //If the command is "exit", then exit shell
            if (strcmp(arguments[0], "exit") == 0) {
                write(fd[1], arguments[0], sizeof(arguments[0]));
            }
            //Check is there any ">". If there is ">", then set output to the file given by user
            int redir_index = findStringIndex(arguments, ">", argc);
            if (redir_index != -1) {
                fd[1] = creat(arguments[redir_index + 1], 0644);
                dup2(fd[1], 1);
                close(fd[1]);
                arguments[redir_index] = NULL;
            }

            // printf("[Child] arguments[0] = %s.\n", arguments[0]);
            execvp(arguments[0], arguments);
            // printf("[Child] command done\n");
            exit(0);
        } else {
            close(fd[1]);
            // printf("[Parent] PID: %d\n", getpid());
            wait(&exit_status);
            read(fd[0], buffer, sizeof(buffer));
            if (strcmp(buffer, "exit") == 0) {
                break;
            }
        }
    }
    return 0;
}

int strSplit(const char *input, char **substrings, char sep) {
    int start = 0, i = 0, substringsIndex = 0, charCounter;
    while (input[i] != '\n') {
        if (input[i] == sep) {
            charCounter = 0;
            for (int j = start; j < i; ++j, ++charCounter) {
                substrings[substringsIndex][charCounter] = input[j];
            }
            substrings[substringsIndex][charCounter] = '\0';
            start = i+1;
            ++substringsIndex;
        }
        ++i;
    }
    charCounter = 0;
    for (int j = start; j < i; ++j) {
        substrings[substringsIndex][charCounter] = input[j];
        charCounter++;
    }
    substrings[substringsIndex][charCounter] = '\0';
    ++substringsIndex;
    substrings[substringsIndex] = 0;
    return substringsIndex;
}

int findStringIndex(char **args, char *target, int args_size) {
    int argsIndex = -1;
    for (int i = 0; i < args_size; ++i) {
        if (strcmp(args[i], target) == 0) {
            argsIndex = i;
            break;
        }
    }
    return argsIndex;
}