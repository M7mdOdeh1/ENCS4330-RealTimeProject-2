#ifndef LOCAL_H
#define LOCAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h> 
#include <wait.h>
#include <signal.h>

#define MAX_PRODUCTS 100
#define MAX_LINE_LENGTH 255
#define MAX_TEAMS 100
#define SHKEY_SHELF 2222 // key for shared memory for shelf products
#define SHKEY_STORAGE 3333  // key for shared memory for storage products


char* trim(char *str);
int randomInRange(int min_range, int max_range);
void readArgumentsFile(FILE *file);
void readItemsFile(FILE *file, int numProducts);
void readTeamsFile(FILE *file, int numShelvingTeams);

struct String {
    char str[MAX_LINE_LENGTH];
};


struct Product {
    int ID;
    struct String Name;
    int onShelvesAmount;
    int StorageAmount;
};


struct Team {
    int team_id;
    int num_employees;
};

struct sembuf acquire = {0, -1, SEM_UNDO}, 
              release = {0,  1, SEM_UNDO};

#endif
