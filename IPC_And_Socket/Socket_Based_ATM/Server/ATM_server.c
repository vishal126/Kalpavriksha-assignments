#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8080
#define ADDRESS "127.0.0.1"
#define DB_FILE "../accountDB.txt"

pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t serverRunning = 1;
int serverId = -1;

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

int readBalance(int *balance)
{
    FILE *fp = fopen(DB_FILE, "r");
    if (!fp)
    {
        perror("fopen");
        return -1;
    }

    if (fscanf(fp, "%d", balance) != 1)
    {
        fclose(fp);
        fprintf(stderr, "Failed to read balance\n");
        return -1;
    }

    fclose(fp);
    return 0;
}

int writeBalance(unsigned int balance)
{
    FILE *fp = fopen(DB_FILE, "w");
    if (!fp)
    {
        perror("fopen");
        return -1;
    }

    if (fprintf(fp, "%u\n", balance) < 0)
    {
        perror("fprintf");
        fclose(fp);
        return -1;
    }

    if (fclose(fp) != 0)
    {
        perror("fclose");
        return -1;
    }

    return 0;
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

        int balance;
        if (readBalance(&balance) == -1)
        {
            pthread_mutex_unlock(&fileMutex);
            snprintf(response, sizeof(response),
                     "Server error: unable to read account database");
            send(clientId, response, strlen(response) + 1, 0);
            break;
        }

        int fatalError = 0;

        switch (transaction.choice)
        {
        case WITHDRAW:
            if (transaction.amount > balance)
            {
                snprintf(response, sizeof(response),
                         "Withdrawal failed! Insufficient balance.");
            }
            else
            {
                balance -= transaction.amount;
                if (writeBalance(balance) == -1)
                {
                    snprintf(response, sizeof(response),
                             "Server error: failed to update account database");
                    fatalError = 1;
                }
                else
                {
                    snprintf(response, sizeof(response),
                             "Withdrawal successful! New balance: %d", balance);
                }
            }
            break;

        case DEPOSIT:
            balance += transaction.amount;
            if (writeBalance(balance) == -1)
            {
                snprintf(response, sizeof(response),
                         "Server error: failed to update account database");
                fatalError = 1;
            }
            else
            {
                snprintf(response, sizeof(response),
                         "Deposit successful! New balance: %d", balance);
            }
            break;

        case DISPLAY:
            snprintf(response, sizeof(response), "Current balance: %d", balance);
            break;

        default:
            snprintf(response, sizeof(response), "Invalid request.");
        }

        pthread_mutex_unlock(&fileMutex);

        if (send(clientId, response, strlen(response) + 1, 0) <= 0)
        {
            perror("send");
            break;
        }
        
        if (fatalError)
            break;
    }

    close(clientId);
    return NULL;
}

void handleSignal(int sig)
{
    (void)sig;
    serverRunning = 0;

    if (serverId != -1)
        close(serverId); // unblock accept()
}

int main()
{
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

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

    while (serverRunning)
    {
        int *clientId = malloc(sizeof(int));
        if (!clientId)
        {
            printf("Memory allocation failed!\n");
            continue;
        }
        if ((*clientId = accept(serverId, (struct sockaddr *)&address, &addressLength)) < 0)
        {
            free(clientId);

            if (!serverRunning)
                break;

            perror("accept");
            continue;
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

    pthread_mutex_destroy(&fileMutex);
    printf("Server shut down cleanly.\n");

    return 0;
}