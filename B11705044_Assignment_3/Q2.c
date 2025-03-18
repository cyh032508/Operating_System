#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 5
#define POINTS_PER_THREAD 1000

pthread_mutex_t lock;
int points_in_circle = 0;

void *generate_points(void *arg) {
    int local_count = 0;
    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(uintptr_t)pthread_self();
    
    for (int i = 0; i < POINTS_PER_THREAD; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if ((x * x + y * y) <= 1.0) {
            local_count++;
        }
    }

    pthread_mutex_lock(&lock);
    // critical section
    points_in_circle += local_count;
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, generate_points, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    int total_points = NUM_THREADS * POINTS_PER_THREAD;
    double pi_estimate = 4.0 * points_in_circle / total_points;
    printf("Estimated value of pi: %f\n", pi_estimate);

    return 0;
}
