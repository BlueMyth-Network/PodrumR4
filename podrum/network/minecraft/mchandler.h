#ifndef PODRUM_NETWORK_MINECRAFT_MCHANDLER_H
#define PODRUM_NETWORK_MINECRAFT_MCHANDLER_H

#define MOJANG_ROOT_KEY "MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE8ELkixyLcwlZryUQcu1TvPOmI2B7vX83ndnWRUaXm74wFfa5f/lwQNTfrLVHa2PmenpGI6JhIMUJaWZrjmMj90NoKNFSNBuKdm8rYiXsfaz3K36x/1U26HpG0ZxK/V1V"

#include <podrum/debug.h>
#include <podrum/network/raknet/rakserver.h>
#include <podrum/network/minecraft/mcplayermanager.h>
#include <podrum/misc/resourcemanager.h>
#include <podrum/misc/json.h>
#include <podrum/misc/jwt.h>

void handle_packet_login(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_manager_t *player_manager);

uint8_t verifly_jwt_chain(jwt_data_t *jwtchain, unsigned char **current_public_key, size_t *first);

void handle_packet_interact(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_t *player, resources_t *resources);

void handle_packet_window_close(binary_stream_t *stream, connection_t *connection, raknet_server_t *server);

void handle_packet_request_chunk_radius(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_t *player, resources_t *resources);

void handle_packet_move_player(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_t *player, resources_t *resources);

#endif