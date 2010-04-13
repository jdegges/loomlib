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

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>

#include <loomlib/thread_pool.h>
#include <loomlib/pipeline.h>

struct pipeline
{
  struct thread_pool *pool;

  void *(*inlet)(void *data);
  void *inlet_data;

  void(*outlet)(void *data, void *product);
  void *outlet_data;

  void *(**pump)(void *data, void *product);
  void **pump_data;
  size_t num_pumps;
  size_t max_pumps;

  size_t max_threads;
  size_t active_lines;
  bool terminated;
  pthread_mutex_t lock;
};

struct pipeline_state
{
    struct pipeline *pipe;
    void *product;
    int current_stage;
};

struct pipeline *
pipeline_new (size_t max_threads)
{
  struct pipeline *pipe = calloc (1, sizeof *pipe);
  assert (pipe);

  pipe->pool = thread_pool_new (max_threads);
  assert (pipe->pool);

  pipe->max_threads = max_threads;
  pthread_mutex_init (&pipe->lock, NULL);

  return pipe;
}

void
pipeline_free (struct pipeline *pipe)
{
  assert (pipe);
  assert (pipe->pool);

  thread_pool_free (pipe->pool);
  pthread_mutex_destroy (&pipe->lock);

  free (pipe->pump_data);
  free (pipe->pump);
  free (pipe);
}

bool
pipeline_terminate (struct pipeline *pipe)
{
  assert (pipe);

  pthread_mutex_lock (&pipe->lock);
  pipe->terminated = true;
  pthread_mutex_unlock (&pipe->lock);
  return true;
}

bool
pipeline_add_inlet (struct pipeline *pipe,
                    void *(*routine)(void *data),
                    void *data)
{
  assert (pipe);

  pthread_mutex_lock (&pipe->lock);

  assert (!pipe->inlet);
  assert (!pipe->inlet_data);
  assert (routine);

  pipe->inlet = routine;
  pipe->inlet_data = data;

  pthread_mutex_unlock (&pipe->lock);
  return true;
}

bool
pipeline_add_outlet (struct pipeline *pipe,
                     void(*routine)(void *data, void *product),
                     void *data)
{
  assert (pipe);

  pthread_mutex_lock (&pipe->lock);

  assert (!pipe->outlet);
  assert (!pipe->outlet_data);
  assert (routine);

  pipe->outlet = routine;
  pipe->outlet_data = data;

  pthread_mutex_unlock (&pipe->lock);
  return true;
}

bool
pipeline_add_pump (struct pipeline *pipe,
                   void *(*routine)(void *data, void *product),
                   void *data)
{
  assert (pipe);
  assert (routine);

  pthread_mutex_lock (&pipe->lock);

  if (0 == pipe->max_pumps)
    {
      pipe->max_pumps = 1;
      pipe->pump = malloc (sizeof *pipe->pump);
      pipe->pump_data = malloc (sizeof *pipe->pump_data);
    }
  else if (pipe->max_pumps <= pipe->num_pumps)
    {
      pipe->max_pumps *= 2;

      pipe->pump = realloc (pipe->pump, (sizeof *pipe->pump) * pipe->max_pumps);
      assert (pipe->pump);

      pipe->pump_data = realloc (pipe->pump_data,
                                 (sizeof *pipe->pump_data) * pipe->max_pumps);
      assert (pipe->pump_data);
    }

  pipe->pump[pipe->num_pumps] = routine;
  pipe->pump_data[pipe->num_pumps] = data;
  pipe->num_pumps++;

  pthread_mutex_unlock (&pipe->lock);
  return true;
}

static void
pipeline_loop (void *data)
{
  struct pipeline_state *state = data;
  struct pipeline *pipe = state->pipe;
  int current_stage = state->current_stage;
  void *product = state->product;
  void *new_product;

  /* inject product from the inlet into the pipe */
  if (current_stage < 0)
    {
      pthread_mutex_lock (&pipe->lock);
      if (pipe->terminated)
        {
          pthread_mutex_unlock (&pipe->lock);
          free (state);
          return;
        }

      if (pipe->max_threads < pipe->active_lines)
        {
          pthread_mutex_unlock (&pipe->lock);
          thread_pool_push (pipe->pool, pipeline_loop, state);
          return;
        }
      pipe->active_lines++;
      pthread_mutex_unlock (&pipe->lock);

      new_product = pipe->inlet (pipe->inlet_data);

      /* if the inlet hasn't dried up, restart this inlet stage */
      if (new_product)
        {
          struct pipeline_state *new_state = malloc (sizeof *new_state);
          assert (new_state);

          new_state->pipe = pipe;
          new_state->product = NULL;
          new_state->current_stage = current_stage;
          thread_pool_push (pipe->pool, pipeline_loop, new_state);
        }
      /* if it has dried up then clean up resources */
      else
        {
          free (state);
          pthread_mutex_lock (&pipe->lock);
          if (0 == --pipe->active_lines)
            thread_pool_terminate (pipe->pool);
          pthread_mutex_unlock (&pipe->lock);
          return;
        }
    }
  /* pump product through the pipe */
  else if ((size_t) current_stage < pipe->num_pumps)
    {
      new_product = pipe->pump[current_stage] (pipe->pump_data[current_stage],
                                               product);
    }
  /* deposit product at the outlet */
  else
    {
      pipe->outlet (pipe->outlet_data, product);

      free (state);
      pthread_mutex_lock (&pipe->lock);
      if (0 == --pipe->active_lines)
        thread_pool_terminate (pipe->pool);
      pthread_mutex_unlock (&pipe->lock);
      return;
    }

  /* start up the next stage, passing the new product along */
  state->product = new_product;
  state->current_stage++;
  thread_pool_push (pipe->pool, pipeline_loop, state);
}

bool
pipeline_execute (struct pipeline *pipe)
{
  struct pipeline_state *state = malloc (sizeof *state);

  assert (state);
  assert (pipe);
  assert (pipe->pool);
  assert (pipe->inlet);
  assert (pipe->outlet);

  state->pipe = pipe;
  state->current_stage = -1;

  return thread_pool_push (pipe->pool, pipeline_loop, state);
}
