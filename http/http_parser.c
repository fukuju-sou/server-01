#include "http_parser.h"

#include <stdio.h>
#include <string.h>

int http_parse_request(
    const char *request,
    HttpRequest *result)
{
    if (request == NULL || result == NULL) {
        return -1;
    }

    memset(
        result,
        0,
        sizeof(HttpRequest)
    );

    /*
     * HTTP Requestの最初の行を解析する。
     *
     * 例:
     *
     * GET /hello HTTP/1.1
     */
    int fields = sscanf(
        request,
        "%15s %255s %15s",
        result->method,
        result->path,
        result->version
    );

    if (fields != 3) {
        return -1;
    }

    /*
     * HTTP HeaderとBodyの境界を探す。
     *
     * GET / HTTP/1.1
     * Host: localhost
     *
     * <---- header ---->
     *                    <---- body ---->
     */
    const char *body_start = strstr(
        request,
        "\r\n\r\n"
    );

    if (body_start != NULL) {

        body_start += 4;

        snprintf(
            result->body,
            HTTP_BODY_SIZE,
            "%s",
            body_start
        );
    }

    return 0;
}
