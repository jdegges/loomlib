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

#ifndef LOOMLIB_PIPELINE_H
#define LOOMLIB_PIPELINE_H

#include <stdbool.h>
#include <stddef.h>

#include <loomlib/thread_pool.h>

/*
 * A thread-safe pipeline.
 */
struct pipeline;

/*
 * Create a new pipeline.
 * MAX_THREADS threads will be started.
 */
struct pipeline *
pipeline_new (size_t max_threads);

/*
 * Free a pipeline.
 * This will block until the pipe dries up (when the INLET routine returns NULL
 * and all of the PRODUCT has passed through the system).
 */
void
pipeline_free (struct pipeline *pipe);

/*
 * Act as if the INLET routine returned NULL. This may cause memory leaks if
 * INLET resources are not properly unallocated after the pipeline dries up.
 */
bool
pipeline_terminate (struct pipeline *pipe);

/*
 * Setup the beginning of the pipe, where the product is injected into the
 * system. ROUTINE should return a PRODUCT pointer that is passed to subsequent
 * pumps and outlets.
 * You must add exactly ONE inlet to every pipeline.
 */
bool
pipeline_add_inlet (struct pipeline *pipe, void *(*routine)(void *data), void *data);

/*
 * Setup the endpoint of the pipe, where the product leaves the pipe.
 * PRODUCT should be unallocated properly since it will no longer be
 * referenced.
 * You must add exactly ONE outlet to every pipeline.
 */
bool
pipeline_add_outlet (struct pipeline *pipe, void(*routine)(void *data, void *product), void *data);

/*
 * Setup a pump to move the product through the pipe.
 * You may add as many pumps as necessary (including zero). Pumps will be
 * executed in the order in which they were added.
 */
bool
pipeline_add_pump (struct pipeline *pipe, void *(*routine)(void *data, void *product), void *data);

/*
 * Start running the pipeline.
 * This will return false if either an INLET or OUTLET has not been setup.
 */
bool
pipeline_execute (struct pipeline *pipe);

#endif
