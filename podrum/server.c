#include <podrum/debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <podrum/misc/logger.h>
#include <podrum/command/commandmanager.h>
#include <podrum/network/raknet/rakserver.h>
#include <podrum/worker.h>
#include <podrum/network/minecraft/mcpackets.h>
#include <czlibhelper/zlib_helper.h>
#include <podrum/misc/json.h>
#include <podrum/misc/base64.h>
#include <openssl/evp.h>
#include <podrum/misc/jwt.h>
#include <podrum/misc/uuid.h>
#include <podrum/misc/resourcemanager.h>
#include <podrum/network/minecraft/mcplayermanager.h>
#include <podrum/network/minecraft/mchandler.h>
#include <podrum/network/minecraft/mcplayer.h>
#include <podrum/world/chunk/chunk.h>
#include <cnbt/nbt.h>

#ifdef _WIN32

#include <windows.h>

#else

#include <unistd.h>

#endif

#define CODE_NAME "Carbon"
#define API_VERSION "1.0.0-alpha1"

static int PODRUM_TPS = 1000 / 90; //90tps

minecraft_player_manager_t player_manager;

resources_t resources;

raknet_server_t raknet_server;

uint8_t is_running;

void cmd1executor(int argc, char **argv)
{
	log_info("Function called!");
}

RETURN_WORKER_EXECUTOR command_task(ARGS_WORKER_EXECUTOR argvp)
{
	char *word = malloc(0);
	size_t size = 0;
	while (is_running == 1) {
		char letter;
		scanf("%c", &letter);
		if (letter == '\r') continue;
		if (letter == '\n') {
			letter = 0;
		}
		++size;
		word = (char *) realloc(word, size);
		word[size - 1] = letter;
		if (letter == 0) {
			if (strcmp(word, "help") == 0) {
				log_info("help - Displays all available commands");
				log_info("stop - Stops the server");
			} else if (strcmp(word, "stop") == 0) {
				log_info("Stopping server...");
				is_running = 0;
			} else if (size > 1) {
				log_error("Invalid command!");
			}
			size = 0;
			word = realloc(word, 0);
		}
	}
	return 0;
}

void on_nic(connection_t *connection) {
	int size = snprintf(NULL, 0, "%s:%d connected!", connection->address.address, connection->address.port);
	char *out = (char *) malloc(size + 1);
	sprintf(out, "%s:%d connected!", connection->address.address, connection->address.port);
	out[size] = 0x00;
	log_info(out);
	free(out);
}

void on_dn(misc_address_t address) {
	remove_minecraft_player(address, &player_manager);
	int size = snprintf(NULL, 0, "%s:%d disconnected.", address.address, address.port);
	char *out = (char *) malloc(size + 1);
	sprintf(out, "%s:%d disconnected.", address.address, address.port);
	out[size] = 0x00;
	log_info(out);
	free(out);
}

void boardcast_packet(binary_stream_t *streams, size_t streams_count, raknet_server_t *server, minecraft_player_manager_t *player_manager){
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
	put_packet_game(game, (&(frame.stream)), 1);
	size_t i;
	for (i = 0; i < player_manager->size; ++i) {
		if(player_manager->players[i].spawned == 1){
			send_raknet_frame(frame, player_manager->players[i].address, server, INTERNAL_THREADED_TO_MAIN);
		}
	}
	free(frame.stream.buffer);
}

void on_f(misc_frame_t frame, connection_t *connection, raknet_server_t *server) {
	if ((frame.stream.buffer[0] & 0xFF) == ID_GAME) {
		packet_game_t game;
		if(connection->encryption_enabled){
			printf("en\n");
			if(frame.stream.size < 9){
				printf("weird\n");
			}
			//TODO:decryption
		}else{
			game = get_packet_game(&frame.stream);
		}
		size_t i;
		for (i = 0; i < game.streams_count; ++i) {
			uint32_t header = get_var_int(&game.streams[i]);
			uint32_t pid = header & 0x3ff;
			printf("inbound -> %d\n", pid);
			uint32_t sender_sub_id = (header >> 10) & 0x03;
		    uint32_t recipient_sub_id = (header >> 12) & 0x03;
			if (pid == ID_REQUEST_NETWORK_SETTINGS){
				packet_request_network_setting_t network_request = get_packet_request_network_setting(&(game.streams[i]));
				size_t streams_count = 1;
				binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
				streams[0].buffer = (int8_t *) malloc(0);
				streams[0].size = 0;
				streams[0].offset = 0;
				if(network_request.protocol_version >= 594){
					put_packet_network_setting((&(streams[0])));
				    minecraft_player_t player;
					player.protocol = network_request.protocol_version;
					player.display_name = (char *) malloc(1);
					player.display_name[0] = 0;
					player.identity = (char *) malloc(1);
					player.identity[0] = 0;
					player.xuid = (char *) malloc(1);
					player.xuid[0] = 0;
					player.title_id = (char *) malloc(1);
					player.title_id[0] = 0;
					player.uuid = (unsigned char *) malloc(1);
					player.uuid[0] = 0;
					player.gamemode = 1;
					player.view_distance = 8;
					size_t size = strlen(connection->address.address) + 1;
					player.address.address = malloc(size);
					memcpy(player.address.address, connection->address.address, size);
					player.address.port = connection->address.port;
					player.address.version = connection->address.version;
					player.x = 0.0;
					player.y = -56.0;
					player.z = 0.0;
					player.pitch = 0.0;
					player.yaw = 0.0;
					player.head_yaw = 0.0;
					player.spawned = 0;
					srand(time(NULL));
					player.entity_unique_id = -rand();
					while (has_minecraft_player_entity_unique_id(player.entity_unique_id, &player_manager)) {
						srand(time(NULL));
						player.entity_unique_id = -rand();
					}
					player.entity_runtime_id = rand();
					while (has_minecraft_player_entity_runtime_id(player.entity_runtime_id, &player_manager)) {
						srand(time(NULL));
						player.entity_runtime_id = rand();
					}
					add_minecraft_player(player, &player_manager);
				}else{
					packet_play_status_t play_status;
	                play_status.status = network_request.protocol_version < 594 ? PLAY_STATUS_FAILED_CLIENT : PLAY_STATUS_FAILED_SERVER;
					put_packet_play_status(play_status, (&(streams[0])));
				}
				send_minecraft_packet(streams, streams_count, connection, server, 0);
				free(streams[0].buffer);
				free(streams);
			} else if (pid == ID_LOGIN) {
				handle_packet_login((&(game.streams[i])), connection, server, &player_manager);
			} else {
				minecraft_player_t *player = get_minecraft_player_address(connection->address, &player_manager);
				if (player == NULL) {
					break;
				}
				if (pid == ID_CLIENT_TO_SERVER_HANDSHAKE) {
					if(connection->encryption_enabled){
						printf("encryption success!!!\n");
						play_success(connection, server, player);
					}
				} else if (pid == ID_RESOURCE_PACK_CLIENT_RESPONSE) {
					packet_resource_pack_client_response_t resource_pack_client_response = get_packet_resource_pack_client_response(((&(game.streams[i]))));//TODO : resource pack and behavier pack support
					if (resource_pack_client_response.response_status == RESOURCE_PACK_CLIENT_RESPONSE_NONE || resource_pack_client_response.response_status == RESOURCE_PACK_CLIENT_RESPONSE_HAVE_ALL_PACKS) {
						size_t streams_count = 1;
						binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
						streams[0].buffer = (int8_t *) malloc(0);
						streams[0].size = 0;
						streams[0].offset = 0;
						packet_resource_pack_stack_t resource_pack_stack;
						resource_pack_stack.must_accept = 0;
						resource_pack_stack.behavior_packs.size = 0;
						resource_pack_stack.resource_packs.size = 0;
						resource_pack_stack.game_version = "*";
						resource_pack_stack.experiments.size = 0;
						resource_pack_stack.experiments_previously_used = 0;
						put_packet_resource_pack_stack(resource_pack_stack, (&(streams[0])));
						send_minecraft_packet(streams, streams_count, connection, server, 1);
						free(streams[0].buffer);
						free(streams);
					} else if (resource_pack_client_response.response_status == RESOURCE_PACK_CLIENT_RESPONSE_COMPLETED) {
						size_t streams_count = 4;
						binary_stream_t *streams = (binary_stream_t *) malloc(streams_count * sizeof(binary_stream_t));
						streams[0].buffer = (int8_t *) malloc(0);
						streams[0].size = 0;
						streams[0].offset = 0;
						streams[1].buffer = (int8_t *) malloc(0);
						streams[1].size = 0;
						streams[1].offset = 0;
						streams[2].buffer = (int8_t *) malloc(0);
						streams[2].size = 0;
						streams[2].offset = 0;
						streams[3].buffer = (int8_t *) malloc(0);
						streams[3].size = 0;
						streams[3].offset = 0;
						packet_start_game_t start_game;
						start_game.entity_unique_id = player->entity_unique_id;
						start_game.entity_runtime_id = player->entity_runtime_id;
						start_game.player_gamemode = player->gamemode;
						start_game.player_x = 0.0;
						start_game.player_y = -56.0;
						start_game.player_z = 0.0;
						start_game.pitch = 0.0;
						start_game.yaw = 0.0;
						start_game.seed = -1;
						start_game.biome_type = 0;
						start_game.biome_name = "";
						start_game.dimension = 0;
						start_game.generator = 0;
						start_game.world_gamemode = 2;
						start_game.difficulty = -1;
						start_game.spawn_x = 0;
						start_game.spawn_y = 4;
						start_game.spawn_z = 0;
						start_game.achievements_disabled = 1;
						start_game.day_cycle_stop_time = 0;
						start_game.edu_offer = 0;
						start_game.edu_features_enabled = 0;
						start_game.edu_product_uuid = "";
						start_game.rain_level = 0.0;
						start_game.lightning_level = 0.0;
						start_game.has_confirmed_platform_locked_content = 0;
						start_game.is_multiplayer = 1;
						start_game.broadcast_to_lan = 1;
						start_game.xbox_live_broadcast_mode = 4;
						start_game.platform_broadcast_mode = 4;
						start_game.enable_commands = 1;
						start_game.are_texture_packs_required = 0;
						start_game.gamerules.size = 0;
						start_game.experiments.size = 7;
						misc_experiment_t experiments[7];
						experiments[0].name = "cameras";
						experiments[0].enabled = 1;
						experiments[1].name = "data_driven_biomes";
						experiments[1].enabled = 1;
						experiments[2].name = "data_driven_items";
						experiments[2].enabled = 1;
						experiments[3].name = "experimental_molang_features";
						experiments[3].enabled = 1;
						experiments[4].name = "gametest";
						experiments[4].enabled = 1;
						experiments[5].name = "short_sneaking";
						experiments[5].enabled = 1;
						experiments[6].name = "upcoming_creator_features";
						experiments[6].enabled = 1;
						start_game.experiments.entries = experiments;
						start_game.experiments_previously_used = 1;
						start_game.bonus_chest = 0;
						start_game.map_enabled = 0;
						start_game.permission_level = -1;
						start_game.server_chunk_tick_range = 0;
						start_game.has_locked_behavior_pack = 0;
						start_game.has_locked_texture_pack = 0;
						start_game.is_from_locked_world_template = 0;
						start_game.msa_gamertags_only = 0;
						start_game.is_from_world_template = 0;
						start_game.is_world_template_option_locked = 0;
						start_game.only_spawn_v1_villagers = 0;
						start_game.game_version = "*";
						start_game.limited_world_width = 16;
						start_game.limited_world_length = 16;
						start_game.is_new_nether = 0;
						start_game.edu_resource_uri.button_name = "";
						start_game.edu_resource_uri.link_uri = "";
						start_game.experimental_gameplay_override = 0;
						start_game.level_id = "";
						start_game.world_name = "BlueMyth Games";
						start_game.premium_world_template_id = "";
						start_game.is_trial = 0;
						start_game.movement_authority = 0;
						start_game.rewind_history_size = 0;
						start_game.server_authoritative_block_breaking = 0;
						start_game.current_tick = 0;
						start_game.enchantment_seed = 0;
						start_game.block_properties.size = 0;//TODO: customblock
						start_game.item_states = resources.item_states;
						start_game.multiplayer_correlation_id = "";
						start_game.server_authoritative_inventory = 0;
						start_game.engine = GAME_ENGINE;
						start_game.block_pallete_checksum = 0;
						put_packet_start_game(start_game, (&(streams[0])));
						packet_creative_content_t creative_content;
						creative_content.size = resources.creative_items.size;
						creative_content.entry_ids = (uint32_t *) malloc(creative_content.size * sizeof(uint32_t));
						size_t ii;
						for (ii = 0; ii < creative_content.size; ++ii) {
							creative_content.entry_ids[i] = i + 1;
						}
						creative_content.items = resources.creative_items.entries;
						put_packet_creative_content(creative_content, (&(streams[1])));//TODO: custom item
						free(creative_content.entry_ids);
						packet_biome_definition_list_t biome_definition_list;
						biome_definition_list.nbt = resources.biome_definitions;
						put_packet_biome_definition_list(biome_definition_list, (&(streams[2])));//TODO: custom biome
						packet_available_entity_identifiers_t available_entity_identifiers;
						available_entity_identifiers.nbt = resources.entity_identifiers;
						put_packet_available_entity_identifiers(available_entity_identifiers, (&(streams[3])));//TODO: custom entity
						send_minecraft_packet(streams, streams_count, connection, server, 1);
						//TODO: add itemcomponent stuff (custom item)
						free(streams[0].buffer);
						free(streams[1].buffer);
						free(streams[2].buffer);
						free(streams[3].buffer);
						free(streams);
					}
					int16_t ii;
					for (ii = 0; ii < resource_pack_client_response.resource_pack_ids.size; ++ii) {
						free(resource_pack_client_response.resource_pack_ids.ids[i]);
					}
					free(resource_pack_client_response.resource_pack_ids.ids);
				} else if (pid == ID_REQUEST_CHUNK_RADIUS) {
					handle_packet_request_chunk_radius((&(game.streams[i])), connection, server, player, &resources);
				} else if (pid == ID_INTERACT) {
					handle_packet_interact((&(game.streams[i])), connection, server, player, &resources);
				} else if (pid == ID_TEXT) {
					packet_text_t text = get_packet_text(&(game.streams[i]));
					teleport(player->x, player->x+10, player->z, player->pitch, player->yaw, player->head_yaw, player, connection, server);
					binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
					streams[0].buffer = (int8_t *) malloc(0);
					streams[0].size = 0;
					streams[0].offset = 0;
					put_packet_text(text, (&(streams[0])));
					boardcast_packet(streams, 1, server, &player_manager);
				} else if (pid == ID_CONTAINER_CLOSE) {
					handle_packet_window_close((&(game.streams[i])), connection, server);
				} else if (pid == ID_MOVE_PLAYER) {
					handle_packet_move_player((&(game.streams[i])), connection, server, player, &resources);
				} else if (pid == ID_REQUEST_ABILITY) {
					packet_request_ability_t request_ability = get_packet_request_ability(&(game.streams[i]));
					switch (request_ability.ability_id){
						case REQUEST_ABILITY_FLYING://???
							printf("player try to fly\n");
							break;
						default:
							break;
					}
				} else if(pid == ID_PLAYER_SKIN){
					binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
					streams[0].buffer = (int8_t *) malloc(0);
					streams[0].size = 0;
					streams[0].offset = 0;
					packet_player_skin_t player_skin = get_packet_player_skin(&game.streams[i]);
					put_packet_player_skin(player_skin, &streams[0]);
					boardcast_packet(streams, 1, server, &player_manager);
				}
			}
			free(game.streams[i].buffer);
		}
		free(game.streams);
	}else{
		send_raknet_disconnect_notification(connection->address, server, INTERNAL_THREADED_TO_MAIN);
	}
}

EVP_PKEY* make_server_key(){
	EVP_PKEY_CTX *pctx = NULL;
    EVP_PKEY *pkey = NULL;

    pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    if (pctx == NULL) {
        return pkey;
    }
    if (EVP_PKEY_keygen_init(pctx) <= 0) {
        EVP_PKEY_CTX_free(pctx);
        return pkey;
    }
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, OBJ_txt2nid(BEDROCK_SIGNING_KEY_CURVE_NAME)) <= 0) {
        EVP_PKEY_CTX_free(pctx);
        return pkey;
    }
    if (EVP_PKEY_keygen(pctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(pctx);
        return pkey;
    }
    EVP_PKEY_CTX_free(pctx);
	return pkey;
}


int main(int argc, char **argv)
{
	is_running = 1;
	#ifdef _WIN32
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dw_mode = 0;
	GetConsoleMode(handle, &dw_mode);
	dw_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(handle, dw_mode);
	#endif
	resources = get_resources();
	player_manager.size = 0;
	player_manager.players = (minecraft_player_t *) malloc(0);
	OpenSSL_add_all_algorithms();
	raknet_server = create_raknet_server(1, "0.0.0.0", 19132, 4, on_f, on_nic, on_dn, make_server_key());
	run_raknet_server(&raknet_server);
	send_set_raknet_option("name", "MCPE;Hive Games;120;1.19.80;0;100;13253860892328930865;normal;Survival;1;19132;19133;", &raknet_server);
	command_manager_t command_manager;
	command_manager.commands = (command_t *) malloc(0);
	command_manager.commands_count = 0;
	create_worker(command_task, NULL);
	log_info("Podrum started up!");
	while (is_running == 1) {
		#ifdef _WIN32
		Sleep(PODRUM_TPS);
		#else
		usleep(PODRUM_TPS * 1000);
		#endif
	}
	send_raknet_shutdown(&raknet_server);
	join_worker(raknet_server.main_worker);
	destroy_resources(&resources);
	free(player_manager.players);
	free(command_manager.commands);
	log_info("Server stopped.");
	return 0;
}