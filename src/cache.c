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

#include "cache.h"
#include "lock.h"

#include <stddef.h>
#include <stdlib.h>


struct cache
{
  size_t head;
  size_t tail;

  loomlib_lock_t head_lock;
  loomlib_lock_t tail_lock;

  size_t size;
  size_t nmemb;
  void **table;

  void *(*alloc)(size_t size);
  void (*free)(void *ptr);
};


struct cache *
cache_init (size_t nmemb,
            size_t size,
            void *(*alloc_ptr)(size_t size),
            void (*free_ptr)(void *ptr))
{
  struct cache *cache;
  void **table;

  if (0 == nmemb || 0 == size)
    return NULL;

  cache = malloc (sizeof *cache);
  if (NULL == cache)
    return NULL;

  table = calloc (nmemb, sizeof *table);
  if (NULL == table)
    return NULL;

  cache->head = 0;
  cache->tail = 0;
  cache->size = size;
  cache->nmemb = nmemb;
  cache->table = table;
  cache->alloc = alloc_ptr;
  cache->free = free_ptr;

  loomlib_lock_init (&cache->head_lock);
  loomlib_lock_init (&cache->tail_lock);

  return cache;
}

void
cache_destroy (struct cache *cache)
{
  loomlib_lock_destroy (&cache->tail_lock);
  loomlib_lock_destroy (&cache->head_lock);

  while (cache->head != cache->tail)
    cache->free (cache->table[cache->head++ % cache->nmemb]);

  free (cache->table);
  free (cache);
}

void *
cache_alloc (struct cache *cache)
{
  void *ptr;

  loomlib_lock_acquire (&cache->head_lock);

  if (NULL == cache->table[cache->head % cache->nmemb])
    {
      loomlib_lock_release (&cache->head_lock);
      ptr = cache->alloc (cache->size);
    }
  else
    {
      ptr = cache->table[cache->head % cache->nmemb];
      cache->table[cache->head++ % cache->nmemb] = NULL;
      loomlib_lock_release (&cache->head_lock);
    }

  return ptr;
}

void
cache_free (struct cache *cache, void *ptr)
{
  if (NULL == ptr)
    return;

  loomlib_lock_acquire (&cache->tail_lock);

  if (NULL == cache->table[cache->tail % cache->nmemb])
    {
      cache->table[cache->tail++ % cache->nmemb] = ptr;
      loomlib_lock_release (&cache->tail_lock);
    }
  else
    {
      loomlib_lock_release (&cache->tail_lock);
      cache->free (ptr);
    }
}
