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

typedef struct animal { // struktura na vytvorenie noveho datoveho typu ANIMAL
    char name[NAME_LENGTH]; // samostatne polia pre hodnoty
    char breed[BREED_LENGTH];
    char height[HEIGHT_LENGTH];
    char weight[WEIGHT_LENGTH];
    char birth_date[DATE_LENGTH];
    char feed_date[DATE_LENGTH];
    char caretaker[NAME_LENGTH];
    struct animal *next; // pointer, ktory bude spajat aktualne zviera s nasledujucim v zozname
} ANIMAL;

void addTerminator(char *buff, int str_len) // funkcia, ktora prida na koniec inputnuteho stringu znak \0
{
    for (int i = 0; i < str_len; i++) { // cyklus ktory prejde celym stringom
        if (buff[i] == 10 || buff[i] == 13) { //
            buff[i] = '\0';
        }
    }
}

void readFile(char* buff, FILE* file) { // vylepseny fgets pre citanie zo suboru
    fgets(buff, MAX_LINE_LENGTH, file); // nacitavanie riadkov
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

void loadLine(char *buff) // funkcia na nacitanie riadka zo vstupu, ktora zaroven prida \0 na koniec vstupu
{
    fseek(stdin, 0, SEEK_END);
    fgets(buff, MAX_LINE_LENGTH, stdin); // nacitanie vstupu
    addTerminator(buff, MAX_LINE_LENGTH);
}


ANIMAL* loadAnimals(ANIMAL* animal, FILE* file) {
    char buff[MAX_LINE_LENGTH];

    // nastavenie buffera, aby obsahoval same nuly
    memset(buff, 0, MAX_LINE_LENGTH);

    if(feof(file)) { // kontrola ci je subor otvoreny, aby sme mohli zacat citat
        exit(1);
    }

    readFile(buff, file); // toto citanie prebehne, no nikam sa nezapise, t.j. odignoruje $$$

    ANIMAL *current = animal; // vytvorenie instancie typu ANIMAL
    readFile(buff, file); // citanie mena

    // meno
    memset(current->name, 0, NAME_LENGTH);
    strncpy(current->name, buff, NAME_LENGTH-1); // skopirovanie obsahu z buffra do daneho pola

    // opakovanie toho isteho pre rozne typy dat
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

void printAnimal(ANIMAL* animal) { // funkcia na vyprintovanie jedneho zaznamu (jedno zviera)
    printf("Meno: %s\n", animal->name);
    printf("Druh: %s\n", animal->breed);
    printf("Vyska: %s\n", animal->height);
    printf("Vaha: %s\n", animal->weight);
    printf("Datum narodenia: %s\n", animal->birth_date);
    printf("Datum krmenia: %s\n", animal->feed_date);
    printf("Meno osetrovatela: %s\n", animal->caretaker);
}



void deallocateAnimals(ANIMAL *start){ // funkcia na dealokaciu vsetkych zaznamov
    ANIMAL *current = start; // vytvorenie typu ANIMAL, ktore bude zo zaciatku ukazovat na prve zviera
    while (current != NULL){ // cyklus, ktory prejde celym zoznamom
        ANIMAL *temp = current->next; // do docasneho typu ANIMAL vlozime to, na co ukazuje aktualny ukazovatel
        free(current); // uvolnenie zaznamu na ktorom aktualne sme
        current = temp; // aktualnym sa stane to, co bolo ulozene v docasnom type ANIMAL
    }
}



int stringCompare(char *s1,char *s2) // funkcia ktora porovna 2 stringy bez ohladu na velkost pismen
{
    for(int i = 0; i < (int)(strlen(s1)); i++) // cyklus ktory prejde celym stringom
    {
        if(tolower(s1[i]) != tolower(s2[i]) ) // porovnanie ak su oba stringy s malymi pismenami
            return 1; // pri aspon jednej nezhode vraciame 1
    }
    return 0;
}



ANIMAL* n(ANIMAL* first, int* size) {
    int count = 0;
    FILE *file = fopen("zvierata.txt", "r"); // otvorenie suboru
    if(!file) { // overenie ci je subor otvoreny
        fprintf(stderr, "Zaznamy neboli nacitane\n");
        exit(1); // ak ano, ukonci program
    }

    deallocateAnimals(first); // pri spustani funkcii n musime dealokovat vsetko, co tu bolo predtym
    ANIMAL* current;
    first = current = NULL; // nastavenie vsetkych pointrov na NULL


    if(file) { // ak je subor otvoreny
        fseek(file, 0, SEEK_SET); // nastavime sa na zaciatok suboru
        while(!feof(file)) { // a prejdeme cely subor cyklom
            ANIMAL *node = malloc(sizeof(ANIMAL)); // alokujeme si pamat pre velkost jedneho zaznamu
            loadAnimals(node, file); // nacitame jedno zviera
            node->next = NULL; // preventivne nastavime pointer next na NULL (potrebne ak ide o posledne zviera)
            if (first == NULL) { // ak ide o prvy zaznam
                current = first = node; // nastavime aktualne aj prve zviera
            }
            else { // ak nejde o prvy zaznam
                current = current->next = node; // aktualne bude ukazovat na novonacitane
            }
            count++; // pomocna premenna na ziskanie poctu zaznamov
        }
        *size = count; // pointrom si aktualizujeme pocet zaznamov pre cely program
        printf("Nacitalo sa %d zaznamov\n", count); // vypis poctu zaznamov
        if(file) { // uzatvorenie suboru
            fclose(file);
        }
    }

    return first; // vratenie adresy pre prve zviera
}


void v(ANIMAL* first) { // funkcia pre vypis celeho zoznamu
    int order_count = 1; // cislovanie zoznamu
    ANIMAL* current = first; // nastavenie aktualneho zaznamu na prvy
    if(!first) { // ak nebola stlacena funkcia n
        fprintf(stderr, "Zoznam nie je vytvoreny.\n");
        return;
    }

    while(current != NULL) { // cyklus ktory prejde vsetkymi zaznamami
        printf("%d.\n", order_count); // vypis cisla zaznamu
        printAnimal(current); // vypis vsetkych dat o zvierati
        current = current->next; // nastavenie pointra na dalsie zviera
        order_count++;
    }
}



void h(ANIMAL* first) {
    char buff[MAX_LINE_LENGTH]; // buffer do ktoreho si nacitame hodnotu zo vstupu
    ANIMAL* current = first; // nastavenie aktualneho zaznamu na prvy
    int order_count = 1;
    int count_occurences = 0;
    if(!first) { // ak nebola stlacena funkcia n
        fprintf(stderr, "Zoznam nie je vytvoreny.\n");
        return;
    }

    scanf("%s", buff); // nacitanie hodnot do buffra
    if(strlen(buff) != 8) {
        fprintf(stderr, "Zly format datumu.\n");
        return;
    }
    else { // ak sme nacitali spravny datum
        while(current != NULL) { // prejdeme celym zoznamom
            if(strcmp(buff, current->feed_date) > 0) { // ak je datum krmenia v buffri neskorsi ako datum krmenia
                printf("%d.\n", order_count);
                printAnimal(current); // vyprintujeme dane zviera
                count_occurences++;
                order_count++;
            }
            current = current->next; // pointer sa nastavi na dalsie zviera
        }
    }
    if (count_occurences == 0) { // specificky pripad ak su vsetky zvierata nakrmene
        printf("Vsetky zvierata boli k datumu %s nakrmene.\n", buff);
    }
}



ANIMAL* a(ANIMAL* first) {
    char buff_name[MAX_LINE_LENGTH]; // buffer do ktoreho si nacitame meno
    char buff_date[MAX_LINE_LENGTH]; // buffer do ktoreho si nacitame datum
    ANIMAL* current = first; // nastavenie aktualneho zaznamu na zaciatok

    if(!first) { // ak zoznam nebol vytvoreny
        fprintf(stderr, "Zoznam nie je vytvoreny.\n");
        return first;
    }

    loadLine(buff_name); // specificke nacitanie vstupu pre pripad ak by meno nebolo jednoslovne
    if((strlen(buff_name) > 50) || (strlen(buff_name) <= 0)) { // kontrola ci je buffer spravnej dlzky
        fprintf(stderr, "Zly format mena.\n");
        return first;
    }

    scanf("%s", buff_date); // nacitanie noveho datumu krmenia
    if(strlen(buff_date) != 8) { // kontrola ci je datum spravnej dlzky
        fprintf(stderr, "Zly format datumu.\n");
        return first;
    }
    else { // ak boli vstupy spravne
        while(current != NULL) { // cyklus pre prejdenie celym zoznamom
            if(strcmp(buff_name, current->name) == 0) { // ak sa meno z buffra rovna menu aktualne nacitaneho zvierata
                strcpy(current->feed_date, buff_date); // aktualizujeme mu datum krmenia
                printf("Zviera s menom %s bolo naposledy nakrmene dna %s\n", current->name, buff_date);
                break; // escape z cyklu
            }
            else
            {
                current = current->next; // pointer sa nastavi na dalsie zviera
            }
        }
        return first; // vratime pointer na prvy zaznam
    }
}



ANIMAL* z(ANIMAL* first, int *size) {
    char buff[MAX_LINE_LENGTH]; // buffer do ktoreho nacitame meno

    if(!first) { // ak nemame vytvoreny zoznam
        fprintf(stderr, "Zoznam nie je vytvoreny.\n");
        return first;
    }

    loadLine(buff); // nacitanie mena zo stringu
    if(strlen(buff) <= 0) { // aby program nespadol pri prazdnom vstupe
        //fprintf(stderr, "Zly format mena.\n");
        return first;
    }


    ANIMAL* current = first; // nastavenie aktualneho zaznamu na prvy
    ANIMAL* previous = NULL; // predchadzajuci zoznam zatial neexistuje


    while(current != NULL) { // cyklus ktory prejde cely suborom
        if (stringCompare(current->name, buff) == 0) { // ak sa nacitane meno zhoduje s menom nacitaneho zvierata
            if(current->next == NULL) { // specificky pripad ak ide o posledny zaznam
                free(current); // uvolnime danu pamat
                if (previous == NULL) { // specificky pripad ak ide o jediny zaznam v celom zozname
                    printf("Zviera s menom %s bolo vymazane\n", buff);
                    (*size)--; // dekrement velkosti zoznamu
                    exit(1); // program sa bezpecne vypne
                }
                previous->next = NULL; // predchadzajuci pointer nema kam ukazovat, vynulujeme ho
                printf("Zviera s menom %s bolo vymazane\n", buff);
                (*size)--; // dekrement velkosti zoznamu
                break;
            }
            else { // ak to nebol posledny zaznam
                break;
            }
        }
        else if (current->next == NULL) {
            return first;
        }
        else { // ak sme nenasli zhodu
            previous = current; // predchadzajuci zaznam bude ukazovat na aktualny zaznam
            current = current->next; // a aktualnym sa posunieme na dalsi
        }
    }


    if(current == first) { // specificky pripad ak ide o prvy zaznam
        *first = (*first->next); // spravne zositie pointrov
        printf("Zviera s menom %s bolo vymazane\n", buff);
        (*size)--; // dekrement velkosti zoznamu
    }
    else if (stringCompare(current->name, buff) == 0) { // kontrola ci sa dane stringy rovnaju bez ohladu na velkost pismen
        previous->next = current->next; // spravne zositie pointrov
        free(current); // uvolnenie aktualneho zaznamu
        printf("Zviera s menom %s bolo vymazane\n", buff);
        (*size)--; // dekrement velkosti zoznamu
    }

    return first; // vratime hodnotu pointra pre prvy zaznam
}




ANIMAL *loadAnimal() // funkcia na nacitanie jedneho zvierata pre funkciu p
{
    char buff[MAX_LINE_LENGTH];
    // nastavenie buffera, aby obsahoval same nuly
    memset(buff, 0, MAX_LINE_LENGTH);

    ANIMAL *current = malloc(sizeof(ANIMAL)); // alokacia pamate pre jedno zviera

    // meno
    loadLine(buff); // nacitanie riadka + pridanie terminatora na koniec
    memset(current->name, 0, NAME_LENGTH); // nastavenie nul na dany segment pamate
    strncpy(current->name, buff, NAME_LENGTH-1); // nakopirovanie buffra do typu ANIMAL

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

    return current; // vratenie pointra na dane zviera
}


ANIMAL *p_beginning(ANIMAL* first)
{
    ANIMAL *current = loadAnimal(); // nacitanie zvierata
    current->next = first; // priradenie jeho pointra, ktory bude ukazovat na povodny prvy prvok
    return current; // vratenie noveho zaciatku zoznamu
}

ANIMAL *p_end(ANIMAL* first)
{
    ANIMAL* current, *last;
    current = loadAnimal(); // nacitanie zvierata
    current->next = NULL; // nastavenie jeho pointra, ktory nebude ukazovat nikam, kedze ide o posledny zaznam
    last = first; // predbezne nastavime posledny zaznam na prvy
    while (last->next!=NULL) // prejdeme cely zoznam az kym nedojdeme na koniec
    {
        last = last->next; // a posledny zaznam v povodnom zozname nastavime na predposledny
    }
    last->next = current; // a pointer predposledneho nastavime na novy posledny zaznam

    return first; // vratime hodnotu pointra pre prvy zaznam
}

ANIMAL *p_insert(ANIMAL* first, int position)
{
    ANIMAL* inserted;
    ANIMAL* current = first; // nastavime si aktualny zaznam na prvy
    int count_line = 1; // pocitadlo pre riadky
    while (current != NULL) { // cyklus ktorym prejdeme cely zoznam
        if (count_line == (position-1)) { // ak sme na danej pozicii, kam chceme vlozit novy zaznam
            inserted = loadAnimal(); // do inserted nacitame hodnoty pre nove zvieratko
            inserted->next = current->next; // pointer noveho bude ukazovat na pointer aktualneho
            current->next = inserted; // a pointer aktualneho bude ukazovat na novopridany zaznam
            break; // escape z cyklu
        }
        else { // ak na chcenej pozicii este nie sme
            current = current->next; // presunieme sa na dalsi zaznam
            count_line++;
        }
    }

    return first; // vratime hodnotu pointra pre prvy zaznam
}


ANIMAL *p(ANIMAL* first, int *size)
{
    int number = 0;
    int count = *size;

    if(!first) { // ak nemame vytvoreny zoznam
        fprintf(stderr, "Zoznam nie je vytvoreny.\n");
        return first;
    }

    scanf("%d", &number); // nacitanie miesta, na ktore budeme vkladat nove zviera
    if ((number <= 0)) { // ak je mensie/rovne 0, vratime sa do mainu
        return first;
    }
    else if (number == 1) { // ak je rovne 0,
        first = p_beginning(first); // dame ho na zaciatok
        count++; // ak pridanie prebehlo uspesne, velkost zoznamu sa nam zvysi o 1
    }
    else if (number > *size) { // ak je zadana pozicia vacsia ako dlzka zoznamu
        first = p_end(first); // dame ho na koniec
        count++; // ak pridanie prebehlo uspesne, velkost zoznamu sa nam zvysi o 1
    }
    else { // ak neboli splnene podmienky vyssie
        first = p_insert(first, number); // budeme ho davat na konkretnu poziciu niekam do stredu
        count++; // ak pridanie prebehlo uspesne, velkost zoznamu sa nam zvysi o 1
    }
    *size = count; // aktualizovanie velkosti zoznamu
    return first; // vratime hodnotu pointra pre prvy zaznam
}




int main() {
    char buff[2]; // buffer ktorym budeme ovladat vyber funkcii
    ANIMAL* first = NULL; // pointer typu ANIMAL, ktory bude ukazovat na prvy zaznam v zozname
    int size = 0; // premenna pre cykly (pocet zvieratok v txt)
    do { // do-while cyklus pre zabezpecenie behu programu
        fgets(buff, 2, stdin); // nacitanie hodnoty do buffra
        switch(buff[0]) { // vsimame si len prvy znak z buffra, zvysok sa ignoruje
            case 'n': // funkcia nacitaj
                first = n(first, &size);
                break;
            case 'v': // funkcia vypis
                if(first == NULL) {
                    break;
                }
                v(first);
                break;
            case 'p': // funkcia pridaj
                first = p(first, &size);
                break;
            case 'z': // funkcia zmaz
                first = z(first, &size);
                break;
            case 'h': // funkcia hladaj
                h(first);
                break;
            case 'a': // funkcia aktualizuj
                a(first);
                break;
            default:
                break;
        }
    } while(buff[0] != 'k'); // pri stlaceni k sa ukonci program

    deallocateAnimals(first); // dealokujeme vsetky zvierata z pamate

    return 0;
}
