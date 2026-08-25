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

    memset(result, 0, sizeof(HttpRequest));

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

    return 0;
}