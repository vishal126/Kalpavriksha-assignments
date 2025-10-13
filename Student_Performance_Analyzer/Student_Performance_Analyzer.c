#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct studentDetails
{
    unsigned int studentRollNumber;
    char studentName[26];
    unsigned int studentMarks[3];
    unsigned int totalMarks;
    float averageMarks;
    char grade;
    char stars[5];
} student;

int getIntInput(const char *prompt)
{
    char buffer[100];
    int value;
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

int comp(const void *student1, const void *student2)
{
    return ((student *)student1)->studentRollNumber - ((student *)student2)->studentRollNumber;
}

void getStudentDetailsInput(student *studentsArray, int numberOfStudents)
{
    char studentDetailsInputBuffer[100];

    for (int i = 0; i < numberOfStudents; i++)
    {
        printf("Enter details of student %d in the following format: \"rollNumber name mark1 mark2 mark3\": ", i + 1);
        fgets(studentDetailsInputBuffer, sizeof(studentDetailsInputBuffer), stdin);

        if (sscanf(studentDetailsInputBuffer, "%u %s %u %u %u", &studentsArray[i].studentRollNumber, &studentsArray[i].studentName, &studentsArray[i].studentMarks[0], &studentsArray[i].studentMarks[1], &studentsArray[i].studentMarks[2]) != 5)
        {
            printf("please enter valid input in described form\n");
            i--;
        }
        else
        {
            if ((studentsArray[i].studentMarks[0] > 100) ||
                (studentsArray[i].studentMarks[1] > 100) ||
                (studentsArray[i].studentMarks[2] > 100))

            {
                printf("marks are not in range input correct marks within range 0-100\n");
                i--;
            }
        }
    }
}

void setTotalAndAverageMarks(student *studentDetailsArray, int numberOfStudents)
{
    for (int studentDetailsArrayCounter = 0; studentDetailsArrayCounter < numberOfStudents; studentDetailsArrayCounter++)
    {
        int totalMarksSum = 0;
        for (int marksCounter = 0; marksCounter < 3; marksCounter++)
        {
            totalMarksSum += studentDetailsArray[studentDetailsArrayCounter].studentMarks[marksCounter];
        }

        studentDetailsArray[studentDetailsArrayCounter].totalMarks = totalMarksSum;

        float individualAverageMarks = totalMarksSum / 3.0;

        studentDetailsArray[studentDetailsArrayCounter].averageMarks = individualAverageMarks;
    }
}

void setGradeAndStars(student *studentDetailsArray, int numberOfStudents)
{
    for (int studentDetailsArrayCounter = 0; studentDetailsArrayCounter < numberOfStudents; studentDetailsArrayCounter++)
    {
        int average = studentDetailsArray[studentDetailsArrayCounter].averageMarks;

        if (average >= 85)
        {
            studentDetailsArray[studentDetailsArrayCounter].grade = 'A';
            strcpy(studentDetailsArray[studentDetailsArrayCounter].stars, "*****");
        }
        else if (average >= 70)
        {
            studentDetailsArray[studentDetailsArrayCounter].grade = 'B';
            strcpy(studentDetailsArray[studentDetailsArrayCounter].stars, "****");
        }
        else if (average >= 50)
        {
            studentDetailsArray[studentDetailsArrayCounter].grade = 'C';
            strcpy(studentDetailsArray[studentDetailsArrayCounter].stars, "***");
        }
        else if (average >= 35)
        {
            studentDetailsArray[studentDetailsArrayCounter].grade = 'D';
            strcpy(studentDetailsArray[studentDetailsArrayCounter].stars, "**");
        }
        else
        {
            studentDetailsArray[studentDetailsArrayCounter].grade = 'F';
        }
    }
}

void printStudentsRollNumber(student *studentDetailsArray, int studentArrayCounter, int numberofStudents)
{
    if (studentArrayCounter == numberofStudents)
    {
        return;
    }
    printf("%d ", studentDetailsArray[studentArrayCounter].studentRollNumber);
    printStudentsRollNumber(studentDetailsArray, studentArrayCounter + 1, numberofStudents);
}

void printStudentDetails(student *studentDetailsArray, int numberofStudents)
{
    for (int studentDetailsArrayCounter = 0; studentDetailsArrayCounter < numberofStudents; studentDetailsArrayCounter++)
    {
        printf("Roll: %u\n", studentDetailsArray[studentDetailsArrayCounter].studentRollNumber);
        printf("Name: %s\n", studentDetailsArray[studentDetailsArrayCounter].studentName);
        printf("Total: %u\n", studentDetailsArray[studentDetailsArrayCounter].totalMarks);
        printf("Average: %.2f\n", studentDetailsArray[studentDetailsArrayCounter].averageMarks);
        printf("Grade: %c\n", studentDetailsArray[studentDetailsArrayCounter].grade);

        if (studentDetailsArray[studentDetailsArrayCounter].grade == 'F')
        {
            continue;
        }
        printf("Performance: %s\n", studentDetailsArray[studentDetailsArrayCounter].stars);
        printf("\n");
    }

    printf("\n");
    
    printf("List of Roll Numbers (via recursion): ");
    printStudentsRollNumber(studentDetailsArray, 0, numberofStudents);
    printf("\n");
}

int main()
{
    unsigned int numberOfStudents = getIntInput("Enter the number of students you want to enter details for: ");

    while (1)
    {
        if (numberOfStudents >= 1 && numberOfStudents <= 100)
        {
            break;
        }
        else
        {
            numberOfStudents = getIntInput("Enter the number of students between 1 to 100:   ");
        }
    }
    student *studentsArray = (student *)malloc(numberOfStudents * sizeof(student));

    if (studentsArray == NULL)
    {
        printf("Dynamic memory allocation failed.");
        return 1;
    }

    getStudentDetailsInput(studentsArray, numberOfStudents);

    qsort(studentsArray, numberOfStudents, sizeof(studentsArray[0]), comp);

    setTotalAndAverageMarks(studentsArray, numberOfStudents);

    setGradeAndStars(studentsArray, numberOfStudents);

    printStudentDetails(studentsArray, numberOfStudents);

    free(studentsArray);
}