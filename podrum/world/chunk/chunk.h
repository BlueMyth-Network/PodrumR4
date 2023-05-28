#ifndef PODRUM_WORLD_CHUNK_CHUNK_H
#define PODRUM_WORLD_CHUNK_CHUNK_H

#include <podrum/debug.h>
#include <podrum/world/chunk/subchunk.h>

#define CHUNK_EMPTY -3
#define CHUNK_SUB_CHUNK_SIZE 25

typedef struct {
	int32_t x;
	int32_t z;
	sub_chunk_t *sub_chunks;
	block_storage_t *biomes;
} chunk_t;

chunk_t new_chunk(int32_t x, int32_t z, int32_t air_runtime_id);

int32_t get_chunk_block_runtime_id(uint8_t x, uint8_t y, uint8_t z, uint8_t layer, chunk_t *chunk);

int32_t get_chunk_biome_runtime_id(uint8_t x, uint8_t y, uint8_t z, uint8_t layer, chunk_t *chunk);

void set_chunk_biome_runtime_id(uint8_t x, uint8_t y, uint8_t z, int32_t runtime_id, uint8_t layer, chunk_t *chunk);

void set_chunk_block_runtime_id(uint8_t x, uint8_t y, uint8_t z, int32_t runtime_id, uint8_t layer, chunk_t *chunk);

int32_t get_chunk_highest_block_at(uint8_t x, uint8_t z, uint8_t layer, chunk_t *chunk);

uint8_t is_empty_chunk(chunk_t *chunk);

uint32_t get_chunk_sub_chunk_send_count(chunk_t *chunk);

void destroy_chunk(chunk_t *chunk);

#endif