#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NAME_LENGTH 51 // zoznam konstant pre rozne dlzky hodnot
#define BREED_LENGTH 31
#define MAX_LINE_LENGTH 64
#define DATE_LENGTH 9
#define HEIGHT_LENGTH 6
#define WEIGHT_LENGTH 14

/**
FIIT STU
Predmet: Proceduralne programovanie
Semester: ZS 2020/2021
Autor: Marko Stahovec

Uloha: Projekt 2
Funkcia programu: Spajane zoznamy
***/

typedef struct animal {
    char name[NAME_LENGTH];
    char breed[BREED_LENGTH];
    char height[HEIGHT_LENGTH];
    char weight[WEIGHT_LENGTH];
    char birth_date[DATE_LENGTH];
    char feed_date[DATE_LENGTH];
    char caretaker[NAME_LENGTH];
    struct animal *next;
} ANIMAL;

void addTerminator(char *buff, int str_len)
{
    for (int i = 0; i < str_len; i++) {
        if (buff[i] == 10 || buff[i] == 13) {
            buff[i] = '\0';
        }
    }
}

void readFile(char* buff, FILE* file) {
    fgets(buff, MAX_LINE_LENGTH, file);
    size_t len = strlen(buff);
    if(len <= 1) {
        //printf("Zly vstup\n");
    }
    else {
        if(buff[len - 2] == '\r' || buff[len - 2] == '\n') {
            buff[len - 2] = '\0'; // manualne pridavanie nuloveho znaku
        }
        if(buff[len - 1] == '\r' || buff[len - 1] == '\n') {
            buff[len - 1] = '\0'; // manualne pridavanie nuloveho znaku
        }
    }
}


ANIMAL *loadAnimals(ANIMAL* animal, FILE* file) {
    char buff[MAX_LINE_LENGTH];

    // nastavenie buffera, aby obsahoval same nuly
    memset(buff, 0, MAX_LINE_LENGTH);

    if(feof(file)) {
        exit(1);
    }

    readFile(buff, file);

    ANIMAL *current = animal;
    readFile(buff, file);

    // meno
    memset(current->name, 0, NAME_LENGTH);
    strncpy(current->name, buff, NAME_LENGTH-1);

    // druh
    readFile(buff, file);
    memset(current->breed, 0, BREED_LENGTH);
    strncpy(current->breed, buff, BREED_LENGTH-1);

    // vyska
    readFile(buff, file);
    memset(current->height, 0, HEIGHT_LENGTH);
    strncpy(current->height, buff, HEIGHT_LENGTH-1);

    // vaha
    readFile(buff, file);
    memset(current->weight, 0, WEIGHT_LENGTH);
    strncpy(current->weight, buff, WEIGHT_LENGTH-1);

    // datum narodenia
    readFile(buff, file);
    memset(current->birth_date, 0, DATE_LENGTH);
    strncpy(current->birth_date, buff, DATE_LENGTH-1);

    // datum krmenia
    readFile(buff, file);
    memset(current->feed_date, 0, DATE_LENGTH);
    strncpy(current->feed_date, buff, DATE_LENGTH-1);

    // meno osetrovatela
    readFile(buff, file);
    memset(current->caretaker, 0, NAME_LENGTH);
    strncpy(current->caretaker, buff, NAME_LENGTH-1);

    return current;
}

void printAnimal(ANIMAL* animal) {
    printf("Meno: %s\n", animal->name);
    printf("Druh: %s\n", animal->breed);
    printf("Vyska: %s\n", animal->height);
    printf("Vaha: %s\n", animal->weight);
    printf("Datum narodenia: %s\n", animal->birth_date);
    printf("Datum krmenia: %s\n", animal->feed_date);
    printf("Meno osetrovatela: %s\n", animal->caretaker);
}



void deallocateAnimals(ANIMAL *start){
    ANIMAL *current = start;
    while (current != NULL){
        ANIMAL *temp = current->next;
        free(current);
        current = temp;
    }
}



int stringCompare(char *s1,char *s2)
{
    for(int i = 0; i < (int)(strlen(s1)); i++)
    {
        if(tolower(s1[i]) != tolower(s2[i]) )
            return 1;
    }
    return 0;
}



ANIMAL* n(ANIMAL* first, int* size) {
    int count = 0;
    FILE *file = fopen("zvierata.txt", "r");
    if(!file) {
        fprintf(stderr, "Zaznamy neboli nacitane\n");
        exit(1);
    }

    deallocateAnimals(first);
    ANIMAL* current;
    first = current = NULL;


    if(file) {
        fseek(file, 0, SEEK_SET);
        while(!feof(file)) {
            ANIMAL *node = malloc(sizeof(ANIMAL));
            loadAnimals(node, file);
            node->next = NULL; /* toto mi tu chybalo */
            if (first == NULL) {
                current = first = node;
            }
            else {
                current = current->next = node;
            }
            count++;
        }
        *size = count;
        printf("Nacitalo sa %d zaznamov\n", count);
        if(file) {
            fclose(file);
        }
    }

    return first;
}


void v(ANIMAL* first) {
    int order_count = 1;
    ANIMAL* current = first;

    while(current != NULL) {
        printf("%d.\n", order_count);
        printAnimal(current);
        current = current->next;
        order_count++;
    }
}



void h(ANIMAL* first) {
    char buff[MAX_LINE_LENGTH];
    ANIMAL* current = first;
    int order_count = 1;
    int count_occurences = 0;
    if(!first) {
        fprintf(stderr, "Zoznam nie je vytvoreny.\n");
        return;
    }

    scanf("%s", buff);
    if(strlen(buff) != 8) {
        fprintf(stderr, "Zly format datumu.\n");
        return;
    }
    else {
        while(current != NULL) {
            if(strcmp(buff, current->feed_date) > 0) {
                printf("%d.\n", order_count);
                printAnimal(current);
                count_occurences++;
                order_count++;
            }
            current = current->next;
        }
    }
    if (count_occurences == 0) {
        printf("Vsetky zvierata boli k datumu %s nakrmene.\n", buff);
    }
}



ANIMAL* a(ANIMAL* first) {
    char buff_name[MAX_LINE_LENGTH];
    char buff_date[MAX_LINE_LENGTH];
    ANIMAL* current = first;

    if(!first) {
        fprintf(stderr, "Zoznam nie je vytvoreny.\n");
        return first;
    }

    fseek(stdin, 0, SEEK_END);
    fgets(buff_name, MAX_LINE_LENGTH, stdin); // nacitanie vstupu
    addTerminator(buff_name, MAX_LINE_LENGTH);
    if((strlen(buff_name) > 50) || (strlen(buff_name) <= 0)) {
        fprintf(stderr, "Zly format mena.\n");
        return first;
    }

    scanf("%s", buff_date);
    if(strlen(buff_date) != 8) {
        fprintf(stderr, "Zly format datumu.\n");
        return first;
    }
    else {
        while(current != NULL) {
            if(strcmp(buff_name, current->name) == 0) {
                strcpy(current->feed_date, buff_date);
                printf("Zviera s menom %s bolo naposledy nakrmene dna %s\n", current->name, buff_date);
                break;
            }
            else
            {
                current = current->next;
            }
        }
        return first;
    }
}



ANIMAL* z(ANIMAL* first, int *size) {
    char buff[MAX_LINE_LENGTH];

    if(!first) {
        fprintf(stderr, "Zoznam nie je vytvoreny.\n");
        return first;
    }

    fseek(stdin, 0, SEEK_END);
    fgets(buff, MAX_LINE_LENGTH, stdin); // nacitanie vstupu
    addTerminator(buff, MAX_LINE_LENGTH);
    if(strlen(buff) <= 0) {
        //fprintf(stderr, "Zly format mena.\n");
        return first;
    }


    ANIMAL* current = first;
    ANIMAL* previous = NULL;


    while(current != NULL) {

        if (stringCompare(current->name, buff) == 0) {
            if(current->next == NULL) { // specificky pripad ak ide o posledny zaznam
                free(current);
                if (previous == NULL) {
                    printf("Zviera s menom %s bolo vymazane\n", buff);
                    (*size)--;
                    exit(1);
                }
                previous->next = NULL;
                printf("Zviera s menom %s bolo vymazane\n", buff);
                (*size)--;
                break;
            }
            else {
                break;
            }
        }
        else if (current->next == NULL) {
            return first;
        }
        else {
            previous = current;
            current = current->next;
        }
    }


    if(current == first) {
        *first = (*first->next);
        printf("Zviera s menom %s bolo vymazane\n", buff);
        (*size)--;
    }
    else if (stringCompare(current->name, buff) == 0) {
        previous->next = current->next;
        printf("Zviera s menom %s bolo vymazane\n", buff);
        (*size)--;
    }

    return first;
}


void loadLine(char *buff)
{
    fseek(stdin, 0, SEEK_END);
    fgets(buff, MAX_LINE_LENGTH, stdin); // nacitanie vstupu
    addTerminator(buff, MAX_LINE_LENGTH);
}


ANIMAL *loadAnimal()
{
    char buff[MAX_LINE_LENGTH];
    /* zero out the buffer (equivalent to a for loop that sets every byte to 0)*/
    memset(buff, 0, MAX_LINE_LENGTH);

    ANIMAL *current = malloc(sizeof(ANIMAL));

    // meno
    loadLine(buff);
    memset(current->name, 0, NAME_LENGTH);
    strncpy(current->name, buff, NAME_LENGTH-1);

    // druh
    loadLine(buff);
    memset(current->breed, 0, BREED_LENGTH);
    strncpy(current->breed, buff, BREED_LENGTH-1);

    // vyska
    loadLine(buff);
    memset(current->height, 0, HEIGHT_LENGTH);
    strncpy(current->height, buff, HEIGHT_LENGTH-1);

    // vaha
    loadLine(buff);
    memset(current->weight, 0, WEIGHT_LENGTH);
    strncpy(current->weight, buff, WEIGHT_LENGTH-1);

    // datum narodenia
    loadLine(buff);
    memset(current->birth_date, 0, DATE_LENGTH);
    strncpy(current->birth_date, buff, DATE_LENGTH-1);

    // datum krmenia
    loadLine(buff);
    memset(current->feed_date, 0, DATE_LENGTH);
    strncpy(current->feed_date, buff, DATE_LENGTH-1);

    // meno osetrovatela
    loadLine(buff);
    memset(current->caretaker, 0, NAME_LENGTH);
    strncpy(current->caretaker, buff, NAME_LENGTH-1);

    return current;
}


ANIMAL *p_beginning(ANIMAL* first)
{
    ANIMAL *current = loadAnimal();
    current->next = first;
    return current;
}

ANIMAL *p_end(ANIMAL* first)
{
    ANIMAL* current, *last;
    current = loadAnimal();
    current->next = NULL;
    last = first;
    while (last->next!=NULL)
    {
        last = last->next;
    }
    last->next = current;

    return first;
}

ANIMAL *p_insert(ANIMAL* first, int position)
{
    ANIMAL* inserted;
    ANIMAL* current = first;
    int count_line = 1;
    while (current != NULL) {
        if (count_line == (position-1)) {
            inserted = loadAnimal();
            inserted->next = current->next;
            current->next = inserted;
            break;
        }
        else {
            current = current->next;
            count_line++;
        }
    }

    return first;
}


ANIMAL *p(ANIMAL* first, int *size)
{
    int number = 0;
    int count = *size;

    if(!first) {
        fprintf(stderr, "Zoznam nie je vytvoreny.\n");
        return first;
    }

    scanf("%d", &number);
    if ((number <= 0)) {
        return first;
    }
    else if (number == 1) {
        first = p_beginning(first);
        count++;
    }
    else if (number > *size) {
        first = p_end(first);
        count++;
    }
    else {
        first = p_insert(first, number);
        count++;
    }
    *size = count;
    return first;
}




int main() {
    char buff[2];
    ANIMAL* first = NULL;
    int size = 0; // premenna pre cykly (pocet zvieratok v txt)
    do {
        fgets(buff, 2, stdin);
        switch(buff[0]) {
            case 'n':
                first = n(first, &size);
                break;
            case 'v':
                if(first == NULL) {
                    break;
                }
                v(first);
                break;
            case 'p':
                first = p(first, &size);
                break;
            case 'z':
                first = z(first, &size);
                break;
            case 'h':
                h(first);
                break;
            case 'a':
                a(first);
                break;
            default:
                break;
        }
    } while(buff[0] != 'k');

    deallocateAnimals(first);

    return 0;
}