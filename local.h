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

#define MAX_CUSTOMERS 500
#define MAX_TEAMS 100

#define MAX_PRODUCTS 100
#define MAX_LINE_LENGTH 255
#define SHKEY_TEAM 1111 // key for shared memory for all teams
#define SHKEY_PRODUCT 2222 // key for shared memory for all products
#define SEMKEY_PRODUCT 3333 // key for semaphore for all products



char* trim(char *str);
int randomInRange(int min_range, int max_range);
void readArgumentsFile(char *arguments_filename);
void readProductsFile(char *items_filename, int numProducts);
void readTeamsFile(char *teams_filename, int numShelvingTeams);
char* createSharedMemory(int key, int size, char *src);
int createSemaphore(int key, int num, char *src);
void printSharedMemory(char *shmptr, char *src);
void deleteSemaphore(int semid);
void deleteSharedMemory(int key, char *shmptr);
void killChildProcesses(int pids[], int numPids);
void exitProgram(int signum);
void acquireSem(int semid, int semnum, char *src);
void releaseSem(int semid, int semnum, char *src);
void selectRandomProductsToBuy();


struct String {
    char str[MAX_LINE_LENGTH];
};


struct Product {
    int ID;
    struct String Name;
    int onShelvesAmount;
    int StorageAmount;
};

typedef struct Product Product;

// used in the shared memory segment
struct AllProducts {
    Product products[MAX_PRODUCTS];
    int numProducts;
    
};

struct Team {
    int team_id;
    int num_employees;
};

typedef struct Team Team;
// used in the shared memory segment
struct AllTeams {
    Team teams[MAX_TEAMS];
    int numTeams;
};

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array; 
};


#endif
