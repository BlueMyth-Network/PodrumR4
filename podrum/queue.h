

#ifndef PODRUM_QUEUE_H
#define PODRUM_QUEUE_H

#include <podrum/debug.h>
#include <podrum/worker.h>
#include <stdlib.h>

typedef struct {
	worker_mutex_t lock;
	void **items;
	size_t items_count;
} queue_t;

queue_t new_queue();

void *get_queue(queue_t *queue);

void put_queue(void *data, queue_t *queue);

#endif