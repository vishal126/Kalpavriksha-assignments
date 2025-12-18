#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX 100

typedef struct message
{
    long msgType;
    unsigned int size;
    int arrayData[MAX];
} message;

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

void getArrayInput(int *arrayData, const unsigned int sizeOfArray)
{
    for (unsigned int inputCounter = 0; inputCounter < sizeOfArray; inputCounter++)
    {
        printf("enter %u element: ", inputCounter + 1);
        arrayData[inputCounter] = getIntInput("");
    }
}

int compare(const void *value1, const void *value2)
{
    return (*(int *)value1 - *(int *)value2);
}

void printArray(const char *message, const int *arrayData, unsigned int size)
{
    printf("%s", message);
    for (unsigned int printCounter = 0; printCounter < size; printCounter++)
        printf("%d ", arrayData[printCounter]);

    printf("\n\n");
}

int createMessageQueue(const key_t key)
{
    int msgId = msgget(key, 0666 | IPC_CREAT);
    if (msgId == -1) {
        perror("msgget");
        return -1;
    }
        
    return msgId;
}

void removeMessageQueue(int msgId)
{
    if (msgctl(msgId, IPC_RMID, NULL) == -1)
        perror("msgctl");
}

bool childProcess(const int msgId, message *msg)
{
    printf("Process 2 (PID: %d)\n", getpid());

    qsort(msg->arrayData, msg->size, sizeof(int), compare);

    if (msgsnd(msgId, msg, sizeof(message) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        return false;
    }
    return true;
}

bool parentProcess(const int msgId, message *msg)
{
    wait(NULL);

    if (msgrcv(msgId, msg, sizeof(message) - sizeof(long), 1, 0) == -1)
    {
        perror("msgrcv");
        return false;
    }

    printArray("Data after sorting (Process 1): ", msg->arrayData, msg->size);
    return true;
}

int main()
{
    message msg;
    msg.size = getIntInput("Enter size of Array: ");
    if (msg.size > MAX)
    {
        printf("Maximum allowed size is %d\n", MAX);
        return 1;
    }
    msg.msgType = 1;

    getArrayInput(msg.arrayData, msg.size);

    printArray("Data before sorting (Process 1): ", msg.arrayData, msg.size);

    key_t key = ftok(".", 'A');
    if (key == -1)
    {
        perror("ftok");
        return 1;
    }

    int msgId = createMessageQueue(key);

    if (msgId == -1)
        return 1;

    int processId = fork();

    if (processId < 0)
    {
        printf("fork failed!\n");
        removeMessageQueue(msgId);
        return 1;
    }
    else if (processId == 0)
    {
        return childProcess(msgId, &msg) ? 0 : 1;
    }
    else
    {
        bool status = parentProcess(msgId, &msg);
        removeMessageQueue(msgId);
        return status ? 0 : 1;
    }
}