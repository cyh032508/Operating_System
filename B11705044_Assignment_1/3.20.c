#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <sourcefile> <destinationfile>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *sourcefile = argv[1];
    char *destinationfile = argv[2];

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) { // Child process
        close(pipefd[1]); // Close the write end of the pipe
        int destfd = open(destinationfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (destfd == -1) {
            perror("open");
            return EXIT_FAILURE;
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            if (write(destfd, buffer, bytes_read) != bytes_read) {
                perror("write");
                return EXIT_FAILURE;
            }
        }

        if (bytes_read == -1) {
            perror("read");
            return EXIT_FAILURE;
        }

        close(pipefd[0]); // Close the read end of the pipe
        close(destfd); // Close the destination file
        return EXIT_SUCCESS;
    } else { // Parent process
        close(pipefd[0]); // Close the read end of the pipe
        int sourcefd = open(sourcefile, O_RDONLY);
        if (sourcefd == -1) {
            perror("open");
            return EXIT_FAILURE;
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(sourcefd, buffer, BUFFER_SIZE)) > 0) {
            if (write(pipefd[1], buffer, bytes_read) != bytes_read) {
                perror("write");
                return EXIT_FAILURE;
            }
        }

        if (bytes_read == -1) {
            perror("read");
            return EXIT_FAILURE;
        }

        close(pipefd[1]); 
        close(sourcefd); 
        wait(NULL); 
        return EXIT_SUCCESS;
    }
}
