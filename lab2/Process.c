#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 3

typedef struct {
    int id;
    int count;
} thread_data;

void* thread_function(void* args) {
    thread_data* data = (thread_data*) args;

    while (1) {
        printf("Thread %d: count = %d\n", data->id, data->count);
        
        data->count += 1;

        sleep(5);
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    thread_data thread_data_array[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data_array[i].id = i + 1;
        thread_data_array[i].count = 0;
        pthread_create(&threads[i], NULL, thread_function, &thread_data_array[i]);
    }

    for (int i = 1; i <= NUM_THread_join(threads[READS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
