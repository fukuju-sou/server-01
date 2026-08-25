#include "http_response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *http_create_response(
    int status_code,
    const char *body)
{
    const char *reason;

    switch (status_code) {

        case 200:
            reason = "OK";
            break;

        case 400:
            reason = "Bad Request";
            break;

        case 404:
            reason = "Not Found";
            break;

        case 405:
            reason = "Method Not Allowed";
            break;

        default:
            reason = "Internal Server Error";
            status_code = 500;
            break;
    }

    size_t body_length = strlen(body);

    size_t response_size =
        1024 + body_length;

    char *response = malloc(response_size);

    if (response == NULL) {
        return NULL;
    }

    snprintf(
        response,
        response_size,

        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",

        status_code,
        reason,
        body_length,
        body
    );

    return response;
}

void http_free_response(const char *response)
{
    free((void *)response);
}