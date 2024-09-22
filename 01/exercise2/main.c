#include <stdio.h>
#include <stdlib.h>
#include "scoreboard.h"

Subject* subjectList = NULL;

int main() {
    int choice;

    do {
        printf("Learning Management System\n");
        printf("-------------------------------------\n");
        printf("1. Add a new score board\n");
        printf("2. Add score\n");
        printf("3. Remove score\n");
        printf("4. Search score\n");
        printf("5. Display score board and score report\n");
        printf("Your choice (1-5, other to quit): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                do {
                    addScoreBoard();
                } while (askToContinue("Do you want to continue adding score boards?"));
                break;
            case 2:
                do {
                    addScore();
                } while (askToContinue("Do you want to continue adding scores?"));
                break;
            case 3:
                do {
                    removeScore();
                } while (askToContinue("Do you want to continue removing scores?"));
                break;
            case 4:
                do {
                    searchScore();
                } while (askToContinue("Do you want to continue searching scores?"));
                break;
            case 5:
                do {
                    displayScoreBoardAndReport();
                } while (askToContinue("Do you want to continue displaying score boards and report?"));
                break;
            default:
                printf("Exiting...\n");
        }
    } while (choice >= 1 && choice <= 5);

    return 0;
}
