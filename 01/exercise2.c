#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student {
    char id[10];
    char firstName[30];
    char lastName[30];
    float progressMark;
    float finalMark;
    char grade;
    struct Student* next;
} Student;

typedef struct Subject {
    char subjectID[10];
    char subjectName[50];
    char semesterID[10];
    int studentCount;
    Student* studentList;
    struct Subject* next;
} Subject;

void addScoreBoard();
void addScore();
void removeScore();
void searchScore();
void displayScoreBoard();
Student* createStudent();
char calculateGrade(float progressMark, float finalMark);
void appendStudent(Subject* subject, Student* newStudent);
Subject* findSubject(const char* subjectID, const char* semesterID);
void writeSubjectToFile(Subject* subject);


Student* createStudent();
char calculateGrade(float progressMark, float finalMark);
void appendStudent(Subject* subject, Student* newStudent);

Subject* subjectList = NULL;

int main() {
    int choice;

    do {
        // Display menu
        printf("Learning Management System\n");
        printf("-------------------------------------\n");
        printf("1. Add a new score board\n");
        printf("2. Add score\n");
        printf("3. Remove score\n");
        printf("4. Search score\n");
        printf("5. Display score board and score report\n");
        printf("Your choice (1-5, other to quit): ");
        scanf("%d", &choice);

        // Handle user's choice
        switch (choice) {
            case 1:
                addScoreBoard();
                break;
            case 2:
                addScore();
                break;
            case 3:
                removeScore();
                break;
            case 4:
                searchScore();
                break;
            case 5:
                displayScoreBoard();
                break;
            default:
                printf("Exiting...\n");
        }
    } while (choice >= 1 && choice <= 5);

    return 0;
}

// Function to create a new student
Student* createStudent() {
    Student* newStudent = (Student*)malloc(sizeof(Student));
    printf("Enter student ID: ");
    scanf("%s", newStudent->id);
    printf("Enter first name: ");
    scanf("%s", newStudent->firstName);
    printf("Enter last name: ");
    scanf("%s", newStudent->lastName);
    printf("Enter progress mark: ");
    scanf("%f", &newStudent->progressMark);
    printf("Enter final mark: ");
    scanf("%f", &newStudent->finalMark);

    newStudent->grade = calculateGrade(newStudent->progressMark, newStudent->finalMark);
    newStudent->next = NULL;

    return newStudent;
}

char calculateGrade(float progressMark, float finalMark) {
    float totalMark = (progressMark * 0.3) + (finalMark * 0.7);

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

void addScoreBoard() {
    Subject* newSubject = (Subject*)malloc(sizeof(Subject));
    newSubject->studentList = NULL;
    newSubject->studentCount = 0;

    printf("Enter subject ID: ");
    scanf("%s", newSubject->subjectID);
    printf("Enter subject name: ");
    scanf(" %[^\n]%*c", newSubject->subjectName); // Input with spaces
    printf("Enter semester ID: ");
    scanf("%s", newSubject->semesterID);

    int numStudents;
    printf("Enter number of students: ");
    scanf("%d", &numStudents);

    // Add students
    for (int i = 0; i < numStudents; i++) {
        printf("Entering details for student %d:\n", i + 1);
        Student* newStudent = createStudent();
        appendStudent(newSubject, newStudent);
    }

    // Append the subject to the subject list
    newSubject->next = subjectList;
    subjectList = newSubject;

    // Write to file
    char filename[50];
    sprintf(filename, "%s_%s.txt", newSubject->subjectID, newSubject->semesterID);
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fprintf(file, "SubjectID|%s\n", newSubject->subjectID);
        fprintf(file, "Subject|%s\n", newSubject->subjectName);
        fprintf(file, "F|30|70\n");
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
        Student* newStudent = createStudent();
        appendStudent(subject, newStudent);
        writeSubjectToFile(subject);
    } else {
        printf("Subject not found!\n");
    }
}

// Function to remove a student's score
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

// Function to search for a student's score
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

// Function to display the score board and report
void displayScoreBoard() {
    char subjectID[10], semesterID[10];
    printf("Enter subject ID: ");
    scanf("%s", subjectID);
    printf("Enter semester ID: ");
    scanf("%s", semesterID);

    Subject* subject = findSubject(subjectID, semesterID);
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

