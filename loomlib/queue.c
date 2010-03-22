/*
 * Author: Katherine Flavel, kate@elide.org - http://elide.org/
 * Original source: http://svn.elide.org/adt/trunk/src/queue.c
 */

/* $Id$ */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <loomlib/queue.h>


struct queue {
	void *item;
	struct queue *prev;
};


bool
queue_push(struct queue **queue, void *item)
{
	struct queue *new;

	assert(queue != NULL);
	assert(item != NULL);

	new = malloc(sizeof *new);
	if (new == NULL) {
		return false;
	}

	new->item = item;

	if (*queue == NULL) {
		*queue = new->prev = new;
	} else {
		new->prev = (*queue)->prev;
		*queue = (*queue)->prev = new;
	}

	return true;
}

void *
queue_pop(struct queue **queue)
{
	assert(queue != NULL);

	if (*queue == NULL) {
		return NULL;
	}

	if ((*queue)->prev == *queue) {
		void *item;

		item = (*queue)->item;
		free(*queue);
		*queue = NULL;

		return item;
	} else {
		struct queue head;

		head = *(*queue)->prev;
		free((*queue)->prev);
		(*queue)->prev = head.prev;

		return head.item;
	}
}

size_t
queue_count(const struct queue *queue)
{
	const struct queue *curr;
	size_t i;

	if (queue == NULL) {
		return 0;
	}

	i = 0;
	for (curr = queue; curr->prev != queue; curr = curr->prev) {
		assert(curr->item != NULL);
		i++;
	}

	return i;
}

