#ifndef BARRIER_H_
#define BARRIER_H_

struct loomlib_barrier;
typedef struct loomlib_barrier loomlib_barrier_t;

int
loomlib_barrier_init(loomlib_barrier_t* restrict barrier,
                     const void* restrict attr, unsigned count);

int
loomlib_barrier_wait(loomlib_barrier_t* barrier);

int
loomlib_barrier_destroy(loomlib_barrier_t* barrier);

#endif
