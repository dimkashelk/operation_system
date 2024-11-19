#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void print_sem_values(int semid) {
    printf("Сервер: Значение семафора 0: %d\n", semctl(semid, 0, GETVAL));
    printf("Сервер: Значение семафора 1: %d\n", semctl(semid, 1, GETVAL));
}

#include "shmem.h"

void findEOF(char* buf) {
    for (int i = 0; i < 80; i++) {
        if (buf[i] == '\n') {
            // printf("There is \\n in buffer\n");
            buf[i] = '\0';
            break;
        }
    }
}

int main(int argc, char **argv) {
    Message *msgptr;
    key_t key;
    int shmid, semid;
    int fd, i, nloop;
    char *ptr;
    struct stat sb;
    char buffer[80];

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
    // print_sem_values(semid);

    /*Ждем, пока клиент начнет работу и заблокирует разделяемую память*/
    if (semop(semid, &proc_wait[0], 1) < 0) {
        printf("Невозможно выполнить операцию\n");
        exit(1);
    }
    // print_sem_values(semid);

    /*Ждем, пока клиент закончит запись в разделяемую память и совободит ее. После этого заблокируем ее*/
    if (semop(semid, &mem_lock[0], 2) < 0) {
        printf("Невозможно выполнить операцию\n");
        exit(1);
    }
    // print_sem_values(semid);
    // printf(msgptr->buff);
    findEOF(msgptr->buff);

  // Открываем файл для чтения и записи
    if ((fd = open(msgptr->buff, O_RDWR)) < 0) {
        sprintf(msgptr->buff, "Can not open file\n");
        if (shmdt(msgptr) < 0) {
            printf("Ошибка отключения\n");
            exit(1);
        }
        /*Освободим разделяемую память*/
        if (semop(semid, &mem_unlock[0], 1) < 0) {
            printf("Невозможно выполнить операцию\n");
            exit(1);
        }
        printf("Error occured while opening file\n");
        exit(0);
    }
    printf("File is opened!\n");

    // Получаем размер файла
    if (fstat(fd, &sb) == -1) {
        perror("fstat failed");
        exit(1);
    }

    // Маппируем файл в память с нужным размером
    ptr = (char*) mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    close(fd);
    i = 0;
    while (i < sb.st_size) {
        buffer[i % 80] = ptr[i % sb.st_size];
        if (i % 80 == 79) {
            sprintf(msgptr->buff, buffer);
            /*Освободим разделяемую память*/
            if (semop(semid, &mem_unlock[0], 1) < 0) {
                printf("Невозможно выполнить операцию\n");
                exit(1);
            }
            // print_sem_values(semid);
            sleep(1);
            /*Ждем, пока клиент закончит запись в разделяемую память и совободит ее. После этого заблокируем ее*/
            if (semop(semid, &mem_lock[0], 2) < 0) {
                printf("Невозможно выполнить операцию\n");
                exit(1);
            }
            // print_sem_values(semid);
        }
        i++;
        // char temp = ptr[i % sb.st_size]; // Чтение символа без изменения
        // printf("Child: %c\n", temp);
    }
    // printf("\nafter while\n");
    if (i % 80 != 79) {
        buffer[i % 80] = '\0';
        sprintf(msgptr->buff, buffer);
        /*Освободим разделяемую память*/
        if (semop(semid, &mem_unlock[0], 1) < 0) {
            printf("Невозможно выполнить операцию\n");
            exit(1);
        }
        // print_sem_values(semid);
        sleep(1);
        /*Ждем, пока клиент закончит запись в разделяемую память и совободит ее. После этого заблокируем ее*/
        if (semop(semid, &mem_lock[0], 2) < 0) {
            printf("Невозможно выполнить операцию\n");
            exit(1);
        }
        // print_sem_values(semid);
    }
    // printf("after if\n");
    sprintf(msgptr->buff, "server finished his work\n");
    // if (semop(semid, &mem_unlock[0], 1) < 0) {
    //     printf("Невозможно выполнить операцию\n");
    //     exit(1);
    // }
    // print_sem_values(semid);
    // sleep(1);
    // /*Ждем, пока клиент закончит запись в разделяемую память и совободит ее. После этого заблокируем ее*/
    // if (semop(semid, &mem_lock[0], 2) < 0) {
    //     printf("Невозможно выполнить операцию\n");
    //     exit(1);
    // }
    // print_sem_values(semid);

    /*Освободим разделяемую память*/
    if (shmdt(msgptr) < 0) {
        printf("Ошибка отключения\n");
        exit(1);
    }
    /*Освободим разделяемую память*/
    if (semop(semid, &mem_unlock[0], 1) < 0) {
        printf("Невозможно выполнить операцию\n");
        exit(1);
    }

  /*Всю остальную работу по удалению объектов сделает клиент*/
  exit(0);
}
