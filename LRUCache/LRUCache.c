#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MIN_CAPACITY 1
#define MAX_CAPACITY 1000

typedef struct node
{
    int key;
    char *value;
    struct node *prev;
    struct node *next;
} node;

typedef struct hashNode
{
    int key;
    struct node *queueNode;
    struct hashNode *next;
} hashNode;

enum Commands
{
    CREATECACHE,
    PUT,
    GET,
    EXIT,
    ERROR
};

typedef struct LRUCache
{
    hashNode **hashMap;
    node *front;
    node *rear;
    unsigned short cacheCapacity;
    unsigned short cacheSize;
    unsigned short hashTableSize;
} LRUCache;

LRUCache cache;

bool isPrime(unsigned short primeNumber)
{
    unsigned short cnt = 0;
    for (unsigned short i = 1; i * i <= primeNumber; i++)
    {
        if (primeNumber % i == 0)
        {
            cnt++;
            if (i != primeNumber / i)
            {
                cnt++;
            }
        }
    }
    if (cnt > 2)
        return false;
    else
        return true;
}

unsigned short getNextPrime(unsigned short capacity)
{
    unsigned short primeNumber = capacity;
    while (!isPrime(primeNumber))
    {
        primeNumber++;
    }

    return primeNumber;
}

void createCache(char *capacity)
{
    cache.front = cache.rear = NULL;

    if (cache.cacheCapacity != 0)
    {
        printf("ERROR cache already initialized with size %hu!\n", cache.cacheCapacity);
        return;
    }
    unsigned short intCapacity;
    if (sscanf(capacity, "%hu", &intCapacity) != 1)
    {
        printf("Invalid Capacity!\n");
        return;
    }

    if (intCapacity < MIN_CAPACITY || intCapacity > MAX_CAPACITY)
    {
        printf("Capacity is not in range(1-1000)!\n");
        return;
    }

    cache.cacheCapacity = intCapacity;
    cache.cacheSize = 0;

    cache.hashTableSize = getNextPrime(intCapacity * 2);

    cache.hashMap = malloc(cache.hashTableSize * sizeof(hashNode *));

    if (!cache.hashMap)
    {
        printf("Memory allocation failed!\n");
        cache.cacheCapacity = 0;
        cache.hashTableSize = 0;
        cache.hashMap = NULL;
        return;
    }

    for (int hashMapCounter = 0; hashMapCounter < cache.hashTableSize; hashMapCounter++)
    {
        cache.hashMap[hashMapCounter] = NULL;
    }

    printf("Cache created successfully with size %hu\n", intCapacity);
}

int hashFunction(int key)
{
    return (key % cache.hashTableSize + cache.hashTableSize) % cache.hashTableSize;
}

void addNodeToQueue(node *tempNode)
{
    tempNode->prev = NULL;
    tempNode->next = NULL;

    if (cache.front == NULL)
    {
        cache.front = cache.rear = tempNode;
    }
    else
    {
        tempNode->next = cache.front;
        cache.front->prev = tempNode;
        cache.front = tempNode;
    }
    cache.cacheSize++;
}

void removeNode()
{
    if (cache.rear == NULL)
        return;

    node *toDelete = cache.rear;

    if (cache.rear == cache.front)
    {
        cache.front = cache.rear = NULL;
    }
    else
    {
        cache.rear = cache.rear->prev;
        cache.rear->next = NULL;
    }

    if (toDelete->value)
        free(toDelete->value);
    free(toDelete);

    if (cache.cacheSize > 0)
        cache.cacheSize--;
}

void removeHashNode(int key)
{
    int index = hashFunction(key);
    hashNode *curr = cache.hashMap[index];
    hashNode *prev = NULL;

    while (curr != NULL && curr->key != key)
    {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL)
    {
        return;
    }

    if (prev == NULL)
    {
        cache.hashMap[index] = curr->next;
    }
    else
    {
        prev->next = curr->next;
    }

    free(curr);
    return;
}

void adjustQueueNode(node *adjustNode)
{
    if (!adjustNode || cache.front == adjustNode)
        return;

    if (adjustNode->prev)
        adjustNode->prev->next = adjustNode->next;
    if (adjustNode->next)
        adjustNode->next->prev = adjustNode->prev;

    if (adjustNode == cache.rear)
    {
        cache.rear = adjustNode->prev;
    }

    adjustNode->prev = NULL;
    adjustNode->next = cache.front;
    if (cache.front)
        cache.front->prev = adjustNode;
    cache.front = adjustNode;

    if (cache.rear == NULL)
        cache.rear = cache.front;
}

void checkCacheCapacity()
{
    if (cache.cacheSize >= cache.cacheCapacity)
    {
        int keyToDelete = cache.rear->key;
        removeNode();
        removeHashNode(keyToDelete);
    }
}

node *initializeNode(int key, char *value)
{
    node *tempNode = malloc(sizeof(node));
    if (!tempNode)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    tempNode->next = NULL;
    tempNode->prev = NULL;
    tempNode->key = key;
    tempNode->value = malloc(strlen(value) + 1);
    if (!tempNode->value)
    {
        printf("Memory allocation failed!\n");
        free(tempNode);
        return NULL;
    }
    strcpy(tempNode->value, value);
    return tempNode;
}

void put(char *key, char *value)
{
    if (cache.cacheCapacity == 0)
    {
        printf("ERROR: Cache not initialized!\n");
        return;
    }

    int intKey;
    sscanf(key, "%d", &intKey);

    int index = hashFunction(intKey);

    if (cache.hashMap[index] == NULL)
    {
        checkCacheCapacity();

        cache.hashMap[index] = malloc(sizeof(hashNode));
        if (!cache.hashMap[index])
        {
            printf("Memory allocation failed!\n");
            cache.hashMap[index] = NULL;
            return;
        }
        cache.hashMap[index]->key = intKey;
        cache.hashMap[index]->next = NULL;

        node *tempNode = initializeNode(intKey, value);
        if (!tempNode)
        {
            free(cache.hashMap[index]);
            return;
        }
        cache.hashMap[index]->queueNode = tempNode;

        addNodeToQueue(tempNode);
    }
    else
    {
        hashNode *curr = cache.hashMap[index];
        while (curr != NULL && curr->key != intKey)
        {
            curr = curr->next;
        }

        if (curr != NULL)
        {
            char *tempValue = realloc(curr->queueNode->value, strlen(value) + 1);
            if (!tempValue)
            {
                printf("Memory reallocation failed!\n");
                return;
            }
            curr->queueNode->value = tempValue;
            strcpy(curr->queueNode->value, value);
            adjustQueueNode(curr->queueNode);
            return;
        }

        checkCacheCapacity();

        hashNode *tempHashNode = malloc(sizeof(hashNode));
        if (!tempHashNode)
        {
            printf("Memory allocation failed!\n");
            return;
        }
        tempHashNode->queueNode = NULL;
        tempHashNode->next = cache.hashMap[index];
        cache.hashMap[index] = tempHashNode;
        tempHashNode->key = intKey;

        node *tempNode = initializeNode(intKey, value);
        if (!tempNode)
        {
            cache.hashMap[index] = tempHashNode->next;
            free(tempHashNode);
            return;
        }
        tempHashNode->queueNode = tempNode;

        addNodeToQueue(tempNode);
    }

    printf("PUT operation successful with key %hu value \"%s\"\n", intKey, value);
}

void get(char *key)
{
    if (cache.cacheCapacity == 0)
    {
        printf("ERROR: Cache not initialized!\n");
        return;
    }

    int intKey;
    sscanf(key, "%d", &intKey);

    int index = hashFunction(intKey);

    if (cache.hashMap[index] == NULL)
    {
        printf("No Value exists for key %d!\n", intKey);
    }
    else
    {
        hashNode *curr = cache.hashMap[index];

        while (curr != NULL && curr->key != intKey)
        {
            curr = curr->next;
        }

        if (curr == NULL)
        {
            printf("No Value exists for key %d!\n", intKey);
            return;
        }

        printf("%s\n", curr->queueNode->value);
        adjustQueueNode(curr->queueNode);
    }
}

enum Commands getEnumCommand(char *commandValue)
{
    if (stricmp(commandValue, "createCache") == 0)
    {
        return CREATECACHE;
    }
    else if (stricmp(commandValue, "put") == 0)
    {
        return PUT;
    }
    else if (stricmp(commandValue, "get") == 0)
    {
        return GET;
    }
    else if (stricmp(commandValue, "exit") == 0)
    {
        return EXIT;
    }
    else
    {
        return ERROR;
    }
}

void checkCommandValidity(char *command, enum Commands type, int *error)
{
    unsigned short tokenCount = 0;

    char *token = strtok(command, " ");
    while (token)
    {
        tokenCount++;
        token = strtok(NULL, " ");
    }

    if (type == CREATECACHE)
    {
        if (tokenCount != 2)
        {
            *error = -1;
            return;
        }
    }
    else if (type == PUT)
    {
        if (tokenCount < 3)
        {
            *error = -1;
            return;
        }
    }
    else if (type == GET)
    {
        if (tokenCount != 2)
        {
            *error = -1;
            return;
        }
    }
    else if (type == EXIT)
    {
        if (tokenCount != 1)
        {
            *error = -1;
            return;
        }
    }
    else
    {
        *error = -1;
        return;
    }
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
        buffer[len++] = (char)ch;
        if (len + 1 >= size)
        {
            size *= 2;
            char *newBuffer = realloc(buffer, size);
            if (!newBuffer)
            {
                free(buffer);
                printf("Memory reallocation failed!\n");
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

void parseCommand(int *returnCode)
{
    char *command = readLine();
    if (command == NULL)
    {
        printf("Error while reading Input!\n");
        return;
    }
    int sizeOfCopyCmd = strlen(command) + 1;
    char *commandCopy = malloc(sizeOfCopyCmd);

    strcpy(commandCopy, command);

    char *commandValue = strtok(commandCopy, " ");
    if (!commandValue)
    {
        printf("Invalid Command!\n");
        free(command);
        free(commandCopy);
        return;
    }
    enum Commands commandType = getEnumCommand(commandValue);
    if (commandType == ERROR)
    {
        printf("Invalid Command!\n");
        free(commandCopy);
        free(command);
        return;
    }
    int error = 0;
    strcpy(commandCopy, command);
    checkCommandValidity(commandCopy, commandType, &error);
    if (error != 0)
    {
        printf("Invalid Command!\n");
        free(commandCopy);
        free(command);
        return;
    }

    commandValue = strtok(command, " ");
    free(commandCopy);
    switch (commandType)
    {
    case CREATECACHE:
    {
        char *capacity = strtok(NULL, "");
        if (capacity == NULL)
        {
            printf("Invalid command!\n");
            free(command);
            return;
        }
        createCache(capacity);
        break;
    }

    case PUT:
    {
        char *key = strtok(NULL, " ");
        char *value = strtok(NULL, "");
        if (key == NULL || value == NULL)
        {
            printf("Invalid command!\n");
            free(command);
            return;
        }
        put(key, value);
        break;
    }

    case GET:
    {
        char *key = strtok(NULL, "");
        if (key == NULL)
        {
            printf("Invalid command!\n");
            free(command);
            return;
        }
        get(key);
        break;
    }

    case EXIT:
        free(command);
        *returnCode = -1;
        return;

    case ERROR:
        printf("Invalid Command!\n");
        free(command);
        return;

    default:
        break;
    }

    free(command);
}

void freeMemory()
{
    if (cache.hashMap)
    {
        for (int i = 0; i < cache.hashTableSize; ++i)
        {
            hashNode *h = cache.hashMap[i];
            while (h)
            {
                hashNode *tmp = h;
                h = h->next;
                free(tmp);
            }
            cache.hashMap[i] = NULL;
        }
        free(cache.hashMap);
        cache.hashMap = NULL;
    }

    node *cur = cache.front;
    while (cur)
    {
        node *tmp = cur;
        cur = cur->next;
        if (tmp->value)
            free(tmp->value);
        free(tmp);
    }
    cache.front = cache.rear = NULL;

    cache.cacheSize = 0;
    cache.cacheCapacity = 0;
    cache.hashTableSize = 0;
}

int main()
{
    cache.cacheCapacity = 0;
    cache.cacheSize = 0;
    cache.front = NULL;
    cache.hashMap = NULL;
    cache.hashTableSize = 0;
    cache.rear = NULL;

    int returnCode = 0;
    while (1)
    {

        printf("\nOperations:(CASE-INSENSITIVE)\n");
        printf("createcache <size>\n");
        printf("put <key> <value>\n");
        printf("get <key>\n");
        printf("exit\n\n");
        printf("Enter command: ");

        parseCommand(&returnCode);

        if (returnCode == -1)
        {
            freeMemory();
            printf("Releasing Memory!");
            exit(0);
        }
    }

    return 0;
}