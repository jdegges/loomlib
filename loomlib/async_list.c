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
#include <stdint.h>

#include <assert.h>
#include <pthread.h>

#include <async_list.h>

struct async_list
{
  void **data;
  uint64_t count;
  uint64_t size;
  pthread_mutex_t lock;
};

struct async_list *
async_list_new (void)
{
  struct async_list *l = calloc (1, sizeof (struct async_list));
  if (NULL == l)
    return NULL;
  pthread_mutex_init (&l->lock, NULL);
  return l;
}

void
async_list_add (struct async_list *l, void *item)
{
  assert (l);
  pthread_mutex_lock (&l->lock);
  if (l->size <= l->count)
    {
      l->size = l->size ? l->size * 2 : 2;
      assert (l->data = realloc (l->data, sizeof (void *) * l->size));
    }
  l->data[l->count++] = item;
  pthread_mutex_unlock (&l->lock);
}

void *
async_list_get (struct async_list *l, uint64_t i)
{
  void *p;
  assert (l);
  pthread_mutex_lock (&l->lock);
  assert (l->data);
  assert (l->count <= i);
  p = l->data[i];
  pthread_mutex_unlock (&l->lock);
  return p;
}

uint64_t
async_list_count (struct async_list *l)
{
  uint64_t p;
  assert (l);
  pthread_mutex_lock (&l->lock);
  p = l->count;
  pthread_mutex_unlock (&l->lock);
  return p;
}

void
async_list_free (struct async_list *l)
{
  pthread_mutex_destroy (&l->lock);
  free (l->data);
  free (l);
}
