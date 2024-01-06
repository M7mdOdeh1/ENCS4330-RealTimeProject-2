/*
customer.c
this file executes the customer process
*/

#include "local.h"

char *shmptr_product;
struct AllProducts *ptrAllProducts;

int main (int argc, char *argv[]) {

    
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s customer_id\n", *argv);
        exit(1);
    }

    int customer_id = atoi(argv[1]);

    printf("Customer %d\n", customer_id);
    fflush(stdout);

    // access the shared memory segment for the all products struct
    shmptr_product = (char *) malloc(sizeof(struct AllProducts));
    shmptr_product = createSharedMemory(SHKEY_PRODUCT, "customer.c");
    ptrAllProducts = (struct AllProducts *) shmptr_product;

    struct String src;
    sprintf(src.str, "customer.c -- customer %d", customer_id);

    // print the shared memory segment
    printSharedMemory(shmptr_product, src.str);

    // access the semaphore for the all products struct
    int semid_product = semget(SEMKEY_PRODUCT, ptrAllProducts->numProducts, 0666);

    


    

}