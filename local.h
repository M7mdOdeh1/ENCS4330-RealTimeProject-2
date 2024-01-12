#ifndef LOCAL_H
#define LOCAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAX_CUSTOMERS 200
#define MAX_TEAMS 10
#define MAX_PRODUCTS 20
#define MAX_LINE_LENGTH 255
#define SHKEY_TEAM 1111 // key for shared memory for pid of the teams
#define SHKEY_PRODUCT 2222 // key for shared memory for all products
#define SEMKEY_PRODUCT 3333 // key for semaphore for all products
#define MSGQKEY_TEAM 1234 // key for message queue for the teams


char* trim(char *str);
int randomInRange(int min_range, int max_range);
void readArgumentsFile(char *arguments_filename);
void readProductsFile(char *items_filename, int numProducts);
void readTeamsFile(char *teams_filename, int numShelvingTeams);
void exitProgram(int signum);
void selectRandomProductsToBuy();
void deleteProduct(int index);
void employee(int *employee_id);
int bringProductFromStorage(int productIndex);
void initializeIPCResources();


struct String {
    char str[MAX_LINE_LENGTH];
};


struct Product {
    int ID;
    struct String Name;
    int shelfCapacity;
    int onShelvesAmount;
    int storageAmount;
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




#endif
