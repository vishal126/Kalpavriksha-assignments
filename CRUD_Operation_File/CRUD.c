#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"

typedef struct userInfo {
    unsigned int id;
    char name[26];
    unsigned int age;
} user;

int getIntInput(const char *prompt) {
    char buffer[100];
    int value;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;

            if (sscanf(buffer, "%d", &value) == 1) {
                return value;
            } else {
                printf("Invalid input. Please enter a valid integer.\n");
            }
        } else {
            printf("Error reading input. Try again.\n");
        }
    }
}

unsigned int getLastUserId()
{
    int lastIdx = -1;
    user temp;
    FILE *getIdx = fopen(FILE_NAME, "r");
    if (!getIdx)
        return 0;

    while (fscanf(getIdx, "%u %s %u", &temp.id, &temp.name, &temp.age) == 3)
    {
        lastIdx = temp.id;
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

void displayData()
{
    FILE *readPtr = fopen(FILE_NAME, "r");
    user temp;
    while (fscanf(readPtr, "%u %s %u", &temp.id, &temp.name, &temp.age) == 3)
    {
        printf("%u %s %u\n", temp.id, temp.name, temp.age);
    }

    fclose(readPtr);
}

void addUser(unsigned int id)
{

    user temp;
    temp.id = id;

    temp.age = getIntInput("Enter age: ");
    printf("enter name: ");
    scanf("%s", &temp.name);

    FILE *writePtr = fopen(FILE_NAME, "a");

    int isSuccess = fprintf(writePtr, "%u %s %u\n", temp.id, temp.name, temp.age);

    if (isSuccess >= 0)
    {
        printf("Write success for id: %u\n", temp.id);
    }
    else
    {
        printf("Write unsuccessful for id: %u\n", temp.id);
    }

    fclose(writePtr);
}

void editUser()
{
    FILE *file = fopen(FILE_NAME, "r");
    if (!file)
    {
        printf("Error opening file\n");
        return;
    }

    unsigned int id = getIntInput("Enter ID of user you want to modify: ");

    // Count users
    int count = 0;
    user temp;
    while (fscanf(file, "%u %s %u", &temp.id, temp.name, &temp.age) == 3)
    {
        count++;
    }
    rewind(file);

    // Load users into memory
    user *users = (user *)malloc(count * sizeof(user));
    if (!users)
    {
        printf("Memory allocation failed\n");
        fclose(file);
        return;
    }

    int i = 0;
    while (fscanf(file, "%u %s %u", &users[i].id, users[i].name, &users[i].age) == 3)
    {
        i++;
    }

    fclose(file);

    int isFound = 0;

    for (int j = 0; j < count; j++)
    {
        if (users[j].id == id)
        {
            isFound = 1;
            printf("Enter new name: ");
            scanf("%s", users[j].name);
            printf("Enter new age: ");
            scanf("%u", &users[j].age);
            break;
        }
    }

    if (!isFound)
    {
        printf("User ID %u not found\n", id);
        free(users);
        return;
    }

    file = fopen(FILE_NAME, "w");
    if (!file)
    {
        printf("Error opening file for writing\n");
        free(users);
        return;
    }

    for (int j = 0; j < count; j++)
    {
        fprintf(file, "%u %s %u\n", users[j].id, users[j].name, users[j].age);
    }

    fclose(file);
    free(users);

    printf("User info edited successfully\n");
}

unsigned int deleteUser()
{
    FILE *file = fopen(FILE_NAME, "r");
    if (!file)
    {
        printf("Error opening file\n");
        return -1;
    }
    
    unsigned int id = getIntInput("Enter ID of user you want to delete: ");

    // Count users
    int count = 0;
    user temp;
    while (fscanf(file, "%u %s %u", &temp.id, temp.name, &temp.age) == 3)
    {
        count++;
    }
    rewind(file);

    user *users = (user *)malloc(count * sizeof(user));
    if (!users)
    {
        printf("Memory allocation failed\n");
        fclose(file);
        return -1;
    }

    int i = 0;
    while (fscanf(file, "%u %s %u", &users[i].id, users[i].name, &users[i].age) == 3)
    {
        i++;
    }
    fclose(file);

    int isFound = 0;

    file = fopen(FILE_NAME, "w");
    if (!file)
    {
        printf("Error opening file for writing\n");
        free(users);
        return -1;
    }

    for (int j = 0; j < count; j++)
    {
        if (users[j].id != id)
        {
            fprintf(file, "%u %s %u\n", users[j].id, users[j].name, users[j].age);
        }
        else
        {
            isFound = 1;
        }
    }

    fclose(file);
    free(users);

    if (isFound)
    {
        printf("User Deleted successfully\n");
        return getLastUserId(FILE_NAME);
    }
    else
    {
        printf("User ID %u not found\n", id);
    }
}

int main()
{
    unsigned int id = getLastUserId();

    while (1)
    {
        printf("Choose Operation:\n1. Display all users\n2. Add a new user\n3. Modify user details\n4. Delete a user\n0. exit\n");

        int ch = getIntInput("Enter your choice: ");

        switch (ch)
        {
        case 1:
            displayData();
            break;

        case 2:
            addUser(id++);
            break;

        case 3:
        {
            editUser();
            break;
        }

        case 4:
        {
            int tempId = deleteUser();
            if(tempId!=-1) {
                id=tempId;
            }
            break;
        }

        case 0:
            exit(0);
            break;

        default:
            printf("enter correct choice\n");
            break;
        }
    }
}