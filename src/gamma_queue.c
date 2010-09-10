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

#include "gamma_queue.h"
#include "lock.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>


struct node
{
	void *item;
  struct node *next;
};

struct gamma_queue
{
  struct node *head;
  struct node *tail;

  loomlib_lock_t head_lock;
  loomlib_lock_t tail_lock;
  loomlib_cond_t nonempty;
};


struct gamma_queue *
gamma_queue_new (void)
{
  struct gamma_queue *queue = calloc (1, sizeof *queue);
  struct node *node = calloc (1, sizeof *node);

  if (NULL == queue || NULL == node)
    return NULL;

  queue->head = node;
  queue->tail = node;

  loomlib_lock_init (&queue->head_lock);
  loomlib_lock_init (&queue->tail_lock);
  loomlib_cond_init (&queue->nonempty);
  return queue;
}

void
gamma_queue_free (struct gamma_queue *queue)
{
  if (queue)
    {
      while (queue->head)
        {
          struct node *next = queue->head->next;
          free (queue->head);
          queue->head = next;
        }
    }

  loomlib_lock_destroy (&queue->head_lock);
  loomlib_lock_destroy (&queue->tail_lock);
  loomlib_cond_destroy (&queue->nonempty);

  free (queue);
}

bool
gamma_queue_push (struct gamma_queue *queue, void *item)
{
  struct node *new;

  if (NULL == queue)
    return false;

	new = calloc (1, sizeof *new);
	if (new == NULL)
		return false;

  new->item = item;

  loomlib_lock_acquire (&queue->tail_lock);

  queue->tail->next = new;
  queue->tail = new;

  loomlib_lock_release (&queue->tail_lock);
  loomlib_cond_broadcast (&queue->nonempty);

	return true;
}

void *
gamma_queue_pop (struct gamma_queue *queue, bool wait)
{
  struct node *temp;
  void *item;

  if (NULL == queue)
    return NULL;

  loomlib_lock_acquire (&queue->head_lock);

  if (false == wait && NULL == queue->head->next)
    {
      loomlib_lock_release (&queue->head_lock);
      return NULL;
    }

  while (NULL == queue->head->next)
    loomlib_cond_wait (&queue->nonempty, &queue->head_lock);

  item = queue->head->next->item;
  temp = queue->head;
  queue->head = queue->head->next;

  loomlib_lock_release (&queue->head_lock);

  free (temp);

  return item;
}
