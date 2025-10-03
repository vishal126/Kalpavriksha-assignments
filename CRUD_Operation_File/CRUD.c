#include <stdio.h>
#include <stdlib.h>

typedef struct userInfo
{
    int id;
    char name[26];
    int age;
} user;

int getLastUserIndex(char fileName[])
{
    int lastIdx = -1;
    int id, age;
    char name[26];
    FILE *getIdx = fopen(fileName, "r");
    if (!getIdx)
        return 0;

    while (fscanf(getIdx, "%d %s %d", &id, name, &age) == 3)
    {
        lastIdx = id;
    }
    fclose(getIdx);

    if (lastIdx == -1)
    {
        lastIdx = 0;
    }
    else
    {
        lastIdx++;
    }
    return lastIdx;
}

void displayData(char fileName[])
{
    FILE *readPtr = fopen(fileName, "r");
    char data[50];

    while (fgets(data, 50, readPtr) != NULL)
    {
        printf("%s", data);
    }

    fclose(readPtr);
}

void addUser(int id, char fileName[])
{

    user temp;
    temp.id = id;

    printf("enter age: ");
    scanf("%d", &temp.age);
    printf("enter name: ");
    scanf("%s", &temp.name);

    FILE *writePtr = fopen(fileName, "a");

    // char data[] = " ";
    // fprintf(writePtr, "%d", temp.id);
    // fputc(data[0], writePtr);
    // fputs(temp.name, writePtr);
    // fputc(data[0],writePtr);
    // fprintf(writePtr, "%d", temp.age);
    // fputc(data[0],writePtr);
    // fputs("\n", writePtr);

    fprintf(writePtr, "%d %s %d\n", temp.id, temp.name, temp.age);

    fclose(writePtr);
}

void editUser(int id, int size, char fileName[])
{
    if (id >= size)
    {
        printf("id is not in range\n");
        return;
    }

    FILE *originalFile = fopen(fileName, "r");
    FILE *tempFile = fopen("tempFile.txt", "w");

    if (!originalFile || !tempFile)
    {
        printf("error opening file\n");
        return;
    }

    int uid, age;
    char name[26];
    int isFound = 0;

    while (fscanf(originalFile, "%d %s %d", &uid, name, &age) == 3)
    {
        if (uid == id)
        {
            isFound = 1;
            printf("enter name to edit: ");
            scanf("%s", name);
            printf("Enter age to edit: ");
            scanf("%d", &age);
        }
        fprintf(tempFile, "%d %s %d\n", uid, name, age);
    }
    fclose(originalFile);
    fclose(tempFile);

    remove(fileName);
    rename("tempFile.txt", fileName);

    if (isFound)
    {
        printf("User Info Edited success\n");
    }
    else
    {
        printf("Error while editing user info\n");
    }
}

void deleteUser(int id, int size, char fileName[])
{
    if (id >= size)
    {
        printf("id is not in range\n");
        return;
    }

    FILE *originalFile = fopen(fileName, "r");
    FILE *tempFile = fopen("tempFile.txt", "w");

    if (!originalFile || !tempFile)
    {
        printf("error opening file\n");
        return;
    }

    int uid, age;
    char name[26];
    int isFound = 0;

    while (fscanf(originalFile, "%d %s %d", &uid, name, &age) == 3)
    {
        if (uid != id)
        {
            isFound = 1;
            fprintf(tempFile, "%d %s %d\n", uid, name, age);
        }
    }
    fclose(originalFile);
    fclose(tempFile);

    remove(fileName);
    rename("tempFile.txt", fileName);

    if (isFound)
    {
        printf("User Deleted successfully\n");
    }
    else
    {
        printf("Error while deleting user info\n");
    }
}

int main()
{
    FILE *fptr;
    char name[] = "users.txt";

    int id = getLastUserIndex(name);
    printf("%d\n", id);

    while (1)
    {
        printf("Choose Operation:\n1. Display all users\n2. Add a new user\n3. Modify user details\n4. Delete a user\n0. exit\n");

        int ch;
        scanf("%d", &ch);

        switch (ch)
        {
        case 1:
            displayData(name);
            break;

        case 2:
            addUser(id++, name);
            break;

        case 3:
        {
            int uid;
            printf("enter id of user you want to modify: ");
            scanf("%d", &uid);
            editUser(uid, id, name);
            break;
        }

        case 4:
        {
            int uid;
            printf("enter id of user you want to delete: ");
            scanf("%d", &uid);
            deleteUser(uid, id, name);
            break;
        }

        case 0:
            // fclose(fptr);
            exit(0);
            break;

        default:
            printf("enter correct choice\n");
            break;
        }
    }
}