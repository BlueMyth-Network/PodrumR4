

#ifndef PODRUM_NETWORK_RAKNET_RAKHANDLER_H
#define PODRUM_NETWORK_RAKNET_RAKHANDLER_H

#include <podrum/debug.h>
#include <podrum/network/raknet/rakpacket.h>
#include <podrum/network/raknet/rakserver.h>

binary_stream_t handle_unconneted_ping(binary_stream_t *stream, raknet_server_t *server);

binary_stream_t handle_open_connection_request_1(binary_stream_t *stream, raknet_server_t *server);

binary_stream_t handle_open_connection_request_2(binary_stream_t *stream, raknet_server_t *server, misc_address_t address);

binary_stream_t handle_connection_request(binary_stream_t *stream, raknet_server_t *server, misc_address_t address);

binary_stream_t handle_connected_ping(binary_stream_t *stream, raknet_server_t *server);

void handle_ack(binary_stream_t *stream, raknet_server_t *server, connection_t *connection);

void handle_nack(binary_stream_t *stream, raknet_server_t *server, connection_t *connection);

void handle_fragmented_frame(misc_frame_t frame, raknet_server_t *server, connection_t *connection);

void handle_frame(misc_frame_t frame, raknet_server_t *server, connection_t *connection);

void handle_frame_set(binary_stream_t *stream, raknet_server_t *server, connection_t *connection);

#endif
