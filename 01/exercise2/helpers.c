#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scoreboard.h"


char calculateGrade(float progressMark, float finalMark, float progressWeight, float finalWeight) {
    float totalMark = ((progressMark * progressWeight) + (finalMark * finalWeight))/100;

    if (totalMark >= 8.5) {
        return 'A';
    } else if (totalMark >= 7.0) {
        return 'B';
    } else if (totalMark >= 5.5) {
        return 'C';
    } else if (totalMark >= 4.0) {
        return 'D';
    } else {
        return 'F';
    }
}

Student* createStudent(float progressWeight, float finalWeight) {
    Student* newStudent = (Student*)malloc(sizeof(Student));
    printf("Enter student ID: ");
    scanf("%s", newStudent->id);
    printf("Enter first name: ");
    scanf(" %[^\n]%*c", newStudent->firstName);
    printf("Enter last name: ");
    scanf("%s", newStudent->lastName);
    printf("Enter progress mark: ");
    scanf("%f", &newStudent->progressMark);
    printf("Enter final mark: ");
    scanf("%f", &newStudent->finalMark);

    newStudent->grade = calculateGrade(newStudent->progressMark, newStudent->finalMark, progressWeight, finalWeight);
    newStudent->next = NULL;

    return newStudent;
}

void appendStudent(Subject* subject, Student* newStudent) {
    if (subject->studentList == NULL) {
        subject->studentList = newStudent;
    } else {
        Student* temp = subject->studentList;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newStudent;
    }
    subject->studentCount++;
}

int askToContinue(const char* message) {
    char continueChoice;
    printf("%s (y/Y to continue): ", message);
    scanf(" %c", &continueChoice);
    return (continueChoice == 'y' || continueChoice == 'Y');
}

Subject* findSubject(const char* subjectID, const char* semesterID) {
    Subject* temp = subjectList;
    while (temp != NULL) {
        if (strcmp(temp->subjectID, subjectID) == 0 && strcmp(temp->semesterID, semesterID) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void writeSubjectToFile(Subject* subject) {
    char filename[50];
    sprintf(filename, "%s_%s.txt", subject->subjectID, subject->semesterID);
    
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        // Write subject details
        fprintf(file, "SubjectID|%s\n", subject->subjectID);
        fprintf(file, "Subject|%s\n", subject->subjectName);
        fprintf(file, "Weights|Progress: %.2f | Final: %.2f\n", subject->progressWeight, subject->finalWeight);
        fprintf(file, "Semester|%s\n", subject->semesterID);
        fprintf(file, "StudentCount|%d\n", subject->studentCount);
        
        // Write students
        Student* temp = subject->studentList;
        while (temp != NULL) {
            fprintf(file, "Student|%s|%s|%s|Progress: %.1f|Final: %.1f|Grade: %c\n",
                    temp->id, temp->firstName, temp->lastName, temp->progressMark, temp->finalMark, temp->grade);
            temp = temp->next;
        }
        fclose(file);
        printf("Subject %s (Semester %s) saved to file %s.\n", subject->subjectID, subject->semesterID, filename);
    } else {
        printf("Error: Unable to save to file %s.\n", filename);
    }
}