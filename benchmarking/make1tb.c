
#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

int main() {
    // Define the file path and size
    const char* file_path = "large_file.bin";
    const uint64_t file_size = (uint64_t) 1 * 1024 * 1024 * 1024 * 1024; // 1 TiB in bytes
    const int file_flags = O_CREAT | O_WRONLY | O_DIRECT;
    const mode_t file_mode = 0666;
    struct timeval start_time, end_time;
    double elapsed_time;

    // Open the file with direct I/O
    int fd = open(file_path, file_flags, file_mode);
    if (fd == -1) {
        printf("Error: Could not open file (%d)\n", errno);
        return 1;
    }

    // Create a FILE stream for the file
    FILE* fp = fdopen(fd, "wb");
    if (fp == NULL) {
        printf("Error: Could not create stream for file for writing\n");
        exit(1);
    }

    // Write to the file in 8 MiB chunks
    const uint64_t chunk_size = (uint64_t) 8 * 1024 * 1024; // 1 MiB in bytes
    uint8_t* buffer = (uint8_t*) malloc(chunk_size);
    if (buffer == NULL) {
        printf("Error: Could not allocate memory for buffer\n");
        exit(1);
    }

    uint64_t remaining_size = file_size;
    while (remaining_size > 0) {
        uint64_t chunk_bytes = remaining_size < chunk_size ? remaining_size : chunk_size;
        gettimeofday(&start_time, NULL); // Start measuring wall-clock time
        fwrite(buffer, chunk_bytes, 1, fp);
 	gettimeofday(&end_time, NULL); // Stop measuring wall-clock time
        elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0; // Calculate elapsed time in seconds
	
        remaining_size -= chunk_bytes;
	printf("%ld bytes remaining (%.2f MiB in %.2f secs = %.2f MiB/s)\n",
		remaining_size,
		(double)chunk_size / 1024 / 1024,
		elapsed_time,
		(double)chunk_size / 1024 / 1024 / elapsed_time);
    }

    // Close the file and free the buffer
    fclose(fp);
    free(buffer);
    close(fd);

    printf("File created successfully\n");
    return 0;
}
