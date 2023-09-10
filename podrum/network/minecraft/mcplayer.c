#include <podrum/network/minecraft/mcplayer.h>
#include <podrum/network/minecraft/mcpackets.h>
#include <podrum/world/chunk/chunk.h>
#include <math.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/bn.h>

void send_minecraft_packet(binary_stream_t *streams, size_t streams_count, connection_t *connection, raknet_server_t *server, uint8_t compress)
{
	packet_game_t game;
	game.streams = streams;
	game.streams_count = streams_count;
	misc_frame_t frame;
	frame.is_fragmented = 0;
	frame.reliability = RELIABILITY_RELIABLE_ORDERED;
	frame.order_channel = 0;
	frame.stream.buffer = (int8_t *) malloc(0);
	frame.stream.offset = 0;
	frame.stream.size = 0;
	put_packet_game(game, (&(frame.stream)), compress);
	send_raknet_frame(frame, connection->address, server, INTERNAL_THREADED_TO_MAIN);
	free(frame.stream.buffer);
}

void send_network_chunk_publisher_update(minecraft_player_t *player, connection_t *connection, raknet_server_t *server)
{
	binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
	streams[0].buffer = (int8_t *) malloc(0);
	streams[0].size = 0;
	streams[0].offset = 0;
	packet_network_chunk_publisher_update_t network_chunk_publisher_update;
	network_chunk_publisher_update.x = (int32_t) floor(player->x);
	network_chunk_publisher_update.y = (uint32_t) (((int32_t) floor(player->y)) & 0xffffffff);
	network_chunk_publisher_update.z = (int32_t) floor(player->z);
	network_chunk_publisher_update.radius = player->view_distance << 4;
	put_packet_network_chunk_publisher_update(network_chunk_publisher_update, (&(streams[0])), player->protocol);
	send_minecraft_packet(streams, 1, connection, server, 1);
	free(streams[0].buffer);
	free(streams);
}

void send_chunk(chunk_t *chunk, minecraft_player_t *player, connection_t *connection, raknet_server_t *server)
{
	binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
	streams[0].buffer = (int8_t *) malloc(0);
	streams[0].size = 0;
	streams[0].offset = 0;
	packet_level_chunk_t level_chunk;
	level_chunk.x = chunk->x;
	level_chunk.z = chunk->z;
	level_chunk.sub_chunk_count = get_chunk_sub_chunk_send_count(chunk);
	level_chunk.cache_enabled = 0;
	level_chunk.payload.buffer = (int8_t *) malloc(0);
	level_chunk.payload.size = 0;
	level_chunk.payload.offset = 0;
	put_misc_chunk(chunk, level_chunk.sub_chunk_count, &level_chunk.payload);
	put_packet_level_chunk(level_chunk, (&(streams[0])));
	free(level_chunk.payload.buffer);
	send_minecraft_packet(streams, 1, connection, server, 1);
	free(streams[0].buffer);
	free(streams);
}

void send_play_status(int32_t status, connection_t *connection, raknet_server_t *server)
{
	binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
	streams[0].buffer = (int8_t *) malloc(0);
	streams[0].size = 0;
	streams[0].offset = 0;
	packet_play_status_t play_status;
	play_status.status = status;
	put_packet_play_status(play_status, (&(streams[0])));
	send_minecraft_packet(streams, 1, connection, server, 1);
	free(streams[0].buffer);
	free(streams);
}

void teleport(float position_x, float position_y, float position_z, float pitch, float yaw, float head_yaw, minecraft_player_t *player, connection_t *connection, raknet_server_t *server){
	binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
	streams[0].buffer = (int8_t *) malloc(0);
	streams[0].size = 0;
	streams[0].offset = 0;
	packet_move_player_t move_plaer;
	move_plaer.runtime_id = player->entity_runtime_id;
	player->x = move_plaer.position_x = position_x;
	player->y = move_plaer.position_y = position_y;
	player->z = move_plaer.position_z = position_z;
	player->pitch = move_plaer.pitch = pitch;
	player->yaw = move_plaer.yaw = yaw;
	player->head_yaw = move_plaer.head_yaw = head_yaw;
	move_plaer.mode = MOVE_PLAYER_MODE_TELEPORT;
	move_plaer.on_ground = 0;
	move_plaer.teleport_cause = 3;
	move_plaer.teleport_source_entity_type = 0;
	move_plaer.tick = 0;
	put_packet_move_player(move_plaer, (&(streams[0])));
	send_minecraft_packet(streams, 1, connection, server, 1);
	free(streams[0].buffer);
	free(streams);
}

void send_msg(char* message, char* source_name, connection_t *connection, raknet_server_t *server){
	binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
	streams[0].buffer = (int8_t *) malloc(0);
	streams[0].size = 0;
	streams[0].offset = 0;
	packet_text_t text;
	text.type = TEXT_CHAT;
	text.needs_translation = 0;
	text.source_name = source_name;
	text.message = message;
	put_packet_text(text, (&(streams[0])));
	send_minecraft_packet(streams, 1, connection, server, 1);
	free(streams[0].buffer);
	free(streams);
}

void send_chunks(mapping_block_states_t block_states, minecraft_player_t *player, connection_t *connection, raknet_server_t *server)
{
	int32_t air_runtime_id = block_state_to_runtime_id("minecraft:air", 0, block_states);
	int32_t bedrock_runtime_id = block_state_to_runtime_id("minecraft:bedrock", 0, block_states);
	int32_t dirt_runtime_id = block_state_to_runtime_id("minecraft:dirt", 0, block_states);
	int32_t grass_runtime_id = block_state_to_runtime_id("minecraft:grass", 0, block_states);
	int32_t current_x = ((int32_t) floor(player->x)) >> 4;
	int32_t current_z = ((int32_t) floor(player->z)) >> 4;
	int32_t x;
	chunk_t chunk;
	for (x = current_x - player->view_distance; x < (current_x + player->view_distance); ++x) {
		int32_t z;
		for (z = current_z - player->view_distance; z < (current_z + player->view_distance); ++z) {
			chunk = new_chunk(x, z, air_runtime_id);

			// temp auto generate chunk
			int32_t xx;
			for (xx = 0; xx < 16; ++xx) {
				int32_t zz;
				for (zz = 0; zz < 16; ++zz) {
					set_chunk_block_runtime_id(xx, 0, zz, bedrock_runtime_id, 0, &chunk);
					set_chunk_block_runtime_id(xx, 1, zz, dirt_runtime_id, 0, &chunk);
					set_chunk_block_runtime_id(xx, 2, zz, dirt_runtime_id, 0, &chunk);
					set_chunk_block_runtime_id(xx, 3, zz, grass_runtime_id, 0, &chunk);
				}
			}
			send_chunk(&chunk, player, connection, server);
			destroy_chunk(&chunk);
		}
	}
	send_network_chunk_publisher_update(player, connection, server);
}
