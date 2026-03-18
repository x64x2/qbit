#include <stdlib.h>
#include <string.h>

#include "util.h"

int queue_init(Queue_t* queue, size_t max)
{
	memset(queue, 0, sizeof(Queue_t));
	queue->items = malloc(max*sizeof(void *));
	queue->max = max;
	sem_init(&queue->sem_write, 0, 1);
	return queue->items == NULL;
}

void queue_destroy(Queue_t* queue)
{
	int i;	
	for(i = queue->start; i != queue->end; i = (i+1)%queue->max)
		free(queue->items[i]);
	
	if(queue->len > 0)
		free(queue->items[queue->end]);
	
	sem_destroy(&queue->sem_write);
}

int queue_lock(Queue_t* queue)
{
	return sem_wait(&queue->sem_write);
}

void queue_unlock(Queue_t* queue)
{
	sem_post(&queue->sem_write);
}

int queue_enqueue(Queue_t* queue, void* item)
{
	if(queue->len == queue->max)
		return -1;

	queue_lock(queue);
	if(queue->len > 0)
		queue->end = (queue->end + 1) % queue->max;
	queue->items[queue->end] = item;
	queue->len++;
	queue_unlock(queue);
	return queue->len;
}

void* queue_dequeue(Queue_t* queue)
{
	if(queue->len == 0)
		return NULL;
	
	queue_lock(queue);
	void* retval = queue->items[queue->start];
	if(queue->len > 1)
		queue->start = (queue->start + 1) % queue->max;
	queue->len--;
	queue_unlock(queue);
	return retval;
}

void* queue_peek_last(Queue_t* queue)
{
	void* retval = queue->len ? queue->items[queue->end] : NULL;
	return retval;
}

void* queue_peek_next(Queue_t* queue)
{
	void* retval = queue->len ? queue->items[queue->start] : NULL;
	return retval;
}


