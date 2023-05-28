#ifndef PODRUM_MISC_RESOURCEMANAGER_H
#define PODRUM_MISC_RESOURCEMANAGER_H

#include <cbinarystream/binary_stream.h>
#include <cnbt/nbt.h>
#include <podrum/network/minecraft/mcmisc.h>
#include <podrum/network/minecraft/mcmapping.h>

typedef struct {
	nbt_named_t biome_definitions;
	nbt_named_t entity_identifiers;
	misc_item_states_t item_states;
	mapping_block_states_t block_states;
	mapping_creative_items_t creative_items;
} resources_t;

binary_stream_t read_file(char *path);

resources_t get_resources();

void destroy_resources(resources_t *resources);

#endif
