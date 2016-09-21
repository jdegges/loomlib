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

#define _GNU_SOURCE

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>

#include "async_queue.h"
#include "barrier.h"
#include "thread_pool.h"

struct work_unit
{
  void (*func)(void *data);
  void *data;
};

struct thread_pool
{
  struct async_queue *work_queue;
  struct async_queue *thread_queue;

  size_t num_threads;
  pthread_mutex_t lock;
};

static void THREAD_POOL_TERM_SIG (void *data) { data = data; }

static void *
thread_loop (void *args)
{
  struct thread_pool *pool = args;
  struct work_unit *work;

  while (NULL != (work = async_queue_pop (pool->work_queue, true)))
    {
      if (THREAD_POOL_TERM_SIG == work->func)
        {
          pthread_mutex_lock (&pool->lock);

          if (1 < pool->num_threads)
            thread_pool_push (pool, THREAD_POOL_TERM_SIG, NULL);

          pool->num_threads--;

          pthread_mutex_unlock (&pool->lock);
          free (work);

          return NULL;
        }

      work->func (work->data);

      free (work);
    }

  return NULL;
}

struct thread_pool *
thread_pool_new (size_t max_threads)
{
  struct thread_pool *pool = malloc (sizeof *pool);
  pool->work_queue = async_queue_new(),
  pool->thread_queue = async_queue_new();
  pool->num_threads = max_threads;
  pthread_mutex_init (&pool->lock, NULL);

  while (max_threads--)
    {
      pthread_t *thread = malloc (sizeof *thread);
      pthread_create (thread, NULL, thread_loop, pool);
      async_queue_push (pool->thread_queue, thread);
    }

  return pool;
}

void
thread_pool_free (struct thread_pool *pool)
{
  while (0 < async_queue_count (pool->thread_queue))
    {
      pthread_t *thread = async_queue_pop (pool->thread_queue, true);
      pthread_join (*thread, NULL);
      free (thread);
    }

  assert (0 == async_queue_count (pool->work_queue));
  async_queue_free (pool->work_queue);
  
  assert (0 == async_queue_count (pool->thread_queue));
  async_queue_free (pool->thread_queue);

  pthread_mutex_destroy (&pool->lock);
  free (pool);
}

bool
thread_pool_push (struct thread_pool *pool,
                  void(*func)(void *data),
                  void *data)
{
  struct work_unit *work = malloc (sizeof *work);
  work->func = func,
  work->data = data;

  return async_queue_push (pool->work_queue, work);
}

bool
thread_pool_terminate (struct thread_pool *pool)
{
  return thread_pool_push (pool, THREAD_POOL_TERM_SIG, NULL);
}

static void
barrier_thread (void *args)
{
  loomlib_barrier_t *barrier = args;
  loomlib_barrier_wait (barrier);
}

bool
thread_pool_barrier_wait (struct thread_pool *pool)
{
  size_t max_threads = pool->num_threads;
  loomlib_barrier_t* barrier;

  pthread_mutex_lock (&pool->lock);

  loomlib_barrier_init (barrier, NULL, pool->num_threads + 1);

  while (max_threads--)
    thread_pool_push (pool, barrier_thread, &barrier);

  loomlib_barrier_wait (barrier);
  loomlib_barrier_destroy (barrier);

  pthread_mutex_unlock (&pool->lock);

  return true;
}
