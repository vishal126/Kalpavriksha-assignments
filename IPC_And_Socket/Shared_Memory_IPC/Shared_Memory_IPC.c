#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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

int createSharedMemory(const key_t key, const unsigned int size)
{
    int shmId = shmget(key, sizeof(int) * size, 0644 | IPC_CREAT);
    if (shmId == -1)
    {
        perror("shmget");
        return -1;
    }
    return shmId;
}

int *attachSharedMemory(int shmId)
{
    int *ptr = shmat(shmId, NULL, 0);
    if (ptr == (void *)-1)
    {
        perror("shmat");
        return NULL;
    }
    return ptr;
}

void cleanupSharedMemory(int shmId, int *shmPtr)
{
    if (shmdt(shmPtr) == -1)
        perror("shmdt");

    if (shmctl(shmId, IPC_RMID, NULL)==-1) 
        perror("shmctl");
}

void printArray(const char *msg, const int *arrayData, const unsigned int sizeOfArray)
{
    printf("%s", msg);
    for (unsigned int printCounter = 0; printCounter < sizeOfArray; printCounter++)
        printf("%d ", arrayData[printCounter]);

    printf("\n\n");;
}

int main()
{
    unsigned int sizeOfArray = getIntInput("Enter size of Array: ");

    int *arrayData = NULL;

    if (!getArrayInput(&arrayData, sizeOfArray))
    {
        return 1;
    }

    printArray("Data before sorting (Process 1): ", arrayData, sizeOfArray);
    

    key_t key = ftok(".", 'S');
    if (key == -1)
    {
        perror("ftok");
        free(arrayData);
        return 1;
    }

    int shmId = createSharedMemory(key, sizeOfArray);
    if(shmId==-1)  {
        free(arrayData);
        return 1;
    }

    int *shmPtr = attachSharedMemory(shmId);
    if (!shmPtr)
    {
        free(arrayData);
        if (shmctl(shmId, IPC_RMID, NULL) == -1) 
            perror("shmctl");
        return 1;
    }

    memcpy(shmPtr, arrayData, sizeof(int)*sizeOfArray);

    int processId = fork();

    if (processId < 0)
    {
        perror("fork");
        cleanupSharedMemory(shmId, shmPtr);
        free(arrayData);
        return 1;
    }
    else if (processId == 0)
    {
        printf("Process 2 (PID: %d)\n", getpid());
        printf("Sorting...\n");

        qsort(shmPtr, sizeOfArray, sizeof(int), compare); 
        if (shmdt(shmPtr) == -1)
            perror("shmdt");

        return 0;
    }
    else {
        wait(NULL);

        printArray("Data after sorting (Process 1): ", shmPtr, sizeOfArray);

        cleanupSharedMemory(shmId, shmPtr);
    }

    free(arrayData);
    return 0;
}