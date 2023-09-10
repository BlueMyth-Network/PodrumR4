
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/asn1.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <stdbool.h>
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
#include <podrum/misc/uuid.h>

void handle_packet_login(binary_stream_t *stream, connection_t *connection, raknet_server_t *server, minecraft_player_manager_t *player_manager)
{
	minecraft_player_t *player = get_minecraft_player_address(connection->address, player_manager);
	if (player == NULL) {
	    return;
	}
	packet_login_t login = get_packet_login(stream);
	json_input_t json_input;
	json_input.json = login.tokens.identity;
	json_input.offset = 0;
	json_root_t json_root = parse_json_root(&json_input);
	json_root_t chain = get_json_object_value("chain", json_root.entry.json_object);
	bool is_mojang = false;
	uint8_t kicktype = -1;
	unsigned char *current_public_key = NULL;
	size_t i;
	for (i = 0; i < chain.entry.json_array.size; ++i) {
		jwt_data_t jwtchain = jwt_decode(get_json_array_value(i, chain.entry.json_array).entry.json_string);
		uint32_t stat = verifly_jwt_chain(&jwtchain, &current_public_key, &i);
		if((stat & (1 << 9)) == 0){
			kicktype = 0;
			if(stat == 0 || (stat & 1) != 0 || (stat & (1 << 2)) != 0 || (stat & (1 << 3)) != 0 || (stat & (1 << 4)) != 0 || (stat & (1 << 7)) != 0){
				kicktype = 1;
				break;
			}
			if((stat & (1 << 6)) != 0){
				kicktype = 2;
				break;
			}
			if((stat & (1 << 8)) != 0){
				kicktype = 3;
				break;
			}
		}
		if(!is_mojang){
			is_mojang = (stat & (1 << 1)) != 0;
		}
		if(i == 2){
			json_root_t value = get_json_object_value("extraData", jwtchain.payload.entry.json_object);
			if (value.type != JSON_OBJECT) {
				kicktype = 1;
				break;
			}
			player->identity = get_json_object_value("identity", value.entry.json_object).entry.json_string;
			player->display_name = get_json_object_value("displayName", value.entry.json_object).entry.json_string;
			player->xuid = get_json_object_value("XUID", value.entry.json_object).entry.json_string;
			player->title_id = get_json_object_value("titleId", value.entry.json_object).entry.json_string;
		}
	}
	destroy_json_root(json_root);
	free(login.tokens.identity);
	if((kicktype != (uint8_t)-1) || !is_mojang){
		binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
		streams[0].buffer = (int8_t *) malloc(0);
		streams[0].size = 0;
		streams[0].offset = 0;
		switch (kicktype){
			case 0:
				put_packet_disconnect("invalid login", (&(streams[0])));
				break;
			case 1:
				put_packet_disconnect("invalid login payload", (&(streams[0])));
				break;
			case 2:
				put_packet_disconnect("invalid login signature", (&(streams[0])));
				break;
			case 3:
				put_packet_disconnect("invalid login timestamp", (&(streams[0])));
				break;
		}
		if(!is_mojang && (kicktype == (uint8_t)-1)){
			put_packet_disconnect("You need to authenticate to Xbox Live.", (&(streams[0])));
		}
		send_minecraft_packet(streams, 1, connection, server, 1);
		free(streams[0].buffer);
		free(streams);
		return;
	}
	jwt_data_t client_data = jwt_decode(login.tokens.client);
	free(login.tokens.client);
	i++;
	uint32_t stat = verifly_jwt_chain(&client_data, &current_public_key, &i);
	if((stat & (1 << 9)) == 0){
		binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
		streams[0].buffer = (int8_t *) malloc(0);
		streams[0].size = 0;
		streams[0].offset = 0;
		put_packet_disconnect("invalid skin payload", (&(streams[0])));
		send_minecraft_packet(streams, 1, connection, server, 1);
		free(streams[0].buffer);
		free(streams);
		return;
	}
	// json_object_t client_payload = client_data.payload.entry.json_object;
	// for (size_t i = 0; i < client_payload.size; i++){
	// 	printf("name %s \n", client_payload.keys[i]);
	// 	printf("type %d \n", (int)client_payload.types[i]);
	// }
	/*
	arc4random_buf(connection->salt, 16UL);//make random salt
	connection->key_combined = make_combined_key(current_public_key, server, connection->salt);
	aes_key_setup(connection, connection->key_combined);//setup aes
	connection->encryption_enabled = (uint8_t)make_and_send_hand_shake(connection, server);
	*/
	if(!connection->encryption_enabled){
		play_success(connection, server, player);
	}
}

int make_and_send_hand_shake(connection_t *connection, raknet_server_t *server){//working
    size_t der_length;
    BIO *bio = BIO_new(BIO_s_mem());
    if (!i2d_PUBKEY_bio(bio, server->server_key)) {
        BIO_free(bio);
        return 0;
    }
    der_length = (size_t)BIO_pending(bio);
    unsigned char * der_string = (unsigned char *)malloc(der_length);
    if (!der_string) {
        BIO_free(bio);
        return 0;
    }
    if (BIO_read(bio, der_string, (int)der_length) != der_length) {
        free(der_string);
        BIO_free(bio);
        return 0;
    }
    BIO_free(bio);
	size_t out_length;
	unsigned char *new_salt = char_base64_encode(connection->salt, strlen((char *)connection->salt), &out_length);
	size_t json_salt_len = strlen("{\"salt\":\"\"}") + out_length + 1;
    char json_salt[json_salt_len];
    snprintf(json_salt, json_salt_len, "{\"salt\":\"%s\"}", (char *)new_salt);
	free(new_salt);
	size_t actual_salt_len;
	unsigned char *actual_salt = char_base64_url_encode((unsigned char *)json_salt, json_salt_len -1, &actual_salt_len);
	unsigned char *x5u = char_base64_encode(der_string, der_length, &out_length);
	free(der_string);
	size_t raw_body_len = strlen("{\"x5u\":\"\",\"alg\":\"ES384\"}") + out_length + 1;
    char raw_body[raw_body_len];
    snprintf(raw_body, raw_body_len, "{\"x5u\":\"%s\",\"alg\":\"ES384\"}", (char *)x5u);
	free(x5u);
	size_t body_hack_len;
	unsigned char *body_hack = char_base64_url_encode((unsigned char *)raw_body, raw_body_len -1, &body_hack_len);
	size_t hack_body_len = body_hack_len + actual_salt_len + 2;
    char hack_body[hack_body_len];
    snprintf(hack_body, hack_body_len, "%s.%s", (char *)body_hack, (char *)actual_salt);
	free(body_hack);
	free(actual_salt);
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	if (!mdctx) {
		fprintf(stderr, "Error creating signature context.\n");
		ERR_print_errors_fp(stderr);
		return 0;
	}
	if (EVP_DigestSignInit(mdctx, NULL, EVP_sha384(), NULL, server->server_key) != 1) {
		fprintf(stderr, "Error initializing signing operation.\n");
		ERR_print_errors_fp(stderr);
		return 0;
	}
	if (EVP_DigestSignUpdate(mdctx, hack_body, hack_body_len-1) != 1) {
		fprintf(stderr, "Error updating signing operation.\n");
		ERR_print_errors_fp(stderr);
		return 0;
	}
	size_t sigLength;
	if (EVP_DigestSignFinal(mdctx, NULL, &sigLength) != 1) {
		fprintf(stderr, "Error getting signature length.\n");
		ERR_print_errors_fp(stderr);
		return 0;
	}
	unsigned char *signature = (unsigned char *)malloc(sigLength);
	if (!signature) {
		fprintf(stderr, "Error allocating memory for signature.\n");
		return 0;
	}
	if (EVP_DigestSignFinal(mdctx, signature, &sigLength) != 1) {
		fprintf(stderr, "Error finalizing signature.\n");
		ERR_print_errors_fp(stderr);
		return 0;
	}
	ECDSA_SIG *parsedSignature = d2i_ECDSA_SIG(NULL, (const unsigned char **)&signature, (long)sigLength);
	if (!parsedSignature) {
		fprintf(stderr, "Error!.\n");
		return 0;
	}
	EVP_MD_CTX_free(mdctx);
	const BIGNUM *pr = ECDSA_SIG_get0_r(parsedSignature);
	const BIGNUM *ps = ECDSA_SIG_get0_s(parsedSignature);
	free(parsedSignature);
	int rlen = BN_num_bytes(pr);
    int slen = BN_num_bytes(ps);
	size_t sig_len = 96;
	unsigned char sig[sig_len];
    BN_bn2bin(pr, sig + sig_len / 2 - rlen);
    BN_bn2bin(ps, sig + sig_len - slen);
	BN_free((BIGNUM *)pr);
	BN_free((BIGNUM *)ps);
	size_t signature_encoded_len;
	unsigned char *signature_encoded = char_base64_url_encode((unsigned char *)sig, sig_len, &signature_encoded_len);
	size_t final_jwt_len = hack_body_len + signature_encoded_len + 2;
    char final_jwt[final_jwt_len];
    snprintf(final_jwt, final_jwt_len, "%s.%s", (char *)hack_body, (char*)signature_encoded);
	free(signature_encoded);
	char jwt[final_jwt_len-1];
	memcpy(jwt, final_jwt, final_jwt_len);
	binary_stream_t *streams = (binary_stream_t *) malloc(sizeof(binary_stream_t));
	streams[0].buffer = (int8_t *) malloc(0);
	streams[0].size = 0;
	streams[0].offset = 0;
	put_var_int(ID_SERVER_TO_CLIENT_HANDSHAKE, &streams[0]);
	put_misc_string_var_int(jwt, &streams[0]);
	send_minecraft_packet(streams, 1, connection, server, 1);
	free(streams[0].buffer);
	free(streams);
	return 1;
}

void aes_key_setup(connection_t *connection, unsigned char *combined_key){//working
	unsigned char iv[16];//keysize
	memcpy(iv, combined_key, 12);//use 0-12
	memset(iv + 12, 0, 4);//set 13-14 to 0
	iv[15] = (unsigned char)2;//set 15 to 2
	EVP_EncryptInit(connection->encrypt_ctx, EVP_aes_256_ctr(), combined_key, iv);//setup encrypt
	EVP_DecryptInit(connection->decrypt_ctx, EVP_aes_256_ctr(), combined_key, iv);//setup decrypt
}

unsigned char *make_combined_key(unsigned char *player_key, raknet_server_t *server, unsigned char *salt){//idk might work or not ???
	size_t keylen = 48;//set key size
	unsigned char *derived_key = (unsigned char *) malloc(keylen);
	EVP_PKEY *player_public_key = d2i_PUBKEY(NULL, (const unsigned char **)&player_key, 120);//load player key
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(server->server_key, NULL);//create ctx
    if (!ctx) {//error
        EVP_PKEY_free(player_public_key);
		free(derived_key);
        return NULL;
    }
    if (EVP_PKEY_derive_init(ctx) <= 0 || EVP_PKEY_derive_set_peer(ctx, player_public_key) <= 0 || EVP_PKEY_derive(ctx, derived_key, &keylen) <= 0) {//combine key
        EVP_PKEY_free(player_public_key);
        EVP_PKEY_CTX_free(ctx);
		free(derived_key);
        return NULL;
    }
    EVP_PKEY_free(player_public_key);//clear
    EVP_PKEY_CTX_free(ctx);
    // Perform SHA-256 hash
    unsigned char *secretKeyBytes = (unsigned char *) malloc(32);
    unsigned int secretKeyLen;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(mdctx, salt, 16); //saltsize = 16
    EVP_DigestUpdate(mdctx, derived_key, keylen);
    EVP_DigestFinal_ex(mdctx, secretKeyBytes, &secretKeyLen);
	free(derived_key);
    EVP_MD_CTX_destroy(mdctx);
	return secretKeyBytes;
}

void play_success(connection_t *connection, raknet_server_t *server, minecraft_player_t *player){//working
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
	player->uuid = uuid_str_to_bin(player->identity, 1);
	printf("%s logged in with entity id %lld\n", player->display_name, player->entity_unique_id);
}

uint32_t verifly_jwt_chain(jwt_data_t *jwtchain, unsigned char **current_public_key, size_t *first){//working
	uint32_t status = 0;
	int64_t timern = (int64_t) time(NULL);
	json_object_t header_entry = jwtchain->header.entry.json_object;
	json_root_t x5u = get_json_object_value("x5u", header_entry);
	status |= (x5u.type != JSON_STRING);
	if(x5u.type != JSON_STRING){
		destroy_json_object(header_entry);
		destroy_json_root(x5u);
		return status;
	}
	status |= (strcmp(x5u.entry.json_string, MOJANG_ROOT_KEY) == 0) << 1;
	char *raw_public_key = x5u.entry.json_string;
	destroy_json_root(x5u);
	size_t out_len;//120
	unsigned char *header_der_key = char_base64_decode((unsigned char *)raw_public_key, strlen(raw_public_key), &out_len);
	destroy_json_object(header_entry);
	status |= (out_len != 120) << 2;//incorrect key size
	if(*current_public_key == NULL){
		if(*first != 0){
			free(header_der_key);
			status |= 1 << 3;//have no key
			return status;
		}
	}else if(strcmp((char *)header_der_key, (char *)*current_public_key) != 0){
		free(header_der_key);
		status |= 1 << 3;//key not same
		return status;
	}
	json_object_t payload_entry = jwtchain->payload.entry.json_object;
	size_t signature_size;//96
	unsigned char *raw_signature = char_base64_url_decode((unsigned char *)jwtchain->signature, strlen(jwtchain->signature), &signature_size);
	if(signature_size != 96){
		free(header_der_key);
		free(raw_signature);
		destroy_json_object(payload_entry);
		status |= 1 << 4;//invild sig size
		return status;
	}
	size_t combined_len = strlen(jwtchain->raw_header) + strlen(jwtchain->raw_payload) + 2;
	char *combined = (char *)malloc(combined_len * sizeof(char));
    snprintf(combined, combined_len, "%s.%s", jwtchain->raw_header, jwtchain->raw_payload);
	combined_len -= 1;
	unsigned char *new_sig = NULL;
    ECDSA_SIG * ecdsa_sig = ECDSA_SIG_new();
    BIGNUM * pr = BN_bin2bn(raw_signature, (int) signature_size / 2, NULL);
    BIGNUM * ps = BN_bin2bn(raw_signature + signature_size / 2, (int) signature_size / 2, NULL);
	free(raw_signature);
    ECDSA_SIG_set0(ecdsa_sig, pr, ps);
    int new_sig_len = i2d_ECDSA_SIG(ecdsa_sig, &new_sig);
	BN_free(pr);
	BN_free(ps);
	EVP_PKEY *public_key = d2i_PUBKEY(NULL, (const unsigned char **)&header_der_key, out_len);
	EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
	if (!md_ctx) {
		status |= 1 << 5;//error
		free(new_sig);
		destroy_json_object(payload_entry);
		EVP_PKEY_free(public_key);
		return status;
	}
	if (EVP_DigestVerifyInit(md_ctx, NULL, EVP_sha384(), NULL, public_key) != 1) {
		status |= 1 << 5;//error
		free(new_sig);
		destroy_json_object(payload_entry);
		EVP_PKEY_free(public_key);
		EVP_MD_CTX_free(md_ctx);
		return status;
	}
	if (EVP_DigestVerifyUpdate(md_ctx, combined, combined_len) != 1) {
		status |= 1 << 5;//error
		free(new_sig);
		destroy_json_object(payload_entry);
		EVP_PKEY_free(public_key);
		EVP_MD_CTX_free(md_ctx);
		return status;
	}
	int result = EVP_DigestVerifyFinal(md_ctx, new_sig, new_sig_len);
	free(new_sig);
	EVP_PKEY_free(public_key);
	EVP_MD_CTX_free(md_ctx);
	status |= (result != 1) << 6;//sig is invaild
	if(*first == (size_t)4){
		status |= 1 << 9; //done
		return status;
	}
	json_root_t identity_public_key = get_json_object_value("identityPublicKey", payload_entry);
	if (identity_public_key.type != JSON_STRING) {
		status |= 1 << 7;//no identityPublicKey
		destroy_json_root(identity_public_key);
		destroy_json_object(payload_entry);
		return status;
	}
	int64_t exp = get_json_object_value("exp", payload_entry).entry.json_number.number.int_number;
	int64_t nbf = get_json_object_value("nbf", payload_entry).entry.json_number.number.int_number;
	if(nbf > timern + 60 || exp < timern - 60){
		status |= 1 << 8;//clock diff
		destroy_json_root(identity_public_key);
		return status;
	}
	status |= 1 << 9;//done
	char *raw_identity_public_key = identity_public_key.entry.json_string;
	*current_public_key = char_base64_decode((unsigned char *)raw_identity_public_key, strlen(raw_identity_public_key), &out_len);
	destroy_json_root(identity_public_key);
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
		container_open.entity_unique_id = player->entity_unique_id;
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
	player->head_yaw = move_player.head_yaw;
}
