#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUMBEROFBLOCKS 20
#define SIZEOFBLOCK 512
#define MAXNAMELENGTH 50

typedef struct FreeBlock
{
    struct FreeBlock *prev;
    unsigned short index;
    struct FreeBlock *next;
} FreeBlock;

typedef struct FileNode
{
    char name[51];
    unsigned short blockCount;
    unsigned short *blockPointers;
    bool type;
    struct FileNode *parent;
    struct FileNode *child;
    struct FileNode *next;
} FileNode;

typedef struct FileSystem
{
    FreeBlock *doublyListHead;
    FreeBlock *doublyListTail;
    FileNode *cwd;
    char currentPath[1024];
    unsigned short freeBlocks;
} FileSystem;

enum commands
{
    MKDIR = 1,
    CREATE,
    WRITE,
    READ,
    DELETE,
    RMDIR,
    LS,
    CD,
    PWD,
    DF,
    ERROR
};

char virtualDisk[NUMBEROFBLOCKS][SIZEOFBLOCK];

FileSystem fileSystem = {
    .doublyListHead = NULL,
    .doublyListTail = NULL,
    .cwd = NULL,
    .currentPath = "",
    .freeBlocks = NUMBEROFBLOCKS
};

short int initializeFreeBlocks()
{
    for (int dllCounter = 0; dllCounter < NUMBEROFBLOCKS; dllCounter++)
    {
        FreeBlock *tempBlock = (FreeBlock *)calloc(1, sizeof(FreeBlock));
        if (tempBlock == NULL)
        {
            printf("Failed to Initialize Free Blocks!\nEnding program.");
            return -1;
        }
        tempBlock->index = dllCounter;
        tempBlock->next = NULL;
        tempBlock->prev = fileSystem.doublyListTail;

        if (fileSystem.doublyListHead == NULL)
        {
            fileSystem.doublyListHead = tempBlock;
        }
        else
        {
            fileSystem.doublyListTail->next = tempBlock;
        }
        fileSystem.doublyListTail = tempBlock;
    }

    return 0;
}

short int allocateBlocksFromFreeBlocks()
{
    if (fileSystem.doublyListHead == NULL)
    {
        printf("No free blocks left\n");
        return -1;
    }

    FreeBlock *tempBlock = fileSystem.doublyListHead;

    fileSystem.doublyListHead = fileSystem.doublyListHead->next;

    if (fileSystem.doublyListHead != NULL)
    {
        fileSystem.doublyListHead->prev = NULL;
    }
    else
    {
        fileSystem.doublyListTail = NULL;
    }

    unsigned short blockIndex = tempBlock->index;
    free(tempBlock);

    fileSystem.freeBlocks--;

    return blockIndex;
}

short int allocateBlocksToFreeBlocks(const unsigned int index)
{
    FreeBlock *tempBlock = (FreeBlock *)calloc(1, sizeof(FreeBlock));
    if (tempBlock == NULL)
    {
        printf("Unable to allocate memory to free Block!\nTry again.\n");
        return -1;
    }

    tempBlock->index = index;
    tempBlock->next = NULL;
    tempBlock->prev = fileSystem.doublyListTail;

    memset(virtualDisk[index], 0, SIZEOFBLOCK);

    if (fileSystem.doublyListHead == NULL)
    {
        fileSystem.doublyListHead = tempBlock;
    }
    else
    {
        fileSystem.doublyListTail->next = tempBlock;
    }

    fileSystem.doublyListTail = tempBlock;
    fileSystem.freeBlocks++;
}

FileNode *createNode(const char *name, const bool type)
{
    FileNode *tempNode = (FileNode *)calloc(1, sizeof(FileNode));
    if (tempNode == NULL)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    strcpy(tempNode->name, name);
    tempNode->next = NULL;
    tempNode->parent = fileSystem.cwd;
    tempNode->type = type;
    tempNode->blockCount = 0;
    tempNode->blockPointers = NULL;
    tempNode->child = NULL;

    return tempNode;
}

void insertInCwd(FileNode *newNode)
{
    if (fileSystem.cwd->child == NULL)
    {
        fileSystem.cwd->child = newNode;
        newNode->next = newNode;
    }
    else
    {
        FileNode *head = fileSystem.cwd->child;
        if (head->next == head)
        {
            head->next = newNode;
            newNode->next = head;
        }
        else
        {
            FileNode *temp = head->next;
            head->next = newNode;
            newNode->next = temp;
        }
    }
    newNode->parent = fileSystem.cwd;
}

int isValidDirName(const char *dirName)
{
    if (dirName == NULL || strlen(dirName) == 0 || strlen(dirName) > MAXNAMELENGTH)
        return 0;

    if (strchr(dirName, '/') || strchr(dirName, '"') || strchr(dirName, '\'') || strchr(dirName, '\\'))
        return 0;

    return 1;
}

int isValidFileName(const char *fileName)
{
    if (fileName == NULL || strlen(fileName) == 0 || strlen(fileName) > MAXNAMELENGTH)
        return 0;

    if (strchr(fileName, ' ') || strchr(fileName, '/') || strchr(fileName, '"') ||
        strchr(fileName, '\'') || strchr(fileName, '\\'))
        return 0;

    return 1;
}

short int checkForDuplicate(const char *name)
{
    FileNode *check = fileSystem.cwd->child;
    if (check != NULL)
    {
        do
        {
            if (strcmp(check->name, name) == 0)
            {
                printf("Error: '%s' already exists in '%s'.\n", name, fileSystem.cwd->name);
                return -1;
            }
            check = check->next;
        } while (check != fileSystem.cwd->child);
    }
    return 0;
}

void mkdir(const char *dirName)
{
    if (!isValidDirName(dirName))
    {
        printf("Invalid directory name '%s'.\n", dirName);
        printf("Rules: up to 50 chars, no /, quotes, or backslashes.\n");
        return;
    }

    short int responseCode = checkForDuplicate(dirName);

    if (responseCode == -1)
    {
        return;
    }

    FileNode *directory = createNode(dirName, true);
    if (directory == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    insertInCwd(directory);
    printf("Directory '%s' created under '%s'\n", dirName, fileSystem.cwd->name);
}

void create(const char *fileName)
{
    if (!isValidFileName(fileName))
    {
        printf("Invalid file name '%s'.\n", fileName);
        printf("Rules: up to 50 chars, no spaces, /, quotes, or backslashes.\n");
        return;
    }

    short int responseCode = checkForDuplicate(fileName);

    if (responseCode == -1)
    {
        return;
    }

    FileNode *file = createNode(fileName, false);
    if (file == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    insertInCwd(file);
    printf("File '%s' created under '%s'\n", fileName, fileSystem.cwd->name);
}

void writeFile(FileNode *node, const char *content)
{
    size_t len = strlen(content);
    size_t blocksRequired = (len + SIZEOFBLOCK - 1) / SIZEOFBLOCK;

    if (fileSystem.freeBlocks < blocksRequired)
    {
        printf("Error: Not enough disk space to write data. Required %d blocks, only %d available.\n", blocksRequired, fileSystem.freeBlocks);
        return;
    }

    if (node->blockPointers != NULL)
    {
        for (int blockCounter = node->blockCount - 1; blockCounter >= 0; blockCounter--)
        {
            short int response = allocateBlocksToFreeBlocks(node->blockPointers[blockCounter]);
            if (response == -1)
            {
                printf("Unable to free all Blocks.\n");
                node->blockPointers = realloc(node->blockPointers, node->blockCount);
                return;
            }
            node->blockCount--;
        }
        free(node->blockPointers);
        node->blockPointers = NULL;
    }

    node->blockCount = blocksRequired;

    node->blockPointers = (unsigned short *)calloc(blocksRequired, sizeof(unsigned short));
    if (node->blockPointers == NULL)
    {
        printf("Memory allocation failed!, try again\n");
        return;
    }

    unsigned short offset = 0;
    for (unsigned short int blockCounter = 0; blockCounter < blocksRequired; blockCounter++)
    {
        short int blockIndex = allocateBlocksFromFreeBlocks();
        if (blockIndex == -1)
        {
            return;
        }
        node->blockPointers[blockCounter] = blockIndex;

        memset(virtualDisk[blockIndex], 0, SIZEOFBLOCK);
        unsigned short remaining = len - offset;
        unsigned short copySize = (remaining >= SIZEOFBLOCK) ? SIZEOFBLOCK : remaining;

        memcpy(virtualDisk[blockIndex], content + offset, copySize);
        offset += copySize;
    }

    printf("Data written successfully, (%d bytes %d blocks)\n", strlen(content) + 1, blocksRequired);
}

FileNode *findFile(const char *fileName)
{
    FileNode *head = fileSystem.cwd->child;
    FileNode *temp = head;
    do
    {
        if (strcmp(temp->name, fileName) == 0)
        {
            if (temp->type == true)
            {
                printf("'%s' is a directory, not a file.\n", fileName);
                return NULL;
            }
            return temp;
        }
        temp = temp->next;
    } while (temp != head);

    printf("File '%s' not found in current directory!\n", fileName);
    return NULL;
}

void write(const char *fileName, const char *content)
{
    FileNode *response = findFile(fileName);
    if (response == NULL)
    {
        return;
    }
    writeFile(response, content);
}

void read(const char *fileName)
{
    FileNode *response = findFile(fileName);

    if (response == NULL)
    {
        return;
    }

    if (response->blockCount == 0 || response->blockPointers == NULL)
    {
        printf("File '%s' is empty.\n", fileName);
        return;
    }

    printf("Reading file '%s':\n", fileName);

    for (int blockCounter = 0; blockCounter < response->blockCount; blockCounter++)
    {
        printf("%s", virtualDisk[response->blockPointers[blockCounter]]);
    }

    printf("\n");
    return;
}
// ** needed
void removeNode(FileNode *temp, FileNode *head, FileNode *prev)
{
    if (temp->next == temp)
    {
        fileSystem.cwd->child = NULL;
    }
    else if (temp == head)
    {
        FileNode *tail = head;
        while (tail->next != head)
        {
            tail = tail->next;
        }
        tail->next = temp->next;
        fileSystem.cwd->child = temp->next;
    }
    else
    {
        prev->next = temp->next;
    }

    free(temp);
    return;
}

void delete(const char *fileName)
{
    if (fileSystem.cwd->child == NULL)
    {
        printf("CWD is empty!\n");
        return;
    }

    FileNode *head = fileSystem.cwd->child;
    FileNode *temp = head;
    FileNode *prev = NULL;

    do
    {
        if (temp->type == false && strcmp(temp->name, fileName) == 0)
        {
            if (temp->blockPointers != NULL)
            {
                for (int blockCounter = temp->blockCount - 1; blockCounter >= 0; blockCounter--)
                {
                    short int response = allocateBlocksToFreeBlocks(temp->blockPointers[blockCounter]);
                    if (response == -1)
                    {
                        printf("Unable to free all Blocks.\n");
                        temp->blockPointers = realloc(temp->blockPointers, temp->blockCount);
                        return;
                    }
                    temp->blockCount--;
                }
                free(temp->blockPointers);
                temp->blockPointers = NULL;
            }
            removeNode(temp, head, prev);
            printf("File '%s' deleted successfully from '%s'.\n", fileName, fileSystem.cwd->name);
            return;
        }
        prev = temp;
        temp = temp->next;
    } while (temp != head);

    printf("File '%s' not found in current directory.\n", fileName);
}

void rmdir(const char *dirName)
{
    if (fileSystem.cwd->child == NULL)
    {
        printf("CWD is empty!\n");
        return;
    }

    FileNode *head = fileSystem.cwd->child;
    FileNode *temp = head;
    FileNode *prev = NULL;

    do
    {
        if (temp->type == true && strcmp(temp->name, dirName) == 0)
        {
            if (temp->child != NULL)
            {
                printf("Directory '%s' is not empty. Cannot remove.\n", dirName);
                return;
            }
            removeNode(temp, head, prev);

            printf("Directory '%s' deleted successfully from '%s'.\n", dirName, fileSystem.cwd->name);

            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    } while (temp != head);

    printf("Directory '%s' not found in current directory.\n", dirName);
}

void ls()
{
    if (fileSystem.cwd->child == NULL)
    {
        printf("No files/sub-directories exist!\n");
        return;
    }

    FileNode *head = fileSystem.cwd->child;
    FileNode *temp = head;
    printf("Contents of '%s':\n", fileSystem.cwd->name);

    do
    {
        printf("  %s%s\n", temp->name, (temp->type == true) ? "/" : "");
        temp = temp->next;
    } while (temp != head);
}

void cd(const char *dirName)
{
    if (strcmp(dirName, "..") == 0)
    {
        if (strcmp(fileSystem.cwd->name, "/") == 0)
        {
            printf("CWD is %s cannot move!\n", fileSystem.cwd->name);
            return;
        }

        fileSystem.cwd = fileSystem.cwd->parent;

        char *lastSlash = strrchr(fileSystem.currentPath, '/');
        if (lastSlash != NULL && lastSlash != fileSystem.currentPath)
            *lastSlash = '\0';
        else
            strcpy(fileSystem.currentPath, "/");

        printf("Moved to parent directory: '%s'\n", fileSystem.cwd->name);
        return;
    }

    if (fileSystem.cwd->child == NULL)
    {
        printf("No subdirectories exist in '%s'.\n", fileSystem.cwd->name);
        return;
    }

    FileNode *head = fileSystem.cwd->child;
    FileNode *temp = head;

    do
    {
        if (strcmp(temp->name, dirName) == 0)
        {
            if (temp->type == false)
            {
                printf("'%s' is a file, not a directory!\n", dirName);
                return;
            }

            fileSystem.cwd = temp;

            if (strcmp(fileSystem.currentPath, "/") == 0)
                sprintf(fileSystem.currentPath + strlen(fileSystem.currentPath), "%s", dirName);
            else
                sprintf(fileSystem.currentPath + strlen(fileSystem.currentPath), "/%s", dirName);

            printf("Now in directory: '%s'\n", fileSystem.cwd->name);
            return;
        }
        temp = temp->next;
    } while (temp != head);

    printf("Directory '%s' not found in current directory '%s'.\n", dirName, fileSystem.cwd->name);
}

void pwd()
{
    printf("PWD is '%s'\n", fileSystem.currentPath);
}

void df()
{
    printf("Total Blocks: %hu\n", NUMBEROFBLOCKS);
    printf("Used Blocks: %hu\n", NUMBEROFBLOCKS - fileSystem.freeBlocks);
    printf("Free Blocks: %hu\n", fileSystem.freeBlocks);
    printf("Disk Usage: %.2f%%\n", ((float)(NUMBEROFBLOCKS - fileSystem.freeBlocks) / NUMBEROFBLOCKS) * 100);
    printf("\n");
}

void executeCommand(enum commands command, char *name, char *message)
{
    switch (command)
    {
    case MKDIR:
        mkdir(name);
        break;

    case CREATE:
        create(name);
        break;

    case WRITE:
        write(name, message);
        break;

    case READ:
        read(name);
        break;

    case DELETE:
        delete(name);
        break;

    case RMDIR:
        rmdir(name);
        break;

    case LS:
        ls();
        break;

    case CD:
        cd(name);
        break;

    case PWD:
        pwd();
        break;

    case DF:
        df();
        break;

    default:
        printf("wrong choice");
        return;
    }
}

int parseCommand(char *commandString)
{
    char *name = NULL;
    char *message = NULL;

    char *token = strtok(commandString, " ");

    enum commands command = ERROR;
    
    if (token == NULL)
    {
        return -1;
    }

    if ((strcmp(token, "mkdir") == 0))
        command = MKDIR;
    else if (strcmp(token, "create") == 0)
        command = CREATE;
    else if (strcmp(token, "write") == 0)
        command = WRITE;
    else if (strcmp(token, "read") == 0)
        command = READ;
    else if (strcmp(token, "delete") == 0)
        command = DELETE;
    else if (strcmp(token, "rmdir") == 0)
        command = RMDIR;
    else if (strcmp(token, "ls") == 0)
        command = LS;
    else if (strcmp(token, "cd") == 0)
        command = CD;
    else if (strcmp(token, "pwd") == 0)
        command = PWD;
    else if (strcmp(token, "df") == 0)
        command = DF;
    else if (strcmp(token, "exit") == 0)
        return 11;
    else
    {
        printf("Wrong command, try again!\n");
        return -1;
    }

    if (command == MKDIR || command == CD || command == RMDIR)
    {
        name = strtok(NULL, "");
        if (name == NULL)
        {
            printf("Please enter valid name of file/directory!\n");
            return -1;
        }
    }
    else if (command == CREATE || command == WRITE || command == READ || command == DELETE)
    {
        name = strtok(NULL, " ");
        if (name == NULL)
        {
            printf("Please enter valid name of file/directory!\n");
            return -1;
        }
    }

    if (command == WRITE)
    {
        message = strtok(NULL, "");

        if (message == NULL)
        {
            printf("Please provide content inside quotes.\n");
            return -1;
        }

        char *start = strchr(message, '"');
        if (start == NULL)
            start = strchr(message, '\'');
        if (start == NULL)
        {
            printf("Message must be enclosed in quotes!\n");
            return -1;
        }

        start++;

        char *end = strrchr(start, '"');
        if (end == NULL)
        {
            end = strrchr(start, '\'');
        }

        if (end == NULL)
        {
            printf("Closing quote missing!\n");
            return -1;
        }

        *end = '\0';

        message = start;
        if (strlen(message) == 0)
        {
            printf("Empty content. Nothing to write.\n");
            return -1;
        }
    }

    executeCommand(command, name, message);

    return 0;
}

void freeNode(FileNode *node)
{
    if (node == NULL)
        return;

    if (node->child != NULL)
    {
        FileNode *head = node->child;
        FileNode *temp = head;
        do
        {
            FileNode *next = temp->next;
            freeNode(temp);
            temp = next;
        } while (temp != head);
    }

    if (node->blockPointers != NULL)
        free(node->blockPointers);

    free(node);
}

void freeMemory()
{
    FreeBlock *temp = fileSystem.doublyListHead;
    while (temp != NULL)
    {
        FreeBlock *next = temp->next;
        free(temp);
        temp = next;
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

int main()
{
    short int initializeFreeBlocksResponseCode = initializeFreeBlocks();
    if (initializeFreeBlocksResponseCode == -1)
    {
        freeMemory();
        exit(0);
    }
    fileSystem.cwd = createNode("/", 1);
    strcpy(fileSystem.currentPath, "/");

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    while (1)
    {
        printf("%s> ", fileSystem.cwd->name);

        char *command = readLine();
        if (command == NULL)
            continue;

        if (strlen(command) == 0)
        {
            free(command);
            continue;
        }

        int res = parseCommand(command);
        free(command);

        if (res == 11)
        {
            freeMemory();
            printf("Memory released, Exiting the program.\n");
            exit(0);
        }
    }
    return 0;
}