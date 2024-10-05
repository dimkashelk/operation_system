#ifndef LAB4_SHMEM_H
#define LAB4_SHMEM_H
#include <semaphore.h>
#define NBUFF 10 // Size of the circular buffer
#define MAXBUFF 80 // Max message length
#define PERM 0666 // Permissions for shared memory
typedef struct
{
  int buff[NBUFF];
  int in; // Index for the next item to produce
  int out; // Index for the next item to consume
} SharedBuffer;
typedef struct
{
  sem_t *mutex;
  sem_t *nempty;
  sem_t *nstored;
} Semaphores;
#endif