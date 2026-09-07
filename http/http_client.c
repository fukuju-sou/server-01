#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http_client.h"
#include "http_parser.h"
#include "http_response.h"

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
     * マルチスレッド動作確認用
     */
    sleep(5);

    printf(
        "[Thread %lu] END\n",
        thread_id
    );

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
        close(client_fd);
        return NULL;
    }

    if (received == 0) {
        close(client_fd);
        return NULL;
    }

    /*
     * 以下は既存のHTTP処理
     */

    // ...

    close(client_fd);

    return NULL;
}
