/*
shelvingteam.c
this file executes the shelving team process
*/

#include "local.h"
#include "local2.h"

char *shmptr_product;
int msgqid_team;

ProductRequestMessage *requestMsg;
struct AllProducts *ptrAllProducts;
int rollingCartAmount, // amount of the product on the rolling cart
    productIndex,  // current product index
    productAmountThresh; 

pthread_mutex_t mutex_rollingCartAmount = PTHREAD_MUTEX_INITIALIZER;


int main (int argc, char *argv[]) {
    
    if (argc != 4) {
        fprintf(stderr, "Usage: %s team_id\n", *argv);
        exit(1);
    }

    int team_id = atoi(argv[1]);
    int num_employees = atoi(argv[2]);
    productAmountThresh = atoi(argv[3]);

    pthread_t employees[num_employees];

    printf("Team %d: %d employees\n", team_id, num_employees);
    fflush(stdout);

    struct String src;
    sprintf(src.str, "shelvingteam.c -- team %d", team_id);

    // access the shared memory segment for the all products struct
    shmptr_product = (char *) malloc(sizeof(struct AllProducts));
    shmptr_product = createSharedMemory(SHKEY_PRODUCT, sizeof(struct AllProducts), src.str);
    ptrAllProducts = (struct AllProducts *) shmptr_product;
    
    // access the msg queue for the teams
    msgqid_team = createMessageQueue(MSGQKEY_TEAM, src.str);

    requestMsg = (ProductRequestMessage *) malloc(sizeof(ProductRequestMessage));

    // keep receiving requests from the customers
    while (msgrcv (msgqid_team, requestMsg, sizeof(ProductRequestMessage), team_id+1, 0) != -1) {
 
        int customer_id = requestMsg->msg_from;
        int productIndex = requestMsg->index;
            
        printf("Team %d: received request for product %d\n", team_id, productIndex);
        fflush(stdout);
        
        // check if the product is available
        if (ptrAllProducts->products[productIndex].storageAmount > 0) {
            /* the manager brings the necessary amount of 
            product from the storage to the rolling cart */
            rollingCartAmount = bringProductFromStorage(productIndex);
            

            // create thread for each employee
            for (int i = 0; i < num_employees; i++) {
                pthread_create(&employees[i], NULL, (void*)employee, (void*)&i);
            }

            // wait for the threads to finish
            for (int i = 0; i < num_employees; i++) {
                pthread_join(employees[i], NULL);
            }  
           
        }
        // if the product is not available in the storage nor on the shelves (out of stock)
        else if (ptrAllProducts->products[productIndex].onShelvesAmount == 0) {
            
            printf("Team %d: product %d is not available in Storage\n", team_id, productIndex);
            fflush(stdout);

            // delete the product from the shared memory segment
            deleteProduct(productIndex);
        

        }
        // if the product is not available in the storage but is available on the shelves (less than the threshold)
        else {
            printf("Team %d: product %d is not available in the storage but is available on the shelves\n", team_id, productIndex);
            fflush(stdout);
            // do nothing
    
        }

        // after the team finishes working, send a message to the customer
        ProductRequestMessage requestMsg;
        requestMsg.msg_to = customer_id;
        requestMsg.msg_from = team_id;
        requestMsg.index = productIndex;
        
        int msgqid_team = createMessageQueue(MSGQKEY_TEAM, src.str);

        if (msgsnd(msgqid_team, &requestMsg, sizeof(ProductRequestMessage), 0) == -1) {
            fprintf(stderr, "msgsnd failed: %s\n", strerror(errno));

            perror("msgsnd -- shelvingteam.c");
            exit(1);
        }
        
        
        
    }
    
    printf("Team %d: finished working\n", team_id);
    fflush(stdout);

}


/*
function to bring the product from the storage to the rolling cart
*/
int bringProductFromStorage(int productIndex) {
    int storageAmount = ptrAllProducts->products[productIndex].storageAmount;
    int onShelvesAmount = ptrAllProducts->products[productIndex].onShelvesAmount;
    int shelfCapacity = ptrAllProducts->products[productIndex].shelfCapacity;

    // if the amount of the product in the storage is more than the shelf capacity
    if (storageAmount + onShelvesAmount >= shelfCapacity) {
        ptrAllProducts->products[productIndex].storageAmount = storageAmount + onShelvesAmount - shelfCapacity;
        return shelfCapacity - onShelvesAmount;
    }
    else { // if the amount of the product in the storage is less than the shelf capacity
        ptrAllProducts->products[productIndex].storageAmount = 0;
        return storageAmount;
    }

}

/*
function to simulate the employee working
*/
void employee(int *employee_id) {
    printf("Employee %d: started working\n", *employee_id);
    fflush(stdout);

    // while there is still product on the rolling cart
    while (1) {
        pthread_mutex_lock(&mutex_rollingCartAmount);

        // Check if there is a product on the rolling cart
        if (rollingCartAmount <= 0) {
            pthread_mutex_unlock(&mutex_rollingCartAmount);
            break;
        }

        // Decrease the amount of the product on the rolling cart
        rollingCartAmount--;

        // Increase the amount of the product on the shelves
        ptrAllProducts->products[productIndex].onShelvesAmount++;

        pthread_mutex_unlock(&mutex_rollingCartAmount);
    }
    pthread_exit(NULL);

    
}


// function to delete the product from the shared memory segment
void deleteProduct(int index) {
    ptrAllProducts->products[index].ID = -1;
    ptrAllProducts->products[index].onShelvesAmount = -1;
    ptrAllProducts->products[index].storageAmount = -1;

    // move the last product to the deleted product's place
    ptrAllProducts->products[index] = ptrAllProducts->products[ptrAllProducts->numProducts - 1];
    ptrAllProducts->numProducts--;

    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("$   Team %d: deleted product %d  $\n", index, ptrAllProducts->numProducts);
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    fflush(stdout);

    // check if the storage is out of stock
    if (ptrAllProducts->numProducts == 0) {
        printf("Storage is out of stock\n");
        fflush(stdout);
        // send signal to parent process to terminate the program
        if (kill(getppid(), SIGUSR1) == -1) {
            perror("kill");
            exit(1);
        }
        exit(1);
    }

}
