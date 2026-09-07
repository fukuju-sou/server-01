#include "http_server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http_client.h"

#define BACKLOG 10

static int create_server_socket(int port)
{
    int server_fd;
    int opt = 1;

    struct sockaddr_in server_addr;

    server_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    /*
     * サーバを再起動したときに
     * "Address already in use" が発生しにくくする。
     */
    if (setsockopt(
            server_fd,
            SOL_SOCKET,
            SO_REUSEADDR,
            &opt,
            sizeof(opt)) < 0) {

        perror("setsockopt");
        close(server_fd);

        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(
            server_fd,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)) < 0) {

        perror("bind");
        close(server_fd);

        return -1;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);

        return -1;
    }

    return server_fd;
}

int http_server_start(int port)
{
    int server_fd;

    server_fd = create_server_socket(port);

    if (server_fd < 0) {
        return -1;
    }

    printf(
        "HTTP server started on port %d\n",
        port
    );

    while (1) {

        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(
            server_fd,
            (struct sockaddr *)&client_addr,
            &client_len
        );

        if (client_fd < 0) {

            if (errno == EINTR) {
                continue;
            }

            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];

        inet_ntop(
            AF_INET,
            &client_addr.sin_addr,
            client_ip,
            sizeof(client_ip)
        );

        printf(
            "Client connected: %s:%d\n",
            client_ip,
            ntohs(client_addr.sin_port)
        );

        /*
         * クライアントFDをヒープに確保する。
         * スレッドごとに独立したFDを渡すため。
         */
        int *client_fd_ptr = malloc(sizeof(int));

        if (client_fd_ptr == NULL) {
            perror("malloc");

            close(client_fd);
            continue;
        }

        *client_fd_ptr = client_fd;

        pthread_t thread;

        int result = pthread_create(
            &thread,
            NULL,
            http_client_thread,
            client_fd_ptr
        );

        if (result != 0) {
            fprintf(
                stderr,
                "pthread_create failed\n"
            );

            free(client_fd_ptr);
            close(client_fd);

            continue;
        }

        /*
         * メインスレッドは待機しない。
         * クライアント処理が終了したら
         * スレッド自身がリソースを解放する。
         */
        pthread_detach(thread);
    }

    close(server_fd);

    return 0;
}
