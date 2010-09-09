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

#ifndef LOOMLIB_ASYNC_QUEUE_H
#define LOOMLIB_ASYNC_QUEUE_H

/* 
 * A FIFO queue of items.
 * Locking is provided.
 */

#include <stdbool.h>
#include <stddef.h>


struct async_queue;

/*
 * Create an empty queue.
 */
struct async_queue *
async_queue_new(void);

/*
 * Free a queue.
 * A queue should not be popped/pushed/counted after freeing.
 */
void
async_queue_free(struct async_queue *queue);

/*
 * Enqueue an item.
 * The item passed may not be NULL.
 */
bool
async_queue_push(struct async_queue *queue, void *item);

/*
 * Dequeue an item.
 * If wait is false then popping an empty queue returns NULL, otherwise the
 * calling thread will block until an item is pushed
 */
void *
async_queue_pop(struct async_queue *queue, const bool wait);

/*
 * Count the number of items in a queue.
 */
size_t
async_queue_count(struct async_queue *queue);

#endif

