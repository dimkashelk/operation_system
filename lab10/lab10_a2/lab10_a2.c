#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd, i, nloop;
    struct stat sb;
    char *ptr;
    sem_t *mutex;

    nloop = 10;

    // Открываем файл для чтения и записи
    if ((fd = open("/home/bee/labs/operation_system/lab10/test.txt", O_RDWR)) < 0) {
        perror("Can not open file");
        exit(1);
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

    // Инициализация семафора
    sem_unlink("mysem");
    mutex = sem_open("mysem", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if (mutex == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    setbuf(stdout, NULL);

    if (fork() == 0) {
        // Дочерний процесс
        for (i = 0; i < nloop; i++) {
            sem_wait(mutex);
            char temp = ptr[i % sb.st_size]; // Чтение символа без изменения
            printf("Child: %c\n", temp);
            sem_post(mutex);
        }
        exit(0);
    }

    usleep(100000);

    // Родительский процесс
    for (i = 0; i < nloop; i++) {
        sem_wait(mutex);
        char temp = ptr[i % sb.st_size]; // Чтение символа без изменения
        printf("Parent: %c\n", temp);
        sem_post(mutex);
    }

    sem_close(mutex);
    sem_unlink("mysem");
    munmap(ptr, sb.st_size);

    exit(0);
}
