

#include"local.h"

struct sembuf acquire = {0, -1, SEM_UNDO}, 
              release = {0,  1, SEM_UNDO};

/*
 * function to create shared memory segment for all product struct if it doesn't exist
 * or access it if it's already exists
 * then attach it to the process
*/
char* createSharedMemory(int key, char *src) {
    // Create a shared memory segment for the all products struct
    int shmid = shmget( key, sizeof(struct AllProducts), 0666 | IPC_CREAT);
    if (shmid == -1) {
        struct String errorSrc;
        sprintf(errorSrc.str, "shmget -- %s -- creation\n", src);
        perror(errorSrc.str);
        exit(1);
    }
   
    char *shmptr;
    // Attach the shared memory segment to the parent process
    if ( (shmptr = (char *) shmat(shmid, 0, 0)) == (char *) -1 ) {
        struct String errorSrc;
        sprintf(errorSrc.str, "shmat -- %s -- attach", src);
        perror(errorSrc.str);
        exit(2);
    }
    return shmptr;
    
}


/*
 * function to create semaphore for all product struct 
 * and initialize it with ones if it doesn't exist
 * or access it if it already exists
*/
int createSemaphore(int key, int num, char* src) {

    int semid = semget(key, num, 0666);

    // if the semaphore doesn't exist create it
    if (semid == -1) {
        // Create a semaphore for the all products struct
        semid = semget(key, num, 0666 | IPC_CREAT);
        if (semid == -1) {
            struct String errorSrc;
            sprintf(errorSrc.str, "semget -- %s -- creation\n", src);
            perror(errorSrc.str);
            exit(3);
        }

        ushort start_val[num];
        // Initialize the semaphore with ones
        for (int i = 0; i < num; i++) {
            start_val[i] = 1;
        } 

        union semun    arg;
        arg.array = start_val; // initialize the semaphore

        if ( semctl(semid, 0, SETALL, arg) == -1 ) {
            struct String errorSrc;
            sprintf(errorSrc.str, "semctl -- %s -- initialization", src);
            perror(errorSrc.str);
            exit(4);
        }
    }


   

    return semid;
}

/*
function to print the shared memory segment of the all products struct
*/
void printSharedMemory(char *shmptr, char *src) {
    printf("====================================\n");
    printf("Shared memory segment accessed from %s:\n", src);

    int numProducts = ((struct AllProducts *) shmptr)->numProducts;
    printf("Number of products: %d\n", numProducts);
    fflush(stdout);

    for (int i = 0; i < numProducts; i++) {
        printf("Product %d:\n", ((struct AllProducts *) shmptr)->products[i].ID);
        printf("Name: %s\n", ((struct AllProducts *) shmptr)->products[i].Name.str);
        printf("On Shelves Amount: %d\n", ((struct AllProducts *) shmptr)->products[i].onShelvesAmount);
        printf("Storage Amount: %d\n", ((struct AllProducts *) shmptr)->products[i].StorageAmount);
        printf("--------------------\n");
        fflush(stdout);
    }
    printf("====================================\n");
    printf("\n");

}

/* 
 * function to clean up the shared memory segment
*/
void deleteSharedMemory(int key, char *shmptr) {
    int shmid = shmget(key, sizeof(struct AllProducts), 0666);

    // detach the shared memory segment from the parent process
    if (shmdt(shmptr) == -1) {
        perror("shmdt -- ipcs.c -- deleteSharedMemory -- detach");
        exit(5);
    }

    // remove the shared memory segment
    if (shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0) == -1) {
        perror("shmctl -- ipcs.c -- deleteSharedMemory -- remove");
        exit(6);

    }
}

/*
 * function to clean up the semaphore
*/
void deleteSemaphore(int semid) {
    // remove the semaphore
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(7);
    }
}


/* 
*function to acuire the semaphore
*/
void acquireSem(int semid, int semnum) {
    acquire.sem_num = semnum;
    if ( semop(semid, &acquire, 1) == -1 ) {
        perror("semop -- acquire");
        exit(4);
    }
}

/* 
*function to release the semaphore
*/
void releaseSem(int semid, int semnum) {
    release.sem_num = semnum;
    if ( semop(semid, &release, 1) == -1 ) {
        perror("semop -- release");
        exit(5);
    }
}


/*
*function to kill child processes  
*/
void killChildProcesses(int pids[], int numPids) {
    for (int i = 0; i < numPids; i++) {
        kill(pids[i], SIGINT);
    }
}
