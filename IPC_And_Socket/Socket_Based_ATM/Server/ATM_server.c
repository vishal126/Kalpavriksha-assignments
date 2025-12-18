#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define ADDRESS "127.0.0.1"
#define DB_FILE "../accountDB.txt"

pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum command
{
    WITHDRAW,
    DEPOSIT,
    DISPLAY,
    EXIT,
} command;

typedef struct ATMServer
{
    enum command choice;
    unsigned int amount;
} ATMServer;

int readBalance()
{
    FILE *fp = fopen(DB_FILE, "r");
    int balance = 0;
    if (fp)
    {
        fscanf(fp, "%d", &balance);
        fclose(fp);
    }
    else
        perror("fopen");

    return balance;
}

void writeBalance(unsigned int balance)
{
    FILE *fp = fopen(DB_FILE, "w");
    if (fp)
    {
        fprintf(fp, "%d", balance);
        fclose(fp);
    }
    else
        perror("fopen");
}

void *handleClient(void *arg)
{
    int clientId = *(int *)arg;
    free(arg);

    ATMServer transaction;
    char response[256];

    while (1)
    {
        int valRead = recv(clientId, &transaction, sizeof(transaction), 0);
        if (valRead != sizeof(transaction))
        {
            printf("Client sent invalid data\n");
            break;
        }

        if (transaction.choice == EXIT)
        {
            printf("Client disconnected\n");
            break;
        }

        pthread_mutex_lock(&fileMutex);

        int balance = readBalance();

        switch (transaction.choice)
        {
        case WITHDRAW:
            if (transaction.amount > balance)
                sprintf(response, "Withdrawal failed! Insufficient balance.");
            else
            {
                balance -= transaction.amount;
                writeBalance(balance);
                sprintf(response, "Withdrawal successful! New balance: %d", balance);
            }
            break;

        case DEPOSIT:
            balance += transaction.amount;
            writeBalance(balance);
            sprintf(response, "Deposit successful! New balance: %d", balance);
            break;

        case DISPLAY:
            sprintf(response, "Current balance: %d", balance);
            break;

        default:
            sprintf(response, "Invalid request.");
        }

        pthread_mutex_unlock(&fileMutex);

        if (send(clientId, response, strlen(response) + 1, 0) <= 0)
        {
            perror("send");
            break;
        }
    }

    close(clientId);
    return NULL;
}

int main()
{
    int serverId;
    int opt = 1;

    if ((serverId = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed!");
        return 1;
    }

    if (setsockopt(serverId, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        close(serverId);
        return 1;
    }

    struct sockaddr_in address;
    int addressLength = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ADDRESS);
    address.sin_port = htons(PORT);

    if (bind(serverId, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        close(serverId);
        return 1;
    }

    if (listen(serverId, 3) < 0)
    {
        perror("listen");
        close(serverId);
        return 1;
    }

    while (1)
    {
        int *clientId = malloc(sizeof(int));
        if ((*clientId = accept(serverId, (struct sockaddr *)&address, &addressLength)) < 0)
        {
            perror("accept");
            close(serverId);
            return 1;
        }

        printf("Connection Built.\n");

        pthread_t threadId;
        if (pthread_create(&threadId, NULL, handleClient, clientId) != 0)
        {
            perror("pthread_create");
            close(*clientId);
            free(clientId);
            continue;
        }
        pthread_detach(threadId);
    }

    close(serverId);
    pthread_mutex_destroy(&fileMutex);

    return 0;
}