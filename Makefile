
CC      = gcc 
CXX     = g++
AR      = ar
RANLIB  = ranlib


C_SRC = $(wildcard src/*.c)
CXX_SRC = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.c,%.o,$(C_SRC)) $(patsubst src/%.cpp,%.o,$(CXX_SRC))
SHARED_OBJS =$(patsubst %.o,%_shared.o,$(OBJS))
DEBUG_OBJS =$(patsubst %.o,%_debug.o,$(OBJS))
PREFIX = /usr/local
LIBS = -lpthread -lrt -ldl
LIB_MALLOC = $(addprefix .libs/, libsfmalloc.a libsfmalloc.so libsfmalloc-debug.so)
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
.objs/%.o: src/%.c #include/lol.h include/lol_def.h lol_config.h 
	$(CC) $(CFLAGS) -c $< -o $@
.objs/%.o: src/%.cpp #include/lol.h include/lol_def.h lol_config.h 
	$(CXX) $(CXXFLAGS) -c $< -o $@

.objs/%_shared.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(SHARED_FLAGS) -DPIC -fPIC -c $< -o $@
.objs/%_debug.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -DPIC -fPIC -c $< -o $@
.objs/%_shared.o: src/%.c
	$(CC) $(CFLAGS) $(SHARED_FLAGS) -DPIC -fPIC -c $< -o $@
.objs/%_debug.o: src/%.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -DPIC -fPIC -c $< -o $@
.libs/libsfmalloc.a: $(addprefix .objs/, $(OBJS))
	$(AR) cr $@ $(addprefix .objs/,$(OBJS))
	$(RANLIB) $@
.libs/libsfmalloc.so: $(addprefix .objs/, $(SHARED_OBJS)) 
	$(CXX) -shared $(LIBS) -DPIC -fPIC $(SHARED_FLAGS)  $(addprefix .objs/, $(SHARED_OBJS))  -o $@  
.libs/libsfmalloc-debug.so: $(addprefix .objs/, $(DEBUG_OBJS)) 
	$(CXX) -shared $(LIBS)  -DPIC -fPIC $(addprefix .objs/, $(DEBUG_OBJS))  -o $@ 
install: .libs/libsfmalloc.so .libs/libsfmalloc.a .libs/libsfmalloc-debug.so
	install .libs/libsfmalloc.so $(PREFIX)/lib/
	install .libs/libsfmalloc-debug.so $(PREFIX)/lib/
	install .libs/libsfmalloc.a  $(PREFIX)/lib/
clean:
	rm -f *.o $(LIB_MALLOC)
	rm -r .objs .libs

