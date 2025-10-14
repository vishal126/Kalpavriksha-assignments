#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned short int getIntInput(const char *prompt)
{
    char buffer[100];
    unsigned short int value;
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
}

int main()
{
    unsigned short int sizeOfMatrix;
    while (1)
    {
        sizeOfMatrix = getIntInput("enter size of matrix between 2-10: ");

        if (sizeOfMatrix > 10 || sizeOfMatrix < 2)
        {
            printf("Input is not in range, enter again\n");
        }
        else
        {
            break;
        }
    }

    unsigned short *sonarImageMatrix = (unsigned short *)malloc(sizeOfMatrix * sizeOfMatrix * sizeof(unsigned short));

    if (sonarImageMatrix == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (unsigned short i = 0; i < sizeOfMatrix; i++)
    {
        for (unsigned short j = 0; j < sizeOfMatrix; j++)
        {
            printf("enter value of element [%hu][%hu]: ", i, j);
            sonarImageMatrix[i * sizeOfMatrix + j] = getIntInput("");
        }
    }

    printf("\nMatrix entered:\n");
    for (unsigned short i = 0; i < sizeOfMatrix; i++)
    {
        for (unsigned short j = 0; j < sizeOfMatrix; j++)
        {
            printf("%hu ", sonarImageMatrix[i * sizeOfMatrix + j]);
        }
        printf("\n");
    }

    free(sonarImageMatrix);
}