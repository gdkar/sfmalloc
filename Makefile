
CC      = gcc
CXX     = g++
AR      = ar
RANLIB  = ranlib

OBJS = sf_malloc.o sf_malloc_new.o sf_malloc_wrapper.o sf_malloc_init.o
SHARED_OBJS = $(patsubst %.o,%_shared.o, $(OBJS))
DEBUG_OBJS  = $(patsubst %.o, %_debug.o, $(OBJS))
#SHARED_OBJS = sf_malloc_shared.o sf_malloc_new_shared.o sf_malloc_init_shared.o \
#              sf_malloc_wrapper_shared.o
LIBS = -lpthread -lrt -ldl
LIB_MALLOC = libsfmalloc.a libsfmalloc.so libsfmalloc-debug.so

OPT_FLAGS = -O2 -Ofast -Wall -g -ggdb -pthread -shared -fPIC \
  -march=native \
  -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc \
  -fno-builtin-calloc -fno-builtin-cfree \
  -fno-builtin-memalign -fno-builtin-posix_memalign \
  -fno-builtin-valloc -fno-builtin-pvalloc -fno-exceptions
INC_FLAGS = 
DEFS += -D_REENTRANT
#DEFS += -D_REENTRANT -DNDEBUG

%_debug.o: DEFS += -DMALLOC_DEBUG -DMALLOC_STATS -UMALLOC_USE_STATIC_LINKING \
		-DMALLOC_NEED_INIT -DMALLOC_NEED_THREAD_INIT
%_shared.o: DEFS += -DMALLOC_NEED_THREAD_INIT -DMALLOC_NEED_INIT -UMALLOC_USE_STATIC_LINKING

CFLAGS   = -std=gnu11 $(OPT_FLAGS) $(INC_FLAGS) $(DEFS) 
CXXFLAGS = -std=gnu++14 $(OPT_FLAGS) $(INC_FLAGS) $(DEFS)

all: $(LIB_MALLOC)

sf_malloc.o: sf_malloc.c sf_malloc.h sf_malloc_def.h sf_malloc_ctrl.h sf_malloc_atomic.h
	$(CC) $(CFLAGS) -DMALLOC_NEED_INIT -DMALLOC_USE_STATIC_LINKING -c $<

sf_malloc_shared.o: sf_malloc.c sf_malloc.h sf_malloc_def.h sf_malloc_ctrl.h sf_malloc_atomic.h
	$(CC) $(CFLAGS) -DPIC -fPIC -c $< -o $@

sf_malloc_debug.o: sf_malloc.c sf_malloc.h sf_malloc_def.h sf_malloc_ctrl.h sf_malloc_atomic.h
	$(CC) $(CFLAGS) -DPIC -fPIC -c $< -o $@

sf_malloc_wrapper.o: sf_malloc_wrapper.c
	$(CC) $(CFLAGS) -c $<

sf_malloc_wrapper_shared.o: sf_malloc_wrapper.c
	$(CC) $(CFLAGS) -DPIC -fPIC -c $< -o $@
sf_malloc_wrapper_debug.o: sf_malloc_wrapper.c
	$(CC) $(CFLAGS) -DPIC -fPIC -c $< -o $@

sf_malloc_new.o: sf_malloc_new.cpp
	$(CXX) $(CXXFLAGS) -c $<

sf_malloc_new_shared.o: sf_malloc_new.cpp
	$(CXX) $(CXXFLAGS) -DPIC -fPIC -c $< -o $@
sf_malloc_new_debug.o: sf_malloc_new.cpp
	$(CXX) $(CXXFLAGS) -DPIC -fPIC -c $< -o $@

sf_malloc_init_shared.o: sf_malloc_init.cpp
	$(CXX) $(CXXFLAGS) -DPIC -fPIC -c $< -o $@
sf_malloc_init_debug.o: sf_malloc_init.cpp
	$(CXX) $(CXXFLAGS) -DPIC -fPIC -c $< -o $@

libsfmalloc.a: $(OBJS)
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@

libsfmalloc.so: $(SHARED_OBJS)
	$(CXX) -shared -fPIC $(LIBS) -o $@ $(SHARED_OBJS) 

libsfmalloc-debug.so: $(DEBUG_OBJS)
	$(CXX) -shared -fPIC $(LIBS) -o $@ $(DEBUG_OBJS) 
distclean:
	rm -f *.o $(LIB_MALLOC)

clean:
	rm -f *.o

