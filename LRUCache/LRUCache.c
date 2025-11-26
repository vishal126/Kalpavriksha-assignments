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

unsigned short getIntInput(const char *prompt)
{
    char buffer[100];
    unsigned short value;
    while (1)
    {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0;

            if (sscanf(buffer, "%hu", &value) == 1)
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

    return 0;
}

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

void createCache(unsigned short capacity, LRUCache **cache, int *responseCode)
{
    *cache = malloc(sizeof(LRUCache));
    if (!*cache)
    {
        printf("Memory allocation failed!\n");
        *responseCode = -1;
        return;
    }

    (*cache)->front = (*cache)->rear = NULL;

    (*cache)->cacheCapacity = capacity;
    (*cache)->cacheSize = 0;

    (*cache)->hashTableSize = getNextPrime(capacity * 2);

    (*cache)->hashMap = malloc((*cache)->hashTableSize * sizeof(hashNode *));
    if (!(*cache)->hashMap)
    {
        printf("Memory allocation failed!\n");
        *responseCode = -1;
        free(*cache);
        (*cache) = NULL;
        return;
    }

    for (int hashMapCounter = 0; hashMapCounter < (*cache)->hashTableSize; hashMapCounter++)
    {
        (*cache)->hashMap[hashMapCounter] = NULL;
    }
}

int hashFunction(int key, LRUCache *cache)
{
    return (key % cache->hashTableSize + cache->hashTableSize) % cache->hashTableSize;
}

void addNodeToQueue(node *tempNode, LRUCache *cache)
{
    tempNode->prev = NULL;
    tempNode->next = NULL;

    if (cache->front == NULL)
    {
        cache->front = cache->rear = tempNode;
    }
    else
    {
        tempNode->next = cache->front;
        cache->front->prev = tempNode;
        cache->front = tempNode;
    }
    cache->cacheSize++;
}

void removeNode(LRUCache *cache)
{
    if (cache->rear == NULL)
        return;

    node *toDelete = cache->rear;

    if (cache->rear == cache->front)
    {
        cache->front = cache->rear = NULL;
    }
    else
    {
        cache->rear = cache->rear->prev;
        cache->rear->next = NULL;
    }

    if (toDelete->value)
        free(toDelete->value);
    free(toDelete);

    if (cache->cacheSize > 0)
        cache->cacheSize--;
}

void removeHashNode(int key, LRUCache *cache)
{
    int index = hashFunction(key, cache);
    hashNode *curr = cache->hashMap[index];
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
        cache->hashMap[index] = curr->next;
    }
    else
    {
        prev->next = curr->next;
    }

    free(curr);
    return;
}

void adjustQueueNode(node *adjustNode, LRUCache *cache)
{
    if (!adjustNode || cache->front == adjustNode)
        return;

    if (adjustNode->prev)
        adjustNode->prev->next = adjustNode->next;
    if (adjustNode->next)
        adjustNode->next->prev = adjustNode->prev;

    if (adjustNode == cache->rear)
    {
        cache->rear = adjustNode->prev;
    }

    adjustNode->prev = NULL;
    adjustNode->next = cache->front;
    if (cache->front)
        cache->front->prev = adjustNode;
    cache->front = adjustNode;

    if (cache->rear == NULL)
        cache->rear = cache->front;
}

void checkCacheCapacity(LRUCache *cache)
{
    if (cache->cacheSize >= cache->cacheCapacity)
    {
        int keyToDelete = cache->rear->key;
        removeNode(cache);
        removeHashNode(keyToDelete, cache);
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

int insertNewKey(LRUCache *cache, int index, int intKey, char *value)
{
    node *tempNode = initializeNode(intKey, value);
    if (!tempNode)
    {
        return 0;
    }

    hashNode *tempHashNode = malloc(sizeof(hashNode));
    if (!tempHashNode)
    {
        printf("Memory allocation failed!\n");
        if (tempNode->value)
            free(tempNode->value);
        free(tempNode);
        return 0;
    }

    tempHashNode->key = intKey;
    tempHashNode->queueNode = tempNode;
    tempHashNode->next = cache->hashMap[index];
    cache->hashMap[index] = tempHashNode;

    addNodeToQueue(tempNode, cache);
    return 1;
}

void put(char *key, char *value, LRUCache *cache)
{
    if (cache->cacheCapacity == 0)
    {
        printf("ERROR: Cache not initialized!\n");
        return;
    }

    int intKey;
    if (sscanf(key, "%d", &intKey) != 1)
    {
        printf("Invalid key! Key must be an integer.\n");
        return;
    }

    int index = hashFunction(intKey, cache);

    hashNode *curr = cache->hashMap[index];
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
        adjustQueueNode(curr->queueNode, cache);
        return;
    }

    checkCacheCapacity(cache);

    if (!insertNewKey(cache, index, intKey, value))
    {
        return;
    }
}

void get(char *key, LRUCache *cache)
{
    if (cache->cacheCapacity == 0)
    {
        printf("ERROR: Cache not initialized!\n");
        return;
    }

    int intKey;
    if (sscanf(key, "%d", &intKey) != 1)
    {
        printf("Invalid key! Key must be an integer.\n");
        return;
    }

    int index = hashFunction(intKey, cache);

    if (cache->hashMap[index] == NULL)
    {
        printf("No Value exists for key %d!\n", intKey);
    }
    else
    {
        hashNode *curr = cache->hashMap[index];

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
        adjustQueueNode(curr->queueNode, cache);
    }
}

enum Commands getEnumCommand(char *commandValue)
{
    if (stricmp(commandValue, "put") == 0)
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

void checkCommandValidity(const char *command, enum Commands type, int *error)
{
    unsigned short tokenCount = 0;
    const char *p = command;

    while (*p != '\0')
    {
        while (*p == ' ' || *p == '\t')
            p++;

        if (*p == '\0')
            break;

        tokenCount++;

        while (*p != ' ' && *p != '\t' && *p != '\0')
            p++;
    }

    if (type == PUT)
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

void getFirstToken(const char *command, char *tokenBuffer, size_t bufSize)
{
    size_t i = 0;
    size_t j = 0;

    while (command[i] == ' ' || command[i] == '\t')
        i++;

    if (command[i] == '\0')
    {
        tokenBuffer[0] = '\0';
        return;
    }

    while (command[i] != ' ' && command[i] != '\t' && command[i] != '\0' && j < bufSize - 1)
    {
        tokenBuffer[j++] = command[i++];
    }
    tokenBuffer[j] = '\0';
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

void parseCommand(int *returnCode, LRUCache *cache)
{
    char commandToken[15];
    char *command = readLine();
    if (command == NULL)
    {
        printf("Error while reading Input!\n");
        return;
    }

    getFirstToken(command, commandToken, sizeof(commandToken));
    if (commandToken[0] == '\0')
    {
        printf("Invalid Command!\n");
        free(command);
        return;
    }

    enum Commands commandType = getEnumCommand(commandToken);
    if (commandType == ERROR)
    {
        printf("Invalid Command!\n");
        free(command);
        return;
    }

    int error = 0;
    checkCommandValidity(command, commandType, &error);
    if (error != 0)
    {
        printf("Invalid Command!\n");
        free(command);
        return;
    }

    char *commandValue = strtok(command, " ");

    switch (commandType)
    {
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
        put(key, value, cache);
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
        get(key, cache);
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

void freeMemory(LRUCache *cache)
{
    if (cache->hashMap)
    {
        for (int i = 0; i < cache->hashTableSize; ++i)
        {
            hashNode *h = cache->hashMap[i];
            while (h)
            {
                hashNode *tmp = h;
                h = h->next;
                free(tmp);
            }
            cache->hashMap[i] = NULL;
        }
        free(cache->hashMap);
        cache->hashMap = NULL;
    }

    node *cur = cache->front;
    while (cur)
    {
        node *tmp = cur;
        cur = cur->next;
        if (tmp->value)
            free(tmp->value);
        free(tmp);
    }

    free(cache);
}

void initializeCache(LRUCache **cache, int *responseCode)
{
    unsigned short cacheSizeInput = getIntInput("Enter size of cache: ");

    while (cacheSizeInput < MIN_CAPACITY || cacheSizeInput > MAX_CAPACITY)
    {
        printf("Enter size in range(1-1000): ");
        cacheSizeInput = getIntInput("Enter size of cache: ");
    }

    createCache(cacheSizeInput, cache, responseCode);
}

int main()
{
    int returnCode = 0;
    LRUCache *cache = NULL;
    initializeCache(&cache, &returnCode);
    if (returnCode == -1)
    {
        return -1;
    }

    returnCode = 0;
    while (1)
    {

        printf("\nOperations:(CASE-INSENSITIVE)\n");
        printf("put <key> <value>\n");
        printf("get <key>\n");
        printf("exit\n\n");
        printf("Enter command: ");

        parseCommand(&returnCode, cache);

        if (returnCode == -1)
        {
            freeMemory(cache);
            printf("Releasing Memory!");
            exit(0);
        }
    }

    return 0;
}
