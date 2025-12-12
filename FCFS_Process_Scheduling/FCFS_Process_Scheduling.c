#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define TICK_MS 1000

enum ProcessState
{
    READY,
    WAITING,
    TERMINATED,
    KILLED
};

typedef struct Pcb
{
    enum ProcessState processState;
    unsigned int processId;
    char *processName;
    unsigned int burstTime;
    unsigned int ioStartTime;
    unsigned int ioDuration;
    unsigned int remainingIoTime;
    unsigned int executionTime;
    unsigned int killTime;
    unsigned int completionTime;
} Pcb;

typedef struct QueueNode
{
    struct Pcb *pcbPointer;
    struct QueueNode *next;
} QueueNode;

typedef struct HashNode
{
    unsigned int key;
    Pcb *pcbPointer;
    struct HashNode *next;
} HashNode;

typedef struct FCFS
{
    QueueNode *readyQueueFront;
    QueueNode *readyQueueRear;
    QueueNode *waitingQueueFront;
    QueueNode *waitingQueueRear;
    QueueNode *terminatedQueueFront;
    QueueNode *terminatedQueueRear;
    HashNode **hashMap;
    unsigned int hashMapSize;
} FCFS;

unsigned int hashFunction(const unsigned int key, const FCFS *details)
{
    return (key % details->hashMapSize + details->hashMapSize) % details->hashMapSize;
}

void freeTempPcb(Pcb **pcbToFree)
{
    if ((*pcbToFree))
    {
        if ((*pcbToFree)->processName)
        {
            free((*pcbToFree)->processName);
        }

        free(*pcbToFree);
    }
}

HashNode *initializeHashNode(Pcb *pcbDetails)
{
    HashNode *tempNode = (HashNode *)malloc(sizeof(HashNode));
    if (!tempNode)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    tempNode->key = pcbDetails->processId;
    tempNode->next = NULL;
    tempNode->pcbPointer = pcbDetails;

    return tempNode;
}

QueueNode *initializeQueueNode(Pcb *pcbDetails)
{
    QueueNode *queueNodeToReturn = (QueueNode *)malloc(sizeof(QueueNode));

    if (!queueNodeToReturn)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    queueNodeToReturn->next = NULL;
    queueNodeToReturn->pcbPointer = pcbDetails;

    return queueNodeToReturn;
}

bool pushInReadyQueue(Pcb *pcbDetails, FCFS **details)
{
    QueueNode *readyQueueNodeToPush = initializeQueueNode(pcbDetails);

    if (!readyQueueNodeToPush)
    {
        printf("Memory allocation failed!\n");
        return false;
    }

    if ((*details)->readyQueueFront == NULL)
    {
        (*details)->readyQueueFront = readyQueueNodeToPush;
    }
    else
    {
        (*details)->readyQueueRear->next = readyQueueNodeToPush;
    }
    (*details)->readyQueueRear = readyQueueNodeToPush;
    return true;
}

bool checkForPcb(const FCFS *details, const unsigned int index, const unsigned int pId)
{
    if (!details->hashMap[index])
        return false;

    HashNode *traversePtr = details->hashMap[index];

    while (traversePtr)
    {
        if (traversePtr->key == pId)
        {
            return true;
        }

        traversePtr = traversePtr->next;
    }

    return false;
}

bool insertInHashMap(Pcb *pcbDetails, FCFS **details)
{
    unsigned int index = hashFunction(pcbDetails->processId, *details);

    HashNode *newNode = initializeHashNode(pcbDetails);
    if (!newNode) return false;

    newNode->next = (*details)->hashMap[index];
    (*details)->hashMap[index] = newNode;

    return pushInReadyQueue(pcbDetails, details);
}

bool insertKillValue(FCFS **details, const unsigned int processId, const unsigned int killTime)
{
    unsigned int index = hashFunction(processId, *details);

    HashNode *traversePtr = (*details)->hashMap[index];

    while (traversePtr)
    {
        if (traversePtr->key == processId)
        {
            traversePtr->pcbPointer->killTime = killTime;
            return true;
        }
        traversePtr = traversePtr->next;
    }

    printf("No process exists with id %u!\n", processId);
    return false;
}

bool parseCommand(char *inputLine, char *command, FCFS **details)
{
    Pcb *tempPcb = (Pcb *)malloc(sizeof(Pcb));
    if (!tempPcb)
    {
        printf("Memory allocation failed!");
        return false;
    }
    tempPcb->processName = (char *)malloc(strlen(command) + 1);

    if (!tempPcb->processName)
    {
        freeTempPcb(&tempPcb);
        printf("Memory allocation failed!");
        return false;
    }

    if (stricmp(command, "kill") == 0)
    {
        if (sscanf(inputLine, "%s %u %u", tempPcb->processName, &tempPcb->processId, &tempPcb->killTime) != 3)
        {
            printf("Wrong Input Format!\n");
            freeTempPcb(&tempPcb);
            return false;
        }
        unsigned int index = hashFunction(tempPcb->processId, *details);

        if ((*details)->hashMap[index] == NULL)
        {
            printf("No process with id %u exists!", tempPcb->processId);
            freeTempPcb(&tempPcb);
            return false;
        }

        bool killValueResponse = insertKillValue(details, tempPcb->processId, tempPcb->killTime);
        freeTempPcb(&tempPcb);

        return true;
    }
    else
    {
        if (sscanf(inputLine, "%s %u %u %u %u", tempPcb->processName, &tempPcb->processId, &tempPcb->burstTime, &tempPcb->ioStartTime, &tempPcb->ioDuration) != 5)
        {
            printf("Wrong Input Format!\n");
            freeTempPcb(&tempPcb);
            return false;
        }

        tempPcb->executionTime = 0;
        tempPcb->killTime = 0;
        tempPcb->completionTime = 0;
        tempPcb->remainingIoTime = tempPcb->ioDuration;
        tempPcb->processState = READY;

        if (checkForPcb(*details, hashFunction(tempPcb->processId, *details), tempPcb->processId))
        {
            printf("Process with id %u already exists!\n", tempPcb->processId);
            freeTempPcb(&tempPcb);
            return true;
        }
        bool hashMapInsertResponse = insertInHashMap(tempPcb, details);

        if (!hashMapInsertResponse)
        {
            freeTempPcb(&tempPcb);
            return false;
        }
    }

    return true;
}

char *readLine()
{
    int ch;
    size_t size = 128;
    size_t len = 0;

    char *buffer = malloc(size);
    if (!buffer)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        buffer[len++] = ch;

        if (len + 1 >= size)
        {
            size *= 2;

            char *newBuffer = malloc(size);
            if (!newBuffer)
            {
                free(buffer);
                printf("Memory allocation failed!\n");
                return NULL;
            }
            buffer = newBuffer;
        }
    }

    if (ch == EOF && len == 0)
    {
        free(buffer);
        return NULL;
    }

    buffer[len] = '\0';
    return buffer;
}

char *ltrim(char *inputLine)
{
    if (!inputLine)
    {
        return NULL;
    }

    if (!*inputLine)
    {
        return inputLine;
    }

    while (*inputLine != '\0' && isspace(*inputLine))
    {
        inputLine++;
    }

    return inputLine;
}

char *rtrim(char *inputLine)
{
    if (!inputLine)
    {
        return NULL;
    }

    if (!*inputLine)
    {
        return inputLine;
    }

    char *end = inputLine + strlen(inputLine) - 1;

    while (end >= inputLine && isspace(*end))
    {
        end--;
    }

    *(end + 1) = '\0';

    return inputLine;
}

bool readInput(FCFS **details)
{
    while (1)
    {
        char *rawLine = readLine();
        char *inputLine = rawLine;
        if (inputLine == NULL)
        {
            break;
        }
        inputLine = ltrim(inputLine);
        inputLine = rtrim(inputLine);

        if (stricmp(inputLine, "") == 0)
        {
            break;
        }

        size_t len = strcspn(inputLine, " ");
        char *command = malloc(len + 1);
        if(!command) {
            free(rawLine);
            return false;
        }
        memcpy(command, inputLine, len);
        command[len] = '\0';

        bool parseCommandResponse = parseCommand(inputLine, command, details);

        if (rawLine != NULL)
            free(rawLine);
        free(command);

        if (!parseCommandResponse)
        {
            return false;
        }       
    }

    return true;
}

bool initializeProgram(FCFS **details)
{
    (*details)->readyQueueFront = NULL;
    (*details)->readyQueueRear = NULL;
    (*details)->waitingQueueFront = NULL;
    (*details)->waitingQueueRear = NULL;
    (*details)->terminatedQueueFront = NULL;
    (*details)->terminatedQueueRear = NULL;
    (*details)->hashMapSize = 1009;

    (*details)->hashMap = malloc(sizeof(HashNode *) * (*details)->hashMapSize);
    if (!(*details)->hashMap)
    {
        printf("Memory allocation failed!\n");
        return false;
    }
    for (unsigned short hashMapCounter = 0; hashMapCounter < (*details)->hashMapSize; hashMapCounter++)
    {
        (*details)->hashMap[hashMapCounter] = NULL;
    }
    return true;
}

Pcb *popFromReadyQueue(FCFS **details)
{
    if (!(*details)->readyQueueFront)
        return NULL;

    QueueNode *node = (*details)->readyQueueFront;
    Pcb *pcbToReturn = node->pcbPointer;

    (*details)->readyQueueFront = node->next;
    if ((*details)->readyQueueFront == NULL)
    {
        (*details)->readyQueueRear = NULL;
    }

    free(node);
    return pcbToReturn;
}

bool pushInWaitingQueue(FCFS **details, Pcb *pcbToPush)
{
    QueueNode *nodeToPush = initializeQueueNode(pcbToPush);

    if (!nodeToPush)
    {
        printf("Memory allocation failed!\n");
        return false;
    }

    if (!(*details)->waitingQueueFront)
    {
        (*details)->waitingQueueFront = nodeToPush;
    }
    else
    {
        (*details)->waitingQueueRear->next = nodeToPush;
    }

    (*details)->waitingQueueRear = nodeToPush;

    return true;
}

Pcb *popFromWaitingQueue(FCFS **details)
{
    if (!(*details)->waitingQueueFront)
        return NULL;

    QueueNode *node = (*details)->waitingQueueFront;
    Pcb *pcbToReturn = node->pcbPointer;

    (*details)->waitingQueueFront = node->next;
    if ((*details)->waitingQueueFront == NULL)
    {
        (*details)->waitingQueueRear = NULL;
    }

    free(node);

    return pcbToReturn;
}

bool pushToTerminatedQueue(FCFS **details, Pcb *pcbToPush)
{
    QueueNode *nodeToPush = initializeQueueNode(pcbToPush);

    if (!nodeToPush)
    {
        printf("Memory allocation failed!\n");
        return false;
    }

    if ((*details)->terminatedQueueFront == NULL)
    {
        (*details)->terminatedQueueFront = nodeToPush;
    }
    else
    {
        (*details)->terminatedQueueRear->next = nodeToPush;
    }
    (*details)->terminatedQueueRear = nodeToPush;

    return true;
}

bool updateWaitingQueue(FCFS **details, const unsigned int currentTime)
{
    QueueNode *prev = NULL;
    QueueNode *curr = (*details)->waitingQueueFront;

    while (curr)
    {
        if (curr->pcbPointer->remainingIoTime == 0)
        {
            QueueNode *toMove = curr;
            QueueNode *next = curr->next;

            if (prev == NULL)
            {
                (*details)->waitingQueueFront = next;
            }
            else
            {
                prev->next = next;
            }

            if (next == NULL)
            {
                (*details)->waitingQueueRear = prev;
            }

            toMove->pcbPointer->processState = READY;
            bool pushResponse = pushInReadyQueue(toMove->pcbPointer, details);
            if (!pushResponse)
            {
                free(toMove);
                return false;
            }

            free(toMove);

            curr = next;
            continue;
        }

        if (curr->pcbPointer->remainingIoTime > 0)
        {
            curr->pcbPointer->remainingIoTime--;
        }

        prev = curr;
        curr = curr->next;
    }

    return true;
}

bool schedular(FCFS **details)
{
    unsigned int currentTime = 0;
    Pcb *currentProcess = NULL;

    while (1)
    {
        if (!currentProcess && (*details)->readyQueueFront)
        {
            currentProcess = popFromReadyQueue(details);
        }

        if (!currentProcess && !(*details)->readyQueueFront && !(*details)->waitingQueueFront)
        {
            break;
        }

        Sleep(TICK_MS);
        currentTime++;

        if (currentProcess)
        {
            currentProcess->executionTime++;

            if (currentProcess->killTime > 0 && currentProcess->executionTime == currentProcess->killTime)
            {
                currentProcess->processState = KILLED;
                currentProcess->completionTime = currentProcess->executionTime;

                if (!pushToTerminatedQueue(details, currentProcess))
                {
                    return false;
                }
                currentProcess = NULL;
            }
            else if (currentProcess->ioStartTime > 0 && currentProcess->executionTime == currentProcess->ioStartTime && currentProcess->remainingIoTime > 0)
            {
                currentProcess->processState = WAITING;

                if (!pushInWaitingQueue(details, currentProcess))
                {
                    return false;
                }
                currentProcess = NULL;
            }
            else if (currentProcess->executionTime == currentProcess->burstTime)
            {
                currentProcess->processState = TERMINATED;
                currentProcess->completionTime = currentTime;

                if (!pushToTerminatedQueue(details, currentProcess))
                {
                    return false;
                }
                currentProcess = NULL;
            }
        }

        if (!updateWaitingQueue(details, currentTime))
        {
            return false;
        }
    }

    return true;
}

void printResult(const Pcb *pcb)
{
    if (pcb->processState == KILLED)
    {
        char killedMsg[30];
        sprintf(killedMsg, "KILLED at %u", pcb->completionTime);

        printf("%-5u %-12s %-5u %-5u %-15s %-10s %-10s\n",
               pcb->processId,
               pcb->processName,
               pcb->burstTime,
               pcb->ioDuration,
               killedMsg,
               "-",
               "-");
    }
    else
    {
        unsigned int turnaround = pcb->completionTime;
        unsigned int waiting = turnaround - pcb->burstTime;

        printf("%-5u %-12s %-5u %-5u %-15s %-10u %-10u\n",
               pcb->processId,
               pcb->processName,
               pcb->burstTime,
               pcb->ioDuration,
               "OK",
               turnaround,
               waiting);
    }
}

void displayTerminated(FCFS *details)
{
    QueueNode *curr = details->terminatedQueueFront;

    printf("%-5s %-12s %-5s %-5s %-15s %-10s %-10s\n",
           "PID", "Name", "CPU", "IO", "Status", "Turnaround", "Waiting");

    while (curr)
    {
        printResult(curr->pcbPointer);
        curr = curr->next;
    }
}

void freeQueues(QueueNode *nodesToFree)
{
    while (nodesToFree)
    {
        QueueNode *next = nodesToFree->next;
        free(nodesToFree);
        nodesToFree = next;
    }
}

void freeMemory(FCFS **details)
{
    if (!(*details))
    {
        return;
    }

    freeQueues((*details)->readyQueueFront);
    (*details)->readyQueueFront = (*details)->readyQueueRear = NULL;

    freeQueues((*details)->waitingQueueFront);
    (*details)->waitingQueueFront = (*details)->waitingQueueRear = NULL;

    freeQueues((*details)->terminatedQueueFront);
    (*details)->terminatedQueueFront = (*details)->terminatedQueueRear = NULL;

    for (int hashMapCounter = 0; hashMapCounter < (*details)->hashMapSize; hashMapCounter++)
    {
        HashNode *nodeToFree = (*details)->hashMap[hashMapCounter];

        while (nodeToFree)
        {
            HashNode *next = nodeToFree->next;
            if (nodeToFree)
            {
                if (nodeToFree->pcbPointer)
                    freeTempPcb(&nodeToFree->pcbPointer);

                free(nodeToFree);
            }
            nodeToFree = next;
        }
    }

    free((*details)->hashMap);
}

int main()
{
    FCFS *details = (FCFS *)malloc(sizeof(FCFS));
    if (!details)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    bool response = initializeProgram(&details);
    if (!response)
    {
        freeMemory(&details);
        printf("Memory allocation failed!\n");
        return 1;
    }

    response = true;
    response = readInput(&details);
    if (!response)
    {
        freeMemory(&details);
        printf("Memory allocation failed!\n");
        return 1;
    }

    bool schedularResponse = schedular(&details);
    if (!schedularResponse)
    {
        freeMemory(&details);
        return 1;
    }

    displayTerminated(details);

    freeMemory(&details);

    return 0;
}