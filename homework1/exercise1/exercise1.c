#include <stdio.h>
#include <string.h>
#include <ctype.h>

int isReverseColor(char c) {
    return c == 'S' || c == 'T' || c == 'Y' || c == 'Z';
}
int isValidSecondChar(char c) {
    return c == 'P' || c == 'R' || c == 'S' || c == 'T' ||
           c == 'W' || c == 'X' || c == 'Y' || c == 'Z';
}

void displayEmergencyAction(char first, char second, int reverseColor, int hasEvacuation) {
    printf("***Emergency action advice***\n");
    printf("Material:    ");
    switch (first) {
        case '1': printf("jets\n"); break;
        case '2': printf("fog\n"); break;
        case '3': printf("foam\n"); break;
        case '4': printf("dry agent\n"); break;
        default: printf("Unknown\n"); return;
    }

    printf("Reactivity:  ");
    printf(reverseColor ? "can be violently reactive\n" : "not violently reactive\n");

    printf("Protection:  ");
    switch (second) {
        case 'P': case 'W': case 'R': case 'X':
            printf("full protective clothing must be worn\n");
            break;
        case 'S': case 'T': case 'Y': case 'Z':
            if (reverseColor)
                printf("breathing apparatus, protective gloves for fire only\n");
            else
                printf("breathing apparatus\n");
            break;
        default:
            printf("Unknown\n");
            return;
    }

    printf("Containment: ");
    switch (second) {
        case 'P': case 'R': case 'S': case 'T':
            printf("may be diluted and washed down the drain\n");
            break;
        case 'W': case 'X': case 'Y': case 'Z':
            printf("need to avoid spillages from entering drains or water courses\n");
            break;
        default:
            printf("no special containment needed\n");
            break;
    }

    if (hasEvacuation) {
        printf("Evacuation:  consider evacuation\n");
    }

    printf("*****************************\n");
}

int main() {
    char hazchemCode[4];  // HAZCHEM code (max length 3 + null terminator)
    char reverseColorInput[4];
    int reverseColor = 0;
    int hasEvacuation = 0;

    printf("Enter HAZCHEM code: ");
    scanf("%3s", hazchemCode);

    int len = strlen(hazchemCode);
    if (hazchemCode[0] < '1' || hazchemCode[0] > '4' || len < 2 || len > 3 || !isValidSecondChar(hazchemCode[1]) || (len == 3 && hazchemCode[2] != 'E')) {
        printf("Invalid HAZCHEM code\n");
        return 1;
    }

    if (isReverseColor(hazchemCode[1])) {
        printf("Is the %c reverse coloured? ", hazchemCode[1]);
        scanf("%3s", reverseColorInput);
        reverseColor = (tolower(reverseColorInput[0]) == 'y');
    }

    if (strlen(hazchemCode) == 3 && hazchemCode[2] == 'E') {
        hasEvacuation = 1;
    }

    printf("\n");
    displayEmergencyAction(hazchemCode[0], hazchemCode[1], reverseColor, hasEvacuation);

    return 0;
}
