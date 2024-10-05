#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include "shmem.h"
SharedBuffer shared;
Semaphores sems;
void handle_sigint(int sig)
{
  // Clean up resources here (e.g., destroy semaphores)
  sem_destroy(sems.mutex);
  sem_destroy(sems.nempty);
  sem_destroy(sems.nstored);
  exit(0);
}
void *produce(void *arg)
{
  for (int i = 0; i < 20; i++)
  { // Produce 20 items
    sem_wait(sems.nempty); // Wait for an empty slot
    sem_wait(sems.mutex);   // Enter critical section

    shared.buff[shared.in] = i; // Produce an item
    printf("Produced: %d\n", i);
    shared.in = (shared.in + 1) % NBUFF; // Circular increment

    sem_post(sems.mutex);   // Exit critical section
    sem_post(sems.nstored);  // Signal that a new item is stored
  }
  return NULL;
}
void *consume(void *arg)
{
  for (int i = 0; i < 20; i++)
  { // Consume 20 items
    sem_wait(sems.nstored); // Wait for a stored item
    sem_wait(sems.mutex);   // Enter critical section

    int item = shared.buff[shared.out]; // Consume an item
    printf("Consumed: %d\n", item);
    shared.out = (shared.out + 1) % NBUFF; // Circular increment

    sem_post(sems.mutex);   // Exit critical section
    sem_post(sems.nempty);   // Signal that an empty slot is available
  }
  return NULL;
}
int main()
{
  signal(SIGINT, handle_sigint); // Register signal handler

  // Initialize shared buffer and semaphores
  shared.in = 0;
  shared.out = 0;
  sems.mutex = malloc(sizeof(sem_t));
  sems.nempty = malloc(sizeof(sem_t));
  sems.nstored = malloc(sizeof(sem_t));
  sem_init(sems.mutex, 1, 1);     // Binary semaphore
  sem_init(sems.nempty, 1, NBUFF); // Initialize with buffer size
  sem_init(sems.nstored, 1, 0);    // Start with zero stored items

  pthread_t tid_produce, tid_consume;
  pthread_create(&tid_produce, NULL, produce, NULL);
  pthread_create(&tid_consume, NULL, consume, NULL);
  pthread_join(tid_produce, NULL);
  pthread_join(tid_consume, NULL);
  free(sems.mutex);
  free(sems.nempty);
  free(sems.nstored);
  return 0;
}