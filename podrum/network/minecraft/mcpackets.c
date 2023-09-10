#include <podrum/network/minecraft/mcpackets.h>
#include <czlibhelper/zlib_helper.h>
#include <stdlib.h>


packet_game_t get_packet_game(binary_stream_t *stream)
{
	packet_game_t game;
	game.streams = (binary_stream_t *) malloc(0);
	game.streams_count = 0;
	stream->offset += stream->size;
	zlib_buf_t in;
	in.data = (Bytef *) (stream->buffer + 1);
	in.size = (uLong) (stream->size - 1);
	zlib_buf_t out;
	int status = zlib_decode(in, &out, ZLIB_RAW_MODE);
	if (status == Z_DATA_ERROR) {
		out.size = in.size;
		out.data = (Bytef *) malloc(out.size);
		memcpy(out.data, in.data, out.size);
	}
	binary_stream_t data_stream;
	data_stream.buffer = (int8_t *) out.data;
	data_stream.size = out.size;
	data_stream.offset = 0;
	while (data_stream.offset < data_stream.size) {
		binary_stream_t entry_stream;
		entry_stream.size = get_var_int(&data_stream);
		entry_stream.buffer = get_bytes(entry_stream.size, &data_stream);
		entry_stream.offset = 0;
		++game.streams_count;
		game.streams = (binary_stream_t *) realloc(game.streams, game.streams_count * sizeof(binary_stream_t));
		game.streams[game.streams_count - 1] = entry_stream;
	}
	free(data_stream.buffer);
	return game;
}

packet_login_t get_packet_login(binary_stream_t *stream)
{
	packet_login_t login;
	login.protocol_version = get_int_be(stream);
	binary_stream_t temp_stream = get_misc_byte_array_var_int(stream);
	login.tokens = get_misc_login_tokens(&temp_stream);
	free(temp_stream.buffer);
	return login;
}

packet_request_network_setting_t get_packet_request_network_setting(binary_stream_t *stream)
{
	packet_request_network_setting_t request;
	request.protocol_version = get_int_be(stream);
	return request;
}

packet_resource_pack_client_response_t get_packet_resource_pack_client_response(binary_stream_t *stream)
{
	packet_resource_pack_client_response_t resource_pack_client_response;
	resource_pack_client_response.response_status = get_unsigned_byte(stream);
	resource_pack_client_response.resource_pack_ids = get_misc_resource_pack_ids(stream);
	return resource_pack_client_response;
}

packet_text_t get_packet_text(binary_stream_t *stream)
{
	packet_text_t text;
	text.type = get_unsigned_byte(stream);
	text.needs_translation = get_unsigned_byte(stream);
	switch (text.type){
		case TEXT_CHAT:
		case TEXT_WHISPER:
		case TEXT_ANNOUNCEMENT:
			text.source_name = get_misc_string_var_int(stream);
		case TEXT_RAW:
		case TEXT_TIP:
		case TEXT_SYSTEM:
		case TEXT_JSON_WHISPER:
		case TEXT_JSON:
		case TEXT_JSON_ANNOUNCEMENT:
			text.message = get_misc_string_var_int(stream);
			break;
		case TEXT_TRANSLATION:
		case TEXT_POPUP:
		case TEXT_JUKEBOX_POPUP:
			text.message = get_misc_string_var_int(stream);
			text.parameter_count = get_var_int(stream);
			uint32_t i;
			for(i = 0; i < text.parameter_count; ++i){
				text.parameters[i] = get_misc_string_var_int(stream);
			}
			break;
	}
	text.xbox_userId = get_misc_string_var_int(stream);
	text.platform_chat_id = get_misc_string_var_int(stream);
	return text;
}

packet_interact_t get_packet_interact(binary_stream_t *stream)
{
	packet_interact_t interact;
	interact.action_id = get_unsigned_byte(stream);
	interact.entity_runtime_id = get_var_long(stream);
	if (interact.action_id == INTERACT_LEAVE_VEHICLE || interact.action_id == INTERACT_MOUSE_OVER_ENTITY) {
		interact.position_x = get_float_le(stream);
		interact.position_y = get_float_le(stream);
		interact.position_z = get_float_le(stream);
	}
	return interact;
}

packet_container_open_t get_packet_container_open(binary_stream_t *stream)
{
	packet_container_open_t container_open;
	container_open.window_id = get_byte(stream);
	container_open.window_type = get_byte(stream);
	container_open.coordinates_x = get_signed_var_int(stream);
	container_open.coordinates_y = get_var_int(stream);
	container_open.coordinates_z = get_signed_var_int(stream);
	container_open.entity_unique_id = get_signed_var_long(stream);
	return container_open;
}

packet_container_close_t get_packet_container_close(binary_stream_t *stream)
{
	packet_container_close_t container_close;
	container_close.window_id = get_byte(stream);
	container_close.server = get_unsigned_byte(stream);
	return container_close;
}

packet_move_player_t get_packet_move_player(binary_stream_t *stream)
{
	packet_move_player_t move_player;
	move_player.runtime_id = get_var_int(stream);
	move_player.position_x = get_float_le(stream);
	move_player.position_y = get_float_le(stream);
	move_player.position_z = get_float_le(stream);
	move_player.pitch = get_float_le(stream);
	move_player.yaw = get_float_le(stream);
	move_player.head_yaw = get_float_le(stream);
	move_player.mode = get_unsigned_byte(stream);
	move_player.on_ground = get_unsigned_byte(stream);
	move_player.ridden_runtime_id = get_var_int(stream);
	if (move_player.mode == MOVE_PLAYER_MODE_TELEPORT) {
		move_player.teleport_cause = get_int_le(stream);
		move_player.teleport_source_entity_type = get_int_le(stream);
	}
	move_player.tick = get_var_long(stream);
	return move_player;
}

packet_request_chunk_radius_t get_packet_request_chunk_radius(binary_stream_t *stream)
{
	packet_request_chunk_radius_t request_chunk_radius;
	request_chunk_radius.chunk_radius = get_signed_var_int(stream);
	return request_chunk_radius;
}

packet_player_skin_t get_packet_player_skin(binary_stream_t *stream)
{
	packet_player_skin_t player_skin;
	player_skin.skin_uuid = get_uuid_le(stream);
	player_skin.skin_data = get_misc_skin(stream);
	player_skin.new_skin_name = get_misc_string_var_int(stream);
	player_skin.old_skin_name = get_misc_string_var_int(stream);
	player_skin.verified = get_unsigned_byte(stream);
	return player_skin;
}

packet_request_ability_t get_packet_request_ability(binary_stream_t *stream)
{
	packet_request_ability_t request_ability;
	request_ability.ability_id = get_signed_var_int(stream);
	request_ability.ability_type = get_unsigned_byte(stream);
	request_ability.ability_bool = get_unsigned_byte(stream);
	request_ability.ability_float = get_float_le(stream);
	return request_ability;
}

void put_packet_game(packet_game_t packet, binary_stream_t *stream, uint8_t iscompress)
{
	binary_stream_t temp_stream;
	temp_stream.buffer = (int8_t *) malloc(0);
	temp_stream.offset = 0;
	temp_stream.size = 0;
	size_t i;
	for (i = 0; i < packet.streams_count; ++i){
		put_var_int(packet.streams[i].size, &temp_stream);
		put_bytes(packet.streams[i].buffer, packet.streams[i].size, &temp_stream);
	}
	put_unsigned_byte(ID_GAME, stream);
	if(iscompress == 1){
		zlib_buf_t in;
		in.data = (Bytef *) temp_stream.buffer;
		in.size = (uLong) temp_stream.size;
		zlib_buf_t out;
		zlib_encode(in, &out, 7, ZLIB_RAW_MODE);
		free(temp_stream.buffer);
		put_bytes((int8_t *) out.data, (size_t) out.size, stream);
		free(out.data);	
	}else{
		put_bytes(temp_stream.buffer, temp_stream.size, stream);
		free(temp_stream.buffer);
	}
}

void put_packet_play_status(packet_play_status_t packet, binary_stream_t *stream)
{
	put_var_int(ID_PLAY_STATUS, stream);
	put_int_be(packet.status, stream);
}

void put_packet_disconnect(char *msg, binary_stream_t *stream)
{
	put_var_int(ID_DISCONNECT, stream);
	put_unsigned_byte(msg[0] == '\0' ? 1 : 0, stream);
	if( msg[0] != '\0' ){
		put_misc_string_var_int(msg, stream);
	}
}

void put_packet_resource_packs_info(packet_resource_packs_info_t packet, binary_stream_t *stream)
{
	put_var_int(ID_RESOURCE_PACKS_INFO, stream);
	put_unsigned_byte(packet.must_accept, stream);
	put_unsigned_byte(packet.has_scripts, stream);
	put_unsigned_byte(packet.force_server_packs, stream);
	put_misc_behavior_pack_infos(packet.behavior_packs, stream);
	put_misc_texture_pack_infos(packet.texture_packs, stream);
	// put_var_int(0, stream);//TODO: PBR stuff
}

void put_packet_resource_pack_stack(packet_resource_pack_stack_t packet, binary_stream_t *stream)
{
	put_var_int(ID_RESOURCE_PACK_STACK, stream);
	put_unsigned_byte(packet.must_accept, stream);
	put_misc_resource_pack_id_versions(packet.behavior_packs, stream);
	put_misc_resource_pack_id_versions(packet.resource_packs, stream);
	put_misc_string_var_int(packet.game_version, stream);
	put_misc_experiments(packet.experiments, stream);
	put_unsigned_byte(packet.experiments_previously_used, stream);
}

void put_packet_resource_pack_client_response(packet_resource_pack_client_response_t packet, binary_stream_t *stream)
{
	put_var_int(ID_RESOURCE_PACK_CLIENT_RESPONSE, stream);
	put_unsigned_byte(packet.response_status, stream);
	put_misc_resource_pack_ids(packet.resource_pack_ids, stream);
}

void put_packet_text(packet_text_t packet, binary_stream_t *stream){
	put_var_int(ID_TEXT, stream);
	put_unsigned_byte(packet.type, stream);
	put_unsigned_byte(packet.needs_translation, stream);
	switch (packet.type){
		case TEXT_CHAT:
		case TEXT_WHISPER:
		case TEXT_ANNOUNCEMENT:
			put_misc_string_var_int(packet.source_name, stream);
		case TEXT_RAW:
		case TEXT_TIP:
		case TEXT_SYSTEM:
		case TEXT_JSON_WHISPER:
		case TEXT_JSON:
		case TEXT_JSON_ANNOUNCEMENT:
			put_misc_string_var_int(packet.message, stream);
			break;
		case TEXT_TRANSLATION:
		case TEXT_POPUP:
		case TEXT_JUKEBOX_POPUP:
			put_misc_string_var_int(packet.message, stream);
			put_var_int(packet.parameter_count, stream);
			uint32_t i;
			for(i = 0; i < packet.parameter_count; ++i){
				put_misc_string_var_int(packet.parameters[i], stream);
			}
			break;
	}
	put_misc_string_var_int(packet.xbox_userId, stream);
	put_misc_string_var_int(packet.platform_chat_id, stream);
}

void put_packet_set_time(int32_t time, binary_stream_t *stream)
{
	put_var_int(ID_SET_TIME, stream);
	put_signed_var_int(time, stream);
}

void put_packet_start_game(packet_start_game_t packet, binary_stream_t *stream)
{
	put_var_int(ID_START_GAME, stream);
	put_signed_var_long(packet.entity_unique_id, stream);
	put_var_long(packet.entity_runtime_id, stream);
	put_signed_var_int(packet.player_gamemode, stream);
	put_float_le(packet.player_x, stream);
	put_float_le(packet.player_y, stream);
	put_float_le(packet.player_z, stream);
	put_float_le(packet.pitch, stream);
	put_float_le(packet.yaw, stream);
	put_long_le(packet.seed, stream);
	put_short_le(packet.biome_type, stream);
	put_misc_string_var_int(packet.biome_name, stream);
	put_signed_var_int(packet.dimension, stream);
	put_signed_var_int(packet.generator, stream);
	put_signed_var_int(packet.world_gamemode, stream);
	put_signed_var_int(packet.difficulty, stream);
	put_signed_var_int(packet.spawn_x, stream);
	put_var_int(packet.spawn_y, stream);
	put_signed_var_int(packet.spawn_z, stream);
	put_unsigned_byte(packet.achievements_disabled, stream);
	put_unsigned_byte(0, stream);//editor
	put_unsigned_byte(1, stream);//createineditor
	put_unsigned_byte(0, stream);//exportededitor
	put_signed_var_int(packet.day_cycle_stop_time, stream);
	put_signed_var_int(packet.edu_offer, stream);
	put_unsigned_byte(packet.edu_features_enabled, stream);
	put_misc_string_var_int(packet.edu_product_uuid, stream);
	put_float_le(packet.rain_level, stream);
	put_float_le(packet.lightning_level, stream);
	put_unsigned_byte(packet.has_confirmed_platform_locked_content, stream);
	put_unsigned_byte(packet.is_multiplayer, stream);
	put_unsigned_byte(packet.broadcast_to_lan, stream);
	put_var_int(packet.xbox_live_broadcast_mode, stream);
	put_var_int(packet.platform_broadcast_mode, stream);
	put_unsigned_byte(packet.enable_commands, stream);
	put_unsigned_byte(packet.are_texture_packs_required, stream);
	put_misc_game_rules(packet.gamerules, stream);
	put_misc_experiments(packet.experiments, stream);
	put_unsigned_byte(packet.experiments_previously_used, stream);
	put_unsigned_byte(packet.bonus_chest, stream);
	put_unsigned_byte(packet.map_enabled, stream);
	put_signed_var_int(packet.permission_level, stream);
	put_int_le(packet.server_chunk_tick_range, stream);
	put_unsigned_byte(packet.has_locked_behavior_pack, stream);
	put_unsigned_byte(packet.has_locked_texture_pack, stream);
	put_unsigned_byte(packet.is_from_locked_world_template, stream);
	put_unsigned_byte(packet.msa_gamertags_only, stream);
	put_unsigned_byte(packet.is_from_world_template, stream);
	put_unsigned_byte(packet.is_world_template_option_locked, stream);
	put_unsigned_byte(packet.only_spawn_v1_villagers, stream);
	put_unsigned_byte(1, stream);//disablepersona
	put_unsigned_byte(0, stream);//disablecustomskin
	put_unsigned_byte(1, stream);//muteemote
	put_misc_string_var_int(packet.game_version, stream);
	put_int_le(packet.limited_world_width, stream);
	put_int_le(packet.limited_world_length, stream);
	put_unsigned_byte(packet.is_new_nether, stream);
	put_misc_education_shared_resource_uri(packet.edu_resource_uri, stream);
	put_unsigned_byte(packet.experimental_gameplay_override, stream);
	put_unsigned_byte(0, stream);//chatrestriction
	put_unsigned_byte(0, stream);//noplayerinteractions
	put_misc_string_var_int(packet.level_id, stream);
	put_misc_string_var_int(packet.world_name, stream);
	put_misc_string_var_int(packet.premium_world_template_id, stream);
	put_unsigned_byte(packet.is_trial, stream);
	put_signed_var_int(packet.movement_authority, stream);
	put_signed_var_int(packet.rewind_history_size, stream);
	put_unsigned_byte(packet.server_authoritative_block_breaking, stream);
	put_long_le(packet.current_tick, stream);
	put_signed_var_int(packet.enchantment_seed, stream);
	put_misc_block_properties(packet.block_properties, stream);
	put_misc_item_states(packet.item_states, stream);
	put_misc_string_var_int(packet.multiplayer_correlation_id, stream);
	put_unsigned_byte(packet.server_authoritative_inventory, stream);
	put_misc_string_var_int(packet.engine, stream);
	put_byte(COMPOUND_TAG, stream);
	put_var_int(0, stream);
	put_byte(END_TAG, stream);
	put_unsigned_long_le(packet.block_pallete_checksum, stream);
	put_misc_uuid((unsigned char *)"0000000000000000", stream);
	put_unsigned_byte(0, stream);//enableclientsidegen
	put_unsigned_byte(0, stream);//blockhash
	put_unsigned_byte(0, stream);//serversidesound
}

/*void put_packet_add_player(binary_stream_t *stream)
{
	put_var_int(ID_ADD_PLAYER, stream);
	put_misc_uuid(, stream);
	put_misc_string_var_int(, stream);
	put_var_long(, stream);
	put_misc_string_var_int(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_misc_item(, stream);
	put_signed_var_int(, stream);
	//metadata
	put_property_sync_data(, stream);
}*/

/*void put_packet_add_entity(binary_stream_t *stream)
{
	put_var_int(ID_ADD_ENTITY, stream);
	put_signed_var_long(entity_unique_id, stream);
	put_var_long(, stream);
	put_misc_string_var_int(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_float_le(, stream);
	put_var_int(, stream);
	for (size_t i = 0; i < count; i++)
	{
		put_misc_string_var_int(attributes[i].id, stream);
		put_float_le(attributes[i].min, stream);
		put_float_le(attributes[i].current, stream);
		put_float_le(attributes[i].max, stream);
	}
	//metadata

}*/

void put_packet_biome_definition_list(packet_biome_definition_list_t packet, binary_stream_t *stream)
{
	put_var_int(ID_BIOME_DEFINITION_LIST, stream);
	put_misc_nbt_tag(packet.nbt, stream);
}

void put_packet_available_entity_identifiers(packet_available_entity_identifiers_t packet, binary_stream_t *stream)
{
	put_var_int(ID_AVAILABLE_ENTITY_IDENTIFIERS, stream);
	put_misc_nbt_tag(packet.nbt, stream);
}

void put_packet_creative_content(packet_creative_content_t packet, binary_stream_t *stream)
{
	put_var_int(ID_CREATIVE_CONTENT, stream);
	put_var_int(packet.size, stream);
	uint32_t i;
	for (i = 0; i < packet.size; ++i) {
		put_var_int(packet.entry_ids[i], stream);
		put_misc_item(packet.items[i], 0, stream);
	}
}

void put_packet_update_attributes(packet_update_attributes_t packet, binary_stream_t *stream)
{
	put_var_int(ID_UPDATE_ATTRIBUTES_PACKET, stream);
	put_var_long(packet.entity_runtime_id, stream);
	put_var_int(packet.size, stream);
	uint32_t i;
	for (i = 0; i < packet.size; ++i) {
		put_misc_attributes(packet.attributes[i], stream);
	}
	put_var_long(packet.tick, stream);
}

void put_packet_interact(packet_interact_t packet, binary_stream_t *stream)
{
	put_var_int(ID_INTERACT, stream);
	put_unsigned_byte(packet.action_id, stream);
	put_var_long(packet.entity_runtime_id, stream);
	if (packet.action_id == INTERACT_LEAVE_VEHICLE || packet.action_id == INTERACT_MOUSE_OVER_ENTITY) {
		put_float_le(packet.position_x, stream);
		put_float_le(packet.position_y, stream);
		put_float_le(packet.position_z, stream);
	}
}

void put_packet_container_open(packet_container_open_t packet, binary_stream_t *stream)
{
	put_var_int(ID_CONTAINER_OPEN, stream);
	put_byte(packet.window_id, stream);
	put_byte(packet.window_type, stream);
	put_signed_var_int(packet.coordinates_x, stream);
	put_var_int(packet.coordinates_y, stream);
	put_signed_var_int(packet.coordinates_z, stream);
	put_signed_var_long(packet.entity_unique_id, stream);
}

void put_packet_container_close(packet_container_close_t packet, binary_stream_t *stream)
{
	put_var_int(ID_CONTAINER_CLOSE, stream);
	put_byte(packet.window_id, stream);
	put_var_int(packet.server, stream);
}

void put_packet_move_player(packet_move_player_t packet, binary_stream_t *stream)
{
	put_var_int(ID_MOVE_PLAYER, stream);
	put_var_int(packet.runtime_id, stream);
	put_float_le(packet.position_x, stream);
	put_float_le(packet.position_y, stream);
	put_float_le(packet.position_z, stream);
	put_float_le(packet.pitch, stream);
	put_float_le(packet.yaw, stream);
	put_float_le(packet.head_yaw, stream);
	put_unsigned_byte(packet.mode, stream);
	put_unsigned_byte(packet.on_ground, stream);
	put_var_int(packet.ridden_runtime_id, stream);
	if (packet.mode == MOVE_PLAYER_MODE_TELEPORT) {
		put_int_le(packet.teleport_cause, stream);
		put_int_le(packet.teleport_source_entity_type, stream);
	}
	put_var_long(packet.tick, stream);
}

void put_packet_move_entity(packet_move_entity_t packet, binary_stream_t *stream)
{
	put_var_int(ID_MOVE_ENTITY, stream);
	put_var_int(packet.runtime_id, stream);
	put_unsigned_byte(packet.flags, stream);
	put_float_le(packet.position_x, stream);
	put_float_le(packet.position_y, stream);
	put_float_le(packet.position_z, stream);
	put_unsigned_byte((uint8_t)(packet.pitch / 1.40625), stream);
	put_unsigned_byte((uint8_t)(packet.yaw / 1.40625), stream);
	put_unsigned_byte((uint8_t)(packet.head_yaw / 1.40625), stream);
}

void put_packet_player_skin(packet_player_skin_t packet, binary_stream_t *stream)
{
	put_var_int(ID_PLAYER_SKIN, stream);
	put_misc_uuid(packet.skin_uuid, stream);
	put_misc_skin(packet.skin_data, stream);
	put_misc_string_var_int(packet.new_skin_name, stream);
	put_misc_string_var_int(packet.old_skin_name, stream);
	put_unsigned_byte(packet.verified, stream);
}

void put_packet_network_chunk_publisher_update(packet_network_chunk_publisher_update_t packet, binary_stream_t *stream, int32_t protocol)
{
	put_var_int(ID_NETWORK_CHUNK_PUBLISHER_UPDATE, stream);
	put_signed_var_int(packet.x, stream);
	put_var_int(packet.y, stream);
	put_signed_var_int(packet.z, stream);
	put_var_int(packet.radius, stream);
	put_int_le(0, stream);//saveed chunk
}

void put_packet_request_chunk_radius(packet_request_chunk_radius_t packet, binary_stream_t *stream)
{
	put_var_int(ID_REQUEST_CHUNK_RADIUS, stream);
	put_signed_var_int(packet.chunk_radius, stream);
}

void put_packet_chunk_radius_updated(packet_chunk_radius_updated_t packet, binary_stream_t *stream)
{
	put_var_int(ID_CHUNK_RADIUS_UPDATED, stream);
	put_signed_var_int(packet.chunk_radius, stream);
}

void put_packet_level_chunk(packet_level_chunk_t packet, binary_stream_t *stream)
{
	put_var_int(ID_LEVEL_CHUNK, stream);
	put_signed_var_int(packet.x, stream);
	put_signed_var_int(packet.z, stream);
	put_var_int(packet.sub_chunk_count, stream);
	if (packet.sub_chunk_count == 0xfffffffe) {
		put_unsigned_short_le(packet.highest_subchunk_count, stream);
	}
	put_unsigned_byte(packet.cache_enabled, stream);
	if (packet.cache_enabled) {
		put_var_int(packet.hashes_count, stream);
		uint32_t i;
		for (i = 0; i < packet.hashes_count; ++i) {
			put_unsigned_long_le(packet.hashes[i], stream);
		}
	}
	put_misc_byte_array_var_int(packet.payload, stream);
}

void put_packet_network_setting(binary_stream_t *stream)
{
	put_var_int(ID_NETWORK_SETTINGS, stream);
	put_short_le(1, stream);
	put_short_le(0, stream);
	put_unsigned_byte(0, stream);
	put_unsigned_byte(0, stream);
	put_float_le(0, stream);
}