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

#ifndef LOOMLIB_CACHE_H
#define LOOMLIB_CACHE_H

/*
 * A simple object cache.
 * This may be useful to use in situations where many objects of the same size
 * are rapidly allocated/freed. Instead of hammering malloc and free this
 * caches previously freed objects for reuse. Multiple threads may execute
 * CACHE_ALLOC and CACHE_FREE at the same time with very little synchronization
 * overhead.
 */

#include <stddef.h>


struct cache;


/*
 * Create a new cache.
 * NMEMB is the total number of objects that may be cached at one time. ALLOC
 * is a pointer to a function that can allocate a new object of SIZE bytes. FREE
 * is a pointer to a function that can free an object. Returns NULL on failure.
 */
struct cache *
cache_init (size_t nmemb,
            size_t size,
            void *(*alloc)(size_t size),
            void (*free)(void *ptr));

/*
 * Release all memory associated with CACHE.
 * Any objects currently cached will be freed with the corresponding FREE
 * function.
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
 * for reuse in the cache. If PTR is NULL then this is does nothing.
 */
void
cache_free (struct cache *cache, void *ptr);

#endif
