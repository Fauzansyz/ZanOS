#ifndef _LINUX_PLATFORM_DEVICE_H
#define _LINUX_PLATFORM_DEVICE_H

struct platform_device {
    const char *name;
    int id;
};

// Makro tiruan untuk mendaftarkan driver ke sistem Linux
#define module_platform_driver(x)

#endif

