#ifndef _NET_H
#define _NET_H

#include <uv.h>
#include <stdbool.h>

typedef struct {
    union {
        uv_stream_t stream;
        uv_handle_t handle;
        uv_tcp_t tcp;
    };
} socket_wrapper_t;

typedef void (*receive_packet_callback_t)(socket_wrapper_t *socket, void *data, int len);
typedef void (*server_accept_client_callback_t)(socket_wrapper_t *socket);
typedef void (*server_client_disconnect_callback_t)(socket_wrapper_t *socket);

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} request_t;

typedef enum {
    DISCONNECTED = 0,
    CONNECTING,
    CONNECTED,
} client_status_t;

typedef struct client_s client_t;

typedef void (*client_connect_callback_t)(client_t *client, int status);
typedef void (*client_disconnect_callback_t)(client_t *client);

struct client_s {
    socket_wrapper_t socket;
    client_status_t status;
    uv_connect_t connect;
    receive_packet_callback_t packet_cb;
    client_disconnect_callback_t disconnect_cb;
    char *packet_read_buffer;
    unsigned int packet_read_buffer_size, packet_read_buffer_index;
};

typedef socket_wrapper_t** socket_list_t;

typedef struct {
    socket_wrapper_t socket;
    socket_list_t client_sockets;
    bool listening;
    receive_packet_callback_t packet_cb;
    server_accept_client_callback_t accept_cb;
    server_client_disconnect_callback_t disconnect_cb;
    char *packet_read_buffer;
    unsigned int packet_read_buffer_size, packet_read_buffer_index;
} server_t;

int packet_send(socket_wrapper_t *socket, void *data, unsigned short length);

int init_socket(socket_wrapper_t *socket);
int close_socket(socket_wrapper_t *socket);

int init_server(server_t *server, receive_packet_callback_t cb);
int close_server(server_t *server);
int open_server(server_t *server, const char *ip, int port, server_accept_client_callback_t accept_cb, server_client_disconnect_callback_t disconnect_cb);

int init_client(client_t *client, receive_packet_callback_t cb);
int close_client(client_t *client);
int connect_client(client_t *client, const char *ip, int port, client_connect_callback_t connect_cb, client_disconnect_callback_t disconnect_cb);
int client_packet_send(client_t *client, void *data, unsigned short length);

#endif