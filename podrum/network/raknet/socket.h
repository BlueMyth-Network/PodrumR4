#ifndef PODRUM_NETWORK_RAKNET_SOCKET_H
#define PODRUM_NETWORK_RAKNET_SOCKET_H

#include <podrum/debug.h>
#include <podrum/network/raknet/rakmisc.h>
#include <cbinarystream/binary_stream.h>

typedef struct {
	binary_stream_t stream;
	misc_address_t address;
} socket_data_t;

int set_nb_socket(int fd, int is_nb);

int create_socket(misc_address_t address);

socket_data_t receive_data(int sock);

void send_data(int sock, socket_data_t socket_data);

void close_socket(int sock);

#endif