#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define PAGE_SIZE sysconf(_SC_PAGESIZE)

int main()
{
    int fd = open("/proc/meminfo", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    printf("PID: %d\n", getpid());

    for (int size = PAGE_SIZE; size <= 10 * PAGE_SIZE; size += PAGE_SIZE) {
        void *ptr = mmap(NULL, 1024*10*size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap");
            exit(1);
        }

        char buffer[1024];
        int bytes_read = read(fd, buffer, sizeof(buffer));
        buffer[bytes_read - 1] = '\0';  // Remove trailing newline

        printf("Allocated memory: %d MB\n", 10*size / (1024));
        // printf("%s\n", buffer);

        munmap(ptr, size);
    }

    close(fd);

    printf("Press any key to exit...\n");
    getchar();

    return 0;
}