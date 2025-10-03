#include <stdio.h>

#define size 100

int calculation(char *expr)
{
    int lastOpIdx = -1;
    int res = 0;
    int term = 0;
    char prevOp = '+';
    int i = 0;

    while (expr[i] != '\0')
    {
        if (expr[i] >= '0' && expr[i] <= '9')
        {
            int operand = 0;
            while (expr[i] >= '0' && expr[i] <= '9')
            {
                operand = operand * 10 + (expr[i] - '0');
                i++;
            }

            if (prevOp == '+')
            {
                term = operand;
            }
            else if (prevOp == '-')
            {
                term = -(operand);
            }
            else if (prevOp == '*')
            {
                term = term * operand;
            }
            else if (prevOp == '/')
            {
                if (operand == 0)
                {
                    printf("Error: Division by zero.\n");
                    return -1;
                }
                term = term / operand;
            }
        }
        else if (expr[i] == '+' || expr[i] == '-')
        {
            // printf("%d %d\n",lastOpIdx, i);
            if (lastOpIdx == i - 1)
            {
                printf("Error: Invalid expression.\n");
                return -1;
            }
            lastOpIdx = i;
            res += term;
            prevOp = expr[i];
            i++;
        }
        else if (expr[i] == '*' || expr[i] == '/')
        {
            if (lastOpIdx == i - 1)
            {
                printf("Error: Invalid expression.\n");
                return -1;
            }
            lastOpIdx = i;
            prevOp = expr[i];
            i++;
        }
        else
        {
            printf("Error: Invalid expression.\n");
            return -1;
        }
    }
    res += term;
    return res;
}

int main()
{
    char expression[size];
    int i = 0, isCorrect = 1;
    printf("enter your expression\npress enter to end your input: ");
    char ch;
    while (i < size - 1)
    {
        ch = getchar();
        if (ch == '\n')
        {
            break;
        }
        if (ch == ' ')
            continue;
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
            isCorrect = 0;
        expression[i++] = ch;
    }
    expression[i] = '\0';
    printf("%s\n", expression);
    if (!isCorrect)
    {
        printf("invalid string\n");
        return 0;
    }

    int result = calculation(expression);

    if (result != -1)
    {
        printf("%d\n", result);
    }

    return 0;
}