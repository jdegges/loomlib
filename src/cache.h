#ifndef LOOMLIB_CACHE_H
#define LOOMLIB_CACHE_H

/*
 * A cache of pointers.
 */

#include <stdbool.h>
#include <stdint.h>


struct cache;


/*
 * Create a new cache.
 * COUNT is the total number of items that may be cached at one time. ALLOC is
 * a pointer to a function that can allocate a new object of SIZE bytes. FREE
 * is a pointer to a function that can free an object. Returns NULL on failure.
 */
struct cache *
cache_init (size_t count,
            size_t size,
            (void *)(*alloc)(size_t size),
            (void)(*free)(void *ptr));

/*
 * Release all memory associated with CACHE.
 */
void
cache_destroy (struct cache *cache);

/*
 * Get a pointer to a new object.
 * The returned object was allocated with the ALLOC that was passed to the
 * corresponding call to CACHE_INIT. Returns NULL on failure.
 */
void *
cache_alloc (struct cache *cache);

/*
 * Release an object.
 * If CACHE is full then the object will be freed using the FREE function
 * passed to the corresponding call to CACHE_INIT. Otherwise PTR will be saved
 * for reuse in the cache.
 */
void
cache_free (struct cache *cache, void *ptr);

#endif
