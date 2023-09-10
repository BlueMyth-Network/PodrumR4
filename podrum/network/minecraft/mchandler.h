#ifndef PODRUM_NETWORK_MINECRAFT_MCHANDLER_H
#define PODRUM_NETWORK_MINECRAFT_MCHANDLER_H

#define MOJANG_ROOT_KEY "MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAECRXueJeTDqNRRgJi/vlRufByu/2G0i2Ebt6YMar5QX/R0DIIyrJMcUpruK4QveTfJSTp3Shlq4Gk34cD/4GUWwkv0DVuzeuB+tXija7HBxii03NHDbPAD0AKnLr2wdAp"

#define BEDROCK_SIGNING_KEY_CURVE_NAME "secp384r1"

#include <podrum/debug.h>
#include <podrum/network/raknet/rakserver.h>
#include <podrum/network/minecraft/mcplayermanager.h>
#include <podrum/misc/resourcemanager.h>
#include <podrum/misc/json.h>
#include <podrum/misc/jwt.h>

void handle_packet_login(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_manager_t *player_manager);

int make_and_send_hand_shake(connection_t *connection, raknet_server_t *server);

void aes_key_setup(connection_t *connection, unsigned char *combined_key);

unsigned char *make_combined_key(unsigned char *player_key, raknet_server_t *server, unsigned char *salt);

void play_success(connection_t *connection, raknet_server_t *server, minecraft_player_t *player);

uint32_t verifly_jwt_chain(jwt_data_t *jwtchain, unsigned char **current_public_key, size_t *first);

void handle_packet_interact(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_t *player, resources_t *resources);

void handle_packet_window_close(binary_stream_t *stream, connection_t *connection, raknet_server_t *server);

void handle_packet_request_chunk_radius(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_t *player, resources_t *resources);

void handle_packet_move_player(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_t *player, resources_t *resources);

#endif