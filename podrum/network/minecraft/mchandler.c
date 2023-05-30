
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <stdbool.h>
#include <ecc/ecc.h>
#include <podrum/network/minecraft/mchandler.h>
#include <podrum/network/minecraft/mcpackets.h>
#include <podrum/misc/logger.h>
#include <podrum/network/minecraft/mcmisc.h>
#include <podrum/network/minecraft/mcplayer.h>
#include <podrum/misc/json.h>
#include <podrum/misc/jwt.h>
#include <podrum/misc/base64.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

void handle_packet_login(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_manager_t *player_manager)
{
	packet_login_t login = get_packet_login(stream);
	//TODO: check protocol
	minecraft_player_t player;
	player.protocol = login.protocol_version;
	player.display_name = (char *) malloc(1);
	player.display_name[0] = 0;
	player.identity = (char *) malloc(1);
	player.identity[0] = 0;
	player.xuid = (char *) malloc(1);
	player.xuid[0] = 0;
	player.title_id = (char *) malloc(1);
	player.title_id[0] = 0;
	player.gamemode = 1;
	player.view_distance = 8;
	size_t size = strlen(connection->address.address) + 1;
	player.address.address = malloc(size);
	memcpy(player.address.address, connection->address.address, size);
	player.address.port = connection->address.port;
	player.address.version = connection->address.version;
	player.x = 0.0;
	player.y = 9.0;
	player.z = 0.0;
	player.pitch = 0.0;
	player.yaw = 0.0;
	player.spawned = 0;
	srand(time(NULL));
	player.entity_id = rand();
	while (has_minecraft_player_entity_id(player.entity_id, player_manager)) {
		srand(time(NULL));
		player.entity_id = rand();
	}
	json_input_t json_input;
	json_input.json = login.tokens.identity;
	json_input.offset = 0;
	json_root_t json_root = parse_json_root(&json_input);
	json_root_t chain = get_json_object_value("chain", json_root.entry.json_object);
	unsigned char *current_public_key = NULL;
	size_t i;
	for (i = 0; i < chain.entry.json_array.size; ++i) {
		jwt_data_t jwtchain = test_jwt_decode(get_json_array_value(i, chain.entry.json_array).entry.json_string);
		verifly_jwt_chain(&jwtchain, &current_public_key, &i);
	}
	destroy_json_root(json_root);
	free(login.tokens.identity);
	json_object_t client_data = test_jwt_decode(login.tokens.client).payload.entry.json_object;
	verifly_jwt_chain(&client_data, &current_public_key, &i);
	// for (size_t i = 0; i < client_data.size; i++){
	// 	printf("test %s \n", client_data.keys[i]);
	// }
	destroy_json_object(client_data);
	free(login.tokens.client);
	add_minecraft_player(player, player_manager);
	send_play_status(PLAY_STATUS_LOGIN_SUCCESS, connection, server);
	binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
	streams[0].buffer = (int8_t *) malloc(0);
	streams[0].size = 0;
	streams[0].offset = 0;
	packet_resource_packs_info_t resource_packs_info;
	resource_packs_info.must_accept = 0;
	resource_packs_info.has_scripts = 0;
	resource_packs_info.force_server_packs = 0;
	resource_packs_info.behavior_packs.size = 0;
	resource_packs_info.texture_packs.size = 0;
	put_packet_resource_packs_info(resource_packs_info, (&(streams[0])));
	send_minecraft_packet(streams, 1, connection, server, 1);
	free(streams[0].buffer);
	free(streams);
	printf("%s logged in with entity id %lld\n", player.display_name, player.entity_id);
}

uint8_t verifly_jwt_chain(jwt_data_t *jwtchain, unsigned char **current_public_key, size_t *first){
	uint8_t status = -11;
	int64_t timern = (int64_t) time(NULL);
	json_object_t header_entry = jwtchain->header.entry.json_object;
	json_root_t x5u = get_json_object_value("x5u", header_entry);
	if(x5u.type != JSON_STRING){
		return -11;
	}
	if(strcmp(x5u.entry.json_string, MOJANG_ROOT_KEY) == 0){
		status = 1;
	}
	char *raw_public_key = x5u.entry.json_string;
	size_t out_len;//120
	unsigned char *header_der_key = char_base64_decode((unsigned char *)raw_public_key, strlen(raw_public_key), &out_len);
	if(*current_public_key == NULL){
		if(*first != 0){
			return -10;
		}
	}else if(strcmp((char *)header_der_key, (char *)*current_public_key) != 0){
		return -9;
	}
	json_object_t payload_entry = jwtchain->payload.entry.json_object;
	size_t signature_size;//96
	unsigned char *raw_signature = char_base64_url_decode((unsigned char *)jwtchain->signature, strlen(jwtchain->signature), &signature_size);
	if(signature_size != 96){
		return -8;
	}

	//TODO: hardest part (verify jwt)

	json_root_t identity_public_key = get_json_object_value("identityPublicKey", payload_entry);
	if (identity_public_key.type != JSON_STRING) {
		destroy_json_root(identity_public_key);
		return -7;
	}
	int64_t exp = get_json_object_value("exp", payload_entry).entry.json_number.number.int_number;
	int64_t nbf = get_json_object_value("nbf", payload_entry).entry.json_number.number.int_number;
	if(nbf > timern + 60 || exp < timern - 60){
		destroy_json_root(identity_public_key);
		return -6;
	}
	char *raw_identity_public_key = identity_public_key.entry.json_string;
	*current_public_key = char_base64_decode((unsigned char *)raw_identity_public_key, strlen(raw_identity_public_key), &out_len);
	return status;
}

void handle_packet_interact(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_t *player, resources_t *resources)
{
	packet_interact_t interact = get_packet_interact(stream);
	if (interact.action_id == INTERACT_OPEN_INVENTORY) {
		binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
		streams[0].buffer = (int8_t *) malloc(0);
		streams[0].size = 0;
		streams[0].offset = 0;
		packet_container_open_t container_open;
		switch (player->gamemode) {
		case GAMEMODE_SURVIVAL:
		case GAMEMODE_ADVENTURE:
		case GAMEMODE_SURVIVAL_SPECTATOR:
		case GAMEMODE_FALLBACK:
			container_open.window_id = WINDOW_ID_INVENTORY;
			break;
		case GAMEMODE_CREATIVE:
		case GAMEMODE_CREATIVE_SPECTATOR:
			container_open.window_id = WINDOW_ID_CREATIVE;
			break;
		default:
			log_warning("Invalid Gamemode");
			send_raknet_disconnect_notification(connection->address, server, INTERNAL_THREADED_TO_MAIN);
		}
		container_open.window_type = WINDOW_TYPE_INVENTORY;
		container_open.coordinates_x = (int32_t) player->x;
		container_open.coordinates_y = (uint32_t) (((int32_t) player->y) & 0xffffffff);
		container_open.coordinates_z = (int32_t) player->z;
		container_open.runtime_entity_id = player->entity_id;
		put_packet_container_open(container_open, (&(streams[0])));
		send_minecraft_packet(streams, 1, connection, server, 1);
		free(streams[0].buffer);
		free(streams);
	}
}

void handle_packet_window_close(binary_stream_t *stream, connection_t *connection, raknet_server_t *server)
{
	packet_container_close_t container_close_in = get_packet_container_close(stream);
	binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
	streams[0].buffer = (int8_t *) malloc(0);
	streams[0].size = 0;
	streams[0].offset = 0;
	packet_container_close_t container_close_out;
	container_close_out.window_id = container_close_in.window_id;
	container_close_out.server = 0;
	put_packet_container_close(container_close_out, (&(streams[0])));
	send_minecraft_packet(streams, 1, connection, server, 1);
	free(streams[0].buffer);
	free(streams);
}

void handle_packet_request_chunk_radius(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_t *player, resources_t *resources)
{
	if (player->spawned == 1) return;
	binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
	streams[0].buffer = (int8_t *) malloc(0);
	streams[0].size = 0;
	streams[0].offset = 0;
	packet_request_chunk_radius_t request_chunk_radius = get_packet_request_chunk_radius(stream);
	packet_chunk_radius_updated_t chunk_radius_updated;
	chunk_radius_updated.chunk_radius = (int32_t) fmin((double) request_chunk_radius.chunk_radius, 8.0); /* server_chunk_radius = 8 */
	put_packet_chunk_radius_updated(chunk_radius_updated, (&(streams[0])));
	player->view_distance = chunk_radius_updated.chunk_radius;
	send_minecraft_packet(streams, 1, connection, server, 1);
	free(streams[0].buffer);
	free(streams);
	send_chunks(resources->block_states, player, connection, server);
	send_play_status(PLAY_STATUS_PLAYER_SPAWN, connection, server);
	player->spawned = 1;
}

void handle_packet_move_player(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_t *player, resources_t *resources)
{
	packet_move_player_t move_player = get_packet_move_player(stream);
	if (player->spawned == 1) {
		if (floor(floor(player->x) / 16.0) != floor(floor(move_player.position_x) / 16.0) || floor(floor(player->z) / 16.0) != floor(floor(move_player.position_z) / 16)) {
			send_chunks(resources->block_states, player, connection, server);
		}
	}
	player->x = move_player.position_x;
	player->y = move_player.position_y;
	player->z = move_player.position_z;
	player->pitch = move_player.pitch;
	player->yaw = move_player.yaw;
}
