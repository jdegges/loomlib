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

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <pthread.h>

#include "async_list.h"
#include "thread_pool.h"
#include "tree.h"

struct tree
{
  struct vertice *root;
  struct thread_pool *pool;
  size_t max_threads;
  size_t active_lines;
  bool terminated;
  pthread_mutex_t lock;
};

struct vertice
{
  struct async_list *children;
  void *(*routine)(void *data, void *product);
  void *data;
  pthread_mutex_t lock;
};

struct tree_state
{
  struct tree *tree;
  struct vertice *vertice;
  void *product;
};

struct tree *
tree_new (size_t max_threads)
{
  struct tree *tree = calloc (1, sizeof *tree);

  if (!tree)
    return NULL;

  tree->pool = thread_pool_new (max_threads);
  if (!tree->pool)
    {
      free (tree);
      return NULL;
    }

  tree->max_threads = max_threads;
  pthread_mutex_init (&tree->lock, NULL);
  return tree;
}

void
tree_free (struct tree *tree)
{
  assert (tree);
  assert (tree->pool);
  pthread_mutex_destroy (&tree->lock);
  thread_pool_free (tree->pool);
  free (tree);
}

bool
tree_terminate (struct tree *tree)
{
  if (!tree)
    return false;
  pthread_mutex_lock (&tree->lock);
  tree->terminated = true;
  pthread_mutex_unlock (&tree->lock);
  return true;
}

struct vertice *
tree_new_vertice (void *(*routine)(void *data, void *product),
                  void *data)
{
  struct vertice *vertice = calloc (1, sizeof *vertice);
  if (!vertice)
    return NULL;
  vertice->routine = routine;
  vertice->data = data;
  pthread_mutex_init (&vertice->lock, NULL);
  return vertice;
}

void
tree_free_vertice (struct vertice *vertice)
{
  assert (vertice);
  if (NULL != vertice->children)
    async_list_free (vertice->children);
  pthread_mutex_destroy (&vertice->lock);
  free (vertice);
}

bool
tree_add_root (struct tree *tree,
               struct vertice *root)
{
  if (!tree || !root)
    return false;
  pthread_mutex_lock (&tree->lock);
  if (tree->root)
    return false;
  tree->root = root;
  pthread_mutex_unlock (&tree->lock);
  return true;
}

bool
tree_add_child (struct vertice *parent,
                struct vertice *child)
{
  if (!parent || !child)
    return false;
  pthread_mutex_lock (&parent->lock);
  if (!parent->children)
    parent->children = async_list_new ();
  if (!parent->children)
    return false;
  async_list_add (parent->children, child);
  pthread_mutex_unlock (&parent->lock);
  return true;
}

static void
tree_loop (void *data)
{
  struct tree_state *state = data;
  struct tree *tree = state->tree;
  struct vertice *vertice = state->vertice;
  void *product = state->product;
  void *new_product = NULL;

  assert (tree);
  assert (vertice);

  if (vertice == tree->root)
    {
      pthread_mutex_lock (&tree->lock);
      if (tree->terminated)
        {
          pthread_mutex_unlock (&tree->lock);
          free (state);
          return;
        }

      /* poor mans cond_wait */
      if (tree->max_threads < tree->active_lines)
        {
          pthread_mutex_unlock (&tree->lock);
          thread_pool_push (tree->pool, tree_loop, state);
          return;
        }

      tree->active_lines++;
      pthread_mutex_unlock (&tree->lock);
    }

  /* execute this vertice */
  new_product = vertice->routine (vertice->data, product);

  /* non-terminal verticies may return NULL and that NULL pointer will still be
   * passed to any children */
  if (NULL != vertice->children)
    {
      uint64_t i;
      for (i = 0; i < async_list_count (vertice->children); i++)
        {
          struct tree_state *new_state = calloc (1, sizeof *new_state);
          new_state->tree = tree;
          new_state->vertice = async_list_get (vertice->children, i);
          new_state->product = new_product;
          thread_pool_push (tree->pool, tree_loop, new_state);
        }

      /* restart root vertice */
      if (vertice == tree->root)
        thread_pool_push (tree->pool, tree_loop, state);
      else
        free (state);
      return;
    }

  /* here the vertice is surely terminal, hence NEW_PRODUCT should be NULL */
  free (state);
  pthread_mutex_lock (&tree->lock);
  if (0 == --tree->active_lines)
    thread_pool_terminate (tree->pool);
  pthread_mutex_unlock (&tree->lock);
}

bool
tree_execute (struct tree *tree)
{
  struct tree_state *state = calloc (1, sizeof *state);

  if (!state || !tree || !tree->root || !tree->pool)
    return false;

  state->tree = tree;
  state->vertice = tree->root;
  state->product = NULL;

  return thread_pool_push (tree->pool, tree_loop, state);
}
