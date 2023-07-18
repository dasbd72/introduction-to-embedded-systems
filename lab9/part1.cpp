#include <pthread.h>

#include <array>
#include <iostream>
#include <vector>

const int num_threads = 5;
int counter = 0;
pthread_mutex_t mutex;

void* thread_func(void* args) {
    for (int l = 0; l < 1000; l++) {
        pthread_mutex_lock(&mutex);
        int i = counter;
        for (int l2 = 0; l2 < 1000; l2++) {
            i += 1;
        }
        counter = i;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    std::array<pthread_t, num_threads> threads;

    pthread_mutex_init(&mutex, NULL);
    for (int t = 0; t < num_threads; t++) {
        pthread_create(&threads[t], NULL, thread_func, NULL);
    }
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    std::cout << counter << "\n";
}