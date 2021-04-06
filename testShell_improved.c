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

int argvAmount;
char *tempFileFormat = ".temp";

char *readCommand();
char ***commandSequenceToArgvs(char *);
int findStringIndex(char **, char *, int);
int execCommand(char ***);
void createProc(char **, int, int);
void redirect_to(char **, int, int);


int main(int argc, char** argv) 
{
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

        int redir_index = findStringIndex(argv, "<", 0);
        if (redir_index != -1) {
            fd_in = open(argv[redir_index + 1], O_RDONLY);
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
            argv[redir_index] = NULL;
        }

        // for (int i = 1; argv[redir_index + i]; ++i) {
        //     printf("[Child] argv:%s.\n", argv[redir_index + i]);
        // }

        if (findStringIndex(argv, ">", redir_index + 1) > -1) {
            printf("Redirect to...\n");
            redirect_to(argv, fd_out, redir_index + 1);
            exit(1);
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
    int fd[2], fd_in = STDIN_FILENO, fd_out = STDOUT_FILENO;
    pipe(fd);
    
    for (int argvCnt = 0; argvCnt < MAX_CMD_AMOUNT; ++argvCnt) {
        char **argv = argvs[argvCnt];
        
        if (!argv) {
            break;
        }

        if (strcmp(argv[0], "exit") == 0) {
            exit_status = 1;
            break;
        }
        // set fd_in and fd_out
        if (argvCnt > 0) {
            fd_in = fd[0];
        }
        if (argvCnt < MAX_CMD_AMOUNT && argvs[argvCnt + 1] != NULL) {
            fd_out = fd[1];
        }
        createProc(argv, fd_in, fd_out);

        if (fd_in != STDIN_FILENO) close(fd_in);
        if (fd_out != STDOUT_FILENO) close(fd_out);
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

void redirect_to(char **argv, int fd_out, int start) {
    char *cmd = argv[0];
    int i = start;
    while (argv[i]) {
        if (strcmp(argv[i], ">") == 0) {
            fd_out = creat(argv[i + 1], 0644);
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
            argv[i] = NULL;
            ++i;
            printf("[redirect_to] %d\n", i);

            pid_t pid = fork();
            if (pid == 0) {
                execvp(cmd, argv);
            }
        } else {
            ++i;
        }
    }
}

int findStringIndex(char **args, char *target, int start) {
    int argsIndex = -1;
    for (int i = start; args[i]; ++i) {
        if (strcmp(args[i], target) == 0) {
            argsIndex = i;
            break;
        }
    }
    return argsIndex;
}