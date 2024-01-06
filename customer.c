/*
customer.c
this file executes the customer process
*/

#include "local.h"

void printProductInfo(struct Product product);

char *shmptr_product, *shmptr_teamPids;
struct AllProducts *ptrAllProducts;
pid_t *teamPids;

int customer_id;
int productAmountThresh;

int main (int argc, char *argv[]) {

    srand((unsigned) getpid()); // seed for the random function with the ID of the current process
    
    if (argc != 4) {
        fprintf(stderr, "Usage: %s customer_id\n", *argv);
        exit(1);
    }

    customer_id = atoi(argv[1]);
    productAmountThresh = atoi(argv[2]); 
    int numShelvingTeams = atoi(argv[3]);   

    struct String src;
    sprintf(src.str, "customer.c -- customer %d", customer_id);

    // access the shared memory segment for pid of the teams
    shmptr_teamPids = (char *) malloc(sizeof(int) * numShelvingTeams);
    shmptr_teamPids = createSharedMemory(SHKEY_TEAM, sizeof(int) * numShelvingTeams, src.str);

    teamPids = (pid_t *) shmptr_teamPids;
    
    // access the shared memory segment for the all products struct
    shmptr_product = (char *) malloc(sizeof(struct AllProducts));
    shmptr_product = createSharedMemory(SHKEY_PRODUCT, sizeof(struct AllProducts), src.str);
    ptrAllProducts = (struct AllProducts *) shmptr_product;

    // print the shared memory segment
    //printSharedMemory(shmptr_product, src.str);

    // access the semaphore for the all products struct
    int semid_product = createSemaphore(SEMKEY_PRODUCT, 2, src.str);

    acquireSem(semid_product, 0, src.str);

    selectRandomProductsToBuy();

    releaseSem(semid_product, 0, src.str);
   


    

}

/*
 function to select radom products with random amount to buy
*/
void selectRandomProductsToBuy() {
    // create temp array of all products which is a copy of the shared memory segment
    struct AllProducts tempProducts;
    memcpy(&tempProducts, ptrAllProducts, sizeof(struct AllProducts));

    // select random number of products to buy
    int numProcutsToBuy = randomInRange(1, ptrAllProducts->numProducts);

    printf("customer %d will buy %d products\n", customer_id, numProcutsToBuy);
    fflush(stdout);

    // select the random product with random amount to buy
    for (int i = 0; i < numProcutsToBuy; i++) {
        // select the index of a product to buy
        int randomProductIndex = randomInRange(0, tempProducts.numProducts);

        //get the product ID of the selected product
        int productID = tempProducts.products[randomProductIndex].ID;
        
        // remove the product from the temp array (to not select it again)
        tempProducts.products[randomProductIndex] = tempProducts.products[tempProducts.numProducts - 1];
        tempProducts.numProducts--;

        // search for the product ID and select random amount to buy from the original shared memory segment
        for (int j = 0; j < ptrAllProducts->numProducts; j++) {
            if (ptrAllProducts->products[j].ID == productID) {
                // select the amount of the product to buy
                int amount = randomInRange(1, ptrAllProducts->products[j].onShelvesAmount);

                // update the amount of the product on shelves
                ptrAllProducts->products[j].onShelvesAmount -= amount;

                /* 
                 * if the amount of the product on shelves becomes less than the threshold
                 * signal a random shelf team to bring more of the product from storage
                */
                if (ptrAllProducts->products[j].onShelvesAmount < productAmountThresh) {
                    /*
                    TODO: signal a random shelf team to bring more of the product from storage
                    */
                   
                }

                printf("customer %d bought %d of product '%s'\n", customer_id, amount, ptrAllProducts->products[j].Name.str );
                fflush(stdout);

                // print the info of the product after buying
                printProductInfo(ptrAllProducts->products[j]);

                break;

            }
        }

        

    }




}


/* 
 * function to print the info of a product
*/
void printProductInfo(struct Product product) {
    printf("*****************************************************************************************\n");
    printf("Product %d:\t", product.ID);
    printf("Name: %s,\t", product.Name.str);
    printf("onShelvesAmount: %d,\t", product.onShelvesAmount);
    printf("StorageAmount: %d,\n", product.StorageAmount);
    printf("****************************************************************************************\n");
    printf("\n");
    fflush(stdout);
}