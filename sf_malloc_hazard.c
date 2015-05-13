#include "sf_malloc_hazard.h"
#include "sf_malloc_atomic.h"
#include "sf_malloc_asm.h"
#include <string.h>
hazard_list_t *g_hazard(){return &g_hazard_list;}
hazard_ptr_t* hazard_ptr_alloc() {
  // Allocate from the current list.
  if (g_hazard_list.num_free> 0) {
    for (hazard_ptr_t* hp = g_hazard_list.head; hp != NULL; hp = hp->next) {
      if ((hp->active) || (atomic_xchg_uint(&hp->active, 1))) continue;
      atomic_dec_int((int32_t*)&g_hazard_list.num_free);
      hp->count = 0;
      memset(hp->node,0,sizeof(hp->node)); 
      return hp;
    }
  }
  // Allocate a new page and split it.
  hazard_ptr_t* first_hptr = (hazard_ptr_t*)do_mmap(PAGE_SIZE);
  first_hptr->active = 1;
  uint32_t rem_len = (PAGE_SIZE / sizeof(hazard_ptr_t)) - 1;
  hazard_ptr_t* last_hptr = first_hptr;
  for (uint32_t i = 0; i < rem_len; i++) {
    hazard_ptr_t* next_hptr = last_hptr + 1;
    last_hptr->next = next_hptr;
    last_hptr = next_hptr;
  }
  hazard_ptr_t* top;
  do {
    top = g_hazard_list.head;
    last_hptr->next = top;
  } while (!cas_ptr((volatile void**)&g_hazard_list.head, (void*)top, (void*)first_hptr));
  atomic_add_uint(&g_hazard_list.num_free, rem_len);
  return first_hptr;
}
void * hazard_ptr_set(hazard_ptr_t*hp, void **target){
  void * t = *target;
  void * prev_t;
  hp->count ++;
  do{
    hp->node[hp->count-1] = t;
    prev_t = t;
  }while(prev_t != (t=*target));
  return t;
}

void hazard_ptr_clr(hazard_ptr_t *hp, void *target){
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
  atomic_inc_uint(&g_hazard_list.num_free);
}
bool hazard_ptr_scan_single(void* sph) {
  for (hazard_ptr_t* hp = g_hazard_list.head; hp != NULL; hp = hp->next) {
    uint32_t count = hp->count;
    while(count){
      count--;
      if (hp->node[count] == sph) return true;
    }
  }
  return false;
}

