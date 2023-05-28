

#ifndef PODRUM_MISC_JSON_H
#define PODRUM_MISC_JSON_H

#include <stdint.h>
#include <stdlib.h>

#define JSON_OBJECT 0
#define JSON_ARRAY 1
#define JSON_STRING 2
#define JSON_NUMBER 3
#define JSON_BOOL 4
#define JSON_NULL 5
#define JSON_EMPTY 6

#define JSON_NUMBER_NAN 0
#define JSON_NUMBER_INT 1
#define JSON_NUMBER_FLOAT 2

typedef struct {
	char *json;
	int offset;
} json_input_t;

typedef union {
	int64_t int_number;
	double float_number;
} json_tmp_number_t;

typedef struct {
	char type;
	json_tmp_number_t number;
} json_number_t;

union _json_multi;

typedef union _json_multi json_multi_t;

typedef struct {
	char **keys;
	json_multi_t *members;
	char *types;
	size_t size;
	char noret;
} json_object_t;

typedef struct {
	json_multi_t *members;
	char *types;
	size_t size;
	char noret;
} json_array_t;

union _json_multi {
	json_object_t json_object;
	json_array_t json_array;
	char *json_string;
	json_number_t json_number;
	char json_bool;
	void *json_null;
};

typedef struct {
	json_multi_t entry;
	char type;
} json_root_t;

char *parse_json_string(json_input_t *json_input);

char parse_json_bool(json_input_t *json_input);

char parse_json_null(json_input_t *json_input);

json_number_t parse_json_number(json_input_t *json_input);

json_array_t parse_json_array(json_input_t *json_input);

json_object_t parse_json_object(json_input_t *json_input);

json_root_t parse_json_root(json_input_t *json_input);

void destroy_json_array(json_array_t json_array);

void destroy_json_object(json_object_t json_object);

void destroy_json_root(json_root_t json_root);

json_root_t get_json_object_value(char *key, json_object_t json_object);

json_root_t get_json_array_value(size_t index, json_array_t json_array);

#endif
