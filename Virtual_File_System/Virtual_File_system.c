#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBEROFBLOCKS 20
#define SIZEOFBLOCK 10

typedef struct FreeBlock
{
    struct FreeBlock *prev;
    unsigned short index;
    struct FreeBlock *next;
} FreeBlock;

typedef struct FileNode
{
    char Name[51];
    unsigned short blockCount;
    unsigned short *blockPointers;
    unsigned short type;
    struct FileNode *parent;
    struct FileNode *child;
    struct FileNode *next;
} FileNode;

char **virtualDisk = NULL;
FreeBlock *doublyListHead = NULL;
FreeBlock *doublyListTail = NULL;
FileNode *cwdHead = NULL;
FileNode *cwdTail = NULL;
FileNode *cwd = NULL;
FileNode *root = NULL;
char currentPath[1024] = "/";

unsigned short freeBlocks = NUMBEROFBLOCKS;

short int allocateSpaceToVirtualDisk()
{
    virtualDisk = (char **)malloc((NUMBEROFBLOCKS) * sizeof(char *));
    if (virtualDisk == NULL)
    {
        printf("Failed to initilize Virtual Disk!\nEnding Program.\n");
        return -1;
    }
    for (int diskCounter = 0; diskCounter < NUMBEROFBLOCKS; diskCounter++)
    {
        virtualDisk[diskCounter] = (char *)calloc(SIZEOFBLOCK, sizeof(char));
        if (virtualDisk[diskCounter] == NULL)
        {
            printf("Failed to initilize Virtual Disk!\nEnding Program.\n");
            return -1;
        }
    }

    return 0;
}

short int initializeDLL()
{
    for (int dllCounter = 0; dllCounter < NUMBEROFBLOCKS; dllCounter++)
    {
        FreeBlock *temp = (FreeBlock *)calloc(1, sizeof(FreeBlock));
        if (temp == NULL)
        {
            printf("Failed to Initialize Free Blocks!\nEnding program.");
            return -1;
        }
        temp->index = dllCounter;
        temp->next = NULL;
        temp->prev = doublyListTail;

        if (doublyListHead == NULL)
        {
            doublyListHead = temp;
        }
        else
        {
            doublyListTail->next = temp;
        }
        doublyListTail = temp;
    }
    return 0;
}

unsigned short allocateBlocksFromDll()
{
    if (doublyListHead == NULL)
    {
        printf("No free blocks left\n");
        return (unsigned short)-1;
    }

    FreeBlock *temp = doublyListHead;

    doublyListHead = doublyListHead->next;

    if (doublyListHead != NULL)
    {
        doublyListHead->prev = NULL;
    }
    else
    {
        doublyListTail = NULL;
    }

    unsigned short blockIndex = temp->index;
    free(temp);

    freeBlocks--;

    return blockIndex;
}

short int freeBlocksToDll(unsigned int index)
{
    FreeBlock *temp = (FreeBlock *)calloc(1, sizeof(FreeBlock));
    if (temp == NULL)
    {
        printf("Unable to allocate memory to free Block!\nTry again.\n");
        return -1;
    }

    temp->index = index;
    temp->next = NULL;
    temp->prev = doublyListTail;

    memset(virtualDisk[index], 0, SIZEOFBLOCK);

    if (doublyListHead == NULL)
    {
        doublyListHead = temp;
    }
    else
    {
        doublyListTail->next = temp;
    }

    doublyListTail = temp;
    freeBlocks++;
}

FileNode *createNode(char *name, unsigned short type)
{
    FileNode *temp = (FileNode *)calloc(1, sizeof(FileNode));
    if (temp == NULL)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    strcpy(temp->Name, name);
    temp->next = NULL;
    temp->parent = cwd;
    temp->type = type;
    temp->blockCount = 0;
    temp->blockPointers = NULL;
    temp->child = NULL;

    return temp;
}

void insertInCwd(FileNode *newNode)
{
    if (cwd->child == NULL)
    {
        cwd->child = newNode;
        newNode->next = newNode;
    }
    else
    {
        FileNode *head = cwd->child;
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
    newNode->parent = cwd;
}

int isValidDirName(const char *name)
{
    if (name == NULL || strlen(name) == 0 || strlen(name) > 50)
        return 0;

    if (strchr(name, '/') || strchr(name, '"') || strchr(name, '\'') || strchr(name, '\\'))
        return 0;

    return 1;
}

int isValidFileName(const char *name)
{
    if (name == NULL || strlen(name) == 0 || strlen(name) > 50)
        return 0;

    if (strchr(name, ' ') || strchr(name, '/') || strchr(name, '"') ||
        strchr(name, '\'') || strchr(name, '\\'))
        return 0;

    return 1;
}

void mkdir(char *dirName)
{
    if (!isValidDirName(dirName))
    {
        printf("Invalid directory name '%s'.\n", dirName);
        printf("Rules: up to 50 chars, no /, quotes, or backslashes.\n");
        return;
    }

    FileNode *check = cwd->child;
    if (check != NULL)
    {
        do
        {
            if (strcmp(check->Name, dirName) == 0)
            {
                printf("Error: '%s' already exists in '%s'.\n", dirName, cwd->Name);
                return;
            }
            check = check->next;
        } while (check != cwd->child);
    }

    FileNode *directory = createNode(dirName, 1);
    if (directory == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    insertInCwd(directory);
    printf("Directory '%s' created under '%s'\n", dirName, cwd->Name);
}

void create(char *fileName)
{
    if (!isValidFileName(fileName))
    {
        printf("Invalid file name '%s'.\n", fileName);
        printf("Rules: up to 50 chars, no spaces, /, quotes, or backslashes.\n");
        return;
    }

    FileNode *check = cwd->child;
    if (check != NULL)
    {
        do
        {
            if (strcmp(check->Name, fileName) == 0)
            {
                printf("Error: '%s' already exists in '%s'.\n", fileName, cwd->Name);
                return;
            }
            check = check->next;
        } while (check != cwd->child);
    }

    FileNode *file = createNode(fileName, 0);
    if (file == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    insertInCwd(file);
    printf("File '%s' created under '%s'\n", fileName, cwd->Name);
}

void writeFile(FileNode *node, char *content)
{
    unsigned short len = strlen(content);
    unsigned short blocksRequired = (len + SIZEOFBLOCK - 1) / SIZEOFBLOCK;

    if (freeBlocks < blocksRequired)
    {
        printf("Error: Not enough disk space to write data. Required %hu blocks, only %hu available.\n", blocksRequired, freeBlocks);
        return;
    }

    if (node->blockPointers != NULL)
    {
        for (int blockCounter = 0; blockCounter < node->blockCount; blockCounter++)
        {
            short int response = freeBlocksToDll(node->blockPointers[blockCounter]);
            if (response == -1)
            {
                blockCounter--;
            }
        }
        free(node->blockPointers);
        node->blockPointers = NULL;
        node->blockCount = 0;
    }

    node->blockCount = blocksRequired;

    unsigned short *tempBlockPointers = (unsigned short *)calloc(blocksRequired, sizeof(unsigned short));
    if (tempBlockPointers == NULL)
    {
        printf("Memory allocation failed!, try again\n");
        return;
    }

    unsigned short offset = 0;
    for (unsigned short int blockCounter = 0; blockCounter < blocksRequired; blockCounter++)
    {
        unsigned short blockIndex = allocateBlocksFromDll();
        tempBlockPointers[blockCounter] = blockIndex;

        memset(virtualDisk[blockIndex], 0, SIZEOFBLOCK);
        unsigned short remaining = len - offset;
        unsigned short copySize = (remaining >= SIZEOFBLOCK) ? SIZEOFBLOCK : remaining;

        memcpy(virtualDisk[blockIndex], content + offset, copySize);
        offset += copySize;
    }
    node->blockPointers = tempBlockPointers;

    printf("Data written successfully, (%hu bytes %hu blocks)\n", strlen(content) + 1, blocksRequired);
}

void write(char *fileName, char *content)
{
    if (cwd->child == NULL)
    {
        printf("CWD is empty!\n");
        return;
    }
    FileNode *head = cwd->child;
    FileNode *temp = head;
    do
    {
        if (temp->type == 0 && strcmp(temp->Name, fileName) == 0)
        {
            writeFile(temp, content);
            return;
        }
        temp = temp->next;
    } while (temp != head);

    printf("File '%s' not found in current directory!\n", fileName);
}

void read(char *fileName)
{
    if (cwd->child == NULL)
    {
        printf("CWD is empty!\n");
        return;
    }

    FileNode *head = cwd->child;
    FileNode *temp = head;

    do
    {
        if (strcmp(temp->Name, fileName) == 0)
        {
            if (temp->type == 1)
            {
                printf("'%s' is a directory, not a file.\n", fileName);
                return;
            }

            if (temp->blockCount == 0 || temp->blockPointers == NULL)
            {
                printf("File '%s' is empty.\n", fileName);
                return;
            }

            printf("Reading file '%s':\n", fileName);

            for (int blockCounter = 0; blockCounter < temp->blockCount; blockCounter++)
            {
                printf("%s", virtualDisk[temp->blockPointers[blockCounter]]);
            }

            printf("\n");
            return;
        }

        temp = temp->next;
    } while (temp != head);

    printf("File '%s' not found in current directory!\n", fileName);
}

void delete(char *fileName)
{
    if (cwd->child == NULL)
    {
        printf("CWD is empty!\n");
        return;
    }

    FileNode *head = cwd->child;
    FileNode *temp = head;
    FileNode *prev = NULL;

    do
    {
        if (strcmp(temp->Name, fileName) == 0)
        {
            if (temp->type == 1)
            {
                printf("'%s' is a directory, not a file.\n", fileName);
                return;
            }

            if (temp->blockPointers != NULL)
            {
                for (int blockCounter = 0; blockCounter < temp->blockCount; blockCounter++)
                {
                    short int response = freeBlocksToDll(temp->blockPointers[blockCounter]);
                    if (response == -1)
                    {
                        blockCounter--;
                    }
                }
                free(temp->blockPointers);
                temp->blockPointers = NULL;
                temp->blockCount = 0;
            }

            if (temp->next == temp)
            {
                cwd->child = NULL;
            }
            else if (temp == head)
            {
                FileNode *tail = head;
                while (tail->next != head)
                {
                    tail = tail->next;
                }
                tail->next = temp->next;
                cwd->child = temp->next;
            }
            else
            {
                prev->next = temp->next;
            }

            printf("File '%s' deleted successfully from '%s'.\n", fileName, cwd->Name);

            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    } while (temp != head);

    printf("File '%s' not found in current directory.\n", fileName);
}

void rmdir(char *dirName)
{
    if (cwd->child == NULL)
    {
        printf("CWD is empty!\n");
        return;
    }

    FileNode *head = cwd->child;
    FileNode *temp = head;
    FileNode *prev = NULL;

    do
    {
        if (strcmp(temp->Name, dirName) == 0)
        {
            if (temp->type == 0)
            {
                printf("'%s' is a file, not a directory!\n", dirName);
                return;
            }

            if (temp->child != NULL)
            {
                printf("Directory '%s' is not empty. Cannot remove.\n", dirName);
                return;
            }

            if (temp->next == temp)
            {
                cwd->child = NULL;
            }
            else if (temp == head)
            {
                FileNode *tail = head;
                while (tail->next != head)
                {
                    tail = tail->next;
                }
                tail->next = temp->next;
                cwd->child = temp->next;
            }
            else
            {
                prev->next = temp->next;
            }

            printf("Directory '%s' deleted successfully from '%s'.\n", dirName, cwd->Name);

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
    if (cwd->child == NULL)
    {
        printf("No files/sub-directories exist!\n");
        return;
    }

    FileNode *head = cwd->child;
    FileNode *temp = head;
    printf("Contents of '%s':\n", cwd->Name);

    do
    {
        printf("  %s%s\n", temp->Name, (temp->type == 1) ? "/" : "");
        temp = temp->next;
    } while (temp != head);
}

void cd(char *dirName)
{
    if (strcmp(dirName, "..") == 0)
    {
        if (strcmp(cwd->Name, "/") == 0)
        {
            printf("CWD is %s cannot move!\n", cwd->Name);
            return;
        }

        cwd = cwd->parent;

        char *lastSlash = strrchr(currentPath, '/');
        if (lastSlash != NULL && lastSlash != currentPath)
            *lastSlash = '\0';
        else
            strcpy(currentPath, "/");

        printf("Moved to parent directory: '%s'\n", cwd->Name);
        return;
    }

    if (cwd->child == NULL)
    {
        printf("No subdirectories exist in '%s'.\n", cwd->Name);
        return;
    }

    FileNode *head = cwd->child;
    FileNode *temp = head;

    do
    {
        if (strcmp(temp->Name, dirName) == 0)
        {
            if (temp->type == 0)
            {
                printf("'%s' is a file, not a directory!\n", dirName);
                return;
            }

            cwd = temp;

            if (strcmp(currentPath, "/") == 0)
                sprintf(currentPath + strlen(currentPath), "%s", dirName);
            else
                sprintf(currentPath + strlen(currentPath), "/%s", dirName);

            printf("Now in directory: '%s'\n", cwd->Name);
            return;
        }
        temp = temp->next;
    } while (temp != head);

    printf("Directory '%s' not found in current directory '%s'.\n", dirName, cwd->Name);
}

void pwd()
{
    printf("PWD is '%s'\n", currentPath);
}

void df()
{
    printf("Total Blocks: %hu\n", NUMBEROFBLOCKS);
    printf("Used Blocks: %hu\n", NUMBEROFBLOCKS - freeBlocks);
    printf("Free Blocks: %hu\n", freeBlocks);
    printf("Disk Usage: %.2f%%\n", ((float)(NUMBEROFBLOCKS - freeBlocks) / NUMBEROFBLOCKS) * 100);
    printf("\n");
}

int parseCommand(char *command)
{
    int typeOfCommand = -1;
    char *name = NULL;
    char *message = NULL;

    char *token = strtok(command, " ");

    if (token == NULL)
    {
        return -1;
    }

    if ((strcmp(token, "mkdir") == 0))
    {
        typeOfCommand = 1;
    }
    else if (strcmp(token, "create") == 0)
    {
        typeOfCommand = 2;
    }
    else if (strcmp(token, "write") == 0)
        typeOfCommand = 3;
    else if (strcmp(token, "read") == 0)
        typeOfCommand = 4;
    else if (strcmp(token, "delete") == 0)
        typeOfCommand = 5;
    else if (strcmp(token, "rmdir") == 0)
        typeOfCommand = 6;
    else if (strcmp(token, "ls") == 0)
        typeOfCommand = 7;
    else if (strcmp(token, "cd") == 0)
        typeOfCommand = 8;
    else if (strcmp(token, "pwd") == 0)
        typeOfCommand = 9;
    else if (strcmp(token, "df") == 0)
        typeOfCommand = 10;
    else if (strcmp(token, "exit") == 0)
        typeOfCommand = 11;
    else
    {
        printf("Wrong command, try again!\n");
        return -1;
    }

    if (typeOfCommand == 1 || typeOfCommand == 8 || typeOfCommand == 6)
    {
        name = strtok(NULL, "");
        if (name == NULL)
        {
            printf("Please enter valid name of file/directory!\n");
            return -1;
        }
    }
    else if (typeOfCommand == 2 || typeOfCommand == 3 || typeOfCommand == 4 || typeOfCommand == 5)
    {
        name = strtok(NULL, " ");
        if (name == NULL)
        {
            printf("Please enter valid name of file/directory!\n");
            return -1;
        }
    }

    if (typeOfCommand == 3)
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

    switch (typeOfCommand)
    {
    case 1:
        mkdir(name);
        break;

    case 2:
        create(name);
        break;

    case 3:
        write(name, message);
        break;

    case 4:
        read(name);
        break;

    case 5:
        delete(name);
        break;

    case 6:
        rmdir(name);
        break;

    case 7:
        ls();
        break;

    case 8:
        cd(name);
        break;

    case 9:
        pwd();
        break;

    case 10:
        df();
        break;

    case 11:
        return 11;

    default:
        printf("wrong choice");
        break;
    }

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
    FreeBlock *temp = doublyListHead;
    while (temp != NULL)
    {
        FreeBlock *next = temp->next;
        free(temp);
        temp = next;
    }

    for (int i = 0; i < NUMBEROFBLOCKS; i++)
        free(virtualDisk[i]);
    free(virtualDisk);

    freeNode(root);
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

        if (ch == EOF && len == 0) {
        free(buffer);
        return NULL;
    }
    buffer[len] = '\0';
    return buffer;

}

int main()
{
    short int vdResponseCode = allocateSpaceToVirtualDisk();
    if (vdResponseCode == -1)
    {
        freeMemory();
        exit(0);
    }

    short int dllResponseCode = initializeDLL();
    if (dllResponseCode == -1)
    {
        freeMemory();
        exit(0);
    }
    root = createNode("/", 1);
    cwd = root;
    strcpy(currentPath, "/");

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    while (1)
    {
        printf("%s> ", cwd->Name);

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