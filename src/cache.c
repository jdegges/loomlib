#include "beta_queue.h"
#include "cache.h"

struct cache
{
  size_t max_count;
  size_t count;
  size_t size;
  (void *)(*alloc)(size_t size);
  (void)(*free)(void *ptr);
  struct beta_queue *queue;
};

struct cache *
cache_init (size_t max_count,
            size_t size,
            (void *)(*alloc_ptr)(size_t size),
            (void)(*free_ptr)(void *ptr))
{
  struct cache *cache;
  struct beta_queue *queue;

  if (0 == size)
    return NULL;

  cache = malloc (sizeof *cache);
  if (NULL == cache)
    return NULL;

  queue = beta_queue_new ();
  if (NULL == queue)
    return NULL;

  cache->max_count = count;
  cache->count = 0;
  cache->size = size;
  cache->alloc = alloc_ptr;
  cache->free = free_ptr;
  cache->queue = queue;

  return cache;
}

void
cache_destroy (struct cache *cache)
{
}

void *
cache_alloc (struct cache *cache)
{
  void *ptr = beta_queue_pop (cache->queue);

  if (NULL == ptr)
    ptr = cache->alloc (cache->size);

  return ptr;
}

void
cache_free (struct cache *cache, void *ptr)
{
  if (0 < cache->max_count && cache->max_count <= cache->count)
    cache->free (ptr);
  else
    {
      beta_queue_push (cache->queue, ptr);
      cache->count++;
    }
}
