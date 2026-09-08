#include "http_router.h"

#include <stdio.h>
#include <string.h>

static int handle_index(
    const HttpRequest *request,
    char *body,
    size_t body_size
)
{
    (void)request;

    snprintf(
        body,
        body_size,
        "Welcome to HTTP Server\n"
    );

    return 200;
}

static int handle_hello(
    const HttpRequest *request,
    char *body,
    size_t body_size
)
{
    (void)request;

    snprintf(
        body,
        body_size,
        "Hello, World!\n"
    );

    return 200;
}

static int handle_users(
    const HttpRequest *request,
    char *body,
    size_t body_size
)
{
    (void)request;

    snprintf(
        body,
        body_size,
        "GET /users\n"
    );

    return 200;
}

static int handle_create_user(
    const HttpRequest *request,
    char *body,
    size_t body_size
)
{
    snprintf(
        body,
        body_size,
        "POST /users\n"
        "Body: %s\n",
        request->body
    );

    return 200;
}

void router_init(HttpRouter *router)
{
    if (router == NULL) {
        return;
    }

    memset(router, 0, sizeof(HttpRouter));

    router_add(
        router,
        "GET",
        "/",
        handle_index
    );

    router_add(
        router,
        "GET",
        "/hello",
        handle_hello
    );

    router_add(
        router,
        "GET",
        "/users",
        handle_users
    );

    router_add(
        router,
        "POST",
        "/users",
        handle_create_user
    );
}

int router_add(
    HttpRouter *router,
    const char *method,
    const char *path,
    HttpHandler handler
)
{
    if (router == NULL ||
        method == NULL ||
        path == NULL ||
        handler == NULL) {

        return -1;
    }

    if (router->count >= HTTP_ROUTER_MAX_ROUTES) {
        return -1;
    }

    snprintf(
        router->routes[router->count].method,
        HTTP_METHOD_SIZE,
        "%s",
        method
    );

    snprintf(
        router->routes[router->count].path,
        HTTP_PATH_SIZE,
        "%s",
        path
    );

    router->routes[router->count].handler = handler;

    router->count++;

    return 0;
}

int router_dispatch(
    const HttpRouter *router,
    const HttpRequest *request,
    char *body,
    size_t body_size
)
{
    if (router == NULL ||
        request == NULL ||
        body == NULL ||
        body_size == 0) {

        return 500;
    }

    /*
     * まずMethod + Pathが完全一致するRouteを探す。
     */
    for (size_t i = 0; i < router->count; i++) {

        const HttpRoute *route = &router->routes[i];

        if (strcmp(route->method, request->method) == 0 &&
            strcmp(route->path, request->path) == 0) {

            return route->handler(
                request,
                body,
                body_size
            );
        }
    }

    /*
     * Pathは存在するがMethodが違う場合。
     */
    for (size_t i = 0; i < router->count; i++) {

        const HttpRoute *route = &router->routes[i];

        if (strcmp(route->path, request->path) == 0) {
            return 405;
        }
    }

    /*
     * Pathそのものが存在しない。
     */
    return 404;
}
