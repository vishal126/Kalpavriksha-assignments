#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define FILENAME "data.txt"

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

bool allocateAndReadArray(int **arrayData, const unsigned int sizeOfArray)
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

bool writeArrayToFile(const int *arrayData, const unsigned int sizeOfArray)
{
    FILE *fileToWrite = fopen(FILENAME, "w");

    if (!fileToWrite)
    {
        printf("Error opening file for Writing!\n");
        return false;
    }

    for (unsigned int writeCounter = 0; writeCounter < sizeOfArray; writeCounter++)
    {
        fprintf(fileToWrite, "%d ", arrayData[writeCounter]);
    }
    fclose(fileToWrite);

    return true;
}

bool readArrayFromFile(int *arrayToWrite, const unsigned int sizeOfArray)
{
    FILE *fileToRead = fopen(FILENAME, "r");

    if (!fileToRead)
    {
        printf("Error opening file for Reading!\n");
        return false;
    }

    for (unsigned readCounter = 0; readCounter < sizeOfArray; readCounter++)
    {
        if (fscanf(fileToRead, "%d", &arrayToWrite[readCounter]) != 1)
        {
            printf("Error while reading Input From File!\n");
            fclose(fileToRead);
            return false;
        }
    }
    fclose(fileToRead);

    return true;
}

int compare(const void *value1, const void *value2)
{
    return (*(int *)value1 - *(int *)value2);
}

void printArray(const char *message, const int *arrayData, const unsigned int sizeOfArray)
{
    printf("%s", message);
    for (unsigned int printCounter = 0; printCounter < sizeOfArray; printCounter++)
        printf("%d ", arrayData[printCounter]);

    printf("\n\n");
}

bool runChildProcess(int *arrayData, const unsigned int sizeOfArray)
{
    printf("Child Process (PID: %d): Sorting...\n", getpid());

    qsort(arrayData, sizeOfArray, sizeof(int), compare);

    return writeArrayToFile(arrayData, sizeOfArray);
}

bool runParentProcess(int *arrayData, const unsigned int sizeOfArray)
{
    wait(NULL);

    printf("Parent Process: Reading sorted data\n");

    if (!readArrayFromFile(arrayData, sizeOfArray))
        return false;

    printArray("Sorted array:", arrayData, sizeOfArray);

    if (remove(FILENAME) != 0)
    {
        perror("Error removing file");
    }
    else
    {
        printf("Parent Process (PID: %d): Temporary file removed.\n", getpid());
    }
    return true;
}

int main()
{
    unsigned int sizeOfArray = getIntInput("Enter size of Array: ");

    int *arrayData = NULL;

    bool inputResponse = allocateAndReadArray(&arrayData, sizeOfArray);
    if (!inputResponse)
    {
        return 1;
    }

    if (!writeArrayToFile(arrayData, sizeOfArray))
    {
        free(arrayData);
        return 1;
    }

    printf("Parent Process (PID: %d):\n", getpid());
    printf("Original array before Sorting: ");
    for (unsigned int printCounter = 0; printCounter < sizeOfArray; printCounter++)
    {
        printf("%d ", arrayData[printCounter]);
    }
    printf("\n\n");

    pid_t pid = fork();

    if (pid < 0)
    {
        free(arrayData);
        printf("Fork failed!\n");
        return 1;
    }
    else if (pid == 0)
    {
        bool status = runChildProcess(arrayData, sizeOfArray);
        free(arrayData);
        printf("Child Process (PID: %d): sorted, and wrote data back to file.\n", getpid());
        return status ? 0 : 1;
    }
    else
    {
        printf("Parent Process (PID: %d). Reading sorted data from file.\n", getpid());
        bool status = runParentProcess(arrayData, sizeOfArray);
        free(arrayData);
        return status ? 0 : 1;
    }

    free(arrayData);
}