#include "ae/net.h"

#include "ae/utility.h"
#include "ae/array.h"

#include <stdlib.h>
#include <string.h>

static void stream_write_cb(uv_write_t *req, int status) {
    request_t* r = (request_t*)req;
    free(r->buf.base);
    free(r);
}

int packet_send(socket_wrapper_t *socket, void *data, unsigned short length) {
    request_t *r = malloc(sizeof(request_t));
    r->buf.len = length + sizeof(unsigned short);
    r->buf.base = malloc(r->buf.len);
    //r->buf.base[0] = length;
    memcpy(r->buf.base, &length, sizeof(unsigned short));
    memcpy(r->buf.base + sizeof(unsigned short), data, length);
    return uv_write((uv_write_t*)r, &socket->stream, &r->buf, 1, stream_write_cb);
}

static void alloc_buffer_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    (void)handle;
    *buf = uv_buf_init(malloc(suggested_size), suggested_size);
}

static void server_stream_close_cb(uv_handle_t *handle) {
    unsigned int i;
    server_t *server = uv_handle_get_data(handle);
    for (i = 0; i < arr_len(server->client_sockets); i++) {
        if (&server->client_sockets[i]->handle == handle) {
            server->disconnect_cb(server->client_sockets[i]);
            free(arr_remove_as(server->client_sockets, i, socket_wrapper_t*));
            return;
        }
    }
}

static void server_stream_read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {

    uv_handle_t *handle = (uv_handle_t*)stream;
    server_t *server = uv_handle_get_data(handle);
    if (nread < 0) {
        uv_close(handle, server_stream_close_cb);
    } else if (nread > 0) {
        unsigned int i;
        unsigned short len = 0;
        for (i = 0; i < nread;) {
            memcpy(&len, buf->base + i, sizeof(unsigned short));
            if (len + sizeof(unsigned short) > nread) {
                puterr("NET", "(Server) Incomplete Packet, expected %uB got %ldB.", 0, len, (long)nread);
            }
            else server->packet_cb((socket_wrapper_t*)stream, buf->base + i + sizeof(unsigned short), len);
            i += len + sizeof(unsigned short);
        }
    }

    if (buf->base)
        free(buf->base);
}

static void client_stream_close_cb(uv_handle_t *handle) {
    client_t *client = uv_handle_get_data(handle);
    client->status = DISCONNECTED;
    client->disconnect_cb(client);
}

static void client_stream_read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {

    uv_handle_t *handle = (uv_handle_t*)stream;
    client_t *client = uv_handle_get_data(handle);

    if (nread < 0) {
        uv_close(handle, client_stream_close_cb);
    } else if (nread > 0) {
        unsigned int i;
        unsigned short len = 0;
        for (i = 0; i < nread; ) {
            memcpy(&len, buf->base + i, sizeof(unsigned short));
            if (len + sizeof(unsigned short) > nread) {
                puterr("NET", "(Client) Incomplete Packet, expected %ub got %ldb.", 0, len, (long)nread);
            }
            else client->packet_cb((socket_wrapper_t*)stream, buf->base + i + sizeof(unsigned short), len);
            i += len + sizeof(unsigned short);
        }
    }

    if (buf->base)
        free(buf->base);
}

static void tcp_connection_cb(uv_stream_t *stream, int status) {
    //putlog("SERVER", "received a tcp connection (%d)", status);

    server_t *server = uv_handle_get_data((uv_handle_t*)stream);
    socket_wrapper_t *client = malloc(sizeof(socket_wrapper_t));
    arr_add(server->client_sockets, client);
    uv_tcp_init(uv_default_loop(), &client->tcp);
    if (uv_accept(stream, &client->stream) == 0) {
        //putlog("SERVER", "accepted client");
        uv_handle_set_data(&client->handle, server);
        uv_read_start(&client->stream, alloc_buffer_cb, server_stream_read_cb);
        server->accept_cb(client);
    }
    else {
        uv_close(&client->handle, NULL);
        free(arr_remove_as(server->client_sockets, -1, socket_wrapper_t*));
    }
}

struct client_connect_data {
    client_t *client;
    client_connect_callback_t cb;
};

static void client_connect_cb(uv_connect_t *connect, int status) {
    //putlog("CLIENT", "status %d", status);
    struct client_connect_data *d = connect->data;
    d->client->status = CONNECTED;
    if (connect->handle == NULL) {
        //putlog("NET", "Warning! For some reason the callback returned a NULL handle.");
        connect->handle = &d->client->socket.stream;
    }
    uv_handle_set_data((uv_handle_t*)connect->handle, d->client);
    uv_read_start(connect->handle, alloc_buffer_cb, client_stream_read_cb);
    d->cb(d->client, status);
    free(d);
}

int init_socket(socket_wrapper_t *socket) {
    if (!socket) return -1;
    return uv_tcp_init(uv_default_loop(), &socket->tcp);
}

int close_socket(socket_wrapper_t *socket) {
    if (!socket) return -1;
    uv_close(&socket->handle, NULL);
    return 0;
}

int init_server(server_t *server, receive_packet_callback_t cb) {
    if (!server) return -1;
    server->listening = false;
    server->packet_cb = cb;
    return init_socket(&server->socket);
}

int close_server(server_t *server) {
    if (!server) return -1;
    server->packet_cb = NULL;
    server->accept_cb = NULL;
    server->listening = false;
    arr_delete(server->client_sockets);
    return close_socket(&server->socket);
}

int open_server(server_t *server, const char *ip, int port, server_accept_client_callback_t accept_cb, server_client_disconnect_callback_t disconnect_cb) {
    struct sockaddr_in addr;

    uv_ip4_addr(ip, port, &addr);
    int r = uv_tcp_bind(&server->socket.tcp, (const struct sockaddr*)&addr, 0);
    //if (r) puterr("SERVER", "bind error %s", r, uv_strerror(r));
    uv_handle_set_data(&server->socket.handle, server);
    r = uv_listen(&server->socket.stream, 1024, tcp_connection_cb);
    //if (r) puterr("SERVER", "listen error %s", r, uv_strerror(r));
    server->accept_cb = accept_cb;
    server->disconnect_cb = disconnect_cb;
    server->listening = true;
    server->client_sockets = arr_new(sizeof(socket_wrapper_t*), 1, 1);

    return r;
}

int init_client(client_t *client, receive_packet_callback_t cb) {
    if (!client) return -1;
    client->status = DISCONNECTED;
    client->packet_cb = cb;
    client->connect = (uv_connect_t){0};
    return init_socket(&client->socket);
}

int close_client(client_t *client) {
    if (!client) return -1;
    client->packet_cb = NULL;
    client->disconnect_cb = NULL;
    client->status = DISCONNECTED;
    return close_socket(&client->socket);
}

int connect_client(client_t *client, const char *ip, int port, client_connect_callback_t connect_cb, client_disconnect_callback_t disconnect_cb) {
    struct sockaddr_in addr;
    uv_ip4_addr(ip, port, &addr);
    struct client_connect_data *d = malloc(sizeof(struct client_connect_data));
    client->disconnect_cb = disconnect_cb;
    d->client = client;
    d->cb = connect_cb;
    uv_req_set_data((uv_req_t*)&client->connect, d);
    int r = uv_tcp_connect(&client->connect, &client->socket.tcp, (const struct sockaddr*)&addr, client_connect_cb);
    //if (r) puterr("CLIENT", "connect error %s", r, uv_strerror(r));
    client->status = CONNECTING;
    return r;
}

int client_packet_send(client_t *client, void *data, unsigned short length) {
    if (!client) return -1;
    if (client->status != CONNECTED) return -2;
    return packet_send((socket_wrapper_t*)client->connect.handle, data, length);
}