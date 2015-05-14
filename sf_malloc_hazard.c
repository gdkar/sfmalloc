#include "sf_malloc_hazard.h"
#include "sf_malloc_asm.h"
#include <alloca.h>
#include <string.h>
#include <stdatomic.h>
extern volatile uint32_t g_thread_num;
static hazard_list_t g_hazard_list = {.head=NULL,.num_free=0};
hazard_list_t *g_hazard(){return &g_hazard_list;}
hazard_ptr_t* hazard_ptr_alloc() {
  // Allocate from the current list.
  if (g_hazard_list.num_free> 0) {
    for (hazard_ptr_t* hp = g_hazard_list.head; hp != NULL; hp = hp->next) {
      if ((hp->active) || (atomic_exchange_explicit(&hp->active, 1,memory_order_relaxed))) continue;
      atomic_fetch_add_explicit((int32_t*)&g_hazard_list.num_free,-1,memory_order_relaxed);
      hp->count = 0;
      memset(hp->node,0,sizeof(hp->node)); 
      return hp;
    }
  }
  // Allocate a new page and split it.
  hazard_ptr_t* first_hptr = (hazard_ptr_t*)do_mmap(0,PAGE_SIZE);
  first_hptr->active = 1;
  uint32_t rem_len = (PAGE_SIZE / sizeof(hazard_ptr_t)) - 1;
  hazard_ptr_t* last_hptr = first_hptr;
  for (uint32_t i = 0; i < rem_len; i++) {
    hazard_ptr_t* next_hptr = last_hptr + 1;
    last_hptr->next = next_hptr;
    last_hptr = next_hptr;
  }
  hazard_ptr_t* top;
  top = g_hazard_list.head;
  do {
    last_hptr->next = top;
  } while (!atomic_compare_exchange_weak(&g_hazard_list.head, &top, first_hptr));
  atomic_fetch_add_explicit(&g_hazard_list.num_free, rem_len,memory_order_relaxed);
  return first_hptr;
}
void * _hazard_ptr_set(hazard_ptr_t*hp, void **target){
  if(UNLIKELY(!hp ||!target))return NULL;
  void * t = *target;
  void * prev_t;
  hp->count ++;
  do{
    hp->node[hp->count-1] = t;
    prev_t = t;
  }while(prev_t != (t=*target));
  return t;
}

void _hazard_ptr_clr(hazard_ptr_t *hp, void *target){
  if(target && hp->count){
    for(uint32_t cur = 0;cur<hp->count;){
      if(hp->node[cur] == target || !hp->node[cur]){
        hp->node[cur] = hp->node[hp->count-1];
        hp->node[hp->count-1]=NULL;
        hp->count--;
      }else{cur++;}
    }
  }
}
void hazard_ptr_free(hazard_ptr_t* hp) {
  hp->count  = 0;
  hp->active = 0;
  atomic_fetch_add_explicit(&g_hazard_list.num_free,1,memory_order_relaxed);
}
bool _hazard_ptr_scan_single(void* sph) {
  for (hazard_ptr_t* hp = g_hazard_list.head; hp != NULL; hp = hp->next) {
    uint32_t count = hp->count;
    while(count){
      count--;
      if (hp->node[count] == sph) return true;
    }
  }
  return false;
}
inline static uint64_t mmh3mod(uint64_t key){
  key ^= (key>> 31);
  key *= 0x7fb5d329728ea185;
  key ^= (key>> 27);
  key *= 0x81dadef4bc2dd44d;
  key ^= (key>> 33);
  return key;
}
static void _hash_insert(uint64_t *table, uint32_t size, uint64_t value){
  const uint32_t mask =size-1;
  const uint32_t hash = mmh3mod(value)&mask;
  for(uint32_t offset = 0; offset < size; offset++){
    const uint32_t index = (hash+offset)&mask;
    if(!table[index] || table[index]==value){
      table[index] = value;
      return;
    }
  }
}
#define hash_insert(_t,_s,_v) _hash_insert(_t,(uint32_t)_s,(uint64_t)_v)
static bool _hash_lookup(uint64_t *table, uint32_t size, uint64_t value){
  const uint32_t mask = size-1;
  const uint32_t hash = mmh3mod(value)&mask;
  for(uint32_t offset = 0; offset < size; offset++){
    const uint32_t index= (hash+offset)&mask;
    if(!table[index]) return false;
    else if(table[index] == value) return true;
  }
  return false;
}
#define hash_lookup(_t, _s, _v) _hash_lookup(_t,(uint32_t)_s,(uint64_t)_v)
void hazard_ptr_scan(hazard_ptr_t *myhp){
  myhp->count = 0;
  uint32_t hash_size = 16 * g_thread_num -1;
  hash_size |= hash_size>>1;
  hash_size |= hash_size>>2;
  hash_size |= hash_size>>4;
  hash_size |= hash_size>>8;
  hash_size |= hash_size>>16;
  hash_size++;
  uint64_t  *storage = (uint64_t*)alloca(sizeof(uint64_t)*hash_size);
  memset(storage,0,sizeof(uint64_t)*hash_size);
  for(hazard_ptr_t *hp = g_hazard_list.head; hp!=NULL; hp = hp->next){
    uint32_t count = hp->count;
    while(count){
      count--;
      hash_insert(storage,hash_size,hp->node[count]);
    }
  }
  hazard_list_elem_t *new_dlist = 0;
  hazard_list_elem_t *old_dlist = myhp->dlist;
  myhp->dsize = 0;
  while(old_dlist){
    if(hash_lookup(storage,hash_size,(uint64_t)old_dlist)){
      myhp->dsize++;
      hazard_list_elem_t *elem = old_dlist;
      old_dlist                = elem->next;
      elem->next               = new_dlist;
      new_dlist                = elem; 
    }else{
      hazard_list_elem_t *elem = old_dlist;
      void (*elem_dtor)(void*) = elem->dtor;
      old_dlist                = elem->next;
      elem_dtor(elem);
    }
  }
}
void _hazard_ptr_delete(hazard_ptr_t *myhp, hazard_list_elem_t *elem, void (*dtor)(void*)){
  elem->dtor = dtor;
  elem->next = myhp->dlist;
  myhp->dlist= elem;
  myhp->dsize++;
  if(myhp->dsize > 2 * g_thread_num){
      hazard_ptr_scan(myhp);
  }
}
