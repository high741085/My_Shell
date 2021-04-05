#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_CMD_AMOUNT 1024
#define MAX_ARG_AMOUNT 1024

typedef struct fd_pair {
    int fd[2];
} fd_pair;

fd_pair io_fd[1024];
int argvAmount;
char *tempFileFormat = ".temp";

char *readCommand();
char ***commandSequenceToArgvs(char *);
int findStringIndex(char **, char *);
int execCommand(char ***);
void createProc(char **, int, int);

int main(int argc, char** argv) {
    while (1) {
        //Declaration
        char ***argvs; 
        //Read input
        char *buffer = readCommand();
        argvs = commandSequenceToArgvs(buffer);
        int exit_status = execCommand(argvs);
        if (exit_status) {break;}
        
    }
    return 0;
}

char *readCommand() {
    static char buffer[BUFSIZ];

    //Print prompt
    memset(buffer, '\0', sizeof(buffer));
    getcwd(buffer, sizeof(buffer));
    printf("%s$ ", buffer);
    fflush(stdout);

    //Read Input
    memset(buffer, '\0', sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);

    return buffer;
}

void createProc(char **argv, int fd_in, int fd_out) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Error: fork() failure");
        exit(-1);
    } else if (pid == 0) {
        if (fd_in != STDIN_FILENO) {
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        if (fd_out != STDOUT_FILENO) {
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        int redir_index = findStringIndex(argv, ">");
        if (redir_index != -1) {
            fd_out = creat(argv[redir_index + 1], 0644);
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
            argv[redir_index] = NULL;
        }

        int exit_status = execvp(argv[0], argv);
        if (exit_status == -1) {
            fprintf(stderr, "Unable to load executable %s.\n", argv[0]);
            exit(-1);
        }
    } else {
        wait(NULL);
    }
}

int execCommand(char ***argvs) {
    int exit_status = 0;

    memset(io_fd, 0, sizeof(io_fd));
    
    
    for (int C = 0; C < MAX_CMD_AMOUNT; ++C) {
        char **argv = argvs[C];
        
        if (!argv) {
            break;
        }

        if (strcmp(argv[0], "exit") == 0) {
            exit_status = 1;
            break;
        }
        // set fd_in and fd_out
        if (C == 0) {
            if (argvs[C+1]) {
                
            } else {
                io_fd[0].fd[0] = STDIN_FILENO;
                io_fd[0].fd[1] = STDOUT_FILENO;
            }
        } else {
            
        }
        createProc(argv, io_fd[C].fd[0], io_fd[C].fd[1]);
    }
    
    return exit_status;
}

char ***commandSequenceToArgvs(char *input) {
    static char *cmds[MAX_CMD_AMOUNT+1];
    memset(cmds, '\0', sizeof(cmds));

    cmds[0] = strtok(input, "|");
    for (int i = 1; i <= MAX_CMD_AMOUNT; ++i) {
        cmds[i] = strtok(NULL, "|");
        if (cmds[i] == NULL) 
            break;
    }

    static char *argvs_temp[MAX_CMD_AMOUNT + 1][MAX_ARG_AMOUNT + 1];
    static char **argvs[MAX_CMD_AMOUNT + 1];
    memset(argvs_temp, '\0', sizeof(argvs_temp));
    memset(argvs, '\0', sizeof(argvs));

    for (int i = 0; cmds[i]; ++i) {
        argvs[i] = argvs_temp[i];
        argvs[i][0] = strtok(cmds[i], " \t\r\n");
        for (int j = 1; j <= MAX_ARG_AMOUNT; ++j) {
            argvs[i][j] = strtok(NULL, " \t\r\n");
            if (argvs[i][j] == NULL)
                break;
        }
        argvAmount = i;
    }

    return argvs;
}

int findStringIndex(char **args, char *target) {
    int argsIndex = -1;
    for (int i = 0; args[i]; ++i) {
        if (strcmp(args[i], target) == 0) {
            argsIndex = i;
            break;
        }
    }
    return argsIndex;
}