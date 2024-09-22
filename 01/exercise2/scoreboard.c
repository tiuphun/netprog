#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scoreboard.h"

void addScoreBoard() {
    Subject* newSubject = (Subject*)malloc(sizeof(Subject));
    newSubject->studentList = NULL;
    newSubject->studentCount = 0;

    printf("Enter subject ID: ");
    scanf("%s", newSubject->subjectID);
    printf("Enter subject name: ");
    scanf(" %[^\n]%*c", newSubject->subjectName); // Input with spaces
    printf("Enter progress weight: ");
    scanf("%f", &newSubject->progressWeight);
    printf("Enter final weight: ");
    scanf("%f", &newSubject->finalWeight);
    printf("Enter semester ID: ");
    scanf("%s", newSubject->semesterID);

    int numStudents;
    printf("Enter number of students: ");
    scanf("%d", &numStudents);

    for (int i = 0; i < numStudents; i++) {
        printf("Entering details for student %d:\n", i + 1);
        Student* newStudent = createStudent(newSubject->progressWeight, newSubject->finalWeight);
        appendStudent(newSubject, newStudent);
    }

    newSubject->next = subjectList;
    subjectList = newSubject;

    char filename[50];
    sprintf(filename, "%s_%s.txt", newSubject->subjectID, newSubject->semesterID);
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fprintf(file, "SubjectID|%s\n", newSubject->subjectID);
        fprintf(file, "Subject|%s\n", newSubject->subjectName);
        fprintf(file, "F|%.0f|%.0f\n", newSubject->progressWeight, newSubject->finalWeight);
        fprintf(file, "Semester|%s\n", newSubject->semesterID);
        fprintf(file, "StudentCount|%d\n", numStudents);
        Student* temp = newSubject->studentList;
        while (temp != NULL) {
            fprintf(file, "S|%s|%s|%s|%.1f|%.1f|%c|\n", temp->id, temp->firstName, temp->lastName, temp->progressMark, temp->finalMark, temp->grade);
            temp = temp->next;
        }
        fclose(file);
        printf("Score board saved to file %s.\n", filename);
    } else {
        printf("Error saving to file.\n");
    }
}

void addScore() {
    char subjectID[10], semesterID[10];
    printf("Enter subject ID: ");
    scanf("%s", subjectID);
    printf("Enter semester ID: ");
    scanf("%s", semesterID);

    Subject* subject = findSubject(subjectID, semesterID);
    if (subject != NULL) {
        Student* newStudent = createStudent(subject->progressWeight, subject->finalWeight);
        appendStudent(subject, newStudent);
        writeSubjectToFile(subject);
    } else {
        printf("Subject not found!\n");
    }
}


void removeScore() {
    char subjectID[10], semesterID[10], studentID[10];
    printf("Enter subject ID: ");
    scanf("%s", subjectID);
    printf("Enter semester ID: ");
    scanf("%s", semesterID);
    printf("Enter student ID to remove: ");
    scanf("%s", studentID);

    Subject* subject = findSubject(subjectID, semesterID);
    if (subject != NULL) {
        Student* temp = subject->studentList;
        Student* prev = NULL;

        while (temp != NULL) {
            if (strcmp(temp->id, studentID) == 0) {
                if (prev == NULL) {
                    subject->studentList = temp->next; // Remove from head
                } else {
                    prev->next = temp->next; // Remove from middle or end
                }
                free(temp);
                subject->studentCount--;
                writeSubjectToFile(subject);
                printf("Student score removed successfully.\n");
                return;
            }
            prev = temp;
            temp = temp->next;
        }
        printf("Student ID not found!\n");
    } else {
        printf("Subject not found!\n");
    }
}

void searchScore() {
    char subjectID[10], semesterID[10], studentID[10];
    printf("Enter subject ID: ");
    scanf("%s", subjectID);
    printf("Enter semester ID: ");
    scanf("%s", semesterID);
    printf("Enter student ID to search: ");
    scanf("%s", studentID);

    Subject* subject = findSubject(subjectID, semesterID);
    if (subject != NULL) {
        Student* temp = subject->studentList;
        while (temp != NULL) {
            if (strcmp(temp->id, studentID) == 0) {
                printf("Student found: %s %s | Progress Mark: %.1f | Final Mark: %.1f | Grade: %c\n",
                       temp->firstName, temp->lastName, temp->progressMark, temp->finalMark, temp->grade);
                return;
            }
            temp = temp->next;
        }
        printf("Student ID not found!\n");
    } else {
        printf("Subject not found!\n");
    }
}

void displayScoreBoardAndReport() {
    char subjectID[10], semesterID[10];
    printf("Enter subject ID: ");
    scanf("%s", subjectID);
    printf("Enter semester ID: ");
    scanf("%s", semesterID);
    Subject* subject = findSubject(subjectID, semesterID);

    displayScoreBoard(subject);
    createScoreReport(subject);
    displayScoreReport(subject);
}

void displayScoreBoard(Subject* subject) {
    if (subject != NULL) {
        printf("Subject ID: %s | Subject Name: %s | Semester: %s\n", subject->subjectID, subject->subjectName, subject->semesterID);
        printf("Total Students: %d\n", subject->studentCount);
        printf("Student List:\n");
        Student* temp = subject->studentList;
        while (temp != NULL) {
            printf("%s %s | Progress Mark: %.1f | Final Mark: %.1f | Grade: %c\n",
                   temp->firstName, temp->lastName, temp->progressMark, temp->finalMark, temp->grade);
            temp = temp->next;
        }
    } else {
        printf("Subject not found!\n");
    }
}

void createScoreReport(Subject* subject) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_%s_rp.txt", subject->subjectID, subject->semesterID);
    
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", filename);
        return;
    }

    fprintf(file, "SubjectID|%s\n", subject->subjectID);
    fprintf(file, "Subject|%s\n", subject->subjectName);
    fprintf(file, "Weights|Progress: %.2f | Final: %.2f\n", subject->progressWeight, subject->finalWeight);
    fprintf(file, "Semester|%s\n", subject->semesterID);
    fprintf(file, "StudentCount|%d\n", subject->studentCount);

    Student* temp = subject->studentList;
    while (temp != NULL) {
        temp->grade = calculateGrade(temp->progressMark, temp->finalMark, subject->progressWeight, subject->finalWeight);
        fprintf(file, "Student|%s|%s|%s|Progress: %.1f|Final: %.1f|Grade: %c\n",
                temp->id, temp->firstName, temp->lastName, temp->progressMark, temp->finalMark, temp->grade);
        temp = temp->next;
    }

    fclose(file);
    printf("Score report created for subject %s (Semester %s) and saved to file %s.\n", subject->subjectID, subject->semesterID, filename);
}

void displayScoreReport(Subject* subject) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_%s_rp.txt", subject->subjectID, subject->semesterID);
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }
    
    char line[256];
    int studentCount = 0;
    char highestName[MAX_NAME_LEN] = "";
    char lowestName[MAX_NAME_LEN] = "";
    float highestMark = 0.0, lowestMark = 10.0; // 10 is the max mark
    float totalMarks = 0.0;
    
    int gradeA = 0, gradeB = 0, gradeC = 0, gradeD = 0, gradeF = 0;
    
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'S') {  // Student line
            char studentID[10], firstName[MAX_NAME_LEN], lastName[MAX_NAME_LEN];
            float progressMark, finalMark;
            char grade;
            
            sscanf(line, "Student|%[^|]|%[^|]|%[^|]|Progress: %f|Final: %f|Grade: %c\n", 
                   studentID, firstName, lastName, &progressMark, &finalMark, &grade);
            
            // Combine first and last name
            char fullName[MAX_NAME_LEN];
            snprintf(fullName, sizeof(fullName), "%s %s", firstName, lastName);
            
            // Calculate total mark based on the two marks
            float totalMark = (progressMark + finalMark) / 2.0;
            totalMarks += totalMark;
            studentCount++;
            
            // Check for highest and lowest marks
            if (totalMark > highestMark) {
                highestMark = totalMark;
                strncpy(highestName, fullName, sizeof(highestName));
            }
            if (totalMark < lowestMark) {
                lowestMark = totalMark;
                strncpy(lowestName, fullName, sizeof(lowestName));
            }
            
            // Count grades for histogram
            switch (grade) {
                case 'A': gradeA++; break;
                case 'B': gradeB++; break;
                case 'C': gradeC++; break;
                case 'D': gradeD++; break;
                case 'F': gradeF++; break;
            }
        }
    }
    
    fclose(file);
    
    // Calculate the average mark
    float averageMark = totalMarks / studentCount;
    
    // Display the report
    printf("The student with the highest mark is: %s\n", highestName);
    printf("The student with the lowest mark is: %s\n", lowestName);
    printf("The average mark is: %.2f\n", averageMark);
    
    // Display histogram of grades
    printf("\nA histogram of the subject %s is:\n", subject->subjectID);
    printf("A:");
    for (int i = 0; i < gradeA; i++) printf("*");
    printf("\nB:");
    for (int i = 0; i < gradeB; i++) printf("*");
    printf("\nC:");
    for (int i = 0; i < gradeC; i++) printf("*");
    printf("\nD:");
    for (int i = 0; i < gradeD; i++) printf("*");
    printf("\nF:");
    for (int i = 0; i < gradeF; i++) printf("*");
    printf("\n");
}
