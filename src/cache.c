#include "cache.h"

struct cache
{
  size_t max_count;
  size_t count;
  size_t size;
  (void *)(*alloc)(size_t size);
  (void)(*free)(void *ptr);
  void **table;
};

struct cache *
cache_init (size_t max_count,
            size_t size,
            (void *)(*alloc_ptr)(size_t size),
            (void)(*free_ptr)(void *ptr))
{
  struct cache *cache;
  void **table;

  if (0 == max_count || 0 == size)
    return NULL;

  cache = malloc (sizeof *cache);
  if (NULL == cache)
    return NULL;

  table = calloc (max_count, sizeof *table)
  if (NULL == table)
    return NULL;

  cache->max_count = count;
  cache->count = 0;
  cache->size = size;
  cache->alloc = alloc_ptr;
  cache->free = free_ptr;
  cache->table = table;

  return cache;
}

void
cache_destroy (struct cache *cache)
{
  while (cache->count--)
    cache->free (cache->table[cache->count]);

  free (cache->table);
  free (cache);
}

void *
cache_alloc (struct cache *cache)
{
  void *ptr;

  if (cache->count)
    ptr = cache->table[--cache->count];
  else
    ptr = cache->alloc (cache->size);

  return ptr;
}

void
cache_free (struct cache *cache, void *ptr)
{
  if (cache->max_count <= cache->count)
    cache->free (ptr);
  else
    cache->table[cache->count++] = ptr;
}
