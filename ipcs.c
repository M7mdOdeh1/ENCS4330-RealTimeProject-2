

#include"local.h"

/*
 * function to create shared memory segment for all product struct 
 *   and attach it to the parent process if not created and access it if created
*/
void createSharedMemory(int key, char *shmptr, char *src) {
    // Create a shared memory segment for the all products struct
    int shmid = shmget( key, sizeof(struct AllProducts), 0666 | IPC_CREAT);
    if (shmid == -1) {
        struct String errorSrc;
        sprintf(errorSrc.str, "shmget -- %s -- creation\n", src);
        perror(errorSrc.str);
        exit(1);
    }
   
    // Attach the shared memory segment to the parent process
    if ( (shmptr = (char *) shmat(shmid, 0, 0)) == (char *) -1 ) {
        perror("shmat -- parent -- attach");
        exit(2);
    }
    
}


// function to create a semaphore
void createSemaphore(int *semid, int key, int num, char* src) {
    // Create a semaphore for the all products struct
    *semid = semget(key, num, 0666 | IPC_CREAT);
    if (*semid == -1) {
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

    if ( semctl(*semid, 0, SETALL, arg) == -1 ) {
      perror("semctl -- initialization");
      exit(4);
    }
}