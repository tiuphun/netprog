#ifndef SCOREBOARD_H
#define SCOREBOARD_H
#define MAX_STUDENTS 100
#define MAX_NAME_LEN 50

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
    float progressWeight;
    float finalWeight;
    char semesterID[10];
    int studentCount;
    Student* studentList;
    struct Subject* next;
} Subject;

// Function prototypes
void addScoreBoard();
void addScore();
void removeScore();
void searchScore();
void displayScoreBoard(Subject* subject);
void createScoreReport(Subject* subject);
void displayScoreReport(Subject* subject);
void displayScoreBoardAndReport();
Student* createStudent(float progressWeight, float finalWeight);
char calculateGrade(float progressMark, float finalMark, float progressWeight, float finalWeight);
void appendStudent(Subject* subject, Student* newStudent);
Subject* findSubject(const char* subjectID, const char* semesterID);
void writeSubjectToFile(Subject* subject);
int askToContinue(const char* message);

extern Subject* subjectList;

#endif
