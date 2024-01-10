
/*
local2.h
This header file contains the function prototypes , constants, 
and structs related to IPCs used in the program.
*/

#ifndef LOCAL2_H
#define LOCAL2_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h> 
#include <wait.h>
#include <signal.h>
#include <pthread.h>  

char* createSharedMemory(int key, int size, char *src);
int createSemaphore(int key, int num, char *src);
void printSharedMemory(char *shmptr, char *src);
void deleteSemaphore(int semid);
void deleteSharedMemory(int key, char *shmptr);
void killChildProcesses(int pids[], int numPids);
void acquireSem(int semid, int semnum, char *src);
void releaseSem(int semid, int semnum, char *src);
void deleteMessageQueue(int msgqid);
int createMessageQueue(int key, char *src);
void initializeSignalHandlers();
void catchSIGUSR1(int signum);
void catchSIGALRM(int signum);


union semun {
    int val;
    struct semid_ds *buf;
    ushort *array; 
};


typedef struct {
    long msg_to;  // identifier of the receiver
    int msg_from; // identifier of the sender
    int index;    // index of the product in the shared memory
} ProductRequestMessage;



#endif