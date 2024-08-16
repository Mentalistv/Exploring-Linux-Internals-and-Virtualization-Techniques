#include <stdio.h>
#include <unistd.h>

int main() {
    // Get the address of the main function
    void *address = (void *)main;

    // Get the process ID
    pid_t pid = getpid();

    // Print the hex address and PID
    printf("Hex Address: %p\n", address);
    printf("Process ID (PID): %d\n", pid);

    // Wait for a key press
    printf("Press any key to exit...\n");
    getchar();

    return 0;
}
