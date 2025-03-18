#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define MAX_SEQUENCE 10

typedef struct {
    long fib_sequence[MAX_SEQUENCE];
    int sequence_size;
} shared_data;

int fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <sequence_size>\n", argv[0]);
        return 1;
    }

    int sequence_size = atoi(argv[1]);
    if (sequence_size <= 0 || sequence_size > MAX_SEQUENCE) {
        fprintf(stderr, "Sequence size must be between 1 and %d\n", MAX_SEQUENCE);
        return 1;
    }

    shared_data *shared_memory = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    shared_memory->sequence_size = sequence_size;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        for (int i = 0; i < sequence_size; ++i) {
            shared_memory->fib_sequence[i] = fibonacci(i);
        }
        exit(0);
    } else {
        wait(NULL);
        printf("Fibonacci sequence:");
        for (int i = 0; i < sequence_size; ++i) {
            printf(" %ld", shared_memory->fib_sequence[i]);
        }
        printf("\n");
        if (munmap(shared_memory, sizeof(shared_data)) == -1) {
            perror("munmap");
            return 1;
        }
    }
    return 0;
}
