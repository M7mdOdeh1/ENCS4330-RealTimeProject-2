#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 100
#define MAX_LINE_LENGTH 255


char* trim(char *str);
int randomInRange(int min_range, int max_range)

struct String {
    char str[MAX_LINE_LENGTH];
};


struct Product {
    int ID;
    char Name[50];
    int onShelvesAmount;
    int StorageAmount;
};
