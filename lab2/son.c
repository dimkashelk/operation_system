#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pid, ppid;
    pid = getpid();    // Получаем PID процесса son
    ppid = getppid();  // Получаем PID родительского процесса son
    printf("SON PARAM: pid=%i ppid=%i\n", pid, ppid);

    // Задержка на 15 секунд, чтобы наблюдать процессы в ps xf
    sleep(5);

    printf("SON process finished.\n");
    return 0;
}
