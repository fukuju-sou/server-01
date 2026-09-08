#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#define HTTP_METHOD_SIZE 16
#define HTTP_PATH_SIZE 256
#define HTTP_VERSION_SIZE 16
#define HTTP_BODY_SIZE 4096

typedef struct {
    char method[HTTP_METHOD_SIZE];
    char path[HTTP_PATH_SIZE];
    char version[HTTP_VERSION_SIZE];
    char body[HTTP_BODY_SIZE];
} HttpRequest;

int http_parse_request(
    const char *request,
    HttpRequest *result
);

#endif
