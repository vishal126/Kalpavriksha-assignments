#include <stdio.h>

#define size 100

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

    return 0;
}