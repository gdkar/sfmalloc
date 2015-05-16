#ifndef SF_MALLOC_HAZARD_H
#define SF_MALLOC_HAZARD_H
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "sf_malloc_def.h"
void* do_mmap(void*,size_t );
typedef struct hazard_ptr hazard_ptr_t;
typedef struct hazard_list hazard_list_t;
typedef struct hazard_list_elem hazard_list_elem_t;
struct hazard_ptr {
  hazard_ptr_t      *next;
  volatile uint32_t  count;
  volatile uint16_t  active;
  volatile uint16_t  dsize;
  hazard_list_elem_t*dlist;
  void*              node[(CACHE_LINE_SIZE-(2*sizeof(void*)+2*sizeof(uint32_t)))/sizeof(void*)];
};
struct hazard_list_elem{
  struct hazard_list_elem *next;
  void (*dtor)(void*);
};
struct hazard_list{
  hazard_ptr_t      *head;
  volatile uint32_t  num_free;
}__attribute__((aligned(CACHE_LINE_SIZE)));
hazard_ptr_t* hazard_ptr_alloc();
hazard_list_t * g_hazard();
void          hazard_ptr_free(hazard_ptr_t*hp);
void*         _hazard_ptr_set(hazard_ptr_t*hp, void**pptr);
#define hazard_ptr_set(_hp,_pptr) _hazard_ptr_set(_hp,(void**)_pptr)
void          _hazard_ptr_clr(hazard_ptr_t*hp, void*ptr);
#define hazard_ptr_clr(_hp,_ptr) _hazard_ptr_clr(_hp,(void*)_ptr)
bool          _hazard_ptr_scan_single(void *ptr);
#define hazard_ptr_scan_single(_ptr) _hazard_ptr_scan_single((void*)_ptr)
void          hazard_ptr_scan(hazard_ptr_t *myhp);
void          _hazard_ptr_delete(hazard_ptr_t *myhp, hazard_list_elem_t *elem, void (*dtor)(void*));
#define hazard_ptr_delete(_myhp,_elem,_dtor) _hazard_ptr_delete(_myhp, (hazard_list_elem_t *)_elem, (void(*)(void*))dtor)
#endif
