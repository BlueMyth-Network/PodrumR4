#ifndef PODRUM_WORLD_CHUNK_SUBCHUNK_H
#define PODRUM_WORLD_CHUNK_SUBCHUNK_H

#include <podrum/debug.h>
#include <podrum/world/chunk/blockstorage.h>

#define SUB_CHUNK_VERSION 8
#define SUB_CHUNK_LAYER_NOT_PRESENT -2

typedef struct {
	uint8_t block_storages_count;
	block_storage_t *block_storages;
} sub_chunk_t;

sub_chunk_t new_sub_chunk(int32_t air_runtime_id);

int32_t get_sub_chunk_block_runtime_id(uint8_t x, uint8_t y, uint8_t z, uint8_t layer, sub_chunk_t *sub_chunk);

void set_sub_chunk_block_runtime_id(uint8_t x, uint8_t y, uint8_t z, int32_t runtime_id, uint8_t layer, sub_chunk_t *sub_chunk);

int8_t get_sub_chunk_highest_block_at(uint8_t x, uint8_t z, uint8_t layer, sub_chunk_t *sub_chunk);

uint8_t is_empty_sub_chunk(sub_chunk_t *sub_chunk);

void destroy_sub_chunk(sub_chunk_t *sub_chunk);

#endif