#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "barrier.h"
#include "lock.h"

typedef struct loomlib_barrier {
  loomlib_lock_t lock;
  loomlib_cond_t cond;
  unsigned count;
  unsigned tripped;
} loomlib_barrier_t;

int
loomlib_barrier_init(loomlib_barrier_t* restrict barrier,
                     const void* restrict attr, unsigned count)
{
  barrier = malloc(sizeof *barrier);
  if (NULL == barrier) {
    return -ENOMEM;
  }

  loomlib_lock_init(&barrier->lock);
  loomlib_cond_init(&barrier->cond);
  barrier->count = count;
  barrier->tripped = 0;

  return 0;
}

int
loomlib_barrier_destroy(loomlib_barrier_t* barrier)
{
  if (NULL == barrier) {
    return -EINVAL;
  }

  loomlib_cond_destroy(&barrier->cond);
  loomlib_lock_destroy(&barrier->lock);
  memset(barrier, 0, sizeof *barrier);
  free(barrier);
  return 0;
}

int
loomlib_barrier_wait(loomlib_barrier_t* barrier)
{
  if (NULL == barrier) {
    return -EINVAL;
  }

  loomlib_lock_acquire(&barrier->lock);

  barrier->count++;

  if (barrier->count <= barrier->tripped) {
    barrier->tripped = 0;
    loomlib_cond_broadcast(&barrier->cond);
    loomlib_lock_release(&barrier->lock);
    return 1;
  } else {
    loomlib_cond_wait(&barrier->cond, &barrier->lock);
    loomlib_lock_release(&barrier->lock);
    return 0;
  }
}
