#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

void print_sem_values(int semid) {
    printf("Сервер: Значение семафора 0: %d\n", semctl(semid, 0, GETVAL));
    printf("Сервер: Значение семафора 1: %d\n", semctl(semid, 1, GETVAL));
}

#include "shmem.h"
int main() {
  Message *msgptr;
  key_t key;
  int shmid, semid;

  /*Получим ключ. Один и тот же ключ можно использовать как для семафора, так и для разделяемой памяти*/
  if ((key = ftok("lab10_a1_server", 'A')) < 0) {
    printf("Невозможно получить ключ\n");
    exit(1);
  }
  printf("Ключ получен\n");

  /*Создадим область разделяемой памяти*/
  if ((shmid = shmget(key, sizeof(Message), PERM | IPC_CREAT)) < 0) {
    printf("Невозможно создать область\n");
    exit(1);
  }
  /*Присоединим ее*/
  if ((msgptr = (Message *)shmat(shmid, 0, 0)) < 0) {
    printf("Ошибка присоединения\n");
    exit(1);
  }

  /*Создадим группу из двух семафоров:
      Первый семафор – для синхронизации работы с разделяемой памятью
      Второй семафор – для синхронизации выполнения процессов*/
  if ((semid = semget(key, 2, PERM | IPC_CREAT)) < 0) {
    printf("Невозможно создать семафор\n");
    exit(1);
  }
  semctl(semid, 0, SETVAL, 0);
  semctl(semid, 1, SETVAL, 0);
  print_sem_values(semid);

  /*Ждем, пока клиент начнет работу и заблокирует разделяемую память*/
  if (semop(semid, &proc_wait[0], 1) < 0) {
    printf("Невозможно выполнить операцию\n");
    exit(1);
  }
  print_sem_values(semid);

  /*Ждем, пока клиент закончит запись в разделяемую память и совободит ее. После этого заблокируем ее*/
  if (semop(semid, &mem_lock[0], 2) < 0) {
    printf("Невозможно выполнить операцию\n");
    exit(1);
  }
  print_sem_values(semid);

  /*Выведем сообщение на терминал*/
  printf("%s", msgptr->buff);

  /*Освободим разделяемую память*/
  if (shmdt(msgptr) < 0) {
    printf("Ошибка отключения\n");
    exit(1);
  }
  print_sem_values(semid);
  /*Освободим разделяемую память*/
  if (semop(semid, &mem_unlock[0], 1) < 0) {
    printf("Невозможно выполнить операцию\n");
    exit(1);
  }
  print_sem_values(semid);

  /*Всю остальную работу по удалению объектов сделает клиент*/
  exit(0);
}
