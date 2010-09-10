#ifndef LOOMLIB_LOCK_H
#define LOOMLIB_LOCK_H

#define _GNU_SOURCE

#include <pthread.h>

#define USE_MUTEX

#ifdef USE_MUTEX
typedef pthread_mutex_t loomlib_lock_t;
typedef pthread_cond_t loomlib_cond_t;
#else
typedef pthread_spinlock_t loomlib_lock_t;
typedef int loomlib_cond_t;
#endif

void
loomlib_lock_init (loomlib_lock_t *p);

void
loomlib_lock_destroy (loomlib_lock_t *p);

void
loomlib_lock_acquire (loomlib_lock_t *p);

void
loomlib_lock_release (loomlib_lock_t *p);

void
loomlib_cond_init (loomlib_cond_t *p);

void
loomlib_cond_destroy (loomlib_cond_t *p);

void
loomlib_cond_wait (loomlib_cond_t *cond, loomlib_lock_t *lock);

void
loomlib_cond_broadcast (loomlib_cond_t *p);

#endif
