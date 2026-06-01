#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stdbool.h>

/* Mulai proses HTTP GET asinkron */
void http_get(const char *hostname, const char *path);

/* Mengembalikan true jika HTTP client sedang sibuk mendownload */
bool http_is_busy(void);

#endif
