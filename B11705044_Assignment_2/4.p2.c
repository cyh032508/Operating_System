#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int* data;
int* result;

typedef struct {
    int start;
    int end;
} thread_data;

void* merge_sort(void* arg) {
    thread_data* td = (thread_data*)arg;
    int start = td->start;
    int end = td->end;

    if (start >= end) {
        return NULL;
    }

    int mid = (start + end) / 2;
    thread_data left_data = {start, mid};
    thread_data right_data = {mid + 1, end};
    pthread_t left_thread, right_thread;
    pthread_create(&left_thread, NULL, merge_sort, &left_data);
    pthread_create(&right_thread, NULL, merge_sort, &right_data);
    
    pthread_join(left_thread, NULL);
    pthread_join(right_thread, NULL);
    
    int i = start;
    int j = mid + 1;
    int k = start;
    while (i <= mid && j <= end) {
        if (data[i] < data[j]) {
            result[k++] = data[i++];
        } else {
            result[k++] = data[j++];
        }
    }
    while (i <= mid) {
        result[k++] = data[i++];
    }
    while (j <= end) {
        result[k++] = data[j++];
    }

    for (int l = start; l <= end; ++l) {
        data[l] = result[l];
    }

    return NULL;
}

int main() {
    int n;

    printf("Enter the number of elements: ");
    scanf("%d", &n);

    data = (int*)malloc(n * sizeof(int));
    result = (int*)malloc(n * sizeof(int));

    printf("Enter %d elements:", n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &data[i]);
    }

    thread_data initial_data = {0, n - 1};
    pthread_t initial_thread;
    
    pthread_create(&initial_thread, NULL, merge_sort, &initial_data);
    
    pthread_join(initial_thread, NULL);

    printf("Sorted Array: ");
    for (int i = 0; i < n; ++i) {
        printf("%d ", data[i]);
    }
    printf("\n");

    free(data);
    free(result);

    return 0;
}
