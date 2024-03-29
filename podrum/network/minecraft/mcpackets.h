#ifndef PODRUM_NETWORK_MINECRAFT_MCPACKETS_H
#define PODRUM_NETWORK_MINECRAFT_MCPACKETS_H

#include <podrum/debug.h>
#include <cbinarystream/binary_stream.h>
#include <stdlib.h>
#include <podrum/network/minecraft/mcmisc.h>
#include <cnbt/nbt.h>

#define GAME_VERSION "1.20.0"
#define SUPPORT_PROTOCOL {589}
#define GAME_ENGINE "BlueSky v0.0.1"

#define ID_LOGIN 0x01
#define ID_PLAY_STATUS 0x02
#define ID_SERVER_TO_CLIENT_HANDSHAKE 0x03
#define ID_CLIENT_TO_SERVER_HANDSHAKE 0x04
#define ID_DISCONNECT 0x05
#define ID_RESOURCE_PACKS_INFO 0x06
#define ID_RESOURCE_PACK_STACK 0x07
#define ID_RESOURCE_PACK_CLIENT_RESPONSE 0x08
#define ID_TEXT 0x09
#define ID_SET_TIME 0x0A
#define ID_START_GAME 0x0B
#define ID_ADD_PLAYER 0x0C //todo :skull:
#define ID_ADD_ENTITY 0x0D //todo :skull:
#define ID_REMOVE_ENTITY 0x0E //todo :skull:
#define ID_ADD_ITEM 0x0F //todo :skull:

#define ID_TAKE_ITEM_ENTITY 0x11 //todo :skull:
#define ID_MOVE_ENTITY 0x12
#define ID_MOVE_PLAYER 0x13
#define ID_PASSENGER_JUMP 0x14 //todo :skull:
#define ID_UPDATE_BLOCK 0x15 //todo :skull:

#define ID_TICK_SYNC 0x17 //todo :skull:

#define ID_LEVEL_EVENT 0x19 //todo :skull:
#define ID_BLOCK_EVENT 0x1A //todo :skull:
#define ID_ENTITY_EVENT 0x1B //todo :skull:
#define ID_ENTITY_EFFECT 0x1C //todo :skull:
#define ID_UPDATE_ATTRIBUTES_PACKET 0x1D

#define ID_INTERACT 0x21
#define ID_CONTAINER_OPEN 0x2E
#define ID_CONTAINER_CLOSE 0x2F
#define ID_LEVEL_CHUNK 0x3A
#define ID_SET_COMMANDS_ENABLED_PACKET 0x3B //todo
#define ID_REQUEST_CHUNK_RADIUS 0x45
#define ID_CHUNK_RADIUS_UPDATED 0x46
#define ID_PLAYER_SKIN 0x5D
#define ID_AVAILABLE_ENTITY_IDENTIFIERS 0x77
#define ID_NETWORK_CHUNK_PUBLISHER_UPDATE 0x79
#define ID_BIOME_DEFINITION_LIST 0x7A
#define ID_NETWORK_SETTINGS 0x8F
#define ID_CREATIVE_CONTENT 0x91
#define ID_REQUEST_ABILITY 0xB8
#define ID_REQUEST_NETWORK_SETTINGS 0xC1

#define ID_GAME 0xFE

#define PLAY_STATUS_LOGIN_SUCCESS 0
#define PLAY_STATUS_FAILED_CLIENT 1
#define PLAY_STATUS_FAILED_SERVER 2
#define PLAY_STATUS_PLAYER_SPAWN 3
#define PLAY_STATUS_FAILED_INVALID_TENANT 4
#define PLAY_STATUS_FAILED_VANILLA_EDU 5
#define PLAY_STATUS_FAILED_EDU_VANILLA 6
#define PLAY_STATUS_FAILED_SERVER_FULL 7

#define RESOURCE_PACK_CLIENT_RESPONSE_NONE 0
#define RESOURCE_PACK_CLIENT_RESPONSE_REFUSED 1
#define RESOURCE_PACK_CLIENT_RESPONSE_SEND_PACKS 2
#define RESOURCE_PACK_CLIENT_RESPONSE_HAVE_ALL_PACKS 3
#define RESOURCE_PACK_CLIENT_RESPONSE_COMPLETED 4

#define MOVE_PLAYER_MODE_NORMAL 0
#define MOVE_PLAYER_MODE_RESET 1
#define MOVE_PLAYER_MODE_TELEPORT 2
#define MOVE_PLAYER_MODE_ROTATION 3

#define MOVE_PLAYER_TELEPORT_CAUSE_UNKNOWN 0
#define MOVE_PLAYER_TELEPORT_CAUSE_PROJECTILE 1
#define MOVE_PLAYER_TELEPORT_CAUSE_CHORUS_FRUIT 2
#define MOVE_PLAYER_TELEPORT_CAUSE_COMMAND 3
#define MOVE_PLAYER_TELEPORT_CAUSE_BEHAVIOR 4

#define INTERACT_LEAVE_VEHICLE 3
#define INTERACT_MOUSE_OVER_ENTITY 4
#define INTERACT_OPEN_INVENTORY 6

#define TEXT_RAW 0
#define TEXT_CHAT 1
#define TEXT_TRANSLATION 2
#define TEXT_POPUP 3
#define TEXT_JUKEBOX_POPUP 4
#define TEXT_TIP 5
#define TEXT_SYSTEM 6
#define TEXT_WHISPER 7
#define TEXT_ANNOUNCEMENT 8
#define TEXT_JSON_WHISPER 9
#define TEXT_JSON 10
#define TEXT_JSON_ANNOUNCEMENT 11

#define COMMAND_ORIGIN_PLAYER 0
#define COMMAND_ORIGIN_BLOCK 1
#define COMMAND_ORIGIN_MINECART_BLOCK 2
#define COMMAND_ORIGIN_DEV_CONSOLE 3
#define COMMAND_ORIGIN_TEST 4
#define COMMAND_ORIGIN_AUTOMATION_PLAYER 5
#define COMMAND_ORIGIN_CLIENT_AUTOMATION 6
#define COMMAND_ORIGIN_DEDICATED_SERVER 7
#define COMMAND_ORIGIN_ENTITY 8
#define COMMAND_ORIGIN_VIRTUAL 9
#define COMMAND_ORIGIN_GAME_ARGUMENT 10
#define COMMAND_ORIGIN_ENTITY_SERVER 11 //???

#define REQUEST_ABILITY_BUILD 0
#define REQUEST_ABILITY_MINE 1
#define REQUEST_ABILITY_DOORS_AND_SWITCHES 2
#define REQUEST_ABILITY_OPEN_CONTAINERS 3
#define REQUEST_ABILITY_ATTACK_PLAYERS 4
#define REQUEST_ABILITY_ATTACK_MOBS 5
#define REQUEST_ABILITY_OPERATOR_COMMANDS 6
#define REQUEST_ABILITY_TELEPORT 7
#define REQUEST_ABILITY_INVULNERABLE 8
#define REQUEST_ABILITY_FLYING 9
#define REQUEST_ABILITY_MAY_FLY 10
#define REQUEST_ABILITY_INSTANT_BUILD 11
#define REQUEST_ABILITY_LIGHTNING 12
#define REQUEST_ABILITY_FLY_SPEED 13
#define REQUEST_ABILITY_WALK_SPEED 14
#define REQUEST_ABILITY_MUTED 15
#define REQUEST_ABILITY_WORLD_BUILDER 16
#define REQUEST_ABILITY_NO_CLIP 17
#define REQUEST_ABILITY_COUNT 18

typedef struct {
	binary_stream_t *streams;
	size_t streams_count;
} packet_game_t;

typedef struct {
	int32_t protocol_version;
	misc_login_tokens_t tokens;
} packet_login_t;

typedef struct{
	int32_t protocol_version;
} packet_request_network_setting_t;

typedef struct {
	int32_t status;
} packet_play_status_t;

typedef struct {
	uint8_t must_accept;
	uint8_t has_scripts;
	uint8_t force_server_packs;
	misc_behavior_pack_infos_t behavior_packs;
	misc_texture_pack_infos_t texture_packs;
} packet_resource_packs_info_t;

typedef struct {
	uint8_t must_accept;
	misc_resource_pack_id_versions_t behavior_packs;
	misc_resource_pack_id_versions_t resource_packs;
	char *game_version;
	misc_experiments_t experiments;
	uint8_t experiments_previously_used;
} packet_resource_pack_stack_t;

typedef struct {
	uint8_t response_status;
	misc_resource_pack_ids_t resource_pack_ids;
} packet_resource_pack_client_response_t;

typedef struct {
	uint8_t type;
	uint8_t needs_translation;
	char *source_name;
	char *message;
	uint32_t parameter_count;
    char **parameters;
    char *xbox_userId;
	char *platform_chat_id;
} packet_text_t;

typedef struct {
	int64_t entity_unique_id;
	uint64_t entity_runtime_id;
	int32_t player_gamemode;
	float player_x;
	float player_y;
	float player_z;
	float pitch;
	float yaw;
	int64_t seed;
	int16_t biome_type;
	char *biome_name;
	int32_t dimension;
	int32_t generator;
	int32_t world_gamemode;
	int32_t difficulty;
	int32_t spawn_x;
	uint32_t spawn_y;
	int32_t spawn_z;
	uint8_t achievements_disabled;
	int32_t day_cycle_stop_time;
	int32_t edu_offer;
	uint8_t edu_features_enabled;
	char *edu_product_uuid;
	float rain_level;
	float lightning_level;
	uint8_t has_confirmed_platform_locked_content;
	uint8_t is_multiplayer;
	uint8_t broadcast_to_lan;
	uint32_t xbox_live_broadcast_mode;
	uint32_t platform_broadcast_mode;
	uint8_t enable_commands;
	uint8_t are_texture_packs_required;
	misc_game_rules_t gamerules;
	misc_experiments_t experiments;
	uint8_t experiments_previously_used;
	uint8_t bonus_chest;
	uint8_t map_enabled;
	int32_t permission_level;
	int32_t server_chunk_tick_range;
	uint8_t has_locked_behavior_pack;
	uint8_t has_locked_texture_pack;
	uint8_t is_from_locked_world_template;
	uint8_t msa_gamertags_only;
	uint8_t is_from_world_template;
	uint8_t is_world_template_option_locked;
	uint8_t only_spawn_v1_villagers;
	char *game_version;
	int32_t limited_world_width;
	int32_t limited_world_length;
	uint8_t is_new_nether;
	misc_education_shared_resource_uri_t edu_resource_uri;
	uint8_t experimental_gameplay_override;
	char *level_id;
	char *world_name;
	char *premium_world_template_id;
	uint8_t is_trial;
	int32_t movement_authority;
	int32_t rewind_history_size;
	uint8_t server_authoritative_block_breaking;
	int64_t current_tick;
	int32_t enchantment_seed;
	misc_block_properties_t block_properties;
	misc_item_states_t item_states;
	char *multiplayer_correlation_id;
	uint8_t server_authoritative_inventory;
	char *engine;
	uint64_t block_pallete_checksum;
} packet_start_game_t;

typedef struct {
	nbt_named_t nbt;
} packet_biome_definition_list_t;

typedef struct {
	nbt_named_t nbt;
} packet_available_entity_identifiers_t;

typedef struct {
	uint32_t size;
	uint32_t *entry_ids;
	misc_item_t *items;
} packet_creative_content_t;

typedef struct {
	uint64_t entity_runtime_id;
	uint32_t size;
	misc_attribute_t *attributes;
	uint64_t tick;
} packet_update_attributes_t;

typedef struct {
	uint8_t action_id;
	uint64_t entity_runtime_id;
	float position_x;
	float position_y;
	float position_z;
} packet_interact_t;

typedef struct {
	int8_t window_id;
	int8_t window_type;
	int32_t coordinates_x;
	uint32_t coordinates_y;
	int32_t coordinates_z;
	int64_t entity_unique_id;
} packet_container_open_t;

typedef struct {
	int8_t window_id;
	uint8_t server;
} packet_container_close_t;

typedef struct {
	uint32_t runtime_id;
	float position_x;
	float position_y;
	float position_z;
	float pitch;
	float yaw;
	float head_yaw;
	uint8_t mode;
	uint8_t on_ground;
	uint32_t ridden_runtime_id;
	int32_t teleport_cause;
	int32_t teleport_source_entity_type;
	uint64_t tick;
} packet_move_player_t;

typedef struct {
	uint32_t runtime_id;
	uint8_t flags;
	float position_x;
	float position_y;
	float position_z;
	float pitch;
	float yaw;
	float head_yaw;
} packet_move_entity_t;

typedef struct {
	unsigned char *skin_uuid;
	misc_skin_t skin_data;
	char *new_skin_name;
	char *old_skin_name;
	uint8_t verified;
} packet_player_skin_t;

typedef struct {
	int32_t x;
	uint32_t y;
	int32_t z;
	uint32_t radius;
} packet_network_chunk_publisher_update_t;

typedef struct {
	int32_t chunk_radius;
} packet_request_chunk_radius_t;

typedef struct {
	int32_t chunk_radius;
} packet_chunk_radius_updated_t;

typedef struct {
	int32_t x;
	int32_t z;
	uint32_t sub_chunk_count;
	uint16_t highest_subchunk_count;
	uint8_t cache_enabled;
	uint32_t hashes_count;
	uint64_t *hashes;
	binary_stream_t payload;
} packet_level_chunk_t;

typedef struct {
	int32_t ability_id;
	uint8_t ability_type;
	uint8_t ability_bool;
	float ability_float;
} packet_request_ability_t;

packet_game_t get_packet_game(binary_stream_t *stream);

packet_login_t get_packet_login(binary_stream_t *stream);

packet_request_network_setting_t get_packet_request_network_setting(binary_stream_t *stream);

packet_resource_pack_client_response_t get_packet_resource_pack_client_response(binary_stream_t *stream);

packet_text_t get_packet_text(binary_stream_t *stream);

packet_interact_t get_packet_interact(binary_stream_t *stream);

packet_container_open_t get_packet_container_open(binary_stream_t *stream);

packet_container_close_t get_packet_container_close(binary_stream_t *stream);

packet_move_player_t get_packet_move_player(binary_stream_t *stream);

packet_request_chunk_radius_t get_packet_request_chunk_radius(binary_stream_t *stream);

packet_player_skin_t get_packet_player_skin(binary_stream_t *stream);

packet_request_ability_t get_packet_request_ability(binary_stream_t *stream);

void put_packet_game(packet_game_t packet, binary_stream_t *stream, uint8_t compress);

void put_packet_network_setting(binary_stream_t *stream);

void put_packet_play_status(packet_play_status_t packet, binary_stream_t *stream);

void put_packet_disconnect(char *msg, binary_stream_t *stream);

void put_packet_resource_packs_info(packet_resource_packs_info_t packet, binary_stream_t *stream);

void put_packet_resource_pack_stack(packet_resource_pack_stack_t packet, binary_stream_t *stream);

void put_packet_resource_pack_client_response(packet_resource_pack_client_response_t packet, binary_stream_t *stream);

void put_packet_text(packet_text_t packet, binary_stream_t *stream);

void put_packet_set_time(int32_t time, binary_stream_t *stream);

void put_packet_start_game(packet_start_game_t packet, binary_stream_t *stream);

void put_packet_biome_definition_list(packet_biome_definition_list_t packet, binary_stream_t *stream);

void put_packet_available_entity_identifiers(packet_available_entity_identifiers_t packet, binary_stream_t *stream);

void put_packet_creative_content(packet_creative_content_t packet, binary_stream_t *stream);

void put_packet_update_attributes(packet_update_attributes_t packet, binary_stream_t *stream);

void put_packet_interact(packet_interact_t packet, binary_stream_t *stream);

void put_packet_container_open(packet_container_open_t packet, binary_stream_t *stream);

void put_packet_container_close(packet_container_close_t packet, binary_stream_t *stream);

void put_packet_move_player(packet_move_player_t packet, binary_stream_t *stream);

void put_packet_move_entity(packet_move_entity_t packet, binary_stream_t *stream);

void put_packet_player_skin(packet_player_skin_t packet, binary_stream_t *stream);

void put_packet_network_chunk_publisher_update(packet_network_chunk_publisher_update_t packet, binary_stream_t *stream, int32_t protocol);

void put_packet_request_chunk_radius(packet_request_chunk_radius_t packet, binary_stream_t *stream);

void put_packet_chunk_radius_updated(packet_chunk_radius_updated_t packet, binary_stream_t *stream);

void put_packet_level_chunk(packet_level_chunk_t packet, binary_stream_t *stream);

#endif
