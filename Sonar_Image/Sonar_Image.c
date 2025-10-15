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

void swap(unsigned short *swapElement1, unsigned short *swapElement2)
{
    unsigned short temp = *swapElement1;
    *swapElement1 = *swapElement2;
    *swapElement2 = temp;
}

void reverseMatrix(unsigned short *transposedMatrix, unsigned short sizeOfMatrix)
{

    for (int i = 0; i < sizeOfMatrix; i++)
    {
        unsigned short *matrixStart = transposedMatrix + i * sizeOfMatrix;
        unsigned short *matrixEnd = matrixStart + sizeOfMatrix - 1;

        while (matrixStart < matrixEnd)
        {
            swap(matrixStart, matrixEnd);
            matrixStart++;
            matrixEnd--;
        }
    }
}

void rotateMatrix(unsigned short *sonarMatrix, unsigned short sizeOfMatrix)
{
    for (unsigned short i = 0; i < sizeOfMatrix - 1; i++)
    {
        for (unsigned short j = i + 1; j < sizeOfMatrix; j++)
        {
            swap(sonarMatrix + (i * sizeOfMatrix + j), sonarMatrix + (j * sizeOfMatrix + i));
        }
    }

    reverseMatrix(sonarMatrix, sizeOfMatrix);
}

unsigned short getAverage(unsigned short *prevRow, unsigned short *tempCurrMatrix, unsigned short *sonarMatrix, unsigned short sizeOfMatrix, unsigned short i, unsigned short j)
{
    unsigned short sum = 0;
    unsigned short count = 0;
    if (i == 0)
    {
        if (j == 0)
        {
            sum = *(prevRow) +
                  *(prevRow + 1) +
                  *(sonarMatrix + sizeOfMatrix) +
                  *(sonarMatrix + sizeOfMatrix + 1);
            count = 4;
        }
        else if (j == sizeOfMatrix - 1)
        {
            sum = *(prevRow + j) +
                  *(prevRow + j - 1) +
                  *(sonarMatrix + sizeOfMatrix + j) +
                  *(sonarMatrix + sizeOfMatrix + j - 1);
            count = 4;
        }
        else
        {
            sum = *(prevRow + j) +
                  *(prevRow + j - 1) +
                  *(prevRow + j + 1) +
                  *(sonarMatrix + sizeOfMatrix + j) +
                  *(sonarMatrix + sizeOfMatrix + j + 1) +
                  *(sonarMatrix + sizeOfMatrix + j - 1);
            count = 6;
        }
    }
    else if (i == sizeOfMatrix - 1)
    {
        if (j == 0)
        {
            sum = *(sonarMatrix + (i * sizeOfMatrix)) +
                  *(sonarMatrix + (i * sizeOfMatrix) + 1) +
                  *(prevRow) +
                  *(prevRow + 1);
            count = 4;
        }
        else if (j == sizeOfMatrix - 1)
        {
            sum = *(sonarMatrix + (i * sizeOfMatrix) + j) +
                  *(tempCurrMatrix + (j - 1)) +
                  *(prevRow + j) +
                  *(prevRow + (j - 1));
            count = 4;
        }
        else
        {
            sum = *(sonarMatrix + (i * sizeOfMatrix) + j) +
                  *(tempCurrMatrix + (j - 1)) +
                  *(sonarMatrix + (i * sizeOfMatrix) + (j + 1)) +
                  *(prevRow + j) +
                  *(prevRow + (j - 1)) +
                  *(prevRow + (j + 1));
            count = 6;
        }
    }
    else if (j == 0)
    {
        sum = *(sonarMatrix + (i * sizeOfMatrix)) +
              *(sonarMatrix + (i * sizeOfMatrix) + 1) +
              *(prevRow) +
              *(prevRow + 1) +
              *(sonarMatrix + ((i + 1) * sizeOfMatrix)) +
              *(sonarMatrix + ((i + 1) * sizeOfMatrix) + 1);
        count = 6;
    }
    else if (j == sizeOfMatrix - 1)
    {
        sum = *(sonarMatrix + (i * sizeOfMatrix) + j) +
              *(tempCurrMatrix + (j - 1)) +
              *(prevRow + j) +
              *(prevRow + (j - 1)) +
              *(sonarMatrix + ((i + 1) * sizeOfMatrix) + j) +
              *(sonarMatrix + ((i + 1) * sizeOfMatrix) + (j - 1));
        count = 6;
    }
    else
    {
        sum = *(sonarMatrix + (i * sizeOfMatrix) + j) +
              *(tempCurrMatrix + (j - 1)) +
              *(sonarMatrix + (i * sizeOfMatrix) + (j + 1)) +
              *(sonarMatrix + ((i + 1) * sizeOfMatrix) + (j)) +
              *(sonarMatrix + ((i + 1) * sizeOfMatrix) + (j - 1)) +
              *(sonarMatrix + ((i + 1) * sizeOfMatrix) + (j + 1)) +
              *(prevRow + (j)) +
              *(prevRow + (j - 1)) +
              *(prevRow + (j + 1));
        count = 9;
    }

    return sum / count;
}

void smoothingFilter(unsigned short *sonarMatrix, unsigned short sizeOfMatrix)
{
    unsigned short *prevRow = (unsigned short *)malloc(sizeOfMatrix * sizeof(unsigned short));
    unsigned short *currRow = (unsigned short *)malloc(sizeOfMatrix * sizeof(unsigned short));

    for (unsigned short j = 0; j < sizeOfMatrix; j++)
        prevRow[j] = sonarMatrix[j];

    for (unsigned short i = 0; i < sizeOfMatrix; i++)
    {
        for (unsigned short j = 0; j < sizeOfMatrix; j++)
            currRow[j] = sonarMatrix[i * sizeOfMatrix + j];

        for (unsigned short j = 0; j < sizeOfMatrix; j++)
        {
            unsigned short average = getAverage(prevRow, currRow, sonarMatrix, sizeOfMatrix, i, j);
            sonarMatrix[i * sizeOfMatrix + j] = average;
        }

        for (unsigned short j = 0; j < sizeOfMatrix; j++)
            prevRow[j] = currRow[j];
    }

    free(prevRow);
    free(currRow);
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
            printf("enter value of element [%hu][%hu] in between 0-255: ", i, j);
            sonarImageMatrix[i * sizeOfMatrix + j] = getIntInput("");
            if (sonarImageMatrix[i * sizeOfMatrix + j] > 255 || sonarImageMatrix[i * sizeOfMatrix + j] < 0)
            {
                printf("value is not in range 0-255, enter correct value\n");
                j--;
            }
        }
    }

    printf("\nOriginal\n");
    for (unsigned short i = 0; i < sizeOfMatrix; i++)
    {
        for (unsigned short j = 0; j < sizeOfMatrix; j++)
        {
            printf("%hu ", sonarImageMatrix[i * sizeOfMatrix + j]);
        }
        printf("\n");
    }

    rotateMatrix(sonarImageMatrix, sizeOfMatrix);
    printf("\nRotated:\n");
    for (unsigned short i = 0; i < sizeOfMatrix; i++)
    {
        for (unsigned short j = 0; j < sizeOfMatrix; j++)
        {
            printf("%hu ", sonarImageMatrix[i * sizeOfMatrix + j]);
        }
        printf("\n");
    }

    smoothingFilter(sonarImageMatrix, sizeOfMatrix);

    printf("\nFinal Output\n");
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