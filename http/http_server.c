#include "http_server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http_parser.h"
#include "http_response.h"

#define BUFFER_SIZE 4096
#define BACKLOG 10

static int create_server_socket(int port)
{
    int server_fd;
    int opt = 1;

    struct sockaddr_in server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

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

static void handle_client(int client_fd)
{
    char buffer[BUFFER_SIZE];

    memset(buffer, 0, sizeof(buffer));

    ssize_t received = recv(
        client_fd,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if (received < 0) {
        perror("recv");
        return;
    }

    if (received == 0) {
        printf("Client closed connection\n");
        return;
    }

    printf("----- HTTP Request -----\n");
    printf("%s", buffer);
    printf("------------------------\n");

    HttpRequest request;

    if (http_parse_request(buffer, &request) != 0) {

        const char *response = http_create_response(
            400,
            "Bad Request"
        );

        send(
            client_fd,
            response,
            strlen(response),
            0
        );

        http_free_response(response);

        return;
    }

    printf(
        "Method : %s\n"
        "Path   : %s\n"
        "Version: %s\n",
        request.method,
        request.path,
        request.version
    );

    const char *response;

    if (strcmp(request.method, "GET") != 0) {

        response = http_create_response(
            405,
            "Method Not Allowed"
        );

    } else if (strcmp(request.path, "/") == 0) {

        response = http_create_response(
            200,
            "Hello, World!"
        );

    } else if (strcmp(request.path, "/hello") == 0) {

        response = http_create_response(
            200,
            "Hello from /hello!"
        );

    } else {

        response = http_create_response(
            404,
            "Not Found"
        );
    }

    if (response != NULL) {

        send(
            client_fd,
            response,
            strlen(response),
            0
        );

        http_free_response(response);
    }
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

        handle_client(client_fd);

        close(client_fd);

        printf("Client disconnected\n");
    }

    close(server_fd);

    return 0;
}