#include <stdio.h>
#include <string.h>

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

int strSplit(const char *input, char **substrings, char sep) {
    int start = 0, i = 0, substringsIndex = 0;
    while (input[i] != '\0') {
        if (input[i] == sep) {
            int charCounter = 0;
            for (int j = start; j < i; ++j, ++charCounter) {
                substrings[substringsIndex][charCounter] = input[j];
            }
            substrings[substringsIndex][charCounter] = '\0';
            start = i+1;
            ++substringsIndex;
        }
        ++i;
    }
    for (int j = start, k = 0; j < i; ++j, ++k) {
        substrings[substringsIndex][k] = input[j];
    }
    ++substringsIndex;
    return substringsIndex;
}

int main (int argc, char **argv) {
    // int input[10005];
    // int arraySize;
    char buffer[BUFSIZ];
    char *arguments[BUFSIZ];
    //Read input
    // scanf("%d", &arraySize);
    // for (int i = 0; i < arraySize; ++i) {
    //     scanf("%d", &input[i]);
    // }

    fgets(buffer, sizeof(buffer), stdin);
    argc = strSplit(buffer, arguments, ' ');
    printf("Splited\n");
    for (int i = 0; i < argc; ++i) {
        printf("%s\n", arguments[i]);
    }

    //Sort input
    // quicksort(0, arraySize-1, input);

    //Print input
    // printf("Sorted Array:\n");
    // for (int i = 0; i < arraySize; ++i) {
    //     printf("%d ", input[i]);
    // }
    // printf("\n");

    return 0;
}