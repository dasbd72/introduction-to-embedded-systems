#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <ctime>
#include <iostream>
#include <vector>

const int num_threads = 40;
const int semaphore_count = 1;
int counter = 0;
pthread_mutex_t mutex;
sem_t semaphore;
auto start_time = std::chrono::high_resolution_clock::now();

void* thread_func(void* args) {
    sem_wait(&semaphore);
    pthread_mutex_lock(&mutex);
    std::cout << "Enter time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time).count()
              << "\n";
    // int delay = (rand() % 10) * 1000000;
    int delay = 10000000;
    pthread_mutex_unlock(&mutex);
    usleep(delay);
    sem_post(&semaphore);
    return NULL;
}

int main() {
    srand(time(NULL));
    std::array<pthread_t, num_threads> threads;

    sem_init(&semaphore, 0, semaphore_count);
    pthread_mutex_init(&mutex, NULL);
    for (int t = 0; t < num_threads; t++) {
        pthread_create(&threads[t], NULL, thread_func, NULL);
    }
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }
    sem_destroy(&semaphore);
    pthread_mutex_destroy(&mutex);

    std::cout << counter << "\n";
}