#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
            sum = *(tempCurrMatrix) +
                  *(sonarMatrix + 1) +
                  *(sonarMatrix + sizeOfMatrix) +
                  *(sonarMatrix + sizeOfMatrix + 1);
            count = 4;
        }
        else if (colIndex == sizeOfMatrix - 1)
        {
            sum = *(sonarMatrix + colIndex) +
                  *(tempCurrMatrix + colIndex - 1) +
                  *(sonarMatrix + sizeOfMatrix + colIndex) +
                  *(sonarMatrix + sizeOfMatrix + colIndex - 1);
            count = 4;
        }
        else
        {
            sum = *(sonarMatrix + colIndex) +
                  *(tempCurrMatrix + colIndex - 1) +
                  *(sonarMatrix + colIndex + 1) +
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

    for (unsigned short rowIndex = 0; rowIndex < sizeOfMatrix; rowIndex++)
    {
        for (unsigned short colIndex = 0; colIndex < sizeOfMatrix; colIndex++)
        {
            *(currRow + colIndex) = *(sonarMatrix + (rowIndex * sizeOfMatrix) + colIndex);
            unsigned short average = getAverage(prevRow, currRow, sonarMatrix, sizeOfMatrix, rowIndex, colIndex);
            *(sonarMatrix + (rowIndex * sizeOfMatrix) + colIndex) = average;
        }

        memcpy(prevRow, currRow, sizeOfMatrix * sizeof(unsigned short));

    }

    free(prevRow);
    free(currRow);
}

void printMatrix(unsigned short *matrix, unsigned short n, const char *label)
{
    printf("\n%s\n", label);
    for (unsigned short row = 0; row < n; row++)
    {
        for (unsigned short col = 0; col < n; col++)
            printf("%hu ", *(matrix + row * n + col));
        printf("\n");
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

    srand(time(NULL));

    for (unsigned short rowIndex = 0; rowIndex < sizeOfMatrix; rowIndex++)
    {
        for (unsigned short colIndex = 0; colIndex < sizeOfMatrix; colIndex++)
        {
            int randomMatrixInput = rand() % 256;
            *(sonarImageMatrix + (rowIndex * sizeOfMatrix) + colIndex) = (unsigned short)randomMatrixInput;
        }
    }

    printMatrix(sonarImageMatrix, sizeOfMatrix, "Original");

    rotateMatrix(sonarImageMatrix, sizeOfMatrix);

    printMatrix(sonarImageMatrix, sizeOfMatrix, "Rotated");

    smoothingFilter(sonarImageMatrix, sizeOfMatrix);

    printMatrix(sonarImageMatrix, sizeOfMatrix, "Final Output");

    free(sonarImageMatrix);
}