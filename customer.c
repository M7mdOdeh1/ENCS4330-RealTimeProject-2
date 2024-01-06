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
    createSharedMemory(SHKEY_PRODUCT, shmptr_product, "customer.c");

    // Copy the all products struct from the shared memory segment
    ptrAllProducts = (struct AllProducts *) shmptr_product;

    // print the shared memory segment
    printf("Shared memory segment:\n");
    for (int i = 0; i < ptrAllProducts->numProducts; i++) {
        printf("Product %d:\n", ptrAllProducts->products[i].ID);
        printf("Name: %s\n", ptrAllProducts->products[i].Name.str);
        printf("On Shelves Amount: %d\n", ptrAllProducts->products[i].onShelvesAmount);
        printf("Storage Amount: %d\n", ptrAllProducts->products[i].StorageAmount);
        printf("\n");
    }

    

}