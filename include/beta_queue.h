/******************************************************************************
 * Copyright (c) 2010 Joey Degges
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

#ifndef LOOMLIB_BETA_QUEUE_H
#define LOOMLIB_BETA_QUEUE_H

/* 
 * A FIFO queue of items.
 * This queue maintains internal locking which allows any number of threads to
 * attempt concurrent push and pop. This queue is built with a similar
 * architecture as ALPHA_QUEUE in that it allows two threads to simultaneously
 * push and pop (any more will spin or block).
 */

#include <stdbool.h>
#include <stddef.h>


struct beta_queue;


/*
 * Create an empty queue.
 * Returns NULL on failure (out of memory).
 */
struct beta_queue *
beta_queue_new (void);

/*
 * Free all memory associated with the queue.
 * All internal memory will be unallocated. If the queue is not empty then it
 * is possible for item pointers to leak.
 */
void
beta_queue_free (struct beta_queue *queue);

/*
 * Enqueue an item.
 * ITEM may be NULL.
 */
bool
beta_queue_push (struct beta_queue *queue, void *item);

/*
 * Dequeue an item.
 * Popping an empty queue gives NULL.
 */
void *
beta_queue_pop (struct beta_queue *queue);


/*
 * NOTE:
 * It is not possible to maintain any accurate item count efficiently since the
 * queue was designed to allow concurrent pop and push operations. If you would
 * like to test for the empty queue then use the pop function and test for NULL.
 */

#endif
