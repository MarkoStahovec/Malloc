#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // kniznice pre funkcie isdigit() a isupper()

#define MAX_LINE_LENGTH 64

/**
FIIT STU
Predmet: Proceduralne programovanie
Semester: ZS 2020/2021
Autor: Marko Stahovec

Uloha: Projekt 1
Funkcia programu: Polia
***/

void readLine(char* buff, FILE* file) { // vylepseny fgets
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

int loadPatient(char* name, char* number, char* diagnosis, char* examination, double* result, char* date, FILE* file) {
    char buff[MAX_LINE_LENGTH]; // buffer
    int retval = 0; // return value - hlavna pomocna premenna
    int dot = 0;
    int dec_counter = 0;

    memset(buff, 0, MAX_LINE_LENGTH); // budeme pouzivat memset na prepisovanie nul, aby sme predisli preteceniu
    readLine(buff, file);

    if(feof(file)) { // kontrola ci sme v subore na konci
        return 1;
    }

    if (strlen(buff) > 50 || strlen(buff) < 0) { // osetrenie, ze meno nebude dlhsie ako 50 znakov
        retval = 1;
        return retval;
    }
    memset(name, 0, 51);
    strncpy(name, buff, 50);

    readLine(buff, file);
    memset(number, 0, 11);
    if (strlen(buff) > 10 || strlen(buff) < 10) { // osetrenie, ze rodne cislo nebude dlhsie ako 10 znakov
        retval = 2;
    }
    strncpy(number, buff, 10);

    int sucet_cislic = 0; // pomocna premenna pre delitelnost 11
    for(int i = 0; number[i] != '\0'; ++i) { // osetrenie pre delitelnost 11
        if(!isdigit(number[i])) {
            retval = 2;
        }
        if(i % 2 == 0) { // pripocitavanie cislic na parnom mieste
            sucet_cislic += number[i] - '0';
        }
        else { // odcitavanie cislic na neparnom mieste
            sucet_cislic -= number[i] - '0';
        }
    }
    if(sucet_cislic % 11) {
        retval = 2;
    }

    if(number[4] > 51 || (number[4] == 51 && number[5] > 49)) { // kontrola pre dni
        retval = 2;
    }

    if(number[2] > 54 || (number[2] == 54 && number[3] > 50)) { // kontrola pre mesiace u zien
        retval = 2;
    }
    if((number[2] == 48 && number[3] == 48) || (number[2] == 53 && number[3] == 48)) { // kontrola pre pripady 00 a 50
        retval = 2;
    }
    if (number[2] < 53) { // kontrola pre mesiace u muzov
        if(number[2] < 49 || (number[2] == 49 && number[3] < 51)) {    }
        else {
            retval = 2;
        }
    }

    readLine(buff, file);
    memset(diagnosis, 0, 4);
    strncpy(diagnosis, buff, 3);
    if(!isupper(diagnosis[0]) || strlen(buff) < 0) { // osetrenie, ze prvy znak bude velke pismeno
        retval = 3;
    }
    if(!isdigit(diagnosis[1]) || !isdigit(diagnosis[2])) { // osetrenie, ze dalsie dva znaky budu cisla
        retval = 3;
    }

    readLine(buff, file);
    if (strlen(buff) > 50 || strlen(buff) < 0) { // osetrenie, ze vysetrenie nebude dlhsie ako 50 znakov
        retval = 4;
        return retval;
    }
    memset(examination, 0, 51);
    strncpy(examination, buff, 50);

    readLine(buff, file);
    for (int i = 0; i < strlen(buff); i++) {
        if (dot == 1) {
            dec_counter++;
        }
        if(buff[i] == '.') {
            dot = 1;
        }
    }
    if (dec_counter > 4) {
        retval = 4;
    }
    dec_counter = dot = 0;
    *result = strtod(buff, NULL); // premena stringu na double
    if((*result > 1000.0) || *result < 0.0 || strlen(buff) < 0) { // kontrola pre interval vysledku
        retval = 5;
    }

    readLine(buff, file);
    memset(date, 0, 9);
    strncpy(date, buff, 8);
    if(!isdigit(date[0]) || !isdigit(date[1]) || !isdigit(date[2]) || !isdigit(date[3])
       || !isdigit(date[4]) || !isdigit(date[5])
       || !isdigit(date[6]) || !isdigit(date[7])
       || strlen(buff) > 8 || strlen(buff) < 0) { // kontrola pre cislice a dlzku datumu
        retval = 6;
    }
    if(date[4] > 49 || (date[4] == 49 && date[5] > 50)) { // kontrola pre mesiace
        retval = 6;
    }
    if(date[6] > 51 || (date[6] == 51 && date[7] > 49)) { // kontrola pre dni
        retval = 6;
    }

    fgets(buff, MAX_LINE_LENGTH, file); // preskocenie prazdneho riadka

    return retval;
}

void printPatient(char* name, char* number, char* diagnosis, char* examination, double result, char* date) {
    printf("meno priezivsko: %s\n", name);
    printf("rodne cislo: %s\n", number);
    printf("diagnoza: %s\n", diagnosis);
    printf("vysetrenie: %s\n", examination);
    printf("vysledok: %g\n", result);
    printf("datum: %s\n\n", date);
}

FILE* v() {
    FILE* file = fopen("pacienti.txt", "r"); // otvorenie suboru na citanie
    if(!file) { // kontrola, ci sa subor spravne otvoril
        fprintf(stderr, "Neotvoreny subor\n");
        return NULL;
    }

    while(!feof(file)) { // cyklus ktory bezi, kym nedojdeme na koniec suboru
        char name[51];
        char number[11];
        char diagnosis[4];
        char examination[51];
        double result;
        char date[9];
        int retval;
        if((retval = loadPatient(name, number, diagnosis, examination, &result, date, file)) == 0) {
            // podmienka, ktora nastartuje cyklus a zaroven hlada chybne udaje
            printPatient(name, number, diagnosis, examination, result, date);
        }

        else if(retval == 1) {
            printf("Meno je prilis dlhe\n\n");
            exit(1);
        }
        else if(retval == 2) {
            printf("Neplatne rodne cislo\n\n");
            exit(1);
        }
        else if(retval == 3) {
            printf("Neplatna diagnoza\n\n");
            exit(1);
        }
        else if(retval == 4) {
            printf("Neplatne vysetrenie\n\n");
            exit(1);
        }
        else if(retval == 5) {
            printf("Neplatny vysledok\n\n");
            exit(1);
        }
        else if(retval == 6) {
            printf("Neplatny datum\n\n");
            exit(1);
        }
    }

    return file;
}

void o(char** diagnosis, char** date, int n) {
    char buff[MAX_LINE_LENGTH];
    if(!diagnosis) {
        fprintf(stderr, "Polia nie su vytvorene.\n");
        return;
    }
    scanf("%s", buff);
    if(strlen(buff) != 8) {
        //fprintf(stderr, "Neplatny datum.\n");
        return;
    }
    else {
        int count[26][100];
        for(int i = 0; i < 26; ++i) {
            for(int j = 0; j < 100; ++j) {
                count[i][j] = 0;
            }
        }
        // cyklom prejdeme vsetkymi moznymi kombinaciami
        // zvysujeme pocet o 1 na mieste, ktore zodpoveda danej kombinacii pismena a dvojcislia
        for(int i = 0; i < n; ++i) {
            if(strcmp(buff, date[i]) > 0) {
                count[diagnosis[i][0] - 'A']
                [(diagnosis[i][1] - '0') * 10 + diagnosis[i][2] - '0']++;
            }
        }
        int max = 0;
        int max_i = 0;
        int max_j = 0;
        for(int i = 0; i < 26; ++i) {
            for(int j = 0; j < 100; ++j) {
                if(count[i][j] > max) {
                    max = count[i][j];
                    max_i = i;
                    max_j = j;
                }
            }
        }
        printf(" Najcastejsie vysetrovana diagnoza do %s je %c",buff, max_i +  'A');
        if(max_j < 10) { // pre spravne formatovanie vypisu cisel
            printf("0");
        }
        printf("%d\n", max_j);
    }
}

void n(FILE* file, char*** name, char*** number, char*** diagnosis, char*** examination, double** result, char*** date, int* n) {
    if(*name) { // uvolnenie vsetkych poli, ak uz boli predtym alokovane
        free(*name);
        free(*number);
        free(*diagnosis);
        free(*examination);
        free(*result);
        free(*date);
        *name = NULL;
        *number = NULL;
        *diagnosis = NULL;
        *examination = NULL;
        *result = NULL;
        *date = NULL;
    }
    if(file) { // ak je subor otvoreny...
        fseek(file, 0, SEEK_SET); // nastavenie kurzora na zaciatok suboru
        int size = 2; // pociatocna velkost pre polia
        int count = 0; // pocitadlo pre riadky
        *name = malloc(sizeof(char*) * size);
        *number = malloc(sizeof(char*) * size);
        *diagnosis = malloc(sizeof(char*) * size);
        *examination = malloc(sizeof(char*) * size);
        *result = malloc(sizeof(double) * size);
        *date = malloc(sizeof(char*) * size);
        if(!(*name) || !(*number) || !(*diagnosis) || !(*examination) || !(*result) || !(*date)) { // kontrola ci su vytvorene polia
            fprintf(stderr, "Polia nie su vytvorene.\n");
            exit(1);
        }
        while(!feof(file)) { // kym nedojdeme na koniec suboru
            if(count == size) { // ak sa cislo riadka rovna pociatocnej velkost
                size *= 2; // zvysime velkost poli
                *name = realloc(*name, sizeof(char*) * size);
                *number = realloc(*number, sizeof(char*) * size);
                *diagnosis = realloc(*diagnosis, sizeof(char*) * size);
                *examination = realloc(*examination, sizeof(char*) * size);
                *result = realloc(*result, sizeof(double) * size);
                *date = realloc(*date, sizeof(char*) * size);
            }
            char name_[51];
            char number_[11];
            char diagnosis_[4];
            char examination_[51];
            double result_;
            char date_[9];
            if((loadPatient(name_, number_, diagnosis_, examination_, &result_, date_, file)) == 0) {
                (*name)[count] = _strdup(name_); // v danom cykle duplikujeme
                (*number)[count] = _strdup(number_);
                (*diagnosis)[count] = _strdup(diagnosis_);
                (*examination)[count] = _strdup(examination_);
                (*result)[count] = result_;
                (*date)[count] = _strdup(date_);
                count++;
            }
        }
        *name = realloc(*name, sizeof(char*) * count); // finalna alokacia pre presny pocet riadkov
        *number = realloc(*number, sizeof(char*) * count);
        *diagnosis = realloc(*diagnosis, sizeof(char*) * count);
        *examination = realloc(*examination, sizeof(char*) * count);
        *result = realloc(*result, sizeof(double) * count);
        *date = realloc(*date, sizeof(char*) * count);
        *n = count;
        if(file) { // zatvorenie suboru
            fclose(file);
        }
    }
}

void s(char** examination, double* result, char** number, int n) {
    char buff[MAX_LINE_LENGTH]; // buffer do ktoreho nacitame string
    if(!examination) { // kontrola ci su vytvorene polia
        fprintf(stderr, "Polia nie su vytvorene.\n");
        return;
    }
    scanf("%s", buff); // nacitanie do buffra
    if(strlen(buff) != 10) { // kontrola pre spravny vstup
        //fprintf(stderr, "Nespravne rodne cislo.\n");
        return;
    }
    else { // ak bol vstup spravny
        for(int i = 0; i < n; ++i) { // cyklus ktory prejde kazdym zaznamom
            if(strcmp(buff, number[i]) == 0) { // ak sa rc z buffra a rc zo suboru rovnaju
                printf("Vysledky vysetreni, ktore boli vykonane pacientovi s rodnym"
                       "cislom %s su nasledovne:\n%s: %g\n", number[i], examination[i], result[i]);
            }
        }
    }
}

void h(char** diagnosis, char** number, int n) {
    /*
     * Vek odcitavam z rodneho cisla a fixneho datumu,
     * ktory som si urcil na 1.11.2020
     */
    char buff[MAX_LINE_LENGTH];
    if(!diagnosis) {
        fprintf(stderr, "Polia nie su vytvorene.\n");
        return;
    }
    scanf("%s", buff);
    if (strlen(buff) != 3 || !isupper(buff[0]) || !isdigit(buff[1]) || !isdigit(buff[2])) {
        return;
    }

    int count[100][2];
    memset(count, 0, 100 * 2 * sizeof(int));
    for(int i = 0; i < n; ++i) {
        if(strcmp(buff, diagnosis[i]) == 0) {
            int age = (number[i][0] - '0') * 10 + number[i][1] - '0'; // vypocet pre vek cez ascii tabulku
            int gender;
            if(age <= 20) { // kedze sa v rodnom cisla pouziva len posledne dvojcislie
                age = 20 - age;
            }
            else { // tak potrebujeme odpocitavat + jedno storocie
                age = 120 - age;
            }

            if(number[i][2] >= '5') { // podmienka pre zistenie pohlavia z rodneho cisla
                gender = 1; // 1 - zena
            }
            else {
                gender = 0; // 0 - muz
            }

            if (gender == 1) { // podmienka pre odpocet 1 roku ak je mesiac narodenia skorsi ako november
                if (number[i][2] == 54 && number[i][3] > 48) {
                    age--;
                }
            }
            else if (gender == 0) {
                if (number[i][2] == 49 && number[i][3] > 48) {
                    age--;
                }
            }

            count[age][gender]++; // pripocitanie daneho pacienta
        }
    }

    printf("Muzi\n"); // cyklus pre vypis muzov
    for(int i = 0; i < 100; ++i) {
        if(count[i][0] > 0) {
            printf("%d: %d\n", i, count[i][0]);
        }
    }
    printf("\nZeny\n"); // cyklus pre vypis zien
    for(int i = 0; i < 100; ++i) {
        if(count[i][1] > 0) {
            printf("%d: %d\n", i, count[i][1]);
        }
    }
}

void p(char** name, char** number, char** diagnosis, char** examination, double* result, char** date, int n) {
    if(!name) { // kontrola ci su polia vytvorene
        fprintf(stderr, "Polia nie su vytvorene.\n");
        return;
    }

    FILE* file = fopen("pacienti.txt", "w"); // otvorenie filu vo w mode

    if(!file) { // ak sa subor nepodarilo otvorit
        fprintf(stderr, "Neotvoreny subor\n");
        return;
    }

    char number_[MAX_LINE_LENGTH]; // buffre, ktore budeme porovnvavat so zaznamami v subore
    char examination_[MAX_LINE_LENGTH];
    char date_[MAX_LINE_LENGTH];


    scanf("%s", number_);
    if(strlen(number_) != 10) {
        //fprintf(stderr, "Nespravne rodne cislo.\n");
        return;
    }
    scanf("%s", examination_);
    scanf("%s", date_);
    if(strlen(date_) != 8) {
        //fprintf(stderr, "Nespravny datum.\n");
        return;
    }

    double result_old;
    double result_;
    scanf("%lf", &result_);

    for(int i = 0; i < n; ++i) {
        if(strcmp(number[i], number_) == 0 // ak sa zadanie stringy rovnaju s tymi v subore
           && strcmp(examination[i], examination_) == 0
           && strcmp(date[i], date_) == 0) {
            result_old = result[i];
            result[i] = result_; // nastane prepis v riadku s vysledkom
            printf("Pacientovi s rodnym cislo %s bol zmeneny vysledok vysetrenia "
                   "%s z povodnej hodnoty %g na novu hodnotu %g.\n", number[i],
                   examination[i], result_old, result_);
        }
    }


    for(int i = 0; i < n; ++i) {
        if(i == n-1) {
            fprintf(file, "%s\n%s\n%s\n%s\n%g\n%s\n",
                    name[i], number[i],
                    diagnosis[i], examination[i],
                    result[i], date[i]);
        }
        else {
            fprintf(file, "%s\n%s\n%s\n%s\n%g\n%s\n\n",
                    name[i], number[i],
                    diagnosis[i], examination[i],
                    result[i], date[i]);
        }
    }

    if(file) {
        fclose(file);
    }
}

void z(char** examination, char** date, double* result, char** name, int n) {
    char buff[3][MAX_LINE_LENGTH];
    if(!examination) {
        fprintf(stderr, "Polia nie su vytvorene.\n");
        return;
    }
    int winner[] = {-1, -1, -1}; // array, v ktorom budu indexy danych pacientov
    double value[] = {0., 0., 0.}; // array v ktorom budu ulozene ich vysledky pre porovnanie
    scanf("%s", buff[0]); // buffer pre prvy datum
    scanf("%s", buff[1]); // buffer pre druhy datum
    scanf("%s", buff[2]); // buffer pre vysetrenie
    if(strlen(buff[0]) != 8 || strlen(buff[1]) != 8) {
        //fprintf(stderr, "Neplatny datum.\n");
        return;
    }
    if((strcmp(buff[1], buff[0]) < 0 )) {
        fprintf(stderr, "Koncovy datum sa nachadza pred pociatocnym datumom.\n");
        return;
    }
    else {
        for(int i = 0; i < n; ++i) {
            if(strcmp(date[i], buff[0]) > 0 // ak je jeho datum vysetrenie z daneho intervalu
               && strcmp(date[i], buff[1]) < 0
               && strcmp(buff[2], examination[i]) == 0) { // a jeho vysetrenie sedi so zadanym vysetrenim
                if(result[i] > value[0]) { // porovnavame velkost jeho vysledku s prvou hodnotu
                    value[2] = value[1]; // ak plati, vsetky hodnoty sa shiftnu o 1 miesto vpravo
                    winner[2] = winner[1];
                    value[1] = value[0];
                    winner[1] = winner[0];
                    value[0] = result[i]; // a jeho vysledok bude v arrayi value na prvom mieste
                    winner[0] = i; // a jeho index bude v arrayi winner taktiez na prvom mieste
                }
                else if(result[i] > value[1]) {
                    value[2] = value[1];
                    winner[2] = winner[1];
                    value[1] = result[i];
                    winner[1] = i;
                }
                else if(result[i] > value[2]) {
                    value[2] = result[i];
                    winner[2] = i;
                }
            }
        }
        if (winner[0] > -1 && winner[1] > -1 && winner[2] > -1) { // rozne vypisy pre roznych pocet pacientov
            printf("Traja pacienti s najvyssimi hodnotami %s za obdobie %s - %s su (namerana "
                   "hodnota v zatvorke):\n", buff[2], buff[0], buff[1]);
        }
        else if (winner[0] > -1 && winner[1] > -1 && winner[2] == -1) {
            printf("Dvaja pacienti s najvyssimi hodnotami %s za obdobie %s - %s su (namerana "
                   "hodnota v zatvorke):\n", buff[2], buff[0], buff[1]);
        }
        else if (winner[0] > -1 && winner[1] == -1 && winner[2] == -1) {
            printf("Jeden pacient s najvyssou hodnotou %s za obdobie %s - %s je (namerana "
                   "hodnota v zatvorke):\n", buff[2], buff[0], buff[1]);
        }

        for(int i = 0; i < 3; ++i) {
            if(winner[i] != -1) {
                printf("%s (%g)\n", name[winner[i]], result[winner[i]]);
            }
        }
    }
}

int main() {
    char buff[2];
    FILE* file = NULL;
    int size = 0;
    char** name = NULL;
    char** number = NULL;
    char** diagnosis = NULL;
    char** examination = NULL;
    double* result = NULL;
    char** date = NULL;
    int fileopen = 0;
    do {
        fgets(buff, 2, stdin);
        switch(buff[0]) {
            case 'v':
                file = v();
                if(file) {
                    fileopen = 1;
                }
                break;
            case 'o':
                o(diagnosis, date, size);
                break;
            case 'n':
                n(file, &name, &number, &diagnosis, &examination, &result, &date, &size);
                if(fileopen) {
                    fileopen = 0;
                }
                break;
            case 's':
                s(examination, result, number, size);
                break;
            case 'h':
                h(diagnosis, number, size);
                break;
            case 'p':
                p(name, number, diagnosis, examination, result, date, size);
                break;
            case 'z':
                z(examination, date, result, name, size);
                break;
            default:
                break;
        }
    }
    while(buff[0] != 'k');

    if(fileopen) {
        fclose(file);
    }

    if(name) {
        for(int i = 0; i < size; ++i) {
            free(name[i]);
            free(number[i]);
            free(diagnosis[i]);
            free(examination[i]);
            free(date[i]);
        }
        free(name);
        free(number);
        free(diagnosis);
        free(examination);
        free(result);
        free(date);
        name = NULL;
        number = NULL;
        diagnosis = NULL;
        examination = NULL;
        result = NULL;
        date = NULL;
    }

    return 0;
}

