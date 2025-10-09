#include <stdio.h>
#include <ctype.h>

#define SIZE 100

int evaluateExpression(char *expr)
{
    int lastOpIdx = -1;
    long long res = 0;
    long long term = 0;
    char prevOp = '+';
    int i = 0;

    while (expr[i] != '\0')
    {
        if (isdigit(expr[i]))
        {
            long long operand = 0;
            while (isdigit(expr[i]))
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
    char expression[SIZE];

    printf("enter your expression\npress enter to end your input: ");
    fgets(expression, SIZE, stdin);

    char normalizedExpression[SIZE];
    int j=0;

    int i = 0, isCorrect = 1;
    char ch;

    while (expression[i] != '\0')
    {
        ch = expression[i];
        if (isspace(ch)) {
            i++;
            continue;
        }
        if (isalpha(ch))
            isCorrect = 0;
            
        normalizedExpression[j++] = ch;
        i++;
    }

    normalizedExpression[i] = '\0';

    printf("%s\n", normalizedExpression);

    if (!isCorrect)
    {
        printf("invalid string\n");
        return 0;
    }

    long long result = evaluateExpression(normalizedExpression);

    printf("%lld\n", result);

    return 0;
}