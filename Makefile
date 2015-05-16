
CC      = gcc 
CXX     = g++
AR      = ar
RANLIB  = ranlib

OBJS = lol.o lol_new.o lol_wrapper.o lol_hazard.o lol_init.o lol_util.o
SHARED_OBJS = lol_shared.o lol_new_shared.o lol_init_shared.o lol_wrapper_shared.o lol_hazard_shared.o lol_util_shared.o
DEBUG_OBJS = lol_debug.o lol_new_debug.o lol_init_debug.o lol_wrapper_debug.o lol_hazard_debug.o lol_util_debug.o
PREFIX = /usr/local
LIBS = -lpthread -lrt -ldl
LIB_MALLOC = $(addprefix .libs/,libsfmalloc.a libsfmalloc.so libsfmalloc-debug.so)
LIB_DIR = .libs
OBJ_DIR = .objs
OPT_FLAGS = -O3 -Wall -Wextra -Werror -g -mmmx -msse -march=native \
  -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc \
  -fno-builtin-calloc -fno-builtin-cfree \
  -fno-builtin-memalign -fno-builtin-posix_memalign \
  -fno-builtin-valloc -fno-builtin-pvalloc -fno-exceptions
INC_FLAGS = -I./ -I./include
DEFS += -D_REENTRANT -DMALLOC_NEED_INIT -DMALLOC_NEED_THREAD_INIT #-DMALLOC_USE_STATIC_LINKING
#DEFS += -D_REENTRANT -DNDEBUG

CFLAGS = -std=gnu11 -fPIC -shared $(OPT_FLAGS) $(INC_FLAGS) $(DEFS) 
CXXFLAGS = -std=gnu++14 -fPIC -shared -ffast-math $(OPT_FLAGS) $(INC_FLAGS) $(DEFS)
DEBUG_FLAGS := -DMALLOC_DEBUG -DMALLOC_DEBUG_DETAIL -DMALLOC_STATS 
SHARED_FLAGS := -UMALLOC_DEBUG -UMALLOC_DEBUG_DETAIL -DNDEBUG

all: dirs $(LIB_MALLOC)
debug: .libs/libsfmalloc-debug.so

shared: .libs/libsfmalloc.so

static: .libs/libsfmalloc.a

dirs:
	mkdir -p $(OBJ_DIR) $(LIB_DIR)
.objs/lol.o: lol.c include/lol.h include/lol_def.h lol_config.h 
	$(CC) $(CFLAGS) -c $< -o $@
.objs/lol_wrapper.o: lol_wrapper.c
	$(CC) $(CFLAGS) -c $< -o $@
.objs/lol_hazard.o: lol_hazard.c
	$(CC) $(CFLAGS) -c $< -o $@
.objs/lol_init.o: lol_init.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
.objs/lol_util.o: lol_util.c
	$(CC) $(CFLAGS) -c $< -o $@
.objs/lol_new.o: lol_new.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
.objs/%_shared.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SHARED_FLAGS) -DPIC -fPIC -c $< -o $@
.objs/%_debug.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -DPIC -fPIC -c $< -o $@
.objs/%_shared.o: %.c
	$(CC) $(CFLAGS) $(SHARED_FLAGS) -DPIC -fPIC -c $< -o $@
.objs/%_debug.o: %.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -DPIC -fPIC -c $< -o $@
.libs/libsfmalloc.a: $(addprefix .objs/,$(OBJS))
	$(AR) cr $@ $(addprefix .objs/,$(OBJS))
	$(RANLIB) $@
.libs/libsfmalloc.so: $(addprefix .objs/,$(SHARED_OBJS)) 
	$(CXX) -shared $(LIBS)  -o $@ $(addprefix .objs/, $(SHARED_OBJS))
.libs/libsfmalloc-debug.so: $(addprefix .objs/,$(DEBUG_OBJS)) 
	$(CXX) -shared $(LIBS)  -o $@ $(addprefix .objs/,$(DEBUG_OBJS))
install: .libs/libsfmalloc.so .iibs/libsfmalloc.a .libs/libsfmalloc-debug.so
	install .libs/libsfmalloc.so $(PREFIX)/lib/
	install .libs/libsfmalloc-debug.so $(PREFIX)/lib/
	install .libs/libsfmalloc.a  $(PREFIX)/lib/
clean:
	rm -f *.o $(LIB_MALLOC)
	rm -r .objs .libs

