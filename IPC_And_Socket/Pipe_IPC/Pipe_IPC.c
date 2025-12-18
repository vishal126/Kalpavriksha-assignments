#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

unsigned int getIntInput(const char *prompt)
{
    unsigned int value;
    char buffer[120];

    while (1)
    {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0;
            if (sscanf(buffer, "%u", &value) == 1)
            {
                return value;
            }
            else
            {
                printf("Invalid input. Please enter a valid integer.\n");
            }
        }
        else
        {
            printf("Error reading input. Try again.\n");
        }
    }
}

bool getArrayInput(int **arrayData, const unsigned int sizeOfArray)
{
    (*arrayData) = (int *)malloc(sizeof(int) * sizeOfArray);
    if (!(*arrayData))
    {
        printf("Memory allocation failed!\n");
        return false;
    }

    for (unsigned int inputCounter = 0; inputCounter < sizeOfArray; inputCounter++)
    {
        printf("enter %u element: ", inputCounter + 1);
        (*arrayData)[inputCounter] = getIntInput("");
    }

    return true;
}

int compare(const void *value1, const void *value2)
{
    return (*(int *)value1 - *(int *)value2);
}

int main() {   
    unsigned int sizeOfArray = getIntInput("Enter size of Array: ");

    int *arrayData = NULL;

    if(!getArrayInput(&arrayData, sizeOfArray)) {
        return 1;
    }

    printf("Data before sorting (Process 1: (PID: %d)): ", getpid());
    for (unsigned int printCounter = 0; printCounter < sizeOfArray; printCounter++)
    {
        printf("%d ", arrayData[printCounter]);
    }
    printf("\n\n");

    int pipeArray[2];

    if(pipe(pipeArray)==-1) {
        perror("pipe");
        free(arrayData);
        return 1;
    }

    int processId = fork();

    if (processId < 0)
    {
        printf("fork failed!\n");
        free(arrayData);
        return 1;
    }
    else if (processId == 0)
    {
        printf("Process 2 (PID: %d)\n", getpid());
        printf("Sorting...\n");
        if(close(pipeArray[0])==-1) {
            printf("Read end close failed!\n");
            free(arrayData);
            return 1;
        }

        qsort(arrayData, sizeOfArray, sizeof(int), compare);

        if(write(pipeArray[1], arrayData, sizeOfArray*sizeof(int))==-1) {
            printf("Write failed!\n");
            free(arrayData);
            return 1;
        }

        if(close(pipeArray[1])==-1) {
            printf("Write end close failed!\n");
            free(arrayData);
            return 1;
        }
        return 0;
    }
    else {
        wait(NULL);

        if(close(pipeArray[1])==-1) {
            printf("Write end close failed!\n");
            free(arrayData);
            return 1;
        }

        if(read(pipeArray[0], arrayData, sizeOfArray*sizeof(int))==-1) {
            printf("Read failed!\n");
            free(arrayData);
            return 1;
        }

        printf("Data after sorting (Process 1: (PID: %d)): ", getpid());
        for (int printCounter = 0; printCounter < sizeOfArray; printCounter++)
        {
            printf("%d ", arrayData[printCounter]);
        }
        printf("\n\n");

        if(close(pipeArray[0])==-1) {
            printf("Read end close failed!\n");
            free(arrayData);
            return 1;
        }
    }

    free(arrayData);

    return 0;
}