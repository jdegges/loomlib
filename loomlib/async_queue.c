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

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>

#include <loomlib/queue.h>
#include <loomlib/async_queue.h>

struct async_queue {
  struct queue *queue;
  size_t size;
  pthread_mutex_t lock;
  pthread_cond_t nonempty;
  pthread_cond_t is_empty;
};

struct async_queue *
async_queue_new (void)
{
  struct async_queue *queue = malloc (sizeof *queue);

  if (queue == NULL)
    return NULL;

  queue->queue = NULL;
  queue->size = 0;
  pthread_mutex_init (&queue->lock, NULL);
  pthread_cond_init (&queue->nonempty, NULL);
  pthread_cond_init (&queue->is_empty, NULL);

  return queue;
}

void
async_queue_free (struct async_queue *queue)
{
  void *ptr;

  pthread_mutex_lock(&queue->lock);

  while (queue->size != 0)
    pthread_cond_wait (&queue->is_empty, &queue->lock);

  pthread_mutex_unlock(&queue->lock);

  pthread_cond_destroy(&queue->is_empty);
  pthread_cond_destroy(&queue->nonempty);
  pthread_mutex_destroy(&queue->lock);
  free (queue);
}

bool
async_queue_push(struct async_queue *queue, void *item)
{
  bool rv;

  pthread_mutex_lock(&queue->lock);

  if (rv = queue_push(&queue->queue, item))
    queue->size++;

  pthread_cond_broadcast(&queue->nonempty);
  pthread_mutex_unlock(&queue->lock);

  return rv;
}

void *
async_queue_pop(struct async_queue *queue, const bool wait)
{
  void *rv;
  pthread_mutex_lock(&queue->lock);

  while ((rv = queue_pop(&queue->queue)) == NULL && wait)
    pthread_cond_wait (&queue->nonempty, &queue->lock);

  if (rv)
    queue->size--;

  if (queue->size == 0)
    pthread_cond_broadcast(&queue->is_empty);

  pthread_mutex_unlock(&queue->lock);

  return rv;
}

size_t
async_queue_count(struct async_queue *queue)
{
  size_t rv;
  pthread_mutex_lock(&queue->lock);
  rv = queue->size;
  pthread_mutex_unlock(&queue->lock);
  return rv;
}
