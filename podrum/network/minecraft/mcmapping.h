#ifndef PODRUM_NETWORK_MINECRAFT_MCMAPPING_H
#define PODRUM_NETWORK_MINECRAFT_MCMAPPING_H

#include <podrum/debug.h>
#include <stdint.h>
#include <stdlib.h>
#include <podrum/network/minecraft/mcmisc.h>

typedef struct {
	char *name;
	uint8_t metadata;
} mapping_block_state_t;

typedef struct {
	mapping_block_state_t *entries;
	size_t size;
} mapping_block_states_t;

typedef struct {
	misc_item_t *entries;
	uint32_t size;
} mapping_creative_items_t;

mapping_block_state_t runtime_id_to_block_state(int32_t runtime_id, mapping_block_states_t states);

int32_t block_state_to_runtime_id(char *name, uint8_t metadata, mapping_block_states_t states);

char *runtime_id_to_item_state(int16_t runtime_id, misc_item_states_t states);

int16_t item_state_to_runtime_id(char *name, misc_item_states_t states);

#endif
