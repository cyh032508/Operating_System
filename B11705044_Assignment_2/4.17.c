#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t lock;
unsigned long long circle_points = 0, square_points = 0;

void* generate_points(void* arg) {
    unsigned long long* num_points = (unsigned long long*)arg;

    for (unsigned long long i = 0; i < *num_points; i++) {
        double rand_x = (double)rand() / RAND_MAX;
        double rand_y = (double)rand() / RAND_MAX;
        double origin_dist = rand_x * rand_x + rand_y * rand_y;

        pthread_mutex_lock(&lock);
        if (origin_dist <= 1) {
            circle_points++;
        }
        square_points++;
        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    unsigned long long num_points;

    printf("Enter the number of random points to generate: ");
    scanf("%llu", &num_points);

    pthread_t thread;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&thread, NULL, generate_points, (void*)&num_points);
    pthread_join(thread, NULL);

    double pi = (double)(4 * circle_points) / num_points;

    printf("Final Estimation of Pi = %f\n", pi);

    pthread_mutex_destroy(&lock);

    return 0;
}
