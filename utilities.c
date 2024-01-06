/*
utilities.c
this file contains the functions that are used by the main program 
to preform various tasks
*/

#include "local.h"





// trim function
// this function removes the white spaces from the beginning and the end of a string
char* trim(char *str) {
    while (*str && (*str == ' ' || *str == '\t' || *str == '\n')) {
        str++;
    }
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\n')) {
        len--;
    }
    str[len] = '\0';

    return str;
}

// randomInRange function
// this function returns a random number between the min_range and the max_range
int randomInRange(int min_range, int max_range) {
    if (min_range == max_range) {
        return min_range;
    }
    if (min_range > max_range) {
        int temp = min_range;
        min_range = max_range;
        max_range = temp;
    }

    return (int) (min_range +  (rand() % (max_range - min_range)));
}

