
CC      = clang
CXX     = clang++
AR      = ar
RANLIB  = ranlib

OBJS = sf_malloc.o sf_malloc_new.o sf_malloc_wrapper.o
SHARED_OBJS = sf_malloc_shared.o sf_malloc_new_shared.o sf_malloc_init_shared.o sf_malloc_wrapper_shared.o
PREFIX = /usr/local
LIBS = -lpthread -lrt -ldl
LIB_MALLOC = $(addprefix .libs/, libsfmalloc.a libsfmalloc.so)
LIB_DIR = .libs
OBJ_DIR = .objs
OPT_FLAGS = -O3 -Wall -g -mmmx -msse -march=native \
  -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc \
  -fno-builtin-calloc -fno-builtin-cfree \
  -fno-builtin-memalign -fno-builtin-posix_memalign \
  -fno-builtin-valloc -fno-builtin-pvalloc -fno-exceptions
INC_FLAGS = 
DEFS += -D_REENTRANT
#DEFS += -D_REENTRANT -DNDEBUG

CFLAGS = -std=gnu11 -fPIC -shared $(OPT_FLAGS) $(INC_FLAGS) $(DEFS) 
CXXFLAGS = $(OPT_FLAGS) -fPIC -shared -ffast-math -std=gnu++14 $(INC_FLAGS) $(DEFS)

all: dirs $(LIB_MALLOC)

dirs:
	mkdir -p $(OBJ_DIR) $(LIB_DIR)
.objs/sf_malloc.o: sf_malloc.c sf_malloc.h sf_malloc_def.h sf_malloc_ctrl.h sf_malloc_atomic.h
	$(CC) $(CFLAGS) -DMALLOC_NEED_INIT -DMALLOC_NEED_THREAD_INIT -DMALLOC_USE_STATIC_LINKING -c $< -o $@

.objs/sf_malloc_shared.o: sf_malloc.c sf_malloc.h sf_malloc_def.h sf_malloc_ctrl.h sf_malloc_atomic.h
	$(CC) $(CFLAGS) -DPIC -fPIC -c $< -o $@

.objs/sf_malloc_wrapper.o: sf_malloc_wrapper.c
	$(CC) $(CFLAGS) -c $< -o $@

.objs/sf_malloc_wrapper_shared.o: sf_malloc_wrapper.c
	$(CC) $(CFLAGS) -DPIC -fPIC -c $< -o $@

.objs/sf_malloc_new.o: sf_malloc_new.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.objs/sf_malloc_new_shared.o: sf_malloc_new.cpp
	$(CXX) $(CXXFLAGS) -DPIC -fPIC -c $< -o $@

.objs/sf_malloc_init_shared.o: sf_malloc_init.cpp
	$(CXX) $(CXXFLAGS) -DPIC -fPIC -c $< -o $@

.libs/libsfmalloc.a: $(addprefix .objs/,$(OBJS))
	$(AR) cr $@ $(addprefix .objs/,$(OBJS))
	$(RANLIB) $@

.libs/libsfmalloc.so: $(addprefix .objs/,$(SHARED_OBJS))
	$(CXX) -shared $(LIBS) -o $@ $(addprefix .objs/, $(SHARED_OBJS))
install:
	install .libs/libsfmalloc.so $(PREFIX)/lib/
	install .libs/libsfmalloc.a  $(PREFIX)/lib/
clean:
	rm -f *.o $(LIB_MALLOC)
	rm -r .objs .libs

