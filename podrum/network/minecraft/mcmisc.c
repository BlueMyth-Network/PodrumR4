#include <podrum/network/minecraft/mcmisc.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <uuid/uuid.h>

char *get_misc_string_var_int(binary_stream_t *stream)
{
	uint32_t size = get_var_int(stream);
	char *output = (char *) malloc(size + 1);
	uint32_t i;
	for (i = 0; i < size; ++i) {
		output[i] = (char) get_unsigned_byte(stream);
	}
	output[size] = 0x00;
	return output;
}

char *get_misc_string_int_le(binary_stream_t *stream)
{
	int32_t size = get_int_le(stream);
	char *output = (char *) malloc(size + 1);
	int32_t i;
	for (i = 0; i < size; ++i) {
		output[i] = (char) get_unsigned_byte(stream);
	}
	output[size] = 0x00;
	return output;
}

binary_stream_t get_misc_byte_array_var_int(binary_stream_t *stream)
{
	binary_stream_t output;
	output.offset = 0;
	output.size = (size_t) get_var_int(stream);
	output.buffer = get_bytes(output.size, stream);
	return output;
}

binary_stream_t get_misc_byte_array_signed_var_int(binary_stream_t *stream)
{
	binary_stream_t output;
	output.offset = 0;
	output.size = (size_t) get_signed_var_int(stream);
	output.buffer = get_bytes(output.size, stream);
	return output;
}

binary_stream_t get_misc_byte_array_short_le(binary_stream_t *stream)
{
	binary_stream_t output;
	output.offset = 0;
	output.size = (size_t) get_short_le(stream);
	output.buffer = get_bytes(output.size, stream);
	return output;
}

misc_login_tokens_t get_misc_login_tokens(binary_stream_t *stream)
{
	misc_login_tokens_t login_tokens;
	login_tokens.identity = get_misc_string_int_le(stream);
	login_tokens.client = get_misc_string_int_le(stream);
	return login_tokens;
}

misc_behavior_pack_infos_t get_misc_behavior_pack_infos(binary_stream_t *stream)
{
	misc_behavior_pack_infos_t behavior_pack_infos;
	behavior_pack_infos.size = get_short_le(stream);
	behavior_pack_infos.infos = (misc_behavior_pack_info_t *) malloc(behavior_pack_infos.size * sizeof(misc_behavior_pack_info_t));
	int16_t i;
	for (i = 0; i < behavior_pack_infos.size; ++i) {
		misc_behavior_pack_info_t behavior_pack_info;
		behavior_pack_info.uuid = get_misc_string_var_int(stream);
		behavior_pack_info.version = get_misc_string_var_int(stream);
		behavior_pack_info.size = get_unsigned_long_le(stream);
		behavior_pack_info.content_key = get_misc_string_var_int(stream);
		behavior_pack_info.sub_pack_name = get_misc_string_var_int(stream);
		behavior_pack_info.content_identity = get_misc_string_var_int(stream);
		behavior_pack_info.has_scripts = get_unsigned_byte(stream);
		behavior_pack_infos.infos[i] = behavior_pack_info;
	}
	return behavior_pack_infos;
}

misc_texture_pack_infos_t get_misc_texture_pack_infos(binary_stream_t *stream)
{
	misc_texture_pack_infos_t texture_pack_infos;
	texture_pack_infos.size = get_short_le(stream);
	texture_pack_infos.infos = (misc_texture_pack_info_t *) malloc(texture_pack_infos.size * sizeof(misc_texture_pack_info_t));
	int16_t i;
	for (i = 0; i < texture_pack_infos.size; ++i) {
		misc_texture_pack_info_t texture_pack_info;
		texture_pack_info.uuid = get_misc_string_var_int(stream);
		texture_pack_info.version = get_misc_string_var_int(stream);
		texture_pack_info.size = get_unsigned_long_le(stream);
		texture_pack_info.content_key = get_misc_string_var_int(stream);
		texture_pack_info.sub_pack_name = get_misc_string_var_int(stream);
		texture_pack_info.content_identity = get_misc_string_var_int(stream);
		texture_pack_info.has_scripts = get_unsigned_byte(stream);
		texture_pack_info.rtx_enabled = get_unsigned_byte(stream);
		texture_pack_infos.infos[i] = texture_pack_info;
	}
	return texture_pack_infos;
}

misc_resource_pack_id_versions_t get_misc_resource_pack_id_versions(binary_stream_t *stream)
{
	misc_resource_pack_id_versions_t resource_pack_id_versions;
	resource_pack_id_versions.size = get_var_int(stream);
	resource_pack_id_versions.id_versions = (misc_resource_pack_id_version_t *) malloc(resource_pack_id_versions.size * sizeof(misc_resource_pack_id_version_t));
	uint32_t i;
	for (i = 0; i < resource_pack_id_versions.size; ++i) {
		misc_resource_pack_id_version_t resource_pack_id_version;
		resource_pack_id_version.uuid = get_misc_string_var_int(stream);
		resource_pack_id_version.version = get_misc_string_var_int(stream);
		resource_pack_id_version.name = get_misc_string_var_int(stream);
		resource_pack_id_versions.id_versions[i] = resource_pack_id_version;
	}
	return resource_pack_id_versions;
}

misc_resource_pack_ids_t get_misc_resource_pack_ids(binary_stream_t *stream)
{
	misc_resource_pack_ids_t resource_pack_ids;
	resource_pack_ids.size = get_short_le(stream);
	resource_pack_ids.ids = (char **) malloc(resource_pack_ids.size * sizeof(char *));
	int16_t i;
	for (i = 0; i < resource_pack_ids.size; ++i) {
		resource_pack_ids.ids[i] = get_misc_string_var_int(stream);
	}
	return resource_pack_ids;
}

misc_experiment_t get_misc_experiment(binary_stream_t *stream)
{
	misc_experiment_t experiment;
	experiment.name = get_misc_string_var_int(stream);
	experiment.enabled = get_unsigned_byte(stream);
	return experiment;
}

misc_experiments_t get_misc_experiments(binary_stream_t *stream) {
	misc_experiments_t experiments;
	experiments.size = get_int_le(stream);
	experiments.entries = (misc_experiment_t *) malloc(experiments.size * sizeof(misc_experiment_t));
	int32_t i;
	for (i = 0; i < experiments.size; ++i) {
		experiments.entries[i] = get_misc_experiment(stream);
	}
	return experiments;
}

misc_game_rule_t get_misc_game_rule(binary_stream_t *stream)
{
	misc_game_rule_t game_rule;
	game_rule.name = get_misc_string_var_int(stream);
	game_rule.editable = get_unsigned_byte(stream);
	game_rule.type = get_var_int(stream);
	switch (game_rule.type) {
	case GAME_RULE_BOOLEAN:
		game_rule.value.boolean = get_unsigned_byte(stream);
		break;
	case GAME_RULE_SIGNED_VAR_INT:
		game_rule.value.signed_var_int = get_signed_var_int(stream);
		break;
	case GAME_RULE_FLOAT_LE:
		game_rule.value.float_le = get_float_le(stream);
		break;
	}
	return game_rule;
}

misc_game_rules_t get_misc_game_rules(binary_stream_t *stream)
{
	misc_game_rules_t game_rules;
	game_rules.size = get_var_int(stream);
	game_rules.entries = (misc_game_rule_t *) malloc(sizeof(misc_game_rule_t) * game_rules.size);
	uint32_t i;
	for (i = 0; i < game_rules.size; ++i) {
		game_rules.entries[i] = get_misc_game_rule(stream);
	}
	return game_rules;
}

misc_education_shared_resource_uri_t get_misc_education_shared_resource_uri(binary_stream_t *stream)
{
	misc_education_shared_resource_uri_t education_shared_resource_uri;
	education_shared_resource_uri.button_name = get_misc_string_var_int(stream);
	education_shared_resource_uri.link_uri = get_misc_string_var_int(stream);
	return education_shared_resource_uri;
}

misc_item_states_t get_misc_item_states(binary_stream_t *stream)
{
	misc_item_states_t item_states;
	item_states.size = get_var_int(stream);
	item_states.entries = (misc_item_state_t *) malloc(sizeof(misc_item_state_t) * item_states.size);
	uint32_t i;
	for (i = 0; i < item_states.size; ++i) {
		item_states.entries[i].name = get_misc_string_var_int(stream);
		item_states.entries[i].runtime_id = get_short_le(stream);
		item_states.entries[i].component_based = get_unsigned_byte(stream);
	}
	return item_states;
}

misc_block_properties_t get_misc_block_properties(binary_stream_t *stream)
{
	misc_block_properties_t block_properties;
	block_properties.size = get_var_int(stream);
	block_properties.entries = (misc_block_property_t *) malloc(sizeof(misc_block_property_t) * block_properties.size);
	uint32_t i;
	for (i = 0; i < block_properties.size; ++i) {
		block_properties.entries[i].name = get_misc_string_var_int(stream);
		block_properties.entries[i].nbt = get_misc_nbt_tag(stream);
	}
	return block_properties;
}

nbt_named_t get_misc_nbt_tag(binary_stream_t *stream)
{
	return get_nbt_named_tag(E_NETWORK_ENDIAN, stream);
}

nbt_named_t get_misc_lnbt_tag(binary_stream_t *stream)
{
	return get_nbt_named_tag(E_LITTLE_ENDIAN, stream);
}

misc_item_extra_data_t get_misc_item_extra_data(uint8_t has_blocking_tick, binary_stream_t *stream)
{
	misc_item_extra_data_t extra_data;
	extra_data.with_nbt = get_unsigned_short_le(stream);
	if (extra_data.with_nbt == ITEM_EXTRA_DATA_WITH_NBT) {
		extra_data.nbt_version = get_unsigned_byte(stream);
		extra_data.nbt = get_misc_lnbt_tag(stream);
	}
	int32_t i;
	extra_data.can_place_on_size = get_int_le(stream);
	extra_data.can_place_on = (binary_stream_t *) malloc(extra_data.can_place_on_size * sizeof(binary_stream_t));
	for (i = 0; i < extra_data.can_place_on_size; ++i) {
		extra_data.can_place_on[i] = get_misc_byte_array_short_le(stream);
	}
	extra_data.can_destroy_size = get_int_le(stream);
	extra_data.can_destroy = (binary_stream_t *) malloc(extra_data.can_destroy_size * sizeof(binary_stream_t));
	for (i = 0; i < extra_data.can_destroy_size; ++i) {
		extra_data.can_destroy[i] = get_misc_byte_array_short_le(stream);
	}
	if (has_blocking_tick) {
		extra_data.blocking_tick = get_long_le(stream);
	} else {
		has_blocking_tick = -1;
	}
	return extra_data;
}

misc_item_t get_misc_item(uint8_t with_stack_id, binary_stream_t *stream)
{
	misc_item_t item;
	item.network_id = get_signed_var_int(stream);
	if (item.network_id != 0) {
		item.count = get_unsigned_short_le(stream);
		item.metadata = get_var_int(stream);
		if (with_stack_id) {
			item.has_stack_id = get_unsigned_byte(stream);
			if (item.has_stack_id != 0) {
				item.stack_id = get_signed_var_int(stream);
			}
		} else {
			item.has_stack_id = 0;
		}
		item.block_runtime_id = get_signed_var_int(stream);
		binary_stream_t temp_stream = get_misc_byte_array_var_int(stream);
		item.extra = get_misc_item_extra_data(item.network_id == 355 ? 1 : 0, &temp_stream);
		free(temp_stream.buffer);
	}
	return item;
}

// uuid_t* get_uuid_le(binary_stream_t *stream) {
//     uuid_t uuid;
//     uint8_t raw[16];
//     memcpy(raw, &stream->buffer[stream->offset], 16);
//     stream->offset += 16;
//     uuid_parse(raw, uuid);
//     return &uuid;
// }

misc_skin_image_t get_misc_skin_image(binary_stream_t *stream){
	misc_skin_image_t skin_image;
	skin_image.width = get_int_le(stream);
	skin_image.height = get_int_le(stream);
	skin_image.data = get_misc_string_var_int(stream);
	return skin_image;
}

misc_skin_animation_t get_misc_skin_animation(binary_stream_t *stream){
	misc_skin_animation_t skin_animation;
	skin_animation.image = get_misc_skin_image(stream);
	skin_animation.type = get_int_le(stream);
	skin_animation.frames = get_float_le(stream);
	skin_animation.expression_type = get_int_le(stream);
	return skin_animation;
}

misc_skin_persona_t get_misc_skin_persona(binary_stream_t *stream){
	misc_skin_persona_t skin_persona;
	skin_persona.piece_id = get_misc_string_var_int(stream);
	skin_persona.piece_type = get_misc_string_var_int(stream);
	skin_persona.pack_id = get_misc_string_var_int(stream);
	skin_persona.is_piece_default = get_unsigned_byte(stream);
	skin_persona.product_id = get_misc_string_var_int(stream);
	return skin_persona;
}

misc_skin_persona_tint_color_t get_misc_skin_persona_tint_color(binary_stream_t *stream){
	misc_skin_persona_tint_color_t skin_persona_tint_color;
	skin_persona_tint_color.piece_type = get_misc_string_var_int(stream);
	skin_persona_tint_color.color_count = get_int_le(stream);
	uint32_t i;
	for (i = 0; i < skin_persona_tint_color.color_count; i++){
		skin_persona_tint_color.colors[i] = get_misc_string_var_int(stream);
	}
	return skin_persona_tint_color;
}

misc_skin_t get_misc_skin(binary_stream_t *stream, int32_t protocol){
	misc_skin_t skin;
	skin.skin_id = get_misc_string_var_int(stream);
	skin.play_fab_id = get_misc_string_var_int(stream);
	skin.resources_patch = get_misc_string_var_int(stream);
	skin.skin_image = get_misc_skin_image(stream);
	skin.animation_count = get_int_le(stream);
	uint32_t i;
	for (size_t i = 0; i < skin.animation_count; i++){
		skin.animations[i] = get_misc_skin_animation(stream);
	}
	skin.cape_image = get_misc_skin_image(stream);
	skin.geometry_data = get_misc_string_var_int(stream);
	skin.geometry_engine_version = get_misc_string_var_int(stream);
	skin.animation_data = get_misc_string_var_int(stream);
	skin.cape_id = get_misc_string_var_int(stream);
	skin.full_skin_id = get_misc_string_var_int(stream);
	skin.arm_size = get_misc_string_var_int(stream);
	skin.skin_color = get_misc_string_var_int(stream);
	skin.persona_count = get_int_le(stream);
	for (size_t i = 0; i < skin.persona_count; i++){
		skin.personas[i] = get_misc_skin_persona(stream);
	}
	skin.persona_tint_color_count = get_int_le(stream);
	for (size_t i = 0; i < skin.persona_tint_color_count; i++){
		skin.persona_tint_colors[i] = get_misc_skin_persona_tint_color(stream);
	}
	skin.is_premium = get_unsigned_byte(stream);
	skin.is_persona = get_unsigned_byte(stream);
	skin.is_persona_cape_classic = get_unsigned_byte(stream);
	skin.is_primary_user = get_unsigned_byte(stream);
	if(protocol >= 568){
		skin.override = get_unsigned_byte(stream);
	}
	return skin;
}

void put_misc_string_var_int(char *value, binary_stream_t *stream)
{
	int length = strlen(value);
	put_var_int(length & 0xffffffff, stream);
	put_bytes((int8_t *) value, length, stream);
}

void put_misc_string_int_le(char *value, binary_stream_t *stream)
{
	int length = strlen(value);
	put_int_le(length, stream);
	put_bytes((int8_t *) value, length, stream);
}

void put_misc_byte_array_var_int(binary_stream_t value, binary_stream_t *stream)
{
	put_var_int(value.size, stream);
	put_bytes(value.buffer, value.size, stream);
}

void put_misc_byte_array_signed_var_int(binary_stream_t value, binary_stream_t *stream)
{
	put_signed_var_int(value.size, stream);
	put_bytes(value.buffer, value.size, stream);
}

void put_misc_byte_array_short_le(binary_stream_t value, binary_stream_t *stream)
{
	put_short_le(value.size, stream);
	put_bytes(value.buffer, value.size, stream);
}

void put_misc_login_tokens(misc_login_tokens_t value, binary_stream_t *stream)
{
	put_misc_string_int_le(value.identity, stream);
	put_misc_string_int_le(value.client, stream);
}

void put_misc_behavior_pack_infos(misc_behavior_pack_infos_t value, binary_stream_t *stream)
{
	put_short_le(value.size, stream);
	int16_t i;
	for (i = 0; i < value.size; ++i) {
		put_misc_string_var_int(value.infos[i].uuid, stream);
		put_misc_string_var_int(value.infos[i].version, stream);
		put_unsigned_long_le(value.infos[i].size, stream);
		put_misc_string_var_int(value.infos[i].content_key, stream);
		put_misc_string_var_int(value.infos[i].sub_pack_name, stream);
		put_misc_string_var_int(value.infos[i].content_identity, stream);
		put_unsigned_byte(value.infos[i].has_scripts, stream);
	}
}

void put_misc_texture_pack_infos(misc_texture_pack_infos_t value, binary_stream_t *stream)
{
	put_short_le(value.size, stream);
	int16_t i;
	for (i = 0; i < value.size; ++i) {
		put_misc_string_var_int(value.infos[i].uuid, stream);
		put_misc_string_var_int(value.infos[i].version, stream);
		put_unsigned_long_le(value.infos[i].size, stream);
		put_misc_string_var_int(value.infos[i].content_key, stream);
		put_misc_string_var_int(value.infos[i].sub_pack_name, stream);
		put_misc_string_var_int(value.infos[i].content_identity, stream);
		put_unsigned_byte(value.infos[i].has_scripts, stream);
		put_unsigned_byte(value.infos[i].rtx_enabled, stream);
	}
}

void put_misc_resource_pack_id_versions(misc_resource_pack_id_versions_t value, binary_stream_t *stream)
{
	put_var_int(value.size, stream);
	uint32_t i;
	for (i = 0; i < value.size; ++i) {
		put_misc_string_var_int(value.id_versions[i].uuid, stream);
		put_misc_string_var_int(value.id_versions[i].version, stream);
		put_misc_string_var_int(value.id_versions[i].name, stream);
	}
}

void put_misc_resource_pack_ids(misc_resource_pack_ids_t value, binary_stream_t *stream)
{
	put_short_le(value.size, stream);
	uint32_t i;
	for (i = 0; i < value.size; ++i) {
		put_misc_string_var_int(value.ids[i], stream);
	}
}

void put_misc_experiment(misc_experiment_t value, binary_stream_t *stream)
{
	put_misc_string_var_int(value.name, stream);
	put_unsigned_byte(value.enabled, stream);
}

void put_misc_experiments(misc_experiments_t value, binary_stream_t *stream)
{
	put_int_le(value.size, stream);
	int32_t i;
	for (i = 0; i < value.size; ++i) {
		put_misc_experiment(value.entries[i], stream);
	}
}

void put_misc_game_rule(misc_game_rule_t value, binary_stream_t *stream)
{
	put_misc_string_var_int(value.name, stream);
	put_unsigned_byte(value.editable, stream);
	put_var_int(value.type, stream);
	switch (value.type) {
	case GAME_RULE_BOOLEAN:
		put_unsigned_byte(value.value.boolean, stream);
		break;
	case GAME_RULE_SIGNED_VAR_INT:
		put_signed_var_int(value.value.signed_var_int, stream);
		break;
	case GAME_RULE_FLOAT_LE:
		put_float_le(value.value.float_le, stream);
		break;
	}
}

void put_misc_game_rules(misc_game_rules_t value, binary_stream_t *stream)
{
	put_var_int(value.size, stream);
	uint32_t i;
	for (i = 0; i < value.size; ++i) {
		put_misc_game_rule(value.entries[i], stream);
	}
}

void put_misc_education_shared_resource_uri(misc_education_shared_resource_uri_t value, binary_stream_t *stream)
{
	put_misc_string_var_int(value.button_name, stream);
	put_misc_string_var_int(value.link_uri, stream);
}

void put_misc_item_states(misc_item_states_t value, binary_stream_t *stream)
{
	put_var_int(value.size, stream);
	uint32_t i;
	for (i = 0; i < value.size; ++i) {
		put_misc_string_var_int(value.entries[i].name, stream);
		put_short_le(value.entries[i].runtime_id, stream);
		put_unsigned_byte(value.entries[i].component_based, stream);
	}
}

void put_misc_block_properties(misc_block_properties_t value, binary_stream_t *stream)
{
	put_var_int(value.size, stream);
	uint32_t i;
	for (i = 0; i < value.size; ++i) {
		put_misc_string_var_int(value.entries[i].name, stream);
		put_misc_nbt_tag(value.entries[i].nbt, stream);
	}
}

void put_misc_nbt_tag(nbt_named_t value, binary_stream_t *stream)
{
	put_nbt_named_tag(value, E_NETWORK_ENDIAN, stream);
}

void put_misc_lnbt_tag(nbt_named_t value, binary_stream_t *stream)
{
	put_nbt_named_tag(value, E_LITTLE_ENDIAN, stream);
}

void put_misc_item_extra_data(misc_item_extra_data_t value, uint8_t has_blocking_tick, binary_stream_t *stream)
{
	put_unsigned_short_le(value.with_nbt, stream);
	if (value.with_nbt == ITEM_EXTRA_DATA_WITH_NBT) {
		put_unsigned_byte(value.nbt_version, stream);
		put_misc_lnbt_tag(value.nbt, stream);
	}
	int32_t i;
	put_int_le(value.can_place_on_size, stream);
	for (i = 0; i < value.can_place_on_size; ++i) {
		put_misc_byte_array_short_le(value.can_place_on[i], stream);
	}
	put_int_le(value.can_destroy_size, stream);
	for (i = 0; i < value.can_destroy_size; ++i) {
		put_misc_byte_array_short_le(value.can_destroy[i], stream);
	}
	if (has_blocking_tick) {
		put_long_le(value.blocking_tick, stream);
	}
}

void put_misc_item(misc_item_t value, uint8_t with_stack_id, binary_stream_t *stream)
{
	put_signed_var_int(value.network_id, stream);
	if (value.network_id != 0) {
		put_unsigned_short_le(value.count, stream);
		put_var_int(value.metadata, stream);
		if (with_stack_id) {
			put_unsigned_byte(value.has_stack_id, stream);
			if (value.has_stack_id != 0) {
				put_signed_var_int(value.stack_id, stream);
			}
		}
		put_signed_var_int(value.block_runtime_id, stream);
		binary_stream_t temp_stream;
		temp_stream.buffer = (int8_t *) malloc(0);
		temp_stream.offset = 0;
		temp_stream.size = 0;
		put_misc_item_extra_data(value.extra, value.network_id == 355 ? 1 : 0, &temp_stream);
		put_misc_byte_array_var_int(temp_stream, stream);
		free(temp_stream.buffer);
	}
}

void put_misc_block_storage(block_storage_t *value, binary_stream_t *stream)
{
	double bits_per_block = ceil(log2((double) value->palette_size));
	int32_t *version;
	if (bits_per_block == 0.0 || bits_per_block == 1.0) {
		version = BITARRAY_V1;
	} else if (bits_per_block == 2.0) {
		version = BITARRAY_V2;
	} else if (bits_per_block == 3.0) {
		version = BITARRAY_V3;
	} else if (bits_per_block == 4.0) {
		version = BITARRAY_V4;
	} else if (bits_per_block == 5.0) {
		version = BITARRAY_V5;
	} else if (bits_per_block == 6.0) {
		version = BITARRAY_V6;
	} else if (bits_per_block == 7.0 || bits_per_block == 8.0) {
		version = BITARRAY_V8;
	} else if (bits_per_block > 8.0) {
		version = BITARRAY_V16;
	}
	put_unsigned_byte((uint8_t) ((version[0] << 1) | 1), stream);
	int32_t position = 0;
	int32_t i;
	for (i = 0; i < version[2]; ++i) {
		uint32_t word = 0;
		int32_t ii;
		for (ii = 0; ii < version[1]; ++ii) {
			if (position >= 4096) {
				break;
			}
			uint32_t state = value->blocks[position];
			word |= state << ((uint32_t) (version[0] * ii));
			++position;
		}
		put_unsigned_int_le(word, stream);
	}
	put_signed_var_int(value->palette_size, stream);
	for (i = 0; i < value->palette_size; ++i) {
		put_signed_var_int(value->palette[i], stream);
	}
}

void put_misc_sub_chunk(sub_chunk_t *value, binary_stream_t *stream)
{
	put_unsigned_byte(SUB_CHUNK_VERSION, stream);
	put_unsigned_byte(value->block_storages_count, stream);
	uint8_t i;
	for (i = 0; i < value->block_storages_count; ++i) {
		put_misc_block_storage((&(value->block_storages[i])), stream);
	}
}

void put_misc_chunk(chunk_t *value, uint32_t sub_chunk_count, binary_stream_t *stream)
{
	uint32_t i;
	for (i = 0; i < sub_chunk_count; ++i) {
		put_misc_sub_chunk((&(value->sub_chunks[i])), stream);
	}
	for (i = 0; i < 25; ++i) {
		put_misc_block_storage((&(value->biomes[i])), stream);
	}
}

void put_misc_uuid(unsigned char *uuid, binary_stream_t *stream) {
    uint8_t *uuid_bytes = (uint8_t*)uuid;
    uint8_t le_uuid_bytes[16];
    size_t i;
    for (i = 0; i < 16; i++) {
        le_uuid_bytes[i] = uuid_bytes[15 - i];
    }
    stream->buffer = (int8_t*) realloc(stream->buffer, stream->size + 16);
    memcpy(stream->buffer + stream->size, le_uuid_bytes, 16);
    stream->size += 16;
}

void put_misc_skin_image(misc_skin_image_t image, binary_stream_t *stream){
	put_int_le(image.width, stream);
	put_int_le(image.height, stream);
	put_misc_string_var_int(image.data, stream);
}

void put_misc_skin_animation(misc_skin_animation_t animation, binary_stream_t *stream){
	put_misc_skin_image(animation.image, stream);
	put_int_le(animation.type, stream);
	put_float_le(animation.frames, stream);
	put_int_le(animation.expression_type, stream);
}

void put_misc_skin_persona(misc_skin_persona_t persona, binary_stream_t *stream){
	put_misc_string_var_int(persona.piece_id, stream);
	put_misc_string_var_int(persona.piece_type, stream);
	put_misc_string_var_int(persona.pack_id, stream);
	put_unsigned_byte(persona.is_piece_default, stream);
	put_misc_string_var_int(persona.product_id, stream);
}

void put_misc_skin_persona_tint_color(misc_skin_persona_tint_color_t persona_tint_color, binary_stream_t *stream){
	put_misc_string_var_int(persona_tint_color.piece_type, stream);
	put_int_le(persona_tint_color.color_count, stream);
	uint32_t i;
	for (i = 0; i < persona_tint_color.color_count; i++){
		put_misc_string_var_int(persona_tint_color.colors[i], stream);
	}
}

void put_misc_skin(misc_skin_t skin, binary_stream_t *stream, int32_t protocol) {
	put_misc_string_var_int(skin.skin_id, stream);
	put_misc_string_var_int(skin.play_fab_id, stream);
	put_misc_string_var_int(skin.resources_patch, stream);
	put_misc_skin_image(skin.skin_image, stream);
	put_int_le(skin.animation_count, stream);
	uint32_t i;
	for (i = 0; i < skin.animation_count; i++){
		put_misc_skin_animation(skin.animations[i], stream);
	}
	put_misc_skin_image(skin.cape_image, stream);
	put_misc_string_var_int(skin.geometry_data, stream);
	put_misc_string_var_int(skin.geometry_engine_version, stream);
	put_misc_string_var_int(skin.animation_data, stream);
	put_misc_string_var_int(skin.cape_id, stream);
	put_misc_string_var_int(skin.full_skin_id, stream);
	put_misc_string_var_int(skin.arm_size, stream);
	put_misc_string_var_int(skin.skin_color, stream);
	put_int_le(skin.persona_count, stream);
	for (i = 0; i < skin.persona_count; i++){
		put_misc_skin_persona(skin.personas[i], stream);
	}
	put_int_le(skin.persona_tint_color_count, stream);
	for (i = 0; i < skin.persona_tint_color_count; i++){
		put_misc_skin_persona_tint_color(skin.persona_tint_colors[i], stream);
	}
	put_unsigned_byte(skin.is_premium, stream);
	put_unsigned_byte(skin.is_persona, stream);
	put_unsigned_byte(skin.is_persona_cape_classic, stream);
	put_unsigned_byte(skin.is_primary_user, stream);
	if(protocol >= 568){
		put_unsigned_byte(skin.override, stream);
	}
}
