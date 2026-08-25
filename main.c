#include <stdio.h>

#include "http/http_server.h"

#define HTTP_PORT 8080

int main(void)
{
    printf("Starting server...\n");

    if (http_server_start(HTTP_PORT) != 0) {
        fprintf(stderr, "Failed to start HTTP server\n");
        return 1;
    }

    return 0;
}