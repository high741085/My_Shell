#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void quicksort(int, int, int []);
int strSplit(const char *, char, int []);

int main(int argc, char** argv) {
    //Main Program
    int fd[2];
    pipe(fd);
    int exit_status;
    //Subprogram
    int arr[10005];
    char buffer[BUFSIZ];

    pid_t myPID = fork();

    if (myPID == -1) {          //Error! fork() failure
        perror("fork()");
        exit(-1);
    } else if (myPID == 0) {    //Child Process: read input and pass the input buffer to parent
        close(fd[0]);
        printf("[Child] PID: %d\n", getpid());
        printf("[Child] Quicksort algorithm!\n");
        printf("[Child] Enter elements(The maximum number of elements is 10000): ");
        fgets(buffer, sizeof(buffer), stdin);
        printf("[Child] %s\n", buffer);
        write(fd[1], buffer, sizeof(buffer));
        close(fd[1]);
        exit_status = 1;
        exit(1);
    } else {                    //Parent Process: do quicksort and print the result on screen
        close(fd[1]);
        printf("[Parent] PID: %d\n", getpid());
        wait(&exit_status);
        read(fd[0], buffer, sizeof(buffer));
        unsigned int arrSize = strlen(buffer) / 2;
        //Split strings into int array and sort it.
        int index = strSplit(buffer, ' ', arr);
        printf("[Parent] Sorting...\n");
        quicksort(0, (short)arrSize, arr);
        //Print the sorted array
        printf("[Parent] Sorted Array: ");
        for (unsigned int i = 1; i <= arrSize; ++i) {
            printf("%d ", arr[i]);
        }
        printf("\n");    
        close(fd[0]);
    }
    return 0;
}

void quicksort(int leftBound, int rightBound, int arr[]) {
    if (leftBound < rightBound) {
        //Partition
        int down = leftBound, up = rightBound;
        int pivot = arr[leftBound];
        while (down < up) {
            while (arr[down] <= pivot) {
                down++;
            }
            while (up > 0 && arr[up] > pivot) {
                up--;
            }
            if (down < up) {
                int temp = arr[down];
                arr[down] = arr[up];
                arr[up] = temp;
            }
        }
        int temp = arr[up];
        arr[up] = pivot;
        arr[leftBound] = temp;
        //Recursive doing quicksort
        quicksort(leftBound, up - 1, arr);
        quicksort(up + 1, rightBound, arr);
    }
}
//TODO: Split strings
int strSplit(const char *input, char sep, int arr[]) {
    char substring[20] = {'\0'};
    int start = 0, i = 0, substringsIndex = 0, charCounter, arrIndex = 0;
    while (input[i] != '\n') {
        if (input[i] == sep) {
            charCounter = 0;

            for (int j = start; j < i; ++j, ++charCounter) {
                substring[charCounter] = input[j];
            }
            substring[charCounter] = '\0';
            arr[arrIndex] = atoi(substring);

            arrIndex++;
            start = i+1;
        }
        ++i;
    }
    charCounter = 0;
    for (int j = start; j < i; ++j) {
        substring[charCounter] = input[j];
        charCounter++;
    }
    substring[charCounter] = '\0';
    arr[arrIndex] = atoi(substring);
    arrIndex++;

    return arrIndex;
}