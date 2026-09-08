#ifndef HTTP_ROUTER_H
#define HTTP_ROUTER_H

#include <stddef.h>

#include "http_parser.h"

#define HTTP_ROUTER_MAX_ROUTES 64
#define HTTP_ROUTER_BODY_SIZE 4096

typedef int (*HttpHandler)(
    const HttpRequest *request,
    char *body,
    size_t body_size
);

typedef struct {
    char method[HTTP_METHOD_SIZE];
    char path[HTTP_PATH_SIZE];
    HttpHandler handler;
} HttpRoute;

typedef struct {
    HttpRoute routes[HTTP_ROUTER_MAX_ROUTES];
    size_t count;
} HttpRouter;

/*
 * Routerを初期化する。
 */
void router_init(HttpRouter *router);

/*
 * Routeを登録する。
 *
 * 例:
 * router_add(router, "GET", "/", handle_index);
 */
int router_add(
    HttpRouter *router,
    const char *method,
    const char *path,
    HttpHandler handler
);

/*
 * HTTPリクエストに対応するHandlerを実行する。
 *
 * 戻り値:
 *   200 = Handler実行成功
 *   404 = Pathが存在しない
 *   405 = Pathは存在するがMethodが違う
 */
int router_dispatch(
    const HttpRouter *router,
    const HttpRequest *request,
    char *body,
    size_t body_size
);

#endif
