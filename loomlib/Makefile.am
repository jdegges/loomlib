AM_LDFLAGS = -avoid-version -module -shared
lib_LTLIBRARIES = libloomlib.la
pkginclude_HEADERS = \
  queue.h            \
  async_queue.h      \
  thread_pool.h

libloomlib_la_SOURCES =  \
  queue.c                \
  async_queue.c          \
  thread_pool.c
libloomlib_la_LIBADD = $(PTHREAD_LIBS)
libloomlib_la_CFLAGS = $(PTHREAD_CFLAGS) $(AM_CFLAGS)
