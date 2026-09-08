#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http_client.h"
#include "http_parser.h"
#include "http_response.h"
#include "http_router.h"

#define BUFFER_SIZE 4096

void *http_client_thread(void *arg)
{
    int client_fd = *(int *)arg;

    free(arg);

    unsigned long thread_id =
        (unsigned long)pthread_self();

    printf(
        "[Thread %lu] START\n",
        thread_id
    );

    /*
     * マルチスレッド動作確認用。
     */
    sleep(5);

    char buffer[BUFFER_SIZE];

    memset(
        buffer,
        0,
        sizeof(buffer)
    );

    ssize_t received = recv(
        client_fd,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if (received < 0) {
        perror("recv");
        close(client_fd);
        return NULL;
    }

    if (received == 0) {
        close(client_fd);
        return NULL;
    }

    printf(
        "[Thread %lu] Received request:\n%s\n",
        thread_id,
        buffer
    );

    /*
     * HTTP RequestをParserで解析する。
     */
    HttpRequest request;

    if (http_parse_request(
            buffer,
            &request) != 0) {

        const char *response =
            http_create_response(
                400,
                "Bad Request\n"
            );

        if (response != NULL) {

            send(
                client_fd,
                response,
                strlen(response),
                0
            );

            http_free_response(response);
        }

        close(client_fd);
        return NULL;
    }

    printf(
        "[Thread %lu] Method: %s\n",
        thread_id,
        request.method
    );

    printf(
        "[Thread %lu] Path: %s\n",
        thread_id,
        request.path
    );

    /*
     * Routerを初期化する。
     */
    HttpRouter router;

    router_init(&router);

    /*
     * RouterにHTTP Requestを渡す。
     */
    char response_body[HTTP_ROUTER_BODY_SIZE];

    memset(
        response_body,
        0,
        sizeof(response_body)
    );

    int status_code = router_dispatch(
        &router,
        &request,
        response_body,
        sizeof(response_body)
    );

    /*
     * 404 / 405の場合はRouterが返した
     * Status Codeに応じてResponse Bodyを作る。
     */
    if (status_code == 404) {

        snprintf(
            response_body,
            sizeof(response_body),
            "Not Found\n"
        );

    } else if (status_code == 405) {

        snprintf(
            response_body,
            sizeof(response_body),
            "Method Not Allowed\n"
        );
    }

    /*
     * HTTP Responseを生成する。
     */
    const char *response =
        http_create_response(
            status_code,
            response_body
        );

    if (response == NULL) {

        close(client_fd);
        return NULL;
    }

    /*
     * ClientへResponseを送信する。
     */
    send(
        client_fd,
        response,
        strlen(response),
        0
    );

    http_free_response(response);

    printf(
        "[Thread %lu] END\n",
        thread_id
    );

    close(client_fd);

    return NULL;
}
