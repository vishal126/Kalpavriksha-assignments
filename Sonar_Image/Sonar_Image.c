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

    for (int rowIndex = 0; rowIndex < sizeOfMatrix; rowIndex++)
    {
        unsigned short *matrixStart = transposedMatrix + rowIndex * sizeOfMatrix;
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
    for (unsigned short rowIndex = 0; rowIndex < sizeOfMatrix - 1; rowIndex++)
    {
        for (unsigned short colIndex = rowIndex + 1; colIndex < sizeOfMatrix; colIndex++)
        {
            swap(sonarMatrix + (rowIndex * sizeOfMatrix + colIndex), sonarMatrix + (colIndex * sizeOfMatrix + rowIndex));
        }
    }

    reverseMatrix(sonarMatrix, sizeOfMatrix);
}

unsigned short getAverage(unsigned short *prevRow, unsigned short *tempCurrMatrix, unsigned short *sonarMatrix, unsigned short sizeOfMatrix, unsigned short rowIndex, unsigned short colIndex)
{
    unsigned short sum = 0;
    unsigned short count = 0;
    if (rowIndex == 0)
    {
        if (colIndex == 0)
        {
            sum = *(prevRow) +
                  *(prevRow + 1) +
                  *(sonarMatrix + sizeOfMatrix) +
                  *(sonarMatrix + sizeOfMatrix + 1);
            count = 4;
        }
        else if (colIndex == sizeOfMatrix - 1)
        {
            sum = *(prevRow + colIndex) +
                  *(prevRow + colIndex - 1) +
                  *(sonarMatrix + sizeOfMatrix + colIndex) +
                  *(sonarMatrix + sizeOfMatrix + colIndex - 1);
            count = 4;
        }
        else
        {
            sum = *(prevRow + colIndex) +
                  *(prevRow + colIndex - 1) +
                  *(prevRow + colIndex + 1) +
                  *(sonarMatrix + sizeOfMatrix + colIndex) +
                  *(sonarMatrix + sizeOfMatrix + colIndex + 1) +
                  *(sonarMatrix + sizeOfMatrix + colIndex - 1);
            count = 6;
        }
    }
    else if (rowIndex == sizeOfMatrix - 1)
    {
        if (colIndex == 0)
        {
            sum = *(sonarMatrix + (rowIndex * sizeOfMatrix)) +
                  *(sonarMatrix + (rowIndex * sizeOfMatrix) + 1) +
                  *(prevRow) +
                  *(prevRow + 1);
            count = 4;
        }
        else if (colIndex == sizeOfMatrix - 1)
        {
            sum = *(sonarMatrix + (rowIndex * sizeOfMatrix) + colIndex) +
                  *(tempCurrMatrix + (colIndex - 1)) +
                  *(prevRow + colIndex) +
                  *(prevRow + (colIndex - 1));
            count = 4;
        }
        else
        {
            sum = *(sonarMatrix + (rowIndex * sizeOfMatrix) + colIndex) +
                  *(tempCurrMatrix + (colIndex - 1)) +
                  *(sonarMatrix + (rowIndex * sizeOfMatrix) + (colIndex + 1)) +
                  *(prevRow + colIndex) +
                  *(prevRow + (colIndex - 1)) +
                  *(prevRow + (colIndex + 1));
            count = 6;
        }
    }
    else if (colIndex == 0)
    {
        sum = *(sonarMatrix + (rowIndex * sizeOfMatrix)) +
              *(sonarMatrix + (rowIndex * sizeOfMatrix) + 1) +
              *(prevRow) +
              *(prevRow + 1) +
              *(sonarMatrix + ((rowIndex + 1) * sizeOfMatrix)) +
              *(sonarMatrix + ((rowIndex + 1) * sizeOfMatrix) + 1);
        count = 6;
    }
    else if (colIndex == sizeOfMatrix - 1)
    {
        sum = *(sonarMatrix + (rowIndex * sizeOfMatrix) + colIndex) +
              *(tempCurrMatrix + (colIndex - 1)) +
              *(prevRow + colIndex) +
              *(prevRow + (colIndex - 1)) +
              *(sonarMatrix + ((rowIndex + 1) * sizeOfMatrix) + colIndex) +
              *(sonarMatrix + ((rowIndex + 1) * sizeOfMatrix) + (colIndex - 1));
        count = 6;
    }
    else
    {
        sum = *(sonarMatrix + (rowIndex * sizeOfMatrix) + colIndex) +
              *(tempCurrMatrix + (colIndex - 1)) +
              *(sonarMatrix + (rowIndex * sizeOfMatrix) + (colIndex + 1)) +
              *(sonarMatrix + ((rowIndex + 1) * sizeOfMatrix) + (colIndex)) +
              *(sonarMatrix + ((rowIndex + 1) * sizeOfMatrix) + (colIndex - 1)) +
              *(sonarMatrix + ((rowIndex + 1) * sizeOfMatrix) + (colIndex + 1)) +
              *(prevRow + (colIndex)) +
              *(prevRow + (colIndex - 1)) +
              *(prevRow + (colIndex + 1));
        count = 9;
    }

    return sum / count;
}

void smoothingFilter(unsigned short *sonarMatrix, unsigned short sizeOfMatrix)
{
    unsigned short *prevRow = (unsigned short *)malloc(sizeOfMatrix * sizeof(unsigned short));
    unsigned short *currRow = (unsigned short *)malloc(sizeOfMatrix * sizeof(unsigned short));

    for (unsigned short colIndex = 0; colIndex < sizeOfMatrix; colIndex++)
        *(prevRow + colIndex) = *(sonarMatrix + colIndex);

    for (unsigned short rowIndex = 0; rowIndex < sizeOfMatrix; rowIndex++)
    {
        for (unsigned short colIndex = 0; colIndex < sizeOfMatrix; colIndex++)
            *(currRow + colIndex) = *(sonarMatrix + (rowIndex * sizeOfMatrix) + colIndex);

        for (unsigned short colIndex = 0; colIndex < sizeOfMatrix; colIndex++)
        {
            unsigned short average = getAverage(prevRow, currRow, sonarMatrix, sizeOfMatrix, rowIndex, colIndex);
            *(sonarMatrix + (rowIndex * sizeOfMatrix) + colIndex) = average;
        }

        for (unsigned short colIndex = 0; colIndex < sizeOfMatrix; colIndex++)
            *(prevRow + colIndex) = *(currRow + colIndex);
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

    for (unsigned short rowIndex = 0; rowIndex < sizeOfMatrix; rowIndex++)
    {
        for (unsigned short colIndex = 0; colIndex < sizeOfMatrix; colIndex++)
        {
            printf("enter value of element [%hu][%hu] in between 0-255: ", rowIndex, colIndex);
            *(sonarImageMatrix + (rowIndex * sizeOfMatrix) + colIndex) = getIntInput("");
            if (*(sonarImageMatrix + (rowIndex * sizeOfMatrix) + colIndex) > 255 || *(sonarImageMatrix + (rowIndex * sizeOfMatrix) + colIndex) < 0)
            {
                printf("value is not in range 0-255, enter correct value\n");
                colIndex--;
            }
        }
    }

    printf("\nOriginal\n");
    for (unsigned short rowIndex = 0; rowIndex < sizeOfMatrix; rowIndex++)
    {
        for (unsigned short colIndex = 0; colIndex < sizeOfMatrix; colIndex++)
        {
            printf("%hu ", *(sonarImageMatrix + (rowIndex * sizeOfMatrix) + colIndex));
        }
        printf("\n");
    }

    rotateMatrix(sonarImageMatrix, sizeOfMatrix);
    printf("\nRotated:\n");
    for (unsigned short rowIndex = 0; rowIndex < sizeOfMatrix; rowIndex++)
    {
        for (unsigned short colIndex = 0; colIndex < sizeOfMatrix; colIndex++)
        {
            printf("%hu ", *(sonarImageMatrix + (rowIndex * sizeOfMatrix) + colIndex));
        }
        printf("\n");
    }

    smoothingFilter(sonarImageMatrix, sizeOfMatrix);

    printf("\nFinal Output\n");
    for (unsigned short rowIndex = 0; rowIndex < sizeOfMatrix; rowIndex++)
    {
        for (unsigned short colIndex = 0; colIndex < sizeOfMatrix; colIndex++)
        {
            printf("%hu ", *(sonarImageMatrix + (rowIndex * sizeOfMatrix) + colIndex));
        }
        printf("\n");
    }

    free(sonarImageMatrix);
}