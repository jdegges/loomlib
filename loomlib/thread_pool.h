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

#ifndef LOOMLIB_THREAD_POOL_H
#define LOOMLIB_THREAD_POOL_H

#include <stdbool.h>
#include <stddef.h>


/*
 * A thread pool.
 */
struct thread_pool;

/*
 * Create a new thread pool.
 * MAX_THREADS threads will be started.
 */
struct thread_pool *
thread_pool_new (size_t max_threads);

/*
 * Free a thread pool.
 * This will block until all of the threads have exited and there is no more
 * work to be done.
 */
void
thread_pool_free (struct thread_pool *pool);

/*
 * Push a new work unit into the pool.
 * EXEC_FUNC must not be NULL.
 */
bool
thread_pool_push (struct thread_pool *pool,
                  void(*exec_func)(void *data),
                  void *data);

/*
 * Will cause all threads to shut down nicely once all of the work has been
 * finished. No work pushed after this call will be done.
 */
bool
thread_pool_terminate (struct thread_pool *pool);

#endif
