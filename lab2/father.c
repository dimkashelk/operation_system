#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void level_2() {
    int pid, ppid, status;
    pid = getpid();    // Получаем PID процесса father
    ppid = getppid();  // Получаем PID родительского процесса father
    printf("\nLEVEL 2: FATHER PARAM: pid=%i ppid=%i\n", pid, ppid);

    // Создаем дочерний процесс
    if (fork() == 0) {
        // Код дочернего процесса
        execl("son", "son", NULL);  // Замещаем процесс son
    }

    // Ожидание завершения дочернего процесса
    wait(&status);
    printf("LEVEL 2: Child process finished.\n");
}

void level_3_a() {
    int pid, ppid;
    pid = getpid();    // Получаем PID процесса father
    ppid = getppid();  // Получаем PID родительского процесса father
    printf("\nLEVEL 3A: FATHER PARAM: pid=%i ppid=%i\n", pid, ppid);

    // Создаем дочерний процесс
    if (fork() == 0) {
        // Код дочернего процесса
        execl("son", "son", NULL);  // Замещаем процесс son
    }

    // Father завершает свою работу без ожидания дочернего процесса
    printf("LEVEL 3A: Father process exiting.\n");
    exit(0);
}

void level_3_b() {
    int pid, ppid;
    pid = getpid();    // Получаем PID процесса father
    ppid = getppid();  // Получаем PID родительского процесса father
    printf("\nLEVEL 3B: FATHER PARAM: pid=%i ppid=%i\n", pid, ppid);

    // Создаем дочерний процесс
    if (fork() == 0) {
        // Код дочернего процесса
        execl("son", "son", NULL);
        system("ps xf");
        // sleep(2);  // Даем время для того, чтобы родительский процесс завершился
        printf("LEVEL 3B: Son process finishing.\n");
        // exit(0); // Завершаем процесс son
    }
    system("ps xf");

    // Father завершает свою работу без ожидания дочернего процесса
    printf("LEVEL 3B: Father process exiting.\n");
    exit(0);
}

int main() {
    printf("Starting levels of process interaction:\n");

    // Выполнение уровня 2
    level_2();

    // Выполнение уровня 3 (A) - father не ждет son
    level_3_a();

    sleep(10);

    // Выполнение уровня 3 (B) - появление процесса-зомби
    level_3_b();

    return 0;
}
