#include <podrum/misc/jwt.h>
#include <podrum/misc/base64.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

jwt_data_t jwt_decode(char *token){
	jwt_data_t jwt;
	jwt.raw_header = strtok(token, ".");
    jwt.raw_payload = strtok(NULL, ".");
    jwt.signature = strtok(NULL, ".");
	size_t dump;
	json_input_t header_json_input;
	header_json_input.json = (char *) base64_url_decode(jwt.raw_header).buffer;
	header_json_input.offset = 0;
	json_root_t header_json_root = parse_json_root(&header_json_input);
	jwt.header = header_json_root;
	json_input_t payload_json_input;
	payload_json_input.json = (char *) base64_url_decode(jwt.raw_payload).buffer;
	payload_json_input.offset = 0;
	json_root_t payload_json_root = parse_json_root(&payload_json_input);
	jwt.payload = payload_json_root;
	return jwt;
}