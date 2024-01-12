/*
customer.c
this file executes the customer process
*/

#include "local.h"
#include "ipcs.h"
#include "gui.h" 

void printProductInfo(struct Product product);

char *shmptr_product, *shmptr_teamPids;
struct AllProducts *ptrAllProducts;
int msgqid_team, msgqid_gui;
int semid_product;

int customer_id,
    productAmountThresh,
    numShelvingTeams;
struct String src;

PositionUpdateMessage *positionUpdateMsg;
ProductRequestMessage requestMsg;

int main (int argc, char *argv[]) {

    srand((unsigned) getpid()); // seed for the random function with the ID of the current process
    
    if (argc != 4) {
        fprintf(stderr, "Usage: %s customer_id\n", *argv);
        exit(1);
    }

    customer_id = atoi(argv[1]);
    productAmountThresh = atoi(argv[2]); 
    numShelvingTeams = atoi(argv[3]);   

    sprintf(src.str, "customer.c -- customer %d", customer_id);

    // access the msg queue for the teams
    msgqid_team = createMessageQueue(MSGQKEY_TEAM, src.str);

    // access the msg queue for the gui
    msgqid_gui = createMessageQueue(MSGQKEY_GUI, src.str);
    
    // access the shared memory segment for the all products struct
    shmptr_product = (char *) malloc(sizeof(struct AllProducts));
    shmptr_product = createSharedMemory(SHKEY_PRODUCT, sizeof(struct AllProducts), src.str);
    ptrAllProducts = (struct AllProducts *) shmptr_product;

    // access the semaphore for the all products struct
    semid_product = createSemaphore(SEMKEY_PRODUCT, 2, src.str);


    sendPositionUpdateMsg(-1, -1);

    acquireSem(semid_product, 0, src.str);

    selectRandomProductsToBuy();

    releaseSem(semid_product, 0, src.str);

    return 0;
        

}

/*
 function to select radom products with random amount to buy
*/
void selectRandomProductsToBuy() {
    // create temp array of all products which is a copy of the shared memory segment
    struct AllProducts tempProducts;
    memcpy(&tempProducts, ptrAllProducts, sizeof(struct AllProducts));

    // select random number of products to buy
    int numProcutsToBuy = randomInRange(1, tempProducts.numProducts);

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

                printf("customer %d bought %d of product %d '%s'\n", customer_id, amount, ptrAllProducts->products[j].ID, ptrAllProducts->products[j].Name.str );
                fflush(stdout);

                // print the info of the product after buying
                printProductInfo(ptrAllProducts->products[j]);

                // send a message to the gui to update the position of the customer
                sendPositionUpdateMsg(-1, productID);
                sleep(1);
                
                /* 
                 * if the amount of the product on shelves becomes less than the threshold
                 * send a message to a random team to bring more of the product
                */
                if (ptrAllProducts->products[j].onShelvesAmount < productAmountThresh) {
                    // send signal to a random team to bring more of the product
                    int randomTeamIndex = randomInRange(0, numShelvingTeams);
          
                    // create the message
                    requestMsg.msg_to = randomTeamIndex+1;
                    requestMsg.msg_from = getpid();
                    requestMsg.index = j;
                    // send the message
                    if (msgsnd(msgqid_team, &requestMsg, sizeof(requestMsg), 0) == -1) {
                        perror("msgsnd");
                        exit(1);
                    }

                    printf("customer %d sent a request to team %d to bring more of product %d '%s'\n", customer_id, randomTeamIndex, ptrAllProducts->products[j].ID, ptrAllProducts->products[j].Name.str);
                    fflush(stdout);
                    // receive the message from the team
                    if (msgrcv(msgqid_team, &requestMsg, sizeof(requestMsg), getpid(), 0) == -1) {
                        perror("msgrcv -- customer.c");
                        exit(1);
                    }

                } 
                
                break;



            }
        }

        

    }
    // send a message to the gui to update the position of the customer
    sendPositionUpdateMsg(1, -1);


}


/* 
 * function to print the info of a product
*/
void printProductInfo(struct Product product) {
    printf("*****************************************************************************************\n");
    printf("Product %d:\t", product.ID);
    printf("Name: %s,\t", product.Name.str);
    printf("onShelvesAmount: %d,\t", product.onShelvesAmount);
    printf("StorageAmount: %d,\n", product.storageAmount);
    printf("****************************************************************************************\n");
    printf("\n");
    fflush(stdout);
}

/*
 * function to send a message to the gui to update the position of the customer
*/
void sendPositionUpdateMsg(int x, int y) {
    positionUpdateMsg = (PositionUpdateMessage *) malloc(sizeof(PositionUpdateMessage));
    positionUpdateMsg->msgType = MSG_POS_UPDATE;
    positionUpdateMsg->personType = 1; // 1 for customer
    positionUpdateMsg->id = customer_id;
    positionUpdateMsg->x = x;
    positionUpdateMsg->y = y;


    if (msgsnd(msgqid_gui, positionUpdateMsg, sizeof(PositionUpdateMessage), 0) == -1) {
        perror("msgsnd -- customer.c");
        exit(1);
    }
}