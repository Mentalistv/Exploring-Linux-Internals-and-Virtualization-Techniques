#ifndef IOCTL_DRIVER_H
#define IOCTL_DRIVER_H

#include <linux/ioctl.h>

// Device name for the ioctl driver
#define DEVICE_NAME "ioctl_driver"

// ioctl command to change the parent process
#define IOCTL_CHANGE_PARENT _IOW('p', 0, pid_t)  // Define the IOCTL command

#endif  // IOCTL_DRIVER_H
