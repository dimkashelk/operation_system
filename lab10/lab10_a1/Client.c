#include "shmem.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

void print_sem_values(int semid) {
    printf("Клиент: Значение семафора 0: %d\n", semctl(semid, 0, GETVAL));
    printf("Клиент: Значение семафора 1: %d\n", semctl(semid, 1, GETVAL));
}

int main() {
  Message *msgptr;
  key_t key;
  int shmid, semid;

  /*Получим ключ. Один и тот же ключ можно использовать как для семафора, так и для разделяемой памяти*/
  if ((key = ftok("lab10_a1_server", 'A')) < 0) {
    printf("Невозможно получить ключ\n");
    exit(1);
  }

  /*Получим доступ к разделяемой памяти*/
  if ((shmid = shmget(key, sizeof(Message), 0)) < 0) {
    printf("Ошибка доступа\n");
    exit(1);
  }

  /*Присоединим ее*/
  if ((msgptr = (Message *)shmat(shmid, 0, 0)) < 0) {
    printf("Ошибка присоединения\n");
    exit(1);
  }

  /*Получим доступ к семафору*/
  if ((semid = semget(key, 2, PERM)) < 0) {
    printf("Ошибка доступа\n");
    exit(1);
  }
  print_sem_values(semid);

  /*Заблокируем разделяемую память*/
  if (semop(semid, &mem_lock[0], 2) < 0) {
    printf("Невозможно выполнить операцию\n");
    exit(1);
  }
  print_sem_values(semid);

  /*Уведомим сервер о начале работы*/
  if (semop(semid, &proc_start[0], 1) < 0) {
    printf("Невозможно выполнить операцию\n");
    exit(1);
  }
  print_sem_values(semid);

  /*Запишем в разделяемую память сообщение*/
  sprintf(msgptr->buff, "Good luck!\n");

  /*Освободим разделяемую память*/
  if (semop(semid, &mem_unlock[0], 1) < 0) {
    printf("Невозможно выполнить операцию\n");
    exit(1);
  }
  print_sem_values(semid);

  sleep(1);

  /*Ждем, пока сервер в свою очередь не освободит разделяемую память*/
  if (semop(semid, &mem_lock[0], 2) < 0) {
    printf("Невозможно выполнить операцию\n");
    exit(1);
  }
  print_sem_values(semid);

  /*Отключимся от области*/
  if (shmdt(msgptr) < 0) {
    printf("Ошибка отключения\n");
    exit(1);
  }
  print_sem_values(semid);

  /*Удалим созданные объекты IPC*/
  if (shmctl(shmid, IPC_RMID, 0) < 0) {
    printf("Невозможно удалить область\n");
    exit(1);
  }
  if (semctl(semid, 0, IPC_RMID) < 0) {
    printf("Невозможно удалить семафор\n");
    exit(1);
  }
  exit(0);
}
