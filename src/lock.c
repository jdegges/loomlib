#include "lock.h"

#include <assert.h>
#include <pthread.h>


void
loomlib_lock_init (loomlib_lock_t *p)
{
#ifdef USE_MUTEX
  assert (0 == pthread_mutex_init (p, NULL));
#else
  assert (0 == pthread_spin_init (p, PTHREAD_PROCESS_SHARED));
#endif
}

void
loomlib_lock_destroy (loomlib_lock_t *p)
{
#ifdef USE_MUTEX
  assert (0 == pthread_mutex_destroy (p));
#else
  assert (0 == pthread_spin_destroy (p));
#endif
}

void
loomlib_lock_acquire (loomlib_lock_t *p)
{
#ifdef USE_MUTEX
  assert (0 == pthread_mutex_lock (p));
#else
  assert (0 == pthread_spin_lock (p));
#endif
}

void
loomlib_lock_release (loomlib_lock_t *p)
{
#ifdef USE_MUTEX
  assert (0 == pthread_mutex_unlock (p));
#else
  assert (0 == pthread_spin_unlock (p));
#endif
}

void
loomlib_cond_init (loomlib_cond_t *p)
{
#ifdef USE_MUTEX
  assert (0 == pthread_cond_init (p, NULL));
#else
  p = NULL;
#endif
}

void
loomlib_cond_destroy (loomlib_cond_t *p)
{
#ifdef USE_MUTEX
  assert (0 == pthread_cond_destroy (p));
#else
  (void) p;
#endif
}

void
loomlib_cond_wait (loomlib_cond_t *cond, loomlib_lock_t *lock)
{
#ifdef USE_MUTEX
  assert (0 == pthread_cond_wait (cond, lock));
#else
  (void) cond;
  (void) lock;
#endif
}

void
loomlib_cond_broadcast (loomlib_cond_t *p)
{
#ifdef USE_MUTEX
  assert (0 == pthread_cond_broadcast (p));
#else
  (void) p;
#endif
}
