#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

const char *http_create_response(
    int status_code,
    const char *body
);

void http_free_response(const char *response);

#endif