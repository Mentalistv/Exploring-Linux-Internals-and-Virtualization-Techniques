// user_program.c

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DEVICE_FILE "/dev/driver"
#define IOCTL_GET_PHYSICAL_ADDR _IOR('k', 1, unsigned long)
#define IOCTL_WRITE_PHYSICAL_ADDR _IOW('k', 2, unsigned long)

struct WritePhysicalParams {
    unsigned long virt_addr;
    char value;
};

int main() {
    int file_desc;
    unsigned long virt_addr, phys_addr;
    char value;

    file_desc = open(DEVICE_FILE, 0);
    if (file_desc < 0) {
        perror("Failed to open the device...");
        return errno;
    }

    // Allocate memory on the heap
    char *allocated_memory = malloc(1);
    if (!allocated_memory) {
        perror("Memory allocation failed");
        close(file_desc);
        return errno;
    }

    // Assign the value "6" to the memory
    *allocated_memory = 6;

    // Print virtual address and value
    printf("Virtual Address: %p, Value: %d\n", allocated_memory, *allocated_memory);

    // Get the physical address of the allocated memory
    virt_addr = (unsigned long)allocated_memory;
    if (ioctl(file_desc, IOCTL_GET_PHYSICAL_ADDR, &virt_addr) < 0) {
        perror("IOCTL_GET_PHYSICAL_ADDR failed");
        free(allocated_memory);
        close(file_desc);
        return errno;
    }

    printf("Physical Address: %lx\n", virt_addr);

    // Change the value using physical address
    struct WritePhysicalParams params;
    params.virt_addr = (unsigned long)allocated_memory;
    params.value = 5;
    
    if (ioctl(file_desc, IOCTL_WRITE_PHYSICAL_ADDR, &params) < 0) {
        perror("IOCTL_WRITE_PHYSICAL_ADDR failed");
        free(allocated_memory);
        close(file_desc);
        return errno;
    }

    // // Verify modified value
    printf("Modified Value: %d\n", *allocated_memory);

    free(allocated_memory);
    close(file_desc);

    return 0;
}
