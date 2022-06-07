#define _CRT_SECURE_NO_WARNINGS // prikaz na ignoraciu errorov spojenych s funkciou scanf
#include <stdio.h> // odtialto si program berie funkciu printf/scanf
#include <stdlib.h>
#include <string.h>
#include "custom_malloc.h"

/**
FIIT STU
Predmet: Proceduralne programovanie
Semester: ZS 2020/2021
Autor: Marko Stahovec

Zadanie: 1
Funkcia programu: Vlastna verzia alokacie pamate
***/

void *heap_start = NULL;

void printArray(unsigned char *a,int array_length){
    int i;
    for(i = 0; i < array_length; i++){
        if (i % 8 == 0) {
            printf("\n0x%x: ", &(a[i]));
        }
        if (a[i] <= 15)
            printf("     0%X", a[i]);
        else {
            printf("  %5X", a[i]);
        }
    }
    printf("\n");
}

void *find_fit(unsigned int size) {
    unsigned int *header = (unsigned int*)(heap_start) + 1;
    //unsigned int *end_heap = ((unsigned int*)((unsigned char*)heap_start + sizeof(header) + *(header)));
    unsigned int *end_heap = ((unsigned int*)((unsigned char*)heap_start + (*(unsigned char*)heap_start)));

    while (header < end_heap) {
        /*
        if (((unsigned char *)(header + (size/sizeof(unsigned int))) > (unsigned char *)end_heap)) {
            if (!IS_ODD(*header)) {
                //errorMessage(MEMORY_FULL);
                return NULL;
            }
        }
         */
        if (!(IS_ODD(*header)) && *header >= size)
            return header;
        header = (char *)header + (*header & ~1L);
    }
    //errorMessage(MEMORY_FULL);
    return NULL;
}


void *memory_alloc(unsigned int size)
{
    if(!heap_start) { // podmienka ak pamat nebola inicializovana
        //errorMessage(NO_INIT);
        return NULL;
    }
    if (size <= 0) { // podmienka pre zapornu/nulovu velkost na alokaciu
        //errorMessage(INVALID_SIZE);
        return NULL;
    }


    unsigned int* header;
    unsigned int block_size = ALIGN(size + sizeof(header) + FOOTER_SIZE);

    header = find_fit(block_size);

    if (!header) {
        //errorMessage(MALLOC_ERROR);
        return NULL;
    }
    else {
        unsigned old_size = *header;
        *header = block_size;
        unsigned int *footer = header + ((unsigned char)(block_size)/sizeof(unsigned int)-1);
        *footer = *header;

        if ((old_size-block_size) == 0) {
            *header = *header | 1;
            *footer = *footer | 1;

            return (char *)header + sizeof(header);
        }
        else {
            unsigned int *new_header = footer + 1;
            *new_header = old_size - block_size;
            unsigned int *new_footer = new_header + ((unsigned char)*new_header/sizeof(unsigned int)-1);
            *new_footer = *new_header;

            *header = *header | 1;
            *footer = *footer | 1;

            return (char *)header + sizeof(header);
        }
    }
}


void *coalesce(unsigned int *current_block) {
    unsigned int *heap_end = ((unsigned int *)((unsigned char*)heap_start + (*(unsigned char*)heap_start)));

    unsigned int *next = (unsigned int *)((unsigned char *)current_block + ((*(unsigned char *)current_block) & ~1L));
    unsigned int *prev = (unsigned int *)((unsigned char *)current_block - (*((unsigned char *)current_block-FOOTER_SIZE) & ~1L));

    unsigned int is_next_alloc;
    unsigned int is_prev_alloc;

    if (current_block == ((unsigned char*)heap_start + HEADER_SIZE)) {
        is_next_alloc = *next & 1;
        is_prev_alloc = 1;
        if (next >= heap_end) {
            is_next_alloc = 1;
        }
    }
    else if (next >= heap_end) {
        is_next_alloc = 1;
        is_prev_alloc = *next & 1;
    }
    else {
        is_next_alloc = *next & 1;
        is_prev_alloc = *prev & 1;
    }

    //(*current_block)--;
    //printf("%d", *current_block);


    if (is_prev_alloc == TRUE && is_next_alloc == TRUE) {
        *current_block = (*current_block) & ~1L;
        *(unsigned int *)((unsigned char *)current_block + *(unsigned char *)current_block - FOOTER_SIZE) = *current_block;
        memset(current_block+1, 0, (unsigned char )(*(unsigned char *)current_block-FOOTER_SIZE-HEADER_SIZE));

        return current_block;
    }
    else if (is_prev_alloc == FALSE && is_next_alloc == TRUE) {
        *prev = (*current_block + *prev) & ~1L; // new header
        *(unsigned int *)((unsigned char *)prev + *(unsigned char *)prev - FOOTER_SIZE) = *prev; // new footer
        memset(prev+1, 0, (unsigned char )(*(unsigned char *)prev-FOOTER_SIZE-HEADER_SIZE));

        return prev;
    }
    else if (is_prev_alloc == TRUE && is_next_alloc == FALSE) {
        *current_block = (*current_block + *next) & ~1L; // new header
        *(unsigned int *)((unsigned char *)current_block + *(unsigned char *)current_block - FOOTER_SIZE) = *current_block; // new footer
        memset(current_block+1, 0, (unsigned char )(*(unsigned char *)current_block-FOOTER_SIZE-HEADER_SIZE));

        return current_block;
    }
    else if (is_prev_alloc == FALSE && is_next_alloc == FALSE){
        *prev = (*prev + *current_block + *next) & ~1L; // new header
        *(unsigned int *)((unsigned char *)prev + *(unsigned char *)prev - FOOTER_SIZE) = *prev; // new footer
        memset(prev+1, 0, (unsigned char )(*(unsigned char *)prev-FOOTER_SIZE-HEADER_SIZE));

        return prev;
    }
}


int memory_free(void *valid_ptr)
{
    //valid_ptr = (unsigned char*)(valid_ptr) - HEADER_SIZE;

    if (heap_start == NULL) {
        //errorMessage(NO_INIT);
        return 1;
    }
    if ((memory_check(valid_ptr) == FALSE)) {
        //errorMessage(INVALID_POINTER);
        return 1;
    }

    coalesce((unsigned char*)(valid_ptr) - HEADER_SIZE);

    return 0;
}

int memory_check(void *ptr)
{
    ptr = (unsigned char *)(ptr) - HEADER_SIZE;

    if ((ptr == NULL) || (ptr < heap_start)) {
        return 0;
    }

    unsigned int *header = (unsigned int*)(heap_start) + 1;
    unsigned int *end_heap = ((unsigned int*)((unsigned char*)heap_start + (*(unsigned char*)heap_start)));


    while (header < end_heap) {
        if ((ptr == header) && (IS_ODD(*header))) {
            return 1;
        }
        header = (char *) header + ((*header & ~1L));
    }

    //errorMessage(INVALID_POINTER);
    return 0;
}

void memory_init(void *ptr, unsigned int size)
{
    if (!ptr) {
        //errorMessage(INVALID_POINTER);
        return;
    }
    if (size <= 0) {
        //errorMessage(INVALID_SIZE);
        return;
    }

    heap_start = ptr;
    size = ALIGN(size);
    *(unsigned int*)heap_start = size;

    unsigned int *header = (unsigned int *)((unsigned char*)heap_start + sizeof(heap_start));
    *header = size - sizeof(heap_start);

    unsigned int *footer = (unsigned int *)((unsigned char*)heap_start + size - FOOTER_SIZE);
    *footer = *header;
}


void z1_testovac(char *region, char **pointer, int minBlock, int maxBlock, int minMemory, int maxMemory, int testFragDefrag) {
    unsigned int allocated = 0;
    unsigned int mallocated = 0;
    unsigned int allocated_count = 0;
    unsigned int mallocated_count = 0;
    unsigned int i = 0;
    int random_memory = 0;
    int random = 0;
    memset(region, 0, 100000);
    random_memory = (rand() % (maxMemory-minMemory+1)) + minMemory;
    memory_init(region + 500, random_memory);
    if (testFragDefrag) {
        do {
            pointer[i] = memory_alloc(8);
            if (pointer[i])
                i++;
        } while (pointer[i]);
        for (int j = 0; j < i; j++) {
            if (memory_check(pointer[j])) {
                memory_free(pointer[j]);
            }
            else {
                printf("Error: Wrong memory check.\n");
            }
        }
    }
    i = 0;
    while (allocated <= random_memory-minBlock) {
        random = (rand() % (maxBlock-minBlock+1)) + minBlock;
        if (allocated + random > random_memory)
            continue;
        allocated += random;
        allocated_count++;
        pointer[i] = memory_alloc(random);
        if (pointer[i]) {
            i++;
            mallocated_count++;
            mallocated += random;
        }
    }
    for (int j = 0; j < i; j++) {
        if (memory_check(pointer[j])) {
            memory_free(pointer[j]);
        }
        else {
            printf("Error: Wrong memory check.\n");
        }
    }
    memset(region + 500, 0, random_memory);
    for (int j = 0; j < 100000; j++) {
        if (region[j] != 0) {
            region[j] = 0;
            printf("Error: Modified memory outside the managed region. index: %d\n",j-500);
        }
    }
    float result = ((float)mallocated_count / allocated_count) * 100;
    float result_bytes = ((float)mallocated / allocated) * 100;
    printf("Memory size of %d bytes: allocated %.2f%% blocks (%.2f%% bytes).\n", random_memory, result, result_bytes);
}


int main(void)
{

#define REGION_SIZE 104
/*
    char region[REGION_SIZE];
    memset(region, 0, REGION_SIZE);
    memory_init(region, REGION_SIZE);
    printArray(region,REGION_SIZE);

    char* pointer = (char*) memory_alloc(10);
    printArray(region,REGION_SIZE);
    char* pointer_2 = (char*) memory_alloc(10);
    printArray(region,REGION_SIZE);
    char* pointer_3 = (char*) memory_alloc(10);
    printArray(region,REGION_SIZE);
    char* pointer_4 = (char*) memory_alloc(10);
    printArray(region,REGION_SIZE);
    char* pointer_5 = (char*) memory_alloc(8);
    printArray(region,REGION_SIZE);
    char* pointer_6 = (char*) memory_alloc(8);
    printArray(region,REGION_SIZE);
    memory_free(pointer_2);
    printf("\nafter first free\n");
    printArray(region,REGION_SIZE);

    memory_free(pointer_4);
    printf("\nafter second free\n");
    printArray(region,REGION_SIZE);

    memory_free(pointer_3);
    printf("\nafter third free\n");
    printArray(region,REGION_SIZE);

    memory_free(pointer);
    printf("\nafter fourth free\n");
    printArray(region,REGION_SIZE);

    char* pointer_7 = (char*) memory_alloc(68);
    printArray(region,REGION_SIZE);

    memory_free(pointer_7);
    printf("\nafter fifth free\n");
    printArray(region,REGION_SIZE);

    memory_free(pointer_5);
    printf("\nafter final free\n");
    printArray(region,REGION_SIZE);
*/



    char region[100000];
    //memory_init(region, 100000);
    char* pointer[13000];
    z1_testovac(region, pointer, 8, 24, 50, 100, 1);
    z1_testovac(region, pointer, 8, 1000, 10000, 20000, 0);
    z1_testovac(region, pointer, 8, 35000, 50000, 99000, 0);
    return 0;


    return 0;
}
