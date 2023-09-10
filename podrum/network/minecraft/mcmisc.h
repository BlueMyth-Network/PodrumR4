

#ifndef PODRUM_NETWORK_MINECRAFT_MCMISC_H
#define PODRUM_NETWORK_MINECRAFT_MCMISC_H

#include <podrum/debug.h>
#include <stdint.h>
#include <stdbool.h>
#include <cbinarystream/binary_stream.h>
#include <cnbt/nbt.h>
#include <podrum/world/chunk/chunk.h>

#define GAMEMODE_SURVIVAL 0
#define GAMEMODE_CREATIVE 1
#define GAMEMODE_ADVENTURE 2
#define GAMEMODE_SURVIVAL_SPECTATOR 3
#define GAMEMODE_CREATIVE_SPECTATOR 4
#define GAMEMODE_FALLBACK 5

#define GAME_RULE_BOOLEAN 1
#define GAME_RULE_SIGNED_VAR_INT 2
#define GAME_RULE_FLOAT_LE 3

#define ITEM_EXTRA_DATA_WITH_NBT 0xffff
#define ITEM_EXTRA_DATA_WITHOUT_NBT 0x0000

#define WINDOW_ID_DROP_CONTENTS -100
#define WINDOW_ID_BEACON -24
#define WINDOW_ID_TRADING_OUTPUT -23
#define WINDOW_ID_TRADING_USE_INPUTS -22
#define WINDOW_ID_TRADING_INPUT_2 -21
#define WINDOW_ID_TRADING_INPUT_1 -20
#define WINDOW_ID_ENCHANT_OUTPUT -17
#define WINDOW_ID_ENCHANT_MATERIAL -16
#define WINDOW_ID_ENCHANT_INPUT -15
#define WINDOW_ID_ANVIL_OUTPUT -13
#define WINDOW_ID_ANVIL_RESULT -12
#define WINDOW_ID_ANVIL_MATERIAL -11
#define WINDOW_ID_CONTAINER_INPUT -10
#define WINDOW_ID_CRAFTING_USE_INGREDIENT -5
#define WINDOW_ID_CRAFTING_RESULT -4
#define WINDOW_ID_CRAFTING_REMOVE_INGREDIENT -3
#define WINDOW_ID_CRAFTING_ADD_INGREDIENT -2
#define WINDOW_ID_NONE -1
#define WINDOW_ID_INVENTORY 0
#define WINDOW_ID_FIRST 1
#define WINDOW_ID_LAST 100
#define WINDOW_ID_OFFHAND 119
#define WINDOW_ID_ARMOR 120
#define WINDOW_ID_CREATIVE 121
#define WINDOW_ID_HOTBAR 122
#define WINDOW_ID_FIXED_INVENTORY 123
#define WINDOW_ID_UI 124

#define WINDOW_TYPE_NONE -9
#define WINDOW_TYPE_INVENTORY -1
#define WINDOW_TYPE_CONTAINER 0
#define WINDOW_TYPE_WORKBENCH 1
#define WINDOW_TYPE_FURNACE 2
#define WINDOW_TYPE_ENCHANTMENT 3
#define WINDOW_TYPE_BREWING_STAND 4
#define WINDOW_TYPE_ANVIL 5
#define WINDOW_TYPE_DISPENSER 6
#define WINDOW_TYPE_DROPPER 7
#define WINDOW_TYPE_HOPPER 8
#define WINDOW_TYPE_CAULDRON 9
#define WINDOW_TYPE_MINECART_CHEST 10
#define WINDOW_TYPE_MINECART_HOPPER 11
#define WINDOW_TYPE_HORSE 12
#define WINDOW_TYPE_BEACON 13
#define WINDOW_TYPE_STRUCTURE_EDITOR 14
#define WINDOW_TYPE_TRADING 15
#define WINDOW_TYPE_COMMAND_BLOCK 16
#define WINDOW_TYPE_JUKEBOX 17
#define WINDOW_TYPE_ARMOR 18
#define WINDOW_TYPE_HAND 19
#define WINDOW_TYPE_COMPOUND_CREATOR 20
#define WINDOW_TYPE_ELEMENT_CONSTRUCTOR 21
#define WINDOW_TYPE_MATERIAL_REDUCER 22
#define WINDOW_TYPE_LAB_TABLE 23
#define WINDOW_TYPE_LOOM 24
#define WINDOW_TYPE_LECTERN 25
#define WINDOW_TYPE_GRINDSTONE 26
#define WINDOW_TYPE_BLAST_FURNACE 27
#define WINDOW_TYPE_SMOKER 28
#define WINDOW_TYPE_SONECUTTER 29
#define WINDOW_TYPE_CARTOGRAPHY 30
#define WINDOW_TYPE_HUD 31
#define WINDOW_TYPE_JIGSAW_EDITOR 32
#define WINDOW_TYPE_SMITHING_TABLE 33

static int32_t BITARRAY_V1[4] = {1, 32, 128, 0};
static int32_t BITARRAY_V2[4] = {2, 16, 256, 0};
static int32_t BITARRAY_V3[4] = {3, 10, 410, 2};
static int32_t BITARRAY_V4[4] = {4, 8, 512, 0};
static int32_t BITARRAY_V5[4] = {5, 6, 683, 2};
static int32_t BITARRAY_V6[4] = {6, 5, 820, 2};
static int32_t BITARRAY_V8[4] = {8, 4, 1024, 0};
static int32_t BITARRAY_V16[4] = {16, 2, 2048, 0};

typedef struct {
	uint32_t sender_sub_id;
	uint32_t recipient_sub_id;
	int32_t protocol;
} misc_client_t;

typedef struct {
	char *identity;
	char *client;
} misc_login_tokens_t;

typedef struct {
	char *uuid;
	char *version;
	uint64_t size;
	char *content_key;
	char *sub_pack_name;
	char *content_identity;
	uint8_t has_scripts;
} misc_behavior_pack_info_t;

typedef struct {
	misc_behavior_pack_info_t *infos;
	int16_t size;
} misc_behavior_pack_infos_t;

typedef struct {
	char *uuid;
	char *version;
	uint64_t size;
	char *content_key;
	char *sub_pack_name;
	char *content_identity;
	uint8_t has_scripts;
	uint8_t rtx_enabled;
} misc_texture_pack_info_t;

typedef struct {
	misc_texture_pack_info_t *infos;
	int16_t size;
} misc_texture_pack_infos_t;

typedef struct {
	char *uuid;
	char *version;
	char *name;
} misc_resource_pack_id_version_t;

typedef struct {
	misc_resource_pack_id_version_t *id_versions;
	uint32_t size;
} misc_resource_pack_id_versions_t;

typedef struct {
	char **ids;
	int16_t size;
} misc_resource_pack_ids_t;

typedef struct {
	char *name;
	uint8_t enabled;
} misc_experiment_t;

typedef struct {
	misc_experiment_t *entries;
	int32_t size;
} misc_experiments_t;

typedef union {
	uint8_t boolean;
	int32_t signed_var_int;
	float float_le;
} misc_multi_game_rule_t;

typedef struct {
	char *name;
	uint8_t editable;
	uint32_t type;
	misc_multi_game_rule_t value;
} misc_game_rule_t;

typedef struct {
	misc_game_rule_t *entries;
	uint32_t size;
} misc_game_rules_t;

typedef struct {
	char *button_name;
	char *link_uri;
} misc_education_shared_resource_uri_t;

typedef struct {
	char *name;
	int16_t runtime_id;
	uint8_t component_based;
} misc_item_state_t;

typedef struct {
	misc_item_state_t *entries;
	uint32_t size;
} misc_item_states_t;

typedef struct {
	char *name;
	nbt_named_t nbt;
} misc_block_property_t;

typedef struct {
	misc_block_property_t *entries;
	uint32_t size;
} misc_block_properties_t;

typedef struct {
	uint16_t with_nbt;
	uint8_t nbt_version;
	nbt_named_t nbt;
	int32_t can_place_on_size;
	binary_stream_t *can_place_on;
	int32_t can_destroy_size;
	binary_stream_t *can_destroy;
	int64_t blocking_tick;
} misc_item_extra_data_t;

typedef struct {
	int32_t network_id;
	uint16_t count;
	uint32_t metadata;
	uint8_t has_stack_id;
	int32_t stack_id;
	int32_t block_runtime_id;
	misc_item_extra_data_t extra;
} misc_item_t;

typedef struct {
	int32_t width;
	int32_t height;
	char *data;
} misc_skin_image_t;

typedef struct {
	misc_skin_image_t image;
	int32_t type;
	float frames;
	int32_t expression_type;
} misc_skin_animation_t;

typedef struct {
	char *piece_id;
	char *piece_type;
	char *pack_id;
	uint8_t is_piece_default;
	char *product_id;
} misc_skin_persona_t;

typedef struct {
	char *piece_type;
	int32_t color_count;
	char **colors;
} misc_skin_persona_tint_color_t;

typedef struct {
	char *skin_id;
	char *play_fab_id;
	char *resources_patch;
	misc_skin_image_t skin_image;
	int32_t animation_count;
	misc_skin_animation_t *animations;
	misc_skin_image_t cape_image;
	char *geometry_data;
	char *geometry_engine_version;
	char *animation_data;
	char *cape_id;
	char *full_skin_id;
	char *arm_size;
	char *skin_color;
	int32_t persona_count;
	misc_skin_persona_t *personas;
	int32_t persona_tint_color_count;
	misc_skin_persona_tint_color_t *persona_tint_colors;
	uint8_t is_premium;
	uint8_t is_persona;
	uint8_t is_persona_cape_classic;
	uint8_t is_primary_user;
	uint8_t is_override;
} misc_skin_t;

typedef struct {
	char *id;
	char *name;
	float amount;
	int32_t operation;
	int32_t operand;
	uint8_t can_serialize;
} misc_attribute_modifier_t;

typedef struct {
	float min;
	float max;
	float current;
	float default_v;
	char *id;
	uint32_t attribute_modifier_count;
	misc_attribute_modifier_t *attribute_modifier;
} misc_attribute_t;

typedef struct {
	int32_t key;
	int32_t int_property;
} int_sync_data_property_t;

typedef struct {
	int32_t key;
	float float_property;
} float_sync_data_property_t;

typedef struct {
	int32_t int_count;
	int_sync_data_property_t *int_properties;
	int32_t float_count;
	float_sync_data_property_t *float_properties;
} property_sync_data_t;

char *get_misc_string_var_int(binary_stream_t *stream);

char *get_misc_string_int_le(binary_stream_t *stream);

binary_stream_t get_misc_byte_array_var_int(binary_stream_t *stream);

binary_stream_t get_misc_byte_array_signed_var_int(binary_stream_t *stream);

binary_stream_t get_misc_byte_array_short_le(binary_stream_t *stream);

misc_login_tokens_t get_misc_login_tokens(binary_stream_t *stream);

misc_behavior_pack_infos_t get_misc_behavior_pack_infos(binary_stream_t *stream);

misc_texture_pack_infos_t get_misc_texture_pack_infos(binary_stream_t *stream);

misc_resource_pack_id_versions_t get_misc_resource_pack_id_versions(binary_stream_t *stream);

misc_resource_pack_ids_t get_misc_resource_pack_ids(binary_stream_t *stream);

misc_experiment_t get_misc_experiment(binary_stream_t *stream);

misc_experiments_t get_misc_experiments(binary_stream_t *stream);

misc_game_rule_t get_misc_game_rule(binary_stream_t *stream);

misc_game_rules_t get_misc_game_rules(binary_stream_t *stream);

misc_education_shared_resource_uri_t get_misc_education_shared_resource_uri(binary_stream_t *stream);

misc_item_states_t get_misc_item_states(binary_stream_t *stream);

misc_block_properties_t get_misc_block_properties(binary_stream_t *stream);

nbt_named_t get_misc_nbt_tag(binary_stream_t *stream);

nbt_named_t get_misc_lnbt_tag(binary_stream_t *stream);

misc_item_extra_data_t get_misc_item_extra_data(uint8_t has_blocking_tick, binary_stream_t *stream);

misc_item_t get_misc_item(uint8_t with_stack_id, binary_stream_t *stream);

unsigned char *get_uuid_le(binary_stream_t *stream);

misc_skin_image_t get_misc_skin_image(binary_stream_t *stream);

misc_skin_animation_t get_misc_skin_animation(binary_stream_t *stream);

misc_skin_persona_t get_misc_skin_persona(binary_stream_t *stream);

misc_skin_persona_tint_color_t get_misc_skin_persona_tint_color(binary_stream_t *stream);

misc_skin_t get_misc_skin(binary_stream_t *stream);

void put_misc_string_var_int(char *value, binary_stream_t *stream);

void put_misc_string_int_le(char *value, binary_stream_t *stream);

void put_misc_byte_array_var_int(binary_stream_t value, binary_stream_t *stream);

void put_misc_byte_array_signed_var_int(binary_stream_t value, binary_stream_t *stream);

void put_misc_byte_array_short_le(binary_stream_t value, binary_stream_t *stream);

void put_misc_login_tokens(misc_login_tokens_t value, binary_stream_t *stream);

void put_misc_behavior_pack_infos(misc_behavior_pack_infos_t value, binary_stream_t *stream);

void put_misc_texture_pack_infos(misc_texture_pack_infos_t value, binary_stream_t *stream);

void put_misc_resource_pack_id_versions(misc_resource_pack_id_versions_t value, binary_stream_t *stream);

void put_misc_resource_pack_ids(misc_resource_pack_ids_t value, binary_stream_t *stream);

void put_misc_experiment(misc_experiment_t value, binary_stream_t *stream);

void put_misc_experiments(misc_experiments_t value, binary_stream_t *stream);

void put_misc_game_rule(misc_game_rule_t value, binary_stream_t *stream);

void put_misc_game_rules(misc_game_rules_t value, binary_stream_t *stream);

void put_misc_education_shared_resource_uri(misc_education_shared_resource_uri_t value, binary_stream_t *stream);

void put_misc_item_states(misc_item_states_t value, binary_stream_t *stream);

void put_misc_block_properties(misc_block_properties_t value, binary_stream_t *stream);

void put_misc_nbt_tag(nbt_named_t value, binary_stream_t *stream);

void put_misc_lnbt_tag(nbt_named_t value, binary_stream_t *stream);

void put_misc_item_extra_data(misc_item_extra_data_t value, uint8_t has_blocking_tick, binary_stream_t *stream);

void put_misc_item(misc_item_t value, uint8_t with_stack_id, binary_stream_t *stream);

void put_misc_block_storage(block_storage_t *value, binary_stream_t *stream);

void put_misc_sub_chunk(sub_chunk_t *value, binary_stream_t *stream);

void put_misc_chunk(chunk_t *value, uint32_t sub_chunk_count, binary_stream_t *stream);

void put_misc_uuid(unsigned char *value, binary_stream_t *stream);

void put_misc_skin_image(misc_skin_image_t image, binary_stream_t *stream);

void put_misc_skin_animation(misc_skin_animation_t animation, binary_stream_t *stream);

void put_misc_skin_persona(misc_skin_persona_t persona, binary_stream_t *stream);

void put_misc_skin(misc_skin_t skin, binary_stream_t *stream);

void put_misc_attributes(misc_attribute_t attribute, binary_stream_t *stream);

void put_property_sync_data(property_sync_data_t property_sync_data, binary_stream_t *stream);
#endif
