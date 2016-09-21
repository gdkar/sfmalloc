
CC     ?= gcc
CXX    ?= g++
AR     ?= ar
RANLIB ?= ranlib

OBJS = .objs/sf_malloc.o .objs/sf_malloc_new.o .objs/sf_malloc_wrapper.o .objs/sf_malloc_init.o

STATIC_OBJS = $(patsubst %.o,%-static.o, $(OBJS))
SHARED_OBJS = $(patsubst %.o,%-shared.o, $(OBJS))
DEBUG_OBJS  = $(patsubst %.o,%-debug.o, $(OBJS))

LIBS = -lpthread -lrt -ldl
LIB_MALLOC = .libs/libsfmalloc-static.a .libs/libsfmalloc-shared.so .libs/libsfmalloc-debug.so

OPT_FLAGS = -O2 -Ofast -Wall -Wextra -g -ggdb -pthread -shared -fPIC \
  -march=native \
  -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc \
  -fno-builtin-calloc -fno-builtin-cfree \
  -fno-builtin-memalign -fno-builtin-posix_memalign \
  -fno-builtin-valloc -fno-builtin-pvalloc -fno-exceptions

INC_FLAGS =
DEFS += -D_REENTRANT
#DEFS += -D_REENTRANT -DNDEBUG

%-debug.o: DEFS += -DMALLOC_DEBUG \
	-DMALLOC_STATS \
	-DMALLOC_NEED_INIT \
	-DMALLOC_NEED_THREAD_INIT \
	-UMALLOC_USE_STATIC_LINKING
%-shared.o: DEFS +=-DNDEBUG \
	-DMALLOC_NEED_THREAD_INIT \
	-DMALLOC_NEED_INIT \
	-UMALLOC_USE_STATIC_LINKING

CFLAGS+=-std=gnu11 $(OPT_FLAGS) $(INC_FLAGS) $(DEFS)
CXXFLAGS+=-std=gnu++14 $(OPT_FLAGS) $(INC_FLAGS) $(DEFS)

all: $(LIB_MALLOC)
show-config:
	@echo CC=$(CC)
	@echo CFLAGS=$(CFLAGS)
	@echo CXX=$(CXX)
	@echo CXXFLAGS=$(CXXFLAGS)
.objs/%-debug.o: %.cpp sf_malloc.h sf_malloc_def.h sf_malloc_ctrl.h Makefile
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -fPIC -shared $(INC_FLAGS) $(OPT_FLAGS) $(DEFS) $< -c -o $@
.objs/%-shared.o: %.cpp sf_malloc.h sf_malloc_def.h sf_malloc_ctrl.h Makefile
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -fPIC -shared $(INC_FLAGS) $(OPT_FLAGS) $(DEFS) $< -c -o $@
.objs/%-static.o: %.cpp sf_malloc.h sf_malloc_def.h sf_malloc_ctrl.h Makefile
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -fPIC -shared $(INC_FLAGS) $(OPT_FLAGS) $(DEFS) $< -c -o $@

.objs/%-debug.o: %.c sf_malloc.h sf_malloc_def.h sf_malloc_ctrl.h Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -fPIC -shared $(INC_FLAGS) $(OPT_FLAGS) $(DEFS) $< -c -o $@
.objs/%-shared.o: %.c sf_malloc.h sf_malloc_def.h sf_malloc_ctrl.h Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -fPIC -shared $(INC_FLAGS) $(OPT_FLAGS) $(DEFS) $< -c -o $@
.objs/%-static.o: %.c sf_malloc.h sf_malloc_def.h sf_malloc_ctrl.h Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -fPIC -shared $(INC_FLAGS) $(OPT_FLAGS) $(DEFS) $< -c -o $@

.libs/libsfmalloc-static.a: $(STATIC_OBJS)
	@mkdir -p $(dir $@)
	$(AR) cr $@ $<
	$(RANLIB) $@

.libs/libsfmalloc-shared.so: $(SHARED_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) -shared -fPIC -Ofast $(LIBS) -o $@ $<

.libs/libsfmalloc-debug.so: $(DEBUG_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) -shared -fPIC -Ofast $(LIBS) -o $@ $<

distclean:
	rm -rf .objs .libs

clean:
	rm -rf .objs
