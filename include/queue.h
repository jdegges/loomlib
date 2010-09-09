/*
 * Author: Katherine Flavel, kate@elide.org - http://elide.org/
 * Original source: http://svn.elide.org/adt/trunk/include/adt/queue.h
 */

/* $Id$ */

#ifndef LOOMLIB_QUEUE_H
#define LOOMLIB_QUEUE_H

/* 
 * A FIFO queue of items items. An empty queue is NULL.
 * No locking is provided.
 */

#include <stdbool.h>
#include <stddef.h>


struct queue;


/*
 * Enqueue an item.
 * The item passed may not be NULL.
 */
bool
queue_push(struct queue **queue, void *item);


/*
 * Dequeue an item.
 * TODO: popping an empty queue ought to give NULL
 */
void *
queue_pop(struct queue **queue);


/*
 * Count the number of items in a queue.
 */
size_t
queue_count(const struct queue *queue);

#endif

