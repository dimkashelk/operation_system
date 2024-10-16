#include "shmem.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

main() {
  Message *msgptr;
  key_t key;
  int shmid, semid;

  /*Получим ключ. Один и тот же ключ можно использовать как для
          семафора, так и для разделяемой памяти*/
  if ((key = ftok("server", ‘A’)) < 0) {
    printf("Невозможно получить ключ\n") : exit(1);
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

  /*Заблокируем разделяемую память*/
  if (semop(semid, &proc_lock[0], 2) < 0) {
    printf("Невозможно выполнить операцию\n");
    exit(1);
  }

  /*Уведомим сервер о начале работы*/
  if (semop(semid, &proc_start[0], 1) < 0) {
    printf("Невозможно выполнить операцию\n");
    exit(1);
  }

  /*Запишем в разделяемую память сообщение*/
  sprintf(msgptr->buff, "Good luck!\n");

  /*Освободим разделяемую память*/
  if (semop(semid, &proc_unlock[0], 1) < 0) {
    printf("Невозможно выполнить операцию\n");
    exit(1);
  }

  /*Ждем, пока сервер в свою очередь не освободит разделяемую
                    память*/
  if (semop(semid, &proc_lock[0], 2) < 0) {
    printf("Невозможно выполнить операцию\n");
    exit(1);
  }

  /*Отключимся от области*/
  if (shmdt(msgptr) < 0) {
    printf("Ошибка отключения\n");
    exit(1);
  }

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