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

#ifndef LOOMLIB_ASYNC_LIST_H
#define LOOMLIB_ASYNC_LIST_H

#include <stdint.h>

struct async_list;

/*
 * Create an empty list
 */
struct async_list *
async_list_new (void);

/*
 * Add an item to the list
 * The item may not be NULL.
 */
void
async_list_add (struct async_list *l, void *item);

/*
 * Retrieve item at position `i'
 */
void *
async_list_get (struct async_list *l, uint64_t i);

/*
 * Count the number of items in a list
 */
uint64_t
async_list_count (struct async_list *l);

/*
 * Free a list object
 */
void
async_list_free (struct async_list *l);

#endif
