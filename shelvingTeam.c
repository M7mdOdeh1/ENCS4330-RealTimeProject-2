/*
shelvingteam.c
this file executes the shelving team process
*/

#include "local.h"

int main (int argc, char *argv[]) {
    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s team_id\n", *argv);
        exit(1);
    }

    int team_id = atoi(argv[1]);
    int num_employees = atoi(argv[2]);

    printf("Team %d: %d employees\n", team_id, num_employees);
    fflush(stdout);

}
