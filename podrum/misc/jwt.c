#include <podrum/misc/jwt.h>
#include <podrum/misc/base64.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

json_root_t jwt_decode(char *token){
	char *payload = (char *) malloc(0);
	size_t size = 0;
	uint8_t step = 0;
	size_t i;
	for (i = 0; i < strlen(token); ++i){
		if (token[i] == '.') {
			++step;
		} else if (step == 1) {
			++size;
			payload = (char *) realloc(payload, size);
			if (token[i] == '-') {
				payload[size - 1] = '+';
			} else if (token[i] == '_') {
				payload[size - 1] = '/';
			} else {
				payload[size - 1] = token[i];
			}
		} else if (step > 1) {
			break;
		}
	}
	++size;
	payload = (char *) realloc(payload, size);
	payload[size - 1] = 0;
	binary_stream_t stream = base64_decode(payload);
	put_unsigned_byte(0, &stream);
	json_input_t json_input;
	json_input.json = (char *) stream.buffer;
	json_input.offset = 0;
	free(payload);
	json_root_t json_root = parse_json_root(&json_input);
	free(stream.buffer);
	return json_root;
}

jwt_data_t test_jwt_decode(char *token){
	char *header = strtok(token, ".");
    char *payload = strtok(NULL, ".");
    char *signature = strtok(NULL, ".");
	jwt_data_t jwt;
	jwt.raw_header = malloc(strlen(header) + 1);
	strcpy(jwt.raw_header, header);
	jwt.raw_payload = malloc(strlen(payload) + 1);
	strcpy(jwt.raw_payload, payload);
	jwt.signature = signature;
	json_input_t header_json_input;
	header_json_input.json = (char *) base64_url_decode(header).buffer;
	header_json_input.offset = 0;
	json_root_t header_json_root = parse_json_root(&header_json_input);
	jwt.header = header_json_root;
	json_input_t payload_json_input;
	payload_json_input.json = (char *) base64_url_decode(payload).buffer;
	payload_json_input.offset = 0;
	json_root_t payload_json_root = parse_json_root(&payload_json_input);
	jwt.payload = payload_json_root;
	return jwt;
}
