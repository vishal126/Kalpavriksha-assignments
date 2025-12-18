#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

#define PORT 8080
#define ADDRESS "127.0.0.1"

typedef enum command
{
    WITHDRAW,
    DEPOSIT,
    DISPLAY,
    EXIT,
    WRONG
} command;

typedef struct ATMClient
{
    enum command choice;
    unsigned int amount;
} ATMClient;

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

enum command getEnumCommand(const unsigned int choice)
{
    switch (choice)
    {
        case 1: return WITHDRAW;
        case 2: return DEPOSIT;
        case 3: return DISPLAY;
        case 4: return EXIT;
        default: return WRONG;
    }
}


bool getInput(const int clientId)
{
    ATMClient transaction = {0};

    printf("\n1. Withdraw\n2. Deposit\n3. Display Balance\n4. Exit\n");
    unsigned int operation = getIntInput("Enter Choice: ");

    transaction.choice = getEnumCommand(operation);

    if (transaction.choice == WRONG)
    {
        printf("Wrong Choice!\n");
        return true;
    }

    if (transaction.choice == WITHDRAW || transaction.choice == DEPOSIT)
        transaction.amount = getIntInput("Enter amount: ");

    if (send(clientId, &transaction, sizeof(transaction), 0) <= 0)
    {
        perror("send");
        return false;
    }

    if (transaction.choice == EXIT)
        return false;

    return true;
}

int main()
{
    int clientId;
    if ((clientId = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serverAddr={0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ADDRESS, &serverAddr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        close(clientId);
        return 1;
    }

    int status;
    if ((status = connect(clientId, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) < 0)
    {
        perror("connect");
        close(clientId);
        return 1;
    }

    while (1)
    {
        if (!getInput(clientId))
        {
            close(clientId);
            return 0;
        }
        char response[256] = {0};
        int valRead = recv(clientId, response, sizeof(response) - 1, 0);

        if (valRead <= 0)
        {
            printf("Server disconnected.\n");
            break;
        }

        response[valRead] = '\0';
        printf("Server Response: %s\n", response);
    }

    close(clientId);

    return 0;
}